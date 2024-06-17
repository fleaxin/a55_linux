/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_DSP_ENCA_H
#define SAMPLE_DSP_ENCA_H

#include "ot_common_dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ot_svp_src_img   src;
    ot_svp_dst_img   dst;
    ot_svp_mem_info  assist_buf;
    ot_svp_dsp_pri   pri;
    ot_svp_dsp_id    dsp_id;
}sample_svp_dsp_enca_dilate_arg;

typedef sample_svp_dsp_enca_dilate_arg sample_svp_dsp_enca_erode_arg;

/*
 * Encapsulate Dilate 3x3
 */
td_s32 sample_svp_dsp_enca_dilate_3x3(ot_svp_dsp_handle *handle, sample_svp_dsp_enca_dilate_arg *arg);

/*
 * Encapsulate Erode 3x3
 */
td_s32 sample_svp_dsp_enca_erode_3x3(ot_svp_dsp_handle *handle, sample_svp_dsp_enca_erode_arg *arg);

#ifdef __cplusplus
}
#endif

#endif
