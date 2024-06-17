/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OS04A10_CMOS_H
#define OS04A10_CMOS_H

#include "ot_common_isp.h"
#include "ot_sns_ctrl.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef clip3
#define clip3(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#endif


#define OS04A10_I2C_ADDR    0x6c
#define OS04A10_ADDR_BYTE   2
#define OS04A10_DATA_BYTE   1
#define os04a10_sensor_get_ctx(dev, ctx)   ((ctx) = os04a10_get_ctx(dev))

#define OS04A10_FULL_LINES_MAX 0x2610
#define OS04A10_INCREASE_LINES 0  /* make real fps less than stand fps because NVR require */
#define OS04A10_VMAX_4M_30FPS_12BIT_LINEAR     (1624 + OS04A10_INCREASE_LINES)

typedef enum {
    OS04A10_4M_30FPS_12BIT_LINEAR_MODE = 0,
    OS04A10_MODE_BUTT
} os04a10_res_mode;

typedef struct {
    td_u32      ver_lines;
    td_u32      max_ver_lines;
    td_float    max_fps;
    td_float    min_fps;
    td_u32      width;
    td_u32      height;
    td_u8       sns_mode;
    ot_wdr_mode wdr_mode;
    const char *mode_name;
} os04a10_video_mode_tbl;

typedef enum {
    EXPO_L_IDX = 0,
    EXPO_H_IDX,
    AGAIN_L_IDX,
    AGAIN_H_IDX,
    DGAIN_L_IDX,
    DGAIN_M_IDX,
    DGAIN_H_IDX,
    VMAX_L_IDX,
    VMAX_H_IDX,
    GROUP1,
    GROUP2,
    HCG_1_IDX,
    GROUP3,
    GROUP4,
    GROUP5,
    GROUP6,
    REG_MAX_IDX
}os04a10_linear_reg_index;

ot_isp_sns_state *os04a10_get_ctx(ot_vi_pipe vi_pipe);
ot_isp_sns_commbus *os04a10_get_bus_info(ot_vi_pipe vi_pipe);

td_void os04a10_init(ot_vi_pipe vi_pipe);
td_void os04a10_exit(ot_vi_pipe vi_pipe);
td_void os04a10_standby(ot_vi_pipe vi_pipe);
td_void os04a10_restart(ot_vi_pipe vi_pipe);
td_s32  os04a10_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data);
td_s32  os04a10_read_register(ot_vi_pipe vi_pipe, td_u32 addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* OS04A10_CMOS_H */
