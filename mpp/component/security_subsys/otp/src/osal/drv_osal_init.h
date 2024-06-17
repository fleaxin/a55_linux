/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef DRV_OSAL_INIT_H
#define DRV_OSAL_INIT_H

#include <linux/delay.h>
#include <asm/io.h>
#include "ot_osal.h"

#define OTP_MUTEX_T                         osal_mutex_t
#define otp_mutex_init(x)                   osal_mutex_init(x)
#define otp_mutex_lock(x)                   osal_mutex_lock(x)
#define otp_mutex_unlock(x)                 osal_mutex_unlock(x)
#define otp_mutex_destroy(x)                osal_mutex_destroy(x)

#define otp_ioremap_nocache(addr, size)     osal_ioremap_nocache(addr, size)
#define otp_iounmap(addr, size)             osal_iounmap(addr)

#define otp_malloc(x)                       osal_vmalloc(x)
#define otp_free(x)                         \
    do {                                    \
        if (x) {                            \
            osal_vfree(x);                  \
            x = TD_NULL;                    \
        }                                   \
    } while (0)

#define otp_write(addr, data)               writel(data, addr)
#define otp_read(addr)                      readl(addr)

#define otp_udelay(x)                       osal_udelay(x)

#endif /* DRV_OSAL_INIT_H */
