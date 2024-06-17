/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "piris.h"
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>

#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/version.h>
#ifdef __LITEOS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ot_type.h"

#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#else
#include <linux/hrtimer.h>
#include <linux/uaccess.h>
#endif
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include "piris_hal.h"
#include "isp_ext.h"
#include "mm_ext.h"

#include <linux/completion.h>
#include "securec.h"

#ifdef PIRIS_DRV_DBG
#define piris_drv_printf(fmt, args...)  osal_printk(fmt, ##args)
#else
#define PIRIS_DRV_PRINTF
#endif

#ifndef __iomem
#define __iomem
#endif
void __iomem *g_reg_piris_ibase[PIRIS_MAX_DEV_NUM];

#define ot_io_pirisi_address(dev, x)  ((td_uintptr_t)g_reg_piris_ibase[dev] + ((x)-(PIRISI_ADRESS_BASE)))

#define piris_a_cfg_reg0(dev)        ot_io_pirisi_address(dev, PIRISI_ADRESS_BASE + 0x002C)
#define piris_a_ctrl_reg0(dev)       ot_io_pirisi_address(dev, PIRISI_ADRESS_BASE + 0x0400)

#define piris_b_cfg_reg0(dev)        ot_io_pirisi_address(dev, PIRISI_ADRESS_BASE + 0x4020)
#define piris_b_ctrl_reg0(dev)       ot_io_pirisi_address(dev, PIRISI_ADRESS_BASE + 0x4400)

#define  piris_write_reg(addr, value) ((*(volatile unsigned int *)(addr)) = (value))
#define  piris_read_reg(addr)         (*(volatile unsigned int *)(addr))

#define MAX_MOTOR_PAHSE 4
#define MAX_STEPS       92
#define PIRIS_PPS       100
#define MAX_TIMEOUT     300

static const unsigned char g_motor_phase_tbl[MAX_MOTOR_PAHSE] = { 0x0, 0x1, 0x2, 0x3};

typedef struct {
    int src_pos;
    int dest_pos;
    unsigned int pps;
    int phase;
    const unsigned char *phase_tbl;
    spinlock_t lock;
    struct semaphore sem;
    struct osal_timer timer;
    struct completion piris_comp;
} piris_dev_s;

static piris_dev_s *g_piris_dev[PIRIS_MAX_DEV_NUM] = { TD_NULL };
static int g_dev;
td_bool g_piris_io_remap = TD_FALSE;

int piris_gpio_update(int dev, const int *piris_pos)
{
    unsigned long flags = 0;
    piris_dev_s *piris = TD_NULL;

    if (dev == 1) {
        g_dev = 1;
    } else if (dev == 0) {
        g_dev = 0;
    }
    piris = g_piris_dev[dev];
    spin_lock_irqsave(&piris->lock, flags);
    piris->dest_pos = MAX2(MIN2(*piris_pos, 200), -200); /* range:[-200,200] */
    piris->pps = PIRIS_PPS;
    piris->pps = MAX2(MIN2(piris->pps, HZ), 1);
    osal_set_timer(&piris->timer, 1);
    spin_unlock_irqrestore(&piris->lock, flags);
    return 0;
}

#define HYS_STEPS       100

/* first go to the full open iris step, set the full open as origin */
#ifdef __LITEOS__
static int piris_origin_set(int dev, piris_data *piris_data_info)
#else
int piris_origin_set(int dev, piris_data *piris_data_info)
#endif
{
    int piris_pos;
    unsigned long flags = 0;
    piris_dev_s *piris;
    int sign, hys_offset, hys_cnt;

    hys_cnt = 3;    /* should be even, default as 3 */
    piris = g_piris_dev[dev];
    piris_pos = piris_data_info->cur_pos;
    if ((piris_pos > 200) || (piris_pos < -200) || (piris_pos == g_piris_dev[dev]->src_pos)) { /* pos: [-200, 200] */
        osal_printk("Invalid cur_pos!\n");
        return -1;
    }

    if ((piris_data_info->total_step) > 400 || (piris_data_info->total_step == 0)) { /* total_step range (0, 400] */
        osal_printk("Invalid total_step!\n");
        return -1;
    }

    piris_gpio_update(dev, &piris_pos);
    init_completion(&piris->piris_comp);
    /* wait for piris origin done */
    wait_for_completion_timeout(&piris->piris_comp, MAX_TIMEOUT);
    /* consider hysteresis effection */
    sign = (piris_data_info->zero_is_max) ? 1 : -1;
    do {
        hys_offset = sign * HYS_STEPS;
        piris_pos += hys_offset;
        piris_gpio_update(dev, &piris_pos);
        wait_for_completion_timeout(&piris->piris_comp, MAX_TIMEOUT);
        sign = -sign;
        hys_cnt--;
    } while (hys_cnt);

#ifdef __LITEOS__
    LOS_TaskDelay(1 * HZ);
#else
    msleep(1000); /* system sleep for 1000 ms (1s) */
#endif
    piris_pos += piris_data_info->total_step * sign;
    piris_gpio_update(dev, &piris_pos);
    wait_for_completion_timeout(&piris->piris_comp, MAX_TIMEOUT);
    /* then offset to full open position */
    spin_lock_irqsave(&piris->lock, flags);
    if (piris_data_info->zero_is_max == 1) {
        piris->src_pos  = 0;
        piris->dest_pos = 0;
    } else {
        piris->src_pos  = piris_data_info->total_step - 1;
        piris->dest_pos = piris_data_info->total_step - 1;
    }
    spin_unlock_irqrestore(&piris->lock, flags);
    return 0;
}

/* go to the full close iris step */
#ifdef __LITEOS__
static int piris_close_set(int dev, piris_data *piris_data_info)
#else
int piris_close_set(int dev, piris_data *piris_data_info)
#endif
{
    int piris_pos;
    unsigned long flags = 0;
    piris_dev_s *piris;

    piris = g_piris_dev[dev];
    piris_pos = piris_data_info->cur_pos;

    init_completion(&piris->piris_comp);
    piris_gpio_update(dev, &piris_pos);
    /* wait for piris origin done */
    wait_for_completion_timeout(&piris->piris_comp, MAX_TIMEOUT);
    spin_lock_irqsave(&piris->lock, flags);
    if (piris_data_info->zero_is_max == 1) {
        piris->src_pos  = piris_data_info->total_step - 1;
        piris->dest_pos = piris_data_info->total_step - 1;
    } else {
        piris->src_pos  = 0;
        piris->dest_pos = 0;
    }

    spin_unlock_irqrestore(&piris->lock, flags);
    return 0;
}

/* file operation */
#ifdef __LITEOS__
int piris_open(struct file *filep)
{
    ot_unused(filep);
    return 0;
}

int  piris_close(struct file *filep)
{
    ot_unused(filep);
    return 0;
}
#else
int piris_open(struct inode *inode, struct file *file)
{
    ot_unused(inode);
    ot_unused(file);
    if (!capable(CAP_SYS_RAWIO)) {
        return -EPERM;
    }
    return 0;
}
int  piris_close(struct inode *inode, struct file *file)
{
    ot_unused(inode);
    ot_unused(file);
    return 0;
}
#endif

int piris_drv_write(int dev, unsigned char bits)
{
    switch (bits) {
        case 0: /* case 0 */
            piris_write_reg(piris_a_ctrl_reg0(dev), 0xB);
            piris_write_reg(piris_b_ctrl_reg0(dev), 0x8);
            piris_write_reg(piris_a_cfg_reg0(dev), PIRIS_A_CASE0_REG0);
            piris_write_reg(piris_b_cfg_reg0(dev), PIRIS_B_CASE0_REG0);
            break;

        case 1: /* case 1 */
            piris_write_reg(piris_a_ctrl_reg0(dev), 0xB);
            piris_write_reg(piris_b_ctrl_reg0(dev), 0x8);
            piris_write_reg(piris_a_cfg_reg0(dev), PIRIS_A_CASE1_REG0);
            piris_write_reg(piris_b_cfg_reg0(dev), PIRIS_B_CASE1_REG0);
            break;

        case 2: /* case 2 */
            piris_write_reg(piris_a_ctrl_reg0(dev), 0xB);
            piris_write_reg(piris_b_ctrl_reg0(dev), 0x8);
            piris_write_reg(piris_a_cfg_reg0(dev), PIRIS_A_CASE2_REG0);
            piris_write_reg(piris_b_cfg_reg0(dev), PIRIS_B_CASE2_REG0);
            break;

        case 3: /* case 3 */
            piris_write_reg(piris_a_ctrl_reg0(dev), 0xB);
            piris_write_reg(piris_b_ctrl_reg0(dev), 0x8);
            piris_write_reg(piris_a_cfg_reg0(dev), PIRIS_A_CASE3_REG0);
            piris_write_reg(piris_b_cfg_reg0(dev), PIRIS_B_CASE3_REG0);
            break;

        default:
            break;
    }
    return 0;
}

#ifdef __LITEOS__
static int piris_ioctl(struct file *file, int cmd, unsigned long arg)
#else
static long piris_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    int ret;
    int piris_pos;
    piris_data    piris_data_info;
    piris_status  piris_status_info;
    piris_dev_s   *piris;

#ifdef __LITEOS__
    int dev;
#else
    long int dev;
#endif

#ifdef __LITEOS__
    dev = (int)(file->f_priv);
#else
    dev = (long int)(td_uintptr_t)(file->private_data);
#endif

    dev = MAX2(MIN2(dev, 1), 0);
    piris = g_piris_dev[dev];

    if (_IOC_TYPE(cmd) != PIRIS_IOC_MAGIC) {
        return -ENOTTY;
    }
    if (_IOC_NR(cmd) > PIRIS_IOC_MAXNR) {
        return -ENOTTY;
    }
    if (arg == 0L) {
        return -1;
    }
    /* lock piris */
    if (down_interruptible(&piris->sem)) {
        return -ERESTARTSYS;
    }

    switch (cmd) {
        case PIRIS_SET_FD:
#ifdef __LITEOS__
            if (read_user_linear_space_valid((td_u8 *)(td_uintptr_t)arg, sizeof(int)) == TD_FALSE) {
                up(&piris->sem);
                return -EFAULT;
            }

            ret = copy_from_user(&dev, (int __user *)(td_uintptr_t)arg, sizeof(int));
            if (ret) {
                up(&piris->sem);
                return -EFAULT;
            }
            file->f_priv = (void *)(td_uintptr_t)(dev);
#else
            if (read_user_linear_space_valid((td_u8 *)(td_uintptr_t)arg, sizeof(long int)) == TD_FALSE) {
                up(&piris->sem);
                return -EFAULT;
            }
            ret = osal_copy_from_user(&dev, (long int __user *)(td_uintptr_t)arg, sizeof(long int));
            if (ret) {
                up(&piris->sem);
                return -EFAULT;
            }
            file->private_data = (void *)(td_uintptr_t)(dev);
#endif
            break;

        case PIRIS_SET_ACT_ARGS:

            if (read_user_linear_space_valid((td_u8 *)(td_uintptr_t)arg, sizeof(int)) == TD_FALSE) {
                up(&piris->sem);
                return -EFAULT;
            }

            ret = osal_copy_from_user(&piris_pos, (int __user *)(td_uintptr_t)arg, sizeof(int));
            if (ret) {
                up(&piris->sem);
                return -EFAULT;
            }
            piris_gpio_update(dev, &piris_pos);
            break;

        case PIRIS_SET_ORGIN:
            if (read_user_linear_space_valid((td_u8 *)(td_uintptr_t)arg, sizeof(piris_data)) == TD_FALSE) {
                up(&piris->sem);
                return -EFAULT;
            }

            ret = osal_copy_from_user(&piris_data_info, (piris_data __user *)(td_uintptr_t)arg, sizeof(piris_data));
            if (ret) {
                up(&piris->sem);
                return -EFAULT;
            }
            piris_origin_set(dev, &piris_data_info);
            break;

        case PIRIS_SET_CLOSE:
            if (read_user_linear_space_valid((td_u8 *)(td_uintptr_t)arg, sizeof(piris_data)) == TD_FALSE) {
                up(&piris->sem);
                return -EFAULT;
            }
            ret = osal_copy_from_user(&piris_data_info, (piris_data __user *)(td_uintptr_t)arg, sizeof(piris_data));
            if (ret) {
                up(&piris->sem);
                return -EFAULT;
            }
            piris_close_set(dev, &piris_data_info);
            break;

        case PIRIS_GET_STATUS:
            if (piris->dest_pos != piris->src_pos) {
                piris_status_info = PIRIS_BUSY;
            } else {
                piris_status_info = PIRIS_IDLE;
            }

            if (write_user_linear_space_valid((td_u8 *)(td_uintptr_t)arg, sizeof(piris_status)) == TD_FALSE) {
                up(&piris->sem);
                return -EFAULT;
            }

            ret = osal_copy_to_user((piris_status __user *)(td_uintptr_t)arg, &piris_status_info, sizeof(piris_status));
            if (ret) {
                up(&piris->sem);
                return -EFAULT;
            }
            break;

        default:  /* redundant, as cmd was checked against MAXNR */
            break;
    }

    /* unlock piris */
    up(&piris->sem);

    return 0;
}

#ifdef __LITEOS__
static struct file_operations_vfs g_piris_fops = {
    .ioctl = piris_ioctl,
    .open  = piris_open,
    .close = piris_close,
};

int ot_piris_device_register(void)
{
    return register_driver("/dev/piris", &g_piris_fops, 0666, 0); /* register addr, 0666 */
}

#else
static struct file_operations g_piris_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = piris_ioctl,
    .open           = piris_open,
    .release        = piris_close,
};
#endif

#ifndef __LITEOS__
static struct miscdevice g_st_piris_dev = {
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "piris",
    .fops    = &g_piris_fops,
};
#endif

#ifdef __LITEOS__
void piris_timer_cb(UINT32 arg)
#else
void piris_timer_cb(unsigned long arg)
#endif
{
    int sign;
    unsigned char bits;
    unsigned long flags = 0;
    piris_dev_s *piris = TD_NULL;

    sign = 1;
    piris = g_piris_dev[g_dev];
    spin_lock_irqsave(&piris->lock, flags);

    if (piris->src_pos == piris->dest_pos) {
        spin_unlock_irqrestore(&piris->lock, flags);
        return;
    }

    sign = (piris->dest_pos - piris->src_pos < 0) ? -1 : 1;
    piris->src_pos += sign;

    /* close iris: 0->1->2->3->0;    open iris: 3->2->1->0->3 */
    piris->phase = (piris->phase + MAX_MOTOR_PAHSE + sign) % MAX_MOTOR_PAHSE;
    bits = piris->phase_tbl[piris->phase];
    piris_drv_write(g_dev, bits);
    if (piris->dest_pos == piris->src_pos) {
        complete(&piris->piris_comp);
    }
    spin_unlock_irqrestore(&piris->lock, flags);
    osal_set_timer(&piris->timer, 1);
    return;
}

static int ot_piris_isp_register(int dev)
{
    isp_piris_callback piris_cb = {0};

    piris_cb.pfn_piris_gpio_update = piris_gpio_update;
    if (ckfn_isp() && ckfn_isp_register_piris_callback()) {
        call_isp_register_piris_callback(dev, &piris_cb);
    } else {
        osal_printk("register piris_gpio_write_callback to isp failed, ot_piris init is failed!\n");
        return -1;
    }

    return 0;
}

static int ot_piris_isp_unregister(int dev)
{
    isp_piris_callback piris_cb = {0};

    if (ckfn_isp() && ckfn_isp_register_piris_callback()) {
        call_isp_register_piris_callback(dev, &piris_cb);
    } else {
        osal_printk("unregister piris_gpio_write_callback to isp failed!\n");
        return -1;
    }

    return 0;
}
#define PIRISI_SIZE_ALIGN 0x10000
int piris_init_remap(int i)
{
    int j;
    if (!g_reg_piris_ibase[i]) {
        g_reg_piris_ibase[i] = osal_ioremap(PIRISI_ADRESS_BASE, PIRISI_SIZE_ALIGN);
        if (!g_reg_piris_ibase[i]) {
            for (j = 0; j < i; j++) {
                osal_iounmap(g_reg_piris_ibase[j]);
            }

#ifdef __LITEOS__
            unregister_driver("/dev/piris");
#else
            misc_deregister(&g_st_piris_dev);
#endif
            g_piris_io_remap = TD_FALSE;
            return -1;
        }
    }

    return 0;
}

int piris_open_dev(int i, piris_dev_s *pdev)
{
    int j;

    if (pdev == TD_NULL) {
        for (j = 0; j < i; j++) {
            kfree(g_piris_dev[j]);
            g_piris_dev[j] = TD_NULL;
        }

#ifdef __LITEOS__
        unregister_driver("/dev/piris");
#else
        misc_deregister(&g_st_piris_dev);
#endif

        for (j = 0; j < PIRIS_MAX_DEV_NUM; j++) {
            if (g_piris_io_remap == TD_TRUE) {
                iounmap(g_reg_piris_ibase[j]);
            }
        }

        g_piris_io_remap = TD_FALSE;

        return -1;
    }
    (td_void)memset_s(pdev, sizeof(piris_dev_s), 0x0, sizeof(piris_dev_s));
    g_piris_dev[i] = pdev;

    return 0;
}

/* module init and exit */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
int __init piris_init(void)
#else
int piris_init(void)
#endif
{
    int ret, i;
    piris_dev_s *pdev = TD_NULL;
#ifdef __LITEOS__
    ret = ot_piris_device_register();
#else
    ret = misc_register(&g_st_piris_dev);
#endif
    if (ret != 0) {
        osal_printk("register piris device failed with %#x!\n", ret);
        return -1;
    }
    /* initiate piris map */
    for (i = 0; i < PIRIS_MAX_DEV_NUM; i++) {
        ret = piris_init_remap(i);
        if (ret != 0) {
            osal_printk("initiate piris map failed at piris dev[%d]!\n", i);
            return -1;
        }
    }
    g_piris_io_remap = TD_TRUE;
    for (i = 0; i < PIRIS_MAX_DEV_NUM; i++) {
        pdev = kmalloc(sizeof(piris_dev_s), GFP_KERNEL);
        piris_check_pointer_return(pdev);
        ret = piris_open_dev(i, pdev);
        if (ret != 0) {
            osal_printk("open piris dev failed at piris dev[%d]!\n", i);
            return -1;
        }
    }
    for (i = 0; i < PIRIS_MAX_DEV_NUM; i++) {
        spin_lock_init(&g_piris_dev[i]->lock);
        sema_init(&g_piris_dev[i]->sem, 1);
        init_completion(&g_piris_dev[i]->piris_comp);
#ifndef __LITEOS__
        g_piris_dev[i]->timer.function = piris_timer_cb;
#else
        g_piris_dev[i]->timer.function = (void *)piris_timer_cb;
        g_piris_dev[i]->timer.data = (unsigned long)(td_uintptr_t)g_piris_dev[i];
#endif
        osal_timer_init(&g_piris_dev[i]->timer);
        g_piris_dev[i]->phase_tbl = g_motor_phase_tbl;
        ot_piris_isp_register(i);
    }
    osal_printk("load ot_piris.ko ....OK!\n");
    return 0;
}

void __exit piris_exit(void)
{
    int i;
    piris_dev_s *pdev = TD_NULL;

    for (i = 0; i < PIRIS_MAX_DEV_NUM; i++) {
        pdev = g_piris_dev[i];
        osal_timer_destroy(&pdev->timer);
        if (g_piris_io_remap == TD_TRUE) {
            iounmap(g_reg_piris_ibase[i]);
        }
        kfree(pdev);
        pdev = TD_NULL;
        ot_piris_isp_unregister(i);
    }

    g_piris_io_remap = TD_FALSE;

#ifdef __LITEOS__
    unregister_driver("/dev/piris");
#else
    misc_deregister(&g_st_piris_dev);
    osal_printk("unload ot_piris.ko ....OK!\n");
#endif
}

#ifndef __LITEOS__
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
module_init(piris_init);
module_exit(piris_exit);

MODULE_DESCRIPTION("piris driver");
MODULE_LICENSE("GPL");

#else
#include <linux/of_platform.h>
static int piris_probe(struct platform_device *pdev)
{
    struct resource *mem = TD_NULL;
    int i;

    for (i = 0; i < PIRIS_MAX_DEV_NUM; i++) {
        mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        g_reg_piris_ibase[i] = devm_ioremap_resource(&pdev->dev, mem);
        if (IS_ERR(g_reg_piris_ibase[i])) {
            return PTR_ERR(g_reg_piris_ibase[i]);
        }
    }

    piris_init();
    return 0;
}

static int piris_remove(struct platform_device *pdev)
{
    osal_printk("<%s> is called\n", __FUNCTION__);
    piris_exit();
    return 0;
}

static const struct of_device_id g_piris_match[] = {
    { .compatible = "vendor,piris" },
    {},
};

MODULE_DEVICE_TABLE(of, g_piris_match);
static struct platform_driver g_piris_driver = {
    .probe          = piris_probe,
    .remove         = piris_remove,
    .driver         = {
        .name   = "piris",
        .of_match_table = g_piris_match,
    }
};

module_platform_driver(g_piris_driver);
MODULE_LICENSE("GPL");
#endif
#endif

