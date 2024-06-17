/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef PCIV_PIC_QUEUE_H
#define PCIV_PIC_QUEUE_H

#include <linux/list.h>
#include "ot_common_pciv.h"
#include "ot_common_video.h"

typedef struct {
    ot_video_frame_info frame_info;
    td_bool             is_block;
    td_u32              index;
} pciv_pic_info;

/* the node in pci queue */
typedef struct {
    pciv_pic_info       pciv_pic;      /* info of image */
    struct list_head    list;
} pciv_pic_node;

/* PCIV queue info */
typedef struct {
    pciv_pic_node       *node_buf;    /* base address of node */

    struct list_head    free_list;
    struct list_head    busy_list;

    td_u32              free_num;
    td_u32              busy_num;
    td_u32              max;
} pciv_pic_queue;

td_s32   pciv_creat_pic_queue(pciv_pic_queue *pic_queue, td_u32 max_num);
td_void  pciv_destroy_pic_queue(pciv_pic_queue *node_queue);
td_void  pciv_pic_queue_put_busy_head(pciv_pic_queue *node_queue, pciv_pic_node *pic_node);
td_void  pciv_pic_queue_put_busy(pciv_pic_queue *node_queue, pciv_pic_node *pic_node);
pciv_pic_node *pciv_pic_queue_get_busy(pciv_pic_queue *node_queue);
pciv_pic_node *pciv_pic_queue_get_free(pciv_pic_queue *node_queue);
td_void  pciv_pic_queue_put_free(pciv_pic_queue *node_queue, pciv_pic_node *pic_node);
td_u32   pciv_pic_queue_get_busy_num(const pciv_pic_queue *node_queue);

#endif
