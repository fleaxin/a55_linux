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

static const td_u16 g_agamma_seg_max_val[AGAMMA_SEG_NUM] = {0, 2,  4,  8, 16,  32,  64, 128};
static const td_u8 g_agamma_seg_idx_base[AGAMMA_SEG_NUM] = {0, 0, 32, 64, 96, 128, 160, 192};
typedef struct {
    td_u32 pre_scale;
    td_u32 agamma_lut[OT_ISP_AGAMMA_NODE_NUM];
    ot_isp_agamma_attr mpi_cfg;
} isp_agamma;

#define OUTPUT_BITS 20
#define FIXED_NUM 1024.0
isp_agamma *g_agamma_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};
#define agamma_get_ctx(dev, ctx)   ((ctx) = g_agamma_ctx[dev])
#define agamma_set_ctx(dev, ctx)   (g_agamma_ctx[dev] = (ctx))
#define agamma_reset_ctx(dev)      (g_agamma_ctx[dev] = TD_NULL)

static td_s32 isp_agamma_attr_check(const char *src, const isp_agamma *agamma_attr)
{
    td_u32 i;
    for (i = 0; i < OT_ISP_AGAMMA_NODE_NUM; i++) {
        if (agamma_attr->agamma_lut[i] > OT_ISP_AGAMMA_LUT_MAX) {
            isp_err_trace("Err %s agamma agamma_lut[%d]: %d!\n", src, i, agamma_attr->agamma_lut[i]);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

static td_s32 agamma_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_agamma *agamma_ctx = TD_NULL;

    agamma_get_ctx(vi_pipe, agamma_ctx);

    if (agamma_ctx == TD_NULL) {
        agamma_ctx = (isp_agamma *)malloc(sizeof(isp_agamma));
        if (agamma_ctx == TD_NULL) {
            isp_err_trace("isp[%d] agamma_ctx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }
    (td_void)memset_s(agamma_ctx, sizeof(isp_agamma), 0, sizeof(isp_agamma)); /* 0 */
    agamma_set_ctx(vi_pipe, agamma_ctx);
    return TD_SUCCESS;
}
static td_s32 agamma_initialize(ot_vi_pipe vi_pipe, isp_agamma *agamma_ctx)
{
    td_s32 ret;
    ot_unused(vi_pipe);
    agamma_ctx->mpi_cfg.enable = TD_FALSE;
    agamma_ctx->mpi_cfg.scale = FIXED_NUM; /* 1024 */
    agamma_ctx->pre_scale = FIXED_NUM; /* 1024 */
    const td_float scale = FIXED_NUM / agamma_ctx->mpi_cfg.scale; /* 1024 */
    gen_lut(agamma_ctx->agamma_lut, OT_ISP_AGAMMA_NODE_NUM, OUTPUT_BITS, scale);
    ret = isp_agamma_attr_check("gen LUT", agamma_ctx);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void agamma_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *isp_cfg, isp_agamma *agamma_ctx)
{
    td_u16 i, j;
    ot_unused(vi_pipe);
    for (i = 0; i < isp_cfg->cfg_num; i++) {
        for (j = 0; j < AGAMMA_SEG_NUM; j++) {
            isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.static_reg_cfg.seg_idx_base[j] = g_agamma_seg_idx_base[j];
            isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.static_reg_cfg.seg_max_val[j] = g_agamma_seg_max_val[j];
        }
        for (j = 0; j < OT_ISP_AGAMMA_NODE_NUM; j++) {
            isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.dyna_reg_cfg.agamma_lut[j] = agamma_ctx->agamma_lut[j];
        }
        isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.enable = agamma_ctx->mpi_cfg.enable;
        isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.static_reg_cfg.resh = TD_TRUE;
        isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.dyna_reg_cfg.buf_id = 0;
        isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
    }

    isp_cfg->cfg_key.bit1_agamma_cfg = 1;
}
static td_void agamma_ext_regs_init(ot_vi_pipe vi_pipe, isp_agamma *agamma_ctx)
{
    isp_agamma_attr_write(vi_pipe, &agamma_ctx->mpi_cfg);
}
static td_s32 isp_agamma_param_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_agamma *agamma_ctx)
{
    td_s32  ret;
    /* initiate struct used in this firmware */
    ret = agamma_initialize(vi_pipe, agamma_ctx);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* initiate logic registers */
    agamma_regs_initialize(vi_pipe, reg_cfg, agamma_ctx);

    /* initiate external registers */
    agamma_ext_regs_init(vi_pipe, agamma_ctx);

    return TD_SUCCESS;
}
static td_s32 isp_agamma_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_s32  ret;
    isp_agamma *agamma_ctx = TD_NULL;
    /* first initiate agamma_ctx, using new template */
    ret = agamma_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }
    agamma_get_ctx(vi_pipe, agamma_ctx);
    isp_check_pointer_success_return(agamma_ctx);

    return isp_agamma_param_init(vi_pipe, (isp_reg_cfg *)reg_cfg, agamma_ctx);
}
static td_void isp_agamma_reg_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_agamma *agamma_ctx)
{
    td_u32 i, j;
    ot_unused(vi_pipe);
    for (i = 0; i < reg_cfg->cfg_num; i++) {
        for (j = 0; j < OT_ISP_AGAMMA_NODE_NUM; j++) {
            reg_cfg->alg_reg_cfg[i].agamma_reg_cfg.dyna_reg_cfg.agamma_lut[j] = agamma_ctx->agamma_lut[j];
            reg_cfg->alg_reg_cfg[i].agamma_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
        }
    }
    return;
}

static td_bool check_agamma_open(const isp_agamma *agamma)
{
    return (agamma->mpi_cfg.enable == TD_TRUE);
}
static td_s32 isp_agamma_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv)
{
    ot_unused(rsv);
    ot_unused(stat_info);

    td_u8 i;
    td_s32 ret;
    isp_reg_cfg *isp_cfg = (isp_reg_cfg *)reg_cfg;
    isp_agamma *agamma_ctx = TD_NULL;
    isp_usr_ctx *isp_ctx  = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    agamma_get_ctx(vi_pipe, agamma_ctx);
    isp_check_pointer_success_return(agamma_ctx);

    if (isp_ctx->linkage.stat_ready == TD_FALSE) {
        return TD_SUCCESS;
    }

    agamma_ctx->mpi_cfg.enable = ot_ext_system_agamma_enable_read(vi_pipe);

    for (i = 0; i < isp_cfg->cfg_num; i++) {
        isp_cfg->alg_reg_cfg[i].agamma_reg_cfg.enable = agamma_ctx->mpi_cfg.enable;
    }

    isp_cfg->cfg_key.bit1_agamma_cfg = 1;

    /* check hardware setting */
    if (!check_agamma_open(agamma_ctx)) {
        return TD_SUCCESS;
    }

    /* update control regs; */
    agamma_ctx->mpi_cfg.scale = ot_ext_system_agamma_scale_read(vi_pipe);
    if (agamma_ctx->mpi_cfg.scale < OT_ISP_SCALE_MIN || agamma_ctx->mpi_cfg.scale > OT_ISP_SCALE_MAX) {
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (agamma_ctx->pre_scale != agamma_ctx->mpi_cfg.scale) {
        agamma_ctx->pre_scale = agamma_ctx->mpi_cfg.scale;
        const td_float scale = FIXED_NUM / agamma_ctx->mpi_cfg.scale; /* 1024 */
        gen_lut(agamma_ctx->agamma_lut, OT_ISP_AGAMMA_NODE_NUM, OUTPUT_BITS, scale);
        ret = isp_agamma_attr_check("gen LUT", agamma_ctx);
        if (ret != TD_SUCCESS) {
            return TD_FAILURE;
        }
        isp_agamma_reg_update(vi_pipe, isp_cfg, agamma_ctx);
    }

    return TD_SUCCESS;
}

static td_s32 isp_agamma_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    ot_unused(vi_pipe);
    ot_unused(value);
    switch (cmd) {
        default:
            break;
    }
    return TD_SUCCESS;
}
static td_void agamma_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_agamma *agamma_ctx = TD_NULL;

    agamma_get_ctx(vi_pipe, agamma_ctx);
    isp_free(agamma_ctx);
    agamma_reset_ctx(vi_pipe);

    return;
}
static td_s32 isp_agamma_exit(ot_vi_pipe vi_pipe)
{
    isp_agamma *agamma_ctx = TD_NULL;

    agamma_get_ctx(vi_pipe, agamma_ctx);
    isp_check_pointer_success_return(agamma_ctx);

    agamma_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}
td_s32 isp_alg_register_agamma(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx  *isp_ctx  = TD_NULL;
    isp_alg_node *alg_node = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_agamma);
    alg_node = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_success_return(alg_node);

    alg_node->alg_type = OT_ISP_ALG_AGAMMA;
    alg_node->alg_func.pfn_alg_init = isp_agamma_init;
    alg_node->alg_func.pfn_alg_run  = isp_agamma_run;
    alg_node->alg_func.pfn_alg_ctrl = isp_agamma_ctrl;
    alg_node->alg_func.pfn_alg_exit = isp_agamma_exit;
    alg_node->used = TD_TRUE;

    return TD_SUCCESS;
}
