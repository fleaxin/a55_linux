/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include "securec.h"
#include "ot_common.h"
#include "ot_common_aio.h"
#include "ss_mpi_audio.h"

#define SAVE_FILE_DEFAULT_SIZE 1024
#define SAVE_FILE_DEFAULT_NAME "default"

#define AO_PATH_NAME_MAX_LEN 256
#define AO_DUMP_ARG_NUMBER_BASE 10

#define AO_DUMP_ARG_FILE_SIZE_INDEX 3
#define AO_DUMP_ARG_SAMPLE_RATE_INDEX 4

#define audio_value_between(x, min, max) (((x) >= (min)) && ((x) < (max)))

static ot_audio_save_file_info g_save_file_info = {0};
static ot_audio_dev g_ao_dev_id = 0;
static ot_ao_chn g_ao_chn = 0;
static ot_audio_sample_rate g_in_sample_rate = OT_AUDIO_SAMPLE_RATE_16000;
static ot_audio_sample_rate g_out_sample_rate = OT_AUDIO_SAMPLE_RATE_16000;

static td_u32 g_signal_flag = 0;

#ifndef __LITEOS__
static FILE *g_ao_cur_path_fp = TD_NULL;

static void ao_dump_handle_sig(td_s32 signo)
{
    if ((signo == SIGINT) || (signo == SIGTERM)) {
        g_signal_flag = 1;
    }
}

static td_void ao_dump_sig_proc(td_void)
{
    g_save_file_info.cfg = TD_FALSE;
    ss_mpi_ao_save_file(g_ao_dev_id, g_ao_chn, &g_save_file_info);

    if (g_ao_cur_path_fp != TD_NULL) {
        pclose(g_ao_cur_path_fp);
        g_ao_cur_path_fp = TD_NULL;
    }

    printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
}
#else
static td_void ao_dump_sig_proc(td_void)
{
    return;
}
#endif

static td_void ao_dump_tool_usage(td_void)
{
    printf("\n*************************************************\n"
           "Usage: ./ao_dump <dev> <chn> [size] [in_sample_rate]\n"
           "1)dev: ao device id.\n"
           "2)chn: ao channel id.\n"
           "3)size: file size(KB).\n"
           "default:1024\n"
           "4)in_sample_rate: input sample rate(Hz).\n"
           "range:8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000\n"
           "\n"
           "Note:\n"
           "1)path for saving is current path, name for saving is 'default'.\n"
           "2)need to assign in_sample_rate manual when enable resample.\n"
           "\n*************************************************\n");
}

static td_bool ao_dump_check_sample_rate(ot_audio_sample_rate sample_rate)
{
    if ((sample_rate != OT_AUDIO_SAMPLE_RATE_8000) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_12000) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_11025) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_16000) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_22050) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_24000) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_32000) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_44100) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_48000) &&
        (sample_rate != OT_AUDIO_SAMPLE_RATE_64000)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static td_s32 ao_dump_set_sample_rate(int argc, char *argv[])
{
    ot_aio_attr ao_attr = { 0 };
    td_s32 ret;
    td_slong result;
    td_char *end_ptr = TD_NULL;

    ret = ss_mpi_ao_get_pub_attr(g_ao_dev_id, &ao_attr);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_ao_get_pub_attr fail, ret:0x%x\n", ret);
        return -1;
    }
    g_out_sample_rate = ao_attr.sample_rate;
    if (argc >= AO_DUMP_ARG_SAMPLE_RATE_INDEX + 1) { /* 5:argc index */
        errno = 0;
        result = strtol(argv[AO_DUMP_ARG_SAMPLE_RATE_INDEX], &end_ptr, AO_DUMP_ARG_NUMBER_BASE);
        if ((end_ptr == argv[AO_DUMP_ARG_SAMPLE_RATE_INDEX]) || (*end_ptr != '\0')) {
            printf("out_sample_rate param invalid!!!\n");
            return -1;
        } else if (((result == LONG_MIN) || (result == LONG_MAX)) && (errno == ERANGE)) {
            printf("out_sample_rate param overflow!!!\n");
            return -1;
        } else if (!ao_dump_check_sample_rate((ot_audio_sample_rate)result)) {
            printf("out_sample_rate must be [8000, 64000]!!!!\n\n");
            return -1;
        }
        g_in_sample_rate = (ot_audio_sample_rate)result;
    } else {
        g_in_sample_rate = ao_attr.sample_rate;
    }

    return 0;
}

static td_s32 ao_dump_set_dev(int argc, char *argv[])
{
    td_char *end_ptr = TD_NULL;

    g_ao_dev_id = strtol(argv[1], &end_ptr, 10); /* 10:base, argv[1]:dev id */
    if (*end_ptr != '\0') {
        ao_dump_tool_usage();
        return -1;
    }
    if (!audio_value_between(g_ao_dev_id, 0, OT_AO_DEV_MAX_NUM)) {
        printf("ao dev id must be [0,%d)!!!!\n\n", OT_AO_DEV_MAX_NUM);
        return -1;
    }

    return 0;
}

static td_s32 ao_dump_set_chn(int argc, char *argv[])
{
    td_char *end_ptr = TD_NULL;

    g_ao_chn = strtol(argv[2], &end_ptr, 10); /* 10:base, argv[2]:chn id */
    if (*end_ptr != '\0') {
        ao_dump_tool_usage();
        return -1;
    }
    if (!audio_value_between(g_ao_chn, 0, OT_AO_MAX_CHN_NUM)) {
        printf("ao chn id must be [0,%d)!!!!\n\n", OT_AO_MAX_CHN_NUM);
        return -1;
    }

    return 0;
}

static td_s32 ao_dump_set_file_size(int argc, char *argv[])
{
    td_slong size = SAVE_FILE_DEFAULT_SIZE;
    td_char *end_ptr = TD_NULL;

    if (argc >= AO_DUMP_ARG_FILE_SIZE_INDEX + 1) { /* 4:argc index */
        errno = 0;
        size = strtol(argv[AO_DUMP_ARG_FILE_SIZE_INDEX], &end_ptr, AO_DUMP_ARG_NUMBER_BASE);
        if ((end_ptr == argv[AO_DUMP_ARG_FILE_SIZE_INDEX]) || (*end_ptr != '\0')) {
            printf("ao file size invalid!!!\n");
            return -1;
        } else if (((size == LONG_MIN) || (size == LONG_MAX)) && (errno == ERANGE)) {
            printf("ao file size overflow!!!\n");
            return -1;
        } else if (!audio_value_between(size, 1, 10 * 1024 + 1)) {
            printf("ao file size must be [1, 10*1024]!!!!\n\n");
            return -1;
        }
    }
    g_save_file_info.file_size = size;

    return 0;
}

static td_s32 ao_dump_set_file_path(td_void)
{
    td_s32 ret;
    td_char cur_path[OT_MAX_AUDIO_FILE_PATH_LEN + 2]; /* 2: '\n'+'\0' */

#ifndef __LITEOS__
    g_ao_cur_path_fp = popen("pwd", "r");
    if (g_ao_cur_path_fp == TD_NULL) {
        return -1;
    }
    fgets(cur_path, sizeof(cur_path), g_ao_cur_path_fp);
    if (!audio_value_between(strlen(cur_path), 1, OT_MAX_AUDIO_FILE_PATH_LEN)) {
        printf("path length must be [1,256]!!!!\n\n");
        pclose(g_ao_cur_path_fp);
        g_ao_cur_path_fp = TD_NULL;
        return -1;
    }
    cur_path[strlen(cur_path) - 1] = '/'; /* replace '\n' with '/' */
    pclose(g_ao_cur_path_fp);
    g_ao_cur_path_fp = TD_NULL;
#else
    cur_path[0] = '.';
    cur_path[1] = '/';
    cur_path[2] = '\0'; /* 2: index */
#endif

    ret = memcpy_s(g_save_file_info.file_path, OT_MAX_AUDIO_FILE_PATH_LEN - 1,
        cur_path, strlen(cur_path) + 1);
    if (ret != EOK) {
        printf("file_path memcpy_s fail, ret: 0x%x.\n\n", ret);
        return -1;
    }

    return 0;
}

static td_s32 ao_dump_set_file_name(td_void)
{
    td_s32 ret;

    ret = memcpy_s(g_save_file_info.file_name, OT_MAX_AUDIO_FILE_NAME_LEN - 1,
        SAVE_FILE_DEFAULT_NAME, strlen(SAVE_FILE_DEFAULT_NAME) + 1);
    if (ret != EOK) {
        printf("ao file_name memcpy_s fail, ret = 0x%x.\n\n", ret);
        return -1;
    }

    return 0;
}

static td_s32 ao_dump_check_and_set(int argc, char *argv[])
{
    td_s32 ret;

    if (argc < 3 || argc > 5) { /* 3,5: min argc, max argc  */
        ao_dump_tool_usage();
        return -1;
    }
    errno = 0;

    if (!strncmp(argv[1], "-h", strlen("-h"))) {
        ao_dump_tool_usage();
        return -1;
    } else {
        ret = ao_dump_set_dev(argc, argv);
        if (ret != 0) {
            return ret;
        }
    }

    ret = ao_dump_set_chn(argc, argv);
    if (ret != 0) {
        return ret;
    }

    ret = ao_dump_set_file_size(argc, argv);
    if (ret != 0) {
        return ret;
    }

    ret = ao_dump_set_sample_rate(argc, argv);
    if (ret != 0) {
        return ret;
    }

    ret = ao_dump_set_file_path();
    if (ret != 0) {
        return ret;
    }

    ret = ao_dump_set_file_name();
    if (ret != 0) {
        return ret;
    }

    g_save_file_info.cfg = TD_TRUE;

    return 0;
}

static td_s32 ao_dump_create_file(ot_audio_dev ao_dev, ot_ao_chn ao_chn,
    const ot_audio_save_file_info *save_file_info, ot_audio_sample_rate in_sample_rate,
    ot_audio_sample_rate out_sample_rate)
{
    td_s32 ret;
    td_char sin_file_path[AO_PATH_NAME_MAX_LEN + 1];
    td_char sou_file_path[AO_PATH_NAME_MAX_LEN + 1];
    FILE *fd_temp = TD_NULL;

    ret = snprintf_s(sin_file_path, AO_PATH_NAME_MAX_LEN + 1, AO_PATH_NAME_MAX_LEN,
        "%s/sin_ao_dev%d_chn%d_%dk_%s.pcm", save_file_info->file_path, ao_dev, ao_chn,
        in_sample_rate / 1000, save_file_info->file_name); /* 1000: kHz */
    if (ret <= EOK) {
        printf("sin file path snprintf_s fail, ret:0x%x\n", ret);
        return -1;
    }

    ret = snprintf_s(sou_file_path, AO_PATH_NAME_MAX_LEN + 1, AO_PATH_NAME_MAX_LEN,
        "%s/sou_ao_dev%d_chn%d_%dk_%s.pcm", save_file_info->file_path, ao_dev, ao_chn,
        out_sample_rate / 1000, save_file_info->file_name); /* 1000: kHz */
    if (ret <= EOK) {
        printf("sou file path snprintf_s fail, ret:0x%x\n", ret);
        return -1;
    }

    fd_temp = fopen(sin_file_path, "wb+");
    if (fd_temp == TD_NULL) {
        printf("ao_dump fopen sin file fail.\n");
        return -1;
    }
    (td_void)fclose(fd_temp);

    fd_temp = fopen(sou_file_path, "wb+");
    if (fd_temp == TD_NULL) {
        printf("ao_dump fopen sou file fail.\n");
        return -1;
    }
    (td_void)fclose(fd_temp);

    return 0;
}

#ifdef __LITEOS__
td_s32 ao_dump(int argc, char *argv[])
#else
td_s32 main(int argc, char *argv[])
#endif
{
    ot_audio_file_status file_status = {0};
    td_s32 ret;

    printf("\nNOTICE: This ao_dump tool only can be used for TESTING !!!\n");

    ret = ao_dump_check_and_set(argc, argv);
    if (ret != 0) {
        return -1;
    }

    ret = ao_dump_create_file(g_ao_dev_id, g_ao_chn, &g_save_file_info,
        g_in_sample_rate, g_out_sample_rate);
    if (ret != 0) {
        return -1;
    }

#ifndef __LITEOS__
    (td_void)signal(SIGINT, ao_dump_handle_sig);
    (td_void)signal(SIGTERM, ao_dump_handle_sig);
#endif

    printf("ao_dump file path:%s, file name:%s, file size:%d*1024\n\n", g_save_file_info.file_path,
        g_save_file_info.file_name, g_save_file_info.file_size);

    ret = ss_mpi_ao_save_file(g_ao_dev_id, g_ao_chn, &g_save_file_info);
    if (ret != TD_SUCCESS) {
        printf("ss_mpi_ao_save_file() error, ret=%x!!!!\n\n", ret);
        return -1;
    }

    printf("ao_dump saving file now, please wait.\n");

    do {
        ret = ss_mpi_ao_query_file_status(g_ao_dev_id, g_ao_chn, &file_status);
        if (g_signal_flag != 0) {
            ao_dump_sig_proc();
            break;
        } else if ((ret != TD_SUCCESS) || (file_status.saving == TD_FALSE)) {
            break;
        }

        usleep(200000); /* 200000 us */
    } while (file_status.saving);

    printf("ao_dump file saving is finished.\n");

    return TD_SUCCESS;
}
