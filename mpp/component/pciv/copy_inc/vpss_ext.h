/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef VPSS_EXT_H
#define VPSS_EXT_H

#include "ot_type.h"
#include "ot_common_video.h"
#include "ot_common_sys.h"
#include "ot_common_vo.h"
#include "ot_common_vpss.h"

#define VPSS_FILED_NUM              2
#define VPSS_DOUBLE_FRAME_NUM       2

#define VPSS_BUFFER_SHARE_MAGIC_NUM 0x4f46
#define VPSS_BUFFER_SHARE_MAGIC_OFFSET 48
#define VPSS_U32_MASK 0xFFFFFFFF
#define VPSS_OFFSET_MASK 0xFFFFFFFFFFFF

#define VPSS_COMPRESS_MASK_NONE 0x1
#define VPSS_COMPRESS_MASK_SEG 0x2
#define VPSS_COMPRESS_MASK_COMPACT 0x4
#define VPSS_COMPRESS_MASK_TILE 0x8
#define VPSS_COMPRESS_MASK_LINE 0x10
#define VPSS_COMPRESS_MASK_FRAME 0x20

#define VPSS_MAX_VGS_NODE_NUM 12

#define VPSS_MAX_MCF_GRP_NUM  4

#define VPSS_MCF_PATH_MAGIC_NUM_FUSION 0x9527
#define VPSS_MCF_PATH_MAGIC_NUM_COLOR 0x9528
#define VPSS_MCF_PATH_MAGIC_NUM_MONO 0x9529

typedef struct {
    ot_video_frame_info video_frame;
    ot_mod_id mod_id;
    td_bool block_mode; /* flashed video frame or not. */
    td_u64 start_time;
    td_u64 node_index; /* node index */
    td_u64 pts[OT_VPSS_MAX_PHYS_CHN_NUM];
    td_bool pre_backup; /* whether pre backup */
    td_bool set_backup; /* whether set backup */
    td_bool vo_pause; /* query info : whether pause */
    td_bool vo_step; /* query info : whether step */
#ifdef CONFIG_OT_VPSS_QUICK_SCHEDULE_SUPPORT
    td_bool put_vb; /* notify VGS whether VPSS released VB */
#endif
} vpss_pic_info;

typedef struct {
    vpss_pic_info *src_pic_info; /* information of source pic */
    vpss_pic_info *old_pic_info; /* information of backup pic */
    td_bool scale_cap; /* whether scaling */
    td_bool trans_cap; /* whether the frame rate is doubled */
    td_bool malloc_buffer; /* whether malloc frame buffer */
    td_u32 src_compress_ability; /* source pic compress_mode ability mask */
    td_u32 old_compress_ability; /* old pic compress_mode ability mask */
    ot_schedule_mode schedule_mode; /* current vpss schedule mode */
} vpss_query_info;

typedef struct {
    td_bool new_frame; /* whether use new pic to query */
    td_bool vpss_proc; /* whether vpss need to process */
    td_bool double_frame; /* whether the frame rate is doubled */
    td_bool update_backup; /* whether update backup pic */
    ot_compress_mode compress_mode; /* compress mode */
    vpss_pic_info dest_pic_info[VPSS_DOUBLE_FRAME_NUM]; /* double frame */
    ot_aspect_ratio aspect_ratio; /* aspect ratio configuration */
    ot_vo_partition_mode partition_mode;
    td_bool vo_pause; /* query info : whether pause */
    td_bool vo_step; /* query info : whether step */
} vpss_inst_info;

typedef struct {
    td_bool suc; /* whether successful completion */
    ot_vpss_grp grp;
    ot_vpss_chn chn;
    vpss_pic_info *dest_pic_info[VPSS_FILED_NUM]; /* pic processed by vpss.0:top field 1:bottom field */
    td_u64 node_index;
} vpss_send_info;

typedef struct {
    ot_mod_id mod_id;
    td_s32(*vpss_query)(td_s32 dev_id, td_s32 chn_id, vpss_query_info *query_info,
           vpss_inst_info *inst_info);
    td_s32 (*vpss_send)(td_s32 dev_id, td_s32 chn_id, vpss_send_info *send_info);
    td_s32 (*reset_call_back)(td_s32 dev_id, td_s32 chn_id, td_void *pv_data);
} vpss_register_info;

typedef struct {
    td_bool vpss_en[OT_VPSS_IP_NUM];
} vpss_ip_enable;

typedef struct {
    td_phys_addr_t tunl_phys_addr;
    td_phys_addr_t rnt0_phys_addr;
    td_phys_addr_t rnt1_phys_addr;
    td_u32 width;
    td_u32 height;
    td_u32 stride;
} vpss_3dnr_rnt_info;

typedef struct {
    td_u32 vgs_id;                                   /* vgs ip id */
    td_u32 job_id;                                   /* vgs job id */
    td_bool is_order_job;                            /* sort job or not */
    td_bool is_job_finish;                           /* job has been submitted all or not */
    td_phys_addr_t phys_addr[VPSS_MAX_VGS_NODE_NUM]; /* physical addr of vgs node */
    td_void *node[VPSS_MAX_VGS_NODE_NUM];            /* pointer of node */
    td_u32 node_num;                                 /* node num of one task */
    td_u32 src_width;                                /* source image width */
    td_u32 src_height;                               /* source image height */
} vpss_vgs_node_info;

typedef struct {
    ot_vpss_grp grp_id[VPSS_MAX_MCF_GRP_NUM];
    td_bool is_big_image_path[VPSS_MAX_MCF_GRP_NUM];
    td_bool is_color_pipe[VPSS_MAX_MCF_GRP_NUM];
} vpss_mcf_grp_info;

typedef struct {
    td_u32 frame_num;
    td_u32 frame_flag;
} vpss_snap_info;

typedef struct {
    td_s32 (*pfn_vpss_register)(const vpss_register_info *info);
    td_s32 (*pfn_vpss_un_register)(ot_mod_id mod_id);
    td_void (*pfn_vpss_update_vi_vpss_mode)(const ot_vi_vpss_mode *vi_vpss_mode);
    td_void (*pfn_vpss_update_vi_video_mode)(ot_vi_video_mode vi_video_mode);
    td_s32 (*pfn_vpss_vi_submit_task)(ot_vpss_grp grp, const ot_video_frame_info *vi_frame, td_bool lost_frame,
           ot_pixel_format pixel_format);
    td_s32 (*pfn_vpss_vi_start_task)(td_u32 vpss_id);
    td_s32 (*pfn_vpss_vi_task_done)(td_s32 vpss_id, td_s32 state);
    td_s32 (*pfn_vpss_down_semaphore)(ot_vpss_grp grp);
    td_s32 (*pfn_vpss_up_semaphore)(ot_vpss_grp grp);
    td_s32 (*pfn_vpss_vi_early_irq_proc)(ot_vpss_grp grp);
    td_bool (*pfn_vpss_is_grp_existed)(ot_vpss_grp grp);
    td_s32 (*pfn_vpss_query_buffer_share_info)(ot_vpss_grp grp, ot_video_frame *video_frame);
    td_s32 (*pfn_vpss_set_3dnr_rnt_info)(ot_vpss_grp grp, const vpss_3dnr_rnt_info *rnt_info);
    td_s32 (*pfn_vpss_start_vgs_task)(const vpss_vgs_node_info *node_info);
#ifdef CONFIG_OT_VPSS_MCF_SUPPORT
    td_s32 (*pfn_vpss_receive_mcf_frame)(ot_vpss_grp grp, ot_video_frame_info *video_frame);
    td_s32 (*pfn_vpss_get_chn_attr)(ot_vpss_grp grp, ot_vpss_chn chn, ot_vpss_chn_attr *chn_attr);
    td_s32 (*pfn_vpss_set_chn_attr)(ot_vpss_grp grp, ot_vpss_chn chn, const ot_vpss_chn_attr *chn_attr);
    td_s32 (*pfn_vpss_enable_chn)(ot_vpss_grp grp, ot_vpss_chn chn);
    td_s32 (*pfn_vpss_disable_chn)(ot_vpss_grp grp, ot_vpss_chn chn);
    td_s32 (*pfn_vpss_set_mcf_grp_info)(const vpss_mcf_grp_info *mcf_grp_info);
    td_s32 (*pfn_vpss_get_grp_attr)(ot_vpss_grp grp, ot_vpss_grp_attr *vpss_grp_attr);
#endif
    td_void (*pfn_vpss_vo_notify_vpss)(td_void);
    td_void (*pfn_vpss_update_schedule_mode)(const ot_schedule_mode *mode);

#ifdef CONFIG_OT_SNAP_SUPPORT
    td_void (*pfn_vpss_online_update_snap_frame_flag)(ot_vpss_grp grp, const vpss_snap_info *snap_info);
#endif
    td_s32 (*pfn_vpss_calc_ratio_coverex)(ot_vpss_grp grp, ot_vpss_chn chn, td_bool rgn_modify,
        const ot_rect *in_rect, ot_rect *out_rect);
    ot_schedule_mode (*pfn_vpss_query_schedule_mode)(td_void);
} vpss_export_func;

#define ckfn_vpss_entry() check_func_entry(OT_ID_VPSS)

#define ckfn_vpss_register() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_register != TD_NULL)
#define call_vpss_register(info) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_register(info)

#define ckfn_vpss_un_register() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_un_register != TD_NULL)
#define call_vpss_un_register(mod_id) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_un_register(mod_id)

#define ckfn_vpss_update_vi_vpss_mode() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_update_vi_vpss_mode != TD_NULL)
#define call_vpss_update_vi_vpss_mode(vi_vpss_mode) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_update_vi_vpss_mode(vi_vpss_mode)

#define ckfn_vpss_update_vi_video_mode() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_update_vi_video_mode != TD_NULL)
#define call_vpss_update_vi_video_mode(vi_video_mode) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_update_vi_video_mode(vi_video_mode)

#define ckfn_vpss_vi_submit_task() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_submit_task != TD_NULL)
#define call_vpss_vi_submit_task(grp, vi_frame, lost_frame, pixel_format) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_submit_task(grp, vi_frame, lost_frame, pixel_format)

#define ckfn_vpss_vi_start_task() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_start_task != TD_NULL)
#define call_vpss_vi_start_task(vpss_id) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_start_task(vpss_id)

#define ckfn_vpss_vi_task_done() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_task_done != TD_NULL)
#define call_vpss_vi_task_done(vpss_id, state) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_task_done(vpss_id, state)

#define ckfn_vpss_down_semaphore() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_down_semaphore != TD_NULL)
#define call_vpss_down_semaphore(grp) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_down_semaphore(grp)

#define ckfn_vpss_up_semaphore() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_up_semaphore != TD_NULL)
#define call_vpss_up_semaphore(grp) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_up_semaphore(grp)

#define ckfn_vpss_vi_early_irq_proc() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_early_irq_proc != TD_NULL)
#define call_vpss_vi_early_irq_proc(grp) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vi_early_irq_proc(grp)

#define ckfn_vpss_is_grp_existed() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_is_grp_existed != TD_NULL)
#define call_vpss_is_grp_existed(grp) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_is_grp_existed(grp)

#define ckfn_vpss_query_buffer_share_info() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_query_buffer_share_info != TD_NULL)
#define call_vpss_query_buffer_share_info(grp, video_frame) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_query_buffer_share_info(grp, video_frame)

#define ckfn_vpss_set_3dnr_rnt_info() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_set_3dnr_rnt_info != TD_NULL)
#define call_vpss_set_3dnr_rnt_info(grp, rnt_info) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_set_3dnr_rnt_info(grp, rnt_info)

#define ckfn_vpss_start_vgs_task() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_start_vgs_task != TD_NULL)
#define call_vpss_start_vgs_task(node_info) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_start_vgs_task(node_info)

#ifdef CONFIG_OT_VPSS_MCF_SUPPORT
#define ckfn_vpss_receive_mcf_frame() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_receive_mcf_frame != TD_NULL)
#define call_vpss_receive_mcf_frame(grp, video_frame) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_receive_mcf_frame(grp, video_frame)
#define ckfn_vpss_get_chn_attr() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_get_chn_attr != TD_NULL)
#define call_vpss_get_chn_attr(grp, chn, attr) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_get_chn_attr(grp, chn, attr)
#define ckfn_vpss_set_chn_attr() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_set_chn_attr != TD_NULL)
#define call_vpss_set_chn_attr(grp, chn, attr) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_set_chn_attr(grp, chn, attr)
#define ckfn_vpss_enable_chn() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_enable_chn != TD_NULL)
#define call_vpss_enable_chn(grp, chn) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_enable_chn(grp, chn)
#define ckfn_vpss_disable_chn() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_disable_chn != TD_NULL)
#define call_vpss_disable_chn(grp, chn) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_disable_chn(grp, chn)
#define ckfn_vpss_set_mcf_grp_info() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_set_mcf_grp_info != TD_NULL)
#define call_vpss_set_mcf_grp_info(mcf_grp_info) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_set_mcf_grp_info(mcf_grp_info)
#define ckfn_vpss_get_grp_attr() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_get_grp_attr != TD_NULL)
#define call_vpss_get_grp_attr(grp, vpss_grp_attr) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_get_grp_attr(grp, vpss_grp_attr)
#endif

#define ckfn_vpss_vo_notify_vpss() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vo_notify_vpss != TD_NULL)
#define call_vpss_vo_notify_vpss() \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_vo_notify_vpss()

#define ckfn_vpss_update_schedule_mode() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_update_schedule_mode != TD_NULL)
#define call_vpss_update_schedule_mode(mode) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_update_schedule_mode(mode)

#ifdef CONFIG_OT_SNAP_SUPPORT
#define ckfn_vpss_online_update_snap_frame_flag() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_online_update_snap_frame_flag != TD_NULL)
#define call_vpss_online_update_snap_frame_flag(grp, snap_info) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_online_update_snap_frame_flag(grp, snap_info)
#endif

#define ckfn_vpss_calc_ratio_coverex() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_calc_ratio_coverex != TD_NULL)
#define call_vpss_calc_ratio_coverex(grp, chn, rgn_modify, in_rect, out_rect) \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_calc_ratio_coverex(grp, chn, rgn_modify, in_rect, out_rect)

#define ckfn_vpss_query_schedule_mode() \
    (func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_query_schedule_mode != TD_NULL)
#define call_vpss_query_schedule_mode() \
    func_entry(vpss_export_func, OT_ID_VPSS)->pfn_vpss_query_schedule_mode()

#ifdef __cplusplus
extern "C" {
#endif

td_u32 vpss_get_state(td_void);

#ifdef __cplusplus
}
#endif

#endif /* VPSS_EXT_H */

