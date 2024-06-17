/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <asm/cacheflush.h>
#include <linux/dma-direction.h>
#include "osal_mmz.h"

void osal_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long length)
{
    ot_mmb_flush_dcache_byaddr(kvirt, phys_addr, length);
}
EXPORT_SYMBOL(osal_flush_dcache_area);

