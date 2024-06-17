/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <securec.h>
#include <sys/ioctl.h>
#include <ot_type.h>
#include "ot_mipi_tx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum {
    MIPI_TX_ARG_CMD             = 0,   /* mipi_tx arg cmd */
    MIPI_TX_ARG_DEV_NO          = 1,   /* mipi_tx arg devno */
    MIPI_TX_ARG_WORK_MODE       = 2,   /* mipi_tx arg work_mode */
    MIPI_TX_ARG_LP_CLK_EN       = 3,   /* mipi_tx arg lp_clk_en */
    MIPI_TX_ARG_DATA_TYPE       = 4,   /* mipi_tx arg datatype */
    MIPI_TX_ARG_SEND_PARAM_NUM  = 5,   /* mipi_tx arg paramnum */
    MIPI_TX_ARG_SEND_PARAM0     = 6,   /* mipi_tx arg param0 */
    MIPI_TX_ARG_SEND_PARAM1     = 7,   /* mipi_tx arg param1 */
    MIPI_TX_ARG_BUTT,
} mipi_tx_arg_index;

#ifndef __LITEOS__
#define MIPI_TX_WRITE_PROGRAM_NAME "./mipitx_write"
#else
#define MIPI_TX_WRITE_PROGRAM_NAME "mipitx_write"
#endif

#define MIPI_TX_ARG_NUMBER_BASE  0  /* 0:base support 10 and 16 */
#define INPUT_PARAM_NUM_BEFORE_SEND_DATA MIPI_TX_ARG_SEND_PARAM0
#define MAX_SEND_DATA_NUM 28
#define MIPI_TX_DEV_NAME "/dev/ot_mipi_tx"

#define MIPI_TX_WRITE_USAGE_STR \
    "\nUsage: "MIPI_TX_WRITE_PROGRAM_NAME" [devno] [work_mode] [lp_clk_en] [datatype] [paramnum] " \
        "[param0][param1]...[paramn]\n" \
    "datatype : DCS Data Type, such as 0x05--DSC write, 0x23--generic short write, 0x29--generic long write\n" \
    "           to use 0x05 or others to write, please refer to screen specification.\n" \
    "paramnum : the total num of param.(unit:byte)\n"

#define MIPI_TX_WRITE_USAGE_EG_STR \
    "   e.g : "MIPI_TX_WRITE_PROGRAM_NAME" 0 0 1 0x05 0x1 0x11\n" \
    "   e.g : "MIPI_TX_WRITE_PROGRAM_NAME" 0 0 1 0x23 0x2 0x12 0x34\n" \
    "   e.g : "MIPI_TX_WRITE_PROGRAM_NAME" 0 0 1 0x29 0x3 0x12 0x34 0x56\n"

static void usage(void)
{
    printf(MIPI_TX_WRITE_USAGE_STR \
        MIPI_TX_WRITE_USAGE_EG_STR);
}

static int mipitx_write_check_param_num(int argc, char *argv[])
{
    if (argc == 1) {
        printf("\tTo see more usage, please enter: "MIPI_TX_WRITE_PROGRAM_NAME" -h\n\n");
        return TD_FAILURE;
    }

    if (argc > 1) {
        if (strncmp(argv[1], "-h", 2) == 0) { /* 2 chars */
            usage();
            return TD_FAILURE;
        }
    }

    if (argc < INPUT_PARAM_NUM_BEFORE_SEND_DATA) {
        printf("\t input not enough,use "MIPI_TX_WRITE_PROGRAM_NAME" -h to get help\n\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static int mipitx_get_argv_val(char *argv[], mipi_tx_arg_index index, long *val)
{
    char *end_ptr = NULL;
    long result;

    errno = 0;
    result = strtol(argv[index], &end_ptr, MIPI_TX_ARG_NUMBER_BASE);
    if ((end_ptr == argv[index]) || (*end_ptr != '\0')) {
        return TD_FAILURE;
    } else if (((result == LONG_MIN) || (result == LONG_MAX)) &&
        (errno == ERANGE)) {
        return TD_FAILURE;
    }

    *val = result;
    return TD_SUCCESS;
}

static const char *mipitx_get_argv_name(mipi_tx_arg_index index)
{
    const char *argv_name[MIPI_TX_ARG_BUTT] = {"mipitx_read", "devno", "work_mode", "lp_clk_en",
        "datatype", "paramnum", "param0", "param1"};

    if (index >= MIPI_TX_ARG_BUTT) {
        return "paramn";
    }

    return argv_name[index];
}

static int mipitx_get_valid_argv_val(char *argv[], mipi_tx_arg_index index,
    int min_val, int max_val, int *val)
{
    int ret;
    long result = 0;

    ret = mipitx_get_argv_val(argv, index, &result);
    if (ret != TD_SUCCESS) {
        printf("Failure: input arg index(%u) name(%s) arg is invalid!\n",
            index, mipitx_get_argv_name(index));
        usage();
        return ret;
    }

    if ((result < min_val) || (result > max_val)) {
        printf("Failure: input arg index(%u) name(%s) arg val (%ld) is wrong. should be [%d, %d]!\n",
            index, mipitx_get_argv_name(index), result, min_val, max_val);
        usage();
        return TD_FAILURE;
    }

    *val = (int)result;
    return TD_SUCCESS;
}

static int mipitx_write_get_argv(char *argv[], cmd_info_t *cmd_info, int *send_param_num)
{
    int ret;
    int val = 0;

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_DEV_NO, 0, 0, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    cmd_info->devno = (unsigned int)val;

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_WORK_MODE,
        MIPI_TX_WORK_MODE_LP, MIPI_TX_WORK_MODE_BUTT - 1, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    cmd_info->work_mode = (mipi_tx_work_mode_t)val;

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_LP_CLK_EN, 0, 1, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    cmd_info->lp_clk_en = (unsigned char)val;

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_DATA_TYPE, 1, USHRT_MAX, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    cmd_info->data_type = (unsigned short)val;

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_SEND_PARAM_NUM, 0, MAX_SEND_DATA_NUM, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    *send_param_num = val;

    return TD_SUCCESS;
}

static unsigned char g_mipitx_send_data[MAX_SEND_DATA_NUM] = {0};

static void mipitx_write_init_write_data(void)
{
    (void)memset_s(g_mipitx_send_data, sizeof(g_mipitx_send_data), 0, sizeof(g_mipitx_send_data));
}

static int mipitx_write_get_data_param1(char *argv[], cmd_info_t *cmd_info, int send_param_num)
{
    int ret;
    int val = 0;
    unsigned short low_byte = 0;
    unsigned short high_byte = 0;

    if (send_param_num == 0) {
        cmd_info->cmd_size = 0;
        cmd_info->cmd = NULL;
        return TD_SUCCESS;
    }

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_SEND_PARAM0, 0, USHRT_MAX, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    low_byte = (unsigned short)val;

    if (send_param_num == MAX_SEND_DATA_NUM) {
        ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_SEND_PARAM1, 0, USHRT_MAX, &val);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        high_byte = (unsigned short)val;
    }

    cmd_info->cmd_size = (low_byte | (high_byte << 8)); /* 8bit */
    cmd_info->cmd = NULL;
    return TD_SUCCESS;
}

static int mipitx_write_get_data_param2(char *argv[], cmd_info_t *cmd_info, int send_param_num)
{
    int ret;
    int val = 0;
    int i;

    mipitx_write_init_write_data();

    for (i = 0; i < send_param_num; i++) {
        ret = mipitx_get_valid_argv_val(argv, i + MIPI_TX_ARG_SEND_PARAM0, 0, USHRT_MAX, &val);
        if (ret != TD_SUCCESS) {
            mipitx_write_init_write_data();
            return ret;
        }
        g_mipitx_send_data[i] = (unsigned char)val;
    }

    cmd_info->cmd_size = (unsigned short)send_param_num;
    cmd_info->cmd = g_mipitx_send_data;
    return TD_SUCCESS;
}

static int mipitx_write_get_data_param(char *argv[], cmd_info_t *cmd_info, int send_param_num)
{
    if (send_param_num <= 2) {  /* 2 param */
        return mipitx_write_get_data_param1(argv, cmd_info, send_param_num);
    }

    return mipitx_write_get_data_param2(argv, cmd_info, send_param_num);
}

#ifdef __LITEOS__
int mipi_tx_write(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int ret;
    int fd;
    cmd_info_t cmd_info = {0};
    int send_param_num = 0;

    ret = mipitx_write_check_param_num(argc, argv);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = mipitx_write_get_argv(argv, &cmd_info, &send_param_num);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (argc < send_param_num + INPUT_PARAM_NUM_BEFORE_SEND_DATA) {
        printf("please input enough param\n");
        return TD_FAILURE;
    }

    ret = mipitx_write_get_data_param(argv, &cmd_info, send_param_num);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    fd = open(MIPI_TX_DEV_NAME, O_RDONLY);
    if (fd < 0) {
        printf("open %s failed\n", MIPI_TX_DEV_NAME);
        return TD_FAILURE;
    }

    ret = ioctl(fd, OT_MIPI_TX_SET_CMD, &cmd_info);
    if (ret != TD_SUCCESS) {
        printf("MIPI_TX SET CMD failed\n");
        close(fd);
        return TD_FAILURE;
    }

    close(fd);
    printf("write end\n");
    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
