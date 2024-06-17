/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "pciv_trans.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "ot_common_pciv.h"
#include "ot_common_vb.h"
#include "pciv_msg.h"
#include "ss_mpi_sys.h"
#include "ss_mpi_vb.h"
#include "ss_mpi_pciv.h"
#include "sample_pciv_comm.h"
#include "securec.h"

#define PCIV_TRANS_MSG_WRITE_DONE 18
#define PCIV_TRANS_MSG_READ_DONE  19
#define PCIV_TRANS_SLEEP_TIME     10000
#define PCIV_TRANS_PRINT_FRE      2001
#define PCIV_TRANS_SENDER_ALW     50

static td_void *pciv_trans_sender_thread(td_void *p);

/*
 * if the buffer tail is not enough then go to the head.
 * and the tail will not be used
 */
static td_s32 trans_get_write_offset(const pciv_trans_rmtbuf *buf, td_s32 size, td_s32 *offset)
{
    td_bool flag = TD_FALSE;
    td_s32 ret = TD_FAILURE;
    td_s32 read_pos;

    *offset = -1;
    /*
     * the circule buffer must be twice as much as the data length that the writepos
     * and readpos can be set rightly as the rule below
     */
    if (size > (buf->length * 2)) { /* 2: if size is more than half of buf length, return FAILURE */
        flag = TD_TRUE;
        return TD_FAILURE;
    }
    if (flag) {
        flag = TD_FALSE;
    }

    /* get the read_pos */
    read_pos = buf->read_pos;

    if (buf->write_pos >= read_pos) {
        if ((buf->write_pos + size) <= (buf->length)) {
            *offset = buf->write_pos;
            ret = TD_SUCCESS;
        } else if (size < read_pos) {
            *offset = 0;
            ret = TD_SUCCESS;
        } else {
        }
    } else {
        if ((buf->write_pos + size) < (read_pos)) {
            *offset = buf->write_pos;
            ret = TD_SUCCESS;
        } else {
        }
    }

    return ret;
}

static td_void trans_upd_write_pos(pciv_trans_sender *sender,
    td_s32 size, td_s32 offset)
{
    sender->rmt_buf.write_pos = offset + size;
    return;
}

static td_void trans_upd_read_pos(pciv_trans_sender *sender, td_s32 offset)
{
    sender->rmt_buf.read_pos = offset;
}

static td_s32 pciv_trans_init_sender_param(ot_vb_blk vb_blk, td_phys_addr_t phys_addr,
    const pciv_trans_attr *attr, pciv_trans_sender *sender)
{
    td_u8 *virt_addr = TD_NULL;

    virt_addr = (td_u8 *)ss_mpi_sys_mmap(phys_addr, attr->buf_size);
    if (virt_addr == TD_NULL) {
        printf("func:%s, info:mpi sys mmap fail\n", __FUNCTION__);
        return TD_FAILURE;
    }

    sender->loc_buf.phys_addr = phys_addr;
    sender->loc_buf.base_addr = virt_addr;
    sender->loc_buf.vb_blk = vb_blk;
    sender->loc_buf.cur_len = 0;
    sender->loc_buf.buf_len = attr->buf_size;
    sender->msg_port_write = attr->msg_port_write;
    sender->msg_port_read = attr->msg_port_read;
    sender->rmt_chip = attr->rmt_chip;
    sender->init = TD_TRUE;
    printf("%s ok, handle:0x%p, remote:%d, dma_buf:0x%lx,len:%d; tmp_buf:(0x%lx, %p), msgport:(%d, %d)\n",
           __FUNCTION__, sender, sender->rmt_chip, (td_ulong)sender->rmt_buf.base_addr, sender->rmt_buf.length,
           (td_ulong)sender->loc_buf.phys_addr, sender->loc_buf.base_addr, sender->msg_port_write,
           sender->msg_port_read);
    return TD_SUCCESS;
}

td_s32 pciv_trans_init_sender(const pciv_trans_attr *attr, td_void **pp_sender)
{
    td_phys_addr_t phys_addr;
    ot_vb_blk vb_blk;
    pciv_trans_sender *sender = TD_NULL;
    td_char *mmz_name = TD_NULL;
    td_s32 ret;

    pciv_check_expr_return(attr != TD_NULL);
    pciv_check_expr_return(pp_sender != TD_NULL);

    sender = (pciv_trans_sender *)malloc(sizeof(pciv_trans_sender));
    if (sender == TD_NULL) {
        printf("malloc sender failed\n");
        return TD_FAILURE;
    }
    (td_void)memset_s(sender, sizeof(pciv_trans_sender), 0, sizeof(pciv_trans_sender));

    /* init buffer info for dest */
    sender->rmt_buf.base_addr = attr->phys_addr;
    sender->rmt_buf.length = attr->buf_size;
    sender->rmt_buf.read_pos = 0;
    sender->rmt_buf.write_pos = 0;

    /* malloc memory for local tmp buffer */
    vb_blk = ss_mpi_vb_get_blk(OT_VB_INVALID_POOL_ID, attr->buf_size, mmz_name);
    if (vb_blk == OT_VB_INVALID_HANDLE) {
        printf("func:%s, info:mpi vb get blk(size:%d) fail\n", __FUNCTION__, attr->buf_size);
        free(sender);
        return TD_FAILURE;
    }

    phys_addr = ss_mpi_vb_handle_to_phys_addr(vb_blk);
    if (phys_addr == 0) {
        printf("func:%s, info:mpi vb handle to phys addr fail\n", __FUNCTION__);
        ss_mpi_vb_release_blk(vb_blk);
        free(sender);
        return TD_FAILURE;
    }

    ret = pciv_trans_init_sender_param(vb_blk, phys_addr, attr, sender);
    if (ret != TD_SUCCESS) {
        ss_mpi_vb_release_blk(vb_blk);
        free(sender);
        return ret;
    }

    /* create thread to process read_done message from receiver */
    sender->thread_start = TD_TRUE;
    pthread_create(&sender->pid, TD_NULL, pciv_trans_sender_thread, sender);

    *pp_sender = sender;
    return TD_SUCCESS;
}

td_s32 pciv_trans_init_receiver(const pciv_trans_attr *attr, td_void **pp_receiver)
{
    pciv_trans_receiver *rev = TD_NULL;
    td_void *vir_addr = TD_NULL;

    pciv_check_expr_return(attr != TD_NULL);
    pciv_check_expr_return(pp_receiver != TD_NULL);

    rev = (pciv_trans_receiver *)malloc(sizeof(pciv_trans_receiver));
    if (rev == TD_NULL) {
        printf("malloc receiver failed\n");
        return TD_FAILURE;
    }
    (td_void)memset_s(rev, sizeof(pciv_trans_receiver), 0, sizeof(pciv_trans_receiver));

    /* this address is used in user mode, so we need to map it */
    vir_addr = (td_void *)ss_mpi_sys_mmap(attr->phys_addr, attr->buf_size);
    if (vir_addr == TD_NULL) {
        free(rev);
        return TD_FAILURE;
    }

    rev->buf_base_addr = (td_u8 *)vir_addr;
    rev->buf_len = attr->buf_size;
    rev->rmt_chip = attr->rmt_chip;
    rev->msg_port_wirte = attr->msg_port_write;
    rev->msg_port_read = attr->msg_port_read;
    rev->init = TD_TRUE;

    printf("%s ok, remote:%d, addr:(0x%lx, %p), buflen:%d, msgport:(%d, %d)\n",
           __FUNCTION__, rev->rmt_chip, (td_ulong)attr->phys_addr, rev->buf_base_addr,
           rev->buf_len, rev->msg_port_wirte, rev->msg_port_read);
    *pp_receiver = (td_void *)rev;
    return TD_SUCCESS;
}

td_s32 pciv_trans_de_init_sender(td_void *sender)
{
    pciv_trans_sender *trans_sender = (pciv_trans_sender *)sender;

    if (!trans_sender->init) {
        return TD_SUCCESS;
    }

    if (trans_sender->thread_start) {
        trans_sender->thread_start = TD_FALSE;
        pthread_join(trans_sender->pid, TD_NULL);
    }

    /* release temp buffer */
    (td_void)ss_mpi_sys_munmap(trans_sender->loc_buf.base_addr, trans_sender->loc_buf.buf_len);
    trans_sender->loc_buf.base_addr = TD_NULL;

    (td_void)ss_mpi_vb_release_blk(trans_sender->loc_buf.vb_blk);
    trans_sender->init = TD_FALSE;
    free(trans_sender);
    return TD_SUCCESS;
}

td_s32 pciv_trans_de_init_receiver(td_void *receiver)
{
    pciv_trans_receiver *rev = (pciv_trans_receiver *)receiver;

    if (!rev->init) {
        return TD_SUCCESS;
    }

    (td_void)ss_mpi_sys_munmap(rev->buf_base_addr, rev->buf_len);

    rev->init = TD_FALSE;
    free(rev);
    return TD_SUCCESS;
}

td_s32 pciv_trans_query_loc_buf(const td_void *sender, pciv_trans_locbuf_stat *status)
{
    pciv_trans_sender *trans_sender = (pciv_trans_sender *)sender;

    pciv_check_expr_return(trans_sender->loc_buf.cur_len <= trans_sender->loc_buf.buf_len);

    status->free_len = trans_sender->loc_buf.buf_len - trans_sender->loc_buf.cur_len;

    return TD_SUCCESS;
}

td_s32 pciv_trans_write_loc_buf(td_void *sender, const td_u8 *addr, td_u32 len)
{
    td_s32            ret;
    pciv_trans_sender *trans_sender = (pciv_trans_sender *)sender;

    if ((trans_sender->loc_buf.cur_len + len) > trans_sender->loc_buf.buf_len) {
        printf("local buf full !!, curpos:%d, len:%d, buflen:%d\n",
               trans_sender->loc_buf.cur_len, len, trans_sender->loc_buf.buf_len);
        return TD_FAILURE;
    }

    ret = memcpy_s(trans_sender->loc_buf.base_addr + trans_sender->loc_buf.cur_len,
        trans_sender->loc_buf.buf_len - trans_sender->loc_buf.cur_len, addr, len);
    if (ret != EOK) {
        printf("pciv_trans_write_loc_buf copy addr failed!\n");
        return TD_FAILURE;
    }
    trans_sender->loc_buf.cur_len += len;

    return TD_SUCCESS;
}

td_s32 pciv_trans_send_data(td_void *sender)
{
    td_s32 ret;
    td_s32 write_off = 0;
    sample_pciv_msg msg_send;
    pciv_trans_notify *stream_info = TD_NULL;
    ot_pciv_dma_task task;
    ot_pciv_dma_blk dma_blk[OT_PCIV_MAX_DMA_BLK];
    pciv_trans_sender *trans_sender = (pciv_trans_sender *)sender;

    /* get written offset (if there is not free space to write,return failure) */
    if (trans_get_write_offset(&trans_sender->rmt_buf, trans_sender->loc_buf.cur_len, &write_off)) {
        return TD_FAILURE;
    }

    /* call the driver to send on frame. */
    dma_blk[0].size = trans_sender->loc_buf.cur_len;
    dma_blk[0].src_addr = trans_sender->loc_buf.phys_addr;
    dma_blk[0].dst_addr = trans_sender->rmt_buf.base_addr + write_off;
    task.dma_blk = &dma_blk[0];
    task.blk_cnt = 1;
    task.is_read = TD_FALSE;

    ret = ss_mpi_pciv_dma_task(&task);
    while (ret == OT_ERR_PCIV_TIMEOUT) {
        usleep(PCIV_TRANS_SLEEP_TIME);
        printf("---- PCI DMA wait ----\n");
        ret = ss_mpi_pciv_dma_task(&task);
    }
    if (ret != TD_SUCCESS) {
        printf("func:%s -> dma task fail,s32ret= 0x%x\n", __FUNCTION__, ret);
        return TD_FAILURE;
    }

    /* update write pos after data have write */
    trans_upd_write_pos(trans_sender, trans_sender->loc_buf.cur_len, write_off);

    /* send write_done message to notify the receiver */
    msg_send.msg_head.msg_type = PCIV_TRANS_MSG_WRITE_DONE;
    msg_send.msg_head.msg_len = sizeof(pciv_trans_notify);
    msg_send.msg_head.target = trans_sender->rmt_chip;
    stream_info = (pciv_trans_notify *)msg_send.c_msg_body;
    stream_info->start = write_off;
    stream_info->end = write_off + trans_sender->loc_buf.cur_len;
    stream_info->seq = trans_sender->msg_seq_send++;
    ret = pciv_send_msg(msg_send.msg_head.target, trans_sender->msg_port_write, &msg_send);
    pciv_check_expr_return(ret != TD_FAILURE);

    if (trans_sender->msg_seq_send % PCIV_TRANS_PRINT_FRE == 0) {
        printf("pciv_trans_send_data -> w:%d, r:%d, offset:%d, len:%d \n",
            trans_sender->rmt_buf.write_pos, trans_sender->rmt_buf.read_pos,
            write_off, trans_sender->loc_buf.cur_len);
    }

    trans_sender->loc_buf.cur_len = 0;
    return TD_SUCCESS;
}

static td_void *pciv_trans_sender_thread(td_void *p)
{
    td_s32 ret;
    sample_pciv_msg msg_rev;
    pciv_trans_notify *stream_info = TD_NULL;
    pciv_trans_sender *sender = (pciv_trans_sender *)p;
    td_s32 rmt_chip = sender->rmt_chip;
    td_s32 msg_port_read = sender->msg_port_read;

    prctl(PR_SET_NAME, "ot_pciv_send", 0, 0, 0);

    while (sender->thread_start) {
        (td_void)memset_s(&msg_rev, sizeof(msg_rev), 0, sizeof(msg_rev));

        /* read read_done message from stream receiver */
        ret = pciv_read_msg(rmt_chip, msg_port_read, &msg_rev);
        if (ret != TD_SUCCESS) {
            usleep(PCIV_TRANS_SLEEP_TIME);
            continue;
        }
        if (msg_rev.msg_head.msg_type != PCIV_TRANS_MSG_READ_DONE) {
            printf("func:%s -> dma task fail,s32ret= 0x%x\n", __FUNCTION__, ret);
            continue;
        }

        /* get stream info in msg body */
        stream_info = (pciv_trans_notify *)&msg_rev.c_msg_body;

        /* check message sequence number */
        if (stream_info->seq - sender->msg_seq_free > 1) {
            printf("%d,%d, start:%d,end:%d \n",
                   stream_info->seq, sender->msg_seq_free,
                   stream_info->start, stream_info->end);
        }
        if (stream_info->seq - sender->msg_seq_free > 1) {
            printf("func %s: the diff of send and free is more than ones\n", __FUNCTION__);
            continue;
        }
        sender->msg_seq_free = stream_info->seq;
        /*
         * the greater the gap between sequence number of send message and
         * release message ,with greater delay the receiver get the data
         */
        if (sender->msg_seq_send - sender->msg_seq_free > PCIV_TRANS_SENDER_ALW) {
            printf("warnning: send:%d,free:%d \n", sender->msg_seq_send, sender->msg_seq_free);
        }

        /* if read_done message is received then update the read position of remote buffer */
        trans_upd_read_pos(sender, stream_info->end);
    }

    return TD_NULL;
}

td_s32 pciv_trans_get_data(td_void *receiver, td_u8 **addr, td_u32 *len)
{
    td_s32 ret;
    sample_pciv_msg msg_rev = { 0 };
    pciv_trans_notify *stream_info = TD_NULL;
    pciv_trans_receiver *trans_receiver = (pciv_trans_receiver *)receiver;

    pciv_check_expr_return(trans_receiver->init);
    /* receive write_done message from sender */
    ret = pciv_read_msg(trans_receiver->rmt_chip, trans_receiver->msg_port_wirte, &msg_rev);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    pciv_check_expr_return(msg_rev.msg_head.msg_type == PCIV_TRANS_MSG_WRITE_DONE);

    /* get stream info in msg body */
    stream_info = (pciv_trans_notify *)&msg_rev.c_msg_body;

    if (!(stream_info->end > stream_info->start)
        && (stream_info->end <= trans_receiver->buf_len)) {
        printf("func:%s,line:%d\n", __FUNCTION__, __LINE__);
        return -1;
    }

    *addr = trans_receiver->buf_base_addr + stream_info->start;
    *len = stream_info->end - stream_info->start;

    if (!(stream_info->seq - trans_receiver->msg_seq_send <= 1)) {
        printf("func:%s failed, line:%d\n", __FUNCTION__, __LINE__);
        return -1;
    }

    /* test and check the sequence num of the message */
    if (stream_info->seq - trans_receiver->msg_seq_send > 1) {
        printf("%d,%d, start:%d,end:%d \n", stream_info->seq, trans_receiver->msg_seq_send,
               stream_info->start, stream_info->end);
    }
    pciv_check_expr_return(stream_info->seq - trans_receiver->msg_seq_send <= 1);
    trans_receiver->msg_seq_send = stream_info->seq;
    return TD_SUCCESS;
}

td_s32 pciv_trans_release_data(td_void *receiver, const td_u8 *addr, td_u32 len)
{
    td_s32 ret;
    sample_pciv_msg msg_rev;
    pciv_trans_notify *stream_info = TD_NULL;
    pciv_trans_receiver *trans_receiver = (pciv_trans_receiver *)receiver;

    pciv_check_expr_return(trans_receiver->init);

    /* send msg to sender, we have used the data over */
    stream_info = (pciv_trans_notify *)msg_rev.c_msg_body;
    stream_info->start = addr - trans_receiver->buf_base_addr;
    stream_info->end = stream_info->start + len;
    stream_info->seq = trans_receiver->msg_seq_free++;

    msg_rev.msg_head.msg_type = PCIV_TRANS_MSG_READ_DONE;
    msg_rev.msg_head.msg_len = sizeof(pciv_trans_notify);
    msg_rev.msg_head.target = trans_receiver->rmt_chip;
    ret = pciv_send_msg(trans_receiver->rmt_chip, trans_receiver->msg_port_read, &msg_rev);
    pciv_check_expr_return(ret != TD_FAILURE);

    return TD_SUCCESS;
}

