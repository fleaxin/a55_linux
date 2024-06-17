/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_st.h"

/* isp sync task */
#define  ISP_SYNC_TSK_MAX_NODES  16
#define  ISP_SYNC_TSK_MAX_NUM    200

typedef void (*ISP_FUNC_PTR)(unsigned long data);

isp_sync_tsk_ctx g_isp_sync_tsk_ctx[OT_ISP_MAX_PIPE_NUM];
#define isp_sync_tsk_get_ctx(dev) (&g_isp_sync_tsk_ctx[dev])

td_void isp_sync_task_find_and_execute(ot_vi_pipe vi_pipe, struct osal_list_head *head)
{
    struct osal_list_head *pos = TD_NULL;
    struct osal_list_head *next = TD_NULL;
    ot_isp_sync_task_node *sync_tsk_node = TD_NULL;

    if (!osal_list_empty(head)) {
        osal_list_for_each_safe(pos, next, head) {
            sync_tsk_node = osal_list_entry(pos, ot_isp_sync_task_node, list);
            if (sync_tsk_node->focus_stat) {
                isp_drv_sta_kernel_get(vi_pipe, sync_tsk_node->focus_stat);
            }
            if (sync_tsk_node->isp_sync_tsk_callback) {
                sync_tsk_node->isp_sync_tsk_callback(sync_tsk_node->data);
            }
        }
    }

    return;
}

static void work_queue_handler(struct osal_work_struct *worker)
{
    isp_sync_tsk_ctx *sync_tsk = osal_container_of((void *)worker, isp_sync_tsk_ctx, worker);

    if (osal_down_interruptible(&sync_tsk->sem)) {
        return;
    }

    isp_sync_task_find_and_execute(sync_tsk->vi_pipe, &sync_tsk->workqueue_list.head);

    osal_up(&sync_tsk->sem);

    return;
}

td_s32 isp_sync_task_process(ot_vi_pipe vi_pipe)
{
    isp_sync_tsk_ctx *sync_tsk = isp_sync_tsk_get_ctx(vi_pipe);

    if (sync_tsk->hwirq_list.num) {
        isp_sync_task_find_and_execute(vi_pipe, &sync_tsk->hwirq_list.head);
    }

    if (sync_tsk->tsklet_list.num) {
    }

    if (sync_tsk->workqueue_list.num) {
        osal_schedule_work(&sync_tsk->worker);
    }

    return TD_SUCCESS;
}

static struct osal_list_head *search_node(struct osal_list_head *head, const char *id)
{
    struct osal_list_head *pos = TD_NULL;
    struct osal_list_head *next = TD_NULL;
    ot_isp_sync_task_node *sync_tsk_node = TD_NULL;

    osal_list_for_each_safe(pos, next, head) {
        sync_tsk_node = osal_list_entry(pos, ot_isp_sync_task_node, list);
        if (!osal_strncmp(sync_tsk_node->sz_id, id, OT_ISP_SYNC_TASK_ID_MAX_LENGTH)) {
            return pos;
        }
    }

    return TD_NULL;
}

td_s32 ot_isp_sync_task_register(ot_vi_pipe vi_pipe, ot_isp_sync_task_node *new_node)
{
    isp_sync_tsk_ctx *sync_tsk = isp_sync_tsk_get_ctx(vi_pipe);
    struct osal_list_head *target_list = TD_NULL;
    struct osal_list_head *pos = TD_NULL;
    list_entry *list_entry_tmp = TD_NULL;

    isp_check_pointer_return(new_node);

    if (new_node->method == ISP_SYNC_TSK_METHOD_HW_IRQ) {
        target_list = &sync_tsk->hwirq_list.head;
    } else {
        target_list = &sync_tsk->workqueue_list.head;
    }

    list_entry_tmp = osal_list_entry(target_list, list_entry, head);
    if (list_entry_tmp == TD_NULL) {
        return TD_FAILURE;
    }

    pos = search_node(target_list, new_node->sz_id);
    if (pos != TD_NULL) {
        return TD_FAILURE;
    }

    if (osal_down_interruptible(&sync_tsk->sem)) {
        return -ERESTARTSYS;
    }

    osal_list_add_tail(&new_node->list, target_list);

    if (list_entry_tmp->num < ISP_SYNC_TSK_MAX_NUM) {
        list_entry_tmp->num++;
    }

    osal_up(&sync_tsk->sem);

    return TD_SUCCESS;
}

td_s32 ot_isp_sync_task_unregister(ot_vi_pipe vi_pipe, ot_isp_sync_task_node *del_node)
{
    isp_sync_tsk_ctx *sync_tsk = isp_sync_tsk_get_ctx(vi_pipe);
    struct osal_list_head *target_list = TD_NULL;
    list_entry *list_entry_tmp = TD_NULL;
    struct osal_list_head *pos = TD_NULL;
    td_s32 del_success = TD_FAILURE;

    isp_check_pointer_return(del_node);

    if (del_node->method == ISP_SYNC_TSK_METHOD_HW_IRQ) {
        target_list = &sync_tsk->hwirq_list.head;
    } else {
        target_list = &sync_tsk->workqueue_list.head;
    }
    list_entry_tmp = osal_list_entry(target_list, list_entry, head);
    if (list_entry_tmp == TD_NULL) {
        return TD_FAILURE;
    }

    if (osal_down_interruptible(&sync_tsk->sem)) {
        return -ERESTARTSYS;
    }

    pos = search_node(target_list, del_node->sz_id);
    if (pos != TD_NULL) {
        osal_list_del(pos);
        if (list_entry_tmp->num > 0) {
            list_entry_tmp->num = list_entry_tmp->num - 1;
        }

        del_success = TD_SUCCESS;
    }

    osal_up(&sync_tsk->sem);

    return del_success;
}

void sync_task_init(ot_vi_pipe vi_pipe)
{
    isp_sync_tsk_ctx *sync_tsk = isp_sync_tsk_get_ctx(vi_pipe);

    OSAL_INIT_LIST_HEAD(&sync_tsk->hwirq_list.head);
    OSAL_INIT_LIST_HEAD(&sync_tsk->tsklet_list.head);
    OSAL_INIT_LIST_HEAD(&sync_tsk->workqueue_list.head);

    sync_tsk->hwirq_list.num     = 0;
    sync_tsk->tsklet_list.num    = 0;
    sync_tsk->workqueue_list.num = 0;
    osal_sema_init(&sync_tsk->sem, 1);

    OSAL_INIT_WORK(&sync_tsk->worker, work_queue_handler);

    return;
}

void sync_task_exit(ot_vi_pipe vi_pipe)
{
    isp_sync_tsk_ctx *sync_tsk = isp_sync_tsk_get_ctx(vi_pipe);

    osal_destroy_work(&sync_tsk->worker);
    osal_sema_destroy(&sync_tsk->sem);

    return;
}
