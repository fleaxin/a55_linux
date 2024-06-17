/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <math.h>
#include "isp_alg.h"
#include "isp_sensor.h"
#include "isp_config.h"
#include "isp_ext_config.h"
#include "isp_math_utils.h"
#include "isp_param_check.h"
#include "isp_ext_reg_access.h"
#include "isp_proc.h"

#define RGBIR_CVTMATRIX_NUM 12
#define EXPTH1_DFT          1500
#define EXPTH2_DFT          75
#define MAX_EXP_CTL_VALUE   2050
#define MAX_DEPTH           12
#define RGBIR_TH_DFT        100
#define RGBIR_TV_DFT        100
#define ISP_ALPHA_DFT_VALUE 100
#define ISP_BETA_DFT_VALUE  300
#define RGBIR_GAIN_DFT      256
#define IR_NORM_DEPTH       8
#define CROSS_TALK_DFT      0
#define CROSS_TALK_LUT_DFT  1024
#define WB_GAIN_DFT         1024
#define WB_CTRL_DFT         0
#define AUTO_GAIN_DFT       256
#define RGBIR_MODE_DFT      0

typedef struct {
    td_bool init;
    td_bool rgbir_attr_update;
    td_u16  exp_ctrl1;
    td_u16  exp_ctrl2;
    td_u16  reci_exp1;
    td_u16  reci_exp2;
    td_u16  thd_alpha;
    td_u16  thd_beta;

    td_u16  pre_auto_gain; /* to save pre frame gain value */

    ot_isp_rgbir_attr mpi_cfg;
} isp_rgbir;

isp_rgbir *g_rgbir_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

#define rgbir_get_ctx(dev, ctx)   ((ctx) = g_rgbir_ctx[dev])
#define rgbir_set_ctx(dev, ctx)   (g_rgbir_ctx[dev] = (ctx))
#define rgbir_reset_ctx(dev)      (g_rgbir_ctx[dev] = TD_NULL)

static const td_s16 g_dft_cvt_matrix[RGBIR_CVTMATRIX_NUM] = {1000, 0, 0, -100, 0, 1000, 0, -100, 0, 0, 1000, -100};

static td_s32 rgbir_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_rgbir *rgbir_ctx = TD_NULL;

    rgbir_get_ctx(vi_pipe, rgbir_ctx);

    if (rgbir_ctx == TD_NULL) {
        rgbir_ctx = (isp_rgbir *)malloc(sizeof(isp_rgbir));
        if (rgbir_ctx == TD_NULL) {
            isp_err_trace("isp[%d] rgbir_ctx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }
    (td_void)memset_s(rgbir_ctx, sizeof(isp_rgbir), 0, sizeof(isp_rgbir)); /* 0 */
    rgbir_set_ctx(vi_pipe, rgbir_ctx);
    return TD_SUCCESS;
}

static td_void rgbir_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_rgbir *rgbir_ctx = TD_NULL;

    rgbir_get_ctx(vi_pipe, rgbir_ctx);
    isp_free(rgbir_ctx);
    rgbir_reset_ctx(vi_pipe);

    return;
}

static td_void rgbir_ext_regs_init(ot_vi_pipe vi_pipe, isp_rgbir *rgbir_ctx)
{
    isp_rgbir_attr_write(vi_pipe, &rgbir_ctx->mpi_cfg);

    ot_ext_system_rgbir_attr_update_write(vi_pipe, rgbir_ctx->rgbir_attr_update);
}

static td_void rgbir_exp_ctrl_cal(ot_vi_pipe vi_pipe, isp_rgbir *rgbir_ctx, td_u16 exp_th1, td_u16 exp_th2)
{
    td_u16 blc, blc00, blc01, blc10, blc11;
    td_u16  exp_ctrl1, exp_ctrl2;
    blc00 = ot_ext_system_black_level_query_f0_r_read(vi_pipe) >> 2; /* right shift 2bits to 12bits */
    blc01 = ot_ext_system_black_level_query_f0_gr_read(vi_pipe) >> 2; /* right shift 2bits to 12bits */
    blc10 = ot_ext_system_black_level_query_f0_gb_read(vi_pipe) >> 2; /* right shift 2bits to 12bits */
    blc11 = ot_ext_system_black_level_query_f0_b_read(vi_pipe) >> 2; /* right shift 2bits to 12bits */
    blc = (blc00 + blc01 + blc10 + blc11 + 2) >> 2; /* used to calculate average value, (sum + 2) / 4 */

    exp_ctrl1 = MAX2(MAX_EXP_CTL_VALUE, (((1 << MAX_DEPTH) - 1) - (MIN2((blc + exp_th1), ((1 << MAX_DEPTH) - 1)))));
    exp_ctrl2 = MAX2(MAX_EXP_CTL_VALUE, (((1 << MAX_DEPTH) - 1) - (MIN2((blc + exp_th2), ((1 << MAX_DEPTH) - 1)))));

    rgbir_ctx->exp_ctrl1 = exp_ctrl1;
    rgbir_ctx->exp_ctrl2 = exp_ctrl2;

    rgbir_ctx->reci_exp1 = ((((1 << MAX_DEPTH) - 1) << IR_NORM_DEPTH) + (exp_ctrl1 >> 1)) / div_0_to_1(exp_ctrl1);
    rgbir_ctx->reci_exp2 = ((((1 << MAX_DEPTH) - 1) << IR_NORM_DEPTH) + (exp_ctrl2 >> 1)) / div_0_to_1(exp_ctrl2);
    return;
}

static td_void rgbir_read_ext_regs(ot_vi_pipe vi_pipe, isp_rgbir *rgbir_ctx)
{
    rgbir_ctx->rgbir_attr_update = ot_ext_system_rgbir_attr_update_read(vi_pipe);
    if (rgbir_ctx->rgbir_attr_update != TD_TRUE) {
        return;
    }

    ot_ext_system_rgbir_attr_update_write(vi_pipe, TD_FALSE);
    isp_rgbir_attr_read(vi_pipe, &rgbir_ctx->mpi_cfg);

    rgbir_exp_ctrl_cal(vi_pipe, rgbir_ctx, rgbir_ctx->mpi_cfg.exp_ctrl[0], rgbir_ctx->mpi_cfg.exp_ctrl[1]);
}

static td_void rgbir_init_from_default(ot_vi_pipe vi_pipe, isp_rgbir *rgbir_ctx, isp_usr_ctx *isp_ctx)
{
    ot_unused(vi_pipe);
    (td_void)memcpy_s(rgbir_ctx->mpi_cfg.cvt_matrix, RGBIR_CVTMATRIX_NUM * sizeof(td_s16),
                      g_dft_cvt_matrix, RGBIR_CVTMATRIX_NUM * sizeof(td_s16));
    rgbir_ctx->mpi_cfg.rgbir_en = TD_FALSE;
    rgbir_ctx->mpi_cfg.exp_ctrl[0] = EXPTH1_DFT;
    rgbir_ctx->mpi_cfg.exp_ctrl[1] = EXPTH2_DFT;
    rgbir_ctx->mpi_cfg.rgbir_cfg.mode = RGBIR_MODE_DFT;
    rgbir_ctx->mpi_cfg.smooth_en = TD_TRUE;
    rgbir_ctx->mpi_cfg.rgbir_cfg.in_rgbir_pattern = OT_ISP_IRBAYER_RGBGI;
    rgbir_ctx->mpi_cfg.rgbir_cfg.out_pattern = (td_u8)isp_ctx->bayer;
    rgbir_ctx->mpi_cfg.ir_cvtmat_mode = OT_ISP_IR_CVTMAT_MODE_NORMAL;
    rgbir_ctx->mpi_cfg.exp_gain[0] = RGBIR_GAIN_DFT;
    rgbir_ctx->mpi_cfg.exp_gain[1] = RGBIR_GAIN_DFT;
    rgbir_ctx->mpi_cfg.rgbir_cfg.out_pattern = 0; /* 0 */
    rgbir_ctx->mpi_cfg.ir_rm_en = TD_FALSE;
    rgbir_ctx->mpi_cfg.ir_rm_ratio[0] = CROSS_TALK_DFT; /* 0 */
    rgbir_ctx->mpi_cfg.ir_rm_ratio[1] = CROSS_TALK_DFT; /* 1 */
    rgbir_ctx->mpi_cfg.ir_rm_ratio[2] = CROSS_TALK_DFT; /* 2 */
    rgbir_ctx->mpi_cfg.ir_sum_info = 0; /* 0 */
    rgbir_ctx->mpi_cfg.wb_ctrl_strength[0] = WB_CTRL_DFT; /* 0 */
    rgbir_ctx->mpi_cfg.wb_ctrl_strength[1] = WB_CTRL_DFT; /* 1 */
    rgbir_ctx->mpi_cfg.auto_gain_en = TD_FALSE;
    rgbir_ctx->mpi_cfg.auto_gain = AUTO_GAIN_DFT;
    rgbir_ctx->mpi_cfg.rgbir_cfg.is_ir_upscale = TD_FALSE;
    rgbir_ctx->mpi_cfg.rgbir_cfg.in_bayer_pattern = (ot_isp_bayer_format)isp_ctx->bayer;
    return;
}

static td_s32 rgbir_initialize(ot_vi_pipe vi_pipe, isp_rgbir *rgbir_ctx)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    ot_isp_cmos_default *sns_dft = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);
    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_linear_mode(isp_ctx->sns_wdr_mode) && sns_dft->key.bit1_rgbir) {
        isp_check_pointer_return(sns_dft->rgbir);
        ret = isp_rgbir_attr_check("cmos", vi_pipe, sns_dft->rgbir);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        (td_void)memcpy_s(&rgbir_ctx->mpi_cfg, sizeof(ot_isp_rgbir_attr), sns_dft->rgbir, sizeof(ot_isp_rgbir_attr));
    } else {
        rgbir_init_from_default(vi_pipe, rgbir_ctx, isp_ctx);
    }
    rgbir_exp_ctrl_cal(vi_pipe, rgbir_ctx, rgbir_ctx->mpi_cfg.exp_ctrl[0], rgbir_ctx->mpi_cfg.exp_ctrl[1]);
    /* common config */
    rgbir_ctx->thd_alpha = ISP_ALPHA_DFT_VALUE * 128.0 / 100; /* convert from 100 to 128.0 */
    rgbir_ctx->thd_beta  = ISP_BETA_DFT_VALUE * 128.0 / 100; /* convert from 100 to 128.0 */
    rgbir_ctx->pre_auto_gain = AUTO_GAIN_DFT;
    rgbir_ctx->rgbir_attr_update = TD_FALSE;

    return TD_SUCCESS;
}

static td_void rgbir_static_regs_init(isp_rgbir_static_cfg *staticr_cfg)
{
    td_u32 i;
    staticr_cfg->thres_h = RGBIR_TH_DFT;
    staticr_cfg->thres_v = RGBIR_TV_DFT;
    for (i = 0; i < OT_ISP_RGBIR_LUT_NUM; i++) {
        staticr_cfg->crosstalk_lut[i] = CROSS_TALK_LUT_DFT;  /* fix 1024 */
    }

    staticr_cfg->static_resh = TD_TRUE;
}

static td_void rgbir_usr_reg_cfg(isp_rgbir_usr_cfg *usr_reg_cfg, isp_rgbir *rgbir_ctx)
{
    td_u8 j;
    const td_u16 norm_factor = 64; /* norm_factor 64 */
    const td_u16 cvt_factor  = 1000; /* cvt_factor 1000 */
    const td_u16 ct_factor = 1000; /* ct_factor 1000 */
    const td_u16 ct_bitdepth = (1 << 10); /* ct_bitdepth 1<<10 */

    usr_reg_cfg->mode          = rgbir_ctx->mpi_cfg.rgbir_cfg.mode; /* 0:rgbir mode; 1:ir remove mode */
    usr_reg_cfg->smooth_enable = rgbir_ctx->mpi_cfg.smooth_en;
    usr_reg_cfg->in_pattern    = rgbir_ctx->mpi_cfg.rgbir_cfg.in_rgbir_pattern;
    usr_reg_cfg->out_pattern   = rgbir_ctx->mpi_cfg.rgbir_cfg.out_pattern;
    usr_reg_cfg->exp_ctrl1 = rgbir_ctx->exp_ctrl1;
    usr_reg_cfg->exp_ctrl2 = rgbir_ctx->exp_ctrl2;
    usr_reg_cfg->reci_exp1 = rgbir_ctx->reci_exp1;
    usr_reg_cfg->reci_exp2 = rgbir_ctx->reci_exp2;
    usr_reg_cfg->gain_r    = rgbir_ctx->mpi_cfg.exp_gain[0];
    usr_reg_cfg->gain_b    = rgbir_ctx->mpi_cfg.exp_gain[1];

    usr_reg_cfg->thd_alpha = rgbir_ctx->thd_alpha;
    usr_reg_cfg->thd_beta  = rgbir_ctx->thd_beta;

    for (j = 0; j < RGBIR_CVTMATRIX_NUM; j++) {
        usr_reg_cfg->cvt_matrix[j] = (rgbir_ctx->mpi_cfg.cvt_matrix[j] * norm_factor) / cvt_factor;
    }

    usr_reg_cfg->out_type = 0;
    usr_reg_cfg->ir_rm_enable = rgbir_ctx->mpi_cfg.ir_rm_en;
    for (j = 0; j < OT_ISP_RGBIR_CROSSTALK_NUM; j++) {
        usr_reg_cfg->ir_rm_ratio[j] = (rgbir_ctx->mpi_cfg.ir_rm_ratio[j] * ct_bitdepth) / ct_factor;
    }

    for (j = 0; j < OT_ISP_RGBIR_CTRL_NUM; j++) {
        usr_reg_cfg->wb_ctrl[j] = 1024 - rgbir_ctx->mpi_cfg.wb_ctrl_strength[j];  /* inverse to 0~1024 */
    }
    usr_reg_cfg->is_ir_upscale = rgbir_ctx->mpi_cfg.rgbir_cfg.is_ir_upscale;
    usr_reg_cfg->in_bayer_pattern = rgbir_ctx->mpi_cfg.rgbir_cfg.in_bayer_pattern;
}

static td_void rgbir_user_regs_init(isp_rgbir_usr_cfg *usr_reg_cfg, isp_rgbir *rgbir_ctx)
{
    rgbir_usr_reg_cfg(usr_reg_cfg, rgbir_ctx);

    usr_reg_cfg->usr_resh = TD_TRUE;
    usr_reg_cfg->update_index = 1;
    return;
}

static td_void rgbir_dyna_regs_init(isp_rgbir_dyna_cfg *dyna_cfg, isp_rgbir *rgbir_ctx)
{
    td_u32 i;
    dyna_cfg->auto_gain_enable = rgbir_ctx->mpi_cfg.auto_gain_en;
    dyna_cfg->auto_gain = rgbir_ctx->mpi_cfg.auto_gain;

    for (i = 0; i < OT_ISP_RGBIR_CTRL_NUM; i++) {
        dyna_cfg->wb_gain[i] = WB_GAIN_DFT;
    }

    dyna_cfg->dyna_resh = TD_TRUE;
}

static td_void rgbir_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *isp_cfg, isp_rgbir *rgbir_ctx)
{
    ot_unused(vi_pipe);
    td_u16 i;
    for (i = 0; i < isp_cfg->cfg_num; i++) {
        rgbir_static_regs_init(&isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.static_reg_cfg);
        rgbir_user_regs_init(&isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.usr_reg_cfg, rgbir_ctx);
        rgbir_dyna_regs_init(&isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.dyna_reg_cfg, rgbir_ctx);
        isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.rgbir_enable = rgbir_ctx->mpi_cfg.rgbir_en;
    }

    isp_cfg->cfg_key.bit1_rgbir_cfg = 1;
}

static td_s32 isp_rgbir_param_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_rgbir *rgbir_ctx)
{
    td_s32  ret;
    rgbir_ctx->init = TD_FALSE;
    /* initiate struct used in this firmware */
    ret = rgbir_initialize(vi_pipe, rgbir_ctx);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* initiate logic registers */
    rgbir_regs_initialize(vi_pipe, reg_cfg, rgbir_ctx);

    /* initiate external registers */
    rgbir_ext_regs_init(vi_pipe, rgbir_ctx);
    rgbir_ctx->init = TD_TRUE;
    ot_ext_system_isp_rgbir_init_status_write(vi_pipe, rgbir_ctx->init);
    return TD_SUCCESS;
}

static td_s32 isp_rgbir_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_s32  ret;
    isp_rgbir *rgbir_ctx = TD_NULL;

    ot_ext_system_isp_rgbir_init_status_write(vi_pipe, TD_FALSE);
    /* first initiate rgbir_ctx, using new template */
    ret = rgbir_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    rgbir_get_ctx(vi_pipe, rgbir_ctx);
    isp_check_pointer_return(rgbir_ctx);

    return isp_rgbir_param_init(vi_pipe, (isp_reg_cfg *)reg_cfg, rgbir_ctx);
}

static td_void isp_rgbir_wdrmode_set(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_u8  i;
    td_s32 ret;
    td_u32 update_idx[OT_ISP_STRIPING_MAX_NUM] = { 0 }; /* 0 */
    isp_reg_cfg *isp_cfg = (isp_reg_cfg *)reg_cfg;

    for (i = 0; i < isp_cfg->cfg_num; i++) {
        update_idx[i] = isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.usr_reg_cfg.update_index;
    }

    ret = isp_rgbir_init(vi_pipe, reg_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("isp_rgbir_init failed!\n");
        return;
    }
    for (i = 0; i < isp_cfg->cfg_num; i++) {
        isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.usr_reg_cfg.update_index = update_idx[i] + 1;  /* 1 */
    }

    return;
}

__inline static td_bool check_rgbir_open(const isp_rgbir *rgbir_ctx)
{
    return (rgbir_ctx->mpi_cfg.rgbir_en == TD_TRUE);
}

static td_void isp_rgbir_usr_fw(isp_rgbir_usr_cfg *usr_reg_cfg, isp_rgbir *rgbir_ctx)
{
    rgbir_usr_reg_cfg(usr_reg_cfg, rgbir_ctx);

    usr_reg_cfg->usr_resh = TD_TRUE;
    usr_reg_cfg->update_index += 1; /* 1 */
    return;
}

static td_void isp_rgbir_dyna_fw(ot_vi_pipe vi_pipe, const isp_usr_ctx *isp_ctx, isp_rgbir_dyna_cfg *dyna_cfg,
    isp_rgbir *rgbir_ctx, isp_stat *stat_info)
{
    td_u32  cur_auto_gain; /* default 256 */
    /* awb gain control */
    td_u16 rgain_recp = MIN2(0x400, (0x4000000 / div_0_to_1(isp_ctx->linkage.white_balance_gain[0]))); /* 0:rgain */
    td_u16 bgain_recp = MIN2(0x400, (0x4000000 / div_0_to_1(isp_ctx->linkage.white_balance_gain[3]))); /* 3:bgain */

    dyna_cfg->wb_gain[0] = rgain_recp; /* 0 */
    dyna_cfg->wb_gain[1] = bgain_recp; /* 1 */
    if (is_online_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        cur_auto_gain = isp_rgbir_auto_gain_o_read(vi_pipe, 0);
        rgbir_ctx->mpi_cfg.ir_sum_info = isp_rgbir_ir_sum_read(vi_pipe, 0); /* 0 */
    } else {
        cur_auto_gain = stat_info->rgbir_stat.auto_gain;
        rgbir_ctx->mpi_cfg.ir_sum_info = stat_info->rgbir_stat.ir_sum_info; /* 0 */
    }

    /* auto gain control */
    dyna_cfg->auto_gain_enable = TD_TRUE; /* always true */
    if (rgbir_ctx->mpi_cfg.auto_gain_en == TD_FALSE) {
        dyna_cfg->auto_gain = rgbir_ctx->mpi_cfg.auto_gain;
    } else {
        rgbir_ctx->pre_auto_gain = (rgbir_ctx->pre_auto_gain * 7 + cur_auto_gain) >> 3;  /* (pre 7 + cur 1) /(1<<3) */
        dyna_cfg->auto_gain = rgbir_ctx->pre_auto_gain;
    }
    /* read ir sum value */
    ot_ext_system_rgbir_ir_sum_write(vi_pipe, rgbir_ctx->mpi_cfg.ir_sum_info);
    dyna_cfg->dyna_resh = TD_TRUE;
    return;
}

static td_void isp_rgbir_reg_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_rgbir *rgbir_ctx, isp_stat *stat_info)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx  = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        if (rgbir_ctx->rgbir_attr_update == TD_TRUE) {
            isp_rgbir_usr_fw(&reg_cfg->alg_reg_cfg[i].rgbir_reg_cfg.usr_reg_cfg, rgbir_ctx);
        }
        isp_rgbir_dyna_fw(vi_pipe, isp_ctx, &reg_cfg->alg_reg_cfg[i].rgbir_reg_cfg.dyna_reg_cfg, rgbir_ctx, stat_info);
    }
}

static td_s32 isp_rgbir_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv)
{
    td_u8 i;
    ot_isp_alg_mod alg_mod = OT_ISP_ALG_RGBIR;
    isp_reg_cfg *isp_cfg = (isp_reg_cfg *)reg_cfg;
    isp_rgbir *rgbir_ctx = TD_NULL;
    isp_usr_ctx *isp_ctx  = TD_NULL;

    ot_unused(rsv);
    isp_get_ctx(vi_pipe, isp_ctx);
    rgbir_get_ctx(vi_pipe, rgbir_ctx);
    isp_check_pointer_success_return(rgbir_ctx);

    if (isp_ctx->linkage.stat_ready == TD_FALSE) {
        return TD_SUCCESS;
    }

    ot_ext_system_isp_rgbir_init_status_write(vi_pipe, rgbir_ctx->init);
    if (rgbir_ctx->init != TD_TRUE) {
        ioctl(isp_get_fd(vi_pipe), ISP_ALG_INIT_ERR_INFO_PRINT, &alg_mod);
        return TD_SUCCESS;
    }

    rgbir_ctx->mpi_cfg.rgbir_en = ot_ext_system_rgbir_enable_read(vi_pipe);

    for (i = 0; i < isp_cfg->cfg_num; i++) { /* 0 */
        isp_cfg->alg_reg_cfg[i].rgbir_reg_cfg.rgbir_enable = rgbir_ctx->mpi_cfg.rgbir_en;
    }

    isp_cfg->cfg_key.bit1_rgbir_cfg = 1; /* 1 */

    /* check hardware setting */
    if (!check_rgbir_open(rgbir_ctx)) {
        return TD_SUCCESS;
    }

    /* update control regs; */
    rgbir_read_ext_regs(vi_pipe, rgbir_ctx);

    isp_rgbir_reg_update(vi_pipe, isp_cfg, rgbir_ctx, (isp_stat *)stat_info);

    return TD_SUCCESS;
}

static td_void rgbir_proc_write(ot_vi_pipe vi_pipe, ot_isp_ctrl_proc_write *proc)
{
    ot_isp_ctrl_proc_write proc_tmp;
    isp_rgbir *rgbir_ctx = TD_NULL;

    rgbir_get_ctx(vi_pipe, rgbir_ctx);
    isp_check_pointer_void_return(rgbir_ctx);

    if ((proc->proc_buff == TD_NULL) || (proc->buff_len == 0)) {
        return;
    }

    proc_tmp.proc_buff = proc->proc_buff;
    proc_tmp.buff_len = proc->buff_len;

    isp_proc_printf(&proc_tmp, proc->write_len, "-----rgbir info-----------------------------------\n");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16s" "%16s" "%16s" "%16s\n", "enable", "auto_gain_en", "smooth_en", "ir_rm_en");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16u" "%16u" "%16u" "%16u\n\n",
                    rgbir_ctx->mpi_cfg.rgbir_en, rgbir_ctx->mpi_cfg.auto_gain_en,
                    rgbir_ctx->mpi_cfg.smooth_en, rgbir_ctx->mpi_cfg.ir_rm_en);

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16s" "%16s" "%16s" "%16s" "%16s" "%16s\n", "in_rgbir_pattern", "out_pattern", "ir_sum_info",
                    "ir_rm_ratio[1]", "ir_rm_ratio[2]", "ir_rm_ratio[3]");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16u" "%16u" "%16u" "%16u" "%16u" "%16u\n\n",
                    rgbir_ctx->mpi_cfg.rgbir_cfg.in_rgbir_pattern, rgbir_ctx->mpi_cfg.rgbir_cfg.out_pattern,
                    rgbir_ctx->mpi_cfg.ir_sum_info, rgbir_ctx->mpi_cfg.ir_rm_ratio[0x0],
                    rgbir_ctx->mpi_cfg.ir_rm_ratio[0x1], rgbir_ctx->mpi_cfg.ir_rm_ratio[0x2]);

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%8s\n", "cvt_matrix:");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%8d" "%8d" "%8d" "%8d" "%8d" "%8d" "%8d" "%8d" "%8d" "%8d" "%8d" "%8d\n",
                    rgbir_ctx->mpi_cfg.cvt_matrix[0x0], rgbir_ctx->mpi_cfg.cvt_matrix[0x1],
                    rgbir_ctx->mpi_cfg.cvt_matrix[0x2], rgbir_ctx->mpi_cfg.cvt_matrix[0x3],
                    rgbir_ctx->mpi_cfg.cvt_matrix[0x4], rgbir_ctx->mpi_cfg.cvt_matrix[0x5],
                    rgbir_ctx->mpi_cfg.cvt_matrix[0x6], rgbir_ctx->mpi_cfg.cvt_matrix[0x7],
                    rgbir_ctx->mpi_cfg.cvt_matrix[0x8], rgbir_ctx->mpi_cfg.cvt_matrix[0x9],
                    rgbir_ctx->mpi_cfg.cvt_matrix[0xa], rgbir_ctx->mpi_cfg.cvt_matrix[0xb]);

    proc->write_len += 1;
}

static td_s32 isp_rgbir_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    isp_reg_cfg_attr *reg_cfg = TD_NULL;
    switch (cmd) {
        case OT_ISP_WDR_MODE_SET:
            isp_regcfg_get_ctx(vi_pipe, reg_cfg);
            isp_check_pointer_return(reg_cfg);
            isp_rgbir_wdrmode_set(vi_pipe, (td_void *)&reg_cfg->reg_cfg);
            break;
        case OT_ISP_PROC_WRITE:
            rgbir_proc_write(vi_pipe, (ot_isp_ctrl_proc_write *)value);
            break;
        default:
            break;
    }

    return TD_SUCCESS;
}

static td_s32 isp_rgbir_exit(ot_vi_pipe vi_pipe)
{
    isp_rgbir *rgbir_ctx = TD_NULL;
    ot_ext_system_isp_rgbir_init_status_write(vi_pipe, TD_FALSE);
    rgbir_get_ctx(vi_pipe, rgbir_ctx);
    isp_check_pointer_return(rgbir_ctx);

    rgbir_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_alg_register_rgbir(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx  *isp_ctx  = TD_NULL;
    isp_alg_node *alg_node = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_rgbir);
    alg_node = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_success_return(alg_node);

    alg_node->alg_type = OT_ISP_ALG_RGBIR;
    alg_node->alg_func.pfn_alg_init = isp_rgbir_init;
    alg_node->alg_func.pfn_alg_run  = isp_rgbir_run;
    alg_node->alg_func.pfn_alg_ctrl = isp_rgbir_ctrl;
    alg_node->alg_func.pfn_alg_exit = isp_rgbir_exit;
    alg_node->used = TD_TRUE;

    return TD_SUCCESS;
}
