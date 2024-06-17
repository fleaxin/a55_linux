/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_param_check.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include "ot_common_isp.h"
#include "ot_common_3a.h"
#include "ot_common_ae.h"
#include "ot_common_awb.h"
#include "isp_debug.h"
#include "isp_main.h"
#include "mkp_isp.h"
#include "isp_ext_config.h"

static td_s32 isp_sharpen_auto_motion_attr_check(const char *src, const ot_isp_sharpen_auto_attr *auto_attr)
{
    td_u8 i, j;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        for (j = 0; j < OT_ISP_SHARPEN_GAIN_NUM; j++) {
            if (auto_attr->motion_texture_strength[j][i] > 0xFFF) {
                isp_err_trace("Err %s auto motion_texture_strength:%d! range:[0, 4095]\n", src,
                              auto_attr->motion_texture_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }

            if (auto_attr->motion_edge_strength[j][i] > 0xFFF) {
                isp_err_trace("Err %s auto motion_edge_strength:%d! range:[0, 4095]\n", src,
                              auto_attr->motion_edge_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }

        if (auto_attr->motion_texture_freq[i] > 0xFFF) {
            isp_err_trace("Err %s auto motion_texture_freq:%d! range:[0, 4095]\n", src,
                          auto_attr->motion_texture_freq[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->motion_edge_freq[i] > 0xFFF) {
            isp_err_trace("Err %s auto motion_edge_freq:%d! range:[0, 4095]\n", src, auto_attr->motion_edge_freq[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->motion_over_shoot[i] > 0x7F) {
            isp_err_trace("Err %s auto motion_over_shoot:%d! range:[0, 127]\n", src, auto_attr->motion_over_shoot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->motion_under_shoot[i] > 0x7F) {
            isp_err_trace("Err %s auto motion_under_shoot:%d! range:[0, 127]\n", src, auto_attr->motion_under_shoot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_sharpen_auto_strength_attr_check(const char *src, const ot_isp_sharpen_auto_attr *auto_attr)
{
    td_u8 i, j;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        for (j = 0; j < OT_ISP_SHARPEN_GAIN_NUM; j++) {
            if (auto_attr->texture_strength[j][i] > 0xFFF) {
                isp_err_trace("Err %s auto texture_strength:%d! range:[0, 4095]\n", src,
                              auto_attr->texture_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
            if (auto_attr->edge_strength[j][i] > 0xFFF) {
                isp_err_trace("Err %s auto edge_strength:%d! range:[0, 4095]\n", src, auto_attr->edge_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }
        for (j = 0; j < OT_ISP_SHARPEN_LUMA_NUM; j++) {
            if (auto_attr->luma_wgt[j][i] > OT_ISP_SHARPEN_LUMAWGT_MAX) {
                isp_err_trace("Err %s auto luma_wgt:%d! range:[0, %d]\n", src, auto_attr->luma_wgt[j][i],
                              OT_ISP_SHARPEN_LUMAWGT_MAX);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }
        if (auto_attr->texture_freq[i] > 0xFFF) {
            isp_err_trace("Err %s auto texture_freq:%d! range:[0, 4095]\n", src, auto_attr->texture_freq[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->edge_freq[i] > 0xFFF) {
            isp_err_trace("Err %s auto edge_freq:%d! range:[0, 4095]\n", src, auto_attr->edge_freq[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_sharpen_auto_shoot_attr_check(const char *src, const ot_isp_sharpen_auto_attr *auto_attr)
{
    td_u8 i;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (auto_attr->over_shoot[i] > 0x7F) {
            isp_err_trace("Err %s auto over_shoot:%d! range:[0, 127]\n", src, auto_attr->over_shoot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->under_shoot[i] > 0x7F) {
            isp_err_trace("Err %s auto under_shoot:%d! range:[0, 127]\n", src, auto_attr->under_shoot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->edge_filt_strength[i] > 0x3F) {
            isp_err_trace("Err %s auto edge_filt_strength:%d! range:[0, 63]\n", src, auto_attr->edge_filt_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->edge_filt_max_cap[i] > OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP_MAX) {
            isp_err_trace("Err %s auto edge_filt_max_cap:%d! range:[0, %d]\n", src,
                          auto_attr->edge_filt_max_cap[i], OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP_MAX);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->r_gain[i] > OT_ISP_SHARPEN_RGAIN_MAX) {
            isp_err_trace("Err %s auto r_gain:%d! range:[0,%d]\n", src, auto_attr->r_gain[i], OT_ISP_SHARPEN_RGAIN_MAX);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->b_gain[i] > OT_ISP_SHARPEN_BGAIN_MAX) {
            isp_err_trace("Err %s auto b_gain:%d! range:0, %d]\n", src, auto_attr->b_gain[i], OT_ISP_SHARPEN_BGAIN_MAX);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->skin_gain[i] > 0x1F) {
            isp_err_trace("Err %s auto skin_gain:%d! range:[0, 31]\n", src, auto_attr->skin_gain[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->shoot_sup_adj[i] > 0XF) {
            isp_err_trace("Err %s auto shoot_sup_adj:%d! range:[0, 15]\n", src, auto_attr->shoot_sup_adj[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (auto_attr->max_sharp_gain[i] > 0x7FF) {
            isp_err_trace("Err %s auto max_sharp_gain:%d! range:[0, 0x7FF]\n", src, auto_attr->max_sharp_gain[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_sharpen_manual_strength_attr_check(const char *src, const ot_isp_sharpen_manual_attr *manual_attr)
{
    td_u8 j;
    for (j = 0; j < OT_ISP_SHARPEN_GAIN_NUM; j++) {
        if (manual_attr->texture_strength[j] > 0xFFF) {
            isp_err_trace("Err %s manual texture_strength:%d! range:[0,4095]\n", src, manual_attr->texture_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (manual_attr->edge_strength[j] > 0xFFF) {
            isp_err_trace("Err %s manual edge_strength:%d! range:[0, 4095]\n", src, manual_attr->edge_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    for (j = 0; j < OT_ISP_SHARPEN_LUMA_NUM; j++) {
        if (manual_attr->luma_wgt[j] > OT_ISP_SHARPEN_LUMAWGT_MAX) {
            isp_err_trace("Err %s manual luma_wgt:%d! range:[0, %d]\n", src, manual_attr->luma_wgt[j],
                OT_ISP_SHARPEN_LUMAWGT_MAX);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    if (manual_attr->texture_freq > 0xFFF) {
        isp_err_trace("Err %s manual texture_freq:%d! range:[0, 4095]\n", src, manual_attr->texture_freq);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->edge_freq > 0xFFF) {
        isp_err_trace("Err %s manual edge_freq:%d! range:[0, 4095]\n", src, manual_attr->edge_freq);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 isp_sharpen_manual_shoot_attr_check(const char *src, const ot_isp_sharpen_manual_attr *manual_attr)
{
    if (manual_attr->over_shoot > 0x7F) {
        isp_err_trace("Err %s manual_attr.over_shoot:%d! range:[0, 127]\n", src, manual_attr->over_shoot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->under_shoot > 0x7F) {
        isp_err_trace("Err %s manual under_shoot:%d! range:[0, 127]\n", src, manual_attr->under_shoot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->edge_filt_strength > 0x3F) {
        isp_err_trace("Err %s manual edge_filt_strength:%d! range:[0, 63]\n", src, manual_attr->edge_filt_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->edge_filt_max_cap > OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP_MAX) {
        isp_err_trace("Err %s manual edge_filt_max_cap:%d! range:[0, %d]\n", src,
                      manual_attr->edge_filt_max_cap, OT_ISP_SHARPEN_EDGE_FILT_MAX_CAP_MAX);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->r_gain > OT_ISP_SHARPEN_RGAIN_MAX) {
        isp_err_trace("Err %s manual r_gain:%d! range:[0, %d]\n", src, manual_attr->r_gain, OT_ISP_SHARPEN_RGAIN_MAX);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->b_gain > OT_ISP_SHARPEN_BGAIN_MAX) {
        isp_err_trace("Err %s manual b_gain:%d! range:[0, %d]\n", src, manual_attr->b_gain, OT_ISP_SHARPEN_BGAIN_MAX);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->skin_gain > 0x1F) {
        isp_err_trace("Err %s manual skin_gain:%d! range:[0, 31]\n", src, manual_attr->skin_gain);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->shoot_sup_adj > 0xF) {
        isp_err_trace("Err %s manual shoot_sup_adj:%d! range:[0, 15]\n", src, manual_attr->shoot_sup_adj);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->max_sharp_gain > 0x7FF) {
        isp_err_trace("Err %s manual max_sharp_gain:%d! range:[0, 0x7FF]\n", src, manual_attr->max_sharp_gain);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}


static td_s32 isp_sharpen_manual_motion_attr_check(const char *src, const ot_isp_sharpen_manual_attr *manual_attr)
{
    td_u8 j;
    for (j = 0; j < OT_ISP_SHARPEN_GAIN_NUM; j++) {
        if (manual_attr->motion_texture_strength[j] > 0xFFF) {
            isp_err_trace("Err %s manual motion_texture_strength:%d! range:[0,4095]\n", src,
                          manual_attr->motion_texture_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (manual_attr->motion_edge_strength[j] > 0xFFF) {
            isp_err_trace("Err %s manual motion_edge_strength:%d! range:[0, 4095]\n", src,
                          manual_attr->motion_edge_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    if (manual_attr->motion_texture_freq > 0xFFF) {
        isp_err_trace("Err %s manual motion_texture_freq:%d! range:[0, 4095]\n", src, manual_attr->motion_texture_freq);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->motion_edge_freq > 0xFFF) {
        isp_err_trace("Err %s manual motion_edge_freq:%d! range:[0, 4095]\n", src, manual_attr->motion_edge_freq);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->motion_over_shoot > 0x7F) {
        isp_err_trace("Err %s manual motion_over_shoot:%d! range:[0, 127]\n", src, manual_attr->motion_over_shoot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->motion_under_shoot > 0x7F) {
        isp_err_trace("Err %s manual motion_under_shoot:%d! range:[0, 127]\n", src, manual_attr->motion_under_shoot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_sharpen_auto_attr_check(const char *src, const ot_isp_sharpen_auto_attr *auto_attr)
{
    td_s32 ret;

    ret = isp_sharpen_auto_strength_attr_check(src, auto_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_sharpen_auto_shoot_attr_check(src, auto_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_sharpen_auto_motion_attr_check(src, auto_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_sharpen_manual_attr_check(const char *src, const ot_isp_sharpen_manual_attr *manual_attr)
{
    td_s32 ret;

    ret = isp_sharpen_manual_strength_attr_check(src, manual_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_sharpen_manual_shoot_attr_check(src, manual_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_sharpen_manual_motion_attr_check(src, manual_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_sharpen_comm_attr_check(const char *src, const ot_isp_sharpen_attr *shp_attr)
{
    isp_check_bool_return(shp_attr->en);
    isp_check_bool_return(shp_attr->motion_en);
    if (shp_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err sharpen type %d!\n", shp_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (shp_attr->detail_map >= OT_ISP_SHARPEN_BUTT) {
        isp_err_trace("Err %s detail_map %d!\n", src, shp_attr->detail_map);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (shp_attr->motion_threshold0 > 15) {   /* range:[0, 15] */
        isp_err_trace("Err %s motion_threshold0 %d!\n", src, shp_attr->motion_threshold0);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (shp_attr->motion_threshold1 > 15) {   /* range:[0, 15] */
        isp_err_trace("Err %s motion_threshold1 %d!\n", src, shp_attr->motion_threshold1);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (shp_attr->motion_threshold0 > shp_attr->motion_threshold1) {
        isp_err_trace("Err %s: motion_threshold0 should not larger than motion_threshold1!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (shp_attr->motion_gain0 > 256) {   /* range:[0, 256] */
        isp_err_trace("Err %s motion_gain0 %d!\n", src, shp_attr->motion_gain0);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (shp_attr->motion_gain1 > 256) {   /* range:[0, 256] */
        isp_err_trace("Err %s motion_gain1 %d!\n", src, shp_attr->motion_gain1);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_drc_attr_check(const char *src, const ot_isp_drc_attr *drc_attr)
{
    td_u8 i;
    isp_check_bool_return(drc_attr->enable);
    isp_check_bool_return(drc_attr->color_correction_ctrl);

    if ((drc_attr->asymmetry_curve.asymmetry > 0x1E) || (drc_attr->asymmetry_curve.asymmetry < 0x1)) {
        isp_err_trace("Err %s asymmetry: %d!\n", src, drc_attr->asymmetry_curve.asymmetry);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((drc_attr->asymmetry_curve.second_pole > 0xD2) || (drc_attr->asymmetry_curve.second_pole < 0x96)) {
        isp_err_trace("Err %s second_pole: %d!\n", src, drc_attr->asymmetry_curve.second_pole);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((drc_attr->asymmetry_curve.stretch > 0x3C) || (drc_attr->asymmetry_curve.stretch < 0x1E)) {
        isp_err_trace("Err %s stretch: %d!\n", src, drc_attr->asymmetry_curve.stretch);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if ((drc_attr->asymmetry_curve.compress > 0xc8) || (drc_attr->asymmetry_curve.compress < 0x64)) {
        isp_err_trace("Err %s compress: %d!\n", src, drc_attr->asymmetry_curve.compress);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((drc_attr->curve_select >= OT_ISP_DRC_CURVE_BUTT)) {
        isp_err_trace("Err %s curve_select: %d!\n", src, drc_attr->curve_select);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type: %d!\n", src, drc_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->auto_attr.strength > OT_ISP_DRC_STRENGTH_MAX) {
        isp_err_trace("Err %s auto_attr.strength: %d!\n", src, drc_attr->auto_attr.strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->auto_attr.strength_max > OT_ISP_DRC_STRENGTH_MAX) {
        isp_err_trace("Err %s auto_attr.strength_max: %d!\n", src, drc_attr->auto_attr.strength_max);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->auto_attr.strength_min > OT_ISP_DRC_STRENGTH_MAX) {
        isp_err_trace("Err %s auto_attr.strength_min: %d!\n", src, drc_attr->auto_attr.strength_min);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->auto_attr.strength_max < drc_attr->auto_attr.strength_min) {
        isp_err_trace("Err %s: auto_attr.strength_max (%d) must be greater than auto_attr.strength_min (%d)!\n",
            src, drc_attr->auto_attr.strength_max, drc_attr->auto_attr.strength_min);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->manual_attr.strength > OT_ISP_DRC_STRENGTH_MAX) {
        isp_err_trace("Err %s manual_attr.strength: %d!\n", src, drc_attr->manual_attr.strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->bright_gain_limit > 0xF) {
        isp_err_trace("Err %s bright_gain_limit: %d!\n", src, drc_attr->bright_gain_limit);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->bright_gain_limit_step > 0xF) {
        isp_err_trace("Err %s bright_gain_limit_step: %d!\n", src, drc_attr->bright_gain_limit_step);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (drc_attr->dark_gain_limit_luma > 0x85) {
        isp_err_trace("Err %s dark_gain_limit_luma: %d!\n", src, drc_attr->dark_gain_limit_luma);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->dark_gain_limit_chroma > 0x85) {
        isp_err_trace("Err %s dark_gain_limit_chroma: %d!\n", src, drc_attr->dark_gain_limit_chroma);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->purple_reduction_strength > 0x80) {
        isp_err_trace("Err %s purple_reduction_strength: %d!\n", src, drc_attr->purple_reduction_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->contrast_ctrl > 0xF) {
        isp_err_trace("Err %s contrast_ctrl: %d!\n", src, drc_attr->contrast_ctrl);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->spatial_filter_coef > OT_ISP_DRC_SPA_FLT_COEF_MAX) {
        isp_err_trace("Err %s spatial_filter_coef: %d!\n", src, drc_attr->spatial_filter_coef);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->range_filter_coef > 0xA) {
        isp_err_trace("Err %s range_filter_coef: %d!\n", src, drc_attr->range_filter_coef);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->rim_reduction_strength > 0x40) {
        isp_err_trace("Err %s rim_reduction_strength: %d!\n", src, drc_attr->rim_reduction_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->rim_reduction_threshold > 0x80) {
        isp_err_trace("Err %s rim_reduction_threshold: %d!\n", src, drc_attr->rim_reduction_threshold);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->detail_adjust_coef > 0xF) {
        isp_err_trace("Err %s detail_adjust_coef: %d!\n", src, drc_attr->detail_adjust_coef);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->detail_adjust_coef_x > 0xF) {
        isp_err_trace("Err %s detail_adjust_coef_x: %d!\n", src, drc_attr->detail_adjust_coef_x);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_luma_max < drc_attr->blend_luma_bright_min) {
        isp_err_trace("Err %s: blend_luma_max (%d) must be greater than blend_luma_bright_min (%d)!\n",
            src, drc_attr->blend_luma_max, drc_attr->blend_luma_bright_min);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_luma_bright_slope > 0xF) {
        isp_err_trace("Err %s blend_luma_bright_slope: %d!\n", src, drc_attr->blend_luma_bright_slope);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_luma_max < drc_attr->blend_luma_dark_min) {
        isp_err_trace("Err %s: blend_luma_max (%d) must be greater than blend_luma_dark_min (%d)!\n",
            src, drc_attr->blend_luma_max, drc_attr->blend_luma_dark_min);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_luma_dark_slope > 0xF) {
        isp_err_trace("Err %s blend_luma_dark_slope: %d!\n", src, drc_attr->blend_luma_dark_slope);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_detail_max < drc_attr->blend_detail_bright_min) {
        isp_err_trace("Err %s: blend_detail_max (%d) must be greater than blend_detail_bright_min (%d)!\n",
            src, drc_attr->blend_detail_max, drc_attr->blend_detail_bright_min);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_detail_bright_slope > 0xF) {
        isp_err_trace("Err %s blend_detail_bright_slope: %d!\n", src, drc_attr->blend_detail_bright_slope);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_detail_max < drc_attr->blend_detail_dark_min) {
        isp_err_trace("Err %s: blend_detail_max (%d) must be greater than blend_detail_dark_min (%d)!\n",
            src, drc_attr->blend_detail_max, drc_attr->blend_detail_dark_min);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->blend_detail_dark_slope > 0xF) {
        isp_err_trace("Err %s blend_detail_dark_slope: %d!\n", src, drc_attr->blend_detail_dark_slope);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->detail_adjust_coef_blend > 0xF) {
        isp_err_trace("Err %s detail_adjust_coef_blend: %d!\n", src, drc_attr->detail_adjust_coef_blend);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->low_saturation_color_ctrl > 0xF) {
        isp_err_trace("Err %s low_saturation_color_ctrl: %d!\n", src, drc_attr->low_saturation_color_ctrl);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (drc_attr->high_saturation_color_ctrl > 0xF) {
        isp_err_trace("Err %s high_saturation_color_ctrl: %d!\n", src, drc_attr->high_saturation_color_ctrl);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_DRC_CC_NODE_NUM; i++) {
        if (drc_attr->color_correction_lut[i] > OT_ISP_DRC_CC_COEF_MAX) {
            isp_err_trace("Err %s color_correction_lut[%d]: %d!\n", src, i, drc_attr->color_correction_lut[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_DRC_FLTX_NODE_NUM; i++) {
        if (drc_attr->filter_coef_x[i] > 0xF) {
            isp_err_trace("Err %s filter_coef_x[%d]: %d!\n", src, i, drc_attr->filter_coef_x[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_DRC_FLTX_NODE_NUM; i++) {
        if (drc_attr->filter_low_threshold_x[i] > 0xF) {
            isp_err_trace("Err %s filter_low_threshold_x[%d]: %d!\n", src, i, drc_attr->filter_low_threshold_x[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_DRC_FLTX_NODE_NUM; i++) {
        if (drc_attr->filter_high_threshold_x[i] > 0xF) {
            isp_err_trace("Err %s filter_high_threshold_x[%d]: %d!\n", src, i, drc_attr->filter_high_threshold_x[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_DRC_LMIX_NODE_NUM; i++) {
        if (drc_attr->local_mixing_bright[i] > 0x80) {
            isp_err_trace("Err %s local_mixing_bright[%d]: %d!\n", src, i, drc_attr->local_mixing_bright[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_DRC_LMIX_NODE_NUM; i++) {
        if (drc_attr->local_mixing_dark[i] > 0x80) {
            isp_err_trace("Err %s local_mixing_dark[%d]: %d!\n", src, i, drc_attr->local_mixing_dark[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_DRC_LMIX_NODE_NUM; i++) {
        if (drc_attr->local_mixing_bright_x[i] > 0x80) {
            isp_err_trace("Err %s local_mixing_bright_x[%d]: %d!\n", src, i, drc_attr->local_mixing_bright_x[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    for (i = 0; i < OT_ISP_DRC_LMIX_NODE_NUM; i++) {
        if (drc_attr->local_mixing_dark_x[i] > 0x80) {
            isp_err_trace("Err %s local_mixing_dark_x[%d]: %d!\n", src, i, drc_attr->local_mixing_dark_x[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_pregamma_attr_check(const char *src, const ot_isp_pregamma_attr *pregamma_attr)
{
    td_u32 i;
    isp_check_bool_return(pregamma_attr->enable);
    for (i = 0; i < OT_ISP_PREGAMMA_NODE_NUM; i++) {
        if (pregamma_attr->table[i] > OT_ISP_PREGAMMA_LUT_MAX) {
            isp_err_trace("Err %s pregamma table[%d]: %d!\n", src, i, pregamma_attr->table[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 isp_demosaic_auto_attr_check(const char *src, const ot_isp_demosaic_auto_attr *auto_attr)
{
    td_s32 i;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (auto_attr->nddm_mf_detail_strength[i] > OT_ISP_DEMOSAIC_NONDIR_MFDETALEHC_STR_MAX) {
            isp_err_trace("Err %s nddm_mf_detail_strength[%d] %d!\n", src, i, auto_attr->nddm_mf_detail_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->nddm_hf_detail_strength[i] > 0x10) {
            isp_err_trace("Err %s nddm_hf_detail_strength[%d] %d!\n", src, i, auto_attr->nddm_hf_detail_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if ((auto_attr->detail_smooth_range[i] > OT_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MAX) ||
            (auto_attr->detail_smooth_range[i] < OT_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MIN)) {
            isp_err_trace("Err %s detail_smooth_range[%d] %d!\n", src, i, auto_attr->detail_smooth_range[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->color_noise_f_strength[i] > 0x8) {
            isp_err_trace("Err %s color_noise_f_strength[%d] %d!\n", src, i, auto_attr->color_noise_f_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->color_noise_y_threshold[i] > OT_ISP_DEMOSAIC_COLOR_NOISE_THDY_MAX) {
            isp_err_trace("Err %s color_noise_y_threshold[%d] %d!\n", src, i, auto_attr->color_noise_y_threshold[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->color_noise_y_strength[i] > OT_ISP_DEMOSAIC_COLOR_NOISE_STRY_MAX) {
            isp_err_trace("Err %s color_noise_y_strength[%d] %d!\n", src, i, auto_attr->color_noise_y_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 isp_demosaic_manual_attr_check(const char *src, const ot_isp_demosaic_manual_attr *manual_attr)
{
    if (manual_attr->nddm_mf_detail_strength > OT_ISP_DEMOSAIC_NONDIR_MFDETALEHC_STR_MAX) {
        isp_err_trace("Err %s nddm_mf_detail_strength %d!\n", src, manual_attr->nddm_mf_detail_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->nddm_hf_detail_strength > 0x10) {
        isp_err_trace("Err %s nddm_hf_detail_strength %d!\n", src, manual_attr->nddm_hf_detail_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if ((manual_attr->detail_smooth_range > OT_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MAX) ||
        (manual_attr->detail_smooth_range < OT_ISP_DEMOSAIC_DETAIL_SMOOTH_RANGE_MIN)) {
        isp_err_trace("Err %s detail_smooth_range %d!\n", src, manual_attr->detail_smooth_range);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->color_noise_f_strength > 0x8) {
        isp_err_trace("Err %s color_noise_f_strength %d!\n", src, manual_attr->color_noise_f_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->color_noise_y_threshold > OT_ISP_DEMOSAIC_COLOR_NOISE_THDY_MAX) {
        isp_err_trace("Err %s color_noise_y_threshold %d!\n", src, manual_attr->color_noise_y_threshold);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->color_noise_y_strength > OT_ISP_DEMOSAIC_COLOR_NOISE_STRY_MAX) {
        isp_err_trace("Err %s color_noise_y_strength %d!\n", src, manual_attr->color_noise_y_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

td_s32 isp_demosaic_attr_check(const char *src, const ot_isp_demosaic_attr *dem_attr)
{
    td_s32 ret;
    if (dem_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type %d!\n", src, dem_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    ret = isp_demosaic_manual_attr_check(src, &dem_attr->manual_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_demosaic_auto_attr_check(src, &dem_attr->auto_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_lcac_attr_check(const char *src, const ot_isp_local_cac_attr *local_cac_attr)
{
    td_s32 i;

    if (local_cac_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (local_cac_attr->var_threshold > 0xFFF) {
        isp_err_trace("Err %s var_threshold!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (local_cac_attr->purple_detect_range > 410) { /* Range:[0, 410] */
        isp_err_trace("Err %s purple_detect_range!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    for (i = 0; i < OT_ISP_LCAC_DET_NUM; i++) {
        if (local_cac_attr->r_detect_threshold[i] > 4095) { /* Range:[0, 4095] */
            isp_err_trace("Err %s r_detect_threshold[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (local_cac_attr->g_detect_threshold[i] > 4095) { /* Range:[0, 4095] */
            isp_err_trace("Err %s g_detect_threshold[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (local_cac_attr->b_detect_threshold[i] > 4095) { /* Range:[0, 4095] */
            isp_err_trace("Err %s b_detect_threshold[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (local_cac_attr->l_detect_threshold[i] > 4095) { /* Range:[0, 4095] */
            isp_err_trace("Err %s l_detect_threshold[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (local_cac_attr->cb_cr_ratio[i] > 2047 || /* Range:[-2048, 2047] */
                   local_cac_attr->cb_cr_ratio[i] < -2048) { /* Range:[-2048, 2047] */
            isp_err_trace("Err %s cb_cr_ratio[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
        if (local_cac_attr->auto_attr.de_purple_cr_strength[i] > 0x8) {
            isp_err_trace("Err %s depur_cr_strength[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (local_cac_attr->auto_attr.de_purple_cb_strength[i] > 0x8) {
            isp_err_trace("Err %s depur_cb_strength[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    if (local_cac_attr->manual_attr.de_purple_cr_strength > 0x8) {
        isp_err_trace("Err %s depur_cr_strength!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (local_cac_attr->manual_attr.de_purple_cb_strength > 0x8) {
        isp_err_trace("Err %s depur_cb_strength!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}


td_s32 isp_acac_comm_attr_check(const char *src, const ot_isp_acac_attr *acac_attr)
{
    if (acac_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (acac_attr->detect_mode > 1) { /* range : [0:1] */
        isp_err_trace("Err %s detect_mode!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}


td_s32 isp_acac_auto_attr_check(const char *src, const isp_acac_auto_attr *auto_attr)
{
    td_u8 i, j;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (auto_attr->edge_gain[i] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s g_edge_gain[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if ((auto_attr->purple_upper_limit[i] > 511) || /* max is 511 min is -511 */
                   (auto_attr->purple_upper_limit[i] < -511)) { /* max is 511 min is -511 */
            isp_err_trace("Err %s purple_upper_limit[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if ((auto_attr->purple_lower_limit[i] > 511) || /* max is 511 min is -511 */
            (auto_attr->purple_lower_limit[i] < -511)) { /* max is 511 min is -511 */
            isp_err_trace("Err %s purple_lower_limit[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->purple_upper_limit[i] <= auto_attr->purple_lower_limit[i]) {
            isp_err_trace("Err %s purple_upper_limit[%d] should be greater than purple_lower_limit[%d]!\n", src, i, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->purple_sat_threshold[i] > 2047) { /* max is 2047 */
            isp_err_trace("Err %s purple_sat_threshold[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->purple_alpha[i] > 63) { /* max is 63 */
            isp_err_trace("Err %s purple_alpha[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->edge_alpha[i] > 63) { /* max is 63 */
            isp_err_trace("Err %s edge_alpha[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->fcc_rb_strength[i] > 511) { /* max is 511 */
            isp_err_trace("Err %s fcc_rb_strength[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (auto_attr->fcc_y_strength[i] > 4095) { /* max is 4095 */
            isp_err_trace("Err %s fcc_y_strength[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        for (j = 0; j < OT_ISP_ACAC_THR_NUM; j++) {
            if (auto_attr->edge_threshold[j][i] > 4095) { /* max is 4095 */
                isp_err_trace("Err %s invalid edge_threshold[%d][%d]!\n", src, j, i);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }
        if (auto_attr->edge_threshold[0][i] >= auto_attr->edge_threshold[1][i]) {
            isp_err_trace("Err %s edge_threshold[0][%d] can't be greater than edge_threshold[1][%d]!\n", src, i, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

td_s32 isp_acac_manual_attr_check(const char *src, const isp_acac_manual_attr *manual_attr)
{
    td_u8 i;
    if (manual_attr->edge_gain > 1023) { /* max is 1023 */
        isp_err_trace("Err %s g_edge_gain!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if ((manual_attr->purple_upper_limit > 511) || (manual_attr->purple_upper_limit < -511)) { /* r:[-511,511] */
        isp_err_trace("Err %s purple_upper_limit!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if ((manual_attr->purple_lower_limit > 511) || (manual_attr->purple_lower_limit < -511)) { /* r:[-511,511] */
            isp_err_trace("Err %s purple_lower_limit!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->purple_upper_limit <= manual_attr->purple_lower_limit) {
        isp_err_trace("Err %s purple_upper_limit should be greater than purple_lower_limit!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->purple_sat_threshold > 2047) { /* max is 2047 */
        isp_err_trace("Err %s purple_sat_threshold!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->purple_alpha > 63) { /* max is 63 */
        isp_err_trace("Err %s purple_alpha!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->edge_alpha > 63) { /* max is 63 */
        isp_err_trace("Err %s edge_alpha!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->fcc_rb_strength > 511) { /* max is 511 */
        isp_err_trace("Err %s fcc_rb_strength!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (manual_attr->fcc_y_strength > 4095) { /* max is 4095 */
        isp_err_trace("Err %s fcc_y_strength!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    for (i = 0; i < OT_ISP_ACAC_THR_NUM; i++) {
        if (manual_attr->edge_threshold[i] > 4095) { /* max is 4095 */
            isp_err_trace("Err %s edge_threshold[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    if (manual_attr->edge_threshold[0] >= manual_attr->edge_threshold[1]) {
        isp_err_trace("Err %s edge_threshold[0] can't be greater than edge_threshold[1]!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

td_s32 isp_bshp_comm_attr_check(const char *src, const ot_isp_bayershp_attr *bshp_attr)
{
    isp_check_bool_return(bshp_attr->en);

    if (bshp_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s invalid op_type %d!\n", src, bshp_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_bshp_auto_attr_check(const char *src, const ot_isp_bayershp_auto_attr *auto_attr)
{
    td_u16 i, j;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (auto_attr->edge_filt_strength[i] > 127) { /* max is 127 */
            isp_err_trace("Err %s edge_filt_strength[%d] = %d!\n", src, i, auto_attr->edge_filt_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->edge_max_gain[i] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s edge_max_gain[%d]=%d!\n", src, i, auto_attr->edge_max_gain[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->texture_max_gain[i] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s texture_max_gain[%d]=%d!\n", src, i, auto_attr->texture_max_gain[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->overshoot[i] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s overshoot[%d]=%d!\n", src, i, auto_attr->overshoot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->undershoot[i] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s undershoot[%d]=%d!\n", src, i, auto_attr->undershoot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (auto_attr->g_chn_gain[i] > 127) { /* max is 127 */
            isp_err_trace("Err %s g_chn_gain[%d] = %d!\n", src, i, auto_attr->g_chn_gain[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        for (j = 0; j < OT_ISP_BSHP_HALF_CURVE_NUM; j++) {
            if (auto_attr->luma_wgt[j][i] > 31) { /* max is 31 */
                isp_err_trace("Err %s luma_wgt[%d][%d]=%d!\n", src, j, i,
                              auto_attr->luma_wgt[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            } else if (auto_attr->edge_mf_strength[j][i] > 1023) { /* max is 1023 */
                isp_err_trace("Err %s edge_mf_strength[%d][%d] = %d!\n", src, j,
                              i, auto_attr->edge_mf_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            } else if (auto_attr->texture_mf_strength[j][i] > 1023) { /* max is 1023 */
                isp_err_trace("Err %s texture_mf_strength[%d][%d] = %d!\n", src, j,
                              i, auto_attr->texture_mf_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            } else if (auto_attr->edge_hf_strength[j][i] > 1023) { /* max is 1023 */
                isp_err_trace("Err %s edge_hf_strength[%d][%d] = %d!\n", src, j,
                              i, auto_attr->edge_hf_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            } else if (auto_attr->texture_hf_strength[j][i] > 1023) { /* max is 1023 */
                isp_err_trace("Err %s texture_hf_strength[%d][%d] = %d!\n", src, j,
                              i, auto_attr->texture_hf_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }
    }
    return TD_SUCCESS;
}

td_s32 isp_bshp_manual_attr_check(const char *src, const ot_isp_bayershp_manual_attr *manual_attr)
{
    td_u16 j;
    if (manual_attr->edge_filt_strength > 127) { /* max is 127 */
        isp_err_trace("Err %s edge_filt_strength %d!\n", src, manual_attr->edge_filt_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (manual_attr->edge_max_gain > 1023) { /* max is 1023 */
        isp_err_trace("Err %s edge_max_gain %d!\n", src, manual_attr->edge_max_gain);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (manual_attr->texture_max_gain > 1023) { /* max is 1023 */
        isp_err_trace("Err %s texture_max_gain %d!\n", src, manual_attr->texture_max_gain);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (manual_attr->overshoot > 1023) { /* max is 1023 */
        isp_err_trace("Err %s overshoot %d!\n", src, manual_attr->overshoot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (manual_attr->undershoot > 1023) { /* max is 1023 */
        isp_err_trace("Err %s undershoot %d!\n", src, manual_attr->undershoot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    } else if (manual_attr->g_chn_gain > 127) { /* max is 127 */
        isp_err_trace("Err %s g_chn_gain %d!\n", src, manual_attr->g_chn_gain);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    for (j = 0; j < OT_ISP_BSHP_HALF_CURVE_NUM; j++) {
        if (manual_attr->luma_wgt[j] > 31) { /* max is 31 */
            isp_err_trace("Err %s luma_wgt[%d] = %d!\n", src, j, manual_attr->luma_wgt[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (manual_attr->edge_mf_strength[j] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s edge_mf_strength[%d] = %d!\n", src, j,
                          manual_attr->edge_mf_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (manual_attr->texture_mf_strength[j] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s texture_mf_strength[%d] = %d!\n", src, j,
                          manual_attr->texture_mf_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (manual_attr->edge_hf_strength[j] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s edge_hf_strength %d!\n", src, manual_attr->edge_hf_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        } else if (manual_attr->texture_hf_strength[j] > 1023) { /* max is 1023 */
            isp_err_trace("Err %s texture_hf_strength %d!\n", src,
                          manual_attr->texture_hf_strength[j]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 isp_rgbir_cfg_check(const char *src, const ot_isp_rgbir_attr *rgbir_attr)
{
    td_u8 i;

    isp_check_bool_return(rgbir_attr->auto_gain_en);
    isp_check_bool_return(rgbir_attr->smooth_en);
    isp_check_bool_return(rgbir_attr->ir_rm_en);

    if (rgbir_attr->auto_gain > 1023) { /* range:[0, 1023] */
        isp_err_trace("Err %s auto_gain!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_RGBIR_CTRL_NUM; i++) {
        if (rgbir_attr->exp_ctrl[i] > 2047) { /* range:[0, 2047] */
            isp_err_trace("Err %s exp_ctrl[%d]\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (rgbir_attr->exp_gain[i] > 511) { /* range:[0, 511] */
            isp_err_trace("Err %s exp_gain[%d]\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (rgbir_attr->wb_ctrl_strength[i] > 1024) { /* range:[0, 1024] */
            isp_err_trace("Err %s wb_ctrl_strength[%d]\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_RGBIR_CROSSTALK_NUM; i++) {
        if (rgbir_attr->ir_rm_ratio[i] > 1000) { /* range:[0, 1000] */
            isp_err_trace("Err %s ir_rm_ratio[%d]\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    if (rgbir_attr->ir_cvtmat_mode >= OT_ISP_IR_CVTMAT_MODE_BUTT) {
        isp_err_trace("Err %s ir_cvtmat_mode!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_RGBIR_CVTMAT_NUM; i++) {
        if (rgbir_attr->cvt_matrix[i] < -16384 || rgbir_attr->cvt_matrix[i] > 16383) { /* [-16384, 16383] */
            isp_err_trace("Err %s cvt_matrix[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_rgbir_attr_check(const char *src, ot_vi_pipe vi_pipe, const ot_isp_rgbir_attr *rgbir_attr)
{
    td_s32 ret;
    ot_wdr_mode wdr_mode;

    isp_check_bool_return(rgbir_attr->rgbir_en);
    isp_check_bool_return(rgbir_attr->rgbir_cfg.is_ir_upscale);

    if (rgbir_attr->rgbir_en == TD_TRUE && rgbir_attr->rgbir_cfg.mode == OT_ISP_RGBIR_MODE_NORMAL) {
        isp_err_trace("RGBIR can not support OT_ISP_RGBIR_MODE_NORMAL mode.\n");
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    wdr_mode = ot_ext_system_sensor_wdr_mode_read(vi_pipe);
    if ((rgbir_attr->rgbir_en == TD_TRUE) && (is_wdr_mode(wdr_mode))) {
        isp_err_trace("RGBIR can not open in WDR mode\n");
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (rgbir_attr->rgbir_cfg.mode >= OT_ISP_RGBIR_MODE_BUTT) {
        isp_err_trace("Err %s rgbir mode %d!\n", src, rgbir_attr->rgbir_cfg.mode);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (rgbir_attr->rgbir_cfg.out_pattern >= OT_ISP_BAYER_BUTT) {
        isp_err_trace("Err %s out_pattern!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (rgbir_attr->rgbir_cfg.in_rgbir_pattern >= OT_ISP_IRBAYER_BUTT) {
        isp_err_trace("Err %s in_rgbir_pattern!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (rgbir_attr->rgbir_cfg.in_bayer_pattern >= OT_ISP_BAYER_BUTT) {
        isp_err_trace("Err %s in_bayer_pattern!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    ret = isp_rgbir_cfg_check(src, rgbir_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 isp_nr_snr_auto_attr_check(const char *src, const ot_isp_nr_snr_auto_attr *snr_auto)
{
    td_u8 i, j;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (snr_auto->sfm0_mode[i] > OT_ISP_BNR_NORM) {
            isp_err_trace("Err %s sfm0_mode %d!\n", src, snr_auto->sfm0_mode[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            if (snr_auto->sfm0_coarse_strength[j][i] > 864) { /* Range:[0, 864] */
                isp_err_trace("Err %s sfm0_coarse_strength[%d][%d] %d!\n", src, j, i,
                    snr_auto->sfm0_coarse_strength[j][i]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }

        if (snr_auto->sfm0_ex_strength[i] > 16) { /* Range:[0, 16] */
            isp_err_trace("Err %s sfm0_ex_strength %d!\n", src, snr_auto->sfm0_ex_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (snr_auto->sfm0_ex_detail_prot[i] > 31) { /* Range:[0, 31] */
            isp_err_trace("Err %s sfm0_ex_detail_prot %d!\n", src, snr_auto->sfm0_ex_detail_prot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (snr_auto->sfm0_norm_edge_strength[i] > 31) { /* Range:[0, 31] */
            isp_err_trace("Err %s sfm0_norm_edge_strength %d!\n", src, snr_auto->sfm0_norm_edge_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (snr_auto->sfm1_detail_prot[i] > 31) { /* Range:[0, 31] */
            isp_err_trace("Err %s sfm1_detail_prot %d!\n", src, snr_auto->sfm1_detail_prot[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (snr_auto->sfm1_coarse_strength[i] > 63) { /* Range:[0, 63] */
            isp_err_trace("Err %s sfm1_coarse_strength %d!\n", src, snr_auto->sfm1_coarse_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (snr_auto->fine_strength[i] > 128) { /* Range:[0, 128] */
            isp_err_trace("Err %s fine_strength %d!\n", src, snr_auto->fine_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (snr_auto->coring_wgt[i] > 3200) { /* Range:[0, 3200] */
            isp_err_trace("Err %s coring_wgt %d!\n", src, snr_auto->coring_wgt[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_nr_snr_manual_attr_check(const char *src, const ot_isp_nr_snr_manual_attr *snr_manual)
{
    td_u8 j;
    if (snr_manual->sfm0_mode > OT_ISP_BNR_NORM) {
        isp_err_trace("Err %s sfm0_mode %d!\n", src, snr_manual->sfm0_mode);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
        if (snr_manual->sfm0_coarse_strength[j] > 864) { /* Range:[0, 864] */
            isp_err_trace("Err %s sfm0_coarse_strength[%d] %d!\n", src, j, snr_manual->sfm0_coarse_strength[0]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    if (snr_manual->sfm0_ex_strength > 16) { /* Range:[0, 16] */
        isp_err_trace("Err %s sfm0_ex_strength %d!\n", src, snr_manual->sfm0_ex_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snr_manual->sfm0_ex_detail_prot > 31) { /* Range:[0, 31] */
        isp_err_trace("Err %s sfm0_ex_detail_prot %d!\n", src, snr_manual->sfm0_ex_detail_prot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snr_manual->sfm0_norm_edge_strength > 31) { /* Range:[0, 31] */
        isp_err_trace("Err %s sfm0_norm_edge_strength %d!\n", src, snr_manual->sfm0_norm_edge_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snr_manual->sfm1_detail_prot > 31) { /* Range:[0, 31] */
        isp_err_trace("Err %s sfm1_detail_prot %d!\n", src, snr_manual->sfm1_detail_prot);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snr_manual->sfm1_coarse_strength > 63) { /* Range:[0, 63] */
        isp_err_trace("Err %s sfm1_coarse_strength %d!\n", src, snr_manual->sfm1_coarse_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snr_manual->fine_strength > 128) { /* Range:[0, 128] */
        isp_err_trace("Err %s fine_strength %d!\n", src, snr_manual->fine_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snr_manual->coring_wgt > 3200) { /* Range:[0, 3200] */
        isp_err_trace("Err %s coring_wgt %d!\n", src, snr_manual->coring_wgt);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

static td_s32 isp_nr_tnr_auto_attr_check(const char *src, const ot_isp_nr_tnr_auto_attr *tnr_auto)
{
    td_u8 i;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (tnr_auto->md_mode[i] > 1) { /* Range:[0, 1] */
            isp_err_trace("Err %s md_mode %d!\n", src, tnr_auto->md_mode[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->md_anti_flicker_strength[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s md_anti_flicker_strength %d!\n", src, tnr_auto->md_anti_flicker_strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->md_static_ratio[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s md_static_ratio %d!\n", src, tnr_auto->md_static_ratio[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->tss[i] > 128) { /* Range:[0, 128] */
            isp_err_trace("Err %s tss %d!\n", src, tnr_auto->tss[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->user_define_md[i] > 1) { /* Range:[0, 1] */
            isp_err_trace("Err %s user_define_md %d!\n", src, tnr_auto->user_define_md[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->user_define_color_thresh[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s user_define_color_thresh %d!\n", src, tnr_auto->user_define_color_thresh[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->sfr_r[i] > 128) { /* Range:[0, 128] */
            isp_err_trace("Err %s sfr_r %d!\n", src, tnr_auto->sfr_r[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->sfr_g[i] > 128) { /* Range:[0, 128] */
            isp_err_trace("Err %s sfr_g %d!\n", src, tnr_auto->sfr_g[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (tnr_auto->sfr_b[i] > 128) { /* Range:[0, 128] */
            isp_err_trace("Err %s sfr_b %d!\n", src, tnr_auto->sfr_b[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 isp_nr_tnr_manual_attr_check(const char *src, const ot_isp_nr_tnr_manual_attr *tnr_manual)
{
    if (tnr_manual->md_mode > 1) { /* Range:[0, 1] */
        isp_err_trace("Err %s md_mode %d!\n", src, tnr_manual->md_mode);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->md_anti_flicker_strength > 64) { /* Range:[0, 64] */
        isp_err_trace("Err %s md_anti_flicker_strength %d!\n", src, tnr_manual->md_anti_flicker_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->md_static_ratio > 64) { /* Range:[0, 64] */
        isp_err_trace("Err %s md_static_ratio %d!\n", src, tnr_manual->md_static_ratio);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->tss > 128) { /* Range:[0, 128] */
        isp_err_trace("Err %s tss %d!\n", src, tnr_manual->tss);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->user_define_md > 1) { /* Range:[0, 1] */
        isp_err_trace("Err %s user_define_md %d!\n", src, tnr_manual->user_define_md);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->user_define_color_thresh > 64) { /* Range:[0, 64] */
        isp_err_trace("Err %s user_define_color_thresh %d!\n", src, tnr_manual->user_define_color_thresh);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->sfr_r > 128) { /* Range:[0, 128] */
        isp_err_trace("Err %s sfr_r %d!\n", src, tnr_manual->sfr_r);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->sfr_g > 128) { /* Range:[0, 128] */
        isp_err_trace("Err %s sfr_g %d!\n", src, tnr_manual->sfr_g);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (tnr_manual->sfr_b > 128) { /* Range:[0, 128] */
        isp_err_trace("Err %s sfr_b %d!\n", src, tnr_manual->sfr_b);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_nr_snr_attr_check(const char *src, const ot_isp_nr_snr_attr *snr_cfg)
{
    td_s32 ret;

    ret = isp_nr_snr_auto_attr_check(src, &snr_cfg->snr_auto);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_nr_snr_manual_attr_check(src, &snr_cfg->snr_manual);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

td_s32 isp_nr_tnr_attr_check(const char *src, const ot_isp_nr_tnr_attr *tnr_cfg)
{
    td_s32 ret;

    ret = isp_nr_tnr_auto_attr_check(src, &tnr_cfg->tnr_auto);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_nr_tnr_manual_attr_check(src, &tnr_cfg->tnr_manual);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

td_s32 isp_nr_comm_attr_check(const char *src, const ot_isp_nr_attr *nr_attr)
{
    td_u8 i;

    isp_check_bool_return(nr_attr->en);
    isp_check_bool_return(nr_attr->tnr_en);
    isp_check_bool_return(nr_attr->lsc_nr_en);

    if (nr_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s NR type %d!\n", src, nr_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (nr_attr->lsc_ratio1 > 15) { /* Range:[0, 15] */
        isp_err_trace("Err %s lsc_ratio1 %d!\n", src, nr_attr->lsc_ratio1);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_BAYERNR_LUT_LENGTH; i++) {
        if (nr_attr->coring_ratio[i] > 0x3ff) {
            isp_err_trace("Err %s coring_ratio %d!\n", src, nr_attr->coring_ratio[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

td_s32 isp_nr_wdr_attr_check(const char *src, const ot_isp_nr_wdr_attr *wdr_cfg)
{
    td_u8 i;
    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        if (wdr_cfg->snr_sfm0_wdr_strength[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s snr_sfm0_wdr_strength!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (wdr_cfg->snr_sfm0_fusion_strength[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s snr_sfm0_fusion_strength!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (wdr_cfg->md_wdr_strength[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s md_wdr_strength!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (wdr_cfg->md_fusion_strength[i] > 64) { /* Range:[0, 64] */
            isp_err_trace("Err %s md_fusion_strength!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_ldci_attr_check(const char *src, const ot_isp_ldci_attr *ldci_attr)
{
    td_u8 i;
    isp_check_bool_return(ldci_attr->en);

    if (ldci_attr->gauss_lpf_sigma < 0x1) {
        isp_err_trace("Err %s gauss_lpf_sigma!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (ldci_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (ldci_attr->auto_attr.he_wgt[i].he_pos_wgt.sigma < 0x1) {
            isp_err_trace("Err %s he_wgt.he_pos_wgt.sigma[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (ldci_attr->auto_attr.he_wgt[i].he_neg_wgt.sigma < 0x1) {
            isp_err_trace("Err %s he_wgt.he_neg_wgt.sigma[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (ldci_attr->auto_attr.blc_ctrl[i] > 0x1ff) {
            isp_err_trace("Err %s blc_ctrl[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    if (ldci_attr->manual_attr.he_wgt.he_pos_wgt.sigma < 0x1) {
        isp_err_trace("Err %s he_wgt.he_pos_wgt.sigma!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (ldci_attr->manual_attr.he_wgt.he_neg_wgt.sigma < 0x1) {
        isp_err_trace("Err %s he_wgt.he_neg_wgt.sigma!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (ldci_attr->manual_attr.blc_ctrl > 0x1ff) {
        isp_err_trace("Err %s blc_ctrl!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (ldci_attr->tpr_incr_coef > 0x100) {
        isp_err_trace("Err %s tpr_incr_coef!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (ldci_attr->tpr_decr_coef > 0x100) {
        isp_err_trace("Err %s tpr_decr_coef!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 isp_fswdr_combine_attr_check_inner(const char *src, const ot_isp_wdr_combine_attr *wdr_combine)
{
    td_u8 i, j;
    for (i = 0; i < OT_ISP_WDR_RATIO_NUM; i++) {
        for (j = 0; j < OT_ISP_AUTO_ISO_NUM; j++) {
            if (wdr_combine->wdr_mdt.auto_attr.md_thr_low_gain[i][j] >
                wdr_combine->wdr_mdt.auto_attr.md_thr_hig_gain[i][j]) {
                isp_err_trace("Err %s: auto: md_thr_low_gain  should NOT be larger than md_thr_hig_gain!\n", src);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }
    }
    if (wdr_combine->wdr_mdt.mdt_full_threshold > 0xFE) {
        isp_err_trace("Err %s mdt_full_thd!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->wdr_mdt.mdt_still_threshold > 0xFE) {
        isp_err_trace("Err %s mdt_still_thd!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->wdr_mdt.mdt_long_blend > 0xFE) {
        isp_err_trace("Err %s mdt_long_blend!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}


static td_s32 isp_fswdr_combine_attr_check(const char *src, const ot_isp_wdr_combine_attr *wdr_combine)
{
    td_s32 ret;
    if (wdr_combine->short_threshold > 0xFFF) {
        isp_err_trace("Err %s short_thr!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->long_threshold > 0xFFF) {
        isp_err_trace("Err %s long_thr!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->long_threshold > wdr_combine->short_threshold) {
        isp_err_trace("Err %s: long_thresh should NOT be larger than short_thresh !\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->wdr_mdt.op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->wdr_mdt.manual_attr.md_thr_low_gain > wdr_combine->wdr_mdt.manual_attr.md_thr_hig_gain) {
        isp_err_trace("Err %s: md_thr_low_gain should NOT be larger than md_thr_hig_gain!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->force_long_hig_threshold > 0XFFF) {
        isp_err_trace("Err %s force_long_hig_thr!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (wdr_combine->force_long_low_threshold > wdr_combine->force_long_hig_threshold) {
        isp_err_trace("Err %s: force_long_low_thr should NOT be larger than force_long_hig_thr!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    ret = isp_fswdr_combine_attr_check_inner(src, wdr_combine);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_fswdr_attr_check(const char *src, const ot_isp_wdr_fs_attr *fswdr_attr)
{
    td_u8 j;
    td_s32 ret;
    isp_check_bool_return(fswdr_attr->wdr_combine.motion_comp);
    isp_check_bool_return(fswdr_attr->wdr_combine.wdr_mdt.md_ref_flicker);
    isp_check_bool_return(fswdr_attr->wdr_combine.force_long);
    isp_check_bool_return(fswdr_attr->wdr_combine.wdr_mdt.short_expo_chk);

    ret = isp_fswdr_combine_attr_check(src, &fswdr_attr->wdr_combine);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if (fswdr_attr->wdr_merge_mode >= OT_ISP_MERGE_BUTT) {
        isp_err_trace("Err %s wdr_merge_mode!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (j = 0; j < OT_ISP_WDR_MAX_FRAME_NUM; j++) {
        if (fswdr_attr->fusion_attr.fusion_threshold[j] > 0x3FFF) {
            isp_err_trace("Err %s fusion_thr!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}
td_s32 isp_dehaze_attr_check(const char *src, const ot_isp_dehaze_attr *dehaze_attr)
{
    isp_check_bool_return(dehaze_attr->en);
    isp_check_bool_return(dehaze_attr->user_lut_en);

    if (dehaze_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (dehaze_attr->tmprflt_decr_coef > 0x80) {
        isp_err_trace("Err %s tmprflt_decr_coef\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (dehaze_attr->tmprflt_incr_coef > 0x80) {
        isp_err_trace("Err %s tmprflt_incr_coef\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_ca_attr_check(const char *src, const ot_isp_ca_attr *ca_attr)
{
    td_u16 i;
    isp_check_bool_return(ca_attr->en);
    if (ca_attr->ca_cp_en >= OT_ISP_CA_BUTT) {
        isp_err_trace("Err ca type %d!\n", ca_attr->ca_cp_en);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    for (i = 0; i < OT_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        if (ca_attr->ca.y_ratio_lut[i] > OT_ISP_CA_RATIO_MAX) {
            isp_err_trace("Err %s y_ratio_lut[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (ca_attr->ca.iso_ratio[i] > OT_ISP_CA_RATIO_MAX) {
            isp_err_trace("Err %s iso_ratio[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

td_s32 isp_clut_attr_check(const char *src, const ot_isp_clut_attr *clut_attr)
{
    isp_check_bool_return(clut_attr->en);
    if (clut_attr->gain_r > OT_ISP_CLUT_GAIN_MAX) {
        isp_err_trace("Err %s gain_r %d!\n", src, clut_attr->gain_r);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (clut_attr->gain_g > OT_ISP_CLUT_GAIN_MAX) {
        isp_err_trace("Err %s gain_g %d!\n", src, clut_attr->gain_g);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (clut_attr->gain_b > OT_ISP_CLUT_GAIN_MAX) {
        isp_err_trace("Err %s gain_b %d!\n", src, clut_attr->gain_b);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

td_s32 isp_crb_attr_check(const char *src, const ot_isp_crb_attr *crb_attr)
{
    td_u16 i;
    isp_check_bool_return(crb_attr->en);
    if (crb_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err crb type %d!\n", crb_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    for (i = 0; i < OT_ISP_WDR_RATIO_NUM; i++) {
        if (crb_attr->auto_attr.r_gain_limit[i] > OT_ISP_CRB_GAIN_MAX ||
            crb_attr->auto_attr.r_gain_limit[i] < OT_ISP_CRB_GAIN_MIN) {
            isp_err_trace("Err %s r_gain_limit[%d] %d!\n", src, i, crb_attr->auto_attr.r_gain_limit[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (crb_attr->auto_attr.b_gain_limit[i] > OT_ISP_CRB_GAIN_MAX ||
            crb_attr->auto_attr.b_gain_limit[i] < OT_ISP_CRB_GAIN_MIN) {
            isp_err_trace("Err %s b_gain_limit[%d] %d!\n", src, i, crb_attr->auto_attr.b_gain_limit[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    if (crb_attr->manual_attr.r_gain_limit > OT_ISP_CRB_GAIN_MAX ||
        crb_attr->manual_attr.r_gain_limit < OT_ISP_CRB_GAIN_MIN) {
        isp_err_trace("Err %s r_gain_limit %d!\n", src, crb_attr->manual_attr.r_gain_limit);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (crb_attr->manual_attr.b_gain_limit > OT_ISP_CRB_GAIN_MAX ||
        crb_attr->manual_attr.b_gain_limit < OT_ISP_CRB_GAIN_MIN) {
        isp_err_trace("Err %s b_gain_limit %d!\n", src, crb_attr->manual_attr.b_gain_limit);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_anti_false_color_attr_check(const char *src, const ot_isp_anti_false_color_attr *anti_false_color)
{
    td_u32 i;
    isp_check_bool_return(anti_false_color->en);
    if (anti_false_color->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type %d!\n", src, anti_false_color->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (anti_false_color->auto_attr.threshold[i] > 0x20) {
            isp_err_trace("Err %s auto threshold %d!\n", src, anti_false_color->auto_attr.threshold[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (anti_false_color->auto_attr.strength[i] > 0x1f) {
            isp_err_trace("Err %s auto strength %d!\n", src, anti_false_color->auto_attr.strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    if (anti_false_color->manual_attr.threshold > 0x20) {
        isp_err_trace("Err %s manual threshold %d!\n", src, anti_false_color->manual_attr.threshold);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (anti_false_color->manual_attr.strength > 0x1f) {
        isp_err_trace("Err %s manual strength %d!\n", src, anti_false_color->manual_attr.strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 isp_dynamic_blc_attr_check_part2(const char *src, const ot_isp_black_level_dynamic_attr *dynamic_attr)
{
    td_u8 i;
    isp_check_bool_return(dynamic_attr->separate_en);

    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (dynamic_attr->offset[i] > 0x3FFF || dynamic_attr->offset[i] < -0x3FFF) {
            isp_err_trace("Err %s dynamicblc offset %d!\n", src, dynamic_attr->offset[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (dynamic_attr->calibration_black_level[i] > 0x3FFF) {
            isp_err_trace("Err %s ag cali dynamicblc %d!\n", src, dynamic_attr->calibration_black_level[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    if (dynamic_attr->tolerance > 0x3FFF) {
        isp_err_trace("Err %s dynamicblc tolerance %d!\n", src, dynamic_attr->tolerance);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (dynamic_attr->filter_strength > 0x40) {
        isp_err_trace("Err %s dynamicblc filter_strength %d!\n", src, dynamic_attr->filter_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (dynamic_attr->filter_thr > 0x3FFF) {
        isp_err_trace("Err %s dynamicblc filter_thr %d!\n", src, dynamic_attr->filter_thr);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 isp_dynamic_blc_ob_area_check(ot_vi_pipe vi_pipe, const char *src, const ot_rect *ob_area)
{
    td_u16 w, h;
    td_s32 ret;
    ot_size fe_in_size = { 0 };

    ret = ioctl(isp_get_fd(vi_pipe), ISP_GET_PIPE_SIZE, &fe_in_size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] get pipe size failed\n", vi_pipe);
        return ret;
    }

    w = fe_in_size.width;
    h = fe_in_size.height;

    if (ob_area->x % 2 != 0 || ob_area->x >= w || /* 2 is num */
        ob_area->x < 0) { /* 0 is num */
        isp_err_trace("Err %s x %d!\n", src, ob_area->x);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (ob_area->y % 2 != 0 || ob_area->y >= h || /* 2 is num */
        ob_area->y < 0) { /* 0 is num */
        isp_err_trace("Err %s y %d!\n", src, ob_area->y);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((ob_area->y + ob_area->height) > h ||
        ob_area->height % 2 != 0x0) { /* 2 is num */
        isp_err_trace("Err %s height %d!\n", src, ob_area->height);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((ob_area->width + ob_area->x) > w ||
        ob_area->width % 2 != 0x0) { /* 2 is num */
        isp_err_trace("Err %s width %d!\n", src, ob_area->width);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 isp_dynamic_blc_attr_check(ot_vi_pipe vi_pipe, const char *src,
    const ot_isp_black_level_dynamic_attr *dynamic_attr, ot_isp_black_level_mode black_level_mode)
{
    td_u8  wdr_mode;
    td_s32 ret;
    wdr_mode  = ot_ext_system_sensor_wdr_mode_read(vi_pipe);

    if (black_level_mode != OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
        return TD_SUCCESS;
    }

    if (is_virt_pipe(vi_pipe) && (black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC)) {
        isp_err_trace("ISP[%d]: Not Support dynamic blc when virtual pipe!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if (is_built_in_wdr_mode(wdr_mode) && (black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC)) {
        isp_err_trace("ISP[%d]: Not Support dynamic blc in sensor built-in mode!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if (is_virt_pipe(vi_pipe)) {
        return TD_SUCCESS;
    }

    if (dynamic_attr->high_threshold > 0x3FFF) {
        isp_err_trace("Err %s high_threshold %d!\n", src, dynamic_attr->high_threshold);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (dynamic_attr->low_threshold >  0xFFF || dynamic_attr->low_threshold >= dynamic_attr->high_threshold) {
        isp_err_trace("Err %s low_threshold %d!\n", src, dynamic_attr->low_threshold);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    ret = isp_dynamic_blc_ob_area_check(vi_pipe, src, &dynamic_attr->ob_area);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (dynamic_attr->pattern >= OT_ISP_DYNAMIC_PATTERN_BUTT) {
        isp_err_trace("Err %s dynamicblc pattern %d!\n", src, dynamic_attr->pattern);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    ret = isp_dynamic_blc_attr_check_part2(src, dynamic_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_black_level_value_check(const char *src, const td_u16 (*black_level)[OT_ISP_BAYER_CHN_NUM])
{
    td_u8 i, j;

    isp_check_pointer_return(black_level);
    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            if (black_level[i][j] > 0x3FFF) {
                isp_err_trace("Err %s black_level[%d][%d]:%d!\n", src, i, j, black_level[i][j]);
                return OT_ERR_ISP_ILLEGAL_PARAM;
            }
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_user_black_level_en_check(ot_vi_pipe vi_pipe, const td_bool user_black_level_en)
{
    td_u8  wdr_mode;
    isp_check_bool_return(user_black_level_en);
    if (is_virt_pipe(vi_pipe) && (user_black_level_en == TD_TRUE)) {
        isp_err_trace("ISP[%d]: Not Support user blc when virtual pipe!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }
    wdr_mode  = ot_ext_system_sensor_wdr_mode_read(vi_pipe);
    if (is_built_in_wdr_mode(wdr_mode) && (user_black_level_en == TD_TRUE)) {
        isp_err_trace("ISP[%d]: Not Support user blc in sensor built-in mode!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    return TD_SUCCESS;
}

td_s32 isp_crosstalk_attr_check(const char *src, const ot_isp_cr_attr *cr_attr)
{
    td_u8 i;
    isp_check_bool_return(cr_attr->en);
    if (cr_attr->slope > OT_ISP_CR_SLOPE_MAX) {
        isp_err_trace("Err %s slope %d!\n", src, cr_attr->slope);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (cr_attr->sensi_slope > OT_ISP_CR_SLOPE_MAX) {
        isp_err_trace("Err %s sensi_slope%d!\n", src, cr_attr->sensi_slope);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (cr_attr->sensi_threshold > OT_ISP_CR_THR_MAX) {
        isp_err_trace("Err %s sensi_threshold %d!\n", src, cr_attr->sensi_threshold);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (cr_attr->strength[i] > 0x100) {
            isp_err_trace("Err %s strength %d!\n", src, cr_attr->strength[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (cr_attr->np_offset[i] > OT_ISP_CR_NPOFFSET_MAX || cr_attr->np_offset[i] < OT_ISP_CR_NPOFFSET_MIN) {
            isp_err_trace("Err %s np_offset %d!\n", src, cr_attr->np_offset[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if (cr_attr->threshold[i] > OT_ISP_CR_THR_MAX) {
            isp_err_trace("Err %s threshold %d!\n", src, cr_attr->threshold[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 isp_mesh_shading_grid_width_check(const char *src, ot_vi_pipe vi_pipe,
    const ot_isp_shading_lut_attr *lut_attr)
{
    td_u16 i;
    td_u32 width, height;
    td_u32 x_sum = 0;
    td_u32 y_sum = 0;

    width  = ot_ext_system_be_total_width_read(vi_pipe);
    height = ot_ext_system_be_total_height_read(vi_pipe);

    for (i = 0; i < OT_ISP_MLSC_X_HALF_GRID_NUM; i++) {
        x_sum += lut_attr->x_grid_width[i];
    }

    for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
        y_sum += lut_attr->y_grid_width[i];
    }

    if ((x_sum != (width / 4)) || (y_sum != (height / 4))) {  /* x_sum(y_sum)should be wdith/4(height/4) */
        isp_err_trace("Err %s block size x_sum = %d, y_sum = %d!\n", src, x_sum, y_sum);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_MLSC_X_HALF_GRID_NUM; i++) {
        if ((lut_attr->x_grid_width[i] > ot_isp_lsc_xgrid_width_max(width)) ||
            (lut_attr->x_grid_width[i] < 0x4)) {
            isp_err_trace("Err %s x_grid_width[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
        if ((lut_attr->y_grid_width[i] > ot_isp_lsc_ygrid_width_max(height)) ||
            (lut_attr->y_grid_width[i] < 0x4)) {
            isp_err_trace("Err %s y_grid_width[%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_mesh_shading_gain_lut_attr_check(const char *src, ot_vi_pipe vi_pipe,
    const ot_isp_shading_lut_attr *lut_attr)
{
    td_u16 i;
    if (lut_attr->mesh_scale > (OT_ISP_LSC_MESHSCALE_NUM - 1)) {
        isp_err_trace("Err %s mesh_scale!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
        if ((lut_attr->lsc_gain_lut[0].r_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[0].gr_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[0].gb_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[0].b_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[1].r_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[1].gr_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[1].gb_gain[i] > OT_ISP_LSC_MAX_GAIN) ||
            (lut_attr->lsc_gain_lut[1].b_gain[i] > OT_ISP_LSC_MAX_GAIN)) {
            isp_err_trace("Err %s gain!\n", src);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return isp_mesh_shading_grid_width_check(src, vi_pipe, lut_attr);
}

td_s32 isp_expander_attr_check(const char *src, ot_vi_pipe vi_pipe, const ot_isp_expander_attr *expander_attr)
{
    td_u8  wdr_mode = ot_ext_system_sensor_wdr_mode_read(vi_pipe);
    td_u16 i;
    isp_check_pointer_return(expander_attr);
    isp_check_bool_return(expander_attr->en);

    if ((expander_attr->en == TD_TRUE) && (is_built_in_wdr_mode(wdr_mode) == TD_FALSE)) {
        isp_err_trace("Only Support enable expander in sensor built-in mode!\n");
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if ((expander_attr->bit_depth_in > 0x14) || (expander_attr->bit_depth_in < 0xC) ||
        (expander_attr->bit_depth_in % 2 != 0)) { /* bit_depth_in must be a multiple of 2 */
        isp_err_trace("Err %s bit_depth_in!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((expander_attr->bit_depth_out > 0x14) || (expander_attr->bit_depth_out < 0xC) ||
        (expander_attr->bit_depth_out % 2 != 0)) { /* bit_depth_out must be a multiple of 2 */
        isp_err_trace("Err %s bit_depth_out!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((expander_attr->knee_point_num > 256) || (expander_attr->knee_point_num < 1)) { /* Range:[1, 256] */
        isp_err_trace("Err %s knee_point_num!\n", src);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < expander_attr->knee_point_num; i++) {
        if ((expander_attr->knee_point_coord[i].x > 0x100) || (expander_attr->knee_point_coord[i].x < 0)) {
            isp_err_trace("Err %s knee_point_coord[%d].x!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }

        if ((expander_attr->knee_point_coord[i].y > 0x100000) || (expander_attr->knee_point_coord[i].y < 0)) {
            isp_err_trace("Err %s knee_point_coord[%d].y!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    for (i = 1; i < expander_attr->knee_point_num; i++) {
        if ((expander_attr->knee_point_coord[i].x < expander_attr->knee_point_coord[i - 1].x) ||
            (expander_attr->knee_point_coord[i].y < expander_attr->knee_point_coord[i - 1].y)) {
            isp_err_trace("The knee_point_coord must be monotonically increasing\n");
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_dp_dynamic_attr_check(const char *src, ot_vi_pipe vi_pipe, const ot_isp_dp_dynamic_attr *dp_dynamic_attr)
{
    td_u8 i;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(dp_dynamic_attr);
    isp_check_bool_return(dp_dynamic_attr->enable);
    isp_check_bool_return(dp_dynamic_attr->sup_twinkle_en);
    isp_check_open_return(vi_pipe);
    isp_check_mem_init_return(vi_pipe);
    isp_check_dpc_init_return(vi_pipe);

    if (dp_dynamic_attr->op_type >= OT_OP_MODE_BUTT) {
        isp_err_trace("Err %s op_type %d!\n", src, dp_dynamic_attr->op_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        if (dp_dynamic_attr->auto_attr.strength[i] > 0xFF) {
            isp_err_trace("Err %s strength [%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        if (dp_dynamic_attr->auto_attr.blend_ratio[i] > 0x80) {
            isp_err_trace("Err %s blend_ratio [%d]!\n", src, i);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    if (dp_dynamic_attr->manual_attr.strength > 0xFF) {
        isp_err_trace("Err %s manual strength %d!\n", src, dp_dynamic_attr->manual_attr.strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (dp_dynamic_attr->manual_attr.blend_ratio > 0x80) {
        isp_err_trace("Err %s manual blend_ratio %d!\n", src, dp_dynamic_attr->manual_attr.blend_ratio);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (dp_dynamic_attr->soft_thr < 0x0) {
        isp_err_trace("Err %s soft_thr %d!\n", src, dp_dynamic_attr->soft_thr);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (dp_dynamic_attr->bright_strength >= 0x80) {
        isp_err_trace("Err %s bright strength %d!\n", src, dp_dynamic_attr->bright_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (dp_dynamic_attr->dark_strength >= 0x80) {
        isp_err_trace("Err %s dark strength %d!\n", src, dp_dynamic_attr->dark_strength);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}
