/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/mutex.h>
#include <linux/slab.h>

int osal_mutex_init(osal_mutex_t *mutex)
{
    struct mutex *p = NULL;

    if (mutex == NULL) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = kmalloc(sizeof(struct mutex), GFP_KERNEL);
    if (p == NULL) {
        osal_trace("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    mutex_init(p);
    mutex->mutex = p;
    return 0;
}
EXPORT_SYMBOL(osal_mutex_init);

int osal_mutex_lock(osal_mutex_t *mutex)
{
    struct mutex *p = NULL;

    if ((mutex == NULL) || (mutex->mutex == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct mutex *)(mutex->mutex);
    mutex_lock(p);
    return 0;
}
EXPORT_SYMBOL(osal_mutex_lock);

int osal_mutex_lock_interruptible(osal_mutex_t *mutex)
{
    struct mutex *p = NULL;

    if ((mutex == NULL) || (mutex->mutex == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct mutex *)(mutex->mutex);
    return mutex_lock_interruptible(p);
}
EXPORT_SYMBOL(osal_mutex_lock_interruptible);

void osal_mutex_unlock(osal_mutex_t *mutex)
{
    struct mutex *p = NULL;

    if ((mutex == NULL) || (mutex->mutex == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (struct mutex *)(mutex->mutex);

    mutex_unlock(p);
}
EXPORT_SYMBOL(osal_mutex_unlock);

void osal_mutex_destroy(osal_mutex_t *mutex)
{
    struct mutex *p = NULL;

    if ((mutex == NULL) || (mutex->mutex == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (struct mutex *)(mutex->mutex);
    kfree(p);
    p = NULL;
    mutex->mutex = NULL;
}
EXPORT_SYMBOL(osal_mutex_destroy);
