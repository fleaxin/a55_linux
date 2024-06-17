/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include "securec.h"
#include <stdlib.h>
#include <memory.h>
#include "ot_type.h"
#include "uvc_venc_glue.h"

#define MULTI_PAYLOAD_HEADER_SIZE 0x1A

void add_multi_payload_header(frame_node_t *node, unsigned long stream_type)
{
    unsigned char *mem = node->mem;

    (td_void)memset_s(mem, MULTI_PAYLOAD_HEADER_SIZE, 0, MULTI_PAYLOAD_HEADER_SIZE);

    // all bytes need to be filled in little endian
    // version
    mem[0] = 0x00;
    mem[1] = 0x01;

    // header length
    mem[2] = 0x16; // [2]:header length
    mem[3] = 0x00; // [3]:header length
    // stream type
    mem[4] = stream_type & 0xff;                 // [4]:stream_type
    mem[5] = ((stream_type & 0xff00) >> 8);      // [5]:stream_type 8 bit
    mem[6] = ((stream_type & 0xff0000) >> 16);   // [6]:stream_type 16 bit
    mem[7] = ((stream_type & 0xff000000) >> 24); // [7]:stream_type 24 bit
    node->used += MULTI_PAYLOAD_HEADER_SIZE;
}

void set_multi_payload_size(frame_node_t *node)
{
    unsigned long payload_size = node->used - 0x1A;
    unsigned char *payload_mem = node->mem + 0x16;

    // filled value in little endian
    payload_mem[0] = payload_size & 0xff;                 // [0]:payload_size
    payload_mem[1] = ((payload_size & 0xff00) >> 8);      // [1]:payload_size 8 bit
    payload_mem[2] = ((payload_size & 0xff0000) >> 16);   // [2]:payload_size 16 bit
    payload_mem[3] = ((payload_size & 0xff000000) >> 24); // [3]:payload_size 24 bit
}
