/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "pciv_firmware_proc.h"

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_video.h"
#include "sys_ext.h"
#include "pciv_fmwext.h"
#include "pciv_firmware.h"

static td_char *pciv_pf_to_string(ot_pixel_format pixel_format)
{
    switch (pixel_format) {
        case OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            return "YVU-SP422";
        case OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
            return "YVU-SP420";
        case OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422:
            return "YUV-SP422";
        case OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420:
            return "YUV-SP420";
        case OT_PIXEL_FORMAT_YUV_400:
            return "YUV400";
        default:
            return "N/A";
    }
}

static td_char *pciv_dr_to_string(ot_dynamic_range dynamic_range)
{
    td_char *dr_string = TD_NULL;
    switch (dynamic_range) {
        case OT_DYNAMIC_RANGE_SDR8:
            dr_string = "SDR8";
            break;
        case OT_DYNAMIC_RANGE_SDR10:
            dr_string = "SDR10";
            break;
        case OT_DYNAMIC_RANGE_HDR10:
            dr_string = "HDR10";
            break;
        case OT_DYNAMIC_RANGE_HLG:
            dr_string = "HLG";
            break;
        case OT_DYNAMIC_RANGE_SLF:
            dr_string = "SLF";
            break;
        case OT_DYNAMIC_RANGE_XDR:
            dr_string = "XDR";
            break;
        default:
            dr_string = "N/A";
            break;
    }
    return dr_string;
}

static td_char *pciv_cm_to_string(ot_compress_mode compress_mode)
{
    td_char *cm_string = TD_NULL;
    switch (compress_mode) {
        case OT_COMPRESS_MODE_NONE:
            cm_string = "NONE";
            break;
        case OT_COMPRESS_MODE_SEG:
            cm_string = "SEG";
            break;
        case OT_COMPRESS_MODE_SEG_COMPACT:
            cm_string = "COMPACT";
            break;
        case OT_COMPRESS_MODE_TILE:
            cm_string = "TILE";
            break;
        case OT_COMPRESS_MODE_LINE:
            cm_string = "LINE";
            break;
        case OT_COMPRESS_MODE_FRAME:
            cm_string = "FRAME";
            break;
        default:
            cm_string = "N/A";
            break;
    }
    return cm_string;
}

static td_char *pciv_vf_to_string(ot_video_format video_format)
{
    td_char *vf_string = TD_NULL;
    switch (video_format) {
        case OT_VIDEO_FORMAT_LINEAR:
            vf_string = "LINEAR";
            break;
        case OT_VIDEO_FORMAT_TILE_64x16:
            vf_string = "TILE_64x16";
            break;
        case OT_VIDEO_FORMAT_TILE_16x8:
            vf_string = "TILE_16x8";
            break;
        default:
            vf_string = "N/A";
            break;
    }
    return vf_string;
}

static td_void pciv_fmw_proc_show_chn_info(osal_proc_entry_t *s)
{
    pciv_fmw_chn    *fmw_chn = TD_NULL;
    ot_pciv_chn     pciv_chn;

    call_sys_print_proc_title(s, "pciv firmware channel info");
    osal_seq_printf(s, "%8s" "%12s" "%12s" "%12s" "%12s" "%12s" "%12s" "%12s\n",
                    "pciv_chn", "get_cnt", "send_cnt", "resp_cnt",
                    "lost_cnt", "new_do",  "old_undo", "pool_id[0]");
    for (pciv_chn = 0; pciv_chn < PCIV_FMW_MAX_CHN_NUM; pciv_chn++) {
        fmw_chn = pciv_fmw_get_context(pciv_chn);
        if (fmw_chn->is_create == TD_FALSE) {
            continue;
        }
        osal_seq_printf(s, "%8d" "%12d" "%12d" "%12d" "%12d" "%12d" "%12d" "%12d\n",
                        pciv_chn,
                        fmw_chn->get_cnt,
                        fmw_chn->send_cnt,
                        fmw_chn->resp_cnt,
                        fmw_chn->lost_cnt,
                        fmw_chn->new_do_cnt,
                        fmw_chn->old_undo_cnt,
                        fmw_chn->pool_id[0]);
    }
}

static td_void pciv_fmw_proc_show_chn_pic_attr_info(osal_proc_entry_t *s)
{
    pciv_fmw_chn    *fmw_chn = TD_NULL;
    ot_pciv_chn     pciv_chn;

    call_sys_print_proc_title(s, "pciv firmware channel picture attr info");
    osal_seq_printf(s, "%8s" "%8s" "%8s" "%8s" "%14s" "%8s" "%10s" "%18s\n",
                    "pciv_chn",     "width",    "height",    "stride",
                    "pixel_format", "dynamic",  "compress",  "video_format");
    for (pciv_chn = 0; pciv_chn < PCIV_FMW_MAX_CHN_NUM; pciv_chn++) {
        fmw_chn = pciv_fmw_get_context(pciv_chn);
        if (fmw_chn->is_create == TD_FALSE) {
            continue;
        }
        osal_seq_printf(s, "%8d" "%8d" "%8d" "%8d" "%14s" "%8s" "%10s" "%18s\n",
                        pciv_chn,
                        fmw_chn->pic_attr.width,
                        fmw_chn->pic_attr.height,
                        fmw_chn->pic_attr.stride[0],
                        pciv_pf_to_string(fmw_chn->pic_attr.pixel_format),
                        pciv_dr_to_string(fmw_chn->pic_attr.dynamic_range),
                        pciv_cm_to_string(fmw_chn->pic_attr.compress_mode),
                        pciv_vf_to_string(fmw_chn->pic_attr.video_format));
    }
}

static td_void pciv_fmw_proc_show_chn_queue_info(osal_proc_entry_t *s)
{
    pciv_fmw_chn    *fmw_chn = TD_NULL;
    ot_pciv_chn     pciv_chn;

    call_sys_print_proc_title(s, "pciv firmware channel queue info");
    osal_seq_printf(s, "%8s" "%10s" "%10s" "%10s" "%12s\n",
                    "pciv_chn", "busy_num", "free_num", "state", "timer_cnt");
    for (pciv_chn = 0; pciv_chn < PCIV_FMW_MAX_CHN_NUM; pciv_chn++) {
        fmw_chn = pciv_fmw_get_context(pciv_chn);
        if (fmw_chn->is_create == TD_FALSE) {
            continue;
        }
        osal_seq_printf(s, "%8d" "%10d" "%10d" "%10d" "%12d\n",
                        pciv_chn,
                        fmw_chn->pic_queue.busy_num,
                        fmw_chn->pic_queue.free_num,
                        fmw_chn->send_state,
                        fmw_chn->timer_cnt);
    }
}

static td_void pciv_fmw_proc_show_chn_call_vgs_info(osal_proc_entry_t *s)
{
    pciv_fmw_chn    *fmw_chn = TD_NULL;
    ot_pciv_chn     pciv_chn;

    call_sys_print_proc_title(s, "pciv firmware channel call vgs job info");
    osal_seq_printf(s, "%8s" "%18s" "%18s" "%18s" "%18s\n",
                    "pciv_chn",
                    "add_job_suc_cnt",  "add_job_fail_cnt",
                    "end_job_suc_cnt",  "end_job_fail_cnt");
    for (pciv_chn = 0; pciv_chn < PCIV_FMW_MAX_CHN_NUM; pciv_chn++) {
        fmw_chn = pciv_fmw_get_context(pciv_chn);
        if (fmw_chn->is_create == TD_FALSE) {
            continue;
        }
        osal_seq_printf(s, "%8d" "%18d" "%18d" "%18d" "%18d\n",
                        pciv_chn,
                        fmw_chn->add_job_suc_cnt,
                        fmw_chn->add_job_fail_cnt,
                        fmw_chn->end_job_suc_cnt,
                        fmw_chn->end_job_fail_cnt);
    }

    call_sys_print_proc_title(s, "pciv firmware channel call vgs func info");
    osal_seq_printf(s, "%16s" "%16s" "%16s" "%16s" "%16s" "%16s\n",
                    "trans_suc_cnt", "trans_fail_cnt",
                    "osd_suc_cnt",   "osd_fail_cnt",
                    "trans_cb_cnt",  "osd_cb_cnt");

    for (pciv_chn = 0; pciv_chn < PCIV_FMW_MAX_CHN_NUM; pciv_chn++) {
        fmw_chn = pciv_fmw_get_context(pciv_chn);
        if (fmw_chn->is_create == TD_FALSE) {
            continue;
        }
        osal_seq_printf(s, "%16d" "%16d" "%16d" "%16d" "%16d" "%16d\n",
                        fmw_chn->trans_task_suc_cnt,
                        fmw_chn->trans_task_fail_cnt,
                        fmw_chn->osd_task_suc_cnt,
                        fmw_chn->osd_task_fail_cnt,
                        fmw_chn->trans_cb_cnt,
                        fmw_chn->osd_cb_cnt);
    }
}

td_s32 pciv_fmw_proc_show(osal_proc_entry_t *s)
{
    osal_seq_printf(s, "\n[PCIVF] Version: [" OT_MPP_VERSION "], Build Time:["__DATE__", "__TIME__"]\n\n");

    if ((ckfn_sys_entry() == TD_FALSE) || (ckfn_sys_print_proc_title() == TD_FALSE)) {
        pciv_fmw_err_trace("sys not ready!\n");
        return OT_ERR_PCIV_NOT_READY;
    }
    pciv_fmw_proc_show_chn_info(s);
    pciv_fmw_proc_show_chn_pic_attr_info(s);
    pciv_fmw_proc_show_chn_queue_info(s);
    pciv_fmw_proc_show_chn_call_vgs_info(s);

    return TD_SUCCESS;
}

