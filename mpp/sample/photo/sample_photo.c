/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include "ss_mpi_sys.h"
#include "ss_mpi_photo.h"
#include "ss_mpi_snap.h"
#include "ot_common_photo.h"
#include "ot_common_snap.h"
#include "ot_common_vb.h"

#include "ot_type.h"
#include "ss_mpi_dsp.h"
#include "ss_mpi_sys.h"
#include "sample_comm.h"

#define PHOTO_SAMPLE_BUFNAME_LEN        32
#define SAMPLE_SVP_DSP_BIN_NUM_PER      4
#define SAMPLE_SVP_DSP_MEM_TYPE_SYS_DDR 0
#define SAMPLE_SVP_DSP_MEM_TYPE_IRAM    1
#define SAMPLE_SVP_DSP_MEM_TYPE_DRAM_0  2
#define SAMPLE_SVP_DSP_MEM_TYPE_DRAM_1  3

#define VIDEO_PIPE_ID 0
#define SNAP_PIPE_ID  1

#define check_digit(x) ((x) >= '0' && (x) <= '9')

volatile static sig_atomic_t g_photo_sample_exit = 0;
static sample_sns_type g_sns_type = SENSOR0_TYPE;
static sample_vi_cfg g_vi_cfg;

static sample_vo_cfg g_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_intf_type      = OT_VO_INTF_HDMI,
    .intf_sync         = OT_VO_OUT_1080P30,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, 1920, 1080},
    .image_size        = {1920, 1080},
    .vo_part_mode      = OT_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len       = 3, /* 3: def buf len for single */
    .dst_dynamic_range = OT_DYNAMIC_RANGE_SDR8,
    .vo_mode           = VO_MODE_1MUX,
    .compress_mode     = OT_COMPRESS_MODE_NONE,
};

static sample_comm_venc_chn_param g_venc_chn_param = {
    .frame_rate           = 30, /* 30 is a number */
    .stats_time           = 1,  /* 1 is a number */
    .gop                  = 30, /* 30 is a number */
    .venc_size            = {1920, 1080},
    .size                 = PIC_1080P,
    .profile              = 0,
    .is_rcn_ref_share_buf = TD_FALSE,
    .gop_attr             = {
        .gop_mode = OT_VENC_GOP_MODE_NORMAL_P,
        .normal_p = {2},
    },
    .type                 = OT_PT_H265,
    .rc_mode              = SAMPLE_RC_VBR,
};

static ot_snap_attr g_norm_snap_attr = {
    .snap_type = OT_SNAP_TYPE_NORM,
    .load_ccm_en = TD_TRUE,
    .norm_attr = {
        .frame_cnt         = 4, /* snap 4 frames */
        .repeat_send_times = 1,
        .zsl_en            = TD_FALSE,
    },
};

static ot_snap_attr g_pro_snap_attr = {
    .snap_type = OT_SNAP_TYPE_PRO,
    .load_ccm_en = TD_TRUE,
    .pro_attr = {
        .frame_cnt = 3,  /* snap 3 frames */
        .repeat_send_times = 1,
        .pro_param.op_mode = OT_OP_MODE_AUTO,
        .pro_param.auto_param.exp_step[0] = 256 / 4, /* auto param[0]: 256 / 4 */
        .pro_param.auto_param.exp_step[1] = 256,     /* auto param[1]: 256 */
        .pro_param.auto_param.exp_step[2] = 256 * 4, /* auto param[2]: 256 * 4 */
    }
};

static td_char g_alg_buf_name[OT_PHOTO_ALG_TYPE_BUTT - 1][PHOTO_SAMPLE_BUFNAME_LEN] = {
    "hdr_pub_mem", "sfnr_pub_mem", "mfnr_pub_mem"};

static td_void sample_get_char(td_void)
{
    if (g_photo_sample_exit == 1) {
        return;
    }

    sample_pause();
}

static td_s32 sample_photo_load_dsp_core_bin(ot_svp_dsp_id core_id)
{
    td_s32 ret;
    td_char *bin[OT_SVP_DSP_ID_BUTT][SAMPLE_SVP_DSP_BIN_NUM_PER] = {
        { "../svp/dsp/dsp_bin/dsp0/ot_sram.bin", "../svp/dsp/dsp_bin/dsp0/ot_iram0.bin",
            "../svp/dsp/dsp_bin/dsp0/ot_dram0.bin", "../svp/dsp/dsp_bin/dsp0/ot_dram1.bin" },
        { "../svp/dsp/dsp_bin/dsp1/ot_sram.bin", "../svp/dsp/dsp_bin/dsp1/ot_iram0.bin",
            "../svp/dsp/dsp_bin/dsp1/ot_dram0.bin", "../svp/dsp/dsp_bin/dsp1/ot_dram1.bin" } };

    ret = ss_mpi_svp_dsp_power_on(core_id);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_power_on failed with %#x!\n",  ret);
        return ret;
    }

    ret = ss_mpi_svp_dsp_load_bin(bin[core_id][SAMPLE_SVP_DSP_MEM_TYPE_IRAM],
        core_id * SAMPLE_SVP_DSP_BIN_NUM_PER + SAMPLE_SVP_DSP_MEM_TYPE_IRAM);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_load_bin failed with %#x!\n",  ret);
        return ret;
    }

    ret = ss_mpi_svp_dsp_load_bin(bin[core_id][SAMPLE_SVP_DSP_MEM_TYPE_SYS_DDR],
        core_id * SAMPLE_SVP_DSP_BIN_NUM_PER + SAMPLE_SVP_DSP_MEM_TYPE_SYS_DDR);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_load_bin failed with %#x!\n",  ret);
        return ret;
    }

    ret = ss_mpi_svp_dsp_load_bin(bin[core_id][SAMPLE_SVP_DSP_MEM_TYPE_DRAM_0],
        core_id * SAMPLE_SVP_DSP_BIN_NUM_PER + SAMPLE_SVP_DSP_MEM_TYPE_DRAM_0);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_load_bin failed with %#x!\n",  ret);
        return ret;
    }

    ret = ss_mpi_svp_dsp_load_bin(bin[core_id][SAMPLE_SVP_DSP_MEM_TYPE_DRAM_1],
        core_id * SAMPLE_SVP_DSP_BIN_NUM_PER + SAMPLE_SVP_DSP_MEM_TYPE_DRAM_1);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_load_bin failed with %#x!\n",  ret);
        return ret;
    }

    ret = ss_mpi_svp_dsp_enable_core(core_id);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_enable_core failed with %#x!\n",  ret);
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 sample_photo_unload_dsp_core_bin(ot_svp_dsp_id core_id)
{
    td_s32 ret = TD_FAILURE;
    ret = ss_mpi_svp_dsp_disable_core(core_id);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_disable_core failed with %#x!\n",  ret);
        return ret;
    }

    ret = ss_mpi_svp_dsp_power_off(core_id);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_svp_dsp_power_off failed with %#x!\n",  ret);
        return ret;
    }

    return TD_SUCCESS;
}

static td_void sample_photo_get_default_vb_config(ot_size *size, ot_vb_cfg *vb_cfg)
{
    ot_vb_calc_cfg calc_cfg;
    ot_pic_buf_attr buf_attr;

    (td_void)memset_s(vb_cfg, sizeof(ot_vb_cfg), 0, sizeof(ot_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 blks */

    /* default YUV pool: SP420 + compress_seg */
    buf_attr.width         = size->width;
    buf_attr.height        = size->height;
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    buf_attr.compress_mode = OT_COMPRESS_MODE_SEG;
    ot_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    vb_cfg->common_pool[0].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[0].blk_cnt  = 12; /* 12 blks */

    /* default raw pool: raw12bpp + compress_line */
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
    buf_attr.compress_mode = OT_COMPRESS_MODE_LINE;
    ot_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);
    vb_cfg->common_pool[1].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[1].blk_cnt  = 12; /* 12 blks */
}

static td_s32 sample_photo_sys_init(ot_vi_vpss_mode_type mode_type, ot_vi_video_mode video_mode)
{
    td_s32 ret;
    ot_size size;
    ot_vb_cfg vb_cfg;
    td_u32 supplement_config;

    size.width  = 3840; /* 3840 pixel */
    size.height = 2160; /* 2160 line */
    sample_photo_get_default_vb_config(&size, &vb_cfg);

    supplement_config = OT_VB_SUPPLEMENT_BNR_MOT_MASK | OT_VB_SUPPLEMENT_JPEG_MASK;
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, video_mode);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 sample_photo_start_vpss(ot_vpss_grp grp, ot_size *in_size)
{
    ot_vpss_grp_attr grp_attr;
    ot_vpss_chn_attr chn_attr;
    td_bool chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {TD_TRUE, TD_FALSE, TD_FALSE, TD_FALSE};

    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    sample_comm_vpss_get_default_chn_attr(&chn_attr);
    chn_attr.width  = in_size->width;
    chn_attr.height = in_size->height;
    if (grp == 1) {
        grp_attr.nr_attr.nr_type = OT_VPSS_NR_TYPE_SNAP_NORM;
        chn_attr.depth = 3; /* snap route dump depth: 3 */
        chn_attr.compress_mode = OT_COMPRESS_MODE_NONE;
    }

    return sample_common_vpss_start(grp, chn_enable, &grp_attr, &chn_attr, OT_VPSS_MAX_PHYS_CHN_NUM);
}

static td_void sample_photo_stop_vpss(ot_vpss_grp grp)
{
    td_bool chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {TD_TRUE, TD_FALSE, TD_FALSE, TD_FALSE};

    sample_common_vpss_stop(grp, chn_enable, OT_VPSS_MAX_PHYS_CHN_NUM);
}

static td_s32 sample_photo_start_venc(ot_venc_chn venc_chn[], td_u32 chn_num)
{
    td_s32 i;
    td_s32 ret;
    ot_size venc_size;

    sample_comm_vi_get_size_by_sns_type(g_sns_type, &venc_size);
    g_venc_chn_param.venc_size.width  = venc_size.width;
    g_venc_chn_param.venc_size.height = venc_size.height;
    g_venc_chn_param.size = sample_comm_sys_get_pic_enum(&venc_size);

    for (i = 0; i < (td_s32)chn_num; i++) {
        ret = sample_comm_venc_start(venc_chn[i], &g_venc_chn_param);
        if (ret != TD_SUCCESS) {
            goto exit;
        }
    }

    ret = sample_comm_venc_start_get_stream(venc_chn, chn_num);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    return TD_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        sample_comm_venc_stop(venc_chn[i]);
    }
    return TD_FAILURE;
}

static td_void sample_photo_stop_venc(ot_venc_chn venc_chn[], td_u32 chn_num)
{
    td_u32 i;

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static td_s32 sample_photo_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

static td_void sample_photo_stop_vo(td_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

static td_s32 sample_photo_start_venc_and_vo(const ot_vpss_grp vpss_grp[], td_u32 grp_num)
{
    td_u32 i;
    td_s32 ret;
    ot_size venc_size;
    sample_vo_mode vo_mode = VO_MODE_1MUX;
    const ot_vpss_chn vpss_chn = 0;
    const ot_vo_layer vo_layer = 0;
    ot_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    ot_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    ret = sample_photo_start_vo(vo_mode);
    if (ret != TD_SUCCESS) {
        goto start_vo_failed;
    }

    sample_comm_vi_get_size_by_sns_type(g_sns_type, &venc_size);
    ret = sample_photo_start_venc(venc_chn, 1);
    if (ret != TD_SUCCESS) {
        goto start_venc_failed;
    }

    ret = sample_comm_venc_photo_start(venc_chn[1], &venc_size, TD_TRUE);
    if (ret != TD_SUCCESS) {
        goto start_venc_photo_failed;
    }

    for (i = 0; i < grp_num; i++) {
        if (i == 0) {
            sample_comm_vpss_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
            sample_comm_vpss_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
        }
    }

    return TD_SUCCESS;

start_venc_photo_failed:
    sample_photo_stop_venc(venc_chn, 1);
start_venc_failed:
    sample_photo_stop_vo();
start_vo_failed:
    return TD_FAILURE;
}

static td_void sample_photo_stop_venc_and_vo(const ot_vpss_grp vpss_grp[], td_u32 grp_num)
{
    td_u32 i;
    const ot_vpss_chn vpss_chn = 0;
    const ot_vo_layer vo_layer = 0;
    ot_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    ot_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    for (i = 0; i < grp_num; i++) {
        if (i == 0) {
            sample_comm_vpss_un_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
            sample_comm_vpss_un_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
        }
    }

    sample_photo_stop_venc(venc_chn, 1);
    sample_comm_venc_snap_stop(venc_chn[1]);
    sample_photo_stop_vo();
}

static td_s32 sample_photo_start_vpss_venc_vo(td_void)
{
    td_s32 ret;
    const ot_vi_chn vi_chn = 0;
    const ot_vpss_grp vpss_grp[2] = {0, 1};
    const td_u32 grp_num = 2;
    const ot_vpss_chn vpss_chn = 0;
    ot_size in_size;

    sample_comm_vi_bind_vpss(VIDEO_PIPE_ID, vi_chn, vpss_grp[0], vpss_chn);
    sample_comm_vi_bind_vpss(SNAP_PIPE_ID, vi_chn, vpss_grp[1], vpss_chn);
    sample_comm_vi_get_size_by_sns_type(g_sns_type, &in_size);
    ret = sample_photo_start_vpss(vpss_grp[0], &in_size);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = sample_photo_start_vpss(vpss_grp[1], &in_size);
    if (ret != TD_SUCCESS) {
        goto start_vpss_failed;
    }

    ret = sample_photo_start_venc_and_vo(vpss_grp, grp_num);
    if (ret != TD_SUCCESS) {
        goto start_venc_vo_failed;
    }

    return TD_SUCCESS;

start_venc_vo_failed:
    sample_photo_stop_vpss(vpss_grp[1]);
start_vpss_failed:
    sample_photo_stop_vpss(vpss_grp[0]);
    return ret;
}

static td_void sample_photo_stop_vpss_venc_vo(td_void)
{
    const ot_vpss_grp vpss_grp[2] = {0, 1};
    const td_u32 grp_num = 2;
    const ot_vi_chn vi_chn = 0;
    const ot_vpss_chn vpss_chn = 0;

    sample_photo_stop_venc_and_vo(vpss_grp, grp_num);
    sample_photo_stop_vpss(vpss_grp[1]);
    sample_photo_stop_vpss(vpss_grp[0]);
    sample_comm_vi_un_bind_vpss(SNAP_PIPE_ID, vi_chn, vpss_grp[1], vpss_chn);
    sample_comm_vi_un_bind_vpss(VIDEO_PIPE_ID, vi_chn, vpss_grp[0], vpss_chn);
}

static td_s32 sample_photo_start_vi_route(td_void)
{
    td_s32 ret;

    sample_comm_vi_get_default_vi_cfg(g_sns_type, &g_vi_cfg);
    g_vi_cfg.bind_pipe.pipe_num = 2; /* 2: double pipe */
    g_vi_cfg.bind_pipe.pipe_id[0] = 0;
    g_vi_cfg.bind_pipe.pipe_id[1] = 1;
    (td_void)memcpy_s(&g_vi_cfg.pipe_info[1], sizeof(sample_vi_pipe_info),
        &g_vi_cfg.pipe_info[0], sizeof(sample_vi_pipe_info));
    g_vi_cfg.pipe_info[1].pipe_need_start = TD_FALSE;
    g_vi_cfg.pipe_info[1].isp_need_run = TD_FALSE;

    ret = sample_comm_vi_start_vi(&g_vi_cfg);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_void sample_photo_stop_vi_route(void)
{
    sample_comm_vi_stop_vi(&g_vi_cfg);
}

static td_s32 sample_photo_start_video(td_void)
{
    td_s32 ret;
    ot_vi_vpss_mode_type mode_type = OT_VI_OFFLINE_VPSS_OFFLINE;
    ot_vi_video_mode video_mode = OT_VI_VIDEO_MODE_NORM;

    ret = sample_photo_sys_init(mode_type, video_mode);
    if (ret != TD_SUCCESS) {
        goto sys_init_failed;
    }

    ret = sample_photo_start_vi_route();
    if (ret != TD_SUCCESS) {
        goto start_vi_failed;
    }
;
    ret = sample_photo_start_vpss_venc_vo();
    if (ret != TD_SUCCESS) {
        goto start_vpss_venc_vo_failed;
    }

    return TD_SUCCESS;

start_vpss_venc_vo_failed:
    sample_photo_stop_vi_route();
start_vi_failed:
    sample_comm_sys_exit();
sys_init_failed:
    return ret;
}

static td_void sample_photo_stop_video(td_void)
{
    sample_photo_stop_vpss_venc_vo();
    sample_photo_stop_vi_route();
    sample_comm_sys_exit();
}

static td_s32 sample_photo_start_snap(ot_snap_type snap_type)
{
    td_s32 ret;
    ot_snap_attr *snap_attr = TD_NULL;
    ot_vi_pipe_param pipe_param;

    if (snap_type == OT_SNAP_TYPE_NORM) {
        snap_attr = &g_norm_snap_attr;
    } else {
        snap_attr = &g_pro_snap_attr;
    }

    ret = ss_mpi_snap_set_pipe_attr(SNAP_PIPE_ID, snap_attr);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_snap_set_pipe_attr failed, ret: 0x%x\n", ret);
        return TD_FAILURE;
    }

    ret = ss_mpi_vi_get_pipe_param(VIDEO_PIPE_ID, &pipe_param);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_vi_get_pipe_param failed, ret: 0x%x\n", ret);
        return TD_FAILURE;
    }
    pipe_param.discard_pro_pic_en = TD_TRUE;
    ret = ss_mpi_vi_set_pipe_param(VIDEO_PIPE_ID, &pipe_param);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_vi_set_pipe_param failed, ret: 0x%x\n", ret);
        return TD_FAILURE;
    }

    ret = ss_mpi_snap_enable_pipe(SNAP_PIPE_ID);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_snap_enable_pipe failed, ret: 0x%x\n", ret);
        return TD_FAILURE;
    }

    printf("=======press Enter key to trigger=====\n");
    sample_get_char();

    ret = ss_mpi_snap_trigger_pipe(SNAP_PIPE_ID);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_snap_trigger_pipe failed, ret: 0x%x\n", ret);
        goto exit;
    }

    printf("=======snap success=====\n");

    return TD_SUCCESS;

exit:
    ss_mpi_snap_disable_pipe(SNAP_PIPE_ID);
    return ret;
}

static ot_vb_blk samplie_photo_get_dst_frame_info(ot_video_frame_info *dst_frame, const ot_video_frame_info *src_frame)
{
    ot_vb_blk vb_blk;
    td_phys_addr_t dst_phys_addr;
    td_s32 ret;
    td_u32 buf_size;
    ot_pic_buf_attr buf_attr = {
        .width = src_frame->video_frame.width,
        .height = src_frame->video_frame.height,
        .align = 0,
        .bit_width = OT_DATA_BIT_WIDTH_8,
        .pixel_format = SAMPLE_PIXEL_FORMAT,
        .compress_mode = OT_COMPRESS_MODE_NONE,
    };

    buf_size = ot_common_get_pic_buf_size(&buf_attr);
    vb_blk = ss_mpi_vb_get_blk(src_frame->pool_id, buf_size, TD_NULL);
    if (vb_blk == OT_VB_INVALID_HANDLE) {
        sample_print("ss_mpi_vb_get_blk failed!\n");
        return OT_VB_INVALID_HANDLE;
    }

    dst_phys_addr = ss_mpi_vb_handle_to_phys_addr(vb_blk);
    (td_void)memcpy_s(dst_frame, sizeof(ot_video_frame_info), src_frame, sizeof(ot_video_frame_info));
    dst_frame->video_frame.phys_addr[0] = dst_phys_addr;
    dst_frame->video_frame.phys_addr[1] = dst_phys_addr + \
        (dst_frame->video_frame.stride[0] * dst_frame->video_frame.height);

    ret = ss_mpi_vb_get_supplement_addr(vb_blk, &dst_frame->video_frame.supplement);
    if (ret != TD_SUCCESS) {
        ss_mpi_vb_release_blk(vb_blk);
        return OT_VB_INVALID_HANDLE;
    }

    return vb_blk;
}

static td_s32 sample_photo_get_iso(ot_video_frame_info *video_frame)
{
    td_u64 jpeg_dcf_phys_addr = 0;
    ot_jpeg_dcf *jpeg_dcf = TD_NULL;
    td_u32 iso;

    jpeg_dcf_phys_addr = video_frame->video_frame.supplement.jpeg_dcf_phys_addr;

    jpeg_dcf = ss_mpi_sys_mmap(jpeg_dcf_phys_addr, sizeof(ot_jpeg_dcf));

    iso = jpeg_dcf->isp_dcf_info.isp_dcf_update_info.iso_speed_ratings;

    ss_mpi_sys_munmap(jpeg_dcf, sizeof(ot_jpeg_dcf));

    return iso;
}

static td_void sample_photo_copy_isp_info(ot_video_frame_info *dst_frame, const ot_video_frame_info *src_frame)
{
    td_u64 src_info_phys_addr, dst_info_phys_addr;
    td_void *src_isp_info = TD_NULL;
    td_void *dst_isp_info = TD_NULL;

    src_info_phys_addr = src_frame->video_frame.supplement.isp_info_phys_addr;
    dst_info_phys_addr = dst_frame->video_frame.supplement.isp_info_phys_addr;

    src_isp_info = ss_mpi_sys_mmap(src_info_phys_addr, sizeof(ot_isp_frame_info));
    dst_isp_info = ss_mpi_sys_mmap(dst_info_phys_addr, sizeof(ot_isp_frame_info));
    if (src_isp_info == TD_NULL || dst_isp_info == TD_NULL) {
        return;
    }

    (td_void)memcpy_s(dst_isp_info, sizeof(ot_isp_frame_info), src_isp_info, sizeof(ot_isp_frame_info));

    ss_mpi_sys_munmap(src_isp_info, sizeof(ot_isp_frame_info));
    ss_mpi_sys_munmap(dst_isp_info, sizeof(ot_isp_frame_info));
}

/******************************************************************************
* function : show usage
******************************************************************************/
static td_void sample_photo_usage(char *argv)
{
    printf("Usage : %s <index>\n", argv);
    printf("index:\n");
    printf("\t 0)HDR.\n");
    printf("\t 1)MFNR.\n");
    printf("\t 2)SFNR.\n");
}

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
static void sample_photo_handle_sig(td_s32 signo)
{
    if (g_photo_sample_exit == 1) {
        return;
    }

    if (signo == SIGINT || signo == SIGTERM) {
        g_photo_sample_exit = 1;
    }
}

static td_s32 sample_photo_alg_init(ot_photo_alg_type alg_type, td_u64 *phys_addr, td_void *vir_addr)
{
    td_s32 ret;
    ot_size input_size;
    td_u32 pub_mem_size;
    ot_photo_alg_init photo_init;

    sample_comm_vi_get_size_by_sns_type(g_sns_type, &input_size);

    if (alg_type == OT_PHOTO_ALG_TYPE_HDR) {
        pub_mem_size = hdr_get_public_mem_size(input_size.width, input_size.height);
        printf("pub_mem_size: %d\n", pub_mem_size);
    } else if (alg_type == OT_PHOTO_ALG_TYPE_SFNR) {
        pub_mem_size = sfnr_get_public_mem_size(input_size.width, input_size.height);
    } else if (alg_type == OT_PHOTO_ALG_TYPE_MFNR) {
        pub_mem_size = mfnr_get_public_mem_size(input_size.width, input_size.height);
    } else {
        sample_print("NOt support alg_type!\n");
        return TD_FAILURE;
    }

    ret = ss_mpi_sys_mmz_alloc_cached(phys_addr, &vir_addr, g_alg_buf_name[alg_type], NULL, pub_mem_size);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_sys_mmz_alloc_cached failed!\n");
        return TD_FAILURE;
    }

    photo_init.public_mem_phy_addr = *phys_addr;
    photo_init.public_mem_vir_addr = (td_u64)vir_addr;
    photo_init.public_mem_size = pub_mem_size;
    photo_init.print_debug_info = TD_FALSE;

    ret = ss_mpi_photo_alg_init(alg_type, &photo_init);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_photo_alg_init failed!\n");
        ss_mpi_sys_mmz_free(*phys_addr, vir_addr);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void sample_photo_alg_deinit(ot_photo_alg_type alg_type, td_u64 phys_addr, td_void *vir_addr)
{
    ss_mpi_photo_alg_deinit(alg_type);
    ss_mpi_sys_mmz_free(phys_addr, vir_addr);
}

static td_s32 sample_photo_save_jpeg(ot_video_frame_info *frame_info)
{
    td_s32 ret;
    const ot_venc_chn venc_chn = 1;

    ret = ss_mpi_venc_send_frame(venc_chn, frame_info, -1);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_venc_send_frame failed:%#x!\n", ret);
        return TD_FAILURE;
    }

    ret = sample_comm_venc_save_jpeg(venc_chn, 1);
    if (ret != TD_SUCCESS) {
        sample_print("sample_comm_venc_save_jpeg failed!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 sample_photo_hdr_process()
{
    td_s32 ret, i;
    ot_video_frame_info video_frame[OT_PHOTO_HDR_FRAME_NUM];
    ot_video_frame_info dst_video_frame;
    ot_vb_blk vb_blk;
    ot_photo_alg_attr photo_attr;
    td_bool dst_buf_alloc = TD_FALSE;

    for (i = 0; i < OT_PHOTO_HDR_FRAME_NUM; i++) {
        ret = ss_mpi_vpss_get_chn_frame(SNAP_PIPE_ID, 0, &video_frame[i], 1000); /* milli_sec: 1000us */
        if (ret != TD_SUCCESS) {
            sample_print("ss_mpi_vpss_get_chn_frame failed!\n");
            goto exit;
        }

        if (!dst_buf_alloc) {
            vb_blk = samplie_photo_get_dst_frame_info(&dst_video_frame, &video_frame[i]);
            if (OT_VB_INVALID_HANDLE == vb_blk) {
                goto exit;
            }
            dst_buf_alloc = TD_TRUE;
        }

        photo_attr.hdr_attr.src_frm = video_frame[i];
        photo_attr.hdr_attr.des_frm = dst_video_frame;
        photo_attr.hdr_attr.frm_index = i;
        photo_attr.hdr_attr.iso = sample_photo_get_iso(&video_frame[i]);
        photo_attr.hdr_attr.face_num = 0;
        ret = ss_mpi_photo_alg_process(OT_PHOTO_ALG_TYPE_HDR, &photo_attr);
        if (ret != TD_SUCCESS) {
            sample_print("ss_mpi_photo_alg_process failed!\n");
            goto exit;
        }
    }

    sample_photo_copy_isp_info(&dst_video_frame, &video_frame[1]);
    ret = sample_photo_save_jpeg(&dst_video_frame);

exit:
    for (i = i - 1; i >= 0; i--) {
        ss_mpi_vpss_release_chn_frame(SNAP_PIPE_ID, 0, &video_frame[i]);
    }

    if (dst_buf_alloc == TD_TRUE) {
        ss_mpi_vb_release_blk(vb_blk);
    }

    return ret;
}

static td_s32 sample_photo_hdr_route(void)
{
    td_s32 ret;
    td_u64 phys_addr;
    td_void *vir_addr = TD_NULL;
    const ot_photo_alg_type alg_type = OT_PHOTO_ALG_TYPE_HDR;

    ret = sample_photo_start_video();
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_start_video failed!\n");
        return ret;
    }

    ret = sample_photo_load_dsp_core_bin(OT_SVP_DSP_ID_0);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_load_dsp_core_bin failed!\n");
        goto exit;
    }

    ret = sample_photo_alg_init(alg_type, &phys_addr, vir_addr);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_alg_init failed!\n");
        goto exit1;
    }

    ret = sample_photo_start_snap(OT_SNAP_TYPE_PRO);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_start_snap failed!\n");
        goto exit2;
    }

    ret = sample_photo_hdr_process();
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_hdr_process failed!\n");
        goto exit3;
    }

    sample_get_char();

exit3:
    ss_mpi_snap_disable_pipe(SNAP_PIPE_ID);
exit2:
    sample_photo_alg_deinit(alg_type, phys_addr, vir_addr);
exit1:
    sample_photo_unload_dsp_core_bin(OT_SVP_DSP_ID_0);
exit:
    sample_photo_stop_video();

    return ret;
}

static td_s32 sample_photo_mfnr_process()
{
    td_s32 ret, i;
    ot_video_frame_info video_frame[OT_PHOTO_HDR_FRAME_NUM];
    ot_video_frame_info dst_video_frame;
    ot_vb_blk vb_blk;
    ot_photo_alg_attr photo_attr;
    td_bool dst_buf_alloc = TD_FALSE;

    for (i = 0; i < OT_PHOTO_MFNR_FRAME_NUM; i++) {
        ret = ss_mpi_vpss_get_chn_frame(SNAP_PIPE_ID, 0, &video_frame[i], 1000); /* milli_sec: 1000us */
        if (ret != TD_SUCCESS) {
            sample_print("ss_mpi_vpss_get_chn_frame failed!\n");
            goto exit;
        }

        if (!dst_buf_alloc) {
            vb_blk = samplie_photo_get_dst_frame_info(&dst_video_frame, &video_frame[i]);
            if (OT_VB_INVALID_HANDLE == vb_blk) {
                goto exit;
            }
            dst_buf_alloc = TD_TRUE;
        }

        photo_attr.mfnr_attr.src_frm = video_frame[i];
        photo_attr.mfnr_attr.des_frm = dst_video_frame;
        photo_attr.mfnr_attr.frm_index = i;
        photo_attr.mfnr_attr.iso = sample_photo_get_iso(&video_frame[i]);
        ret = ss_mpi_photo_alg_process(OT_PHOTO_ALG_TYPE_MFNR, &photo_attr);
        if (ret != TD_SUCCESS) {
            sample_print("ss_mpi_photo_alg_process failed!\n");
            goto exit;
        }
    }

    sample_photo_copy_isp_info(&dst_video_frame, &video_frame[1]);
    ret = sample_photo_save_jpeg(&dst_video_frame);

exit:
    for (i = i - 1; i >= 0; i--) {
        ss_mpi_vpss_release_chn_frame(SNAP_PIPE_ID, 0, &video_frame[i]);
    }

    if (dst_buf_alloc == TD_TRUE) {
        ss_mpi_vb_release_blk(vb_blk);
    }

    return ret;
}

static td_s32 sample_photo_mfnr_set_coef(td_void)
{
    td_s32 ret;
    const ot_photo_alg_type alg_type = OT_PHOTO_ALG_TYPE_MFNR;
    ot_photo_alg_coef alg_coef;

    ret = ss_mpi_photo_get_alg_coef(alg_type, &alg_coef);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_photo_get_alg_coef failed!\n");
        return TD_FAILURE;
    }

    alg_coef.mfnr_coef.de_enable = TD_FALSE;
    ret = ss_mpi_photo_set_alg_coef(alg_type, &alg_coef);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_photo_set_alg_coef failed!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 sample_photo_mfnr_route(void)
{
    td_s32 ret;
    td_u64 phys_addr;
    td_void *vir_addr = TD_NULL;
    const ot_photo_alg_type alg_type = OT_PHOTO_ALG_TYPE_MFNR;

    ret = sample_photo_start_video();
    if (ret != TD_SUCCESS) {
        sample_print("SAMPLE_PHOTO_COMM_Start_Video failed!\n");
        return ret;
    }

    ret = sample_photo_load_dsp_core_bin(OT_SVP_DSP_ID_0);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_load_dsp_core_bin failed!\n");
        goto exit;
    }

    ret = sample_photo_alg_init(alg_type, &phys_addr, vir_addr);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_alg_init failed!\n");
        goto exit1;
    }

    ret = sample_photo_mfnr_set_coef();
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_mfnr_set_coef failed!\n");
        goto exit2;
    }

    ret = sample_photo_start_snap(OT_SNAP_TYPE_NORM);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_start_snap failed!\n");
        goto exit2;
    }

    ret = sample_photo_mfnr_process();
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_mfnr_process failed!\n");
        goto exit3;
    }

    sample_get_char();

exit3:
    ss_mpi_snap_disable_pipe(SNAP_PIPE_ID);
exit2:
    sample_photo_alg_deinit(alg_type, phys_addr, vir_addr);
exit1:
    sample_photo_unload_dsp_core_bin(OT_SVP_DSP_ID_0);
exit:
    sample_photo_stop_video();

    return ret;
}

static td_s32 sample_photo_sfnr_process()
{
    td_s32 ret;
    const td_s32 vpss_grp = SNAP_PIPE_ID;
    const td_s32 vpss_chn = 0;
    const ot_photo_alg_type alg_type = OT_PHOTO_ALG_TYPE_SFNR;
    ot_video_frame_info video_frame;
    ot_photo_alg_attr photo_attr;

    ret = ss_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn, &video_frame, -1); /* milli_sec: -1 */
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_vpss_get_chn_frame failed!\n");
        return TD_FAILURE;
    }

    photo_attr.sfnr_attr.frm = video_frame;
    photo_attr.sfnr_attr.iso = sample_photo_get_iso(&video_frame);
    ret = ss_mpi_photo_alg_process(alg_type, &photo_attr);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_photo_alg_process failed!\n");
        goto exit;
    }

    ret = sample_photo_save_jpeg(&video_frame);

exit:
    ss_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn, &video_frame);
    return ret;
}

static td_s32 sample_photo_sfnr_route(void)
{
    td_s32 ret;
    td_u64 phys_addr;
    td_void *vir_addr = TD_NULL;
    const ot_photo_alg_type alg_type = OT_PHOTO_ALG_TYPE_SFNR;

    ret = sample_photo_start_video();
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_start_video failed!\n");
        return ret;
    }

    ret = sample_photo_load_dsp_core_bin(OT_SVP_DSP_ID_0);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_load_dsp_core_bin failed!\n");
        goto exit;
    }

    ret = sample_photo_alg_init(alg_type, &phys_addr, vir_addr);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_alg_init failed!\n");
        goto exit1;
    }

    ret = sample_photo_start_snap(OT_SNAP_TYPE_NORM);
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_start_snap failed!\n");
        goto exit2;
    }

    ret = sample_photo_sfnr_process();
    if (ret != TD_SUCCESS) {
        sample_print("sample_photo_sfnr_process failed!\n");
        goto exit3;
    }

    sample_get_char();

exit3:
    ss_mpi_snap_disable_pipe(SNAP_PIPE_ID);
exit2:
    sample_photo_alg_deinit(alg_type, phys_addr, vir_addr);
exit1:
    sample_photo_unload_dsp_core_bin(OT_SVP_DSP_ID_0);
exit:
    sample_photo_stop_video();
    return ret;
}

#ifdef __LITEOS__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    td_s32 ret = TD_FAILURE;
    td_s32 index = 0;

    if (argc != 2) { /* argc_num: 2 */
        sample_photo_usage(argv[0]);
        return TD_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_photo_usage(argv[0]);
        return TD_FAILURE;
    }

    if (strlen(argv[1]) != 1 || !check_digit(argv[1][0])) {
        sample_photo_usage(argv[0]);
        return TD_FAILURE;
    }

#ifndef __LITEOS__
    sample_sys_signal(sample_photo_handle_sig);
#endif

    index = atoi(argv[1]);
    switch (index) {
        case 0:
            ret = sample_photo_hdr_route();
            break;

        case 1:
            ret = sample_photo_mfnr_route();
            break;

        case 2: /* argv param: 2 */
            ret = sample_photo_sfnr_route();
            break;

        default:
            sample_print("the index %d is invalid!\n", index);
            sample_photo_usage(argv[0]);
            return TD_FAILURE;
    }

    if ((ret == TD_SUCCESS) && (g_photo_sample_exit == 0)) {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    return (ret);
}