/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>

#include "ss_mpi_sys.h"
#ifdef CONFIG_OT_VI_ALL_SUPPORT
#include "ss_mpi_vi.h"
#else
#include "ss_mpi_vpss.h"
#include "ot_common_vpss.h"
#endif
#include "ss_mpi_vb.h"
#include "ot_common_vb.h"
#include "ot_common_gdc.h"
#include "ot_buffer.h"
#include "securec.h"

#include "ss_mpi_fisheye_calibration.h"

#define FILE_NAME_LENGTH 128
#define LINE_SIZE 2048
#define VB_BLK_CNT 2

#define FISHEYE_CALIBRATE_LEVEL OT_FISHEYE_CALIBRATION_LEVEL_1
td_u32  g_orig_depth = 0;
static volatile td_bool g_quit = TD_FALSE;   /* g_quit may be set in the signal handler */
static FILE *g_fd_out = TD_NULL;
ot_fisheye_correction_attr g_correction_attr = {0};
td_bool g_fisheye_orig_en = TD_FALSE;

#ifdef CONFIG_OT_VI_ALL_SUPPORT
typedef ot_vi_chn chn_type;
typedef ot_vi_chn_attr chn_attr_type;
typedef ot_vi_pipe parameter_type;
static ot_vi_chn g_chn = 0;
static ot_vi_pipe g_parameter = 0;
#else
typedef ot_vpss_chn chn_type;
typedef ot_vpss_chn_attr chn_attr_type;
typedef ot_vpss_grp parameter_type;
static td_u32 g_vpss_chn_mode = 0;
static ot_vpss_chn g_chn = 0;
static ot_vpss_grp g_parameter = 0;
#endif

#ifndef __LITEOS__
#define SAVE_PATH "."
#else
#define SAVE_PATH "/sharefs/output"
#endif

#define fisheye_clip3(low, high, x) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

static void usage(void)
{
    printf(
        "\n"
        "*************************************************\n"
#ifdef CONFIG_OT_VI_ALL_SUPPORT
#ifndef __LITEOS__
        "Usage: ./fisheye_calibrate [ViPipe] [ViPhyChn].\n"
#else
        "Usage: fisheye_cal [ViPipe] [ViPhyChn].\n"
#endif
        "1)ViPipe: \n"
        "   the source ViPipe\n"
        "2)ViPhyChn: \n"
        "   the source physic channel bind the exetend channel\n"
        "*)Example:\n"
#else
#ifndef __LITEOS__
        "Usage: ./fisheye_calibrate [VpssGrp] [VpssChn].\n"
#else
        "Usage: fisheye_cal [VpssGrp] [VpssChn].\n"
#endif
        "1)VpssGrp: \n"
        "   the source ViPipe\n"
        "2)VpssPhyChn: \n"
        "   the source physic channel bind the exetend channel\n"
        "*)Example:\n"
#endif

#ifndef __LITEOS__
        "   e.g : ./fisheye_calibration 0 0\n"
#else
        "   e.g : fisheye_cal 0 0\n"
#endif
        "*************************************************\n\n");
}

static inline td_void safe_close_file(td_void)
{
    if (g_fd_out != TD_NULL) {
        fclose(g_fd_out);
        g_fd_out = TD_NULL;
    }
}

static void sig_handler(td_s32 signo)
{
    if (g_quit == TD_TRUE) {
        return;
    }

    if ((SIGINT == signo) || (SIGTERM == signo)) {
        g_quit = TD_TRUE;
    }
}

static td_s32 fisheye_calibrate_map_virt_addr(ot_video_frame_info *frame)
{
    td_s32 size = 0;
    td_phys_addr_t phy_addr;
    ot_pixel_format pixel_format;
    td_u32 y_size;
    /* td_u32 uv_interval = 0 */

    pixel_format = frame->video_frame.pixel_format;
    phy_addr = frame->video_frame.phys_addr[0];
    if (pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        size = (frame->video_frame.stride[0]) * (frame->video_frame.height) * 3 / 2; /* 3 / 2 for size */
        y_size = frame->video_frame.stride[0] * frame->video_frame.height;
        /* uv_interval = 0 */
    } else if (pixel_format == OT_PIXEL_FORMAT_YVU_PLANAR_420) {
        size = (frame->video_frame.stride[0]) * (frame->video_frame.height) * 3 / 2; /* 3 / 2 for size */
        y_size = frame->video_frame.stride[0] * frame->video_frame.height;
        /* uv_interval = frame->video_frame.width * frame->video_frame.height / 2 */
    } else {
        printf("not support pixelformat: %d\n", pixel_format);
        return TD_FAILURE;
    }

    frame->video_frame.virt_addr[0] = ss_mpi_sys_mmap_cached(phy_addr, size);
    if (frame->video_frame.virt_addr[0] == TD_NULL) {
        printf("Map Virt Addr Failed!\n");
        return TD_FAILURE;
    }

    frame->video_frame.virt_addr[1] = frame->video_frame.virt_addr[0] + y_size;
    /* frame->video_frame.virt_addr[2] = frame->video_frame.virt_addr[1] + uv_interval */
    return TD_SUCCESS;
}

static td_void fisheye_calibrate_save_yuv(const ot_video_frame *v_buf, unsigned char *tmp_buff,
    td_u32 line_size, const char *v_buf_virt_y, td_u32 uv_height)
{
    unsigned int w, h;
    const char *mem_content = TD_NULL;
    const char *v_buf_virt_c = TD_NULL;
    td_u32 ret = 0;

    if (v_buf->width / 2 > LINE_SIZE) { /* 2 half */
        return;
    }

    v_buf_virt_c = (char *)(v_buf_virt_y + (v_buf->stride[0]) * (v_buf->height));
    /* save Y */
    for (h = 0; h < v_buf->height; h++) {
        mem_content = v_buf_virt_y + h * v_buf->stride[0];
        ret = fwrite(mem_content, 1, v_buf->width, g_fd_out);
        if (ret != v_buf->width) {
            printf("fwrite fail ! Func:%s line:%d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    /* save U */
    for (h = 0; h < uv_height; h++) {
        mem_content = v_buf_virt_c + h * v_buf->stride[1];
        mem_content += 1;
        for (w = 0; w < v_buf->width / 2; w++) { /* 2 half */
            tmp_buff[w] = *mem_content;
            mem_content += 2; /* 2 for content */
        }
        ret = fwrite(tmp_buff, 1, v_buf->width / 2, g_fd_out); /* 2 half */
        if (ret != v_buf->width / 2) {
            printf("fwrite fail ! Func:%s line:%d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    /* save V */
    for (h = 0; h < uv_height; h++) {
        mem_content = v_buf_virt_c + h * v_buf->stride[1];
        for (w = 0; w < v_buf->width / 2; w++) { /* 2 half */
            tmp_buff[w] = *mem_content;
            mem_content += 2; /* 2 for content */
        }
        ret = fwrite(tmp_buff, 1, v_buf->width / 2, g_fd_out); /* 2 half */
        if (ret != v_buf->width / 2) {
            printf("fwrite fail ! Func:%s line:%d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
}
static td_void fisheye_calibrate_save_sp42x_to_planar(const ot_video_frame *v_buf)
{
    char *v_buf_virt_y = TD_NULL;
    unsigned char *tmp_buff = TD_NULL;
    ot_pixel_format pixel_format = v_buf->pixel_format;
    td_u32 uv_height;
    td_u32 line_size = LINE_SIZE;

    if (pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        uv_height = v_buf->height / 2; /* 2 half */
    } else {
        uv_height = v_buf->height;
    }

    v_buf_virt_y = (char *)v_buf->virt_addr[0];
    tmp_buff = (unsigned char *)malloc(line_size);
    if (tmp_buff == TD_NULL) {
        printf("Func:%s line:%d -- unable alloc %dB memory for tmp buffer\n",
               __FUNCTION__, __LINE__, line_size);
        return;
    }

    fisheye_calibrate_save_yuv(v_buf, tmp_buff, line_size, v_buf_virt_y, uv_height);
    free(tmp_buff);
}

static td_s32 fisheye_calibrate_misc_buf_attr_get(ot_pic_buf_attr *pic_buf_attr, ot_vb_calc_cfg *cal_config,
    const ot_video_frame_info *in_frame)
{
    pic_buf_attr->align = 0;
    pic_buf_attr->bit_width = OT_DATA_BIT_WIDTH_8;
    pic_buf_attr->compress_mode = OT_COMPRESS_MODE_NONE;
    pic_buf_attr->width = in_frame->video_frame.width;
    pic_buf_attr->height = in_frame->video_frame.height;
    pic_buf_attr->pixel_format = in_frame->video_frame.pixel_format;

    ot_common_get_pic_buf_cfg(pic_buf_attr, cal_config);
    if (in_frame->video_frame.pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        /* uv_interval = 0 */
    } else if (in_frame->video_frame.pixel_format == OT_PIXEL_FORMAT_YVU_PLANAR_420) {
        /* uv_interval = cal_config->main_y_size >> 2 */
    } else {
        printf("Error!!!, not support pixel_format: %d\n", in_frame->video_frame.pixel_format);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 fisheye_calibrate_misc_get_vb(ot_video_frame_info *out_frame, const ot_video_frame_info *in_frame,
    ot_vb_blk *out_vb_blk, ot_vb_pool pool)
{
    td_phys_addr_t phys_addr;
    td_void *virt_addr = TD_NULL;
    ot_vb_calc_cfg cal_config;
    /* td_u32 uv_interval */
    ot_pic_buf_attr pic_buf_attr;

    if (fisheye_calibrate_misc_buf_attr_get(&pic_buf_attr, &cal_config, in_frame) != TD_SUCCESS) {
        goto end0;
    }

    *out_vb_blk = ss_mpi_vb_get_blk(pool, cal_config.vb_size, TD_NULL);
    if (*out_vb_blk == OT_VB_INVALID_HANDLE) {
        printf("ss_mpi_vb_get_blk err! size:%d\n", cal_config.vb_size);
        goto end0;
    }

    phys_addr = ss_mpi_vb_handle_to_phys_addr(*out_vb_blk);
    if (phys_addr == 0) {
        printf("ss_mpi_vb_handle_to_phys_addr err!\n");
        goto end1;
    }

    virt_addr = ss_mpi_sys_mmap(phys_addr, cal_config.vb_size);
    if (virt_addr == TD_NULL) {
        printf("ss_mpi_sys_mmap err!\n");
        goto end1;
    }

    out_frame->pool_id = ss_mpi_vb_handle_to_pool_id(*out_vb_blk);
    if (out_frame->pool_id == OT_VB_INVALID_POOL_ID) {
        printf("pool_id err!\n");
        goto end2;
    }

    out_frame->video_frame.phys_addr[0] = phys_addr;
    out_frame->video_frame.phys_addr[1] = out_frame->video_frame.phys_addr[0] + cal_config.main_y_size;
    /* out_frame->video_frame.phys_addr[2] = out_frame->video_frame.phys_addr[1] + uv_interval */
    out_frame->video_frame.virt_addr[0] = virt_addr;
    out_frame->video_frame.virt_addr[1] = out_frame->video_frame.virt_addr[0] + cal_config.main_y_size;
    /* out_frame->video_frame.virt_addr[2] = out_frame->video_frame.virt_addr[1] + uv_interval */
    out_frame->video_frame.width  = pic_buf_attr.width;
    out_frame->video_frame.height = pic_buf_attr.height;
    out_frame->video_frame.stride[0] = cal_config.main_stride;
    out_frame->video_frame.stride[1] = cal_config.main_stride;
    /* out_frame->video_frame.stride[2] = cal_config.main_stride */
    out_frame->video_frame.field = OT_VIDEO_FIELD_FRAME;
    out_frame->video_frame.compress_mode = OT_COMPRESS_MODE_NONE;
    out_frame->video_frame.pixel_format = pic_buf_attr.pixel_format;

    return TD_SUCCESS;

end2:
    ss_mpi_sys_munmap(virt_addr, cal_config.vb_size);
end1:
    ss_mpi_vb_release_blk(*out_vb_blk);
end0:
    return TD_FAILURE;
}

static td_s32 fisheye_calibrate_get_chn_fisheye(ot_fisheye_correction_attr *fisheye_correction_attr)
{
#ifdef CONFIG_OT_VI_ALL_SUPPORT
    if (ss_mpi_vi_get_chn_fisheye(g_parameter, g_chn, fisheye_correction_attr) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#else
    if (ss_mpi_vpss_get_chn_fisheye(g_parameter, g_chn, fisheye_correction_attr) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#endif
    return TD_SUCCESS;
}

static td_s32 fisheye_calibrate_set_chn_fisheye(const ot_fisheye_correction_attr *fisheye_correction_attr)
{
#ifdef CONFIG_OT_VI_ALL_SUPPORT
    if (ss_mpi_vi_set_chn_fisheye(g_parameter, g_chn, fisheye_correction_attr) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#else
    if (ss_mpi_vpss_set_chn_fisheye(g_parameter, g_chn, fisheye_correction_attr) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#endif
    return TD_SUCCESS;
}

static td_s32 fisheye_calibrate_check_and_set(int argc, char *argv[])
{
    const td_s32 base = 10; /* 10:Decimal */
    td_char *end_ptr = TD_NULL;

    printf("\nNOTICE: This tool only can be used for TESTING !!!\n");
    printf("NOTICE: This tool only only support PIXEL_FORMAT_YVU_SEMIPLANAR_420\n");
    printf("NOTICE: This tool only only support DYNAMIC_RANGE_SDR8\n");
    printf("NOTICE: This tool only only support COMPRESS_MODE_NONE\n");

    /* register signal handler */
    (td_void)signal(SIGINT, sig_handler);
    (td_void)signal(SIGTERM, sig_handler);

    if ((argc > 1) && (!strncmp(argv[1], "-h", 2))) {   /* 2 char */
        goto print_usage;
    }

    if (argc != 3) { /* argc 3 */
        goto print_usage;
    }

    errno = 0;
    g_parameter = (td_s32)strtol(argv[1], &end_ptr, base);
    if ((errno != 0) || (*end_ptr != '\0')) {
        goto print_usage;
    }

    errno = 0;
    g_chn = (td_s32)strtol(argv[2], &end_ptr, base); /* 2: chn id */
    if ((errno != 0) || (*end_ptr != '\0')) {
        goto print_usage;
    }

#ifdef CONFIG_OT_VI_ALL_SUPPORT
    if ((g_parameter < 0) || (g_parameter >= OT_VI_MAX_PIPE_NUM)) {
        printf("Not Correct vi pipe index\n");
        return TD_FAILURE;
    }
    if ((g_chn < 0) || (g_chn >= OT_VI_MAX_PHYS_CHN_NUM))    {
        printf("Not Correct Physic Channel\n");
        return TD_FAILURE;
    }
#else
    if ((g_parameter < 0) || (g_parameter >= OT_VPSS_MAX_GRP_NUM)) {
        printf("Not Correct vpss grp index\n");
        return TD_FAILURE;
    }
    if ((g_chn < 0) || (g_chn >= OT_VPSS_MAX_PHYS_CHN_NUM))    {
        printf("Not Correct Physic Channel\n");
        return TD_FAILURE;
    }
#endif
    return TD_SUCCESS;

print_usage:
    usage();
    return TD_FAILURE;
}

static td_s32 fisheye_calibrate_recover_chn_attr(chn_attr_type *chn_attr)
{
    td_s32 ret;

    if (g_orig_depth != -1U) {
        chn_attr->depth = g_orig_depth;
#ifdef CONFIG_OT_VI_ALL_SUPPORT
        ret = ss_mpi_vi_set_chn_attr(g_parameter, g_chn, chn_attr);
#else
        chn_attr->chn_mode = g_vpss_chn_mode;
        ret = ss_mpi_vpss_set_chn_attr(g_parameter, g_chn, chn_attr);
#endif
        if (ret != TD_SUCCESS) {
            printf("set chn attr error!\n");
            return TD_FAILURE;
        }
    }
    return TD_SUCCESS;
}

static td_s32 fisheye_calibrate_set_chn_attr(chn_attr_type *chn_attr)
{
    td_s32 ret;

#ifdef CONFIG_OT_VI_ALL_SUPPORT
    ret = ss_mpi_vi_get_chn_attr(g_parameter, g_chn, chn_attr);
#else
    ret = ss_mpi_vpss_get_chn_attr(g_parameter, g_chn, chn_attr);
#endif
    if (ret != TD_SUCCESS) {
        printf("get chn attr error!!!\n");
        return TD_FAILURE;
    }

    g_orig_depth = chn_attr->depth;
    chn_attr->depth = 1;

#ifdef CONFIG_OT_VI_ALL_SUPPORT
    ret = ss_mpi_vi_set_chn_attr(g_parameter, g_chn, chn_attr);
#else
    g_vpss_chn_mode = chn_attr->chn_mode;
    chn_attr->chn_mode = OT_VPSS_CHN_MODE_USER;
    ret = ss_mpi_vpss_set_chn_attr(g_parameter, g_chn, chn_attr);
#endif
    if (ret != TD_SUCCESS) {
        printf("set chn attr error!!!\n");
        return TD_FAILURE;
    }

    /* switch off fisheye if enabled */
    ret = fisheye_calibrate_get_chn_fisheye(&g_correction_attr);
    if (ret != TD_SUCCESS) {
        printf("get fisheye attr failed with ret:0x%x!\n", ret);
        return ret;
    }
    g_fisheye_orig_en = g_correction_attr.enable;
    if (g_fisheye_orig_en == TD_TRUE) {
        g_correction_attr.enable = TD_FALSE;
        ret = fisheye_calibrate_set_chn_fisheye(&g_correction_attr);
        if (ret != TD_SUCCESS) {
            printf("set fisheye attr failed with ret:0x%x!\n", ret);
            return ret;
        }
    }

    usleep(900000); /* 900000us */

    if (g_quit == TD_TRUE) {
        (td_void)fisheye_calibrate_recover_chn_attr(chn_attr);
        return TD_FAILURE;
    }

    return ret;
}

static td_s32 fisheye_calibrate_vb_create_pool(const ot_video_frame_info *frame_in,
    td_u32 *blk_size, ot_vb_pool *h_pool)
{
    ot_pic_buf_attr pic_buf_attr;
    ot_vb_pool_cfg vb_pool_cfg;

    if ((frame_in->video_frame.compress_mode != OT_COMPRESS_MODE_NONE) ||
        (frame_in->video_frame.dynamic_range != OT_DYNAMIC_RANGE_SDR8) ||
        (frame_in->video_frame.pixel_format != OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420)) {
        printf("compress_mode or dynamic_range or pixel_format not support!!! \n");
        return TD_FAILURE;
    }

    /* Save VI PIC For UserMode */
    pic_buf_attr.width = frame_in->video_frame.width;
    pic_buf_attr.height = frame_in->video_frame.height;
    pic_buf_attr.pixel_format = frame_in->video_frame.pixel_format;
    pic_buf_attr.bit_width = OT_DATA_BIT_WIDTH_8;
    pic_buf_attr.compress_mode = OT_COMPRESS_MODE_NONE;
    pic_buf_attr.align = 32; /* align 32 bytes */
    *blk_size = ot_common_get_pic_buf_size(&pic_buf_attr);
    (td_void)memset_s(&vb_pool_cfg, sizeof(ot_vb_pool_cfg), 0, sizeof(ot_vb_pool_cfg));
    vb_pool_cfg.blk_size = *blk_size;
    vb_pool_cfg.blk_cnt  = VB_BLK_CNT;
    vb_pool_cfg.remap_mode = OT_VB_REMAP_MODE_NONE;
    *h_pool = ss_mpi_vb_create_pool(&vb_pool_cfg);
    if (*h_pool == OT_VB_INVALID_POOL_ID) {
        printf("ss_mpi_vb_create_pool failed! \n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void fisheye_calibrate_result_process(const ot_video_frame_info *frame_in,
                                                const ot_video_frame_info *frame_out)
{
    td_s32 ret, i;
    ot_fisheye_calibration_output out_calibrate;

    if (g_quit == TD_TRUE) {
        return;
    }

    (td_void)memset_s(&out_calibrate, sizeof(ot_fisheye_calibration_output), 0, sizeof(ot_fisheye_calibration_output));

    printf("Compute Calibrate Result.....\n");
    ret = ss_mpi_fisheye_calibration_compute(&frame_in->video_frame, FISHEYE_CALIBRATE_LEVEL, &out_calibrate);
    if (ret != TD_SUCCESS) {
        printf("Mark Result Failed!\n");
        return;
    }

    if (g_quit == TD_TRUE) {
        return;
    }

    printf(" Radius_X=%d,\n Radius_Y=%d,\n Radius=%d,\n OffsetH=%d,\n OffsetV=%d. \n",
           out_calibrate.calibration_result.radius_x, out_calibrate.calibration_result.radius_y,
           out_calibrate.calibration_result.radius, out_calibrate.calibration_result.offset_h,
           out_calibrate.calibration_result.offset_v);

    printf("Mark Calibrate Result.....\n");
    ret = ss_mpi_fisheye_calibration_mark(&frame_in->video_frame, &frame_out->video_frame,
        &out_calibrate.calibration_result);
    if (ret != TD_SUCCESS) {
        printf("Mark Result Failed!\n");
        return;
    }

    /* switch on fisheye and apply fisheye calibrate result if enabled */
    if (g_fisheye_orig_en == TD_TRUE) {
        g_correction_attr.enable = TD_TRUE;
        g_correction_attr.fisheye_attr.hor_offset =
            fisheye_clip3(-511, 511, out_calibrate.calibration_result.offset_h); /* 511 : 2^9 - 1 */

        g_correction_attr.fisheye_attr.ver_offset =
            fisheye_clip3(-511, 511, out_calibrate.calibration_result.offset_v); /* 511 : 2^9 - 1 */

        for (i = 0; i < g_correction_attr.fisheye_attr.rgn_num; i++) {
            g_correction_attr.fisheye_attr.fisheye_rgn_attr[i].out_radius = out_calibrate.calibration_result.radius;
        }

        ret = fisheye_calibrate_set_chn_fisheye(&g_correction_attr);
        if (ret != TD_SUCCESS) {
            printf("Set Fisheye Attr Failed!\n");
            return;
        }
    }
}

static td_s32 fisheye_calibrate_open_file(const ot_video_frame_info *frame_in)
{
    td_s32 ret;
    td_char yuv_name[FILE_NAME_LENGTH] = {0};
    td_char *pix_frm = TD_NULL;

    if (frame_in == NULL) {
        printf("fisheye_calibrate_open_file null ptr err\n");
        return TD_FAILURE;
    }

    pix_frm = (OT_PIXEL_FORMAT_YUV_400 == frame_in->video_frame.pixel_format) ? "single" :
              ((frame_in->video_frame.pixel_format == OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ? "p420" : "p422");

    ret = snprintf_s(yuv_name, sizeof(yuv_name), sizeof(yuv_name) - 1,
        "./fisheye_calibrate_out_%d_%d_%d_%s.yuv",
        g_chn, frame_in->video_frame.width, frame_in->video_frame.height, pix_frm);
    if (ret == -1) {
        return TD_FAILURE;
    }

    g_fd_out = fopen(yuv_name, "wb");
    if (g_fd_out == TD_NULL) {
        printf("open file %s err\n", yuv_name);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_s32 fisheye_calibrate_get_chn_frame(ot_video_frame_info *frame_in)
{
#ifdef CONFIG_OT_VI_ALL_SUPPORT
    if (ss_mpi_vi_get_chn_frame(g_parameter, g_chn, frame_in, 2000) != TD_SUCCESS) { /* 2000 milli_sec */
        return TD_FAILURE;
    }
#else
    if (ss_mpi_vpss_get_chn_frame(g_parameter, g_chn, frame_in, 2000) != TD_SUCCESS) { /* 2000 milli_sec */
        return TD_FAILURE;
    }
#endif
    return TD_SUCCESS;
}

static td_s32 fisheye_calibrate_release_chn_frame(const ot_video_frame_info *frame_in)
{
#ifdef CONFIG_OT_VI_ALL_SUPPORT
    if (ss_mpi_vi_release_chn_frame(g_parameter, g_chn, frame_in) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#else
    if (ss_mpi_vpss_release_chn_frame(g_parameter, g_chn, frame_in) != TD_SUCCESS) {
        return TD_FAILURE;
    }
#endif
    return TD_SUCCESS;
}

#ifdef __LITEOS__
td_s32 fisheye_calibrate(int argc, char *argv[])
#else
td_s32 main(int argc, char *argv[])
#endif
{
    ot_vb_pool h_pool;
    td_u32 blk_size;
    ot_vb_blk vb_blk = OT_VB_INVALID_HANDLE;
    ot_video_frame_info frame_in = { 0 };
    ot_video_frame_info frame_out;

    chn_attr_type chn_attr;

    if (fisheye_calibrate_check_and_set(argc, argv) != TD_SUCCESS) {
        goto end0;
    }

    if (fisheye_calibrate_set_chn_attr(&chn_attr) != TD_SUCCESS) {
        goto end0;
    }

    if (fisheye_calibrate_get_chn_frame(&frame_in) != TD_SUCCESS) {
        printf("fisheye calibrate get chn frame err\n");
        goto end1;
    }

    if (fisheye_calibrate_open_file(&frame_in) != TD_SUCCESS) {
        goto end2;
    }

    if (fisheye_calibrate_vb_create_pool(&frame_in, &blk_size, &h_pool) != TD_SUCCESS) {
        goto end3;
    }

    (td_void)memcpy_s(&frame_out, sizeof(ot_video_frame_info), &frame_in, sizeof(ot_video_frame_info));
    if (fisheye_calibrate_misc_get_vb(&frame_out, &frame_in, &vb_blk, h_pool) != TD_SUCCESS) {
        goto end4;
    }

    if (fisheye_calibrate_map_virt_addr(&frame_in) != TD_SUCCESS) {
        goto end5;
    }

    fisheye_calibrate_result_process(&frame_in, &frame_out);
    fisheye_calibrate_save_sp42x_to_planar(&frame_out.video_frame);
    printf("Calibrate Finished.....\n");

end5:
    ss_mpi_sys_munmap((td_void*)frame_out.video_frame.virt_addr[0], blk_size);
    ss_mpi_vb_release_blk(vb_blk);
end4:
    ss_mpi_vb_destroy_pool(h_pool);
end3:
    safe_close_file();
end2:
    fisheye_calibrate_release_chn_frame(&frame_in);
end1:
    fisheye_calibrate_recover_chn_attr(&chn_attr);
end0:
    return TD_SUCCESS;
}
