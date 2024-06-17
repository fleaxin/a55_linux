/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_SAMPLE_GYRO_DIS_H__
#define __OT_SAMPLE_GYRO_DIS_H__

#include "ot_type.h"
#include "ot_common_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_s32 sample_dis_ipc_gyro(ot_vo_intf_type vo_intf_type);

td_s32 sample_dis_dv_gyro(ot_vo_intf_type vo_intf_type);

td_s32 sample_dis_gyro_switch(ot_vo_intf_type vo_intf_type, ot_ldc_version ldc_version);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of __cplusplus */

#endif /* __OT_SAMPLE_GYRO_DIS_H__ */

