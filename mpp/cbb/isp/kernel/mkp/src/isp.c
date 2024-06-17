/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "isp.h"
#include "dev_ext.h"
#include "ot_common.h"
#include "ot_common_isp.h"
#include "ot_i2c.h"
#include "ot_spi.h"
#include "ot_osal.h"
#include "isp_drv.h"
#include "isp_drv_define.h"
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
#include "isp_drv_proc.h"
#endif
#include "isp_drv_vreg.h"
#include "isp_ext.h"
#include "isp_list.h"
#include "mkp_isp.h"
#include "mm_ext.h"
#include "mod_ext.h"
#include "proc_ext.h"
#include "securec.h"
#include "sys_ext.h"

/* MACRO DEFINITION */
int isp_irq_route(ot_vi_pipe vi_pipe, td_u32 int_num);
int isp_drv_int_bottom_half(int irq);

/*  GLOBAL VARIABLES */
static isp_drv_ctx           g_isp_drv_ctx[OT_ISP_MAX_PIPE_NUM] = {{0}};

static osal_spinlock_t g_isp_lock[OT_ISP_MAX_PIPE_NUM];
static osal_spinlock_t g_isp_sync_lock[OT_ISP_MAX_PIPE_NUM];
/* ldci temporal filter enable */
static td_bool               g_ldci_tpr_flt_en[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = TD_FALSE };

static isp_version g_isp_lib_info = { { 0 } };

/* ISP ModParam info */
td_u32 g_pwm_number[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 3 }; /* num 3 */
td_u32 g_update_pos[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 0 }; /* 0: frame start; 1: frame end */

/* The time(unit:ms) of interrupt timeout */
td_u32  g_int_timeout[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 200 }; /* inter timeout 200 */

/* update isp statistic information per stat-intval frame, purpose to reduce CPU load */
td_u32  g_stat_intvl[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 1 };

/* 0: close proc; n: write proc info's interval int num */
td_u32  g_proc_param[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 30 }; /* interval num 30 */
td_u32  g_port_int_delay[OT_ISP_MAX_PIPE_NUM] = { 0 };                                 /* Port intertupt delay value */
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
td_u8   g_be_buf_num[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 8 }; /* buf num 8 */
#else
td_u8   g_be_buf_num[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 4 }; /* buf num 4 */
#endif
/* default ob statistics update position is fe_frame_delay */
ot_isp_ob_stats_update_pos g_ob_stats_update_pos[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 0 };
ot_isp_alg_run_select g_isp_alg_run_select[OT_ISP_MAX_PIPE_NUM] =  { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = 0 };

td_bool g_int_bottom_half = TD_FALSE; /* 1 to enable interrupt processing at bottom half */
td_bool g_quick_start = TD_FALSE;     /* 1 to enable interrupt processing at bottom half */
td_bool g_long_frm_int_en = TD_FALSE; /* 1 : enable long frame  pipe interrupt */
td_bool g_use_bottom_half = TD_FALSE; /* 1 to use interrupt processing at bottom half */
td_u32 g_isp_exit_timeout = 2000;     /* The time(unit:ms) of exit be buffer timeout */
td_u16  g_res_max_width[OT_ISP_MAX_PIPE_NUM] = { OT_ISP_RES_WIDTH_MAX };
td_u16  g_res_max_height[OT_ISP_MAX_PIPE_NUM] = { OT_ISP_RES_HEIGHT_MAX };
static td_bool g_mask_is_open[OT_ISP_MAX_PIPE_NUM] = { [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = TD_FALSE };
#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
isp_work_queue_ctx g_isp_work_queue_ctx;
#endif
#define ISP_ALG_MOD_NAME_LENGTH  20
static td_char g_isp_alg_mod_name[OT_ISP_ALG_MOD_BUTT][ISP_ALG_MOD_NAME_LENGTH] = {
    "ae", "af", "awb", "blc", "dpc", "pregamma", "agamma", "adgamma", "drc", "demosaic", "antifalsecolor",
    "gamma", "crosstalk", "sharpen", "fswdr", "fpn", "dehaze", "local cac", "acac", "bayershp", "csc",
    "expander", "mcds", "auto color shading", "mesh shading", "rc", "rgbir", "hrs", "dgain", "bayer_nr", "flicker",
    "ldci", "ca", "clut", "ccm", "pq_ai", "crb", "hnr"
};

isp_drv_ctx *isp_drv_get_ctx(ot_vi_pipe vi_pipe)
{
    return &g_isp_drv_ctx[vi_pipe];
}

osal_spinlock_t *isp_drv_get_lock(ot_vi_pipe vi_pipe)
{
    return &g_isp_lock[vi_pipe];
}

osal_spinlock_t *isp_drv_get_sync_lock(ot_vi_pipe vi_pipe)
{
    return &g_isp_sync_lock[vi_pipe];
}

td_bool isp_drv_get_ldci_tpr_flt_en(ot_vi_pipe vi_pipe)
{
    return g_ldci_tpr_flt_en[vi_pipe];
}

td_u32 isp_drv_get_update_pos(ot_vi_pipe vi_pipe)
{
    return g_update_pos[vi_pipe];
}

ot_isp_ob_stats_update_pos isp_drv_get_ob_stats_update_pos(ot_vi_pipe vi_pipe)
{
    return g_ob_stats_update_pos[vi_pipe];
}

ot_isp_alg_run_select isp_drv_get_alg_run_select(ot_vi_pipe vi_pipe)
{
    return g_isp_alg_run_select[vi_pipe];
}

td_void isp_drv_slave_pipe_int_enable(ot_vi_pipe vi_pipe, td_bool en)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u8 k;
    td_u8 chn_num_max;
    ot_vi_pipe vi_pipe_bind;
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (g_long_frm_int_en == TD_FALSE) {
        return;
    }
    if (is_fs_wdr_mode(drv_ctx->sync_cfg.wdr_mode) == TD_FALSE) {
        return;
    }

    chn_num_max = clip3(drv_ctx->wdr_attr.pipe_num, 1, ISP_WDR_CHN_MAX);
    if (en) {
        for (k = 0; k < chn_num_max; k++) {
            vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];
            if (vi_pipe_bind == vi_pipe) {
                continue;
            }
            if (((vi_pipe_bind) < 0) || ((vi_pipe_bind) >= OT_ISP_MAX_PHY_PIPE_NUM)) {
                continue;
            }
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE) = 0xff;
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK) |= ISP_INT_FE_FSTART;
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK) |= ISP_INT_FE_FEND;
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK) |= ISP_INT_FE_DYNABLC_END;
        }
    } else {
        for (k = 0; k < chn_num_max; k++) {
            vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];
            if (vi_pipe_bind == vi_pipe) {
                continue;
            }
            if (((vi_pipe_bind) < 0) || ((vi_pipe_bind) >= OT_ISP_MAX_PHY_PIPE_NUM)) {
                continue;
            }
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE) = 0xff;
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK) &= ~ISP_INT_FE_FSTART;
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK) &= ~ISP_INT_FE_FEND;
            io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK) &= ~ISP_INT_FE_DYNABLC_END;
        }
    }
}

td_void isp_drv_set_frame_start_int_mask(ot_vi_pipe vi_pipe, td_bool en)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (en) {
        if (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE) {
            isp_drv_set_pre_proc_fstart_int_mask(vi_pipe);
        } else {
            io_rw_fe_address(vi_pipe, ISP_INT_FE) = 0xFF;  /* clear interrupt */
        }
    } else {
        if (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE) {
            isp_drv_clear_pre_proc_fstart_int_mask(vi_pipe);
        } else {
            io_rw_fe_address(vi_pipe, ISP_INT_FE) = 0xFF; /* clear interrupt */
        }
    }
}

td_s32 isp_drv_set_int_enable(ot_vi_pipe vi_pipe, td_bool en)
{
    td_s32 vi_dev;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_bool_return(en);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    vi_dev = drv_ctx->wdr_attr.vi_dev;

    isp_check_vir_pipe_return(vi_pipe);

    if (en) {
        g_mask_is_open[vi_pipe] = TD_TRUE;
        if (is_full_wdr_mode(drv_ctx->wdr_cfg.wdr_mode) || is_half_wdr_mode(drv_ctx->wdr_cfg.wdr_mode)) {
            io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK) |= VI_PT_INT_FSTART;
            io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_FSTART_DLY) = g_port_int_delay[vi_pipe];
        }
        io_rw_pt_address(VICAP_HD_MASK) |= vicap_int_mask_pt((td_u32)vi_pipe);
        io_rw_pt_address(VICAP_HD_MASK) |= vicap_int_mask_isp((td_u32)vi_pipe);

        io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK) |= VI_PT_INT_ERR;
        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) |= ISP_INT_FE_FEND;
        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) |= ISP_INT_FE_DYNABLC_END;
        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) |= ISP_INT_FE_DELAY;
    } else {
        if (is_full_wdr_mode(drv_ctx->wdr_cfg.wdr_mode) || is_half_wdr_mode(drv_ctx->wdr_cfg.wdr_mode)) {
            io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK) &= ~(VI_PT_INT_FSTART);
            io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_FSTART_DLY) = 0;
        }
        io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT) = 0xF;
        io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK) &= ~(VI_PT_INT_ERR);
        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) &= ~(ISP_INT_FE_FEND);
        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) &= ~(ISP_INT_FE_DYNABLC_END);
        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) &= ~(ISP_INT_FE_DELAY);

        io_rw_pt_address(VICAP_HD_MASK) &= ~vicap_int_mask_pt((td_u32)vi_pipe);
        io_rw_pt_address(VICAP_HD_MASK) &= ~vicap_int_mask_isp((td_u32)vi_pipe);

        g_mask_is_open[vi_pipe] = TD_FALSE;
    }
    isp_drv_set_frame_start_int_mask(vi_pipe, en);
    isp_drv_slave_pipe_int_enable(vi_pipe, en);
    return TD_SUCCESS;
}

td_s32 isp_drv_wait_condition_callback(const td_void *param)
{
    td_bool condition;

    condition = *(td_bool *)param;

    return (condition == TD_TRUE);
}

td_s32 isp_drv_wait_exit_callback(const td_void *param)
{
    td_s32 condition;

    condition = *(td_s32 *)param;

    return (condition == 0);
}

td_s32 isp_get_frame_edge(ot_vi_pipe vi_pipe, td_u32 *status)
{
    unsigned long flags = 0;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(status);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = osal_wait_event_timeout_interruptible(&drv_ctx->isp_wait, isp_drv_wait_condition_callback, &drv_ctx->edge,
        g_int_timeout[vi_pipe]);
    if (ret <= 0) {
        *status = 0;
        isp_warn_trace("Get Interrupt timeout failed!\n");
        return TD_FAILURE;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->edge = TD_FALSE;
    *status = drv_ctx->status;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

static int isp_get_vd_start_time_out(ot_vi_pipe vi_pipe, td_u32 milli_sec, td_u32 *pu32status)
{
    unsigned long flags = 0;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (osal_down_interruptible(&drv_ctx->isp_sem)) {
        return -ERESTARTSYS;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->vd_start = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (milli_sec != 0) {
        ret = osal_wait_event_timeout_interruptible(&drv_ctx->isp_wait_vd_start, isp_drv_wait_condition_callback,
            &drv_ctx->vd_start, (milli_sec));
        if (ret <= 0) {
            osal_up(&drv_ctx->isp_sem);
            return TD_FAILURE;
        }
    } else {
        ret = osal_wait_event_interruptible(&drv_ctx->isp_wait_vd_start, isp_drv_wait_condition_callback,
            &drv_ctx->vd_start);
        if (ret) {
            osal_up(&drv_ctx->isp_sem);
            return TD_FAILURE;
        }
    }

    *pu32status = drv_ctx->status;

    osal_up(&drv_ctx->isp_sem);

    return TD_SUCCESS;
}

static int isp_get_vd_end_time_out(ot_vi_pipe vi_pipe, td_u32 milli_sec, td_u32 *pu32status)
{
    unsigned long flags = 0;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (osal_down_interruptible(&drv_ctx->isp_sem_vd)) {
        return -ERESTARTSYS;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->vd_end = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (milli_sec != 0) {
        ret = osal_wait_event_timeout_interruptible(&drv_ctx->isp_wait_vd_end, isp_drv_wait_condition_callback,
            &drv_ctx->vd_end, (milli_sec));
        if (ret <= 0) {
            osal_up(&drv_ctx->isp_sem_vd);
            return TD_FAILURE;
        }
    } else {
        ret =
            osal_wait_event_interruptible(&drv_ctx->isp_wait_vd_end, isp_drv_wait_condition_callback, &drv_ctx->vd_end);
        if (ret) {
            osal_up(&drv_ctx->isp_sem_vd);
            return TD_FAILURE;
        }
    }

    *pu32status = drv_ctx->status;

    osal_up(&drv_ctx->isp_sem_vd);

    return TD_SUCCESS;
}

static int isp_get_vd_be_end_time_out(ot_vi_pipe vi_pipe, td_u32 milli_sec, td_u32 *pu32status)
{
    unsigned long flags = 0;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (osal_down_interruptible(&drv_ctx->isp_sem_be_vd)) {
        return -ERESTARTSYS;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->vd_be_end = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (milli_sec != 0) {
        ret = osal_wait_event_timeout_interruptible(&drv_ctx->isp_wait_vd_be_end, isp_drv_wait_condition_callback,
            &drv_ctx->vd_be_end, (milli_sec));
        if (ret <= 0) {
            osal_up(&drv_ctx->isp_sem_be_vd);
            return TD_FAILURE;
        }
    } else {
        ret = osal_wait_event_interruptible(&drv_ctx->isp_wait_vd_be_end, isp_drv_wait_condition_callback,
            &drv_ctx->vd_be_end);
        if (ret) {
            osal_up(&drv_ctx->isp_sem_be_vd);
            return TD_FAILURE;
        }
    }

    *pu32status = 1;

    osal_up(&drv_ctx->isp_sem_be_vd);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_nr_mode(ot_vi_pipe vi_pipe, td_bool *nr_en)
{
    if (!ckfn_vi_is_ia_nr_en()) {
        return TD_FAILURE;
    }

    *nr_en = call_vi_is_ia_nr_en(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_split_attr(ot_vi_pipe vi_pipe, vi_pipe_split_attr *pipe_split_attr)
{
    if (!ckfn_vi_get_split_attr()) {
        return TD_FAILURE;
    }

    return call_vi_get_split_attr(vi_pipe, pipe_split_attr);
}

td_s32 isp_drv_get_frame_interrupt_attr(ot_vi_pipe vi_pipe, ot_frame_interrupt_attr *frame_int_attr)
{
    if (!ckfn_vi_get_vi_frame_interrupt_attr()) {
        return TD_FAILURE;
    }

    return call_vi_get_vi_frame_interrupt_attr(vi_pipe, frame_int_attr);
}

td_s32 isp_drv_get_pipe_low_delay_en(ot_vi_pipe vi_pipe, td_bool *pipe_low_delay_en)
{
    if (is_virt_pipe(vi_pipe)) {
        return TD_SUCCESS;
    }

    if (!ckfn_vi_get_vi_pipe_low_delay_en()) {
        return TD_FAILURE;
    }

    return call_vi_get_vi_pipe_low_delay_en(vi_pipe, pipe_low_delay_en);
}

td_s32 isp_drv_get_running_mode(const vi_pipe_split_attr *pipe_split_attr, isp_running_mode *running_mode)
{
    isp_check_pointer_return(running_mode);
    isp_check_pointer_return(pipe_split_attr);
    switch (pipe_split_attr->vi_vpss_mode) {
        case OT_VI_ONLINE_VPSS_OFFLINE:
        case OT_VI_ONLINE_VPSS_ONLINE:
            if (pipe_split_attr->video_mode == OT_VI_VIDEO_MODE_NORM) {
                *running_mode = ISP_MODE_RUNNING_ONLINE;
            } else {
                *running_mode = ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE;
            }
            break;
        case OT_VI_OFFLINE_VPSS_OFFLINE:
        case OT_VI_OFFLINE_VPSS_ONLINE:
            if (pipe_split_attr->split_num == 1) {
                *running_mode = ISP_MODE_RUNNING_OFFLINE;
            } else {
                *running_mode = ISP_MODE_RUNNING_STRIPING;
            }
            break;
        default:
            *running_mode = ISP_MODE_RUNNING_BUTT;
            return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_void isp_drv_get_yuv_mode(const vi_pipe_split_attr *pipe_split_attr, td_bool *yuv_mode,
                             isp_data_input_mode *data_input_mode)
{
    if ((pipe_split_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420) ||
        (pipe_split_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (pipe_split_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422) ||
        (pipe_split_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (pipe_split_attr->pixel_format == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_444) ||
        (pipe_split_attr->pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_444)) {
        *yuv_mode = TD_TRUE;
        *data_input_mode = ISP_MODE_BT1120_YUV;
    } else {
        *yuv_mode = TD_FALSE;
        *data_input_mode = ISP_MODE_RAW;
    }

    return;
}

td_s32 isp_drv_work_mode_init(ot_vi_pipe vi_pipe, isp_block_attr *blk_attr)
{
    td_s8 block_id;
    td_s32 ret;
    td_bool nr_en = TD_FALSE;
    unsigned long flags = 0;
    isp_running_mode running_mode = ISP_MODE_RUNNING_OFFLINE;
    vi_pipe_split_attr pipe_split_attr;
    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_frame_interrupt_attr frame_int_attr = { 0 };
    td_bool yuv_mode = TD_FALSE;
    isp_data_input_mode data_input_mode = ISP_MODE_RAW;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(blk_attr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    drv_ctx->work_mode.pre_block_num = drv_ctx->work_mode.block_num;

    (td_void)memset_s(&pipe_split_attr, sizeof(vi_pipe_split_attr), 0, sizeof(vi_pipe_split_attr));

    ret = isp_drv_get_split_attr(vi_pipe, &pipe_split_attr);
    isp_check_return(vi_pipe, ret, "isp_drv_get_split_attr");

    ret = isp_drv_get_nr_mode(vi_pipe, &nr_en);
    isp_check_return(vi_pipe, ret, "isp_drv_get_nr_mode");

    drv_ctx->work_mode.is_ia_nr_enable = nr_en;

    isp_check_block_num_return(pipe_split_attr.split_num);

    ret = isp_drv_get_running_mode(&pipe_split_attr, &running_mode);
    isp_check_return(vi_pipe, ret, "isp_drv_get_running_mode");

    isp_drv_get_yuv_mode(&pipe_split_attr, &yuv_mode, &data_input_mode);

    ret = isp_drv_get_frame_interrupt_attr(vi_pipe, &frame_int_attr);
    isp_check_return(vi_pipe, ret, "isp_drv_get_frame_interrupt_attr");

    block_id = isp_drv_get_block_id(vi_pipe, running_mode);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->yuv_mode = yuv_mode;
    drv_ctx->work_mode.data_input_mode = data_input_mode;

    drv_ctx->work_mode.block_num = pipe_split_attr.split_num;
    drv_ctx->work_mode.block_dev = (td_u8)block_id;
    drv_ctx->work_mode.running_mode = running_mode;
    drv_ctx->work_mode.video_mode = pipe_split_attr.video_mode;
    drv_ctx->work_mode.over_lap = pipe_split_attr.overlap;

    blk_attr->block_num = pipe_split_attr.split_num;
    blk_attr->over_lap = pipe_split_attr.overlap;
    blk_attr->running_mode = running_mode;

    drv_ctx->work_mode.frame_rect.width = pipe_split_attr.wch_out_rect.width;
    drv_ctx->work_mode.frame_rect.height = pipe_split_attr.wch_out_rect.height;

    (td_void)memcpy_s(&blk_attr->frame_rect, sizeof(ot_size), &drv_ctx->work_mode.frame_rect, sizeof(ot_size));
    (td_void)memcpy_s(drv_ctx->work_mode.block_rect, sizeof(ot_rect) * OT_ISP_STRIPING_MAX_NUM, pipe_split_attr.rect,
        sizeof(ot_rect) * OT_ISP_STRIPING_MAX_NUM);
    (td_void)memcpy_s(blk_attr->block_rect, sizeof(ot_rect) * OT_ISP_STRIPING_MAX_NUM, pipe_split_attr.rect,
        sizeof(ot_rect) * OT_ISP_STRIPING_MAX_NUM);
    drv_ctx->frame_int_attr = frame_int_attr;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_work_mode_exit(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    drv_ctx->work_mode.running_mode = ISP_MODE_RUNNING_OFFLINE;

    return TD_SUCCESS;
}

td_u8 isp_drv_get_block_num(ot_vi_pipe vi_pipe)
{
    return g_isp_drv_ctx[vi_pipe].work_mode.block_num;
}

td_s32 isp_drv_chn_select_write(ot_vi_pipe vi_pipe, td_u32 channel_sel)
{
    td_u8 i;
    td_u32 chn_switch[ISP_CHN_SWITCH_NUM] = { 0 };
    isp_chn_sel chn_sel;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    chn_switch[4] = drv_ctx->yuv_mode ? 1 : 0; /* register[4] value 1 means yuv mode */

    for (i = 0; i < ISP_CHN_SWITCH_NUM - 1; i++) {
        chn_switch[i] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[i];
    }

    chn_sel = (isp_chn_sel)(channel_sel & 0x3);
    switch (chn_sel) {
        default:
        case ISP_CHN_SWITCH_NORMAL:
            break;

        case ISP_CHN_SWITCH_2LANE:
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0];
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1];
            break;

        case ISP_CHN_SWITCH_3LANE:
            chn_switch[2] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0]; /* array index 2 */
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1];
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[2]; /* array index 2 */
            break;

        case ISP_CHN_SWITCH_4LANE:
            chn_switch[3] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0]; /* array index 3 */
            chn_switch[2] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1]; /* array index 2 */
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[2]; /* array index 2 */
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[3]; /* array index 3 */
            break;
    }

    if (is_fs_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_drv_set_input_sel(vi_pipe, &chn_switch[0], sizeof(chn_switch));
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_switch_be_online_stt_addr(ot_vi_pipe vi_pipe)
{
    td_u8 read_buf_idx;
    td_u8 cur_read_buf_idx;
    td_u8 write_buf_idx;
    td_s32 ret;
    td_u32 cur_read_flag;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    cur_read_flag = 1 - drv_ctx->be_online_stt_buf.cur_write_flag;

    ret = isp_drv_set_online_stt_addr(vi_pipe, drv_ctx->be_online_stt_buf.be_stt_buf[cur_read_flag].phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Set ISP online stt addr Err!\n", vi_pipe);
    }

    if (g_ldci_tpr_flt_en[vi_pipe] == TD_TRUE) {
        cur_read_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;
        ret = isp_drv_set_ldci_stt_addr(vi_pipe, drv_ctx->ldci_read_buf_attr.ldci_buf[0].phy_addr,
            drv_ctx->ldci_write_buf_attr.ldci_buf[cur_read_buf_idx].phy_addr);
    } else {
        read_buf_idx = drv_ctx->ldci_read_buf_attr.buf_idx;
        write_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;

        ret = isp_drv_set_ldci_stt_addr(vi_pipe, drv_ctx->ldci_read_buf_attr.ldci_buf[read_buf_idx].phy_addr,
            drv_ctx->ldci_write_buf_attr.ldci_buf[write_buf_idx].phy_addr);
    }

    return TD_SUCCESS;
}

/* ISP BE read sta from FHY, online mode */
td_s32 isp_drv_be_online_statistics_read(ot_vi_pipe vi_pipe, isp_stat_info *stat_info, isp_drv_ctx *drv_ctx)
{
    isp_stat *stat = TD_NULL;
    isp_stat_key stat_key;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stat_info);
    ot_unused(drv_ctx);

    stat = (isp_stat *)stat_info->virt_addr;
    if (stat == TD_NULL) {
        return TD_FAILURE;
    }

    stat_key.key = stat_info->stat_key.bit32_isr_access;
    stat->be_update = TD_TRUE;

    isp_drv_be_apb_statistics_read(vi_pipe, stat, stat_key);
    isp_drv_be_stt_statistics_read(vi_pipe, stat, stat_key);

    return isp_drv_switch_be_online_stt_addr(vi_pipe); /* for debug */
}

static td_s32 isp_drv_fe_all_statistics_read(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_stat_info *stat_info)
{
    td_s32 ret;
    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
#ifdef CONFIG_OT_VI_STITCH_GRP
        if (drv_ctx->stitch_attr.main_pipe == TD_TRUE) {
            isp_drv_fe_stitch_statistics_read(vi_pipe, stat_info);
        }

        isp_drv_fe_stitch_non_statistics_read(vi_pipe, stat_info);
#endif
    } else {
        ret = isp_drv_fe_statistics_read(vi_pipe, stat_info);
        if (ret != TD_SUCCESS) {
            isp_err_trace("isp_drv_fe_statistics_read failed!\n");
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_be_all_statistics_read(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_stat_info *stat_info)
{
    td_s32 ret;
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        /* BE statistics for online */
        ret = isp_drv_be_online_statistics_read(vi_pipe, stat_info, drv_ctx);
        if (ret != TD_SUCCESS) {
            isp_err_trace("isp_drv_be_online_statistics_read failed!\n");
            return TD_FAILURE;
        }

        isp_drv_be_online_stitch_statistics_read(vi_pipe, stat_info);
    } else if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode) ||
        is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        /* BE statistics for offline */
        ret = isp_drv_be_offline_statistics_read(vi_pipe, stat_info);
        if (ret) {
            isp_err_trace("isp_drv_be_offline_statistics_read failed!\n");
            return TD_FAILURE;
        }

        isp_drv_be_offline_stitch_statistics_read(vi_pipe, stat_info);
    } else {
        isp_err_trace("running_mode err 0x%x!\n", drv_ctx->work_mode.running_mode);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_statistics_read(ot_vi_pipe vi_pipe, isp_stat_info *stat_info)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stat_info);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    /* online snap, AE and AWB params set by the preview pipe.
      In order to get picture as fast as, dehaze don't used. */
#ifdef CONFIG_OT_SNAP_SUPPORT
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        if ((drv_ctx->snap_attr.picture_pipe_id == vi_pipe) &&
            (drv_ctx->snap_attr.picture_pipe_id != drv_ctx->snap_attr.preview_pipe_id)) {
            return TD_SUCCESS;
        }
    }
#endif
    ret = isp_drv_fe_all_statistics_read(vi_pipe, drv_ctx, stat_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if ((g_isp_alg_run_select[vi_pipe] == OT_ISP_ALG_RUN_FE_ONLY) && (drv_ctx->yuv_mode != TD_TRUE)) {
        return TD_SUCCESS;
    }

    ret = isp_drv_be_all_statistics_read(vi_pipe, drv_ctx, stat_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stat_buf_init(ot_vi_pipe vi_pipe, td_phys_addr_t *point_phy_addr)
{
    td_s32 ret, i;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_u8 *vir_addr = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = {0};
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(point_phy_addr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->statistics_buf.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].Stat", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_stat) * MAX_ISP_STAT_BUF_NUM;
    ret = cmpi_mmz_malloc_cached(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP statistics buf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size, 0, size);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->statistics_buf.phy_addr = phy_addr;
    drv_ctx->statistics_buf.vir_addr = (td_void *)vir_addr;
    drv_ctx->statistics_buf.size = size;

    OSAL_INIT_LIST_HEAD(&drv_ctx->statistics_buf.free_list);
    OSAL_INIT_LIST_HEAD(&drv_ctx->statistics_buf.busy_list);
    OSAL_INIT_LIST_HEAD(&drv_ctx->statistics_buf.user_list);

    for (i = 0; i < MAX_ISP_STAT_BUF_NUM; i++) {
        drv_ctx->statistics_buf.node[i].stat_info.phy_addr = phy_addr + i * sizeof(isp_stat);
        drv_ctx->statistics_buf.node[i].stat_info.virt_addr = (td_void *)(vir_addr + i * sizeof(isp_stat));

        drv_ctx->statistics_buf.node[i].stat_info.stat_key.key =
            ((td_u64)ISP_STATISTICS_KEY << 32) + ISP_STATISTICS_KEY; /* left shift 32 bits */

        osal_list_add_tail(&drv_ctx->statistics_buf.node[i].list, &drv_ctx->statistics_buf.free_list);
    }

    drv_ctx->statistics_buf.init = TD_TRUE;
    drv_ctx->statistics_buf.busy_num = 0;
    drv_ctx->statistics_buf.user_num = 0;
    drv_ctx->statistics_buf.free_num = MAX_ISP_STAT_BUF_NUM;

    *point_phy_addr = drv_ctx->statistics_buf.phy_addr;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_stat_buf_exit(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u8 *vir_addr = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->statistics_buf.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    phy_addr = drv_ctx->statistics_buf.phy_addr;
    vir_addr = (td_u8 *)drv_ctx->statistics_buf.vir_addr;

    drv_ctx->statistics_buf.vir_addr = TD_NULL;
    drv_ctx->statistics_buf.node[0].stat_info.virt_addr = TD_NULL;
    drv_ctx->statistics_buf.node[1].stat_info.virt_addr = TD_NULL;
    drv_ctx->statistics_buf.phy_addr = 0;
    drv_ctx->statistics_buf.node[0].stat_info.phy_addr = 0;
    drv_ctx->statistics_buf.node[1].stat_info.phy_addr = 0;
    drv_ctx->statistics_buf.init = TD_FALSE;

    OSAL_INIT_LIST_HEAD(&drv_ctx->statistics_buf.free_list);
    OSAL_INIT_LIST_HEAD(&drv_ctx->statistics_buf.busy_list);
    OSAL_INIT_LIST_HEAD(&drv_ctx->statistics_buf.user_list);
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stat_buf_user_get(ot_vi_pipe vi_pipe, isp_stat_info **stat_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    struct osal_list_head *plist = TD_NULL;
    isp_stat_node *node = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stat_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_stabuf_init_return(vi_pipe, drv_ctx->statistics_buf.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    if (osal_list_empty(&drv_ctx->statistics_buf.busy_list)) {
        isp_warn_trace("busy list empty\n");
        *stat_info = TD_NULL;
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }

    /* get busy */
    plist = drv_ctx->statistics_buf.busy_list.next;
    if (plist == TD_NULL) {
        isp_warn_trace("busy list empty\n");
        *stat_info = TD_NULL;
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }
    osal_list_del(plist);
    drv_ctx->statistics_buf.busy_num--;

    /* return info */
    node = osal_list_entry(plist, isp_stat_node, list);
    *stat_info = &node->stat_info;

    /* put user */
    osal_list_add_tail(plist, &drv_ctx->statistics_buf.user_list);
    drv_ctx->statistics_buf.user_num++;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_stat_buf_user_put(ot_vi_pipe vi_pipe, isp_stat_info *stat_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    struct osal_list_head *plist = TD_NULL;
    isp_stat_node *node = TD_NULL;
    td_bool valid = TD_FALSE;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stat_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_stabuf_init_return(vi_pipe, drv_ctx->statistics_buf.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    osal_list_for_each(plist, &drv_ctx->statistics_buf.user_list)
    {
        node = osal_list_entry(plist, isp_stat_node, list);
        if (node == TD_NULL) {
            isp_err_trace("node  null pointer\n");
            break;
        }

        if (node->stat_info.phy_addr == stat_info->phy_addr) {
            valid = TD_TRUE;
            node->stat_info.stat_key.key = stat_info->stat_key.key;
            break;
        }
    }

    if (!valid) {
        isp_err_trace("invalid stat info, phy 0x%lx\n", (td_ulong)stat_info->phy_addr);
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }

    /* get user */
    if (plist == TD_NULL) {
        isp_err_trace("user list empty\n");
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }
    osal_list_del(plist);
    drv_ctx->statistics_buf.user_num--;

    /* put free */
    osal_list_add_tail(plist, &drv_ctx->statistics_buf.free_list);
    drv_ctx->statistics_buf.free_num++;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_update_ldci_normal_online_attr(ot_vi_pipe vi_pipe)
{
    td_u8 write_buf_idx;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    write_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;

    drv_ctx->ldci_read_buf_attr.buf_idx = write_buf_idx;
    drv_ctx->ldci_write_buf_attr.buf_idx = (write_buf_idx + 1) % div_0_to_1(drv_ctx->ldci_write_buf_attr.buf_num);

    return TD_SUCCESS;
}

td_s32 isp_drv_update_ldci_tpr_online_attr(ot_vi_pipe vi_pipe, isp_stat *stat)
{
    td_u8 cur_read_buf_idx;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_ldci_stat *ldci_stat = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stat);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    cur_read_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;

    /* Get LDCI Statistics from WriteBuffer(copy statistics to stat), then update WriteSttAddr */
    ldci_stat = (isp_ldci_stat *)drv_ctx->ldci_write_buf_attr.ldci_buf[cur_read_buf_idx].vir_addr;

    if (ldci_stat != TD_NULL) {
        (td_void)memcpy_s(&stat->ldci_stat, sizeof(isp_ldci_stat), ldci_stat, sizeof(isp_ldci_stat));
    }
    drv_ctx->ldci_write_buf_attr.buf_idx = (cur_read_buf_idx + 1) % div_0_to_1(drv_ctx->ldci_write_buf_attr.buf_num);

    return TD_SUCCESS;
}

td_s32 isp_drv_ldci_online_attr_update(ot_vi_pipe vi_pipe, isp_stat *stat)
{
    isp_check_pipe_return(vi_pipe);

    if (g_ldci_tpr_flt_en[vi_pipe] == TD_TRUE) {
        /* Copy LDCI statistics information to stat, then update LDCI WriteSttAddr */
        isp_drv_update_ldci_tpr_online_attr(vi_pipe, stat);
    } else {
        /* Only update LDCI ReadSttAddr and WriteSttAddr */
        isp_drv_update_ldci_normal_online_attr(vi_pipe);
    }
    return TD_SUCCESS;
}

td_s32 isp_drv_switch_be_offline_stt_info(ot_vi_pipe vi_pipe, td_u32 viproc_id)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_viproc_id_return(drv_ctx->work_mode.running_mode, viproc_id);
    if (drv_ctx->isp_init != TD_TRUE) {
        return TD_FAILURE;
    }
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        return TD_SUCCESS;
    }

    isp_drv_switch_be_offline_stt_buf_index(vi_pipe, viproc_id, drv_ctx);
#ifdef CONFIG_OT_VI_STITCH_GRP
    return isp_drv_stitch_sync_be_stt_info(vi_pipe, viproc_id, drv_ctx);
#else
    return TD_SUCCESS;
#endif
}

td_s32 isp_drv_get_be_offline_stt_addr(ot_vi_pipe vi_pipe, td_u32 viproc_id, td_u64 pts,
                                       isp_be_offline_stt_buf *be_offline_stt_addr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_offline_stt_addr);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_viproc_id_return(drv_ctx->work_mode.running_mode, viproc_id);
    if (drv_ctx->isp_init != TD_TRUE) {
        return TD_FAILURE;
    }
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        return TD_FAILURE;
    }

    isp_drv_be_offline_addr_info_update(vi_pipe, viproc_id, pts, be_offline_stt_addr);

    return TD_SUCCESS;
}

td_s32 isp_drv_stat_buf_busy_put(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    struct osal_list_head *plist = TD_NULL;
    isp_stat_node *node = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_stabuf_init_return(vi_pipe, drv_ctx->statistics_buf.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    /* There should be one frame of the newest statistics info in busy list. */
    while (!osal_list_empty(&drv_ctx->statistics_buf.busy_list)) {
        plist = drv_ctx->statistics_buf.busy_list.next;
        if (plist == TD_NULL) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            return TD_FAILURE;
        }
        osal_list_del(plist);
        drv_ctx->statistics_buf.busy_num--;

        osal_list_add_tail(plist, &drv_ctx->statistics_buf.free_list);
        drv_ctx->statistics_buf.free_num++;
    }

    if (osal_list_empty(&drv_ctx->statistics_buf.free_list)) {
        isp_warn_trace("free list empty\n");
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

        return TD_FAILURE;
    }

    /* get free */
    plist = drv_ctx->statistics_buf.free_list.next;
    if (plist == TD_NULL) {
        isp_warn_trace("free list empty\n");
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }
    osal_list_del(plist);
    drv_ctx->statistics_buf.free_num--;

    /* read statistics */
    node = osal_list_entry(plist, isp_stat_node, list);

    drv_ctx->statistics_buf.act_stat = &node->stat_info;
    ret = isp_drv_statistics_read(vi_pipe, &node->stat_info);

    /* put busy */
    osal_list_add_tail(plist, &drv_ctx->statistics_buf.busy_list);
    drv_ctx->statistics_buf.busy_num++;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return ret;
}

static td_s32 isp_drv_fswdr_buf_init(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    td_s32 ret;
    td_u64 size;
    unsigned long flags = 0;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_buf_init_return(vi_pipe, drv_ctx->fswdr_buf_attr.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].WDR", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = WDR_STAT_HOR_NUM * WDR_STAT_VER_NUM * sizeof(td_u32);

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phys_addr, (td_void **)&virt_addr, size * PING_PONG_NUM);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] MmzMalloc wdr buffer Failure!\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(virt_addr, size * PING_PONG_NUM, 0, size * PING_PONG_NUM);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->fswdr_buf_attr.init = TD_TRUE;
    drv_ctx->fswdr_buf_attr.cur_write_idx = 0;
    for (i = 0; i < PING_PONG_NUM; i++) {
        drv_ctx->fswdr_buf_attr.fswdr_buf[i].size = size;
        drv_ctx->fswdr_buf_attr.fswdr_buf[i].phy_addr = phys_addr + i * size;
        drv_ctx->fswdr_buf_attr.fswdr_buf[i].vir_addr = (td_void *)(virt_addr + i * size);
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

static td_s32 isp_drv_fswdr_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    td_phys_addr_t phys_addr;
    unsigned long flags = 0;
    td_void *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);
    isp_check_buf_exit_return(vi_pipe, drv_ctx->fswdr_buf_attr.init);

    phys_addr = drv_ctx->fswdr_buf_attr.fswdr_buf[0].phy_addr;
    virt_addr = drv_ctx->fswdr_buf_attr.fswdr_buf[0].vir_addr;

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->fswdr_buf_attr.init = TD_FALSE;
    drv_ctx->fswdr_buf_attr.cur_write_idx = 0;
    for (i = 0; i < PING_PONG_NUM; i++) {
        drv_ctx->fswdr_buf_attr.fswdr_buf[i].size = 0;
        drv_ctx->fswdr_buf_attr.fswdr_buf[i].phy_addr = 0;
        drv_ctx->fswdr_buf_attr.fswdr_buf[i].vir_addr = TD_NULL;
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (phys_addr != 0) {
        cmpi_mmz_free(phys_addr, virt_addr);
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_drc_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_u8 *vir_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_buf_init_return(vi_pipe, drv_ctx->drc_buf_attr.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].DRC", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = ISP_DRC_BUF_SIZE;

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] MmzMalloc drc buffer Failure!\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size, 0, size);

    drv_ctx->drc_buf_attr.init = TD_TRUE;
    drv_ctx->drc_buf_attr.drc_buf.size = size;
    drv_ctx->drc_buf_attr.drc_buf.phy_addr = phy_addr;
    drv_ctx->drc_buf_attr.drc_buf.vir_addr = (td_void *)vir_addr;

    return TD_SUCCESS;
}

static td_s32 isp_drv_drc_buf_exit(ot_vi_pipe vi_pipe)
{
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);
    isp_check_buf_exit_return(vi_pipe, drv_ctx->drc_buf_attr.init);

    phy_addr = drv_ctx->drc_buf_attr.drc_buf.phy_addr;
    vir_addr = drv_ctx->drc_buf_attr.drc_buf.vir_addr;

    drv_ctx->drc_buf_attr.init = TD_FALSE;
    drv_ctx->drc_buf_attr.drc_buf.size = 0;
    drv_ctx->drc_buf_attr.drc_buf.phy_addr = 0;
    drv_ctx->drc_buf_attr.drc_buf.vir_addr = TD_NULL;

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_ldci_buf_attr_reset(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i;

    drv_ctx->ldci_read_buf_attr.init = TD_FALSE;
    drv_ctx->ldci_write_buf_attr.init = TD_FALSE;
    drv_ctx->ldci_read_buf_attr.buf_num = 0;
    drv_ctx->ldci_write_buf_attr.buf_num = 0;

    for (i = 0; i < g_be_buf_num[vi_pipe]; i++) {
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].phy_addr = 0;
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].vir_addr = TD_NULL;
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].size = 0;
    }

    for (i = 0; i < g_be_buf_num[vi_pipe]; i++) {
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].phy_addr = 0;
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].vir_addr = TD_NULL;
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].size = 0;
    }
}

td_s32 isp_drv_ldci_tpr_buf_malloc(ot_vi_pipe vi_pipe, td_phys_addr_t *ldci_phy_addr, td_u8 **ldci_vir_addr)
{
    td_u8 i, j, write_buf_num, read_buf_num, buf_num_all;
    td_u8 *vir_addr = TD_NULL;
    td_s32 ret;
    td_phys_addr_t phy_addr;
    td_u64 size;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].LDCI", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_ldci_stat);

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        write_buf_num = ISP_ONLINE_LDCI_TPR_BUF_NUM;
        read_buf_num = ISP_ONLINE_LDCI_TPR_BUF_NUM;
    } else {
        write_buf_num = g_be_buf_num[vi_pipe];
        read_buf_num = g_be_buf_num[vi_pipe];
    }

    buf_num_all = write_buf_num + read_buf_num;

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size * buf_num_all);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] MmzMalloc Ldci buffer Failure!\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size * buf_num_all, 0, size * buf_num_all);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->ldci_write_buf_attr.init = TD_TRUE;
    drv_ctx->ldci_write_buf_attr.buf_num = write_buf_num;
    drv_ctx->ldci_write_buf_attr.buf_idx = 0;

    drv_ctx->ldci_read_buf_attr.init = TD_TRUE;
    drv_ctx->ldci_read_buf_attr.buf_num = read_buf_num;
    drv_ctx->ldci_read_buf_attr.buf_idx = 0;

    for (i = 0; i < write_buf_num; i++) {
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].phy_addr = phy_addr + i * size;
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].vir_addr = (td_void *)(vir_addr + i * size);
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].size = size;
    }

    for (i = 0; i < read_buf_num; i++) {
        j = i + write_buf_num;
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].phy_addr = phy_addr + j * size;
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].vir_addr = (td_void *)(vir_addr + j * size);
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].size = size;
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    *ldci_phy_addr = phy_addr;
    *ldci_vir_addr = vir_addr;

    return TD_SUCCESS;
}

td_s32 isp_drv_ldci_tpr_buf_init(ot_vi_pipe vi_pipe)
{
    td_u8 *vir_addr = TD_NULL;
    td_s32 ret;
    td_phys_addr_t phy_addr;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_buf_init_return(vi_pipe, drv_ctx->ldci_read_buf_attr.init);
    isp_check_buf_init_return(vi_pipe, drv_ctx->ldci_write_buf_attr.init);

    ret = isp_drv_ldci_tpr_buf_malloc(vi_pipe, &phy_addr, &vir_addr);
    isp_check_return(vi_pipe, ret, "isp_drv_ldci_tpr_buf_malloc");

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        ret = isp_drv_set_ldci_stt_addr(vi_pipe, drv_ctx->ldci_read_buf_attr.ldci_buf[0].phy_addr,
            drv_ctx->ldci_write_buf_attr.ldci_buf[0].phy_addr);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] Set Ldci Param/OutParam addr Err!\n", vi_pipe);
            goto fail0;
        }

        /* update Write Index */
        drv_ctx->ldci_write_buf_attr.buf_idx =
            (drv_ctx->ldci_write_buf_attr.buf_idx + 1) % div_0_to_1(drv_ctx->ldci_write_buf_attr.buf_num);
    }

    return TD_SUCCESS;

fail0:
    isp_drv_ldci_buf_attr_reset(vi_pipe, drv_ctx);
    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, (td_void *)vir_addr);
    }
    return TD_FAILURE;
}

td_s32 isp_drv_ldci_tpr_buf_exit(ot_vi_pipe vi_pipe)
{
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->ldci_read_buf_attr.init);
    isp_check_buf_exit_return(vi_pipe, drv_ctx->ldci_write_buf_attr.init);

    phy_addr = drv_ctx->ldci_write_buf_attr.ldci_buf[0].phy_addr;
    vir_addr = drv_ctx->ldci_write_buf_attr.ldci_buf[0].vir_addr;

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    isp_drv_ldci_buf_attr_reset(vi_pipe, drv_ctx);
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_ldci_normal_buf_init(ot_vi_pipe vi_pipe)
{
    td_u8 i, buf_num;
    td_s32 ret;
    td_phys_addr_t phy_addr;
    td_u64 size;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->ldci_read_buf_attr.init);
    isp_check_buf_init_return(vi_pipe, drv_ctx->ldci_write_buf_attr.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].LDCI", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }
    size = sizeof(isp_ldci_stat);

    buf_num = is_striping_mode(drv_ctx->work_mode.running_mode) ? ISP_STRIPING_LDCI_NORMAL_BUF_NUM : 1;

    phy_addr = cmpi_mmz_malloc(TD_NULL, ac_name, size * buf_num);
    if (phy_addr == 0) {
        isp_err_trace("alloc ISP[%d] Ldci buf err\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->ldci_read_buf_attr.init = TD_TRUE;
    drv_ctx->ldci_read_buf_attr.buf_num = buf_num;
    drv_ctx->ldci_read_buf_attr.buf_idx = 0;

    drv_ctx->ldci_write_buf_attr.init = TD_TRUE;
    drv_ctx->ldci_write_buf_attr.buf_num = buf_num;
    drv_ctx->ldci_write_buf_attr.buf_idx = MIN2(buf_num - 1, drv_ctx->ldci_read_buf_attr.buf_idx + 1);

    for (i = 0; i < buf_num; i++) {
        drv_ctx->ldci_read_buf_attr.ldci_buf[i].phy_addr = phy_addr + i * size;
        drv_ctx->ldci_write_buf_attr.ldci_buf[i].phy_addr = phy_addr + i * size;
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        ret = isp_drv_set_ldci_stt_addr(vi_pipe, phy_addr, phy_addr);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] Set Ldci stt addr Err!\n", vi_pipe);
            goto fail0;
        }

        /* update Write & Read Index */
        drv_ctx->ldci_read_buf_attr.buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;
        drv_ctx->ldci_write_buf_attr.buf_idx =
            (drv_ctx->ldci_write_buf_attr.buf_idx + 1) % div_0_to_1(drv_ctx->ldci_write_buf_attr.buf_num);
    }

    return TD_SUCCESS;

fail0:
    isp_drv_ldci_buf_attr_reset(vi_pipe, drv_ctx);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, TD_NULL);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_ldci_normal_buf_exit(ot_vi_pipe vi_pipe)
{
    td_phys_addr_t phy_addr;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->ldci_read_buf_attr.init);
    isp_check_buf_exit_return(vi_pipe, drv_ctx->ldci_write_buf_attr.init);

    phy_addr = drv_ctx->ldci_write_buf_attr.ldci_buf[0].phy_addr;

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    isp_drv_ldci_buf_attr_reset(vi_pipe, drv_ctx);
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, TD_NULL);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_ldci_buf_init(ot_vi_pipe vi_pipe)
{
    isp_check_pipe_return(vi_pipe);

    if (g_ldci_tpr_flt_en[vi_pipe] == TD_TRUE) {
        return isp_drv_ldci_tpr_buf_init(vi_pipe);
    } else {
        return isp_drv_ldci_normal_buf_init(vi_pipe);
    }
}

td_s32 isp_drv_ldci_buf_exit(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    if (g_ldci_tpr_flt_en[vi_pipe] == TD_TRUE) {
        return isp_drv_ldci_tpr_buf_exit(vi_pipe);
    } else {
        return isp_drv_ldci_normal_buf_exit(vi_pipe);
    }

    return TD_FAILURE;
}

td_s32 isp_drv_ldci_read_stt_buf_get(ot_vi_pipe vi_pipe, isp_ldci_read_stt_buf *ldci_read_stt_buf)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(ldci_read_stt_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    ldci_read_stt_buf->buf_num = drv_ctx->ldci_read_buf_attr.buf_num;
    ldci_read_stt_buf->buf_idx = 0;
    ldci_read_stt_buf->head_phy_addr = drv_ctx->ldci_read_buf_attr.ldci_buf[0].phy_addr;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

static td_s32 isp_drv_sharpen_mot_set(ot_vi_pipe vi_pipe, td_bool *mot_en)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(mot_en);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->bnr_tpr_filt.sharpen_mot_en = *mot_en;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

static td_s32 isp_drv_bnr_temporal_filt_set(ot_vi_pipe vi_pipe, isp_bnr_temporal_filt *tpr_filt)
{
    td_s32 ret;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(tpr_filt);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->bnr_tpr_filt.cur.nr_en = tpr_filt->nr_en;
    drv_ctx->bnr_tpr_filt.cur.tnr_en = tpr_filt->tnr_en;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (ckfn_vi_set_pipe_bnr_en() != TD_NULL) {
        ret = call_vi_set_pipe_bnr_en(vi_pipe, tpr_filt->nr_en && tpr_filt->tnr_en);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] call_vi_set_pipe_bnr_en Failed!\n", vi_pipe);
            return ret;
        }
    } else {
        isp_err_trace("ISP[%d] vi_set_pipe_bnr_en is TD_NULL\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}
td_s32 isp_drv_clut_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_u8 *vir_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->clut_buf_attr.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].Clut", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = OT_ISP_CLUT_LUT_LENGTH * sizeof(td_u32);

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] MmzMalloc Clut buffer Failure!\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size, 0, size);

    drv_ctx->clut_buf_attr.init = TD_TRUE;
    drv_ctx->clut_buf_attr.clut_buf.size = size;
    drv_ctx->clut_buf_attr.clut_buf.phy_addr = phy_addr;
    drv_ctx->clut_buf_attr.clut_buf.vir_addr = (td_void *)vir_addr;

    return TD_SUCCESS;
}

td_s32 isp_drv_clut_buf_exit(ot_vi_pipe vi_pipe)
{
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->clut_buf_attr.init);

    phy_addr = drv_ctx->clut_buf_attr.clut_buf.phy_addr;
    vir_addr = drv_ctx->clut_buf_attr.clut_buf.vir_addr;

    drv_ctx->clut_buf_attr.init = TD_FALSE;
    drv_ctx->clut_buf_attr.clut_buf.size = 0;
    drv_ctx->clut_buf_attr.clut_buf.phy_addr = 0;
    drv_ctx->clut_buf_attr.clut_buf.vir_addr = TD_NULL;

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_be_buf_malloc(ot_vi_pipe vi_pipe, td_phys_addr_t *be_phy_addr, td_u8 **be_vir_addr, td_u64 *be_size)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_u8 *vir_addr = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = {0};
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].BeCfg", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_wo_reg_cfg);

    ret = cmpi_mmz_malloc_cached(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size * g_be_buf_num[vi_pipe]);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe[%d] alloc ISP BeCfgBuf err!\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size * g_be_buf_num[vi_pipe], 0, size * g_be_buf_num[vi_pipe]);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->be_buf_info.init = TD_TRUE;
    drv_ctx->be_buf_info.be_buf_haddr.phy_addr = phy_addr;
    drv_ctx->be_buf_info.be_buf_haddr.vir_addr = (td_void *)vir_addr;
    drv_ctx->be_buf_info.be_buf_haddr.size = size * g_be_buf_num[vi_pipe];

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    *be_phy_addr = phy_addr;
    *be_vir_addr = vir_addr;
    *be_size = size;

    return TD_SUCCESS;
}

td_s32 isp_drv_be_buf_init(ot_vi_pipe vi_pipe, td_phys_addr_t *point_phy_addr)
{
    td_s32 ret, i;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_buf_node *node = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_u8 *vir_addr = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(point_phy_addr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    ret = isp_drv_be_buf_malloc(vi_pipe, &phy_addr, &vir_addr, &size);
    isp_check_return(vi_pipe, ret, "isp_drv_be_buf_malloc");

    ret = isp_creat_be_buf_queue(&drv_ctx->be_buf_queue, g_be_buf_num[vi_pipe]);
    isp_check_ret_goto(ret, ret, fail0, "Pipe[%d] CreatBeBufQueue fail!\n", vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    for (i = 0; i < g_be_buf_num[vi_pipe]; i++) {
        node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);
        if (node == TD_NULL) {
            isp_err_trace("Pipe[%d] Get QueueGetFreeBeBuf fail!\r\n", vi_pipe);
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            goto fail1;
        }

        isp_drv_be_cfg_buf_addr_init(node, phy_addr, vir_addr, size, i);

        isp_queue_put_free_be_buf(&drv_ctx->be_buf_queue, node);
    }

    drv_ctx->use_node = TD_NULL;
    drv_ctx->running_state = ISP_BE_BUF_STATE_INIT;
    drv_ctx->exit_state = ISP_BE_BUF_READY;

    *point_phy_addr = drv_ctx->be_buf_info.be_buf_haddr.phy_addr;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;

fail1:
    isp_destroy_be_buf_queue(&drv_ctx->be_buf_queue);

fail0:
    drv_ctx->be_buf_info.init = TD_FALSE;
    drv_ctx->be_buf_info.be_buf_haddr.phy_addr = 0;
    drv_ctx->be_buf_info.be_buf_haddr.vir_addr = TD_NULL;
    drv_ctx->be_buf_info.be_buf_haddr.size = 0;

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, (td_void *)vir_addr);
    }

    return TD_FAILURE;
}

td_s32 isp_drv_be_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->be_buf_info.init);

    phy_addr = drv_ctx->be_buf_info.be_buf_haddr.phy_addr;
    vir_addr = drv_ctx->be_buf_info.be_buf_haddr.vir_addr;

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->exit_state = ISP_BE_BUF_WAITING;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    if (check_func_entry(OT_ID_VI) && ckfn_vi_update_vi_vpss_mode()) {
        /* Note: this function cannot be placed in the ISP lock, otherwise it will be deadlocked. */
        call_vi_isp_clear_input_queue(vi_pipe);
    }

    ret = osal_wait_event_timeout_interruptible(&drv_ctx->isp_exit_wait, isp_drv_wait_exit_callback,
        &drv_ctx->be_buf_info.use_cnt, g_isp_exit_timeout);
    if (ret <= 0) {
        isp_err_trace("Pipe:%d isp exit wait failed:ret:%d!\n", vi_pipe, ret);
        return TD_FAILURE;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    isp_destroy_be_buf_queue(&drv_ctx->be_buf_queue);

    drv_ctx->be_buf_info.init = TD_FALSE;
    drv_ctx->be_buf_info.be_buf_haddr.phy_addr = 0;
    drv_ctx->be_buf_info.be_buf_haddr.vir_addr = TD_NULL;
    drv_ctx->be_buf_info.be_buf_haddr.size = 0;
    drv_ctx->exit_state = ISP_BE_BUF_EXIT;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

osal_spinlock_t *isp_drv_get_spin_lock(ot_vi_pipe vi_pipe)
{
    ot_vi_pipe main_pipe;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return &g_isp_lock[vi_pipe];
    } else {
        main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        return &g_isp_sync_lock[main_pipe];
    }
}


td_s32 isp_drv_be_buf_switch_state(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);

    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        drv_ctx->running_state = ISP_BE_BUF_STATE_SWITCH_START;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_void isp_drv_switch_mode_cfg_vc(isp_sync_cfg *sync_cfg)
{
    sync_cfg->vc_num = 0;
    sync_cfg->vc_cfg_num = 0;
    sync_cfg->cfg2_vld_dly_max = 1;

    /* get N (N to 1 WDR) */
    switch (sync_cfg->wdr_mode) {
        default:
            sync_cfg->vc_num_max = 0;
            break;

        case OT_WDR_MODE_2To1_FRAME:
            sync_cfg->vc_num_max = 1;
            break;

        case OT_WDR_MODE_3To1_FRAME:
            sync_cfg->vc_num_max = 2; /* reg config is 2 */
            break;

        case OT_WDR_MODE_4To1_FRAME:
            sync_cfg->vc_num_max = 3; /* reg config is 3 */
            break;
    }
}

static td_void isp_drv_switch_mode_cfg_chn_switch(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    td_u32 chn_switch[ISP_CHN_SWITCH_NUM] = { 0 };

    for (i = 0; i < ISP_CHN_SWITCH_NUM - 1; i++) {
        chn_switch[i] = i;
    }

    if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        chn_switch[0] = 1 % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
        chn_switch[1] = (chn_switch[0] + 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
        chn_switch[2] = (chn_switch[0] + 2) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* array index 2 */
        chn_switch[3] = (chn_switch[0] + 3) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* array index 3 */
    }

    chn_switch[4] = drv_ctx->yuv_mode ? 1 : 0; /* array index 4 */

    isp_drv_set_input_sel(vi_pipe, &chn_switch[0], sizeof(chn_switch));
    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        (td_void)memcpy_s(&drv_ctx->chn_sel_attr[i].wdr_chn_sel, sizeof(chn_switch), chn_switch, sizeof(chn_switch));
    }
}

td_s32 isp_drv_switch_mode(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    td_s32 vi_dev;

    isp_sync_cfg *sync_cfg = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);
    vi_dev = drv_ctx->wdr_attr.vi_dev;

    sync_cfg = &drv_ctx->sync_cfg;
    sync_cfg->wdr_mode = drv_ctx->wdr_cfg.wdr_mode;

    for (j = 0; j < OT_ISP_EXP_RATIO_NUM; j++) {
        for (i = 0; i < CFG2VLD_DLY_LIMIT; i++) {
            sync_cfg->exp_ratio[j][i] = drv_ctx->wdr_cfg.exp_ratio[j];
        }
    }

    /* init cfg when modes change */
    (td_void)memset_s(&drv_ctx->sync_cfg.sync_cfg_buf, sizeof(isp_sync_cfg_buf), 0, sizeof(isp_sync_cfg_buf));
    (td_void)memset_s(&drv_ctx->sync_cfg.node, sizeof(drv_ctx->sync_cfg.node), 0, sizeof(drv_ctx->sync_cfg.node));
    (td_void)memset_s(&drv_ctx->isp_int_info, sizeof(isp_interrupt_info), 0, sizeof(isp_interrupt_info));

    isp_drv_switch_mode_cfg_vc(sync_cfg);

    /* Channel Switch config */
    isp_drv_switch_mode_cfg_chn_switch(vi_pipe, drv_ctx);

    /* pt_int_mask */
    if ((is_full_wdr_mode(sync_cfg->wdr_mode)) || (is_half_wdr_mode(sync_cfg->wdr_mode))) {
        io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK) |= VI_PT_INT_FSTART;
    } else {
        io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK) &= ~(VI_PT_INT_FSTART);
    }

    sync_cfg->pre_wdr_mode = sync_cfg->wdr_mode;

    return TD_SUCCESS;
}

td_s32 isp_drv_get_sync_controlnfo(ot_vi_pipe vi_pipe, isp_sync_cfg *sync_cfg)
{
    td_s32 vi_dev;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg_buf_node *cur_node = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(sync_cfg);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    vi_dev = drv_ctx->wdr_attr.vi_dev;

    sync_cfg->vc_num = (io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT0_ID) & 0x30) >> 4; /* right shift 4 */

    if (sync_cfg->vc_num_max == 0) {
        sync_cfg->vc_num = 0;
    }

    if (sync_cfg->vc_num > sync_cfg->vc_num_max) {
        isp_err_trace("err VC number(%d), can't be large than VC total(%d)!\n", sync_cfg->vc_num, sync_cfg->vc_num_max);
    }

    /* get Cfg2VldDlyMAX */
    if (!isp_sync_buf_is_empty(&sync_cfg->sync_cfg_buf)) {
        cur_node = &sync_cfg->sync_cfg_buf.sync_cfg_buf_node[sync_cfg->sync_cfg_buf.buf_rd_flag];

        if (cur_node != TD_NULL) {
            if (cur_node->valid) {
                sync_cfg->cfg2_vld_dly_max = cur_node->sns_regs_info.cfg2_valid_delay_max;
            }
        }
    }

    if ((sync_cfg->cfg2_vld_dly_max > CFG2VLD_DLY_LIMIT) || (sync_cfg->cfg2_vld_dly_max < 1)) {
        isp_warn_trace("Delay of config to valid is:0x%x\n", sync_cfg->cfg2_vld_dly_max);
        sync_cfg->cfg2_vld_dly_max = 1;
    }

    sync_cfg->vc_cfg_num =
        (sync_cfg->vc_num + sync_cfg->vc_num_max * sync_cfg->cfg2_vld_dly_max) % div_0_to_1(sync_cfg->vc_num_max + 1);

    return TD_SUCCESS;
}

static td_void isp_drv_calc_wdr_gain(isp_sync_cfg *sync_cfg, isp_sync_cfg_buf_node *cur_node, td_u64 *exp)
{
    td_u32 i, j;
    td_u64 tmp;
    td_u64 ratio;

    for (j = 0; j < OT_ISP_EXP_RATIO_NUM; j++) {
        for (i = CFG2VLD_DLY_LIMIT - 1; i >= 1; i--) {
            sync_cfg->exp_ratio[j][i] = sync_cfg->exp_ratio[j][i - 1];
            sync_cfg->wdr_gain[j][i] = sync_cfg->wdr_gain[j][i - 1];
        }

        ratio = exp[j + 1];
        tmp = exp[j];
        tmp = div_0_to_1(tmp);

        while (ratio > (0x1LL << 25) || tmp > (0x1LL << 25)) { /* left shift 25 */
            tmp >>= 1;
            ratio >>= 1;
        }

        ratio = (ratio * cur_node->ae_reg_cfg.wdr_gain[j + 1]) << WDR_EXP_RATIO_SHIFT;
        tmp = (tmp * cur_node->ae_reg_cfg.wdr_gain[j]);

        while (tmp > (0x1LL << 31)) { /* left shift 31 */
            tmp >>= 1;
            ratio >>= 1;
        }

        ratio = osal_div64_u64(ratio, div_0_to_1(tmp));
        sync_cfg->exp_ratio[j][0] = (td_u32)ratio;
        sync_cfg->wdr_gain[j][0] = cur_node->ae_reg_cfg.wdr_gain[j];
    }

    for (i = CFG2VLD_DLY_LIMIT - 1; i >= 1; i--) {
        sync_cfg->lf_mode[i] = sync_cfg->lf_mode[i - 1];
        sync_cfg->wdr_gain[3][i] = sync_cfg->wdr_gain[3][i - 1]; /* array index 3 */
    }

    sync_cfg->wdr_gain[3][0] = 0x100; /* array index 3 */

    if ((is_line_wdr_mode(sync_cfg->wdr_mode)) && (cur_node->ae_reg_cfg.fs_wdr_mode == OT_ISP_FSWDR_LONG_FRAME_MODE)) {
        for (j = 0; j < OT_ISP_EXP_RATIO_NUM; j++) {
            sync_cfg->exp_ratio[j][0] = 0x40;
        }

        sync_cfg->lf_mode[0] = OT_ISP_FSWDR_LONG_FRAME_MODE;
    } else if ((is_line_wdr_mode(sync_cfg->wdr_mode)) &&
        (cur_node->ae_reg_cfg.fs_wdr_mode == OT_ISP_FSWDR_AUTO_LONG_FRAME_MODE)) {
        for (j = 0; j < OT_ISP_EXP_RATIO_NUM; j++) {
            sync_cfg->exp_ratio[j][0] = (sync_cfg->exp_ratio[j][0] < 0x45) ? 0x40 : sync_cfg->exp_ratio[j][0];
        }

        sync_cfg->lf_mode[0] = OT_ISP_FSWDR_AUTO_LONG_FRAME_MODE;
    } else {
        sync_cfg->lf_mode[0] = OT_ISP_FSWDR_NORMAL_MODE;
    }
}

static td_void isp_drv_calc_exp_ratio(isp_sync_cfg *sync_cfg, isp_sync_cfg_buf_node *cur_node,
    isp_sync_cfg_buf_node *pre_node, td_u64 *cur_sns_gain, td_u64 *pre_sns_gain)
{
    td_u32 i;
    td_u64 exp[OT_ISP_WDR_MAX_FRAME_NUM] = { 0 };
    isp_vc_num vc_cfg_num;

    vc_cfg_num = (isp_vc_num)sync_cfg->vc_cfg_num;

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        exp[i] = cur_node->ae_reg_cfg.int_time[i] * cur_sns_gain[0];
    }

    if (is_full_wdr_mode(sync_cfg->wdr_mode)) {
        switch (vc_cfg_num) {
            case ISP_VC_NUM_VS:
                exp[1] = pre_node->ae_reg_cfg.int_time[1] * pre_sns_gain[0];
                exp[2] = pre_node->ae_reg_cfg.int_time[2] * pre_sns_gain[0]; /* array index 2 */
                exp[3] = pre_node->ae_reg_cfg.int_time[3] * pre_sns_gain[0]; /* array index 3 */
                break;

            case ISP_VC_NUM_S:
                exp[2] = pre_node->ae_reg_cfg.int_time[2] * pre_sns_gain[0]; /* array index 2 */
                exp[3] = pre_node->ae_reg_cfg.int_time[3] * pre_sns_gain[0]; /* array index 3 */
                break;

            case ISP_VC_NUM_M:
                exp[3] = pre_node->ae_reg_cfg.int_time[3] * pre_sns_gain[0]; /* array index 3 */
                break;

            default:
            case ISP_VC_NUM_L:
                break;
        }
    } else if (is_2to1_wdr_mode(sync_cfg->wdr_mode)) {
        exp[1] = cur_node->ae_reg_cfg.int_time[1] * cur_sns_gain[1];
    }

    isp_drv_calc_wdr_gain(sync_cfg, cur_node, exp);
}

static td_s32 isp_drv_calc_drc_comp(isp_sync_cfg *sync_cfg, isp_sync_cfg_buf_node *cur_node,
    isp_sync_cfg_buf_node *pre_node)
{
    td_u32 i;
    td_u64 tmp, cur_exp, pre_exp;

    if (is_2to1_wdr_mode(sync_cfg->wdr_mode)) {
        cur_exp = cur_node->ae_reg_cfg.exposure_sf * 0x100;
        pre_exp = pre_node->ae_reg_cfg.exposure_sf * 0x100;
    } else {
        cur_exp = cur_node->ae_reg_cfg.exposure;
        pre_exp = pre_node->ae_reg_cfg.exposure;
        cur_exp = cur_exp * cur_node->ae_reg_cfg.wdr_gain[0];
        pre_exp = pre_exp * pre_node->ae_reg_cfg.wdr_gain[0];
    }
    while (cur_exp > (0x1LL << 31) || pre_exp > (0x1LL << 31)) { /* left shift 31 */
        cur_exp >>= 1;
        pre_exp >>= 1;
    }

    for (i = CFG2VLD_DLY_LIMIT - 1; i >= 1; i--) {
        sync_cfg->drc_comp[i] = sync_cfg->drc_comp[i - 1];
    }

    cur_exp = cur_exp << DRC_COMP_SHIFT;
    tmp = div_0_to_1(pre_exp);

    while (tmp > (0x1LL << 31)) { /* left shift 31 */
        tmp >>= 1;
        cur_exp >>= 1;
    }

    cur_exp = osal_div64_u64(cur_exp, tmp);
    sync_cfg->drc_comp[0] = (td_u32)cur_exp;

    return TD_SUCCESS;
}

td_s32 isp_drv_get_sync_cfg_node(isp_sync_cfg *sync_cfg, isp_sync_cfg_buf_node **cur_node_point,
    isp_sync_cfg_buf_node **pre_node_point)
{
    td_bool err = TD_FALSE;
    td_u32 i;
    isp_sync_cfg_buf_node *cur_node = TD_NULL;
    isp_sync_cfg_buf_node *pre_node = TD_NULL;

    /* update node when VCCfgNum is 0 */
    if (sync_cfg->vc_cfg_num == 0) {
        for (i = CFG2VLD_DLY_LIMIT; i >= 1; i--) {
            sync_cfg->node[i] = sync_cfg->node[i - 1];
        }

        /* avoid skip effective AE results */
        if (isp_sync_buf_is_err(&sync_cfg->sync_cfg_buf)) {
            err = TD_TRUE;
        }

        /* read the newest information */
        isp_sync_buf_read2(&sync_cfg->sync_cfg_buf, &sync_cfg->node[0]);
    }

    cur_node = sync_cfg->node[0];
    if (cur_node == TD_NULL) {
        return TD_FAILURE;
    }
    if (cur_node->valid == TD_FALSE) {
        return TD_FAILURE;
    }

    if (err == TD_TRUE) {
        if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_I2C_TYPE) {
            for (i = 0; i < cur_node->sns_regs_info.reg_num; i++) {
                cur_node->sns_regs_info.i2c_data[i].update = TD_TRUE;
            }
        } else if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_SSP_TYPE) {
            for (i = 0; i < cur_node->sns_regs_info.reg_num; i++) {
                cur_node->sns_regs_info.ssp_data[i].update = TD_TRUE;
            }
        }

        cur_node->sns_regs_info.slv_sync.update = TD_TRUE;
    }

    pre_node = sync_cfg->node[0];
    if (sync_cfg->node[1] != TD_NULL) {
        if (sync_cfg->node[1]->valid == TD_TRUE) {
            pre_node = sync_cfg->node[1];
        }
    }

    *cur_node_point = cur_node;
    *pre_node_point = pre_node;

    return TD_SUCCESS;
}

td_void isp_drv_calc_sns_gain(isp_sync_cfg *sync_cfg, isp_sync_cfg_buf_node *cur_node, isp_sync_cfg_buf_node *pre_node,
    td_u64 *cur_gain, td_u64 *pre_gain)
{
    td_u64 isp_total_gain;

    if (is_2to1_wdr_mode(sync_cfg->wdr_mode)) {
        if (sync_cfg->vc_cfg_num == 0) {
            isp_total_gain = (td_u64)pre_node->ae_reg_cfg.isp_dgain * pre_node->ae_reg_cfg.wdr_gain[0];
            isp_total_gain = div_0_to_1(isp_total_gain >> ISP_GAIN_SHIFT);
            pre_gain[0] = pre_node->ae_reg_cfg.exposure_sf;
            pre_gain[0] = osal_div_u64(pre_gain[0], div_0_to_1(pre_node->ae_reg_cfg.int_time[0]));
            pre_gain[0] = osal_div_u64(pre_gain[0] << ISP_GAIN_SHIFT, isp_total_gain);

            isp_total_gain = (td_u64)cur_node->ae_reg_cfg.isp_dgain * cur_node->ae_reg_cfg.wdr_gain[0];
            isp_total_gain = div_0_to_1(isp_total_gain >> ISP_GAIN_SHIFT);
            cur_gain[0] = cur_node->ae_reg_cfg.exposure_sf;
            cur_gain[0] = osal_div_u64(cur_gain[0], div_0_to_1(cur_node->ae_reg_cfg.int_time[0]));
            cur_gain[0] = osal_div_u64(cur_gain[0] << ISP_GAIN_SHIFT, isp_total_gain);

            isp_total_gain = (td_u64)pre_node->ae_reg_cfg.isp_dgain * pre_node->ae_reg_cfg.wdr_gain[1];
            isp_total_gain = div_0_to_1(isp_total_gain >> ISP_GAIN_SHIFT);
            pre_gain[1] = pre_node->ae_reg_cfg.exposure;
            pre_gain[1] = osal_div_u64(pre_gain[1], div_0_to_1(pre_node->ae_reg_cfg.int_time[1]));
            pre_gain[1] = osal_div_u64(pre_gain[1] << ISP_GAIN_SHIFT, isp_total_gain);

            isp_total_gain = (td_u64)cur_node->ae_reg_cfg.isp_dgain * cur_node->ae_reg_cfg.wdr_gain[1];
            isp_total_gain = div_0_to_1(isp_total_gain >> ISP_GAIN_SHIFT);
            cur_gain[1] = cur_node->ae_reg_cfg.exposure;
            cur_gain[1] = osal_div_u64(cur_gain[1], div_0_to_1(cur_node->ae_reg_cfg.int_time[1]));
            cur_gain[1] = osal_div_u64(cur_gain[1] << ISP_GAIN_SHIFT, isp_total_gain);

            sync_cfg->pre_sns_gain_sf = pre_gain[0];
            sync_cfg->cur_sns_gain_sf = cur_gain[0];
            sync_cfg->pre_sns_gain = pre_gain[1];
            sync_cfg->cur_sns_gain = cur_gain[1];
        }

        pre_gain[0] = sync_cfg->pre_sns_gain_sf;
        cur_gain[0] = sync_cfg->cur_sns_gain_sf;
        pre_gain[1] = sync_cfg->pre_sns_gain;
        cur_gain[1] = sync_cfg->cur_sns_gain;
    } else {
        if (sync_cfg->vc_cfg_num == 0) {
            pre_gain[0] = pre_node->ae_reg_cfg.exposure;
            pre_gain[0] = osal_div_u64(pre_gain[0], div_0_to_1(pre_node->ae_reg_cfg.int_time[0]));
            pre_gain[0] = osal_div_u64(pre_gain[0] << ISP_GAIN_SHIFT, div_0_to_1(pre_node->ae_reg_cfg.isp_dgain));

            cur_gain[0] = cur_node->ae_reg_cfg.exposure;
            cur_gain[0] = osal_div_u64(cur_gain[0], div_0_to_1(cur_node->ae_reg_cfg.int_time[0]));
            cur_gain[0] = osal_div_u64(cur_gain[0] << ISP_GAIN_SHIFT, div_0_to_1(cur_node->ae_reg_cfg.isp_dgain));

            sync_cfg->pre_sns_gain = pre_gain[0];
            sync_cfg->cur_sns_gain = cur_gain[0];
        }

        pre_gain[0] = sync_cfg->pre_sns_gain;
        cur_gain[0] = sync_cfg->cur_sns_gain;
    }
}

td_s32 isp_drv_calc_sync_cfg(isp_sync_cfg *sync_cfg)
{
    td_s32 ret;
    td_u32 i;
    isp_sync_cfg_buf_node *cur_node = TD_NULL;
    isp_sync_cfg_buf_node *pre_node = TD_NULL;
    td_u64 cur_sns_gain[2] = { 0 }; /* array 2 */
    td_u64 pre_sns_gain[2] = { 0 }; /* array 2 */

    isp_check_pointer_return(sync_cfg);

    ret = isp_drv_get_sync_cfg_node(sync_cfg, &cur_node, &pre_node);
    if (ret != TD_SUCCESS) {
        return TD_SUCCESS;
    }

    isp_drv_calc_sns_gain(sync_cfg, cur_node, pre_node, cur_sns_gain, pre_sns_gain);

    /* calculate exposure ratio */
    isp_drv_calc_exp_ratio(sync_cfg, cur_node, pre_node, cur_sns_gain, pre_sns_gain);

    /* calculate AlgProc */
    if (is_line_wdr_mode(sync_cfg->wdr_mode)) {
        for (i = CFG2VLD_DLY_LIMIT - 1; i >= 1; i--) {
            sync_cfg->alg_proc[i] = sync_cfg->alg_proc[i - 1];
        }
        sync_cfg->alg_proc[0] = cur_node->wdr_reg_cfg.wdr_mdt_en;
    }

    /* calculate DRC compensation */
    if (sync_cfg->vc_cfg_num == 0) {
        isp_drv_calc_drc_comp(sync_cfg, cur_node, pre_node);
    }

    return TD_SUCCESS;
}

td_u8 isp_drv_get_fe_sync_index(const isp_drv_ctx *drv_ctx)
{
    td_u8 index;
    index = (drv_ctx->sync_cfg.cfg2_vld_dly_max - 1) / div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
    return index;
}

td_u8 isp_drv_get_index_part2(const isp_drv_ctx *drv_ctx)
{
    td_u8 index;

    if ((drv_ctx->frame_int_attr.interrupt_type == OT_FRAME_INTERRUPT_START) ||
        (drv_ctx->frame_int_attr.interrupt_type == OT_FRAME_INTERRUPT_EARLY)) {
        if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            index = drv_ctx->sync_cfg.cfg2_vld_dly_max;
        } else {
            index = drv_ctx->sync_cfg.cfg2_vld_dly_max + 1;
        }
    } else {
        if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            index = drv_ctx->sync_cfg.cfg2_vld_dly_max - 1;
        } else {
            index = drv_ctx->sync_cfg.cfg2_vld_dly_max;
        }
    }

    if (g_use_bottom_half && drv_ctx->run_once_flag != TD_TRUE) {
        if (index == 0) {
            index = CFG2VLD_DLY_LIMIT - 1;
        } else {
            index = index - 1;
        }
    }

    if (drv_ctx->sync_cfg.node[0] != TD_NULL) {
        if (((drv_ctx->sync_cfg.node[0]->sns_regs_info.sns_type == OT_ISP_SNS_I2C_TYPE) &&
            (drv_ctx->sync_cfg.node[0]->sns_regs_info.com_bus.i2c_dev == -1)) ||
            ((drv_ctx->sync_cfg.node[0]->sns_regs_info.sns_type == OT_ISP_SNS_SSP_TYPE) &&
            (drv_ctx->sync_cfg.node[0]->sns_regs_info.com_bus.ssp_dev.bit4_ssp_dev == -1))) {
            index = 0;
        }
    }

    return index;
}


td_u8 isp_drv_get_be_sync_index(ot_vi_pipe vi_pipe, const isp_drv_ctx *drv_ctx)
{
    td_u8 index;
    td_s32 ret;
    td_bool pipe_low_delay_en = TD_FALSE;
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        index = (drv_ctx->sync_cfg.cfg2_vld_dly_max - 1) / div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
    } else {
        index = isp_drv_get_index_part2(drv_ctx);
        ret = isp_drv_get_pipe_low_delay_en(vi_pipe, &pipe_low_delay_en);
        if (ret != TD_SUCCESS) {
            isp_warn_trace("Get pipe_low_delay_en_attr failed !!!");
        }

        if (pipe_low_delay_en == TD_TRUE) {
            index = index - 1;
            if (g_use_bottom_half) {
                index = index + 1;
            }
        }
    }

    return clip3(index, 0, CFG2VLD_DLY_LIMIT - 1);
}

td_u8 isp_drv_get_pre_be_sync_index(ot_vi_pipe vi_pipe, const isp_drv_ctx *drv_ctx)
{
    td_u8 index;
    if (drv_ctx == TD_NULL) {
        return 0;
    }
    if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        index = isp_drv_get_fe_sync_index(drv_ctx);
    } else {
        index = isp_drv_get_be_sync_index(vi_pipe, drv_ctx);
    }

    return index;
}

static td_u8 isp_get_exp_ratio_sync_index(ot_vi_pipe vi_pipe)
{
    td_u8 sync_index;
    isp_drv_ctx *drv_ctx = TD_NULL;
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    sync_index = isp_drv_get_pre_be_sync_index(vi_pipe, drv_ctx);
    sync_index = MIN2(sync_index, CFG2VLD_DLY_LIMIT - 1);

    if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode)) {
            sync_index += 0x2;
            sync_index = MIN2(sync_index, CFG2VLD_DLY_LIMIT - 1);
        }
    }

    return sync_index;
}

td_void isp_drv_get_exp_ratio(isp_drv_ctx *drv_ctx, ot_vi_pipe vi_pipe, td_u32 *ratio)
{
    td_u8 exp_ratio_index;
    td_u32 i;

    exp_ratio_index = isp_get_exp_ratio_sync_index(vi_pipe);
    if (is_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        if (drv_ctx->sync_cfg.node[0] != TD_NULL) {
            if (((drv_ctx->sync_cfg.node[0]->sns_regs_info.sns_type == OT_ISP_SNS_I2C_TYPE) &&
                (drv_ctx->sync_cfg.node[0]->sns_regs_info.com_bus.i2c_dev == -1)) ||
                ((drv_ctx->sync_cfg.node[0]->sns_regs_info.sns_type == OT_ISP_SNS_SSP_TYPE) &&
                (drv_ctx->sync_cfg.node[0]->sns_regs_info.com_bus.ssp_dev.bit4_ssp_dev == -1))) {
                exp_ratio_index = 0;
            }
        }

        for (i = 0; i < OT_ISP_EXP_RATIO_NUM; i++) {
            ratio[i] = drv_ctx->sync_cfg.exp_ratio[i][exp_ratio_index];
        }
    } else {
    }

    /* when the data of sensor built-in WDR after decompand is 16bit, the ratio value is as follow. */
    if (is_built_in_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        ratio[0] = BUILT_IN_WDR_RATIO_VS_S;
        ratio[1] = BUILT_IN_WDR_RATIO_S_M;
        ratio[2] = BUILT_IN_WDR_RATIO_M_L; /* array index 2 */
    }

    for (i = 0; i < OT_ISP_EXP_RATIO_NUM; i++) {
        ratio[i] = clip3(ratio[i], EXP_RATIO_MIN, EXP_RATIO_MAX);
    }
}

td_u8 isp_drv_get_cfg_node_vc(isp_drv_ctx *drv_ctx)
{
    td_u8 cfg_node_vc;

    if (is_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        if (is_online_mode(drv_ctx->work_mode.running_mode)) {
            cfg_node_vc = (drv_ctx->sync_cfg.cfg2_vld_dly_max - 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
        } else {
            cfg_node_vc = (drv_ctx->sync_cfg.cfg2_vld_dly_max + 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
        }
    } else {
        cfg_node_vc = 0;
    }

    return cfg_node_vc;
}

td_bool isp_drv_snap_get_pictrue_pipe(isp_drv_ctx *drv_ctx)
{
#ifdef CONFIG_OT_SNAP_SUPPORT
    ot_vi_pipe picture_pipe;
    isp_drv_ctx *drv_ctx_pic = TD_NULL;
    isp_running_mode picture_running_mode;
    td_bool online_have_pictrue_pipe = TD_FALSE;

    isp_check_pointer_return(drv_ctx);
    if (drv_ctx->snap_attr.picture_pipe_id >= 0) {
        picture_pipe = drv_ctx->snap_attr.picture_pipe_id;
        drv_ctx_pic = isp_drv_get_ctx(picture_pipe);
        picture_running_mode = drv_ctx_pic->work_mode.running_mode;

        if (is_online_mode(picture_running_mode)) {
            if (picture_pipe != drv_ctx->snap_attr.preview_pipe_id) {
                online_have_pictrue_pipe = TD_TRUE;
            }
        }
    }

    return online_have_pictrue_pipe;
#else
    ot_unused(drv_ctx);

    return TD_FALSE;
#endif
}

isp_sync_cfg_buf_node *isp_drv_stitch_get_cfg_node(isp_drv_ctx *drv_ctx, isp_sync_cfg_buf_node *cfg_node,
    td_u8 cfg_node_idx)
{
    ot_vi_pipe main_pipe;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    if ((drv_ctx->stitch_attr.stitch_enable == TD_TRUE) && (drv_ctx->stitch_attr.main_pipe == TD_FALSE)) {
        main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx_s = isp_drv_get_ctx(main_pipe);
        cfg_node = drv_ctx_s->sync_cfg.node[cfg_node_idx];
        isp_check_pointer_success_return(cfg_node);
    }

    return cfg_node;
}

static td_void isp_drv_reg_config_pq_ai_dgain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 isp_dgain)
{
    td_u8 i;
    isp_drv_ctx *ai_drv_ctx = TD_NULL;

    if (vi_pipe == drv_ctx->ai_info.base_pipe_id) {
        if (drv_ctx->ai_info.ai_pipe_id < 0 || drv_ctx->ai_info.ai_pipe_id >= OT_ISP_MAX_PIPE_NUM) {
            isp_err_trace("Err ai pipe %d!\n", drv_ctx->ai_info.ai_pipe_id);
            return;
        }

        ai_drv_ctx = isp_drv_get_ctx(drv_ctx->ai_info.ai_pipe_id);
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            ai_drv_ctx->be_sync_para.isp_dgain[i] = isp_dgain;
        }
    }
}

static td_void isp_drv_reg_config_be_dgain_snap(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 *isp_dgain)
{
    ot_unused(vi_pipe);
    ot_unused(drv_ctx);
    ot_unused(isp_dgain);
}

td_void isp_drv_reg_config_be_dgain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_ae_reg_cfg_2 *ae_reg_cfg)
{
    td_u32 isp_dgain, gain_coef;
    td_u16 black_offset;
    td_s32 i;

    if ((drv_ctx->ai_info.pq_ai_en == TD_TRUE) && (vi_pipe == drv_ctx->ai_info.ai_pipe_id)) {
        return;
    }

    /* Ispdg can make sure the max value of the histogram */
    isp_dgain = ae_reg_cfg->isp_dgain;
    black_offset = drv_ctx->be_sync_para.be_blc.raw_blc.blc[1] >> 2; /* 2 */
    gain_coef = clip3(0xFFF * 0x100 / div_0_to_1(0xFFF - black_offset) + 1, 0x100, 0x200);
    isp_dgain = ((td_u64)isp_dgain * gain_coef) >> 8; /* 8 */

    isp_drv_reg_config_be_dgain_snap(vi_pipe, drv_ctx, &isp_dgain);
    isp_dgain = clip3(isp_dgain, ISP_DIGITAL_GAIN_MIN, ISP_DIGITAL_GAIN_MAX);
    if (drv_ctx->ai_info.pq_ai_en == TD_TRUE) {
        isp_drv_reg_config_pq_ai_dgain(vi_pipe, drv_ctx, isp_dgain);
    }

    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        drv_ctx->be_sync_para.isp_dgain[i] = isp_dgain;
        drv_ctx->be_sync_para.isp_dgain_no_blc[i] = ae_reg_cfg->isp_dgain;
    }
}

static td_void isp_drv_reg_config_sync_4dgain_snap(ot_vi_pipe vi_pipe, isp_sync_4dgain_cfg *sync_4dgain_cfg,
    isp_drv_ctx *drv_ctx)
{
    ot_unused(vi_pipe);
    ot_unused(sync_4dgain_cfg);
    ot_unused(drv_ctx);
}

td_void isp_drv_reg_config_sync_4dgain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 i;
    td_u8 sync_index;
    isp_sync_4dgain_cfg sync_4dgain_cfg;
    td_u32  wdr_gain[OT_ISP_WDR_MAX_FRAME_NUM] = { 0x100, 0x100, 0x100, 0x100 };
    sync_index = isp_get_exp_ratio_sync_index(vi_pipe);
    sync_index = MIN2(sync_index, CFG2VLD_DLY_LIMIT - 1);

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        wdr_gain[i] = drv_ctx->sync_cfg.wdr_gain[i][sync_index];
        wdr_gain[i] = clip3(wdr_gain[i], ISP_DIGITAL_GAIN_MIN, ISP_DIGITAL_GAIN_MAX);
        sync_4dgain_cfg.wdr_gain[i] = wdr_gain[i];
    }

    isp_drv_reg_config_sync_4dgain_snap(vi_pipe, &sync_4dgain_cfg, drv_ctx);

    isp_drv_reg_config_4dgain(vi_pipe, drv_ctx, &sync_4dgain_cfg);
}

td_void isp_drv_reg_config_sync_piris(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_ae_reg_cfg_2 *ae_reg_cfg)
{
    /* config Piris */
    if (ae_reg_cfg != TD_NULL) {
        if (ae_reg_cfg->piris_valid == TD_TRUE) {
            if (drv_ctx->piris_cb.pfn_piris_gpio_update != TD_NULL) {
                drv_ctx->piris_cb.pfn_piris_gpio_update(vi_pipe, &ae_reg_cfg->piris_pos);
            }
        }
    }
}

static td_s32 isp_drv_reg_config_sync(ot_vi_pipe vi_pipe, td_u8 cfg_node_idx, td_u8 cfg_node_vc)
{
    ot_vi_pipe main_pipe;
    isp_ae_reg_cfg_2 *ae_reg_cfg = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if ((drv_ctx->stitch_attr.stitch_enable == TD_TRUE) && (drv_ctx->stitch_attr.main_pipe == TD_FALSE)) {
        main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx_s = isp_drv_get_ctx(main_pipe);
        cfg_node = drv_ctx_s->sync_cfg.node[cfg_node_idx];
    } else {
        cfg_node = drv_ctx->sync_cfg.node[cfg_node_idx];
    }

    isp_check_pointer_return(cfg_node);

    isp_drv_reg_config_sync_awb_ccm(vi_pipe, drv_ctx, cfg_node_idx, cfg_node_vc);

    if (drv_ctx->sync_cfg.vc_cfg_num == cfg_node_vc) {
        ae_reg_cfg = &cfg_node->ae_reg_cfg;

        isp_drv_reg_config_be_dgain(vi_pipe, drv_ctx, ae_reg_cfg);

        isp_drv_reg_config_sync_4dgain(vi_pipe, drv_ctx);

        isp_drv_reg_config_sync_piris(vi_pipe, drv_ctx, ae_reg_cfg);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_reg_config_isp(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 cfg_node_idx, cfg_node_idx_pre_be, cfg_node_vc;
    td_s32 ret;
    td_u32 ratio[OT_ISP_EXP_RATIO_NUM] = { 0x40 };
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    isp_sync_cfg_buf_node *pre_be_cfg_node = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);

    cfg_node_idx = MIN2(isp_drv_get_be_sync_index(vi_pipe, drv_ctx), CFG2VLD_DLY_LIMIT - 1);
    cfg_node_idx_pre_be = MIN2(isp_drv_get_pre_be_sync_index(vi_pipe, drv_ctx), CFG2VLD_DLY_LIMIT - 1);

    isp_drv_get_exp_ratio(drv_ctx, vi_pipe, ratio);

    cfg_node_vc = isp_drv_get_cfg_node_vc(drv_ctx);

    if (is_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        /* Channel Switch */
        isp_drv_reg_config_chn_sel(vi_pipe, drv_ctx);
    }

    isp_drv_reg_config_fe_blc(vi_pipe, drv_ctx);

    isp_drv_reg_config_fe_dgain(vi_pipe, drv_ctx);

    cfg_node = drv_ctx->sync_cfg.node[cfg_node_idx];
    isp_check_pointer_success_return(cfg_node);

    pre_be_cfg_node = drv_ctx->sync_cfg.node[cfg_node_idx_pre_be];
    isp_check_pointer_success_return(pre_be_cfg_node);

    if (drv_ctx->sync_cfg.vc_cfg_num == cfg_node_vc) {
        isp_drv_reg_config_be_blc(vi_pipe, drv_ctx, cfg_node, pre_be_cfg_node);

        isp_drv_reg_config_wdr(vi_pipe, &pre_be_cfg_node->wdr_reg_cfg, ratio);

        /* config Ldci compensation */
        isp_drv_reg_config_ldci(vi_pipe, drv_ctx);

        /* config drc strength */
        isp_drv_reg_config_drc(vi_pipe, &cfg_node->drc_reg_cfg);
    }
#ifdef CONFIG_OT_SNAP_SUPPORT
    /* online mode double pipe snap, when vipipe == preview_pipe_id, config picture_pipe */
    if ((isp_drv_snap_get_pictrue_pipe(drv_ctx) == TD_TRUE) && (vi_pipe == drv_ctx->snap_attr.picture_pipe_id)) {
        return TD_SUCCESS;
    }
#endif

    ret = isp_drv_reg_config_sync(vi_pipe, cfg_node_idx, cfg_node_vc);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_set_online_reg_be(ot_vi_pipe vi_pipe, td_u64 viproc_irq_status)
{
    return TD_SUCCESS;
}

isp_sync_cfg_buf_node *isp_drv_get_sns_cfg_node(isp_drv_ctx *drv_ctx, td_u8 delay_frm_num)
{
    td_u8 wdr_mode, cfg_node_idx, cfg_node_vc;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    isp_sync_cfg_buf_node *pre_cfg_node = TD_NULL;

    wdr_mode = drv_ctx->wdr_cfg.wdr_mode;
    cfg_node_idx = delay_frm_num / div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
    cfg_node_vc = delay_frm_num % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
    if (drv_ctx->sync_cfg.vc_cfg_num == cfg_node_vc) {
        if (cfg_node_idx > CFG2VLD_DLY_LIMIT - 1) {
            return TD_NULL;
        }

        cfg_node = drv_ctx->sync_cfg.node[cfg_node_idx];
        pre_cfg_node = drv_ctx->sync_cfg.node[cfg_node_idx + 1];

        if (cfg_node == TD_NULL) {
            return TD_NULL;
        }

        /* not config sensor when cur == pre */
        if ((pre_cfg_node != TD_NULL) && (cfg_node == pre_cfg_node)) {
            if ((is_linear_mode(wdr_mode)) || (is_built_in_wdr_mode(wdr_mode))) {
                return TD_NULL;
            }
        }
    }

    return cfg_node;
}

#ifndef ISP_WRITE_I2C_THROUGH_MUL_REG
static td_void isp_drv_write_i2c_one_reg(isp_drv_ctx *drv_ctx, isp_sync_cfg_buf_node *cur_node, td_u32 update_pos,
    td_s8 i2c_dev)
{
    td_u32 i;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    ot_isp_i2c_data *i2c_data = TD_NULL;
    for (i = 0; i < cur_node->sns_regs_info.reg_num; i++) {
        cfg_node = isp_drv_get_sns_cfg_node(drv_ctx, cur_node->sns_regs_info.i2c_data[i].delay_frame_num);
        if (cfg_node == TD_NULL) {
            continue;
        }
        i2c_data = &cfg_node->sns_regs_info.i2c_data[i];
        if (((i2c_data->update == TD_TRUE) && (update_pos == i2c_data->interrupt_pos)) ||
            drv_ctx->bottom_half_cross_frame == TD_TRUE) {
            drv_ctx->bus_cb.pfn_isp_write_i2c_data(i2c_dev, i2c_data->dev_addr, i2c_data->reg_addr,
                i2c_data->addr_byte_num, i2c_data->data, i2c_data->data_byte_num);
        }
    }
    drv_ctx->bottom_half_cross_frame = TD_FALSE;
    return;
}
#else
static td_void isp_drv_write_i2c_mul_reg(isp_drv_ctx *drv_ctx, isp_sync_cfg_buf_node *cur_node, td_u32 update_pos,
    td_s8 i2c_dev)
{
    td_u32 i;
    td_s8 tmp_buf[ISP_WRITE_I2C_MAX_REG_NUM];
    td_u8 idx = 0;
    td_u8 dev_addr, addr_byte_num, data_byte_num;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    ot_isp_i2c_data *i2c_data = TD_NULL;
    for (i = 0; i < cur_node->sns_regs_info.reg_num; i++) {
        cfg_node = isp_drv_get_sns_cfg_node(drv_ctx, cur_node->sns_regs_info.i2c_data[i].delay_frame_num);
        if (cfg_node == TD_NULL) {
            continue;
        }
        i2c_data = &cfg_node->sns_regs_info.i2c_data[i];
        if (((i2c_data->update == TD_TRUE) && (update_pos == i2c_data->interrupt_pos)) ||
            drv_ctx->bottom_half_cross_frame == TD_TRUE) {
            dev_addr = i2c_data->dev_addr;
            addr_byte_num = i2c_data->addr_byte_num;
            data_byte_num = i2c_data->data_byte_num;
            if ((idx + addr_byte_num + data_byte_num) >= ISP_WRITE_I2C_MAX_REG_NUM) {
                drv_ctx->bus_cb.pfn_isp_write_i2c_data(i2c_dev, dev_addr, tmp_buf, sizeof(tmp_buf), idx, addr_byte_num,
                    data_byte_num);
                idx = 0;
            }

            if (i2c_data->addr_byte_num == 1) { /* reg_addr config */
                tmp_buf[idx++] = i2c_data->reg_addr & 0xff;
            } else {
                tmp_buf[idx++] = (i2c_data->reg_addr >> 8) & 0xff; /* reg-addr shift by 8 */
                tmp_buf[idx++] = i2c_data->reg_addr & 0xff;
            }
            if (i2c_data->data_byte_num == 1) { /* data config */
                tmp_buf[idx++] = i2c_data->data & 0xff;
            } else {
                tmp_buf[idx++] = (i2c_data->data >> 8) & 0xff; /* reg addr shift by 8 */
                tmp_buf[idx++] = i2c_data->data & 0xff;
            }
        }
    }
    drv_ctx->bottom_half_cross_frame = TD_FALSE;
    if (idx != 0) {
        drv_ctx->bus_cb.pfn_isp_write_i2c_data(i2c_dev, dev_addr, tmp_buf, sizeof(tmp_buf), idx, addr_byte_num,
            data_byte_num);
    }
    return;
}
#endif

td_s32 isp_drv_write_i2c_data(isp_drv_ctx *drv_ctx, isp_sync_cfg_buf_node *cur_node, td_u32 update_pos, td_s8 i2c_dev)
{
    if (i2c_dev == -1) {
        return TD_SUCCESS;
    }
    if (drv_ctx->bus_cb.pfn_isp_write_i2c_data == TD_NULL) {
        isp_warn_trace("pfn_isp_write_i2c_data is TD_NULL point!\n");
        return TD_FAILURE;
    }

#ifndef ISP_WRITE_I2C_THROUGH_MUL_REG
    isp_drv_write_i2c_one_reg(drv_ctx, cur_node, update_pos, i2c_dev);
#else
    isp_drv_write_i2c_mul_reg(drv_ctx, cur_node, update_pos, i2c_dev);
#endif

    return TD_SUCCESS;
}

td_s32 isp_drv_write_ssp_data(isp_drv_ctx *drv_ctx, isp_sync_cfg_buf_node *cur_node, td_u32 update_pos, td_s8 ssp_dev,
    td_s8 ssp_cs)
{
    td_u32 i;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    ot_isp_ssp_data *ssp_data = TD_NULL;

    if (ssp_dev == -1) {
        return TD_SUCCESS;
    }

    if (drv_ctx->bus_cb.pfn_isp_write_ssp_data == TD_NULL) {
        isp_warn_trace("pfn_isp_write_ssp_data is TD_NULL point!\n");
        return TD_FAILURE;
    }

    for (i = 0; i < cur_node->sns_regs_info.reg_num; i++) {
        cfg_node = isp_drv_get_sns_cfg_node(drv_ctx, cur_node->sns_regs_info.ssp_data[i].delay_frame_num);
        if (cfg_node == TD_NULL) {
            continue;
        }

        ssp_data = &cfg_node->sns_regs_info.ssp_data[i];
        if (((ssp_data->update == TD_TRUE) && (update_pos == ssp_data->interrupt_pos)) ||
            drv_ctx->bottom_half_cross_frame == TD_TRUE) {
            drv_ctx->bus_cb.pfn_isp_write_ssp_data(ssp_dev, ssp_cs, ssp_data->dev_addr, ssp_data->dev_addr_byte_num,
                ssp_data->reg_addr, ssp_data->reg_addr_byte_num, ssp_data->data, ssp_data->data_byte_num);
        }
    }
    drv_ctx->bottom_half_cross_frame = TD_FALSE;
    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
static td_s32 isp_drv_sticth_regs_cfg_sensor_check(isp_sync_cfg_buf_node *cur_node)
{
    if (cur_node == TD_NULL) {
        isp_warn_trace("NULL pointer Stitch!\n");
        return TD_FAILURE;
    }
    if (cur_node->valid == TD_FALSE) {
        isp_warn_trace("Invalid Node Stitch!\n");
        return TD_FAILURE;
    }

    if (cur_node->sns_regs_info.reg_num == 0) {
        isp_warn_trace("Err reg_num Stitch!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stitch_regs_cfg_sensor(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 update_pos)
{
    td_s32 ret;
    td_u32 slave_dev;
    ot_vi_pipe main_pipe = 0;
    isp_sync_cfg_buf_node *cur_node = TD_NULL;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    td_s8 i2c_dev = 0;
    td_s8 ssp_dev = 0;
    td_s8 ssp_cs = 0;

    cur_node = drv_ctx->sync_cfg.node[0];
    ret = isp_drv_sticth_regs_cfg_sensor_check(cur_node);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    slave_dev = cur_node->sns_regs_info.slv_sync.slave_bind_dev;

    if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_I2C_TYPE) {
        i2c_dev = cur_node->sns_regs_info.com_bus.i2c_dev;
    } else if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_SSP_TYPE) {
        ssp_dev = cur_node->sns_regs_info.com_bus.ssp_dev.bit4_ssp_dev;
        ssp_cs = cur_node->sns_regs_info.com_bus.ssp_dev.bit4_ssp_cs;
    }

    if (drv_ctx->stitch_attr.main_pipe == TD_FALSE) {
        main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx = isp_drv_get_ctx(main_pipe);
        cur_node = drv_ctx->sync_cfg.node[0];
        if (cur_node == TD_NULL) {
            isp_warn_trace("cur_node NULL pointer Stitch!\n");
            return TD_FAILURE;
        }
    }

    if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_I2C_TYPE) {
        ret = isp_drv_write_i2c_data(drv_ctx, cur_node, update_pos, i2c_dev);
        isp_check_return(vi_pipe, ret, "isp_drv_write_i2c_data");
    } else if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_SSP_TYPE) {
        ret = isp_drv_write_ssp_data(drv_ctx, cur_node, update_pos, ssp_dev, ssp_cs);
        isp_check_return(vi_pipe, ret, "isp_drv_write_ssp_data");
    }

    /* write slave sns vmax sync */
    cfg_node = isp_drv_get_sns_cfg_node(drv_ctx, cur_node->sns_regs_info.slv_sync.delay_frame_num);
    if (cfg_node == TD_NULL) {
        return TD_FAILURE;
    }

    if ((cfg_node->valid == TD_TRUE) && (cfg_node->sns_regs_info.slv_sync.update == TD_TRUE)) {
        /* adjust the relationship between slavedev and vipipe */
        io_rw_pt_address(vicap_slave_vstime(slave_dev)) = cfg_node->sns_regs_info.slv_sync.slave_vs_time;
    }

    return TD_SUCCESS;
}
#endif

td_s32 isp_drv_normal_regs_cfg_sensor(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 ret;
    td_u32 slave_dev;
    isp_sync_cfg_buf_node *cur_node = TD_NULL;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    td_s8 i2c_dev, ssp_dev, ssp_cs;
    cur_node = drv_ctx->sync_cfg.node[0];
    if (cur_node == TD_NULL) {
        isp_warn_trace("NULL pointer Normal!\n");
        return TD_FAILURE;
    }

    if (cur_node->valid == TD_FALSE) {
        isp_warn_trace("Invalid Node Normal!\n");
        return TD_FAILURE;
    }

    if (cur_node->sns_regs_info.reg_num == 0) {
        isp_warn_trace("Err reg_num Normal!\n");
        return TD_FAILURE;
    }

    if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_I2C_TYPE) {
        i2c_dev = cur_node->sns_regs_info.com_bus.i2c_dev;

        ret = isp_drv_write_i2c_data(drv_ctx, cur_node, drv_ctx->int_pos, i2c_dev);
        isp_check_success_return(vi_pipe, ret, "isp_drv_write_i2c_data");
    } else if (cur_node->sns_regs_info.sns_type == OT_ISP_SNS_SSP_TYPE) {
        ssp_dev = cur_node->sns_regs_info.com_bus.ssp_dev.bit4_ssp_dev;
        ssp_cs = cur_node->sns_regs_info.com_bus.ssp_dev.bit4_ssp_cs;

        ret = isp_drv_write_ssp_data(drv_ctx, cur_node, drv_ctx->int_pos, ssp_dev, ssp_cs);
        isp_check_success_return(vi_pipe, ret, "isp_drv_write_ssp_data");
    }

    /* write slave sns vmax sync */
    cfg_node = isp_drv_get_sns_cfg_node(drv_ctx, cur_node->sns_regs_info.slv_sync.delay_frame_num);
    if (cfg_node == TD_NULL) {
        return TD_FAILURE;
    }

    if ((cfg_node->valid == TD_TRUE) && (cfg_node->sns_regs_info.slv_sync.update == TD_TRUE)) {
        /* adjust the relationship between slavedev and vipipe */
        slave_dev = cfg_node->sns_regs_info.slv_sync.slave_bind_dev;
        io_rw_pt_address(vicap_slave_vstime(slave_dev)) = cfg_node->sns_regs_info.slv_sync.slave_vs_time;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_reg_config_sensor(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 ret;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);

    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        ret = isp_drv_normal_regs_cfg_sensor(vi_pipe, drv_ctx);
        if (ret != TD_SUCCESS) {
            isp_warn_trace("isp_drv_normal_regs_cfg_sensor failure!\n");
            return ret;
        }
    } else {
#ifdef CONFIG_OT_VI_STITCH_GRP
        ret = isp_drv_stitch_regs_cfg_sensor(vi_pipe, drv_ctx, drv_ctx->int_pos);
        if (ret != TD_SUCCESS) {
            isp_warn_trace("isp_drv_stitch_regs_cfg_sensor failure!\n");
            return ret;
        }
#endif
    }

    return TD_SUCCESS;
}


#ifdef CONFIG_OT_SNAP_SUPPORT
/* vi send Proenable */
td_s32 isp_set_pro_enable(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->pro_enable = TD_TRUE;
    drv_ctx->pro_start = TD_FALSE;
    drv_ctx->pro_frm_num = 0;
    drv_ctx->start_snap_num = 0;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_set_snap_attr(ot_vi_pipe vi_pipe, const isp_snap_attr *snap_attr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    td_u8 i;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(snap_attr);

    for (i = 0; i < OT_ISP_MAX_PIPE_NUM; i++) {
        drv_ctx = isp_drv_get_ctx(i);
        if (!drv_ctx->mem_init) {
            continue;
        }

        if ((i == snap_attr->picture_pipe_id) || (i == snap_attr->preview_pipe_id)) {
            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
            (td_void)memcpy_s(&drv_ctx->snap_attr, sizeof(isp_snap_attr), snap_attr, sizeof(isp_snap_attr));
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_set_pro_enable(i);
        }
    }
    return TD_SUCCESS;
}
#endif

static td_void isp_drv_be_af_offline_statistics_read_end_int(ot_vi_pipe vi_pipe)
{
    td_u8 blk_num;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_stat *stat = TD_NULL;
    isp_stat_info *stat_info = TD_NULL;
    isp_stat_key stat_key;

    if ((vi_pipe < 0) || (vi_pipe >= OT_ISP_MAX_PIPE_NUM)) {
        return;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->isp_run_flag == TD_FALSE) {
        return;
    }
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        return;
    }

    /* read af statistics when offline mode at be end proc interrupt */
    blk_num = isp_drv_get_block_num(vi_pipe);
    blk_num = div_0_to_1(blk_num);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    if (drv_ctx->statistics_buf.init == TD_FALSE) {
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return;
    }

    stat_info = drv_ctx->statistics_buf.act_stat;
    if (stat_info == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return;
    }
    stat = (isp_stat *)stat_info->virt_addr;
    if (stat == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return;
    }

    stat_key.key = stat_info->stat_key.bit32_isr_access;
    isp_drv_read_af_offline_stats_end_int(vi_pipe, drv_ctx, blk_num, stat, stat_key);

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
}

td_s32 isp_drv_be_end_int_proc(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->run_once_ok == TD_TRUE) {
        isp_drv_stat_buf_busy_put(vi_pipe);
        drv_ctx->run_once_ok = TD_FALSE;
    }

    if (drv_ctx->yuv_run_once_ok == TD_TRUE) {
        isp_drv_stat_buf_busy_put(vi_pipe);
        drv_ctx->yuv_run_once_ok = TD_FALSE;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->vd_be_end = TD_TRUE;
    osal_wakeup(&drv_ctx->isp_wait_vd_be_end);
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if ((drv_ctx->run_once_ok == TD_TRUE) || (drv_ctx->yuv_run_once_ok == TD_TRUE)) {
        return TD_SUCCESS;
    }

    isp_drv_be_af_offline_statistics_read_end_int(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_drv_proc_init(ot_vi_pipe vi_pipe, isp_proc_mem *proc_mem)
{
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    td_s32 ret;
    td_phys_addr_t phy_addr;
    td_u8 *vir_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(proc_mem);

    if (g_proc_param[vi_pipe] == 0) {
        return TD_SUCCESS;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->porc_mem.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].Proc", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, ISP_PROC_SIZE);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc proc buf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    ((td_char *)vir_addr)[0] = '\0';
    ((td_char *)vir_addr)[ISP_PROC_SIZE - 1] = '\0';

    if (osal_down_interruptible(&drv_ctx->proc_sem)) {
        if (phy_addr != 0) {
            cmpi_mmz_free(phy_addr, vir_addr);
        }
        return -ERESTARTSYS;
    }

    drv_ctx->porc_mem.init = TD_TRUE;
    drv_ctx->porc_mem.phy_addr = phy_addr;
    drv_ctx->porc_mem.size = ISP_PROC_SIZE;
    drv_ctx->porc_mem.virt_addr = (td_void *)vir_addr;

    (td_void)memcpy_s(proc_mem, sizeof(isp_proc_mem), &drv_ctx->porc_mem, sizeof(isp_proc_mem));

    osal_up(&drv_ctx->proc_sem);
#endif
    return TD_SUCCESS;
}

td_s32 isp_drv_proc_exit(ot_vi_pipe vi_pipe)
{
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u8 *vir_addr = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    if (g_proc_param[vi_pipe] == 0) {
        return TD_SUCCESS;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->porc_mem.init);

    phy_addr = drv_ctx->porc_mem.phy_addr;
    vir_addr = (td_u8 *)drv_ctx->porc_mem.virt_addr;

    if (osal_down_interruptible(&drv_ctx->proc_sem)) {
        return -ERESTARTSYS;
    }

    drv_ctx->porc_mem.init = TD_FALSE;
    drv_ctx->porc_mem.phy_addr = 0;
    drv_ctx->porc_mem.size = 0;
    drv_ctx->porc_mem.virt_addr = TD_NULL;
    osal_up(&drv_ctx->proc_sem);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }
#endif
    return TD_SUCCESS;
}

td_s32 isp_update_info_sync(ot_vi_pipe vi_pipe, ot_isp_dcf_update_info *isp_update_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    td_s32 i;
    ot_isp_dcf_update_info *update_info_vir_addr = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    if (drv_ctx->trans_info.update_info.vir_addr == TD_NULL) {
        isp_warn_trace("UpdateInfo buf don't init ok!\n");
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return OT_ERR_ISP_NOT_INIT;
    }

    update_info_vir_addr = (ot_isp_dcf_update_info *)drv_ctx->trans_info.update_info.vir_addr;

    for (i = ISP_MAX_UPDATEINFO_BUF_NUM - 1; i >= 1; i--) {
        (td_void)memcpy_s(update_info_vir_addr + i, sizeof(ot_isp_dcf_update_info), update_info_vir_addr + i - 1,
            sizeof(ot_isp_dcf_update_info));
    }
    (td_void)memcpy_s(update_info_vir_addr, sizeof(ot_isp_dcf_update_info), isp_update_info,
        sizeof(ot_isp_dcf_update_info));
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_frame_info_sync(ot_vi_pipe vi_pipe, ot_isp_frame_info *ispframe_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    td_s32 i;
    ot_isp_frame_info *pframe_info_vir_addr = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    if (drv_ctx->trans_info.frame_info.vir_addr == TD_NULL) {
        isp_warn_trace("UpdateInfo buf don't init ok!\n");
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return OT_ERR_ISP_NOT_INIT;
    }

    pframe_info_vir_addr = (ot_isp_frame_info *)drv_ctx->trans_info.frame_info.vir_addr;

    for (i = ISP_MAX_FRAMEINFO_BUF_NUM - 1; i >= 1; i--) {
        (td_void)memcpy_s(pframe_info_vir_addr + i, sizeof(ot_isp_frame_info), pframe_info_vir_addr + i - 1,
            sizeof(ot_isp_frame_info));
    }
    (td_void)memcpy_s(pframe_info_vir_addr, sizeof(ot_isp_frame_info), ispframe_info, sizeof(ot_isp_frame_info));
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_void isp_cal_sync_info_index(ot_vi_pipe vi_pipe, td_s32 *index)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u8 cfg_dly_max;
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    cfg_dly_max = MAX2(drv_ctx->sync_cfg.cfg2_vld_dly_max, 2); /* cfg_dly_max max 2] */
    if (g_update_pos[vi_pipe] == 0) {
        *index = cfg_dly_max - 1;
    } else {
        *index = cfg_dly_max - 2; /* index [cfg_dly_max - 2] */
    }

    *index = clip3(*index, 0, CFG2VLD_DLY_LIMIT - 1);
}

#ifdef CONFIG_OT_SNAP_SUPPORT
td_s32 isp_get_preview_dcf_info(ot_vi_pipe vi_pipe, ot_isp_dcf_info *isp_dcf)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_isp_dcf_update_info *isp_update_info = TD_NULL;
    ot_isp_dcf_const_info *isp_dcf_const_info = TD_NULL;
    unsigned long flags = 0;
    td_s32 index = 0;
    ot_isp_dcf_update_info *update_info_vir_addr = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    if (drv_ctx->trans_info.update_info.vir_addr == TD_NULL) {
        isp_warn_trace("UpdateInfo buf don't init ok!\n");
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return OT_ERR_ISP_NOT_INIT;
    }

    update_info_vir_addr = (ot_isp_dcf_update_info *)drv_ctx->trans_info.update_info.vir_addr;
    isp_cal_sync_info_index(vi_pipe, &index);

    isp_update_info = update_info_vir_addr + index;

    isp_dcf_const_info = (ot_isp_dcf_const_info *)(update_info_vir_addr + ISP_MAX_UPDATEINFO_BUF_NUM);

    (td_void)memcpy_s(&isp_dcf->isp_dcf_const_info, sizeof(ot_isp_dcf_const_info), isp_dcf_const_info,
        sizeof(ot_isp_dcf_const_info));
    (td_void)memcpy_s(&isp_dcf->isp_dcf_update_info, sizeof(ot_isp_dcf_update_info), isp_update_info,
        sizeof(ot_isp_dcf_update_info));
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}
#endif

td_s32 isp_drv_trans_info_buf_init(ot_vi_pipe vi_pipe, isp_trans_info_buf *trans_info)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_u8 *vir_addr = TD_NULL;
    td_u32 size_dng_info, size_update_info, size_frame_info, size_attach_info, size_color_gammut;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(trans_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->trans_info.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].Trans", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size_dng_info = sizeof(ot_isp_dng_image_static_info);
    size_attach_info = sizeof(ot_isp_attach_info);
    size_color_gammut = sizeof(ot_isp_colorgammut_info);
    size_frame_info = sizeof(ot_isp_frame_info) * ISP_MAX_FRAMEINFO_BUF_NUM;
    size_update_info = sizeof(ot_isp_dcf_update_info) * ISP_MAX_UPDATEINFO_BUF_NUM + sizeof(ot_isp_dcf_const_info);

    size = size_dng_info + size_attach_info + size_color_gammut + size_frame_info + size_update_info;

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] alloc ISP Trans info buf err\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size, 0, size);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->trans_info.init = TD_TRUE;

    drv_ctx->trans_info.dng_info.phy_addr = phy_addr;
    drv_ctx->trans_info.dng_info.vir_addr = (td_void *)vir_addr;

    drv_ctx->trans_info.atta_info.phy_addr = drv_ctx->trans_info.dng_info.phy_addr + size_dng_info;
    drv_ctx->trans_info.atta_info.vir_addr =
        (td_void *)((td_u8 *)drv_ctx->trans_info.dng_info.vir_addr + size_dng_info);

    drv_ctx->trans_info.color_gammut_info.phy_addr = drv_ctx->trans_info.atta_info.phy_addr + size_attach_info;
    drv_ctx->trans_info.color_gammut_info.vir_addr =
        (td_void *)((td_u8 *)drv_ctx->trans_info.atta_info.vir_addr + size_attach_info);

    drv_ctx->trans_info.frame_info.phy_addr = drv_ctx->trans_info.color_gammut_info.phy_addr + size_color_gammut;
    drv_ctx->trans_info.frame_info.vir_addr =
        (td_void *)((td_u8 *)drv_ctx->trans_info.color_gammut_info.vir_addr + size_color_gammut);

    drv_ctx->trans_info.update_info.phy_addr = drv_ctx->trans_info.frame_info.phy_addr + size_frame_info;
    drv_ctx->trans_info.update_info.vir_addr =
        (td_void *)((td_u8 *)drv_ctx->trans_info.frame_info.vir_addr + size_frame_info);

    (td_void)memcpy_s(trans_info, sizeof(isp_trans_info_buf), &drv_ctx->trans_info, sizeof(isp_trans_info_buf));

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_trans_info_buf_exit(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u64 phy_addr;
    td_u8 *vir_addr = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->trans_info.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    phy_addr = drv_ctx->trans_info.dng_info.phy_addr;
    vir_addr = (td_u8 *)drv_ctx->trans_info.dng_info.vir_addr;

    drv_ctx->trans_info.init = TD_FALSE;

    drv_ctx->trans_info.dng_info.phy_addr = 0;
    drv_ctx->trans_info.dng_info.vir_addr = TD_NULL;

    drv_ctx->trans_info.atta_info.phy_addr = 0;
    drv_ctx->trans_info.atta_info.vir_addr = TD_NULL;

    drv_ctx->trans_info.color_gammut_info.phy_addr = 0;
    drv_ctx->trans_info.color_gammut_info.vir_addr = TD_NULL;

    drv_ctx->trans_info.frame_info.phy_addr = 0;
    drv_ctx->trans_info.frame_info.vir_addr = TD_NULL;

    drv_ctx->trans_info.update_info.phy_addr = 0;
    drv_ctx->trans_info.update_info.vir_addr = TD_NULL;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_SNAP_SUPPORT
static td_s32 isp_get_actual_preview_frame_info(ot_vi_pipe vi_pipe, ot_isp_frame_info *isp_frame)
{
    td_u8 vi_pipes;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    unsigned long flags = 0;
    td_s32 index = 0;
    ot_isp_frame_info *frame_info_vir_addr = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

    vi_pipes = drv_ctx->snap_attr.preview_pipe_id;
    drv_ctx_s = isp_drv_get_ctx(vi_pipes);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    if ((isp_frame != TD_NULL) && (drv_ctx_s->trans_info.frame_info.vir_addr != TD_NULL)) {
        frame_info_vir_addr = (ot_isp_frame_info *)drv_ctx_s->trans_info.frame_info.vir_addr;
        isp_cal_sync_info_index(vi_pipe, &index);
        (td_void)memcpy_s(isp_frame, sizeof(ot_isp_frame_info), frame_info_vir_addr + index,
            sizeof(ot_isp_frame_info));
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}
#endif

static td_void isp_get_actual_exp_distance(ot_isp_frame_info *isp_frame, isp_drv_ctx *drv_ctx)
{
    td_u8 i, fe_cfg_node_idx;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;

    fe_cfg_node_idx = isp_drv_get_fe_sync_index(drv_ctx);
    fe_cfg_node_idx = MIN2(fe_cfg_node_idx, CFG2VLD_DLY_LIMIT - 1);
    cfg_node = drv_ctx->sync_cfg.node[fe_cfg_node_idx];
    if (cfg_node == TD_NULL) {
        return;
    }

    for (i = 0; i < (OT_ISP_WDR_MAX_FRAME_NUM - 1); i++) {
        isp_frame->exposure_distance[i] = cfg_node->sns_regs_info.exp_distance[i];
    }
}

td_s32 isp_get_actual_frame_info(ot_vi_pipe vi_pipe, ot_isp_frame_info *isp_frame)
{
    td_u8 vi_pipes;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    unsigned long flags = 0;
    td_s32 index = 0;
    ot_isp_frame_info *frame_info_vir_addr = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_frame);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

    if ((drv_ctx->mem_init == TD_FALSE) && (drv_ctx->wdr_attr.wdr_mode == OT_WDR_MODE_NONE)) {
        for (vi_pipes = 0; vi_pipes < OT_ISP_MAX_PIPE_NUM; vi_pipes++) {
            drv_ctx_s = isp_drv_get_ctx(vi_pipes);
            if ((drv_ctx_s->mem_init == TD_FALSE) || (!is_wdr_mode(drv_ctx_s->wdr_attr.wdr_mode))) {
                continue;
            }

            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

            if ((isp_frame != TD_NULL) && (drv_ctx_s->trans_info.frame_info.vir_addr != TD_NULL)) {
                frame_info_vir_addr = (ot_isp_frame_info *)drv_ctx_s->trans_info.frame_info.vir_addr;
                isp_cal_sync_info_index(vi_pipe, &index);
                (td_void)memcpy_s(isp_frame, sizeof(ot_isp_frame_info), frame_info_vir_addr + index,
                    sizeof(ot_isp_frame_info));
            }
            isp_get_actual_exp_distance(isp_frame, drv_ctx_s);
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        }
    } else {
#ifdef CONFIG_OT_SNAP_SUPPORT
        if (vi_pipe == drv_ctx->snap_attr.picture_pipe_id) {
            isp_get_actual_preview_frame_info(drv_ctx->snap_attr.preview_pipe_id, isp_frame);
        } else {
#endif
            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

            if ((isp_frame != TD_NULL) && (drv_ctx->trans_info.frame_info.vir_addr != TD_NULL)) {
                frame_info_vir_addr = (ot_isp_frame_info *)drv_ctx->trans_info.frame_info.vir_addr;
                isp_cal_sync_info_index(vi_pipe, &index);
                (td_void)memcpy_s(isp_frame, sizeof(ot_isp_frame_info), frame_info_vir_addr + index,
                    sizeof(ot_isp_frame_info));
            }

            isp_get_actual_exp_distance(isp_frame, drv_ctx);
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
#ifdef CONFIG_OT_SNAP_SUPPORT
        }
#endif
    }

    return TD_SUCCESS;
}


td_s32 isp_set_mod_param(ot_isp_mod_param *mod_param)
{
    ot_vi_pipe vi_pipe;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pointer_return(mod_param);

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PIPE_NUM; vi_pipe++) {
        drv_ctx = isp_drv_get_ctx(vi_pipe);
        if (drv_ctx->mem_init == TD_TRUE) {
            isp_err_trace("Does not support changed after isp init!\n");
            return OT_ERR_ISP_NOT_SUPPORT;
        }
    }

    if ((mod_param->interrupt_bottom_half != 0) && (mod_param->interrupt_bottom_half != 1)) {
        isp_err_trace("u32IntBotHalf must be 0 or 1.\n");
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((mod_param->quick_start != 0) && (mod_param->quick_start != 1)) {
        isp_err_trace("u32QuickStart must be 0 or 1.\n");
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((mod_param->long_frame_interrupt_en != 0) && (mod_param->long_frame_interrupt_en != 1)) {
        isp_err_trace("long_frame_interrupt_en must be 0 or 1.\n");
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    g_int_bottom_half = mod_param->interrupt_bottom_half;
    g_quick_start = mod_param->quick_start;
    g_long_frm_int_en = mod_param->long_frame_interrupt_en;
#ifndef __LITEOS__
    if (g_int_bottom_half) {
        g_use_bottom_half = TD_TRUE;
    }
#else
#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
    if (g_int_bottom_half) {
        g_use_bottom_half = TD_TRUE;
    }
#endif
#endif

    return TD_SUCCESS;
}

td_s32 isp_get_mod_param(ot_isp_mod_param *mod_param)
{
    isp_check_pointer_return(mod_param);

    mod_param->interrupt_bottom_half = g_int_bottom_half;
    mod_param->quick_start = g_quick_start;
    mod_param->long_frame_interrupt_en = g_long_frm_int_en;
    return TD_SUCCESS;
}

static td_void isp_drv_update_ctrl_param(ot_vi_pipe vi_pipe, ot_isp_ctrl_param *isp_ctrl_param)
{
    g_proc_param[vi_pipe] = isp_ctrl_param->proc_param;
    g_stat_intvl[vi_pipe] = isp_ctrl_param->stat_interval;
    g_update_pos[vi_pipe] = isp_ctrl_param->update_pos;
    g_int_timeout[vi_pipe] = isp_ctrl_param->interrupt_time_out;
    g_pwm_number[vi_pipe] = isp_ctrl_param->pwm_num;
    g_port_int_delay[vi_pipe] = isp_ctrl_param->port_interrupt_delay;
    g_ldci_tpr_flt_en[vi_pipe] = isp_ctrl_param->ldci_tpr_flt_en;
    g_be_buf_num[vi_pipe] = isp_ctrl_param->be_buf_num;
    g_ob_stats_update_pos[vi_pipe] = isp_ctrl_param->ob_stats_update_pos;
    g_isp_alg_run_select[vi_pipe] = isp_ctrl_param->alg_run_select;
}

static td_s32 isp_drv_ctrl_interrupt_param_check(ot_vi_pipe vi_pipe, ot_isp_ctrl_param *isp_ctrl_param,
    isp_drv_ctx *drv_ctx)
{
    if ((isp_ctrl_param->update_pos != 0) && (isp_ctrl_param->update_pos != 1)) {
        isp_err_trace("vi_pipe:%d update_pos must be 0 or 1.\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((g_update_pos[vi_pipe] != isp_ctrl_param->update_pos) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("vi_pipe:%d Does not support changed after isp init (update_pos)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if ((g_port_int_delay[vi_pipe] != isp_ctrl_param->port_interrupt_delay) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("vi_pipe:%d Does not support changed after isp init (port_interrupt_delay)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if (isp_ctrl_param->ob_stats_update_pos >= OT_ISP_UPDATE_OB_STATS_BUTT) {
        isp_err_trace("vi_pipe:%d err ob_stats_update_pos\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((g_ob_stats_update_pos[vi_pipe] != isp_ctrl_param->ob_stats_update_pos) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("vi_pipe:%d Does not support changed after isp init (ob_stats_update_pos)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_ctrl_alg_sel_check(ot_vi_pipe vi_pipe, ot_isp_ctrl_param *isp_ctrl_param,
    isp_drv_ctx *drv_ctx)
{
    if (isp_ctrl_param->alg_run_select >= OT_ISP_ALG_RUN_BUTT) {
        isp_err_trace("vi_pipe:%d err alg_run_select\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((g_isp_alg_run_select[vi_pipe] != isp_ctrl_param->alg_run_select) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("vi_pipe:%d Does not support changed after isp init (alg_run_select)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if (is_virt_pipe(vi_pipe) && (isp_ctrl_param->alg_run_select == OT_ISP_ALG_RUN_FE_ONLY)) {
        isp_err_trace("vi_pipe:%d Does not support set alg_run_select to %d when virtual pipe!\n",
            vi_pipe, isp_ctrl_param->alg_run_select);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    return TD_SUCCESS;
}

td_s32 isp_set_ctrl_param(ot_vi_pipe vi_pipe, ot_isp_ctrl_param *isp_ctrl_param)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_ctrl_param);
    isp_check_bool_return(isp_ctrl_param->ldci_tpr_flt_en);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    /* isp proc can be closed before mem init */
    /* if isp proc is opened, cannot change proc_param to 0 after mem init */
    if ((g_proc_param[vi_pipe] != 0) && (isp_ctrl_param->proc_param == 0) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("Vipipe:%d proc_param do not support to change %d to 0.\n", vi_pipe, g_proc_param[vi_pipe]);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    /* if isp proc is closed, cannot change proc_param to non-0 after mem init */
    if ((g_proc_param[vi_pipe] == 0) && (isp_ctrl_param->proc_param != 0) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("Vipipe:%d proc_param do not support to change %d to Non-0.\n", vi_pipe, g_proc_param[vi_pipe]);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if (!isp_ctrl_param->stat_interval) {
        isp_err_trace("Vipipe:%d stat_interval must be larger than 0.\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    ret = isp_drv_ctrl_interrupt_param_check(vi_pipe, isp_ctrl_param, drv_ctx);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_drv_ctrl_alg_sel_check(vi_pipe, isp_ctrl_param, drv_ctx);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if ((g_pwm_number[vi_pipe] != isp_ctrl_param->pwm_num) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("Vipipe:%d Does not support changed after isp init (pwm_num)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if ((g_ldci_tpr_flt_en[vi_pipe] != isp_ctrl_param->ldci_tpr_flt_en) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("Vipipe:%d Does not support changed after isp init (ldci_tpr_flt_en)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    if (isp_ctrl_param->be_buf_num < OT_ISP_BE_BUF_NUM_MIN || isp_ctrl_param->be_buf_num > OT_ISP_BE_BUF_NUM_MAX) {
        isp_err_trace("err be_buf_num, range:[%d, %d]\n", OT_ISP_BE_BUF_NUM_MIN, OT_ISP_BE_BUF_NUM_MAX);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((g_be_buf_num[vi_pipe] != isp_ctrl_param->be_buf_num) && (drv_ctx->mem_init == TD_TRUE)) {
        isp_err_trace("Vipipe:%d Does not support changed after isp init (be_buf_num)!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    isp_drv_update_ctrl_param(vi_pipe, isp_ctrl_param);

    return TD_SUCCESS;
}

td_s32 isp_get_ctrl_param(ot_vi_pipe vi_pipe, ot_isp_ctrl_param *isp_ctrl_param)
{
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_ctrl_param);

    isp_ctrl_param->proc_param = g_proc_param[vi_pipe];
    isp_ctrl_param->stat_interval = g_stat_intvl[vi_pipe];
    isp_ctrl_param->update_pos = g_update_pos[vi_pipe];
    isp_ctrl_param->interrupt_time_out = g_int_timeout[vi_pipe];
    isp_ctrl_param->pwm_num = g_pwm_number[vi_pipe];
    isp_ctrl_param->port_interrupt_delay = g_port_int_delay[vi_pipe];
    isp_ctrl_param->ldci_tpr_flt_en = g_ldci_tpr_flt_en[vi_pipe];
    isp_ctrl_param->be_buf_num = g_be_buf_num[vi_pipe];
    isp_ctrl_param->ob_stats_update_pos = g_ob_stats_update_pos[vi_pipe];
    isp_ctrl_param->alg_run_select      = g_isp_alg_run_select[vi_pipe];

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
td_s32 isp_drv_stitch_sync_ex(ot_vi_pipe vi_pipe)
{
    td_u8 k;
    ot_vi_pipe vi_pipe_id;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    for (k = 0; k < drv_ctx->stitch_attr.stitch_pipe_num; k++) {
        vi_pipe_id = drv_ctx->stitch_attr.stitch_bind_id[k];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_id);
        if (drv_ctx_s->stitch_sync != TD_TRUE) {
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stitch_sync(ot_vi_pipe vi_pipe)
{
    td_u8 k;
    ot_vi_pipe vi_pipe_id;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    for (k = 0; k < drv_ctx->stitch_attr.stitch_pipe_num; k++) {
        vi_pipe_id = drv_ctx->stitch_attr.stitch_bind_id[k];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_id);
        if (drv_ctx_s->isp_init != TD_TRUE) {
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}
#endif

td_s32 isp_drv_get_version(isp_version *version)
{
    isp_check_pointer_return(version);

    (td_void)memcpy_s(&g_isp_lib_info, sizeof(isp_version), version, sizeof(isp_version));

    g_isp_lib_info.magic = VERSION_MAGIC + ISP_MAGIC_OFFSET;
    (td_void)memcpy_s(version, sizeof(isp_version), &g_isp_lib_info, sizeof(isp_version));

    return TD_SUCCESS;
}

td_s32 isp_get_frame_info(ot_vi_pipe vi_pipe, ot_isp_frame_info *isp_frame)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_vi_pipe base_pipe = vi_pipe;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_frame);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        base_pipe = drv_ctx->wdr_attr.pipe_id[0];
    }
    if (drv_ctx->ai_info.pq_ai_en == TD_TRUE) {
        base_pipe = drv_ctx->ai_info.base_pipe_id;
    }

    return isp_get_actual_frame_info(base_pipe, isp_frame);
}

td_s32 isp_drv_get_dng_info(ot_vi_pipe vi_pipe, ot_isp_dng_image_static_info *dng_info)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(dng_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if ((dng_info != TD_NULL) && (drv_ctx->trans_info.dng_info.vir_addr != TD_NULL)) {
        (td_void)memcpy_s(dng_info, sizeof(ot_isp_dng_image_static_info),
            (ot_isp_dng_image_static_info *)drv_ctx->trans_info.dng_info.vir_addr,
            sizeof(ot_isp_dng_image_static_info));
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_set_dng_info(ot_vi_pipe vi_pipe, ot_dng_image_dynamic_info *dng_img_dyn_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(dng_img_dyn_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    (td_void)memcpy_s(&drv_ctx->dng_image_dynamic_info[1], sizeof(ot_dng_image_dynamic_info),
        &drv_ctx->dng_image_dynamic_info[0], sizeof(ot_dng_image_dynamic_info));
    (td_void)memcpy_s(&drv_ctx->dng_image_dynamic_info[0], sizeof(ot_dng_image_dynamic_info), dng_img_dyn_info,
        sizeof(ot_dng_image_dynamic_info));

    return TD_SUCCESS;
}

td_s32 isp_drv_get_user_stat_buf(ot_vi_pipe vi_pipe, isp_stat_info *stat)
{
    td_s32 ret;
    isp_stat_info *stat_info = TD_NULL;
    isp_check_pointer_return(stat);

    ret = isp_drv_stat_buf_user_get(vi_pipe, &stat_info);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    if (stat_info == TD_NULL) {
        return TD_FAILURE;
    }

    (td_void)memcpy_s(stat, sizeof(isp_stat_info), stat_info, sizeof(isp_stat_info));

    return TD_SUCCESS;
}


td_s32 isp_drv_get_stat_info_active(ot_vi_pipe vi_pipe, isp_stat_info *stat_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_stat_info act_stat_info;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stat_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->statistics_buf.act_stat == TD_NULL) {
        isp_warn_trace("Pipe[%d] get statistic active buffer err, stat not ready!\n", vi_pipe);
        return TD_FAILURE;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    (td_void)memcpy_s(&act_stat_info, sizeof(isp_stat_info), drv_ctx->statistics_buf.act_stat, sizeof(isp_stat_info));
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    (td_void)memcpy_s(stat_info, sizeof(isp_stat_info), &act_stat_info, sizeof(isp_stat_info));

    return TD_SUCCESS;
}

td_s32 isp_drv_set_reg_kernel_cfgs(ot_vi_pipe vi_pipe, isp_kernel_reg_cfg *reg_kernel_cfg)
{
    td_u32 flag;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(reg_kernel_cfg);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if ((drv_ctx->reg_cfg_info_flag != 0) && (drv_ctx->reg_cfg_info_flag != 1)) {
        isp_err_trace("Pipe[%d] Err reg_cfg_info_flag != 0/1 !!!\n", vi_pipe);
    }

    flag = 1 - drv_ctx->reg_cfg_info_flag;
    (td_void)memcpy_s(&drv_ctx->kernel_cfg[flag], sizeof(isp_kernel_reg_cfg), reg_kernel_cfg,
        sizeof(isp_kernel_reg_cfg));

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->reg_cfg_info_flag = flag;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_set_config_info(ot_vi_pipe vi_pipe, ot_isp_config_info *isp_config_info)
{
    td_u32 i;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_config_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    for (i = ISP_SAVEINFO_MAX - 1; i >= 1; i--) {
        (td_void)memcpy_s(&drv_ctx->snap_info_save[i], sizeof(ot_isp_config_info), &drv_ctx->snap_info_save[i - 1],
            sizeof(ot_isp_config_info));
    }

    (td_void)memcpy_s(&drv_ctx->snap_info_save[0], sizeof(ot_isp_config_info), isp_config_info,
        sizeof(ot_isp_config_info));

    return TD_SUCCESS;
}

td_void isp_drv_be_buf_queue_put_busy(ot_vi_pipe vi_pipe)
{
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_void *vir_addr = TD_NULL;
    isp_be_buf_node *node = TD_NULL;
    struct osal_list_head *list_tmp = TD_NULL;
    struct osal_list_head *list_node = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->use_node == TD_NULL) {
        return;
    }

    osal_list_for_each_safe(list_node, list_tmp, &drv_ctx->be_buf_queue.busy_list)
    {
        node = osal_list_entry(list_node, isp_be_buf_node, list);
        if (node->hold_cnt == 0) {
            isp_queue_del_busy_be_buf(&drv_ctx->be_buf_queue, node);
            isp_queue_put_free_be_buf(&drv_ctx->be_buf_queue, node);
        }
    }

    phy_addr = drv_ctx->use_node->be_cfg_buf.phy_addr;
    vir_addr = drv_ctx->use_node->be_cfg_buf.vir_addr;
    size = drv_ctx->use_node->be_cfg_buf.size;

    osal_flush_dcache_area(vir_addr, phy_addr, size);

    isp_queue_put_busy_be_buf(&drv_ctx->be_buf_queue, drv_ctx->use_node);

    drv_ctx->use_node = TD_NULL;

    return;
}

td_s32 isp_drv_run_once_process(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg *sync_cfg = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->mem_init == TD_FALSE) {
        return OT_ERR_ISP_MEM_NOT_INIT;
    }

    sync_cfg = &drv_ctx->sync_cfg;

    ret = isp_drv_get_sync_controlnfo(vi_pipe, sync_cfg);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_get_sync_controlnfo");
    ret = isp_drv_calc_sync_cfg(sync_cfg);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_calc_sync_cfg");
    ret = isp_drv_reg_config_isp(vi_pipe, drv_ctx);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_isp");
    ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");
    ret = isp_update_info_sync(vi_pipe, &drv_ctx->update_info);
    isp_check_ret_continue(vi_pipe, ret, "isp_update_info_sync");
    ret = isp_frame_info_sync(vi_pipe, &drv_ctx->frame_info);
    isp_check_ret_continue(vi_pipe, ret, "isp_frame_info_sync");

    isp_drv_reg_config_dynamic_blc(vi_pipe, drv_ctx);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    if (drv_ctx->run_once_flag == TD_TRUE) {
        isp_drv_be_buf_queue_put_busy(vi_pipe);
    }
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->run_once_ok = TD_TRUE;

    return TD_SUCCESS;
}

td_s32 isp_drv_yuv_run_once_process(ot_vi_pipe vi_pipe)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->mem_init == TD_FALSE) {
        return OT_ERR_ISP_MEM_NOT_INIT;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    if (drv_ctx->yuv_run_once_flag == TD_TRUE) {
        isp_drv_be_buf_queue_put_busy(vi_pipe);
    }
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->yuv_run_once_ok = TD_TRUE;

    return TD_SUCCESS;
}

td_s32 isp_drv_opt_run_once_info(ot_vi_pipe vi_pipe, td_bool *run_once)
{
    td_bool run_once_flag;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(run_once);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    run_once_flag = *run_once;

    if (run_once_flag == TD_TRUE) {
        if (drv_ctx->use_node != TD_NULL) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("Pipe[%d] isp is running!\r\n", vi_pipe);
            return TD_FAILURE;
        }

        drv_ctx->use_node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);

        if (drv_ctx->use_node == TD_NULL) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("Pipe[%d] get FreeBeBuf is fail!\r\n", vi_pipe);

            return TD_FAILURE;
        }
    }

    drv_ctx->run_once_flag = run_once_flag;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_yuv_run_once_info(ot_vi_pipe vi_pipe, td_bool *run_once)
{
    td_bool yuv_run_once_flag;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(run_once);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    yuv_run_once_flag = *run_once;

    if (yuv_run_once_flag == TD_TRUE) {
        if (drv_ctx->use_node != TD_NULL) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("Pipe[%d] isp is running!\r\n", vi_pipe);
            return TD_FAILURE;
        }

        drv_ctx->use_node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);

        if (drv_ctx->use_node == TD_NULL) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("Pipe[%d] get FreeBeBuf is fail!\r\n", vi_pipe);

            return TD_FAILURE;
        }
    }

    drv_ctx->yuv_run_once_flag = yuv_run_once_flag;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
static td_void isp_drv_stitch_be_buf_ctl(ot_vi_pipe vi_pipe)
{
    td_s32 i;
    td_s32 ret;
    ot_vi_pipe vi_pipes;
    ot_vi_pipe main_pipe;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    unsigned long flags = 0;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];

    osal_spin_lock_irqsave(&g_isp_sync_lock[main_pipe], &flags);

    if (drv_ctx->running_state != ISP_BE_BUF_STATE_RUNNING) {
        osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
        return;
    }

    drv_ctx->running_state = ISP_BE_BUF_STATE_FINISH;

    ret = isp_drv_stitch_sync(vi_pipe);
    if (ret != TD_SUCCESS) {
        osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
        return;
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipes);
        if (drv_ctx_s->running_state != ISP_BE_BUF_STATE_FINISH) {
            osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
            return;
        }
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipes);
        if (drv_ctx_s->be_buf_info.init != TD_TRUE) {
            isp_err_trace("Pipe[%d] BeBuf (bInit != TRUE) !\n", vi_pipe);
            osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
            return;
        }

        if (drv_ctx_s->run_once_flag != TD_TRUE) {
            isp_drv_be_buf_queue_put_busy(vi_pipes);
        }
        drv_ctx_s->running_state = ISP_BE_BUF_STATE_INIT;
    }

    osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);

    return;
}
#endif

td_s32 isp_drv_write_all_ldci_stt_addr(ot_vi_pipe vi_pipe)
{
    td_u8 i, k, write_buf_idx, blk_num, free_num, write_buf_num;
    td_u32 size;
    td_phys_addr_t write_stt_head_addr, write_stt_addr;
    isp_be_wo_reg_cfg *be_reg_cfg = TD_NULL;
    isp_be_buf_node *node = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    blk_num = drv_ctx->work_mode.block_num;
    size = drv_ctx->ldci_write_buf_attr.ldci_buf[0].size;
    write_buf_num = drv_ctx->ldci_write_buf_attr.buf_num;
    write_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;
    write_stt_head_addr = drv_ctx->ldci_write_buf_attr.ldci_buf[write_buf_idx].phy_addr;

    be_reg_cfg = (isp_be_wo_reg_cfg *)drv_ctx->use_node->be_cfg_buf.vir_addr;

    for (i = 0; i < blk_num; i++) {
        write_stt_addr = write_stt_head_addr + i * (size / div_0_to_1(blk_num));
        isp_drv_set_ldci_write_stt_addr_offline(&be_reg_cfg->be_reg_cfg[i], write_stt_addr);
    }

    drv_ctx->ldci_write_buf_attr.buf_idx = (write_buf_idx + 1) % div_0_to_1(write_buf_num);

    free_num = isp_queue_get_free_num(&drv_ctx->be_buf_queue);

    for (k = 0; k < free_num; k++) {
        node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);
        if (node == TD_NULL) {
            isp_err_trace("ISP[%d] Get QueueGetFreeBeBuf fail!\r\n", vi_pipe);
            return TD_FAILURE;
        }

        be_reg_cfg = (isp_be_wo_reg_cfg *)node->be_cfg_buf.vir_addr;
        write_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;
        write_stt_head_addr = drv_ctx->ldci_write_buf_attr.ldci_buf[write_buf_idx].phy_addr;

        for (i = 0; i < blk_num; i++) {
            write_stt_addr = write_stt_head_addr + i * (size / div_0_to_1(blk_num));
            isp_drv_set_ldci_write_stt_addr_offline(&be_reg_cfg->be_reg_cfg[i], write_stt_addr);
        }

        drv_ctx->ldci_write_buf_attr.buf_idx = (write_buf_idx + 1) % div_0_to_1(write_buf_num);
        isp_queue_put_free_be_buf(&drv_ctx->be_buf_queue, node);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_write_be_free_buf(ot_vi_pipe vi_pipe)
{
    td_s32 i;
    td_s32 free_num, ret;
    isp_running_mode running_mode;
    isp_be_buf_node *node = TD_NULL;
    isp_be_wo_reg_cfg *be_reg_cfg_src = TD_NULL;
    isp_be_wo_reg_cfg *be_reg_cfg_dst = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    td_void *vir_addr = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    if (drv_ctx->use_node == TD_NULL) {
        isp_err_trace("Pipe[%d] pstCurNode is null for init!\r\n", vi_pipe);
        return TD_FAILURE;
    }

    be_reg_cfg_src = drv_ctx->use_node->be_cfg_buf.vir_addr;
    running_mode = drv_ctx->work_mode.running_mode;

    free_num = isp_queue_get_free_num(&drv_ctx->be_buf_queue);

    for (i = 0; i < free_num; i++) {
        node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);
        if (node == TD_NULL) {
            isp_err_trace("Pipe[%d] Get QueueGetFreeBeBuf fail!\r\n", vi_pipe);
            return TD_FAILURE;
        }

        be_reg_cfg_dst = (isp_be_wo_reg_cfg *)node->be_cfg_buf.vir_addr;

        if ((running_mode == ISP_MODE_RUNNING_SIDEBYSIDE) || (running_mode == ISP_MODE_RUNNING_STRIPING)) {
            (td_void)memcpy_s(be_reg_cfg_dst, sizeof(isp_be_wo_reg_cfg), be_reg_cfg_src, sizeof(isp_be_wo_reg_cfg));
        } else {
            (td_void)memcpy_s(&be_reg_cfg_dst->be_reg_cfg[0], sizeof(isp_be_all_reg_type),
                &be_reg_cfg_src->be_reg_cfg[0], sizeof(isp_be_all_reg_type));
        }

        phy_addr = drv_ctx->use_node->be_cfg_buf.phy_addr;
        vir_addr = drv_ctx->use_node->be_cfg_buf.vir_addr;
        size = drv_ctx->use_node->be_cfg_buf.size;

        osal_flush_dcache_area(vir_addr, phy_addr, size);

        isp_queue_put_free_be_buf(&drv_ctx->be_buf_queue, node);
    }

    ret = isp_drv_write_all_ldci_stt_addr(vi_pipe);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_be_buf_switch_finish_state(ot_vi_pipe vi_pipe)
{
    td_s32 i;
    ot_vi_pipe vi_pipes;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        if (drv_ctx->running_state != ISP_BE_BUF_STATE_SWITCH_START) {
            osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
            isp_warn_trace("Pipe[%d] isp isn't init state!\n", vi_pipe);
            return TD_FAILURE;
        }

        drv_ctx->running_state = ISP_BE_BUF_STATE_SWITCH;

        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[i];
            drv_ctx_s = isp_drv_get_ctx(vi_pipes);
            if (drv_ctx_s->running_state != ISP_BE_BUF_STATE_SWITCH) {
                osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
                isp_warn_trace("Pipe[%d] isp isn't  finish state!\n", vi_pipe);
                return TD_FAILURE;
            }
        }

        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[i];
            drv_ctx_s = isp_drv_get_ctx(vi_pipes);
            drv_ctx_s->running_state = ISP_BE_BUF_STATE_INIT;
        }
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
static td_s32 isp_drv_stitch_write_be_buf_all(ot_vi_pipe vi_pipe)
{
    td_s32 i, ret;
    ot_vi_pipe vi_pipes, main_pipe;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];

    ret = isp_drv_write_be_free_buf(vi_pipe);
    isp_check_return(vi_pipe, ret, "isp_drv_write_be_free_buf");

    osal_spin_lock_irqsave(&g_isp_sync_lock[main_pipe], &flags);

    ret = isp_drv_stitch_sync_ex(vi_pipe);
    if (ret != TD_SUCCESS) {
        osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);

        return TD_SUCCESS;
    }

    if (drv_ctx->running_state == ISP_BE_BUF_STATE_SWITCH_START) {
        drv_ctx->running_state = ISP_BE_BUF_STATE_SWITCH;
        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[i];
            drv_ctx_s = isp_drv_get_ctx(vi_pipes);
            if (drv_ctx_s->running_state != ISP_BE_BUF_STATE_SWITCH) {
                osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
                drv_ctx->running_state = ISP_BE_BUF_STATE_SWITCH_START;
                return TD_SUCCESS;
            }
        }
        drv_ctx->running_state = ISP_BE_BUF_STATE_SWITCH_START;
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipes);
        if ((drv_ctx_s->be_buf_info.init != TD_TRUE) || (drv_ctx_s->use_node == TD_NULL)) {
            osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
            isp_err_trace("Pipe[%d] BeBuf (bInit != TRUE) or use_node is TD_NULL!\n", vi_pipe);
            return TD_FAILURE;
        }

        isp_queue_put_busy_be_buf(&drv_ctx_s->be_buf_queue, drv_ctx_s->use_node);
        drv_ctx_s->use_node = TD_NULL;
        drv_ctx_s->running_state = ISP_BE_BUF_STATE_INIT;
    }

    osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);

    return TD_SUCCESS;
}
#endif

td_s32 isp_drv_get_be_free_buf(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_wo_cfg_buf)
{
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_wo_cfg_buf *cur_node_buf = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_wo_cfg_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);

    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (drv_ctx->use_node == TD_NULL) {
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        return TD_FAILURE;
    }

    cur_node_buf = &drv_ctx->use_node->be_cfg_buf;
    (td_void)memcpy_s(be_wo_cfg_buf, sizeof(isp_be_wo_cfg_buf), cur_node_buf, sizeof(isp_be_wo_cfg_buf));

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_be_buf_first(ot_vi_pipe vi_pipe, td_phys_addr_t *point_phy_addr)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_buf_node *node = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(point_phy_addr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);
    if (node == TD_NULL) {
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

        isp_err_trace("Pipe[%d] Get FreeBeBuf to user fail!\r\n", vi_pipe);
        return TD_FAILURE;
    }

    drv_ctx->use_node = node;

    *point_phy_addr = drv_ctx->use_node->be_cfg_buf.phy_addr;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_be_last_buf(ot_vi_pipe vi_pipe, td_phys_addr_t *point_phy_addr)
{
    td_u8 i;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_buf_node *node = TD_NULL;
    isp_be_wo_reg_cfg *be_reg_cfg_dst = TD_NULL;
    struct osal_list_head *list_tmp = TD_NULL;
    struct osal_list_head *list_node = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(point_phy_addr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);

    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    osal_list_for_each_safe(list_node, list_tmp, &drv_ctx->be_buf_queue.busy_list)
    {
        node = osal_list_entry(list_node, isp_be_buf_node, list);

        node->hold_cnt = 0;

        isp_queue_del_busy_be_buf(&drv_ctx->be_buf_queue, node);
        isp_queue_put_free_be_buf(&drv_ctx->be_buf_queue, node);
    }

    if (drv_ctx->use_node == TD_NULL) {
        node = isp_queue_get_free_be_buf_tail(&drv_ctx->be_buf_queue);
        if (node == TD_NULL) {
            osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
            isp_err_trace("Pipe[%d] Get LastBeBuf fail!\r\n", vi_pipe);
            return TD_FAILURE;
        }

        drv_ctx->use_node = node;
    }

    be_reg_cfg_dst = (isp_be_wo_reg_cfg *)drv_ctx->use_node->be_cfg_buf.vir_addr;

    for (i = drv_ctx->work_mode.pre_block_num; i < drv_ctx->work_mode.block_num; i++) {
        (td_void)memcpy_s(&be_reg_cfg_dst->be_reg_cfg[i], sizeof(isp_be_all_reg_type), &be_reg_cfg_dst->be_reg_cfg[0],
            sizeof(isp_be_all_reg_type));
    }

    *point_phy_addr = drv_ctx->use_node->be_cfg_buf.phy_addr;

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_be_buf_run_state(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);
    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);

    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        if (drv_ctx->running_state != ISP_BE_BUF_STATE_INIT) {
            osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

            isp_warn_trace("Pipe[%d] isp isn't init state!\n", vi_pipe);
            return TD_FAILURE;
        }

        drv_ctx->running_state = ISP_BE_BUF_STATE_RUNNING;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_be_buf_ctl(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);

    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        if (drv_ctx->be_buf_info.init != TD_TRUE) {
            isp_err_trace("Pipe[%d] BeBuf (bInit != TRUE) !\n", vi_pipe);
            return TD_FAILURE;
        }

        osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
        if (drv_ctx->run_once_flag != TD_TRUE) {
            isp_drv_be_buf_queue_put_busy(vi_pipe);
        }
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    } else {
#ifdef CONFIG_OT_VI_STITCH_GRP
        isp_drv_stitch_be_buf_ctl(vi_pipe);
#endif
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_all_be_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);

    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        if (drv_ctx->be_buf_info.init != TD_TRUE) {
            isp_err_trace("Pipe[%d] BeBuf (bInit != TRUE) !\n", vi_pipe);
            return TD_FAILURE;
        }

        osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

        ret = isp_drv_write_be_free_buf(vi_pipe);
        if (ret != TD_SUCCESS) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

            isp_err_trace("Pipe[%d] ISP_DRV_WriteBeFreeBuf fail!\n", vi_pipe);
            return ret;
        }

        isp_queue_put_busy_be_buf(&drv_ctx->be_buf_queue, drv_ctx->use_node);
        drv_ctx->use_node = TD_NULL;

        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    } else {
#ifdef CONFIG_OT_VI_STITCH_GRP
        ret = isp_drv_stitch_write_be_buf_all(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_err_trace("Pipe[%d] ISP_DRV_StitchWriteBeBufAll fail!\n", vi_pipe);
            return ret;
        }
#endif
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_sync_cfg_set(ot_vi_pipe vi_pipe, isp_sync_cfg_buf_node *sync_cfg_buf_node)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg_buf *sync_cfg_buf = TD_NULL;
    isp_sync_cfg_buf_node *cur_node = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(sync_cfg_buf_node);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    sync_cfg_buf = &drv_ctx->sync_cfg.sync_cfg_buf;

    if (isp_sync_buf_is_full(sync_cfg_buf)) {
        isp_err_trace("Pipe[%d] isp sync buffer is full\n", vi_pipe);
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }

    if ((sync_cfg_buf_node->sns_regs_info.sns_type >= OT_ISP_SNS_TYPE_BUTT) ||
        (sync_cfg_buf_node->ae_reg_cfg.fs_wdr_mode >= OT_ISP_FSWDR_MODE_BUTT) ||
        (sync_cfg_buf_node->sns_regs_info.cfg2_valid_delay_max > CFG2VLD_DLY_LIMIT) ||
        (sync_cfg_buf_node->sns_regs_info.cfg2_valid_delay_max < 1) ||
        (sync_cfg_buf_node->sns_regs_info.slv_sync.delay_frame_num > CFG2VLD_DLY_LIMIT) ||
        (sync_cfg_buf_node->sns_regs_info.reg_num > OT_ISP_MAX_SNS_REGS)) {
        isp_err_trace("Pipe[%d] Invalid sns_regs_info!\n", vi_pipe);
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return TD_FAILURE;
    }

    cur_node = &sync_cfg_buf->sync_cfg_buf_node[sync_cfg_buf->buf_wr_flag];
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    (td_void)memcpy_s(cur_node, sizeof(isp_sync_cfg_buf_node), sync_cfg_buf_node, sizeof(isp_sync_cfg_buf_node));

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    sync_cfg_buf->buf_wr_flag = (sync_cfg_buf->buf_wr_flag + 1) % ISP_SYNC_BUF_NODE_NUM;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_set_wdr_cfg(ot_vi_pipe vi_pipe, isp_wdr_cfg *wdr_cfg)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(wdr_cfg);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    if (wdr_cfg->wdr_mode >= OT_WDR_MODE_BUTT) {
        isp_err_trace("Pipe[%d] Invalid WDR mode %d!\n", vi_pipe, wdr_cfg->wdr_mode);
        return TD_FAILURE;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    (td_void)memcpy_s(&drv_ctx->wdr_cfg, sizeof(isp_wdr_cfg), wdr_cfg, sizeof(isp_wdr_cfg));
    ret = isp_drv_switch_mode(vi_pipe, drv_ctx);
    if (ret != TD_SUCCESS) {
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        isp_err_trace("Pipe[%d] isp_drv_switch_mode err 0x%x!\n", vi_pipe, ret);
        return ret;
    }
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}
#ifdef CONFIG_OT_VI_STITCH_GRP
td_s32 isp_drv_get_stitch_attr(ot_vi_pipe vi_pipe, vi_stitch_attr *stitch_attr)
{
    td_u8 i;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    vi_stitch_attr stitch_attr_str = { 0 };
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stitch_attr);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (!ckfn_vi_get_pipe_stitch_attr()) {
        isp_err_trace("pipe[%d] ckfn_vi_get_pipe_stitch_attr is null\n", vi_pipe);
        return TD_FAILURE;
    }

    ret = call_vi_get_pipe_stitch_attr(vi_pipe, &stitch_attr_str);
    if (ret != TD_SUCCESS) {
        isp_err_trace("pipe[%d] call_vi_get_pipe_stitch_attr failed 0x%x!\n", vi_pipe, ret);
        return ret;
    }

    if (stitch_attr_str.stitch_enable) {
        if ((stitch_attr_str.stitch_pipe_num < 1) || (stitch_attr_str.stitch_pipe_num > OT_ISP_MAX_STITCH_NUM)) {
            isp_err_trace("pipe[%d] err stitch num %d\n", vi_pipe, stitch_attr_str.stitch_pipe_num);
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        for (i = 0; i < stitch_attr_str.stitch_pipe_num; i++) {
            isp_check_pipe_return(stitch_attr_str.stitch_bind_id[i]);
        }
    }
    (td_void)memcpy_s(&drv_ctx->stitch_attr, sizeof(vi_stitch_attr), &stitch_attr_str, sizeof(vi_stitch_attr));
    (td_void)memcpy_s(stitch_attr, sizeof(vi_stitch_attr), &stitch_attr_str, sizeof(vi_stitch_attr));
    return TD_SUCCESS;
}
#else
td_s32 isp_drv_get_stitch_attr(ot_vi_pipe vi_pipe, vi_stitch_attr *stitch_attr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stitch_attr);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    (td_void)memset_s(&drv_ctx->stitch_attr, sizeof(vi_stitch_attr), 0, sizeof(vi_stitch_attr));
    (td_void)memset_s(stitch_attr, sizeof(vi_stitch_attr), 0, sizeof(vi_stitch_attr));
    return TD_SUCCESS;
}
#endif

td_s32 isp_get_bas_crop_attr(ot_vi_pipe vi_pipe, vi_blc_crop_info *bas_crop_attr)
{
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(bas_crop_attr);

    if (!ckfn_vi_get_blc_crop_info()) {
        return TD_FAILURE;
    }

    return call_vi_get_blc_crop_info(vi_pipe, bas_crop_attr);
}

static td_s32 isp_get_stagger_attr(ot_vi_pipe vi_pipe, isp_stagger_attr *stagger_attr)
{
    td_s32 ret;
    vi_out_mode_info out_mode_info;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(stagger_attr);
    if (is_virt_pipe(vi_pipe)) {
        stagger_attr->stagger_en       = TD_FALSE;
        stagger_attr->merge_frame_num  = 1;
        stagger_attr->crop_info.enable = TD_FALSE;
        return TD_SUCCESS;
    }

    if (!ckfn_vi_get_out_mode()) {
        isp_err_trace("pipe[%d] ckfn_vi_get_out_mode is null\n", vi_pipe);
        return TD_FAILURE;
    }

    ret = call_vi_get_out_mode(vi_pipe, &out_mode_info);
    if (ret != TD_SUCCESS) {
        isp_err_trace("pipe[%d] call_vi_get_out_mode failed 0x%x!\n", vi_pipe, ret);
        return ret;
    }

    if (out_mode_info.out_mode == OT_VI_OUT_MODE_2F1_STAGGER) {
        stagger_attr->stagger_en      = TD_TRUE;
        stagger_attr->merge_frame_num = 2; /* 2F1_STAGGER */
    } else if (out_mode_info.out_mode == OT_VI_OUT_MODE_3F1_STAGGER) {
        stagger_attr->stagger_en      = TD_TRUE;
        stagger_attr->merge_frame_num = 3; /* 3F1_STAGGER */
    } else if (out_mode_info.out_mode == OT_VI_OUT_MODE_4F1_STAGGER) {
        stagger_attr->stagger_en      = TD_TRUE;
        stagger_attr->merge_frame_num = 4; /* 4F1_STAGGER */
    } else {
        stagger_attr->stagger_en      = TD_FALSE;
        stagger_attr->merge_frame_num = 1;
    }
    (td_void)memcpy_s(&stagger_attr->crop_info, sizeof(ot_crop_info),
                      &out_mode_info.satgger_crop_info, sizeof(ot_crop_info));

    return TD_SUCCESS;
}
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
static td_s32 isp_drv_set_hnr_en(ot_vi_pipe vi_pipe, td_bool *hnr_en)
{
    return TD_SUCCESS;
}

td_s32 isp_drv_update_hnr_be_cfg(ot_vi_pipe vi_pipe, td_void *be_node,
    td_bool hnr_en, td_bool bnr_bypass, td_bool ref_none)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_node);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    drv_ctx->hnr_info.hnr_en = hnr_en;

    if ((isp_drv_get_alg_run_select(vi_pipe) == OT_ISP_ALG_RUN_FE_ONLY) && (drv_ctx->yuv_mode != TD_TRUE)) {
        return TD_SUCCESS;
    }

    if (drv_ctx->isp_init != TD_TRUE) {
        return TD_FAILURE;
    }
    isp_drv_set_hnr_bnr_cfg((isp_be_wo_reg_cfg *)be_node, drv_ctx, bnr_bypass, ref_none);

    return TD_SUCCESS;
}

td_s32 isp_drv_update_hnr_normal_blend(ot_vi_pipe vi_pipe, td_bool normal_blend)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->isp_init != TD_TRUE) {
        return TD_FAILURE;
    }

    drv_ctx->hnr_info.normal_blend = normal_blend;

    return TD_SUCCESS;
}
#endif

td_s32 isp_drv_get_pipe_size(ot_vi_pipe vi_pipe, ot_size *pipe_size)
{
    td_s32 ret;
    ot_size pipe_size_str = { 0 };

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(pipe_size);
    isp_check_vir_pipe_return(vi_pipe);

    if (!ckfn_vi_get_pipe_in_size()) {
        isp_err_trace("pipe[%d] ckfn_vi_get_pipe_bind_dev_size is null\n", vi_pipe);
        return TD_FAILURE;
    }

    ret = call_vi_get_pipe_in_size(vi_pipe, &pipe_size_str);
    if (ret != TD_SUCCESS) {
        isp_err_trace("pipe[%d] call_vi_get_pipe_bind_dev_size failed 0x%x!\n", vi_pipe, ret);
        return ret;
    }

    if ((pipe_size_str.width < OT_ISP_WIDTH_MIN) || (pipe_size_str.width > OT_ISP_SENSOR_WIDTH_MAX) ||
        (pipe_size_str.height < OT_ISP_HEIGHT_MIN) || (pipe_size_str.height > OT_ISP_SENSOR_HEIGHT_MAX)) {
        isp_err_trace("pipe[%d]: Image Width should between [%d, %d], Height should between[%d, %d]\n", vi_pipe,
            OT_ISP_WIDTH_MIN, OT_ISP_SENSOR_WIDTH_MAX, OT_ISP_HEIGHT_MIN, OT_ISP_SENSOR_HEIGHT_MAX);
        return TD_FAILURE;
    }

    (td_void)memcpy_s(pipe_size, sizeof(ot_size), &pipe_size_str, sizeof(ot_size));

    return TD_SUCCESS;
}

td_s32 isp_drv_get_hdr_attr(ot_vi_pipe vi_pipe, vi_pipe_hdr_attr *hdr_attr)
{
    td_s32 ret;
    vi_pipe_hdr_attr hdr_attr_str = { 0 };

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(hdr_attr);

    if (!ckfn_vi_get_pipe_hdr_attr()) {
        isp_err_trace("pipe[%d] ckfn_vi_get_pipe_hdr_attr is null\n", vi_pipe);
        return TD_FAILURE;
    }

    ret = call_vi_get_pipe_hdr_attr(vi_pipe, &hdr_attr_str);
    if (ret != TD_SUCCESS) {
        isp_err_trace("pipe[%d] call_vi_get_pipe_hdr_attr failed 0x%x!\n", vi_pipe, ret);
        return ret;
    }

    (td_void)memcpy_s(hdr_attr, sizeof(vi_pipe_hdr_attr), &hdr_attr_str, sizeof(vi_pipe_hdr_attr));

    return TD_SUCCESS;
}

td_s32 isp_drv_get_wdr_attr(ot_vi_pipe vi_pipe, vi_pipe_wdr_attr *wdr_attr)
{
    td_u32 i;
    td_u32 num;
    td_s32 ret;
    vi_pipe_wdr_attr wdr_attr_str = { 0 };
    vi_pipe_wdr_attr wdr_attr_str_tmp = { 0 };
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(wdr_attr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (!ckfn_vi_get_pipe_wdr_attr()) {
        isp_err_trace("pipe[%d] ckfn_vi_get_pipe_wdr_attr is null\n", vi_pipe);
        return TD_FAILURE;
    }

    ret = call_vi_get_pipe_wdr_attr(vi_pipe, &wdr_attr_str);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* Not WDR mode,BindPipe attr update */
    if (!is_fs_wdr_mode(wdr_attr_str.wdr_mode) && (wdr_attr_str.pipe_num != 1)) {
        wdr_attr_str.pipe_num = 1;
        wdr_attr_str.pipe_id[0] = vi_pipe;
    }

    num = wdr_attr_str.pipe_num;
    if ((num < 1) || (num > ISP_WDR_CHN_MAX)) {
        isp_err_trace("pipe[%d] Err wdr bind num %d!\n", vi_pipe, num);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    for (i = 0; i < num; i++) {
        isp_check_pipe_return(wdr_attr_str.pipe_id[i]);
    }
    if ((wdr_attr_str.wdr_mode > OT_WDR_MODE_BUTT)) {
        isp_err_trace("pipe[%d] Err wdr mode %d!\n", vi_pipe, wdr_attr_str.wdr_mode);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    (td_void)memcpy_s(&wdr_attr_str_tmp, sizeof(vi_pipe_wdr_attr), &wdr_attr_str, sizeof(vi_pipe_wdr_attr));

    for (i = 0; i < num; i++) {
        wdr_attr_str_tmp.pipe_id[i] = wdr_attr_str.pipe_id[num - 1 - i];
    }
    (td_void)memcpy_s(&drv_ctx->wdr_attr, sizeof(vi_pipe_wdr_attr), &wdr_attr_str_tmp, sizeof(vi_pipe_wdr_attr));
    (td_void)memcpy_s(wdr_attr, sizeof(vi_pipe_wdr_attr), &wdr_attr_str_tmp, sizeof(vi_pipe_wdr_attr));

    return TD_SUCCESS;
}

td_s32 isp_drv_chn_select_cfg(ot_vi_pipe vi_pipe, td_u32 chn_sel)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
        drv_ctx->chn_sel_attr[i].channel_sel = chn_sel;
    }

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        ret = isp_drv_chn_select_write(vi_pipe, chn_sel);
        if (ret != TD_SUCCESS) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("isp[%d] ChnSelect Write err!\n", vi_pipe);
            return ret;
        }
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return ret;
}

td_s32 isp_drv_set_isp_run_state(ot_vi_pipe vi_pipe, td_u64 *hand_signal)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(hand_signal);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    if (*hand_signal == ISP_INIT_HAND_SIGNAL) {
        drv_ctx->isp_run_flag = TD_TRUE;
    } else if (*hand_signal == ISP_EXIT_HAND_SIGNAL) {
        drv_ctx->isp_run_flag = TD_FALSE;
        isp_drv_reset_fe_cfg(vi_pipe);
    } else {
        isp_err_trace("ISP[%d] set isp run state failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void isp_drv_reset_stitch_ctx(isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    ot_vi_pipe stitch_pipe_id;
    isp_drv_ctx *stitch_drv_ctx = TD_NULL;

    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return;
    }
    if ((drv_ctx->stitch_attr.main_pipe != TD_TRUE)) {
        drv_ctx->stitch_attr.stitch_enable = TD_FALSE;
        return;
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        stitch_pipe_id = drv_ctx->stitch_attr.stitch_bind_id[i];
        if ((stitch_pipe_id < 0) || (stitch_pipe_id >= OT_ISP_MAX_PIPE_NUM)) {
            return;
        }
        stitch_drv_ctx = isp_drv_get_ctx(stitch_pipe_id);
        stitch_drv_ctx->stitch_sync = TD_FALSE;
    }

    drv_ctx->stitch_attr.stitch_enable = TD_FALSE;
    return;
}

static td_void isp_drv_reset_snap_ctx(isp_drv_ctx *drv_ctx)
{
#ifdef CONFIG_OT_SNAP_SUPPORT
    drv_ctx->snap_attr.snap_type = OT_SNAP_TYPE_NORM;
    drv_ctx->snap_attr.picture_pipe_id = -1;
    drv_ctx->snap_attr.preview_pipe_id = -1;
    drv_ctx->snap_attr.load_ccm = TD_TRUE;
    drv_ctx->snap_attr.pro_param.operation_mode = OT_OP_MODE_AUTO;
#else
    ot_unused(drv_ctx);
#endif
}

static td_void isp_drv_reset_pq_ai_ctx(isp_drv_ctx *drv_ctx)
{
    drv_ctx->ai_info.pq_ai_en = TD_FALSE;
    drv_ctx->ai_info.ai_pipe_id = -1;
    drv_ctx->ai_info.base_pipe_id = -1;
}

td_s32 isp_drv_reset_ctx(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->isp_run_flag == TD_TRUE) {
        isp_err_trace("ISP[%d] Should set isp_run_flag to TD_FALSE first!!\n", vi_pipe);
        return TD_FAILURE;
    }

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->frame_cnt = 0;
    drv_ctx->mem_init = TD_FALSE;
    drv_ctx->isp_init = TD_FALSE;
    drv_ctx->pub_attr_ok = TD_FALSE;
    drv_ctx->run_once_ok = TD_FALSE;
    drv_ctx->run_once_flag = TD_FALSE;
    drv_ctx->yuv_run_once_ok = TD_FALSE;
    drv_ctx->yuv_run_once_flag = TD_FALSE;
    drv_ctx->int_pos = 0;
    drv_ctx->fpn_work_mode = FPN_MODE_NONE;
    drv_ctx->hnr_info.hnr_en = TD_FALSE;
    drv_ctx->hnr_info.pre_hnr_en = TD_FALSE;
    drv_ctx->hnr_info.off_switch_cnt = 0;
    drv_ctx->hnr_info.normal_blend = TD_FALSE;

    isp_drv_reset_stitch_ctx(drv_ctx);

    for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
        drv_ctx->chn_sel_attr[i].channel_sel = 0;
    }

    isp_drv_reset_snap_ctx(drv_ctx);
    isp_drv_reset_pq_ai_ctx(drv_ctx);

    (td_void)memset_s(&drv_ctx->bnr_tpr_filt, sizeof(isp_kernel_tpr_filt_reg), 0, sizeof(isp_kernel_tpr_filt_reg));
    (td_void)memset_s(&drv_ctx->dyna_blc_info, sizeof(isp_dynamic_blc_info), 0, sizeof(isp_dynamic_blc_info));
    (td_void)memset_s(&drv_ctx->proc_frame_info, sizeof(isp_proc_frame_info), 0, sizeof(isp_proc_frame_info));

    drv_ctx->edge = TD_FALSE;
    drv_ctx->vd_start = TD_FALSE;
    drv_ctx->vd_end = TD_FALSE;
    drv_ctx->vd_be_end = TD_FALSE;

    g_int_bottom_half = TD_FALSE;
    g_use_bottom_half = TD_FALSE;
    g_quick_start = TD_FALSE;
    g_long_frm_int_en = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

td_s32 isp_set_pub_attr_info(ot_vi_pipe vi_pipe, ot_isp_pub_attr *pub_attr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(pub_attr);

    if ((pub_attr->wnd_rect.width < OT_ISP_WIDTH_MIN) || (pub_attr->wnd_rect.width > g_res_max_width[vi_pipe]) ||
        (pub_attr->wnd_rect.width % OT_ISP_ALIGN_WIDTH != 0)) {
        isp_err_trace("Invalid Image Width:%d!\n", pub_attr->wnd_rect.width);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((pub_attr->wnd_rect.height < OT_ISP_HEIGHT_MIN) || (pub_attr->wnd_rect.height > g_res_max_height[vi_pipe]) ||
        (pub_attr->wnd_rect.height % OT_ISP_ALIGN_HEIGHT != 0)) {
        isp_err_trace("Invalid Image Height:%d!\n", pub_attr->wnd_rect.height);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((pub_attr->sns_size.width < OT_ISP_WIDTH_MIN) || (pub_attr->sns_size.width > OT_ISP_SENSOR_WIDTH_MAX)) {
        isp_err_trace("Invalid Sensor Image Width:%d!\n", pub_attr->sns_size.width);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((pub_attr->sns_size.height < OT_ISP_HEIGHT_MIN) || (pub_attr->sns_size.height > OT_ISP_SENSOR_HEIGHT_MAX)) {
        isp_err_trace("Invalid Sensor Image Height:%d!\n", pub_attr->sns_size.height);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((pub_attr->wnd_rect.x < 0) || (pub_attr->wnd_rect.x > g_res_max_width[vi_pipe] - OT_ISP_WIDTH_MIN)) {
        isp_err_trace("Invalid Image X:%d!\n", pub_attr->wnd_rect.x);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if ((pub_attr->wnd_rect.y < 0) || (pub_attr->wnd_rect.y > g_res_max_height[vi_pipe] - OT_ISP_HEIGHT_MIN)) {
        isp_err_trace("Invalid Image Y:%d!\n", pub_attr->wnd_rect.y);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (pub_attr->bayer_format >= OT_ISP_BAYER_BUTT) {
        isp_err_trace("Invalid Bayer Pattern:%d!\n", pub_attr->bayer_format);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    if (pub_attr->wdr_mode >= OT_WDR_MODE_BUTT) {
        isp_err_trace("Invalid WDR mode %d!\n", pub_attr->wdr_mode);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    (td_void)memcpy_s(&drv_ctx->proc_pub_info, sizeof(ot_isp_pub_attr), pub_attr, sizeof(ot_isp_pub_attr));
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    drv_ctx->pub_attr_ok = TD_TRUE;

    return TD_SUCCESS;
}

static td_s32 isp_drv_alg_init_err_info_print(ot_vi_pipe vi_pipe, ot_isp_alg_mod alg_mod)
{
    isp_check_pipe_return(vi_pipe);
    if (alg_mod >= OT_ISP_ALG_MOD_BUTT) {
        isp_err_trace("Not Support this alg module!!\n");
        return TD_FAILURE;
    }

    isp_err_trace("ISP[%d] %s NOT init. Please check cmos parameters or mem malloc!\n",
                  vi_pipe, g_isp_alg_mod_name[alg_mod]);

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
static td_void isp_drv_be_stitch_sync_param_init(ot_vi_pipe vi_pipe, const isp_be_sync_para *be_sync_param,
    isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    ot_vi_pipe stitch_pipe;
    isp_drv_ctx *drv_ctx_stitch_pipe = TD_NULL;
    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return;
    }
    if (drv_ctx->stitch_attr.main_pipe != TD_TRUE) {
        return;
    }
    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        (td_void)memcpy_s(&drv_ctx->be_sync_para_stitch[i], sizeof(isp_be_sync_para), be_sync_param,
            sizeof(isp_be_sync_para));
        (td_void)memcpy_s(&drv_ctx->be_pre_sync_para_stitch[i], sizeof(isp_be_sync_para), be_sync_param,
            sizeof(isp_be_sync_para));
    }

    for (i = 1; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        stitch_pipe = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_stitch_pipe = isp_drv_get_ctx(stitch_pipe);
        (td_void)memcpy_s(&drv_ctx_stitch_pipe->be_sync_para_stitch[0], sizeof(isp_be_sync_para) * OT_ISP_MAX_PIPE_NUM,
            &drv_ctx->be_sync_para_stitch[0], sizeof(isp_be_sync_para) * OT_ISP_MAX_PIPE_NUM);
        (td_void)memcpy_s(&drv_ctx_stitch_pipe->be_pre_sync_para_stitch[0],
            sizeof(isp_be_sync_para) * OT_ISP_MAX_PIPE_NUM, &drv_ctx->be_pre_sync_para_stitch[0],
            sizeof(isp_be_sync_para) * OT_ISP_MAX_PIPE_NUM);
    }
}
#endif
static td_s32 isp_drv_be_sync_param_init(ot_vi_pipe vi_pipe, const isp_be_sync_para *be_sync_param)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_sync_param);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    (td_void)memcpy_s(&drv_ctx->be_sync_para, sizeof(isp_be_sync_para), be_sync_param, sizeof(isp_be_sync_para));
    (td_void)memcpy_s(&drv_ctx->be_pre_sync_para, sizeof(isp_be_sync_para), be_sync_param, sizeof(isp_be_sync_para));
    drv_ctx->dyna_blc_info.pre_black_level_mode = OT_ISP_BLACK_LEVEL_MODE_BUTT;
#ifdef CONFIG_OT_VI_STITCH_GRP
    isp_drv_be_stitch_sync_param_init(vi_pipe, be_sync_param, drv_ctx);
#endif
    return TD_SUCCESS;
}

static td_s32 isp_drv_get_dynamic_actual_info(ot_vi_pipe vi_pipe, isp_blc_actual_info *actual_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_vir_pipe_return(vi_pipe);
    isp_check_pointer_return(actual_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    (td_void)memcpy_s(actual_info, sizeof(isp_blc_actual_info),
                      &drv_ctx->dyna_blc_info.actual_info, sizeof(isp_blc_actual_info));
    return TD_SUCCESS;
}

static td_s32 isp_drv_set_fpn_work_mode_set(ot_vi_pipe vi_pipe, const td_u8 *work_mode)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    if ((*work_mode != FPN_MODE_CORRECTION) && (*work_mode != FPN_MODE_CALIBRATE)) {
        return TD_FAILURE;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    drv_ctx->fpn_work_mode = *work_mode;

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

    return TD_SUCCESS;
}

static td_s32 isp_ioctl_cmd_buf_init(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_TRANS_BUF_INIT:
            return isp_drv_trans_info_buf_init(vi_pipe, (isp_trans_info_buf *)ioctl_para->arg);

        case ISP_STAT_BUF_INIT:
            return isp_drv_stat_buf_init(vi_pipe, (td_phys_addr_t *)ioctl_para->arg);

        case ISP_BE_CFG_BUF_INIT:
            return isp_drv_be_buf_init(vi_pipe, (td_phys_addr_t *)ioctl_para->arg);

        case ISP_BE_ALL_BUF_INIT:
            return isp_drv_all_be_buf_init(vi_pipe);

        case ISP_CLUT_BUF_INIT:
            return isp_drv_clut_buf_init(vi_pipe);

        case ISP_FSWDR_BUF_INIT:
            return isp_drv_fswdr_buf_init(vi_pipe);

        case ISP_DRC_BUF_INIT:
            return isp_drv_drc_buf_init(vi_pipe);

        case ISP_LDCI_BUF_INIT:
            return isp_drv_ldci_buf_init(vi_pipe);

        case ISP_STT_BUF_INIT:
            return isp_drv_stt_buf_init(vi_pipe);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_buf_exit(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_TRANS_BUF_EXIT:
            return isp_drv_trans_info_buf_exit(vi_pipe);

        case ISP_STAT_BUF_EXIT:
            return isp_drv_stat_buf_exit(vi_pipe);

        case ISP_BE_CFG_BUF_EXIT:
            return isp_drv_be_buf_exit(vi_pipe);

        case ISP_CLUT_BUF_EXIT:
            return isp_drv_clut_buf_exit(vi_pipe);

        case ISP_FSWDR_BUF_EXIT:
            return isp_drv_fswdr_buf_exit(vi_pipe);

        case ISP_DRC_BUF_EXIT:
            return isp_drv_drc_buf_exit(vi_pipe);

        case ISP_LDCI_BUF_EXIT:
            return isp_drv_ldci_buf_exit(vi_pipe);

        case ISP_STT_BUF_EXIT:
            return isp_drv_stt_buf_exit(vi_pipe);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}


static td_s32 isp_ioctl_cmd_be_ctrl(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_BE_BUF_NUM_GET: {
            *(td_u8 *)ioctl_para->arg = g_be_buf_num[vi_pipe];
            return TD_SUCCESS;
        }

        case ISP_GET_BE_BUF_FIRST:
            return isp_drv_get_be_buf_first(vi_pipe, (td_phys_addr_t *)ioctl_para->arg);

        case ISP_BE_FREE_BUF_GET:
            return isp_drv_get_be_free_buf(vi_pipe, (isp_be_wo_cfg_buf *)ioctl_para->arg);

        case ISP_BE_LAST_BUF_GET:
            return isp_drv_get_be_last_buf(vi_pipe, (td_phys_addr_t *)ioctl_para->arg);

        case ISP_BE_CFG_BUF_RUNNING:
            return isp_drv_be_buf_run_state(vi_pipe);

        case ISP_BE_CFG_BUF_CTL:
            return isp_drv_be_buf_ctl(vi_pipe);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_mod_ctrl(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_SET_MOD_PARAM:
            return isp_set_mod_param((ot_isp_mod_param *)ioctl_para->arg);

        case ISP_GET_MOD_PARAM:
            return isp_get_mod_param((ot_isp_mod_param *)ioctl_para->arg);

        case ISP_SET_CTRL_PARAM:
            return isp_set_ctrl_param(vi_pipe, (ot_isp_ctrl_param *)ioctl_para->arg);

        case ISP_GET_CTRL_PARAM:
            return isp_get_ctrl_param(vi_pipe, (ot_isp_ctrl_param *)ioctl_para->arg);

        case ISP_ALG_INIT_ERR_INFO_PRINT:
            return isp_drv_alg_init_err_info_print(vi_pipe, *(ot_isp_alg_mod *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

#ifdef CONFIG_OT_ISP_PQ_FOR_AI_SUPPORT
static td_s32 isp_ioctl_cmd_pq_for_ai(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_PQ_AI_GROUP_ATTR_SET:
            return isp_set_pq_ai_attr((pq_ai_attr *)ioctl_para->arg);

        case ISP_PQ_AI_GROUP_ATTR_GET:
            return isp_get_pq_ai_attr(vi_pipe, (pq_ai_attr *)ioctl_para->arg);

        case ISP_PQ_AI_POST_NR_ATTR_SET:
            return isp_set_pq_ai_post_nr_attr(vi_pipe, (ot_pq_ai_noiseness_post_attr *)ioctl_para->arg);

        case ISP_PQ_AI_POST_NR_ATTR_GET:
            return isp_get_pq_ai_post_nr_attr(vi_pipe, (ot_pq_ai_noiseness_post_attr *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}
#endif

static td_s32 isp_ioctl_cmd_proc(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_PROC_INIT:
            return isp_drv_proc_init(vi_pipe, (isp_proc_mem *)ioctl_para->arg);

        case ISP_PROC_WRITE_ING: {
            if (osal_down_interruptible(&g_isp_drv_ctx[vi_pipe].proc_sem)) {
                return -ERESTARTSYS;
            }
            return TD_SUCCESS;
        }

        case ISP_PROC_WRITE_OK: {
            osal_up(&g_isp_drv_ctx[vi_pipe].proc_sem);
            return TD_SUCCESS;
        }

        case ISP_PROC_EXIT:
            return isp_drv_proc_exit(vi_pipe);

        case ISP_PROC_PARAM_GET: {
            *(td_u32 *)ioctl_para->arg = g_proc_param[vi_pipe];
            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

#ifdef CONFIG_OT_SNAP_SUPPORT
static td_s32 isp_ioctl_cmd_snap(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_PRO_TRIGGER_GET: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            *(td_bool *)ioctl_para->arg = drv_ctx->pro_enable;

            if (drv_ctx->pro_enable == TD_TRUE) {
                drv_ctx->pro_enable = TD_FALSE;
                drv_ctx->pro_trig_flag = 1;
            }

            return TD_SUCCESS;
        }

        case ISP_SNAP_ATTR_GET: {
            isp_snap_attr *snap_attr = TD_NULL;
            isp_drv_ctx *drv_ctx = TD_NULL;

            snap_attr = (isp_snap_attr *)ioctl_para->arg;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            (td_void)memcpy_s(snap_attr, sizeof(isp_snap_attr), &drv_ctx->snap_attr, sizeof(isp_snap_attr));

            return TD_SUCCESS;
        }

        case ISP_CONFIG_INFO_SET:
            return isp_drv_set_config_info(vi_pipe, (ot_isp_config_info *)ioctl_para->arg);

        case ISP_SNAP_ATTR_SET:
            return isp_set_snap_attr(vi_pipe, (isp_snap_attr*)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}
#endif

static td_s32 isp_ioctl_cmd_stats(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_STAT_BUF_GET:
            return isp_drv_get_user_stat_buf(vi_pipe, (isp_stat_info *)ioctl_para->arg);

        case ISP_STAT_BUF_PUT:
            return isp_drv_stat_buf_user_put(vi_pipe, (isp_stat_info *)ioctl_para->arg);

        case ISP_STAT_ACT_GET:
            return isp_drv_get_stat_info_active(vi_pipe, (isp_stat_info *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_work_mode(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_WORK_MODE_INIT:
            return isp_drv_work_mode_init(vi_pipe, (isp_block_attr *)ioctl_para->arg);

        case ISP_WORK_MODE_EXIT: {
            return isp_drv_work_mode_exit(vi_pipe);
        }

        case ISP_WORK_MODE_GET: {
            isp_drv_ctx *drv_ctx = isp_drv_get_ctx(vi_pipe);
            (td_void)memcpy_s((isp_working_mode *)ioctl_para->arg, sizeof(isp_working_mode),
                &drv_ctx->work_mode, sizeof(isp_working_mode));
            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_stt_init(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_LDCI_READ_STT_BUF_GET:
            return isp_drv_ldci_read_stt_buf_get(vi_pipe, (isp_ldci_read_stt_buf *)ioctl_para->arg);

        case ISP_STT_ADDR_INIT: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            isp_check_vir_pipe_return(vi_pipe);

            drv_ctx = isp_drv_get_ctx(vi_pipe);
            if (drv_ctx->stitch_attr.stitch_enable == TD_FALSE) {
                return isp_drv_fe_stt_addr_init(vi_pipe);
            } else {
#ifdef CONFIG_OT_VI_STITCH_GRP
                return isp_drv_fe_stitch_stt_addr_init(vi_pipe);
#endif
            }
        }

        case ISP_BE_LUT_STT_BUF_GET: {
            td_phys_addr_t phy_addr;
            if (g_isp_drv_ctx[vi_pipe].be_lut2stt_attr.init == TD_FALSE) {
                return OT_ERR_ISP_NOMEM;
            }
            phy_addr = g_isp_drv_ctx[vi_pipe].be_lut2stt_attr.be_lut_stt_buf[0].lut_stt_buf[0].phy_addr;
            *(td_phys_addr_t *)ioctl_para->arg = phy_addr;

            return TD_SUCCESS;
        }

        case ISP_BE_PRE_LUT_STT_BUF_GET: {
            td_phys_addr_t phy_addr;
            if (g_isp_drv_ctx[vi_pipe].be_pre_on_post_off_lut2stt_attr.init == TD_FALSE) {
                return OT_ERR_ISP_NOMEM;
            }
            phy_addr = g_isp_drv_ctx[vi_pipe].be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[0].lut_stt_buf[0].phy_addr;
            *(td_phys_addr_t *)ioctl_para->arg = phy_addr;

            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_sync_ctrl(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_SYNC_CFG_SET:
            return isp_drv_sync_cfg_set(vi_pipe, (isp_sync_cfg_buf_node *)ioctl_para->arg);

        case ISP_BE_SYNC_PARAM_INIT: {
            return isp_drv_be_sync_param_init(vi_pipe, (isp_be_sync_para *)ioctl_para->arg);
        }

        case ISP_SYNC_INIT_SET: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            isp_check_bool_return(*(td_bool *)ioctl_para->arg);
            drv_ctx = isp_drv_get_ctx(vi_pipe);

            drv_ctx->stitch_sync = TD_FALSE;
            if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
                drv_ctx->stitch_sync = *(td_bool *)ioctl_para->arg;
            }

            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_vd(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_GET_FRAME_EDGE:
            return isp_get_frame_edge(vi_pipe, (td_u32 *)ioctl_para->arg);

        case ISP_GET_VD_TIMEOUT: {
            isp_vd_timeout isp_vd_time_out;
            isp_vd_timeout *vd_time_out = TD_NULL;

            vd_time_out = (isp_vd_timeout *)ioctl_para->arg;
            (td_void)memcpy_s(&isp_vd_time_out, sizeof(isp_vd_timeout), vd_time_out, sizeof(isp_vd_timeout));
            isp_get_vd_start_time_out(vi_pipe, isp_vd_time_out.milli_sec, &isp_vd_time_out.int_status);
            (td_void)memcpy_s(vd_time_out, sizeof(isp_vd_timeout), &isp_vd_time_out, sizeof(isp_vd_timeout));

            return TD_SUCCESS;
        }

        case ISP_GET_VD_END_TIMEOUT: {
            isp_vd_timeout isp_vd_time_out;
            isp_vd_timeout *vd_time_out = TD_NULL;

            vd_time_out = (isp_vd_timeout *)ioctl_para->arg;
            (td_void)memcpy_s(&isp_vd_time_out, sizeof(isp_vd_timeout), vd_time_out, sizeof(isp_vd_timeout));
            isp_get_vd_end_time_out(vi_pipe, isp_vd_time_out.milli_sec, &isp_vd_time_out.int_status);
            (td_void)memcpy_s(vd_time_out, sizeof(isp_vd_timeout), &isp_vd_time_out, sizeof(isp_vd_timeout));

            return TD_SUCCESS;
        }

        case ISP_GET_VD_BEEND_TIMEOUT: {
            isp_vd_timeout isp_vd_time_out;
            isp_vd_timeout *vd_time_out = TD_NULL;

            vd_time_out = (isp_vd_timeout *)ioctl_para->arg;
            (td_void)memcpy_s(&isp_vd_time_out, sizeof(isp_vd_timeout), vd_time_out, sizeof(isp_vd_timeout));
            isp_get_vd_be_end_time_out(vi_pipe, isp_vd_time_out.milli_sec, &isp_vd_time_out.int_status);
            (td_void)memcpy_s(vd_time_out, sizeof(isp_vd_timeout), &isp_vd_time_out, sizeof(isp_vd_timeout));

            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_info(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_UPDATE_INFO_SET: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            ot_isp_dcf_update_info *isp_update_info = TD_NULL;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            isp_update_info = (ot_isp_dcf_update_info *)ioctl_para->arg;
            (td_void)memcpy_s(&drv_ctx->update_info, sizeof(ot_isp_dcf_update_info), isp_update_info,
                sizeof(ot_isp_dcf_update_info));
            return TD_SUCCESS;
        }

        case ISP_FRAME_INFO_SET: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            ot_isp_frame_info *isp_frame_info = TD_NULL;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            isp_frame_info = (ot_isp_frame_info *)ioctl_para->arg;
            (td_void)memcpy_s(&drv_ctx->frame_info, sizeof(ot_isp_frame_info), isp_frame_info,
                sizeof(ot_isp_frame_info));
            return TD_SUCCESS;
        }

        case ISP_FRAME_INFO_GET:
            return isp_get_frame_info(vi_pipe, (ot_isp_frame_info *)ioctl_para->arg);

        case ISP_DNG_INFO_SET:
            return isp_drv_set_dng_info(vi_pipe, (ot_dng_image_dynamic_info *)ioctl_para->arg);

        case ISP_DNG_INFO_GET:
            return isp_drv_get_dng_info(vi_pipe, (ot_isp_dng_image_static_info *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_mem_init(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_SET_INT_ENABLE: {
            td_bool en;
            en = *(td_bool *)ioctl_para->arg;

            return isp_drv_set_int_enable(vi_pipe, en);
        }

        case ISP_MEM_INFO_SET: {
            isp_check_bool_return(*(td_bool *)ioctl_para->arg);
            g_isp_drv_ctx[vi_pipe].mem_init = TD_TRUE;

            return TD_SUCCESS;
        }

        case ISP_MEM_INFO_GET: {
            *(td_bool *)ioctl_para->arg = g_isp_drv_ctx[vi_pipe].mem_init;

            return TD_SUCCESS;
        }

        case ISP_INIT_INFO_SET: {
            isp_check_bool_return(*(td_bool *)ioctl_para->arg);
            g_isp_drv_ctx[vi_pipe].isp_init = *(td_bool *)ioctl_para->arg;

            return TD_SUCCESS;
        }

        case ISP_RUN_STATE_SET:
            return isp_drv_set_isp_run_state(vi_pipe, (td_u64 *)ioctl_para->arg);

        case ISP_GET_WDR_ATTR:
            return isp_drv_get_wdr_attr(vi_pipe, (vi_pipe_wdr_attr *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_mem_init_update(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_PRE_BLK_NUM_UPDATE: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            drv_ctx->work_mode.pre_block_num = *(td_u8 *)ioctl_para->arg;

            return TD_SUCCESS;
        }

        case ISP_GET_PIPE_SIZE:
            return isp_drv_get_pipe_size(vi_pipe, (ot_size *)ioctl_para->arg);

        case ISP_GET_HDR_ATTR:
            return isp_drv_get_hdr_attr(vi_pipe, (vi_pipe_hdr_attr *)ioctl_para->arg);

        case ISP_P2EN_INFO_GET:
            return isp_drv_get_p2_en_info(vi_pipe, (td_bool *)ioctl_para->arg);

        case ISP_GET_STITCH_ATTR:
            return isp_drv_get_stitch_attr(vi_pipe, (vi_stitch_attr *)ioctl_para->arg);

        case ISP_GET_VERSION:
            return isp_drv_get_version((isp_version *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_mode_switch(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_RES_SWITCH_SET: {
            isp_drv_be_buf_switch_state(vi_pipe);
            return TD_SUCCESS;
        }

        case ISP_BE_SWITCH_FINISH_STATE_SET:
            return isp_drv_be_buf_switch_finish_state(vi_pipe);

        case ISP_WDR_CFG_SET:
            return isp_drv_set_wdr_cfg(vi_pipe, (isp_wdr_cfg *)ioctl_para->arg);

        case ISP_SYNC_STITCH_PARAM_INIT: {
#ifdef CONFIG_OT_VI_STITCH_GRP
            return isp_drv_stitch_sync_ctrl_init(vi_pipe);
#else
            return TD_FAILURE;
#endif
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_run_once(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_OPT_RUNONCE_INFO:
            return isp_drv_opt_run_once_info(vi_pipe, (td_bool *)ioctl_para->arg);

        case ISP_YUV_RUNONCE_INFO:
            return isp_drv_yuv_run_once_info(vi_pipe, (td_bool *)ioctl_para->arg);

        case ISP_KERNEL_RUNONCE:
            return isp_drv_run_once_process(vi_pipe);

        case ISP_KERNEL_YUV_RUNONCE:
            return isp_drv_yuv_run_once_process(vi_pipe);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_run_blc(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_GET_BAS_CROP_ATTR:
            return isp_get_bas_crop_attr(vi_pipe, (vi_blc_crop_info *)ioctl_para->arg);

        case ISP_STAGGER_ATTR_GET:
            return isp_get_stagger_attr(vi_pipe, (isp_stagger_attr *)ioctl_para->arg);

#ifdef CONFIG_OT_ISP_HNR_SUPPORT
        case ISP_HNR_EN_SET:
            return isp_drv_set_hnr_en(vi_pipe, (td_bool *)ioctl_para->arg);
#endif
        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_run_alg(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_REG_CFG_SET:
            return isp_drv_set_reg_kernel_cfgs(vi_pipe, (isp_kernel_reg_cfg *)ioctl_para->arg);

        case ISP_CHN_SELECT_CFG:
            return isp_drv_chn_select_cfg(vi_pipe, *(td_u32 *)ioctl_para->arg);

        case ISP_MCF_EN_GET: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            td_bool *nr_en = TD_NULL;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            nr_en = (td_bool *)ioctl_para->arg;
            isp_drv_get_nr_mode(vi_pipe, nr_en);
            drv_ctx->work_mode.is_ia_nr_enable = *nr_en;
            return TD_SUCCESS;
        }

        case ISP_BNR_TEMPORAL_FILT_CFG_SET:
            return isp_drv_bnr_temporal_filt_set(vi_pipe, (isp_bnr_temporal_filt *)ioctl_para->arg);

        case ISP_MOT_CFG_SET:
            return isp_drv_sharpen_mot_set(vi_pipe, (td_bool *)ioctl_para->arg);

        case ISP_CLUT_BUF_GET: {
            if (g_isp_drv_ctx[vi_pipe].clut_buf_attr.init == TD_FALSE) {
                return OT_ERR_ISP_NOMEM;
            }
            *(td_phys_addr_t *)ioctl_para->arg = g_isp_drv_ctx[vi_pipe].clut_buf_attr.clut_buf.phy_addr;

            return TD_SUCCESS;
        }

        case ISP_SET_PROCALCDONE: {
            g_isp_drv_ctx[vi_pipe].pro_start = TD_TRUE;

            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_mpi_ctrl(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_SET_RGBIR_FORMAT: {
            g_isp_drv_ctx[vi_pipe].isp_rgbir_format = *(td_u32 *)ioctl_para->arg;
            return TD_SUCCESS;
        }

        case ISP_UPDATE_POS_GET: {
            *(td_u32 *)ioctl_para->arg = g_update_pos[vi_pipe];

            return TD_SUCCESS;
        }

        case ISP_FRAME_CNT_GET: {
            isp_drv_ctx *drv_ctx = TD_NULL;
            drv_ctx = isp_drv_get_ctx(vi_pipe);
            *(td_u32 *)ioctl_para->arg = drv_ctx->frame_cnt;

            return TD_SUCCESS;
        }

        case ISP_PUB_ATTR_INFO:
            return isp_set_pub_attr_info(vi_pipe, (ot_isp_pub_attr *)ioctl_para->arg);

        case ISP_RESET_CTX:
            return isp_drv_reset_ctx(vi_pipe);

        case ISP_DYNAMIC_ACTUAL_INFO_GET:
            return isp_drv_get_dynamic_actual_info(vi_pipe, (isp_blc_actual_info *)ioctl_para->arg);

        case ISP_FPN_WORK_MODE_SET:
            return isp_drv_set_fpn_work_mode_set(vi_pipe, (td_u8 *)ioctl_para->arg);

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}


static td_s32 isp_ioctl_cmd_dev_init(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    ot_vi_pipe vi_pipe = isp_get_dev(ioctl_para->private_data);
    isp_check_pipe_return(vi_pipe);

    *find_flag = TD_TRUE;

    switch (ioctl_para->cmd) {
        case ISP_DEV_SET_FD: {
            *((td_u32 *)(ioctl_para->private_data)) = *(td_u32 *)ioctl_para->arg;

            return TD_SUCCESS;
        }

        case ISP_PWM_NUM_GET: {
            *(td_u32 *)ioctl_para->arg = g_pwm_number[vi_pipe];

            return TD_SUCCESS;
        }

        default:
            *find_flag = TD_FALSE;
            break;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}


static td_s32 isp_ioctl_cmd_init(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    td_s32 ret;
    ret = isp_ioctl_cmd_mem_init(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_mem_init_update(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_buf_init(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_stt_init(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_dev_init(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_blc_ctrl(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    td_s32 ret;

    ret = isp_ioctl_cmd_run_blc(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}


static td_s32 isp_ioctl_cmd_running_ctrl(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    td_s32 ret;
    ret = isp_ioctl_cmd_run_once(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_run_alg(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_sync_ctrl(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_mode_switch(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_mod_ctrl(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_be_ctrl(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_buf_exit(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

#ifdef CONFIG_OT_SNAP_SUPPORT
    ret = isp_ioctl_cmd_snap(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }
#endif

#ifdef CONFIG_OT_ISP_PQ_FOR_AI_SUPPORT
    ret = isp_ioctl_cmd_pq_for_ai(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }
#endif

    ret = isp_ioctl_cmd_mpi_ctrl(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_ioctl_cmd_state(isp_ioctl_para *ioctl_para, td_bool *find_flag)
{
    td_s32 ret;
    ret = isp_ioctl_cmd_work_mode(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_stats(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_info(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_vd(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_proc(ioctl_para, find_flag);
    if (*find_flag == TD_TRUE) {
        return ret;
    }

    return OT_ERR_ISP_ILLEGAL_PARAM;
}

static td_s32 isp_device_ioctl(isp_ioctl_para *ioctl_para)
{
    td_s32 ret;
    td_bool find_flag = TD_FALSE;

    ret = isp_ioctl_cmd_init(ioctl_para, &find_flag);
    if (find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_running_ctrl(ioctl_para, &find_flag);
    if (find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_blc_ctrl(ioctl_para, &find_flag);
    if (find_flag == TD_TRUE) {
        return ret;
    }

    ret = isp_ioctl_cmd_state(ioctl_para, &find_flag);
    if (find_flag == TD_TRUE) {
        return ret;
    }

    return vreg_drv_ioctl(ioctl_para->cmd, ioctl_para->arg, ioctl_para->private_data);
}

static long isp_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
    int ret;
    isp_ioctl_para ioctl_para = { 0 };

    ioctl_para.cmd = cmd;
    ioctl_para.arg = (td_uintptr_t)arg;
    ioctl_para.private_data = private_data;
    ret = isp_device_ioctl(&ioctl_para);
    return ret;
}

#ifdef CONFIG_COMPAT
static long isp_compat_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
    switch (cmd) {
        case ISP_BE_CFG_BUF_INIT: {
            isp_be_wo_cfg_buf *be_wo_cfg_buf = (isp_be_wo_cfg_buf *)(td_uintptr_t)arg;
            OT_COMPAT_POINTER(be_wo_cfg_buf->vir_addr, td_void *);
            break;
        }

        default: {
            break;
        }
    }

    return isp_ioctl(cmd, arg, private_data);
}
#endif

static int isp_open(void *data)
{
    ot_unused(data);
    return 0;
}

static int isp_close(void *data)
{
    ot_unused(data);
    return 0;
}

static struct osal_fileops g_isp_file_ops = {
    .unlocked_ioctl = isp_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = isp_compat_ioctl,
#endif
    .open = isp_open,
    .release = isp_close
};

static osal_dev_t *g_isp_device = TD_NULL;

#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
static void isp_drv_work_queue_handler(struct osal_work_struct *worker)
{
    td_s32 ret;
    isp_work_queue_ctx *isp_work_queue = osal_container_of((void *)worker, isp_work_queue_ctx, worker);
    if (osal_down_interruptible(&isp_work_queue->sem)) {
        return;
    }
    ret = isp_drv_int_bottom_half(0);
    if (ret != OSAL_IRQ_HANDLED) {
    }
    osal_up(&isp_work_queue->sem);

    return;
}

void isp_drv_work_queue_run(ot_vi_pipe vi_pipe)
{
    osal_schedule_work(&g_isp_work_queue_ctx.worker);
}

void isp_drv_work_queue_init(void)
{
    osal_sema_init(&g_isp_work_queue_ctx.sem, 1);
    OSAL_INIT_WORK(&g_isp_work_queue_ctx.worker, isp_drv_work_queue_handler);
}

void isp_drv_work_queue_exit(void)
{
    osal_sema_destroy(&g_isp_work_queue_ctx.sem);
    osal_destroy_work(&g_isp_work_queue_ctx.worker);
}
#endif

static td_void isp_update_interrupt_info(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_interrupt_sch *int_sch)
{
    td_u8 write_flag;
    unsigned long flags = 0;

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
    if (isp_interrupt_buf_is_full(&drv_ctx->isp_int_info)) {
        isp_warn_trace("ISP[%d] interrupts buf is full!\n", vi_pipe);
        (td_void)memset_s(&drv_ctx->isp_int_info, sizeof(isp_interrupt_info), 0, sizeof(isp_interrupt_info));
    }

    (td_void)memset_s(&drv_ctx->isp_int_info.int_sch[drv_ctx->isp_int_info.write_flag], sizeof(isp_interrupt_sch), 0,
        sizeof(isp_interrupt_sch));

    if (int_sch->isp_int_status || int_sch->port_int_status ||
        ((int_sch->wch_int_status != 0) && (drv_ctx->yuv_mode == TD_TRUE))) {
        write_flag = drv_ctx->isp_int_info.write_flag;
        (td_void)memcpy_s(&drv_ctx->isp_int_info.int_sch[write_flag], sizeof(isp_interrupt_sch), int_sch,
            sizeof(isp_interrupt_sch));
        drv_ctx->isp_int_info.write_flag = (write_flag + 1) % ISP_INTERRUPTS_SAVEINFO_MAX;
    }

    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
}

static td_void isp_bottom_half_cross_frame(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    unsigned long flags = 0;
    td_s8 w_flag_num = 0;
    td_s8 w_flag_end = 0;
    td_u8 r_flag_num = 0;
    td_u8 count = 0;
    td_s8 cur_write_flag = 0;

    osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

    cur_write_flag = drv_ctx->isp_int_info.write_flag == 0 ? ISP_INTERRUPTS_SAVEINFO_MAX - 1
        : drv_ctx->isp_int_info.write_flag;

    r_flag_num = drv_ctx->isp_int_info.read_flag;

    if (r_flag_num == drv_ctx->isp_int_info.write_flag) {
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
        return;
    } else {
        while (r_flag_num != cur_write_flag && count < ISP_INTERRUPTS_SAVEINFO_MAX) {
            count++;
            r_flag_num = (r_flag_num + 1) % ISP_INTERRUPTS_SAVEINFO_MAX;
        }
    }

    if (count >= CROSS_FRAME_FLAG) {
        w_flag_num = cur_write_flag;

        if ((drv_ctx->isp_int_info.int_sch[w_flag_num].isp_int_status & ISP_1ST_INT) == 1) {
            drv_ctx->bottom_half_cross_frame = TD_TRUE;
        } else {
            while (((drv_ctx->isp_int_info.int_sch[w_flag_num].isp_int_status & ISP_1ST_INT) != 1)
                && (w_flag_end < ISP_INTERRUPTS_SAVEINFO_MAX)) {
                w_flag_num = w_flag_num - 1 < 0 ? ISP_INTERRUPTS_SAVEINFO_MAX - 1 : w_flag_num - 1;
                w_flag_end++;
            }
        }
        drv_ctx->isp_int_info.read_flag = (td_u8)w_flag_num;
    }
    osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
}

static td_s32 isp_get_interrupt_info(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 *int_read_num)
{
    td_u8 read_flag;
    unsigned long flags = 0;
    isp_interrupt_sch *int_sch = TD_NULL;

    isp_bottom_half_cross_frame(vi_pipe, drv_ctx);

    while (drv_ctx->isp_int_info.read_flag != drv_ctx->isp_int_info.write_flag) {
        osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
        read_flag = drv_ctx->isp_int_info.read_flag;
        int_sch = &drv_ctx->isp_int_info.int_sch[read_flag];

        if ((!int_sch->isp_int_status) && (!int_sch->port_int_status) &&
            (!(int_sch->wch_int_status && (drv_ctx->yuv_mode == TD_TRUE)))) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            return 0;
        }

        if (isp_interrupt_buf_is_empty(&drv_ctx->isp_int_info)) {
            isp_warn_trace("ISP[%d] interrupts buf is empty\n", vi_pipe);
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            return 0;
        }

        (td_void)memcpy_s(&drv_ctx->int_sch[*int_read_num], sizeof(isp_interrupt_sch),
            &drv_ctx->isp_int_info.int_sch[read_flag], sizeof(isp_interrupt_sch));
        (td_void)memset_s(&drv_ctx->isp_int_info.int_sch[read_flag], sizeof(isp_interrupt_sch), 0,
            sizeof(isp_interrupt_sch));
        drv_ctx->isp_int_info.read_flag = (read_flag + 1) % ISP_INTERRUPTS_SAVEINFO_MAX;
        (*int_read_num)++;
        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    }

    return 1;
}

td_s32 isp_drv_int_status_process(ot_vi_pipe vi_pipe, td_s32 vi_dev, isp_drv_ctx *drv_ctx, td_u32 int_status)
{
    td_u32 port_int, port_int_f_start, port_int_err, isp_raw_int, isp_int_status, wch_int, wch_int_f_start;
    td_u32 be_pre_int;
    isp_interrupt_sch int_sch = { 0 };
    /* read interrupt status */
    port_int = io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT);
    port_int &= io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT_MASK);
    port_int_f_start = port_int & VI_PT_INT_FSTART;
    port_int_err = port_int & VI_PT_INT_ERR;
    if (g_mask_is_open[vi_pipe] == TD_TRUE) {
        wch_int = io_rw_ch_address(vi_pipe, VI_WCH_INT);
        wch_int_f_start = wch_int & VI_WCH_INT_FSTART;
    } else {
        wch_int_f_start = 0;
    }

    isp_raw_int = io_rw_fe_address(vi_pipe, ISP_INT_FE);

    isp_int_status = isp_raw_int & io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK);

    int_sch.isp_int_status = isp_int_status;

    if (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE) {
        be_pre_int = isp_drv_get_pre_proc_fstart_status(vi_dev);
        int_sch.isp_int_status |= be_pre_int; /* when in this mode, only use be_pre frame start interrupt */
                                              /* replace fe frame start interrupt */
        if (be_pre_int) {
            isp_drv_clear_pre_proc_fstart_int(vi_dev);
        }
    } else {
        if (isp_is_chn_int(vi_pipe, int_status)) {
            isp_int_status |= wch_int_f_start;
        }
        int_sch.isp_int_status = isp_int_status;
    }

    int_sch.port_int_status = port_int_f_start;
    int_sch.port_int_err = port_int_err;
    int_sch.wch_int_status = wch_int_f_start;

    isp_update_interrupt_info(vi_pipe, drv_ctx, &int_sch);

    /* clear interrupt */
    if (!port_int && !isp_raw_int) {
        return TD_FAILURE;
    }
    if (port_int) {
        io_rw_pt_address(vi_pt_base(vi_dev) + VI_PT_INT) = port_int;
    }

    if (isp_raw_int) {
        io_rw_fe_address(vi_pipe, ISP_INT_FE) = isp_raw_int;
    }

    if (port_int_err) {
        drv_ctx->drv_dbg_info.isp_reset_cnt++;
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

int isp_isr(int irq, void *id, td_u32 int_status)
{
    td_u32 i;
    td_s32 vi_dev, ret;
    ot_vi_pipe vi_pipe;

    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_unused(id);

    /* Isp FE Interrupt Process Begin */
    for (i = 0; i < OT_ISP_MAX_PHY_PIPE_NUM; i++) {
        vi_pipe = i;

        drv_ctx = isp_drv_get_ctx(vi_pipe);
        if (drv_ctx->mem_init == TD_FALSE) {
            continue;
        }
        vi_dev = drv_ctx->wdr_attr.vi_dev;

        ret = isp_drv_int_status_process(vi_pipe, vi_dev, drv_ctx, int_status);
        if (ret != TD_SUCCESS) {
            continue;
        }
    }

    if (!g_use_bottom_half) {
        isp_drv_int_bottom_half(irq);
    } else {
#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
#ifdef __LITEOS__
        isp_drv_work_queue_run(vi_pipe);
#endif
#endif
    }

    return OSAL_IRQ_WAKE_THREAD;
}

int isp_int_bottom_half(int irq, void *id)
{
    ot_unused(id);
    if (g_use_bottom_half) {
        return isp_drv_int_bottom_half(irq);
    } else {
        return OSAL_IRQ_HANDLED;
    }
}

td_void isp_drv_wake_up_thread(ot_vi_pipe vi_pipe)
{
    td_bool wake_up_tread = TD_TRUE;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode) ||
        is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
            if (drv_ctx->running_state != ISP_BE_BUF_STATE_INIT) {
                wake_up_tread = TD_FALSE;
            }
        }
    }

    if (wake_up_tread == TD_TRUE) {
        drv_ctx->edge = TD_TRUE;
        drv_ctx->vd_start = TD_TRUE;

        osal_wakeup(&drv_ctx->isp_wait);
        osal_wakeup(&drv_ctx->isp_wait_vd_start);
    }

    return;
}

td_s32 isp_drv_get_use_node_yuv_mode(ot_vi_pipe vi_pipe, td_u32 isp_int_status, td_u32 wch_int_status)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg *sync_cfg = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    sync_cfg = &drv_ctx->sync_cfg;

    if (!is_offline_mode(drv_ctx->work_mode.running_mode) && !is_striping_mode(drv_ctx->work_mode.running_mode) &&
        !is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        return TD_SUCCESS;
    }

    if ((drv_ctx->yuv_mode == TD_TRUE) && wch_int_status && (drv_ctx->isp_run_flag == TD_TRUE)) {
        if (drv_ctx->use_node) {
            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

            ret = isp_drv_get_sync_controlnfo(vi_pipe, sync_cfg);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_get_sync_controlnfo");
            ret = isp_drv_calc_sync_cfg(sync_cfg);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_calc_sync_cfg");
            ret = isp_drv_reg_config_isp(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_isp");

            drv_ctx->status = isp_int_status;

            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

            if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
                isp_drv_stat_buf_busy_put(vi_pipe);
            }

            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
            isp_drv_wake_up_thread(vi_pipe);
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_sync_task_process(vi_pipe);

            return TD_FAILURE;
        }

        osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
        drv_ctx->use_node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);
        if (drv_ctx->use_node == TD_NULL) {
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("Pipe[%d] get FreeBeBuf is fail!\r\n", vi_pipe);

            return TD_FAILURE;
        }

        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_use_node_raw_mode(ot_vi_pipe vi_pipe, td_u32 isp_int_status)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg *sync_cfg = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    sync_cfg = &drv_ctx->sync_cfg;

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        return TD_SUCCESS;
    }

    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    if (isp_int_status & ISP_1ST_INT) {
        if (drv_ctx->use_node != TD_NULL) {
            /* Need to configure the sensor registers and get statistics for AE/AWB. */
            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

            ret = isp_drv_get_sync_controlnfo(vi_pipe, sync_cfg);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_get_sync_controlnfo");
            ret = isp_drv_calc_sync_cfg(sync_cfg);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_calc_sync_cfg");
            ret = isp_drv_reg_config_isp(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_isp");
            ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");

            drv_ctx->status = isp_int_status;

            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);

            if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
                isp_drv_stat_buf_busy_put(vi_pipe);
            }

            osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);
            isp_drv_wake_up_thread(vi_pipe);
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_sync_task_process(vi_pipe);

            return TD_FAILURE;
        }

        osal_spin_lock_irqsave(&g_isp_lock[vi_pipe], &flags);

        drv_ctx->use_node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);
        if (drv_ctx->use_node == TD_NULL) {
            /* Need to configure the sensor registers. */
            ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");
            osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
            isp_err_trace("Pipe[%d] get FreeBeBuf is fail!\r\n", vi_pipe);

            return TD_FAILURE;
        }

        osal_spin_unlock_irqrestore(&g_isp_lock[vi_pipe], &flags);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_use_node(ot_vi_pipe vi_pipe, td_u32 isp_int_status, td_u32 wch_int_status)
{
    td_s32 ret;

    isp_check_pipe_return(vi_pipe);

    ret = isp_drv_get_use_node_raw_mode(vi_pipe, isp_int_status);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_drv_get_use_node_yuv_mode(vi_pipe, isp_int_status, wch_int_status);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
td_s32 isp_drv_stitch_get_use_node(ot_vi_pipe vi_pipe, td_u32 isp_int_status)
{
    td_s32 ret;
    ot_vi_pipe main_pipe;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_sync_cfg *sync_cfg = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    sync_cfg = &drv_ctx->sync_cfg;
    main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];

    if (!is_offline_mode(drv_ctx->work_mode.running_mode) && !is_striping_mode(drv_ctx->work_mode.running_mode) &&
        !is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        return TD_SUCCESS;
    }

    isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);

    if (isp_int_status & ISP_1ST_INT) {
        osal_spin_lock_irqsave(&g_isp_sync_lock[main_pipe], &flags);

        if (drv_ctx->use_node != TD_NULL) {
            /* Need to configure the sensor registers and get statistics for AE/AWB. */
            ret = isp_drv_get_sync_controlnfo(vi_pipe, sync_cfg);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_get_sync_controlnfo");
            ret = isp_drv_calc_sync_cfg(sync_cfg);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_calc_sync_cfg");
            ret = isp_drv_reg_config_isp(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_isp");
            ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");

            drv_ctx->status = isp_int_status;

            if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
                isp_drv_stat_buf_busy_put(vi_pipe);
            }

            isp_drv_wake_up_thread(vi_pipe);
            osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
            isp_sync_task_process(vi_pipe);

            return TD_FAILURE;
        }

        drv_ctx->use_node = isp_queue_get_free_be_buf(&drv_ctx->be_buf_queue);

        if (drv_ctx->use_node == TD_NULL) {
            /* Need to configure the sensor registers. */
            ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
            isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");
            osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
            isp_err_trace("Pipe[%d] get FreeBeBuf is fail!\r\n", vi_pipe);

            return TD_FAILURE;
        }

        osal_spin_unlock_irqrestore(&g_isp_sync_lock[main_pipe], &flags);
    }

    return TD_SUCCESS;
}
#endif

td_s32 isp_drv_get_be_buf_use_node(ot_vi_pipe vi_pipe, td_u32 isp_int_status, td_u32 int_num)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u32 wch_int_f_start;
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    wch_int_f_start = drv_ctx->int_sch[int_num].wch_int_status;
    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return isp_drv_get_use_node(vi_pipe, isp_int_status, wch_int_f_start);
    } else {
#ifdef CONFIG_OT_VI_STITCH_GRP
        return isp_drv_stitch_get_use_node(vi_pipe, isp_int_status);
#else
        return TD_FAILURE;
#endif
    }
}

td_void isp_drv_proc_get_port_int_time1(isp_drv_ctx *drv_ctx, td_u32 port_int_f_start, td_u64 *pt_time1)
{
    if (port_int_f_start) { /* port int proc */
        drv_ctx->drv_dbg_info.pt_int_cnt++;
        *pt_time1 = call_sys_get_time_stamp();

        if (drv_ctx->drv_dbg_info.pt_last_int_time) {
            drv_ctx->drv_dbg_info.pt_int_gap_time = *pt_time1 - drv_ctx->drv_dbg_info.pt_last_int_time;

            if (drv_ctx->drv_dbg_info.pt_int_gap_time > drv_ctx->drv_dbg_info.pt_int_gap_time_max) {
                drv_ctx->drv_dbg_info.pt_int_gap_time_max = drv_ctx->drv_dbg_info.pt_int_gap_time;
            }
        }

        drv_ctx->drv_dbg_info.pt_last_int_time = *pt_time1;
    }
}

td_void isp_drv_proc_get_isp_int_time1(isp_drv_ctx *drv_ctx, td_u32 isp_int_status, td_u64 *isp_time1)
{
    if (isp_int_status & ISP_1ST_INT) { /* isp int proc */
        drv_ctx->drv_dbg_info.isp_int_cnt++;
        *isp_time1 = call_sys_get_time_stamp();

        if (drv_ctx->drv_dbg_info.isp_last_int_time) {
            drv_ctx->drv_dbg_info.isp_int_gap_time = *isp_time1 - drv_ctx->drv_dbg_info.isp_last_int_time;

            if (drv_ctx->drv_dbg_info.isp_int_gap_time > drv_ctx->drv_dbg_info.isp_int_gap_time_max) {
                drv_ctx->drv_dbg_info.isp_int_gap_time_max = drv_ctx->drv_dbg_info.isp_int_gap_time;
            }
        }

        drv_ctx->drv_dbg_info.isp_last_int_time = *isp_time1;
    }
}

td_void isp_drv_proc_calc_port_int(isp_drv_ctx *drv_ctx, td_u32 port_int_f_start, td_u64 pt_time1)
{
    td_u64 pt_time2;

    if (port_int_f_start) { /* port int proc */
        pt_time2 = call_sys_get_time_stamp();
        drv_ctx->drv_dbg_info.pt_int_time = pt_time2 - pt_time1;

        if (drv_ctx->drv_dbg_info.pt_int_time > drv_ctx->drv_dbg_info.pt_int_time_max) {
            drv_ctx->drv_dbg_info.pt_int_time_max = drv_ctx->drv_dbg_info.pt_int_time;
        }

        if ((pt_time2 - drv_ctx->drv_dbg_info.pt_last_rate_time) >= 1000000ul) {
            drv_ctx->drv_dbg_info.pt_last_rate_time = pt_time2;
            drv_ctx->drv_dbg_info.pt_rate = drv_ctx->drv_dbg_info.pt_rate_int_cnt;
            drv_ctx->drv_dbg_info.pt_rate_int_cnt = 0;
        }

        drv_ctx->drv_dbg_info.pt_rate_int_cnt++;
    }
}

td_void isp_drv_proc_calc_isp_int(isp_drv_ctx *drv_ctx, td_u32 isp_int_status, td_u64 isp_time1)
{
    td_u64 isp_time2;

    if (isp_int_status & ISP_1ST_INT) { /* isp int proc */
        isp_time2 = call_sys_get_time_stamp();
        drv_ctx->drv_dbg_info.isp_int_time = isp_time2 - isp_time1;

        if (drv_ctx->drv_dbg_info.isp_int_time > drv_ctx->drv_dbg_info.isp_int_time_max) {
            drv_ctx->drv_dbg_info.isp_int_time_max = drv_ctx->drv_dbg_info.isp_int_time;
        }

        if ((isp_time2 - drv_ctx->drv_dbg_info.isp_last_rate_time) >= 1000000ul) {
            drv_ctx->drv_dbg_info.isp_last_rate_time = isp_time2;
            drv_ctx->drv_dbg_info.isp_rate = drv_ctx->drv_dbg_info.isp_rate_int_cnt;
            drv_ctx->drv_dbg_info.isp_rate_int_cnt = 0;
        }

        drv_ctx->drv_dbg_info.isp_rate_int_cnt++;
    }
}

td_void isp_drv_proc_calc_sensor_cfg_time(isp_drv_ctx *drv_ctx)
{
    if (drv_ctx->drv_dbg_info.sensor_cfg_time > drv_ctx->drv_dbg_info.sensor_cfg_time_max) {
        drv_ctx->drv_dbg_info.sensor_cfg_time_max = drv_ctx->drv_dbg_info.sensor_cfg_time;
    }
}

td_s32 isp_drv_sync_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 ret;
    td_u64 sensor_cfg_time1;
    td_u64 sensor_cfg_time2;
    isp_sync_cfg *sync_cfg = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    sync_cfg = &drv_ctx->sync_cfg;

    ret = isp_drv_get_sync_controlnfo(vi_pipe, sync_cfg);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_get_sync_controlnfo");
    ret = isp_drv_calc_sync_cfg(sync_cfg);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_calc_sync_cfg");
    ret = isp_drv_reg_config_isp(vi_pipe, drv_ctx);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_isp");

    sensor_cfg_time1 = call_sys_get_time_stamp();
    ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
    isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");
    sensor_cfg_time2 = call_sys_get_time_stamp();
    drv_ctx->drv_dbg_info.sensor_cfg_time = sensor_cfg_time2 - sensor_cfg_time1;

    return TD_SUCCESS;
}

td_void isp_get_slave_pipe_int_status(ot_vi_pipe vi_pipe, td_bool *slave_pipe_int_active)
{
    td_u8 chn_num_max;
    td_u8 k;
    td_u8 int_status_index;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u32 isp_int_status = 0;
    td_u32 isp_raw_int = 0;
    ot_vi_pipe vi_pipe_bind;
    if (g_long_frm_int_en == TD_FALSE) {
        *slave_pipe_int_active = TD_FALSE;
        return;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if ((drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) || (is_fs_wdr_mode(drv_ctx->wdr_attr.wdr_mode) == TD_FALSE)) {
        (td_void)memset_s(&(drv_ctx->slave_pipe_int_status), sizeof(isp_slave_pipe_int_status) * (ISP_WDR_CHN_MAX - 1),
            0, sizeof(isp_slave_pipe_int_status) * (ISP_WDR_CHN_MAX - 1));
        *slave_pipe_int_active = TD_FALSE;
        return;
    }

    chn_num_max = clip3(drv_ctx->wdr_attr.pipe_num, 1, ISP_WDR_CHN_MAX);
    int_status_index = 0;
    for (k = 0; k < chn_num_max; k++) {
        vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];
        if (vi_pipe == vi_pipe_bind) {
            continue;
        } else {
            if (vi_pipe_bind >= OT_ISP_MAX_PHY_PIPE_NUM) {
                continue;
            }
            /* read fe interrupt status */
            isp_raw_int = io_rw_fe_address(vi_pipe_bind, ISP_INT_FE);
            isp_int_status = isp_raw_int & io_rw_fe_address(vi_pipe_bind, ISP_INT_FE_MASK);
            if (isp_raw_int) {  /* clear interrupts */
                io_rw_fe_address(vi_pipe_bind, ISP_INT_FE) = isp_raw_int;
            }

            drv_ctx->slave_pipe_int_status[int_status_index].vi_pipe_id = vi_pipe_bind;
            drv_ctx->slave_pipe_int_status[int_status_index].isp_int_status = isp_int_status;
            int_status_index++;
            if (isp_int_status != 0) {
                *slave_pipe_int_active = TD_TRUE;
            }
        }
    }

    return;
}

isp_cfg_sensor_time isp_drv_get_sns_cfg_int_pos(isp_drv_ctx *drv_ctx, ot_vi_pipe vi_pipe_bind)
{
    ot_vi_pipe wdr_short_frame_pipe_id;
    ot_vi_pipe wdr_long_frame_pipe_id;
    ot_vi_pipe status0_pipe_id = drv_ctx->slave_pipe_int_status[0].vi_pipe_id;
    ot_vi_pipe status1_pipe_id = drv_ctx->slave_pipe_int_status[1].vi_pipe_id; /* 1 */
    ot_vi_pipe status2_pipe_id = drv_ctx->slave_pipe_int_status[2].vi_pipe_id; /* 2 */

    if (drv_ctx->wdr_attr.wdr_mode == OT_WDR_MODE_2To1_LINE) {
        return ISP_CFG_SNS_SHORT_FRAME;
    }
    if (drv_ctx->wdr_attr.wdr_mode == OT_WDR_MODE_3To1_LINE) {
        if (status0_pipe_id < status1_pipe_id) {
            wdr_short_frame_pipe_id = status0_pipe_id;
        } else {
            wdr_short_frame_pipe_id = status1_pipe_id;
        }
        if (vi_pipe_bind == wdr_short_frame_pipe_id) {
            return ISP_CFG_SNS_SHORT_FRAME;
        } else {
            return ISP_CFG_SNS_MIDDLE_FRAME;
        }
    }
    if (drv_ctx->wdr_attr.wdr_mode == OT_WDR_MODE_4To1_LINE) {
        if ((status0_pipe_id < status1_pipe_id) && (status0_pipe_id < status2_pipe_id)) {
            wdr_short_frame_pipe_id = status0_pipe_id;
        } else if ((status1_pipe_id < status0_pipe_id) && (status1_pipe_id < status2_pipe_id)) {
            wdr_short_frame_pipe_id = status1_pipe_id;
        } else {
            wdr_short_frame_pipe_id = status2_pipe_id;
        }
        if ((status0_pipe_id > status1_pipe_id) && (status0_pipe_id > status2_pipe_id)) {
            wdr_long_frame_pipe_id = status0_pipe_id;
        } else if ((status1_pipe_id > status0_pipe_id) && (status1_pipe_id > status2_pipe_id)) {
            wdr_long_frame_pipe_id = status1_pipe_id;
        } else {
            wdr_long_frame_pipe_id = status2_pipe_id;
        }
        if (vi_pipe_bind == wdr_short_frame_pipe_id) {
            return ISP_CFG_SNS_SHORT_FRAME;
        } else if (vi_pipe_bind == wdr_long_frame_pipe_id) {
            return ISP_CFG_SNS_LONG_FRAME;
        } else {
            return ISP_CFG_SNS_MIDDLE_FRAME;
        }
    }
    return ISP_CFG_SNS_SHORT_FRAME;
}
td_void isp_long_frm_cfg_sensor(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u32 bind_pipe_int_status;
    td_u32 pipe_int_pos_save;
    td_u32 pipe_int_status_save;
    td_u32 k;
    ot_vi_pipe vi_pipe_bind;
    isp_cfg_sensor_time cfg_sns_int_pos;
    if (g_long_frm_int_en == TD_FALSE) {
        return;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (is_fs_wdr_mode(drv_ctx->wdr_attr.wdr_mode) == TD_FALSE) {
        return;
    }

    pipe_int_status_save = drv_ctx->status;
    pipe_int_pos_save = drv_ctx->int_pos;

    for (k = 0; k < ISP_WDR_CHN_MAX - 1; k++) {
        bind_pipe_int_status = drv_ctx->slave_pipe_int_status[k].isp_int_status;
        vi_pipe_bind = drv_ctx->slave_pipe_int_status[k].vi_pipe_id;
        cfg_sns_int_pos = isp_drv_get_sns_cfg_int_pos(drv_ctx, vi_pipe_bind);
        if (bind_pipe_int_status & ISP_1ST_INT) {
            drv_ctx->int_pos = cfg_sns_int_pos + 0; /* 0:frame start */
            drv_ctx->status = bind_pipe_int_status;
            isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
        }
        if (bind_pipe_int_status & ISP_2ND_INT) {
            drv_ctx->int_pos = cfg_sns_int_pos + 1; /* 1:frame end */
            drv_ctx->status = bind_pipe_int_status;
            isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
        }
    }
    drv_ctx->status = pipe_int_status_save;
    drv_ctx->int_pos = pipe_int_pos_save;
    return;
}

static td_s32 isp_drv_dynamic_blc_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_u32 isp_int_status = 0;
    isp_check_vir_pipe_return(vi_pipe);
    if (g_ob_stats_update_pos[vi_pipe] >= OT_ISP_UPDATE_OB_STATS_BUTT) {
        return TD_SUCCESS;
    }
    if (g_ob_stats_update_pos[vi_pipe] == OT_ISP_UPDATE_OB_STATS_FE_FRAME_END) {
        isp_int_status = drv_ctx->int_sch[int_num].isp_int_status & ISP_INT_FE_DYNABLC_END;
    } else if (g_ob_stats_update_pos[vi_pipe] == OT_ISP_UPDATE_OB_STATS_FE_FRAME_START) {
        isp_int_status = drv_ctx->int_sch[int_num].isp_int_status & ISP_INT_FE_FSTART;
    }

    if (isp_int_status) {
        isp_drv_reg_config_dynamic_blc(vi_pipe, drv_ctx);
    }

    return TD_SUCCESS;
}

static td_u32 isp_be_cfg_int_time_get(ot_vi_pipe vi_pipe, const isp_drv_ctx *drv_ctx,
    td_u32 isp_calc_sync_timing, td_u32 int_num)
{
    td_u32 isp_int_status;

    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;
    if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        return (isp_int_status & ISP_INT_FE_DELAY); /* fe_int_int_delay */
    }

    if (is_line_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        return (isp_int_status & ISP_INT_FE_DELAY); /* fe_int_int_delay */
    }

    if (g_update_pos[vi_pipe] == 0) { /* frame start */
        return (isp_int_status & ISP_INT_FE_DELAY); /* fe_int_int_delay */
    } else {
        return isp_calc_sync_timing;
    }
}

/* because FE frame start time is early than BE frame start when ONLINE mode, */
/* sometime sync register configure at FE frame start will be valid at BE frame start. this means configure register */
/* and register active at the same frame, this make an error occurred. */
/* for this case, we use frame delay interrupt at ONLINE mode, */
/* when frame delay interrupt triggered, configure register */

static td_void isp_drv_reg_config_isp_be_online(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    td_u32 isp_calc_sync_timing, td_u32 int_num)
{
    td_u32 be_cfg_timing;
    if (is_offline_mode(drv_ctx->work_mode.running_mode) ||
        is_striping_mode(drv_ctx->work_mode.running_mode)) {
        return;
    }

    be_cfg_timing = isp_be_cfg_int_time_get(vi_pipe, drv_ctx, isp_calc_sync_timing, int_num);
    if (be_cfg_timing) {
        isp_drv_set_isp_be_sync_param_online(vi_pipe, drv_ctx);
    }
    return;
}

#ifdef CONFIG_OT_SNAP_SUPPORT
td_s32 isp_irq_snap_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_u32 isp_int_status;
    td_u8 cfg_dly_max;

    isp_check_pointer_return(drv_ctx);
    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;

    if (isp_int_status & ISP_2ND_INT) {
        if (drv_ctx->pro_trig_flag == 1) {
            drv_ctx->pro_trig_flag++;
            drv_ctx->start_snap_num = drv_ctx->frame_cnt;
        }

        if (drv_ctx->pro_trig_flag > 1 && drv_ctx->start_snap_num > 0) {
            cfg_dly_max = MAX2(drv_ctx->sync_cfg.cfg2_vld_dly_max, 1);
            if (ckfn_vi_set_pro_frame_flag() &&
                (drv_ctx->frame_cnt - drv_ctx->start_snap_num > (td_u32)cfg_dly_max) &&
                (drv_ctx->pro_frm_num < drv_ctx->snap_attr.pro_param.pro_frame_num)) {
                call_vi_set_pro_frame_flag(vi_pipe);
                drv_ctx->pro_frm_num++;
            }

            if (drv_ctx->pro_frm_num >= drv_ctx->snap_attr.pro_param.pro_frame_num) {
                drv_ctx->pro_trig_flag = 0;
            }
        }
    }

    return TD_SUCCESS;
}
#endif

td_s32 isp_irq_full_wdr_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_s32 ret;
    td_u32 isp_int_status;

    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;

    drv_ctx->int_pos = 0;
    if (isp_int_status & ISP_1ST_INT) {
        if (is_full_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            ret = isp_drv_sync_process(vi_pipe, drv_ctx);
            isp_check_return(vi_pipe, ret, "isp_drv_sync_process");
        }
    }

    if (isp_int_status & ISP_2ND_INT) {
        drv_ctx->int_pos = 1;

        ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
        isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");

        drv_ctx->vd_end = TD_TRUE;
        isp_update_info_sync(vi_pipe, &drv_ctx->update_info);
        isp_frame_info_sync(vi_pipe, &drv_ctx->frame_info);
        osal_wakeup(&drv_ctx->isp_wait_vd_end);
    }

    if (isp_int_status & ISP_1ST_INT) {
        /* N to 1 fullrate frame WDR mode, get statistics only in the last frame(N-1) */
        if (is_full_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            if (drv_ctx->sync_cfg.vc_num != drv_ctx->sync_cfg.vc_num_max) {
                return TD_SUCCESS;
            }
        }

        drv_ctx->status = isp_int_status;

        if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
            isp_drv_stat_buf_busy_put(vi_pipe);
        }

        isp_drv_wake_up_thread(vi_pipe);
        /* Sync  task AF statistics */
        isp_sync_task_process(vi_pipe);
    }

    isp_long_frm_cfg_sensor(vi_pipe);
    return TD_SUCCESS;
}

td_s32 isp_irq_half_wdr_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_s32 ret;
    td_u32 port_int_f_start, isp_int_status;

    port_int_f_start = drv_ctx->int_sch[int_num].port_int_status;
    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;

    drv_ctx->int_pos = 0;
    if (port_int_f_start) {
        if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            ret = isp_drv_sync_process(vi_pipe, drv_ctx);
            isp_check_return(vi_pipe, ret, "isp_drv_sync_process");
        }
    }
    isp_drv_dynamic_blc_process(vi_pipe, drv_ctx, int_num);
    isp_drv_reg_config_isp_be_online(vi_pipe, drv_ctx, port_int_f_start, int_num);

    if (isp_int_status & ISP_2ND_INT) {
        drv_ctx->int_pos = 1;

        ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
        isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");

        drv_ctx->vd_end = TD_TRUE;
        isp_update_info_sync(vi_pipe, &drv_ctx->update_info);
        isp_frame_info_sync(vi_pipe, &drv_ctx->frame_info);
        osal_wakeup(&drv_ctx->isp_wait_vd_end);
    }

    if (isp_int_status & ISP_1ST_INT) {
        drv_ctx->status = isp_int_status;

        if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
            isp_drv_stat_buf_busy_put(vi_pipe);
        }

        isp_drv_wake_up_thread(vi_pipe);
        /* Sync  task AF statistics */
        isp_sync_task_process(vi_pipe);
    }

    isp_long_frm_cfg_sensor(vi_pipe);
    return TD_SUCCESS;
}

td_s32 isp_irq_line_wdr_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_s32 ret;
    td_u32 isp_int_status;

    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;

    drv_ctx->int_pos = 0;
    if (isp_int_status & ISP_1ST_INT) {
        if (is_line_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            ret = isp_drv_sync_process(vi_pipe, drv_ctx);
            isp_check_return(vi_pipe, ret, "isp_drv_sync_process");
        }
    }
    isp_drv_dynamic_blc_process(vi_pipe, drv_ctx, int_num);
    isp_drv_reg_config_isp_be_online(vi_pipe, drv_ctx, isp_int_status & ISP_1ST_INT, int_num);

    if (isp_int_status & ISP_2ND_INT) {
        drv_ctx->int_pos = 1;

        ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
        isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");

        drv_ctx->vd_end = TD_TRUE;

        isp_update_info_sync(vi_pipe, &drv_ctx->update_info);
        isp_frame_info_sync(vi_pipe, &drv_ctx->frame_info);
        osal_wakeup(&drv_ctx->isp_wait_vd_end);
    }

    if (isp_int_status & ISP_1ST_INT) {
        drv_ctx->status = isp_int_status;

        if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
            isp_drv_stat_buf_busy_put(vi_pipe);
        }

        isp_drv_wake_up_thread(vi_pipe);
        /* Sync  task AF statistics */
        isp_sync_task_process(vi_pipe);
    }

    isp_long_frm_cfg_sensor(vi_pipe);
    return TD_SUCCESS;
}

td_s32 isp_irq_linear_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_s32 ret;
    td_u32 isp_int_status, sensor_cfg_int;

    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;

    drv_ctx->int_pos = 0;
    sensor_cfg_int = (isp_int_status & ISP_2ND_INT);
    if (g_update_pos[vi_pipe] == 0) { /* frame start */
        sensor_cfg_int = (isp_int_status & ISP_1ST_INT);
    }
    if (sensor_cfg_int) {
        if (is_linear_mode(drv_ctx->sync_cfg.wdr_mode) || is_built_in_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
            ret = isp_drv_sync_process(vi_pipe, drv_ctx);
            isp_check_return(vi_pipe, ret, "isp_drv_sync_process");
        }
    }
    isp_drv_dynamic_blc_process(vi_pipe, drv_ctx, int_num);
    isp_drv_reg_config_isp_be_online(vi_pipe, drv_ctx, sensor_cfg_int, int_num);

    if (isp_int_status & ISP_2ND_INT) {
        drv_ctx->int_pos = 1;

        ret = isp_drv_reg_config_sensor(vi_pipe, drv_ctx);
        isp_check_ret_continue(vi_pipe, ret, "isp_drv_reg_config_sensor");

        drv_ctx->vd_end = TD_TRUE;
        isp_update_info_sync(vi_pipe, &drv_ctx->update_info);
        isp_frame_info_sync(vi_pipe, &drv_ctx->frame_info);
        osal_wakeup(&drv_ctx->isp_wait_vd_end);
    }

    if (isp_int_status & ISP_1ST_INT) {
        drv_ctx->status = isp_int_status;

        if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
            isp_drv_stat_buf_busy_put(vi_pipe);
        }

        isp_drv_wake_up_thread(vi_pipe);
        /* Sync  task AF statistics */
        isp_sync_task_process(vi_pipe);
    }

    return TD_SUCCESS;
}

td_s32 isp_irq_yuv_process(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    td_u32 wch_int_f_start;

    wch_int_f_start = drv_ctx->int_sch[int_num].wch_int_status;

    if (wch_int_f_start && (drv_ctx->yuv_mode == TD_TRUE) && (drv_ctx->isp_run_flag == TD_TRUE)) { /* WCH int */
        drv_ctx->status = wch_int_f_start;

        if (drv_ctx->frame_cnt++ % div_0_to_1(g_stat_intvl[vi_pipe]) == 0) {
            isp_drv_stat_buf_busy_put(vi_pipe);
        }

        isp_drv_wake_up_thread(vi_pipe);
        /* Sync  task AF statistics */
        isp_sync_task_process(vi_pipe);
    }

    return TD_SUCCESS;
}

static td_void isp_irq_proc_by_wdr_mode(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 int_num)
{
    if (is_linear_mode(drv_ctx->sync_cfg.wdr_mode) || is_built_in_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_irq_linear_process(vi_pipe, drv_ctx, int_num);
    }

    if (is_line_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_irq_line_wdr_process(vi_pipe, drv_ctx, int_num);
    }

    if (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_irq_half_wdr_process(vi_pipe, drv_ctx, int_num);
    }

    if (is_full_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_irq_full_wdr_process(vi_pipe, drv_ctx, int_num);
    }
}

int isp_irq_route(ot_vi_pipe vi_pipe, td_u32 int_num)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u32 port_int_f_start, isp_int_status;
    td_u64 pt_time1 = 0;
    td_u64 isp_time1 = 0;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    port_int_f_start = drv_ctx->int_sch[int_num].port_int_status;
    isp_int_status = drv_ctx->int_sch[int_num].isp_int_status;

    isp_drv_proc_get_port_int_time1(drv_ctx, port_int_f_start, &pt_time1);
    isp_drv_proc_get_isp_int_time1(drv_ctx, isp_int_status, &isp_time1);

    drv_ctx->int_pos = 0;

    ret = isp_drv_get_be_buf_use_node(vi_pipe, isp_int_status, int_num);
    if (ret != TD_SUCCESS) {
        return OSAL_IRQ_HANDLED;
    }

#ifdef CONFIG_OT_SNAP_SUPPORT
    if ((vi_pipe != drv_ctx->snap_attr.picture_pipe_id) ||
        ((drv_ctx->snap_attr.picture_pipe_id != -1 &&
        (drv_ctx->snap_attr.picture_pipe_id == drv_ctx->snap_attr.preview_pipe_id)))) {
        isp_irq_proc_by_wdr_mode(vi_pipe, drv_ctx, int_num);

        if (drv_ctx->snap_attr.snap_type == OT_SNAP_TYPE_PRO) {
            isp_irq_snap_process(vi_pipe, drv_ctx, int_num);
        }
    }
#else
    isp_irq_proc_by_wdr_mode(vi_pipe, drv_ctx);
#endif

    if (drv_ctx->yuv_mode == TD_TRUE) {
        isp_irq_yuv_process(vi_pipe, drv_ctx, int_num);
    }

    isp_drv_proc_calc_sensor_cfg_time(drv_ctx);
    isp_drv_proc_calc_port_int(drv_ctx, port_int_f_start, pt_time1);
    isp_drv_proc_calc_isp_int(drv_ctx, isp_int_status, isp_time1);

    return OSAL_IRQ_HANDLED;
}

static td_void isp_stitch_irq_route(ot_vi_pipe vi_pipe, td_u32 isp_int_status, td_u32 port_int_status, td_u32 int_num)
{
#ifdef CONFIG_OT_VI_STITCH_GRP
    td_u32 j;
    td_s32 ret;
    ot_vi_pipe vi_pipes;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->stitch_attr.main_pipe == TD_FALSE) {
        return;
    }
    ret = isp_drv_stitch_sync(vi_pipe);
    if (ret != TD_SUCCESS) {
        return;
    }

    for (j = 0; j < drv_ctx->stitch_attr.stitch_pipe_num; j++) {
        vi_pipes = drv_ctx->stitch_attr.stitch_bind_id[j];
        drv_ctx_s = isp_drv_get_ctx(vi_pipes);

        drv_ctx_s->int_sch[int_num].isp_int_status = isp_int_status;
        drv_ctx_s->int_sch[int_num].port_int_status = port_int_status;
        drv_ctx_s->int_sch[int_num].port_int_err = drv_ctx->int_sch[int_num].port_int_err;

        ret = isp_irq_route(vi_pipes, int_num);
        if (ret != OSAL_IRQ_HANDLED) {
            isp_warn_trace("isp[%d] isp_irq_route failed\n", vi_pipes);
            return;
        }
    }
#else
    ot_unused(vi_pipe);
    ot_unused(isp_int_status);
    ot_unused(port_int_status);
#endif
}
int isp_drv_int_bottom_half(int irq)
{
    ot_vi_pipe vi_pipe;
    td_u32 i;
    td_u32 j;
    td_u32 isp_int_read_num = 0;

    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u32 port_int_status;
    td_u32 isp_int_status;
    td_u32 wch_int_f_start;
    td_bool slave_pipe_int_active;
    ot_unused(irq);

    for (i = 0; i < OT_ISP_MAX_PHY_PIPE_NUM; i++) {
        vi_pipe = i;
        isp_int_read_num = 0;
        drv_ctx = isp_drv_get_ctx(vi_pipe);
        if (isp_get_interrupt_info(vi_pipe, drv_ctx, &isp_int_read_num) == 0) {
            continue;
        }

        for (j = 0; j < isp_int_read_num; j++) {
            isp_int_status = drv_ctx->int_sch[j].isp_int_status;
            port_int_status = drv_ctx->int_sch[j].port_int_status;
            wch_int_f_start = drv_ctx->int_sch[j].wch_int_status;
            isp_get_slave_pipe_int_status(vi_pipe, &slave_pipe_int_active);
            if (!port_int_status && !isp_int_status && !wch_int_f_start && (slave_pipe_int_active == TD_FALSE)) {
                continue;
            }

            if (!drv_ctx->mem_init) {
                continue;
            }

            if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
                isp_stitch_irq_route(vi_pipe, isp_int_status, port_int_status, j);
            } else {
                isp_irq_route(vi_pipe, j);
            }
        }
    }

    return OSAL_IRQ_HANDLED;
}

static int isp_drv_init(void)
{
    td_s32 ret;

    ret = isp_drv_be_remap();
    if (ret == TD_FAILURE) {
        return ret;
    }

    ret = isp_drv_vicap_remap();
    if (ret == TD_FAILURE) {
        return ret;
    }

    ret = isp_drv_fe_remap();
    if (ret == TD_FAILURE) {
        return ret;
    }
#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
    isp_drv_work_queue_init();
#endif
    return 0;
}

static int isp_drv_exit(void)
{
    isp_drv_be_unmap();

    isp_drv_vicap_unmap();

    isp_drv_fe_unmap();
#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
    isp_drv_work_queue_exit();
#endif

    return 0;
}

td_s32 isp_kern_init(void *p)
{
    td_u32 vi_pipe;
    ot_unused(p);

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PIPE_NUM; vi_pipe++) {
        (td_void)memset_s(&g_isp_drv_ctx[vi_pipe].drv_dbg_info, sizeof(isp_drv_dbg_info), 0, sizeof(isp_drv_dbg_info));
    }

    return TD_SUCCESS;
}

td_void isp_kern_exit(void)
{
    td_u32 vi_pipe;
    td_void *reg_vicap_base_va = TD_NULL;
    td_void  *reg_ispfe_base_va[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

    reg_vicap_base_va = isp_drv_get_reg_vicap_base_va();
    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PIPE_NUM; vi_pipe++) {
        reg_ispfe_base_va[vi_pipe] = isp_drv_get_ispfe_base_va(vi_pipe);
        if ((reg_vicap_base_va != TD_NULL) && (reg_ispfe_base_va[vi_pipe] != TD_NULL)) {
            isp_drv_set_int_enable(vi_pipe, TD_FALSE);
        }
        (td_void)memset_s(&g_isp_drv_ctx[vi_pipe].drv_dbg_info, sizeof(isp_drv_dbg_info), 0, sizeof(isp_drv_dbg_info));
    }

    return;
}

static td_u32 isp_get_ver_magic(td_void)
{
    return VERSION_MAGIC;
}

static umap_module g_isp_module = {
    .mod_id = OT_ID_ISP,
    .mod_name = "isp",
    .pfn_init = isp_kern_init,
    .pfn_exit = isp_kern_exit,
    .pfn_ver_checker = isp_get_ver_magic,
    .export_funcs = TD_NULL,
    .data = TD_NULL,
};

int isp_module_init(void)
{
    td_u32 vi_pipe;

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_proc_entry_t *proc = TD_NULL;
#endif

    g_isp_device = osal_createdev("isp_dev");
    g_isp_device->fops = &g_isp_file_ops;
    g_isp_device->minor = UMAP_ISP_MINOR_BASE;

    if (osal_registerdevice(g_isp_device) < 0) {
        OT_PRINT("Kernel: Could not register isp devices\n");
        return TD_FAILURE;
    }

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    proc = osal_create_proc_entry(PROC_ENTRY_ISP, TD_NULL);
    if (proc == TD_NULL) {
        OT_PRINT("Kernel: Register isp proc failed!\n");
        goto OUT2;
    }

    proc->read = isp_proc_show;
#endif
    g_isp_module.export_funcs = isp_get_export_func();
    if (cmpi_register_module(&g_isp_module)) {
        goto OUT1;
    }

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PIPE_NUM; vi_pipe++) {
        osal_spin_lock_init(&g_isp_lock[vi_pipe]);
        osal_spin_lock_init(&g_isp_sync_lock[vi_pipe]);
    }

    if (isp_drv_init() != 0) {
        OT_PRINT("isp init failed\n");
        goto OUT1;
    }

    OT_PRINT("load isp.ko ....OK!\n");
    return TD_SUCCESS;

OUT1:
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_ISP, TD_NULL);
OUT2:
#endif
    osal_deregisterdevice(g_isp_device);
    osal_destroydev(g_isp_device);

    OT_PRINT("isp mod init failed!\n");
    return TD_FAILURE;
}

void isp_module_exit(void)
{
    int i;

    isp_drv_exit();

    for (i = 0; i < OT_ISP_MAX_PIPE_NUM; i++) {
        osal_spin_lock_destroy(&g_isp_lock[i]);
        osal_spin_lock_destroy(&g_isp_sync_lock[i]);
    }

    cmpi_unregister_module(OT_ID_ISP);

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_ISP, TD_NULL);
#endif
    osal_deregisterdevice(g_isp_device);
    osal_destroydev(g_isp_device);

    OT_PRINT("unload isp.ko ....OK!\n");
}
