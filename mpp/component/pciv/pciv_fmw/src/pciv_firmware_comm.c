/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "pciv_firmware_comm.h"

#include "securec.h"
#include "sys_ext.h"
#include "mm_ext.h"

#include "pciv_fmwext.h"
#include "pciv_firmware.h"
#include "pciv_pic_queue.h"

#define FMW_MEMORY_STRIDE  64
#define FMW_PIXEL_FMT_NUM  5
#define FMW_RGN_LAYER_MIN  0
#define FMW_RGN_LAYER_MAX  (OT_RGN_PCIV_MAX_OVERLAYEX_NUM - 1)

#define FMW_POINT_START_X_ALIGN        2
#define FMW_POINT_START_Y_ALIGN        2

td_s32 pciv_fmw_get_rgn(ot_pciv_chn pciv_chn, ot_rgn_type type, rgn_info *info)
{
    td_s32      ret;
    ot_mpp_chn  chn;

    if (!ckfn_rgn() || !ckfn_rgn_get_region()) {
        return TD_FAILURE;
    }

    chn.mod_id = OT_ID_PCIV;
    chn.chn_id = pciv_chn;
    chn.dev_id = 0;
    ret = call_rgn_get_region(type, &chn, info);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("get rgn error! chn%d\n", pciv_chn);
    }
    return ret;
}

td_void pciv_fmw_put_rgn(ot_pciv_chn pciv_chn, ot_rgn_type type)
{
    td_s32      ret;
    ot_mpp_chn  chn;

    if (!ckfn_rgn() || !ckfn_rgn_put_region()) {
        return;
    }

    chn.mod_id = OT_ID_PCIV;
    chn.chn_id = pciv_chn;
    chn.dev_id = 0;

    ret = call_rgn_put_region(type, &chn, TD_NULL);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("put rng error! chn%d\n", pciv_chn);
    }
}

td_s32 pciv_fmw_vb_user_add(const ot_video_frame_info *img_info)
{
    td_s32 ret;

    if (img_info->video_frame.compress_mode == OT_COMPRESS_MODE_NONE) {
        ret = call_vb_user_add(img_info->pool_id, img_info->video_frame.phys_addr[0], OT_VB_UID_PCIV);
    } else {
        ret = call_vb_user_add(img_info->pool_id, img_info->video_frame.header_phys_addr[0], OT_VB_UID_PCIV);
    }
    if (ret != TD_SUCCESS) {
        pciv_fmw_alert_trace("add vb failed\n");
        return ret;
    }
    return TD_SUCCESS;
}

td_void pciv_fmw_vb_user_sub(const ot_video_frame_info *img_info)
{
    td_s32 ret;

    if (img_info->video_frame.compress_mode == OT_COMPRESS_MODE_NONE) {
        ret = call_vb_user_sub(img_info->pool_id, img_info->video_frame.phys_addr[0], OT_VB_UID_PCIV);
    } else {
        ret = call_vb_user_sub(img_info->pool_id, img_info->video_frame.header_phys_addr[0], OT_VB_UID_PCIV);
    }
    if (ret != TD_SUCCESS) {
        pciv_fmw_alert_trace("release vb failed\n");
    }
}

td_s32 pciv_check_input_frame_size(ot_pciv_chn pciv_chn, const ot_video_frame *video_frame)
{
    vb_base_info       base_info;
    ot_vb_calc_cfg     vb_calc_cfg;
    const pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    pciv_fmw_check_ptr_return(video_frame);

    base_info.mod_id = OT_ID_PCIVFMW;
    base_info.is_3dnr_buffer = TD_FALSE;
    base_info.align = 0;

    base_info.compress_mode = video_frame->compress_mode;
    base_info.dynamic_range = video_frame->dynamic_range;
    base_info.video_format = video_frame->video_format;

    base_info.pixel_format = video_frame->pixel_format;
    base_info.width = video_frame->width;
    base_info.height = video_frame->height;

    if (!ckfn_sys_get_vb_cfg()) {
        pciv_fmw_err_trace("sys_get_vb_cfg is null!\n");
        return TD_FAILURE;
    }
    call_sys_get_vb_cfg(&base_info, &vb_calc_cfg);

    if (vb_calc_cfg.vb_size > fmw_chn->blk_size) {
        pciv_fmw_err_trace("the size(%d) of input image is big than the pciv(%d) chn buff(%d)!\n",
            vb_calc_cfg.vb_size, pciv_chn, fmw_chn->blk_size);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_void pciv_fmw_src_pic_release(ot_pciv_chn pciv_chn)
{
    ot_video_frame_info *frame   = TD_NULL;
    pciv_fmw_chn        *fmw_chn = pciv_fmw_get_context(pciv_chn);

    if (!fmw_chn->is_master && fmw_chn->cur_vdec_node != TD_NULL) {
        frame = &(fmw_chn->cur_vdec_node->pciv_pic.frame_info);

        pciv_fmw_vb_user_sub(frame);

        pciv_pic_queue_put_free(&fmw_chn->pic_queue, fmw_chn->cur_vdec_node);
        fmw_chn->cur_vdec_node = TD_NULL;
    }
}

td_void pciv_firmware_chn_base_init(ot_pciv_chn pciv_chn)
{
    td_s32       i;
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    fmw_chn->is_start           = TD_FALSE;
    /*
     * the member should initialize to 0,otherwise int re-creat and re-dostroy and switch the bind-ship situation,
     * it may occur receive the wrong image for out-of-order
     */
    fmw_chn->time_ref           = 0;
    fmw_chn->send_cnt           = 0;
    fmw_chn->get_cnt            = 0;
    fmw_chn->resp_cnt           = 0;
    fmw_chn->lost_cnt           = 0;
    fmw_chn->new_do_cnt         = 0;
    fmw_chn->old_undo_cnt       = 0;
    fmw_chn->send_state         = PCIVFMW_SEND_OK;
    fmw_chn->timer_cnt          = 0;
    fmw_chn->rgn_num            = 0;

    fmw_chn->add_job_suc_cnt    = 0;
    fmw_chn->add_job_fail_cnt   = 0;

    fmw_chn->trans_task_suc_cnt  = 0;
    fmw_chn->trans_task_fail_cnt = 0;

    fmw_chn->osd_task_suc_cnt   = 0;
    fmw_chn->osd_task_fail_cnt  = 0;

    fmw_chn->end_job_suc_cnt    = 0;
    fmw_chn->end_job_fail_cnt   = 0;

    fmw_chn->trans_cb_cnt       = 0;
    fmw_chn->osd_cb_cnt         = 0;

    for (i = 0; i < OT_PCIV_MAX_BUF_NUM; i++) {
        fmw_chn->is_pciv_hold[i] = TD_FALSE;
    }
}

static td_bool pciv_firmware_is_pix_fmt_sp420(ot_pixel_format pix_fmt)
{
    if ((pix_fmt == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ||
        (pix_fmt == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420)) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

static td_bool pciv_firmware_is_pix_fmt_sp422(ot_pixel_format pix_fmt)
{
    if ((pix_fmt == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422) ||
        (pix_fmt == OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422)) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

static td_s32 pciv_firmware_check_pic_attr(const ot_pciv_attr *attr)
{
    const ot_pciv_pic_attr *pic_attr = &attr->pic_attr;

    /* Check The Image Width and Height */
    if (!pic_attr->height || !pic_attr->width || (pic_attr->height & 1) || (pic_attr->width & 1)) {
        pciv_fmw_err_trace("pic w:%d, h:%d invalid\n",
            attr->pic_attr.width, attr->pic_attr.height);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if (pic_attr->stride[0] < pic_attr->width) {
        pciv_fmw_err_trace("pic stride0:%d invalid\n", pic_attr->stride[0]);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((pic_attr->stride[0] & 0xf)) {
        pciv_fmw_err_trace("illegal param: not align stride(y_stride:%d)\n", pic_attr->stride[0]);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((pciv_firmware_is_pix_fmt_sp420(pic_attr->pixel_format) == TD_FALSE) &&
        (pciv_firmware_is_pix_fmt_sp422(pic_attr->pixel_format) == TD_FALSE) &&
        (pic_attr->pixel_format != OT_PIXEL_FORMAT_YUV_400)) {
        pciv_fmw_err_trace("illegal param: illegal pixel_format:%d)\n", pic_attr->pixel_format);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((pic_attr->compress_mode < OT_COMPRESS_MODE_NONE) || (pic_attr->compress_mode >= OT_COMPRESS_MODE_BUTT)) {
        pciv_fmw_err_trace("illegal param: not permmit compress mode:%d\n", pic_attr->compress_mode);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if (pic_attr->dynamic_range != OT_DYNAMIC_RANGE_SDR8) {
        pciv_fmw_err_trace("illegal param: not permmit dynamic_range mode:%d\n", pic_attr->dynamic_range);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((pic_attr->video_format < OT_VIDEO_FORMAT_LINEAR) || (pic_attr->video_format >= OT_VIDEO_FORMAT_BUTT)) {
        pciv_fmw_err_trace("illegal param: not permmit video_format mode:%d\n", pic_attr->video_format);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((pic_attr->field < OT_VIDEO_FIELD_TOP) || (pic_attr->field >= OT_VIDEO_FIELD_BUTT)) {
        pciv_fmw_err_trace("illegal param: not permmit field mode:%d\n", attr->pic_attr.field);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

td_s32 pciv_firmware_check_attr(const ot_pciv_attr *attr)
{
    td_s32              ret;
    vb_base_info        base_info;
    ot_vb_calc_cfg      vb_calc_cfg;

    /* Check The Image attr */
    ret = pciv_firmware_check_pic_attr(attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    base_info.mod_id = OT_ID_PCIVFMW;
    base_info.is_3dnr_buffer = TD_FALSE;
    base_info.align = 0;

    base_info.compress_mode = attr->pic_attr.compress_mode;
    base_info.dynamic_range = attr->pic_attr.dynamic_range;
    base_info.video_format  = attr->pic_attr.video_format;

    base_info.pixel_format  = attr->pic_attr.pixel_format;
    base_info.width         = attr->pic_attr.width;
    base_info.height        = attr->pic_attr.height;

    if (!ckfn_sys_get_vb_cfg()) {
        pciv_fmw_err_trace("sys_get_vb_cfg is null!\n");
        return TD_FAILURE;
    }
    call_sys_get_vb_cfg(&base_info, &vb_calc_cfg);

    /* Check the Image attr is match or not with the buffer size */
    if (attr->blk_size < vb_calc_cfg.vb_size) {
        pciv_fmw_err_trace("Buffer block is too small(%d < %d)\n", attr->blk_size, vb_calc_cfg.vb_size);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 pciv_firmware_set_fmw_chn(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id)
{
    td_u32       i;
    td_s32       ret;
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    ret = memcpy_s(&fmw_chn->pic_attr, sizeof(ot_pciv_pic_attr), &attr->pic_attr, sizeof(ot_pciv_pic_attr));
    if (ret != EOK) {
        pciv_fmw_err_trace("chn%d copy pic_attr failed\n", pciv_chn);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if (local_id == 0) {
        for (i = 0; i < attr->blk_cnt; i++) {
            if (fmw_chn->phys_addr[i] == 0) {
                pciv_fmw_err_trace("chn%d not malloc buff\n", pciv_chn);
                return OT_ERR_PCIV_NOT_PERM;
            }
            ret = cmpi_check_mmz_phy_addr(attr->phys_addr[i], attr->blk_size);
            if (ret != TD_SUCCESS) {
                pciv_fmw_err_trace("chn%d, attr->phys_addr[%u]:0x%lx and attr->blk_size:%u is illegal!\n",
                    pciv_chn, i, (td_ulong)attr->phys_addr[i], attr->blk_size);
                return OT_ERR_PCIV_ILLEGAL_PARAM;
            }
            if (fmw_chn->phys_addr[i] != attr->phys_addr[i]) {
                pciv_fmw_err_trace("chn%d, buffer address:0x%lx is invalid!\n",
                    pciv_chn, (td_ulong)attr->phys_addr[i]);
                return OT_ERR_PCIV_NOT_PERM;
            }
        }
    } else {
        ret = memcpy_s(fmw_chn->phys_addr, sizeof(attr->phys_addr), attr->phys_addr, sizeof(attr->phys_addr));
        if (ret != EOK) {
            pciv_fmw_err_trace("chn%d copy phys_addr failed\n", pciv_chn);
            return OT_ERR_PCIV_ILLEGAL_PARAM;
        }
    }
    return TD_SUCCESS;
}

td_s32 pciv_firmware_put_pic_to_queue(ot_pciv_chn pciv_chn, const ot_video_frame_info *video_frm_info, td_u32 index,
    td_bool is_block)
{
    td_s32          ret;
    pciv_pic_node   *node    = TD_NULL;
    pciv_fmw_chn    *fmw_chn = pciv_fmw_get_context(pciv_chn);

    if (fmw_chn->is_start != TD_TRUE) {
        return OT_ERR_PCIV_NOT_READY;
    }

    if (index < 0 || index >= OT_PCIV_MAX_BUF_NUM) {
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    fmw_chn->is_pciv_hold[index] = TD_TRUE;
    node = pciv_pic_queue_get_free(&fmw_chn->pic_queue);
    if (node == TD_NULL) {
        pciv_fmw_err_trace("chn%d no free node\n", pciv_chn);
        return TD_FAILURE;
    }

    node->pciv_pic.is_block = is_block;
    node->pciv_pic.index = index;
    ret = memcpy_s(&node->pciv_pic.frame_info, sizeof(ot_video_frame_info),
        video_frm_info, sizeof(ot_video_frame_info));
    if (ret != EOK) {
        pciv_pic_queue_put_free(&fmw_chn->pic_queue, node);
        pciv_fmw_err_trace("chn%d copy frame info failed\n", pciv_chn);
        return TD_FAILURE;
    }
    pciv_pic_queue_put_busy(&fmw_chn->pic_queue, node);

    return TD_SUCCESS;
}

td_s32 pciv_firmware_get_pic_from_queue_and_send(ot_pciv_chn pciv_chn)
{
    td_s32              ret               = TD_SUCCESS;
    pciv_fmw_chn        *fmw_chn          = pciv_fmw_get_context(pciv_chn);
    ot_video_frame_info *video_frame_info = TD_NULL;
    ot_mpp_chn          mpp_chn;

    if (fmw_chn->is_start != TD_TRUE) {
        return OT_ERR_PCIV_NOT_READY;
    }

    /* send the data in cycle queue, until the data in queue is not less or send fail */
    while (pciv_pic_queue_get_busy_num(&fmw_chn->pic_queue)) {
        fmw_chn->cur_vdec_node = pciv_pic_queue_get_busy(&fmw_chn->pic_queue);
        if (fmw_chn->cur_vdec_node == TD_NULL) {
            pciv_fmw_err_trace("chn%d busy list is empty, no vdec pic\n", pciv_chn);
            return TD_FAILURE;
        }

        /*
         * send the vdec image to vpss or venc or vo, if success, put the node to fee queue, else nothing to do
         * it will send by the time or next interrupt
         */
        fmw_chn->send_state = PCIVFMW_SEND_ING;
        video_frame_info = &fmw_chn->cur_vdec_node->pciv_pic.frame_info;
        pciv_fmw_warn_trace("chn%d, start: %d, start send pic to VPSS/VO.\n", pciv_chn, fmw_chn->is_start);
        call_sys_set_mpp_chn(&mpp_chn, OT_ID_PCIV, PCIV_DEV_ID, pciv_chn);
        ret = call_sys_send_data(&mpp_chn, fmw_chn->cur_vdec_node->pciv_pic.is_block,
            MPP_DATA_VDEC_FRAME, video_frame_info);
        if ((ret != TD_SUCCESS) && (fmw_chn->cur_vdec_node->pciv_pic.is_block == TD_TRUE)) {
            /*
             * bBlock is true(playback mode),if failed,get out of the circle,
             * do nothing ,it will send by the timer or next DMA interrupt
             * set the point NULL,put the node to the head of busy,while
             * send again, get it from the header of busy
             */
            pciv_fmw_warn_trace("chn%d, start: %d, finish send pic to VPSS/VO.\n", pciv_chn, fmw_chn->is_start);
            pciv_fmw_info_trace("chn%d send pic failed, put to queue and send again. ret: 0x%x\n", pciv_chn, ret);
            pciv_pic_queue_put_busy_head(&fmw_chn->pic_queue, fmw_chn->cur_vdec_node);
            fmw_chn->send_state = PCIVFMW_SEND_NOK;
            fmw_chn->cur_vdec_node = TD_NULL;
            ret = TD_SUCCESS;
            break;
        } else {
            /*
             * bBlock is true(playback mode),if success, put the node to free
             * bBlock is false(preview mode),no matter success or not, put the
             * node to free,and do not send the Image again
             */
            pciv_fmw_warn_trace("chn%d, start: %d, finish send pic to VPSS/VO.\n", pciv_chn, fmw_chn->is_start);
            pciv_fmw_info_trace("chn%d send pic ok\n", pciv_chn);
            fmw_chn->is_pciv_hold[fmw_chn->cur_vdec_node->pciv_pic.index] = TD_FALSE;
            pciv_pic_queue_put_free(&fmw_chn->pic_queue, fmw_chn->cur_vdec_node);
            fmw_chn->send_state = PCIVFMW_SEND_OK;
            fmw_chn->cur_vdec_node = TD_NULL;
            ret = TD_SUCCESS;
        }
    }

    return ret;
}

static td_s32 pciv_firmware_receiver_send_vdec_pic(ot_pciv_chn pciv_chn,
    ot_video_frame_info *video_frm_info, td_u32 index, td_bool is_block)
{
    td_s32          ret;
    td_u32          busy_num;
    pciv_fmw_chn    *fmw_chn = pciv_fmw_get_context(pciv_chn);
    ot_mpp_chn      mpp_chn;

    if (fmw_chn->is_start != TD_TRUE) {
        return OT_ERR_PCIV_NOT_READY;
    }

    /*
     * When the DMA arrive,first query the queue if has data or not,
     * if yes,send the data in the queue first,
     * if not,send the cuurent Image directly; if success, return,
     * else put the node queue,and trig by the timer next time
     */
    busy_num = pciv_pic_queue_get_busy_num(&fmw_chn->pic_queue);
    if (busy_num != 0) {
        /* if the current queue has data, put the image to the tail of the queue */
        ret = pciv_firmware_put_pic_to_queue(pciv_chn, video_frm_info, index, is_block);
        if (ret != TD_SUCCESS) {
            return TD_FAILURE;
        }

        /* Get the data from the header to send */
        ret = pciv_firmware_get_pic_from_queue_and_send(pciv_chn);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    } else {
        /*
         * if the current queue has no data,
         * send the current Image directly, if success,return success,
         * else return failure,put the Image to the tail of the queue
         */
        pciv_fmw_warn_trace("chn%d, start: %d, start send pic to VPSS/VO.\n", pciv_chn, fmw_chn->is_start);
        fmw_chn->is_pciv_hold[index] = TD_TRUE;
        call_sys_set_mpp_chn(&mpp_chn, OT_ID_PCIV, PCIV_DEV_ID, pciv_chn);
        ret = call_sys_send_data(&mpp_chn, is_block, MPP_DATA_VDEC_FRAME, video_frm_info);
        if ((ret != TD_SUCCESS) && (is_block == TD_TRUE)) {
            /*
             * bBlock is true(playback mode),if failure,put the Image to
             * the tail of the queue
             * bBlock is false(preview mode),no matter success or not,
             * we think it as success, do not put it to queue to send again
             */
            pciv_fmw_warn_trace("chn%d, start: %d, finish send pic to VPSS/VO.\n",
                pciv_chn, fmw_chn->is_start);
            pciv_fmw_info_trace("chn%d send pic failed, put to queue and send again. ret: 0x%x\n",
                pciv_chn, ret);
            if (pciv_firmware_put_pic_to_queue(pciv_chn, video_frm_info, index, is_block)) {
                return TD_FAILURE;
            }
            ret = TD_SUCCESS;
        } else {
            pciv_fmw_warn_trace("chn%d, start: %d, finish send pic to VPSS/VO.\n",
                pciv_chn, fmw_chn->is_start);
            pciv_fmw_info_trace("chn%d send pic ok\n", pciv_chn);
            fmw_chn->is_pciv_hold[index] = TD_FALSE;
            ret = TD_SUCCESS;
        }
    }

    return ret;
}

static td_void pciv_firmware_set_frame_addr(vb_blk_handle vb_handle, const ot_vb_calc_cfg *vb_calc_cfg,
    ot_video_frame *frm)
{
    frm->header_phys_addr[0] = call_vb_handle_to_phys(vb_handle);
    frm->header_phys_addr[1] = frm->header_phys_addr[0] + vb_calc_cfg->head_y_size;

    frm->header_virt_addr[0] = (td_void *)(td_uintptr_t)call_vb_handle_to_kern(vb_handle);
    frm->header_virt_addr[1] = frm->header_virt_addr[0] + vb_calc_cfg->head_y_size;

    frm->header_stride[0] = vb_calc_cfg->head_stride;
    frm->header_stride[1] = vb_calc_cfg->head_stride;

    frm->phys_addr[0] = frm->header_phys_addr[0] + vb_calc_cfg->head_size;
    frm->phys_addr[1] = frm->phys_addr[0] + vb_calc_cfg->main_y_size;

    frm->virt_addr[0] = frm->header_virt_addr[0] + vb_calc_cfg->head_size;
    frm->virt_addr[1] = frm->virt_addr[0] + vb_calc_cfg->main_y_size;

    frm->stride[0] = vb_calc_cfg->main_stride;
    frm->stride[1] = vb_calc_cfg->main_stride;
}

td_void pciv_fmw_config_out_frame(ot_pciv_chn pciv_chn,
    const vb_blk_handle vb_handle, const ot_vb_calc_cfg *calc_cfg, ot_video_frame *out_frame)
{
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    out_frame->width        = fmw_chn->pic_attr.width;
    out_frame->height       = fmw_chn->pic_attr.height;

    out_frame->pixel_format  = fmw_chn->pic_attr.pixel_format;
    out_frame->compress_mode = fmw_chn->pic_attr.compress_mode;
    out_frame->dynamic_range = fmw_chn->pic_attr.dynamic_range;
    out_frame->video_format  = fmw_chn->pic_attr.video_format;
    out_frame->field         = fmw_chn->pic_attr.field;

    pciv_firmware_set_frame_addr(vb_handle, calc_cfg, out_frame);
}

static td_void pciv_firmware_set_src_frame_addr(vb_blk_handle vb_handle,
    const pciv_pic *recv_pic, ot_video_frame *frm)
{
    td_phys_addr_t base_phys;
    td_void        *base_virt;
    td_s32         i;

    base_phys = call_vb_handle_to_phys(vb_handle);
    base_virt = (td_void *)(td_uintptr_t)call_vb_handle_to_kern(vb_handle);

    for (i = 0; i < OT_MAX_COLOR_COMPONENT; i++) {
        frm->header_phys_addr[i] = base_phys + recv_pic->header_offset[i];
        frm->header_virt_addr[i] = base_virt + recv_pic->header_offset[i];
        frm->header_stride[i] = recv_pic->header_stride[i];
        frm->phys_addr[i] = base_phys + recv_pic->data_offset[i];
        frm->virt_addr[i] = base_virt + recv_pic->data_offset[i];
        frm->stride[i] = recv_pic->stride[i];
    }
}

td_void pciv_firmware_recv_set_video_frame(ot_pciv_chn pciv_chn, ot_video_frame_info *video_frm_info,
    const pciv_pic *recv_pic, vb_blk_handle vb_handle)
{
    const pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);
    ot_video_frame     *vfrm    = TD_NULL;
    td_s32             i;

    video_frm_info->pool_id  = call_vb_handle_to_pool_id(vb_handle);
    video_frm_info->mod_id   = recv_pic->mod_id;
    vfrm                    = &video_frm_info->video_frame;

    vfrm->width         = fmw_chn->pic_attr.width;
    vfrm->height        = fmw_chn->pic_attr.height;
    vfrm->pixel_format  = fmw_chn->pic_attr.pixel_format;
    vfrm->dynamic_range = fmw_chn->pic_attr.dynamic_range;
    vfrm->compress_mode = fmw_chn->pic_attr.compress_mode;
    vfrm->video_format  = fmw_chn->pic_attr.video_format;

    vfrm->pts           = recv_pic->pts;
    vfrm->time_ref      = recv_pic->time_ref;
    vfrm->field         = recv_pic->filed;
    vfrm->color_gamut   = recv_pic->color_gamut;

    vfrm->frame_flag = recv_pic->frame_flag;
    for (i = 0; i < OT_MAX_USER_DATA_NUM; i++) {
        vfrm->user_data[i] = recv_pic->user_data[i];
    }

    pciv_firmware_set_src_frame_addr(vb_handle, recv_pic, vfrm);
}

td_bool pciv_firmware_is_vb_can_release(ot_pciv_chn pciv_chn, td_s32 index)
{
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    /* if bPcivHold is false,only when the VB count occupied by vo/vpss/venc is 0,the VB can release */
    if (call_vb_inquire_one_user_cnt(call_vb_phy_to_handle(fmw_chn->phys_addr[index]), OT_VB_UID_VO) != 0) {
        return TD_FALSE;
    }
    if (call_vb_inquire_one_user_cnt(call_vb_phy_to_handle(fmw_chn->phys_addr[index]), OT_VB_UID_VPSS) != 0) {
        return TD_FALSE;
    }
    if (call_vb_inquire_one_user_cnt(call_vb_phy_to_handle(fmw_chn->phys_addr[index]), OT_VB_UID_VENC) != 0) {
        return TD_FALSE;
    }
    if (fmw_chn->is_pciv_hold[index] == TD_TRUE) {
        return TD_FALSE;
    }
    return TD_TRUE;
}

td_void pciv_fmw_set_src_pic(pciv_pic *src_pic, const pciv_bind_obj *bind_obj,
    const ot_video_frame_info *video_frame)
{
    vb_blk_handle   vb_handle;
    td_phys_addr_t  base_addr;
    td_s32          i;

    src_pic->pool_id         = video_frame->pool_id;
    src_pic->src_type        = bind_obj->type;
    src_pic->block           = bind_obj->is_block;
    src_pic->mod_id          = video_frame->mod_id;
    src_pic->pts             = video_frame->video_frame.pts;
    src_pic->time_ref        = video_frame->video_frame.time_ref;
    src_pic->filed           = video_frame->video_frame.field;
    src_pic->color_gamut     = video_frame->video_frame.color_gamut;
    src_pic->frame_flag      = video_frame->video_frame.frame_flag;
    for (i = 0; i < OT_MAX_USER_DATA_NUM; i++) {
        src_pic->user_data[i] = video_frame->video_frame.user_data[i];
    }

    /*
     * The reason why here add if&else is that there is some situation that
     * some module doesn't set value to phy addr in the mode of noncompress
     */
    if (video_frame->video_frame.compress_mode == OT_COMPRESS_MODE_NONE) {
        vb_handle = call_vb_phy_to_handle(video_frame->video_frame.phys_addr[0]);
    } else {
        vb_handle = call_vb_phy_to_handle(video_frame->video_frame.header_phys_addr[0]);
    }

    base_addr = call_vb_handle_to_phys(vb_handle);
    for (i = 0; i < OT_MAX_COLOR_COMPONENT; i++) {
        src_pic->header_offset[i] = video_frame->video_frame.header_phys_addr[i] - base_addr;
        src_pic->data_offset[i]   = video_frame->video_frame.phys_addr[i] - base_addr;
        src_pic->stride[i]        = video_frame->video_frame.stride[i];
        src_pic->header_stride[i] = video_frame->video_frame.header_stride[i];
    }
    src_pic->phys_addr = base_addr;
}

td_s32 pciv_fmw_get_vb_calc_cfg(ot_pciv_chn pciv_chn, ot_vb_calc_cfg *vb_calc_cfg)
{
    vb_base_info    base_info;
    pciv_fmw_chn    *fmw_chn = pciv_fmw_get_context(pciv_chn);

    base_info.mod_id = OT_ID_PCIVFMW;
    base_info.is_3dnr_buffer = TD_FALSE;
    base_info.align = 0;

    base_info.compress_mode = fmw_chn->pic_attr.compress_mode;
    base_info.dynamic_range = fmw_chn->pic_attr.dynamic_range;
    base_info.video_format  = fmw_chn->pic_attr.video_format;
    base_info.pixel_format  = fmw_chn->pic_attr.pixel_format;
    base_info.width         = fmw_chn->pic_attr.width;
    base_info.height        = fmw_chn->pic_attr.height;

    if (!ckfn_sys_get_vb_cfg()) {
        pciv_fmw_err_trace("sys_get_vb_cfg is null!\n");
        return TD_FAILURE;
    }
    call_sys_get_vb_cfg(&base_info, vb_calc_cfg);
    return TD_SUCCESS;
}

td_s32 pciv_fmw_get_vgs_out_vb(ot_pciv_chn pciv_chn, vb_blk_handle *vb_handle)
{
    ot_mpp_chn      mpp_chn;
    td_void         *mmz_name = TD_NULL;
    pciv_fmw_chn    *fmw_chn = pciv_fmw_get_context(pciv_chn);

    /* the output Image is same to chn size, request VB */
    if (!ckfn_sys_get_mmz_name()) {
        pciv_fmw_err_trace("GetMmzName is null!\n");
        return TD_FAILURE;
    }

    mpp_chn.mod_id = OT_ID_PCIV;
    mpp_chn.dev_id = 0;
    mpp_chn.chn_id = pciv_chn;
    call_sys_get_mmz_name(&mpp_chn, &mmz_name);

    *vb_handle = call_vb_get_blk_by_size(fmw_chn->blk_size, OT_VB_UID_PCIV, mmz_name);
    if (*vb_handle == OT_VB_INVALID_HANDLE) {
        pciv_fmw_err_trace("=======get VB(%d_byte) buffer for image out fail,chn: %d.=======\n",
            fmw_chn->blk_size, pciv_chn);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 pciv_fmw_config_vgs_opt(const rgn_info *info, vgs_online_opt *vgs_opt)
{
    td_u32 i;
    td_s32 ret;

    if (info->num > OT_RGN_PCIV_MAX_OVERLAYEX_NUM || info->num > MAX_VGS_OSD) {
        pciv_fmw_err_trace("OSD number %d is too much!\n", info->num);
        return TD_FAILURE;
    }

    if (info->num > 0) {
        vgs_opt->osd = TD_TRUE;
    } else {
        vgs_opt->osd = TD_FALSE;
    }

    for (i = 0; i < info->num; i++) {
        vgs_opt->osd_opt[i].osd_en = TD_TRUE;
        vgs_opt->osd_opt[i].global_alpha = info->get_info[i].common->chn_attr.overlay.global_alpha;

        vgs_opt->osd_opt[i].osd.phys_addr = info->get_info[i].common->phys_addr;
        vgs_opt->osd_opt[i].osd.pixel_format = info->get_info[i].common->overlay_attr.pixel_format;
        vgs_opt->osd_opt[i].osd.stride = info->get_info[i].common->stride;

        if (vgs_opt->osd_opt[i].osd.pixel_format == OT_PIXEL_FORMAT_ARGB_1555) {
            vgs_opt->osd_opt[i].osd.alpha_ext1555 = TD_TRUE;
            vgs_opt->osd_opt[i].osd.alpha0 = info->get_info[i].common->chn_attr.overlay.bg_alpha;
            vgs_opt->osd_opt[i].osd.alpha1 = info->get_info[i].common->chn_attr.overlay.fg_alpha;
        }

        if (vgs_opt->osd_opt[i].osd.pixel_format == OT_PIXEL_FORMAT_ARGB_CLUT2 ||
            vgs_opt->osd_opt[i].osd.pixel_format == OT_PIXEL_FORMAT_ARGB_CLUT4) {
            ret = memcpy_s(vgs_opt->osd_opt[i].clut, sizeof(vgs_opt->osd_opt[i].clut),
                info->get_info[i].common->overlay_attr.clut,  sizeof(info->get_info[i].common->overlay_attr.clut));
            if (ret != EOK) {
                pciv_fmw_err_trace("copy clut info failed!\n");
                return TD_FAILURE;
            }
        }

        vgs_opt->osd_opt[i].osd_rect.x = info->get_info[i].common->point.x;
        vgs_opt->osd_opt[i].osd_rect.y = info->get_info[i].common->point.y;
        vgs_opt->osd_opt[i].osd_rect.height = info->get_info[i].common->size.height;
        vgs_opt->osd_opt[i].osd_rect.width = info->get_info[i].common->size.width;
    }
    return TD_SUCCESS;
}

td_s32 pciv_firmware_send_data(ot_pciv_chn pciv_chn, const pciv_pic *recv_pic, ot_video_frame_info *video_frm_info)
{
    td_s32 ret;
    td_s32 chn_id = pciv_chn;
    ot_mpp_chn mpp_chn;
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    call_sys_set_mpp_chn(&mpp_chn, OT_ID_PCIV, PCIV_DEV_ID, chn_id);
    if (recv_pic->src_type == PCIV_BIND_VI) {
        ret = call_sys_send_data(&mpp_chn, recv_pic->block, MPP_DATA_VI_FRAME, video_frm_info);
    } else if (recv_pic->src_type == PCIV_BIND_VO) {
        ret = call_sys_send_data(&mpp_chn, recv_pic->block, MPP_DATA_VOU_FRAME, video_frm_info);
    } else if (recv_pic->src_type == PCIV_BIND_VDEC) {
        ret = pciv_firmware_receiver_send_vdec_pic(pciv_chn, video_frm_info, recv_pic->index, recv_pic->block);
    } else {
        pciv_fmw_err_trace("chn%d bind type error, type value:%d.\n", pciv_chn, recv_pic->src_type);
        ret = TD_FAILURE;
    }

    if ((ret != TD_SUCCESS) && (ret != OT_ERR_VO_NOT_ENABLE)) {
        pciv_fmw_err_trace("chn%d call_sys_send_data failed, ret:0x%x\n", pciv_chn, ret);
        fmw_chn->lost_cnt++;
    } else {
        fmw_chn->send_cnt++;
    }
    return ret;
}

td_s32 pciv_fmw_state_check(ot_pciv_chn pciv_chn)
{
    const pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    if (fmw_chn->is_create == TD_FALSE) {
        pciv_fmw_alert_trace("chn doesn't exist, please create it!\n");
        return TD_FAILURE;
    }

    if (fmw_chn->is_start == TD_FALSE) {
        pciv_fmw_alert_trace("chn has stopped!\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
td_void pciv_fmw_vpss_query_set_video_frame(ot_pciv_chn pciv_chn, vb_blk_handle vb_handle,
    const ot_vb_calc_cfg *vb_config, const vpss_query_info *query_info, ot_video_frame *video_frame)
{
    td_u32       pic_width;
    td_u32       pic_height;
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    if (query_info->scale_cap == TD_TRUE) {
        pic_width = fmw_chn->pic_attr.width;
        pic_height = fmw_chn->pic_attr.height;
    } else {
        pic_width = query_info->src_pic_info->video_frame.video_frame.width;
        pic_height = query_info->src_pic_info->video_frame.video_frame.height;
    }

    video_frame->width          = pic_width;
    video_frame->height         = pic_height;
    video_frame->pixel_format   = fmw_chn->pic_attr.pixel_format;
    video_frame->compress_mode  = fmw_chn->pic_attr.compress_mode;
    video_frame->dynamic_range  = fmw_chn->pic_attr.dynamic_range;
    video_frame->video_format   = fmw_chn->pic_attr.video_format;
    video_frame->field          = OT_VIDEO_FIELD_FRAME;

    pciv_firmware_set_frame_addr(vb_handle, vb_config, video_frame);
}

td_s32 pciv_fmw_vpss_query_mod_check(ot_pciv_chn pciv_chn, const vpss_query_info *query_info)
{
    td_u32 cur_time_ref;
    ot_mod_id mod_id;
    const pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    if (query_info->src_pic_info == TD_NULL) {
        pciv_fmw_info_trace("chn%d src_pic_info is null!\n", pciv_chn);
        return TD_FAILURE;
    }
    mod_id = query_info->src_pic_info->mod_id;
    cur_time_ref = query_info->src_pic_info->video_frame.video_frame.time_ref;
    if (((mod_id == OT_ID_VI) || (mod_id == OT_ID_VO)) && (fmw_chn->time_ref == cur_time_ref)) {
        /* duplicate frame not received again */
        pciv_fmw_err_trace("chn%d duplicated frame!\n", pciv_chn);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}
#endif

td_s32 pciv_fmw_vpss_send_state_check(ot_pciv_chn pciv_chn, const vpss_send_info *send_info)
{
    if (pciv_fmw_state_check(pciv_chn) != TD_SUCCESS) {
        return TD_FAILURE;
    }

    if (send_info->suc == TD_FALSE) {
        pciv_fmw_err_trace("vpss send suc is false.\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_void pciv_fmw_vpss_send_get_bind_obj(const vpss_send_info *send_info, pciv_bind_obj *bind_obj)
{
    ot_mod_id mod_id;

    mod_id = send_info->dest_pic_info[0]->mod_id;
    if (mod_id == OT_ID_VDEC) {
        bind_obj->type = PCIV_BIND_VDEC;
    } else if (mod_id == OT_ID_VO) {
        bind_obj->type = PCIV_BIND_VO;
    } else {
        bind_obj->type = PCIV_BIND_VI;
    }
    bind_obj->is_block = send_info->dest_pic_info[0]->block_mode;
    bind_obj->is_vpss_send = TD_TRUE;
}

td_s32 pciv_fmw_init_vpss(td_void)
{
    td_s32              ret;
    vpss_register_info vpss_info;

    if ((ckfn_vpss_entry()) && (ckfn_vpss_register())) {
        vpss_info.mod_id = OT_ID_PCIV;
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
        vpss_info.vpss_query       = pciv_fmw_vpss_query;
#else
        vpss_info.vpss_query       = TD_NULL;
#endif
        vpss_info.vpss_send        = pciv_fmw_vpss_send;
        vpss_info.reset_call_back  = pciv_fmw_reset_call_back;
        ret = call_vpss_register(&vpss_info);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("register vpss failed\n");
            return ret;
        }
    } else {
        pciv_fmw_err_trace("vpss module does not exist.\n");
    }
    return TD_SUCCESS;
}

td_void pciv_fmw_exit_vpss(td_void)
{
    td_s32 ret;

    if ((ckfn_vpss_entry()) && (ckfn_vpss_un_register())) {
        ret = call_vpss_un_register(OT_ID_PCIV);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("unregister vpss failed\n");
        }
    } else {
        pciv_fmw_err_trace("vpss module does not exist.\n");
    }
}

static td_void pciv_fmw_init_rgn_overlay(rgn_capacity_overlay *overlay)
{
    /* the pixel format of region */
    overlay->pixel_format.spt_reset        = TD_TRUE;
    overlay->pixel_format.pixel_format_num = FMW_PIXEL_FMT_NUM;
    overlay->pixel_format.pixel_format[0]  = OT_PIXEL_FORMAT_ARGB_1555;
    overlay->pixel_format.pixel_format[1]  = OT_PIXEL_FORMAT_ARGB_4444;
    overlay->pixel_format.pixel_format[2]  = OT_PIXEL_FORMAT_ARGB_8888;  /* 2:save the ability of pixel format */
    overlay->pixel_format.pixel_format[3]  = OT_PIXEL_FORMAT_ARGB_CLUT2; /* 3:save the ability of pixel format */
    overlay->pixel_format.pixel_format[4]  = OT_PIXEL_FORMAT_ARGB_CLUT4; /* 4:save the ability of pixel format */

    /* the foreground alpha of region */
    overlay->fg_alpha.spt_alpha = TD_TRUE;
    overlay->fg_alpha.spt_reset = TD_TRUE;
    overlay->fg_alpha.alpha_max = OT_RGN_OVERLAYEX_MAX_ALPHA;
    overlay->fg_alpha.alpha_min = OT_RGN_OVERLAYEX_MIN_ALPHA;

    /* the back alpha of region */
    overlay->bg_alpha.spt_alpha = TD_TRUE;
    overlay->bg_alpha.spt_reset = TD_TRUE;
    overlay->bg_alpha.alpha_max = OT_RGN_OVERLAYEX_MAX_ALPHA;
    overlay->bg_alpha.alpha_min = OT_RGN_OVERLAYEX_MIN_ALPHA;

    /* the panoramic alpha of region */
    overlay->global_alpha.spt_alpha = TD_FALSE;

    /* support backcolor or not */
    overlay->bg_color.spt_reset = TD_TRUE;

    /* QP */
    overlay->qp.spt_qp  = TD_FALSE;

    /* the weight and height of region */
    overlay->size.spt_reset        = TD_FALSE;
    overlay->size.width_align      = OT_RGN_ALIGN;
    overlay->size.height_align     = OT_RGN_ALIGN;
    overlay->size.size_min.width   = OT_RGN_MIN_WIDTH;
    overlay->size.size_min.height  = OT_RGN_MIN_HEIGHT;
    overlay->size.size_max.width   = OT_RGN_OVERLAYEX_MAX_WIDTH;
    overlay->size.size_max.height  = OT_RGN_OVERLAYEX_MAX_HEIGHT;
    overlay->size.max_area         = OT_RGN_OVERLAYEX_MAX_WIDTH * OT_RGN_OVERLAYEX_MAX_HEIGHT;

    /* memory STRIDE */
    overlay->stride = FMW_MEMORY_STRIDE;
}

static td_void pciv_fmw_init_rgn_capacity(rgn_capacity *capacity)
{
    pciv_fmw_init_rgn_overlay(&capacity->type.overlay);

    /* the layer attr of region */
    capacity->layer.spt_layer = TD_TRUE;
    capacity->layer.spt_reset = TD_TRUE;
    capacity->layer.layer_min = FMW_RGN_LAYER_MIN;
    capacity->layer.layer_max = FMW_RGN_LAYER_MAX;

    /* the start position of region */
    capacity->point.spt_reset       = TD_TRUE;
    capacity->point.start_x_align   = FMW_POINT_START_X_ALIGN;
    capacity->point.start_y_align   = FMW_POINT_START_Y_ALIGN;
    capacity->point.point_min.x     = OT_RGN_OVERLAYEX_MIN_X;
    capacity->point.point_min.y     = OT_RGN_OVERLAYEX_MIN_Y;
    capacity->point.point_max.x     = OT_RGN_OVERLAYEX_MAX_X;
    capacity->point.point_max.y     = OT_RGN_OVERLAYEX_MAX_Y;

    /* support overlap or not */
    capacity->spt_overlap    = TD_TRUE;
    capacity->rgn_num_in_chn = OT_RGN_PCIV_MAX_OVERLAYEX_NUM;

    /* rank */
    capacity->chn_sort.is_sort      = TD_TRUE;
    capacity->chn_sort.key          = OT_RGN_SORT_BY_LAYER;
    capacity->chn_sort.small_to_big = TD_TRUE;

    /* the level of region */
    capacity->pfn_check_attr         = TD_NULL;
    capacity->pfn_check_chn_capacity = TD_NULL;
}

td_s32 pciv_fmw_init_rgn(td_void)
{
    td_s32              ret;
    rgn_capacity        capacity;
    rgn_register_info   info;

    if ((ckfn_rgn()) && (ckfn_rgn_register_mod())) {
        info.mod_id  = OT_ID_PCIV;
        info.dev_min = 0;
        info.chn_min = 0;
        info.dev_max = 1;
        info.chn_max = OT_PCIV_MAX_CHN_NUM - 1;
        info.detach_chn = TD_NULL;

        (td_void)memset_s(&capacity, sizeof(rgn_capacity), 0, sizeof(rgn_capacity));

        pciv_fmw_init_rgn_capacity(&capacity);

        /* register OVERLAY */
        ret = call_rgn_register_mod(OT_RGN_OVERLAYEX, &capacity, &info);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("register rgn failed\n");
            return ret;
        }
    } else {
        pciv_fmw_err_trace("rgn module does not exist.\n");
    }
    return TD_SUCCESS;
}

td_void pciv_fmw_exit_rgn(td_void)
{
    td_s32 ret;

    if ((ckfn_rgn()) && (ckfn_rgn_unregister_mod())) {
        ret = call_rgn_unregister_mod(OT_RGN_OVERLAYEX, OT_ID_PCIV);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("unregister rgn overlayex failed\n");
        }
    } else {
        pciv_fmw_err_trace("rgn module does not exist.\n");
    }
}
