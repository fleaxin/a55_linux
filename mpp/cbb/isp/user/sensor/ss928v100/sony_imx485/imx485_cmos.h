/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef IMX485_CMOS_H
#define IMX485_CMOS_H

#include "ot_common.h"
#include "ot_common_isp.h"
#include "ot_common_video.h"
#include "ot_sns_ctrl.h"
#include "ot_type.h"
#include "ot_math.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define IMX485_I2C_ADDR    0x34
#define IMX485_ADDR_BYTE   2
#define IMX485_DATA_BYTE   1
#define imx485_sensor_get_ctx(dev, ctx)   ((ctx) = imx485_get_ctx(dev))

#define IMX485_FULL_LINES_MAX 0xFFFFF
#define IMX485_FULL_LINES_MAX_3TO1_WDR 4086  /* considering the YOUT_SIZE and bad frame */

#define IMX485_INCREASE_LINES 1 /* make real fps less than stand fps because NVR require */
#define IMX485_VMAX_2160P30_LINEAR                      (2250)
#define IMX485_VMAX_2160P90TO30_WDR                     (2250)   /* 10bit */

typedef enum {
    IMX485_SENSOR_2160P_30FPS_LINEAR_MODE = 0,
    IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE,
    IMX485_MODE_BUTT
} imx485_res_mode;

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
} imx485_video_mode_tbl;

typedef struct {
    td_u32 dec[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32 inc[OT_ISP_WDR_MAX_FRAME_NUM];
} time_step;

ot_isp_sns_state *imx485_get_ctx(ot_vi_pipe vi_pipe);

ot_isp_sns_commbus *imx485_get_bus_info(ot_vi_pipe vi_pipe);

td_void imx485_init(ot_vi_pipe vi_pipe);
td_void imx485_exit(ot_vi_pipe vi_pipe);
td_void imx485_standby(ot_vi_pipe vi_pipe);
td_void imx485_restart(ot_vi_pipe vi_pipe);
td_void imx485_set_blc_clamp_value(ot_vi_pipe vi_pipe, td_bool clamp_en);
td_void imx485_blc_clamp(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp blc_clamp);

td_s32  imx485_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data);
td_s32  imx485_read_register(ot_vi_pipe vi_pipe, td_u32 addr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* IMX485_CMOS_H */
