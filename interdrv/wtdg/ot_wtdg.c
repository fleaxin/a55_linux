/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_wtdg.h"
#ifdef __LITEOS__
#include "pthread.h"
#include <sys/prctl.h>
#endif
#include "ot_osal.h"
#include "ot_wtdg_hal.h"

#ifndef NULL
#define NULL  ((void *)0)
#endif

#define ot_wdt_reg(x)    (OT_WDT_BASE + (x))

#define OT_WDT_LOAD      0x000
#define OT_WDT_VALUE     0x004
#define OT_WDT_CTRL      0x008
#define OT_WDT_INTCLR    0x00C
#define OT_WDT_RIS       0x010
#define OT_WDT_MIS       0x014
#define OT_WDT_LOCK      0xC00

#define OT_WDT_UNLOCK_VAL    0x1ACCE551

volatile void *g_wtdg_reg_base = NULL;

#define ot_wdt_io_address(x) ((uintptr_t)(g_wtdg_reg_base) + ((x) - (OT_WDT_BASE)))

#define ot_wdt_readl(x)       osal_readl(ot_wdt_io_address(ot_wdt_reg(x)))
#define ot_wdt_writel(v, x)   osal_writel(v, ot_wdt_io_address(ot_wdt_reg(x)))

/* debug */
#define OT_DOG_PFX "Dog: "
#define ot_dog_dbg(params...) osal_printk(OT_DOG_PFX params)

/* module param */
static int g_default_margin = OT_DOG_TIMER_MARGIN;    /* in seconds */

#define OT_DOG_TIMER_DEMULTIPLY  9

static int g_nodeamon = 0;

/* watchdog info */
static const struct watchdog_info g_ident = {
    .options =  WDIOF_SETTIMEOUT |
                WDIOF_KEEPALIVEPING |
                WDIOF_MAGICCLOSE,
    .firmware_version = 0,
    .identity = "Vendor Watchdog",
};

/* local var */
static struct osal_spinlock g_ot_dog_lock;
static int g_cur_margin = OT_DOG_TIMER_MARGIN;
static int g_need_iounmap = 0;

#ifdef __LITEOS__
static pthread_t g_task_ot_dog_deamon = 0;
#else
static struct osal_task *g_task_ot_dog_deamon = NULL;
#endif

#define OT_DOG_EXIT      0
#define OT_DOG_SELFCLR   1
#define OT_DOG_EXTCLR    2

static volatile unsigned int g_ot_dog_state = 0;
static osal_atomic_t g_driver_open;
static unsigned int g_options = WDIOS_ENABLECARD;

#ifndef MHZ
#define MHZ (1000 * 1000)
#endif

static const unsigned long g_rate = 3 * MHZ; /* 3MHZ */

volatile void **get_wtdg_reg_base(void)
{
    return &g_wtdg_reg_base;
}

void wtdg_set_module_param(int margin, int daemon)
{
    g_default_margin = margin;
    g_nodeamon = daemon;
}

static void ot_dog_set_timeout(unsigned int nr)
{
    unsigned int cnt_0 = ~0x0;
    unsigned int cnt = cnt_0 / g_rate;  /* max cnt */
    unsigned long flags;

    osal_spin_lock_irqsave(&g_ot_dog_lock, &flags);

    if (nr == 0 || nr > cnt) {
        cnt = ~0x0;
    } else {
        cnt = nr * g_rate;
    }
    /* unlock watchdog registers */
    ot_wdt_writel(OT_WDT_UNLOCK_VAL, OT_WDT_LOCK);
    ot_wdt_writel(cnt, OT_WDT_LOAD);
    ot_wdt_writel(cnt, OT_WDT_VALUE);
    /* lock watchdog registers */
    ot_wdt_writel(0, OT_WDT_LOCK);
    osal_spin_unlock_irqrestore(&g_ot_dog_lock, &flags);
}

static void ot_dog_feed(void)
{
    unsigned long flags;

    /* read the RIS state of current wdg */
    unsigned int v = (unsigned int)ot_wdt_readl(OT_WDT_RIS);
    v &= 0x1; /* 0x1: get INT bit [1] */
    if (v == 0) { /* no INT on current wdg */
        return;
    }

    osal_spin_lock_irqsave(&g_ot_dog_lock, &flags);
    /* unlock watchdog registers */
    ot_wdt_writel(OT_WDT_UNLOCK_VAL, OT_WDT_LOCK);
    /* clear watchdog */
    ot_wdt_writel(0x00, OT_WDT_INTCLR);
    /* lock watchdog registers */
    ot_wdt_writel(0, OT_WDT_LOCK);
    osal_spin_unlock_irqrestore(&g_ot_dog_lock, &flags);
}

static int ot_dog_set_heartbeat(int t)
{
    int ret = 0;
    unsigned int cnt_0 = ~0x0;
    unsigned int cnt = cnt_0 / g_rate;

    if (t <= 0) {
        osal_printk("set heartbeat less or equal to 0, heartbeat will not be changed.\n");
        t = g_cur_margin;
        ret = -1;
    } else if ((unsigned int)t > cnt) {
        osal_printk("set heartbeat range error, t = %d\n", t);
        osal_printk("force heartbeat to %u\n", cnt);
        t = cnt;
        ret = -1;
    }

    g_cur_margin = t;

    ot_dog_set_timeout(t);
    ot_dog_feed();

    return ret;
}

static int ot_dog_keepalive(void)
{
    ot_dog_feed();
    return 0;
}

static void ot_dog_start(void)
{
    unsigned long flags;

    osal_spin_lock_irqsave(&g_ot_dog_lock, &flags);
    /* unlock watchdog registers */
    ot_wdt_writel(OT_WDT_UNLOCK_VAL, OT_WDT_LOCK);
    ot_wdt_writel(0x00, OT_WDT_CTRL);     /* 0x00: disable watch dog reset signal and interrupt */
    ot_wdt_writel(0x00, OT_WDT_INTCLR);   /* 0x00: clear interrupt */
    ot_wdt_writel(0x03, OT_WDT_CTRL);     /* 0x03: enable watch dog reset signal and interrupt */
    /* lock watchdog registers */
    ot_wdt_writel(0, OT_WDT_LOCK);
    osal_spin_unlock_irqrestore(&g_ot_dog_lock, &flags);

    g_options = WDIOS_ENABLECARD;
}

static void ot_dog_stop(void)
{
    unsigned long flags;

    osal_spin_lock_irqsave(&g_ot_dog_lock, &flags);
    /* unlock watchdog registers */
    ot_wdt_writel(OT_WDT_UNLOCK_VAL, OT_WDT_LOCK);
    /* stop watchdog timer */
    ot_wdt_writel(0x00, OT_WDT_CTRL);     /* 0x00: disable watch dog reset signal and interrupt */
    ot_wdt_writel(0x00, OT_WDT_INTCLR);   /* 0x00: clear interrupt */
    /* lock watchdog registers */
    ot_wdt_writel(0, OT_WDT_LOCK);
    osal_spin_unlock_irqrestore(&g_ot_dog_lock, &flags);

    ot_dog_set_timeout(0);
    g_options = WDIOS_DISABLECARD;
}

static int ot_dog_open(void *private_data)
{
    int ret = 0;

    wtdg_unused(private_data);
    if (osal_atomic_dec_return(&g_driver_open) != 0) {
        ret = osal_atomic_inc_return(&g_driver_open);
        osal_printk("Error: device already open:%d.\n", ret);
        return -1;
    }

    ot_dog_keepalive();

    return ret;
}

static int ot_dog_release(void *private_data)
{
    wtdg_unused(private_data);
    if (osal_atomic_inc_return(&g_driver_open) != 1) {
        osal_atomic_dec_return(&g_driver_open);
        return 0;
    }

    g_ot_dog_state = OT_DOG_SELFCLR;
    ot_dog_set_heartbeat(g_cur_margin);

    if (g_options == WDIOS_DISABLECARD) {
        osal_printk("Watchdog is disabled!\n");
    }
    return 0;
}

static int ot_dog_set_options(unsigned int new_options)
{
    if (new_options & WDIOS_ENABLECARD) {
        ot_dog_start();
        ot_dog_set_heartbeat(g_cur_margin);
        return 0;
    } else if (new_options & WDIOS_DISABLECARD) {
        ot_dog_stop();
        return 0;
    } else {
        return -WDIOS_UNKNOWN;
    }
}

static long ot_dog_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
    void *argp = (void *)(uintptr_t)arg;

    wtdg_unused(private_data);
    switch (cmd) {
        case WDIOC_GETSUPPORT:
            if (memcpy_s(argp, sizeof(struct watchdog_info), &g_ident, sizeof(g_ident)) != EOK) {
                return -1;
            }
            return 0;

        case WDIOC_GETSTATUS:
        case WDIOC_GETBOOTSTATUS:
            *(unsigned int *)argp = g_options;
            return 0;

        case WDIOC_KEEPALIVE:
            ot_dog_keepalive();
            return 0;

        case WDIOC_SETTIMEOUT:
            if (ot_dog_set_heartbeat(*(int *)argp)) {
                return -1;
            }
            ot_dog_keepalive();
            return 0;

        case WDIOC_GETTIMEOUT:
            *(int *)argp = g_cur_margin;
            return 0;

        case WDIOC_SETOPTIONS:
            return ot_dog_set_options(*(unsigned int *)argp);

        default:
            return -1;
    }
}

/* Kernel Interfaces */
static struct osal_fileops g_ot_dog_fops = {
    .unlocked_ioctl = ot_dog_ioctl,
    .open           = ot_dog_open,
    .release        = ot_dog_release,
};

static struct osal_dev *g_ot_dog_miscdev = NULL;

#define OT_BANNER_STRING "Watchdog Timer: 0.01 initialized. default_margin = %d sec (nodeamon = %d)\n"

static int ot_dog_deamon(void *data)
{
    wtdg_unused(data);
#ifdef __LITEOS__
    prctl(PR_SET_NAME, "ot_dog_deamon", 0, 0, 0);
#endif
    while (g_ot_dog_state != OT_DOG_EXIT) {
        switch (g_ot_dog_state) {
            case OT_DOG_SELFCLR:
                if (g_options & WDIOS_ENABLECARD) {
                    ot_dog_feed();
                }
                break;
            case OT_DOG_EXTCLR:
                break;
            default:
                break;
        }
        /* sleep; when self feed dog, only use the default margin */
        osal_msleep(g_default_margin * 1000 / 2 + 10);  /* sleep (60*1000/2 + 10)->30.01s */
    }

    return 0;
}

static int ot_dog_init(void)
{
    ot_dog_start();

    g_ot_dog_state = OT_DOG_SELFCLR;
    if (!g_nodeamon) {
#ifdef __LITEOS__
        if (pthread_create(&g_task_ot_dog_deamon, NULL, (void *)ot_dog_deamon, 0) < 0) {
            osal_printk("create ot_dog_deamon failed!\n");
            return -1;
        }
#else
        struct osal_task *dog = NULL;

        dog = osal_kthread_create(ot_dog_deamon, NULL, "ot_dog");
        if (dog == NULL) {
            osal_printk("create ot_dog_deamon failed!\n");
            return -1;
        }
        g_task_ot_dog_deamon = dog;
#endif
    }
    return 0;
}

static int get_margin(void)
{
    int ret = g_default_margin;

    /* Check that the g_default_margin value is within it's range ; if not reset to the default */
    if (ot_dog_set_heartbeat(g_default_margin)) {
        g_default_margin = OT_DOG_TIMER_MARGIN;
        ot_dog_set_heartbeat(OT_DOG_TIMER_MARGIN);
        osal_printk("g_default_margin value must be 0<g_default_margin<%lu, using %d\n",
            (~0x0UL) / g_rate, OT_DOG_TIMER_MARGIN);
    }

    return ret;
}

static int ptr_ioremap(void)
{
    if (g_wtdg_reg_base == NULL) {
        g_wtdg_reg_base = (volatile void *)osal_ioremap(OT_WDT_BASE, 0x1000); /* 0x1000: watch dog reg length */
        if (g_wtdg_reg_base == NULL) {
            osal_printk("osal_ioremap err. \n");
            osal_spin_lock_destroy(&g_ot_dog_lock);
            osal_atomic_destroy(&g_driver_open);
            return -1;
        }
        g_need_iounmap = 1;
    }
    return 1;
}

int watchdog_init(void)
{
    if (osal_atomic_init(&g_driver_open) != 0) {
        osal_printk("Error: init atomic\n");
        return -1;
    }
    osal_atomic_set(&g_driver_open, 1);

    if (osal_spin_lock_init(&g_ot_dog_lock) != 0) {
        osal_printk("function %s line %u failed\n", __FUNCTION__, __LINE__);
        goto spin_lock_init_fail;
    }

    if (ptr_ioremap() != 1) {
        return -1;
    }

    g_cur_margin = get_margin();

    g_ot_dog_miscdev = osal_createdev("watchdog");
    if (g_ot_dog_miscdev == NULL) {
        osal_printk("fail to create dev\n");
        goto create_dev_fail;
    }

    g_ot_dog_miscdev->minor = WATCHDOG_MINOR;
    g_ot_dog_miscdev->fops = &g_ot_dog_fops;
    if (osal_registerdevice(g_ot_dog_miscdev) != 0) {
        osal_printk("fail to register dev\n");
        goto register_dev_fail;
    }

    if (ot_dog_init() != 0) {
        goto dog_init_fail;
    }

    osal_printk(OT_BANNER_STRING, g_default_margin, g_nodeamon);
    osal_printk("ot_wtdg init ok. ver=%s, %s.\n", __DATE__, __TIME__);
    return 0;

dog_init_fail:
    osal_deregisterdevice(g_ot_dog_miscdev);
register_dev_fail:
    osal_destroydev(g_ot_dog_miscdev);
create_dev_fail:
    if (g_need_iounmap) {
        osal_iounmap((void *)g_wtdg_reg_base);
        g_need_iounmap = 0;
        g_wtdg_reg_base = NULL;
    }
    osal_spin_lock_destroy(&g_ot_dog_lock);
spin_lock_init_fail:
    osal_atomic_destroy(&g_driver_open);
    return -1;
}

static void ot_dog_exit(void)
{
    ot_dog_set_timeout(0);
    ot_dog_stop();
    g_ot_dog_state = OT_DOG_EXIT;
    if (!g_nodeamon) {
#ifdef __LITEOS__
        pthread_join(g_task_ot_dog_deamon, NULL);
#else
        struct osal_task *p_dog = g_task_ot_dog_deamon;
        if (p_dog == NULL) {
            return;
        }

        osal_kthread_destroy(p_dog, 1);
#endif
        osal_yield();
    }

#ifdef __LITEOS__
    g_task_ot_dog_deamon = 0;
#else
    g_task_ot_dog_deamon = NULL;
#endif
}

void watchdog_exit(void)
{
    ot_dog_exit();

    osal_deregisterdevice(g_ot_dog_miscdev);
    osal_destroydev(g_ot_dog_miscdev);
    if (g_need_iounmap) {
        osal_iounmap((void *)g_wtdg_reg_base);
        g_need_iounmap = 0;
        g_wtdg_reg_base = NULL;
    }
    osal_spin_lock_destroy(&g_ot_dog_lock);
    osal_atomic_destroy(&g_driver_open);
    osal_printk("ot_wtdg exit ok.\n");
}
