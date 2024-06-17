/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef COMMON_ISP_INNER_H
#define COMMON_ISP_INNER_H

#include "ot_type.h"
#include "ot_errno.h"
#include "ot_common.h"
#include "ot_isp_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    ot_vi_pipe video_pipe;
    ot_vi_pipe pic_pipe;
} isp_snap_pipe;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* COMMON_ISP_INNER_H */
