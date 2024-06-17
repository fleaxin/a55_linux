/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_drv.h"
#include "isp_drv_define.h"
#include "isp_reg_define.h"
#include "isp_stt_define.h"
#include "ot_common.h"
#include "ot_osal.h"
#include "ot_math.h"
#include "mkp_isp.h"
#include "isp.h"
#include "mm_ext.h"
#include "sys_ext.h"

static td_void *g_reg_vicap_base_va = TD_NULL;
static td_void  *g_reg_vicap_ch_base_va[OT_ISP_MAX_PIPE_NUM] = { TD_NULL };
static td_void  *g_reg_ispfe_base_va[OT_ISP_MAX_PIPE_NUM] = { TD_NULL };
static td_void  *g_reg_isp_pre_be_base_va[ISP_MAX_BE_NUM] = { TD_NULL };
static td_void  *g_reg_isp_post_be_base_va[ISP_MAX_BE_NUM] = { TD_NULL };
static td_void  *g_reg_pre_viproc_base_va[ISP_MAX_BE_NUM] = { TD_NULL };
static td_void  *g_reg_post_viproc_base_va[ISP_MAX_BE_NUM] = { TD_NULL };

td_void *isp_drv_get_reg_vicap_base_va(td_void)
{
    return g_reg_vicap_base_va;
}

td_void *isp_drv_get_vicap_ch_base_va(ot_vi_pipe vi_pipe)
{
    return g_reg_vicap_ch_base_va[vi_pipe];
}

td_void *isp_drv_get_ispfe_base_va(ot_vi_pipe vi_pipe)
{
    return g_reg_ispfe_base_va[vi_pipe];
}

td_void *isp_drv_get_isp_pre_be_base_va(td_u8 blk_dev)
{
    return g_reg_isp_pre_be_base_va[blk_dev];
}

td_void *isp_drv_get_isp_post_be_base_va(td_u8 blk_dev)
{
    return g_reg_isp_post_be_base_va[blk_dev];
}

td_void *isp_drv_get_pre_viproc_base_va(td_u8 blk_dev)
{
    return g_reg_pre_viproc_base_va[blk_dev];
}

td_void *isp_drv_get_post_viproc_base_va(td_u8 blk_dev)
{
    return g_reg_post_viproc_base_va[blk_dev];
}
td_s32 isp_drv_stitch_sync_get_working_index(isp_drv_ctx *drv_ctx_s)
{
    return drv_ctx_s->be_off_stitch_stt_buf.working_idx;
}
td_void isp_drv_stitch_sync_set_working_index(isp_drv_ctx *drv_ctx_s, td_s32 working_idx)
{
    drv_ctx_s->be_off_stitch_stt_buf.working_idx = working_idx;
}

td_void isp_drv_stitch_sync_info_ready(isp_drv_ctx *drv_ctx_s, td_s32 ready_idx, td_bool is_ready)
{
    drv_ctx_s->be_off_stitch_stt_buf.is_ready[ready_idx] = is_ready;
}

td_void isp_drv_set_ldci_write_stt_addr(isp_viproc_reg_type *viproc_reg, td_phys_addr_t write_addr)
{
    viproc_reg->viproc_out_para_dci_addr_low.u32 = get_low_addr(write_addr);
    viproc_reg->viproc_out_para_dci_addr_high.u32 = get_high_addr(write_addr);
}

td_void isp_drv_set_ldci_read_stt_addr(isp_viproc_reg_type *viproc_reg, td_phys_addr_t read_addr)
{
    viproc_reg->viproc_para_dci_addr_low.u32 = get_low_addr(read_addr);
    viproc_reg->viproc_para_dci_addr_high.u32 = get_high_addr(read_addr);
}

td_void isp_drv_set_ldci_write_stt_addr_offline(isp_be_all_reg_type *be_reg_cfg, td_phys_addr_t write_addr)
{
    isp_drv_set_ldci_write_stt_addr(&be_reg_cfg->post_viproc, write_addr);
}

td_u32 isp_drv_get_pre_proc_fstart_status(td_s32 vi_dev)
{
    td_u32 int_status;
    int_status = io_rw_pre_proc_address(vi_dev, ISP_INT_BE_PRE);
    return (int_status & (1 << ISP_INT_BE_PRE_FSTART_BIT)) >> ISP_INT_BE_PRE_FSTART_BIT;
}

td_void isp_drv_clear_pre_proc_fstart_int(td_s32 vi_dev)
{
    io_rw_pre_proc_address(vi_dev, ISP_INT_BE_PRE) = (1 << ISP_INT_BE_PRE_FSTART_BIT);
}

td_void isp_drv_set_pre_proc_fstart_int_mask(ot_vi_pipe vi_pipe)
{
    td_s32 vi_dev;
    isp_drv_ctx *drv_ctx = TD_NULL;
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    vi_dev = drv_ctx->wdr_attr.vi_dev;
    io_rw_pre_proc_address(vi_dev, ISP_INT_BE_PRE) |= (1 << ISP_INT_BE_PRE_FSTART_BIT); /* clear interrupt */
    io_rw_pre_proc_address(vi_dev, ISP_INT_BE_PRE_MASK) |= (1 << ISP_INT_BE_PRE_FSTART_BIT);
}

td_void isp_drv_clear_pre_proc_fstart_int_mask(ot_vi_pipe vi_pipe)
{
    td_s32 vi_dev;
    isp_drv_ctx *drv_ctx = TD_NULL;
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    vi_dev = drv_ctx->wdr_attr.vi_dev;
    io_rw_pre_proc_address(vi_dev, ISP_INT_BE_PRE) &= ~(1 << ISP_INT_BE_PRE_FSTART_BIT); /* clear interrupt */
    io_rw_pre_proc_address(vi_dev, ISP_INT_BE_PRE_MASK) &= ~(1 << ISP_INT_BE_PRE_FSTART_BIT);
}


td_s32 isp_drv_be_remap(void)
{
    td_u32 isp_be_phy_pipe;

    for (isp_be_phy_pipe = 0; isp_be_phy_pipe < ISP_MAX_BE_NUM; isp_be_phy_pipe++) {
        g_reg_isp_pre_be_base_va[isp_be_phy_pipe] =
            (void *)osal_ioremap(isp_pre_be_reg_base(isp_be_phy_pipe), (td_u32)VI_ISP_BE_REG_SIZE);

        if (g_reg_isp_pre_be_base_va[isp_be_phy_pipe] == TD_NULL) {
            osal_printk("Remap ISP BE[%d] failed!\n", isp_be_phy_pipe);
            goto exit;
        }

        g_reg_isp_post_be_base_va[isp_be_phy_pipe] =
            (void *)osal_ioremap(isp_post_be_reg_base(isp_be_phy_pipe), (td_u32)VI_ISP_BE_REG_SIZE);

        if (g_reg_isp_post_be_base_va[isp_be_phy_pipe] == TD_NULL) {
            osal_printk("Remap ISP BE[%d] failed!\n", isp_be_phy_pipe);
            goto exit;
        }

        g_reg_pre_viproc_base_va[isp_be_phy_pipe] =
            (void *)osal_ioremap(isp_pre_viproc_reg_base(isp_be_phy_pipe), (td_u32)VIPROC_REG_SIZE);

        if (g_reg_pre_viproc_base_va[isp_be_phy_pipe] == TD_NULL) {
            osal_printk("Remap isp viproc[%d] failed!\n", isp_be_phy_pipe);
            goto exit;
        }

        g_reg_post_viproc_base_va[isp_be_phy_pipe] =
            (void *)osal_ioremap(isp_post_viproc_reg_base(isp_be_phy_pipe), (td_u32)VIPROC_REG_SIZE);

        if (g_reg_post_viproc_base_va[isp_be_phy_pipe] == TD_NULL) {
            osal_printk("Remap isp viproc[%d] failed!\n", isp_be_phy_pipe);
            goto exit;
        }
    }

    return TD_SUCCESS;

exit:
    isp_drv_be_unmap();
    return TD_FAILURE;
}

td_void isp_drv_be_unmap(void)
{
    td_u32 isp_be_phy_pipe;

    for (isp_be_phy_pipe = 0; isp_be_phy_pipe < ISP_MAX_BE_NUM; isp_be_phy_pipe++) {
        if (g_reg_isp_pre_be_base_va[isp_be_phy_pipe] != TD_NULL) {
            osal_iounmap(g_reg_isp_pre_be_base_va[isp_be_phy_pipe]);
            g_reg_isp_pre_be_base_va[isp_be_phy_pipe] = TD_NULL;
        }

        if (g_reg_isp_post_be_base_va[isp_be_phy_pipe] != TD_NULL) {
            osal_iounmap(g_reg_isp_post_be_base_va[isp_be_phy_pipe]);
            g_reg_isp_post_be_base_va[isp_be_phy_pipe] = TD_NULL;
        }

        if (g_reg_pre_viproc_base_va[isp_be_phy_pipe] != TD_NULL) {
            osal_iounmap(g_reg_pre_viproc_base_va[isp_be_phy_pipe]);
            g_reg_pre_viproc_base_va[isp_be_phy_pipe] = TD_NULL;
        }

        if (g_reg_post_viproc_base_va[isp_be_phy_pipe] != TD_NULL) {
            osal_iounmap(g_reg_post_viproc_base_va[isp_be_phy_pipe]);
            g_reg_post_viproc_base_va[isp_be_phy_pipe] = TD_NULL;
        }
    }
}

td_s32 isp_drv_vicap_remap(void)
{
    g_reg_vicap_base_va = (void *)osal_ioremap(CAP_REG_BASE, (td_u32)CAP_REG_SIZE_ALIGN);
    if (g_reg_vicap_base_va == TD_NULL) {
        osal_printk("Remap ISP PT failed!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_void isp_drv_vicap_unmap(void)
{
    if (g_reg_vicap_base_va != TD_NULL) {
        osal_iounmap(g_reg_vicap_base_va);
        g_reg_vicap_base_va = TD_NULL;
    }
}

static td_void isp_drv_sys_init(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    (td_void)memset_s(drv_ctx, sizeof(isp_drv_ctx), 0, sizeof(isp_drv_ctx));
    ret = snprintf_s(drv_ctx->name, sizeof(drv_ctx->name), sizeof(drv_ctx->name) - 1, "ISP[%d]", vi_pipe);
    if (ret < 0) {
        return;
    }

    drv_ctx->work_mode.running_mode = ISP_MODE_RUNNING_OFFLINE;
    drv_ctx->work_mode.block_num = 1;
    drv_ctx->fpn_work_mode = FPN_MODE_NONE;

    drv_ctx->ai_info.pq_ai_en = TD_FALSE;
    drv_ctx->ai_info.ai_pipe_id = -1;
    drv_ctx->ai_info.base_pipe_id = -1;

    for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
        drv_ctx->chn_sel_attr[i].channel_sel = 0;
    }

    drv_ctx->running_state = ISP_BE_BUF_STATE_INIT;
    drv_ctx->exit_state = ISP_BE_BUF_EXIT;
    drv_ctx->hnr_info.hnr_en = TD_FALSE;
    drv_ctx->hnr_info.pre_hnr_en = TD_FALSE;
    drv_ctx->hnr_info.off_switch_cnt = 0;
    drv_ctx->hnr_info.normal_blend = TD_FALSE;

    (td_void)memset_s(&drv_ctx->dyna_blc_info, sizeof(isp_dynamic_blc_info), 0, sizeof(isp_dynamic_blc_info));
}

static td_void isp_drv_stitch_init(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    drv_ctx->stitch_attr.stitch_enable = TD_FALSE;
    drv_ctx->stitch_attr.stitch_pipe_num = 2; /* default: stitch num is 2 for online mode */
    drv_ctx->stitch_attr.stitch_bind_id[0] = 0;
    drv_ctx->stitch_attr.stitch_bind_id[1] = 1;      /* default: stitch pipe 0 & 1 */
    for (i = 2; i < OT_ISP_MAX_STITCH_NUM; i++) {    /* 2 */
        drv_ctx->stitch_attr.stitch_bind_id[i] = -1; /* id[2] Not use stitch pipe is -1 */
    }
}

static td_void isp_drv_snap_init(ot_vi_pipe vi_pipe)
{
#ifdef CONFIG_OT_SNAP_SUPPORT
    td_u8 i;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    drv_ctx->snap_attr.snap_type = OT_SNAP_TYPE_NORM;
    drv_ctx->snap_attr.picture_pipe_id = -1;
    drv_ctx->snap_attr.preview_pipe_id = -1;
    drv_ctx->snap_attr.load_ccm = TD_TRUE;
    drv_ctx->snap_attr.pro_param.operation_mode = OT_OP_MODE_AUTO;

    for (i = 0; i < OT_ISP_PRO_MAX_FRAME_NUM; i++) {
        drv_ctx->snap_attr.pro_param.auto_param.exp_step[i] = 0x100;
        drv_ctx->snap_attr.pro_param.manual_param.exp_time[i] = 10000; /* default value 10000 */
        drv_ctx->snap_attr.pro_param.manual_param.sys_gain[i] = 0x400;
    }

    drv_ctx->snap_attr.pro_param.pro_frame_num = 3; /* default frame num is 3 */
#else
    ot_unused(vi_pipe);
#endif
}

static td_void isp_drv_wait_sema_init(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    drv_ctx->use_node = TD_NULL;
    drv_ctx->run_once_ok = TD_FALSE;
    drv_ctx->run_once_flag = TD_FALSE;
    drv_ctx->yuv_run_once_ok = TD_FALSE;
    drv_ctx->yuv_run_once_flag = TD_FALSE;

    osal_wait_init(&drv_ctx->isp_wait);
    osal_wait_init(&drv_ctx->isp_wait_vd_start);
    osal_wait_init(&drv_ctx->isp_wait_vd_end);
    osal_wait_init(&drv_ctx->isp_wait_vd_be_end);
    osal_wait_init(&drv_ctx->isp_exit_wait);
    drv_ctx->edge = TD_FALSE;
    drv_ctx->vd_start = TD_FALSE;
    drv_ctx->vd_end = TD_FALSE;
    drv_ctx->vd_be_end = TD_FALSE;
    drv_ctx->mem_init = TD_FALSE;
    drv_ctx->isp_init = TD_FALSE;
    drv_ctx->pub_attr_ok = TD_FALSE;
    drv_ctx->isp_run_flag = TD_FALSE;
    drv_ctx->pro_trig_flag = 0;
    osal_sema_init(&drv_ctx->isp_sem, 1);
    osal_sema_init(&drv_ctx->isp_sem_vd, 1);
    osal_sema_init(&drv_ctx->isp_sem_be_vd, 1);
    osal_sema_init(&drv_ctx->proc_sem, 1);
    osal_sema_init(&drv_ctx->pro_info_sem, 1);
}

td_s32 isp_drv_fe_remap(void)
{
    ot_vi_pipe vi_pipe;

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PHY_PIPE_NUM; vi_pipe++) {
        g_reg_ispfe_base_va[vi_pipe] = (void *)osal_ioremap(isp_fe_reg_base(vi_pipe), (td_u32)VI_ISP_FE_REG_SIZE);

        if (g_reg_ispfe_base_va[vi_pipe] == TD_NULL) {
            osal_printk("Remap ISP[%d] FE failed!\n", vi_pipe);
            return TD_FAILURE;
        }

        g_reg_vicap_ch_base_va[vi_pipe] =
            (void *)osal_ioremap(isp_vicap_ch_reg_base(vi_pipe), (td_u32)VICAP_CH_REG_SIZE_ALIGN);

        if (g_reg_vicap_ch_base_va[vi_pipe] == TD_NULL) {
            osal_printk("Remap ISP[%d] FE STT failed!\n", vi_pipe);
            return TD_FAILURE;
        }

        /* enable port int & isp int */
        io_rw_pt_address(VICAP_HD_MASK) |= vicap_int_mask_pt((td_u32)vi_pipe);
        io_rw_pt_address(VICAP_HD_MASK) |= vicap_int_mask_isp((td_u32)vi_pipe);

        io_rw_fe_address(vi_pipe, ISP_INT_FE_MASK) = (0x0);
    }

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PIPE_NUM; vi_pipe++) {
        isp_drv_sys_init(vi_pipe);

        isp_drv_stitch_init(vi_pipe);

        isp_drv_snap_init(vi_pipe);

        isp_drv_wait_sema_init(vi_pipe);

        sync_task_init(vi_pipe);
    }

    return TD_SUCCESS;
}

td_void isp_drv_fe_unmap(void)
{
    ot_vi_pipe vi_pipe;
    isp_drv_ctx *drv_ctx = TD_NULL;

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PHY_PIPE_NUM; vi_pipe++) {
        if (g_reg_ispfe_base_va[vi_pipe] != TD_NULL) {
            osal_iounmap(g_reg_ispfe_base_va[vi_pipe]);
            g_reg_ispfe_base_va[vi_pipe] = TD_NULL;
        }

        if (g_reg_vicap_ch_base_va[vi_pipe] != TD_NULL) {
            osal_iounmap(g_reg_vicap_ch_base_va[vi_pipe]);
            g_reg_vicap_ch_base_va[vi_pipe] = TD_NULL;
        }
    }

    for (vi_pipe = 0; vi_pipe < OT_ISP_MAX_PIPE_NUM; vi_pipe++) {
        drv_ctx = isp_drv_get_ctx(vi_pipe);

        osal_sema_destroy(&drv_ctx->isp_sem);
        osal_sema_destroy(&drv_ctx->isp_sem_vd);
        osal_sema_destroy(&drv_ctx->isp_sem_be_vd);
        osal_sema_destroy(&drv_ctx->proc_sem);
        osal_sema_destroy(&drv_ctx->pro_info_sem);

        osal_wait_destroy(&drv_ctx->isp_wait);
        osal_wait_destroy(&drv_ctx->isp_wait_vd_start);
        osal_wait_destroy(&drv_ctx->isp_wait_vd_end);
        osal_wait_destroy(&drv_ctx->isp_wait_vd_be_end);
        osal_wait_destroy(&drv_ctx->isp_exit_wait);

        sync_task_exit(vi_pipe);
    }
}

td_s32 isp_drv_get_pre_be_reg_virt_addr(ot_vi_pipe vi_pipe, isp_pre_be_reg_type *be_reg[])
{
    td_u8 k, block_dev, striping_num;
    isp_running_mode running_mode;
    isp_be_wo_reg_cfg *isp_be_reg_cfg = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    running_mode = drv_ctx->work_mode.running_mode;
    block_dev = drv_ctx->work_mode.block_dev;
    striping_num = (running_mode == ISP_MODE_RUNNING_STRIPING) ? OT_ISP_STRIPING_MAX_NUM : 1;

    switch (running_mode) {
        case ISP_MODE_RUNNING_ONLINE:
        case ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE:
            isp_drv_pre_bereg_ctx(block_dev, be_reg[block_dev]);
            break;
        case ISP_MODE_RUNNING_OFFLINE:
        case ISP_MODE_RUNNING_STRIPING:
            isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);
            if (drv_ctx->use_node == TD_NULL) {
                isp_warn_trace("ISP[%d] UseNode is Null!\n", vi_pipe);
                return TD_FAILURE;
            }
            isp_be_reg_cfg = (isp_be_wo_reg_cfg *)drv_ctx->use_node->be_cfg_buf.vir_addr;
            for (k = 0; k < striping_num; k++) {
                be_reg[k] = &isp_be_reg_cfg->be_reg_cfg[k].pre_be;
            }
            break;
        default:
            isp_err_trace("ISP[%d] Running Mode Err!\n", vi_pipe);
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_post_be_reg_virt_addr(ot_vi_pipe vi_pipe, isp_post_be_reg_type *be_reg[])
{
    td_u8 k, block_dev, striping_num;
    isp_running_mode running_mode;
    isp_be_wo_reg_cfg *isp_be_reg_cfg = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    running_mode = drv_ctx->work_mode.running_mode;
    block_dev = drv_ctx->work_mode.block_dev;
    striping_num = (running_mode == ISP_MODE_RUNNING_STRIPING) ? OT_ISP_STRIPING_MAX_NUM : 1;

    switch (running_mode) {
        case ISP_MODE_RUNNING_ONLINE:
            isp_drv_post_bereg_ctx(block_dev, be_reg[block_dev]);
            break;
        case ISP_MODE_RUNNING_OFFLINE:
        case ISP_MODE_RUNNING_STRIPING:
        case ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE:
            isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);
            if (drv_ctx->use_node == TD_NULL) {
                isp_warn_trace("ISP[%d] UseNode is Null!\n", vi_pipe);
                return TD_FAILURE;
            }
            isp_be_reg_cfg = (isp_be_wo_reg_cfg *)drv_ctx->use_node->be_cfg_buf.vir_addr;
            for (k = 0; k < striping_num; k++) {
                be_reg[k] = &isp_be_reg_cfg->be_reg_cfg[k].post_be;
            }
            break;

        default:
            isp_err_trace("ISP[%d] Running Mode Err!\n", vi_pipe);
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_pre_viproc_reg_virt_addr(ot_vi_pipe vi_pipe, isp_viproc_reg_type *viproc_reg[])
{
    td_u8 k, block_dev, striping_num;
    isp_running_mode running_mode;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_wo_reg_cfg *isp_be_reg_cfg = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    running_mode = drv_ctx->work_mode.running_mode;
    block_dev = drv_ctx->work_mode.block_dev;
    striping_num = (running_mode == ISP_MODE_RUNNING_STRIPING) ? OT_ISP_STRIPING_MAX_NUM : 1;

    switch (running_mode) {
        case ISP_MODE_RUNNING_ONLINE:
        case ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE:
            isp_drv_pre_viprocreg_ctx(block_dev, viproc_reg[block_dev]);
            break;
        case ISP_MODE_RUNNING_OFFLINE:
        case ISP_MODE_RUNNING_STRIPING:
            isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);
            if (drv_ctx->use_node == TD_NULL) {
                isp_warn_trace("ISP[%d] UseNode is Null!\n", vi_pipe);
                return TD_FAILURE;
            }
            isp_be_reg_cfg = (isp_be_wo_reg_cfg *)drv_ctx->use_node->be_cfg_buf.vir_addr;
            for (k = 0; k < striping_num; k++) {
                viproc_reg[k] = &isp_be_reg_cfg->be_reg_cfg[k].pre_viproc;
            }
            break;

        default:
            isp_err_trace("ISP[%d] Running Mode Err!\n", vi_pipe);
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_post_viproc_reg_virt_addr(ot_vi_pipe vi_pipe, isp_viproc_reg_type *viproc_reg[])
{
    td_u8 k, block_dev, striping_num;
    isp_running_mode running_mode;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_wo_reg_cfg *isp_be_reg_cfg = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    running_mode = drv_ctx->work_mode.running_mode;
    block_dev = drv_ctx->work_mode.block_dev;
    striping_num = (running_mode == ISP_MODE_RUNNING_STRIPING) ? OT_ISP_STRIPING_MAX_NUM : 1;

    switch (running_mode) {
        case ISP_MODE_RUNNING_ONLINE:
            isp_drv_post_viprocreg_ctx(block_dev, viproc_reg[block_dev]);
            break;
        case ISP_MODE_RUNNING_OFFLINE:
        case ISP_MODE_RUNNING_STRIPING:
        case ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE:
            isp_check_bebuf_init_return(vi_pipe, drv_ctx->be_buf_info.init);
            if (drv_ctx->use_node == TD_NULL) {
                isp_warn_trace("ISP[%d] UseNode is Null!\n", vi_pipe);
                return TD_FAILURE;
            }
            isp_be_reg_cfg = (isp_be_wo_reg_cfg *)drv_ctx->use_node->be_cfg_buf.vir_addr;
            for (k = 0; k < striping_num; k++) {
                viproc_reg[k] = &isp_be_reg_cfg->be_reg_cfg[k].post_viproc;
            }
            break;

        default:
            isp_err_trace("ISP[%d] Running Mode Err!\n", vi_pipe);
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_set_online_stt_addr(ot_vi_pipe vi_pipe, td_u64 phy_base_addr)
{
    td_u8 k;
    td_u8 blk_dev;
    td_u8 blk_num;
    td_u32 ret;
    td_u64 stt_size;
    td_u64 phy_addr;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_pipe_return(vi_pipe);

    ret = isp_drv_get_post_viproc_reg_virt_addr(vi_pipe, post_viproc);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    blk_dev = drv_ctx->work_mode.block_dev;
    blk_num = drv_ctx->work_mode.block_num;

    stt_size = sizeof(isp_be_online_stat_reg_type);

    for (k = 0; k < blk_num; k++) {
        phy_addr = phy_base_addr + k * stt_size;

        /* post be ae */
        post_viproc[k + blk_dev]->ae_hist_addr_low.u32 = get_low_addr(phy_addr + BE_AE_HIST_ADDR_OFFSET);
        post_viproc[k + blk_dev]->ae_hist_addr_high.u32 = get_high_addr(phy_addr + BE_AE_HIST_ADDR_OFFSET);
        post_viproc[k + blk_dev]->ae_aver_r_gr_addr_low.u32 = get_low_addr(phy_addr + BE_AE_AVER_R_GR_ADDR_OFFSET);
        post_viproc[k + blk_dev]->ae_aver_r_gr_addr_high.u32 = get_high_addr(phy_addr + BE_AE_AVER_R_GR_ADDR_OFFSET);
        post_viproc[k + blk_dev]->ae_aver_gb_b_addr_low.u32 = get_low_addr(phy_addr + BE_AE_AVER_GB_B_ADDR_OFFSET);
        post_viproc[k + blk_dev]->ae_aver_gb_b_addr_high.u32 = get_high_addr(phy_addr + BE_AE_AVER_GB_B_ADDR_OFFSET);

        /* awb */
        post_viproc[k + blk_dev]->awb_stat_addr_low.u32 = get_low_addr(phy_addr + BE_AWB_STAT_ADDR_OFFSET);
        post_viproc[k + blk_dev]->awb_stat_addr_high.u32 = get_high_addr(phy_addr + BE_AWB_STAT_ADDR_OFFSET);
        /* af */
        post_viproc[k + blk_dev]->af_stat_addr_low.u32 = get_low_addr(phy_addr + BE_AF_STAT_ADDR_OFFSET);
        post_viproc[k + blk_dev]->af_stat_addr_high.u32 = get_high_addr(phy_addr + BE_AF_STAT_ADDR_OFFSET);
        /* la */
        post_viproc[k + blk_dev]->la_aver_addr_low.u32 = get_low_addr(phy_addr + BE_LA_AVER_ADDR_OFFSET);
        post_viproc[k + blk_dev]->la_aver_addr_high.u32 = get_high_addr(phy_addr + BE_LA_AVER_ADDR_OFFSET);
        /* dehaze */
        post_viproc[k + blk_dev]->dehaze_min_stat_addr_low.u32 = get_low_addr(phy_addr + BE_DEHAZE_MIN_ADDR_OFFSET);
        post_viproc[k + blk_dev]->dehaze_min_stat_addr_high.u32 = get_high_addr(phy_addr + BE_DEHAZE_MIN_ADDR_OFFSET);
        post_viproc[k + blk_dev]->dehaze_max_stat_addr_low.u32 = get_low_addr(phy_addr + BE_DEHAZE_MAX_ADDR_OFFSET);
        post_viproc[k + blk_dev]->dehaze_max_stat_addr_high.u32 = get_high_addr(phy_addr + BE_DEHAZE_MAX_ADDR_OFFSET);
        /* ldci */
        post_viproc[k + blk_dev]->ldci_lpf_addr_low.u32 = get_low_addr(phy_addr + BE_LDCI_LPF_MAP_ADDR_OFFSET);
        post_viproc[k + blk_dev]->ldci_lpf_addr_high.u32 = get_high_addr(phy_addr + BE_LDCI_LPF_MAP_ADDR_OFFSET);
        /* awb_lsc */
        post_viproc[k + blk_dev]->awblsc_addr_low.u32 = get_low_addr(phy_addr + BE_AWB_LSC_ADDR_OFFSET);
        post_viproc[k + blk_dev]->awblsc_addr_high.u32 = get_high_addr(phy_addr + BE_AWB_LSC_ADDR_OFFSET);
    }

    return TD_SUCCESS;
}

td_void isp_drv_set_fe_stt_addr(ot_vi_pipe vi_pipe_bind, td_u64 phy_addr)
{
    isp_vicap_ch_reg_type *fe_stt_addr_reg = TD_NULL;

    if (is_virt_pipe(vi_pipe_bind)) {
        isp_info_trace("Dont support virtual pipe %d to set fe stt addr !\n", vi_pipe_bind);
        return;
    }

    isp_drv_festtreg_ctx(vi_pipe_bind, fe_stt_addr_reg);

    fe_stt_addr_reg->ch_wch_stt_ae_hist_addr_l.u32 = get_low_addr(phy_addr + FE_AE_HIST_ADDR_OFFSET);
    fe_stt_addr_reg->ch_wch_stt_ae_hist_addr_h.u32 = get_high_addr(phy_addr + FE_AE_HIST_ADDR_OFFSET);

    fe_stt_addr_reg->ch_wch_stt_ae_aver_r_gr_addr_l.u32 = get_low_addr(phy_addr + FE_AE_AVER_R_GR_ADDR_OFFSET);
    fe_stt_addr_reg->ch_wch_stt_ae_aver_r_gr_addr_h.u32 = get_high_addr(phy_addr + FE_AE_AVER_R_GR_ADDR_OFFSET);

    fe_stt_addr_reg->ch_wch_stt_ae_aver_gb_b_addr_l.u32 = get_low_addr(phy_addr + FE_AE_AVER_GB_B_ADDR_OFFSET);
    fe_stt_addr_reg->ch_wch_stt_ae_aver_gb_b_addr_h.u32 = get_high_addr(phy_addr + FE_AE_AVER_GB_B_ADDR_OFFSET);

    fe_stt_addr_reg->ch_wch_stt_af_stat_addr_l.u32 = get_low_addr(phy_addr + FE_AF_STAT_ADDR_OFFSET);
    fe_stt_addr_reg->ch_wch_stt_af_stat_addr_h.u32 = get_high_addr(phy_addr + FE_AF_STAT_ADDR_OFFSET);

    if (vi_pipe_bind == 0) {
        fe_stt_addr_reg->ch_wch_stt_ae_ir_hist_addr_l.u32 = get_low_addr(phy_addr + FE_AE_IR_HIST_ADDR_OFFSET);
        fe_stt_addr_reg->ch_wch_stt_ae_ir_hist_addr_h.u32 = get_high_addr(phy_addr + FE_AE_IR_HIST_ADDR_OFFSET);

        fe_stt_addr_reg->ch_wch_stt_ae_ir_aver_addr_l.u32 = get_low_addr(phy_addr + FE_AE_IR_AVER_ADDR_OFFSET);
        fe_stt_addr_reg->ch_wch_stt_ae_ir_aver_addr_h.u32 = get_high_addr(phy_addr + FE_AE_IR_AVER_ADDR_OFFSET);
    }

    fe_stt_addr_reg->ch_reg_newer.bits.ch_reg_newer = 1;
}

td_s32 isp_drv_set_ldci_stt_addr(ot_vi_pipe vi_pipe, td_phys_addr_t read_stt_addr, td_phys_addr_t write_stt_addr)
{
    td_u8 k, blk_dev, blk_num;
    td_s32 ret;
    td_u32 size = sizeof(isp_ldci_stat);
    td_phys_addr_t write_addr;
    isp_viproc_reg_type *viproc_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = isp_drv_get_post_viproc_reg_virt_addr(vi_pipe, viproc_reg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Get ViprocRegs Attr Err!\n", vi_pipe);
        return TD_FAILURE;
    }

    blk_dev = drv_ctx->work_mode.block_dev;
    blk_num = drv_ctx->work_mode.block_num;
    for (k = 0; k < blk_num; k++) {
        write_addr = write_stt_addr + k * (size / div_0_to_1(blk_num));
        isp_drv_set_ldci_write_stt_addr(viproc_reg[k + blk_dev], write_addr);

        if (isp_drv_get_ldci_tpr_flt_en(vi_pipe) == TD_FALSE) {
            isp_drv_set_ldci_read_stt_addr(viproc_reg[k + blk_dev], read_stt_addr);
        }
    }

    return TD_SUCCESS;
}

td_void isp_drv_be_cfg_buf_addr_init(isp_be_buf_node *node, td_phys_addr_t phy_addr, td_u8 *vir_addr, td_u64 size,
    td_u8 i)
{
    td_u8 j;
    td_phys_addr_t buf_size;

    buf_size = sizeof(isp_be_wo_reg_cfg) / OT_ISP_STRIPING_MAX_NUM;

    /* every block's head addr */
    node->be_cfg_buf.phy_addr = phy_addr + (td_phys_addr_t)(i * size);

    node->be_cfg_buf.vir_addr = (td_void *)(vir_addr + i * size);
    node->be_cfg_buf.size = size;
    node->hold_cnt = 0;

    /* pre be */
    for (j = 0; j < OT_ISP_STRIPING_MAX_NUM; j++) {
        node->be_cfg_buf.pre_phy_addr[j] = node->be_cfg_buf.phy_addr + PRE_BE_OFFLINE_OFFSET + j * buf_size;
        node->be_cfg_buf.pre_vir_addr[j] =
            (td_void *)((td_u8 *)node->be_cfg_buf.vir_addr + PRE_BE_OFFLINE_OFFSET + j * buf_size);
    }

    node->be_cfg_buf.pre_size = PRE_BE_SIZE;

    for (j = 0; j < OT_ISP_STRIPING_MAX_NUM; j++) {
        node->be_cfg_buf.post_phy_addr[j] = node->be_cfg_buf.phy_addr + POST_BE_OFFLINE_OFFSET + j * buf_size;
        node->be_cfg_buf.post_vir_addr[j] =
            (td_void *)((td_u8 *)node->be_cfg_buf.vir_addr + POST_BE_OFFLINE_OFFSET + j * buf_size);
    }

    node->be_cfg_buf.post_size = POST_BE_SIZE;

    for (j = 0; j < OT_ISP_STRIPING_MAX_NUM; j++) {
        node->be_cfg_buf.post_viproc_vir_addr[j] =
            (td_void *)((td_u8 *)node->be_cfg_buf.vir_addr + POST_VIPROC_OFFLINE_OFFSET + j * buf_size);
        node->be_cfg_buf.pre_viproc_vir_addr[j] =
            (td_void *)((td_u8 *)node->be_cfg_buf.vir_addr + PRE_VIPROC_OFFLINE_OFFSET + j * buf_size);
    }
}

td_s32 isp_drv_pre_on_post_off_lut_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret;
    td_u8 *vir_addr = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    unsigned long flags;
    td_char ac_name[MAX_MMZ_NAME_LEN] = {0};
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_pre_on_post_off_lut2stt_attr.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "PreOnLutStt[%d]", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_pre_be_lut_wstt_type);

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr,
        size * 2 * ISP_MAX_BE_NUM); /* get memory size (*2) */
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP be Pre On Post Off Lut stt buf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size * 2 * ISP_MAX_BE_NUM, 0, size * 2 * ISP_MAX_BE_NUM); /* get memory size (*2) */
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->be_pre_on_post_off_lut2stt_attr.init = TD_TRUE;

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].phy_addr =
            phy_addr + 2 * i * size; /* get phyaddr index(*2) */
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].vir_addr =
            (td_void *)(vir_addr + 2 * i * size); /* get viraddr index(*2) */
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].size = size;

        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].phy_addr =
            phy_addr + (2 * i + 1) * size; /* get phyaddr index(*2) */
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].vir_addr =
            (td_void *)(vir_addr + (2 * i + 1) * size); /* get viraddr index(*2) */
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].size = size;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_pre_on_post_off_lut_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    unsigned long flags;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->be_pre_on_post_off_lut2stt_attr.init);

    phy_addr = drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[0].lut_stt_buf[0].phy_addr;
    vir_addr = drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[0].lut_stt_buf[0].vir_addr;

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->be_pre_on_post_off_lut2stt_attr.init = TD_FALSE;

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].phy_addr = 0;
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].size = 0;
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].vir_addr = TD_NULL;

        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].phy_addr = 0;
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].size = 0;
        drv_ctx->be_pre_on_post_off_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].vir_addr = TD_NULL;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_be_lut_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret;
    td_u8 *vir_addr = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    unsigned long flags;
    td_char ac_name[MAX_MMZ_NAME_LEN] = {0};
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_lut2stt_attr.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "BeLutStt[%d]", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);

    ret = cmpi_mmz_malloc_nocache(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr,
        size * 2 * ISP_MAX_BE_NUM); /* get memory size (*2) */
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP be Lut stt buf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size * 2 * ISP_MAX_BE_NUM, 0, size * 2 * ISP_MAX_BE_NUM); /* get memory size (*2) */
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->be_lut2stt_attr.init = TD_TRUE;

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].phy_addr =
            phy_addr + 2 * i * size; /* get phyaddr index(*2) */
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].vir_addr =
            (td_void *)(vir_addr + 2 * i * size); /* get viraddr index(*2) */
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].size = size;

        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].phy_addr =
            phy_addr + (2 * i + 1) * size; /* get phyaddr index(*2) */
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].vir_addr =
            (td_void *)(vir_addr + (2 * i + 1) * size); /* get viraddr index(*2) */
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].size = size;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_be_lut_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    unsigned long flags;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->be_lut2stt_attr.init);

    phy_addr = drv_ctx->be_lut2stt_attr.be_lut_stt_buf[0].lut_stt_buf[0].phy_addr;
    vir_addr = drv_ctx->be_lut2stt_attr.be_lut_stt_buf[0].lut_stt_buf[0].vir_addr;

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->be_lut2stt_attr.init = TD_FALSE;

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].phy_addr = 0;
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].size = 0;
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[0].vir_addr = TD_NULL;

        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].phy_addr = 0;
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].size = 0;
        drv_ctx->be_lut2stt_attr.be_lut_stt_buf[i].lut_stt_buf[1].vir_addr = TD_NULL;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_be_online_stt_buf_init_fail(isp_drv_ctx *drv_ctx)
{
    td_u32 i;
    drv_ctx->be_online_stt_buf.init = TD_FALSE;

    for (i = 0; i < PING_PONG_NUM; i++) {
        drv_ctx->be_online_stt_buf.be_stt_buf[i].phy_addr = 0;
        drv_ctx->be_online_stt_buf.be_stt_buf[i].vir_addr = TD_NULL;
        drv_ctx->be_online_stt_buf.be_stt_buf[i].size = 0;
    }
}

td_s32 isp_drv_be_online_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret;
    td_u8 *vir_addr = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size;
    unsigned long flags;
    td_char ac_name[MAX_MMZ_NAME_LEN] = {0};
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_online_stt_buf.init);

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].BeSttOn", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_rw_online_stt_reg);

    ret = cmpi_mmz_malloc_cached(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size * PING_PONG_NUM);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP be online stt buf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size * PING_PONG_NUM, 0, size * PING_PONG_NUM);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->be_online_stt_buf.init = TD_TRUE;

    for (i = 0; i < PING_PONG_NUM; i++) {
        drv_ctx->be_online_stt_buf.be_stt_buf[i].phy_addr = phy_addr + i * size;
        drv_ctx->be_online_stt_buf.be_stt_buf[i].vir_addr = (td_void *)(vir_addr + i * size);
        drv_ctx->be_online_stt_buf.be_stt_buf[i].size = size;
    }

    drv_ctx->be_online_stt_buf.cur_write_flag = 0;
    drv_ctx->be_online_stt_buf.get_first_stat_info_flag = TD_FALSE;
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    ret = isp_drv_set_online_stt_addr(vi_pipe, drv_ctx->be_online_stt_buf.be_stt_buf[0].phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Set ISP online stt addr Err!\n", vi_pipe);
        goto fail0;
    }

    return TD_SUCCESS;

fail0:
    isp_drv_be_online_stt_buf_init_fail(drv_ctx);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, (td_void *)vir_addr);
    }

    return TD_FAILURE;
}

td_s32 isp_drv_be_online_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_phys_addr_t phy_addr;
    td_void *vir_addr = TD_NULL;
    unsigned long flags;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    phy_addr = drv_ctx->be_online_stt_buf.be_stt_buf[0].phy_addr;
    vir_addr = drv_ctx->be_online_stt_buf.be_stt_buf[0].vir_addr;
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->be_online_stt_buf.init = TD_FALSE;

    for (i = 0; i < PING_PONG_NUM; i++) {
        drv_ctx->be_online_stt_buf.be_stt_buf[i].phy_addr = 0;
        drv_ctx->be_online_stt_buf.be_stt_buf[i].size = 0;
        drv_ctx->be_online_stt_buf.be_stt_buf[i].vir_addr = TD_NULL;
    }

    drv_ctx->be_online_stt_buf.cur_write_flag = 0;
    drv_ctx->be_online_stt_buf.get_first_stat_info_flag = TD_FALSE;
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    if (phy_addr != 0) {
        cmpi_mmz_free(phy_addr, vir_addr);
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_fe_stt_addr_init(ot_vi_pipe vi_pipe)
{
    td_u8 chn_num_max, k;
    ot_vi_pipe vi_pipe_bind;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_vicap_ch_reg_type *fe_stt_addr_reg = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_vir_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->fe_stt_attr.init);

    chn_num_max = drv_ctx->wdr_attr.pipe_num;

    if (chn_num_max > ISP_WDR_CHN_MAX) {
        isp_err_trace("unsupported %dTo1 WDR\n", chn_num_max);
        return TD_FAILURE;
    }

    for (k = 0; k < chn_num_max; k++) {
        drv_ctx->fe_stt_attr.fe_stt[k].first_frame = TD_TRUE;
        vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];

        isp_check_vir_pipe_return(vi_pipe_bind);

        isp_drv_set_fe_stt_addr(vi_pipe_bind, drv_ctx->fe_stt_attr.fe_stt[k].fe_stt_buf[0].phy_addr);

        isp_drv_festtreg_ctx(vi_pipe_bind, fe_stt_addr_reg);
        fe_stt_addr_reg->ch_reg_newer.bits.ch_reg_newer = 1;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_normal_fe_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_u32 i, j;
    td_u8 *vir_addr = TD_NULL;
    td_phys_addr_t phy_addr;
    td_u64 size, size_temp;
    unsigned long flags;
    td_char ac_name[MAX_MMZ_NAME_LEN] = {0};
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->fe_stt_attr.init);

    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        return TD_SUCCESS;
    }

    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].FeStt", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_fe_stat_reg_type);

    size_temp = size * 2 * ISP_WDR_CHN_MAX; /* get memory size (*2) */
    ret = cmpi_mmz_malloc_cached(TD_NULL, ac_name, &phy_addr, (td_void **)&vir_addr, size_temp);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP fe stt buf[%d] err\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(vir_addr, size_temp, 0, size_temp);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    drv_ctx->fe_stt_attr.init = TD_TRUE;

    for (i = 0; i < ISP_WDR_CHN_MAX; i++) {
        for (j = 0; j < PING_PONG_NUM; j++) {
            drv_ctx->fe_stt_attr.fe_stt[i].fe_stt_buf[j].phy_addr =
                phy_addr + (2 * i + j) * size; /* get array index (*2) */
            drv_ctx->fe_stt_attr.fe_stt[i].fe_stt_buf[j].vir_addr =
                (td_void *)(vir_addr + (2 * i + j) * size); /* get array index (*2) */
            drv_ctx->fe_stt_attr.fe_stt[i].fe_stt_buf[j].size = size;
        }

        drv_ctx->fe_stt_attr.fe_stt[i].cur_write_flag = 0;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
td_s32 isp_drv_fe_stitch_stt_addr_init(ot_vi_pipe vi_pipe)
{
    td_u8 i, j;
    td_u8 stit_pipe_num;
    ot_vi_pipe wdr_pipe_bind, stit_pipe_bind;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_bind_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->fe_stit_stt_attr.init);

    if (drv_ctx->stitch_attr.main_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    stit_pipe_num = drv_ctx->stitch_attr.stitch_pipe_num;

    for (i = 0; i < stit_pipe_num; i++) {
        stit_pipe_bind = drv_ctx->stitch_attr.stitch_bind_id[i];

        isp_check_pipe_return(stit_pipe_bind);

        drv_bind_ctx = isp_drv_get_ctx(stit_pipe_bind);

        for (j = 0; j < drv_ctx->wdr_attr.pipe_num; j++) {
            drv_ctx->fe_stit_stt_attr.fe_stt[i][j].first_frame = TD_TRUE;

            wdr_pipe_bind = drv_bind_ctx->wdr_attr.pipe_id[j];

            isp_check_pipe_return(wdr_pipe_bind);

            isp_drv_set_fe_stt_addr(wdr_pipe_bind, drv_ctx->fe_stit_stt_attr.fe_stt[i][j].fe_stt_buf[0].phy_addr);
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_stitch_fe_stt_malloc_save_buf(ot_vi_pipe vi_pipe)
{
    td_u8 stit_pipe_num;
    td_u32 i, k;
    td_u64 size, total_size, offset;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    stit_pipe_num = drv_ctx->stitch_attr.stitch_pipe_num;
    size = sizeof(isp_fe_stat_reg_type);
    total_size = size * stit_pipe_num * ISP_WDR_CHN_MAX; /* save_buffer no need pingpong */

    if (drv_ctx->save_stt_stat == TD_NULL) {
        drv_ctx->save_stt_stat = (td_u8 *)osal_vmalloc(total_size);

        if (drv_ctx->save_stt_stat == TD_NULL) {
            isp_err_trace("vmalloc for Save SttStat failed!\n");
            return OT_ERR_ISP_NOMEM;
        }

        (td_void)memset_s(drv_ctx->save_stt_stat, total_size, 0, total_size);
    }

    for (k = 0; k < stit_pipe_num; k++) {
        for (i = 0; i < ISP_WDR_CHN_MAX; i++) {
            offset = (k * ISP_WDR_CHN_MAX + i) * size;
            drv_ctx->fe_stit_stt_attr.save_stt_stat[k][i] = (isp_fe_stat_reg_type *)(drv_ctx->save_stt_stat + offset);
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_stitch_fe_stt_malloc_main_buf(ot_vi_pipe vi_pipe)
{
    td_u8 stit_pipe_num;
    td_s32 i, k, j, ret;
    td_u64 size, total_size, addr_offset;
    unsigned long flags;
    td_phys_addr_t phys_addr;
    td_char ac_name[MAX_MMZ_NAME_LEN] = { 0 };
    td_u8 *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_sync_lock = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    ret = snprintf_s(ac_name, sizeof(ac_name), sizeof(ac_name) - 1, "ISP[%d].StitFe", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }
    stit_pipe_num = drv_ctx->stitch_attr.stitch_pipe_num;
    size = sizeof(isp_fe_stat_reg_type);
    total_size = size * PING_PONG_NUM * stit_pipe_num * ISP_WDR_CHN_MAX;

    ret = cmpi_mmz_malloc_cached(TD_NULL, ac_name, &phys_addr, (td_void **)&virt_addr, total_size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP stitch fe stt buf failed!\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(virt_addr, total_size, 0, total_size);
    isp_sync_lock = isp_drv_get_sync_lock(vi_pipe);

    osal_spin_lock_irqsave(isp_sync_lock, &flags);

    drv_ctx->fe_stit_stt_attr.init = TD_TRUE;
    for (k = 0; k < drv_ctx->stitch_attr.stitch_pipe_num; k++) {
        for (i = 0; i < ISP_WDR_CHN_MAX; i++) {
            for (j = 0; j < PING_PONG_NUM; j++) {
                addr_offset = ((k * ISP_WDR_CHN_MAX + i) * PING_PONG_NUM + j) * size;
                drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[j].phy_addr = phys_addr + addr_offset;
                drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[j].vir_addr = (td_void *)(virt_addr + addr_offset);
                drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[j].size = size;
            }

            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].cur_write_flag = 0;
        }
    }

    osal_spin_unlock_irqrestore(isp_sync_lock, &flags);

    return TD_SUCCESS;
}

static td_s32 isp_drv_stitch_fe_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 i, k, ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->fe_stit_stt_attr.init);
    if (drv_ctx->stitch_attr.main_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    ret = isp_drv_stitch_fe_stt_malloc_save_buf(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_drv_stitch_fe_stt_malloc_main_buf(vi_pipe);
    if (ret != TD_SUCCESS) {
        goto fail0;
    }

    return TD_SUCCESS;

fail0:
    for (k = 0; k < OT_ISP_MAX_STITCH_NUM; k++) {
        for (i = 0; i < ISP_WDR_CHN_MAX; i++) {
            drv_ctx->fe_stit_stt_attr.save_stt_stat[k][i] = TD_NULL;
        }
    }

    if (drv_ctx->save_stt_stat != TD_NULL) {
        osal_vfree(drv_ctx->save_stt_stat);
        drv_ctx->save_stt_stat = TD_NULL;
    }

    return OT_ERR_ISP_NOMEM;
}
#endif

td_s32 isp_drv_fe_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_vir_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->stitch_attr.stitch_enable == TD_FALSE) {
        ret = isp_drv_normal_fe_stt_buf_init(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_warn_trace("isp_drv_normal_fe_stt_buf_init failure!\n");
            return ret;
        }
    } else {
#ifdef CONFIG_OT_VI_STITCH_GRP
        ret = isp_drv_stitch_fe_stt_buf_init(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_warn_trace("isp_drv_stitch_fe_stt_buf_init failure!\n");
            return ret;
        }
#endif
    }

    return TD_SUCCESS;
}

static td_void isp_drv_normal_fe_buf_ctx_reset(isp_drv_ctx *drv_ctx)
{
    td_u32 i, j;
    drv_ctx->fe_stt_attr.init = TD_FALSE;

    for (i = 0; i < ISP_WDR_CHN_MAX; i++) {
        for (j = 0; j < PING_PONG_NUM; j++) {
            drv_ctx->fe_stt_attr.fe_stt[i].fe_stt_buf[j].phy_addr = 0;
            drv_ctx->fe_stt_attr.fe_stt[i].fe_stt_buf[j].size = 0;
            drv_ctx->fe_stt_attr.fe_stt[i].fe_stt_buf[j].vir_addr = TD_NULL;
        }

        drv_ctx->fe_stt_attr.fe_stt[i].cur_write_flag = 0;
        drv_ctx->fe_stt_attr.fe_stt[i].first_frame = TD_FALSE;
    }
    return;
}
static td_void isp_drv_stitch_fe_buf_ctx_reset(isp_drv_ctx *drv_ctx)
{
    td_u32 i, k;

    drv_ctx->fe_stit_stt_attr.init = TD_FALSE;

    for (k = 0; k < OT_ISP_MAX_STITCH_NUM; k++) {
        for (i = 0; i < ISP_WDR_CHN_MAX; i++) {
            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[0].phy_addr = 0;
            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[0].size = 0;
            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[0].vir_addr = TD_NULL;

            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[1].phy_addr = 0;
            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[1].size = 0;
            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].fe_stt_buf[1].vir_addr = TD_NULL;

            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].cur_write_flag = 0;
            drv_ctx->fe_stit_stt_attr.fe_stt[k][i].first_frame = TD_FALSE;

            drv_ctx->fe_stit_stt_attr.save_stt_stat[k][i] = TD_NULL;
        }
    }
}

td_s32 isp_drv_fe_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u64 phy_addr, phy_stitch_addr;
    td_void *vir_addr = TD_NULL;
    td_void *stitch_vir_addr = TD_NULL;
    unsigned long flags;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    osal_spinlock_t *isp_sync_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_vir_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);
    if (drv_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    phy_addr = drv_ctx->fe_stt_attr.fe_stt[0].fe_stt_buf[0].phy_addr;
    vir_addr = drv_ctx->fe_stt_attr.fe_stt[0].fe_stt_buf[0].vir_addr;
    phy_stitch_addr = drv_ctx->fe_stit_stt_attr.fe_stt[0][0].fe_stt_buf[0].phy_addr;
    stitch_vir_addr = drv_ctx->fe_stit_stt_attr.fe_stt[0][0].fe_stt_buf[0].vir_addr;
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    if (drv_ctx->stitch_attr.stitch_enable == TD_FALSE) {
        isp_check_buf_exit_return(vi_pipe, drv_ctx->fe_stt_attr.init);
        osal_spin_lock_irqsave(isp_spin_lock, &flags);
        isp_drv_normal_fe_buf_ctx_reset(drv_ctx);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        if (phy_addr != 0) {
            cmpi_mmz_free(phy_addr, vir_addr);
        }
    } else {
        if (drv_ctx->stitch_attr.main_pipe != TD_TRUE) {
            return TD_SUCCESS;
        }
        isp_check_buf_exit_return(vi_pipe, drv_ctx->fe_stit_stt_attr.init);
        isp_sync_lock = isp_drv_get_sync_lock(vi_pipe);
        osal_spin_lock_irqsave(isp_sync_lock, &flags);
        isp_drv_stitch_fe_buf_ctx_reset(drv_ctx);
        osal_spin_unlock_irqrestore(isp_sync_lock, &flags);

        if (phy_stitch_addr != 0) {
            cmpi_mmz_free(phy_stitch_addr, stitch_vir_addr);
        }

        if (drv_ctx->save_stt_stat != TD_NULL) {
            osal_vfree(drv_ctx->save_stt_stat);
            drv_ctx->save_stt_stat = TD_NULL;
        }
    }

    return TD_SUCCESS;
}

static td_void isp_drv_be_offline_stt_ctx_init(td_phys_addr_t phys_addr, td_u8 *virt_addr, td_u32 split_num,
    td_u64 size, isp_be_offline_stt_attr *stt_buf)
{
    td_u32 i, j;
    stt_buf->init = TD_TRUE;
    for (i = 0; i < PING_PONG_NUM; i++) {
        for (j = 0; j < split_num; j++) {
            stt_buf->be_stt_buf[i].phys_addr[j] = phys_addr + (td_phys_addr_t)(size * (split_num * i + j));
            stt_buf->be_stt_buf[i].virt_addr[j] = (td_void *)(virt_addr + (size * (split_num * i + j)));
        }
    }
    for (i = 0; i < VI_PROC_IP_NUM; i++) {
        stt_buf->working_be_stt_idx[i] = 0;
        stt_buf->last_ready_be_stt_idx[i] = 1;
        for (j = 0; j < PING_PONG_NUM; j++) {
            stt_buf->pts[i][j] = 0;
        }
    }
    return;
}

static td_void isp_drv_be_offline_stt_ctx_reset(isp_be_offline_stt_attr *stt_buf)
{
    td_u32 i, j;

    stt_buf->init = TD_FALSE;
    for (i = 0; i < PING_PONG_NUM; i++) {
        for (j = 0; j < OT_ISP_STRIPING_MAX_NUM; j++) {
            stt_buf->be_stt_buf[i].phys_addr[j] = 0;
            stt_buf->be_stt_buf[i].virt_addr[j] = TD_NULL;
        }
    }
    for (i = 0; i < VI_PROC_IP_NUM; i++) {
        stt_buf->working_be_stt_idx[i] = 0;
        stt_buf->last_ready_be_stt_idx[i] = -1;
        for (j = 0; j < PING_PONG_NUM; j++) {
            stt_buf->pts[i][j] = 0;
        }
    }
    return;
}

static td_s32 isp_drv_be_offline_stt_malloc_buf(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 ret;
    td_u32 split_num;
    td_u64 size, total_size;
    unsigned long flags;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    td_char mmb_name[MAX_MMZ_NAME_LEN] = {0};
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    ret = snprintf_s(mmb_name, sizeof(mmb_name), sizeof(mmb_name) - 1, "ISP[%d].BeStt", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }
    split_num = OT_ISP_STRIPING_MAX_NUM;
    size = sizeof(isp_be_offline_stat_reg_type);
    size = OT_ALIGN_UP(size, 256); /* 256 align */
    total_size = size * split_num * PING_PONG_NUM;

    ret = cmpi_mmz_malloc_cached(TD_NULL, mmb_name, &phys_addr, (td_void **)&virt_addr, total_size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP BeSttBuf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(virt_addr, total_size, 0, total_size);

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    isp_drv_be_offline_stt_ctx_init(phys_addr, virt_addr, split_num, size, &drv_ctx->be_off_stt_buf);
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

static td_s32 isp_drv_be_offline_stitch_stt_malloc_buf(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u32 i, j;
    td_s32 ret;
    td_u64 size, total_size, addr_offset;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    unsigned long flags;
    td_char mmb_name[MAX_MMZ_NAME_LEN] = {0};
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    if ((drv_ctx->stitch_attr.stitch_enable != TD_TRUE) || (drv_ctx->stitch_attr.main_pipe != TD_TRUE)) {
        return TD_SUCCESS;
    }

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_off_stitch_stt_buf.init);

    ret = snprintf_s(mmb_name, sizeof(mmb_name), sizeof(mmb_name) - 1, "ISP[%d].StitBe", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_stitch_stat_reg_type);
    size = OT_ALIGN_UP(size, 256); /* 256 align */
    total_size = size * drv_ctx->stitch_attr.stitch_pipe_num * MAX_ISP_STITCH_STAT_BUF_NUM;

    ret = cmpi_mmz_malloc_cached(TD_NULL, mmb_name, &phys_addr, (td_void **)&virt_addr, total_size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP stitch statistics buf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(virt_addr, total_size, 0, total_size);

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    drv_ctx->be_off_stitch_stt_buf.init = TD_TRUE;
    drv_ctx->be_off_stitch_stt_buf.is_ready[0] = TD_FALSE;
    drv_ctx->be_off_stitch_stt_buf.is_ready[1] = TD_FALSE;
    drv_ctx->be_off_stitch_stt_buf.working_idx = 0;
    for (i = 0; i < OT_ISP_MAX_STITCH_NUM; i++) {
        drv_ctx->be_off_stitch_stt_buf.sync_ok_index[0][i] = -1;
        drv_ctx->be_off_stitch_stt_buf.sync_ok_index[1][i] = -1;
        drv_ctx->be_off_stitch_stt_buf.lost_frame[i] = TD_FALSE;
    }
    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        for (j = 0; j < MAX_ISP_STITCH_STAT_BUF_NUM; j++) {
            addr_offset = size * (i * MAX_ISP_STITCH_STAT_BUF_NUM + j);
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].phys_addr[j] = phys_addr + addr_offset;
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].stitch_stt_reg[j] =
                (isp_stitch_stat_reg_type *)(virt_addr + addr_offset);
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].pts[j] = 0;
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].finish[j] = TD_FALSE;
        }
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

static td_void isp_drv_be_offline_stitch_stt_exit(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    ot_unused(vi_pipe);

    if ((drv_ctx->stitch_attr.stitch_enable != TD_TRUE) || (drv_ctx->stitch_attr.main_pipe != TD_TRUE)) {
        return;
    }
    if (drv_ctx->be_off_stitch_stt_buf.init != TD_TRUE) {
        return;
    }

    phys_addr = drv_ctx->be_off_stitch_stt_buf.sync_be_buf[0].phys_addr[0];
    virt_addr = (td_u8 *)drv_ctx->be_off_stitch_stt_buf.sync_be_buf[0].stitch_stt_reg[0];
    drv_ctx->be_off_stitch_stt_buf.init = TD_FALSE;
    drv_ctx->be_off_stitch_stt_buf.is_ready[0] = TD_FALSE;
    drv_ctx->be_off_stitch_stt_buf.is_ready[1] = TD_FALSE;
    drv_ctx->be_off_stitch_stt_buf.working_idx = 0;
    for (i = 0; i < OT_ISP_MAX_STITCH_NUM; i++) {
        drv_ctx->be_off_stitch_stt_buf.sync_ok_index[0][i] = -1;
        drv_ctx->be_off_stitch_stt_buf.sync_ok_index[1][i] = -1;
        drv_ctx->be_off_stitch_stt_buf.lost_frame[i] = TD_FALSE;
    }
    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        for (j = 0; j < MAX_ISP_STITCH_STAT_BUF_NUM; j++) {
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].phys_addr[j] = 0;
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].stitch_stt_reg[j] = TD_NULL;
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].pts[j] = 0;
            drv_ctx->be_off_stitch_stt_buf.sync_be_buf[i].finish[j] = TD_FALSE;
        }
    }

    if (phys_addr != 0) {
        cmpi_mmz_free(phys_addr, virt_addr);
    }

    return;
}

td_s32 isp_drv_be_offline_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_off_stt_buf.init);

    ret = isp_drv_be_offline_stt_malloc_buf(vi_pipe, drv_ctx);
    if (ret != TD_SUCCESS) {
        isp_err_trace("isp[%d] isp_drv_be_offline_stt_malloc_buf failed\n", vi_pipe);
        return ret;
    }

    ret = isp_drv_be_offline_stitch_stt_malloc_buf(vi_pipe, drv_ctx);
    if (ret != TD_SUCCESS) {
        isp_err_trace("isp[%d] isp_drv_be_offline_stitch_stt_malloc_buf failed\n", vi_pipe);
        goto fail0;
    }

    return TD_SUCCESS;

fail0:
    phys_addr = drv_ctx->be_off_stt_buf.be_stt_buf[0].phys_addr[0];
    virt_addr = drv_ctx->be_off_stt_buf.be_stt_buf[0].virt_addr[0];
    isp_drv_be_offline_stt_ctx_reset(&drv_ctx->be_off_stt_buf);
    if (phys_addr != 0) {
        cmpi_mmz_free(phys_addr, virt_addr);
    }
    return TD_FAILURE;
}

td_s32 isp_drv_be_offline_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u64 phys_addr;
    unsigned long flags;
    td_void *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->be_off_stt_buf.init);

    phys_addr = drv_ctx->be_off_stt_buf.be_stt_buf[0].phys_addr[0];
    virt_addr = drv_ctx->be_off_stt_buf.be_stt_buf[0].virt_addr[0];

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    isp_drv_be_offline_stt_ctx_reset(&drv_ctx->be_off_stt_buf);
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    if (phys_addr != 0) {
        cmpi_mmz_free(phys_addr, virt_addr);
    }
    isp_drv_be_offline_stitch_stt_exit(vi_pipe, drv_ctx);

    return TD_SUCCESS;
}

static td_s32 isp_drv_be_pre_online_post_offline_stt_malloc_buf(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 ret;
    td_u32 split_num;
    td_u64 size, total_size;
    unsigned long flags;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    td_char mmb_name[MAX_MMZ_NAME_LEN] = {0};
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    ret = snprintf_s(mmb_name, sizeof(mmb_name), sizeof(mmb_name) - 1, "ISP[%d].BeStt", vi_pipe);
    if (ret < 0) {
        return TD_FAILURE;
    }
    split_num = OT_ISP_STRIPING_MAX_NUM;
    size = sizeof(isp_post_be_offline_stat_type);
    size = OT_ALIGN_UP(size, 256); /* 256 align */
    total_size = size * split_num * PING_PONG_NUM;
    ret = cmpi_mmz_malloc_cached(TD_NULL, mmb_name, &phys_addr, (td_void **)&virt_addr, total_size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc ISP BeSttBuf err\n");
        return OT_ERR_ISP_NOMEM;
    }

    (td_void)memset_s(virt_addr, total_size, 0, total_size);

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    isp_drv_be_offline_stt_ctx_init(phys_addr, virt_addr, split_num, size, &drv_ctx->be_pre_on_post_off_stt_buf);
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_be_pre_online_post_offline_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_phys_addr_t phys_addr;
    td_u8 *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_init_return(vi_pipe, drv_ctx->be_pre_on_post_off_stt_buf.init);

    ret = isp_drv_be_pre_online_post_offline_stt_malloc_buf(vi_pipe, drv_ctx);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_drv_be_offline_stitch_stt_malloc_buf(vi_pipe, drv_ctx);
    if (ret != TD_SUCCESS) {
        goto fail0;
    }

    return TD_SUCCESS;
fail0:
    phys_addr = drv_ctx->be_pre_on_post_off_stt_buf.be_stt_buf[0].phys_addr[0];
    virt_addr = drv_ctx->be_pre_on_post_off_stt_buf.be_stt_buf[0].virt_addr[0];
    isp_drv_be_offline_stt_ctx_reset(&drv_ctx->be_pre_on_post_off_stt_buf);
    if (phys_addr != 0) {
        cmpi_mmz_free(phys_addr, virt_addr);
    }
    return TD_FAILURE;
}

td_s32 isp_drv_be_pre_online_post_offline_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_u64 phys_addr;
    unsigned long flags;
    td_void *virt_addr = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_buf_exit_return(vi_pipe, drv_ctx->be_pre_on_post_off_stt_buf.init);

    phys_addr = drv_ctx->be_pre_on_post_off_stt_buf.be_stt_buf[0].phys_addr[0];
    virt_addr = drv_ctx->be_pre_on_post_off_stt_buf.be_stt_buf[0].virt_addr[0];

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    isp_drv_be_offline_stt_ctx_reset(&drv_ctx->be_pre_on_post_off_stt_buf);
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    if (phys_addr != 0) {
        cmpi_mmz_free(phys_addr, virt_addr);
    }
    isp_drv_be_offline_stitch_stt_exit(vi_pipe, drv_ctx);

    return TD_SUCCESS;
}

td_s32 isp_drv_be_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_be_online_stt_buf_init(vi_pipe) != TD_SUCCESS) {
            return TD_FAILURE;
        }

        if (isp_drv_be_lut_stt_buf_init(vi_pipe) != TD_SUCCESS) {
            goto fail0;
        }
    } else if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_be_offline_stt_buf_init(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("isp[%d] isp_drv_be_offline_stt_buf_init failed\n", vi_pipe);
            return TD_FAILURE;
        }
    } else if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_be_pre_online_post_offline_stt_buf_init(vi_pipe) != TD_SUCCESS) {
            return TD_FAILURE;
        }
        if (isp_drv_pre_on_post_off_lut_stt_buf_init(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("isp_drv_be_pre_online_post_offline_stt_buf_init failed\n");
            return isp_drv_be_pre_online_post_offline_stt_buf_exit(vi_pipe);
        }
    } else {
        isp_err_trace("enIspOnlineMode err 0x%x!\n", drv_ctx->work_mode.running_mode);
        return TD_FAILURE;
    }

    return TD_SUCCESS;

fail0:
    ret = isp_drv_be_online_stt_buf_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("pipe[%d] isp_drv_be_online_stt_buf_exit failed!\n", vi_pipe);
    }

    return TD_FAILURE;
}

td_s32 isp_drv_be_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_be_online_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("pipe[%d] isp_drv_be_online_stt_buf_exit failed!\n", vi_pipe);
            return TD_FAILURE;
        }

        if (isp_drv_be_lut_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("pipe[%d] isp_drv_be_lut_stt_buf_exit failed!\n", vi_pipe);
            return TD_FAILURE;
        }
    } else if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_be_offline_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("pipe[%d] isp_drv_be_offline_stt_buf_exit failed!\n", vi_pipe);
            return TD_FAILURE;
        }
    } else if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_be_pre_online_post_offline_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("pipe[%d] isp_drv_be_pre_on_post_off_stt_buf_exit failed!\n", vi_pipe);
            return TD_FAILURE;
        }
        if (isp_drv_pre_on_post_off_lut_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
            isp_err_trace("pipe[%d] isp_drv_pre_on_post_off_lut_stt_buf_exit failed!\n", vi_pipe);
            return TD_FAILURE;
        }
    } else {
        isp_err_trace("enIspOnlineMode err 0x%x!\n", drv_ctx->work_mode.running_mode);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stt_buf_init(ot_vi_pipe vi_pipe)
{
    isp_check_pipe_return(vi_pipe);

    if (isp_drv_be_stt_buf_init(vi_pipe) != TD_SUCCESS) {
        isp_err_trace("isp[%d] be_stt_buf_init failed\n", vi_pipe);
        return TD_FAILURE;
    }

    if (isp_drv_fe_stt_buf_init(vi_pipe) != TD_SUCCESS) {
        isp_err_trace("isp[%d] fe_stt_buf_init failed\n", vi_pipe);
        isp_drv_be_stt_buf_exit(vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_exit_state_return(vi_pipe, drv_ctx->isp_run_flag);

    if (isp_drv_be_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
        isp_err_trace("pipe[%d] isp_drv_be_stt_buf_exit failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    if (isp_drv_fe_stt_buf_exit(vi_pipe) != TD_SUCCESS) {
        isp_err_trace("pipe[%d] isp_drv_fe_stt_buf_exit failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_p2_en_info(ot_vi_pipe vi_pipe, td_bool *p2_en)
{
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(p2_en);
    if (vi_pipe == 0) {
        *p2_en = (td_bool)((io_rw_pt_address(VI_ISP_MODE) & (1 << VI_ISP0_P2_EN_BIT)) >> VI_ISP0_P2_EN_BIT);
    } else {
        *p2_en = TD_FALSE;
    }
    return TD_SUCCESS;
}

td_s32 isp_drv_sta_kernel_get(ot_vi_pipe vi_pipe, ot_isp_drv_af_statistics *focus_stat)
{
    unsigned long flags;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_stat *stat = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    if (focus_stat == TD_NULL) {
        isp_err_trace("get statistic active buffer err, focus_stat is TD_NULL!\n");
        return TD_FAILURE;
    }

    if (!drv_ctx->statistics_buf.act_stat) {
        isp_info_trace("get statistic active buffer err, stat not ready!\n");
        return TD_FAILURE;
    }

    if (!drv_ctx->statistics_buf.act_stat->virt_addr) {
        isp_err_trace("get statistic active buffer err, virt_addr is TD_NULL!\n");
        return TD_FAILURE;
    }

    stat = (isp_stat *)drv_ctx->statistics_buf.act_stat->virt_addr;
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    (td_void)memcpy_s(&focus_stat->fe_af_stat, sizeof(ot_isp_drv_fe_focus_statistics), &stat->fe_af_stat,
        sizeof(ot_isp_drv_fe_focus_statistics));
    (td_void)memcpy_s(&focus_stat->be_af_stat, sizeof(ot_isp_drv_be_focus_statistics), &stat->be_af_stat,
        sizeof(ot_isp_drv_be_focus_statistics));
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_reset_fe_cfg(ot_vi_pipe vi_pipe)
{
#ifdef ENABLE_CLOCK
    td_u8 k;
    td_bool reset = TD_TRUE;
    ot_vi_pipe vi_pipe_bind;
    ot_mpp_chn mpp_chn;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    mpp_chn.mod_id = OT_ID_ISP;

    for (k = 0; k < drv_ctx->wdr_attr.pipe_num; k++) {
        vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];

        isp_check_vir_pipe_return(vi_pipe_bind);
        mpp_chn.dev_id = vi_pipe_bind;
        call_sys_drv_ioctrl(&mpp_chn, SYS_VI_ISP_CFG_RESET_SEL, &reset);
    }
    reset = TD_FALSE;
    for (k = 0; k < drv_ctx->wdr_attr.pipe_num; k++) {
        vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];

        isp_check_vir_pipe_return(vi_pipe_bind);
        mpp_chn.dev_id = vi_pipe_bind;
        call_sys_drv_ioctrl(&mpp_chn, SYS_VI_ISP_CFG_RESET_SEL, &reset);
    }
#else
    ot_unused(vi_pipe);
#endif

    return TD_SUCCESS;
}

td_s32 isp_drv_update_ldci_tpr_offline_stat(ot_vi_pipe vi_pipe, isp_be_wo_reg_cfg *be_cfg)
{
    td_u8 i;
    td_phys_addr_t phy_addr;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_cfg);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    phy_addr = be_cfg->be_reg_cfg[0].post_viproc.viproc_out_para_dci_addr_low.u32;

    for (i = 0; i < drv_ctx->ldci_read_buf_attr.buf_num; i++) {
        if (phy_addr == drv_ctx->ldci_write_buf_attr.ldci_buf[i].phy_addr) {
            (td_void)memcpy_s(&drv_ctx->ldci_stt_addr.ldci_stat, sizeof(isp_ldci_stat),
                drv_ctx->ldci_write_buf_attr.ldci_buf[i].vir_addr, sizeof(isp_ldci_stat));
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_update_ldci_tpr_stt_addr(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_be_wo_reg_cfg *be_cfg)
{
    td_bool ldci_tpr_flt_en;
    td_u8 i;
    td_phys_addr_t phys_addr, phys_addr_high;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);

    ldci_tpr_flt_en = isp_drv_get_ldci_tpr_flt_en(vi_pipe);
    if (ldci_tpr_flt_en == TD_FALSE) {
        return TD_SUCCESS;
    }

    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        phys_addr_high = be_cfg->be_reg_cfg[i].post_viproc.viproc_out_para_dci_addr_high.u32;
        phys_addr = be_cfg->be_reg_cfg[i].post_viproc.viproc_out_para_dci_addr_low.u32;
        phys_addr |= (phys_addr_high << 32); /* shitf left  32 bit  */
        drv_ctx->be_offline_addr_info.post_be[i].ldci_write_stat_addr = phys_addr;

        phys_addr_high = be_cfg->be_reg_cfg[i].post_viproc.viproc_para_dci_addr_high.u32;
        phys_addr = be_cfg->be_reg_cfg[i].post_viproc.viproc_para_dci_addr_low.u32;
        phys_addr |= (phys_addr_high << 32); /* shitf left  32 bit  */
        drv_ctx->be_offline_addr_info.post_be[i].ldci_read_stat_addr = phys_addr;
    }

    return TD_SUCCESS;
}

static td_void isp_drv_update_wdr_stat_addr_info(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_be_wo_reg_cfg *be_cfg)
{
    td_u8 i, hor_blk_num, ver_blk_num, sti_hor_blk_num, sti_ver_blk_num;
    td_u8 cur_write_idx;
    td_phys_addr_t buf_offset = 0;
    td_phys_addr_t read_phys_addr, write_phys_addr;
    td_bool wdr_en, fusion_mode_en;
    td_bool stat_en = TD_FALSE;
    isp_pre_be_offline_addr_info *pre_be_addr = TD_NULL;
    ot_unused(vi_pipe);

    wdr_en = be_cfg->be_reg_cfg[0].pre_viproc.viproc_ispbe_ctrl0.bits.isp_wdr_en;
    fusion_mode_en = drv_ctx->be_sync_para.fusion_mode;
    if ((wdr_en == TD_TRUE) && (fusion_mode_en != TD_TRUE)) {
        stat_en = TD_TRUE;
    }

    cur_write_idx = drv_ctx->fswdr_buf_attr.cur_write_idx;
    write_phys_addr = drv_ctx->fswdr_buf_attr.fswdr_buf[cur_write_idx].phy_addr;
    read_phys_addr = drv_ctx->fswdr_buf_attr.fswdr_buf[1 - cur_write_idx].phy_addr;

    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        pre_be_addr = &drv_ctx->be_offline_addr_info.pre_be[i];
        hor_blk_num = be_cfg->be_reg_cfg[i].pre_be.isp_wdr_stat_blk_num.bits.isp_wdr_ref_stat_hblk_num;
        ver_blk_num = be_cfg->be_reg_cfg[i].pre_be.isp_wdr_stat_blk_num.bits.isp_wdr_ref_stat_vblk_num;
        sti_hor_blk_num = be_cfg->be_reg_cfg[i].pre_be.isp_wdr_clac_blk_num.bits.isp_wdr_cur_clac_hblk_num;
        sti_ver_blk_num = be_cfg->be_reg_cfg[i].pre_be.isp_wdr_clac_blk_num.bits.isp_wdr_cur_clac_vblk_num;

        pre_be_addr->wdr_sti.en = stat_en;
        pre_be_addr->wdr_sti.bit_width = 0x10;
        pre_be_addr->wdr_sti.width = sti_hor_blk_num;
        pre_be_addr->wdr_sti.height = sti_ver_blk_num;
        pre_be_addr->wdr_sti.stride = (sti_hor_blk_num * 32 + 127) / 128 * 16; /* (blk_num * 32 + 127) / 128 * 16 */
        pre_be_addr->wdr_sti.phys_addr = read_phys_addr;

        pre_be_addr->wdr_sto.en = pre_be_addr->wdr_sti.en;
        pre_be_addr->wdr_sto.bit_width = pre_be_addr->wdr_sti.bit_width;
        pre_be_addr->wdr_sto.width = hor_blk_num;
        pre_be_addr->wdr_sto.height = ver_blk_num;
        pre_be_addr->wdr_sto.stride = pre_be_addr->wdr_sti.stride;
        pre_be_addr->wdr_sto.phys_addr = write_phys_addr + buf_offset;

        buf_offset += (hor_blk_num * sizeof(td_u32));
    }

    drv_ctx->fswdr_buf_attr.cur_write_idx = 1 - cur_write_idx;
}

static td_s32 isp_drv_update_pre_be_offline_addr_info(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    td_u8 i;
    td_phys_addr_t phys_addr;
    td_phys_addr_t lut2stt_addr;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_pre_be_offline_addr_info *pre_be_addr = TD_NULL;
    isp_be_wo_reg_cfg *be_cfg = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_cfg_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    be_cfg = (isp_be_wo_reg_cfg *)be_cfg_buf->vir_addr;

    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        pre_be_addr = &drv_ctx->be_offline_addr_info.pre_be[i];
        phys_addr = be_cfg_buf->pre_phy_addr[i];
        /* be normal cfg */
        pre_be_addr->be0_cfg_addr = phys_addr + ISP_BE0_CFG_LOAD_ADDR_OFFSET;
        pre_be_addr->be1_cfg_addr = phys_addr + ISP_BE1_CFG_LOAD_ADDR_OFFSET;
        /* be apb lut */
        pre_be_addr->be_param0_addr = phys_addr + PRE_BE_REG_SIZE + ISP_BE_LUT0_CFG_LOAD_ADDR_OFFSET;
        pre_be_addr->be_param1_addr = phys_addr + PRE_BE_REG_SIZE + ISP_BE_LUT1_CFG_LOAD_ADDR_OFFSET;
        pre_be_addr->be_param2_addr = phys_addr + PRE_BE_REG_SIZE + ISP_BE_LUT2_CFG_LOAD_ADDR_OFFSET;
        pre_be_addr->be_param3_addr = phys_addr + PRE_BE_REG_SIZE + ISP_BE_LUT3_CFG_LOAD_ADDR_OFFSET;

        /* be lut2stt */
        lut2stt_addr = phys_addr + PRE_BE_REG_SIZE + ISP_BE_APB_LUT_SIZE;
        pre_be_addr->dpc.phys_addr = lut2stt_addr + ISP_DPC_LUT_WSTT_OFFSET;
        pre_be_addr->dpc.width_word = be_cfg->be_reg_cfg[i].pre_viproc.viproc_isp_dcg_lut_width_word.u32;

        pre_be_addr->dpc1.phys_addr = lut2stt_addr + ISP_DPC1_LUT_WSTT_OFFSET;
        pre_be_addr->dpc1.width_word = be_cfg->be_reg_cfg[i].pre_viproc.viproc_isp_dcg1_lut_width_word.u32;

        pre_be_addr->pregammafe.phys_addr = lut2stt_addr + ISP_PREGAMMAFE_LUT_WSTT_OFFSET;
        pre_be_addr->pregammafe.width_word = be_cfg->be_reg_cfg[i].pre_viproc.viproc_isp_pregammafe_lut_width_word.u32;

        pre_be_addr->crb.phys_addr = lut2stt_addr + ISP_CRB_LUT_WSTT_OFFSET;
        pre_be_addr->crb.width_word = be_cfg->be_reg_cfg[i].pre_viproc.viproc_isp_crb_lut_width_word.u32;
    }

    isp_drv_update_wdr_stat_addr_info(vi_pipe, drv_ctx, be_cfg);

    return TD_SUCCESS;
}

static td_void isp_drv_update_post_be_lut2stt_addr_info(td_phys_addr_t lut2stt_addr,
    isp_post_be_offline_addr_info *post_be_addr, isp_viproc_reg_type *post_viproc)
{
    post_be_addr->bnr.phys_addr = lut2stt_addr + ISP_BNR_LUT_WSTT_OFFSET;
    post_be_addr->bnr.width_word = post_viproc->viproc_isp_bnr_lut_width_word.u32;

    post_be_addr->lsc.phys_addr = lut2stt_addr + ISP_LSC_LUT_WSTT_OFFSET;
    post_be_addr->lsc.width_word = post_viproc->viproc_isp_lsc_lut_width_word.u32;

    post_be_addr->clut.phys_addr = lut2stt_addr + ISP_CLUT_LUT_WSTT_OFFSET;
    post_be_addr->clut.width_word = post_viproc->viproc_isp_clut_lut_width_word.u32;

    post_be_addr->gamma.phys_addr = lut2stt_addr + ISP_GAMMA_LUT_WSTT_OFFSET;
    post_be_addr->gamma.width_word = post_viproc->viproc_isp_gamma_lut_width_word.u32;

    post_be_addr->dehaze.phys_addr = lut2stt_addr + ISP_DEHAZE_LUT_WSTT_OFFSET;
    post_be_addr->dehaze.width_word = post_viproc->viproc_isp_dehaze_lut_width_word.u32;

    post_be_addr->ldci.phys_addr = lut2stt_addr + ISP_LDCI_LUT_WSTT_OFFSET;
    post_be_addr->ldci.width_word = post_viproc->viproc_isp_ldci_lut_width_word.u32;

    post_be_addr->sharpen.phys_addr = lut2stt_addr + ISP_SHARPEN_LUT_WSTT_OFFSET;
    post_be_addr->sharpen.width_word = post_viproc->viproc_isp_sharpen_lut_width_word.u32;

    post_be_addr->degamma_fe.phys_addr = lut2stt_addr + ISP_DEGAMMAFE_LUT_WSTT_OFFSET;
    post_be_addr->degamma_fe.width_word = post_viproc->viproc_isp_degammafe_lut_width_word.u32;

    post_be_addr->dmnr.phys_addr = lut2stt_addr + ISP_BNR_SHP_LUT_WSTT_OFFSET;
    post_be_addr->dmnr.width_word = post_viproc->viproc_isp_bnrshp_lut_width_word.u32;
}

td_s32 isp_drv_update_ldci_normal_offline_attr(ot_vi_pipe vi_pipe)
{
    td_u8 i, read_buf_idx, write_buf_idx, blk_num;
    td_u32 size;
    td_phys_addr_t write_stt_addr, read_stt_addr;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    blk_num = drv_ctx->work_mode.block_num;
    size = sizeof(isp_ldci_stat);
    read_buf_idx = drv_ctx->ldci_read_buf_attr.buf_idx;
    write_buf_idx = drv_ctx->ldci_write_buf_attr.buf_idx;
    write_stt_addr = drv_ctx->ldci_write_buf_attr.ldci_buf[write_buf_idx].phy_addr;
    read_stt_addr = drv_ctx->ldci_read_buf_attr.ldci_buf[read_buf_idx].phy_addr;

    for (i = 0; i < blk_num; i++) {
        drv_ctx->be_offline_addr_info.post_be[i].ldci_read_stat_addr = read_stt_addr;
        drv_ctx->be_offline_addr_info.post_be[i].ldci_write_stat_addr =
            write_stt_addr + i * (size / div_0_to_1(blk_num));
    }

    drv_ctx->ldci_read_buf_attr.buf_idx = write_buf_idx;
    drv_ctx->ldci_write_buf_attr.buf_idx = (write_buf_idx + 1) % div_0_to_1(drv_ctx->ldci_write_buf_attr.buf_num);

    return TD_SUCCESS;
}

static td_void isp_drv_update_ldci_offline_addr_info(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    isp_be_wo_reg_cfg *be_cfg)
{
    td_bool ldci_tpr_flt_en;

    ldci_tpr_flt_en = isp_drv_get_ldci_tpr_flt_en(vi_pipe);
    if (ldci_tpr_flt_en == TD_TRUE) {
        isp_drv_update_ldci_tpr_stt_addr(vi_pipe, drv_ctx, be_cfg);
    } else {
        isp_drv_update_ldci_normal_offline_attr(vi_pipe);
    }
}

static td_s32 isp_drv_update_post_be_offline_addr_info(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    td_u8 i;
    td_phys_addr_t phys_addr;
    td_phys_addr_t lut2stt_addr;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_post_be_offline_addr_info *post_be_addr = TD_NULL;
    isp_be_wo_reg_cfg *be_cfg = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_cfg_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    be_cfg = (isp_be_wo_reg_cfg *)be_cfg_buf->vir_addr;

    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        post_be_addr = &drv_ctx->be_offline_addr_info.post_be[i];
        phys_addr = be_cfg_buf->post_phy_addr[i];
        /* be normal cfg */
        post_be_addr->be0_cfg_addr = phys_addr + ISP_BE0_CFG_LOAD_ADDR_OFFSET;
        post_be_addr->be1_cfg_addr = phys_addr + ISP_BE1_CFG_LOAD_ADDR_OFFSET;
        /* be apb lut */
        post_be_addr->be_param0_addr = phys_addr + POST_BE_REG_SIZE + ISP_BE_LUT0_CFG_LOAD_ADDR_OFFSET;
        post_be_addr->be_param1_addr = phys_addr + POST_BE_REG_SIZE + ISP_BE_LUT1_CFG_LOAD_ADDR_OFFSET;
        post_be_addr->be_param2_addr = phys_addr + POST_BE_REG_SIZE + ISP_BE_LUT2_CFG_LOAD_ADDR_OFFSET;
        post_be_addr->be_param3_addr = phys_addr + POST_BE_REG_SIZE + ISP_BE_LUT3_CFG_LOAD_ADDR_OFFSET;
        /* be lut2stt */
        lut2stt_addr = phys_addr + POST_BE_REG_SIZE + ISP_BE_APB_LUT_SIZE;
        isp_drv_update_post_be_lut2stt_addr_info(lut2stt_addr, post_be_addr, &be_cfg->be_reg_cfg[i].post_viproc);
        /* drc */
        post_be_addr->drc_read_stat_addr = drv_ctx->drc_buf_attr.drc_buf.phy_addr;
        post_be_addr->drc_write_stat_addr = drv_ctx->drc_buf_attr.drc_buf.phy_addr;
    }

    isp_drv_update_ldci_offline_addr_info(vi_pipe, drv_ctx, be_cfg);

    return TD_SUCCESS;
}

td_s32 isp_drv_update_be_offline_addr_info(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    td_s32 ret;
    /* pre be */
    ret = isp_drv_update_pre_be_offline_addr_info(vi_pipe, be_cfg_buf);
    isp_check_ret_continue(vi_pipe, ret, "update_pre_be_offline_addr");
    /* post be */
    ret = isp_drv_update_post_be_offline_addr_info(vi_pipe, be_cfg_buf);
    isp_check_ret_continue(vi_pipe, ret, "update_post_be_offline_addr");
    return TD_SUCCESS;
}

static td_void isp_drv_get_all_offline_stt_addr(ot_vi_pipe vi_pipe, td_u32 viproc_id, td_u64 pts,
    isp_be_offline_stt_buf *be_off_stt_addr, isp_be_offline_stt_attr *be_off_stt_buf)
{
    td_u8 i, blk_num;
    td_u32 working_idx;
    td_u32 addr_offset;

    blk_num = isp_drv_get_block_num(vi_pipe);
    blk_num = div_0_to_1(blk_num);
    working_idx = be_off_stt_buf->working_be_stt_idx[viproc_id];
    for (i = 0; i < blk_num; i++) {
        addr_offset = viproc_id * sizeof(isp_pre_be_offline_stat_type);
        be_off_stt_addr->phys_addr[i] = be_off_stt_buf->be_stt_buf[working_idx].phys_addr[i] + addr_offset;
        be_off_stt_addr->virt_addr[i] = be_off_stt_buf->be_stt_buf[working_idx].virt_addr[i] + addr_offset;
    }
    be_off_stt_buf->pts[viproc_id][working_idx] = pts;

    return;
}

static td_void isp_drv_pre_on_post_off_stt_addr(ot_vi_pipe vi_pipe, td_u32 viproc_id, td_u64 pts,
    isp_be_offline_stt_buf *be_off_stt_addr, isp_be_offline_stt_attr *be_off_stt_buf)
{
    td_u8 i, blk_num;
    td_u32 working_idx;

    blk_num = isp_drv_get_block_num(vi_pipe);
    blk_num = div_0_to_1(blk_num);
    working_idx = be_off_stt_buf->working_be_stt_idx[viproc_id];
    for (i = 0; i < blk_num; i++) {
        be_off_stt_addr->phys_addr[i] = be_off_stt_buf->be_stt_buf[working_idx].phys_addr[i];
    }
    be_off_stt_buf->pts[viproc_id][working_idx] = pts;
    return;
}

td_void isp_drv_be_offline_addr_info_update(ot_vi_pipe vi_pipe, td_u32 viproc_id, td_u64 pts,
    isp_be_offline_stt_buf *be_offline_stt_addr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode)) {
        isp_drv_get_all_offline_stt_addr(vi_pipe, viproc_id, pts, be_offline_stt_addr, &drv_ctx->be_off_stt_buf);
        return;
    }

    if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        isp_drv_pre_on_post_off_stt_addr(vi_pipe, viproc_id, pts, be_offline_stt_addr,
            &drv_ctx->be_pre_on_post_off_stt_buf);
        return;
    }
}

static td_void isp_drv_switch_be_offline_stt_index(ot_vi_pipe vi_pipe, td_u32 viproc_id,
    isp_be_offline_stt_attr *be_off_stt_buf)
{
    td_u32 index;
    unsigned long flags;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    index = be_off_stt_buf->working_be_stt_idx[viproc_id];
    be_off_stt_buf->last_ready_be_stt_idx[viproc_id] = index;
    be_off_stt_buf->working_be_stt_idx[viproc_id] = 1 - index;
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return;
}

td_void isp_drv_switch_be_offline_stt_buf_index(ot_vi_pipe vi_pipe, td_u32 viproc_id, isp_drv_ctx *drv_ctx)
{
    if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode)) {
        isp_drv_switch_be_offline_stt_index(vi_pipe, viproc_id, &drv_ctx->be_off_stt_buf);
        return;
    }
    if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        isp_drv_switch_be_offline_stt_index(vi_pipe, viproc_id, &drv_ctx->be_pre_on_post_off_stt_buf);
        return;
    }
}

td_u8 isp_drv_get_block_id(ot_vi_pipe vi_pipe, isp_running_mode running_mode)
{
    const td_s8 block_id = 0; /* only support one BE */
    ot_unused(running_mode);
    ot_unused(vi_pipe);
    return block_id;
}

#ifdef CONFIG_OT_VI_STITCH_GRP
td_s32 isp_drv_stitch_sync_ctrl_init(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    ot_vi_pipe vi_pipe_s;
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
            drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
            drv_ctx_s->isp_sync_ctrl.stitch_flag = 0;
            drv_ctx_s->isp_sync_ctrl.read_all = 0;
            drv_ctx_s->isp_sync_ctrl.first_order = 0;
            drv_ctx_s->isp_sync_ctrl.first_read_all = 0;
            drv_ctx_s->isp_sync_ctrl.all_init = 0;
            drv_ctx_s->isp_sync_ctrl.vi_callback_count = 0;
            drv_ctx_s->isp_sync_ctrl.mode_switch = TD_TRUE;
        }
    }

    return TD_SUCCESS;
}

static td_void isp_drv_be_sync_para_status_rest(ot_vi_pipe vi_pipe)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    ot_vi_pipe vi_pipe_s;
    td_u32 i;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
        drv_ctx_s->isp_sync_ctrl.first_order = 0;
        drv_ctx_s->isp_sync_ctrl.read_all = 0;
        drv_ctx_s->isp_sync_ctrl.vi_callback_count = 0;
        drv_ctx_s->isp_sync_ctrl.stitch_flag = 0;
    }

    drv_ctx->isp_sync_ctrl.first_order = 1;
    drv_ctx->isp_sync_ctrl.stitch_flag = 1;
    drv_ctx->isp_sync_ctrl.vi_callback_count = 1;
}

static td_s32 isp_drv_vi_get_be_sync_para_sub_func0(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u32 i;
    td_u64 max_gap;
    ot_vi_pipe vi_pipe_s;
    td_bool all_init = 0;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    if (drv_ctx->isp_sync_ctrl.all_init == 0) {
        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
            drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
            if (drv_ctx_s->isp_run_flag == TD_FALSE) {
                all_init = TD_FALSE;
                break;
            }

            if (i == (td_u32)(drv_ctx->stitch_attr.stitch_pipe_num - 1)) {
                all_init = TD_TRUE;
            }
        }

        if (all_init == TD_TRUE) {
            for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
                vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
                drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
                drv_ctx_s->isp_sync_ctrl.all_init = 1;
            }
        }
    }

    if (drv_ctx->isp_run_flag == TD_FALSE) {
        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
            drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
            drv_ctx_s->isp_sync_ctrl.all_init = 0;
        }
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
        if ((drv_ctx_s->isp_sync_ctrl.first_order == 1) && (vi_pipe_s != vi_pipe)) {
            max_gap = (drv_ctx->isp_sync_ctrl.pts > drv_ctx_s->isp_sync_ctrl.pts) ?
                (drv_ctx->isp_sync_ctrl.pts - drv_ctx_s->isp_sync_ctrl.pts) :
                (drv_ctx_s->isp_sync_ctrl.pts - drv_ctx->isp_sync_ctrl.pts);

            if (max_gap >= 6000) { /* const value 6000 */
                isp_drv_be_sync_para_status_rest(vi_pipe);
                return TD_SUCCESS;
            }
        }
    }

    return TD_FAILURE;
}

static td_s32 isp_drv_vi_get_be_sync_para_sub_func1(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_bool *same_cnt,
    td_u32 *cross_frame)
{
    td_u32 i, j, k;
    ot_vi_pipe vi_pipe_s;
    td_u32 x, y;
    td_u32 cross_frame_num = 0;
    td_bool same_count = TD_FALSE;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    x = drv_ctx->isp_sync_ctrl.vi_callback_count;

    for (j = 0; j < drv_ctx->stitch_attr.stitch_pipe_num; j++) {
        vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[j];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
        if ((drv_ctx_s->isp_sync_ctrl.first_read_all == 1) && drv_ctx_s->isp_sync_ctrl.all_init) {
            break;
        }

        if (j == (td_u32)(drv_ctx->stitch_attr.stitch_pipe_num - 1)) {
            for (k = 0; k < drv_ctx->stitch_attr.stitch_pipe_num; k++) {
                vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[k];
                drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
                drv_ctx_s->isp_sync_ctrl.vi_callback_count = 0;
            }

            return TD_SUCCESS;
        }
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
        y = drv_ctx_s->isp_sync_ctrl.vi_callback_count;

        if (y != x) {
            cross_frame_num = x > y ? (x - y) : (y - x);
        } else {
            j++;
        }

        if ((cross_frame_num >= 2) && (drv_ctx_s->isp_sync_ctrl.all_init)) { /* const value 2 */
            isp_drv_be_sync_para_status_rest(vi_pipe);

            return TD_SUCCESS;
        }

        if ((i == (td_u32)(drv_ctx->stitch_attr.stitch_pipe_num - 1)) && (i == (j - 1)) &&
            drv_ctx_s->isp_sync_ctrl.all_init) {
            same_count = TD_TRUE;
            cross_frame_num = 0;
        }
    }

    *same_cnt = same_count;
    *cross_frame = cross_frame_num;

    return TD_FAILURE;
}

static td_u32 isp_drv_vi_get_be_sync_para_cross_frame(ot_vi_pipe vi_pipe)
{
    td_s32 j, ret;
    ot_vi_pipe vi_pipe_s;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *drv_ctx_s = TD_NULL;
    td_bool same_count = TD_FALSE;
    td_u32 cross_frame_num = 0;
    const td_u32 time_cross_frame = 100;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = isp_drv_vi_get_be_sync_para_sub_func0(vi_pipe, drv_ctx);
    if (ret == TD_SUCCESS) {
        return time_cross_frame;
    }

    ret = isp_drv_vi_get_be_sync_para_sub_func1(vi_pipe, drv_ctx, &same_count, &cross_frame_num);
    if (ret == TD_SUCCESS) {
        return time_cross_frame;
    }

    if (same_count == TD_TRUE) {
        for (j = 0; j < drv_ctx->stitch_attr.stitch_pipe_num; j++) {
            vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[j];
            drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
            drv_ctx_s->isp_sync_ctrl.vi_callback_count = 0;
        }
    }

    return cross_frame_num;
}

static td_void isp_drv_get_stitch_be_sync_para_sub_func0(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    ot_vi_pipe vi_pipe_s;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[i];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
        if ((drv_ctx_s->isp_sync_ctrl.stitch_flag == 0) && (drv_ctx_s->isp_sync_ctrl.read_all == 0)) {
        } else {
            drv_ctx->isp_sync_ctrl.first_order = 0;
            break;
        }

        if (i == (drv_ctx->stitch_attr.stitch_pipe_num - 1) && (drv_ctx->isp_sync_ctrl.all_init == 1)) {
            drv_ctx->isp_sync_ctrl.first_order = 1;
        }
    }

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
            drv_ctx->isp_sync_ctrl.stitch_flag = 1;
            break;
        }
    }
}

static td_void isp_drv_get_stitch_be_sync_para_sub_func1(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 j;
    ot_vi_pipe vi_pipe_s;
    isp_drv_ctx *drv_ctx_s = TD_NULL;

    for (j = 0; j < drv_ctx->stitch_attr.stitch_pipe_num; j++) {
        vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[j];
        drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
        if (drv_ctx_s->isp_sync_ctrl.stitch_flag != 1) {
            drv_ctx_s->isp_sync_ctrl.read_all = 0;
            break;
        }

        if (j == drv_ctx->stitch_attr.stitch_pipe_num - 1) {
            drv_ctx->isp_sync_ctrl.read_all = 1;
            drv_ctx->isp_sync_ctrl.first_read_all = 1;
        }
    }

    if (drv_ctx->isp_sync_ctrl.read_all == 1) {
        for (j = 0; j < drv_ctx->stitch_attr.stitch_pipe_num; j++) {
            vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[j];
            drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);
            drv_ctx_s->isp_sync_ctrl.stitch_flag = 0;
            drv_ctx_s->isp_sync_ctrl.read_all = 0;
        }
    }

    if (drv_ctx->isp_sync_ctrl.first_order == 1) {
        for (j = 0; j < drv_ctx->stitch_attr.stitch_pipe_num; j++) {
            vi_pipe_s = drv_ctx->stitch_attr.stitch_bind_id[j];
            drv_ctx_s = isp_drv_get_ctx(vi_pipe_s);

            if (vi_pipe_s != vi_pipe) {
                drv_ctx_s->isp_sync_ctrl.first_order = 0;
            }
        }
    }
}

static td_void isp_stitch_be_sync_param_update(ot_vi_pipe vi_pipe, isp_be_sync_para *be_sync_para,
    const isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    td_u8 stitch_idx = 0;
    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
            stitch_idx = i;
            break;
        }
    }

    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        be_sync_para->wb_gain[i] = drv_ctx->be_pre_sync_para_stitch[stitch_idx].wb_gain[i];
    }

    for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
        be_sync_para->ccm[i] = drv_ctx->be_pre_sync_para_stitch[stitch_idx].ccm[i];
    }

    (td_void)memcpy_s(&be_sync_para->be_blc, sizeof(isp_be_blc_dyna_cfg),
        &drv_ctx->be_pre_sync_para_stitch[stitch_idx].be_blc, sizeof(isp_be_blc_dyna_cfg));
}

static td_void isp_drv_get_stitch_be_sync_para_sub_func2(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    isp_be_sync_para *be_sync_para)
{
    ot_vi_pipe main_pipe;
    td_u32 cross_frame_num;
    unsigned long flags;
    isp_drv_ctx *drv_ctx_main_pipe = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    cross_frame_num = isp_drv_vi_get_be_sync_para_cross_frame(vi_pipe);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);

    if (cross_frame_num >= 2 || drv_ctx->isp_sync_ctrl.all_init == 0 || /* const value 2 */
        (drv_ctx->isp_sync_ctrl.mode_switch == TD_TRUE)) {
        osal_spin_lock_irqsave(isp_spin_lock, &flags);
        (td_void)memcpy_s(be_sync_para, sizeof(isp_be_sync_para), &drv_ctx->be_sync_para, sizeof(isp_be_sync_para));
        drv_ctx->isp_sync_ctrl.mode_switch = TD_FALSE;
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
    } else if ((drv_ctx->isp_sync_ctrl.first_order == 1) && (drv_ctx->isp_sync_ctrl.all_init == 1)) {
        main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx_main_pipe = isp_drv_get_ctx(main_pipe);

        osal_spin_lock_irqsave(isp_spin_lock, &flags);
        (td_void)memcpy_s(&drv_ctx_main_pipe->be_pre_sync_para, sizeof(isp_be_sync_para), &drv_ctx->be_sync_para,
            sizeof(isp_be_sync_para));
        (td_void)memcpy_s(&drv_ctx_main_pipe->be_pre_sync_para_stitch[0],
                          sizeof(isp_be_sync_para) * OT_ISP_MAX_PIPE_NUM,
                          &drv_ctx_main_pipe->be_sync_para_stitch[0],
                          sizeof(isp_be_sync_para) * OT_ISP_MAX_PIPE_NUM);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

        osal_spin_lock_irqsave(isp_spin_lock, &flags);
        (td_void)memcpy_s(be_sync_para, sizeof(isp_be_sync_para), &drv_ctx->be_sync_para, sizeof(isp_be_sync_para));
        isp_stitch_be_sync_param_update(vi_pipe, be_sync_para, drv_ctx_main_pipe);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
    } else {
        main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx_main_pipe = isp_drv_get_ctx(main_pipe);

        osal_spin_lock_irqsave(isp_spin_lock, &flags);
        (td_void)memcpy_s(be_sync_para, sizeof(isp_be_sync_para),
                          &drv_ctx_main_pipe->be_pre_sync_para, sizeof(isp_be_sync_para));
        (td_void)memcpy_s(&be_sync_para->fpn_cfg, sizeof(isp_fpn_sync_cfg),
                          &drv_ctx->be_sync_para.fpn_cfg, sizeof(isp_fpn_sync_cfg));

        isp_stitch_be_sync_param_update(vi_pipe, be_sync_para, drv_ctx_main_pipe);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
    }
}

td_s32 isp_drv_get_stitch_be_sync_para(ot_vi_pipe vi_pipe, isp_be_sync_para *be_sync_para)
{
    td_u64 isp_irq_begin_time;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_sync_para);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    drv_ctx->isp_sync_ctrl.vi_callback_count += 1;

    isp_irq_begin_time = call_sys_get_time_stamp();

    drv_ctx->isp_sync_ctrl.pts = isp_irq_begin_time;
    isp_drv_get_stitch_be_sync_para_sub_func0(vi_pipe, drv_ctx);

    isp_drv_get_stitch_be_sync_para_sub_func1(vi_pipe, drv_ctx);

    isp_drv_get_stitch_be_sync_para_sub_func2(vi_pipe, drv_ctx, be_sync_para);

    return TD_SUCCESS;
}

static td_s32 isp_drv_get_index_of_stitch_pipe(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_s32 i;

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
            return i;
        }
    }

    return -1;
}

static td_u64 isp_drv_get_pipe_stitch_sync_pts(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index, td_s32 buf_index)
{
    return drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_index].pts[buf_index];
}

static td_void isp_drv_set_pipe_stitch_sync_pts(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index, td_s32 buf_index,
    td_u64 pts)
{
    drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_index].pts[buf_index] = pts;
}

static td_void isp_drv_set_pipe_stitch_sync_finish_flag(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index, td_s32 buf_index,
    td_bool finish)
{
    drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_index].finish[buf_index] = finish;
}

static td_bool isp_drv_get_pipe_stitch_sync_finish_flag(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index, td_s32 buf_index)
{
    return drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_index].finish[buf_index];
}

td_s32 isp_drv_get_free_stitch_stt_be_buf_index(isp_drv_ctx *drv_ctx_s, td_s32 pipe_idx)
{
    td_s32 i;
    td_u64 min_pts;
    td_u64 tmp_pts;
    td_s32 working_idx;
    td_s32 ready_idx;
    td_s32 free_be_buf_index;
    td_s32 ready_be_buf_index;

    /* 1: if there is free memory, use that buffer */
    for (i = 0; i < MAX_ISP_STITCH_STAT_BUF_NUM; i++) {
        if (drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_idx].finish[i] == TD_FALSE) {
            return i;
        }
    }
    /* 1: if no free buffer is available, discard the oldest */
    working_idx = isp_drv_stitch_sync_get_working_index(drv_ctx_s);
    ready_idx = 1 - working_idx;
    /* Note: The statistics that has been synchronized but has not been read by isp cannot be overwritten */
    if (drv_ctx_s->be_off_stitch_stt_buf.is_ready[ready_idx]) {
        ready_be_buf_index = drv_ctx_s->be_off_stitch_stt_buf.sync_ok_index[ready_idx][pipe_idx];
        min_pts =
            isp_drv_get_pipe_stitch_sync_pts(drv_ctx_s, pipe_idx, MAX_ISP_STITCH_STAT_BUF_NUM - 1 - ready_be_buf_index);
        free_be_buf_index = MAX_ISP_STITCH_STAT_BUF_NUM - 1 - ready_be_buf_index;
    } else {
        ready_be_buf_index = 0;
        min_pts = isp_drv_get_pipe_stitch_sync_pts(drv_ctx_s, pipe_idx, 0);
        free_be_buf_index = 0;
    }

    for (i = 0; i < MAX_ISP_STITCH_STAT_BUF_NUM; i++) {
        if (ready_be_buf_index == i) {
            continue;
        }

        tmp_pts = isp_drv_get_pipe_stitch_sync_pts(drv_ctx_s, pipe_idx, i);
        if (min_pts > tmp_pts) {
            min_pts = tmp_pts;
            free_be_buf_index = i;
        }
    }

    /* clear finish_flag and pts */
    isp_drv_set_pipe_stitch_sync_finish_flag(drv_ctx_s, pipe_idx, free_be_buf_index, TD_FALSE);
    isp_drv_set_pipe_stitch_sync_pts(drv_ctx_s, pipe_idx, free_be_buf_index, 0);

    return free_be_buf_index;
}

static td_void copy_be_ready_stats_to_free_stitch_buffer(isp_drv_ctx *drv_ctx_s, td_s32 pipe_idx,
    td_s32 free_stitch_stt_buf_idx, td_u32 last_ready_be_stt_idx, isp_drv_ctx *drv_ctx)
{
    isp_stitch_stat_reg_type *dst = TD_NULL;
    isp_stitch_stat_reg_type *src = TD_NULL;
    td_u8 *virt_addr = TD_NULL;
    td_u64 pts = drv_ctx->be_off_stt_buf.pts[1][last_ready_be_stt_idx];

    dst = drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_idx].stitch_stt_reg[free_stitch_stt_buf_idx];
    if (is_pre_online_post_offline(drv_ctx_s->work_mode.running_mode)) {
        virt_addr = drv_ctx->be_pre_on_post_off_stt_buf.be_stt_buf[last_ready_be_stt_idx].virt_addr[0];
        src = (isp_stitch_stat_reg_type *)virt_addr;
    } else {
        virt_addr = drv_ctx->be_off_stt_buf.be_stt_buf[last_ready_be_stt_idx].virt_addr[0];
        src = (isp_stitch_stat_reg_type *)(virt_addr + sizeof(isp_pre_be_offline_stat_type));
    }

    (td_void)memcpy_s(dst, sizeof(isp_stitch_stat_reg_type), src, sizeof(isp_stitch_stat_reg_type));

    isp_drv_set_pipe_stitch_sync_finish_flag(drv_ctx_s, pipe_idx, free_stitch_stt_buf_idx, TD_TRUE);
    isp_drv_set_pipe_stitch_sync_pts(drv_ctx_s, pipe_idx, free_stitch_stt_buf_idx, pts);
}

td_bool isp_drv_is_pipe_stitch_sync_finish(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index)
{
    td_s32 i;

    for (i = 0; i < MAX_ISP_STITCH_STAT_BUF_NUM; i++) {
        if (drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_index].finish[i]) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}


td_bool isp_drv_stitch_sync_info_all_finish(isp_drv_ctx *drv_ctx_s)
{
    td_s32 i;

    for (i = 0; i < drv_ctx_s->stitch_attr.stitch_pipe_num; i++) {
        if (isp_drv_is_pipe_stitch_sync_finish(drv_ctx_s, i) == TD_FALSE) {
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}
static td_s32 isp_drv_try_sync_single_pipe(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index_base, ot_vi_pipe pipe_index,
    td_u64 sync_pts)
{
    td_s32 i;
    td_s32 aheadward_frame_cnt = 0;
    td_u64 pts_tmp;
    td_u64 pts_inter;
    td_s32 working_index;

    working_index = drv_ctx_s->be_off_stitch_stt_buf.working_idx;

    for (i = 0; i < MAX_ISP_STITCH_STAT_BUF_NUM; i++) {
        if (isp_drv_get_pipe_stitch_sync_finish_flag(drv_ctx_s, pipe_index, i) != TD_TRUE) {
            continue;
        }
        /* compare PTS */
        pts_tmp = isp_drv_get_pipe_stitch_sync_pts(drv_ctx_s, pipe_index, i);
        pts_inter = (sync_pts >= pts_tmp) ? (sync_pts - pts_tmp) : (pts_tmp - sync_pts);
        if (pts_inter > drv_ctx_s->stitch_attr.max_pts_gap) {
            /* the current pipe's pts is old than other pipe */
            if (sync_pts < pts_tmp) {
                aheadward_frame_cnt++;
            }
        } else {
            drv_ctx_s->be_off_stitch_stt_buf.sync_ok_index[working_index][pipe_index] = i;
            return TD_SUCCESS;
        }
    }

    if (aheadward_frame_cnt >= MAX_ISP_STITCH_STAT_BUF_NUM) {
        drv_ctx_s->be_off_stitch_stt_buf.lost_frame[pipe_index_base] = TD_TRUE;
    }
    return TD_FAILURE;
}
td_s32 isp_drv_try_stitch_sync_be_stt_info(isp_drv_ctx *drv_ctx_s, ot_vi_pipe pipe_index, td_s32 buf_index)
{
    td_s32 ret;
    td_s32 i;
    td_u64 pts;

    pts = drv_ctx_s->be_off_stitch_stt_buf.sync_be_buf[pipe_index].pts[buf_index];

    for (i = 0; i < drv_ctx_s->stitch_attr.stitch_pipe_num; i++) {
        ret = isp_drv_try_sync_single_pipe(drv_ctx_s, pipe_index, i, pts);
        if (ret != TD_SUCCESS) {
            return TD_FAILURE;
        }
    }

    for (i = 0; i < drv_ctx_s->stitch_attr.stitch_pipe_num; i++) {
        if (drv_ctx_s->be_off_stitch_stt_buf.lost_frame[i] == TD_TRUE) {
            drv_ctx_s->be_off_stitch_stt_buf.lost_frame[i] = TD_FALSE;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_stitch_sync_be_stt_info(ot_vi_pipe vi_pipe, td_u32 viproc_id, isp_drv_ctx *drv_ctx)
{
    td_bool all_finish;
    td_u32 last_ready_be_stt_idx;
    td_s32 ret, working_idx, pipe_idx;
    ot_vi_pipe stitch_main_pipe;
    td_s32 free_stitch_stt_buf_idx;

    isp_drv_ctx *drv_ctx_s = TD_NULL;

    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return TD_SUCCESS;
    }

    if (viproc_id != 1) {
        return TD_SUCCESS;
    }

    if (is_offline_mode(drv_ctx->work_mode.running_mode) || is_striping_mode(drv_ctx->work_mode.running_mode)) {
        last_ready_be_stt_idx = drv_ctx->be_off_stt_buf.last_ready_be_stt_idx[viproc_id];
    } else {
        last_ready_be_stt_idx = drv_ctx->be_pre_on_post_off_stt_buf.last_ready_be_stt_idx[viproc_id];
    }

    pipe_idx = isp_drv_get_index_of_stitch_pipe(vi_pipe, drv_ctx);
    if (pipe_idx == -1) {
        return TD_FAILURE;
    }

    stitch_main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
    drv_ctx_s = isp_drv_get_ctx(stitch_main_pipe);
    working_idx = isp_drv_stitch_sync_get_working_index(drv_ctx_s);
    /* get a free index */
    free_stitch_stt_buf_idx = isp_drv_get_free_stitch_stt_be_buf_index(drv_ctx_s, pipe_idx);

    /* copy be statistics to free_buffer,then update finish_flag and pts */
    copy_be_ready_stats_to_free_stitch_buffer(drv_ctx_s, pipe_idx, free_stitch_stt_buf_idx, last_ready_be_stt_idx,
        drv_ctx);

    /* all_finish-> update ready status and switch working_id */
    all_finish = isp_drv_stitch_sync_info_all_finish(drv_ctx_s);
    if (all_finish == TD_TRUE) {
        ret = isp_drv_try_stitch_sync_be_stt_info(drv_ctx_s, pipe_idx, free_stitch_stt_buf_idx);
        if (ret == TD_SUCCESS) {
            isp_drv_stitch_sync_info_ready(drv_ctx_s, working_idx, TD_TRUE);
            isp_drv_stitch_sync_set_working_index(drv_ctx_s, 1 - working_idx);
        }
    }

    return TD_SUCCESS;
}
td_bool isp_drv_is_stitch_sync_lost_frame(ot_vi_pipe vi_pipe, td_u32 viproc_id)
{
    td_bool lost_frame;
    td_s32 pipe_idx;
    isp_drv_ctx *drv_ctx = TD_NULL;

    if ((vi_pipe < 0) || (vi_pipe >= OT_ISP_MAX_PIPE_NUM)) {
        isp_err_trace("Err isp pipe %d!\n", vi_pipe);
        return TD_FALSE;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    if (viproc_id != 1) {
        return TD_FALSE;
    }

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        return TD_FALSE;
    }

    if (drv_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return TD_FALSE;
    }

    pipe_idx = isp_drv_get_index_of_stitch_pipe(vi_pipe, drv_ctx);
    if (pipe_idx == -1) {
        return TD_FALSE;
    }

    lost_frame = drv_ctx->be_off_stitch_stt_buf.lost_frame[pipe_idx];
    drv_ctx->be_off_stitch_stt_buf.lost_frame[pipe_idx] = TD_FALSE;

    return lost_frame;
}
#else
td_bool isp_drv_is_stitch_sync_lost_frame(ot_vi_pipe vi_pipe, td_u32 viproc_id)
{
    return TD_FALSE;
}
#endif

#ifdef CONFIG_OT_ISP_PQ_FOR_AI_SUPPORT
static td_s32 isp_drv_pq_ai_check_change_after_meminit(pq_ai_attr *ai_attr)
{
    td_bool change_flag = TD_FALSE;
    td_bool base_change_flag = TD_FALSE;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *base_drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(ai_attr->ai_pipe_id);
    if ((ai_attr->pq_ai_en != drv_ctx->ai_info.pq_ai_en) || (ai_attr->ai_pipe_id != drv_ctx->ai_info.ai_pipe_id) ||
        (ai_attr->base_pipe_id != drv_ctx->ai_info.base_pipe_id)) {
        change_flag = TD_TRUE;
    }

    if ((drv_ctx->mem_init == TD_TRUE) && (change_flag == TD_TRUE)) {
        isp_err_trace("Does not support changed after isp init!\n");
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    base_drv_ctx = isp_drv_get_ctx(ai_attr->base_pipe_id);
    if ((ai_attr->pq_ai_en != base_drv_ctx->ai_info.pq_ai_en) ||
        (ai_attr->ai_pipe_id != base_drv_ctx->ai_info.ai_pipe_id) ||
        (ai_attr->base_pipe_id != base_drv_ctx->ai_info.base_pipe_id)) {
        base_change_flag = TD_TRUE;
    }

    if ((base_drv_ctx->mem_init == TD_TRUE) && (base_change_flag == TD_TRUE)) {
        isp_err_trace("Does not support changed after isp init!\n");
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    return TD_SUCCESS;
}
static td_void isp_drv_unbind_pq_ai_attr(pq_ai_attr *ai_attr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *base_drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(ai_attr->ai_pipe_id);
    if ((drv_ctx->mem_init == TD_FALSE) && (drv_ctx->ai_info.pq_ai_en == TD_TRUE)) {
        drv_ctx->ai_info.pq_ai_en = TD_FALSE;
        drv_ctx->ai_info.ai_pipe_id = -1;
        drv_ctx->ai_info.base_pipe_id = -1;
    }

    base_drv_ctx = isp_drv_get_ctx(ai_attr->base_pipe_id);
    if ((base_drv_ctx->mem_init == TD_FALSE) && (base_drv_ctx->ai_info.pq_ai_en == TD_TRUE)) {
        base_drv_ctx->ai_info.pq_ai_en = TD_FALSE;
        base_drv_ctx->ai_info.ai_pipe_id = -1;
        base_drv_ctx->ai_info.base_pipe_id = -1;
    }
}

td_s32 isp_set_pq_ai_attr(pq_ai_attr *ai_attr)
{
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_drv_ctx *base_drv_ctx = TD_NULL;

    isp_check_pointer_return(ai_attr);
    isp_check_pipe_return(ai_attr->ai_pipe_id);
    isp_check_pipe_return(ai_attr->base_pipe_id);
    isp_check_bool_return(ai_attr->pq_ai_en);

    ret = isp_drv_pq_ai_check_change_after_meminit(ai_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (ai_attr->pq_ai_en == TD_FALSE) {
        isp_drv_unbind_pq_ai_attr(ai_attr);
        return TD_SUCCESS;
    }

    if (ai_attr->pq_ai_en == TD_TRUE) {
        if (ai_attr->ai_pipe_id == ai_attr->base_pipe_id) {
            return OT_ERR_ISP_ILLEGAL_PARAM;
        }
        base_drv_ctx = isp_drv_get_ctx(ai_attr->base_pipe_id);
        if (base_drv_ctx->mem_init == TD_TRUE) {
            isp_err_trace("Base pipe has been init, not support changed after isp init!\n");
            return OT_ERR_ISP_NOT_SUPPORT;
        }

        if (base_drv_ctx->ai_info.pq_ai_en == TD_TRUE) {
            if (base_drv_ctx->ai_info.ai_pipe_id != ai_attr->ai_pipe_id) {
                isp_err_trace("base_pipe[%d] has been bind ai_pipe[%d]!\n", ai_attr->base_pipe_id,
                    base_drv_ctx->ai_info.ai_pipe_id);
                return OT_ERR_ISP_NOT_SUPPORT;
            }
        }

        (td_void)memcpy_s(&base_drv_ctx->ai_info, sizeof(pq_ai_attr), ai_attr, sizeof(pq_ai_attr));
    }
    drv_ctx = isp_drv_get_ctx(ai_attr->ai_pipe_id);
    if (drv_ctx->ai_info.pq_ai_en == TD_TRUE) {
        if (drv_ctx->ai_info.base_pipe_id != ai_attr->base_pipe_id) {
            isp_err_trace("ai_pipe[%d] has been bind base_pipe[%d]!\n", ai_attr->ai_pipe_id,
                drv_ctx->ai_info.base_pipe_id);
            return OT_ERR_ISP_NOT_SUPPORT;
        }
    }

    (td_void)memcpy_s(&drv_ctx->ai_info, sizeof(pq_ai_attr), ai_attr, sizeof(pq_ai_attr));

    return TD_SUCCESS;
}

td_s32 isp_get_pq_ai_attr(ot_vi_pipe vi_pipe, pq_ai_attr *ai_attr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(ai_attr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    (td_void)memcpy_s(ai_attr, sizeof(pq_ai_attr), &drv_ctx->ai_info, sizeof(pq_ai_attr));

    return TD_SUCCESS;
}

td_s32 isp_set_pq_ai_post_nr_attr(ot_vi_pipe vi_pipe, ot_pq_ai_noiseness_post_attr *ai_3dnr_attr)
{
    td_s32 ret = TD_FAILURE;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(ai_3dnr_attr);
    if (check_func_entry(OT_ID_VI) && ckfn_vi_set_pq_3dnr_ai_attr()) {
        ret = call_vi_set_pq_3dnr_ai_att(vi_pipe, ai_3dnr_attr);
    }
    return ret;
}

td_s32 isp_get_pq_ai_post_nr_attr(ot_vi_pipe vi_pipe, ot_pq_ai_noiseness_post_attr *ai_3dnr_attr)
{
    td_s32 ret = TD_FAILURE;

    if (check_func_entry(OT_ID_VI) && ckfn_vi_get_pq_3dnr_ai_attr()) {
        ret = call_vi_get_pq_3dnr_ai_att(vi_pipe, ai_3dnr_attr);
    }
    return ret;
}
#endif
