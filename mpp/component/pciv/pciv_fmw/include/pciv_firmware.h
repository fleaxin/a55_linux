/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PCIV_FIRMWARE_H
#define PCIV_FIRMWARE_H

#include <linux/sched.h>
#include <linux/delay.h>

#include "ot_common_pciv.h"
#include "pciv_fmwext.h"
#include "pciv_pic_queue.h"
#include "vb_ext.h"
#include "vgs_ext.h"
#include "vpss_ext.h"

typedef struct {
    struct list_head list;
    ot_pciv_chn      pciv_chn;
} pciv_portmap_node;

typedef struct {
    td_u32 pool_cnt;
    td_u32 pool_id[OT_PCIV_MAX_VB_CNT];
    td_u32 size[OT_PCIV_MAX_VB_CNT];
} pciv_vb_pool;

typedef enum {
    PCIVFMW_SEND_OK = 0,
    PCIVFMW_SEND_NOK,
    PCIVFMW_SEND_ING,
    PCIVFMW_SEND_BUTT
} pciv_fmw_send_state;

typedef struct {
    td_bool is_create;
    td_bool is_start;
    td_bool is_master; /* the flag of master or not */

    td_u32  rgn_num;   /* number of channel region */
    td_u32  time_ref;  /* the serial number of VI source image */
    td_u32  get_cnt;   /* the times of sender get VI image,
                          or the receiver get the image for VO's display */
    td_u32  send_cnt;  /* the times of sender send the image,
                          or the receiver send to VO displaying */
    td_u32  resp_cnt;  /* the times of sender finish sending the image and releasing,
                          or the receiver finish sending to VO displaying */
    td_u32  lost_cnt;  /* the times of sender fail sending the image,
                          or the receiver fail sending to VO displaying */
    td_u32  timer_cnt; /* the times of the timer running to send VDEC image */

    td_u32  add_job_suc_cnt;     /* success submitting the job */
    td_u32  add_job_fail_cnt;    /* fail submitting the job */

    td_u32  trans_task_suc_cnt;  /* trans task success */
    td_u32  trans_task_fail_cnt; /* trans task fail */

    td_u32  osd_task_suc_cnt;    /* osd task success */
    td_u32  osd_task_fail_cnt;   /* osd task  fail */

    td_u32  end_job_suc_cnt;     /* vgs end job success */
    td_u32  end_job_fail_cnt;    /* vgs end job fail */

    td_u32  trans_cb_cnt;        /* vgs trans callback success */
    td_u32  osd_cb_cnt;          /* vgs osd callback success */

    td_u32  new_do_cnt;
    td_u32  old_undo_cnt;

    pciv_fmw_send_state send_state;

    pciv_pic_queue  pic_queue;      /* vdec image queue */
    pciv_pic_node   *cur_vdec_node; /* the current vdec node */

    /* record the tartget image attr after zoom */
    ot_pciv_pic_attr    pic_attr; /* record the target image attr of PCI transmit */
    td_u32              blk_size;
    td_u32              count;   /* the total buffer count */
    td_phys_addr_t      phys_addr[OT_PCIV_MAX_BUF_NUM];
    td_u32              pool_id[OT_PCIV_MAX_BUF_NUM];
    vb_blk_handle       vb_blk_hdl[OT_PCIV_MAX_BUF_NUM];   /* vb handle,used to check the VB is release by VO or not */
    td_bool             is_pciv_hold[OT_PCIV_MAX_BUF_NUM]; /* buffer is been hold by the pciv queue or not */

    struct timer_list   buf_timer;
} pciv_fmw_chn;

#define PCIV_DEV_ID 0

pciv_fmw_chn *pciv_fmw_get_context(ot_pciv_chn pciv_chn);
td_s32 pciv_fmw_reset_call_back(td_s32 dev_id, td_s32 chn_id, td_void *pv_data);
td_s32 pciv_fmw_vpss_send(td_s32 dev_id, td_s32 chn_id, vpss_send_info *send_info);
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
td_s32 pciv_fmw_vpss_query(td_s32 dev_id, td_s32 chn_id, vpss_query_info *query_info, vpss_inst_info *inst_info);
#endif
#endif
