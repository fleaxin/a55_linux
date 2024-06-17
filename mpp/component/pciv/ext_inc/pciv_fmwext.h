/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef PCIV_FMWEXT_H
#define PCIV_FMWEXT_H

#include "ot_common_pciv.h"

#define PCIV_FMW_MAX_CHN_NUM OT_PCIV_MAX_CHN_NUM

/*  bind type for pciv  */
typedef enum {
    PCIV_BIND_VI    = 0,
    PCIV_BIND_VO    = 1,
    PCIV_BIND_VDEC  = 2,
    PCIV_BIND_VPSS  = 3,
    PCIV_BIND_VENC  = 4,
    PCIV_BIND_BUTT
} pciv_bind_type;

/*  bind object struct for pciv  */
typedef struct {
    td_bool         is_block;
    td_bool         is_vgs_bypass;
    td_bool         is_vpss_send;
    pciv_bind_type  type; /*  bind type for pciv  */
} pciv_bind_obj;

typedef struct {
    td_u32              index;        /* index of pciv channel buffer */
    td_u32              cnt;          /* total number of picture */

    td_phys_addr_t      phys_addr;     /* physical address of video buffer which store pcicture info */
    td_u32              pool_id;      /* pool id of video buffer which store picture info */

    td_u64              pts;          /* time stamp */
    td_u32              time_ref;     /* time reference */
    pciv_bind_type      src_type;     /* bind type for pciv */
    ot_video_field      filed;        /* video field type */
    td_bool             block;        /* the flag of block */
    ot_color_gamut      color_gamut;
    td_u32              header_stride[OT_MAX_COLOR_COMPONENT];
    td_u32              stride[OT_MAX_COLOR_COMPONENT];
    td_u32              header_offset[OT_MAX_COLOR_COMPONENT];
    td_u32              data_offset[OT_MAX_COLOR_COMPONENT];
    ot_mod_id           mod_id;

    td_u64              user_data[OT_MAX_USER_DATA_NUM];
    td_u32              frame_flag;     /* frame_flag, can be OR operation. */
} pciv_pic;

typedef td_s32 fn_pciv_src_send_pic(ot_pciv_chn pciv_chn, const pciv_pic *src_pic);
typedef td_s32 fn_pciv_recv_pic_free(ot_pciv_chn pciv_chn, pciv_pic *recv_pic);
typedef td_s32 fn_pciv_get_chn_share_buf_state(ot_pciv_chn pciv_chn);
typedef td_s32 fn_pciv_set_chn_share_buf_state(ot_pciv_chn pciv_chn);

typedef struct {
    fn_pciv_src_send_pic              *pf_src_send_pic;
    fn_pciv_recv_pic_free             *pf_recv_pic_free;
    fn_pciv_get_chn_share_buf_state   *pf_query_pciv_chn_share_buf_state;
    fn_pciv_set_chn_share_buf_state   *pf_free_pciv_chn_share_buf_state;
} pciv_fmw_callback;

#define pciv_fmw_err_trace(format, ...)                                                                       \
    OT_ERR_TRACE(OT_ID_PCIVFMW, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_fmw_warn_trace(format, ...)                                                                       \
    OT_WARN_TRACE(OT_ID_PCIVFMW, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_fmw_info_trace(format, ...)                                                                       \
    OT_INFO_TRACE(OT_ID_PCIVFMW, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_fmw_debug_trace(format, ...)                                                                       \
    OT_DEBUG_TRACE(OT_ID_PCIVFMW, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_fmw_alert_trace(format, ...)                                                                       \
    OT_ALERT_TRACE(OT_ID_PCIVFMW, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_fmw_check_chn_id_return(chn_id) \
do { \
    if (((chn_id) < 0) || ((chn_id) >= PCIV_FMW_MAX_CHN_NUM)) { \
        pciv_fmw_err_trace("invalid chn id:%d \n", chn_id); \
        return OT_ERR_PCIV_INVALID_CHN_ID; \
    } \
}while (0)

#define pciv_fmw_check_chn_id_void_return(chn_id) \
do { \
    if (((chn_id) < 0) || ((chn_id) >= PCIV_FMW_MAX_CHN_NUM)) { \
        pciv_fmw_err_trace("invalid chn id:%d \n", chn_id); \
        return; \
    } \
}while (0)

#define pciv_fmw_check_ptr_return(ptr) \
do { \
    if ((ptr) == TD_NULL) { \
        pciv_fmw_err_trace("PTR is NULL!\n"); \
        return OT_ERR_PCIV_NULL_PTR; \
    } \
}while (0)

#define pciv_fmw_check_ptr_void_return(ptr) \
do { \
    if ((ptr) == TD_NULL) { \
        pciv_fmw_err_trace("PTR is NULL!\n"); \
        return; \
    } \
}while (0)

td_s32  pciv_firmware_create(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id);
td_s32  pciv_firmware_destroy(ot_pciv_chn pciv_chn);
td_s32  pciv_firmware_set_attr(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id);
td_s32  pciv_firmware_start(ot_pciv_chn pciv_chn);
td_s32  pciv_firmware_stop(ot_pciv_chn pciv_chn);
td_s32  pciv_firmware_malloc(td_u32 size, td_s32 local_id, td_phys_addr_t *phys_addr);
td_s32  pciv_firmware_free(td_phys_addr_t phys_addr);
td_s32  pciv_firmware_malloc_chn_buffer(ot_pciv_chn pciv_chn, td_u32 index,
    td_u32 size, td_s32 local_id, td_phys_addr_t *phys_addr);
td_s32  pciv_firmware_free_chn_buffer(ot_pciv_chn pciv_chn, td_u32 index, td_s32 local_id);
td_s32  pciv_firmware_src_pic_free(ot_pciv_chn pciv_chn, const pciv_pic *src_pic);
td_s32  pciv_firmware_recv_pic_and_send(ot_pciv_chn pciv_chn, const pciv_pic *recv_pic);
td_s32  pciv_firmware_window_vb_create(const ot_pciv_window_vb_cfg *cfg);
td_s32  pciv_firmware_window_vb_destroy(td_void);
td_s32  pciv_firmware_register_func(const pciv_fmw_callback *call_back);

#endif

