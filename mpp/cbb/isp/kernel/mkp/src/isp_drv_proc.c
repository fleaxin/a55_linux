/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_spi.h"
#include "ot_osal.h"
#include "ot_common_isp.h"
#include "isp_drv_define.h"
#include "isp.h"

static td_void isp_proc_module_param_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    ot_isp_ctrl_param ctrl_param = { 0 };
    ot_isp_mod_param mod_param = { 0 };
    ot_unused(drv_ctx);

    isp_get_mod_param(&mod_param);
    isp_get_ctrl_param(vi_pipe, &ctrl_param);
    call_sys_print_proc_title(s, "module/control param");
    osal_seq_printf(s, " %12s" " %12s" " %12s" " %12s" " %12s" " %12s" "\n", "proc_param",
                    "stat_intvl", "update_pos", "int_bothalf", "int_timeout", "pwm_number");
    osal_seq_printf(s, " %12u" " %12u" " %12u" " %12u" " %12u" " %12u" "\n",
                    ctrl_param.proc_param, ctrl_param.stat_interval, ctrl_param.update_pos,
                    mod_param.interrupt_bottom_half, ctrl_param.interrupt_time_out, ctrl_param.pwm_num);
    osal_seq_printf(s, "\n");

    osal_seq_printf(s, " %14s" " %12s" " %15s" " %16s" " %16s" " %14s" " %10s""\n",
                    "port_int_delay", "quick_start", "ldci_tprflten", "long_frm_int_en",
                    "be_buf_num", "ob_update_pos", "alg_run_sel");
    osal_seq_printf(s, " %14u" " %12u" " %15u" " %16u" " %16u" " %14s" " %10s""\n",
                    ctrl_param.port_interrupt_delay, mod_param.quick_start, ctrl_param.ldci_tpr_flt_en,
                    mod_param.long_frame_interrupt_en, ctrl_param.be_buf_num,
                    (ctrl_param.ob_stats_update_pos == OT_ISP_UPDATE_OB_STATS_FE_FRAME_END) ? "frame end"  :
                    (ctrl_param.ob_stats_update_pos == OT_ISP_UPDATE_OB_STATS_FE_FRAME_START) ? "frame start" : "butt",
                    (ctrl_param.alg_run_select == OT_ISP_ALG_RUN_NORM) ? "normal" :
                    (ctrl_param.alg_run_select == OT_ISP_ALG_RUN_FE_ONLY) ? "fe only" : "butt");
}

static td_void isp_proc_isp_mode_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    ot_unused(vi_pipe);
    call_sys_print_proc_title(s, "isp mode");
    osal_seq_printf(s, " %15s" " %15s" " %15s" "\n",  "stitch_mode", "running_mode", "block_num");
    osal_seq_printf(s, " %15s" " %15s" " %15d" "\n",
                    drv_ctx->stitch_attr.stitch_enable ? "stitch" : "normal",
                    (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_OFFLINE) ? "offline"  :
                    (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE) ? "preon_postoff"  :
                    (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_ONLINE) ? "online"   :
                    (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_SIDEBYSIDE) ? "sbs"      :
                    (drv_ctx->work_mode.running_mode == ISP_MODE_RUNNING_STRIPING) ? "striping" : "butt",
                    drv_ctx->work_mode.block_num);
}

static td_void isp_proc_sensor_info_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    ot_isp_sns_type sns_type;
    td_s32 sns_dev, ret;
    td_char sns_type_name[MAX_MMZ_NAME_LEN] = { 0 };
    if (drv_ctx->sync_cfg.node[0] == TD_NULL) {
        return;
    }
    ot_unused(vi_pipe);

    sns_type = drv_ctx->sync_cfg.node[0]->sns_regs_info.sns_type;
    if (sns_type == OT_ISP_SNS_I2C_TYPE) {
        sns_dev = drv_ctx->sync_cfg.node[0]->sns_regs_info.com_bus.i2c_dev;
        ret =  snprintf_s(sns_type_name, sizeof(sns_type_name), sizeof(sns_type_name) - 1, "i2c");
        if (ret < 0) {
            return;
        }
    } else if (sns_type == OT_ISP_SNS_SSP_TYPE) {
        sns_dev = drv_ctx->sync_cfg.node[0]->sns_regs_info.com_bus.ssp_dev.bit4_ssp_dev;
        ret =  snprintf_s(sns_type_name, sizeof(sns_type_name), sizeof(sns_type_name) - 1, "ssp");
        if (ret < 0) {
            return;
        }
    } else {
        sns_dev = -1;
        ret =  snprintf_s(sns_type_name, sizeof(sns_type_name), sizeof(sns_type_name) - 1, "butt");
        if (ret < 0) {
            return;
        }
    }

    call_sys_print_proc_title(s, "sensor info");

    osal_seq_printf(s, "%14s" "%10s\n", "sensor_type", "dev");
    osal_seq_printf(s, "%14s" "%10d\n", sns_type_name, sns_dev);
}

static td_void isp_proc_int_status_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    call_sys_print_proc_title(s, "drv info");

    osal_seq_printf(s, "%11s" "%11s" "%11s" "%11s" "%11s" "%11s" "%9s" "%15s" "%16s\n",
                    "vi_pipe", "int_cnt", "int_t", "max_int_t", "int_gap_t", "max_gap_t", "int_rat",
                    "isp_reset_cnt", "isp_besta_lost");

    osal_seq_printf(s, "%11d" "%11d" "%11d" "%11d" "%11d" "%11d" "%9d" "%15d" "%16d\n\n", vi_pipe,
                    drv_ctx->drv_dbg_info.isp_int_cnt, drv_ctx->drv_dbg_info.isp_int_time,
                    drv_ctx->drv_dbg_info.isp_int_time_max, drv_ctx->drv_dbg_info.isp_int_gap_time,
                    drv_ctx->drv_dbg_info.isp_int_gap_time_max, drv_ctx->drv_dbg_info.isp_rate,
                    drv_ctx->drv_dbg_info.isp_reset_cnt, drv_ctx->drv_dbg_info.isp_be_sta_lost);

    osal_seq_printf(s, "%13s" "%13s" "%13s" "%13s" "%13s" "%13s" "%13s" "%13s" "%13s\n", "int_type", "pt_int_cnt",
                    "pt_int_t", "pt_max_int_t", "pt_int_gap_t", "pt_max_gap_t",
                    "pt_int_rat", "sensor_cfg_t", "sensor_max_t");

    osal_seq_printf(s, "%13s" "%13d" "%13d" "%13d" "%13d" "%13d" "%13d" "%13d" "%13d\n",
                    (drv_ctx->frame_int_attr.interrupt_type == OT_FRAME_INTERRUPT_START) ? "start" : "other",
                    drv_ctx->drv_dbg_info.pt_int_cnt, drv_ctx->drv_dbg_info.pt_int_time,
                    drv_ctx->drv_dbg_info.pt_int_time_max, drv_ctx->drv_dbg_info.pt_int_gap_time,
                    drv_ctx->drv_dbg_info.pt_int_gap_time_max, drv_ctx->drv_dbg_info.pt_rate,
                    drv_ctx->drv_dbg_info.sensor_cfg_time, drv_ctx->drv_dbg_info.sensor_cfg_time_max);
}

static td_void isp_proc_be_cfg_phy_addr_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    td_u8  i, j, max_loop, remainder, loop, index;
    td_u64 be_buf_size;
    ot_isp_ctrl_param ctrl_param = { 0 };

    if (is_online_mode(drv_ctx->work_mode.running_mode) ||
        (drv_ctx->be_buf_info.init == TD_FALSE)) {
        return;
    }
    isp_get_ctrl_param(vi_pipe, &ctrl_param);

    osal_seq_printf(s, "\n");
    call_sys_print_proc_title(s, "be_cfg phy addr");

    remainder = ctrl_param.be_buf_num % 4; /* const 4 */
    max_loop  = ctrl_param.be_buf_num / 4; /* const 4 */
    if (remainder != 0) {
        max_loop += 1;
    }

    loop = 4; /* const 4 */
    be_buf_size = sizeof(isp_be_wo_reg_cfg);

    for (j = 0; j < max_loop; j++) {
        if ((j + 1) * 4 > ctrl_param.be_buf_num) { /* const 4 */
            loop = remainder;
        }

        for (i = 0; i < loop; i++) {
            index = j * 4 + i; /* const 4 */
            if (index < 10) { /* const 10 */
                osal_seq_printf(s, "%18s""%d""%s", "be_cfg[", index, "]");
            } else {
                osal_seq_printf(s, "%17s""%d""%s", "be_cfg[", index, "]");
            }
        }

        osal_seq_printf(s, "\n");

        for (i = 0; i < loop; i++) {
            index = j * 4 + i; /* const 4 */
            osal_seq_printf(s, "%#20llx", drv_ctx->be_buf_info.be_buf_haddr.phy_addr + index * be_buf_size); /* 4 */
        }

        osal_seq_printf(s, "\n\n");
    }
}

static td_void isp_proc_pub_attr_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    ot_unused(vi_pipe);
    /*  show isp attribute here. width/height/bayer_format, etc..
              Read parameter from memory directly. */
    call_sys_print_proc_title(s, "pubattr info");

    osal_seq_printf(s, "%12s" "%12s" "%12s" "%12s" "%12s" "%12s" "%12s\n",
                    "wnd_x", "wnd_y", "wnd_w", "wnd_h", "sns_w", "sns_h", "bayer");

    osal_seq_printf(s, "%12d" "%12d" "%12d" "%12d" "%12d" "%12d" "%12s\n\n",
                    drv_ctx->proc_pub_info.wnd_rect.x, drv_ctx->proc_pub_info.wnd_rect.y,
                    drv_ctx->proc_pub_info.wnd_rect.width, drv_ctx->proc_pub_info.wnd_rect.height,
                    drv_ctx->proc_pub_info.sns_size.width, drv_ctx->proc_pub_info.sns_size.height,
                    (drv_ctx->proc_pub_info.bayer_format == OT_ISP_BAYER_RGGB) ? "rggb" :
                    (drv_ctx->proc_pub_info.bayer_format == OT_ISP_BAYER_GRBG) ? "grbg" :
                    (drv_ctx->proc_pub_info.bayer_format == OT_ISP_BAYER_GBRG) ? "gbrg" :
                    (drv_ctx->proc_pub_info.bayer_format == OT_ISP_BAYER_BGGR) ? "bggr" : "butt");
}

static td_void isp_proc_wdr_mode_show(isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    static ot_isp_fswdr_mode fs_wdr_mode = 0;
    fs_wdr_mode = drv_ctx->frame_info.fs_wdr_mode;

    osal_seq_printf(s, "%16s\n", "fswdr mode");

    if (is_fs_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        if (fs_wdr_mode == OT_ISP_FSWDR_NORMAL_MODE) {
            osal_seq_printf(s, "%16s\n\n", "normal");
        } else if (fs_wdr_mode == OT_ISP_FSWDR_LONG_FRAME_MODE) {
            osal_seq_printf(s, "%16s\n\n", "long frame");
        } else if (fs_wdr_mode == OT_ISP_FSWDR_AUTO_LONG_FRAME_MODE) {
            osal_seq_printf(s, "%16s\n\n", "auto long frame");
        }
    }
}

static td_void isp_proc_frame_info_show(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, osal_proc_entry_t *s)
{
    td_u8       i = 0;
    td_u8       loop = 0;

    ot_unused(vi_pipe);

    if (drv_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return;
    }

    /*  show isp frame info here. fe_id/fs_wdr_mode/bayer_format, etc..
              Read parameter from memory directly. */
    call_sys_print_proc_title(s, "send raw isp frame info");

    if (drv_ctx->proc_frame_info.print_en != TD_TRUE) {
        return;
    }

    osal_seq_printf(s, "%16s" "%16s" "%16s" "%16s" "%16s" "%26s\n",
                    "fe_id", "exp_time", "isp_dg", "again", "dgain", "vi_send_raw_cnt");

    if (is_linear_mode(drv_ctx->wdr_attr.wdr_mode) || is_built_in_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        loop = 1;
    } else if (is_2to1_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        loop = 2; /* 2 */
    } else if (is_3to1_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        loop = 3; /* 3 */
    } else if (is_4to1_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        loop = 4; /* 4 */
    }

    for (i = 0; i < loop; i++) {
        osal_seq_printf(s, "%16u" "%16u" "%16u" "%16u" "%16u" "%26llu\n\n",
                        drv_ctx->proc_frame_info.fe_id[i], drv_ctx->proc_frame_info.exposure_time[i],
                        drv_ctx->proc_frame_info.isp_dgain[i], drv_ctx->proc_frame_info.again[i],
                        drv_ctx->proc_frame_info.dgain[i], drv_ctx->proc_frame_info.vi_send_raw_cnt);
    }

    isp_proc_wdr_mode_show(drv_ctx, s);
}

static td_s32 isp_drv_proc_printf(ot_vi_pipe vi_pipe, osal_proc_entry_t *s)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    td_u32 proc_buf_len;
    const td_char *psz_str = TD_NULL;
    td_char *psz_buf = TD_NULL;
    ot_isp_ctrl_param ctrl_param = { 0 };

    isp_check_pipe_return(vi_pipe);

    isp_get_ctrl_param(vi_pipe, &ctrl_param);
    if (ctrl_param.proc_param == 0) {
        return TD_SUCCESS;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (osal_down_interruptible(&drv_ctx->proc_sem)) {
        return -ERESTARTSYS;
    }

    if (drv_ctx->porc_mem.virt_addr != TD_NULL) {
        psz_buf = osal_kmalloc((PROC_PRT_SLICE_SIZE + 1), osal_gfp_atomic);
        if (psz_buf == TD_NULL) {
            isp_err_trace("isp_drv_proc_printf malloc slice buf err\n");
            osal_up(&drv_ctx->proc_sem);
            return OT_ERR_ISP_NULL_PTR;
        }

        psz_buf[PROC_PRT_SLICE_SIZE] = '\0';
        psz_str = (td_char *)drv_ctx->porc_mem.virt_addr;
        proc_buf_len = osal_strlen((td_char *)drv_ctx->porc_mem.virt_addr);

        while (proc_buf_len) {
            (td_void)strncpy_s(psz_buf, PROC_PRT_SLICE_SIZE + 1, psz_str, PROC_PRT_SLICE_SIZE);
            osal_seq_printf(s, "%s", psz_buf);
            psz_str += PROC_PRT_SLICE_SIZE;

            if (proc_buf_len < PROC_PRT_SLICE_SIZE) {
                proc_buf_len = 0;
            } else {
                proc_buf_len -= PROC_PRT_SLICE_SIZE;
            }
        }

        osal_kfree((td_void *)psz_buf);
    }

    osal_up(&drv_ctx->proc_sem);

    return TD_SUCCESS;
}

td_s32 isp_proc_show(osal_proc_entry_t *s)
{
    td_s32 ret;
    ot_vi_pipe  vi_pipe = 0;
    isp_drv_ctx *drv_ctx = TD_NULL;

    osal_seq_printf(s, "\n[ISP] Version: ["OT_MPP_VERSION"], Build Time["__DATE__", "__TIME__"]\n\n");

    do {
        drv_ctx = isp_drv_get_ctx(vi_pipe);
        if (!drv_ctx->mem_init) {
            continue;
        }

        call_sys_print_proc_title(s, "isp proc pipe[%d]", vi_pipe);

        isp_proc_module_param_show(vi_pipe, drv_ctx, s);
        isp_proc_isp_mode_show(vi_pipe, drv_ctx, s);
        isp_proc_sensor_info_show(vi_pipe, drv_ctx, s);
        isp_proc_int_status_show(vi_pipe, drv_ctx, s);
        isp_proc_be_cfg_phy_addr_show(vi_pipe, drv_ctx, s);
        isp_proc_pub_attr_show(vi_pipe, drv_ctx, s);
        isp_proc_frame_info_show(vi_pipe, drv_ctx, s);

        ret = isp_drv_proc_printf(vi_pipe, s);
        if (ret != TD_SUCCESS) {
        }

        call_sys_print_proc_title(s, "isp proc end[%d]", vi_pipe);
    } while (++vi_pipe < OT_ISP_MAX_PIPE_NUM);

    return 0;
}
