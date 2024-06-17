/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/barrier.h>

void osal_isb(void)
{
    isb();
}
EXPORT_SYMBOL(osal_isb);
void osal_dsb(void)
{
#ifdef CONFIG_64BIT
    dsb(sy);
#else
    dsb();
#endif
}
EXPORT_SYMBOL(osal_dsb);
void osal_dmb(void)
{
#ifdef CONFIG_64BIT
    dmb(sy);
#else
    dmb();
#endif
}
EXPORT_SYMBOL(osal_dmb);
