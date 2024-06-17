/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ot_mpi_isp.h"
#include "ot_mpi_ae.h"
#include "ot_mpi_awb.h"
#include "securec.h"
#include "imx485_cmos_ex.h"
#include "imx485_cmos.h"

#define IMX485_ID                    485
#define SENSOR_IMX485_WIDTH          3840
#define SENSOR_IMX485_HEIGHT         2160
#define IMX485_BLACK_LEVEL           800
#define IMX485_RATIO                 64

#define higher_4bits(x) (((x) & 0xf0000) >> 16)
#define high_8bits(x) (((x) & 0xff00) >> 8)
#define low_8bits(x)  ((x) & 0x00ff)

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/
#define imx485_sensor_set_ctx(dev, ctx)   ((g_imx485_sns_state[dev]) = (ctx))
#define imx485_sensor_reset_ctx(dev)      (g_imx485_sns_state[dev] = TD_NULL)

static ot_isp_fswdr_mode g_fswdr_mode[OT_ISP_MAX_PIPE_NUM] = {
    [0 ... OT_ISP_MAX_PIPE_NUM - 1] = OT_ISP_FSWDR_NORMAL_MODE
};

static td_u32 g_max_time_get_cnt[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u32 g_init_exposure[OT_ISP_MAX_PIPE_NUM]  = {0};
static td_u32 g_lines_per500ms[OT_ISP_MAX_PIPE_NUM] = {0};

static td_u16 g_init_wb_gain[OT_ISP_MAX_PIPE_NUM][OT_ISP_RGB_CHN_NUM] = {{0}};
static td_u16 g_sample_r_gain[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u16 g_sample_b_gain[OT_ISP_MAX_PIPE_NUM] = {0};
static td_bool g_quick_start_en[OT_ISP_MAX_PIPE_NUM] = {TD_FALSE};

static td_bool g_ae_route_ex_valid[OT_ISP_MAX_PIPE_NUM] = {0};
static ot_isp_ae_route g_init_ae_route[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route_ex g_init_ae_route_ex[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route g_init_ae_route_sf[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route_ex g_init_ae_route_sf_ex[OT_ISP_MAX_PIPE_NUM] = {{0}};

typedef struct {
    td_u8       hcg;
    td_u32      brl;
    td_u32      rhs1_max;
    td_u32      rhs2_max;
} imx485_state;

imx485_state g_imx485_state[OT_ISP_MAX_PIPE_NUM] = {{0}};
ot_isp_sns_commbus g_imx485_bus_info[OT_ISP_MAX_PIPE_NUM] = {
    [0] = { .i2c_dev = 0},
    [1 ... OT_ISP_MAX_PIPE_NUM - 1] = { .i2c_dev = -1}
};

ot_isp_sns_state *g_imx485_sns_state[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

static td_bool blc_clamp_info[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = TD_TRUE};

ot_isp_sns_commbus *imx485_get_bus_info(ot_vi_pipe vi_pipe)
{
    return &g_imx485_bus_info[vi_pipe];
}

ot_isp_sns_state *imx485_get_ctx(ot_vi_pipe vi_pipe)
{
    return g_imx485_sns_state[vi_pipe];
}

td_void imx485_set_blc_clamp_value(ot_vi_pipe vi_pipe, td_bool clamp_en)
{
    blc_clamp_info[vi_pipe] = clamp_en;
}

const imx485_video_mode_tbl g_imx485_mode_tbl[IMX485_MODE_BUTT] = {
    {IMX485_VMAX_2160P30_LINEAR,   IMX485_FULL_LINES_MAX,          30, 0.8,
     3840, 2160, 0, OT_WDR_MODE_NONE,       "IMX485_SENSOR_2160P_30FPS_LINEAR_MODE"},
    {IMX485_VMAX_2160P90TO30_WDR,  IMX485_FULL_LINES_MAX_3TO1_WDR, 30, 16.51,
     3840, 2160, 0, OT_WDR_MODE_3To1_LINE,  "IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE"},
};

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
/* Imx327 Register Address */
#define IMX485_SHR0_ADDR                              0x3050
#define IMX485_SHR1_ADDR                              0x3054
#define IMX485_SHR2_ADDR                              0x3058

#define IMX485_RHS1_ADDR                              0x3060
#define IMX485_RHS2_ADDR                              0x3064

#define IMX485_GAIN_ADDR                              0x3084
#define IMX485_HCG_ADDR                               0x3034
#define IMX485_HCG_SEL1_ADDR                          0x3035
#define IMX485_HCG_SEL2_ADDR                          0x3036

#define IMX485_VMAX_ADDR                              0x3024
#define IMX485_HMAX_ADDR                              0x3028

#define IMX485_WDR_BRL                                 2210


#define imx485_err_mode_print(sensor_image_mode, sns_state) \
    do { \
        isp_err_trace("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", \
                      (sensor_image_mode)->width,  \
                      (sensor_image_mode)->height, \
                      (sensor_image_mode)->fps,    \
                      (sns_state)->wdr_mode); \
    } while (0)

static td_void cmos_get_ae_comm_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
    const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->flicker_freq = 50 * 256; /* light flicker freq: 50Hz, accuracy: 256 */
    ae_sns_dft->full_lines_max = IMX485_FULL_LINES_MAX;
    ae_sns_dft->hmax_times = (1000000000) / (sns_state->fl_std * 30); /* 1000000000ns, 30fps */

    ae_sns_dft->again_accu.accu_type = OT_ISP_AE_ACCURACY_TABLE;
    ae_sns_dft->again_accu.accuracy  = 1;

    ae_sns_dft->dgain_accu.accu_type = OT_ISP_AE_ACCURACY_TABLE;
    ae_sns_dft->dgain_accu.accuracy = 1;

    ae_sns_dft->isp_dgain_shift = 8; /* accuracy: 8 */
    ae_sns_dft->min_isp_dgain_target = 1 << ae_sns_dft->isp_dgain_shift;
    ae_sns_dft->max_isp_dgain_target = 2 << ae_sns_dft->isp_dgain_shift; /* max 2 */
    if (g_lines_per500ms[vi_pipe] == 0) {
        ae_sns_dft->lines_per500ms = sns_state->fl_std * 30 / 2; /* 30fps, div 2 */
    } else {
        ae_sns_dft->lines_per500ms = g_lines_per500ms[vi_pipe];
    }

    ae_sns_dft->max_iris_fno = OT_ISP_IRIS_F_NO_1_0;
    ae_sns_dft->min_iris_fno = OT_ISP_IRIS_F_NO_32_0;

    ae_sns_dft->ae_route_ex_valid = TD_FALSE;
    ae_sns_dft->ae_route_attr.total_num = 0;
    ae_sns_dft->ae_route_attr_ex.total_num = 0;
    ae_sns_dft->quick_start.quick_start_enable = g_quick_start_en[vi_pipe];
    ae_sns_dft->quick_start.black_frame_num = 0;

    return;
}

static td_void cmos_get_ae_linear_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
    const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->int_time_accu.accu_type = OT_ISP_AE_ACCURACY_LINEAR;
    ae_sns_dft->int_time_accu.accuracy = 2; /* accuracy 2 */
    ae_sns_dft->int_time_accu.offset = 0;

    ae_sns_dft->max_again = 62416; /* max 62416 */
    ae_sns_dft->min_again = 1024; /* min 1024 */
    ae_sns_dft->max_again_target = ae_sns_dft->max_again;
    ae_sns_dft->min_again_target = ae_sns_dft->min_again;

    ae_sns_dft->max_dgain = 1024; /* max 1024 */
    ae_sns_dft->min_dgain = 1024; /* min 1024 */
    ae_sns_dft->max_dgain_target = ae_sns_dft->max_dgain;
    ae_sns_dft->min_dgain_target = ae_sns_dft->min_dgain;

    ae_sns_dft->isp_dgain_shift = 8; /* accuracy: 8 */
    ae_sns_dft->min_isp_dgain_target = 1 << ae_sns_dft->isp_dgain_shift;
    ae_sns_dft->max_isp_dgain_target = 255 << ae_sns_dft->isp_dgain_shift; /* max 255 */

    ae_sns_dft->ae_compensation = 0x38;
    ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_HIGHLIGHT_PRIOR;
    ae_sns_dft->init_exposure = g_init_exposure[vi_pipe] ? g_init_exposure[vi_pipe] : 76151; /* init 76151 */

    ae_sns_dft->max_int_time = sns_state->fl_std - 6; /* sub 6 */
    ae_sns_dft->min_int_time = 2; /* min 2 */
    ae_sns_dft->max_int_time_target = 65535; /* max 65535 */
    ae_sns_dft->min_int_time_target = 1; /* min 1 */
    ae_sns_dft->ae_route_ex_valid = g_ae_route_ex_valid[vi_pipe];
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route),
                      &g_init_ae_route[vi_pipe],  sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr_ex, sizeof(ot_isp_ae_route_ex),
                      &g_init_ae_route_ex[vi_pipe], sizeof(ot_isp_ae_route_ex));
    return;
}

static td_void cmos_get_ae_3to1_line_wdr_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
    const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->int_time_accu.accu_type = OT_ISP_AE_ACCURACY_LINEAR;
    ae_sns_dft->int_time_accu.accuracy = 6; /* accuracy 6 */
    ae_sns_dft->int_time_accu.offset = 0;

    ae_sns_dft->max_int_time = sns_state->fl_std - 2; /* sub 2 */
    ae_sns_dft->min_int_time = 6; /* min_int_time 6 */

    ae_sns_dft->max_int_time_target = 65535; /* max 65535 */
    ae_sns_dft->min_int_time_target = ae_sns_dft->min_int_time;

    ae_sns_dft->max_again = 62416; /* max 62416 */
    ae_sns_dft->min_again = 1024; /* min 1024 */
    ae_sns_dft->max_again_target = ae_sns_dft->max_again;
    ae_sns_dft->min_again_target = ae_sns_dft->min_again;

    ae_sns_dft->max_dgain = 38485; /* max 38485 */
    ae_sns_dft->min_dgain = 1024; /* min 1024 */
    ae_sns_dft->max_dgain_target = ae_sns_dft->max_dgain;
    ae_sns_dft->min_dgain_target = ae_sns_dft->min_dgain;
    ae_sns_dft->max_isp_dgain_target = 16 << ae_sns_dft->isp_dgain_shift; /* max 16 << shift */

    ae_sns_dft->init_exposure = g_init_exposure[vi_pipe] ? g_init_exposure[vi_pipe] : 52000; /* init 52000 */

    if (g_fswdr_mode[vi_pipe] == OT_ISP_FSWDR_LONG_FRAME_MODE) {
        ae_sns_dft->ae_compensation = 64; /* ae_compensation 64 */
        ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_HIGHLIGHT_PRIOR;
    } else {
        ae_sns_dft->ae_compensation = 32; /* ae_compensation 32 */
        ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_LOWLIGHT_PRIOR;
        ae_sns_dft->man_ratio_enable = TD_TRUE;
        ae_sns_dft->arr_ratio[0] = 0x200;
        ae_sns_dft->arr_ratio[1] = 0x200;
        ae_sns_dft->arr_ratio[2] = 0x40; /* array index 2 */
    }
    ae_sns_dft->ae_route_ex_valid = g_ae_route_ex_valid[vi_pipe];
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route),
                      &g_init_ae_route[vi_pipe],  sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr_ex, sizeof(ot_isp_ae_route_ex),
                      &g_init_ae_route_ex[vi_pipe],  sizeof(ot_isp_ae_route_ex));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_sf_attr, sizeof(ot_isp_ae_route),
                      &g_init_ae_route_sf[vi_pipe], sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_sf_attr_ex, sizeof(ot_isp_ae_route_ex),
                      &g_init_ae_route_sf_ex[vi_pipe],  sizeof(ot_isp_ae_route_ex));
    return;
}

static td_s32 cmos_get_ae_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(ae_sns_dft);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    (td_void)memset_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route), 0, sizeof(ot_isp_ae_route));

    cmos_get_ae_comm_default(vi_pipe, ae_sns_dft, sns_state);

    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_NONE:   /* linear mode */
            cmos_get_ae_linear_default(vi_pipe, ae_sns_dft, sns_state);
            break;

        case OT_WDR_MODE_3To1_LINE:
            cmos_get_ae_3to1_line_wdr_default(vi_pipe, ae_sns_dft, sns_state);
            break;

        default:
            cmos_get_ae_linear_default(vi_pipe, ae_sns_dft, sns_state);
            break;
    }

    return TD_SUCCESS;
}

static td_void cmos_config_vmax(ot_isp_sns_state *sns_state, td_u32 vmax)
{
    if (sns_state->wdr_mode == OT_WDR_MODE_NONE) {
        sns_state->regs_info[0].i2c_data[5].data = low_8bits(vmax); /* array index 5 */
        sns_state->regs_info[0].i2c_data[6].data = high_8bits(vmax); /* array index 6 */
        sns_state->regs_info[0].i2c_data[7].data = higher_4bits(vmax); /* array index 7 */
    } else {
        sns_state->regs_info[0].i2c_data[11].data = low_8bits(vmax); /* array index 11 */
        sns_state->regs_info[0].i2c_data[12].data = high_8bits(vmax); /* array index 12 */
        sns_state->regs_info[0].i2c_data[13].data = higher_4bits(vmax); /* array index 13 */
    }

    return;
}

/* the function of sensor set fps */
static td_bool cmos_3to1_vmax_limit(td_u32 *vmax, td_u32 full_line, td_u32 step, td_bool fps_up)
{
    if (fps_up) {
        if (3 * (*vmax) + step < full_line) { /* 3 frame */
            (*vmax) = (full_line - step) / 3;   /* 3 frame */
            return TD_FALSE;
        }
    } else {
        if (3 * (*vmax) > full_line + step) { /* 3 frame */
            (*vmax) = (full_line + step) / 3;   /* 3 frame */
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

static td_void cmos_fps_set(ot_vi_pipe vi_pipe, td_float fps, ot_isp_ae_sensor_default *ae_sns_dft)
{
    td_u32 lines, lines_max, vmax;
    td_bool achieve_fps_flag;
    td_float max_fps, min_fps;
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_void_return(ae_sns_dft);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    lines = g_imx485_mode_tbl[sns_state->img_mode].ver_lines;
    lines_max = g_imx485_mode_tbl[sns_state->img_mode].max_ver_lines;
    max_fps = g_imx485_mode_tbl[sns_state->img_mode].max_fps;
    min_fps = g_imx485_mode_tbl[sns_state->img_mode].min_fps;

    if ((fps > max_fps) || (fps < min_fps)) {
        isp_err_trace("Not support Fps: %f\n", fps);
        return;
    }
    achieve_fps_flag = TD_TRUE;
    vmax = lines * max_fps / div_0_to_1_float(fps);
    if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        achieve_fps_flag = cmos_3to1_vmax_limit(&vmax, sns_state->fl[0], 144, fps > ae_sns_dft->fps); /* step 144 */
        vmax = vmax - vmax % 6; /* mod 6 */
        vmax = (vmax > lines_max) ? lines_max : vmax;
        vmax = (vmax > 0x8ca) ? vmax : 0x8ca;
        cmos_config_vmax(sns_state, vmax);
        sns_state->fl_std = vmax * 3;  /* x3 */
        ae_sns_dft->lines_per500ms = lines * max_fps * 3 / 2;  /* x3 2 */
        g_imx485_state[vi_pipe].rhs2_max = g_imx485_state[vi_pipe].brl * 6; /* 6 */
    } else {
        vmax = (vmax > lines_max) ? lines_max : vmax;
        vmax = vmax + vmax % 2; /* mod 2 */
        cmos_config_vmax(sns_state, vmax);
        sns_state->fl_std = vmax;
        ae_sns_dft->lines_per500ms = lines * max_fps / 2; /* div 2 */
    }

    ae_sns_dft->fps = lines * max_fps * 0x40 / vmax / 0x40;
    ae_sns_dft->fps = (achieve_fps_flag) ? fps : ae_sns_dft->fps;

    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->max_int_time = sns_state->fl_std - 2;  /* sub 2 */
    sns_state->fl[0] = sns_state->fl_std;
    ae_sns_dft->full_lines = sns_state->fl[0];
    ae_sns_dft->hmax_times =
        (1000000000) / (sns_state->fl_std * div_0_to_1_float(fps)); /* 1000000000ns */
    return;
}

static td_void cmos_slow_framerate_set(ot_vi_pipe vi_pipe, td_u32 full_lines, ot_isp_ae_sensor_default *ae_sns_dft)
{
    td_u32 lines_max, vmax;
    td_bool achieve_fps;
    ot_isp_sns_state *sns_state = TD_NULL;
    sensor_check_pointer_void_return(ae_sns_dft);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    lines_max = g_imx485_mode_tbl[sns_state->img_mode].max_ver_lines;

    if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        vmax = full_lines / 3; /* divide 3 */
        achieve_fps = cmos_3to1_vmax_limit(&vmax, sns_state->fl[0], 144, full_lines < sns_state->fl[0]); /* 144 */
        vmax = vmax - vmax % 6; /* mod 6 */
        vmax = (vmax > lines_max) ? lines_max : vmax;
        vmax = (vmax > 0x8ca) ? vmax : 0x8ca;
        sns_state->fl[0] = vmax * 3; /* multiply 3 */
        g_imx485_state[vi_pipe].rhs2_max = g_imx485_state[vi_pipe].brl * 6; /* 6 */
    } else {
        vmax = full_lines;
        vmax = (vmax > lines_max) ? lines_max : vmax;
        sns_state->fl[0] = vmax;
    }
    ot_unused(achieve_fps);
    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_NONE:
            sns_state->regs_info[0].i2c_data[5].data = vmax & 0xFF; /* index 5 */
            sns_state->regs_info[0].i2c_data[6].data = (vmax & 0xFF00) >> 8; /* index 6, shift 8 */
            sns_state->regs_info[0].i2c_data[7].data = (vmax & 0xF0000) >> 16; /* index 7, shift 16 */
            break;
        case OT_WDR_MODE_3To1_LINE:
            sns_state->regs_info[0].i2c_data[11].data = (vmax & 0xFF); /* index 11 */
            sns_state->regs_info[0].i2c_data[12].data = (vmax & 0xFF00) >> 8; /* index 12, shift 8 */
            sns_state->regs_info[0].i2c_data[13].data = (vmax & 0xF0000) >> 16; /* index 13, shift 16 */
            break;
        default:
            break;
    }

    ae_sns_dft->full_lines = sns_state->fl[0];
    ae_sns_dft->max_int_time = sns_state->fl[0] - 2; /* max_int_time: Flstd - 2 */

    return;
}

static td_void cmos_inttime_update_linear(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    td_u32 value;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    value = sns_state->fl[0] - int_time + int_time % 2; /* mode 2 */
    value = MIN2(value, 0xFFFFF);
    value = MIN2(MAX2(value, 6), sns_state->fl[0] - 2); /* max 6, sub 2 */
    sns_state->regs_info[0].i2c_data[0].data = low_8bits(value);
    sns_state->regs_info[0].i2c_data[1].data = high_8bits(value);
    sns_state->regs_info[0].i2c_data[2].data = higher_4bits(value); /* index 2 */
}

static td_void cmos_inttime_update_reg_3to1_line_shr(td_u32 shr0, td_u32 shr1, td_u32 shr2,
    ot_isp_sns_state *sns_state)
{
    sns_state->regs_info[0].i2c_data[0].data = low_8bits(shr0);
    sns_state->regs_info[0].i2c_data[1].data = high_8bits(shr0); /* index 1 */
    sns_state->regs_info[0].i2c_data[2].data = higher_4bits(shr0); /* index 2 */

    sns_state->regs_info[0].i2c_data[5].data = low_8bits(shr1); /* index 5 */
    sns_state->regs_info[0].i2c_data[6].data = high_8bits(shr1); /* index 6 */
    sns_state->regs_info[0].i2c_data[7].data = higher_4bits(shr1); /* index 7 */

    sns_state->regs_info[0].i2c_data[8].data = low_8bits(shr2); /* index 8 */
    sns_state->regs_info[0].i2c_data[9].data = high_8bits(shr2); /* index 9 */
    sns_state->regs_info[0].i2c_data[10].data = higher_4bits(shr2); /* index 10 */
}

static td_void cmos_inttime_update_reg_3to1_line_rhs(td_u32 rhs1, td_u32 rhs2, ot_isp_sns_state *sns_state)
{
    sns_state->regs_info[0].i2c_data[14].data = low_8bits(rhs1); /* index 14 */
    sns_state->regs_info[0].i2c_data[15].data = high_8bits(rhs1); /* index 15 */
    sns_state->regs_info[0].i2c_data[16].data = higher_4bits(rhs1); /* index 16 */

    sns_state->regs_info[0].i2c_data[17].data = low_8bits(rhs2); /* index 17 */
    sns_state->regs_info[0].i2c_data[18].data = high_8bits(rhs2); /* index 18 */
    sns_state->regs_info[0].i2c_data[19].data = higher_4bits(rhs2); /* index 19 */
}


static td_void cmos_inttime_update_3to1_line_long(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    td_u32 shr0, shr1, shr2, rhs1, rhs2;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);
    shr0 = sns_state->fl[1] - int_time;

    shr1 = 20; /* min 20 */
    rhs1 = shr1 + sns_state->wdr_int_time[1];
    rhs1 = ((rhs1 - 2) % 6 == 0) ? rhs1 : rhs1 + 6 - (rhs1 - 2) % 6; /* reduce 2 mod 6 */

    shr2 = rhs1 + 14; /* add 14 */
    shr2 = ((shr2 - 4) % 6 == 0) ? shr2 : shr2 + 6 - (shr2 - 4) % 6; /* reduce 4 mod 6 */

    rhs2 = shr2 + sns_state->wdr_int_time[0]; /* add 6 */
    rhs2 = ((rhs2 - 4) % 6 == 0) ? rhs2 : rhs2 + 6 - (rhs2 - 4) % 6; /* reduce 4 mod 6 */

    cmos_inttime_update_reg_3to1_line_shr(shr0, shr1, shr2, sns_state);
    cmos_inttime_update_reg_3to1_line_rhs(rhs1, rhs2, sns_state);
}

static td_void cmos_inttime_update_3to1_line(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    static td_u32 count[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 0};

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    if (count[vi_pipe] == 0) { /* short short exposure */
        sns_state->wdr_int_time[0] = int_time;
        count[vi_pipe]++;
    } else if (count[vi_pipe] == 1) { /* short exposure */
        sns_state->wdr_int_time[1] = int_time;
        count[vi_pipe]++;
    } else { /* long exposure */
        sns_state->wdr_int_time[2] = int_time; /* index 2 */
        cmos_inttime_update_3to1_line_long(vi_pipe, sns_state->wdr_int_time[2]); /* index 2 */
        count[vi_pipe] = 0;
    }
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static td_void cmos_inttime_update(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        cmos_inttime_update_3to1_line(vi_pipe, int_time);
    } else {
        cmos_inttime_update_linear(vi_pipe, int_time);
    }

    return;
}
#define GAIN_NODE_NUM    231
#define DGAIN_NODE_NUM   141
static td_u32 g_gain_table[GAIN_NODE_NUM] = {
    1024,    1059,    1097,    1135,    1175,    1217,    1259,    1304,    1349,    1397,    1446,    1497,    1549,
    1604,    1660,    1719,    1779,    1842,    1906,    1973,    2043,    2114,    2189,    2266,    2345,    2428,
    2513,    2602,    2693,    2788,    2886,    2987,    3092,    3201,    3313,    3430,    3550,    3675,    3804,
    3938,    4076,    4219,    4368,    4521,    4680,    4845,    5015,    5191,    5374,    5562,    5758,    5960,
    6170,    6387,    6611,    6843,    7084,    7333,    7591,    7857,    8134,    8419,    8715,    9022,    9339,
    9667,    10007,   10358,   10722,   11099,   11489,   11893,   12311,   12743,   13191,   13655,   14135,   14631,
    15146,   15678,   16229,   16799,   17390,   18001,   18633,   19288,   19966,   20668,   21394,   22146,   22924,
    23730,   24564,   25427,   26320,   27245,   28203,   29194,   30220,   31282,   32381,   33519,   34697,   35917,
    37179,   38485,   39838,   41238,   42687,   44187,   45740,   47347,   49011,   50734,   52517,   54362,   56272,
    58250,   60297,   62416,   64610,   66880,   69231,   71663,   74182,   76789,   79487,   82281,   85172,   88165,
    91264,   94471,   97791,   101227,  104785,  108467,  112279,  116225,  120309,  124537,  128913,  133444,  138133,
    142988,  148013,  153214,  158599,  164172,  169941,  175913,  182095,  188495,  195119,  201976,  209073,  216421,
    224026,  231899,  240049,  248485,  257217,  266256,  275613,  285298,  295324,  305703,  320110,  327567,  339078,
    350994,  363329,  376097,  389314,  402995,  417157,  431817,  446992,  462700,  478960,  495792,  513215,  531251,
    549920,  569246,  589250,  609958,  631393,  653581,  676550,  700325,  724936,  750412,  776783,  804081,  832338,
    861588,  891866,  923208,  955652,  989236,  1024000, 1059985, 1097235, 1135795, 1175709, 1217026, 1259795, 1304067,
    1349894, 1397333, 1446438, 1497269, 1549886, 1604353, 1660733, 1719095, 1779508, 1842043, 1906777, 1913785, 2043148,
    2114949, 2189273, 2266208, 2345848, 2428286, 2513621, 2601956, 2662563, 2788046, 2886024
};

static td_void cmos_again_calc_table(ot_vi_pipe vi_pipe, td_u32 *again_lin, td_u32 *again_db)
{
    int i;

    ot_unused(vi_pipe);
    sensor_check_pointer_void_return(again_lin);
    sensor_check_pointer_void_return(again_db);

    if (*again_lin >= g_gain_table[GAIN_NODE_NUM - 1]) {
        *again_lin = g_gain_table[GAIN_NODE_NUM - 1];
        *again_db = GAIN_NODE_NUM - 1;
        return;
    }

    for (i = 1; i < GAIN_NODE_NUM; i++) {
        if (*again_lin < g_gain_table[i]) {
            *again_lin = g_gain_table[i - 1];
            *again_db = i - 1;
            break;
        }
    }
    return;
}

static td_void cmos_dgain_calc_table(ot_vi_pipe vi_pipe, td_u32 *dgain_lin, td_u32 *dgain_db)
{
    int i;

    ot_unused(vi_pipe);
    sensor_check_pointer_void_return(dgain_lin);
    sensor_check_pointer_void_return(dgain_db);

    if (*dgain_lin >= g_gain_table[DGAIN_NODE_NUM - 1]) {
        *dgain_lin = g_gain_table[DGAIN_NODE_NUM - 1];
        *dgain_db = DGAIN_NODE_NUM - 1;
        return;
    }

    for (i = 1; i < DGAIN_NODE_NUM; i++) {
        if (*dgain_lin < g_gain_table[i]) {
            *dgain_lin = g_gain_table[i - 1];
            *dgain_db = i - 1;
            break;
        }
    }
    return;
}

static td_void cmos_gains_update(ot_vi_pipe vi_pipe, td_u32 again, td_u32 dgain)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    td_u32 hcg = g_imx485_state[vi_pipe].hcg;
    td_u32 tmp;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    if (again >= 27) { /* HCG 27 */
        hcg = 0x1;
        again = again - 27; /* again sub 27 */
    } else {
        hcg = 0x0;
    }

    tmp = again + dgain;

    if (sns_state->wdr_mode == OT_WDR_MODE_NONE) {
        sns_state->regs_info[0].i2c_data[3].data = (tmp & 0xFF); /* index 3 */
        sns_state->regs_info[0].i2c_data[4].data = (hcg & 0xFF); /* index 4 */
    } else if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        sns_state->regs_info[0].i2c_data[3].data = (tmp & 0xFF); /* index 3 */
        sns_state->regs_info[0].i2c_data[4].data = (hcg & 0xFF); /* index 4 */
        sns_state->regs_info[0].i2c_data[20].data = (hcg & 0xFF); /* index 20 */
        sns_state->regs_info[0].i2c_data[21].data = (hcg & 0xFF); /* index 21 */
    }

    return;
}

/* limitation for DOL 3t1
   SHR1 limitation
   18 or more
   RHS1 - 6 or less

   RHS1 Limitation
   6n + 2 (n =: 0,1,2...)
   (SHR1 + 6)<=RHS1<=(SHR2 - 14)

   SHR2 limitation
   RHS1 + 14 or more
   RHS2 - 6 or less

   RHS2 Limitation
   6n + 4 (n =: 0,1,2...)
   RHS2 <= SHR0 - 14

   SHR0 limitation
   RHS2 + 14 or more
   FSC - 6 or less
   short exposure time 1 =: RHS1 - SHR1 <=: RHS1 - 18
   short exposure time 2 =: RHS2 - SHR2 <=: RHS2 - (RHS1 + 14)
   short exposure time 2 <=: (RHS2 - 32) / (ratio[0] + 1)
   long exposure time =: FSC - SHR0 <=: FSC - (RHS2 + 14)
   short exposure time 1 + short exposure time 2 + long exposure time <=: FSC - 46
   short exposure time 2 <=: (FSC - 46) / (ratio[0]*ratio[1] + ratio[0] + 1)
*/

static td_void cmos_clip_ratio_range(td_u32 *a_exp_ratio, const ot_isp_ae_int_time_range *int_time,
    td_u32 wdr_frame_num)
{
    td_u32 i, ratio_min, ratio_max;
    for (i = 0; i < wdr_frame_num - 1; ++i) {
        ratio_max = int_time->int_time_max[i + 1] * 0x40 / int_time->int_time_min[i];
        ratio_min = int_time->int_time_min[i + 1] * 0x40 / int_time->int_time_max[i];
        a_exp_ratio[i] = MIN2(MAX2(a_exp_ratio[i], ratio_min), ratio_max);
        a_exp_ratio[i] = MIN2(MAX2(a_exp_ratio[i], 0x40), 0x4000);
    }
}

static td_void cmos_step_limit(ot_isp_ae_int_time_range *int_time,
    const time_step *step, td_u32 full_lines, td_u32 wdr_frame_num)
{
    td_u32 i;
    td_u32 max_int_time = 0;
    for (i = 0; i < wdr_frame_num; ++i) {
        if (int_time->pre_int_time[i] == 0) {
            return ;
        }
    }
    for (i = 0; i < wdr_frame_num; ++i) {
        if (step->inc[i] > 0) {
            int_time->int_time_max[i] = MIN2(int_time->pre_int_time[i] + step->inc[i], int_time->int_time_max[i]);
        }
        if (step->dec[i] > 0) {
            if (int_time->pre_int_time[i] > step->dec[i]) {
                int_time->int_time_min[i] = MAX2(int_time->pre_int_time[i] - step->dec[i], int_time->int_time_min[i]);
            } else {
                int_time->int_time_min[i] = MAX2(int_time->int_time_min[i], 0);
            }
        }
        if (int_time->int_time_min[i] > int_time->int_time_max[i]) {
            int_time->int_time_max[i] = int_time->int_time_min[i];
        }
        /* make sure LEF > SEF1 and SEF1 > SEF2 */
        if (i > 0) {
            int_time->int_time_max[i] = MAX2(int_time->int_time_max[i], int_time->int_time_max[i - 1]);
            int_time->int_time_min[i] = MAX2(int_time->int_time_min[i], int_time->int_time_min[i - 1]);
        }
    }
    /* make sure max_int_time < full_lines */
    for (i = 0; i < wdr_frame_num; ++i) {
        max_int_time += int_time->int_time_max[i];
    }
    if (max_int_time > full_lines) {
        max_int_time = max_int_time - full_lines;
        for (i = 0; i < wdr_frame_num; ++i) {
            if (int_time->int_time_max[i] - int_time->int_time_min[i] > max_int_time) {
                int_time->int_time_max[i] = int_time->int_time_max[i] - max_int_time;
                return;
            }
        }
    }
    return;
}

static td_void cmos_set_3to1_long_frame_range(ot_isp_ae_int_time_range *int_time,
    td_s32 time_min, td_s32 time_max, td_s32 step)
{
    int_time->int_time_max[0] = MAX2((td_s32)int_time->pre_int_time[0] - step, time_min);    /* index 0 */
    int_time->int_time_min[0] = MAX2((td_s32)int_time->pre_int_time[0] - step, time_min);    /* index 0 */
    int_time->int_time_max[1] = MAX2((td_s32)int_time->pre_int_time[1] - step, time_min);    /* index 1 */
    int_time->int_time_min[1] = MAX2((td_s32)int_time->pre_int_time[1] - step, time_min);    /* index 1 */
    int_time->int_time_max[2] = time_max;                                                       /* index 2 */
    int_time->int_time_min[2] = time_min;                                                       /* index 2 */
}

static td_void cmos_get_inttime_max_3to1_line(ot_vi_pipe vi_pipe,
    td_u32 *ratio, ot_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time)
{
    const td_u32 frame_num = 3;
    time_step step;
    ot_isp_sns_state *sns_state = TD_NULL;
    td_u32 short_max0, short_max, short_time_min_limit, rhs2_max, i;

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; ++i) {
        step.inc[i] = 0;
        step.dec[i] = 0;
    }

    ot_unused(lf_max_int_time);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);
    short_time_min_limit = 6; /* short_time_min_limit 6 */
    if (g_fswdr_mode[vi_pipe] == OT_ISP_FSWDR_LONG_FRAME_MODE) {
        short_max0 = sns_state->fl[1] - 100 - sns_state->wdr_int_time[0] - sns_state->wdr_int_time[1]; /* sub 100 */
        short_max = sns_state->fl[0] - 6; /* sensor limit: sub 6 */
        short_max = (short_max0 < short_max) ? short_max0 : short_max;
        cmos_set_3to1_long_frame_range(int_time, short_time_min_limit, short_max, 90); /* step 90 */
        return;
    } else {
        short_max0 = ((sns_state->fl[1] - 100 - sns_state->wdr_int_time[0] - sns_state->wdr_int_time[1]) * /* sub 100 */
                      0x40 * 0x40) / div_0_to_1(ratio[0] * ratio[1]);
        short_max = ((sns_state->fl[0] - 100) * 0x40) /  /* sub 100 */
                    div_0_to_1(ratio[0] * ratio[1] / IMX485_RATIO + ratio[0] + 0x40);
        short_max = (short_max0 < short_max) ? short_max0 : short_max;
        rhs2_max = ((g_imx485_state[vi_pipe].rhs2_max - 32) * 0x40) / (ratio[0] + 0x40); /* sub 32 */
        short_max = (short_max > rhs2_max) ? rhs2_max : short_max;
    }

    int_time->int_time_max[0] = short_max >= short_time_min_limit ? short_max : short_time_min_limit;
    int_time->int_time_min[0] = short_max >= short_time_min_limit ? short_time_min_limit : int_time->int_time_max[0];
    if (short_max >= short_time_min_limit) {
        int_time->int_time_max[1] = (int_time->int_time_max[0] * ratio[0]) >> 6; /* shift 6 */
        int_time->int_time_max[2] = (int_time->int_time_max[1] * ratio[1]) >> 6; /* index 2, shift 6 */
        int_time->int_time_min[1] = (int_time->int_time_min[0] * ratio[0]) >> 6; /* shift 6 */
        int_time->int_time_min[2] = (int_time->int_time_min[1] * ratio[1]) >> 6; /* index 2, shift 6 */
    } else {
        int_time->int_time_max[1] = (int_time->int_time_max[0] * 0xFFF) >> 6; /* shift 6 */
        int_time->int_time_max[2] = (int_time->int_time_max[1] * 0xFFF) >> 6; /* index 2, shift 6 */
        int_time->int_time_min[1] = int_time->int_time_max[1];
        int_time->int_time_min[2] = int_time->int_time_max[2]; /* index 2 */
    }
    /* calc step limit */
    if (g_imx485_state[vi_pipe].brl * frame_num < sns_state->fl[1]) {
        step.dec[0] = (sns_state->fl[1] - g_imx485_state[vi_pipe].brl * frame_num + 3) >> 1; /* default value 3 */
        step.dec[1] = (sns_state->fl[1] - g_imx485_state[vi_pipe].brl * frame_num + 3) >> 1; /* default value 3 */
    }
    cmos_step_limit(int_time, &step, sns_state->fl[1], frame_num);
    cmos_clip_ratio_range(ratio, (const ot_isp_ae_int_time_range *)int_time, frame_num);
    return;
}

static td_void cmos_get_inttime_max(ot_vi_pipe vi_pipe, td_u16 man_ratio_enable, td_u32 *ratio,
    ot_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time)
{
    ot_unused(man_ratio_enable);
    ot_isp_sns_state *sns_state = TD_NULL;
    sensor_check_pointer_void_return(ratio);
    sensor_check_pointer_void_return(int_time);
    sensor_check_pointer_void_return(lf_max_int_time);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_3To1_LINE:
            cmos_get_inttime_max_3to1_line(vi_pipe, ratio, int_time, lf_max_int_time);
            break;
        default:
            break;
    }

    return;
}

/* Only used in LINE_WDR mode */
static td_void cmos_ae_fswdr_attr_set(ot_vi_pipe vi_pipe, ot_isp_ae_fswdr_attr *ae_fswdr_attr)
{
    sensor_check_pointer_void_return(ae_fswdr_attr);

    g_fswdr_mode[vi_pipe] = ae_fswdr_attr->fswdr_mode;
    g_max_time_get_cnt[vi_pipe] = 0;
    return;
}

static td_s32 cmos_init_ae_exp_function(ot_isp_ae_sensor_exp_func *exp_func)
{
    sensor_check_pointer_return(exp_func);

    (td_void)memset_s(exp_func, sizeof(ot_isp_ae_sensor_exp_func), 0, sizeof(ot_isp_ae_sensor_exp_func));

    exp_func->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    exp_func->pfn_cmos_fps_set           = cmos_fps_set;
    exp_func->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    exp_func->pfn_cmos_inttime_update    = cmos_inttime_update;
    exp_func->pfn_cmos_gains_update      = cmos_gains_update;
    exp_func->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    exp_func->pfn_cmos_dgain_calc_table  = cmos_dgain_calc_table;
    exp_func->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
    exp_func->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return TD_SUCCESS;
}

/* awb static param for Fuji Lens New IR_Cut */
#define CALIBRATE_STATIC_TEMP                         4950
#define CALIBRATE_STATIC_WB_R_GAIN                    450
#define CALIBRATE_STATIC_WB_GR_GAIN                   256
#define CALIBRATE_STATIC_WB_GB_GAIN                   256
#define CALIBRATE_STATIC_WB_B_GAIN                    447

/* Calibration results for Auto WB Planck */
#define CALIBRATE_AWB_P1                              (-53)
#define CALIBRATE_AWB_P2                              309
#define CALIBRATE_AWB_Q1                              0
#define CALIBRATE_AWB_A1                              160279
#define CALIBRATE_AWB_B1                              128
#define CALIBRATE_AWB_C1                              (-110119)

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN                                  0
#define GOLDEN_BGAIN                                  0
static td_s32 cmos_get_awb_default(ot_vi_pipe vi_pipe, ot_isp_awb_sensor_default *awb_sns_dft)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(awb_sns_dft);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    (td_void)memset_s(awb_sns_dft, sizeof(ot_isp_awb_sensor_default), 0, sizeof(ot_isp_awb_sensor_default));
    awb_sns_dft->wb_ref_temp = CALIBRATE_STATIC_TEMP; /* wb_ref_temp 4950 */

    awb_sns_dft->gain_offset[0] = CALIBRATE_STATIC_WB_R_GAIN;
    awb_sns_dft->gain_offset[1] = CALIBRATE_STATIC_WB_GR_GAIN;
    awb_sns_dft->gain_offset[2] = CALIBRATE_STATIC_WB_GB_GAIN; /* index 2 */
    awb_sns_dft->gain_offset[3] = CALIBRATE_STATIC_WB_B_GAIN; /* index 3 */

    awb_sns_dft->wb_para[0] = CALIBRATE_AWB_P1;
    awb_sns_dft->wb_para[1] = CALIBRATE_AWB_P2;
    awb_sns_dft->wb_para[2] = CALIBRATE_AWB_Q1; /* index 2 */
    awb_sns_dft->wb_para[3] = CALIBRATE_AWB_A1; /* index 3 */
    awb_sns_dft->wb_para[4] = CALIBRATE_AWB_B1; /* index 4 */
    awb_sns_dft->wb_para[5] = CALIBRATE_AWB_C1; /* index 5 */

    awb_sns_dft->golden_rgain = GOLDEN_RGAIN;
    awb_sns_dft->golden_bgain = GOLDEN_BGAIN;

    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_NONE:
            (td_void)memcpy_s(&awb_sns_dft->ccm, sizeof(ot_isp_awb_ccm), &g_awb_ccm, sizeof(ot_isp_awb_ccm));
            (td_void)memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_isp_awb_agc_table),
                              &g_awb_agc_table, sizeof(ot_isp_awb_agc_table));
            break;

        case OT_WDR_MODE_3To1_LINE:
            (td_void)memcpy_s(&awb_sns_dft->ccm, sizeof(ot_isp_awb_ccm), &g_awb_ccm_wdr, sizeof(ot_isp_awb_ccm));
            (td_void)memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_isp_awb_agc_table),
                              &g_awb_agc_table_wdr, sizeof(ot_isp_awb_agc_table));

            break;

        default:
            (td_void)memcpy_s(&awb_sns_dft->ccm, sizeof(ot_isp_awb_ccm), &g_awb_ccm, sizeof(ot_isp_awb_ccm));
            (td_void)memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_isp_awb_agc_table),
                              &g_awb_agc_table, sizeof(ot_isp_awb_agc_table));
            break;
    }

    awb_sns_dft->init_rgain = g_init_wb_gain[vi_pipe][0]; /* 0: Rgain */
    awb_sns_dft->init_ggain = g_init_wb_gain[vi_pipe][1]; /* 1: Ggain */
    awb_sns_dft->init_bgain = g_init_wb_gain[vi_pipe][2]; /* 2: Bgain */
    awb_sns_dft->sample_rgain = g_sample_r_gain[vi_pipe];
    awb_sns_dft->sample_bgain = g_sample_b_gain[vi_pipe];

    return TD_SUCCESS;
}

static td_s32 cmos_init_awb_exp_function(ot_isp_awb_sensor_exp_func *exp_func)
{
    sensor_check_pointer_return(exp_func);

    (td_void)memset_s(exp_func, sizeof(ot_isp_awb_sensor_exp_func), 0, sizeof(ot_isp_awb_sensor_exp_func));

    exp_func->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return TD_SUCCESS;
}

static ot_isp_cmos_dng_color_param g_dng_color_param = {{ 378, 256, 430 }, { 439, 256, 439 }};

static td_void cmos_get_isp_dng_default(const ot_isp_sns_state *sns_state, ot_isp_cmos_default *isp_def)
{
    (td_void)memcpy_s(&isp_def->dng_color_param, sizeof(ot_isp_cmos_dng_color_param), &g_dng_color_param,
                      sizeof(ot_isp_cmos_dng_color_param));

    switch (sns_state->img_mode) {
        case IMX485_SENSOR_2160P_30FPS_LINEAR_MODE:
            isp_def->sensor_mode.dng_raw_format.bits_per_sample = 12; /* 12bit */
            isp_def->sensor_mode.dng_raw_format.white_level = 4095; /* max 4095 */
            break;

        case IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE:
            isp_def->sensor_mode.dng_raw_format.bits_per_sample = 10; /* 10bit */
            isp_def->sensor_mode.dng_raw_format.white_level = 1023; /* max 1023 */
            break;

        default:
            isp_def->sensor_mode.dng_raw_format.bits_per_sample = 12; /* 12bit */
            isp_def->sensor_mode.dng_raw_format.white_level = 4095; /* max 4095 */
            break;
    }

    isp_def->sensor_mode.dng_raw_format.default_scale.default_scale_hor.denominator = 1;
    isp_def->sensor_mode.dng_raw_format.default_scale.default_scale_hor.numerator = 1;
    isp_def->sensor_mode.dng_raw_format.default_scale.default_scale_ver.denominator = 1;
    isp_def->sensor_mode.dng_raw_format.default_scale.default_scale_ver.numerator = 1;
    isp_def->sensor_mode.dng_raw_format.cfa_repeat_pattern_dim.repeat_pattern_dim_row = 2; /* pattern 2 */
    isp_def->sensor_mode.dng_raw_format.cfa_repeat_pattern_dim.repeat_pattern_dim_col = 2; /* pattern 2 */
    isp_def->sensor_mode.dng_raw_format.black_level_repeat_dim.repeat_row = 2; /* pattern 2 */
    isp_def->sensor_mode.dng_raw_format.black_level_repeat_dim.repeat_col = 2; /* pattern 2 */
    isp_def->sensor_mode.dng_raw_format.cfa_layout = OT_ISP_CFALAYOUT_TYPE_RECTANGULAR;
    isp_def->sensor_mode.dng_raw_format.cfa_plane_color[0] = 0;
    isp_def->sensor_mode.dng_raw_format.cfa_plane_color[1] = 1;
    isp_def->sensor_mode.dng_raw_format.cfa_plane_color[2] = 2; /* index 2, cfa_plane_color 2 */
    isp_def->sensor_mode.dng_raw_format.cfa_pattern[0] = 0;
    isp_def->sensor_mode.dng_raw_format.cfa_pattern[1] = 1;
    isp_def->sensor_mode.dng_raw_format.cfa_pattern[2] = 1; /* index 2, cfa_pattern 1 */
    isp_def->sensor_mode.dng_raw_format.cfa_pattern[3] = 2; /* index 3, cfa_pattern 2 */
    isp_def->sensor_mode.valid_dng_raw_format = TD_TRUE;
    return;
}

static void cmos_get_isp_linear_default(ot_isp_cmos_default *isp_def)
{
    isp_def->key.bit1_demosaic         = 1;
    isp_def->demosaic                  = &g_cmos_demosaic;
    isp_def->key.bit1_sharpen          = 1;
    isp_def->sharpen                   = &g_cmos_yuv_sharpen;
    isp_def->key.bit1_drc              = 1;
    isp_def->drc                       = &g_cmos_drc;
    isp_def->key.bit1_bayer_nr         = 1;
    isp_def->bayer_nr                  = &g_cmos_bayer_nr;
    isp_def->key.bit1_anti_false_color = 1;
    isp_def->anti_false_color          = &g_cmos_anti_false_color;
    isp_def->key.bit1_ldci             = 1;
    isp_def->ldci                      = &g_cmos_ldci;
    isp_def->key.bit1_gamma            = 1;
    isp_def->gamma                     = &g_cmos_gamma;
#ifdef CONFIG_OT_ISP_CR_SUPPORT
    isp_def->key.bit1_ge               = 1;
    isp_def->ge                        = &g_cmos_ge;
#endif
    isp_def->key.bit1_dehaze = 1;
    isp_def->dehaze = &g_cmos_dehaze;
    isp_def->key.bit1_ca = 1;
    isp_def->ca = &g_cmos_ca;
    (td_void)memcpy_s(&isp_def->noise_calibration, sizeof(ot_isp_noise_calibration),
                      &g_cmos_noise_calibration, sizeof(ot_isp_noise_calibration));
    return;
}

static void cmos_get_isp_wdr_default(ot_isp_cmos_default *isp_def)
{
    isp_def->key.bit1_dpc            = 1;
    isp_def->dpc                     = &g_cmos_dpc_wdr;
    isp_def->key.bit1_demosaic       = 1;
    isp_def->demosaic                = &g_cmos_demosaic_wdr;
    isp_def->key.bit1_sharpen        = 1;
    isp_def->sharpen                 = &g_cmos_yuv_sharpen_wdr;
    isp_def->key.bit1_drc            = 1;
    isp_def->drc                     = &g_cmos_drc_wdr;
    isp_def->key.bit1_gamma          = 1;
    isp_def->gamma                   = &g_cmos_gamma_wdr;
#ifdef CONFIG_OT_ISP_PREGAMMA_SUPPORT
    isp_def->key.bit1_pregamma       = 1;
    isp_def->pregamma                = &g_cmos_pregamma;
#endif
    isp_def->key.bit1_bayer_nr       = 1;
    isp_def->bayer_nr                = &g_cmos_bayer_nr_wdr;
#ifdef CONFIG_OT_ISP_CR_SUPPORT
    isp_def->key.bit1_ge             = 1;
    isp_def->ge                      = &g_cmos_ge_wdr;
#endif
    isp_def->key.bit1_anti_false_color = 1;
    isp_def->anti_false_color = &g_cmos_anti_false_color_wdr;
    isp_def->key.bit1_ldci = 1;
    isp_def->ldci = &g_cmos_ldci_wdr;
    isp_def->key.bit1_dehaze = 1;
    isp_def->dehaze = &g_cmos_dehaze_wdr;
    isp_def->key.bit1_crb = 1;
    isp_def->crb = &g_cmos_crb_wdr;
    (td_void)memcpy_s(&isp_def->noise_calibration, sizeof(ot_isp_noise_calibration),
                      &g_cmos_noise_calibration, sizeof(ot_isp_noise_calibration));
    return;
}

static td_s32 cmos_get_isp_default(ot_vi_pipe vi_pipe, ot_isp_cmos_default *isp_def)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(isp_def);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    (td_void)memset_s(isp_def, sizeof(ot_isp_cmos_default), 0, sizeof(ot_isp_cmos_default));
#ifdef CONFIG_OT_ISP_CA_SUPPORT
    isp_def->key.bit1_ca      = 1;
    isp_def->ca               = &g_cmos_ca;
#endif
    isp_def->key.bit1_clut    = 1;
    isp_def->clut             = &g_cmos_clut;

    isp_def->key.bit1_dpc     = 1;
    isp_def->dpc              = &g_cmos_dpc;

    isp_def->key.bit1_wdr     = 1;
    isp_def->wdr              = &g_cmos_wdr;

    isp_def->key.bit1_lsc      = 0;
    isp_def->lsc               = &g_cmos_lsc;

    isp_def->key.bit1_acs      = 0;
    isp_def->acs               = &g_cmos_acs;

#ifdef CONFIG_OT_ISP_PREGAMMA_SUPPORT
    isp_def->key.bit1_pregamma = 1;
    isp_def->pregamma          = &g_cmos_pregamma;
#endif
    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_NONE:
            cmos_get_isp_linear_default(isp_def);
            break;

        case OT_WDR_MODE_3To1_LINE:
            cmos_get_isp_wdr_default(isp_def);

            break;

        default:
            cmos_get_isp_linear_default(isp_def);
            break;
    }

    isp_def->wdr_switch_attr.exp_ratio[0] = 0x40;

    if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        isp_def->wdr_switch_attr.exp_ratio[0] = 0x200;
        isp_def->wdr_switch_attr.exp_ratio[1] = 0x200;
    }

    isp_def->sensor_mode.sensor_id = IMX485_ID;
    isp_def->sensor_mode.sensor_mode = sns_state->img_mode;
    cmos_get_isp_dng_default(sns_state, isp_def);

    return TD_SUCCESS;
}

static td_s32 cmos_get_isp_black_level(ot_vi_pipe vi_pipe, ot_isp_cmos_black_level *black_level)
{
    td_s32  i;
    ot_isp_sns_state *sns_state = TD_NULL;
    const ot_isp_cmos_black_level *cmos_blc_def = TD_NULL;

    sensor_check_pointer_return(black_level);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);
    if ((sns_state->wdr_mode == OT_WDR_MODE_NONE) && (is_virt_pipe(vi_pipe) == TD_FALSE)) {
        cmos_blc_def = &g_cmos_blc;
    } else {
        cmos_blc_def = &g_cmos_blc_wdr;
    }
    (td_void)memcpy_s(black_level, sizeof(ot_isp_cmos_black_level), cmos_blc_def, sizeof(ot_isp_cmos_black_level));

    /* Don't need to update black level when iso change */
    black_level->auto_attr.update = TD_FALSE;

    /* black level of linear mode */
    if (sns_state->wdr_mode == OT_WDR_MODE_NONE) {
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            black_level->auto_attr.black_level[0][i] = IMX485_BLACK_LEVEL;
        }
    } else { /* black level of DOL mode */
        for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
            black_level->auto_attr.black_level[i][0] = IMX485_BLACK_LEVEL;
            black_level->auto_attr.black_level[i][1] = IMX485_BLACK_LEVEL;
            black_level->auto_attr.black_level[i][2] = IMX485_BLACK_LEVEL; /* index 2 */
            black_level->auto_attr.black_level[i][3] = IMX485_BLACK_LEVEL; /* index 3 */
        }
    }

    return TD_SUCCESS;
}

static td_s32 cmos_get_isp_blc_clamp_info(ot_vi_pipe vi_pipe, td_bool *blc_clamp_en)
{
    sensor_check_pointer_return(blc_clamp_en);

    *blc_clamp_en = blc_clamp_info[vi_pipe];

    return TD_SUCCESS;
}

static td_void cmos_set_pixel_detect(ot_vi_pipe vi_pipe, td_bool enable)
{
    td_u32 full_lines_5fps, max_int_time_5fps;
    ot_isp_sns_state *sns_state = TD_NULL;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        return;
    } else {
        if (sns_state->img_mode == IMX485_SENSOR_2160P_30FPS_LINEAR_MODE)  {
            full_lines_5fps = (IMX485_VMAX_2160P30_LINEAR * 30) / 5; /* 30fps, 5fps */
        } else {
            return;
        }
    }

    max_int_time_5fps = 6; /* max_int_time_5fps 6 */

    if (enable) { /* setup for ISP pixel calibration mode */
        imx485_write_register(vi_pipe, IMX485_GAIN_ADDR, 0x00);

        imx485_write_register(vi_pipe, IMX485_VMAX_ADDR, low_8bits(full_lines_5fps));
        imx485_write_register(vi_pipe, IMX485_VMAX_ADDR + 1, high_8bits(full_lines_5fps));
        imx485_write_register(vi_pipe, IMX485_VMAX_ADDR + 2, higher_4bits(full_lines_5fps)); /* index 2 */

        imx485_write_register(vi_pipe, IMX485_SHR0_ADDR, low_8bits(max_int_time_5fps));
        imx485_write_register(vi_pipe, IMX485_SHR0_ADDR + 1, high_8bits(max_int_time_5fps));
        imx485_write_register(vi_pipe, IMX485_SHR0_ADDR + 2, higher_4bits(max_int_time_5fps)); /* index 2 */
    } else { /* setup for ISP 'normal mode' */
        sns_state->fl_std = (sns_state->fl_std > 0xFFFFF) ? 0xFFFFF : sns_state->fl_std;
        imx485_write_register(vi_pipe, IMX485_VMAX_ADDR, low_8bits(sns_state->fl_std));
        imx485_write_register(vi_pipe, IMX485_VMAX_ADDR + 1, high_8bits(sns_state->fl_std));
        imx485_write_register(vi_pipe, IMX485_VMAX_ADDR + 2, higher_4bits(sns_state->fl_std)); /* index 2 */
        sns_state->sync_init = TD_FALSE;
    }

    return;
}

static td_s32 cmos_set_wdr_mode(ot_vi_pipe vi_pipe, td_u8 mode)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    sns_state->sync_init    = TD_FALSE;

    switch (mode & 0x3F) {
        case OT_WDR_MODE_NONE:
            sns_state->wdr_mode = OT_WDR_MODE_NONE;
            printf("linear mode\n");
            break;

        case OT_WDR_MODE_3To1_LINE:
            sns_state->wdr_mode = OT_WDR_MODE_3To1_LINE;
            printf("3to1 line WDR 2160p mode(90fps->30fps)\n");
            break;

        default:
            isp_err_trace("NOT support this mode!\n");
            return TD_FAILURE;
    }

    (td_void)memset_s(sns_state->wdr_int_time, sizeof(sns_state->wdr_int_time), 0, sizeof(sns_state->wdr_int_time));

    return TD_SUCCESS;
}

static td_void cmos_comm_sns_reg_info_init(ot_vi_pipe vi_pipe, ot_isp_sns_state *sns_state)
{
    td_u32 i;
    sns_state->regs_info[0].sns_type = OT_ISP_SNS_I2C_TYPE;
    sns_state->regs_info[0].com_bus.i2c_dev = g_imx485_bus_info[vi_pipe].i2c_dev;
    sns_state->regs_info[0].cfg2_valid_delay_max = 2; /* delay_max 2 */
    sns_state->regs_info[0].reg_num = 8; /* reg_num 8 */

    if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
        sns_state->regs_info[0].reg_num = 22; /* reg_num 22 */
        sns_state->regs_info[0].cfg2_valid_delay_max = 2; /* delay_max 2 */
    }

    for (i = 0; i < sns_state->regs_info[0].reg_num; i++) {
        sns_state->regs_info[0].i2c_data[i].update = TD_TRUE;
        sns_state->regs_info[0].i2c_data[i].dev_addr = IMX485_I2C_ADDR;
        sns_state->regs_info[0].i2c_data[i].addr_byte_num = IMX485_ADDR_BYTE;
        sns_state->regs_info[0].i2c_data[i].data_byte_num = IMX485_DATA_BYTE;
    }

    /* Linear Mode Regs */
    sns_state->regs_info[0].i2c_data[0].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[0].reg_addr = IMX485_SHR0_ADDR;
    sns_state->regs_info[0].i2c_data[1].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[1].reg_addr = IMX485_SHR0_ADDR + 1;
    sns_state->regs_info[0].i2c_data[2].delay_frame_num = 0; /* index 2 */
    sns_state->regs_info[0].i2c_data[2].reg_addr = IMX485_SHR0_ADDR + 2; /* index 2, index 2 */

    /* gain */
    sns_state->regs_info[0].i2c_data[3].delay_frame_num = 0; /* index 3 */
    sns_state->regs_info[0].i2c_data[3].reg_addr = IMX485_GAIN_ADDR; /* index 3 */
    sns_state->regs_info[0].i2c_data[4].delay_frame_num = 1;  /* index 4 */
    sns_state->regs_info[0].i2c_data[4].reg_addr = IMX485_HCG_ADDR; /* index 4 */

    sns_state->regs_info[0].i2c_data[5].delay_frame_num = 0; /* index 5 */
    sns_state->regs_info[0].i2c_data[5].reg_addr = IMX485_VMAX_ADDR; /* index 5 */
    sns_state->regs_info[0].i2c_data[6].delay_frame_num = 0; /* index 6 */
    sns_state->regs_info[0].i2c_data[6].reg_addr = IMX485_VMAX_ADDR + 1; /* index 6 */
    sns_state->regs_info[0].i2c_data[7].delay_frame_num = 0; /* index 7 */
    sns_state->regs_info[0].i2c_data[7].reg_addr = IMX485_VMAX_ADDR + 2; /* index 7, index 2 */
    return;
}

static td_void cmos_3to1_line_wdr_sns_reg_info_init(ot_vi_pipe vi_pipe, ot_isp_sns_state *sns_state)
{
    ot_unused(vi_pipe);
    sns_state->regs_info[0].i2c_data[0].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[1].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[2].delay_frame_num = 0; /* index 2 */

    sns_state->regs_info[0].i2c_data[5].delay_frame_num = 0; /* index 5 */
    sns_state->regs_info[0].i2c_data[5].reg_addr = IMX485_SHR1_ADDR; /* index 5 */
    sns_state->regs_info[0].i2c_data[6].delay_frame_num = 0; /* index 6 */
    sns_state->regs_info[0].i2c_data[6].reg_addr = IMX485_SHR1_ADDR + 1; /* index 6 */
    sns_state->regs_info[0].i2c_data[7].delay_frame_num = 0; /* index 7 */
    sns_state->regs_info[0].i2c_data[7].reg_addr = IMX485_SHR1_ADDR + 2; /* index 7, index 2 */

    sns_state->regs_info[0].i2c_data[8].delay_frame_num = 0; /* index 8 */
    sns_state->regs_info[0].i2c_data[8].reg_addr = IMX485_SHR2_ADDR; /* index 8 */
    sns_state->regs_info[0].i2c_data[9].delay_frame_num = 0; /* index 9 */
    sns_state->regs_info[0].i2c_data[9].reg_addr = IMX485_SHR2_ADDR + 1; /* index 9 */
    sns_state->regs_info[0].i2c_data[10].delay_frame_num = 0; /* index 10 */
    sns_state->regs_info[0].i2c_data[10].reg_addr = IMX485_SHR2_ADDR + 2; /* index 10, index 2 */

    sns_state->regs_info[0].i2c_data[11].delay_frame_num = 1; /* index 11 */
    sns_state->regs_info[0].i2c_data[11].reg_addr = IMX485_VMAX_ADDR; /* index 11 */
    sns_state->regs_info[0].i2c_data[12].delay_frame_num = 1; /* index 12 */
    sns_state->regs_info[0].i2c_data[12].reg_addr = IMX485_VMAX_ADDR + 1; /* index 12 */
    sns_state->regs_info[0].i2c_data[13].delay_frame_num = 1; /* index 13 */
    sns_state->regs_info[0].i2c_data[13].reg_addr = IMX485_VMAX_ADDR + 2; /* index 13, index 2 */

    sns_state->regs_info[0].i2c_data[14].delay_frame_num = 0; /* index 14 */
    sns_state->regs_info[0].i2c_data[14].reg_addr = IMX485_RHS1_ADDR; /* index 14 */
    sns_state->regs_info[0].i2c_data[15].delay_frame_num = 0; /* index 15 */
    sns_state->regs_info[0].i2c_data[15].reg_addr = IMX485_RHS1_ADDR + 1; /* index 15 */
    sns_state->regs_info[0].i2c_data[16].delay_frame_num = 0; /* index 16 */
    sns_state->regs_info[0].i2c_data[16].reg_addr = IMX485_RHS1_ADDR + 2; /* index 16, index 2 */

    sns_state->regs_info[0].i2c_data[17].delay_frame_num = 0; /* index 17 */
    sns_state->regs_info[0].i2c_data[17].reg_addr = IMX485_RHS2_ADDR; /* index 17 */
    sns_state->regs_info[0].i2c_data[18].delay_frame_num = 0; /* index 18 */
    sns_state->regs_info[0].i2c_data[18].reg_addr = IMX485_RHS2_ADDR + 1; /* index 18 */
    sns_state->regs_info[0].i2c_data[19].delay_frame_num = 0; /* index 19 */
    sns_state->regs_info[0].i2c_data[19].reg_addr = IMX485_RHS2_ADDR + 2; /* index 19, index 2 */

    sns_state->regs_info[0].i2c_data[20].delay_frame_num = 1; /* index 20 */
    sns_state->regs_info[0].i2c_data[20].reg_addr = IMX485_HCG_SEL1_ADDR; /* index 20 */
    sns_state->regs_info[0].i2c_data[21].delay_frame_num = 1; /* index 21 */
    sns_state->regs_info[0].i2c_data[21].reg_addr = IMX485_HCG_SEL2_ADDR; /* index 21 */

    return;
}

static td_void cmos_sns_reg_info_update(ot_vi_pipe vi_pipe, ot_isp_sns_state *sns_state)
{
    td_u32 i;
    ot_unused(vi_pipe);

    for (i = 0; i < sns_state->regs_info[0].reg_num; i++) {
        if (sns_state->regs_info[0].i2c_data[i].data ==
            sns_state->regs_info[1].i2c_data[i].data) {
            sns_state->regs_info[0].i2c_data[i].update = TD_FALSE;
        } else {
            sns_state->regs_info[0].i2c_data[i].update = TD_TRUE;
        }
    }
    return;
}

static td_s32 cmos_get_sns_regs_info(ot_vi_pipe vi_pipe, ot_isp_sns_regs_info *sns_regs_info)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(sns_regs_info);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    if ((sns_state->sync_init == TD_FALSE) || (sns_regs_info->config == TD_FALSE)) {
        cmos_comm_sns_reg_info_init(vi_pipe, sns_state);

        if (sns_state->wdr_mode == OT_WDR_MODE_3To1_LINE) {
            /* DOL 3t1 Mode Regs */
            cmos_3to1_line_wdr_sns_reg_info_init(vi_pipe, sns_state);
        }
        sns_state->sync_init = TD_TRUE;
    } else {
        cmos_sns_reg_info_update(vi_pipe, sns_state);
    }

    sns_regs_info->config = TD_FALSE;
    (td_void)memcpy_s(sns_regs_info, sizeof(ot_isp_sns_regs_info),
                      &sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info));
    (td_void)memcpy_s(&sns_state->regs_info[1], sizeof(ot_isp_sns_regs_info),
                      &sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info));
    sns_state->fl[1] = sns_state->fl[0];

    return TD_SUCCESS;
}

static td_void cmos_config_image_mode_param(ot_vi_pipe vi_pipe, td_u8 sensor_image_mode,
    ot_isp_sns_state *sns_state)
{
    switch (sensor_image_mode) {
        case IMX485_SENSOR_2160P_30FPS_LINEAR_MODE:
            sns_state->fl_std         = IMX485_VMAX_2160P30_LINEAR;
            g_imx485_state[vi_pipe].hcg = 0x1;
            break;
        case IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE:
            sns_state->fl_std         = IMX485_VMAX_2160P90TO30_WDR * 3; /* *3 */
            g_imx485_state[vi_pipe].brl = 2210; /* brl 2210 */
            g_imx485_state[vi_pipe].hcg = 0x1;
            break;
        default:
            sns_state->fl_std         = IMX485_VMAX_2160P30_LINEAR;
            g_imx485_state[vi_pipe].hcg = 0x1;
            break;
    }
}

static td_s32 cmos_set_image_mode(ot_vi_pipe vi_pipe, const ot_isp_cmos_sensor_image_mode *sensor_image_mode)
{
    td_u32 i;
    td_u8 image_mode;
    ot_isp_sns_state *sns_state = TD_NULL;
    sensor_check_pointer_return(sensor_image_mode);
    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    image_mode = sns_state->img_mode;
    for (i = 0; i < IMX485_MODE_BUTT; i++) {
        if (sensor_image_mode->fps <= g_imx485_mode_tbl[i].max_fps &&
            sensor_image_mode->width <= g_imx485_mode_tbl[i].width &&
            sensor_image_mode->height <= g_imx485_mode_tbl[i].height &&
            sns_state->wdr_mode == g_imx485_mode_tbl[i].wdr_mode) {
            image_mode = (imx485_res_mode)i;
            break;
        }
    }

    if (i >= IMX485_MODE_BUTT) {
        imx485_err_mode_print(sensor_image_mode, sns_state);
        return TD_FAILURE;
    }

    cmos_config_image_mode_param(vi_pipe, image_mode, sns_state);

    if ((sns_state->init == TD_TRUE) && (image_mode == sns_state->img_mode)) {
        return OT_ISP_DO_NOT_NEED_SWITCH_IMAGEMODE; /* Don't need to switch image_mode */
    }

    sns_state->sync_init = TD_FALSE;
    sns_state->img_mode = image_mode;
    sns_state->fl[0] = sns_state->fl_std;
    sns_state->fl[1] = sns_state->fl[0];
    return TD_SUCCESS;
}

static td_void sensor_global_init(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    imx485_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    sns_state->init = TD_FALSE;
    sns_state->sync_init = TD_FALSE;
    sns_state->img_mode = IMX485_SENSOR_2160P_30FPS_LINEAR_MODE;
    sns_state->wdr_mode = OT_WDR_MODE_NONE;
    sns_state->fl_std  = IMX485_VMAX_2160P30_LINEAR;
    sns_state->fl[0] = IMX485_VMAX_2160P30_LINEAR;
    sns_state->fl[1] = IMX485_VMAX_2160P30_LINEAR;

    (td_void)memset_s(&sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info), 0, sizeof(ot_isp_sns_regs_info));
    (td_void)memset_s(&sns_state->regs_info[1], sizeof(ot_isp_sns_regs_info), 0, sizeof(ot_isp_sns_regs_info));
    return;
}

static td_s32 cmos_init_sensor_exp_function(ot_isp_sensor_exp_func *sensor_exp_func)
{
    sensor_check_pointer_return(sensor_exp_func);

    (td_void)memset_s(sensor_exp_func, sizeof(ot_isp_sensor_exp_func), 0, sizeof(ot_isp_sensor_exp_func));

    sensor_exp_func->pfn_cmos_sensor_init         = imx485_init;
    sensor_exp_func->pfn_cmos_sensor_exit         = imx485_exit;
    sensor_exp_func->pfn_cmos_sensor_global_init  = sensor_global_init;
    sensor_exp_func->pfn_cmos_set_image_mode      = cmos_set_image_mode;
    sensor_exp_func->pfn_cmos_set_wdr_mode        = cmos_set_wdr_mode;
    sensor_exp_func->pfn_cmos_get_isp_default     = cmos_get_isp_default;
    sensor_exp_func->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    sensor_exp_func->pfn_cmos_get_blc_clamp_info  = cmos_get_isp_blc_clamp_info;
    sensor_exp_func->pfn_cmos_set_pixel_detect    = cmos_set_pixel_detect;
    sensor_exp_func->pfn_cmos_get_sns_reg_info    = cmos_get_sns_regs_info;

    return TD_SUCCESS;
}

static td_s32 imx485_set_bus_info(ot_vi_pipe vi_pipe, ot_isp_sns_commbus sns_bus_info)
{
    g_imx485_bus_info[vi_pipe].i2c_dev = sns_bus_info.i2c_dev;

    return TD_SUCCESS;
}

static td_s32 sensor_ctx_init(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state_ctx = TD_NULL;

    imx485_sensor_get_ctx(vi_pipe, sns_state_ctx);

    if (sns_state_ctx == TD_NULL) {
        sns_state_ctx = (ot_isp_sns_state *)malloc(sizeof(ot_isp_sns_state));
        if (sns_state_ctx == TD_NULL) {
            isp_err_trace("Isp[%d] SnsCtx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void)memset_s(sns_state_ctx, sizeof(ot_isp_sns_state), 0, sizeof(ot_isp_sns_state));

    imx485_sensor_set_ctx(vi_pipe, sns_state_ctx);

    return TD_SUCCESS;
}

static td_void sensor_ctx_exit(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state_ctx = TD_NULL;

    imx485_sensor_get_ctx(vi_pipe, sns_state_ctx);
    sensor_free(sns_state_ctx);
    imx485_sensor_reset_ctx(vi_pipe);
    return;
}

static td_s32 sensor_register_callback(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib)
{
    td_s32 ret;
    ot_isp_sensor_register isp_register;
    ot_isp_ae_sensor_register ae_register;
    ot_isp_awb_sensor_register awb_register;
    ot_isp_sns_attr_info sns_attr_info;

    sensor_check_pointer_return(ae_lib);
    sensor_check_pointer_return(awb_lib);

    ret = sensor_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    sns_attr_info.sensor_id = IMX485_ID;
    ret = cmos_init_sensor_exp_function(&isp_register.sns_exp);
    if (ret != TD_SUCCESS) {
        isp_err_trace("cmos init exp function failed!\n");
        return TD_FAILURE;
    }
    ret = ot_mpi_isp_sensor_reg_callback(vi_pipe, &sns_attr_info, &isp_register);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor register callback function failed!\n");
        return ret;
    }

    ret = cmos_init_ae_exp_function(&ae_register.sns_exp);
    if (ret != TD_SUCCESS) {
        isp_err_trace("cmos init ae exp function failed!\n");
        return TD_FAILURE;
    }
    ret = ot_mpi_ae_sensor_reg_callback(vi_pipe, ae_lib, &sns_attr_info, &ae_register);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor register callback function to ae lib failed!\n");
        return ret;
    }

    ret = cmos_init_awb_exp_function(&awb_register.sns_exp);
    if (ret != TD_SUCCESS) {
        isp_err_trace("cmos init awb exp function failed!\n");
        return TD_FAILURE;
    }
    ret = ot_mpi_awb_sensor_reg_callback(vi_pipe, awb_lib, &sns_attr_info, &awb_register);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor register callback function to awb lib failed!\n");
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 sensor_unregister_callback(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib)
{
    td_s32 ret;

    sensor_check_pointer_return(ae_lib);
    sensor_check_pointer_return(awb_lib);

    ret = ot_mpi_isp_sensor_unreg_callback(vi_pipe, IMX485_ID);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor unregister callback function failed!\n");
        return ret;
    }

    ret = ot_mpi_ae_sensor_unreg_callback(vi_pipe, ae_lib, IMX485_ID);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor unregister callback function to ae lib failed!\n");
        return ret;
    }

    ret = ot_mpi_awb_sensor_unreg_callback(vi_pipe, awb_lib, IMX485_ID);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor unregister callback function to awb lib failed!\n");
        return ret;
    }

    sensor_ctx_exit(vi_pipe);
    return TD_SUCCESS;
}

static td_s32 sensor_set_init(ot_vi_pipe vi_pipe, ot_isp_init_attr *init_attr)
{
    sensor_check_pointer_return(init_attr);

    g_init_exposure[vi_pipe]  = init_attr->exposure;
    g_lines_per500ms[vi_pipe] = init_attr->lines_per500ms;
    g_init_wb_gain[vi_pipe][0] = init_attr->wb_r_gain; /* 0: rgain */
    g_init_wb_gain[vi_pipe][1] = init_attr->wb_g_gain; /* 1: ggain */
    g_init_wb_gain[vi_pipe][2] = init_attr->wb_b_gain; /* 2: bgain */
    g_sample_r_gain[vi_pipe] = init_attr->sample_r_gain;
    g_sample_b_gain[vi_pipe] = init_attr->sample_b_gain;
    g_quick_start_en[vi_pipe] = init_attr->quick_start_en;

    g_ae_route_ex_valid[vi_pipe] = init_attr->ae_route_ex_valid;
    (td_void)memcpy_s(&g_init_ae_route[vi_pipe], sizeof(ot_isp_ae_route),
                      &init_attr->ae_route, sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&g_init_ae_route_ex[vi_pipe], sizeof(ot_isp_ae_route_ex),
                      &init_attr->ae_route_ex, sizeof(ot_isp_ae_route_ex));
    (td_void)memcpy_s(&g_init_ae_route_sf[vi_pipe], sizeof(ot_isp_ae_route),
                      &init_attr->ae_route_sf, sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&g_init_ae_route_sf_ex[vi_pipe], sizeof(ot_isp_ae_route_ex),
                      &init_attr->ae_route_sf_ex, sizeof(ot_isp_ae_route_ex));

    return TD_SUCCESS;
}

ot_isp_sns_obj g_sns_imx485_obj = {
    .pfn_register_callback     = sensor_register_callback,
    .pfn_un_register_callback  = sensor_unregister_callback,
    .pfn_standby               = imx485_standby,
    .pfn_restart               = imx485_restart,
    .pfn_mirror_flip           = TD_NULL,
    .pfn_set_blc_clamp         = imx485_blc_clamp,
    .pfn_write_reg             = imx485_write_register,
    .pfn_read_reg              = imx485_read_register,
    .pfn_set_bus_info          = imx485_set_bus_info,
    .pfn_set_init              = sensor_set_init
};
