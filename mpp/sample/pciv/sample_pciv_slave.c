/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "sample_pciv_slave.h"

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
#include <pthread.h>
#include <math.h>
#include <signal.h>
#include <sys/prctl.h>

#include "ot_debug.h"

#include "ss_mpi_pciv.h"
#include "sample_pciv_comm.h"
#include "pciv_msg.h"
#include "pciv_trans.h"
#include "loadbmp.h"
#include "securec.h"

static td_bool g_is_quit = TD_FALSE;
static td_u32  g_echo_msg_len = 0;
static td_s32  g_pci_local_id = -1;
static td_bool g_signo_flag = TD_TRUE;

static sample_pciv_co_thread g_co_thrad[SAMPLE_PCIV_CHN_PER_CHIP] = { 0 };

static inline td_u32 get_rect_len(td_u32 x)
{
    return OT_ALIGN_UP((x) / 8, 2); /* 8:2: get a rect 1/8 length for a pic which is 2 alignment */
}

static td_s32 sample_pciv_echo_msg(td_s32 ret_val, sample_pciv_msg *msg)
{
    msg->msg_head.target   = 0; /* To host */
    msg->msg_head.ret_val  = ret_val;
    msg->msg_head.msg_type = SAMPLE_PCIV_MSG_ECHO;
    msg->msg_head.msg_len  = g_echo_msg_len + sizeof(sample_pciv_msghead);

    return pciv_send_msg(0, PCIV_MSGPORT_COMM_CMD, msg);
}

int sample_pciv_get_local_id(td_s32 *local_id)
{
    td_s32 fd = -1;
    struct ot_mcc_handle_attr attr;

    fd = open("/dev/mcc_userdev", O_RDWR);
    if (fd < 0) {
        pciv_printf("open mcc dev fail");
        return -1;
    }

    *local_id = ioctl(fd, OT_MCC_IOC_GET_LOCAL_ID, &attr);
    pciv_printf("pci local id is %d", *local_id);

    attr.target_id = 0;
    attr.port = 0;
    attr.priority = 0;
    ioctl(fd, OT_MCC_IOC_CONNECT, &attr);
    printf("===================close port %d!\n", attr.port);
    close(fd);
    return 0;
}

static td_s32 sample_pciv_get_vb_cfg(ot_size *size, ot_vb_cfg *vb_cfg)
{
    td_u32 blk_size;
    ot_pic_buf_attr pic_buf_attr;

    (td_void)memset_s(vb_cfg, sizeof(ot_vb_cfg), 0, sizeof(ot_vb_cfg));
    sample_pciv_set_buf_attr(size->width, size->height, OT_COMPRESS_MODE_NONE,
        OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420, &pic_buf_attr);
    blk_size = ot_common_get_pic_buf_size(&pic_buf_attr);

    vb_cfg->max_pool_cnt            = 1;
    vb_cfg->common_pool[0].blk_size = blk_size;
    vb_cfg->common_pool[0].blk_cnt  = 4; /* 4: block for processor */

    return TD_SUCCESS;
}

static td_s32 sample_pciv_init_slave_media_sys(td_void)
{
    td_s32 ret;
    ot_vb_cfg vb_cfg = { 0 };
    ot_pic_size pic_size = PIC_360P;
    ot_size size;

    ret = sample_comm_sys_get_pic_size(pic_size, &size);
    if (ret != TD_SUCCESS) {
        pciv_printf("get pic size failed with 0x%x!", ret);
        return ret;
    }

    ret = sample_pciv_get_vb_cfg(&size, &vb_cfg);
    if (ret != TD_SUCCESS) {
        pciv_printf("set vb cfg fail for 0x%x", ret);
    }

    ret = sample_comm_sys_init(&vb_cfg);
    if (ret != TD_SUCCESS) {
        pciv_printf("system init failed with 0x%x", ret);
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 sample_pciv_dma_task(td_bool is_read, td_s32 blk_cnt, ot_pciv_dma_blk *dma_blk)
{
    td_s32              ret;
    ot_pciv_dma_task    dma_task = { 0 };

    dma_task.is_read = is_read;
    dma_task.blk_cnt = blk_cnt;
    dma_task.dma_blk = dma_blk;

    ret = ss_mpi_pciv_dma_task(&dma_task);
    while (ret == OT_ERR_PCIV_TIMEOUT) {
        usleep(PCIV_WAIT_DMA_TIME);
        pciv_printf("---- PCI DMA Wait ----");
        ret = ss_mpi_pciv_dma_task(&dma_task);
    }
    return ret;
}

static td_void sample_pciv_get_rect(td_u32 width, td_u32 height, ot_rect *rect)
{
    rect->width = get_rect_len(width);
    rect->height = get_rect_len(height);
    rect->x = (rand() << 1) % (width - rect->width);
    rect->y = (rand() << 1) % (height - rect->height);
    return;
}

static td_void sample_pciv_deal_co_task(sample_pciv_args_write_done *args,
    td_phys_addr_t phys_addr, td_s32 port)
{
    td_s32 ret;
    ot_pciv_dma_blk dma_blk = { 0 };
    ot_rect rect = {0, 0, 128, 72};
    sample_pciv_msg send_msg = {
        .msg_head = {
            .target = 0,
            .msg_type = SAMPLE_PCIV_MSG_READ_DONE,
            .msg_len = sizeof(ot_rect),
        },
    };

    /* slave do dma read task */
    dma_blk.src_addr = args->addr;
    dma_blk.dst_addr = phys_addr;
    dma_blk.size     = args->blk_size;

    ret = sample_pciv_dma_task(TD_TRUE, 1, &dma_blk);
    if (ret != TD_SUCCESS) {
        send_msg.msg_head.ret_val = TD_FAILURE;
        pciv_printf("lost one dma task, error code:0x%x", ret);
        goto return_msg;
    }
    /* do something with vb_blk, we use the random take the place of ai(in fact ,it is) */
    sample_pciv_get_rect(args->width, args->height, &rect);
    send_msg.msg_head.ret_val = ret;
    ret = memcpy_s(send_msg.c_msg_body, SAMPLE_PCIV_MSG_MAXLEN, &rect, sizeof(ot_rect));
    if (ret != EOK) {
        send_msg.msg_head.ret_val = TD_FAILURE;
        pciv_printf("memcpy_s send msg failed, error code:0x%x", ret);
    }
return_msg:
    ret = pciv_send_msg(0, port, &send_msg);
    if (ret != TD_SUCCESS) {
        pciv_printf("send msg failed, error code:0x%x", ret);
    }
}

static td_void *sample_pciv_slave_co_thread(td_void *p)
{
    sample_pciv_co_thread *co = (sample_pciv_co_thread *)p;
    td_s32 ret;
    ot_vb_blk vb_blk;
    td_char *mmz_name = TD_NULL;
    td_phys_addr_t phys_addr;
    sample_pciv_args_write_done *args = TD_NULL;
    sample_pciv_msg recv_msg;

    prctl(PR_SET_NAME, "co_thread", 0, 0, 0);

    vb_blk = ss_mpi_vb_get_blk(OT_VB_INVALID_POOL_ID, co->blk_size, mmz_name);
    if (vb_blk == OT_VB_INVALID_HANDLE) {
        pciv_printf("mpi vb get blk size:%d fail", co->blk_size);
        return TD_NULL;
    }
    phys_addr = ss_mpi_vb_handle_to_phys_addr(vb_blk);
    if (phys_addr == 0) {
        pciv_printf("mpi vb handle to phys addr fail");
        ss_mpi_vb_release_blk(vb_blk);
        return TD_NULL;
    }

    while (co->is_start && g_signo_flag) {
        ret = pciv_read_msg(0, co->port, &recv_msg);
        if (ret != TD_SUCCESS || recv_msg.msg_head.msg_type != SAMPLE_PCIV_MSG_WRITE_DONE) {
            usleep(PCIV_WAIT_MSG_TIME);
            continue;
        }
        args = (sample_pciv_args_write_done *)recv_msg.c_msg_body;
        sample_pciv_deal_co_task(args, phys_addr, co->port);
    }

    ss_mpi_vb_release_blk(vb_blk);
    return TD_NULL;
}

static td_s32 sample_pciv_slave_init_coprocessor(sample_pciv_msg *msg)
{
    td_u32 id;
    sample_pciv_args_co *co = (sample_pciv_args_co *)msg->c_msg_body;

    pciv_check_null_return(co);
    id = co->id;
    if (id >= SAMPLE_PCIV_CHN_PER_CHIP) {
        pciv_printf("id is invalid");
        return TD_FAILURE;
    }
    g_co_thrad[id].id       = id;
    g_co_thrad[id].is_start = TD_TRUE;
    g_co_thrad[id].blk_size = co->blk_size;
    g_co_thrad[id].port     = co->port;

    pciv_check_err_return(pciv_open_msg_port(0, co->port));
    pthread_create(&g_co_thrad[id].pid, TD_NULL, sample_pciv_slave_co_thread, &g_co_thrad[id]);
    return TD_SUCCESS;
}

static td_s32 sample_pciv_slave_exit_coprocessor(sample_pciv_msg *msg)
{
    td_u32 id;
    sample_pciv_args_co *co = (sample_pciv_args_co *)msg->c_msg_body;

    pciv_check_null_return(co);
    id = co->id;
    if (id >= SAMPLE_PCIV_CHN_PER_CHIP) {
        pciv_printf("id is invalid");
        return TD_FAILURE;
    }
    if (g_co_thrad[id].is_start == TD_TRUE) {
        g_co_thrad[id].is_start = TD_FALSE;
        pthread_join(g_co_thrad[id].pid, TD_NULL);
    }

    pciv_check_err_return(pciv_close_msg_port(0, g_co_thrad[id].port));
    return TD_SUCCESS;
}

static td_char *pciv_msg_print_type(sample_pciv_msg_type type)
{
    switch (type) {
        case SAMPLE_PCIV_MSG_INIT_COPROCESSOR:
            return "init coprocessor";
        case SAMPLE_PCIV_MSG_EXIT_COPROCESSOR:
            return "exit coprocessor";
        case SAMPLE_PCIV_MSG_QUIT:
            return "normal quite";
        default:
            return "invalid type";
    }
}

static td_s32 sample_pciv_deal_with_msg_type(sample_pciv_msg_type type, sample_pciv_msg *msg)
{
    switch (type) {
        case SAMPLE_PCIV_MSG_INIT_COPROCESSOR:
            return sample_pciv_slave_init_coprocessor(msg);
        case SAMPLE_PCIV_MSG_EXIT_COPROCESSOR:
            return sample_pciv_slave_exit_coprocessor(msg);
        case SAMPLE_PCIV_MSG_QUIT:
            g_is_quit = TD_TRUE;
            return TD_SUCCESS;
        default:
            pciv_printf("invalid msg, type:%d", type);
            return TD_FAILURE;
    }
}

static td_s32 sample_pciv_init_slave()
{
    td_s32 ret;
    ot_pciv_window_base pci_base_window;

    (td_void)memset_s(&pci_base_window, sizeof(ot_pciv_window_base), 0, sizeof(ot_pciv_window_base));

    sample_pciv_get_local_id(&g_pci_local_id);

    /* wait for pci host ... */
    ret = pciv_wait_connect(0);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    pciv_printf("g_pci_local_id=%d", g_pci_local_id);

    /* open pci msg port for common cmd */
    ret = pciv_open_msg_port(0, PCIV_MSGPORT_COMM_CMD);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = sample_pciv_init_slave_media_sys();
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* get PF Window info of this pci device */
    pci_base_window.chip_id = 0;
    ret = ss_mpi_pciv_get_window_base(0, &pci_base_window);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    pciv_printf("PF AHB Addr:0x%lx", (td_ulong)pci_base_window.pf_ahb_addr);
    return ret;
}

static td_void sample_pciv_exit_slave()
{
    /* close pci msg port for common cmd */
    (td_void)pciv_close_msg_port(0, PCIV_MSGPORT_COMM_CMD);

    /* exit */
    sample_comm_sys_exit();
}

static td_void sample_pciv_handle_sig(td_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_signo_flag = TD_FALSE;
    }
}

int main(void)
{
    td_s32 ret;
    td_u32 msg_type;
    sample_pciv_msg msg;

    g_signo_flag = TD_TRUE;
    sample_sys_signal(&sample_pciv_handle_sig);

    ret = sample_pciv_init_slave();
    if (ret != TD_SUCCESS) {
        return ret;
    }

    while (g_signo_flag) {
        g_echo_msg_len = 0;
        ret = pciv_read_msg(0, PCIV_MSGPORT_COMM_CMD, &msg);
        if (ret != TD_SUCCESS) {
            usleep(PCIV_WAIT_MSG_TIME);
            continue;
        }

        msg_type = msg.msg_head.msg_type;
        printf("\nreceive msg, MsgType:(%d, %s) \n", msg_type, pciv_msg_print_type(msg_type));
        ret = sample_pciv_deal_with_msg_type(msg_type, &msg);
        ret = sample_pciv_echo_msg(ret, &msg);
        if (g_is_quit || ret != TD_SUCCESS) {
            break;
        }
    }
    sample_pciv_exit_slave();
    if (!g_signo_flag || ret != TD_SUCCESS) {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        return TD_FAILURE;
    } else {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
        return TD_SUCCESS;
    }
}

