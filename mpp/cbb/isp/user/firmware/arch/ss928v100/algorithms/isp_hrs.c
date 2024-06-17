/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_alg.h"
#include "isp_config.h"

#define HRS_FILTER_CLASS_MAX 2

static td_s16 g_hrs_filter_lut[HRS_FILTER_CLASS_MAX][HRS_FILTER_ELEMENT_NUM] = {
    { -14, 23, 232, 23, -14, 6 },
    { 10, -39, 157, 157, -39, 10 }
};

static td_void hrs_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u8 j;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_hrs_static_cfg *hrs_static_reg_cfg = &reg_cfg->alg_reg_cfg[0].hrs_reg_cfg.static_reg_cfg;

    isp_get_ctx(vi_pipe, isp_ctx);

    hrs_static_reg_cfg->rs_enable = (is_hrs_on(vi_pipe) ? TD_TRUE : TD_FALSE);
    hrs_static_reg_cfg->enable = TD_TRUE;
    hrs_static_reg_cfg->height = isp_ctx->sys_rect.height;
    hrs_static_reg_cfg->width = isp_ctx->sys_rect.width;

    for (j = 0; j < HRS_FILTER_ELEMENT_NUM; j++) {
        hrs_static_reg_cfg->hrs_filter_lut0[j] = g_hrs_filter_lut[0][j];
        hrs_static_reg_cfg->hrs_filter_lut1[j] = g_hrs_filter_lut[1][j];
    }

    hrs_static_reg_cfg->resh = TD_TRUE;

    reg_cfg->cfg_key.bit1_hrs_cfg = 1;
}

static td_s32 isp_hrs_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    hrs_regs_initialize(vi_pipe, (isp_reg_cfg *)reg_cfg);

    return TD_SUCCESS;
}

static td_s32 isp_hrs_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg_input, td_s32 rsv)
{
    ot_unused(vi_pipe);
    ot_unused(stat_info);
    ot_unused(reg_cfg_input);
    ot_unused(rsv);
    return TD_SUCCESS;
}

static td_s32 isp_hrs_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    ot_unused(vi_pipe);
    ot_unused(cmd);
    ot_unused(value);
    return TD_SUCCESS;
}

static td_s32 isp_hrs_exit(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return TD_SUCCESS;
}

td_s32 isp_alg_register_hrs(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_node *algs = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    algs = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_return(algs);

    algs->alg_type = OT_ISP_ALG_HRS;
    algs->alg_func.pfn_alg_init = isp_hrs_init;
    algs->alg_func.pfn_alg_run  = isp_hrs_run;
    algs->alg_func.pfn_alg_ctrl = isp_hrs_ctrl;
    algs->alg_func.pfn_alg_exit = isp_hrs_exit;
    algs->used = TD_TRUE;

    return TD_SUCCESS;
}
