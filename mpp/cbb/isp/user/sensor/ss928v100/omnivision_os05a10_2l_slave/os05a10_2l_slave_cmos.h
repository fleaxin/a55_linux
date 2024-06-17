/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OS05A10_2L_SLAVE_CMOS_H
#define OS05A10_2L_SLAVE_CMOS_H

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

#ifndef min
#define min(a, b) (((a) > (b)) ?  (b) : (a))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ?  (a) : (b))
#endif


#define check_ret(express) \
    do { \
        td_s32 ret; \
        ret = express; \
        if (ret != TD_SUCCESS) { \
            printf("Failed at %s: LINE: %d with %#x!\n", __FUNCTION__, __LINE__, ret); \
        } \
    } while (0)

#define OS05A10_SLAVE_I2C_ADDR    0x6c
#define OS05A10_SLAVE_ADDR_BYTE   2
#define OS05A10_SLAVE_DATA_BYTE   1
#define os05a10_slave_sensor_get_ctx(dev, ctx)   ((ctx) = os05a10_slave_get_ctx(dev))

#define OS05A10_SLAVE_FULL_LINES_MAX 0x25F8
#define OS05A10_SLAVE_FULL_LINES_MAX_2TO1_WDR 0xFFFF
#define OS05A10_SLAVE_VS_TIME_MAX   0xFFFFFFFF

#define OS05A10_SLAVE_INCREASE_LINES 0  /* make real fps less than stand fps because NVR require */
#define OS05A10_SLAVE_VMAX_8M_30FPS_12BIT_LINEAR     (0x654 + OS05A10_SLAVE_INCREASE_LINES) /* before is 0x7CB */
#define OS05A10_SLAVE_VMAX_8M_30FPS_10BIT_2TO1_WDR   (0x7CD + OS05A10_SLAVE_INCREASE_LINES)   /* 10bit */

typedef enum {
    OS05A10_SLAVE_8M_30FPS_12BIT_LINEAR_MODE = 0,
    OS05A10_SLAVE_8M_30FPS_10BIT_2TO1_VC_MODE,
    OS05A10_SLAVE_MODE_BUTT
} os05a10_slave_res_mode;

typedef struct {
    td_u32      inck;
    td_float    inck_per_hs;
    td_float    inck_per_vs;
    td_u32      ver_lines;
    td_u32      max_ver_lines;
    td_float    max_fps;
    td_float    min_fps;
    td_u32      width;
    td_u32      height;
    td_u8       sns_mode;
    ot_wdr_mode wdr_mode;
    const char *mode_name;
} os05a10_slave_video_mode_tbl;

typedef enum {
    VMAX_L_IDX = 0,
    VMAX_H_IDX, /* 1 */
    EXPO_L_IDX, /* 2 */
    EXPO_H_IDX, /* 3 */
    AGAIN_L_IDX, /* 4 */
    AGAIN_H_IDX, /* 5 */
    DGAIN_L_IDX, /* 6 */
    DGAIN_H_IDX, /* 7 */
    EXP_SHORT_L_IDX, /* 8 */
    EXP_SHORT_H_IDX, /* 9 */
    AGAIN_SHORT_L_IDX, /* 10 */
    AGAIN_SHORT_H_IDX, /* 11 */
    DGAIN_SHORT_L_IDX, /* 12 */
    DGAIN_SHORT_H_IDX, /* 13 */
    REG_MAX_IDX
}os05a10_slave_linear_reg_index;


ot_isp_sns_state *os05a10_slave_get_ctx(ot_vi_pipe vi_pipe);
ot_isp_sns_commbus *os05a10_slave_get_bus_info(ot_vi_pipe vi_pipe);

td_void os05a10_slave_init(ot_vi_pipe vi_pipe);
td_void os05a10_slave_exit(ot_vi_pipe vi_pipe);
td_void os05a10_slave_standby(ot_vi_pipe vi_pipe);
td_void os05a10_slave_restart(ot_vi_pipe vi_pipe);
td_s32  os05a10_slave_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data);
td_s32  os05a10_slave_read_register(ot_vi_pipe vi_pipe, td_u32 addr);
os05a10_slave_video_mode_tbl *os05a10_get_slave_mode_tbl(td_u8 img_mode);
ot_isp_slave_sns_sync *os05a10_get_slave_sync(ot_vi_pipe vi_pipe);
td_s32 os05a10_get_slave_bind_dev(ot_vi_pipe vi_pipe);
td_u32 os05a10_get_slave_sensor_mode_time(ot_vi_pipe vi_pipe);
ot_isp_sns_state *os05a10_get_slave_sns_state(ot_vi_pipe vi_pipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* OS05A10_SLAVE_CMOS_H */
