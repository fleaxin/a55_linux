/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "isp_ext.h"
#include "isp_drv_define.h"
#include "osal_mmz.h"

td_s32 isp_register_bus_callback(ot_vi_pipe vi_pipe, isp_bus_type type, isp_bus_callback *bus_cb)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pointer_return(bus_cb);
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);

    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (type == ISP_BUS_TYPE_I2C) {
        drv_ctx->bus_cb.pfn_isp_write_i2c_data = bus_cb->pfn_isp_write_i2c_data;
    } else if (type == ISP_BUS_TYPE_SSP) {
        drv_ctx->bus_cb.pfn_isp_write_ssp_data = bus_cb->pfn_isp_write_ssp_data;
    } else {
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        osal_printk("The bus type %d registered to isp is err!", type);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
    return TD_SUCCESS;
}

td_s32 isp_register_piris_callback(ot_vi_pipe vi_pipe, isp_piris_callback *piris_cb)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;

    isp_check_pointer_return(piris_cb);
    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    drv_ctx->piris_cb.pfn_piris_gpio_update = piris_cb->pfn_piris_gpio_update;
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_get_dcf_info(ot_vi_pipe vi_pipe, ot_isp_dcf_info *isp_dcf)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_isp_dcf_update_info *isp_update_info = TD_NULL;
    ot_isp_dcf_const_info *isp_dcf_const_info = TD_NULL;
    unsigned long flags = 0;
    td_s32 index = 0;
    ot_isp_dcf_update_info *update_info_vir_addr = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_dcf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

#ifdef CONFIG_OT_SNAP_SUPPORT
    if (vi_pipe == drv_ctx->snap_attr.picture_pipe_id) {
        return isp_get_preview_dcf_info(drv_ctx->snap_attr.preview_pipe_id, isp_dcf);
    } else {
#endif
        isp_spin_lock = isp_drv_get_lock(vi_pipe);
        osal_spin_lock_irqsave(isp_spin_lock, &flags);

        if (drv_ctx->trans_info.update_info.vir_addr == TD_NULL) {
            isp_warn_trace("UpdateInfo buf don't init ok!\n");
            osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
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
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
#ifdef CONFIG_OT_SNAP_SUPPORT
    }
#endif

    return TD_SUCCESS;
}

td_s32 isp_get_attach_info(ot_vi_pipe vi_pipe, ot_isp_attach_info *isp_attach_info)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_attach_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if ((isp_attach_info != TD_NULL) && (drv_ctx->trans_info.atta_info.vir_addr != TD_NULL)) {
        (td_void)memcpy_s(isp_attach_info, sizeof(ot_isp_attach_info),
            (ot_isp_attach_info *)drv_ctx->trans_info.atta_info.vir_addr, sizeof(ot_isp_attach_info));
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_get_color_gamut_info(ot_vi_pipe vi_pipe, ot_isp_colorgammut_info *isp_color_gamut_info)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(isp_color_gamut_info);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_tranbuf_init_return(vi_pipe, drv_ctx->trans_info.init);

    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if ((isp_color_gamut_info != TD_NULL) && (drv_ctx->trans_info.color_gammut_info.vir_addr != TD_NULL)) {
        (td_void)memcpy_s(isp_color_gamut_info, sizeof(ot_isp_colorgammut_info),
            (ot_isp_colorgammut_info *)drv_ctx->trans_info.color_gammut_info.vir_addr, sizeof(ot_isp_colorgammut_info));
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_dng_image_dynamic_info(ot_vi_pipe vi_pipe, ot_dng_image_dynamic_info *dng_image_dynamic_info)
{
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    if (isp_drv_get_update_pos(vi_pipe) == 0) { /* frame start */
        (td_void)memcpy_s(dng_image_dynamic_info, sizeof(ot_dng_image_dynamic_info),
            &drv_ctx->dng_image_dynamic_info[1], sizeof(ot_dng_image_dynamic_info));
    } else {
        (td_void)memcpy_s(dng_image_dynamic_info, sizeof(ot_dng_image_dynamic_info),
            &drv_ctx->dng_image_dynamic_info[0], sizeof(ot_dng_image_dynamic_info));
    }

    return TD_SUCCESS;
}

td_s32 isp_drv_get_be_offline_addr_info(ot_vi_pipe vi_pipe, isp_be_offline_addr_info *be_offline_addr)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_offline_addr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    (td_void)memcpy_s(be_offline_addr, sizeof(isp_be_offline_addr_info), &drv_ctx->be_offline_addr_info,
        sizeof(isp_be_offline_addr_info));
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_ready_be_buf(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    unsigned long flags = 0;
    isp_be_buf_node *node = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_cfg_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if ((drv_ctx->exit_state == ISP_BE_BUF_EXIT) || (drv_ctx->exit_state == ISP_BE_BUF_WAITING)) {
        isp_err_trace("ViPipe[%d] ISP BE Buf not existed!!!\n", vi_pipe);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        return TD_FAILURE;
    }

    node = isp_queue_query_busy_be_buf(&drv_ctx->be_buf_queue);
    if (node == TD_NULL) {
        isp_err_trace("ViPipe[%d] QueueQueryBusyBeBuf pstNode is null!\n", vi_pipe);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        return TD_FAILURE;
    }

    node->hold_cnt++;
    drv_ctx->be_buf_info.use_cnt++;

    (td_void)memcpy_s(be_cfg_buf, sizeof(isp_be_wo_cfg_buf), &node->be_cfg_buf, sizeof(isp_be_wo_cfg_buf));

    if (node->hold_cnt == 1) {
        isp_drv_reg_config_bnr_offline((isp_be_wo_reg_cfg *)be_cfg_buf->vir_addr, drv_ctx);
        isp_drv_reg_config_vi_fpn_offline((isp_be_wo_reg_cfg *)be_cfg_buf->vir_addr, drv_ctx);
    }
    osal_flush_dcache_area(be_cfg_buf->vir_addr, be_cfg_buf->phy_addr, be_cfg_buf->size);

    isp_drv_update_be_offline_addr_info(vi_pipe, be_cfg_buf);
    drv_ctx->exit_state = ISP_BE_BUF_READY;

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_void isp_drv_put_busy_to_free(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    isp_be_buf_node *node = TD_NULL;
    struct osal_list_head *list_tmp = TD_NULL;
    struct osal_list_head *list_node = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;

    drv_ctx = isp_drv_get_ctx(vi_pipe);

    osal_list_for_each_safe(list_node, list_tmp, &drv_ctx->be_buf_queue.busy_list)
    {
        node = osal_list_entry(list_node, isp_be_buf_node, list);
        if (node->be_cfg_buf.phy_addr == be_cfg_buf->phy_addr) {
            if (node->hold_cnt > 0) {
                node->hold_cnt--;
            }

            if ((node->hold_cnt == 0) && (isp_queue_get_busy_num(&drv_ctx->be_buf_queue) > 1)) {
                isp_queue_del_busy_be_buf(&drv_ctx->be_buf_queue, node);
                isp_queue_put_free_be_buf(&drv_ctx->be_buf_queue, node);
            }
        }
    }

    return;
}

td_s32 isp_drv_put_free_be_buf(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_cfg_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (drv_ctx->exit_state == ISP_BE_BUF_EXIT) {
        isp_err_trace("ViPipe[%d] ISP BE Buf not existed!!!\n", vi_pipe);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        return TD_FAILURE;
    }

    isp_drv_put_busy_to_free(vi_pipe, be_cfg_buf);

    if (drv_ctx->be_buf_info.use_cnt > 0) {
        drv_ctx->be_buf_info.use_cnt--;
    }
    if (isp_drv_get_ldci_tpr_flt_en(vi_pipe) == TD_TRUE) {
        isp_drv_update_ldci_tpr_offline_stat(vi_pipe, (isp_be_wo_reg_cfg *)be_cfg_buf->vir_addr);
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    osal_wakeup(&drv_ctx->isp_exit_wait);

    return TD_SUCCESS;
}


td_s32 isp_drv_hold_busy_be_buf(ot_vi_pipe vi_pipe, isp_be_wo_cfg_buf *be_cfg_buf)
{
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_be_buf_node *node = TD_NULL;
    struct osal_list_head *list_tmp = TD_NULL;
    struct osal_list_head *list_node = TD_NULL;
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_cfg_buf);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_check_online_mode_return(vi_pipe, drv_ctx->work_mode.running_mode);

    isp_spin_lock = isp_drv_get_spin_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);

    if (drv_ctx->exit_state == ISP_BE_BUF_EXIT) {
        isp_err_trace("ViPipe[%d] ISP BE Buf not existed!!!\n", vi_pipe);
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
        return TD_FAILURE;
    }

    osal_list_for_each_safe(list_node, list_tmp, &drv_ctx->be_buf_queue.busy_list)
    {
        node = osal_list_entry(list_node, isp_be_buf_node, list);
        if (node->be_cfg_buf.phy_addr == be_cfg_buf->phy_addr) {
            node->hold_cnt++;
            drv_ctx->be_buf_info.use_cnt++;
        }
    }

    if (drv_ctx->exit_state != ISP_BE_BUF_WAITING) {
        drv_ctx->exit_state = ISP_BE_BUF_READY;
    }

    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_be_sync_para(ot_vi_pipe vi_pipe, isp_be_sync_para *be_sync_para)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    unsigned long flags = 0;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_sync_para);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
#ifdef CONFIG_OT_VI_STITCH_GRP
        if (isp_drv_get_stitch_be_sync_para(vi_pipe, be_sync_para) != TD_SUCCESS) {
            return TD_FAILURE;
        }
#endif
    } else {
        osal_spin_lock_irqsave(isp_spin_lock, &flags);
        (td_void)memcpy_s(be_sync_para, sizeof(isp_be_sync_para), &drv_ctx->be_sync_para, sizeof(isp_be_sync_para));
        osal_spin_unlock_irqrestore(isp_spin_lock, &flags);
    }
    return TD_SUCCESS;
}

/* vi get pubAttr */
td_s32 isp_get_pub_attr(ot_vi_pipe vi_pipe, ot_isp_pub_attr *pub_attr)
{
    unsigned long flags = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    osal_spinlock_t *isp_spin_lock = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(pub_attr);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (!drv_ctx->pub_attr_ok) {
        return TD_FAILURE;
    }
    isp_spin_lock = isp_drv_get_lock(vi_pipe);
    osal_spin_lock_irqsave(isp_spin_lock, &flags);
    (td_void)memcpy_s(pub_attr, sizeof(ot_isp_pub_attr), &drv_ctx->proc_pub_info, sizeof(ot_isp_pub_attr));
    osal_spin_unlock_irqrestore(isp_spin_lock, &flags);

    return TD_SUCCESS;
}

td_s32 isp_drv_get_fpn_sum(ot_vi_pipe vi_pipe, td_u64 *sum, td_phys_addr_t stt_phy_addr)
{
    td_u32 i, k;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_offline_stat_reg_type *stat_tmp = TD_NULL;
    isp_pre_be_offline_stat_type *pre_stt = TD_NULL;
    td_void *vir_addr = TD_NULL;
    td_phys_addr_t  phy_addr;
    td_u64  size;
    td_bool addr_match = TD_FALSE;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(sum);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (!drv_ctx->isp_init) {
        return TD_FAILURE;
    }
    if (is_online_mode(drv_ctx->work_mode.running_mode) ||
        is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        return TD_FAILURE;
    }
    for (k = 0; k < OT_ISP_STRIPING_MAX_NUM; k++) {
        if (addr_match == TD_TRUE) {
            break;
        }
        for (i = 0; i < PING_PONG_NUM; i++) {
            vir_addr = drv_ctx->be_off_stt_buf.be_stt_buf[i].virt_addr[k];
            phy_addr = drv_ctx->be_off_stt_buf.be_stt_buf[i].phys_addr[k];
            if (phy_addr == stt_phy_addr) {
                addr_match = TD_TRUE;
                break;
            }
        }
    }
    if (addr_match == TD_FALSE) {
        return TD_FAILURE;
    }
    size     =  sizeof(isp_be_offline_stat_reg_type);
    ot_mmb_invalid_cache_byaddr(vir_addr, phy_addr, size);

    stat_tmp = (isp_be_offline_stat_reg_type *)vir_addr;
    isp_check_pointer_return(stat_tmp);
    pre_stt = &stat_tmp->pre_be_stat;
    isp_check_pointer_return(pre_stt);
    *sum = pre_stt->isp_fpn_sum0_rstt.u32 +
        ((td_u64)(pre_stt->isp_fpn_sum1_rstt.u32) << 32); /* 32: hight 32bit */
    return TD_SUCCESS;
}

td_bool isp_drv_get_wbf_en(ot_vi_pipe vi_pipe)
{
    return TD_FALSE;
}

static td_s32 isp_drv_update_pts(ot_vi_pipe vi_pipe, td_u64 pts)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    drv_ctx->frame_pts = pts;
    return TD_SUCCESS;
}

static td_s32 isp_drv_vi_set_frame_info(ot_vi_pipe vi_pipe, ot_isp_frame_info *isp_frame)
{
    td_u8 i = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    drv_ctx->proc_frame_info.vi_send_raw_cnt += 1;
    drv_ctx->proc_frame_info.print_en = TD_TRUE;

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        drv_ctx->proc_frame_info.again[i] = isp_frame->again[i];
        drv_ctx->proc_frame_info.dgain[i] = isp_frame->dgain[i];
        drv_ctx->proc_frame_info.isp_dgain[i] = isp_frame->isp_dgain[i];
        drv_ctx->proc_frame_info.exposure_time[i] = isp_frame->exposure_time[i];
        drv_ctx->proc_frame_info.fe_id[i] = isp_frame->fe_id[i];
    }

    return TD_SUCCESS;
}

isp_export_func g_isp_exp_func = {
    .pfn_isp_register_bus_callback = isp_register_bus_callback,
    .pfn_isp_register_piris_callback = isp_register_piris_callback,
    .pfn_isp_get_dcf_info = isp_get_dcf_info,
    .pfn_isp_get_frame_info = isp_get_frame_info,
    .pfn_isp_get_attach_info = isp_get_attach_info,
    .pfn_isp_get_color_gamut_info = isp_get_color_gamut_info,
    .pfn_isp_get_dng_image_dynamic_info = isp_drv_get_dng_image_dynamic_info,
    .pfn_isp_drv_get_be_offline_addr_info = isp_drv_get_be_offline_addr_info,
    .pfn_isp_drv_get_ready_be_buf = isp_drv_get_ready_be_buf,
    .pfn_isp_drv_put_free_be_buf = isp_drv_put_free_be_buf,
    .pfn_isp_drv_hold_busy_be_buf = isp_drv_hold_busy_be_buf,
    .pfn_isp_drv_get_be_sync_para = isp_drv_get_be_sync_para,
    .pfn_isp_drv_set_be_sync_para = isp_drv_set_be_sync_para_offline,
    .pfn_isp_drv_set_be_format = isp_drv_set_be_format,
    .pfn_isp_drv_be_end_int_proc = isp_drv_be_end_int_proc,
    .pfn_isp_register_sync_task = ot_isp_sync_task_register,
    .pfn_isp_unregister_sync_task = ot_isp_sync_task_unregister,
    .pfn_isp_int_bottom_half = isp_int_bottom_half,
    .pfn_isp_isr = isp_isr,
    .pfn_isp_get_pub_attr = isp_get_pub_attr,
    .pfn_isp_drv_get_fpn_sum = isp_drv_get_fpn_sum,
    .pfn_isp_drv_switch_be_offline_stt_info = isp_drv_switch_be_offline_stt_info,
    .pfn_isp_drv_get_be_offline_stt_addr = isp_drv_get_be_offline_stt_addr,
    .pfn_isp_drv_is_stitch_sync_lost_frame = isp_drv_is_stitch_sync_lost_frame,
    .pfn_isp_drv_get_wbf_en = isp_drv_get_wbf_en,
    .pfn_isp_update_pts = isp_drv_update_pts,
    .pfn_isp_drv_vi_set_frame_info = isp_drv_vi_set_frame_info,
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
    .pfn_isp_drv_update_hnr_be_cfg = isp_drv_update_hnr_be_cfg,
    .pfn_isp_drv_update_hnr_normal_blend = isp_drv_update_hnr_normal_blend,
#endif
};

td_void *isp_get_export_func(td_void)
{
    return (td_void *)&g_isp_exp_func;
}
