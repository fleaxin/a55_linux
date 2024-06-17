/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef DRV_OSAL_INIT_H
#define DRV_OSAL_INIT_H

#include <asm/io.h>
#include "ot_osal.h"

#define KLAD_MUTEX_T                         osal_mutex_t
#define klad_mutex_init(x)                   osal_mutex_init(x)
#define klad_mutex_lock(x)                   osal_mutex_lock(x)
#define klad_mutex_unlock(x)                 osal_mutex_unlock(x)
#define klad_mutex_destroy(x)                osal_mutex_destroy(x)

#define klad_ioremap_nocache(addr, size)     osal_ioremap_nocache(addr, size)
#define klad_iounmap(addr, size)             osal_iounmap(addr)

#define klad_malloc(x)                       osal_vmalloc(x)
#define klad_free(x)                         osal_vfree(x)

#define klad_write(addr, data)               writel(data, addr)
#define klad_read(addr)                      readl(addr)

#define klad_udelay(x)                       osal_udelay(x)

#define KLAD_QUEUE_HEAD                      osal_wait_t
#define klad_queue_init(x)                   osal_wait_init(x)
#define klad_queue_wait_up(x)                osal_wakeup(x)
#define klad_queue_destroy(x)                osal_wait_destroy(x)

#define KLAD_IRQ_HANDLED                     OSAL_IRQ_HANDLED
#define klad_queue_wait_timeout(head, func, param, time) \
    osal_wait_timeout_interruptible(head, func, param, time)

#define klad_request_irq(irq, func, name) \
    osal_request_irq(irq, func, TD_NULL, name, (td_void *)(name))

#define klad_free_irq(irq, name)          \
    osal_free_irq(irq, (td_void *)(name))

#endif /* DRV_OSAL_INIT_H */

