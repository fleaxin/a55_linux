/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_osal.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/semaphore.h>
#include <linux/slab.h>

int osal_sema_init(osal_semaphore_t *sem, int val)
{
    struct semaphore *p = NULL;

    if (sem == NULL) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    if (p == NULL) {
        osal_trace("%s - kmalloc error!\n", __FUNCTION__);
        return -1;
    }
    sema_init(p, val);
    sem->sem = p;
    return 0;
}
EXPORT_SYMBOL(osal_sema_init);
int osal_down(osal_semaphore_t *sem)
{
    struct semaphore *p = NULL;

    if ((sem == NULL) || (sem->sem == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct semaphore *)(sem->sem);
    down(p);
    return 0;
}
EXPORT_SYMBOL(osal_down);
int osal_down_interruptible(osal_semaphore_t *sem)
{
    struct semaphore *p = NULL;

    if ((sem == NULL) || (sem->sem == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct semaphore *)(sem->sem);
    return down_interruptible(p);
}
EXPORT_SYMBOL(osal_down_interruptible);
int osal_down_trylock(osal_semaphore_t *sem)
{
    struct semaphore *p = NULL;

    if ((sem == NULL) || (sem->sem == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }
    p = (struct semaphore *)(sem->sem);
    return down_trylock(p);
}
EXPORT_SYMBOL(osal_down_trylock);
void osal_up(osal_semaphore_t *sem)
{
    struct semaphore *p = NULL;

    if ((sem == NULL) || (sem->sem == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    p = (struct semaphore *)(sem->sem);
    up(p);
}
EXPORT_SYMBOL(osal_up);
void osal_sema_destroy(osal_semaphore_t *sem)
{
    struct semaphore *p = NULL;

    if ((sem == NULL) || (sem->sem == NULL)) {
        osal_trace("%s - parameter invalid!\n", __FUNCTION__);
        return;
    }
    p = (struct semaphore *)(sem->sem);
    kfree(p);
    p = NULL;
    sem->sem = NULL;
}
EXPORT_SYMBOL(osal_sema_destroy);
