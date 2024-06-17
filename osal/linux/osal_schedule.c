/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

void osal_yield(void)
{
    cond_resched();
}
EXPORT_SYMBOL(osal_yield);
