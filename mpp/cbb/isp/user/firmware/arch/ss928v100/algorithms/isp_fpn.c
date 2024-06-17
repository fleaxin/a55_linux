/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_common_vi.h"

#include "isp_alg.h"
#include "isp_config.h"
#include "isp_ext_config.h"
#include "isp_proc.h"
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
#include "isp_hnr_ext_config.h"
#endif

/* MACRO DEFINITION */
#define FPN_OVERFLOWTHR                     0x7C0
#define ISP_FPN_MAX_O                       0xFFF
#define FPN_OVERFLOWTHR_OFF                 0x3FFF
#define ISP_FPN_MODE_CORRECTION             0x0
#define ISP_FPN_MODE_CALIBRATE              0x1

#define isp_fpn_clip(min, max, x)             (((x) <= (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

td_s32 isp_set_calibrate_attr(ot_vi_pipe vi_pipe, ot_isp_fpn_calibrate_attr *calibrate)
{
    td_u8 i;
    vi_fpn_attr fpn_attr;
    td_s32 ret;

    fpn_attr.fpn_work_mode = FPN_MODE_CALIBRATE;
    (td_void)memcpy_s(&fpn_attr.calibrate_attr, sizeof(ot_isp_fpn_calibrate_attr),
                      calibrate, sizeof(ot_isp_fpn_calibrate_attr));

    ot_ext_system_fpn_cablibrate_enable_write(vi_pipe, 1);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_FPN_WORK_MODE_SET, &fpn_attr.fpn_work_mode);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] set isp fpn work mode failed!\n", vi_pipe);
        return ret;
    }

    ret = mpi_vi_set_fpn_attr(vi_pipe, &fpn_attr);
    if (ret != TD_SUCCESS) {
        ot_ext_system_fpn_cablibrate_enable_write(vi_pipe, 0);
        return ret;
    }

    (td_void)memcpy_s(calibrate, sizeof(ot_isp_fpn_calibrate_attr),
                      &fpn_attr.calibrate_attr, sizeof(ot_isp_fpn_calibrate_attr));
    calibrate->fpn_cali_frame.iso = ot_ext_system_fpn_sensor_iso_read(vi_pipe);

    if (calibrate->fpn_cali_frame.fpn_frame.video_frame.pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_16BPP) {
        /* for 16BPP, origin offset output is 12bits, other BPP is 14bits */
        for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
            calibrate->fpn_cali_frame.offset[i] = calibrate->fpn_cali_frame.offset[i] << 2; /* left shift 2 to 14bits */
        }
    }

    ot_ext_system_fpn_cablibrate_enable_write(vi_pipe, 0);

    return TD_SUCCESS;
}

static td_u32 isp_fpn_get_strength(td_u32 iso, td_u32 calibrate_iso)
{
    const td_u32 strength = 0x100 * iso / div_0_to_1(calibrate_iso);

    return strength;
}

td_s32 isp_set_correction_attr(ot_vi_pipe vi_pipe, const ot_isp_fpn_attr *correction)
{
    td_u8 i;
    td_s32 ret;
    vi_fpn_attr fpn_attr;

    fpn_attr.fpn_work_mode = FPN_MODE_CORRECTION;
    (td_void)memcpy_s(&fpn_attr.correction_attr, sizeof(ot_isp_fpn_attr), correction, sizeof(ot_isp_fpn_attr));

    if (correction->fpn_frm_info.fpn_frame.video_frame.pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_16BPP) {
        /* for 16BPP, origin offset input is 12bits, other BPP is 14bits */
        for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
            fpn_attr.correction_attr.fpn_frm_info.offset[i] = correction->fpn_frm_info.offset[i] >> 2; /* rshift 2 */
        }
        ot_ext_system_fpn_offset_shift_en_write(vi_pipe, TD_TRUE);
    } else {
        ot_ext_system_fpn_offset_shift_en_write(vi_pipe, TD_FALSE);
    }

    ret = mpi_vi_set_fpn_attr(vi_pipe, &fpn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_FPN_WORK_MODE_SET, &fpn_attr.fpn_work_mode);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] set isp fpn work mode failed!\n", vi_pipe);
        return ret;
    }

    ot_ext_system_manual_fpn_opmode_write(vi_pipe, correction->op_type);
    ot_ext_system_fpn_cor_enable_write(vi_pipe, correction->enable);
    ot_ext_system_manual_fpn_iso_write(vi_pipe, correction->fpn_frm_info.iso);
    ot_ext_system_manual_fpn_gain_write(vi_pipe, correction->manual_attr.strength);
    ot_ext_system_manual_fpn_offset0_write(vi_pipe, correction->fpn_frm_info.offset[0]);
    ot_ext_system_manual_fpn_offset1_write(vi_pipe, correction->fpn_frm_info.offset[1]);
    ot_ext_system_manual_fpn_offset2_write(vi_pipe, correction->fpn_frm_info.offset[2]); /* 2: STRIPING_MAX_NUM -1 */
    return ret;
}

td_s32 isp_get_correction_attr(ot_vi_pipe vi_pipe, ot_isp_fpn_attr *correction)
{
    td_u8 i;
    td_s32 ret;
    vi_fpn_attr temp_vi_fpn_attr;

    ret = mpi_vi_get_fpn_attr(vi_pipe, &temp_vi_fpn_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    (td_void)memcpy_s(correction, sizeof(ot_isp_fpn_attr), &temp_vi_fpn_attr.correction_attr, sizeof(ot_isp_fpn_attr));

    if (correction->fpn_frm_info.fpn_frame.video_frame.pixel_format == OT_PIXEL_FORMAT_RGB_BAYER_16BPP) {
        /* for 16BPP, origin offset output is 12bits, other BPP is 14bits */
        for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
            correction->fpn_frm_info.offset[i] = correction->fpn_frm_info.offset[i] << 2; /* lshift 2 */
        }
    }

    return TD_SUCCESS;
}

static td_void fpn_ext_regs_default(ot_vi_pipe vi_pipe)
{
    ot_ext_system_fpn_sensor_iso_write(vi_pipe, OT_EXT_SYSTEM_FPN_SENSOR_ISO_DEFAULT);
    ot_ext_system_manual_fpn_iso_write(vi_pipe, OT_EXT_SYSTEM_FPN_MANU_ISO_DEFAULT);

    ot_ext_system_manual_fpn_corr_cfg_write(vi_pipe, OT_EXT_SYSTEM_FPN_MANU_CORRCFG_DEFAULT);
    ot_ext_system_manual_fpn_gain_write(vi_pipe, OT_EXT_SYSTEM_FPN_STRENGTH_DEFAULT);
    ot_ext_system_manual_fpn_opmode_write(vi_pipe, OT_EXT_SYSTEM_FPN_OPMODE_DEFAULT);
    ot_ext_system_manual_fpn_update_write(vi_pipe, OT_EXT_SYSTEM_FPN_MANU_UPDATE_DEFAULT);

    ot_ext_system_manual_fpn_type_write(vi_pipe, 0);
    ot_ext_system_manual_fpn_offset0_write(vi_pipe, 0);
    ot_ext_system_manual_fpn_offset1_write(vi_pipe, 0);
    ot_ext_system_manual_fpn_offset2_write(vi_pipe, 0);
    ot_ext_system_fpn_cor_enable_write(vi_pipe, 0);
    ot_ext_system_manual_fpn_pixel_format_write(vi_pipe, 0);
    ot_ext_system_fpn_cablibrate_enable_write(vi_pipe, 0);
    ot_ext_system_fpn_pre_hnr_status_write(vi_pipe, 0);
    ot_ext_system_fpn_offset_shift_en_write(vi_pipe, 0);

    return;
}

static td_void fpn_regs_default(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    reg_cfg->cfg_key.bit1_fpn_cfg = 1;
    ot_unused(vi_pipe);

    return;
}

static td_void fpn_ext_regs_initialize(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

static td_void fpn_regs_initialize(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

static td_s32 fpn_read_extregs(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return 0;
}

static td_void fpn_update_regs(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    ot_ext_system_fpn_sensor_iso_write(vi_pipe, isp_ctx->linkage.sensor_iso);

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        if (ot_ext_system_fpn_cablibrate_enable_read(vi_pipe)) {
            reg_cfg->alg_reg_cfg[i].fpn_reg_cfg.dyna_reg_cfg.isp_fpn_calib_corr = ISP_FPN_MODE_CALIBRATE;
        } else {
            reg_cfg->alg_reg_cfg[i].fpn_reg_cfg.dyna_reg_cfg.isp_fpn_calib_corr = ISP_FPN_MODE_CORRECTION;
        }

        reg_cfg->alg_reg_cfg[i].fpn_reg_cfg.dyna_reg_cfg.isp_fpn_overflow_thr = FPN_OVERFLOWTHR_OFF;
    }

    return;
}

static td_void isp_set_strength(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u32  iso, gain, i, j;
    td_u32  calibrate_iso;
    td_u8   fpn_op_mode, fpn_en;

    fpn_en = isp_ext_system_fpn_cor_enable_read(vi_pipe);
    for (j = 0; j < reg_cfg->cfg_num; j++) {
        reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.fpn_cor_en = fpn_en;
    }
    if (fpn_en != TD_TRUE) {
        return;
    }

    fpn_op_mode = ot_ext_system_manual_fpn_opmode_read(vi_pipe);
    if (fpn_op_mode == OT_OP_MODE_MANUAL) {
        for (j = 0; j < reg_cfg->cfg_num; j++) {
            for (i = 0; i < FPN_CHN_NUM; i++) {
                reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.dyna_reg_cfg.isp_fpn_strength[i] =
                    isp_ext_system_manual_fpn_gain_read(vi_pipe);
            }
        }
        reg_cfg->cfg_key.bit1_fpn_cfg = 1;
        return;
    }

    iso = ot_ext_system_fpn_sensor_iso_read(vi_pipe);
    calibrate_iso = ot_ext_system_manual_fpn_iso_read(vi_pipe);
    gain = isp_fpn_get_strength(iso, calibrate_iso);
    gain = isp_fpn_clip(0, 0x3FF, gain);

    for (j = 0; j < reg_cfg->cfg_num; j++) {
        for (i = 0; i < FPN_CHN_NUM; i++) {
            reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.dyna_reg_cfg.isp_fpn_strength[i] = gain;
        }
    }

    reg_cfg->cfg_key.bit1_fpn_cfg = 1;
}

static td_void isp_fpn_sync_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg)
{
    td_u8 i, j;
    td_bool hnr_en, shift_en;
    td_u16  fpn_offset[OT_ISP_STRIPING_MAX_NUM];

    shift_en = ot_ext_system_fpn_offset_shift_en_read(vi_pipe);
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
    hnr_en = ot_ext_system_hnr_en_read(vi_pipe);
#else
    hnr_en = TD_FALSE;
#endif
    fpn_offset[0] = (td_u16)isp_ext_system_manual_fpn_offset0_read(vi_pipe);
    fpn_offset[1] = (td_u16)isp_ext_system_manual_fpn_offset1_read(vi_pipe);
    fpn_offset[2] = (td_u16)isp_ext_system_manual_fpn_offset2_read(vi_pipe); /* 2: OT_ISP_STRIPING_MAX_NUM -1 */

    for (j = 0; j < reg_cfg->cfg_num; j++) {
        reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.hnr_en     = hnr_en;
        reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.fpn_cor_en = isp_ext_system_fpn_cor_enable_read(vi_pipe);
        for (i = 0; i < FPN_CHN_NUM; i++) {
            reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.black_frame_offset[i] = fpn_offset[j];
            if (shift_en == TD_TRUE) {
                reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.shift_en = TD_TRUE;
                reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.add_offset[i] = fpn_offset[j] >> 2; /* right shift 2 */
            } else {
                reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.shift_en = TD_FALSE;
                reg_cfg->alg_reg_cfg[j].fpn_reg_cfg.sync_cfg.add_offset[i] = fpn_offset[j];
            }
        }
    }
}

static td_s32 isp_fpn_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    fpn_regs_default(vi_pipe, (isp_reg_cfg *)reg_cfg);
    fpn_ext_regs_default(vi_pipe);
    fpn_read_extregs(vi_pipe);
    fpn_regs_initialize(vi_pipe);
    fpn_ext_regs_initialize(vi_pipe);

    return TD_SUCCESS;
}

static td_s32 isp_fpn_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg_info, td_s32 rsv)
{
    isp_reg_cfg *reg_cfg = (isp_reg_cfg *)reg_cfg_info;

    ot_unused(stat_info);
    ot_unused(rsv);
    fpn_update_regs(vi_pipe, reg_cfg);
    isp_set_strength(vi_pipe, reg_cfg);
    isp_fpn_sync_update(vi_pipe, reg_cfg);

    return TD_SUCCESS;
}

static td_s32 fpn_proc_write(ot_vi_pipe vi_pipe, ot_isp_ctrl_proc_write *proc)
{
    ot_isp_ctrl_proc_write proc_tmp;
    td_u32 offset[OT_ISP_STRIPING_MAX_NUM];
    td_u32 strength;

    if ((proc->proc_buff == TD_NULL) || (proc->buff_len == 0)) {
        return TD_FAILURE;
    }

    isp_pre_be_reg_type *be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, 0);
    if (be_reg == TD_NULL) {
        return TD_FAILURE;
    }

    proc_tmp.proc_buff = proc->proc_buff;
    proc_tmp.buff_len  = proc->buff_len;

    offset[0] = isp_ext_system_manual_fpn_offset0_read(vi_pipe);
    offset[1] = isp_ext_system_manual_fpn_offset1_read(vi_pipe);
    offset[2] = isp_ext_system_manual_fpn_offset2_read(vi_pipe); /* 2: OT_ISP_STRIPING_MAX_NUM -1 */
    strength = (isp_fpn_strengthoffset_read(be_reg) >> 0x10) & 0xffff;

    isp_proc_printf(&proc_tmp, proc->write_len, "-----fpn correct info--------------------------------------------\n");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%8s" "%8s" "%10s" "%8s" "%8s" "%8s\n",
                    "en", "op_type", "strength", "offset0", "offset1", "offset2");

    if (isp_ext_system_fpn_cor_enable_read(vi_pipe)) {
        isp_proc_printf(&proc_tmp, proc->write_len,
                        "%8d" "%8d"  "%10d"  "%8d" "%8d" "%8d\n",
                        isp_ext_system_fpn_cor_enable_read(vi_pipe),
                        ot_ext_system_manual_fpn_opmode_read(vi_pipe),
                        strength, offset[0], offset[1], offset[2]); /* 2: OT_ISP_STRIPING_MAX_NUM -1 */
    } else {
        isp_proc_printf(&proc_tmp, proc->write_len,
                        "%8d" "%8s" "%10s" "%8s" "%8s" "%8s\n",
                        0, "--", "--", "--", "--", "--");
    }

    proc->write_len += 1;

    return TD_SUCCESS;
}

static td_s32 isp_fpn_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    switch (cmd) {
        case OT_ISP_PROC_WRITE:
            fpn_proc_write(vi_pipe, (ot_isp_ctrl_proc_write *)value);
            break;

        default:
            break;
    }
    return TD_SUCCESS;
}

static td_s32 isp_fpn_exit(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return TD_SUCCESS;
}

td_s32 isp_alg_register_fpn(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_node *algs = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_fpn);
    algs = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_return(algs);

    algs->alg_type = OT_ISP_ALG_FPN;
    algs->alg_func.pfn_alg_init = isp_fpn_init;
    algs->alg_func.pfn_alg_run  = isp_fpn_run;
    algs->alg_func.pfn_alg_ctrl = isp_fpn_ctrl;
    algs->alg_func.pfn_alg_exit = isp_fpn_exit;
    algs->used = TD_TRUE;

    return TD_SUCCESS;
}
