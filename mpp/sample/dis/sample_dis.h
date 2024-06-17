/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_SAMPLE_DIS_H__
#define __OT_SAMPLE_DIS_H__

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

td_void sample_dis_pause(td_void);
td_s32 sample_dis_start_sample(sample_vi_cfg *vi_cfg, sample_vo_cfg *vo_cfg, ot_size *img_size);
td_s32 sample_dis_stop_sample(sample_vi_cfg *vi_cfg, sample_vo_cfg *vo_cfg);
td_void sample_dis_stop_sample_without_sys_exit(sample_vi_cfg *vi_cfg, sample_vo_cfg *vo_cfg);

#ifdef __cplusplus
}
#endif /* End of __cplusplus */

#endif /* __OT_SAMPLE_DIS_H__ */