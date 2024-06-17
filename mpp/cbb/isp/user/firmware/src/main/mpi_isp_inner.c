/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "mpi_isp_inner.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>

#include "ot_mpi_sys.h"
#include "common_isp_inner.h"
#include "isp_ext_config.h"
#include "isp_debug.h"
#include "isp_main.h"
#include "isp_math_utils.h"
#include "isp_vreg.h"
#include "ot_math.h"
#include "isp_alg.h"

#ifdef CONFIG_OT_SNAP_SUPPORT
static td_s32 isp_snap_attr_check(ot_vi_pipe vi_pipe, const ot_snap_attr *snap_attr,
    const isp_snap_pipe *snap_pipe)
{
    if (snap_attr->snap_type >= OT_SNAP_TYPE_BUTT) {
        isp_err_trace("Invalid op mode %d!\n", snap_attr->snap_type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snap_pipe->video_pipe == -1) {
        isp_err_trace("Invalid video pipe %d!\n", snap_pipe->video_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (snap_pipe->pic_pipe == -1) {
        isp_err_trace("Invalid pic pipe %d!\n", snap_pipe->pic_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 isp_set_snap_attr(ot_vi_pipe vi_pipe, const ot_snap_attr *snap_attr,
    const isp_snap_pipe *snap_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx_s = TD_NULL;
    isp_snap_attr mkp_snap_attr;

    isp_get_ctx(vi_pipe, isp_ctx_s);
    isp_check_mem_init_return(vi_pipe);
    isp_ctx_s->linkage.snap_type = snap_attr->snap_type;
    isp_ctx_s->linkage.preview_pipe_id = snap_pipe->video_pipe;
    isp_ctx_s->linkage.picture_pipe_id = snap_pipe->pic_pipe;
    isp_ctx_s->linkage.load_ccm = snap_attr->load_ccm_en;
    if (vi_pipe == snap_pipe->video_pipe) {
        isp_ctx_s->linkage.snap_pipe_mode = ISP_SNAP_PREVIEW;
    } else {
        isp_ctx_s->linkage.snap_pipe_mode = ISP_SNAP_PICTURE;
    }

    if (isp_ctx_s->linkage.snap_type == OT_SNAP_TYPE_PRO) {
        isp_ctx_s->pro_param.operation_mode = snap_attr->pro_attr.pro_param.op_mode;
        if (isp_ctx_s->pro_param.operation_mode == OT_OP_MODE_AUTO) {
            (td_void)memcpy_s(&(isp_ctx_s->pro_param.auto_param), sizeof(ot_snap_pro_auto_param),
                &snap_attr->pro_attr.pro_param.auto_param, sizeof(ot_snap_pro_auto_param));
        } else if (isp_ctx_s->pro_param.operation_mode == OT_OP_MODE_MANUAL) {
            (td_void)memcpy_s(&(isp_ctx_s->pro_param.manual_param), sizeof(ot_snap_pro_manual_param),
                &snap_attr->pro_attr.pro_param.manual_param, sizeof(ot_snap_pro_manual_param));
        }
    }

    (td_void)memset_s(&mkp_snap_attr, sizeof(isp_snap_attr), 0, sizeof(isp_snap_attr));
    mkp_snap_attr.load_ccm = snap_attr->load_ccm_en;
    mkp_snap_attr.snap_type = snap_attr->snap_type;
    mkp_snap_attr.preview_pipe_id = snap_pipe->video_pipe;
    mkp_snap_attr.picture_pipe_id = snap_pipe->pic_pipe;
    mkp_snap_attr.pro_param.pro_frame_num = snap_attr->pro_attr.frame_cnt;
    mkp_snap_attr.pro_param.operation_mode = snap_attr->pro_attr.pro_param.op_mode;
    if (isp_ctx_s->linkage.snap_type == OT_SNAP_TYPE_PRO) {
        if (isp_ctx_s->pro_param.operation_mode == OT_OP_MODE_AUTO) {
            (td_void)memcpy_s(&(mkp_snap_attr.pro_param.auto_param), sizeof(ot_snap_pro_auto_param),
                &snap_attr->pro_attr.pro_param.auto_param, sizeof(ot_snap_pro_auto_param));
        } else if (isp_ctx_s->pro_param.operation_mode == OT_OP_MODE_MANUAL) {
            (td_void)memcpy_s(&(mkp_snap_attr.pro_param.manual_param), sizeof(ot_snap_pro_manual_param),
                &snap_attr->pro_attr.pro_param.manual_param, sizeof(ot_snap_pro_manual_param));
        }
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_SNAP_ATTR_SET, &mkp_snap_attr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] set snap attr to kernel failed ec %#x!\n", vi_pipe, ret);
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 mpi_isp_set_snap_attr(ot_vi_pipe vi_pipe, const ot_snap_attr *snap_attr, const isp_snap_pipe *snap_pipe)
{
    td_s32 ret;
    td_s32 video_pipe;
    td_s32 pic_pipe;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(snap_attr);
    isp_check_pointer_return(snap_pipe);
    isp_check_open_return(vi_pipe);
    isp_check_mem_init_return(vi_pipe);

    ret = isp_snap_attr_check(vi_pipe, snap_attr, snap_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    video_pipe = snap_pipe->video_pipe;
    pic_pipe = snap_pipe->pic_pipe;

    ret = isp_set_snap_attr(video_pipe, snap_attr, snap_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Set pipe snap attr %d fail!\n", video_pipe);
        return ret;
    }

    ret = isp_set_snap_attr(pic_pipe, snap_attr, snap_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Set pipe snap attr %d fail!\n", pic_pipe);
        return ret;
    }

    return TD_SUCCESS;
}
#endif

#ifdef CONFIG_OT_ISP_HNR_SUPPORT
td_s32 mpi_isp_set_hnr_enable(ot_vi_pipe vi_pipe, td_bool hnr_en)
{
    isp_check_pipe_return(vi_pipe);
    isp_check_bool_return(hnr_en);
    isp_check_open_return(vi_pipe);

    return ioctl(isp_get_fd(vi_pipe), ISP_HNR_EN_SET, &hnr_en);
}
#endif

/* find or search the empty pos of algs */
static isp_alg_node *isp_find_register_alg(isp_alg_node *algs, td_u32 alg_id)
{
    td_s32 i;

    for (i = 0; i < ISP_MAX_ALGS_NUM; i++) {
        if (algs[i].alg_type == alg_id) {
            return &algs[i];
        }
    }

    for (i = 0; i < ISP_MAX_ALGS_NUM; i++) {
        if (!algs[i].used) {
            return &algs[i];
        }
    }

    return TD_NULL;
}

td_s32 mpi_isp_register_alg(ot_vi_pipe vi_pipe, td_u32 alg_id, const isp_register_alg_func *funcs)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_node *algs = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    isp_check_pointer_return(funcs);
    algs = isp_find_register_alg(isp_ctx->algs, alg_id);
    isp_check_pointer_return(algs);
    algs->alg_type = alg_id;
    algs->alg_func.pfn_alg_init = funcs->isp_register_alg_init;
    algs->alg_func.pfn_alg_run  = funcs->isp_register_alg_run;
    algs->alg_func.pfn_alg_ctrl = funcs->isp_register_alg_ctrl;
    algs->alg_func.pfn_alg_exit = funcs->isp_register_alg_exit;
    algs->used = TD_TRUE;
    return TD_SUCCESS;
}

td_s32 mpi_isp_get_master_pipe(ot_vi_pipe vi_pipe, ot_vi_pipe *master_pipe)
{
    td_s32 ret;
    vi_pipe_wdr_attr wdr_attr = { 0 };
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(master_pipe);
    isp_check_open_return(vi_pipe);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_GET_WDR_ATTR, &wdr_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    *master_pipe = wdr_attr.pipe_id[0];

    return TD_SUCCESS;
}

td_u32 mpi_isp_get_pipe_index(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret;
    vi_pipe_wdr_attr wdr_attr = { 0 };
    isp_check_pipe_return(vi_pipe);
    isp_check_open_return(vi_pipe);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_GET_WDR_ATTR, &wdr_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    for (i = 0; i < wdr_attr.pipe_num; i++) {
        if (wdr_attr.pipe_id[i] == vi_pipe) {
            return i;
        }
    }

    return 0;
}

