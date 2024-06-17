/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "pciv_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "ot_mcc_usrdev.h"

/* we use msg port from PCIV_MSG_BASE_PORT to (PCIV_MSG_BASE_PORT + PCIV_MSG_MAX_PORT_NUM) */
static td_s32 g_msg_fd[OT_PCIV_MAX_CHIP_NUM][PCIV_MSG_MAX_PORT_NUM + 1];

static td_s32 g_cur_msg_port = PCIV_MSG_BASE_PORT + 1;

#define pciv_msg_check_target_port_return(id, port) \
do { \
    if (((id) < 0 || (id) >= OT_PCIV_MAX_CHIP_NUM) || \
        ((port) < 0 || (port) > PCIV_MSG_MAX_PORT_NUM)) { \
        printf("invalid pci msg port(%d, %d)!\n", (id), (port) + PCIV_MSG_BASE_PORT); \
        return TD_FAILURE; \
    } \
} while (0)

td_void pciv_free_all_msg_port(td_void)
{
    g_cur_msg_port = PCIV_MSG_BASE_PORT + 1;
}

td_s32 pciv_alloc_msg_port(td_s32 *msg_port)
{
    if (g_cur_msg_port > PCIV_MSG_MAX_PORT) {
        return TD_FAILURE;
    }
    *msg_port = g_cur_msg_port++;
    return TD_SUCCESS;
}

td_s32 pciv_wait_connect(td_s32 tgt_id)
{
    td_s32 msg_fd = -1;
    td_s32 ret;
    struct ot_mcc_handle_attr attr;

    msg_fd = open("/dev/mcc_userdev", O_RDWR);
    if (msg_fd < 0) {
        printf("open pci msg dev fail!\n");
        return TD_FAILURE;
    }
    printf("open msg dev ok, fd:%d\n", msg_fd);

    if (ioctl(msg_fd, OT_MCC_IOC_ATTR_INIT, &attr)) {
        printf("initialization for attr failed!\n");
        close(msg_fd);
        return -1;
    }

    attr.target_id = tgt_id;
    printf("start check pci target id:%d  ... ... ... \n", tgt_id);
    while (ioctl(msg_fd, OT_MCC_IOC_CHECK, &attr)) {
        usleep(10000); /* 10000: check every 10000us */
    }
    printf("have checked pci target id:%d ok ! \n", tgt_id);

    attr.port = 1000; /* 1000: default port */
    attr.priority = 0;
    ret = ioctl(msg_fd, OT_MCC_IOC_CONNECT, &attr);
    /*
     * check target chip whether is start up,
     * PCI's master chip and slave chip shake hands :
     * call the check interface to check the other end,
     * at the sametime the other end must finished the
     * process of the handshake
     */
    close(msg_fd);
    return ret;
}

td_s32 pciv_open_msg_port(td_s32 tgt_id, td_s32 port)
{
    td_s32 ret;
    struct ot_mcc_handle_attr attr;
    td_s32 msg_fd = -1;
    td_s32 port_index = port - PCIV_MSG_BASE_PORT;

    pciv_msg_check_target_port_return(tgt_id, port_index);

    if (g_msg_fd[tgt_id][port_index] > 0) {
        printf("pci msg port(%d, %d) have open!\n", tgt_id, port);
        return TD_FAILURE;
    }

    msg_fd = open("/dev/mcc_userdev", O_RDWR);
    if (msg_fd < 0) {
        printf("open pci msg dev fail!\n");
        return TD_FAILURE;
    }

    attr.target_id = tgt_id;
    attr.port = port;
    attr.priority = 2; /* 2: default priority */
    ret = ioctl(msg_fd, OT_MCC_IOC_CONNECT, &attr);
    if (ret) {
        printf("connect err, target:%d, port:%d\n", tgt_id, port);
        close(msg_fd);
        return -1;
    }

    g_msg_fd[tgt_id][port_index] = msg_fd;

    return TD_SUCCESS;
}

td_s32 pciv_close_msg_port(td_s32 tgt_id, td_s32 port)
{
    td_s32 msg_fd = -1;
    td_s32 port_index = port - PCIV_MSG_BASE_PORT;

    pciv_msg_check_target_port_return(tgt_id, port_index);

    msg_fd = g_msg_fd[tgt_id][port_index];

    if (msg_fd < 0) {
        return TD_SUCCESS;
    }
    close(msg_fd);
    g_msg_fd[tgt_id][port_index] = -1;

    return TD_SUCCESS;
}

td_s32 pciv_send_msg(td_s32 tgt_id, td_s32 port, sample_pciv_msg *msg)
{
    td_s32 ret;
    td_s32 msg_fd = -1;
    td_s32 port_index = port - PCIV_MSG_BASE_PORT;

    pciv_msg_check_target_port_return(tgt_id, port_index);

    if (g_msg_fd[tgt_id][port_index] <= 0) {
        printf("you should open msg port before send message!tgt_id:%d  port:%d\n", tgt_id, port);
        return TD_FAILURE;
    }
    msg_fd = g_msg_fd[tgt_id][port_index];

    pciv_check_expr_return(msg->msg_head.msg_len < SAMPLE_PCIV_MSG_MAXLEN);

    ret = write(msg_fd, msg, msg->msg_head.msg_len + sizeof(sample_pciv_msghead));
    if (ret != (td_s32)(msg->msg_head.msg_len + sizeof(sample_pciv_msghead))) {
        printf("pciv_send_msg write_len err:%d\n", ret);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_s32 pciv_read_msg(td_s32 tgt_id, td_s32 port, sample_pciv_msg *msg)
{
    td_s32 ret;
    td_u32 msg_len = sizeof(sample_pciv_msghead) + SAMPLE_PCIV_MSG_MAXLEN;
    td_s32 port_index = port - PCIV_MSG_BASE_PORT;

    pciv_msg_check_target_port_return(tgt_id, port_index);

    if (g_msg_fd[tgt_id][port_index] <= 0) {
        printf("you should open msg port before read message!\n");
        return TD_FAILURE;
    }

    ret = read(g_msg_fd[tgt_id][port_index], msg, msg_len);
    if (ret <= 0) {
        /*
         * if msg list has no data, do not print anything, or it will dominating the screen
         * when we waiting the msg
         */
        return TD_FAILURE;
    } else if (ret < (td_s32)sizeof(sample_pciv_msghead)) {
        printf("%s -> read len err:%d\n", __FUNCTION__, ret);
        return TD_FAILURE;
    }

    msg->msg_head.msg_len = ret - sizeof(sample_pciv_msghead);
    return TD_SUCCESS;
}

