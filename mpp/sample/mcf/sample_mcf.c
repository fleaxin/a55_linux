/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <limits.h>

#include "sample_comm.h"
#include "ot_sns_ctrl.h"
#include "sample_comm_mcf.h"
#include "sample_mcf_calibration.h"
#include "ot_common_mcf.h"
#include "ss_mpi_mcf_vi.h"
#ifdef __LITEOS__
#define SAMPLE_MCF_SAVE_PATH   "/sharefs/data"
#else
#define SAMPLE_MCF_SAVE_PATH   "./data"
#endif

#define B_32BYTEALINE 0
#define VB_SYS_STRIDE 32

#define MCF_BIND_VPSS_GRP 2
#define MCF_VO_WIDHT_1920 1920
#define MCF_VO_HEIGHT_1080 1080

#define MCF_VENC_WIDHT_3840 3840
#define MCF_VNEC_HEIGHT_2160 2160

#define VB_MCF_YUV_CNT  15
#define VB_MCF_EQUAL_YUV_CNT  20
#define MCF_SAMPLE_ARG_NUMBER_BASE 10

volatile static sig_atomic_t g_sig_flag = 0;

td_bool g_vpss_chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {TD_TRUE, TD_TRUE, TD_FALSE};
td_bool g_mcf_chn_enable[OT_MCF_MAX_PHYS_CHN_NUM] = {TD_TRUE, TD_FALSE, TD_FALSE};
ot_size g_mcf_chn_out_size[OT_MCF_MAX_PHYS_CHN_NUM] = { 0 };
static sample_vo_cfg g_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_intf_type      = OT_VO_INTF_HDMI,
    .intf_sync         = OT_VO_OUT_1080P30,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, MCF_VO_WIDHT_1920, MCF_VO_HEIGHT_1080},
    .image_size        = {MCF_VO_WIDHT_1920, MCF_VO_HEIGHT_1080},
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
    .venc_size            = {MCF_VENC_WIDHT_3840, MCF_VNEC_HEIGHT_2160},
    .size                 = PIC_3840X2160,
    .profile              = 0,
    .is_rcn_ref_share_buf = TD_FALSE,
    .gop_attr             = {
        .gop_mode = OT_VENC_GOP_MODE_NORMAL_P,
        .normal_p = {2},
    },
    .type                 = OT_PT_H265,
    .rc_mode              = SAMPLE_RC_CBR,
};

static td_void sample_get_char(td_char *s)
{
    if (g_sig_flag == 1) {
        return;
    }

    printf("---------------press any key to %s!---------------\n", s);
    (td_void)getchar();
}

static td_void sample_mcf_get_default_vb_config(const ot_size *size, ot_vb_cfg *vb_cfg)
{
    ot_vb_calc_cfg calc_cfg;
    ot_pic_buf_attr buf_attr;

    (td_void)memset_s(vb_cfg, sizeof(ot_vb_cfg), 0, sizeof(ot_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 pool cnt */

    buf_attr.width         = size->width;
    buf_attr.height        = size->height;
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    buf_attr.compress_mode = OT_COMPRESS_MODE_NONE;
    ot_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    vb_cfg->common_pool[0].blk_size = ot_common_get_pic_buf_size(&buf_attr);
    vb_cfg->common_pool[0].blk_cnt  = 30; /* 30 vb block cnt */
}

static td_s32 sample_mcf_sys_init(ot_vi_vpss_mode_type mode_type, ot_vi_video_mode video_mode,
                                  const ot_size *color_size)
{
    td_s32 ret;
    ot_vb_cfg vb_cfg;
    td_u32 supplement_config;

    sample_mcf_get_default_vb_config(color_size, &vb_cfg);

    supplement_config = OT_VB_SUPPLEMENT_BNR_MOT_MASK;
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

static td_s32 sample_mcf_start_venc(ot_venc_chn venc_chn[], td_u32 chn_num, td_u32 chn_len)
{
    td_s32 i;
    td_s32 ret;
    if (chn_len != 3) {    /* 3: chn len  */
        return TD_FAILURE;
    }
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

static td_void sample_mcf_stop_venc(ot_venc_chn venc_chn[], td_u32 chn_num, td_u32 chn_len)
{
    td_u32 i;
    if (chn_len != 3) {           /* 3: chn len  */
        return;
    }
    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static td_s32 sample_mcf_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

static td_void sample_mcf_stop_vo(td_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

static td_s32 sample_mcf_start_vpss(ot_vpss_grp grp, ot_size *in_size)
{
    ot_vpss_grp_attr grp_attr = {0};
    ot_vpss_chn_attr chn_attr[OT_VPSS_MAX_PHYS_CHN_NUM] = {0};
    ot_vpss_chn_attr *attr = TD_NULL;
    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    grp_attr.mcf_en     = TD_FALSE;
    attr = &chn_attr[0];
    sample_comm_vpss_get_default_chn_attr(attr);
    chn_attr[0].width  = in_size->width;
    chn_attr[0].height = in_size->height;
    attr = &chn_attr[1];
    sample_comm_vpss_get_default_chn_attr(attr);
    chn_attr[1].width  = MCF_VO_WIDHT_1920;
    chn_attr[1].height = MCF_VO_HEIGHT_1080;
    chn_attr[1].compress_mode = OT_COMPRESS_MODE_NONE;
    return sample_common_vpss_start(grp, g_vpss_chn_enable, &grp_attr, &chn_attr[0], OT_VPSS_MAX_PHYS_CHN_NUM);
}

static td_void sample_mcf_stop_vpss(ot_vpss_grp grp)
{
    sample_common_vpss_stop(grp, g_vpss_chn_enable, OT_VPSS_MAX_PHYS_CHN_NUM);
}

static td_s32 sample_mcf_start_vpss_venc_and_vo(ot_size *out_size)
{
    td_s32 ret;
    const sample_vo_mode vo_mode = VO_MODE_1MUX;
    ot_venc_chn venc_chn[3] = {0, 1, 2}; /* 3: max chn num, 0/1/2 chn id */
    const ot_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    const td_s32 grp_num = 1;
    ret = sample_mcf_start_vpss(vpss_grp, out_size);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    ret = sample_mcf_start_vo(vo_mode);
    if (ret != TD_SUCCESS) {
        goto start_vo_failed;
    }

    g_venc_chn_param.venc_size.width        = out_size->width;
    g_venc_chn_param.venc_size.height       = out_size->height;

    if (SENSOR0_TYPE == SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT) {
        g_venc_chn_param.size             = PIC_2560X1440;
    } else {
        g_venc_chn_param.size             = PIC_3840X2160;
    }

    ret = sample_mcf_start_venc(venc_chn, grp_num, 3); /* 3: max chn num, 0/1/2 chn id */
    if (ret != TD_SUCCESS) {
        goto start_venc_failed;
    }
    return TD_SUCCESS;

start_vo_failed:
    sample_mcf_stop_vpss(vpss_grp);

start_venc_failed:
    sample_mcf_stop_vo();
    return TD_FAILURE;
}

static td_void sample_mcf_stop_vpss_venc_and_vo()
{
    const td_s32 grp_num = 1;
    ot_venc_chn venc_chn[3] = {0, 1, 2}; /* 3: chn num, 0/1/2 chn id */
    const ot_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    sample_mcf_stop_venc(venc_chn, grp_num, 3); /* 3: chn num, 0/1/2 chn id */
    sample_mcf_stop_vo();
    sample_mcf_stop_vpss(vpss_grp);
}

static td_void sample_mcf_get_vi_config(sample_sns_type sns_type, sample_vi_cfg *vi_cfg, ot_vi_pipe pipe_id)
{
    ot_vi_bind_pipe *bind_pipe = TD_NULL;
    sample_vi_grp_info  *grp_info = TD_NULL;

    (td_void)memset_s(vi_cfg, sizeof(sample_vi_cfg), 0, sizeof(sample_vi_cfg));
    /* sensor info */
    sample_comm_vi_get_default_sns_info(sns_type, &vi_cfg->sns_info);
    /* mipi info */
    sample_comm_vi_get_default_mipi_info(sns_type, &vi_cfg->mipi_info);
    /* dev info */
    sample_comm_vi_get_default_dev_info(sns_type, &vi_cfg->dev_info);

    /* bind info */
    bind_pipe = &vi_cfg->bind_pipe;
    bind_pipe->pipe_num = 1;
    bind_pipe->pipe_id[0] = pipe_id;

    /* grp info */
    grp_info = &vi_cfg->grp_info;
    grp_info->grp_num = 1;
    grp_info->fusion_grp[0] = 0;
    grp_info->fusion_grp[1] = 1;
    grp_info->fusion_grp_attr[0].wdr_mode = OT_WDR_MODE_NONE;
    grp_info->fusion_grp_attr[0].pipe_id[0] = pipe_id;
    grp_info->fusion_grp_attr[0].cache_line = 100; /* 100 line value */

    sample_comm_vi_get_default_pipe_info(sns_type, &vi_cfg->bind_pipe, vi_cfg->pipe_info);
    vi_cfg->pipe_info->chn_info[0].chn_attr.depth                          = 1;
}

static td_s32 sample_mcf_start_vi0(const ot_mcf_grp_attr *mcf_grp_attr, sample_vi_cfg *vi_cfg0)
{
    td_s32 ret;
    ot_vi_pipe vi_mono_pipe;
    vi_mono_pipe = VI_MONO_PIPE;
    /* start mono pipe */
    sample_mcf_get_vi_config(SENSOR0_TYPE, vi_cfg0, vi_mono_pipe);
    vi_cfg0->mipi_info.divide_mode = LANE_DIVIDE_MODE_1;
    vi_cfg0->dev_info.vi_dev = 0;
    ret = sample_comm_vi_start_vi(vi_cfg0);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 sample_mcf_start_vi1(const ot_mcf_grp_attr *mcf_grp_attr, sample_vi_cfg *vi_cfg1)
{
    td_s32 ret;
    ot_vi_pipe vi_color_pipe;
    const ot_vi_dev vi_dev = 2; /* dev 2 for sensor1 */
    vi_color_pipe = VI_COLOR_PIPE;
    /* start color pipe */
    sample_mcf_get_vi_config(SENSOR1_TYPE, vi_cfg1, vi_color_pipe);
    vi_cfg1->sns_info.bus_id = 5;  /* senor id 5 */
    vi_cfg1->sns_info.sns_clk_src = 1;
    vi_cfg1->sns_info.sns_rst_src = 1;
    sample_comm_vi_get_mipi_info_by_dev_id(SENSOR1_TYPE, vi_dev, &vi_cfg1->mipi_info);
    vi_cfg1->mipi_info.divide_mode = LANE_DIVIDE_MODE_1;
    vi_cfg1->dev_info.vi_dev = vi_dev;
    ret = sample_comm_vi_start_vi(vi_cfg1);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void sample_mcf_bind_vi(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr, ot_vi_chn vi_chn)
{
    ot_mcf_pipe mcf_color_pipe, mcf_mono_pipe;
    ot_vi_pipe vi_mono_pipe, vi_color_pipe;

    vi_mono_pipe = VI_MONO_PIPE;
    vi_color_pipe = VI_COLOR_PIPE;

    mcf_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    sample_common_vi_bind_mcf(vi_mono_pipe, vi_chn, mcf_grp, mcf_mono_pipe);
    sample_common_vi_bind_mcf(vi_color_pipe, vi_chn, mcf_grp, mcf_color_pipe);
}

static td_void sample_mcf_un_bind_vi(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr, ot_vi_chn vi_chn)
{
    ot_mcf_pipe mcf_color_pipe, mcf_mono_pipe;
    ot_vi_pipe vi_mono_pipe, vi_color_pipe;

    vi_mono_pipe = VI_MONO_PIPE;
    vi_color_pipe = VI_COLOR_PIPE;

    mcf_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    sample_common_vi_unbind_mcf(vi_mono_pipe, vi_chn, mcf_grp, mcf_mono_pipe);
    sample_common_vi_unbind_mcf(vi_color_pipe, vi_chn, mcf_grp, mcf_color_pipe);
}

static td_void sample_mcf_start_mcf_vi_cfg(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr)
{
    ot_mcf_vi_attr mcf_vi_attr;
    td_s32 ret;
    mcf_vi_attr.enable     = TD_TRUE;
    mcf_vi_attr.mono_pipe  = VI_MONO_PIPE;
    mcf_vi_attr.color_pipe = VI_COLOR_PIPE;
    ret = ss_mpi_mcf_set_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_mcf_set_vi_attr error\n");
    }
}

static td_void sample_mcf_stop_mcf_vi_cfg(ot_mcf_grp mcf_grp)
{
    ot_mcf_vi_attr mcf_vi_attr;
    td_s32 ret;
    ret = ss_mpi_mcf_get_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_mcf_get_vi_attr err\n");
        return;
    }
    mcf_vi_attr.enable     = TD_FALSE;
    ret = ss_mpi_mcf_set_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_mcf_set_vi_attr error\n");
    }
}

td_void sample_mcf_set_default_vpss_attr(ot_vpss_grp_attr *vpss_grp_attr)
{
    (td_void)memset_s(vpss_grp_attr, sizeof(ot_vpss_grp_attr), 0, sizeof(ot_vpss_grp_attr));
    vpss_grp_attr->nr_en                     = TD_TRUE;
    vpss_grp_attr->ie_en                     = TD_FALSE;
    vpss_grp_attr->dci_en                    = TD_FALSE;
    vpss_grp_attr->buf_share_en              = TD_FALSE;
    vpss_grp_attr->mcf_en                    = TD_FALSE;
    vpss_grp_attr->max_width                 = 4096;  /* 4096 max width */
    vpss_grp_attr->max_height                = 4096; /* 4096 max height */
    vpss_grp_attr->max_dei_width             = 0;
    vpss_grp_attr->max_dei_height            = 0;
    vpss_grp_attr->dynamic_range             = OT_DYNAMIC_RANGE_SDR8;
    vpss_grp_attr->dei_mode                  = OT_VPSS_DEI_MODE_OFF;
    vpss_grp_attr->buf_share_chn             = OT_VPSS_CHN0;
    vpss_grp_attr->nr_attr.nr_type           = OT_VPSS_NR_TYPE_VIDEO_NORM;
    vpss_grp_attr->nr_attr.compress_mode     = OT_COMPRESS_MODE_NONE;
    vpss_grp_attr->nr_attr.nr_motion_mode    = OT_VPSS_NR_MOTION_MODE_NORM;
    vpss_grp_attr->pixel_format = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vpss_grp_attr->frame_rate.src_frame_rate = -1;
    vpss_grp_attr->frame_rate.dst_frame_rate = -1;
}

static td_void sample_mcf_bind(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr)
{
    const ot_vi_chn vi_chn = 0;
    const ot_mcf_chn mcf_chn = 0;
    const ot_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    ot_vpss_chn vpss_chn[2] = {0, 1};
    const ot_vo_layer vo_layer = 0;
    const ot_vo_chn vo_chn = 0;
    const ot_venc_chn venc_chn = 0;

    sample_mcf_bind_vi(mcf_grp, mcf_grp_attr, vi_chn);
    sample_common_mcf_bind_vpss(mcf_grp, mcf_chn, vpss_grp, vpss_chn[0]);
    sample_comm_vpss_bind_venc(vpss_grp, vpss_chn[0], venc_chn);
    sample_comm_vpss_bind_vo(vpss_grp, vpss_chn[1], vo_layer, vo_chn);
}

static td_void sample_mcf_un_bind(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr)
{
    const ot_vi_chn vi_chn = 0;
    const ot_mcf_chn mcf_chn = 0;
    const ot_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    ot_vpss_chn vpss_chn[2] = {0, 1};
    const ot_vo_layer vo_layer = 0;
    const ot_vo_chn vo_chn = 0;
    const ot_venc_chn venc_chn = 0;

    sample_comm_vpss_un_bind_venc(vpss_grp, vpss_chn[0], venc_chn);
    sample_comm_vpss_un_bind_vo(vpss_grp, vpss_chn[1], vo_layer, vo_chn);
    sample_common_mcf_unbind_vpss(mcf_grp, mcf_chn, vpss_grp, vpss_chn[0]);
    sample_mcf_un_bind_vi(mcf_grp, mcf_grp_attr, vi_chn);
}

static td_s32 sample_mcf_start_mcf_data_flow(ot_mcf_grp mcf_grp, ot_mcf_grp_attr *mcf_grp_attr,
    ot_size out_size, ot_crop_info *grp_crop_info, sample_yuv_cnt yuv_cnt)
{
    td_s32 ret;

    sample_mcf_start_mcf_vi_cfg(mcf_grp, mcf_grp_attr);

    ret = sample_common_mcf_start_vpss(mcf_grp_attr, yuv_cnt.color_cnt, yuv_cnt.mono_cnt);
    if (ret != TD_SUCCESS) {
        goto stop_mcf_vi_cfg;
    }

    ret = sample_common_mcf_start(mcf_grp, mcf_grp_attr, grp_crop_info,
                                  &g_mcf_chn_enable[0], &g_mcf_chn_out_size[0]);
    if (ret != TD_SUCCESS) {
        goto start_mcf_failed;
    }
    ret = sample_mcf_start_vpss_venc_and_vo(&out_size);
    if (ret != TD_SUCCESS) {
        goto start_vpss_venc_and_vo_failed;
    }
    sample_mcf_bind(mcf_grp, mcf_grp_attr);

    return ret;
start_vpss_venc_and_vo_failed:
    sample_common_mcf_stop(mcf_grp, g_mcf_chn_enable, OT_MCF_MAX_PHYS_CHN_NUM);
start_mcf_failed:
    sample_common_mcf_stop_vpss(mcf_grp_attr);
stop_mcf_vi_cfg:
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);
    return ret;
}

static td_void sample_mcf_stop_mcf_data_flow(ot_mcf_grp mcf_grp, ot_mcf_grp_attr *mcf_grp_attr)
{
    const td_s32 mcf_grp_num = 1;
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);
    sample_mcf_un_bind(mcf_grp, mcf_grp_attr);
    sample_mcf_stop_vpss_venc_and_vo(mcf_grp, mcf_grp_num);
    sample_common_mcf_stop(mcf_grp, g_mcf_chn_enable, OT_MCF_MAX_PHYS_CHN_NUM);
    sample_common_mcf_stop_vpss(mcf_grp_attr);
}

static td_void sample_mcf_set_chn_defualt_outsize(const ot_size *out_size)
{
    td_s32 i;
    for (i = 0; i < OT_MCF_MAX_PHYS_CHN_NUM; i++) {
        (td_void)memcpy_s(&g_mcf_chn_out_size[i], sizeof(ot_size), out_size, sizeof(ot_size));
    }
}

static td_void sample_mcf_get_large_size(ot_size mono_size, ot_size color_size, ot_size *large_size)
{
    if (mono_size.width > color_size.width) {
        large_size->width = mono_size.width;
        large_size->height = mono_size.height;
    } else {
        large_size->width = color_size.width;
        large_size->height = color_size.height;
    }
}

static td_void sample_mcf_set_path(ot_mcf_grp mcf_grp, ot_mcf_path mcf_path)
{
    td_s32 ret;
    ot_mcf_grp_attr grp_attr;

    ret = ss_mpi_mcf_get_grp_attr(mcf_grp, &grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("get mcf grp attr failed.ret:0x%x !\n", ret);
        return;
    }
    grp_attr.mcf_path = mcf_path;
    ret = ss_mpi_mcf_set_grp_attr(mcf_grp, &grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("set mcf grp attr failed.ret:0x%x !\n", ret);
        return;
    }
}

static td_s32 sample_mcf_switch_to_color_path(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg0)
{
    td_s32 ret = TD_SUCCESS;
    ot_vpss_grp vpss_mono_grp;

    /* switch */
    sample_mcf_set_path(mcf_grp, OT_MCF_PATH_COLOR);
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);

    /* stop mono */
    vpss_mono_grp = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    ret = ss_mpi_vpss_stop_grp(vpss_mono_grp);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_mcf_destroy_grp failed with %#x!\n", ret);
    }
    ret = ss_mpi_vpss_destroy_grp(vpss_mono_grp);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_mcf_destroy_grp failed with %#x!\n", ret);
    }

    sample_comm_vi_stop_vi(vi_cfg0);
    return ret;
}

static td_s32 sample_mcf_switch_to_mono_path(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg0, sample_vi_cfg *vi_cfg1)
{
    td_s32 ret = TD_SUCCESS;
    ot_vpss_grp vpss_mono_grp;
    ot_vpss_grp vpss_color_grp;
    ot_vpss_grp_attr vpss_grp_attr;

    /* start mono */
    ret = sample_mcf_start_vi0(mcf_grp_attr, vi_cfg0);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    vpss_mono_grp = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    sample_common_mcf_set_default_vpss_attr(&vpss_grp_attr, mcf_grp_attr, TD_TRUE);
    ret = ss_mpi_vpss_create_grp(vpss_mono_grp, &vpss_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_vpss_create_grp failed with %#x!\n", ret);
    }

    ret = ss_mpi_vpss_start_grp(vpss_mono_grp);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_vpss_start_grp failed with %#x!\n", ret);
    }
    /* switch */
    sample_mcf_set_path(mcf_grp, OT_MCF_PATH_MONO);

    /* stop color */
    vpss_color_grp = mcf_grp_attr->color_pipe_attr.vpss_grp;
    ret = ss_mpi_vpss_stop_grp(vpss_color_grp);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_mcf_destroy_grp failed with %#x!\n", ret);
    }
    ret = ss_mpi_vpss_destroy_grp(vpss_color_grp);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_mcf_destroy_grp failed with %#x!\n", ret);
    }

    sample_comm_vi_stop_vi(vi_cfg1);

    return ret;
}

static td_s32 sample_mcf_switch_to_fusion_path(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg1)
{
    td_s32 ret = TD_SUCCESS;
    ot_vpss_grp vpss_color_grp;
    ot_vpss_grp_attr vpss_grp_attr;

    /* start color */
    ret = sample_mcf_start_vi1(mcf_grp_attr, vi_cfg1);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    vpss_color_grp = mcf_grp_attr->color_pipe_attr.vpss_grp;
    sample_common_mcf_set_default_vpss_attr(&vpss_grp_attr, mcf_grp_attr, TD_FALSE);
    ret = ss_mpi_vpss_create_grp(vpss_color_grp, &vpss_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_vpss_create_grp failed with %#x!\n", ret);
    }

    ret = ss_mpi_vpss_start_grp(vpss_color_grp);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_vpss_start_grp failed with %#x!\n", ret);
    }

    sample_mcf_start_mcf_vi_cfg(mcf_grp, mcf_grp_attr);

    /* switch */
    sample_mcf_set_path(mcf_grp, OT_MCF_PATH_FUSION);

    return ret;
}

static td_s32 sample_mcf_path_switch(ot_mcf_grp mcf_grp, const ot_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg0, sample_vi_cfg *vi_cfg1)
{
    td_s32 ret;

    sample_get_char("switch to color path!");
    ret = sample_mcf_switch_to_color_path(mcf_grp, mcf_grp_attr, vi_cfg0);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to mono path!");
    ret = sample_mcf_switch_to_mono_path(mcf_grp, mcf_grp_attr, vi_cfg0, vi_cfg1);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to fusion path!");
    ret = sample_mcf_switch_to_fusion_path(mcf_grp, mcf_grp_attr, vi_cfg1);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return ret;
}

static td_void sample_mcf_imx347_cfg(ot_mcf_grp_attr *mcf_grp_attr)
{
    ot_isp_sns_blc_clamp sns_blc_clamp;
    g_sns_imx347_slave_obj.pfn_mirror_flip(mcf_grp_attr->color_pipe_attr.pipe_id, ISP_SNS_MIRROR);
    sns_blc_clamp.blc_clamp_en = TD_TRUE;
    g_sns_imx347_slave_obj.pfn_set_blc_clamp(mcf_grp_attr->color_pipe_attr.pipe_id, sns_blc_clamp);
    return;
}

static td_s32 sample_mcf_two_vi_init_start(sample_vi_cfg *vi_cfg0, sample_vi_cfg *vi_cfg1,
    const ot_mcf_grp mcf_grp, ot_mcf_grp_attr *mcf_grp_attr, ot_size *large_size)
{
    td_s32 ret;
    ot_vi_vpss_mode_type mode_type = OT_VI_OFFLINE_VPSS_OFFLINE;
    ot_vi_video_mode video_mode = OT_VI_VIDEO_MODE_NORM;
    ot_size mono_size, color_size;

    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &mono_size);
    sample_comm_vi_get_size_by_sns_type(SENSOR1_TYPE, &color_size);

    sample_mcf_get_large_size(mono_size, color_size, large_size);
    ret = sample_mcf_sys_init(mode_type, video_mode, large_size);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    sample_common_mcf_get_default_grp_attr(mcf_grp_attr, &mono_size, &color_size);
    sample_mcf_set_chn_defualt_outsize(large_size);

    ret = sample_mcf_start_vi0(mcf_grp_attr, vi_cfg0);
    if (ret != TD_SUCCESS) {
        goto release_mem;
    }
    ret = sample_mcf_start_vi1(mcf_grp_attr, vi_cfg1);
    if (ret != TD_SUCCESS) {
        goto stop_vi0;
    }

    sample_mcf_imx347_cfg(mcf_grp_attr);

    return TD_SUCCESS;

stop_vi0:
    sample_comm_vi_stop_vi(vi_cfg0);
release_mem:
    sample_comm_sys_exit();
    return ret;
}

static td_s32 sample_mcf_two_sensor_vo_venc(td_void)
{
    td_s32 ret;
    const ot_mcf_grp mcf_grp = 0;
    ot_mcf_grp_attr mcf_grp_attr;
    sample_vi_cfg vi_cfg0, vi_cfg1;
    sample_yuv_cnt yuv_cnt = { 0 };
    ot_crop_info grp_crop = { 0 }; /* do not use mcf grp crop function */
    ot_size large_size;

    ret = sample_mcf_two_vi_init_start(&vi_cfg0, &vi_cfg1, mcf_grp, &mcf_grp_attr, &large_size);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    yuv_cnt.color_cnt = VB_MCF_YUV_CNT;
    yuv_cnt.mono_cnt = VB_MCF_YUV_CNT;
    ret = sample_mcf_start_mcf_data_flow(mcf_grp, &mcf_grp_attr, large_size, &grp_crop, yuv_cnt);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    sample_get_char("exit");
    sample_mcf_stop_mcf_data_flow(mcf_grp, &mcf_grp_attr);
exit:
    sample_comm_vi_stop_vi(&vi_cfg0);
    sample_comm_vi_stop_vi(&vi_cfg1);
    sample_comm_sys_exit();
    return ret;
}

static td_s32 sample_mcf_switch(td_void)
{
    td_s32 ret;
    const ot_mcf_grp mcf_grp = 0;
    ot_mcf_grp_attr mcf_grp_attr;
    sample_vi_cfg vi_cfg0, vi_cfg1;
    sample_yuv_cnt yuv_cnt = { 0 };
    ot_crop_info grp_crop = { 0 }; /* do not use mcf grp crop function */
    ot_size large_size;

    ret = sample_mcf_two_vi_init_start(&vi_cfg0, &vi_cfg1, mcf_grp, &mcf_grp_attr, &large_size);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    yuv_cnt.color_cnt = VB_MCF_YUV_CNT;
    yuv_cnt.mono_cnt = VB_MCF_YUV_CNT;
    ret = sample_mcf_start_mcf_data_flow(mcf_grp, &mcf_grp_attr, large_size, &grp_crop, yuv_cnt);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    ret = sample_mcf_path_switch(mcf_grp, &mcf_grp_attr, &vi_cfg0, &vi_cfg1);
    if (ret == TD_SUCCESS) {
        sample_get_char("exit");
    }

    sample_mcf_stop_mcf_data_flow(mcf_grp, &mcf_grp_attr);
exit:
    sample_comm_vi_stop_vi(&vi_cfg0);
    sample_comm_vi_stop_vi(&vi_cfg1);
    sample_comm_sys_exit();
    return ret;
}

static td_s32 sample_mcf_equal_size_sensor_correction_vo_venc(td_void)
{
    td_s32 ret;
    const ot_mcf_grp mcf_grp = 0;
    ot_mcf_grp_attr mcf_grp_attr;
    sample_vi_cfg vi_cfg0, vi_cfg1;
    sample_yuv_cnt yuv_cnt = { 0 };
    ot_crop_info grp_crop = { 0 }; /* do not use mcf grp crop function */
    ot_size large_size;

    ret = sample_mcf_two_vi_init_start(&vi_cfg0, &vi_cfg1, mcf_grp, &mcf_grp_attr, &large_size);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    printf("---------------start calibration---------------");
    ret = sample_mcf_calibrate_online(mcf_grp, &mcf_grp_attr, &grp_crop);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    yuv_cnt.color_cnt = VB_MCF_EQUAL_YUV_CNT;
    yuv_cnt.mono_cnt = VB_MCF_EQUAL_YUV_CNT;
    ret = sample_mcf_start_mcf_data_flow(mcf_grp, &mcf_grp_attr, large_size, &grp_crop, yuv_cnt);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    sample_get_char("exit");
    sample_mcf_stop_mcf_data_flow(mcf_grp, &mcf_grp_attr);
exit:
    sample_comm_vi_stop_vi(&vi_cfg0);
    sample_comm_vi_stop_vi(&vi_cfg1);
    sample_comm_sys_exit();
    return ret;
}

static td_void sample_mcf_usage(char *prg_name)
{
    sample_print("usage : %s <index> \n", prg_name);
    sample_print("index :\n");
    sample_print("    (0) mcf calib            : mcf same img_size calib \n");
    sample_print("    (1) mcf calib            : mcf diff img_size calib \n");
    sample_print("    (2) mcf base mode        : vi->vpss->mcf->vpss->venc && vo.\n");
    sample_print("    (3) mcf switch           : mcf switch to daytime and night.\n");
    sample_print("    (4) mcf correction route : mcf correct with image of equal size, "
        "you'd better keep still while calibrating.\n");
}

static td_void sample_mcf_handle_sig(td_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_sig_flag = 1;
    }
}

static td_void sample_register_sig_handler(td_void (*sig_handle)(td_s32))
{
    struct sigaction sa;

    (td_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sig_handle;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, TD_NULL);
    sigaction(SIGTERM, &sa, TD_NULL);
}

static td_s32 sample_mcf_execute_case(td_u32 case_index)
{
    td_s32 ret;

    switch (case_index) {
        case 0: /* 0 mcf calib */
            ret = sample_mcf_calibration();
            break;
        case 1: /* 1 mcf calib */
            ret = sample_mcf_diff_img_size_calibration();
            break;
        case 2: /* 2 two same sensor route */
            ret = sample_mcf_two_sensor_vo_venc();
            break;
        case 3: /* 3 mcf switch to daytime and night. */
            ret = sample_mcf_switch();
            break;
        case 4: /* 4 mcf with input image of equal size , and do correction. */
            ret = sample_mcf_equal_size_sensor_correction_vo_venc();
            break;
        default:
            ret = TD_FAILURE;
            break;
    }
    return ret;
}

#ifdef __LITEOS__
td_s32 app_main(td_s32 argc, td_char *argv[])
#else
td_s32 main(td_s32 argc, td_char *argv[])
#endif
{
    td_s32 ret;
    td_u32 index;
    td_slong val;
    td_char *end_ptr = TD_NULL;

    if (argc != 2) { /* 2:arg num */
        sample_mcf_usage(argv[0]);
        return TD_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_mcf_usage(argv[0]);
        return TD_FAILURE;
    }

    if ((strlen(argv[1]) != 1) ||
        (argv[1][0] < '0' || argv[1][0] > '4')) {
        sample_mcf_usage(argv[0]);
        return TD_FAILURE;
    }

#ifndef __LITEOS__
    sample_register_sig_handler(sample_mcf_handle_sig);
#endif

    errno = 0;
    val = strtol(argv[1], &end_ptr, MCF_SAMPLE_ARG_NUMBER_BASE);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
        return TD_FAILURE;
    }
    if ((end_ptr == argv[1]) || (*end_ptr != '\0')) {
        return TD_FAILURE;
    }
    index = (td_u32)val;

    ret = sample_mcf_execute_case(index);
    if ((ret == TD_SUCCESS) && (g_sig_flag == 0)) {
        sample_print("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        sample_print("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

#ifdef __LITEOS__
    return ret;
#else
    exit(ret);
#endif
}
