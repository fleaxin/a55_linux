/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_alg.h"
#include "isp_config.h"
#include "isp_ext_config.h"
#include "isp_math_utils.h"
#include "isp_proc.h"
#include "isp_param_check.h"


/* Lobal CAC,default value */
#define ISP_LCAC_LUMA_THR_R_WDR                  1500
#define ISP_LCAC_LUMA_THR_G_WDR                  1500
#define ISP_LCAC_LUMA_THR_B_WDR                  2150

#define ISP_LCAC_LUMA_THR_R_LINEAR               1500
#define ISP_LCAC_LUMA_THR_G_LINEAR               1500
#define ISP_LCAC_LUMA_THR_B_LINEAR               3500

#define LCAC_DEPURPLE_CB_STR_LIN    3 /* [0,8] */
#define LCAC_DEPURPLE_CR_STR_LIN    0 /* [0,8] */

#define LCAC_DEPURPLE_CB_STR_WDR       7 /* [0,8] */
#define LCAC_DEPURPLE_CR_STR_WDR       0 /* [0,8] */

#define ISP_LCAC_CRCB_RATIO_HIGH_LIMIT_LINEAR    292
#define ISP_LCAC_CRCB_RATIO_LOW_LIMIT_LINEAR     (-50)

#define ISP_LCAC_CRCB_RATIO_HIGH_LIMIT_WDR       292
#define ISP_LCAC_CRCB_RATIO_LOW_LIMIT_WDR        (-50)

#define ISP_LCAC_PURPLE_DET_RANGE_DEFAULT_LINEAR 60
#define ISP_LCAC_PURPLE_DET_RANGE_DEFAULT_WDR    60

#define ISP_LCAC_MAX_STRENGTH                    8
#define RANGE_MAX_VALUE                          3

#define MIN_EXP_RATIO                            64
#define MAX_EXP_RATIO                            16384

static const td_u32 g_exp_ratio_lut[OT_ISP_LCAC_EXP_RATIO_NUM] = {
    64, 128, 256, 512, 1024, 1536, 2048, 3072, 4096, 5120, 6144, 8192, 10240, 12288, 14336, 16384
};

static const td_u8  g_wdr_cb_strdef_lut[OT_ISP_LCAC_EXP_RATIO_NUM] = {
    0, 0, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};
static const td_u8  g_wdr_cr_strdef_lut[OT_ISP_LCAC_EXP_RATIO_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const td_u8  g_lin_cb_strdef_lut[OT_ISP_LCAC_EXP_RATIO_NUM] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};
static const td_u8  g_lin_cr_strdef_lut[OT_ISP_LCAC_EXP_RATIO_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const td_u8  g_de_purp_str_lut[LCAC_STRENGTH_NUM]           = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const td_u16 g_r_luma[RANGE_MAX_VALUE] = {1500, 1500, 0};
static const td_u16 g_g_luma[RANGE_MAX_VALUE] = {1500, 1500, 0};
static const td_u16 g_b_luma[RANGE_MAX_VALUE] = {4095, 1500, 0};
static const td_u16 g_y_luma[RANGE_MAX_VALUE] = {3200, 1500, 0};
static const td_s16 g_cbcrl[RANGE_MAX_VALUE] = {-50, -50, -50};
static const td_u16 g_purple_det_range[RANGE_MAX_VALUE] = {0, 260, 410};

typedef struct {
    td_bool init;
    td_bool local_cac_en;
    td_bool cac_manual_en;
    td_bool coef_update_en;
    td_u8   auto_de_purple_str[LCAC_STRENGTH_NUM];
    td_u8   auto_depurple_cb_str[OT_ISP_LCAC_EXP_RATIO_NUM];
    td_u8   auto_depurple_cr_str[OT_ISP_LCAC_EXP_RATIO_NUM];
    td_u8   manual_depurple_cb_str;
    td_u8   manual_depurple_cr_str;
    td_u8   actual_depurple_cb_str;
    td_u8   actual_depurple_cr_str;
    td_u8   luma_high_cnt_thr;    /* u8.0, [0,153] */
    td_u8   cb_cnt_high_thr;      /* u7.0, [0,68] */
    td_u8   cb_cnt_low_thr;       /* u7.0, [0,68] */
    td_u8   bld_avg_cur;          /* u4.0, [0, 8] */
    td_u16  luma_thr;             /* u12.0, [0,4095] */
    td_u16  cb_thr;               /* u12.0, [0,4095] */
    td_u16  purple_det_range;

    td_u16  r_luma_thr[OT_ISP_LCAC_DET_NUM];
    td_u16  g_luma_thr[OT_ISP_LCAC_DET_NUM];
    td_u16  b_luma_thr[OT_ISP_LCAC_DET_NUM];
    td_u16  y_luma_thr[OT_ISP_LCAC_DET_NUM];
    td_s16  cb_cr_ratio[OT_ISP_LCAC_DET_NUM];

    td_u16  purple_var_thr;
} isp_lcac;

isp_lcac *g_lcac_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

#define lcac_get_ctx(dev, ctx)   ((ctx) = g_lcac_ctx[dev])
#define lcac_set_ctx(dev, ctx)   (g_lcac_ctx[dev] = (ctx))
#define lcac_reset_ctx(dev)      (g_lcac_ctx[dev] = TD_NULL)

static td_u32 lcac_get_exp_ratio(ot_vi_pipe vi_pipe, td_u8 cur_blk, td_u8 wdr_mode)
{
    td_u32 exp_ratio;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    ot_unused(cur_blk);

    exp_ratio = isp_ctx->linkage.exp_ratio;

    if (is_linear_mode(wdr_mode)) {
        return MIN_EXP_RATIO;
    } else if (is_built_in_wdr_mode(wdr_mode)) {
        return MAX_EXP_RATIO;
    } else if (is_2to1_wdr_mode(wdr_mode) || is_3to1_wdr_mode(wdr_mode)) {
        if ((isp_ctx->linkage.fswdr_mode == OT_ISP_FSWDR_LONG_FRAME_MODE) ||
            (isp_ctx->linkage.fswdr_mode == OT_ISP_FSWDR_AUTO_LONG_FRAME_MODE)) {
            return MIN_EXP_RATIO;
        } else {
            return exp_ratio;
        }
    } else {
        return MIN_EXP_RATIO;
    }
}

static td_s32 lcac_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_lcac *lcac_ctx = TD_NULL;

    lcac_get_ctx(vi_pipe, lcac_ctx);

    if (lcac_ctx == TD_NULL) {
        lcac_ctx = (isp_lcac *)isp_malloc(sizeof(isp_lcac));
        if (lcac_ctx == TD_NULL) {
            isp_err_trace("isp[%d] lcac_ctx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void)memset_s(lcac_ctx, sizeof(isp_lcac), 0, sizeof(isp_lcac));

    lcac_set_ctx(vi_pipe, lcac_ctx);

    return TD_SUCCESS;
}

static td_void lcac_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_lcac *lcac_ctx = TD_NULL;

    lcac_get_ctx(vi_pipe, lcac_ctx);
    isp_free(lcac_ctx);
    lcac_reset_ctx(vi_pipe);
}

static td_void lcac_ext_regs_initialize(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    td_u8 wdr_mode;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    wdr_mode = isp_ctx->sns_wdr_mode;
    ot_ext_system_local_cac_manual_mode_enable_write(vi_pipe, OT_OP_MODE_AUTO);
    ot_ext_system_local_cac_enable_write(vi_pipe, TD_TRUE);
    ot_ext_system_local_cac_purple_var_thr_write(vi_pipe, OT_ISP_LCAC_PURPLE_DET_THR_DEFAULT);
    ot_ext_system_local_cac_luma_high_cnt_thr_write(vi_pipe, OT_ISP_DEMOSAIC_CAC_LUMA_HIGH_CNT_THR_DEFAULT);
    ot_ext_system_local_cac_cb_cnt_high_thr_write(vi_pipe, OT_ISP_DEMOSAIC_CAC_CB_CNT_HIGH_THR_DEFAULT);
    ot_ext_system_local_cac_cb_cnt_low_thr_write(vi_pipe, OT_ISP_DEMOSAIC_CAC_CB_CNT_LOW_THR_DEFAULT);
    ot_ext_system_local_cac_luma_thr_write(vi_pipe, OT_ISP_DEMOSAIC_LUMA_THR_DEFAULT);
    ot_ext_system_local_cac_cb_thr_write(vi_pipe, OT_ISP_DEMOSAIC_CB_THR_DEFAULT);
    ot_ext_system_local_cac_bld_avg_cur_write(vi_pipe, OT_ISP_DEMOSAIC_CAC_BLD_AVG_CUR_DEFAULT);
    ot_ext_system_local_cac_defcolor_cr_write(vi_pipe, OT_ISP_DEMOSAIC_DEFCOLOR_CR_DEFAULT);
    ot_ext_system_local_cac_defcolor_cb_write(vi_pipe, OT_ISP_DEMOSAIC_DEFCOLOR_CB_DEFAULT);

    for (i = 0; i < OT_ISP_LCAC_DET_NUM; i++) {
        ot_ext_system_local_cac_r_thd_table_write(vi_pipe, i, g_r_luma[i]);
        ot_ext_system_local_cac_g_thd_table_write(vi_pipe, i, g_g_luma[i]);
        ot_ext_system_local_cac_b_thd_table_write(vi_pipe, i, g_b_luma[i]);
        ot_ext_system_local_cac_luma_thd_table_write(vi_pipe, i, g_y_luma[i]);
        ot_ext_system_local_cac_cbcr_ratio_table_write(vi_pipe, i, g_cbcrl[i]);
    }

    if (is_linear_mode(wdr_mode)) { /* linear mode */
        for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
            ot_ext_system_local_cac_auto_cb_str_table_write(vi_pipe, i, g_lin_cb_strdef_lut[i]);
            ot_ext_system_local_cac_auto_cr_str_table_write(vi_pipe, i, g_lin_cr_strdef_lut[i]);
        }

        ot_ext_system_local_cac_manual_cb_str_write(vi_pipe, LCAC_DEPURPLE_CB_STR_LIN);
        ot_ext_system_local_cac_manual_cr_str_write(vi_pipe, LCAC_DEPURPLE_CR_STR_LIN);
        ot_ext_system_local_cac_purple_det_range_write(vi_pipe, ISP_LCAC_PURPLE_DET_RANGE_DEFAULT_LINEAR);
    } else { /* WDR mode */
        for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
            ot_ext_system_local_cac_auto_cb_str_table_write(vi_pipe, i, g_wdr_cb_strdef_lut[i]);
            ot_ext_system_local_cac_auto_cr_str_table_write(vi_pipe, i, g_wdr_cr_strdef_lut[i]);
        }

        ot_ext_system_local_cac_manual_cb_str_write(vi_pipe, LCAC_DEPURPLE_CB_STR_WDR);
        ot_ext_system_local_cac_manual_cr_str_write(vi_pipe, LCAC_DEPURPLE_CR_STR_WDR);
        ot_ext_system_local_cac_purple_det_range_write(vi_pipe, ISP_LCAC_PURPLE_DET_RANGE_DEFAULT_WDR);
    }

    ot_ext_system_local_cac_coef_update_en_write(vi_pipe, TD_TRUE);
}

static td_void lcac_static_regs_initialize(isp_local_cac_static_cfg  *static_reg_cfg)
{
    td_u8 i;

    static_reg_cfg->nddm_cac_blend_en     = TD_FALSE;
    static_reg_cfg->new_lcac_proc         = TD_FALSE;
    static_reg_cfg->nddm_cac_blend_rate   = OT_ISP_NDDM_CAC_BLEND_RATE_DEFAULT;
    static_reg_cfg->r_counter_thr         = OT_ISP_LCAC_COUNT_THR_R_DEFAULT;
    static_reg_cfg->g_counter_thr         = OT_ISP_LCAC_COUNT_THR_G_DEFAULT;
    static_reg_cfg->b_counter_thr         = OT_ISP_LCAC_COUNT_THR_B_DEFAULT;
    static_reg_cfg->satu_thr              = OT_ISP_LCAC_SATU_THR_DEFAULT;
    static_reg_cfg->cb_cr_ratio_lmt_high2 = OT_ISP_LCAC_CBCR_RATIO_LMT_H2_DEFAULT;
    static_reg_cfg->cb_cr_ratio_lmt_low2  = OT_ISP_LCAC_CBCR_RATIO_LMT_L2_DEFAULT;

    for (i = 0; i < LCAC_STRENGTH_NUM; i++) {
        static_reg_cfg->de_purple_str[i] = ISP_LCAC_MAX_STRENGTH - g_de_purp_str_lut[i];
    }

    static_reg_cfg->static_resh         = TD_TRUE;
}

static td_void lcac_usr_regs_initialize(ot_vi_pipe vi_pipe, td_u8 wdr_mode,
                                        isp_local_cac_usr_cfg *usr_reg_cfg, isp_lcac *lcac_ctx)
{
    ot_isp_cmos_default *sns_dft = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);

    usr_reg_cfg->resh              = TD_TRUE;
    usr_reg_cfg->update_index      = 1;
    usr_reg_cfg->var_thr           = OT_ISP_LCAC_PURPLE_DET_THR_DEFAULT;
    usr_reg_cfg->luma_high_cnt_thr = OT_ISP_DEMOSAIC_CAC_LUMA_HIGH_CNT_THR_DEFAULT;
    usr_reg_cfg->cb_cnt_high_thr   = OT_ISP_DEMOSAIC_CAC_CB_CNT_HIGH_THR_DEFAULT;
    usr_reg_cfg->cb_cnt_low_thr    = OT_ISP_DEMOSAIC_CAC_CB_CNT_LOW_THR_DEFAULT;
    usr_reg_cfg->bld_avg_cur       = OT_ISP_DEMOSAIC_CAC_BLD_AVG_CUR_DEFAULT;
    usr_reg_cfg->cb_thr            = OT_ISP_DEMOSAIC_CB_THR_DEFAULT;

    if (is_linear_mode(wdr_mode)) {
        usr_reg_cfg->cb_cr_ratio_lmt_high  = ISP_LCAC_CRCB_RATIO_HIGH_LIMIT_LINEAR;
    } else {
        usr_reg_cfg->cb_cr_ratio_lmt_high  = ISP_LCAC_CRCB_RATIO_HIGH_LIMIT_WDR;
    }

    /* Read from cmos or init from pre value */
    if (sns_dft->key.bit1_lcac) {
        usr_reg_cfg->var_thr = lcac_ctx->purple_var_thr;
    } else {
        usr_reg_cfg->var_thr = OT_ISP_LCAC_PURPLE_DET_THR_DEFAULT;
    }
}

static td_s32 lcac_get_idx(td_u16 purple_det_range)
{
    td_s32 idx;
    for (idx = 0; idx < (RANGE_MAX_VALUE - 1); idx++) {
        if (purple_det_range < g_purple_det_range[idx]) {
            break;
        }
    }

    return idx;
}

static td_void lcac_dyna_update_from_ctx(ot_vi_pipe vi_pipe, isp_local_cac_dyna_cfg *dyna_reg_cfg,
                                         td_u8 cur_blk, isp_lcac *lcac_ctx, td_u8 wdr_mode)
{
    td_u8 i, depur_cb, depur_cr;
    td_u16 pdet_range = lcac_ctx->purple_det_range;
    td_s32 r_low, r_up, range_idx_up, range_idx_low, ratio_h, ratio_l;
    range_idx_up  = lcac_get_idx(pdet_range);
    range_idx_low = MAX2(range_idx_up - 1, 0);
    r_up          = g_purple_det_range[range_idx_up];
    r_low         = g_purple_det_range[range_idx_low];
    td_u16 r_luma_thr = (td_u16)linear_inter(pdet_range, r_low, lcac_ctx->r_luma_thr[range_idx_low],
                                             r_up, lcac_ctx->r_luma_thr[range_idx_up]);
    td_u16 g_luma_thr = (td_u16)linear_inter(pdet_range, r_low, lcac_ctx->g_luma_thr[range_idx_low],
                                             r_up, lcac_ctx->g_luma_thr[range_idx_up]);
    td_u16 b_luma_thr = (td_u16)linear_inter(pdet_range, r_low, lcac_ctx->b_luma_thr[range_idx_low],
                                             r_up, lcac_ctx->b_luma_thr[range_idx_up]);
    td_u16 luma_thr   = (td_u16)linear_inter(pdet_range, r_low, lcac_ctx->y_luma_thr[range_idx_low],
                                             r_up, lcac_ctx->y_luma_thr[range_idx_up]);
    td_s16 cb_cr_l    =         linear_inter(pdet_range, r_low, lcac_ctx->cb_cr_ratio[range_idx_low],
                                             r_up, lcac_ctx->cb_cr_ratio[range_idx_up]);
    td_u32 ratio  = lcac_get_exp_ratio(vi_pipe, cur_blk, wdr_mode);
    if (lcac_ctx->cac_manual_en) {
        depur_cb = lcac_ctx->manual_depurple_cb_str;
        depur_cr = lcac_ctx->manual_depurple_cr_str;
    } else {
        ratio_h = OT_ISP_LCAC_EXP_RATIO_NUM - 1;
        for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
            if (ratio <= g_exp_ratio_lut[i]) {
                ratio_h = i;
                break;
            }
        }
        ratio_l  = MAX2(ratio_h - 1, 0);
        depur_cb = linear_inter(ratio, g_exp_ratio_lut[ratio_l], lcac_ctx->auto_depurple_cb_str[ratio_l],
                                g_exp_ratio_lut[ratio_h], lcac_ctx->auto_depurple_cb_str[ratio_h]);
        depur_cr = linear_inter(ratio, g_exp_ratio_lut[ratio_l], lcac_ctx->auto_depurple_cr_str[ratio_l],
                                g_exp_ratio_lut[ratio_h], lcac_ctx->auto_depurple_cr_str[ratio_h]);
    }
    dyna_reg_cfg->r_luma_thr = r_luma_thr;
    dyna_reg_cfg->g_luma_thr = g_luma_thr;
    dyna_reg_cfg->b_luma_thr = b_luma_thr;
    dyna_reg_cfg->luma_thr   = luma_thr;
    dyna_reg_cfg->cb_cr_ratio_lmt_low = cb_cr_l;
    dyna_reg_cfg->de_purple_ctr_cb    = ISP_LCAC_MAX_STRENGTH - depur_cb;
    dyna_reg_cfg->de_purple_ctr_cr    = ISP_LCAC_MAX_STRENGTH - depur_cr;
    return;
}

static td_void lcac_dyna_regs_initialize(ot_vi_pipe vi_pipe, td_u8 wdr_mode, td_u8 cur_blk,
                                         isp_local_cac_dyna_cfg *dyna_reg_cfg, isp_lcac *lcac_ctx)
{
    ot_isp_cmos_default *sns_dft = TD_NULL;
    isp_sensor_get_default(vi_pipe, &sns_dft);
    if (sns_dft->key.bit1_lcac) {
        lcac_dyna_update_from_ctx(vi_pipe, dyna_reg_cfg, cur_blk, lcac_ctx, wdr_mode);
    } else {
        if (is_linear_mode(wdr_mode)) {
            dyna_reg_cfg->r_luma_thr          = ISP_LCAC_LUMA_THR_R_LINEAR;
            dyna_reg_cfg->g_luma_thr          = ISP_LCAC_LUMA_THR_G_LINEAR;
            dyna_reg_cfg->b_luma_thr          = ISP_LCAC_LUMA_THR_B_LINEAR;
            dyna_reg_cfg->de_purple_ctr_cb    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CB_STR_LIN;
            dyna_reg_cfg->de_purple_ctr_cr    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CR_STR_LIN;
            dyna_reg_cfg->cb_cr_ratio_lmt_low = ISP_LCAC_CRCB_RATIO_LOW_LIMIT_LINEAR;
        } else {
            dyna_reg_cfg->r_luma_thr          = ISP_LCAC_LUMA_THR_R_WDR;
            dyna_reg_cfg->g_luma_thr          = ISP_LCAC_LUMA_THR_G_WDR;
            dyna_reg_cfg->b_luma_thr          = ISP_LCAC_LUMA_THR_B_WDR;
            dyna_reg_cfg->de_purple_ctr_cb    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CB_STR_WDR;
            dyna_reg_cfg->de_purple_ctr_cr    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CR_STR_WDR;
            dyna_reg_cfg->cb_cr_ratio_lmt_low = ISP_LCAC_CRCB_RATIO_LOW_LIMIT_WDR;
        }
    }
    dyna_reg_cfg->luma_thr = OT_ISP_DEMOSAIC_LUMA_THR_DEFAULT;
    dyna_reg_cfg->resh     = TD_TRUE;
}

static td_s32  lcac_set_long_frame_mode(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u8 i;
    isp_usr_ctx            *isp_ctx = TD_NULL;
    isp_local_cac_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_local_cac_usr_cfg  *usr_reg_cfg = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        usr_reg_cfg  = &reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.usr_reg_cfg;
        dyna_reg_cfg = &reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.dyna_reg_cfg;

        if (is_linear_mode(isp_ctx->sns_wdr_mode) ||
            (isp_ctx->linkage.fswdr_mode == OT_ISP_FSWDR_LONG_FRAME_MODE) ||
            (isp_ctx->linkage.fswdr_mode == OT_ISP_FSWDR_AUTO_LONG_FRAME_MODE)) {
            dyna_reg_cfg->r_luma_thr          = ISP_LCAC_LUMA_THR_R_LINEAR;
            dyna_reg_cfg->g_luma_thr          = ISP_LCAC_LUMA_THR_G_LINEAR;
            dyna_reg_cfg->b_luma_thr          = ISP_LCAC_LUMA_THR_B_LINEAR;
            dyna_reg_cfg->de_purple_ctr_cb    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CB_STR_LIN;
            dyna_reg_cfg->de_purple_ctr_cr    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CR_STR_LIN;
            dyna_reg_cfg->cb_cr_ratio_lmt_low = ISP_LCAC_CRCB_RATIO_LOW_LIMIT_LINEAR;
            usr_reg_cfg->cb_cr_ratio_lmt_high = ISP_LCAC_CRCB_RATIO_HIGH_LIMIT_LINEAR;
        } else {
            dyna_reg_cfg->r_luma_thr          = ISP_LCAC_LUMA_THR_R_WDR;
            dyna_reg_cfg->g_luma_thr          = ISP_LCAC_LUMA_THR_G_WDR;
            dyna_reg_cfg->b_luma_thr          = ISP_LCAC_LUMA_THR_B_WDR;
            dyna_reg_cfg->de_purple_ctr_cb    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CB_STR_WDR;
            dyna_reg_cfg->de_purple_ctr_cr    = ISP_LCAC_MAX_STRENGTH - LCAC_DEPURPLE_CR_STR_WDR;
            dyna_reg_cfg->cb_cr_ratio_lmt_low = ISP_LCAC_CRCB_RATIO_LOW_LIMIT_WDR;

            usr_reg_cfg->cb_cr_ratio_lmt_high = ISP_LCAC_CRCB_RATIO_HIGH_LIMIT_WDR;
        }

        usr_reg_cfg->resh = TD_TRUE;
    }

    return TD_SUCCESS;
}

static td_void lcac_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_lcac    *lcac_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    lcac_get_ctx(vi_pipe, lcac_ctx);

    /* local CAC */
    for (i = 0; i < reg_cfg->cfg_num; i++) {
        lcac_static_regs_initialize(&reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.static_reg_cfg);
        lcac_usr_regs_initialize(vi_pipe, isp_ctx->sns_wdr_mode,
                                 &reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.usr_reg_cfg, lcac_ctx);
        lcac_dyna_regs_initialize(vi_pipe, isp_ctx->sns_wdr_mode, i,
                                  &reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.dyna_reg_cfg, lcac_ctx);

        reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.local_cac_en = TD_TRUE;
    }

    reg_cfg->cfg_key.bit1_local_cac_cfg  = 1;
}

static td_void local_cac_read_extregs(ot_vi_pipe vi_pipe, isp_lcac *local_cac)
{
    td_u8 i;

    local_cac->coef_update_en = ot_ext_system_local_cac_coef_update_en_read(vi_pipe);
    if (local_cac->coef_update_en) {
        ot_ext_system_local_cac_coef_update_en_write(vi_pipe, TD_FALSE);
        for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
            local_cac->auto_depurple_cb_str[i] = ot_ext_system_local_cac_auto_cb_str_table_read(vi_pipe, i);
            local_cac->auto_depurple_cr_str[i] = ot_ext_system_local_cac_auto_cr_str_table_read(vi_pipe, i);
        }

        for (i = 0; i < OT_ISP_LCAC_DET_NUM; i++) {
            local_cac->r_luma_thr[i] = ot_ext_system_local_cac_r_thd_table_read(vi_pipe, i);
            local_cac->g_luma_thr[i] = ot_ext_system_local_cac_g_thd_table_read(vi_pipe, i);
            local_cac->b_luma_thr[i] = ot_ext_system_local_cac_b_thd_table_read(vi_pipe, i);
            local_cac->y_luma_thr[i] = ot_ext_system_local_cac_luma_thd_table_read(vi_pipe, i);
            local_cac->cb_cr_ratio[i] = ot_ext_system_local_cac_cbcr_ratio_table_read(vi_pipe, i);
        }

        local_cac->cac_manual_en          = ot_ext_system_local_cac_manual_mode_enable_read(vi_pipe);
        local_cac->manual_depurple_cb_str = ot_ext_system_local_cac_manual_cb_str_read(vi_pipe);
        local_cac->manual_depurple_cr_str = ot_ext_system_local_cac_manual_cr_str_read(vi_pipe);
        local_cac->purple_det_range       = ot_ext_system_local_cac_purple_det_range_read(vi_pipe);
        local_cac->purple_var_thr         = ot_ext_system_local_cac_purple_var_thr_read(vi_pipe);

        local_cac->luma_high_cnt_thr = ot_ext_system_local_cac_luma_high_cnt_thr_read(vi_pipe);
        local_cac->cb_cnt_high_thr   = ot_ext_system_local_cac_cb_cnt_high_thr_read(vi_pipe);
        local_cac->cb_cnt_low_thr    = ot_ext_system_local_cac_cb_cnt_low_thr_read(vi_pipe);
        local_cac->bld_avg_cur       = ot_ext_system_local_cac_bld_avg_cur_read(vi_pipe);
        local_cac->luma_thr          = ot_ext_system_local_cac_luma_thr_read(vi_pipe);
        local_cac->cb_thr            = ot_ext_system_local_cac_cb_thr_read(vi_pipe);
    }

    return;
}


static td_s32 lcac_initialize(ot_vi_pipe vi_pipe, isp_lcac *local_cac)
{
    td_u16 i;
    td_s32 ret = TD_SUCCESS;
    ot_isp_cmos_default *sns_dft = TD_NULL;
    isp_sensor_get_default(vi_pipe, &sns_dft);

    if (sns_dft->key.bit1_lcac) {
        isp_check_pointer_return(sns_dft->lcac);
        isp_check_bool_return(sns_dft->lcac->en);
        ret = isp_lcac_attr_check("cmos", sns_dft->lcac);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        ot_ext_system_local_cac_enable_write(vi_pipe, sns_dft->lcac->en);
        ot_ext_system_local_cac_manual_mode_enable_write(vi_pipe, sns_dft->lcac->op_type);
        ot_ext_system_local_cac_purple_det_range_write(vi_pipe, sns_dft->lcac->purple_detect_range);
        ot_ext_system_local_cac_purple_var_thr_write(vi_pipe, sns_dft->lcac->var_threshold);
        ot_ext_system_local_cac_manual_cb_str_write(vi_pipe, sns_dft->lcac->manual_attr.de_purple_cb_strength);
        ot_ext_system_local_cac_manual_cr_str_write(vi_pipe, sns_dft->lcac->manual_attr.de_purple_cr_strength);
        for (i = 0; i < OT_ISP_LCAC_EXP_RATIO_NUM; i++) {
            ot_ext_system_local_cac_auto_cb_str_table_write(vi_pipe, i,
                                                            sns_dft->lcac->auto_attr.de_purple_cb_strength[i]);
            ot_ext_system_local_cac_auto_cr_str_table_write(vi_pipe, i,
                                                            sns_dft->lcac->auto_attr.de_purple_cr_strength[i]);
            local_cac->auto_depurple_cb_str[i] = sns_dft->lcac->auto_attr.de_purple_cb_strength[i];
            local_cac->auto_depurple_cr_str[i] = sns_dft->lcac->auto_attr.de_purple_cr_strength[i];
        }
        for (i = 0; i < OT_ISP_LCAC_DET_NUM; i++) {
            ot_ext_system_local_cac_r_thd_table_write(vi_pipe, i, sns_dft->lcac->r_detect_threshold[i]);
            ot_ext_system_local_cac_g_thd_table_write(vi_pipe, i, sns_dft->lcac->g_detect_threshold[i]);
            ot_ext_system_local_cac_b_thd_table_write(vi_pipe, i, sns_dft->lcac->b_detect_threshold[i]);
            ot_ext_system_local_cac_luma_thd_table_write(vi_pipe, i, sns_dft->lcac->l_detect_threshold[i]);
            ot_ext_system_local_cac_cbcr_ratio_table_write(vi_pipe, i, sns_dft->lcac->cb_cr_ratio[i]);
            local_cac->r_luma_thr[i] = sns_dft->lcac->r_detect_threshold[i];
            local_cac->g_luma_thr[i] = sns_dft->lcac->g_detect_threshold[i];
            local_cac->b_luma_thr[i] = sns_dft->lcac->b_detect_threshold[i];
            local_cac->y_luma_thr[i] = sns_dft->lcac->l_detect_threshold[i];
            local_cac->cb_cr_ratio[i] = sns_dft->lcac->cb_cr_ratio[i];
        }
        local_cac->purple_det_range     = sns_dft->lcac->purple_detect_range;
        local_cac->purple_var_thr       = sns_dft->lcac->var_threshold;
        local_cac->manual_depurple_cb_str = sns_dft->lcac->manual_attr.de_purple_cb_strength;
        local_cac->manual_depurple_cr_str = sns_dft->lcac->manual_attr.de_purple_cr_strength;
        local_cac->cac_manual_en        = sns_dft->lcac->op_type;
        local_cac->local_cac_en         = sns_dft->lcac->en;
    } else {
        local_cac->local_cac_en = TD_TRUE;
    }
    return ret;
}

static td_void local_cac_usr_fw(isp_local_cac_usr_cfg  *usr_reg_cfg, isp_lcac *local_cac)
{
    usr_reg_cfg->resh              = TD_TRUE;
    usr_reg_cfg->var_thr           = local_cac->purple_var_thr;
    usr_reg_cfg->luma_high_cnt_thr = local_cac->luma_high_cnt_thr;
    usr_reg_cfg->bld_avg_cur       = local_cac->bld_avg_cur;
    usr_reg_cfg->cb_thr            = local_cac->cb_thr;

    usr_reg_cfg->cb_cnt_high_thr   = local_cac->cb_cnt_high_thr;
    usr_reg_cfg->cb_cnt_low_thr    = local_cac->cb_cnt_low_thr;

    usr_reg_cfg->update_index  += 1;

    return;
}

static td_void local_cac_dyna_fw(ot_vi_pipe vi_pipe, td_u8 cur_blk, isp_lcac *local_cac,
                                 isp_local_cac_dyna_cfg *dyna_reg_cfg, td_u8 wdr_mode)
{
    lcac_dyna_update_from_ctx(vi_pipe, dyna_reg_cfg, cur_blk, local_cac, wdr_mode);

    local_cac->actual_depurple_cb_str = ISP_LCAC_MAX_STRENGTH - dyna_reg_cfg->de_purple_ctr_cb;
    local_cac->actual_depurple_cr_str = ISP_LCAC_MAX_STRENGTH - dyna_reg_cfg->de_purple_ctr_cr;

    dyna_reg_cfg->resh = TD_TRUE;

    return;
}

static __inline td_bool  check_lcac_open(const isp_lcac *local_cac)
{
    return (local_cac->local_cac_en == TD_TRUE);
}

static td_s32 isp_lcac_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_s32 ret;
    isp_lcac    *local_cac     = TD_NULL;
    isp_reg_cfg *local_reg_cfg = (isp_reg_cfg *)reg_cfg;

    ot_ext_system_isp_lcac_init_status_write(vi_pipe, TD_FALSE);
    ret = lcac_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    lcac_get_ctx(vi_pipe, local_cac);
    isp_check_pointer_return(local_cac);

    local_cac->init = TD_FALSE;
    lcac_ext_regs_initialize(vi_pipe);
    ret = lcac_initialize(vi_pipe, local_cac);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    lcac_regs_initialize(vi_pipe, local_reg_cfg);

    local_cac->init = TD_TRUE;
    ot_ext_system_isp_lcac_init_status_write(vi_pipe, local_cac->init);

    return TD_SUCCESS;
}

static td_void isp_lcac_wdr_mode_set(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_u8 i;
    td_s32 ret;
    td_u32 update_idx[OT_ISP_STRIPING_MAX_NUM] = {0};
    isp_reg_cfg *local_reg_cfg = (isp_reg_cfg *)reg_cfg;

    for (i = 0; i < local_reg_cfg->cfg_num; i++) {
        update_idx[i] = local_reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.usr_reg_cfg.update_index;
    }

    ret = isp_lcac_init(vi_pipe, reg_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("isp[%d] isp_lcac_init failed!\n", vi_pipe);
        return;
    }

    for (i = 0; i < local_reg_cfg->cfg_num; i++) {
        local_reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.usr_reg_cfg.update_index = update_idx[i] + 1;
    }
}

static td_s32 isp_lcac_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv)
{
    td_u8        i;
    ot_isp_alg_mod alg_mod = OT_ISP_ALG_LCAC;
    isp_lcac     *local_cac     = TD_NULL;
    isp_reg_cfg  *local_reg_cfg = (isp_reg_cfg *)reg_cfg;
    isp_usr_ctx  *isp_ctx       = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    lcac_get_ctx(vi_pipe, local_cac);
    isp_check_pointer_return(local_cac);

    ot_unused(stat_info);
    ot_unused(rsv);

    if (isp_ctx->linkage.defect_pixel) {
        return TD_SUCCESS;
    }

    ot_ext_system_isp_lcac_init_status_write(vi_pipe, local_cac->init);
    if (local_cac->init != TD_TRUE) {
        ioctl(isp_get_fd(vi_pipe), ISP_ALG_INIT_ERR_INFO_PRINT, &alg_mod);
        return TD_SUCCESS;
    }

    if (isp_ctx->linkage.fswdr_mode != isp_ctx->linkage.pre_fswdr_mode) {
        lcac_set_long_frame_mode(vi_pipe, reg_cfg);
    }

    local_cac->local_cac_en = ot_ext_system_local_cac_enable_read(vi_pipe);

    for (i = 0; i < local_reg_cfg->cfg_num; i++) {
        local_reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.local_cac_en = local_cac->local_cac_en;
    }

    local_reg_cfg->cfg_key.bit1_local_cac_cfg = 1;

    /* check hardware setting */
    if (!check_lcac_open(local_cac)) {
        return TD_SUCCESS;
    }

    local_cac_read_extregs(vi_pipe, local_cac);

    if (local_cac->coef_update_en) {
        for (i = 0; i < local_reg_cfg->cfg_num; i++) {
            local_cac_usr_fw(&local_reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.usr_reg_cfg, local_cac);
        }
    }

    for (i = 0; i < local_reg_cfg->cfg_num; i++) {
        local_cac_dyna_fw(vi_pipe, i, local_cac, &local_reg_cfg->alg_reg_cfg[i].lcac_reg_cfg.dyna_reg_cfg,
                          isp_ctx->sns_wdr_mode);
    }

    return TD_SUCCESS;
}

static td_void lcac_proc_write(ot_vi_pipe vi_pipe, ot_isp_ctrl_proc_write *proc)
{
    ot_isp_ctrl_proc_write  proc_tmp;
    isp_lcac *local_cac = TD_NULL;

    lcac_get_ctx(vi_pipe, local_cac);
    isp_check_pointer_void_return(local_cac);

    if ((proc->proc_buff == TD_NULL) || (proc->buff_len == 0)) {
        return;
    }

    proc_tmp.proc_buff = proc->proc_buff;
    proc_tmp.buff_len  = proc->buff_len;

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "-----lcac info-----------------------------------\n");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%12s" "%12s" "%12s\n", "enable", "cr_str", "cb_str");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%12u" "%12u" "%12u\n",
                    local_cac->local_cac_en, local_cac->actual_depurple_cr_str,
                    local_cac->actual_depurple_cb_str);

    proc->write_len += 1;
}

static td_s32 isp_lcac_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    isp_reg_cfg_attr  *local_reg_cfg = TD_NULL;

    switch (cmd) {
        case OT_ISP_WDR_MODE_SET:
            isp_regcfg_get_ctx(vi_pipe, local_reg_cfg);
            isp_check_pointer_return(local_reg_cfg);
            isp_lcac_wdr_mode_set(vi_pipe, (td_void *)&local_reg_cfg->reg_cfg);
            break;
        case OT_ISP_PROC_WRITE:
            lcac_proc_write(vi_pipe, (ot_isp_ctrl_proc_write *)value);
            break;
        default:
            break;
    }
    return TD_SUCCESS;
}

static td_s32 isp_lcac_exit(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_reg_cfg_attr  *local_reg_cfg = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, local_reg_cfg);
    ot_ext_system_isp_lcac_init_status_write(vi_pipe, TD_FALSE);
    for (i = 0; i < local_reg_cfg->reg_cfg.cfg_num; i++) {
        local_reg_cfg->reg_cfg.alg_reg_cfg[i].lcac_reg_cfg.local_cac_en = TD_FALSE;
    }

    local_reg_cfg->reg_cfg.cfg_key.bit1_local_cac_cfg = 1;

    lcac_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_alg_register_lcac(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_node *algs = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_lcac);
    algs = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_return(algs);

    algs->alg_type = OT_ISP_ALG_LCAC;
    algs->alg_func.pfn_alg_init = isp_lcac_init;
    algs->alg_func.pfn_alg_run  = isp_lcac_run;
    algs->alg_func.pfn_alg_ctrl = isp_lcac_ctrl;
    algs->alg_func.pfn_alg_exit = isp_lcac_exit;
    algs->used = TD_TRUE;

    return TD_SUCCESS;
}
