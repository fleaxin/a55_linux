/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_EXT_REG_ACCESS_H
#define ISP_EXT_REG_ACCESS_H

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "isp_main.h"
#include "ot_type.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

td_void isp_sharpen_comm_attr_write(ot_vi_pipe vi_pipe, const ot_isp_sharpen_attr *shp_attr);
td_void isp_sharpen_auto_attr_write(ot_vi_pipe vi_pipe, const ot_isp_sharpen_auto_attr *auto_attr);
td_void isp_sharpen_manual_attr_write(ot_vi_pipe vi_pipe, const ot_isp_sharpen_manual_attr *manual_attr);

td_void isp_sharpen_comm_attr_read(ot_vi_pipe vi_pipe, ot_isp_sharpen_attr *shp_attr);
td_void isp_sharpen_manual_attr_read(ot_vi_pipe vi_pipe, ot_isp_sharpen_manual_attr *manual_attr);
td_void isp_sharpen_auto_attr_read(ot_vi_pipe vi_pipe, ot_isp_sharpen_auto_attr *auto_attr);

td_void isp_nr_comm_attr_write(ot_vi_pipe vi_pipe, const ot_isp_nr_attr *nr_attr);
td_void isp_nr_snr_attr_write(ot_vi_pipe pipe, const ot_isp_nr_snr_attr *snr_cfg);
td_void isp_nr_tnr_attr_write(ot_vi_pipe pipe, const ot_isp_nr_tnr_attr *tnr_cfg);
td_void isp_nr_wdr_attr_write(ot_vi_pipe vi_pipe, const ot_isp_nr_wdr_attr *wdr_cfg);

td_void isp_nr_comm_attr_read(ot_vi_pipe vi_pipe, ot_isp_nr_attr *nr_attr);
td_void isp_nr_snr_attr_read(ot_vi_pipe pipe, ot_isp_nr_snr_attr *snr_cfg);
td_void isp_nr_tnr_attr_read(ot_vi_pipe pipe, ot_isp_nr_tnr_attr *tnr_cfg);
td_void isp_nr_wdr_attr_read(ot_vi_pipe vi_pipe, ot_isp_nr_wdr_attr *wdr_cfg);

td_void isp_drc_attr_write(ot_vi_pipe vi_pipe, const ot_isp_drc_attr *drc_attr);
td_void isp_drc_attr_read(ot_vi_pipe vi_pipe, ot_isp_drc_attr *drc_attr);

td_void isp_ldci_attr_write(ot_vi_pipe vi_pipe, const ot_isp_ldci_attr *ldci_attr);
td_void isp_ldci_attr_read(ot_vi_pipe vi_pipe, ot_isp_ldci_attr *ldci_attr);

td_void isp_black_level_manual_attr_write(ot_vi_pipe vi_pipe, const ot_isp_black_level_manual_attr *manual_attr);
td_void isp_black_level_dynamic_attr_write(ot_vi_pipe vi_pipe, const ot_isp_black_level_dynamic_attr *dynamic_attr);
td_void isp_user_black_level_write(ot_vi_pipe vi_pipe, const td_u16 (*user_black_level)[OT_ISP_BAYER_CHN_NUM]);
td_void isp_black_level_manual_attr_read(ot_vi_pipe vi_pipe, ot_isp_black_level_manual_attr *manual_attr);
td_void isp_black_level_dynamic_attr_read(ot_vi_pipe vi_pipe, ot_isp_black_level_dynamic_attr *dynamic_attr);
td_void isp_user_black_level_read(ot_vi_pipe vi_pipe, td_u16 (*user_black_level)[OT_ISP_BAYER_CHN_NUM]);
td_void black_level_actual_value_write(ot_vi_pipe vi_pipe, const isp_blc_actual_info *actual);
td_void isp_black_level_actual_value_read(ot_vi_pipe vi_pipe, td_u16 black_level_actual[][4]);
td_void sns_black_level_actual_value_read(ot_vi_pipe vi_pipe, td_u16 (*sns_black_level)[OT_ISP_BAYER_CHN_NUM]);

td_void isp_bayershp_auto_attr_write(ot_vi_pipe vi_pipe, const ot_isp_bayershp_auto_attr *auto_attr);
td_void isp_bayershp_manual_attr_write(ot_vi_pipe vi_pipe, const ot_isp_bayershp_manual_attr *manual_attr);

td_void isp_bayershp_manual_attr_read(ot_vi_pipe vi_pipe, ot_isp_bayershp_manual_attr *manual_attr);
td_void isp_bayershp_auto_attr_read(ot_vi_pipe vi_pipe, ot_isp_bayershp_auto_attr *auto_attr);

td_void isp_demosaic_attr_write(ot_vi_pipe vi_pipe, const ot_isp_demosaic_attr *dm_attr);
td_void isp_demosaic_attr_read(ot_vi_pipe vi_pipe, ot_isp_demosaic_attr *dm_attr);

td_void isp_acac_manual_attr_write(ot_vi_pipe vi_pipe, const isp_acac_manual_attr *manual_attr);
td_void isp_acac_auto_attr_write(ot_vi_pipe vi_pipe, const isp_acac_auto_attr *auto_attr);

td_void isp_acac_manual_attr_read(ot_vi_pipe vi_pipe, isp_acac_manual_attr *manual_attr);
td_void isp_acac_auto_attr_read(ot_vi_pipe vi_pipe, isp_acac_auto_attr *auto_attr);

td_void isp_anti_false_color_attr_write(ot_vi_pipe vi_pipe, const ot_isp_anti_false_color_attr *anti_false_color);
td_void isp_anti_false_color_attr_read(ot_vi_pipe vi_pipe, ot_isp_anti_false_color_attr *anti_false_color);

td_void isp_rgbir_attr_write(ot_vi_pipe vi_pipe, const ot_isp_rgbir_attr *rgbir_attr);
td_void isp_rgbir_attr_read(ot_vi_pipe vi_pipe, ot_isp_rgbir_attr *rgbir_attr);

td_void isp_agamma_attr_write(ot_vi_pipe vi_pipe, const ot_isp_agamma_attr *agamma_attr);

td_void isp_af_stats_config_write(ot_vi_pipe vi_pipe, const ot_isp_focus_stats_cfg *af_cfg);

td_void isp_fswdr_attr_write(ot_vi_pipe vi_pipe, const ot_isp_wdr_fs_attr *fswdr_attr);
td_void isp_fswdr_attr_read(ot_vi_pipe vi_pipe, ot_isp_wdr_fs_attr *fswdr_attr);

td_void isp_expander_attr_write(ot_vi_pipe vi_pipe, const ot_isp_expander_attr *expander_attr);
td_void isp_expander_attr_read(ot_vi_pipe vi_pipe, ot_isp_expander_attr *expander_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
