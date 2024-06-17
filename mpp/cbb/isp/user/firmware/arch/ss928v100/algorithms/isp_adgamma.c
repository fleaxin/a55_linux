/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <math.h>
#include "isp_alg.h"
#include "isp_config.h"
#include "isp_ext_config.h"
#include "isp_math_utils.h"
#include "isp_ext_reg_access.h"
#include "isp_gammalut.h"

static const td_u16 g_adgamma_seg_max_val[AGAMMA_SEG_NUM] = {0, 2,  4,  8, 16,  32,  64, 128};
static const td_u8 g_adgamma_seg_idx_base[AGAMMA_SEG_NUM] = {0, 0, 32, 64, 96, 128, 160, 192};
typedef struct {
    td_u32 pre_scale;
    td_u32 adgamma_lut[OT_ISP_AGAMMA_NODE_NUM];
    ot_isp_agamma_attr mpi_cfg;
} isp_adgamma;

#define OUTPUT_BITS 20
#define FIXED_NUM 1024.0
isp_adgamma *g_adgamma_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};
#define adgamma_get_ctx(dev, ctx)   ((ctx) = g_adgamma_ctx[dev])
#define adgamma_set_ctx(dev, ctx)   (g_adgamma_ctx[dev] = (ctx))
#define adgamma_reset_ctx(dev)      (g_adgamma_ctx[dev] = TD_NULL)

static td_s32 isp_adgamma_attr_check(const char *src, const isp_adgamma *adgamma_attr)
{
    td_u32 i;
    for (i = 0; i < OT_ISP_AGAMMA_NODE_NUM; i++) {
        if (adgamma_attr->adgamma_lut[i] > OT_ISP_ADGAMMA_LUT_MAX) {
            isp_err_trace("Err %s adgamma adgamma_lut[%d]: %d!\n", src, i, adgamma_attr->adgamma_lut[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 adgamma_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_adgamma *adgamma_ctx = TD_NULL;

    adgamma_get_ctx(vi_pipe, adgamma_ctx);

    if (adgamma_ctx == TD_NULL) {
        adgamma_ctx = (isp_adgamma *)malloc(sizeof(isp_adgamma));
        if (adgamma_ctx == TD_NULL) {
            isp_err_trace("isp[%d] adgamma_ctx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }
    (td_void)memset_s(adgamma_ctx, sizeof(isp_adgamma), 0, sizeof(isp_adgamma)); /* 0 */
    adgamma_set_ctx(vi_pipe, adgamma_ctx);
    return TD_SUCCESS;
}
static td_s32 adgamma_initialize(ot_vi_pipe vi_pipe, isp_adgamma *adgamma_ctx)
{
    td_s32 ret;
    ot_unused(vi_pipe);
    adgamma_ctx->mpi_cfg.enable = TD_FALSE;
    adgamma_ctx->mpi_cfg.scale = FIXED_NUM; /* 1024 */
    adgamma_ctx->pre_scale = FIXED_NUM; /* 1024 */
    td_float scale =  (td_float)adgamma_ctx->mpi_cfg.scale / FIXED_NUM; /* 1024 */
    gen_lut(adgamma_ctx->adgamma_lut, OT_ISP_AGAMMA_NODE_NUM, OUTPUT_BITS, scale);
    ret = isp_adgamma_attr_check("gen LUT", adgamma_ctx);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}
static td_void adgamma_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *isp_cfg, isp_adgamma *adgamma_ctx)
{
    td_u16 i, j;
    ot_unused(vi_pipe);
    for (i = 0; i < isp_cfg->cfg_num; i++) {
        for (j = 0; j < AGAMMA_SEG_NUM; j++) {
            isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.static_reg_cfg.seg_idx_base[j] = g_adgamma_seg_idx_base[j];
            isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.static_reg_cfg.seg_max_val[j] = g_adgamma_seg_max_val[j];
        }
        for (j = 0; j < OT_ISP_AGAMMA_NODE_NUM; j++) {
            isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.dyna_reg_cfg.adgamma_lut[j] = adgamma_ctx->adgamma_lut[j];
        }
        isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.enable = adgamma_ctx->mpi_cfg.enable;
        isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.static_reg_cfg.resh = TD_TRUE;
        isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.dyna_reg_cfg.buf_id = 0;
        isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
    }

    isp_cfg->cfg_key.bit1_adgamma_cfg = 1;
}
static td_void adgamma_ext_regs_init(ot_vi_pipe vi_pipe, isp_adgamma *adgamma_ctx)
{
    isp_agamma_attr_write(vi_pipe, &adgamma_ctx->mpi_cfg);
}
static td_s32 isp_adgamma_param_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_adgamma *adgamma_ctx)
{
    td_s32  ret;
    /* initiate struct used in this firmware */
    ret = adgamma_initialize(vi_pipe, adgamma_ctx);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* initiate logic registers */
    adgamma_regs_initialize(vi_pipe, reg_cfg, adgamma_ctx);

    /* initiate external registers */
    adgamma_ext_regs_init(vi_pipe, adgamma_ctx);

    return TD_SUCCESS;
}
static td_s32 isp_adgamma_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_s32  ret;
    isp_adgamma *adgamma_ctx = TD_NULL;

    /* first initiate adgamma_ctx, using new template */
    ret = adgamma_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    adgamma_get_ctx(vi_pipe, adgamma_ctx);
    isp_check_pointer_success_return(adgamma_ctx);

    return isp_adgamma_param_init(vi_pipe, (isp_reg_cfg *)reg_cfg, adgamma_ctx);
}
static td_void isp_adgamma_reg_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_adgamma *adgamma_ctx)
{
    td_u32 i, j;
    ot_unused(vi_pipe);
    for (i = 0; i < reg_cfg->cfg_num; i++) {
        for (j = 0; j < OT_ISP_AGAMMA_NODE_NUM; j++) {
            reg_cfg->alg_reg_cfg[i].adgamma_reg_cfg.dyna_reg_cfg.adgamma_lut[j] = adgamma_ctx->adgamma_lut[j];
            reg_cfg->alg_reg_cfg[i].adgamma_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
        }
    }
    return;
}

static td_bool check_adgamma_open(const isp_adgamma *adgamma)
{
    return (adgamma->mpi_cfg.enable == TD_TRUE);
}
static td_s32 isp_adgamma_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv)
{
    td_u8 i;
    ot_unused(rsv);
    ot_unused(stat_info);

    td_s32 ret;
    isp_reg_cfg *isp_cfg = (isp_reg_cfg *)reg_cfg;
    isp_adgamma *adgamma_ctx = TD_NULL;
    isp_usr_ctx *isp_ctx  = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    adgamma_get_ctx(vi_pipe, adgamma_ctx);
    isp_check_pointer_success_return(adgamma_ctx);

    if (isp_ctx->linkage.stat_ready == TD_FALSE) {
        return TD_SUCCESS;
    }

    adgamma_ctx->mpi_cfg.enable = ot_ext_system_adgamma_enable_read(vi_pipe);

    for (i = 0; i < isp_cfg->cfg_num; i++) { /* 0 */
        isp_cfg->alg_reg_cfg[i].adgamma_reg_cfg.enable = adgamma_ctx->mpi_cfg.enable;
    }

    isp_cfg->cfg_key.bit1_adgamma_cfg = 1; /* 1 */

    /* check hardware setting */
    if (!check_adgamma_open(adgamma_ctx)) {
        return TD_SUCCESS;
    }

    /* update control regs; */
    adgamma_ctx->mpi_cfg.scale = ot_ext_system_adgamma_scale_read(vi_pipe);
    if (adgamma_ctx->mpi_cfg.scale < OT_ISP_SCALE_MIN || adgamma_ctx->mpi_cfg.scale > OT_ISP_SCALE_MAX) {
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    if (adgamma_ctx->pre_scale != adgamma_ctx->mpi_cfg.scale) {
        adgamma_ctx->pre_scale = adgamma_ctx->mpi_cfg.scale;
        td_float scale =  (td_float)adgamma_ctx->mpi_cfg.scale / FIXED_NUM;  /* 1024.0 */
        gen_lut(adgamma_ctx->adgamma_lut, OT_ISP_AGAMMA_NODE_NUM, OUTPUT_BITS, scale);
        ret = isp_adgamma_attr_check("gen LUT", adgamma_ctx);
        if (ret != TD_SUCCESS) {
            return TD_FAILURE;
        }
        isp_adgamma_reg_update(vi_pipe, isp_cfg, adgamma_ctx);
    }

    return TD_SUCCESS;
}
static td_s32 isp_adgamma_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    ot_unused(vi_pipe);
    ot_unused(value);
    switch (cmd) {
        default:
            break;
    }

    return TD_SUCCESS;
}
static td_void adgamma_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_adgamma *adgamma_ctx = TD_NULL;

    adgamma_get_ctx(vi_pipe, adgamma_ctx);
    isp_free(adgamma_ctx);
    adgamma_reset_ctx(vi_pipe);

    return;
}
static td_s32 isp_adgamma_exit(ot_vi_pipe vi_pipe)
{
    isp_adgamma *adgamma_ctx = TD_NULL;

    adgamma_get_ctx(vi_pipe, adgamma_ctx);
    isp_check_pointer_success_return(adgamma_ctx);

    adgamma_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}
td_s32 isp_alg_register_adgamma(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx  *isp_ctx  = TD_NULL;
    isp_alg_node *alg_node = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_adgamma);
    alg_node = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_success_return(alg_node);

    alg_node->alg_type = OT_ISP_ALG_ADGAMMA;
    alg_node->alg_func.pfn_alg_init = isp_adgamma_init;
    alg_node->alg_func.pfn_alg_run  = isp_adgamma_run;
    alg_node->alg_func.pfn_alg_ctrl = isp_adgamma_ctrl;
    alg_node->alg_func.pfn_alg_exit = isp_adgamma_exit;
    alg_node->used = TD_TRUE;

    return TD_SUCCESS;
}
