/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef PCIV_H
#define PCIV_H

#include <linux/list.h>
#include <linux/fs.h>

#include "ot_osal.h"
#include "ot_defines.h"
#include "ot_common_pciv.h"
#include "pciv_fmwext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* status of share buf */
typedef enum {
    BUFF_FREE = 0,
    BUFF_BUSY = 1,
    BUFF_HOLD = 2,
    BUFF_BUTT
} ot_pciv_buf_status;

typedef struct {
    volatile td_bool    is_create;
    volatile td_bool    is_start;
    volatile td_bool    is_config;         /* the flag of config or not */
    volatile td_bool    is_hide;           /* the flag of hide or not */

    volatile td_u32     get_cnt;        /* record the times of reading recycle_cb or pic_cb */
    volatile td_u32     send_cnt;       /* record the times of writing recycle_cb or pic_cb */
    volatile td_u32     resp_cnt;       /* record the times of finishing response interrupt */
    volatile td_u32     lost_cnt;       /* record the times of dropping the image for no idle buffer */
    volatile td_u32     notify_cnt;     /* the times of notify(receive the message of read_done or write_done) */

    ot_pciv_attr        pciv_attr;   /* record the dest image info and the opposite end dev info */
    volatile ot_pciv_buf_status buf_status[OT_PCIV_MAX_BUF_NUM];  /* used by sender. */
    volatile td_u32     buf_use_cnt[OT_PCIV_MAX_BUF_NUM];               /* used by sender. */
    volatile td_bool    can_recv;       /* the flag of memory info synchronous, used in the receiver end */

    struct semaphore pciv_mutex;
} pciv_chn_ctx;

td_s32 pciv_create(ot_pciv_chn chn, const ot_pciv_attr *attr);
td_s32 pciv_destroy(ot_pciv_chn chn);
td_s32 pciv_set_attr(ot_pciv_chn chn, const ot_pciv_attr *attr);
td_s32 pciv_get_attr(ot_pciv_chn chn, ot_pciv_attr *attr);
td_s32 pciv_start(ot_pciv_chn chn);
td_s32 pciv_stop(ot_pciv_chn chn);
td_s32 pciv_hide(ot_pciv_chn chn, td_bool hide);
td_s32 pciv_window_vb_create(const ot_pciv_window_vb_cfg *cfg);
td_s32 pciv_window_vb_destroy(td_void);
td_s32 pciv_malloc(td_u32 size, td_phys_addr_t *phys_addr);
td_s32 pciv_free(td_phys_addr_t phys_addr);
td_s32 pciv_malloc_chn_buffer(ot_pciv_chn chn, td_u32 index, td_u32 size, td_phys_addr_t *phys_addr);
td_s32 pciv_free_chn_buffer(ot_pciv_chn chn, const td_u32 index);
td_s32 pciv_user_dma_task(const ot_pciv_dma_task *task);
td_s32 pciv_free_share_buf(ot_pciv_chn chn, td_u32 index, td_u32 count);
td_s32 pciv_free_all_buf(ot_pciv_chn chn);
td_s32 pciv_src_pic_send(ot_pciv_chn chn, const pciv_pic *src_pic);
td_s32 pciv_src_pic_free(ot_pciv_chn chn, const pciv_pic *src_pic);
td_s32 pciv_receive_pic(ot_pciv_chn chn, pciv_pic *recv_pic);
td_s32 pciv_recv_pic_free(ot_pciv_chn chn, pciv_pic *recv_pic);
pciv_chn_ctx *pciv_get_context(ot_pciv_chn pciv_chn);
td_s32 pciv_init(void);
td_void pciv_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

