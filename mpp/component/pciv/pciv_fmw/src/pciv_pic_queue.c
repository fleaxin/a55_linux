/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "pciv_pic_queue.h"

#include "ot_osal.h"
#include "ot_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 pciv_creat_pic_queue(pciv_pic_queue *pic_queue, td_u32 max_num)
{
    td_u32          i;
    pciv_pic_node   *pic_node = TD_NULL;

    /* initialize the header of queue */
    INIT_LIST_HEAD(&pic_queue->free_list);
    INIT_LIST_HEAD(&pic_queue->busy_list);

    /* malloc node buffer(cached) */
    pic_queue->node_buf = osal_kmalloc(max_num * sizeof(pciv_pic_node), osal_gfp_kernel);
    if (pic_queue->node_buf == TD_NULL) {
        return TD_FAILURE;
    }

    /* put node buffer to free queue */
    pic_node = pic_queue->node_buf;

    for (i = 0; i < max_num; i++) {
        list_add_tail(&pic_node->list, &pic_queue->free_list);
        pic_node++;
    }
    /* initialize other element of the initialize queue */
    pic_queue->free_num = max_num;
    pic_queue->max      = max_num;
    pic_queue->busy_num = 0;

    return TD_SUCCESS;
}

td_void pciv_destroy_pic_queue(pciv_pic_queue *node_queue)
{
    /* initialize the head of queue */
    INIT_LIST_HEAD(&node_queue->free_list);
    INIT_LIST_HEAD(&node_queue->busy_list);

    node_queue->free_num    = 0;
    node_queue->busy_num    = 0;
    node_queue->max         = 0;

    if (node_queue->node_buf) {
        osal_kfree(node_queue->node_buf);
        node_queue->node_buf = TD_NULL;
    }

    return;
}

pciv_pic_node *pciv_pic_queue_get_free(pciv_pic_queue *node_queue)
{
    struct list_head    *plist = TD_NULL;
    pciv_pic_node       *pic_node = NULL;

    if (list_empty(&node_queue->free_list)) {
        return NULL;
    }

    plist = node_queue->free_list.next;
    ot_assert(plist != TD_NULL);
    list_del(plist);

    pic_node = list_entry(plist, pciv_pic_node, list);

    node_queue->free_num--;
    return pic_node;
}

td_void  pciv_pic_queue_put_busy_head(pciv_pic_queue *node_queue, pciv_pic_node  *pic_node)
{
    list_add(&pic_node->list, &node_queue->busy_list);
    node_queue->busy_num++;

    return;
}

td_void  pciv_pic_queue_put_busy(pciv_pic_queue *node_queue, pciv_pic_node  *pic_node)
{
    list_add_tail(&pic_node->list, &node_queue->busy_list);
    node_queue->busy_num++;

    return;
}

pciv_pic_node *pciv_pic_queue_get_busy(pciv_pic_queue *node_queue)
{
    struct list_head    *plist = TD_NULL;
    pciv_pic_node       *pic_node = NULL;

    if (list_empty(&node_queue->busy_list)) {
        return NULL;
    }

    plist = node_queue->busy_list.next;
    list_del(plist);

    pic_node = list_entry(plist, pciv_pic_node, list);

    node_queue->busy_num--;
    return pic_node;
}

td_void pciv_pic_queue_put_free(pciv_pic_queue *node_queue, pciv_pic_node  *pic_node)
{
    list_add_tail(&pic_node->list, &node_queue->free_list);
    node_queue->free_num++;
    return;
}

td_u32 pciv_pic_queue_get_busy_num(const pciv_pic_queue *node_queue)
{
    return node_queue->busy_num;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

