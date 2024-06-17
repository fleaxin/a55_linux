/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef MPI_ISP_INNER_H
#define MPI_ISP_INNER_H

#include "ot_type.h"
#include "common_isp_inner.h"
#ifdef CONFIG_OT_SNAP_SUPPORT
#include "ot_common_snap.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    td_s32 (*isp_register_alg_init)(ot_vi_pipe vi_pipe, td_void *reg_cfg);
    td_s32 (*isp_register_alg_run)(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv);
    td_s32 (*isp_register_alg_ctrl)(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value);
    td_s32 (*isp_register_alg_exit)(ot_vi_pipe vi_pipe);
}isp_register_alg_func;

#ifdef CONFIG_OT_SNAP_SUPPORT
td_s32 mpi_isp_set_snap_attr(ot_vi_pipe vi_pipe, const ot_snap_attr *snap_attr, const isp_snap_pipe *snap_pipe);
#endif

#ifdef CONFIG_OT_ISP_HNR_SUPPORT
td_s32 mpi_isp_set_hnr_enable(ot_vi_pipe vi_pipe, td_bool hnr_en);
#endif

td_s32 mpi_isp_register_alg(ot_vi_pipe vi_pipe, td_u32 alg_id, const isp_register_alg_func *funcs);

td_s32 mpi_isp_get_master_pipe(ot_vi_pipe vi_pipe, ot_vi_pipe *master_pipe);
td_u32 mpi_isp_get_pipe_index(ot_vi_pipe vi_pipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* MPI_ISP_INNER_H */
