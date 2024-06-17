/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef __SAMPLE_MCF_CALIBRATION_H__
#define __SAMPLE_MCF_CALIBRATION_H__

#include "sample_comm_mcf.h"
#include "ss_mpi_mcf_calibration.h"

td_s32 sample_mcf_calibration(td_void);
td_s32 sample_mcf_diff_img_size_calibration(td_void);
td_s32 sample_mcf_calibrate_online(ot_mcf_grp mcf_grp, ot_mcf_grp_attr *mcf_grp_attr, ot_crop_info *grp_crop);

#endif
