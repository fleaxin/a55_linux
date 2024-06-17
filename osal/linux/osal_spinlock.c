/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

int osal_spin_lock_init(osal_spinlock_t *lock)
{
    spinlock_t *p = NULL;

    if (lock == NULL) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (spinlock_t *)kmalloc(sizeof(spinlock_t), GFP_KERNEL);
    if (p == NULL) {
        osal_trace("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    spin_lock_init(p);
    lock->lock = p;
    return 0;
}
EXPORT_SYMBOL(osal_spin_lock_init);
void osal_spin_lock(osal_spinlock_t *lock)
{
    spinlock_t *p = NULL;

    if ((lock == NULL) || (lock->lock == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (spinlock_t *)(lock->lock);
    spin_lock(p);
}
EXPORT_SYMBOL(osal_spin_lock);

void osal_spin_unlock(osal_spinlock_t *lock)
{
    spinlock_t *p = NULL;

    if ((lock == NULL) || (lock->lock == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (spinlock_t *)(lock->lock);
    spin_unlock(p);
}
EXPORT_SYMBOL(osal_spin_unlock);
void osal_spin_lock_irqsave(osal_spinlock_t *lock, unsigned long *flags)
{
    spinlock_t *p = NULL;
    unsigned long f;

    if ((lock == NULL) || (lock->lock == NULL) || (flags == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (spinlock_t *)(lock->lock);
    spin_lock_irqsave(p, f);
    *flags = f;
}
EXPORT_SYMBOL(osal_spin_lock_irqsave);
void osal_spin_unlock_irqrestore(osal_spinlock_t *lock, const unsigned long *flags)
{
    spinlock_t *p = NULL;
    unsigned long f;

    if ((lock == NULL) || (lock->lock == NULL) || (flags == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (spinlock_t *)(lock->lock);
    f = *flags;
    spin_unlock_irqrestore(p, f);
}
EXPORT_SYMBOL(osal_spin_unlock_irqrestore);
void osal_spin_lock_destroy(osal_spinlock_t *lock)
{
    spinlock_t *p = NULL;

    if ((lock == NULL) || (lock->lock == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }

    p = (spinlock_t *)(lock->lock);
    kfree(p);
    lock->lock = NULL;
}
EXPORT_SYMBOL(osal_spin_lock_destroy);
