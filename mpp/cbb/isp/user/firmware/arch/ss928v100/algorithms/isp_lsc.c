/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <math.h>
#include "isp_alg.h"
#include "isp_ext_config.h"
#include "isp_param_check.h"
#include "isp_config.h"
#include "isp_sensor.h"
#include "isp_math_utils.h"

#define BNR_LSC_DEFAULT_GAIN 4096
#define RLSC_DEFAULT_RADIAL_STR    4096
#define RLSC_DEFAULT_SCALE         3
#define LSC_INV_Q_VALUE      12

static const td_u16  g_mesh_gain_def[OT_ISP_LSC_MESHSCALE_NUM] = {
    OT_ISP_LSC_MESHSCALE0_DEF_GAIN, OT_ISP_LSC_MESHSCALE1_DEF_GAIN, OT_ISP_LSC_MESHSCALE2_DEF_GAIN,
    OT_ISP_LSC_MESHSCALE3_DEF_GAIN, 0, 0, 0, 0
};

typedef struct {
    td_u8  idx_r, idx_gr, idx_gb, idx_b;
    td_u8  bayer;
    td_u16 win_x;
    td_u16 win_y;
    td_u16 blk_index_x;
    td_u16 blk_index_y;
} bnr_rlsc_cfg;

typedef struct {
    td_bool init;
    td_bool lsc_enable;
    td_bool bnr_lsc_auto_en;
    td_bool lsc_coef_update;
    td_bool lut_update;

    td_bool acs_enable;
    td_bool acs_lsc_lut_update;

    td_u8  mesh_scale;
    td_u16 mesh_strength;
    td_u16 mesh_weight;
    td_u16 first_point_pos_x[OT_ISP_STRIPING_MAX_NUM];
    td_u16 cur_width[OT_ISP_STRIPING_MAX_NUM];
    td_u16 blk_col_start;
    td_u16 blk_col_end;

    td_u16 delta_x[OT_ISP_LSC_GRID_COL - 1];
    td_u16 delta_y[OT_ISP_MLSC_Y_HALF_GRID_NUM];
    td_u16 inv_x[OT_ISP_LSC_GRID_COL - 1];
    td_u16 inv_y[OT_ISP_MLSC_Y_HALF_GRID_NUM];
} isp_lsc;

isp_lsc *g_lsc_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

#define lsc_get_ctx(dev, ctx)   ((ctx) = g_lsc_ctx[dev])
#define lsc_set_ctx(dev, ctx)   (g_lsc_ctx[dev] = (ctx))
#define lsc_reset_ctx(dev)         (g_lsc_ctx[dev] = TD_NULL)

static td_s32 lsc_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_lsc *lsc_ctx = TD_NULL;

    lsc_get_ctx(vi_pipe, lsc_ctx);

    if (lsc_ctx == TD_NULL) {
        lsc_ctx = (isp_lsc *)isp_malloc(sizeof(isp_lsc));
        if (lsc_ctx == TD_NULL) {
            isp_err_trace("isp[%d] lsc_ctx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void)memset_s(lsc_ctx, sizeof(isp_lsc), 0, sizeof(isp_lsc));

    lsc_set_ctx(vi_pipe, lsc_ctx);

    return TD_SUCCESS;
}

static td_void lsc_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_lsc *lsc_ctx = TD_NULL;

    lsc_get_ctx(vi_pipe, lsc_ctx);
    isp_free(lsc_ctx);
    lsc_reset_ctx(vi_pipe);
}

static td_void lsc_geometric_inv_size(isp_lsc *lsc)
{
    td_s32 i;

    for (i = 0; i < (OT_ISP_LSC_GRID_COL - 1); i++) {
        if (lsc->delta_x[i] != 0) {
            lsc->inv_x[i] = (0x1000 * 0x400 / lsc->delta_x[i] + 0x200) >> OT_ISP_LSC_GAIN_BITWIDTH;
        } else {
            lsc->inv_x[i] = 0;
        }
    }

    for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
        if (lsc->delta_y[i] != 0) {
            lsc->inv_y[i] = (0x1000 * 0x400 / lsc->delta_y[i] + 0x200) >> OT_ISP_LSC_GAIN_BITWIDTH;
        } else {
            lsc->inv_y[i] = 0;
        }
    }

    return;
}

static td_void lsc_get_lut_index(td_u8 cur_blk, isp_lsc *lsc, isp_lsc_usr_cfg *usr_reg_cfg)
{
    td_u16 width = lsc->cur_width[cur_blk] >> 1;
    td_u16 first_point_pos_x = lsc->first_point_pos_x[cur_blk] >> 1;
    td_u16 last_point_pos_x;
    td_u16 width_sum_of_blk;
    td_u16 dis2_right, dis2_left;
    td_u16 blk_col_start;
    td_u16 blk_col_end;

    blk_col_start      = 0;
    blk_col_end        = 0;
    width_sum_of_blk    = lsc->delta_x[0];

    while ((first_point_pos_x >= width_sum_of_blk) && (first_point_pos_x != 0)) {
        blk_col_start++;
        width_sum_of_blk += lsc->delta_x[blk_col_start];
    }

    dis2_right = width_sum_of_blk - first_point_pos_x;
    dis2_left  = lsc->delta_x[blk_col_start] - dis2_right;

    usr_reg_cfg->width_offset = dis2_left;

    last_point_pos_x = first_point_pos_x + width;
    blk_col_end     = blk_col_start;
    while (last_point_pos_x > width_sum_of_blk) {
        blk_col_end++;
        width_sum_of_blk += lsc->delta_x[blk_col_end];
    }
    blk_col_end += 1;

    lsc->blk_col_start  = blk_col_start;
    lsc->blk_col_end    = blk_col_end;

    return;
}

static td_void lsc_get_gain_lut_initialize(ot_vi_pipe vi_pipe, const isp_lsc *lsc, isp_lsc_usr_cfg *usr_reg_cfg)
{
    td_u16 i, j;
    td_u16 blk_col_end, blk_col_start, index_offset, src_index;
    td_u32 def_gain;
    ot_isp_cmos_default   *sns_dft  = TD_NULL;
    const ot_isp_cmos_lsc *cmos_lsc = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);

    blk_col_end   = lsc->blk_col_end;
    blk_col_start = lsc->blk_col_start;

    if (sns_dft->key.bit1_lsc) {
        isp_check_pointer_void_return(sns_dft->lsc);

        cmos_lsc = sns_dft->lsc;

        for (j = 0; j < OT_ISP_LSC_GRID_ROW; j++) {
            for (i = 0; i <= (blk_col_end - blk_col_start); i++) {
                index_offset = j * OT_ISP_LSC_GRID_COL;
                src_index    = index_offset  + blk_col_start + i;

                usr_reg_cfg->r_gain[index_offset + i] = cmos_lsc->lsc_lut.lsc_gain_lut[0].r_gain[src_index] +
                    ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].r_gain[src_index] << OT_ISP_LSC_GAIN_BITWIDTH);
                usr_reg_cfg->gr_gain[index_offset + i] = cmos_lsc->lsc_lut.lsc_gain_lut[0].gr_gain[src_index] +
                    ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].gr_gain[src_index] << OT_ISP_LSC_GAIN_BITWIDTH);
                usr_reg_cfg->gb_gain[index_offset + i] = cmos_lsc->lsc_lut.lsc_gain_lut[0].gb_gain[src_index] +
                    ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].gb_gain[src_index] << OT_ISP_LSC_GAIN_BITWIDTH);
                usr_reg_cfg->b_gain[index_offset + i] = cmos_lsc->lsc_lut.lsc_gain_lut[0].b_gain[src_index] +
                    ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].b_gain[src_index] << OT_ISP_LSC_GAIN_BITWIDTH);
            }
        }
    } else {
        def_gain = ((td_u32)g_mesh_gain_def[lsc->mesh_scale] << OT_ISP_LSC_GAIN_BITWIDTH) +
            g_mesh_gain_def[lsc->mesh_scale];

        for (j = 0; j < OT_ISP_LSC_GRID_ROW; j++) {
            for (i = 0; i <= (blk_col_end - blk_col_start); i++) {
                index_offset = j * OT_ISP_LSC_GRID_COL;

                usr_reg_cfg->r_gain[index_offset + i]   = def_gain;
                usr_reg_cfg->gr_gain[index_offset + i]  = def_gain;
                usr_reg_cfg->gb_gain[index_offset + i]  = def_gain;
                usr_reg_cfg->b_gain[index_offset + i]   = def_gain;
            }
        }
    }

    for (i = 0; i < (blk_col_end - blk_col_start); i++) {
        usr_reg_cfg->delta_x[i] = lsc->delta_x[blk_col_start + i];
        usr_reg_cfg->inv_x[i]   = lsc->inv_x[blk_col_start   + i];
    }

    for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
        usr_reg_cfg->delta_y[i] = lsc->delta_y[i];
        usr_reg_cfg->inv_y[i]   = lsc->inv_y[i];
    }

    return;
}

static td_void lsc_get_gain_lut(ot_vi_pipe vi_pipe, const isp_lsc *lsc, isp_lsc_usr_cfg *usr_reg_cfg)
{
    td_u16 i, j;
    td_u16 blk_col_end;
    td_u16 blk_col_start;
    td_u16 index_offset;
    td_u16 src_index;
    td_u16 r_gain0, r_gain1, gr_gain0, gr_gain1, gb_gain0, gb_gain1, b_gain0, b_gain1;

    blk_col_end   = lsc->blk_col_end;
    blk_col_start = lsc->blk_col_start;

    for (j = 0; j < OT_ISP_LSC_GRID_ROW; j++) {
        for (i = 0; i <= (blk_col_end - blk_col_start); i++) {
            index_offset = j * OT_ISP_LSC_GRID_COL;
            src_index    = index_offset  + blk_col_start + i;

            r_gain0  = ot_ext_system_isp_mesh_shading_r_gain0_read(vi_pipe, src_index);
            r_gain1  = ot_ext_system_isp_mesh_shading_r_gain1_read(vi_pipe, src_index);

            gr_gain0 = ot_ext_system_isp_mesh_shading_gr_gain0_read(vi_pipe, src_index);
            gr_gain1 = ot_ext_system_isp_mesh_shading_gr_gain1_read(vi_pipe, src_index);

            gb_gain0 = ot_ext_system_isp_mesh_shading_gb_gain0_read(vi_pipe, src_index);
            gb_gain1 = ot_ext_system_isp_mesh_shading_gb_gain1_read(vi_pipe, src_index);

            b_gain0  = ot_ext_system_isp_mesh_shading_b_gain0_read(vi_pipe, src_index);
            b_gain1  = ot_ext_system_isp_mesh_shading_b_gain1_read(vi_pipe, src_index);

            usr_reg_cfg->r_gain[index_offset + i]  = ((td_u32)r_gain1  << OT_ISP_LSC_GAIN_BITWIDTH) + r_gain0;
            usr_reg_cfg->gr_gain[index_offset + i] = ((td_u32)gr_gain1 << OT_ISP_LSC_GAIN_BITWIDTH) + gr_gain0;
            usr_reg_cfg->gb_gain[index_offset + i] = ((td_u32)gb_gain1 << OT_ISP_LSC_GAIN_BITWIDTH) + gb_gain0;
            usr_reg_cfg->b_gain[index_offset + i]  = ((td_u32)b_gain1  << OT_ISP_LSC_GAIN_BITWIDTH) + b_gain0;
        }
    }

    for (i = 0; i < (blk_col_end - blk_col_start); i++) {
        usr_reg_cfg->delta_x[i] = lsc->delta_x[blk_col_start + i];
        usr_reg_cfg->inv_x[i]   = lsc->inv_x[blk_col_start   + i];
    }

    for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
        usr_reg_cfg->delta_y[i] = lsc->delta_y[i];
        usr_reg_cfg->inv_y[i]   = lsc->inv_y[i];
    }

    return;
}

static td_void lsc_static_regs_initialize(isp_lsc_static_cfg *lsc_static_reg_cfg)
{
    lsc_static_reg_cfg->win_num_h   = OT_ISP_LSC_GRID_COL - 1;
    lsc_static_reg_cfg->win_num_v   = OT_ISP_LSC_GRID_ROW - 1;
    lsc_static_reg_cfg->static_resh = TD_TRUE;

    return;
}

static td_void lsc_usr_regs_multi_initialize(td_u8 cur_blk, ot_vi_pipe vi_pipe, isp_lsc_usr_cfg *usr_reg_cfg)
{
    isp_lsc *lsc = TD_NULL;

    lsc_get_ctx(vi_pipe, lsc);
    isp_check_pointer_void_return(lsc);

    usr_reg_cfg->mesh_str  = lsc->mesh_strength;
    usr_reg_cfg->weight   = lsc->mesh_weight;
    usr_reg_cfg->mesh_scale = lsc->mesh_scale;

    lsc_get_lut_index(cur_blk, lsc, usr_reg_cfg);

    lsc_get_gain_lut_initialize(vi_pipe, lsc, usr_reg_cfg);

    usr_reg_cfg->update_index = 1;

    usr_reg_cfg->lut_update   = TD_TRUE;
    usr_reg_cfg->coef_update  = TD_TRUE;
    usr_reg_cfg->buf_id       = 0;
}

static td_void lsc_usr_regs_single_initialize(ot_vi_pipe vi_pipe, isp_lsc_usr_cfg *usr_reg_cfg)
{
    td_u32  i, def_gain;
    isp_lsc *lsc = TD_NULL;

    ot_isp_cmos_default   *sns_dft  = TD_NULL;
    const ot_isp_cmos_lsc *cmos_lsc = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);
    lsc_get_ctx(vi_pipe, lsc);
    isp_check_pointer_void_return(lsc);

    usr_reg_cfg->mesh_str     = lsc->mesh_strength;
    usr_reg_cfg->weight       = lsc->mesh_weight;
    usr_reg_cfg->mesh_scale   = lsc->mesh_scale;
    usr_reg_cfg->width_offset = 0;

    (td_void)memcpy_s(usr_reg_cfg->delta_x, sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1), lsc->delta_x,
                      sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1));
    (td_void)memcpy_s(usr_reg_cfg->inv_x, sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1), lsc->inv_x,
                      sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1));
    (td_void)memcpy_s(usr_reg_cfg->delta_y, sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM, lsc->delta_y,
                      sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM);
    (td_void)memcpy_s(usr_reg_cfg->inv_y, sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM, lsc->inv_y,
                      sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM);

    if (sns_dft->key.bit1_lsc) {
        isp_check_pointer_void_return(sns_dft->lsc);

        cmos_lsc = sns_dft->lsc;

        for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
            usr_reg_cfg->r_gain[i]  = ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].r_gain[i] <<
                OT_ISP_LSC_GAIN_BITWIDTH) + cmos_lsc->lsc_lut.lsc_gain_lut[0].r_gain[i];
            usr_reg_cfg->gr_gain[i] = ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].gr_gain[i] <<
                OT_ISP_LSC_GAIN_BITWIDTH) + cmos_lsc->lsc_lut.lsc_gain_lut[0].gr_gain[i];
            usr_reg_cfg->gb_gain[i] = ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].gb_gain[i] <<
                OT_ISP_LSC_GAIN_BITWIDTH) + cmos_lsc->lsc_lut.lsc_gain_lut[0].gb_gain[i];
            usr_reg_cfg->b_gain[i]  = ((td_u32)cmos_lsc->lsc_lut.lsc_gain_lut[1].b_gain[i] <<
                OT_ISP_LSC_GAIN_BITWIDTH) + cmos_lsc->lsc_lut.lsc_gain_lut[0].b_gain[i];
        }
    } else {
        def_gain = ((td_u32)g_mesh_gain_def[lsc->mesh_scale] << OT_ISP_LSC_GAIN_BITWIDTH) +
            g_mesh_gain_def[lsc->mesh_scale];

        for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
            usr_reg_cfg->r_gain[i]  = def_gain;
            usr_reg_cfg->gr_gain[i] = def_gain;
            usr_reg_cfg->gb_gain[i] = def_gain;
            usr_reg_cfg->b_gain[i]  = def_gain;
        }
    }

    usr_reg_cfg->update_index = 1;

    usr_reg_cfg->lut_update           = TD_TRUE;
    usr_reg_cfg->coef_update          = TD_TRUE;
    usr_reg_cfg->buf_id               = 0;
    usr_reg_cfg->switch_lut2_stt_reg_new = TD_FALSE;
    usr_reg_cfg->switch_reg_new_cnt   = 0;
}

static td_void lsc_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, const isp_lsc *lsc)
{
    td_bool offline_mode;
    td_s32  i;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode = (is_offline_mode(isp_ctx->block_attr.running_mode) ||
                    is_striping_mode(isp_ctx->block_attr.running_mode) ||
                    is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg->cfg_num == 1) {
        lsc_static_regs_initialize(&reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.static_reg_cfg);
        lsc_usr_regs_single_initialize(vi_pipe, &reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.usr_reg_cfg);

        /* for online lut2stt, frame 1 should disable */
        reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.lsc_en             = offline_mode ? (lsc->lsc_enable) : (TD_FALSE);
        reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.lut2_stt_en        = TD_TRUE;
        reg_cfg->alg_reg_cfg[0].be_lut_update_cfg.lsc_lut_update = TD_TRUE;
    } else {
        for (i = 0; i < reg_cfg->cfg_num; i++) {
            lsc_static_regs_initialize(&reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.static_reg_cfg);
            lsc_usr_regs_multi_initialize(i, vi_pipe, &reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg);

            reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.lsc_en             = lsc->lsc_enable;
            reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.lut2_stt_en        = TD_TRUE;
            reg_cfg->alg_reg_cfg[i].be_lut_update_cfg.lsc_lut_update = TD_TRUE;
        }
    }

    reg_cfg->cfg_key.bit1_lsc_cfg    = 1;
}

static td_void bnr_lsc_static_regs_initialize(ot_vi_pipe vi_pipe, isp_rlsc_static_cfg *static_reg_cfg)
{
    ot_unused(vi_pipe);
    static_reg_cfg->node_num = OT_ISP_RLSC_POINTS;
    static_reg_cfg->static_resh = TD_TRUE;
    return;
}

static td_void bnr_lsc_usr_regs_initialize_sub(td_u8 cur_blk, ot_vi_pipe vi_pipe, isp_rlsc_usr_cfg *usr_reg_cfg)
{
    isp_rect block_rect;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_u32 width, height;
    td_u32 off_center;
    const td_u32 temp = (1U << 31); /* left bitshift 31 for inverse calculation */

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_get_block_rect(&block_rect, &isp_ctx->block_attr, cur_blk);

    width  = isp_ctx->block_attr.frame_rect.width;
    height = isp_ctx->block_attr.frame_rect.height;

    usr_reg_cfg->width_offset = ABS(block_rect.x);
    usr_reg_cfg->gain_scale = RLSC_DEFAULT_SCALE;
    usr_reg_cfg->center_rx  = width  >> 1;
    usr_reg_cfg->center_ry  = height >> 1;
    usr_reg_cfg->center_gr_x = width  >> 1;
    usr_reg_cfg->center_gr_y = height >> 1;
    usr_reg_cfg->center_gb_x = width  >> 1;
    usr_reg_cfg->center_gb_y = height >> 1;
    usr_reg_cfg->center_bx  = width  >> 1;
    usr_reg_cfg->center_by  = height >> 1;

    off_center = temp / div_0_to_1(width * width / 4 + height * height / 4); /* 4 is square of 2, half width/height */

    usr_reg_cfg->off_center_r  = off_center;
    usr_reg_cfg->off_center_gr = off_center;
    usr_reg_cfg->off_center_gb = off_center;
    usr_reg_cfg->off_center_b  = off_center;

    usr_reg_cfg->update_index        = 1;
    usr_reg_cfg->lut_update          = TD_TRUE;
    usr_reg_cfg->coef_update         = TD_TRUE;
    usr_reg_cfg->usr_resh            = TD_TRUE;
    usr_reg_cfg->buf_id              = 0;
    usr_reg_cfg->switch_reg_new_cnt  = 0;
    usr_reg_cfg->switch_lut2_stt_reg_new  = TD_FALSE;
}

static td_void bnr_lsc_usr_regs_initialize(td_u8 cur_blk, ot_vi_pipe vi_pipe, isp_rlsc_usr_cfg *usr_reg_cfg)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    ot_isp_cmos_default    *sns_dft  = TD_NULL;
    const ot_isp_cmos_lsc  *cmos_lsc = TD_NULL;

    td_u8  i, bayer;
    td_u8  index_r, index_gr, index_gb, index_b;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_sensor_get_default(vi_pipe, &sns_dft);

    bnr_lsc_usr_regs_initialize_sub(cur_blk, vi_pipe, usr_reg_cfg);

    bayer = isp_ctx->bayer;
    index_r  = bayer ^ 0x0;
    index_gr = bayer ^ 0x1;
    index_gb = bayer ^ 0x2;
    index_b  = bayer ^ 0x3;

    if (sns_dft->key.bit1_lsc) {
        isp_check_pointer_void_return(sns_dft->lsc);
        cmos_lsc = sns_dft->lsc;

        usr_reg_cfg->gain_str = cmos_lsc->lsc_attr.mesh_strength;
        (td_void)memcpy_s(usr_reg_cfg->gain_lut[index_r], OT_ISP_RLSC_POINTS * sizeof(td_u16),
                          cmos_lsc->lsc_lut.bnr_lsc_gain_lut.r_gain, OT_ISP_RLSC_POINTS * sizeof(td_u16));
        (td_void)memcpy_s(usr_reg_cfg->gain_lut[index_gr], OT_ISP_RLSC_POINTS * sizeof(td_u16),
                          cmos_lsc->lsc_lut.bnr_lsc_gain_lut.gr_gain, OT_ISP_RLSC_POINTS * sizeof(td_u16));
        (td_void)memcpy_s(usr_reg_cfg->gain_lut[index_gb], OT_ISP_RLSC_POINTS * sizeof(td_u16),
                          cmos_lsc->lsc_lut.bnr_lsc_gain_lut.gb_gain, OT_ISP_RLSC_POINTS * sizeof(td_u16));
        (td_void)memcpy_s(usr_reg_cfg->gain_lut[index_b], OT_ISP_RLSC_POINTS * sizeof(td_u16),
                          cmos_lsc->lsc_lut.bnr_lsc_gain_lut.b_gain, OT_ISP_RLSC_POINTS * sizeof(td_u16));
    } else {
        usr_reg_cfg->gain_str  = RLSC_DEFAULT_RADIAL_STR;
        for (i = 0; i < OT_ISP_RLSC_POINTS; i++) {
            usr_reg_cfg->gain_lut[index_r][i]  = BNR_LSC_DEFAULT_GAIN;
            usr_reg_cfg->gain_lut[index_gr][i] = BNR_LSC_DEFAULT_GAIN;
            usr_reg_cfg->gain_lut[index_gb][i] = BNR_LSC_DEFAULT_GAIN;
            usr_reg_cfg->gain_lut[index_b][i]  = BNR_LSC_DEFAULT_GAIN;
        }
    }

    usr_reg_cfg->gain_lut[index_r][OT_ISP_RLSC_POINTS]  = usr_reg_cfg->gain_lut[index_r][OT_ISP_RLSC_POINTS - 1];
    usr_reg_cfg->gain_lut[index_gr][OT_ISP_RLSC_POINTS] = usr_reg_cfg->gain_lut[index_gr][OT_ISP_RLSC_POINTS - 1];
    usr_reg_cfg->gain_lut[index_gb][OT_ISP_RLSC_POINTS] = usr_reg_cfg->gain_lut[index_gb][OT_ISP_RLSC_POINTS - 1];
    usr_reg_cfg->gain_lut[index_b][OT_ISP_RLSC_POINTS]  = usr_reg_cfg->gain_lut[index_b][OT_ISP_RLSC_POINTS - 1];

    return;
}

static td_void bnr_lsc_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, const isp_lsc *lsc)
{
    td_s32 i;

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        bnr_lsc_static_regs_initialize(vi_pipe, &reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.static_reg_cfg);
        bnr_lsc_usr_regs_initialize(i, vi_pipe, &reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg);

        reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.rlsc_en = lsc->lsc_enable;
    }

    reg_cfg->cfg_key.bit1_rlsc_cfg = 1;
}

static td_void bnr_lsc_ext_regs_initialize(ot_vi_pipe vi_pipe)
{
    td_u16 i;
    ot_isp_cmos_default    *sns_dft  = TD_NULL;
    const ot_isp_cmos_lsc  *cmos_lsc = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);

    if (sns_dft->key.bit1_lsc) {
        isp_check_pointer_void_return(sns_dft->lsc);
        cmos_lsc = sns_dft->lsc;
        for (i = 0; i < OT_ISP_RLSC_POINTS; i++) {
            ot_ext_system_isp_bnr_shading_r_gain_write(vi_pipe,  i, cmos_lsc->lsc_lut.bnr_lsc_gain_lut.r_gain[i]);
            ot_ext_system_isp_bnr_shading_gr_gain_write(vi_pipe, i, cmos_lsc->lsc_lut.bnr_lsc_gain_lut.gr_gain[i]);
            ot_ext_system_isp_bnr_shading_gb_gain_write(vi_pipe, i, cmos_lsc->lsc_lut.bnr_lsc_gain_lut.gb_gain[i]);
            ot_ext_system_isp_bnr_shading_b_gain_write(vi_pipe,  i, cmos_lsc->lsc_lut.bnr_lsc_gain_lut.b_gain[i]);
        }
    } else {
        for (i = 0; i < OT_ISP_RLSC_POINTS; i++) {
            ot_ext_system_isp_bnr_shading_r_gain_write(vi_pipe, i,  BNR_LSC_DEFAULT_GAIN);
            ot_ext_system_isp_bnr_shading_gr_gain_write(vi_pipe, i, BNR_LSC_DEFAULT_GAIN);
            ot_ext_system_isp_bnr_shading_gb_gain_write(vi_pipe, i, BNR_LSC_DEFAULT_GAIN);
            ot_ext_system_isp_bnr_shading_b_gain_write(vi_pipe, i,  BNR_LSC_DEFAULT_GAIN);
        }
    }
    return;
}

static td_void lsc_ext_regs_initialize(ot_vi_pipe vi_pipe, const isp_lsc *lsc)
{
    td_u16 i;
    ot_isp_cmos_default   *sns_dft  = TD_NULL;
    const ot_isp_cmos_lsc *cmos_lsc = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);

    ot_ext_system_isp_mesh_shading_enable_write(vi_pipe, lsc->lsc_enable);
    ot_ext_system_isp_bnr_lsc_auto_enable_write(vi_pipe, lsc->bnr_lsc_auto_en);
    ot_ext_system_isp_mesh_shading_attr_updata_write(vi_pipe, TD_FALSE);
    ot_ext_system_isp_mesh_shading_lut_attr_updata_write(vi_pipe, TD_FALSE);
    ot_ext_system_isp_mesh_shading_mesh_strength_write(vi_pipe, lsc->mesh_strength);
    ot_ext_system_isp_mesh_shading_blendratio_write(vi_pipe, lsc->mesh_weight);
    ot_ext_system_isp_mesh_shading_mesh_scale_write(vi_pipe, lsc->mesh_scale);

    for (i = 0; i < OT_ISP_MLSC_X_HALF_GRID_NUM; i++) {
        ot_ext_system_isp_mesh_shading_xgrid_write(vi_pipe, i, lsc->delta_x[i]);
    }

    for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
        ot_ext_system_isp_mesh_shading_ygrid_write(vi_pipe, i, lsc->delta_y[i]);
    }

    if (sns_dft->key.bit1_lsc) {
        isp_check_pointer_void_return(sns_dft->lsc);

        cmos_lsc = sns_dft->lsc;

        for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
            ot_ext_system_isp_mesh_shading_r_gain0_write(vi_pipe,  i, cmos_lsc->lsc_lut.lsc_gain_lut[0].r_gain[i]);
            ot_ext_system_isp_mesh_shading_gr_gain0_write(vi_pipe, i, cmos_lsc->lsc_lut.lsc_gain_lut[0].gr_gain[i]);
            ot_ext_system_isp_mesh_shading_gb_gain0_write(vi_pipe, i, cmos_lsc->lsc_lut.lsc_gain_lut[0].gb_gain[i]);
            ot_ext_system_isp_mesh_shading_b_gain0_write(vi_pipe,  i, cmos_lsc->lsc_lut.lsc_gain_lut[0].b_gain[i]);

            ot_ext_system_isp_mesh_shading_r_gain1_write(vi_pipe,  i, cmos_lsc->lsc_lut.lsc_gain_lut[1].r_gain[i]);
            ot_ext_system_isp_mesh_shading_gr_gain1_write(vi_pipe, i, cmos_lsc->lsc_lut.lsc_gain_lut[1].gr_gain[i]);
            ot_ext_system_isp_mesh_shading_gb_gain1_write(vi_pipe, i, cmos_lsc->lsc_lut.lsc_gain_lut[1].gb_gain[i]);
            ot_ext_system_isp_mesh_shading_b_gain1_write(vi_pipe,  i, cmos_lsc->lsc_lut.lsc_gain_lut[1].b_gain[i]);
        }
    } else {
        for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
            ot_ext_system_isp_mesh_shading_r_gain0_write(vi_pipe,  i, g_mesh_gain_def[lsc->mesh_scale]);
            ot_ext_system_isp_mesh_shading_gr_gain0_write(vi_pipe, i, g_mesh_gain_def[lsc->mesh_scale]);
            ot_ext_system_isp_mesh_shading_gb_gain0_write(vi_pipe, i, g_mesh_gain_def[lsc->mesh_scale]);
            ot_ext_system_isp_mesh_shading_b_gain0_write(vi_pipe,  i, g_mesh_gain_def[lsc->mesh_scale]);

            ot_ext_system_isp_mesh_shading_r_gain1_write(vi_pipe,  i, g_mesh_gain_def[lsc->mesh_scale]);
            ot_ext_system_isp_mesh_shading_gr_gain1_write(vi_pipe, i, g_mesh_gain_def[lsc->mesh_scale]);
            ot_ext_system_isp_mesh_shading_gb_gain1_write(vi_pipe, i, g_mesh_gain_def[lsc->mesh_scale]);
            ot_ext_system_isp_mesh_shading_b_gain1_write(vi_pipe,  i, g_mesh_gain_def[lsc->mesh_scale]);
        }
    }

    return;
}

static td_void lsc_read_ext_regs(ot_vi_pipe vi_pipe, isp_lsc *lsc)
{
    td_u16 i;
    lsc->lsc_coef_update  = ot_ext_system_isp_mesh_shading_attr_updata_read(vi_pipe);
    if (lsc->lsc_coef_update) {
        ot_ext_system_isp_mesh_shading_attr_updata_write(vi_pipe, TD_FALSE);
        lsc->mesh_strength = ot_ext_system_isp_mesh_shading_mesh_strength_read(vi_pipe);
        lsc->mesh_weight   = ot_ext_system_isp_mesh_shading_blendratio_read(vi_pipe);
        lsc->bnr_lsc_auto_en = ot_ext_system_isp_bnr_lsc_auto_enable_read(vi_pipe);
    }

    lsc->acs_enable = ot_ext_system_isp_acs_enable_read(vi_pipe);
    lsc->acs_lsc_lut_update = ot_ext_system_isp_acs_lsc_lut_attr_updata_read(vi_pipe);
    ot_ext_system_isp_acs_lsc_lut_attr_updata_write(vi_pipe, TD_FALSE);

    lsc->mesh_scale = ot_ext_system_isp_mesh_shading_mesh_scale_read(vi_pipe);

    lsc->lut_update = ot_ext_system_isp_mesh_shading_lut_attr_updata_read(vi_pipe);
    if (lsc->lut_update) {
        ot_ext_system_isp_mesh_shading_lut_attr_updata_write(vi_pipe, TD_FALSE);
        for (i = 0; i < OT_ISP_MLSC_X_HALF_GRID_NUM; i++) {
            lsc->delta_x[i] = ot_ext_system_isp_mesh_shading_xgrid_read(vi_pipe, i);
        }

        for (i = OT_ISP_MLSC_X_HALF_GRID_NUM; i < OT_ISP_LSC_GRID_COL - 1; i++) {
            lsc->delta_x[i] = lsc->delta_x[OT_ISP_LSC_GRID_COL - 2 - i]; /* 2 for other half of delta_x */
        }

        for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
            lsc->delta_y[i] = ot_ext_system_isp_mesh_shading_ygrid_read(vi_pipe, i);
        }

        lsc_geometric_inv_size(lsc);
    }
    /* need to update bnr lsc lut when bnr_lsc_auto_en is on */
    if (lsc->lsc_coef_update && lsc->bnr_lsc_auto_en) {
        lsc->lut_update = TD_TRUE;
    }
    return;
}

static td_void lsc_geometric_grid_size(td_u16 *delta, td_u16 *inv, td_u16 length, td_u16 grid_size)
{
    td_u16 i, sum;
    td_u16 half_grid_size;
    td_u16 diff;
    td_u16 *tmp_step = TD_NULL;
    td_u16 sum_r;

    half_grid_size = (grid_size - 1) >> 1;

    if (half_grid_size == 0) {
        return;
    }

    tmp_step = (td_u16 *)isp_malloc(sizeof(td_u16) * half_grid_size);
    if (tmp_step == TD_NULL) {
        return;
    }

    (td_void)memset_s(tmp_step, sizeof(td_u16) * half_grid_size, 0, sizeof(td_u16) * half_grid_size);

    sum_r = (td_u16)((((length >> 1) * 0x400 / div_0_to_1(half_grid_size)) + 0x200) >> OT_ISP_LSC_GAIN_BITWIDTH);

    for (i = 0; i < half_grid_size; i++) {
        tmp_step[i] = sum_r;
    }

    sum = 0;
    for (i = 0; i < half_grid_size; i++) {
        sum = sum + tmp_step[i];
    }

    if (sum != (length >> 1)) {
        if (sum > (length >> 1)) {
            diff = sum - (length >> 1);
            for (i = 0; i < diff; i++) {
                tmp_step[i] = tmp_step[i] - 1;
            }
        } else {
            diff = (length >> 1) - sum;
            for (i = 0; i < diff; i++) {
                tmp_step[half_grid_size - i - 1] = tmp_step[half_grid_size - i - 1] + 1;
            }
        }
    }

    for (i = 0; i < half_grid_size; i++) {
        delta[i] = tmp_step[i];
        inv[i] = (delta[i] == 0) ? (0) : ((0x1000 * 0x400 / div_0_to_1(delta[i]) + 0x200) >> OT_ISP_LSC_GAIN_BITWIDTH);
    }

    isp_free(tmp_step);
}

static td_void lsc_image_size(ot_vi_pipe vi_pipe, td_u8 blk_num, isp_lsc  *lsc, td_bool cmos_en)
{
    td_u16 i;
    isp_rect block_rect;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (cmos_en) {
        ot_isp_cmos_default *sns_dft = TD_NULL;
        isp_sensor_get_default(vi_pipe, &sns_dft);
        for (i = 0; i < OT_ISP_MLSC_X_HALF_GRID_NUM; i++) {
            lsc->delta_x[i] = sns_dft->lsc->lsc_lut.x_grid_width[i];
            lsc->inv_x[i] = (lsc->delta_x[i] == 0) ? (0) :
                ((0x1000 * 0x400 / div_0_to_1(lsc->delta_x[i]) + 0x200) >> OT_ISP_LSC_GAIN_BITWIDTH);
        }
        for (i = 0; i < OT_ISP_MLSC_Y_HALF_GRID_NUM; i++) {
            lsc->delta_y[i] = sns_dft->lsc->lsc_lut.y_grid_width[i];
            lsc->inv_y[i] = (lsc->delta_y[i] == 0) ? (0) :
                ((0x1000 * 0x400 / div_0_to_1(lsc->delta_y[i]) + 0x200) >> OT_ISP_LSC_GAIN_BITWIDTH);
        }
    } else {
        lsc_geometric_grid_size(lsc->delta_x, lsc->inv_x,
            isp_ctx->block_attr.frame_rect.width >> 1, OT_ISP_LSC_GRID_COL);
        lsc_geometric_grid_size(lsc->delta_y, lsc->inv_y,
            isp_ctx->block_attr.frame_rect.height >> 1, OT_ISP_LSC_GRID_ROW);
    }

    for (i = 0; i < OT_ISP_MLSC_X_HALF_GRID_NUM; i++) {
        lsc->delta_x[OT_ISP_LSC_GRID_COL - 2 - i] = lsc->delta_x[i]; /* 2 for other half of delta_x */
        lsc->inv_x[OT_ISP_LSC_GRID_COL - 2 - i]   = lsc->inv_x[i]; /* 2 for other half of inv_x */
    }

    for (i = 0; i < blk_num; i++) {
        isp_get_block_rect(&block_rect, &isp_ctx->block_attr, i);

        lsc->cur_width[i]       = block_rect.width;
        lsc->first_point_pos_x[i] = block_rect.x;
    }
}

static td_void bnr_lsc_image_size(ot_vi_pipe vi_pipe, td_u8 blk_num, isp_rlsc_usr_cfg *usr_reg_cfg)
{
    td_u32 width, height;
    td_u32 off_center;
    const td_u32 temp = (1U << 31); /* left bitshift 31 for inverse calculation */

    isp_usr_ctx  *isp_ctx = TD_NULL;
    isp_rect  block_rect;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_get_block_rect(&block_rect, &isp_ctx->block_attr, blk_num);

    width  = isp_ctx->block_attr.frame_rect.width;
    height = isp_ctx->block_attr.frame_rect.height;

    usr_reg_cfg->width_offset = ABS(block_rect.x);

    usr_reg_cfg->center_rx  = width  >> 1;
    usr_reg_cfg->center_ry  = height >> 1;
    usr_reg_cfg->center_gr_x = width  >> 1;
    usr_reg_cfg->center_gr_y = height >> 1;
    usr_reg_cfg->center_gb_x = width  >> 1;
    usr_reg_cfg->center_gb_y = height >> 1;
    usr_reg_cfg->center_bx  = width  >> 1;
    usr_reg_cfg->center_by  = height >> 1;

    off_center = temp / div_0_to_1(width * width / 4 + height * height / 4); /* 4 is square of 2, half width/height */

    usr_reg_cfg->off_center_r  = off_center;
    usr_reg_cfg->off_center_gr = off_center;
    usr_reg_cfg->off_center_gb = off_center;
    usr_reg_cfg->off_center_b  = off_center;

    usr_reg_cfg->update_index += 1;
    usr_reg_cfg->lut_update     = TD_TRUE;
    usr_reg_cfg->usr_resh       = TD_TRUE;
}

static td_s32 lsc_check_cmos_param(ot_vi_pipe vi_pipe, const ot_isp_cmos_lsc *cmos_lsc)
{
    isp_check_bool_return(cmos_lsc->lsc_attr.en);
    isp_check_bool_return(cmos_lsc->lsc_attr.bnr_lsc_auto_en);

    if (cmos_lsc->lsc_attr.blend_ratio > 0x100) {
        isp_err_trace("Invalid blend_ratio!\n");
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return isp_mesh_shading_gain_lut_attr_check("cmos", vi_pipe, &cmos_lsc->lsc_lut);
}

static td_s32 lsc_initialize(ot_vi_pipe vi_pipe, const isp_reg_cfg *reg_cfg, isp_lsc *lsc)
{
    td_s32 ret;
    ot_isp_cmos_default *sns_dft = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);
    if (sns_dft->key.bit1_lsc) {
        isp_check_pointer_return(sns_dft->lsc);

        ret = lsc_check_cmos_param(vi_pipe, sns_dft->lsc);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        lsc->lsc_enable      = sns_dft->lsc->lsc_attr.en;
        lsc->mesh_strength   = sns_dft->lsc->lsc_attr.mesh_strength;
        lsc->mesh_weight     = sns_dft->lsc->lsc_attr.blend_ratio;
        lsc->bnr_lsc_auto_en = sns_dft->lsc->lsc_attr.bnr_lsc_auto_en;
        lsc->mesh_scale      = sns_dft->lsc->lsc_lut.mesh_scale;

        lsc_image_size(vi_pipe, reg_cfg->cfg_num, lsc, TD_TRUE);
    } else {
        lsc->lsc_enable      = TD_TRUE;
        lsc->mesh_strength   = OT_ISP_LSC_DEFAULT_MESH_STRENGTH;
        lsc->mesh_weight     = OT_ISP_LSC_DEFAULT_WEIGHT;
        lsc->mesh_scale      = OT_ISP_LSC_DEFAULT_MESH_SCALE;
        lsc->bnr_lsc_auto_en = TD_FALSE;

        lsc_image_size(vi_pipe, reg_cfg->cfg_num, lsc, TD_FALSE);
    }

    return TD_SUCCESS;
}

static td_void lsc_usr_fw(ot_vi_pipe vi_pipe, td_u8 cur_blk, isp_lsc *lsc, isp_lsc_usr_cfg *usr_reg_cfg)
{
    lsc_get_lut_index(cur_blk, lsc, usr_reg_cfg);

    lsc_get_gain_lut(vi_pipe, lsc, usr_reg_cfg);

    usr_reg_cfg->mesh_scale = lsc->mesh_scale;
    usr_reg_cfg->update_index += 1;
    usr_reg_cfg->lut_update = TD_TRUE;
}

static td_void  lsc_usr_fw_normal(ot_vi_pipe vi_pipe, const isp_lsc *lsc, isp_lsc_usr_cfg *usr_reg_cfg)
{
    td_u16 i;
    td_u16 r_gain0, r_gain1, gr_gain0, gr_gain1, gb_gain0, gb_gain1, b_gain0, b_gain1;

    usr_reg_cfg->width_offset  = 0;
    usr_reg_cfg->mesh_scale     = lsc->mesh_scale;
    usr_reg_cfg->update_index += 1;
    usr_reg_cfg->lut_update      = TD_TRUE;

    (td_void)memcpy_s(usr_reg_cfg->delta_x, sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1),
                      lsc->delta_x, sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1));
    (td_void)memcpy_s(usr_reg_cfg->inv_x, sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1),
                      lsc->inv_x, sizeof(td_u16) * (OT_ISP_LSC_GRID_COL - 1));
    (td_void)memcpy_s(usr_reg_cfg->delta_y, sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM,
                      lsc->delta_y, sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM);
    (td_void)memcpy_s(usr_reg_cfg->inv_y, sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM,
                      lsc->inv_y, sizeof(td_u16) * OT_ISP_MLSC_Y_HALF_GRID_NUM);

    for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
        r_gain0  = ot_ext_system_isp_mesh_shading_r_gain0_read(vi_pipe, i);
        r_gain1  = ot_ext_system_isp_mesh_shading_r_gain1_read(vi_pipe, i);

        gr_gain0 = ot_ext_system_isp_mesh_shading_gr_gain0_read(vi_pipe, i);
        gr_gain1 = ot_ext_system_isp_mesh_shading_gr_gain1_read(vi_pipe, i);

        gb_gain0 = ot_ext_system_isp_mesh_shading_gb_gain0_read(vi_pipe, i);
        gb_gain1 = ot_ext_system_isp_mesh_shading_gb_gain1_read(vi_pipe, i);

        b_gain0  = ot_ext_system_isp_mesh_shading_b_gain0_read(vi_pipe, i);
        b_gain1  = ot_ext_system_isp_mesh_shading_b_gain1_read(vi_pipe, i);

        usr_reg_cfg->r_gain[i]  = ((td_u32)r_gain1  << OT_ISP_LSC_GAIN_BITWIDTH) + r_gain0;
        usr_reg_cfg->gr_gain[i] = ((td_u32)gr_gain1 << OT_ISP_LSC_GAIN_BITWIDTH) + gr_gain0;
        usr_reg_cfg->gb_gain[i] = ((td_u32)gb_gain1 << OT_ISP_LSC_GAIN_BITWIDTH) + gb_gain0;
        usr_reg_cfg->b_gain[i]  = ((td_u32)b_gain1  << OT_ISP_LSC_GAIN_BITWIDTH) + b_gain0;
    }

    return;
}

static td_u16 bilinear_interp(ot_vi_pipe vi_pipe, const bnr_rlsc_cfg *cfg, const isp_lsc *lsc, td_u8 bayer)
{
    td_s32 v00, v01, v10, v11, iv1, iv2, re, tmp_x, tmp_y;
    td_u16 idx_0 = cfg->blk_index_y * OT_ISP_LSC_GRID_COL + cfg->blk_index_x;
    td_u16 idx_1 = cfg->blk_index_y * OT_ISP_LSC_GRID_COL + cfg->blk_index_x + 1;
    td_u16 idx_2 = (cfg->blk_index_y + 1) * OT_ISP_LSC_GRID_COL + cfg->blk_index_x;
    td_u16 idx_3 = (cfg->blk_index_y + 1) * OT_ISP_LSC_GRID_COL + cfg->blk_index_x + 1;

    switch (bayer) {
        case OT_ISP_CHN_R:
            v00 = ot_ext_system_isp_mesh_shading_r_gain0_read(vi_pipe, idx_0);
            v01 = ot_ext_system_isp_mesh_shading_r_gain0_read(vi_pipe, idx_1);
            v10 = ot_ext_system_isp_mesh_shading_r_gain0_read(vi_pipe, idx_2);
            v11 = ot_ext_system_isp_mesh_shading_r_gain0_read(vi_pipe, idx_3);
            break;
        case OT_ISP_CHN_GR:
            v00 = ot_ext_system_isp_mesh_shading_gr_gain0_read(vi_pipe, idx_0);
            v01 = ot_ext_system_isp_mesh_shading_gr_gain0_read(vi_pipe, idx_1);
            v10 = ot_ext_system_isp_mesh_shading_gr_gain0_read(vi_pipe, idx_2);
            v11 = ot_ext_system_isp_mesh_shading_gr_gain0_read(vi_pipe, idx_3);
            break;
        case OT_ISP_CHN_GB:
            v00 = ot_ext_system_isp_mesh_shading_gb_gain0_read(vi_pipe, idx_0);
            v01 = ot_ext_system_isp_mesh_shading_gb_gain0_read(vi_pipe, idx_1);
            v10 = ot_ext_system_isp_mesh_shading_gb_gain0_read(vi_pipe, idx_2);
            v11 = ot_ext_system_isp_mesh_shading_gb_gain0_read(vi_pipe, idx_3);
            break;
        default: /* OT_ISP_CHN_B */
            v00 = ot_ext_system_isp_mesh_shading_b_gain0_read(vi_pipe, idx_0);
            v01 = ot_ext_system_isp_mesh_shading_b_gain0_read(vi_pipe, idx_1);
            v10 = ot_ext_system_isp_mesh_shading_b_gain0_read(vi_pipe, idx_2);
            v11 = ot_ext_system_isp_mesh_shading_b_gain0_read(vi_pipe, idx_3);
            break;
    }
    tmp_x = clip3(cfg->win_x * lsc->inv_x[cfg->blk_index_x], 0, OT_ISP_LSC_DEFAULT_MESH_STRENGTH);
    tmp_y = clip3(cfg->win_y * lsc->inv_y[cfg->blk_index_y], 0, OT_ISP_LSC_DEFAULT_MESH_STRENGTH);
    iv1 = v00 + (((v10 - v00) * tmp_y) / BNR_LSC_DEFAULT_GAIN);
    iv1 = clip3(iv1, 0, OT_ISP_LSC_MAX_GAIN);
    iv2 = v01 + (((v11 - v01) * tmp_y) / BNR_LSC_DEFAULT_GAIN);
    iv2 = clip3(iv2, 0, OT_ISP_LSC_MAX_GAIN);
    re = iv1 + (((iv2 - iv1) * tmp_x) / BNR_LSC_DEFAULT_GAIN);
    re = clip3(re, 0, OT_ISP_LSC_MAX_GAIN);

    if (lsc->mesh_scale < 4) { /* less than 4 */
        re = (td_u32)re <<  (lsc->mesh_scale + 3); /* 3 scale offset */
    } else {
        re = ((td_u32)re << (lsc->mesh_scale - 2)) + BNR_LSC_DEFAULT_GAIN; /* 2 scale offset */
    }
    return re;
}

static td_void bnr_lsc_usr_fw_auto(ot_vi_pipe vi_pipe, const isp_lsc *lsc, isp_rlsc_usr_cfg *usr_reg_cfg,
    bnr_rlsc_cfg *cfg)
{
    td_s16 i;
    td_u16 loc_x, loc_y, del_x, del_y;

    td_u32 diff_x = ot_ext_system_be_total_width_read(vi_pipe) >> 2; /* 2 */
    td_u32 diff_y = ot_ext_system_be_total_height_read(vi_pipe) >> 2;  /* 2 */

    td_u32 square_radius = diff_x * diff_x + diff_y * diff_y;
    td_u32 square_sec = square_radius / (OT_ISP_RLSC_POINTS - 1);
    td_u16 w_uplimit = lsc->delta_x[0];
    td_u16 w_downlimit = 0;
    td_u16 h_uplimit = lsc->delta_y[0];
    td_u16 h_downlimit = 0;

    td_float sin = (td_float)diff_y / (td_float)sqrt((td_double)square_radius);
    td_float cos = (td_float)diff_x / (td_float)sqrt((td_double)square_radius);

    for (i = OT_ISP_RLSC_POINTS - 1; i >= 0; --i) {
        del_x = (td_u16)(cos * sqrt((td_float)(i * square_sec)));
        del_y = (td_u16)(sin * sqrt((td_float)(i * square_sec)));
        loc_x = diff_x  - del_x;
        loc_y = diff_y - del_y;

        while (loc_x > w_uplimit) {
            cfg->blk_index_x++;
            w_uplimit += lsc->delta_x[cfg->blk_index_x];
            w_downlimit += lsc->delta_x[cfg->blk_index_x - 1];
        }
        while (loc_y > h_uplimit) {
            cfg->blk_index_y++;
            h_uplimit += lsc->delta_y[cfg->blk_index_y];
            h_downlimit += lsc->delta_y[cfg->blk_index_y - 1];
        }
        cfg->win_x = loc_x - w_downlimit;
        cfg->win_y = loc_y - h_downlimit;

        usr_reg_cfg->gain_lut[cfg->idx_r][i]  = bilinear_interp(vi_pipe, cfg, lsc, OT_ISP_CHN_R);
        usr_reg_cfg->gain_lut[cfg->idx_gr][i] = bilinear_interp(vi_pipe, cfg, lsc, OT_ISP_CHN_GR);
        usr_reg_cfg->gain_lut[cfg->idx_gb][i] = bilinear_interp(vi_pipe, cfg, lsc, OT_ISP_CHN_GB);
        usr_reg_cfg->gain_lut[cfg->idx_b][i]  = bilinear_interp(vi_pipe, cfg, lsc, OT_ISP_CHN_B);
    }
}

static td_void lsc_usr_fw_bnr_lsc(ot_vi_pipe vi_pipe, isp_rlsc_usr_cfg *usr_reg_cfg)
{
    td_u32 i;
    bnr_rlsc_cfg cfg;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_lsc *lsc = TD_NULL;

    lsc_get_ctx(vi_pipe, lsc);
    isp_get_ctx(vi_pipe, isp_ctx);

    (td_void)memset_s(&cfg, sizeof(bnr_rlsc_cfg), 0, sizeof(bnr_rlsc_cfg));
    cfg.bayer = isp_ctx->bayer;
    cfg.idx_r  = cfg.bayer ^ 0x0;
    cfg.idx_gr = cfg.bayer ^ 0x1;
    cfg.idx_gb = cfg.bayer ^ 0x2;
    cfg.idx_b  = cfg.bayer ^ 0x3;

    if (lsc->bnr_lsc_auto_en) {
        bnr_lsc_usr_fw_auto(vi_pipe, lsc, usr_reg_cfg, &cfg);
        for (i = 0; i < OT_ISP_RLSC_POINTS; i++) {
            ot_ext_system_isp_bnr_shading_r_gain_write(vi_pipe, i, usr_reg_cfg->gain_lut[cfg.idx_r][i]);
            ot_ext_system_isp_bnr_shading_gr_gain_write(vi_pipe, i, usr_reg_cfg->gain_lut[cfg.idx_gr][i]);
            ot_ext_system_isp_bnr_shading_gb_gain_write(vi_pipe, i, usr_reg_cfg->gain_lut[cfg.idx_gb][i]);
            ot_ext_system_isp_bnr_shading_b_gain_write(vi_pipe, i, usr_reg_cfg->gain_lut[cfg.idx_b][i]);
        }
    } else {
        for (i = 0; i < OT_ISP_RLSC_POINTS; i++) {
            usr_reg_cfg->gain_lut[cfg.idx_r][i]  = ot_ext_system_isp_bnr_shading_r_gain_read(vi_pipe, i);
            usr_reg_cfg->gain_lut[cfg.idx_gr][i] = ot_ext_system_isp_bnr_shading_gr_gain_read(vi_pipe, i);
            usr_reg_cfg->gain_lut[cfg.idx_gb][i] = ot_ext_system_isp_bnr_shading_gb_gain_read(vi_pipe, i);
            usr_reg_cfg->gain_lut[cfg.idx_b][i]  = ot_ext_system_isp_bnr_shading_b_gain_read(vi_pipe, i);
        }
    }

    usr_reg_cfg->gain_lut[cfg.idx_r][OT_ISP_RLSC_POINTS]  = usr_reg_cfg->gain_lut[cfg.idx_r][OT_ISP_RLSC_POINTS - 1];
    usr_reg_cfg->gain_lut[cfg.idx_gr][OT_ISP_RLSC_POINTS] = usr_reg_cfg->gain_lut[cfg.idx_gr][OT_ISP_RLSC_POINTS - 1];
    usr_reg_cfg->gain_lut[cfg.idx_gb][OT_ISP_RLSC_POINTS] = usr_reg_cfg->gain_lut[cfg.idx_gb][OT_ISP_RLSC_POINTS - 1];
    usr_reg_cfg->gain_lut[cfg.idx_b][OT_ISP_RLSC_POINTS]  = usr_reg_cfg->gain_lut[cfg.idx_b][OT_ISP_RLSC_POINTS - 1];

    usr_reg_cfg->update_index += 1;
    usr_reg_cfg->lut_update    = TD_TRUE;
    usr_reg_cfg->usr_resh      = TD_TRUE;
}

__inline static td_bool check_lsc_open(const isp_lsc *lsc)
{
    return (lsc->lsc_enable == TD_TRUE);
}

/* when acs is on, it will update lsc_gain_lut from inside, meshscale, grid_width, bnr_lsc_lut are get from mpi */
static td_void lsc_acs_fw_inner(ot_vi_pipe vi_pipe, isp_lsc *lsc, isp_reg_cfg *reg_cfg)
{
    td_s32 i;
    if (reg_cfg->cfg_num == 1) {
        lsc_usr_fw_normal(vi_pipe, lsc, &reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.usr_reg_cfg);
    } else {
        for (i = 0; i < reg_cfg->cfg_num; i++) {
            lsc_usr_fw(vi_pipe, i, lsc, &reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg);
        }
    }

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        lsc_usr_fw_bnr_lsc(vi_pipe, &reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg);
    }
}

static td_s32 lsc_image_res_write(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u8  i;
    td_s32 ret;
    isp_lsc *lsc = TD_NULL;

    lsc_get_ctx(vi_pipe, lsc);
    isp_check_pointer_return(lsc);

    lsc->init = TD_FALSE;
    ot_ext_system_isp_lsc_init_status_write(vi_pipe, lsc->init);
    ret = lsc_initialize(vi_pipe, reg_cfg, lsc);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    lsc_ext_regs_initialize(vi_pipe, lsc);
    bnr_lsc_ext_regs_initialize(vi_pipe);

    if (reg_cfg->cfg_num == 1) {
        lsc_usr_fw_normal(vi_pipe, lsc, &reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.usr_reg_cfg);
    } else {
        for (i = 0; i < reg_cfg->cfg_num; i++) {
            lsc_usr_fw(vi_pipe, i, lsc, &reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg);
        }
    }

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        bnr_lsc_image_size(vi_pipe, i, &reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg);
    }

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_lut2_stt_reg_new    = TD_TRUE;
        reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_reg_new_cnt         = 0;
        reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg.switch_lut2_stt_reg_new   = TD_TRUE;
        reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg.switch_reg_new_cnt        = 0;
        reg_cfg->alg_reg_cfg[i].be_lut_update_cfg.lsc_lut_update                   = TD_TRUE;
    }

    lsc->init = TD_TRUE;
    ot_ext_system_isp_lsc_init_status_write(vi_pipe, lsc->init);

    reg_cfg->cfg_key.bit1_lsc_cfg  = 1;
    reg_cfg->cfg_key.bit1_rlsc_cfg = 1;

    return TD_SUCCESS;
}

static td_s32 isp_lsc_init(ot_vi_pipe vi_pipe, td_void *reg_cfg_in)
{
    td_s32 ret;
    isp_lsc *lsc_ctx = TD_NULL;
    isp_reg_cfg *reg_cfg = (isp_reg_cfg *)reg_cfg_in;

    ot_ext_system_isp_lsc_init_status_write(vi_pipe, TD_FALSE);
    ret = lsc_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    lsc_get_ctx(vi_pipe, lsc_ctx);
    isp_check_pointer_return(lsc_ctx);
    lsc_ctx->init = TD_FALSE;
    ret = lsc_initialize(vi_pipe, reg_cfg, lsc_ctx);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    lsc_regs_initialize(vi_pipe, reg_cfg, lsc_ctx);
    lsc_ext_regs_initialize(vi_pipe, lsc_ctx);

    bnr_lsc_regs_initialize(vi_pipe, reg_cfg, lsc_ctx);
    bnr_lsc_ext_regs_initialize(vi_pipe);

    lsc_ctx->init = TD_TRUE;
    ot_ext_system_isp_lsc_init_status_write(vi_pipe, lsc_ctx->init);

    return TD_SUCCESS;
}

static td_void isp_lsc_fw_coef_update(isp_alg_reg_cfg *alg_reg_cfg, const isp_lsc *lsc)
{
    alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.coef_update   = TD_TRUE;
    alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.mesh_str      = lsc->mesh_strength;
    alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.weight        = lsc->mesh_weight;
    alg_reg_cfg->rlsc_reg_cfg.usr_reg_cfg.coef_update  = TD_TRUE;
    alg_reg_cfg->rlsc_reg_cfg.usr_reg_cfg.gain_str     = lsc->mesh_strength;
}

static td_s32 isp_lsc_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg_in, td_s32 rsv)
{
    td_s32 i;
    ot_isp_alg_mod alg_mod = OT_ISP_ALG_LSC;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_lsc *lsc = TD_NULL;
    isp_reg_cfg *reg_cfg  = (isp_reg_cfg *)reg_cfg_in;
    ot_unused(stat_info);
    ot_unused(rsv);

    isp_get_ctx(vi_pipe, isp_ctx);
    lsc_get_ctx(vi_pipe, lsc);
    isp_check_pointer_return(lsc);

    if (isp_ctx->linkage.defect_pixel) {
        return TD_SUCCESS;
    }

    ot_ext_system_isp_lsc_init_status_write(vi_pipe, lsc->init);
    if (lsc->init != TD_TRUE) {
        ioctl(isp_get_fd(vi_pipe), ISP_ALG_INIT_ERR_INFO_PRINT, &alg_mod);
        return TD_SUCCESS;
    }

    lsc->lsc_enable = ot_ext_system_isp_mesh_shading_enable_read(vi_pipe);
    for (i = 0; i < reg_cfg->cfg_num; i++) {
        reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.lsc_en = lsc->lsc_enable;
    }

    reg_cfg->cfg_key.bit1_lsc_cfg  = 1;
    reg_cfg->cfg_key.bit1_rlsc_cfg = 1;
    /* check hardware setting */
    if (!check_lsc_open(lsc)) {
        return TD_SUCCESS;
    }

    lsc_read_ext_regs(vi_pipe, lsc);

    if (lsc->lsc_coef_update) {
        for (i = 0; i < reg_cfg->cfg_num; i++) {
            isp_lsc_fw_coef_update(&reg_cfg->alg_reg_cfg[i], lsc);
        }
    }

    if (lsc->lut_update && !(lsc->acs_enable)) {
        if (reg_cfg->cfg_num == 1) {
            lsc_usr_fw_normal(vi_pipe, lsc, &reg_cfg->alg_reg_cfg[0].lsc_reg_cfg.usr_reg_cfg);
        } else {
            for (i = 0; i < reg_cfg->cfg_num; i++) {
                lsc_usr_fw(vi_pipe, i, lsc, &reg_cfg->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg);
            }
        }

        for (i = 0; i < reg_cfg->cfg_num; i++) {
            lsc_usr_fw_bnr_lsc(vi_pipe, &reg_cfg->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg);
        }
    } else if (lsc->acs_lsc_lut_update) {
        lsc_acs_fw_inner(vi_pipe, lsc, reg_cfg);
    }

    return TD_SUCCESS;
}

static td_s32 isp_lsc_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    isp_reg_cfg_attr *reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    ot_unused(value);

    switch (cmd) {
        case OT_ISP_CHANGE_IMAGE_MODE_SET:
            isp_regcfg_get_ctx(vi_pipe, reg_cfg);
            isp_check_pointer_return(reg_cfg);
            lsc_image_res_write(vi_pipe, &reg_cfg->reg_cfg);
            break;
        case OT_ISP_WDR_MODE_SET:
            isp_get_ctx(vi_pipe, isp_ctx);

            if (isp_ctx->block_attr.block_num != isp_ctx->block_attr.pre_block_num) {
                isp_regcfg_get_ctx(vi_pipe, reg_cfg);
                isp_check_pointer_return(reg_cfg);
                lsc_image_res_write(vi_pipe, &reg_cfg->reg_cfg);
            }
            break;
        default:
            break;
    }
    return TD_SUCCESS;
}

static td_s32 isp_lsc_exit(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, reg_cfg);

    ot_ext_system_isp_lsc_init_status_write(vi_pipe, TD_FALSE);
    for (i = 0; i < reg_cfg->reg_cfg.cfg_num; i++) {
        reg_cfg->reg_cfg.alg_reg_cfg[i].lsc_reg_cfg.lsc_en = TD_FALSE;
        reg_cfg->reg_cfg.alg_reg_cfg[i].rlsc_reg_cfg.rlsc_en = TD_FALSE;
    }

    reg_cfg->reg_cfg.cfg_key.bit1_lsc_cfg = 1;
    reg_cfg->reg_cfg.cfg_key.bit1_rlsc_cfg = 1;

    lsc_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_alg_register_lsc(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_node *algs = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_lsc);
    algs = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_return(algs);

    algs->alg_type = OT_ISP_ALG_LSC;
    algs->alg_func.pfn_alg_init = isp_lsc_init;
    algs->alg_func.pfn_alg_run  = isp_lsc_run;
    algs->alg_func.pfn_alg_ctrl = isp_lsc_ctrl;
    algs->alg_func.pfn_alg_exit = isp_lsc_exit;
    algs->used = TD_TRUE;

    return TD_SUCCESS;
}
