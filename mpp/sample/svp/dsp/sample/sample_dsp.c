/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_sys.h"
#include "ot_common_svp.h"

#include "sample_common_ive.h"
#include "sample_common_svp.h"
#include "sample_dsp_main.h"
#include "sample_dsp_enca.h"
#include "ss_mpi_dsp.h"

#define SAMPLE_SVP_DSP_QUERY_SLEEP       100
#define SAMPLE_SVP_DSP_MILLIC_SEC        20000

static sample_svp_dsp_enca_dilate_arg g_dsp_dilate_arg = {0};
static sample_vi_cfg g_dsp_vi_config = {0};
static ot_sample_svp_switch g_dsp_switch = { TD_FALSE, TD_FALSE };
static td_bool g_dsp_stop_signal = TD_FALSE;
static pthread_t g_dsp_thread = 0;

/*
 * Uninit Dilate
 */
static td_void sample_svp_dsp_dilate_uninit(sample_svp_dsp_enca_dilate_arg *dilate_arg)
{
    sample_common_svp_destroy_mem_info(&(dilate_arg->assist_buf), 0);
    sample_svp_mmz_free(dilate_arg->dst.phys_addr[0], dilate_arg->dst.virt_addr[0]);
}
/*
 * Init Dilate
 */
static td_s32 sample_svp_dsp_dilate_init(sample_svp_dsp_enca_dilate_arg *dilate_arg,
    td_u32 width, td_u32 height, ot_svp_dsp_id dsp_id, ot_svp_dsp_pri pri)
{
    td_s32 ret;
    td_u32 size = sizeof(ot_svp_src_img) + sizeof(ot_svp_dst_img);
    (td_void)memset_s(dilate_arg, sizeof(*dilate_arg), 0, sizeof(*dilate_arg));

    /* Do not malloc src address ,it get from vpss */
    dilate_arg->src.width  = width;
    dilate_arg->src.height = height;
    dilate_arg->src.type   = OT_SVP_IMG_TYPE_U8C1;

    ret = sample_common_ive_create_image(&(dilate_arg->dst), OT_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_return(ret != TD_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):sample_common_ive_create_image failed!\n", ret);

    ret = sample_common_svp_create_mem_info(&(dilate_arg->assist_buf), size, 0);
    sample_svp_check_exps_goto(ret != TD_SUCCESS, fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):sample_common_svp_create_mem_info failed!\n", ret);

    dilate_arg->dsp_id = dsp_id;
    dilate_arg->pri   = pri;
    return ret;
fail_0:
    sample_svp_mmz_free(dilate_arg->dst.phys_addr[0], dilate_arg->dst.virt_addr[0]);
    (td_void)memset_s(dilate_arg, sizeof(*dilate_arg), 0, sizeof(*dilate_arg));
    return ret;
}
/*
 * Process Dilate
 */
static td_s32 sample_svp_dsp_dilate_proc(sample_svp_dsp_enca_dilate_arg *dilate_arg,
    ot_video_frame_info *ext_frm_info)
{
    ot_svp_dsp_handle handle;
    td_bool finish;
    td_bool block = TD_TRUE;
    td_s32 ret;
    /* Ony get YVU400 */
    dilate_arg->src.phys_addr[0] = ext_frm_info->video_frame.phys_addr[0];
    dilate_arg->src.virt_addr[0] = (td_u64)(td_uintptr_t)ext_frm_info->video_frame.virt_addr[0];
    dilate_arg->src.stride[0]  = ext_frm_info->video_frame.stride[0];
    /* Call enca mpi */
    ret = sample_svp_dsp_enca_dilate_3x3(&handle, dilate_arg);
    sample_svp_check_exps_return(ret != TD_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):OT_MPI_SVP_DSP_ENCA_Dilate3x3 failed!\n", ret);

    /* Wait dsp finish */
    while (OT_ERR_SVP_DSP_QUERY_TIMEOUT == (ret = ss_mpi_svp_dsp_query(dilate_arg->dsp_id, handle, &finish, block))) {
        usleep(SAMPLE_SVP_DSP_QUERY_SLEEP);
    }

    return ret;
}

/*
 * Process thread function
 */
static td_void *sample_svp_dsp_vi_to_vo(td_void *args)
{
    td_s32 ret;
    sample_svp_dsp_enca_dilate_arg *dilate_info = NULL;
    ot_video_frame_info base_frm_info;
    ot_video_frame_info ext_frm_info;
    const td_s32 milli_sec = SAMPLE_SVP_DSP_MILLIC_SEC;
    const ot_vo_layer vo_layer = 0;
    const ot_vo_chn vo_chn = 0;
    const td_s32 vpss_grp = 0;
    td_s32 vpss_chn[] = { OT_VPSS_CHN0, OT_VPSS_CHN1 };

    dilate_info = (sample_svp_dsp_enca_dilate_arg*)args;

    while (TD_FALSE == g_dsp_stop_signal) {
        ret = ss_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[1], &ext_frm_info, milli_sec);
        sample_svp_check_exps_continue(ret != TD_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Err(%#x),vpss_get_chn_frame failed, vpss_grp(%d), vpss_chn(%d)!\n", ret, vpss_grp, vpss_chn[1]);

        ret = ss_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[0], &base_frm_info, milli_sec);
        sample_svp_check_exps_goto(ret != TD_SUCCESS, ext_release, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),ss_mpi_vpss_get_chn_frame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
            ret, vpss_grp, vpss_chn[0]);

        ret = sample_svp_dsp_dilate_proc(dilate_info, &ext_frm_info);
        sample_svp_check_exps_goto(ret != TD_SUCCESS, base_release, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_svp_dsp_dilate_proc failed!\n", ret);

        ret = ss_mpi_vo_send_frame(vo_layer, vo_chn, &base_frm_info, milli_sec);
        sample_svp_check_exps_goto(ret != TD_SUCCESS, base_release, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),ss_mpi_vo_send_frame failed!\n", ret);
base_release:
        ret = ss_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[0], &base_frm_info);
        sample_svp_check_exps_trace(ret != TD_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, vpss_grp, vpss_chn[0]);
ext_release:
        ret = ss_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[1], &ext_frm_info);
        sample_svp_check_exps_trace(ret != TD_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, vpss_grp, vpss_chn[1]);
    }

    return TD_NULL;
}
static td_s32 sample_svp_dsp_chceck_abnormal(td_void)
{
    if (g_dsp_stop_signal == TD_TRUE) {
        if (g_dsp_thread != 0) {
            pthread_join(g_dsp_thread, TD_NULL);
            g_dsp_thread = 0;
        }
        sample_svp_dsp_dilate_uninit(&g_dsp_dilate_arg);
        sample_comm_svp_unload_core_binary(g_dsp_dilate_arg.dsp_id);
        (td_void)memset_s(&g_dsp_dilate_arg, sizeof(g_dsp_dilate_arg), 0, sizeof(g_dsp_dilate_arg));
        sample_common_svp_stop_vi_vpss_venc_vo(&g_dsp_vi_config, &g_dsp_switch);
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}
static td_s32 sample_svp_dsp_pause(td_void)
{
    td_s32 ret;
    printf("---------------press Enter key to exit!---------------\n");
    ret = sample_svp_dsp_chceck_abnormal();
    sample_svp_check_exps_return(ret != TD_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "program termination abnormally!\n");
    (td_void)getchar();
    ret = sample_svp_dsp_chceck_abnormal();
    sample_svp_check_exps_return(ret != TD_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "program termination abnormally!\n");
    return TD_SUCCESS;
}
static td_void sample_svp_dsp_dilate_core(ot_svp_dsp_id dsp_id, ot_svp_dsp_pri pri)
{
    td_s32 ret;
    ot_pic_size pic_type = PIC_CIF;
    ot_size pic_size;

    g_dsp_switch.is_vo_open   = TD_TRUE;
    g_dsp_switch.is_venc_open = TD_FALSE;

    ret = sample_common_svp_start_vi_vpss_venc_vo(&g_dsp_vi_config, &g_dsp_switch, &pic_type);
    sample_svp_check_exps_goto(ret != TD_SUCCESS, end_dsp_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vi_vpss_venc_vo failed!\n", ret);

    ret = sample_comm_sys_get_pic_size(pic_type, &pic_size);
    sample_svp_check_exps_goto(ret != TD_SUCCESS, end_dsp_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_comm_sys_get_pic_size failed!\n", ret);

    /* Load bin */
    ret = sample_comm_svp_load_core_binary(dsp_id);
    sample_svp_check_exps_goto(ret != TD_SUCCESS, end_dsp_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):sample_comm_svp_load_core_binary failed!\n", ret);

    /* Init */
    ret = sample_svp_dsp_dilate_init(&g_dsp_dilate_arg, pic_size.width, pic_size.height, dsp_id, pri);
    sample_svp_check_exps_goto(ret != TD_SUCCESS, end_dsp_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):sample_svp_dsp_dilate_init failed!\n", ret);

    ret = prctl(PR_SET_NAME, "DSP_ViToVo", 0, 0, 0);
    sample_svp_check_exps_goto(ret != TD_SUCCESS, end_dsp_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "set thread name failed!\n");

    ret = pthread_create(&g_dsp_thread, 0, sample_svp_dsp_vi_to_vo, (td_void*)&g_dsp_dilate_arg);
    sample_svp_check_exps_goto(ret != 0, end_dsp_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "thread create failed!\n");

    ret = sample_svp_dsp_pause();
    sample_svp_check_exps_return_void(ret != TD_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "pause failed!\n");

    g_dsp_stop_signal = TD_TRUE;
    pthread_join(g_dsp_thread, TD_NULL);
    g_dsp_thread = 0;

    sample_svp_dsp_dilate_uninit(&g_dsp_dilate_arg);
end_dsp_1:
    sample_comm_svp_unload_core_binary(dsp_id);
end_dsp_0:
    sample_common_svp_stop_vi_vpss_venc_vo(&g_dsp_vi_config, &g_dsp_switch);
}

/*
 * Dilate sample
 */
td_void sample_svp_dsp_dilate(td_void)
{
    ot_svp_dsp_pri pri = OT_SVP_DSP_PRI_0;
    ot_svp_dsp_id dsp_id = OT_SVP_DSP_ID_0;
    sample_svp_dsp_dilate_core(dsp_id, pri);
}

/*
 * Dilate signal handle
 */
td_void sample_svp_dsp_dilate_handle_sig(td_void)
{
    g_dsp_stop_signal = TD_TRUE;
}