/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#ifndef __LITEOS__
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/miscdevice.h>

#include <linux/proc_fs.h>
#include <linux/poll.h>

#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>


#define SSP_DBG_ERR     KERN_ALERT
#define SSP_DBG_INFO    KERN_DEBUG
#define ssp_trace(level, fmt, ...) printk(level fmt, ##__VA_ARGS__)

#else

#ifndef __iomem
#define __iomem
#endif

#define SSP_DBG_ERR     "err"
#define SSP_DBG_INFO    "inf"
#define ssp_trace(level, fmt, ...)   dprintf(fmt, ##__VA_ARGS__)
#endif

#include "ot_ssp.h"
#include "isp_ext.h"

#define DEV_NAME        "ssp"
#define MAX_ISP_DEV_NUM 6
#define SSP_DEV_NUM     5

#define  ssp_readw(addr, ret)            ((ret) = (*(volatile unsigned int *)(addr)))
#define  ssp_writew(addr, value)         ((*(volatile unsigned int *)(addr)) = (value))

#define  ot_reg_read(addr, ret)          ((ret) = (*(volatile unsigned int *)(addr)))
#define  ot_reg_write(addr, value)       ((*(volatile unsigned int *)(addr)) = (value))

#define SSP_BASE        0x12120000
#define SSP_SIZE        0x10000             /* 64KB */
#define SSP_INT         41                  /* Interrupt No. */

static void __iomem *g_reg_ssp_base_va = TD_NULL;
#define io_address_verify(x) ((void __iomem *)((unsigned char __iomem *)g_reg_ssp_base_va + ((x)-(SSP_BASE))))

/* SSP register definition . */
#define ssp_cr0(ssp_no)     io_address_verify(SSP_BASE + 0x00 + ((ssp_no) * 0x1000))
#define ssp_cr1(ssp_no)     io_address_verify(SSP_BASE + 0x04 + ((ssp_no) * 0x1000))
#define ssp_dr(ssp_no)      io_address_verify(SSP_BASE + 0x08 + ((ssp_no) * 0x1000))
#define ssp_sr(ssp_no)      io_address_verify(SSP_BASE + 0x0C + ((ssp_no) * 0x1000))
#define ssp_cpsr(ssp_no)    io_address_verify(SSP_BASE + 0x10 + ((ssp_no) * 0x1000))
#define ssp_imsc(ssp_no)    io_address_verify(SSP_BASE + 0x14 + ((ssp_no) * 0x1000))
#define ssp_ris(ssp_no)     io_address_verify(SSP_BASE + 0x18 + ((ssp_no) * 0x1000))
#define ssp_mis(ssp_no)     io_address_verify(SSP_BASE + 0x1C + ((ssp_no) * 0x1000))
#define ssp_icr(ssp_no)     io_address_verify(SSP_BASE + 0x20 + ((ssp_no) * 0x1000))
#define ssp_dmacr(ssp_no)   io_address_verify(SSP_BASE + 0x24 + ((ssp_no) * 0x1000))

static spinlock_t g_ssp_sony_lock;
#define ssp_spin_lock_init()    spin_lock_init(&g_ssp_sony_lock)
#define ssp_spin_lock(flags)    spin_lock_irqsave(&g_ssp_sony_lock, flags)
#define ssp_spin_unlock(flags)  spin_unlock_irqrestore(&g_ssp_sony_lock, flags)

static void ot_ssp_disable(unsigned int ssp_no)
{
    int ret = 0;
    ssp_readw(ssp_cr1(ssp_no), ret);
    ret = ret & (~(0x1 << 1));
    ssp_writew(ssp_cr1(ssp_no), ret);
}

/*
 * set SSP frame form routine.
 *
 * @param framemode: frame form
 * 00: Motorola SPI frame form.
 * when set the mode,need set SSPCLKOUT phase and SSPCLKOUT voltage level.
 * 01: TI synchronous serial frame form
 * 10: National Microwire frame form
 * 11: reserved
 * @param sphvalue: SSPCLKOUT phase (0/1)
 * @param sp0: SSPCLKOUT voltage level (0/1)
 * @param datavalue: data bit
 * 0000: reserved    0001: reserved    0010: reserved    0011: 4bit data
 * 0100: 5bit data   0101: 6bit data   0110:7bit data    0111: 8bit data
 * 1000: 9bit data   1001: 10bit data  1010:11bit data   1011: 12bit data
 * 1100: 13bit data  1101: 14bit data  1110:15bit data   1111: 16bit data
 *
 * @return value: 0--success; -1--error.
 *
 */
static int ot_ssp_set_frameform(unsigned int ssp_no, unsigned char framemode,
                                unsigned char spo, unsigned char sph, unsigned char datawidth)
{
    int ret = 0;
    ssp_readw(ssp_cr0(ssp_no), ret);
    if (framemode > 3) { /* mode 3 */
        ssp_trace(SSP_DBG_ERR, "set frame parameter err.\n");
        return -1;
    }
    ret = (ret & 0xFFCF) | (framemode << 4); /* mode shift by 4 */
    if ((ret & 0x30) == 0) {
        if (spo > 1) {
            ssp_trace(SSP_DBG_ERR, "set spo parameter err.\n");
            return -1;
        }
        if (sph > 1) {
            ssp_trace(SSP_DBG_ERR, "set sph parameter err.\n");
            return -1;
        }
        ret = (ret & 0xFF3F) | (sph << 7) | (spo << 6); /* sph shift by 7 and 6 */
    }
    if ((datawidth > 16) || (datawidth < 4)) { /* bit width should be no larger than 16 or smaller than 4 */
        ssp_trace(SSP_DBG_ERR, "set datawidth parameter err.\n");
        return -1;
    }
    ret = (ret & 0xFFF0) | (datawidth - 1);
    ssp_writew(ssp_cr0(ssp_no), ret);
    return 0;
}

/*
 * set SSP serial clock rate routine.
 *
 * @param scr: scr value.(0-255,usually it is 0)
 * @param cpsdvsr: Clock prescale divisor.(2-254 even)
 *
 * @return value: 0--success; -1--error.
 *
 */
static int ot_ssp_set_serialclock(unsigned int ssp_no, unsigned char scr, unsigned char cpsdvsr)
{
    int ret = 0;
    ssp_readw(ssp_cr0(ssp_no), ret);
    ret = (ret & 0xFF) | (scr << 8); /* scr shift by 8 */
    ssp_writew(ssp_cr0(ssp_no), ret);
    if ((cpsdvsr & 0x1)) {
        ssp_trace(SSP_DBG_ERR, "set cpsdvsr parameter err.\n");
        return -1;
    }
    ssp_writew(ssp_cpsr(ssp_no), cpsdvsr);
    return 0;
}

static int ot_ssp_alt_mode_set(unsigned int ssp_no, int enable)
{
    int ret = 0;

    ssp_readw(ssp_cr1(ssp_no), ret);
    if (enable) {
        ret = ret & (~0x40);
    } else {
        ret = (ret & 0xFF) | 0x40;
    }
    ssp_writew(ssp_cr1(ssp_no), ret);

    return 0;
}

static unsigned int ot_ssp_is_fifo_empty(unsigned int ssp_no, int send)
{
    unsigned int ret = 0;
    ssp_readw(ssp_sr(ssp_no), ret);

    if (send) {
        if ((ret & 0x1) == 0x1) { /* send fifo */
            return 1;
        } else {
            return 0;
        }
    } else {
        if ((ret & 0x4) == 0x4) { /* receive fifo */
            return 0;
        } else {
            return 1;
        }
    }
}

static void spi_enable(unsigned int ssp_no)
{
    /* little endian */
    ot_reg_write(ssp_cr1(ssp_no), 0x42);
}

static void spi_disable(unsigned int ssp_no)
{
    ot_reg_write(ssp_cr1(ssp_no), 0x40);
}

int g_spo = 1;
int g_sph = 1;

#ifndef __LITEOS__

module_param(g_spo, int, S_IRUGO);
module_param(g_sph, int, S_IRUGO);
#endif

static int ot_ssp_init_cfg(unsigned int ssp_no)
{
    const unsigned char framemode = 0;
    unsigned char spo;
    unsigned char sph;
    const unsigned char datawidth = 8;

#ifdef OT_FPGA
    const unsigned char scr = 1;
    const unsigned char cpsdvsr = 2;
#else
    const unsigned char scr = 1;  /* 8 */
    const unsigned char cpsdvsr = 8;
#endif

    spi_disable(ssp_no);

    if (ssp_no == 0) {
        spo = 0;
        sph = 0;
        ot_ssp_set_frameform(ssp_no, framemode, spo, sph, datawidth);
    } else {
        spo = 1;
        sph = 1;
        ot_ssp_set_frameform(ssp_no, framemode, spo, sph, datawidth);
    }

    ot_ssp_set_serialclock(ssp_no, scr, cpsdvsr);

    /* altasens mode, which CS won't be pull high between 16bit data transfer */
    ot_ssp_alt_mode_set(ssp_no, 0);

    return 0;
}

static unsigned short ot_ssp_read_alt(unsigned int ssp_no, unsigned short devaddr,
                                      unsigned short addr_high, unsigned short addr_low)
{
    unsigned int ret;
    unsigned short value;
    const unsigned short dontcare = 0x00;
    unsigned long flags;

    ot_unused(devaddr);
    ot_unused(addr_high);

    value = 0;

    ssp_spin_lock(flags);

    spi_enable(ssp_no);

    if (ssp_no == 0) {
        ssp_writew(ssp_dr(ssp_no), (addr_low & 0x7f));
    } else {
        ssp_writew(ssp_dr(ssp_no), ((addr_low << 1) | 0x01));
    }

    ssp_writew(ssp_dr(ssp_no), dontcare);

    while (ot_ssp_is_fifo_empty(ssp_no, 0)) {};
    ssp_readw(ssp_dr(ssp_no), ret);

    while (ot_ssp_is_fifo_empty(ssp_no, 0)) {};
    ssp_readw(ssp_dr(ssp_no), ret);

    spi_disable(ssp_no);
    value = (unsigned short)(ret & 0xff);

    ssp_spin_unlock(flags);

    return value;
}

static int ot_ssp_write_alt(unsigned int ssp_no, unsigned short devaddr,
                            unsigned short addr_high, unsigned short addr_low, unsigned short data)
{
    unsigned int ret;
    unsigned long flags;

    ot_unused(devaddr);
    ot_unused(addr_high);

    ssp_spin_lock(flags);

    spi_enable(ssp_no);

    if (ssp_no == 0) {
        ssp_writew(ssp_dr(ssp_no), (addr_low | 0x80));
    } else {
        ssp_writew(ssp_dr(ssp_no), ((addr_low << 1) & 0xfe));
    }

    ssp_writew(ssp_dr(ssp_no), data);

    /* wait receive fifo has data */
    while (ot_ssp_is_fifo_empty(ssp_no, 0)) {};
    ssp_readw(ssp_dr(ssp_no), ret);

    /* wait receive fifo has data */
    while (ot_ssp_is_fifo_empty(ssp_no, 0)) {};
    ssp_readw(ssp_dr(ssp_no), ret);

    spi_disable(ssp_no);

    ssp_spin_unlock(flags);

    ret = 0;
    return ret;
}

static int ot_ssp_write(unsigned int ssp_no, unsigned int spi_csn,
                        unsigned int addr1, unsigned int addr1bytenum,
                        unsigned int addr2, unsigned int addr2bytenum,
                        unsigned int data, unsigned int databytenum)
{
    unsigned short spi_no, devaddr, addr_high, addr_low, data_value;

    ot_unused(spi_csn);
    ot_unused(addr1bytenum);
    ot_unused(addr2bytenum);
    ot_unused(databytenum);

    spi_no = (unsigned short)(ssp_no & 0xff);
    devaddr   = (unsigned short)(addr1 & 0xff);
    addr_high = (unsigned short)((addr2 & 0xff00) >> 8); /* address 2 shift by 8 */
    addr_low  = (unsigned short)(addr2 & 0xff);
    data_value = (unsigned short)(data & 0xff);

    return ot_ssp_write_alt(spi_no, devaddr, addr_high, addr_low, data_value);
}

#ifdef __LITEOS__
static int ssp_ioctl(struct file *file, int cmd, unsigned long arg)
#else
static long ssp_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
    unsigned short spi_no, devaddr, addr_high, addr_low, data;
    ot_spi_data __user *argp = (ot_spi_data __user *)(td_uintptr_t)arg;
    ot_spi_data spi_data;
    ot_unused(file);

    if (copy_from_user(&spi_data, argp, sizeof(ot_spi_data))) {
        return -EFAULT;
    }

    switch (cmd) {
        case OT_SSP_READ_ALT:
            /* MN34041 LSB first! */
            spi_no = ((unsigned short)(spi_data.spi_no & 0xff));
            devaddr = ((unsigned short)(spi_data.dev_addr & 0xff));
            addr_high = ((unsigned short)((spi_data.reg_addr & 0xff00) >> 8)); /* reg_addr shift by 8 */
            addr_low = ((unsigned short)(spi_data.reg_addr & 0xff));

            if (spi_no >= SSP_DEV_NUM) {
                ssp_trace(SSP_DBG_ERR, "Kernel: spi_no[%u] is out of range!\n", spi_no);
                return -1;
            }

            data = ot_ssp_read_alt(spi_no, devaddr, addr_high, addr_low);

            spi_data.data = (unsigned int)(data & 0xff);
            if (copy_to_user(argp, &spi_data, sizeof(ot_spi_data))) {
                return -EFAULT;
            }
            break;

        case OT_SSP_WRITE_ALT:
            spi_no = (unsigned short)(spi_data.spi_no & 0xff);
            devaddr = (unsigned short)(spi_data.dev_addr & 0xff);
            addr_high = (unsigned short)((spi_data.reg_addr & 0xff00) >> 8); /* reg_addr shift by 8 */
            addr_low = (unsigned short)(spi_data.reg_addr & 0xff);
            data    = (unsigned short)(spi_data.data & 0xff);

            if (spi_no >= SSP_DEV_NUM) {
                ssp_trace(SSP_DBG_ERR, "Kernel: spi_no[%u] is out of range!\n", spi_no);
                return -1;
            }
            ot_ssp_write_alt(spi_no, devaddr, addr_high, addr_low, data);
            break;

        default: {
            ssp_trace(SSP_DBG_ERR, "Kernel: No such ssp command %#x!\n", cmd);
            return -1;
        }
    }

    return 0;
}

#ifdef __LITEOS__
static int ssp_open(struct file *file)
{
    ot_unused(file);
    return 0;
}
static int ssp_close(struct file *file)
{
    ot_unused(file);
    return 0;
}
const static struct file_operations_vfs g_ssp_fops = {
    .open = ssp_open,
    .close = ssp_close,
    .ioctl = ssp_ioctl
};

int ot_ssp_init(void *args)
{
    unsigned int ssp_no;
    int ret;

    isp_bus_callback bus_cb = {0};
    bus_cb.pfn_isp_write_ssp_data = ot_ssp_write;

    if (ckfn_isp() && ckfn_isp_register_bus_callback()) {
        ot_isp_dev isp_dev;
        for (isp_dev = 0;  isp_dev < MAX_ISP_DEV_NUM; isp_dev++) {
            call_isp_register_bus_callback(isp_dev, ISP_BUS_TYPE_SSP, &bus_cb);
        }
    } else {
        ssp_trace(SSP_DBG_ERR, "register ssp_write_callback to isp failed, ssp init is failed!\n");
        return -1;
    }

    g_reg_ssp_base_va = (void __iomem *)IO_ADDRESS((unsigned long)SSP_BASE);

    ret = register_driver("/dev/ssp", &g_ssp_fops, 0666, 0); /* register address 0666 */
    if (ret) {
        ssp_trace(SSP_DBG_ERR, "register pwd device failed with %#x!\n", ret);
        return -1;
    }

    for (ssp_no = 0; ssp_no < SSP_DEV_NUM; ssp_no++) {
        ret = ot_ssp_init_cfg(ssp_no);
        if (ret) {
            ssp_trace(SSP_DBG_ERR, "Debug: ssp initial failed!\n");
            return -1;
        }
    }

    ssp_spin_lock_init();
    ssp_trace(SSP_DBG_INFO, "Kernel: ssp initial ok!\n");

    return 0;
}

void ot_ssp_exit(void)
{
    unsigned int ssp_no;

    for (ssp_no = 0; ssp_no < SSP_DEV_NUM; ssp_no++) {
        ot_ssp_disable(ssp_no);
    }

    g_reg_ssp_base_va = TD_NULL;
    unregister_driver("/dev/ssp");
}

#else
static int ssp_open(struct inode *inode, struct file *file)
{
    ot_unused(file);
    ot_unused(inode);
    return 0;
}
static int ssp_close(struct inode *inode, struct file *file)
{
    ot_unused(file);
    ot_unused(inode);
    return 0;
}

static struct file_operations g_ssp_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl         = ssp_ioctl,
    .open       = ssp_open,
    .release    = ssp_close
};


static struct miscdevice g_ssp_dev = {
    .minor       = MISC_DYNAMIC_MINOR,
    .name        = DEV_NAME,
    .fops        = &g_ssp_fops,
};


/*
 * initializes SSP interface routine.
 *
 * @return value:0--success.
 *
 */
static int __init ot_ssp_init(void)
{
    unsigned int ssp_no;
    int ret;
    isp_bus_callback bus_cb = {0};
    bus_cb.pfn_isp_write_ssp_data = ot_ssp_write;

    if (ckfn_isp() && ckfn_isp_register_bus_callback()) {
        ot_isp_dev isp_dev;
        for (isp_dev = 0;  isp_dev < MAX_ISP_DEV_NUM; isp_dev++) {
            call_isp_register_bus_callback(isp_dev, ISP_BUS_TYPE_SSP, &bus_cb);
        }
    } else {
        ssp_trace(SSP_DBG_ERR, "register ssp_write_callback to isp failed, ssp init is failed!\n");
        return -1;
    }

    g_reg_ssp_base_va = ioremap_nocache((unsigned long)SSP_BASE, (unsigned long)(SSP_SIZE * SSP_DEV_NUM));
    if (!g_reg_ssp_base_va) {
        ssp_trace(SSP_DBG_ERR, "Kernel: ioremap ssp base failed!\n");
        return -ENOMEM;
    }

    ret = misc_register(&g_ssp_dev);
    if (ret != 0) {
        ssp_trace(SSP_DBG_ERR, "Kernel: register ssp_0 device failed!\n");
        return -1;
    }

    for (ssp_no = 0; ssp_no < SSP_DEV_NUM; ssp_no++) {
        ot_ssp_init_cfg(ssp_no);
    }

    ssp_spin_lock_init();
    ssp_trace(SSP_DBG_INFO, "Kernel: ssp initial ok!\n");

    return 0;
}

static void __exit ot_ssp_exit(void)
{
    unsigned int ssp_no;
    for (ssp_no = 0; ssp_no < SSP_DEV_NUM; ssp_no++) {
        ot_ssp_disable(ssp_no);
    }

    iounmap((void *)g_reg_ssp_base_va);
    misc_deregister(&g_ssp_dev);
}

module_init(ot_ssp_init);
module_exit(ot_ssp_exit);
MODULE_DESCRIPTION("ssp driver");
MODULE_LICENSE("GPL");

#endif
