/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SENSOR_SPI_H
#define SENSOR_SPI_H

#define C_SENOSOR_NUM 64
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/delay.h>
#ifdef __LITEOS__
#include <spi.h>
#include "fcntl.h"
#else
#include <linux/spi/spi.h>
#endif
#include <asm/uaccess.h>
#include "isp_ext.h"

typedef struct {
    td_u32 bus_num;
    td_u32 u32csn;
    td_char c_sensor[C_SENOSOR_NUM];
} spi_module_params;

extern struct bus_type spi_bus_type;

#ifdef __LITEOS__
extern int spi_dev_set(int host_no, int cs_no, ot_spi_ioc_transfer *transfer);   /* ot_spidev_set */
#endif

int ssp_write_alt(unsigned int spi_bus, unsigned int spi_csn,
                  unsigned int addr1, unsigned int addr1bytenum,
                  unsigned int addr2, unsigned int addr2bytenum,
                  unsigned int data, unsigned int databytenum);

int ssp_read_alt(unsigned int spi_bus, unsigned int spi_csn,
                 unsigned char devaddr, unsigned char addr, unsigned char *data);
#endif
