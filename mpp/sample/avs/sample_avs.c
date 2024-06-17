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
#include <sys/prctl.h>

#include "ss_mpi_avs.h"
#include "sample_comm.h"
#include "ss_mpi_avs_lut_generate.h"
#include "ss_mpi_avs_pos_query.h"
#include "ss_mpi_avs_conversion.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static td_phys_addr_t  g_avs_lut_addr = 0;
static td_phys_addr_t  g_avs_mask_addr = 0;
static td_phys_addr_t  g_avs_convert_in_addr = 0;
static td_phys_addr_t  g_avs_convert_out_addr = 0;
static td_u64 g_mesh_addr = 0;

static td_bool   g_avs_go;
static pthread_t g_avs_thread_id;
static sample_vi_cfg g_avs_vi_cfg[OT_VI_MAX_DEV_NUM];
static sample_vo_cfg g_avs_vo_config;
static td_char g_cal_file[OT_AVS_CALIBRATION_FILE_LEN];

#define LUT_NAME_MAX_LEN 64
#define FILE_MAX_LEN 128

static volatile sig_atomic_t g_avs_sig_flag = 0;

#define AVS_IN_W 2688
#define AVS_IN_H 1520

#define AVS_OUT_W 6080
#define AVS_OUT_H 2800

static td_void sample_avs_get_char(td_void)
{
    if (g_avs_sig_flag == 1) {
        return;
    }

    sample_pause();
}

typedef struct {
    td_u32            out_w;
    td_u32            out_h;
    ot_avs_grp_attr   grp_attr;
    ot_compress_mode  out_cmp_mode;
    td_bool           chn1_en;
    td_char           lut_name[OT_AVS_PIPE_NUM][LUT_NAME_MAX_LEN];
} sample_avs_cfg;

typedef struct {
    ot_size dst_size;  /* the output stitch image size */
    ot_size cell_size; /* the cell size of each block in mesh lut */
    ot_size mesh_size; /* the mesh vertices number */
    td_bool mesh_normalized; /* mesh is normalized or not (prevent reentry) */
    td_u32 cam_num;
} avs_convert_config;

static sample_comm_venc_chn_param g_avs_venc_chn_param = {
    .frame_rate           = 30, /* 30 framerate */
    .stats_time           = 1,  /* 1 stats time */
    .gop                  = 30, /* 30 gop */
    .venc_size            = {6080, 2800},
    .size                 = PIC_6080X2800,
    .profile              = 0,
    .is_rcn_ref_share_buf = TD_FALSE,
    .gop_attr             = {
        .gop_mode = OT_VENC_GOP_MODE_NORMAL_P,
        .normal_p = {2},
    },
    .type                 = OT_PT_H265,
    .rc_mode              = SAMPLE_RC_CBR,
};

static td_s32 sample_avs_start_venc(td_bool is_blend)
{
    td_s32 ret;
    const td_u32 chn_num = 1;
    ot_venc_chn venc_chn[1] = {0};

    if (is_blend == TD_TRUE) {
        g_avs_venc_chn_param.venc_size.width  = AVS_OUT_W;
        g_avs_venc_chn_param.venc_size.height = AVS_OUT_H;
        g_avs_venc_chn_param.size = PIC_6080X2800;
    } else {
        g_avs_venc_chn_param.venc_size.width  = 3840; /* 3840 noblend out width */
        g_avs_venc_chn_param.venc_size.height = 2160; /* 2160 noblend out height */
        g_avs_venc_chn_param.size = PIC_3840X2160;
    }

    ret = sample_comm_venc_start(venc_chn[0], &g_avs_venc_chn_param);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    ret = sample_comm_venc_start_get_stream(venc_chn, chn_num);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    return TD_SUCCESS;

exit:
    sample_comm_venc_stop(venc_chn[0]);
    return TD_FAILURE;
}

static td_void sample_avs_stop_venc(td_void)
{
    const td_u32 chn_num = 1;
    ot_venc_chn venc_chn[1] = {0};

    sample_comm_venc_stop_get_stream(chn_num);
    sample_comm_venc_stop(venc_chn[0]);
}

static td_u32 sample_avs_get_file_len(const td_char *file)
{
    FILE *fd;
    td_u32 file_len;
    td_s32 ret;

    fd = fopen(file, "rb");
    if (fd != NULL) {
        ret = fseek(fd, 0L, SEEK_END);
        if (ret != 0) {
            sample_print("fseek err!\n");
            fclose(fd);
            return 0;
        }

        file_len = ftell(fd);

        ret = fseek(fd, 0L, SEEK_SET);
        if (ret != 0) {
            sample_print("fseek err!\n");
            fclose(fd);
            return 0;
        }

        fclose(fd);
    } else {
        sample_print("open file %s fail!\n", file);
        file_len = 0;
    }

    return file_len;
}

static td_s32 sample_avs_load_file(const td_char* file, td_char *addr, td_u32 size)
{
    FILE *fd;
    td_u32 read_bytes;

    fd = fopen(file, "rb");
    if (fd != NULL) {
        read_bytes = fread(addr, size, 1, fd);
        if (read_bytes != 1) {
            sample_print("read file error!\n");
            fclose(fd);
            return TD_FAILURE;
        }
        fclose(fd);
    } else {
        sample_print("Error: Open file of %s failed!\n", file);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}


static td_s32 sample_avs_bind(td_u32 pipe_num, td_bool chn1_en, td_bool is_blend)
{
    const ot_avs_grp  avs_grp = 0;
    ot_avs_pipe avs_pipe;
    ot_avs_chn  avs_chn = 0;
    ot_vi_pipe  vi_pipe;
    const ot_vi_chn vi_chn = 0;
    td_s32 ret;
    ot_vpss_grp vpss_grp;
    ot_vpss_chn vpss_chn = 1;
    const ot_venc_chn venc_chn = 0;
    const ot_vo_layer vo_layer = 0;
    const ot_vo_chn   vo_chn = 0;
    td_u32      i;

    if (is_blend == TD_FALSE) {
        vpss_chn = 0;
    }

    for (i = 0; i < pipe_num; i++) {
        vi_pipe = i;
        vpss_grp = i;
        ret = sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
        if (ret != TD_SUCCESS) {
            sample_print("vi bind vpss fail with %#x", ret);
            return TD_FAILURE;
        }
    }

    for (i = 0; i < pipe_num; i++) {
        avs_pipe = i;
        vpss_grp = i;

        ret = sample_comm_vpss_bind_avs(vpss_grp, vpss_chn, avs_grp, avs_pipe);
        if (ret != TD_SUCCESS) {
            sample_print("vpss bind avs fail with %#x!\n", ret);
            return TD_FAILURE;
        }
    }

    ret = sample_comm_avs_bind_venc(avs_grp, avs_chn, venc_chn);
    if (ret != TD_SUCCESS) {
        sample_print("avs bind venc fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    if (chn1_en) {
        avs_chn = 1;
    } else {
        avs_chn = 0;
    }

    ret = sample_comm_avs_bind_vo(avs_grp, avs_chn, vo_layer, vo_chn);
    if (ret != TD_SUCCESS) {
        sample_print("avs bind vo fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}


static td_void sample_avs_un_bind(td_u32 pipe_num, td_bool chn1_en, td_bool is_blend)
{
    const ot_avs_grp  avs_grp  = 0;
    ot_avs_pipe avs_pipe;
    ot_avs_chn  avs_chn  = 0;
    ot_vi_pipe  vi_pipe;
    const ot_vi_chn vi_chn   = 0;
    td_s32      ret;
    const ot_vo_layer vo_layer = 0;
    const ot_vo_chn   vo_chn   = 0;
    td_u32   i;
    ot_vpss_grp vpss_grp;
    ot_vpss_chn vpss_chn = 1;
    const ot_venc_chn venc_chn = 0;

    if (is_blend == TD_FALSE) {
        vpss_chn = 0;
    }

    ret = sample_comm_avs_un_bind_venc(avs_grp, avs_chn, venc_chn);
    if (ret != TD_SUCCESS) {
        sample_print("avs unbind venc fail with %#x", ret);
        return;
    }

    if (chn1_en) {
        avs_chn = 1;
    } else {
        avs_chn = 0;
    }

    ret = sample_comm_avs_un_bind_vo(avs_grp, avs_chn, vo_layer, vo_chn);
    if (ret != TD_SUCCESS) {
        sample_print("avs unbind vo fail with %#x", ret);
        return;
    }

    for (i = 0; i < pipe_num; i++) {
        avs_pipe = i;
        vpss_grp = i;
        ret = sample_comm_vpss_un_bind_avs(vpss_grp, vpss_chn, avs_grp, avs_pipe);
        if (ret != TD_SUCCESS) {
            sample_print("VPSS unbind AVS fail with %#x", ret);
            return;
        }
    }

    for (i = 0; i < pipe_num; i++) {
        vi_pipe = i;
        vpss_grp = i;

        ret = sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp, 0);
        if (ret != TD_SUCCESS) {
            sample_print("VI unbind VPSS fail with %#x", ret);
            return;
        }
    }

    return;
}

static td_void sample_avs_get_avs_out_vb_config(ot_vb_cfg *vb_cfg, td_u32 case_index)
{
    ot_pic_buf_attr buf_attr;

    buf_attr.width                  = AVS_OUT_W;
    buf_attr.height                 = AVS_OUT_H;
    buf_attr.align                  = OT_DEFAULT_ALIGN;
    buf_attr.bit_width              = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format           = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    buf_attr.compress_mode          = OT_COMPRESS_MODE_SEG;

    switch (case_index) {
        case 0:
            vb_cfg->common_pool[2].blk_size = ot_common_get_pic_buf_size(&buf_attr); /* 2 index */
            vb_cfg->common_pool[2].blk_cnt  = 3; /* 3 buf cnt */
            buf_attr.width                  = 1920; /* 1920 chn1 */
            buf_attr.height                 = 1080; /* 1080 chn1 */
            buf_attr.compress_mode          = OT_COMPRESS_MODE_NONE;
            vb_cfg->common_pool[3].blk_size = ot_common_get_pic_buf_size(&buf_attr); /* 3 index */
            vb_cfg->common_pool[3].blk_cnt  = 3; /* 3 buf cnt */
            break;
        case 1:
            buf_attr.compress_mode          = OT_COMPRESS_MODE_NONE;
            vb_cfg->common_pool[2].blk_size = ot_common_get_pic_buf_size(&buf_attr); /* 2 index */
            vb_cfg->common_pool[2].blk_cnt  = 4; /* index 4 buf cnt */
            break;
        case 2: /* 2 index */
            buf_attr.width                  = AVS_IN_W * 2; /* output 2 */
            buf_attr.height                 = AVS_IN_H * 2; /* output 2 */
            buf_attr.compress_mode          = OT_COMPRESS_MODE_SEG;
            vb_cfg->common_pool[2].blk_size = ot_common_get_pic_buf_size(&buf_attr); /* 2 index */
            vb_cfg->common_pool[2].blk_cnt  = 4; /* 2 index 4 buf cnt */
            break;
        default:
            break;
    }
}

static td_void sample_avs_get_default_vb_config(ot_vb_cfg *vb_cfg, td_u32 case_index)
{
    ot_pic_buf_attr buf_attr;

    if (memset_s(vb_cfg, sizeof(ot_vb_cfg), 0, sizeof(ot_vb_cfg)) != EOK) {
        sample_print("memset_s fail!\n");
        return;
    }
    vb_cfg->max_pool_cnt = 128; /* max pool 128 */

    buf_attr.width         = AVS_IN_W;
    buf_attr.height        = AVS_IN_H;
    buf_attr.align         = OT_DEFAULT_ALIGN;
    buf_attr.bit_width     = OT_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
    buf_attr.compress_mode = OT_COMPRESS_MODE_LINE;
    vb_cfg->common_pool[0].blk_size = ot_common_get_pic_buf_size(&buf_attr);
    vb_cfg->common_pool[0].blk_cnt  = 12; /* 12 raw buf cnt */

    vb_cfg->common_pool[1].blk_size = ot_avs_get_buf_size(AVS_IN_W, AVS_IN_H);
    vb_cfg->common_pool[1].blk_cnt  = 12; /* 12 buf cnt */

    sample_avs_get_avs_out_vb_config(vb_cfg, case_index);
}

static td_s32 sample_avs_set_sys_cfg(ot_vi_vpss_mode_type mode_type, ot_vi_video_mode video_mode, td_u32 case_index)
{
    td_s32 ret;
    ot_vb_cfg vb_cfg;
    td_u32 supplement_config;

    sample_avs_get_default_vb_config(&vb_cfg, case_index);

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

static td_s32 sample_avs_create_grp(sample_avs_cfg *avs_cfg)
{
    td_u32 lut_size;
    ot_avs_grp_attr avs_grp_attr = {0};
    td_void *lut_virt_addr = NULL;
    td_u32 pipe_num;
    td_u32 i;
    td_s32 ret;
    const ot_avs_grp avs_grp = 0;

    (td_void)memcpy_s(&avs_grp_attr, sizeof(ot_avs_grp_attr), &avs_cfg->grp_attr, sizeof(ot_avs_grp_attr));

    pipe_num = avs_grp_attr.pipe_num;

    if (avs_grp_attr.mode == OT_AVS_MODE_BLEND) {
        lut_size = sample_avs_get_file_len(avs_cfg->lut_name[0]);
        if (lut_size == 0) {
            sample_print("Open lut file fail!\n");
            return TD_FAILURE;
        }

        lut_size = OT_ALIGN_UP(lut_size, 256); /* lut 256 align */

        ret = ss_mpi_sys_mmz_alloc(&(g_avs_lut_addr), &(lut_virt_addr), "avs_lut", NULL, lut_size * pipe_num);
        if (ret != TD_SUCCESS) {
            sample_print("alloc LUT buf fail with %#x!\n", ret);
            return TD_FAILURE;
        }

        for (i = 0; i < pipe_num; i++) {
            ret = sample_avs_load_file(avs_cfg->lut_name[i], ((td_char*)lut_virt_addr + lut_size * i), lut_size);
            if (ret != TD_SUCCESS) {
                return TD_FAILURE;
            }
            avs_grp_attr.lut.phys_addr[i] = g_avs_lut_addr + lut_size * i;
        }
    }

    avs_grp_attr.sync_pipe_en                = TD_TRUE;
    avs_grp_attr.frame_rate.src_frame_rate   = -1;
    avs_grp_attr.frame_rate.dst_frame_rate   = -1;
    avs_grp_attr.lut.accuracy                = OT_AVS_LUT_ACCURACY_HIGH;

    ret = ss_mpi_avs_create_grp(avs_grp, &avs_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("Creat grp failed with %#x!\n", ret);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_s32 sample_avs_start_avs_chn(sample_avs_cfg *avs_cfg)
{
    ot_avs_chn      avs_chn = 0;
    ot_avs_chn_attr chn_attr = {0};
    const ot_avs_grp avs_grp = 0;
    td_s32          ret;

    chn_attr.compress_mode = avs_cfg->out_cmp_mode;
    chn_attr.depth         = 0;
    chn_attr.width         = avs_cfg->out_w;
    chn_attr.height        = avs_cfg->out_h;
    chn_attr.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
    ret = ss_mpi_avs_set_chn_attr(avs_grp, avs_chn, &chn_attr);
    if (ret != TD_SUCCESS) {
        sample_print("set chn attr failed with %#x!\n", ret);
        return TD_FAILURE;
    }

    ret = ss_mpi_avs_enable_chn(avs_grp, avs_chn);
    if (ret != TD_SUCCESS) {
        sample_print("avs enable chn failed with %#x!\n", ret);
        return TD_FAILURE;
    }

    if (avs_cfg->chn1_en) {
        avs_chn = 1;
        chn_attr.width  = 1920; /* 1920 out width */
        chn_attr.height = 1080; /* 1080 out height */
        chn_attr.compress_mode = OT_COMPRESS_MODE_NONE;
        ret = ss_mpi_avs_set_chn_attr(avs_grp, avs_chn, &chn_attr);
        if (ret != TD_SUCCESS) {
            sample_print("Set chnattr failed with %#x!\n", ret);
            return TD_FAILURE;
        }

        ret = ss_mpi_avs_enable_chn(avs_grp, avs_chn);
        if (ret != TD_SUCCESS) {
            sample_print("AVS enable chn failed with %#x!\n", ret);
            return TD_FAILURE;
        }
    }
    return TD_SUCCESS;
}

static td_s32 sample_avs_start_avs(sample_avs_cfg *avs_cfg)
{
    td_s32 ret;
    const ot_avs_grp avs_grp = 0;

    ret = sample_avs_create_grp(avs_cfg);
    if (ret != TD_SUCCESS) {
        sample_print("creat grp failed with %#x!\n", ret);
        goto exit;
    }

    ret = sample_avs_start_avs_chn(avs_cfg);
    if (ret != TD_SUCCESS) {
        sample_print("start chn failed with %#x!\n", ret);
        goto exit;
    }

    ret = ss_mpi_avs_start_grp(avs_grp);
    if (ret != TD_SUCCESS) {
        sample_print("AVS start grp failed with %#x!\n", ret);
        goto exit;
    }

    return TD_SUCCESS;

exit:
    if (g_avs_lut_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_lut_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("mmz_free fail!\n");
        }
        g_avs_lut_addr = 0;
    }
    return TD_FAILURE;
}


static td_void sample_avs_stop_avs(td_bool chn1_en)
{
    ot_avs_chn avs_chn;
    td_s32     ret;
    const ot_avs_grp avs_grp = 0;

    ret = ss_mpi_avs_stop_grp(avs_grp);
    if (ret != TD_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return;
    }

    if (chn1_en) {
        avs_chn = 1;
        ret = ss_mpi_avs_disable_chn(avs_grp, avs_chn);
        if (ret != TD_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return;
        }
    }

    avs_chn = 0;
    ret = ss_mpi_avs_disable_chn(avs_grp, avs_chn);
    if (ret != TD_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return;
    }

    ret = ss_mpi_avs_destroy_grp(avs_grp);
    if (ret != TD_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return;
    }

    if (g_avs_lut_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_lut_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return;
        }
        g_avs_lut_addr = 0;
    }

    return;
}

static td_void* sample_avs_set_grp_attr_thread(td_void *arg)
{
    td_s32          ret;
    const ot_avs_grp avs_grp = 0;
    ot_avs_grp_attr avs_grp_attr;
    const td_u32    step = 1;

    ot_unused(arg);

    prctl(PR_SET_NAME, "avs_cruise", 0, 0, 0);

    printf("\nstart to cruise......\n\n");

    while (g_avs_go == TD_FALSE) {
        ret = ss_mpi_avs_get_grp_attr(avs_grp, &avs_grp_attr);
        if (ret != TD_SUCCESS) {
            sample_print("get grp attr failed with %#x!\n", ret);
            return TD_NULL;
        }

        avs_grp_attr.out_attr.rotation.yaw += step;
        if (avs_grp_attr.out_attr.rotation.yaw > 18000) {       /* 18000 max yaw */
            avs_grp_attr.out_attr.rotation.yaw = -18000 + step; /* -18000 min yaw */
        }

        ret = ss_mpi_avs_set_grp_attr(avs_grp, &avs_grp_attr);
        if (ret != TD_SUCCESS) {
            sample_print("set grp attr failed with %#x!\n", ret);
            return TD_NULL;
        }

        usleep(10 * 1000); /* 10 * 1000us sleep */
    }

    return TD_NULL;
}

static td_void sample_avs_start_set_grp_attr_thrd(td_void)
{
    g_avs_go = TD_FALSE;
    pthread_create(&g_avs_thread_id, TD_NULL, sample_avs_set_grp_attr_thread, NULL);
    sleep(1);
    return;
}

static td_void sample_avs_stop_set_grp_attr_thrd(td_void)
{
    if (g_avs_go == TD_FALSE) {
        g_avs_go = TD_TRUE;
        pthread_join(g_avs_thread_id, TD_NULL);
    }
    return;
}

static td_s32 sampel_avs_start_4_eq_avs(td_void)
{
    ot_avs_mod_param mod_param;
    td_s32           ret;
    const td_u32     pipe_num = 4;
    td_s32           i;
    sample_avs_cfg   avs_cfg = {0};
    ot_avs_grp_attr *avs_grp_attr = NULL;

    mod_param.working_set_size = 247 * 1024; /* 247 * 1024 is workingset size */
    ret = ss_mpi_avs_set_mod_param(&mod_param);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_avs_set_mod_param fail for %#x!\n", ret);
        return TD_FAILURE;
    }

    for (i = 0; i < (td_s32)pipe_num; i++) {
        if (snprintf_s(avs_cfg.lut_name[i], LUT_NAME_MAX_LEN,
            LUT_NAME_MAX_LEN - 1, "./data/avsp_mesh_out_%d.bin", i + 1) == -1) {
            sample_print("set file name fail!\n");
            return TD_FAILURE;
        }
    }

    avs_cfg.out_w        = AVS_OUT_W;
    avs_cfg.out_h        = AVS_OUT_H;
    avs_cfg.out_cmp_mode = OT_COMPRESS_MODE_SEG;
    avs_cfg.chn1_en      = TD_TRUE;

    avs_grp_attr                  = &avs_cfg.grp_attr;
    avs_grp_attr->mode            = OT_AVS_MODE_BLEND;
    avs_grp_attr->pipe_num        = pipe_num;
    avs_grp_attr->gain_attr.mode  = OT_AVS_GAIN_MODE_AUTO;

    avs_grp_attr->out_attr.projection_mode     = OT_AVS_PROJECTION_EQUIRECTANGULAR;
    avs_grp_attr->out_attr.center.x            = AVS_OUT_W / 2; /* 2 half of out_w */
    avs_grp_attr->out_attr.center.y            = 560;       /* 560 center */
    avs_grp_attr->out_attr.fov.fov_x           = 18000;     /* 18000 fov */
    avs_grp_attr->out_attr.fov.fov_y           = 8000;      /* 8000 fov */
    avs_grp_attr->out_attr.orig_rotation.roll  = 0;
    avs_grp_attr->out_attr.orig_rotation.pitch = 0;
    avs_grp_attr->out_attr.orig_rotation.yaw   = 0;
    avs_grp_attr->out_attr.rotation.roll       = 0;
    avs_grp_attr->out_attr.rotation.pitch      = 0;
    avs_grp_attr->out_attr.rotation.yaw        = 0;
    return sample_avs_start_avs(&avs_cfg);
}

static td_s32 sample_avs_swith_projection_mode(td_void)
{
    td_s32 ret;
    ot_avs_grp_attr grp_attr;
    const ot_avs_grp avs_grp = 0;

    ret = ss_mpi_avs_get_grp_attr(avs_grp, &grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("get avs grp attr fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    grp_attr.out_attr.fov.fov_x = 36000;  /* 36000 fov */
    grp_attr.out_attr.fov.fov_y = 11200;  /* 11200 fov */
    grp_attr.out_attr.projection_mode = OT_AVS_PROJECTION_CYLINDRICAL;

    ret = ss_mpi_avs_set_grp_attr(avs_grp, &grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("get avs grp attr fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    sample_avs_get_char();
    printf("Switch to rectilinear projection mode!\n");

    ret = ss_mpi_avs_get_grp_attr(avs_grp, &grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("get avs grp attr fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    grp_attr.out_attr.fov.fov_x = 11200;  /* 11200 fov */
    grp_attr.out_attr.fov.fov_y = 11200;  /* 11200 fov */
    grp_attr.out_attr.projection_mode = OT_AVS_PROJECTION_RECTILINEAR;
    ret = ss_mpi_avs_set_grp_attr(avs_grp, &grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("set avs grp attr fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    sample_avs_start_set_grp_attr_thrd();

    sample_avs_get_char();

    sample_avs_stop_set_grp_attr_thrd();
    return TD_SUCCESS;
}

static td_void sample_avs_get_4_vi_cfg(sample_sns_type sns_type, sample_vi_cfg *vi_cfg, td_s32 dev_num)
{
    td_s32 i;

    for (i = 0; i < dev_num; i++) {
        sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg[i]);
        sample_comm_vi_get_mipi_info_by_dev_id(sns_type, i, &vi_cfg[i].mipi_info);
        vi_cfg[i].mipi_info.divide_mode = LANE_DIVIDE_MODE_3;

        vi_cfg[i].dev_info.vi_dev = i;
        vi_cfg[i].bind_pipe.pipe_id[0] = i;
        vi_cfg[i].grp_info.grp_num = 1;
        vi_cfg[i].grp_info.fusion_grp[0] = i;
        vi_cfg[i].grp_info.fusion_grp_attr[0].pipe_id[0] = i;

        vi_cfg[i].sns_info.sns_clk_src = i;
        vi_cfg[i].sns_info.sns_rst_src = i;
    }

    vi_cfg[0].sns_info.bus_id = 2; /* arg0: i2c5 */
    vi_cfg[1].sns_info.bus_id = 3; /* arg1: i2c3 */
    vi_cfg[2].sns_info.bus_id = 5; /* arg2: i2c5 */
    vi_cfg[3].sns_info.bus_id = 4; /* arg3: i2c4 */
}

static td_void sample_avs_get_vpss_cfg(ot_vpss_grp_attr *vpss_grp_attr, td_bool chn_enable[],
    ot_vpss_chn_attr chn_attr[], td_s32 chn_num, td_bool blend_en)
{
    sample_comm_vpss_get_default_grp_attr(vpss_grp_attr);

    if (blend_en == TD_TRUE) {
        if (chn_num < 1) {
            return;
        }
        sample_comm_vpss_get_default_chn_attr(&chn_attr[1]);
        chn_attr[1].width = AVS_IN_W;
        chn_attr[1].height = AVS_IN_H;
        chn_attr[1].compress_mode = OT_COMPRESS_MODE_TILE;
        chn_attr[1].video_format = OT_VIDEO_FORMAT_TILE_16x8;
        chn_enable[1] = 1;
    } else {
        sample_comm_vpss_get_default_chn_attr(&chn_attr[0]);
        chn_enable[0] = 1;
        chn_attr[0].width  = 1920; /* 1920 width */
        chn_attr[0].height = 1080; /* 1080 height */
    }
}

static td_s32 sample_avs_enable_vi_stitch_grp(td_u32 pipe_num)
{
    ot_vi_stitch_grp_attr stitch_grp_attr;
    const ot_vi_grp stitch_grp = 0;
    td_s32 ret;

    stitch_grp_attr.stitch_en   = TD_FALSE;
    stitch_grp_attr.cfg_mode    = OT_VI_STITCH_CFG_MODE_NORM;
    stitch_grp_attr.max_pts_gap = 10000; /* 10000: 10ms */
    stitch_grp_attr.pipe_num    = pipe_num;
    stitch_grp_attr.pipe_id[0]  = 0;
    stitch_grp_attr.pipe_id[1]  = 1;
    stitch_grp_attr.pipe_id[2]  = 2; /* pipe 2 */
    stitch_grp_attr.pipe_id[3]  = 3; /* pipe 3 */

    ret = ss_mpi_vi_set_stitch_grp_attr(stitch_grp, &stitch_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("disable stitch grp fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    stitch_grp_attr.stitch_en = TD_TRUE;
    ret = ss_mpi_vi_set_stitch_grp_attr(stitch_grp, &stitch_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("enable stitch grp fail!\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void sample_avs_disable_vi_stitch_grp(td_void)
{
    ot_vi_stitch_grp_attr stitch_grp_attr;
    const ot_vi_grp stitch_grp = 0;
    td_s32 ret;

    ret = ss_mpi_vi_get_stitch_grp_attr(stitch_grp, &stitch_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("get stitch grp fail!\n");
        return;
    }

    stitch_grp_attr.stitch_en = TD_FALSE;
    ret = ss_mpi_vi_set_stitch_grp_attr(stitch_grp, &stitch_grp_attr);
    if (ret != TD_SUCCESS) {
        sample_print("enable stitch grp fail!\n");
        return;
    }
}

static td_s32 sample_avs_start_vi_vpss(td_u32 pipe_num, td_bool blend_en, td_u32 case_index)
{
    td_s32           ret, i, j;
    ot_vpss_grp      vpss_grp;
    ot_vpss_grp_attr vpss_grp_attr;
    ot_vpss_chn_attr chn_attr[OT_VPSS_MAX_PHYS_CHN_NUM];
    td_bool          chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {0, 0, 0, 0};

    /* init sys */
    ret = sample_avs_set_sys_cfg(OT_VI_OFFLINE_VPSS_ONLINE, OT_VI_VIDEO_MODE_NORM, case_index);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* start vi */
    ret = sample_avs_enable_vi_stitch_grp(pipe_num);
    if (ret != TD_SUCCESS) {
        goto exit;
    }

    sample_avs_get_4_vi_cfg(SENSOR0_TYPE, g_avs_vi_cfg, OT_VI_MAX_DEV_NUM);

    for (i = 0; i < (td_s32)pipe_num; i++) {
        ret = sample_comm_vi_start_vi(&g_avs_vi_cfg[i]);
        if (ret != TD_SUCCESS) {
            goto exit1;
        }
    }

    /* start vpss */
    sample_avs_get_vpss_cfg(&vpss_grp_attr, chn_enable, chn_attr, OT_VPSS_MAX_PHYS_CHN_NUM, blend_en);

    for (i = 0; i < (td_s32)pipe_num; i++) {
        vpss_grp = i;
        ret = sample_common_vpss_start(vpss_grp, chn_enable, &vpss_grp_attr, chn_attr, OT_VPSS_MAX_PHYS_CHN_NUM);
        if (ret != TD_SUCCESS) {
            sample_print("start vpss fail for %#x!\n", ret);
            goto exit2;
        }
    }
    return TD_SUCCESS;

exit2:
    for (j = i - 1; j >= 0; j--) {
        vpss_grp = j;
        sample_common_vpss_stop(vpss_grp, chn_enable, OT_VPSS_MAX_PHYS_CHN_NUM);
    }
    i = 4; /* reset 4 */
exit1:
    for (j = i - 1; j >= 0; j--) {
        sample_comm_vi_stop_vi(&g_avs_vi_cfg[j]);
    }
    sample_avs_disable_vi_stitch_grp();
exit:
    sample_comm_sys_exit();
    return TD_FAILURE;
}

static td_void sample_avs_stop_vi_vpss(td_u32 pipe_num)
{
    ot_vpss_grp vpss_grp;
    td_u32 i;
    td_bool chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {0, 1, 0, 0};

    for (i = 0; i < pipe_num; i++) {
        vpss_grp = i;
        sample_common_vpss_stop(vpss_grp, chn_enable, OT_VPSS_MAX_PHYS_CHN_NUM);
    }

    sample_comm_vi_stop_four_vi(g_avs_vi_cfg, pipe_num);

    sample_avs_disable_vi_stitch_grp();

    sample_comm_sys_exit();
    return;
}

static td_s32 sample_avs_start_venc_vo(td_bool is_blend)
{
    td_s32 ret;

    /* start vo */
    sample_comm_vo_get_def_config(&g_avs_vo_config);
    ret = sample_comm_vo_start_vo(&g_avs_vo_config);
    if (ret != TD_SUCCESS) {
        sample_print("start vo failed with 0x%x!\n", ret);
        return TD_FAILURE;
    }

    /* start venc */
    ret = sample_avs_start_venc(is_blend);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_start_venc failed with %d\n", ret);
        sample_comm_vo_stop_vo(&g_avs_vo_config);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void sample_avs_stop_venc_vo()
{
    sample_avs_stop_venc();
    sample_comm_vo_stop_vo(&g_avs_vo_config);
    return;
}

static td_s32 sample_avs_4stitching_switch_projection_mode(void)
{
    td_s32   ret;
    const td_u32   pipe_num = 4;
    td_bool  chn1_en = TD_TRUE;

    /* start vi/vpss */
    ret = sample_avs_start_vi_vpss(pipe_num, TD_TRUE, 0);
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* start avs */
    ret = sampel_avs_start_4_eq_avs();
    if (ret != TD_SUCCESS) {
        sample_print("sampel_avs_start_4_eq_avs failed with %d\n", ret);
        goto exit;
    }

    /* start vo/venc */
    ret = sample_avs_start_venc_vo(TD_TRUE);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_start_venc_vo failed with %d\n", ret);
        goto exit1;
    }

    /* bind */
    ret = sample_avs_bind(pipe_num, chn1_en, TD_TRUE);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_bind failed with %d\n", ret);
        goto exit2;
    }

    sample_avs_get_char();

    ret = sample_avs_swith_projection_mode();
    if (ret != TD_SUCCESS) {
        sample_print("set avs grp attr fail with %#x!\n", ret);
    }

    sample_avs_un_bind(pipe_num, chn1_en, TD_TRUE);
exit2:
    sample_avs_stop_venc_vo();
exit1:
    sample_avs_stop_avs(chn1_en);
exit:
    sample_avs_stop_vi_vpss(pipe_num);
    return ret;
}

static td_void sample_avs_set_cube_map_attr(ot_avs_grp_attr *avs_grp_attr)
{
    /* the numbers behind is to set the start points of cube map */
    avs_grp_attr->out_attr.cube_map_attr.bg_color_en      = TD_TRUE;
    avs_grp_attr->out_attr.cube_map_attr.bg_color         = 0xFFFFFF;

    avs_grp_attr->out_attr.cube_map_attr.surface_len      = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[0].x = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[0].y = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[1].x = 0;
    avs_grp_attr->out_attr.cube_map_attr.start_point[1].y = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[2].x = 2700;
    avs_grp_attr->out_attr.cube_map_attr.start_point[2].y = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[3].x = 1800;
    avs_grp_attr->out_attr.cube_map_attr.start_point[3].y = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[4].x = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[4].y = 0;
    avs_grp_attr->out_attr.cube_map_attr.start_point[5].x = 900;
    avs_grp_attr->out_attr.cube_map_attr.start_point[5].y = 1800;
    return;
}

static td_s32 sampel_avs_start_4_cube_avs(td_void)
{
    ot_avs_mod_param mod_param;
    td_s32           ret;
    const td_u32     pipe_num = 4;
    td_s32           i;
    sample_avs_cfg   avs_cfg = {0};
    const td_u32     out_w = 6080;
    const td_u32     out_h = 2800;
    ot_avs_grp_attr *avs_grp_attr = TD_NULL;

    mod_param.working_set_size = 247 * 1024; /* 247 * 1024 is workingset size */
    ret = ss_mpi_avs_set_mod_param(&mod_param);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_avs_set_mod_param fail for %#x!\n", ret);
        return TD_FAILURE;
    }

    for (i = 0; i < (td_s32)pipe_num; i++) {
        if (snprintf_s(avs_cfg.lut_name[i], LUT_NAME_MAX_LEN,
            LUT_NAME_MAX_LEN - 1, "./data/avsp_mesh_out_%d.bin", i + 1) == -1) {
            sample_print("set file name fail!\n");
            return TD_FAILURE;
        }
    }

    avs_cfg.out_w        = out_w;
    avs_cfg.out_h        = out_h;
    avs_cfg.out_cmp_mode = OT_COMPRESS_MODE_NONE;
    avs_cfg.chn1_en      = TD_FALSE;

    avs_grp_attr                    = &avs_cfg.grp_attr;
    avs_grp_attr->mode              = OT_AVS_MODE_BLEND;
    avs_grp_attr->pipe_num          = pipe_num;
    avs_grp_attr->gain_attr.mode    = OT_AVS_GAIN_MODE_MANUAL;
    avs_grp_attr->gain_attr.coef[0] = 0x4000; /* 0x4000 gain value */
    avs_grp_attr->gain_attr.coef[1] = 0x4000;
    avs_grp_attr->gain_attr.coef[2] = 0x4000; /* 2 index */
    avs_grp_attr->gain_attr.coef[3] = 0x4000;  /* 3 index */

    avs_grp_attr->out_attr.projection_mode     = OT_AVS_PROJECTION_CUBE_MAP;
    avs_grp_attr->out_attr.center.x            = out_w / 2; /* 2 half of out_w */
    avs_grp_attr->out_attr.center.y            = 560;       /* 560 center y */
    avs_grp_attr->out_attr.fov.fov_x           = 18000;     /* 18000 fov */
    avs_grp_attr->out_attr.fov.fov_y           = 8000;      /* 8000 fov */
    avs_grp_attr->out_attr.orig_rotation.roll  = 0;
    avs_grp_attr->out_attr.orig_rotation.pitch = 0;
    avs_grp_attr->out_attr.orig_rotation.yaw   = 0;
    avs_grp_attr->out_attr.rotation.roll       = 0;
    avs_grp_attr->out_attr.rotation.pitch      = 0;
    avs_grp_attr->out_attr.rotation.yaw        = 0;

    sample_avs_set_cube_map_attr(avs_grp_attr);

    return sample_avs_start_avs(&avs_cfg);
}

static td_s32 sample_avs_4stitching_cube_map(void)
{
    td_s32   ret;
    const td_u32   pipe_num = 4;
    td_bool  chn1_en = TD_FALSE;

    /* start vi/vpss */
    ret = sample_avs_start_vi_vpss(pipe_num, TD_TRUE, 1); /* case 1 */
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* start avs */
    ret = sampel_avs_start_4_cube_avs();
    if (ret != TD_SUCCESS) {
        sample_print("sampel_avs_start_4_cube_avs failed with %d\n", ret);
        goto exit;
    }

    /* start vo/venc */
    ret = sample_avs_start_venc_vo(TD_TRUE);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_start_venc_vo failed with %d\n", ret);
        goto exit1;
    }

    /* bind */
    ret = sample_avs_bind(pipe_num, chn1_en, TD_TRUE);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_bind failed with %d\n", ret);
        goto exit2;
    }

    sample_avs_get_char();

    sample_avs_un_bind(pipe_num, chn1_en, TD_TRUE);
exit2:
    sample_avs_stop_venc_vo();
exit1:
    sample_avs_stop_avs(chn1_en);
exit:
    sample_avs_stop_vi_vpss(pipe_num);
    return ret;
}

static td_s32 sampel_avs_start_4no_blend(td_void)
{
    const td_u32     pipe_num = 4;
    sample_avs_cfg   avs_cfg = {0};
    ot_avs_grp_attr *avs_grp_attr = TD_NULL;

    avs_cfg.out_cmp_mode = OT_COMPRESS_MODE_SEG;
    avs_cfg.chn1_en      = TD_FALSE;

    avs_grp_attr           = &avs_cfg.grp_attr;
    avs_grp_attr->mode     = OT_AVS_MODE_NOBLEND_QR;
    avs_grp_attr->pipe_num = pipe_num;

    return sample_avs_start_avs(&avs_cfg);
}

static td_s32 sample_avs_4no_blend(void)
{
    td_s32   ret;
    const td_u32 pipe_num = 4;
    td_bool  chn1_en = TD_FALSE;

    /* start vi/vpss */
    ret = sample_avs_start_vi_vpss(pipe_num, TD_FALSE, 2);  /* case 2 */
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    /* start avs */
    ret = sampel_avs_start_4no_blend();
    if (ret != TD_SUCCESS) {
        sample_print("sampel_avs_start_4no_blend failed with %d\n", ret);
        goto exit;
    }

    /* start vo/venc */
    ret = sample_avs_start_venc_vo(TD_FALSE);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_start_venc_vo failed with %d\n", ret);
        goto exit1;
    }

    /* bind */
    ret = sample_avs_bind(pipe_num, chn1_en, TD_FALSE);
    if (ret != TD_SUCCESS) {
        sample_print("sample_avs_bind failed with %d\n", ret);
        goto exit2;
    }

    sample_avs_get_char();

    sample_avs_un_bind(pipe_num, chn1_en, TD_FALSE);
exit2:
    sample_avs_stop_venc_vo();
exit1:
    sample_avs_stop_avs(chn1_en);
exit:
    sample_avs_stop_vi_vpss(pipe_num);
    return ret;
}

static td_void sample_avs_lut_mask_cfg(ot_avs_lut_mask *lut_mask, td_u32 width, td_u32 height)
{
    lut_mask->input_yuv_mask_flag = TD_FALSE;
    lut_mask->same_mask_flag      = TD_FALSE;
    lut_mask->mask_define[0].mask_shape      = OT_AVS_MASK_SHAPE_RECT;
    lut_mask->mask_define[0].offset_h        = 0;
    lut_mask->mask_define[0].offset_v        = 0;
    lut_mask->mask_define[0].half_major_axis = width / 2;   /* half 2 */
    lut_mask->mask_define[0].half_minor_axis = height / 2;  /* half 2 */

    lut_mask->mask_define[1].mask_shape      = OT_AVS_MASK_SHAPE_ELLIPSE;
    lut_mask->mask_define[1].offset_h        = 500; /* 500 */
    lut_mask->mask_define[1].offset_v        = 500; /* 500 */
    lut_mask->mask_define[1].half_major_axis = width / 2; /* half 2 */
    lut_mask->mask_define[1].half_minor_axis = height / 2; /* half 2 */
}

static td_void sample_avs_lut_input_cfg(ot_avs_lut_generate_input *lut_input, ot_avs_lut_mask *lut_mask)
{
    ot_avs_fine_tuning   fine_tuning_cfg;

    fine_tuning_cfg.fine_tuning_en = TD_TRUE;

    fine_tuning_cfg.adjust[0].adjust_en   = TD_FALSE;
    fine_tuning_cfg.adjust[0].pitch    = 0;
    fine_tuning_cfg.adjust[0].yaw      = 0 * 100;  /* 100 */
    fine_tuning_cfg.adjust[0].roll     = -0 * 100; /* 100 */
    fine_tuning_cfg.adjust[0].offset_x  = 0;
    fine_tuning_cfg.adjust[0].offset_y  = 0;

    fine_tuning_cfg.adjust[1].adjust_en  = TD_TRUE;
    fine_tuning_cfg.adjust[1].pitch   = -10 * 100; /* 100 */
    fine_tuning_cfg.adjust[1].yaw     = 0;
    fine_tuning_cfg.adjust[1].roll    = 0;
    fine_tuning_cfg.adjust[1].offset_x = 0 * 100;  /* 100 */
    fine_tuning_cfg.adjust[1].offset_y = -0 * 100; /* 100 */

    lut_input->fine_tuning_cfg                = fine_tuning_cfg;
    lut_input->fine_tuning_cfg.fine_tuning_en = TD_FALSE;
    lut_input->lut_accuracy                   = OT_AVS_LUT_ACCURACY_HIGH;
    lut_input->type                           = OT_AVS_TYPE_AVSP;
    lut_input->stitch_distance                = 2.0; /* 2.0 */
    lut_input->mask                           = lut_mask;
    lut_input->file_input_addr                = (td_u64)(td_uintptr_t)g_cal_file;
}

static td_void sample_avs_lut_write(td_u32 length, td_u64 lut_output[OT_AVS_MAX_CAMERA_NUM], td_u32 max_len)
{
    td_s32 i, ret;
    td_char out_lut[2][FILE_MAX_LEN]; /* 2 */
    FILE *lut_file = NULL;

    for (i = 0; i < (td_s32)length; i++) {
        if (snprintf_s(out_lut[i], FILE_MAX_LEN, FILE_MAX_LEN - 1, "./2fisheye_3000x3000_mesh_%d.bin", i) == -1) {
            sample_print("set file name fail!\n");
            return;
        }
        lut_file = fopen(out_lut[i], "wb");
        if (lut_file == NULL) {
            sample_print("fopen err!\n");
            return;
        }
        ret = fwrite((td_char*)(td_uintptr_t)lut_output[i], OT_AVS_LUT_SIZE, 1, lut_file);
        if (ret < 0) {
            sample_print("fwrite err %d\n", ret);
            fclose(lut_file);
            return;
        }
        fclose(lut_file);
    }
}

static td_s32 sample_avs_lut_free()
{
    td_s32 ret = TD_SUCCESS;
    if (g_avs_lut_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_lut_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("mmz_free fail!\n");
        }
        g_avs_lut_addr = 0;
    }
    return ret;
}

static td_s32 sample_avs_mask_free()
{
    td_s32 ret = TD_SUCCESS;
    if (g_avs_mask_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_mask_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("mmz_free fail!\n");
        }
        g_avs_mask_addr = 0;
    }
    return ret;
}

static td_s32 sample_avs_generating_lut(void)
{
    const td_u32 camera_num = 2; /* 2 */
    const td_u32 width = 3000;   /* 3000 */
    const td_u32 height = 3000;  /* 3000 */
    td_u64  lut_output[OT_AVS_MAX_CAMERA_NUM];

    ot_avs_status status;
    td_void *mask_addr = NULL;
    td_void *lut_addr = NULL;
    td_s32 i, cal_size, ret;

    ot_avs_lut_generate_input lut_input;
    ot_avs_lut_mask lut_mask;

    cal_size = sample_avs_get_file_len("./data/2fisheye_3000x3000.cal");
    if (cal_size == 0) {
        sample_print("Open cal file fail!\n");
        return TD_FAILURE;
    }

    ret = sample_avs_load_file("./data/2fisheye_3000x3000.cal", g_cal_file, cal_size);
    check_return(ret, "Load cal file fail!");

    sample_avs_lut_mask_cfg(&lut_mask, width, height);

    ret = ss_mpi_sys_mmz_alloc(&(g_avs_mask_addr), &(mask_addr), "avs_mask", NULL, sizeof(td_u16) * width * height);
    if (ret != TD_SUCCESS) {
        sample_print("alloc mask buf fail with %#x!\n", ret);
        goto exit;
    }

    lut_mask.mask_addr[0] = (td_u64)(td_uintptr_t)mask_addr;
    ret = ss_mpi_sys_mmz_alloc(&(g_avs_lut_addr), &(lut_addr), "avs_lut", NULL, OT_AVS_LUT_SIZE * camera_num);
    if (ret != TD_SUCCESS) {
        sample_print("alloc lut buf fail with %#x!\n", ret);
        goto exit1;
    }

    for (i = 0; i < (td_s32)camera_num; i++) {
        lut_output[i] = (td_u64)(td_uintptr_t)lut_addr + i * OT_AVS_LUT_SIZE;
    }

    sample_avs_lut_input_cfg(&lut_input, &lut_mask);

    status = ss_mpi_avs_lut_generate(&lut_input, lut_output);
    if (status != OT_AVS_STATUS_OK) {
        sample_print("Generate lut error!\n");
        goto exit2;
    }

    sample_avs_lut_write(camera_num, lut_output, OT_AVS_MAX_CAMERA_NUM * sizeof(td_u64));

exit2:
    ret = sample_avs_lut_free();

exit1:
    ret = sample_avs_mask_free();

exit:
    return ret;
}

static td_s32 sample_avs_malloc(ot_avs_pos_cfg *avs_cfg, td_u64 mesh_addr[], td_u32 max_len, td_u32 len)
{
    td_u32 i;
    td_u32 mesh_size, mesh_point_x, mesh_point_y;
    if (avs_cfg->mesh_mode == OT_AVS_DST_QUERY_SRC) {
        mesh_point_x = (avs_cfg->dst_size.width - 1) / avs_cfg->window_size + 2;  /* 2 */
        mesh_point_y = (avs_cfg->dst_size.height - 1) / avs_cfg->window_size + 2; /* 2 */
    } else {
        mesh_point_x = (avs_cfg->src_size.width - 1) / avs_cfg->window_size + 2;  /* 2 */
        mesh_point_y = (avs_cfg->src_size.height - 1) / avs_cfg->window_size + 2; /* 2 */
    }
    mesh_size = sizeof(td_s16) * mesh_point_x * mesh_point_y * 2; /* 2 */

    g_mesh_addr = (td_ulong)malloc(mesh_size * avs_cfg->camera_num);
    if (g_mesh_addr == 0) {
        sample_print("malloc error!\n");
        return TD_FAILURE;
    }
    for (i = 0; i < len; i++) {
        mesh_addr[i] = g_mesh_addr + mesh_size * i;
    }
    return TD_SUCCESS;
}

static td_void sample_avs_malloc_free()
{
    if (g_mesh_addr) {
        free((td_void *)(td_ulong)g_mesh_addr);
        g_mesh_addr = 0;
    }
}

static td_s32 sample_avs_read_lut_data(ot_avs_pos_cfg *avs_cfg)
{
    td_s32 ret, i;
    td_void *lut_start_addr = NULL;
    td_void *lut_addr[OT_AVS_MAX_INPUT_NUM];
    td_char  lut_path[FILE_MAX_LEN];

    ret = ss_mpi_sys_mmz_alloc(&(g_avs_lut_addr), &(lut_start_addr), "avs_lut",
        NULL, OT_AVS_LUT_SIZE * avs_cfg->camera_num);
    if (ret != TD_SUCCESS) {
        sample_print("alloc lut buf fail with %#x!\n", ret);
        return TD_FAILURE;
    }

    for (i = 0; i < (td_s32)avs_cfg->camera_num; i++) {
        lut_addr[i] = (td_void *)((td_ulong)lut_start_addr + OT_AVS_LUT_SIZE * i);

        if (snprintf_s(lut_path, FILE_MAX_LEN, FILE_MAX_LEN - 1, "./data/avsp_mesh_out_%d.bin", i + 1) == -1) {
            sample_print("set file name fail!\n");
            return TD_FAILURE;
        }

        FILE    *lut_file = NULL;

        lut_file = fopen(lut_path, "rb");
        if (lut_file == NULL) {
            sample_print("open mesh file (%s) fail!\n", lut_path);
            return TD_FAILURE;
        }

        ret = fread((td_char *)lut_addr[i], OT_AVS_LUT_SIZE, 1, lut_file);
        if (ret != 1) {
            sample_print("Error reading file %s\n", lut_path);
            ret = TD_FAILURE;
        }

        avs_cfg->lut_addr[i] = (td_ulong)lut_addr[i];

        fclose(lut_file);
    }
    return TD_SUCCESS;
}

static td_void sample_avs_lut_data_free()
{
    td_s32 ret;
    if (g_avs_lut_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_lut_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("mmz_free fail!\n");
        }
        g_avs_lut_addr = 0;
    }
}

static td_void sample_avs_get_mesh_cfg(td_u32 *mesh_size, td_u32 *mesh_point_x, td_u32 *mesh_point_y,
    ot_avs_pos_cfg *avs_cfg)
{
    if (avs_cfg->mesh_mode == OT_AVS_DST_QUERY_SRC) {
        *mesh_point_x = (avs_cfg->dst_size.width - 1) / avs_cfg->window_size + 2;  /* 2 winsize */
        *mesh_point_y = (avs_cfg->dst_size.height - 1) / avs_cfg->window_size + 2; /* 2 winsize */
    } else {
        *mesh_point_x = (avs_cfg->src_size.width - 1) / avs_cfg->window_size + 2; /* 2 winsize */
        *mesh_point_y = (avs_cfg->src_size.height - 1) / avs_cfg->window_size + 2; /* 2 winsize */
    }
    *mesh_size = sizeof(td_s16) * (*mesh_point_x) * (*mesh_point_y) * 2; /* 2 winsize */
}

static td_s32 sample_avs_save_mesh_file(ot_avs_pos_cfg *avs_cfg, const td_char *file_prefix,
    td_u64 mesh_addr[OT_AVS_MAX_INPUT_NUM], td_u32 max_len, td_u32 len)
{
    td_char file_path1[FILE_MAX_LEN + 1], file_path2[FILE_MAX_LEN + 1], file_index[FILE_MAX_LEN];
    td_u32 mesh_size, mesh_point_x, mesh_point_y;

    sample_avs_get_mesh_cfg(&mesh_size, &mesh_point_x, &mesh_point_y, avs_cfg);

    for (td_s32 i = 0; i < (td_s32)len; i++) {
        if (snprintf_s(file_index, FILE_MAX_LEN, FILE_MAX_LEN - 1, "%d.csv", i) == -1) {
            sample_print("set file name fail!\n");
            return TD_FAILURE;
        }
        strncpy_s(file_path1, sizeof(file_path1), file_prefix, FILE_MAX_LEN);
        if (strcat_s(file_path1, sizeof(file_path1), file_index) != 0) {
            return TD_FAILURE;
        }

        FILE *fp = fopen(file_path1, "wb");
        if (fp == NULL) {
            sample_print("Can not open file.\n");
            return TD_FAILURE;
        }

        td_s16 *addr = (td_s16 *)(td_ulong)mesh_addr[i];
        for (td_s32 n = 0; n < (td_s32)mesh_point_y; n++) {
            for (td_s32 m = 0; m < (td_s32)mesh_point_x; m++) {
                td_s16 x_index = addr[n * mesh_point_x * 2 + m * 2]; /* 2 */
                td_s16 y_index = addr[n * mesh_point_x * 2 + m * 2 + 1]; /* 2 */

                fprintf(fp, "%d %d,", x_index, y_index);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);

        if (snprintf_s(file_index, FILE_MAX_LEN, FILE_MAX_LEN - 1, "%d.bin", i) == -1) {
            sample_print("set output file name fail!\n");
            return TD_FAILURE;
        }
        strncpy_s(file_path2, sizeof(file_path2), file_prefix, FILE_MAX_LEN);
        if (strcat_s(file_path2, sizeof(file_path2), file_index) != 0) {
            return TD_FAILURE;
        }
        fp = fopen(file_path2, "wb");
        if (fp == NULL) {
            sample_print("Can not open file.\n");
            return TD_FAILURE;
        }

        if (fwrite((td_s16*)(td_ulong)mesh_addr[i], mesh_size, 1, fp) != 1) {
            sample_print("write file fail!\n");
        }
        fclose(fp);
    }
    return TD_SUCCESS;
}

static td_s32 sample_avs_dst_to_src(ot_avs_pos_cfg *avs_cfg)
{
    td_s32 ret, i;
    td_u64 mesh_addr[OT_AVS_MAX_INPUT_NUM];
    ot_size dst_size;
    ot_point src_point, dst_point;
    const td_u32 length = 4;  /* 4 length */

    avs_cfg->projection_mode = OT_AVS_PROJECTION_EQUIRECTANGULAR;
    avs_cfg->src_size.width  = 3840;  /* 3840 */
    avs_cfg->src_size.height = 2160;  /* 2160 */
    avs_cfg->dst_size.width  = 7680;  /* 7680 */
    avs_cfg->dst_size.height = 4320;  /* 4320 */
    avs_cfg->center.x  = avs_cfg->dst_size.width / 2;  /* 2 */
    avs_cfg->center.y  = avs_cfg->dst_size.height / 2; /* 2 */
    avs_cfg->fov.fov_x = 18000;  /* 18000 */
    avs_cfg->fov.fov_y = 8000;   /* 8000 */
    avs_cfg->ori_rotation.yaw   = 0;
    avs_cfg->ori_rotation.pitch = 6500;  /* 6500 */
    avs_cfg->ori_rotation.roll  = -9000; /* -9000 */
    avs_cfg->rotation.yaw      = 0;
    avs_cfg->rotation.pitch    = 0;
    avs_cfg->rotation.roll     = 0;
    avs_cfg->mesh_mode         = OT_AVS_DST_QUERY_SRC;
    avs_cfg->window_size       = 128;  /* 128 */
    avs_cfg->lut_accuracy      = OT_AVS_LUT_ACCURACY_HIGH;

    ret = sample_avs_malloc(avs_cfg, mesh_addr, OT_AVS_MAX_INPUT_NUM, length);
    check_return(ret, "sample_avs_dst_to_src");

    ret = ss_mpi_avs_pos_mesh_generate(avs_cfg, mesh_addr);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_avs_pos_mesh_generate failed, error:%d \n", ret);
        sample_avs_malloc_free();
        return TD_FAILURE;
    }

    const td_char *file_prefix = "./data/DST_QUERY_SRC_win128_4x3840x2160_7680x4320_";
    ret = sample_avs_save_mesh_file(avs_cfg, file_prefix, mesh_addr, OT_AVS_MAX_INPUT_NUM * sizeof(td_u64), length);
    if (ret != TD_SUCCESS) {
        sample_avs_malloc_free();
        return TD_FAILURE;
    }

    dst_size.width = avs_cfg->dst_size.width;
    dst_size.height = avs_cfg->dst_size.height;
    dst_point.x = 1000;  /* 1000 x */
    dst_point.y = 1000;  /* 1000 y */
    for (i = 0; i < (td_s32)length; i++) {
        ret = ss_mpi_avs_pos_query_dst_to_src(&dst_size, avs_cfg->window_size, mesh_addr[i], &dst_point, &src_point);
        printf("dst_to_src In: (%d,%d) ->%d:(%d,%d) \n", dst_point.x, dst_point.y, i, src_point.x, src_point.y);
    }
    sample_avs_malloc_free();
    return TD_SUCCESS;
}

static td_s32 sample_avs_src_to_dst(ot_avs_pos_cfg *avs_cfg)
{
    td_s32 ret, i;
    td_u64 mesh_addr[OT_AVS_MAX_INPUT_NUM];
    ot_size src_size;
    ot_point src_point, dst_point;
    const td_u32 length = 4;  /* 4length */

    avs_cfg->projection_mode = OT_AVS_PROJECTION_EQUIRECTANGULAR;
    avs_cfg->src_size.width  = 3840;  /* 3840 */
    avs_cfg->src_size.height = 2160;  /* 2160 */
    avs_cfg->dst_size.width  = 4000;  /* 4000 */
    avs_cfg->dst_size.height = 2000;  /* 2000 */
    avs_cfg->center.x  = avs_cfg->dst_size.width / 2; /* 2 */
    avs_cfg->center.y  = avs_cfg->dst_size.height / 2; /* 2 */
    avs_cfg->fov.fov_x = 16000;  /* 16000 */
    avs_cfg->fov.fov_y = 8000;   /* 8000 */
    avs_cfg->ori_rotation.yaw   = 0;
    avs_cfg->ori_rotation.pitch = 5000;  /* 5000 */
    avs_cfg->ori_rotation.roll  = -9000; /* -9000 */
    avs_cfg->rotation.yaw      = 0;
    avs_cfg->rotation.pitch    = 0;
    avs_cfg->rotation.roll     = 0;
    avs_cfg->mesh_mode         = OT_AVS_SRC_QUERY_DST;
    avs_cfg->window_size       = 128;  /* 128 */
    avs_cfg->lut_accuracy      = OT_AVS_LUT_ACCURACY_HIGH;

    ret = sample_avs_malloc(avs_cfg, mesh_addr, OT_AVS_MAX_INPUT_NUM * sizeof(td_u64), length);
    check_return(ret, "sample_avs_src_to_dst");

    ret = ss_mpi_avs_pos_mesh_generate(avs_cfg, mesh_addr);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_avs_pos_mesh_generate failed, error:%d \n", ret);
        sample_avs_malloc_free();
        return TD_FAILURE;
    }

    const td_char *file_prefix = "./data/SRC_QUERY_DST_win128_4x3840x2160_4000x2000_";
    ret = sample_avs_save_mesh_file(avs_cfg, file_prefix, mesh_addr, OT_AVS_MAX_INPUT_NUM * sizeof(td_u64), length);
    if (ret != TD_SUCCESS) {
        sample_avs_malloc_free();
        return TD_FAILURE;
    }
    src_size.width = avs_cfg->src_size.width;
    src_size.height = avs_cfg->src_size.height;
    src_point.x = 1000;  /* 1000 x */
    src_point.y = 1000;  /* 1000 y */
    for (i = 0; i < (td_s32)length; i++) {
        ret = ss_mpi_avs_pos_query_src_to_dst(&src_size, avs_cfg->window_size, mesh_addr[i], &src_point, &dst_point);
        printf("src_to_dst In: %d:(%d,%d) -> (%d,%d) \n", i, src_point.x, src_point.y, dst_point.x, dst_point.y);
    }
    sample_avs_malloc_free();
    return TD_SUCCESS;
}

static td_s32 sample_avs_pos_query(void)
{
    td_s32 ret;
    ot_avs_pos_cfg avs_cfg;

    avs_cfg.camera_num = 4; /* 4 */
    ret = sample_avs_read_lut_data(&avs_cfg);
    if (ret != TD_SUCCESS) {
        sample_avs_lut_data_free();
        return TD_FAILURE;
    }

    /* 1. pos_query_dst_to_src */
    ret = sample_avs_dst_to_src(&avs_cfg);
    if (ret != TD_SUCCESS) {
        sample_avs_lut_data_free();
        return TD_FAILURE;
    }

    /* 2. pos_query_src_to_dst */
    ret = sample_avs_src_to_dst(&avs_cfg);
    if (ret != TD_SUCCESS) {
        sample_avs_lut_data_free();
        return TD_FAILURE;
    }

    sample_avs_lut_data_free();
    return TD_SUCCESS;
}

static td_void sample_avs_convert_config_file_path(td_char input_file_path[4][FILE_MAX_LEN],
    td_char avsp_mesh_path[4][FILE_MAX_LEN])
{
    /* input and output file path */
    strncpy_s(input_file_path[0], sizeof(input_file_path[0]),
        "./data/avs_convert/lut_cell10x8_dst5108x1520_head_0.bin", FILE_MAX_LEN);
    strncpy_s(input_file_path[1], sizeof(input_file_path[1]),
        "./data/avs_convert/lut_cell10x8_dst5108x1520_head_1.bin", FILE_MAX_LEN);
    strncpy_s(avsp_mesh_path[0], sizeof(avsp_mesh_path[0]), "./data/avs_convert/lut_avsp_0.bin", FILE_MAX_LEN);
    strncpy_s(avsp_mesh_path[1], sizeof(avsp_mesh_path[1]), "./data/avs_convert/lut_avsp_1.bin", FILE_MAX_LEN);
}

static td_s32 sample_avs_convert_read_lut_header(avs_convert_config *cfg, const char *file_path, FILE *lut_file)
{
    td_s32 ret;

    /* read lut header configuration */
    ret = fread(&cfg->mesh_size.width, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }

    ret = fread(&cfg->mesh_size.height, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }

    ret = fread(&cfg->cell_size.width, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }

    ret = fread(&cfg->cell_size.height, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }

    ret = fread(&cfg->dst_size.width, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }

    ret = fread(&cfg->dst_size.height, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }

    ret = fread(&cfg->mesh_normalized, sizeof(td_u32), 1, lut_file);
    if (ret != 1) {
        sample_print("Error reading file %s\n", file_path);
        ret = TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void sample_avs_convert_free_input()
{
    td_s32 ret;

    if (g_avs_convert_in_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_convert_in_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("mmz_free fail!\n");
        }
        g_avs_convert_in_addr = 0;
    }
}

static td_void sample_avs_convert_free_output()
{
    td_s32 ret;
    if (g_avs_convert_out_addr) {
        ret = ss_mpi_sys_mmz_free(g_avs_convert_out_addr, NULL);
        if (ret != TD_SUCCESS) {
            sample_print("mmz_free fail!\n");
        }
        g_avs_convert_out_addr = 0;
    }
}

static td_s32 sample_avs_convert_cam(td_s32 cam, avs_convert_config *cfg,
    td_char input_file_path[4][FILE_MAX_LEN], td_char avsp_mesh_path[4][FILE_MAX_LEN])
{
    td_s32 ret;
    td_void *mesh_in_addr = TD_NULL;
    td_void *mesh_out_addr = TD_NULL;

    FILE *lut_file = fopen(input_file_path[cam], "rb");
    check_null_ptr_return(lut_file);

    if (sample_avs_convert_read_lut_header(cfg, input_file_path[cam], lut_file) != TD_SUCCESS) {
        sample_print("convert read lut header fail with %#x!\n", ret);
        goto end0;
    }

    const td_u32 lut_size = 6 * cfg->mesh_size.width * cfg->mesh_size.height + AVS_FILE_HEADER_SIZE; /* 6 byte(x,y,a) */
    printf("lut_size = %d \n", lut_size);

    /* malloc memory for mesh_in_addr */
    ret = ss_mpi_sys_mmz_alloc(&(g_avs_convert_in_addr), &(mesh_in_addr), "lut_input", NULL, lut_size);
    if (ret != TD_SUCCESS) {
        sample_print("alloc lut_input buf fail with %#x!\n", ret);
        goto end0;
    }

    /* malloc memory for mesh_out_addr */
    ret = ss_mpi_sys_mmz_alloc(&(g_avs_convert_out_addr), &(mesh_out_addr), "lut_output", NULL, AVS_LUT_SIZE);
    if (ret != TD_SUCCESS) {
        sample_print("alloc lut_output buf fail with %#x!\n", ret);
        goto end1;
    }

    (td_void)memset_s(mesh_out_addr, AVS_LUT_SIZE, 0, AVS_LUT_SIZE);

    (td_void)fseek(lut_file, 0, SEEK_SET); /* set lut_file to its head address */

    if (fread(mesh_in_addr, lut_size, 1, lut_file) != 1) {
        sample_print("Error reading lut file!\n");
    }

    ret = ss_mpi_avs_conversion((td_u64)(td_uintptr_t)mesh_in_addr, (td_u64)(td_uintptr_t)mesh_out_addr);
    if (ret != TD_SUCCESS) {
        sample_print("ss_mpi_avs_conversion fail with %#x!\n", ret);
        goto end2;
    }

    FILE *lut_out_file = fopen(avsp_mesh_path[cam], "wb");
    if (lut_out_file == TD_NULL) {
        sample_print("open fail with %#x!\n", ret);
        goto end2;
    }

    if (fwrite((td_char *)(td_uintptr_t)mesh_out_addr, OT_AVS_LUT_SIZE, 1, lut_out_file) < 0) {
        sample_print("fwrite err!\n");
    }

    (td_void)fclose(lut_out_file);

end2:
    sample_avs_convert_free_output();
end1:
    sample_avs_convert_free_input();
end0:
    (td_void)fclose(lut_file);

    return ret;
}

static td_s32 sample_avs_convert(td_void)
{
    td_s32 ret, cam;
    td_char input_file_path[4][FILE_MAX_LEN]; /* 4: input lut path */
    td_char avsp_mesh_path[4][FILE_MAX_LEN];  /* 4: output avsp lut path */
    avs_convert_config cfg = {0};

    cfg.cam_num = 2; /* camera/lut number 2 */
    sample_avs_convert_config_file_path(input_file_path, avsp_mesh_path);

    for (cam = 0; cam < cfg.cam_num; cam++) {   /* camera/lut number 2 */
        ret = sample_avs_convert_cam(cam, &cfg, input_file_path, avsp_mesh_path);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    return TD_SUCCESS;
}

#ifndef __LITEOS__
static void sample_avs_handle_signal(td_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_avs_sig_flag = 1;
    }
}
#endif

static void sample_avs_usage(const char *name)
{
    printf("usage : %s <index>\n", name);
    printf("index:\n");
    printf("\t 0) 4 blend stitching, projection mode switch.\n");
    printf("\t 1) 4 blend stitching, cube map.\n");
    printf("\t 2) 4 pic no blend stitching.\n");
    printf("\t 3) generate lut.\n");
    printf("\t 4) position query, dst->src & src->dst.\n");
    printf("\t 5) avs convert.\n");

    return;
}

#ifdef __LITEOS__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    td_s32 ret = TD_FAILURE;

    if (argc < 2) { /* 2 argc */
        sample_avs_usage(argv[0]);
        return TD_FAILURE;
    }

#ifndef __LITEOS__
    struct sigaction sa;
    (td_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sample_avs_handle_signal;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, TD_NULL);
    sigaction(SIGTERM, &sa, TD_NULL);
#endif

    switch (*argv[1]) {
        case '0':
            ret = sample_avs_4stitching_switch_projection_mode();
            break;
        case '1':
            ret = sample_avs_4stitching_cube_map();
            break;
        case '2':
            ret = sample_avs_4no_blend();
            break;
        case '3':
            ret = sample_avs_generating_lut();
            break;
        case '4':
            ret = sample_avs_pos_query();
            break;
        case '5':
            ret = sample_avs_convert();
            break;
        default:
            sample_print("the index is invalid!\n");
            sample_avs_usage(argv[0]);
            return TD_FAILURE;
    }

    if (ret == TD_SUCCESS && g_avs_sig_flag == 0) {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

