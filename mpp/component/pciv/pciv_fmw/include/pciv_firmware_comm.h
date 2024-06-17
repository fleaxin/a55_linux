/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PCIV_FIRMWARE_COMM_H
#define PCIV_FIRMWARE_COMM_H

#include "ot_type.h"
#include "ot_common.h"
#include "securec.h"

#include "region_ext.h"
#include "vpss_ext.h"
#include "vb_ext.h"
#include "vgs_ext.h"
#include "pciv_fmwext.h"

td_s32 pciv_check_input_frame_size(ot_pciv_chn pciv_chn, const ot_video_frame *video_frame);
td_void pciv_fmw_src_pic_release(ot_pciv_chn pciv_chn);
td_void pciv_firmware_chn_base_init(ot_pciv_chn pciv_chn);
td_s32 pciv_firmware_check_attr(const ot_pciv_attr *attr);
td_s32 pciv_firmware_set_fmw_chn(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id);
td_s32 pciv_firmware_put_pic_to_queue(ot_pciv_chn pciv_chn, const ot_video_frame_info *video_frm_info, td_u32 index,
    td_bool is_block);
td_s32 pciv_firmware_send_data(ot_pciv_chn pciv_chn, const pciv_pic *recv_pic, ot_video_frame_info *video_frm_info);
td_void pciv_firmware_recv_set_video_frame(ot_pciv_chn pciv_chn, ot_video_frame_info *video_frm_info,
    const pciv_pic *recv_pic, vb_blk_handle vb_handle);
td_bool pciv_firmware_is_vb_can_release(ot_pciv_chn pciv_chn, td_s32 index);
td_void pciv_fmw_set_src_pic(pciv_pic *src_pic, const pciv_bind_obj *bind_obj,
    const ot_video_frame_info *video_frame);
td_s32 pciv_fmw_get_vb_calc_cfg(ot_pciv_chn pciv_chn, ot_vb_calc_cfg *vb_calc_cfg);
td_void pciv_fmw_config_out_frame(ot_pciv_chn pciv_chn,
    const vb_blk_handle vb_handle, const ot_vb_calc_cfg *calc_cfg, ot_video_frame *out_frame);
td_s32 pciv_fmw_get_vgs_out_vb(ot_pciv_chn pciv_chn, vb_blk_handle *vb_handle);
td_s32 pciv_fmw_config_vgs_opt(const rgn_info *info, vgs_online_opt *vgs_opt);

td_s32 pciv_fmw_state_check(ot_pciv_chn pciv_chn);
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
td_void pciv_fmw_vpss_query_set_video_frame(ot_pciv_chn pciv_chn, vb_blk_handle vb_handle,
    const ot_vb_calc_cfg *vb_config, const vpss_query_info *query_info, ot_video_frame *video_frame);
td_s32 pciv_fmw_vpss_query_mod_check(ot_pciv_chn pciv_chn, const vpss_query_info *query_info);
#endif
td_s32 pciv_fmw_vpss_send_state_check(ot_pciv_chn pciv_chn, const vpss_send_info *send_info);
td_void pciv_fmw_vpss_send_get_bind_obj(const vpss_send_info *send_info, pciv_bind_obj *bind_obj);

td_s32 pciv_fmw_vb_user_add(const ot_video_frame_info *img_info);
td_void pciv_fmw_vb_user_sub(const ot_video_frame_info *img_info);

td_s32 pciv_fmw_init_vpss(td_void);
td_void pciv_fmw_exit_vpss(td_void);

td_s32 pciv_fmw_get_rgn(ot_pciv_chn pciv_chn, ot_rgn_type type, rgn_info *info);
td_void pciv_fmw_put_rgn(ot_pciv_chn pciv_chn, ot_rgn_type type);
td_s32 pciv_fmw_init_rgn(td_void);
td_void pciv_fmw_exit_rgn(td_void);

#endif