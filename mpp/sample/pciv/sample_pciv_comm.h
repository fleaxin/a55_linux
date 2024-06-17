/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_PCIV_COMM_H
#define SAMPLE_PCIV_COMM_H

#include "ot_mcc_usrdev.h"
#include "sample_comm.h"
#include "pciv_msg.h"
#include "pciv_trans.h"

#include "ot_common_pciv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_PCIV_CHN_PER_CHIP 4
#define SAMPLE_PCIV_STREAM_PATH        "./"
#define SAMPLE_PCIV_STREAM_FILE        "3840x2160_8bit.h264"

#define PCIV_WAIT_MSG_TIME 10000
#define PCIV_WAIT_DMA_TIME 1000

#define PCIV_REPEAT_MAX_TIME 300

typedef enum {
    SAMPLE_PCIV_MSG_INIT_COPROCESSOR,
    SAMPLE_PCIV_MSG_EXIT_COPROCESSOR,

    SAMPLE_PCIV_MSG_WRITE_DONE,
    SAMPLE_PCIV_MSG_READ_DONE,

    SAMPLE_PCIV_MSG_QUIT,

    SAMPLE_PCIV_MSG_ECHO,

    SAMPLE_PCIV_MSG_BUTT
} sample_pciv_msg_type;

typedef struct {
    td_s32    id;
    td_s32    port;
    td_u32    blk_size;
} sample_pciv_args_co;

typedef struct {
    td_u32          width;
    td_u32          height;
    td_u32          blk_size;
    td_phys_addr_t  addr;
} sample_pciv_args_write_done;

typedef struct {
    td_s32    id;
    pthread_t pid;
    td_s32    chip;
    td_u32    blk_size;
    td_s32    port;
    td_bool   is_start;
} sample_pciv_co_thread;

__inline static td_void sample_pciv_set_buf_attr(td_u32 width, td_u32 height,
    ot_compress_mode compress_mode, ot_pixel_format pixel_format, ot_pic_buf_attr *buf_attr)
{
    buf_attr->align = OT_DEFAULT_ALIGN;
    buf_attr->bit_width = OT_DATA_BIT_WIDTH_8;
    buf_attr->compress_mode = compress_mode;
    buf_attr->pixel_format = pixel_format;
    buf_attr->width = width;
    buf_attr->height = height;
    return;
}

#define pciv_check_err_return(err) \
do { \
    td_s32 err_ = err; \
    if (err_ != TD_SUCCESS) { \
        printf("\033[0;31mSample Pciv err:%x, Func:%s, Line:%d\033[0;39m\n", err, __FUNCTION__, __LINE__); \
        return err_; \
    } \
} while (0)

#define pciv_check_null_return(ptr) \
do { \
    if ((ptr) == TD_NULL) { \
        printf("\033[0;31mptr is null, Func:%s, Line:%d\033[0;39m\n", __FUNCTION__, __LINE__); \
        return TD_FAILURE; \
    } \
} while (0)


#define pciv_printf(...) \
do { \
    printf("Func:%s Line:%d\n", __FUNCTION__, __LINE__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
} while (0)

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif

