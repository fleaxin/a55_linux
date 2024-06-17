/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef PCIV_DRVADP_H
#define PCIV_DRVADP_H

#include <linux/list.h>
#include <linux/fs.h>

#include "ot_defines.h"
#include "ot_common_pciv.h"
#include "pciv_fmwext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PCIV_DMA_MAX_PRV_DATA 5

typedef struct tag_pciv_send_task {
    struct list_head list;

    td_bool         read;
    td_phys_addr_t  src_phys_addr;
    td_phys_addr_t  dst_phys_addr;
    td_u32          len;
    td_u64          prv_data[PCIV_DMA_MAX_PRV_DATA];
    td_void         *pv_prv_data;
    td_void         (*call_back)(struct tag_pciv_send_task *task);
} pciv_send_task;

typedef struct {
    ot_pciv_chn  pciv_chn;       /* the PCI device to be notified */
    pciv_pic     pic_info;
} pciv_notify_pic_end;

typedef struct {
    td_u64 rdone_time;
    td_u32 rdone_gap;
    td_u32 max_rdone_gap;
    td_u32 min_rdone_gap;

    td_u64 wdone_time;
    td_u32 wdone_gap;
    td_u32 max_wdone_gap;
    td_u32 min_wdone_gap;
}pciv_gap_record;

typedef enum {
    PCIV_MSGTYPE_CREATE,
    PCIV_MSGTYPE_START,
    PCIV_MSGTYPE_DESTROY,
    PCIV_MSGTYPE_SETATTR,
    PCIV_MSGTYPE_GETATTR,
    PCIV_MSGTYPE_CMDECHO,
    PCIV_MSGTYPE_WRITEDONE,
    PCIV_MSGTYPE_READDONE,
    PCIV_MSGTYPE_NOTIFY,
    PCIV_MSGTYPE_MALLOC,
    PCIV_MSGTYPE_FREE,
    PCIV_MSGTYPE_BIND,
    PCIV_MSGTYPE_UNBIND,
    PCIV_MSGTYPE_BUTT
} pciv_msg_type;

#define PCIV_MSG_HEAD_LEN (16)
#define PCIV_MSG_MAX_LEN  (384 - PCIV_MSG_HEAD_LEN)

typedef struct {
    td_s32          target; /* the final user of this message */
    pciv_msg_type   msg_type;
    td_u32          dev_type;
    td_u32          msg_len;
    td_u8           c_msg_body[PCIV_MSG_MAX_LEN];
} pciv_msg;

td_s32 pciv_drv_adp_add_dma_task(pciv_send_task *task);
td_void pciv_drv_adp_start_dma_task(td_void);
td_s32 pciv_drv_adp_set_gap_record(ot_pciv_chn pciv_chn, const pciv_gap_record *gap);
td_s32 pciv_drv_adp_get_gap_record(ot_pciv_chn pciv_chn, pciv_gap_record *gap);
td_s32 pciv_drv_adp_dma_end_notify(const ot_pciv_remote_obj *remote_obj, const pciv_pic *recv_pic);
td_s32 pciv_drv_adp_buf_free_notify(const ot_pciv_remote_obj *remote_obj, const pciv_pic *recv_pic);
td_s32 pciv_drv_adp_addr_check(const ot_pciv_dma_blk *block, td_bool is_read);
td_s32 pciv_drv_adp_send_msg(const ot_pciv_remote_obj *remote_obj, pciv_msg_type type, const pciv_pic *recv_pic);
td_s32 pciv_drv_adp_get_window_base(ot_pciv_window_base *win_base);
td_s32 pciv_drv_adp_get_local_id(td_void);
td_s32 pciv_drv_adp_enum_chip(ot_pciv_enum_chip *chips);
td_s32 pciv_drv_adp_init(td_void);
td_void pciv_drv_adp_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

