/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "pwm.h"
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <asm/io.h>

#include "ot_osal.h"
#include "mm_ext.h"


#ifdef __LITEOS__
#ifndef __iomem
#define __iomem
#endif
#ifndef ENOIOCTLCMD
#define ENOIOCTLCMD     515 /* No ioctl command */
#endif
#endif

#include "pwm_arch.h"

void __iomem *g_reg_pwm_ibase = 0;

#define ot_io_pwmi_address(x)  ((void __iomem *)((unsigned char __iomem *)g_reg_pwm_ibase + ((x)-(PWMI_ADRESS_BASE))))

/* PWMI */

#define PWM0_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0000)
#define PWM0_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0004)
#define PWM0_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0010)
#define PWM0_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0014)

#define PWM1_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0100)
#define PWM1_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0104)
#define PWM1_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0110)
#define PWM1_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0114)

#define PWM2_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0200)
#define PWM2_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0204)
#define PWM2_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0210)
#define PWM2_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0214)

#define PWM3_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0100)
#define PWM3_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0104)
#define PWM3_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0110)
#define PWM3_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0114)

#define PWM4_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0400)
#define PWM4_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0404)
#define PWM4_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0410)
#define PWM4_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0414)

#define PWM5_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0500)
#define PWM5_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0504)
#define PWM5_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0510)
#define PWM5_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0514)

#define PWM6_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0600)
#define PWM6_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0604)
#define PWM6_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0610)
#define PWM6_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0614)

#define PWM7_CFG_REG0   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0700)
#define PWM7_CFG_REG1   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0704)
#define PWM7_CFG_REG2   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0710)
#define PWM7_CTRL_REG   ot_io_pwmi_address(PWMI_ADRESS_BASE + 0x0714)

#define  pwm_write_reg(addr, value) ((*(volatile unsigned int *)(addr)) = (value))
#define  pwm_read_reg(addr)         (*(volatile unsigned int *)(addr))

/* PWM */
#define PWM_NUM_MAX            0x08
#define PWM_ENABLE             0x01
#define PWM_DISABLE            0x00
#define PWM_CHN1_WORK_MODE     0x16
#define PWM_DEVNAME     "pwm"

static int pwm_drv_disable(unsigned char pwm_num)
{
    if (pwm_num >= PWM_NUM_MAX) {
        osal_printk("The pwm number is big than the max value!\n");
        return -1;
    }
    switch (pwm_num) {
        case 0: /* case 0 */
            pwm_write_reg(PWM0_CTRL_REG, PWM_DISABLE);
            break;
        case 1: /* case 1 */
            pwm_write_reg(PWM1_CTRL_REG, PWM_DISABLE);
            break;
        case 2: /* case 2 */
            pwm_write_reg(PWM2_CTRL_REG, PWM_DISABLE);
            break;
        case 3: /* case 3 */
            pwm_write_reg(PWM3_CTRL_REG, PWM_DISABLE);
            break;
        case 4: /* case 4 */
            pwm_write_reg(PWM4_CTRL_REG, PWM_DISABLE);
            break;
        case 5: /* case 5 */
            pwm_write_reg(PWM5_CTRL_REG, PWM_DISABLE);
            break;
        case 6: /* case 6 */
            pwm_write_reg(PWM6_CTRL_REG, PWM_DISABLE);
            break;
        case 7: /* case 7 */
            pwm_write_reg(PWM7_CTRL_REG, PWM_DISABLE);
            break;
        default:
            break;
    }

    return 0;
}

int pwm_drv_write(unsigned char pwm_num, unsigned int duty, unsigned int period, unsigned char enable)
{
    if (pwm_num >= PWM_NUM_MAX) {
        osal_printk("The pwm number is big than the max value!\n");
        return -1;
    }
    if (enable) {
        switch (pwm_num) {
            case 0:  /* case 0 */
                pwm_write_reg(PWM0_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM0_CFG_REG0, period);
                pwm_write_reg(PWM0_CFG_REG1, duty);
                pwm_write_reg(PWM0_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM0_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            case 1:  /* case 1 */
                pwm_write_reg(PWM1_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM1_CFG_REG0, period);
                pwm_write_reg(PWM1_CFG_REG1, duty);
                pwm_write_reg(PWM1_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM1_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            case 2: /* case 2 */
                pwm_write_reg(PWM2_CTRL_REG, PWM_DISABLE);

                pwm_write_reg(PWM2_CFG_REG0, period);
                pwm_write_reg(PWM2_CFG_REG1, duty);
                pwm_write_reg(PWM2_CFG_REG2, 10); /* pwm output number */

                pwm_write_reg(PWM2_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            case 3: /* case 3 */
                pwm_write_reg(PWM3_CTRL_REG, PWM_CHN1_WORK_MODE | PWM_DISABLE); /* work mode cfg */
                pwm_write_reg(PWM3_CFG_REG0, period);
                pwm_write_reg(PWM3_CFG_REG1, duty);       /* duty0 */
                pwm_write_reg(PWM3_CFG_REG1 + 0x4, duty); /* duty1 */
                pwm_write_reg(PWM3_CFG_REG1 + 0x8, duty); /* duty2 */
                pwm_write_reg(PWM3_CTRL_REG, PWM_CHN1_WORK_MODE | PWM_ENABLE);  /* work mode cfg */
                break;

            case 4: /* case 4 */
                pwm_write_reg(PWM4_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM4_CFG_REG0, period);
                pwm_write_reg(PWM4_CFG_REG1, duty);
                pwm_write_reg(PWM4_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM4_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            case 5: /* case 5 */
                pwm_write_reg(PWM5_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM5_CFG_REG0, period);
                pwm_write_reg(PWM5_CFG_REG1, duty);
                pwm_write_reg(PWM5_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM5_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            case 6: /* case 6 */
                pwm_write_reg(PWM6_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM6_CFG_REG0, period);
                pwm_write_reg(PWM6_CFG_REG1, duty);
                pwm_write_reg(PWM6_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM6_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            case 7: /* case 7 */
                pwm_write_reg(PWM7_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM7_CFG_REG0, period);
                pwm_write_reg(PWM7_CFG_REG1, duty);
                pwm_write_reg(PWM7_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM7_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;

            default:
                pwm_write_reg(PWM0_CTRL_REG, PWM_DISABLE);
                pwm_write_reg(PWM0_CFG_REG0, period);
                pwm_write_reg(PWM0_CFG_REG1, duty);
                pwm_write_reg(PWM0_CFG_REG2, 10); /* pwm output number */
                pwm_write_reg(PWM0_CTRL_REG, ((1 << 2) | PWM_ENABLE)); /* keep the pwm always output; */
                break;
        }
    } else {
        pwm_drv_disable(pwm_num);
    }

    return 0;
}

/* file operation */
#ifdef __LITEOS__
int pwm_open(struct file *file)
{
    return 0;
}

int  pwm_close(struct file *file)
{
    return 0;
}
#else
int pwm_open(struct inode *inode, struct file *file)
{
    ot_unused(inode);
    ot_unused(file);
    return 0;
}

int  pwm_close(struct inode *inode, struct file *file)
{
    ot_unused(inode);
    ot_unused(file);
    return 0;
}
#endif

#ifdef __LITEOS__
static int pwm_ioctl(struct file *file, int cmd, unsigned long arg)
#else
static long pwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    int ret;
    unsigned char  pwm_num;
    unsigned int duty;
    unsigned int period;
    unsigned char  enable;
    pwm_data pwm_data_info;
    ot_unused(file);

    switch (cmd) {
        case PWM_CMD_WRITE:
            if (arg == 0L) {
                return -1;
            }

            if (read_user_linear_space_valid((unsigned char *)(td_uintptr_t)arg, sizeof(pwm_data)) == 0) {
                osal_printk("pwm address is invalid!\n");
                return -EFAULT;
            }

            ret = osal_copy_from_user(&pwm_data_info, (pwm_data __user *)(td_uintptr_t)arg, sizeof(pwm_data));
            if (ret) {
                return -EFAULT;
            }

            pwm_num = pwm_data_info.pwm_num;
            duty    = pwm_data_info.duty;
            period  = pwm_data_info.period;
            enable  = pwm_data_info.enable;

            pwm_drv_write(pwm_num, duty, period, enable);
            break;

        case PWM_CMD_READ:
            break;

        default:
            osal_printk("invalid ioctl command!\n");
            return -ENOIOCTLCMD;
    }

    return 0;
}

#ifdef __LITEOS__
const static struct file_operations_vfs g_pwm_fops = {
    .open = pwm_open,
    .close = pwm_close,
    .ioctl = pwm_ioctl,
};
#else
static struct file_operations g_pwm_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl = pwm_ioctl,
    .open       = pwm_open,
    .release    = pwm_close,
};
#endif

#ifndef __LITEOS__
static struct miscdevice g_pwm_dev = {
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "pwm",
    .fops    = &g_pwm_fops,
};
#endif

/* module init and exit */
#ifdef __LITEOS__
int pwm_init(void)
{
    int     ret;
    g_reg_pwm_ibase = (void __iomem *)IO_ADDRESS(PWMI_ADRESS_BASE);

    ret = register_driver("/dev/pwm", &g_pwm_fops, 0666, 0); /* register address 0666 */
    if (ret) {
        osal_printk("register pwd device failed with %#x!\n", ret);
        return -1;
    }

    return 0;
}

void pwm_exit(void)
{
    int i;

    for (i = 0; i < PWM_NUM_MAX; i++) {
        pwm_drv_disable(i);
    }

    g_reg_pwm_ibase = TD_NULL;
    unregister_driver("/dev/pwm");
}
#else

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
static int __init pwm_init(void)
#else
static int pwm_init(void)
#endif
{
    int     ret;

    if (!g_reg_pwm_ibase) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
        g_reg_pwm_ibase = (void __iomem *)osal_ioremap(PWMI_ADRESS_BASE, 0xFFF);
#else
        g_reg_pwm_ibase = TD_NULL;
#endif
    }

    ret = misc_register(&g_pwm_dev);
    if (ret != 0) {
        osal_printk("register i2c device failed with %#x!\n", ret);
        return -1;
    }

    osal_printk("load pwm.ko ....OK!\n");

    return 0;
}
static void __exit pwm_exit(void)
{
    int i;
    for (i = 0; i < PWM_NUM_MAX; i++) {
        pwm_drv_disable(i);
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
    if (g_reg_pwm_ibase) {
        osal_iounmap((td_void *)g_reg_pwm_ibase);
        g_reg_pwm_ibase = TD_NULL;
    }
#else
    g_reg_pwm_ibase = TD_NULL;
#endif

    misc_deregister(&g_pwm_dev);

    osal_printk("unload pwm.ko ....OK!\n");
}
#endif

#ifndef __LITEOS__
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
module_init(pwm_init);
module_exit(pwm_exit);

MODULE_DESCRIPTION("PWM Driver");
MODULE_LICENSE("GPL");
#else
#include <linux/of_platform.h>
static int pwm_probe(struct platform_device *pdev)
{
    struct resource *mem;

    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    g_reg_pwm_ibase = devm_ioremap_resource(&pdev->dev, mem);
    if (IS_ERR(g_reg_pwm_ibase)) {
        return PTR_ERR(g_reg_pwm_ibase);
    }

    osal_printk("++++++++++ g_reg_pwm_ibase = %p\n", g_reg_pwm_ibase);

    pwm_init();
    return 0;
}
static int pwm_remove(struct platform_device *pdev)
{
    osal_printk("<%s> is called\n", __FUNCTION__);
    pwm_exit();
    return 0;
}
static const struct of_device_id g_pwm_match[] = {
    { .compatible = "vendor,pwm" },
    {},
};
MODULE_DEVICE_TABLE(of, g_pwm_match);
static struct platform_driver g_pwm_driver = {
    .probe          = pwm_probe,
    .remove         = pwm_remove,
    .driver         = {
        .name   = "pwm",
        .of_match_table = g_pwm_match,
    }
};
module_platform_driver(g_pwm_driver);
MODULE_LICENSE("GPL");
#endif
#endif
