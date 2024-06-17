/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PCIV_TRANS_H
#define PCIV_TRANS_H

#include <pthread.h>

#include "ot_common.h"
#include "ot_type.h"
#include "ot_common_vb.h"

typedef struct {
    td_s32 rmt_chip;
    td_bool reciver; /* receiver or sender ? */
    td_u32 buf_size;
    td_phys_addr_t phys_addr;
    td_s32 msg_port_write;
    td_s32 msg_port_read;
    td_s32 chn_id;
} pciv_trans_attr;

/* buffer information of remote receiver */
typedef struct {
    td_phys_addr_t base_addr; /* physic address of remote buffer */
    td_s32 length;            /* buffer length */
    td_s32 read_pos;          /* read position */
    td_s32 write_pos;         /* write position */
} pciv_trans_rmtbuf;

/* buffer information of local sender */
typedef struct {
    ot_vb_blk vb_blk;         /* VB of buffer */
    td_u8 *base_addr;         /* virtual address */
    td_phys_addr_t phys_addr; /* physic address */
    td_u32 buf_len;           /* buffer length */
    td_u32 cur_len;           /* current data length in buffer */
} pciv_trans_locbuf;

/* the notify message when a new frame is writed */
typedef struct {
    td_u32 start; /* write or read start position of this frame */
    td_u32 end;   /* write or read end position of this frame */
    td_u32 seq;
} pciv_trans_notify;

typedef struct {
    td_bool init;
    td_s32 rmt_chip;
    td_u8 *buf_base_addr;  /* virtual address of receiver buffer */
    td_u32 buf_len;        /* length of receiver buffer */
    td_s32 msg_port_wirte; /* message port for write_done */
    td_s32 msg_port_read;  /* message port for read_done */
    td_u32 msg_seq_send;
    td_u32 msg_seq_free;
} pciv_trans_receiver;

typedef struct {
    td_bool init;
    td_s32 rmt_chip;
    pciv_trans_locbuf loc_buf; /* buffer info of local source */
    pciv_trans_rmtbuf rmt_buf; /* buffer info of remote receiver */
    td_s32 msg_port_write;     /* message port for write_done */
    td_s32 msg_port_read;      /* message port for read_done */
    pthread_t pid;
    td_bool thread_start;
    td_u32 msg_seq_send;
    td_u32 msg_seq_free;
} pciv_trans_sender;

typedef struct {
    td_u32 free_len; /* free length of local buffer to write */
} pciv_trans_locbuf_stat;

td_void pciv_trans_init(td_void);

td_s32 pciv_trans_init_sender(const pciv_trans_attr *attr, td_void **pp_sender);
td_s32 pciv_trans_de_init_sender(td_void *sender);
td_s32 pciv_trans_query_loc_buf(const td_void *sender, pciv_trans_locbuf_stat *status);
td_s32 pciv_trans_write_loc_buf(td_void *sender, const td_u8 *addr, td_u32 len);
td_s32 pciv_trans_send_data(td_void *sender);

td_s32 pciv_trans_init_receiver(const pciv_trans_attr *attr, td_void **pp_receiver);
td_s32 pciv_trans_de_init_receiver(td_void *receiver);
td_s32 pciv_trans_get_data(td_void *receiver, td_u8 **addr, td_u32 *len);
td_s32 pciv_trans_release_data(td_void *receiver, const td_u8 *addr, td_u32 len);

#endif
