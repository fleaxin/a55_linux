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
#include "os05a10_2l_slave_cmos_ex.h"
#include "os05a10_2l_slave_cmos.h"

#define OS05A10_SLAVE_ID                    05
#define SENSOR_OS05A10_SLAVE_WIDTH          2560
#define SENSOR_OS05A10_SLAVE_HEIGHT         1440
#define INCK_ONCE_INCREASE_MAX              750000

#define higher_4bits(x) (((x) & 0xf0000) >> 16)
#define high_8bits(x) (((x) & 0xff00) >> 8)
#define low_8bits(x)  ((x) & 0x00ff)

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/
#define os05a10_slave_sensor_set_ctx(dev, ctx)   ((g_os05a10_slave_sns_state[dev]) = (ctx))
#define os05a10_slave_sensor_reset_ctx(dev)      (g_os05a10_slave_sns_state[dev] = TD_NULL)

static ot_isp_fswdr_mode fswdr_mode[OT_ISP_MAX_PIPE_NUM] = {
    [0 ... OT_ISP_MAX_PIPE_NUM - 1] = OT_ISP_FSWDR_NORMAL_MODE
};

static td_u32 max_time_get_cnt[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u32 init_exposure[OT_ISP_MAX_PIPE_NUM]  = {0};
static td_u32 lines_per500ms[OT_ISP_MAX_PIPE_NUM] = {0};

static td_u16 init_wb_gain[OT_ISP_MAX_PIPE_NUM][OT_ISP_RGB_CHN_NUM] = {{0}};
static td_u16 sample_r_gain[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u16 sample_b_gain[OT_ISP_MAX_PIPE_NUM] = {0};
static td_bool quick_start_en[OT_ISP_MAX_PIPE_NUM] = {TD_FALSE};

static td_bool ae_route_ex_valid[OT_ISP_MAX_PIPE_NUM] = {0};
static ot_isp_ae_route init_ae_route[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route_ex init_ae_route_ex[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route init_ae_route_sf[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route_ex init_ae_route_sf_ex[OT_ISP_MAX_PIPE_NUM] = {{0}};

ot_isp_sns_commbus g_os05a10_slave_bus_info[OT_ISP_MAX_PIPE_NUM] = {
    [0] = {.i2c_dev = 0},
    [1 ... OT_ISP_MAX_PIPE_NUM - 1] = {.i2c_dev = -1}
};

td_s32 g_os05a10_slave_bind_dev[OT_ISP_MAX_PIPE_NUM] = {0, 0, 1, 1, 2, 2, 3, 3};
static td_u32 g_os05a10_slave_sensor_mode_time[OT_ISP_MAX_PIPE_NUM] = {0, 0, 0, 0, 0, 0, 0, 0};
ot_isp_sns_state *g_os05a10_slave_sns_state[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

static td_bool blc_clamp_info[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = TD_TRUE};
ot_isp_slave_sns_sync g_os05a10_slave_sync[OT_ISP_MAX_PIPE_NUM];
ot_isp_sns_commbus *os05a10_slave_get_bus_info(ot_vi_pipe vi_pipe)
{
    return &g_os05a10_slave_bus_info[vi_pipe];
}

ot_isp_sns_state *os05a10_slave_get_ctx(ot_vi_pipe vi_pipe)
{
    return g_os05a10_slave_sns_state[vi_pipe];
}

os05a10_slave_video_mode_tbl g_os05a10_slave_mode_tbl[OS05A10_SLAVE_MODE_BUTT] = {
    {
        24000000, 400, 799920, OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR, OS05A10_SLAVE_FULL_LINES_MAX, 30.003,
        5, 2688, 1520, 0, OT_WDR_MODE_NONE, "OS05A10_SLAVE_8M_30FPS_12BIT_LINEAR_MODE"
    },
    {
        24000000, 400, 799594, OS05A10_SLAVE_VMAX_8M_30FPS_10BIT_2TO1_WDR, OS05A10_SLAVE_FULL_LINES_MAX_2TO1_WDR,
        30, 15, 2592, 1944, 0, OT_WDR_MODE_2To1_LINE,  "OS05A10_SLAVE_8M_30FPS_10BIT_2TO1_VC_MODE"
    },
};

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
/* Os08a20 Register Address */
#define OS05A10_SLAVE_VMAX_H_ADDR          0x380e
#define OS05A10_SLAVE_VMAX_L_ADDR          0x380f

#define OS05A10_SLAVE_EXPO_H_ADDR          0x3501
#define OS05A10_SLAVE_EXPO_L_ADDR          0x3502

#define OS05A10_SLAVE_AGAIN_H_ADDR         0x3508
#define OS05A10_SLAVE_AGAIN_L_ADDR         0x3509
#define OS05A10_SLAVE_DGAIN_H_ADDR         0x350a
#define OS05A10_SLAVE_DGAIN_L_ADDR         0x350b

#define OS05A10_SLAVE_SHORT_EXPO_H_ADDR    0x3511
#define OS05A10_SLAVE_SHORT_EXPO_L_ADDR    0x3512

#define OS05A10_SLAVE_SHORT_AGAIN_H_ADDR   0x350c
#define OS05A10_SLAVE_SHORT_AGAIN_L_ADDR   0x350d
#define OS05A10_SLAVE_SHORT_DGAIN_H_ADDR   0x350e
#define OS05A10_SLAVE_SHORT_DGAIN_L_ADDR   0x350f

#define OS05A10_SLAVE_R3740_ADDR           0x3740
#define OS05A10_SLAVE_R3741_ADDR           0x3741


#define os05a10_slave_err_mode_print(sensor_image_mode, sns_state) \
do { \
    isp_err_trace("Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n", \
                  (sensor_image_mode)->width, \
                  (sensor_image_mode)->height, \
                  (sensor_image_mode)->fps, \
                  (sns_state)->wdr_mode); \
} while (0)

os05a10_slave_video_mode_tbl *os05a10_get_slave_mode_tbl(td_u8 img_mode)
{
    return &g_os05a10_slave_mode_tbl[img_mode];
}

ot_isp_slave_sns_sync *os05a10_get_slave_sync(ot_vi_pipe vi_pipe)
{
    return &g_os05a10_slave_sync[vi_pipe];
}

td_s32 os05a10_get_slave_bind_dev(ot_vi_pipe vi_pipe)
{
    return g_os05a10_slave_bind_dev[vi_pipe];
}

td_u32 os05a10_get_slave_sensor_mode_time(ot_vi_pipe vi_pipe)
{
    return g_os05a10_slave_sensor_mode_time[vi_pipe];
}

ot_isp_sns_state *os05a10_get_slave_sns_state(ot_vi_pipe vi_pipe)
{
    return g_os05a10_slave_sns_state[vi_pipe];
}


static td_void cmos_get_ae_comm_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
    const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->flicker_freq = 50 * 256; /* light flicker freq: 50Hz, accuracy: 256 */
    ae_sns_dft->full_lines_max = OS05A10_SLAVE_FULL_LINES_MAX;
    ae_sns_dft->hmax_times = (1000000000) / (sns_state->fl_std * 30); /* 1000000000ns, 30fps */

    ae_sns_dft->int_time_accu.accu_type = OT_ISP_AE_ACCURACY_LINEAR;
    ae_sns_dft->int_time_accu.accuracy = 1;
    ae_sns_dft->int_time_accu.offset = 0;

    ae_sns_dft->again_accu.accu_type = OT_ISP_AE_ACCURACY_TABLE;
    ae_sns_dft->again_accu.accuracy  = 0.0625; /* accuracy: 0.0625 */

    ae_sns_dft->dgain_accu.accu_type = OT_ISP_AE_ACCURACY_LINEAR;
    ae_sns_dft->dgain_accu.accuracy = 0.0009765625; /* accuracy: 0.0009765625 */

    ae_sns_dft->isp_dgain_shift = 8; /* accuracy: 8 */
    ae_sns_dft->min_isp_dgain_target = 1 << ae_sns_dft->isp_dgain_shift;
    ae_sns_dft->max_isp_dgain_target = 6 << ae_sns_dft->isp_dgain_shift; /* max 6 */
    if (lines_per500ms[vi_pipe] == 0) {
        ae_sns_dft->lines_per500ms = sns_state->fl_std * 30 / 2; /* 30fps, div 2 */
    } else {
        ae_sns_dft->lines_per500ms = lines_per500ms[vi_pipe];
    }

    ae_sns_dft->max_iris_fno = OT_ISP_IRIS_F_NO_1_0;
    ae_sns_dft->min_iris_fno = OT_ISP_IRIS_F_NO_32_0;

    ae_sns_dft->ae_route_ex_valid = TD_FALSE;
    ae_sns_dft->ae_route_attr.total_num = 0;
    ae_sns_dft->ae_route_attr_ex.total_num = 0;
    ae_sns_dft->quick_start.quick_start_enable = quick_start_en[vi_pipe];
    ae_sns_dft->quick_start.black_frame_num = 0;

    return;
}

static td_void cmos_get_ae_linear_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
    const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->max_again = 15872; /* max 15872 */
    ae_sns_dft->min_again = 1024;  /* min 1024 */
    ae_sns_dft->max_again_target = ae_sns_dft->max_again;
    ae_sns_dft->min_again_target = ae_sns_dft->min_again;

    ae_sns_dft->max_dgain = 16383; /* max 16383 */
    ae_sns_dft->min_dgain = 1024;  /* min 1024 */
    ae_sns_dft->max_dgain_target = ae_sns_dft->max_dgain;
    ae_sns_dft->min_dgain_target = ae_sns_dft->min_dgain;

    ae_sns_dft->ae_compensation = 0x40;
    ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_HIGHLIGHT_PRIOR;
    ae_sns_dft->init_exposure = init_exposure[vi_pipe] ? init_exposure[vi_pipe] : 76151; /* init 76151 */

    ae_sns_dft->max_int_time = sns_state->fl_std - 10; /* sub 10 */
    ae_sns_dft->min_int_time = 8; /* min int 8 */
    ae_sns_dft->max_int_time_target = 65535; /* max int 65535 */
    ae_sns_dft->min_int_time_target = ae_sns_dft->min_int_time;
    ae_sns_dft->ae_route_ex_valid = ae_route_ex_valid[vi_pipe];
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route),
                      &init_ae_route[vi_pipe],  sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr_ex, sizeof(ot_isp_ae_route_ex),
                      &init_ae_route_ex[vi_pipe], sizeof(ot_isp_ae_route_ex));
    return;
}

static td_void cmos_get_ae_2to1_line_wdr_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
    const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->max_int_time = sns_state->fl_std - 10; /* sub 10 */
    ae_sns_dft->min_int_time = 8; /* min_int_time 8 */

    ae_sns_dft->max_int_time_target = 65535; /* max 65535 */
    ae_sns_dft->min_int_time_target = ae_sns_dft->min_int_time;

    ae_sns_dft->max_again = 1984; /* max 1984 */
    ae_sns_dft->min_again = 128;  /* min 128 */
    ae_sns_dft->max_again_target = ae_sns_dft->max_again;
    ae_sns_dft->min_again_target = ae_sns_dft->min_again;

    ae_sns_dft->max_dgain = 2047; /* max 2047 */
    ae_sns_dft->min_dgain = 128;  /* min 128 */
    ae_sns_dft->max_dgain_target = ae_sns_dft->max_dgain;
    ae_sns_dft->min_dgain_target = ae_sns_dft->min_dgain;
    ae_sns_dft->max_isp_dgain_target = 16 << ae_sns_dft->isp_dgain_shift; /* max 16 << shift */

    ae_sns_dft->init_exposure = init_exposure[vi_pipe] ? init_exposure[vi_pipe] : 52000; /* init 52000 */

    if (fswdr_mode[vi_pipe] == OT_ISP_FSWDR_LONG_FRAME_MODE) {
        ae_sns_dft->ae_compensation = 56; /* ae_compensation 56 */
        ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_HIGHLIGHT_PRIOR;
    } else {
        ae_sns_dft->ae_compensation = 32; /* ae_compensation 32 */
        ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_LOWLIGHT_PRIOR;
        ae_sns_dft->man_ratio_enable = TD_TRUE;
        ae_sns_dft->arr_ratio[0] = 0x400;
        ae_sns_dft->arr_ratio[1] = 0x40;
        ae_sns_dft->arr_ratio[2] = 0x40; /* array index 2 */
    }
    ae_sns_dft->ae_route_ex_valid = ae_route_ex_valid[vi_pipe];
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route),
                      &init_ae_route[vi_pipe],  sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_attr_ex, sizeof(ot_isp_ae_route_ex),
                      &init_ae_route_ex[vi_pipe],  sizeof(ot_isp_ae_route_ex));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_sf_attr, sizeof(ot_isp_ae_route),
                      &init_ae_route_sf[vi_pipe], sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&ae_sns_dft->ae_route_sf_attr_ex, sizeof(ot_isp_ae_route_ex),
                      &init_ae_route_sf_ex[vi_pipe],  sizeof(ot_isp_ae_route_ex));
    return;
}

static td_s32 cmos_get_ae_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(ae_sns_dft);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    (td_void)memset_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route), 0, sizeof(ot_isp_ae_route));

    cmos_get_ae_comm_default(vi_pipe, ae_sns_dft, sns_state);

    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_NONE:   /* linear mode */
            cmos_get_ae_linear_default(vi_pipe, ae_sns_dft, sns_state);
            break;

        case OT_WDR_MODE_2To1_LINE:
            cmos_get_ae_2to1_line_wdr_default(vi_pipe, ae_sns_dft, sns_state);
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
        sns_state->regs_info[0].i2c_data[VMAX_L_IDX].data = low_8bits(vmax);
        sns_state->regs_info[0].i2c_data[VMAX_H_IDX].data = high_8bits(vmax);
    } else {
        sns_state->regs_info[0].i2c_data[VMAX_L_IDX].data = low_8bits(vmax);
        sns_state->regs_info[0].i2c_data[VMAX_H_IDX].data = high_8bits(vmax);
    }

    return;
}

/* the function of sensor set fps */
static td_void cmos_fps_set(ot_vi_pipe vi_pipe, td_float fps, ot_isp_ae_sensor_default *ae_sns_dft)
{
    td_u32 lines, lines_max, vmax, inck, new_inck;
    td_float max_fps, min_fps;
    ot_isp_sns_state *sns_state = TD_NULL;
    static td_u32 last_inck[OT_ISP_MAX_PIPE_NUM] = {0};

    sensor_check_pointer_void_return(ae_sns_dft);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    lines = g_os05a10_slave_mode_tbl[sns_state->img_mode].ver_lines;
    lines_max = g_os05a10_slave_mode_tbl[sns_state->img_mode].max_ver_lines;
    max_fps = g_os05a10_slave_mode_tbl[sns_state->img_mode].max_fps;
    min_fps = g_os05a10_slave_mode_tbl[sns_state->img_mode].min_fps;
    inck = g_os05a10_slave_mode_tbl[sns_state->img_mode].inck;

    if ((fps > max_fps) || (fps < min_fps)) {
        isp_err_trace("Not support Fps: %f\n", fps);
        return;
    }

    new_inck = inck / div_0_to_1_float(fps);
    if (last_inck[vi_pipe] == 0) {
        last_inck[vi_pipe] = new_inck;
    } else {
        if (new_inck > last_inck[vi_pipe] && (new_inck - last_inck[vi_pipe]) > INCK_ONCE_INCREASE_MAX) {
            new_inck = last_inck[vi_pipe] + INCK_ONCE_INCREASE_MAX;
            fps = (td_float)inck / (td_float)new_inck;
        }
        last_inck[vi_pipe] = new_inck;
    }
    g_os05a10_slave_sync[vi_pipe].vs_time = new_inck;

    vmax = lines * max_fps / div_0_to_1_float(fps);
    vmax = (vmax > lines_max) ? lines_max : vmax;

    cmos_config_vmax(sns_state, vmax);

    sns_state->fl_std = vmax;
    sns_state->regs_info[0].slv_sync.slave_vs_time = g_os05a10_slave_sync[vi_pipe].vs_time;
    ae_sns_dft->lines_per500ms = lines * 15; /* *15 */

    ae_sns_dft->fps = fps;
    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->max_int_time = sns_state->fl_std - 8; /* sub 8 */
    sns_state->fl[0] = sns_state->fl_std;
    ae_sns_dft->full_lines = sns_state->fl[0];

    return;
}

static td_void cmos_slow_framerate_set(ot_vi_pipe vi_pipe, td_u32 full_lines, ot_isp_ae_sensor_default *ae_sns_dft)
{
    td_u32 lines, lines_max, inck, new_inck;
    td_u32 vmax;
    td_float fps, fps_max;
    ot_isp_sns_state *sns_state = TD_NULL;
    static td_u32 last_inck[OT_ISP_MAX_PIPE_NUM] = {0};
    sensor_check_pointer_void_return(ae_sns_dft);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    lines = g_os05a10_slave_mode_tbl[sns_state->img_mode].ver_lines;
    lines_max = g_os05a10_slave_mode_tbl[sns_state->img_mode].max_ver_lines;
    inck = g_os05a10_slave_mode_tbl[sns_state->img_mode].inck;
    fps_max = g_os05a10_slave_mode_tbl[sns_state->img_mode].max_fps;

    vmax = full_lines;
    vmax = (vmax > lines_max) ? lines_max : vmax;
    fps = lines * fps_max / (td_float)full_lines;
    new_inck = inck / div_0_to_1_float(fps);
    if (last_inck[vi_pipe] == 0) {
        last_inck[vi_pipe] = g_os05a10_slave_sync[vi_pipe].vs_time;
    }
    if (new_inck > last_inck[vi_pipe] && (new_inck - last_inck[vi_pipe]) > INCK_ONCE_INCREASE_MAX) {
        new_inck = last_inck[vi_pipe] + INCK_ONCE_INCREASE_MAX;
        fps = (td_float)inck / (td_float)new_inck;
        vmax = lines * fps_max / div_0_to_1_float(fps);
    }
    last_inck[vi_pipe] = new_inck;
    sns_state->fl[0] = vmax;
    g_os05a10_slave_sync[vi_pipe].vs_time = new_inck;
    sns_state->regs_info[0].slv_sync.slave_vs_time = g_os05a10_slave_sync[vi_pipe].vs_time;

    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_NONE:
            sns_state->regs_info[0].i2c_data[VMAX_L_IDX].data = low_8bits(sns_state->fl[0]);
            sns_state->regs_info[0].i2c_data[VMAX_H_IDX].data = high_8bits(sns_state->fl[0]);
            break;
        case OT_WDR_MODE_2To1_LINE:
            sns_state->regs_info[0].i2c_data[VMAX_L_IDX].data = low_8bits(sns_state->fl[0]);
            sns_state->regs_info[0].i2c_data[VMAX_H_IDX].data = high_8bits(sns_state->fl[0]);
            break;
        default:
            break;
    }

    ae_sns_dft->full_lines = sns_state->fl[0];
    ae_sns_dft->max_int_time = sns_state->fl[0] - 10; /* max_int_time: Flstd - 10 */

    return;
}

static td_void cmos_inttime_update_linear(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    sns_state->regs_info[0].i2c_data[EXPO_L_IDX].data = low_8bits(int_time);
    sns_state->regs_info[0].i2c_data[EXPO_H_IDX].data = high_8bits(int_time);

    return;
}

static td_void cmos_inttime_update_2to1_line(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    static td_bool is_first[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 1};

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    if (is_first[vi_pipe]) { /* short exposure */
        sns_state->wdr_int_time[0] = int_time;
        sns_state->regs_info[0].i2c_data[EXP_SHORT_L_IDX].data = low_8bits(int_time);
        sns_state->regs_info[0].i2c_data[EXP_SHORT_H_IDX].data = high_8bits(int_time);
        is_first[vi_pipe] = TD_FALSE;
    } else { /* long exposure */
        sns_state->wdr_int_time[1] = int_time;
        sns_state->regs_info[0].i2c_data[EXPO_L_IDX].data = low_8bits(int_time);
        sns_state->regs_info[0].i2c_data[EXPO_H_IDX].data = high_8bits(int_time);
        is_first[vi_pipe] = TD_TRUE;
    }

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static td_void cmos_inttime_update(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE) {
        cmos_inttime_update_2to1_line(vi_pipe, int_time);
    } else {
        cmos_inttime_update_linear(vi_pipe, int_time);
    }

    return;
}


#define AGAIN_NODE_NUM 64
static td_u32 again_table[AGAIN_NODE_NUM] = {
    1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984,
    2048, 2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968,
    4096, 4352, 4608, 4864, 5120, 5376, 5632, 5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936,
    8192, 8704, 9216, 9728, 10240, 10752, 11264, 11776, 12288, 12800, 13312, 13824, 14336, 14848,
    15360, 15872
};

static td_void cmos_again_calc_table(ot_vi_pipe vi_pipe, td_u32 *again_linear, td_u32 *again_db)
{
    int i;

    sensor_check_pointer_void_return(again_linear);
    sensor_check_pointer_void_return(again_db);

    ot_unused(vi_pipe);

    if (*again_linear >= again_table[AGAIN_NODE_NUM - 1]) {
        *again_linear = again_table[AGAIN_NODE_NUM - 1];
        *again_db = AGAIN_NODE_NUM - 1;
        return;
    }

    for (i = 1; i < AGAIN_NODE_NUM; i++) {
        if (*again_linear < again_table[i]) {
            *again_linear = again_table[i - 1];
            *again_db = i - 1;
            break;
        }
    }
    return;
}

static td_void cmos_gains_update(ot_vi_pipe vi_pipe, td_u32 again, td_u32 dgain)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    td_u32 again_reg = again_table[again] >> 3; /* 3 */

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    sns_state->regs_info[0].i2c_data[AGAIN_L_IDX].data = low_8bits(again_reg);
    sns_state->regs_info[0].i2c_data[AGAIN_H_IDX].data = ((again_reg & 0x3F00) >> 8); /* 8 */
    sns_state->regs_info[0].i2c_data[DGAIN_L_IDX].data = low_8bits(dgain);
    sns_state->regs_info[0].i2c_data[DGAIN_H_IDX].data = ((dgain & 0x3F00) >> 8); /* 8 */

    if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE) {
        sns_state->regs_info[0].i2c_data[AGAIN_SHORT_L_IDX].data = sns_state->regs_info[0].i2c_data[AGAIN_L_IDX].data;
        sns_state->regs_info[0].i2c_data[AGAIN_SHORT_H_IDX].data = sns_state->regs_info[0].i2c_data[AGAIN_H_IDX].data;
        sns_state->regs_info[0].i2c_data[DGAIN_SHORT_L_IDX].data = sns_state->regs_info[0].i2c_data[DGAIN_L_IDX].data;
        sns_state->regs_info[0].i2c_data[DGAIN_SHORT_H_IDX].data = sns_state->regs_info[0].i2c_data[DGAIN_H_IDX].data;
    }

    return;
}

static td_void cmos_get_inttime_max_2to1_line(ot_vi_pipe vi_pipe, td_u32 *ratio,
    ot_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time)
{
    td_u32 short_max0;
    td_u32 short_max;
    td_u32 short_time_min_limit;
    ot_isp_sns_state *sns_state = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    ot_unused(lf_max_int_time);

    short_time_min_limit = 8; /* short_time_min_limit 8 */

    if (fswdr_mode[vi_pipe] == OT_ISP_FSWDR_LONG_FRAME_MODE) {
        short_max0 = sns_state->fl[1] - 50 - sns_state->wdr_int_time[0]; /* sensor limit: sub 50 */
        short_max = sns_state->fl[0] - 50; /* sensor limit: sub 50 */
        short_max = (short_max0 < short_max) ? short_max0 : short_max;
        int_time->int_time_max[0] = short_time_min_limit;
        int_time->int_time_min[0] = short_time_min_limit;
        int_time->int_time_max[1] = short_max;
        int_time->int_time_min[1] = short_time_min_limit;
        return;
    } else {
        short_max0 = ((sns_state->fl[1] - 50 - sns_state->wdr_int_time[0]) * 0x40) / div_0_to_1(ratio[0]); /* sub 50 */

        short_max = ((sns_state->fl[0] - 50) * 0x40) / (ratio[0] + 0x40); /* sub 50 */
        short_max = (short_max0 < short_max) ? short_max0 : short_max;
        short_max = (short_max == 0) ? 1 : short_max;
    }

    if (short_max >= short_time_min_limit) {
        int_time->int_time_max[0] = short_max;
        int_time->int_time_max[1] = (int_time->int_time_max[0] * ratio[0]) >> 6; /* shift 6 */
        int_time->int_time_min[0] = short_time_min_limit;
        int_time->int_time_min[1] = (int_time->int_time_min[0] * ratio[0]) >> 6; /* shift 6 */
    } else {
        short_max = short_time_min_limit;

        int_time->int_time_max[0] = short_max;
        int_time->int_time_max[1] = (int_time->int_time_max[0] * 0xFFF) >> 6; /* shift 6 */
        int_time->int_time_min[0] = int_time->int_time_max[0];
        int_time->int_time_min[1] = int_time->int_time_max[1];
    }

    return;
}

static td_void cmos_get_inttime_max(ot_vi_pipe vi_pipe, td_u16 man_ratio_enable, td_u32 *ratio,
    ot_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    sensor_check_pointer_void_return(ratio);
    sensor_check_pointer_void_return(int_time);
    sensor_check_pointer_void_return(lf_max_int_time);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    ot_unused(man_ratio_enable);

    switch (sns_state->wdr_mode) {
        case OT_WDR_MODE_2To1_LINE:
            cmos_get_inttime_max_2to1_line(vi_pipe, ratio, int_time, lf_max_int_time);
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

    fswdr_mode[vi_pipe] = ae_fswdr_attr->fswdr_mode;
    max_time_get_cnt[vi_pipe] = 0;

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
    exp_func->pfn_cmos_again_calc_table = cmos_again_calc_table;
    exp_func->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
    exp_func->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return TD_SUCCESS;
}

/* awb static param for Fuji Lens New IR_Cut */
#define CALIBRATE_STATIC_TEMP                         5000
#define CALIBRATE_STATIC_WB_R_GAIN                    492
#define CALIBRATE_STATIC_WB_GR_GAIN                   256
#define CALIBRATE_STATIC_WB_GB_GAIN                   256
#define CALIBRATE_STATIC_WB_B_GAIN                    443

/* Calibration results for Auto WB Planck */
#define CALIBRATE_AWB_P1                              16
#define CALIBRATE_AWB_P2                              240
#define CALIBRATE_AWB_Q1                              0
#define CALIBRATE_AWB_A1                              188580
#define CALIBRATE_AWB_B1                              128
#define CALIBRATE_AWB_C1                              (-139106)

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN                                  0
#define GOLDEN_BGAIN                                  0
static td_s32 cmos_get_awb_default(ot_vi_pipe vi_pipe, ot_isp_awb_sensor_default *awb_sns_dft)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(awb_sns_dft);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
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

        case OT_WDR_MODE_2To1_FRAME:
        case OT_WDR_MODE_2To1_LINE:
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

    awb_sns_dft->init_rgain = init_wb_gain[vi_pipe][0]; /* 0: Rgain */
    awb_sns_dft->init_ggain = init_wb_gain[vi_pipe][1]; /* 1: Ggain */
    awb_sns_dft->init_bgain = init_wb_gain[vi_pipe][2]; /* 2: Bgain */
    awb_sns_dft->sample_rgain = sample_r_gain[vi_pipe];
    awb_sns_dft->sample_bgain = sample_b_gain[vi_pipe];

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
        case OS05A10_SLAVE_8M_30FPS_12BIT_LINEAR_MODE:
            isp_def->sensor_mode.dng_raw_format.bits_per_sample = 12; /* 12bit */
            isp_def->sensor_mode.dng_raw_format.white_level = 4095; /* max 4095 */
            break;

        case OS05A10_SLAVE_8M_30FPS_10BIT_2TO1_VC_MODE:
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
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
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
        case OT_WDR_MODE_2To1_LINE:
            cmos_get_isp_wdr_default(isp_def);
            break;
        default:
            cmos_get_isp_linear_default(isp_def);
            break;
    }

    isp_def->wdr_switch_attr.exp_ratio[0] = 0x40;

    if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE) {
        isp_def->wdr_switch_attr.exp_ratio[0] = 0x400;
    }

    isp_def->sensor_mode.sensor_id = OS05A10_SLAVE_ID;
    isp_def->sensor_mode.sensor_mode = sns_state->img_mode;
    cmos_get_isp_dng_default(sns_state, isp_def);

    return TD_SUCCESS;
}

static td_s32 cmos_get_isp_black_level(ot_vi_pipe vi_pipe, ot_isp_cmos_black_level *black_level)
{
    td_s32  i;
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(black_level);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    (td_void)memcpy_s(black_level, sizeof(ot_isp_cmos_black_level), &g_cmos_blc, sizeof(ot_isp_cmos_black_level));

    /* Don't need to update black level when iso change */
    black_level->auto_attr.update = TD_FALSE;

    /* black level of linear mode */
    if (sns_state->wdr_mode == OT_WDR_MODE_NONE) {
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            black_level->auto_attr.black_level[0][i] = 0x400;
        }
    } else { /* black level of DOL mode */
        for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
            black_level->auto_attr.black_level[i][0] = 0x400;
            black_level->auto_attr.black_level[i][1] = 0x400;
            black_level->auto_attr.black_level[i][2] = 0x400; /* index 2 */
            black_level->auto_attr.black_level[i][3] = 0x400; /* index 3 */
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
    td_u32 full_lines_5fps;
    ot_isp_sns_state *sns_state = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    check_ret(ot_mpi_isp_get_sns_slave_attr(vi_pipe, &g_os05a10_slave_sync[vi_pipe]));

    if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE) {
        return;
    } else {
        if (sns_state->img_mode == OS05A10_SLAVE_8M_30FPS_12BIT_LINEAR_MODE) {
            g_os05a10_slave_sync[vi_pipe].vs_time = (g_os05a10_slave_mode_tbl[sns_state->img_mode].inck_per_vs) *
                g_os05a10_slave_mode_tbl[sns_state->img_mode].max_fps / 5; /* divide 5 */
            full_lines_5fps = (OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR * 30) / 5; /* 30fps, 5fps */
        } else {
            return;
        }
    }

    if (enable) { /* setup for ISP pixel calibration mode */
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_AGAIN_L_ADDR, 0x00);
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_AGAIN_H_ADDR, 0x00);

        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_DGAIN_L_ADDR, 0x00);
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_DGAIN_H_ADDR, 0x00);

        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_VMAX_L_ADDR, low_8bits(full_lines_5fps));
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_VMAX_H_ADDR, high_8bits(full_lines_5fps));

        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_EXPO_L_ADDR, low_8bits(full_lines_5fps - 10));
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_EXPO_H_ADDR, high_8bits(full_lines_5fps - 10));
    } else { /* setup for ISP 'normal mode' */
        sns_state->fl_std = (sns_state->fl_std > OS05A10_SLAVE_FULL_LINES_MAX) ? OS05A10_SLAVE_FULL_LINES_MAX :
            sns_state->fl_std;
        g_os05a10_slave_sync[vi_pipe].vs_time = g_os05a10_slave_mode_tbl[sns_state->img_mode].inck_per_vs;
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_VMAX_L_ADDR, low_8bits(sns_state->fl_std));
        os05a10_slave_write_register(vi_pipe, OS05A10_SLAVE_VMAX_H_ADDR, high_8bits(sns_state->fl_std));
        sns_state->sync_init = TD_FALSE;
    }

    check_ret(ot_mpi_isp_set_sns_slave_attr(vi_pipe, &g_os05a10_slave_sync[vi_pipe]));
    return;
}

static td_s32 cmos_set_wdr_mode(ot_vi_pipe vi_pipe, td_u8 mode)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    sns_state->sync_init = TD_FALSE;

    switch (mode & 0x3F) {
        case OT_WDR_MODE_NONE:
            sns_state->wdr_mode = OT_WDR_MODE_NONE;
            printf("linear mode\n");
            break;

        case OT_WDR_MODE_2To1_LINE:
            sns_state->wdr_mode = OT_WDR_MODE_2To1_LINE;
            printf("2to1 line WDR 4k mode(60fps->30fps)\n");
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
    sns_state->regs_info[0].com_bus.i2c_dev = g_os05a10_slave_bus_info[vi_pipe].i2c_dev;
    sns_state->regs_info[0].cfg2_valid_delay_max = 2; /* delay_max 2 */
    sns_state->regs_info[0].reg_num = REG_MAX_IDX;

    if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE) {
        sns_state->regs_info[0].reg_num = REG_MAX_IDX;
        sns_state->regs_info[0].cfg2_valid_delay_max = 2; /* delay_max 2 */
    }

    for (i = 0; i < sns_state->regs_info[0].reg_num; i++) {
        sns_state->regs_info[0].i2c_data[i].update = TD_TRUE;
        sns_state->regs_info[0].i2c_data[i].dev_addr = OS05A10_SLAVE_I2C_ADDR;
        sns_state->regs_info[0].i2c_data[i].addr_byte_num = OS05A10_SLAVE_ADDR_BYTE;
        sns_state->regs_info[0].i2c_data[i].data_byte_num = OS05A10_SLAVE_DATA_BYTE;
    }

    /* Linear Mode Regs */
    sns_state->regs_info[0].i2c_data[VMAX_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[VMAX_L_IDX].reg_addr = OS05A10_SLAVE_VMAX_L_ADDR;
    sns_state->regs_info[0].i2c_data[VMAX_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[VMAX_H_IDX].reg_addr = OS05A10_SLAVE_VMAX_H_ADDR;

    sns_state->regs_info[0].i2c_data[EXPO_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[EXPO_L_IDX].reg_addr = OS05A10_SLAVE_EXPO_L_ADDR;
    sns_state->regs_info[0].i2c_data[EXPO_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[EXPO_H_IDX].reg_addr = OS05A10_SLAVE_EXPO_H_ADDR;

    sns_state->regs_info[0].i2c_data[AGAIN_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[AGAIN_L_IDX].reg_addr = OS05A10_SLAVE_AGAIN_L_ADDR;
    sns_state->regs_info[0].i2c_data[AGAIN_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[AGAIN_H_IDX].reg_addr = OS05A10_SLAVE_AGAIN_H_ADDR;

    sns_state->regs_info[0].i2c_data[DGAIN_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[DGAIN_L_IDX].reg_addr = OS05A10_SLAVE_DGAIN_L_ADDR;
    sns_state->regs_info[0].i2c_data[DGAIN_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[DGAIN_H_IDX].reg_addr = OS05A10_SLAVE_DGAIN_H_ADDR;

    sns_state->regs_info[0].slv_sync.update = TD_TRUE;
    sns_state->regs_info[0].slv_sync.delay_frame_num = 2; /* delay_max 2 */
    sns_state->regs_info[0].slv_sync.slave_bind_dev = g_os05a10_slave_bind_dev[vi_pipe];

    return;
}

static td_void cmos_2to1_line_wdr_sns_reg_info_init(ot_vi_pipe vi_pipe, ot_isp_sns_state *sns_state)
{
    ot_unused(vi_pipe);
    sns_state->regs_info[0].i2c_data[EXP_SHORT_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[EXP_SHORT_L_IDX].reg_addr = OS05A10_SLAVE_SHORT_EXPO_L_ADDR;
    sns_state->regs_info[0].i2c_data[EXP_SHORT_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[EXP_SHORT_H_IDX].reg_addr = OS05A10_SLAVE_SHORT_EXPO_H_ADDR;

    sns_state->regs_info[0].i2c_data[AGAIN_SHORT_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[AGAIN_SHORT_L_IDX].reg_addr = OS05A10_SLAVE_SHORT_AGAIN_L_ADDR;
    sns_state->regs_info[0].i2c_data[AGAIN_SHORT_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[AGAIN_SHORT_H_IDX].reg_addr = OS05A10_SLAVE_SHORT_AGAIN_H_ADDR;

    sns_state->regs_info[0].i2c_data[DGAIN_SHORT_L_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[DGAIN_SHORT_L_IDX].reg_addr = OS05A10_SLAVE_SHORT_DGAIN_L_ADDR;
    sns_state->regs_info[0].i2c_data[DGAIN_SHORT_H_IDX].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[DGAIN_SHORT_H_IDX].reg_addr = OS05A10_SLAVE_SHORT_DGAIN_H_ADDR;

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

    if (sns_state->regs_info[0].slv_sync.slave_vs_time == sns_state->regs_info[1].slv_sync.slave_vs_time) {
        sns_state->regs_info[0].slv_sync.update = TD_FALSE;
    } else {
        sns_state->regs_info[0].slv_sync.update = TD_TRUE;
    }

    return;
}

static td_s32 cmos_get_sns_regs_info(ot_vi_pipe vi_pipe, ot_isp_sns_regs_info *sns_regs_info)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sensor_check_pointer_return(sns_regs_info);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    if ((sns_state->sync_init == TD_FALSE) || (sns_regs_info->config == TD_FALSE)) {
        cmos_comm_sns_reg_info_init(vi_pipe, sns_state);

        if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE) {
            /* DOL 2t1 Mode Regs */
            cmos_2to1_line_wdr_sns_reg_info_init(vi_pipe, sns_state);
        }
        sns_state->sync_init = TD_TRUE;
    } else {
        cmos_sns_reg_info_update(vi_pipe, sns_state);
    }

    if (sns_state->wdr_mode == OT_WDR_MODE_NONE) {
        if (sns_state->fl[0] > sns_state->fl[1]) {
            sns_state->regs_info[0].i2c_data[VMAX_L_IDX].delay_frame_num = 0;
            sns_state->regs_info[0].i2c_data[VMAX_H_IDX].delay_frame_num = 0;
            sns_state->regs_info[0].slv_sync.delay_frame_num = 1; /* delay_max 1 */
        } else if (sns_state->fl[0] < sns_state->fl[1]) {
            sns_state->regs_info[0].i2c_data[VMAX_L_IDX].delay_frame_num = 1;
            sns_state->regs_info[0].i2c_data[VMAX_H_IDX].delay_frame_num = 1;
            sns_state->regs_info[0].slv_sync.delay_frame_num = 2; /* delay_max 2 */
        } else {
        }
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
    ot_unused(vi_pipe);
    switch (sensor_image_mode) {
        case OS05A10_SLAVE_8M_30FPS_12BIT_LINEAR_MODE:
            sns_state->fl_std = OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR;
            break;
        case OS05A10_SLAVE_8M_30FPS_10BIT_2TO1_VC_MODE:
            sns_state->fl_std = OS05A10_SLAVE_VMAX_8M_30FPS_10BIT_2TO1_WDR;
            break;
        default:
            sns_state->fl_std = OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR;
            break;
    }

    return;
}

static td_s32 cmos_set_image_mode(ot_vi_pipe vi_pipe, const ot_isp_cmos_sensor_image_mode *sensor_image_mode)
{
    td_u32 i;
    td_u8 image_mode;
    ot_isp_sns_state *sns_state = TD_NULL;
    sensor_check_pointer_return(sensor_image_mode);
    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_return(sns_state);

    image_mode = sns_state->img_mode;
    for (i = 0; i < OS05A10_SLAVE_MODE_BUTT; i++) {
        if (sensor_image_mode->fps <= g_os05a10_slave_mode_tbl[i].max_fps &&
            sensor_image_mode->width <= g_os05a10_slave_mode_tbl[i].width &&
            sensor_image_mode->height <= g_os05a10_slave_mode_tbl[i].height &&
            sns_state->wdr_mode == g_os05a10_slave_mode_tbl[i].wdr_mode) {
            image_mode = (os05a10_slave_res_mode)i;
            break;
        }
    }

    if (i >= OS05A10_SLAVE_MODE_BUTT) {
        os05a10_slave_err_mode_print(sensor_image_mode, sns_state);
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

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state);
    sensor_check_pointer_void_return(sns_state);

    sns_state->init      = TD_FALSE;
    sns_state->sync_init = TD_FALSE;
    sns_state->img_mode  = OS05A10_SLAVE_8M_30FPS_12BIT_LINEAR_MODE;
    sns_state->wdr_mode  = OT_WDR_MODE_NONE;
    sns_state->fl_std    = OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR;
    sns_state->fl[0]     = OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR;
    sns_state->fl[1]     = OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR;

    (td_void)memset_s(&sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info), 0, sizeof(ot_isp_sns_regs_info));
    (td_void)memset_s(&sns_state->regs_info[1], sizeof(ot_isp_sns_regs_info), 0, sizeof(ot_isp_sns_regs_info));

    return;
}

static td_s32 cmos_init_sensor_exp_function(ot_isp_sensor_exp_func *sensor_exp_func)
{
    sensor_check_pointer_return(sensor_exp_func);

    (td_void)memset_s(sensor_exp_func, sizeof(ot_isp_sensor_exp_func), 0, sizeof(ot_isp_sensor_exp_func));

    sensor_exp_func->pfn_cmos_sensor_init         = os05a10_slave_init;
    sensor_exp_func->pfn_cmos_sensor_exit         = os05a10_slave_exit;
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

static td_s32 os05a10_slave_set_bus_info(ot_vi_pipe vi_pipe, ot_isp_sns_commbus sns_bus_info)
{
    g_os05a10_slave_bus_info[vi_pipe].i2c_dev = sns_bus_info.i2c_dev;

    return TD_SUCCESS;
}

static td_s32 sensor_ctx_init(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state_ctx = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state_ctx);

    if (sns_state_ctx == TD_NULL) {
        sns_state_ctx = (ot_isp_sns_state *)malloc(sizeof(ot_isp_sns_state));
        if (sns_state_ctx == TD_NULL) {
            isp_err_trace("Isp[%d] SnsCtx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void)memset_s(sns_state_ctx, sizeof(ot_isp_sns_state), 0, sizeof(ot_isp_sns_state));

    os05a10_slave_sensor_set_ctx(vi_pipe, sns_state_ctx);

    return TD_SUCCESS;
}

static td_void sensor_ctx_exit(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state_ctx = TD_NULL;

    os05a10_slave_sensor_get_ctx(vi_pipe, sns_state_ctx);
    sensor_free(sns_state_ctx);
    os05a10_slave_sensor_reset_ctx(vi_pipe);

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

    sns_attr_info.sensor_id = OS05A10_SLAVE_ID;
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

    ret = ot_mpi_isp_sensor_unreg_callback(vi_pipe, OS05A10_SLAVE_ID);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor unregister callback function failed!\n");
        return ret;
    }

    ret = ot_mpi_ae_sensor_unreg_callback(vi_pipe, ae_lib, OS05A10_SLAVE_ID);
    if (ret != TD_SUCCESS) {
        isp_err_trace("sensor unregister callback function to ae lib failed!\n");
        return ret;
    }

    ret = ot_mpi_awb_sensor_unreg_callback(vi_pipe, awb_lib, OS05A10_SLAVE_ID);
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

    init_exposure[vi_pipe]  = init_attr->exposure;
    lines_per500ms[vi_pipe] = init_attr->lines_per500ms;
    init_wb_gain[vi_pipe][0] = init_attr->wb_r_gain; /* 0: rgain */
    init_wb_gain[vi_pipe][1] = init_attr->wb_g_gain; /* 1: ggain */
    init_wb_gain[vi_pipe][2] = init_attr->wb_b_gain; /* 2: bgain */
    sample_r_gain[vi_pipe] = init_attr->sample_r_gain;
    sample_b_gain[vi_pipe] = init_attr->sample_b_gain;
    quick_start_en[vi_pipe] = init_attr->quick_start_en;

    ae_route_ex_valid[vi_pipe] = init_attr->ae_route_ex_valid;
    (td_void)memcpy_s(&init_ae_route[vi_pipe], sizeof(ot_isp_ae_route),
                      &init_attr->ae_route, sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&init_ae_route_ex[vi_pipe], sizeof(ot_isp_ae_route_ex),
                      &init_attr->ae_route_ex, sizeof(ot_isp_ae_route_ex));
    (td_void)memcpy_s(&init_ae_route_sf[vi_pipe], sizeof(ot_isp_ae_route),
                      &init_attr->ae_route_sf, sizeof(ot_isp_ae_route));
    (td_void)memcpy_s(&init_ae_route_sf_ex[vi_pipe], sizeof(ot_isp_ae_route_ex),
                      &init_attr->ae_route_sf_ex, sizeof(ot_isp_ae_route_ex));

    return TD_SUCCESS;
}

ot_isp_sns_obj g_sns_os05a10_2l_slave_obj = {
    .pfn_register_callback     = sensor_register_callback,
    .pfn_un_register_callback  = sensor_unregister_callback,
    .pfn_standby               = os05a10_slave_standby,
    .pfn_restart               = os05a10_slave_restart,
    .pfn_mirror_flip           = TD_NULL,
    .pfn_set_blc_clamp         = TD_NULL,
    .pfn_write_reg             = os05a10_slave_write_register,
    .pfn_read_reg              = os05a10_slave_read_register,
    .pfn_set_bus_info          = os05a10_slave_set_bus_info,
    .pfn_set_init              = sensor_set_init
};
