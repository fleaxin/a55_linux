/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "pciv_proc.h"

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_video.h"
#include "sys_ext.h"
#include "pciv.h"
#include "mkp_pciv.h"
#include "pciv_drvadp.h"
#include "securec.h"

#define PCIV_PROC_SHOW_NUM      (OT_PCIV_MAX_BUF_NUM * 2 + 1)

static td_char *print_pixel_format(ot_pixel_format pixel_format)
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

inline static td_char *print_field(ot_video_field field)
{
    switch (field) {
        case OT_VIDEO_FIELD_TOP:        return "top";
        case OT_VIDEO_FIELD_BOTTOM:     return "bot";
        case OT_VIDEO_FIELD_FRAME:      return "frm";
        case OT_VIDEO_FIELD_INTERLACED: return "intl";
        default: return "N/A";
    }
}

inline static td_char *print_hide(td_bool is_hide)
{
    if (is_hide) {
        return "Y";
    } else {
        return "N";
    }
}

static td_void pciv_proc_show_chn_attr(osal_proc_entry_t *s)
{
    pciv_chn_ctx    *chn_ctx = TD_NULL;
    ot_pciv_attr    *pciv_attr = TD_NULL;
    ot_pciv_chn     pciv_chn;

    call_sys_print_proc_title(s, "pciv channel attr");
    osal_seq_printf(s, "%8s"     "%8s"    "%8s"     "%8s"     "%8s"
                    "%14s"     "%8s"     "%10s"     "%18s" "\n",
                    "pciv_chn", "width", "height", "stride", "field",
                    "pixel_format", "buf_cnt", "buf_size", "phys_addr[0]");

    for (pciv_chn = 0; pciv_chn < OT_PCIV_MAX_CHN_NUM; pciv_chn++) {
        chn_ctx = pciv_get_context(pciv_chn);
        if (chn_ctx->is_create == TD_FALSE) {
            continue;
        }

        pciv_attr = &chn_ctx->pciv_attr;
        osal_seq_printf(s, "%8d" "%8d" "%8d" "%8d" "%8s" "%14s" "%8d" "%10d" "%18lx" "\n",
                        pciv_chn,
                        pciv_attr->pic_attr.width,
                        pciv_attr->pic_attr.height,
                        pciv_attr->pic_attr.stride[0],
                        print_field(pciv_attr->pic_attr.field),
                        print_pixel_format(pciv_attr->pic_attr.pixel_format),
                        pciv_attr->blk_cnt,
                        pciv_attr->blk_size,
                        (td_ulong)pciv_attr->phys_addr[0]);
    }
}

static td_void pciv_proc_show_chn_status(osal_proc_entry_t *s)
{
    td_u32          i;
    td_s32          ret;
    pciv_chn_ctx    *chn_ctx = TD_NULL;
    ot_pciv_chn     pciv_chn;
    td_char         buf_status_string[PCIV_PROC_SHOW_NUM] = { 0 };

    call_sys_print_proc_title(s, "pciv channel status");
    osal_seq_printf(s, "%8s"    "%8s"   "%12s"   "%12s"   "%12s"  "%12s"
                    "%12s"     "%12s"     "%12s"     "%20s"    "\n",
                    "pciv_chn",  "is_hide",   "remote_chip", "remote_chn", "get_cnt",
                    "send_cnt", "resp_cnt", "lost_cnt", "notify_cnt", "buf_status");

    for (pciv_chn = 0; pciv_chn < OT_PCIV_MAX_CHN_NUM; pciv_chn++) {
        chn_ctx = pciv_get_context(pciv_chn);
        if (chn_ctx->is_create == TD_FALSE) {
            continue;
        }

        for (i = 0; i < chn_ctx->pciv_attr.blk_cnt; i++) {
            // 2: Separate buff state
            ret = sprintf_s(&buf_status_string[i * 2], PCIV_PROC_SHOW_NUM - i * 2, "%2d", chn_ctx->buf_status[i]);
            if (ret < 0) {
                pciv_err_trace("sprintf_s get a error return code!\n");
                return;
            }
        }

        osal_seq_printf(s, "%8d" "%8s"  "%12d" "%12d" "%12d" "%12d" "%12d" "%12d" "%12d" "%20s" "\n",
                        pciv_chn,
                        print_hide(chn_ctx->is_hide),
                        chn_ctx->pciv_attr.remote_obj.chip_id,
                        chn_ctx->pciv_attr.remote_obj.pciv_chn,
                        chn_ctx->get_cnt,
                        chn_ctx->send_cnt,
                        chn_ctx->resp_cnt,
                        chn_ctx->lost_cnt,
                        chn_ctx->notify_cnt,
                        buf_status_string);
    }
}

static td_void pciv_proc_show_msg_status(osal_proc_entry_t *s)
{
    pciv_chn_ctx    *chn_ctx = TD_NULL;
    ot_pciv_chn     pciv_chn;
    pciv_gap_record gap_record;
    td_s32          ret;

    call_sys_print_proc_title(s, "pciv message status");
    osal_seq_printf(s, "%8s"     "%15s"      "%15s"     "%15s"
                    "%15s"      "%15s"      "%15s"    "\n",
                    "pciv_chn", "rdone_gap", "max_rdone_gap", "min_rdone_gap",
                    "wdone_gap", "max_wdone_gap", "min_wdone_gap");

    for (pciv_chn = 0; pciv_chn < OT_PCIV_MAX_CHN_NUM; pciv_chn++) {
        chn_ctx = pciv_get_context(pciv_chn);
        if (chn_ctx->is_create == TD_FALSE) {
            continue;
        }
        ret = pciv_drv_adp_get_gap_record(pciv_chn, &gap_record);
        if (ret != TD_SUCCESS) {
            pciv_err_trace("get gap record failed!\n");
            return;
        }
        osal_seq_printf(s, "%8d" "%15u" "%15u" "%15u" "%15u" "%15u" "%15u" "\n",
                        pciv_chn,
                        gap_record.rdone_gap,
                        gap_record.max_rdone_gap,
                        gap_record.min_rdone_gap,
                        gap_record.wdone_gap,
                        gap_record.max_wdone_gap,
                        gap_record.min_wdone_gap);
    }
}

td_s32 pciv_proc_show(osal_proc_entry_t *s)
{
    osal_seq_printf(s, "\n[PCIV] Version: [" OT_MPP_VERSION "], Build Time:["__DATE__", "__TIME__"]\n\n");

    if ((ckfn_sys_entry() == TD_FALSE) || (ckfn_sys_print_proc_title() == TD_FALSE)) {
        pciv_err_trace("sys not ready!\n");
        return OT_ERR_PCIV_NOT_READY;
    }
    pciv_proc_show_chn_attr(s);
    pciv_proc_show_chn_status(s);
    pciv_proc_show_msg_status(s);

    return TD_SUCCESS;
}

