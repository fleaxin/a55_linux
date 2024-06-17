/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_alg.h"
#include "isp_sensor.h"
#include "isp_config.h"
#include "isp_ext_config.h"
#include "isp_math_utils.h"
#include "isp_proc.h"
#include "isp_param_check.h"
#include "isp_ext_reg_access.h"

typedef struct {
    td_bool init;
    td_bool coef_update_en;
    isp_acac_manual_attr actual;
    ot_isp_acac_attr mpi_cfg;
} isp_acac;

#define ISP_ACAC_NORMFACTOR        8
#define ISP_ACAC_GLOBALSTR_DEFAULT 256
#define ISP_ACAC_DETMODE_DEFAULT   0
#define ISP_ACAC_ALPHARB_DEFAULT   256
#define ISP_EDETCOLORDIF_DEFAULT   240
#define ISP_EDETGREY_DEFAULT       240
#define ISP_EDETVAR_DEFAULT        100
#define FCF_CRCB_RATIO_H_LIMIT2    290
#define FCF_CRCB_RATIO_H_LIMIT     100
#define FCF_CRCB_RATIO_L_LIMIT     (-50)
#define FCF_CRCB_RATIO_L_LIMIT2    0
#define PURPLE_ALPHA               63
#define EDGE_ALPHA                 63
#define DET_SATU_THD               0
#define FCC_PURPLE_H_OFFSET        80
#define FCC_PURPLE_L_OFFSET        (-80)
#define ACAC_LAMDA_PRECS           16
#define ACAC_THDRB_PRECS           16

static const td_u16 g_acac_rb_thresh[OT_ISP_ACAC_THR_NUM] = {50, 600};
static const td_u16 g_acac_lamda_thresh[OT_ISP_ACAC_THR_NUM] = {10, 300};
static const td_u16 g_acac_var_thresh[OT_ISP_AUTO_ISO_NUM] = {
    100, 100, 100, 100, 100, 100, 100, 100, 80, 80, 60, 60, 60, 30, 10, 0
};
static const td_u16  g_edge_thd[OT_ISP_ACAC_THR_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {100, 100, 100, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150},
    {500, 500, 500, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600}
};
static const td_u16  g_cac_th_rb[OT_ISP_ACAC_THR_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {50,  50,  50,  50,  50,  50,  50,  60,  80, 100, 120, 150, 150, 150, 150, 150},
    {1000, 1000, 1000, 1000, 1000, 1000, 900, 900, 800, 600, 600, 600, 600, 600, 600, 600}
};
static const td_u16  g_edge_gain[OT_ISP_AUTO_ISO_NUM] = {
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256
};
static const td_u16  g_det_pur_limit_h[OT_ISP_AUTO_ISO_NUM] = {
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150
};
static const td_u16  g_det_pur_limit_l[OT_ISP_AUTO_ISO_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const td_u16  g_det_pur_sat_thd[OT_ISP_AUTO_ISO_NUM] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const td_u16  g_purple_alpha[OT_ISP_AUTO_ISO_NUM] = {
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63
};
static const td_u16  g_edge_alpha[OT_ISP_AUTO_ISO_NUM] = {
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63
};
static const td_u16  g_fcc_y[OT_ISP_AUTO_ISO_NUM] = {
    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256
};
static const td_u16  g_fcc_rb[OT_ISP_AUTO_ISO_NUM] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

isp_acac *g_past_acac_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

#define acac_get_ctx(dev, ctx)   ((ctx) = g_past_acac_ctx[dev])
#define acac_set_ctx(dev, ctx)   (g_past_acac_ctx[dev] = (ctx))
#define acac_reset_ctx(dev)      (g_past_acac_ctx[dev] = TD_NULL)

static td_s32 acac_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_acac *past_acac_ctx = TD_NULL;
    acac_get_ctx(vi_pipe, past_acac_ctx);

    if (past_acac_ctx == TD_NULL) {
        past_acac_ctx = (isp_acac *)isp_malloc(sizeof(isp_acac));
        if (past_acac_ctx == TD_NULL) {
            isp_err_trace("isp[%d] acac_ctx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void)memset_s(past_acac_ctx, sizeof(isp_acac), 0, sizeof(isp_acac));
    acac_set_ctx(vi_pipe, past_acac_ctx);

    return TD_SUCCESS;
}

static td_void acac_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_acac *past_acac_ctx = TD_NULL;
    acac_get_ctx(vi_pipe, past_acac_ctx);
    isp_free(past_acac_ctx);
    acac_reset_ctx(vi_pipe);
}

static td_s32 acac_check_cmos_param(const ot_isp_acac_attr *acac_attr)
{
    td_s32 ret;
    isp_check_bool_return(acac_attr->en);
    ret = isp_acac_comm_attr_check("cmos", acac_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_acac_manual_attr_check("cmos", &acac_attr->manual_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_acac_auto_attr_check("cmos", &acac_attr->auto_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_void isp_acac_comm_attr_write(ot_vi_pipe vi_pipe, ot_isp_acac_attr *mpi_cfg)
{
    ot_ext_system_acac_enable_write(vi_pipe, mpi_cfg->en);
    ot_ext_system_acac_op_type_write(vi_pipe, mpi_cfg->op_type);
    ot_ext_system_acac_det_mode_write(vi_pipe, mpi_cfg->detect_mode);
}

static td_void acac_ext_regs_init(ot_vi_pipe vi_pipe, isp_acac *acac_ctx)
{
    isp_acac_comm_attr_write(vi_pipe, &acac_ctx->mpi_cfg);
    isp_acac_auto_attr_write(vi_pipe, &acac_ctx->mpi_cfg.auto_attr);
    isp_acac_manual_attr_write(vi_pipe, &acac_ctx->mpi_cfg.manual_attr);
}

static td_void acac_static_regs_init(ot_vi_pipe vi_pipe, isp_acac_static_cfg *acac_static_reg_cfg)
{
    ot_unused(vi_pipe);

    const td_u16  tao_value = 240; /* default is 240 */
    acac_static_reg_cfg->cac_norm_factor    = ISP_ACAC_NORMFACTOR;
    acac_static_reg_cfg->cac_tao            = tao_value;
    acac_static_reg_cfg->static_resh        = TD_TRUE;
    return;
}

static td_void acac_usr_regs_init(isp_acac_usr_cfg *acac_usr_reg_cfg)
{
    td_u16 lamda_mul;
    acac_usr_reg_cfg->cac_det_mode                = ISP_ACAC_DETMODE_DEFAULT;
    acac_usr_reg_cfg->cac_str                     = ISP_ACAC_GLOBALSTR_DEFAULT;
    acac_usr_reg_cfg->alpha_r                     = ISP_ACAC_ALPHARB_DEFAULT;
    acac_usr_reg_cfg->alpha_b                     = ISP_ACAC_ALPHARB_DEFAULT;
    acac_usr_reg_cfg->color_dif_thd               = ISP_EDETCOLORDIF_DEFAULT;
    acac_usr_reg_cfg->grey_thd                    = ISP_EDETGREY_DEFAULT;
    acac_usr_reg_cfg->lamda_thd0                  = g_acac_lamda_thresh[0];
    acac_usr_reg_cfg->lamda_thd1                  = g_acac_lamda_thresh[1];
    acac_usr_reg_cfg->fcf_cr_cb_ratio_high_limit2 = FCF_CRCB_RATIO_H_LIMIT2;
    acac_usr_reg_cfg->fcf_cr_cb_ratio_high_limit  = FCF_CRCB_RATIO_H_LIMIT;
    acac_usr_reg_cfg->fcf_cr_cb_ratio_low_limit   = FCF_CRCB_RATIO_L_LIMIT;
    acac_usr_reg_cfg->fcf_cr_cb_ratio_low_limit2  = FCF_CRCB_RATIO_L_LIMIT2;
    acac_usr_reg_cfg->purple_alpha                = PURPLE_ALPHA;
    acac_usr_reg_cfg->edge_alpha                  = EDGE_ALPHA;
    acac_usr_reg_cfg->det_satu_thr                = DET_SATU_THD;
    if (g_acac_lamda_thresh[0] == g_acac_lamda_thresh[1]) {
        lamda_mul = 0;
    } else {
        lamda_mul = (0x100 << ACAC_LAMDA_PRECS) / (g_acac_lamda_thresh[1] - g_acac_lamda_thresh[0]);
    }
    acac_usr_reg_cfg->lamda_mul = lamda_mul;
    acac_usr_reg_cfg->usr_resh  = TD_TRUE;
    acac_usr_reg_cfg->update_index = 1;
    return;
}

static td_void acac_dyna_regs_init(isp_acac_dyna_cfg *acac_dyna_reg_cfg)
{
    td_u16 edge_thdrb_mul;
    acac_dyna_reg_cfg->edge_thd0   = g_acac_rb_thresh[0];
    acac_dyna_reg_cfg->edge_thd1   = g_acac_rb_thresh[1];
    acac_dyna_reg_cfg->var_thd     = ISP_EDETVAR_DEFAULT;
    /* calculate by using default value */
    if (g_acac_rb_thresh[0] == g_acac_rb_thresh[1]) {
        edge_thdrb_mul = 0;
    } else {
        edge_thdrb_mul = (1 << ACAC_THDRB_PRECS) / (g_acac_rb_thresh[1] - g_acac_rb_thresh[0]);
    }
    acac_dyna_reg_cfg->edge_thd_mul = edge_thdrb_mul;
    acac_dyna_reg_cfg->dyna_resh    = TD_TRUE;

    return;
}

static td_void acac_regs_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_acac *acac_ctx)
{
    td_u8 i;
    isp_usr_ctx  *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    for (i = 0; i < isp_ctx->block_attr.block_num; i++) {
        acac_static_regs_init(vi_pipe, &reg_cfg->alg_reg_cfg[i].acac_reg_cfg.static_reg_cfg);
        acac_usr_regs_init(&reg_cfg->alg_reg_cfg[i].acac_reg_cfg.usr_reg_cfg);
        acac_dyna_regs_init(&reg_cfg->alg_reg_cfg[i].acac_reg_cfg.dyna_reg_cfg);
        reg_cfg->alg_reg_cfg[i].acac_reg_cfg.acac_en = acac_ctx->mpi_cfg.en;
    }
    reg_cfg->cfg_key.bit1_acac_cfg = 1;
}

static td_void acac_ctx_def_comm_initialize(ot_isp_acac_attr *mpi_cfg)
{
    mpi_cfg->en          = TD_FALSE;
    mpi_cfg->detect_mode = ISP_ACAC_DETMODE_DEFAULT;
    mpi_cfg->op_type     = OT_OP_MODE_AUTO;
}

static td_void acac_ctx_def_auto_initialize(isp_acac_auto_attr *auto_attr)
{
    td_u8 i;
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        auto_attr->fcc_rb_strength[i]     = g_fcc_rb[i];
        auto_attr->edge_gain[i]           = g_edge_gain[i];
        auto_attr->fcc_y_strength[i]      = g_fcc_y[i];
        auto_attr->purple_upper_limit[i]  = g_det_pur_limit_h[i];
        auto_attr->purple_lower_limit[i]  = g_det_pur_limit_l[i];
        auto_attr->purple_alpha[i]        = g_purple_alpha[i];
        auto_attr->edge_alpha[i]          = g_edge_alpha[i];
        auto_attr->purple_sat_threshold[i] = g_det_pur_sat_thd[i];
        auto_attr->edge_threshold[0][i]    = g_edge_thd[0][i];
        auto_attr->edge_threshold[1][i]    = g_edge_thd[1][i];
    }
}

static td_void acac_ctx_def_manual_initialize(isp_acac_manual_attr *manual_attr)
{
    td_u8 i;
    manual_attr->fcc_rb_strength     = ISP_ACAC_ALPHARB_DEFAULT;
    manual_attr->edge_gain           = ISP_ACAC_GLOBALSTR_DEFAULT;
    manual_attr->fcc_y_strength      = ISP_EDETGREY_DEFAULT;
    manual_attr->purple_upper_limit  = FCF_CRCB_RATIO_H_LIMIT;
    manual_attr->purple_lower_limit  = FCF_CRCB_RATIO_L_LIMIT;
    manual_attr->purple_alpha        = PURPLE_ALPHA;
    manual_attr->edge_alpha          = EDGE_ALPHA;
    manual_attr->purple_sat_threshold = DET_SATU_THD;
    for (i = 0; i < OT_ISP_ACAC_THR_NUM; i++) {
        manual_attr->edge_threshold[i] = g_acac_lamda_thresh[i];
    }
}

static td_s32 isp_acac_ctx_initialize(ot_vi_pipe vi_pipe, isp_acac *acac_ctx)
{
    td_s32 ret;
    ot_isp_cmos_default *sns_dft = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);
    if (sns_dft->key.bit1_acac) {
        isp_check_pointer_return(sns_dft->acac);

        ret = acac_check_cmos_param(sns_dft->acac);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        (td_void)memcpy_s(&acac_ctx->mpi_cfg, sizeof(ot_isp_acac_attr),
                          sns_dft->acac, sizeof(ot_isp_acac_attr));
    } else {
        acac_ctx_def_comm_initialize(&acac_ctx->mpi_cfg);
        acac_ctx_def_auto_initialize(&acac_ctx->mpi_cfg.auto_attr);
        acac_ctx_def_manual_initialize(&acac_ctx->mpi_cfg.manual_attr);
    }

    return TD_SUCCESS;
}

static td_void acac_read_ext_regs(ot_vi_pipe vi_pipe, isp_acac *acac_ctx)
{
    acac_ctx->coef_update_en = ot_ext_system_acac_coef_update_read(vi_pipe);
    if (acac_ctx->coef_update_en != TD_TRUE) {
        return;
    }

    ot_ext_system_acac_coef_update_write(vi_pipe, TD_FALSE);
    acac_ctx->mpi_cfg.op_type     = ot_ext_system_acac_op_type_read(vi_pipe);
    acac_ctx->mpi_cfg.detect_mode = ot_ext_system_acac_det_mode_read(vi_pipe);
    acac_ctx->mpi_cfg.en          = ot_ext_system_acac_enable_read(vi_pipe);

    if (acac_ctx->mpi_cfg.op_type == OT_OP_MODE_MANUAL) {
        isp_acac_manual_attr_read(vi_pipe, &acac_ctx->mpi_cfg.manual_attr);
    } else {
        isp_acac_auto_attr_read(vi_pipe, &acac_ctx->mpi_cfg.auto_attr);
    }
}

static td_void acac_dyn_reg_update(ot_vi_pipe vi_pipe, td_u32 iso, isp_acac_dyna_cfg *dyn_cfg)
{
    td_u16 edge_thd_mul;
    td_u8 iso_index_up, iso_index_low;
    td_u32 iso1, iso2;
    ot_unused(vi_pipe);

    iso_index_up  = get_iso_index(iso);
    iso_index_low = MAX2((td_s8)iso_index_up - 1, 0);
    iso1          = get_iso(iso_index_low);
    iso2          = get_iso(iso_index_up);

    dyn_cfg->edge_thd0 = (td_u16)linear_inter(iso, iso1, g_cac_th_rb[0][iso_index_low],
                                              iso2, g_cac_th_rb[0][iso_index_up]);
    dyn_cfg->edge_thd1 = (td_u16)linear_inter(iso, iso1, g_cac_th_rb[1][iso_index_low],
                                              iso2, g_cac_th_rb[1][iso_index_up]);
    dyn_cfg->var_thd   = (td_u16)linear_inter(iso, iso1, g_acac_var_thresh[iso_index_low],
                                              iso2, g_acac_var_thresh[iso_index_up]);
    if (dyn_cfg->edge_thd0 <= dyn_cfg->edge_thd1) {
        edge_thd_mul = 0;
    } else {
        edge_thd_mul = (0x100 << ACAC_LAMDA_PRECS) / (dyn_cfg->edge_thd1 - dyn_cfg->edge_thd0);
    }
    dyn_cfg->edge_thd_mul = edge_thd_mul;
    dyn_cfg->dyna_resh    = TD_TRUE;
}

static td_void acac_user_reg_update(ot_vi_pipe vi_pipe, td_u32 iso, isp_acac_usr_cfg *user_cfg)
{
    td_u16 lamda_mul;
    ot_unused(iso);

    isp_acac *acac_ctx;
    acac_get_ctx(vi_pipe, acac_ctx);
    if (acac_ctx->mpi_cfg.detect_mode == 0) { // mode 0
        user_cfg->cac_det_mode  = acac_ctx->mpi_cfg.detect_mode;
    } else { // mode 1 for logic register is 3
        user_cfg->cac_det_mode  = MIN2(3, acac_ctx->mpi_cfg.detect_mode + 2); // mode 3 = 1 add 2
    }
    user_cfg->cac_str       = acac_ctx->actual.edge_gain;
    user_cfg->lamda_thd0    = acac_ctx->actual.edge_threshold[0];
    user_cfg->lamda_thd1    = acac_ctx->actual.edge_threshold[1];
    user_cfg->alpha_r       = acac_ctx->actual.fcc_rb_strength;
    user_cfg->color_dif_thd = acac_ctx->actual.fcc_y_strength;
    user_cfg->grey_thd      = acac_ctx->actual.fcc_y_strength;
    user_cfg->edge_alpha    = acac_ctx->actual.edge_alpha;
    user_cfg->purple_alpha  = acac_ctx->actual.purple_alpha;
    user_cfg->det_satu_thr  = acac_ctx->actual.purple_sat_threshold;
    user_cfg->fcf_cr_cb_ratio_high_limit = acac_ctx->actual.purple_upper_limit;
    user_cfg->fcf_cr_cb_ratio_low_limit  = acac_ctx->actual.purple_lower_limit;

    if (user_cfg->lamda_thd0 <= user_cfg->lamda_thd1) {
        lamda_mul = 0;
    } else {
        lamda_mul = (0x100 << ACAC_LAMDA_PRECS) / (user_cfg->lamda_thd1 - user_cfg->lamda_thd0);
    }
    user_cfg->lamda_mul                    = lamda_mul;
    user_cfg->alpha_b                      = user_cfg->alpha_r;
    user_cfg->fcf_cr_cb_ratio_high_limit2  = user_cfg->fcf_cr_cb_ratio_high_limit + FCC_PURPLE_H_OFFSET;
    user_cfg->fcf_cr_cb_ratio_low_limit2   = user_cfg->fcf_cr_cb_ratio_low_limit + FCC_PURPLE_L_OFFSET;
    user_cfg->usr_resh       = TD_TRUE;
    user_cfg->update_index  += 1;
}

static td_void isp_acac_get_mpi_auto_inter_result(td_u32 iso, isp_acac *acac_ctx)
{
    td_u8 iso_index_up, iso_index_low;
    td_u32 iso1, iso2;
    isp_acac_auto_attr   *auto_attr = &acac_ctx->mpi_cfg.auto_attr;
    isp_acac_manual_attr *actual    = &acac_ctx->actual;

    iso_index_up  = get_iso_index(iso);
    iso_index_low = MAX2((td_s8)iso_index_up - 1, 0);
    iso1 = get_iso(iso_index_low);
    iso2 = get_iso(iso_index_up);

    actual->edge_gain        = (td_u16)linear_inter(iso, iso1, auto_attr->edge_gain[iso_index_low],
                                                    iso2, auto_attr->edge_gain[iso_index_up]);
    actual->edge_threshold[0] = (td_u16)linear_inter(iso, iso1, auto_attr->edge_threshold[0][iso_index_low],
                                                     iso2, auto_attr->edge_threshold[0][iso_index_up]);
    actual->edge_threshold[1] = (td_u16)linear_inter(iso, iso1, auto_attr->edge_threshold[1][iso_index_low],
                                                     iso2, auto_attr->edge_threshold[1][iso_index_up]);
    actual->fcc_rb_strength  = (td_u16)linear_inter(iso, iso1, auto_attr->fcc_rb_strength[iso_index_low],
                                                    iso2, auto_attr->fcc_rb_strength[iso_index_up]);
    actual->fcc_y_strength   = (td_u16)linear_inter(iso, iso1, auto_attr->fcc_y_strength[iso_index_low],
                                                    iso2, auto_attr->fcc_y_strength[iso_index_up]);
    actual->edge_alpha       = (td_u16)linear_inter(iso, iso1, auto_attr->edge_alpha[iso_index_low],
                                                    iso2, auto_attr->edge_alpha[iso_index_up]);
    actual->purple_alpha     = (td_u16)linear_inter(iso, iso1, auto_attr->purple_alpha[iso_index_low],
                                                    iso2, auto_attr->purple_alpha[iso_index_up]);
    actual->purple_sat_threshold = (td_u16)linear_inter(iso, iso1, auto_attr->purple_sat_threshold[iso_index_low],
                                                        iso2, auto_attr->purple_sat_threshold[iso_index_up]);
    actual->purple_upper_limit = (td_u16)linear_inter(iso, iso1, auto_attr->purple_upper_limit[iso_index_low],
                                                      iso2, auto_attr->purple_upper_limit[iso_index_up]);
    actual->purple_lower_limit = (td_u16)linear_inter(iso, iso1, auto_attr->purple_lower_limit[iso_index_low],
                                                      iso2, auto_attr->purple_lower_limit[iso_index_up]);
}

static td_void isp_acac_actual_calc(td_u32 iso, isp_acac *acac_ctx)
{
    if (acac_ctx->mpi_cfg.op_type == OT_OP_MODE_MANUAL) {
        (td_void)memcpy_s(&acac_ctx->actual, sizeof(isp_acac_manual_attr),
            &acac_ctx->mpi_cfg.manual_attr, sizeof(isp_acac_manual_attr));
    } else {
        isp_acac_get_mpi_auto_inter_result(iso, acac_ctx);
    }
}

static td_void isp_acac_fw(td_u32 iso, ot_vi_pipe vi_pipe, td_u8 cur_blk, isp_acac_reg_cfg *acac_reg_cfg)
{
    ot_unused(cur_blk);

    /* update dyna regs */
    acac_dyn_reg_update(vi_pipe, iso, &acac_reg_cfg->dyna_reg_cfg);
    /* update user regs */
    acac_user_reg_update(vi_pipe, iso, &acac_reg_cfg->usr_reg_cfg);
}

static td_void isp_acac_reg_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_acac *acac_ctx)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx  = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    isp_acac_actual_calc(isp_ctx->linkage.iso, acac_ctx);

    for (i = 0; i < isp_ctx->block_attr.block_num; i++) {
        isp_acac_fw(isp_ctx->linkage.iso, vi_pipe, i, &reg_cfg->alg_reg_cfg[i].acac_reg_cfg);
    }
}

static __inline td_bool  check_acac_open(const isp_acac *acac)
{
    return (acac->mpi_cfg.en == TD_TRUE);
}

static td_s32 isp_acac_param_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_acac *acac_ctx)
{
    td_s32 ret;
    acac_ctx->init = TD_FALSE;
    ret = isp_acac_ctx_initialize(vi_pipe, acac_ctx);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    acac_regs_init(vi_pipe, reg_cfg, acac_ctx);
    acac_ext_regs_init(vi_pipe, acac_ctx);

    acac_ctx->init = TD_TRUE;
    ot_ext_system_isp_acac_init_status_write(vi_pipe, acac_ctx->init);
    return TD_SUCCESS;
}

static td_s32 isp_acac_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_s32 ret;
    isp_acac *acac_ctx = TD_NULL;
    ot_ext_system_isp_acac_init_status_write(vi_pipe, TD_FALSE);
    ret = acac_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    acac_get_ctx(vi_pipe, acac_ctx);
    isp_check_pointer_return(acac_ctx);

    return isp_acac_param_init(vi_pipe, (isp_reg_cfg *)reg_cfg, acac_ctx);
}

static td_s32 isp_acac_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv)
{
    td_u8 i;
    ot_isp_alg_mod alg_mod = OT_ISP_ALG_ACAC;
    isp_usr_ctx *isp_ctx  = TD_NULL;
    isp_acac    *acac_ctx = TD_NULL;
    isp_reg_cfg *reg      = (isp_reg_cfg *)reg_cfg;

    ot_unused(stat_info);
    ot_unused(rsv);

    isp_get_ctx(vi_pipe, isp_ctx);
    acac_get_ctx(vi_pipe, acac_ctx);
    isp_check_pointer_return(acac_ctx);

    if (isp_ctx->linkage.defect_pixel) {
        return TD_SUCCESS;
    }

    ot_ext_system_isp_acac_init_status_write(vi_pipe, acac_ctx->init);
    if (acac_ctx->init != TD_TRUE) {
        ioctl(isp_get_fd(vi_pipe), ISP_ALG_INIT_ERR_INFO_PRINT, &alg_mod);
        return TD_SUCCESS;
    }

    acac_ctx->mpi_cfg.en = ot_ext_system_acac_enable_read(vi_pipe);

    for (i = 0; i < reg->cfg_num; i++) {
        reg->alg_reg_cfg[i].acac_reg_cfg.acac_en = acac_ctx->mpi_cfg.en;
    }

    reg->cfg_key.bit1_acac_cfg = 1;

    /* check hardware setting */
    if (!check_acac_open(acac_ctx)) {
        return TD_SUCCESS;
    }

    acac_read_ext_regs(vi_pipe, acac_ctx);

    isp_acac_reg_update(vi_pipe, reg_cfg, acac_ctx);

    return TD_SUCCESS;
}

static td_s32 acac_proc_write(ot_vi_pipe vi_pipe, ot_isp_ctrl_proc_write *proc, const isp_acac *acac_ctx)
{
    ot_isp_ctrl_proc_write  proc_tmp;
    ot_unused(vi_pipe);

    if ((proc->proc_buff == TD_NULL) || (proc->buff_len == 0)) {
        return TD_FAILURE;
    }

    proc_tmp.proc_buff = proc->proc_buff;
    proc_tmp.buff_len  = proc->buff_len;

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "-----acac info-----------------------------------\n");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16s" "%16s" "%16s" "%16s" "%16s" "%16s\n",
                    "enable", "edge_thr[0]", "edge_thr[1]", "edge_gain", "purple_upper", "purple_lower");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16u" "%16u" "%16u" "%16u" "%16d" "%16d\n",
                    acac_ctx->mpi_cfg.en, acac_ctx->actual.edge_threshold[0x0],
                    acac_ctx->actual.edge_threshold[0x1], acac_ctx->actual.edge_gain,
                    acac_ctx->actual.purple_upper_limit, acac_ctx->actual.purple_lower_limit);

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16s" "%16s" "%16s" "%16s" "%16s\n",
                    "purple_sat_thr", "purple_alpha", "edge_alpha", "fcc_y_str", "fcc_rb_str");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16u" "%16u" "%16u" "%16u" "%16u\n",
                    acac_ctx->actual.purple_sat_threshold, acac_ctx->actual.purple_alpha, acac_ctx->actual.edge_alpha,
                    acac_ctx->actual.fcc_y_strength, acac_ctx->actual.fcc_rb_strength);

    proc->write_len += 1;
    return TD_SUCCESS;
}

static td_s32 isp_acac_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    isp_acac *acac_ctx = TD_NULL;

    switch (cmd) {
        case OT_ISP_PROC_WRITE:
            acac_get_ctx(vi_pipe, acac_ctx);
            isp_check_pointer_return(acac_ctx);
            acac_proc_write(vi_pipe, (ot_isp_ctrl_proc_write *)value, (const isp_acac *)acac_ctx);
            break;
        default:
            break;
    }

    return TD_SUCCESS;
}

static td_s32 isp_acac_exit(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_reg_cfg_attr  *reg_cfg    = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, reg_cfg);
    ot_ext_system_isp_acac_init_status_write(vi_pipe, TD_FALSE);
    for (i = 0; i < reg_cfg->reg_cfg.cfg_num; i++) {
        reg_cfg->reg_cfg.alg_reg_cfg[i].acac_reg_cfg.acac_en = TD_FALSE;
    }

    reg_cfg->reg_cfg.cfg_key.bit1_acac_cfg = 1;

    acac_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_alg_register_acac(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx  *isp_ctx = TD_NULL;
    isp_alg_node *algs    = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_acac);
    algs = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_return(algs);

    algs->alg_type = OT_ISP_ALG_ACAC;
    algs->alg_func.pfn_alg_init = isp_acac_init;
    algs->alg_func.pfn_alg_run  = isp_acac_run;
    algs->alg_func.pfn_alg_ctrl = isp_acac_ctrl;
    algs->alg_func.pfn_alg_exit = isp_acac_exit;
    algs->used = TD_TRUE;

    return TD_SUCCESS;
}
