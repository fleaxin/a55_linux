/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SPI_DEV_H
#define SPI_DEV_H

#include "ot_type.h"
#include "ot_osal.h"

#ifdef __LITEOS__
#include <spi.h>
#include "fcntl.h"
#else
#include <linux/spi/spi.h>
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#ifndef __LITEOS__
td_s32 motionsersor_spi_write(struct spi_device *ot_spi, td_u8 addr, const td_u8 *data, td_u32 cnt);
td_s32 motionsersor_spi_read(struct spi_device *ot_spi, td_u8 addr, td_u8 *data, td_u32 cnt);
td_s32 motionsersor_spi_init(struct spi_device **ot_spi);
td_s32 motionsersor_spi_deinit(const struct spi_device *spi_device);
#else
td_s32 motionsersor_spi_write(td_u8 addr, const td_u8 *data, td_u32 cnt, td_u32 spi_num);
td_s32 motionsersor_spi_read(td_u8 addr, td_u8 *data, td_u32 cnt, td_u32 spi_num);
td_s32 motionsersor_spi_init(td_void);
td_s32 motionsersor_spi_deinit(td_void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
