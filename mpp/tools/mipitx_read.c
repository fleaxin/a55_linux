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
    MIPI_TX_ARG_READ_PARAM_NUM  = 5,   /* mipi_tx arg readparamnum */
    MIPI_TX_ARG_SEND_PARAM_NUM  = 6,   /* mipi_tx arg sendparamnum */
    MIPI_TX_ARG_SEND_PARAM0     = 7,   /* mipi_tx arg send param0 */
    MIPI_TX_ARG_SEND_PARAM1     = 8,   /* mipi_tx arg send param1 */
    MIPI_TX_ARG_BUTT,
} mipi_tx_arg_index;

#ifndef __LITEOS__
#define MIPI_TX_READ_PROGRAM_NAME "./mipitx_read"
#define MIPI_TX_WRITE_PROGRAM_NAME "./mipitx_write"
#else
#define MIPI_TX_READ_PROGRAM_NAME "mipitx_read"
#define MIPI_TX_WRITE_PROGRAM_NAME "mipitx_write"
#endif

#define MIPI_TX_ARG_NUMBER_BASE  0  /* 0:base support 10 and 16 */
#define INPUT_PARAM_NUM_BEFORE_SEND_DATA MIPI_TX_ARG_SEND_PARAM0
#define MAX_SEND_DATA_NUM 2
#define MAX_READ_DATA_NUM 100
#define MIPI_TX_DEV_NAME "/dev/ot_mipi_tx"

#define MIPI_TX_READ_USAGE_STR \
    "\nUsage: "MIPI_TX_READ_PROGRAM_NAME" [devno] [work_mode] [lp_clk_en] [datatype] [readparamnum] [sendparamnum] " \
        "[param0]...[param1]\n" \
    "datatype : DCS Data Type ,such as 0x14--generic read 1 param,0x24--generic read 2 param,\n" \
    "           to use 0x14 or 0x24,please refer to screen specification.\n" \
    "readparamnum: the num of data (byte) you want to get from the screen.\n" \
    "sendparamnum: the num of data (byte),you want to send to the screen.\n"

#define MIPI_TX_READ_USAGE_EG_STR \
    "   e.g : read 1 byte from screen, target register is 0xda\n" \
    "                "MIPI_TX_READ_PROGRAM_NAME" 0 0 1 0x14 0x1 0x1 0xda\n" \
    "   e.g : read 3 bytes from screen, target register is 0xa1, first set max return data num to 3 in step 1,\n" \
    "         then use 0x14 or 0x24(refer to screen) to get data, in step 2\n" \
    "             step 1 "MIPI_TX_WRITE_PROGRAM_NAME" 0 0 1 0x37 0x1 0x3\n" \
    "             step 2 "MIPI_TX_READ_PROGRAM_NAME" 0 0 1 0x14 0x3 0x1 0xa1\n"

static void usage(void)
{
    printf(MIPI_TX_READ_USAGE_STR \
        MIPI_TX_READ_USAGE_EG_STR);
}

static int mipitx_read_check_param_num(int argc, char *argv[])
{
    if (argc == 1) {
        printf("\tTo see more usage, please enter: "MIPI_TX_READ_PROGRAM_NAME" -h\n\n");
        return TD_FAILURE;
    }

    if (argc > 1) {
        if (strncmp(argv[1], "-h", 2) == 0) { /* 2 chars */
            usage();
            return TD_FAILURE;
        }
    }

    if (argc < INPUT_PARAM_NUM_BEFORE_SEND_DATA) {
        printf("\t input not enough,use "MIPI_TX_READ_PROGRAM_NAME" -h to get help\n\n");
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
        "datatype", "readparamnum", "sendparamnum", "sendparam0", "sendparam1"};

    if (index >= MIPI_TX_ARG_BUTT) {
        return "-";
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

static int mipitx_read_get_argv(char *argv[], get_cmd_info_t *cmd_info, int *read_param_num, int *send_param_num)
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

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_READ_PARAM_NUM, 0, MAX_READ_DATA_NUM, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    *read_param_num = val;

    ret = mipitx_get_valid_argv_val(argv, MIPI_TX_ARG_SEND_PARAM_NUM, 0, MAX_SEND_DATA_NUM, &val);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    *send_param_num = val;

    return TD_SUCCESS;
}

static int mipitx_read_get_data_param(char *argv[], int send_param_num, get_cmd_info_t *cmd_info)
{
    int ret;
    int val = 0;
    unsigned short low_byte = 0;
    unsigned short high_byte = 0;

    if (send_param_num == 0) {
        cmd_info->data_param = 0;
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

    cmd_info->data_param = (low_byte | (high_byte << 8)); /* 8bit */
    return TD_SUCCESS;
}

#ifdef __LITEOS__
int mipi_tx_read(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    int ret;
    int i;
    int fd;
    unsigned char readdata[MAX_READ_DATA_NUM] = {0};
    get_cmd_info_t cmd_info = {0};
    int read_param_num = 0;
    int send_param_num = 0;

    ret = mipitx_read_check_param_num(argc, argv);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = mipitx_read_get_argv(argv, &cmd_info, &read_param_num, &send_param_num);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    if (argc < send_param_num + INPUT_PARAM_NUM_BEFORE_SEND_DATA) {
        printf("please input enough param\n");
        return TD_FAILURE;
    }

    ret = mipitx_read_get_data_param(argv, send_param_num, &cmd_info);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    fd = open(MIPI_TX_DEV_NAME, O_RDONLY);
    if (fd < 0) {
        printf("open ot_mipi_tx dev failed\n");
        return TD_FAILURE;
    }

    cmd_info.devno = 0;
    cmd_info.get_data = readdata;
    cmd_info.get_data_size = (unsigned short)read_param_num;
    ret = ioctl(fd, OT_MIPI_TX_GET_CMD, &cmd_info);
    if (ret != TD_SUCCESS) {
        printf("MIPI_TX GET CMD failed, please check, may not use mipitx_set first\n");
        close(fd);
        return TD_FAILURE;
    }

    for (i = 0; i < read_param_num; i++) {
        printf("readdata[%d] = 0x%x\n", i, cmd_info.get_data[i]);
    }

    close(fd);
    printf("read end\n");
    return TD_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
