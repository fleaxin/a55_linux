/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include "frame_cache.h"
#include "ot_camera.h"
#include "sample_comm.h"
#include "sample_venc.h"
#include "uvc_media.h"
#include "uvc.h"

#define SAMPLE_UVC_MAX_STREAM_NAME_LEN 64

#define SAMPLE_RETURN_CONTINUE  1
#define SAMPLE_RETURN_BREAK     2

typedef struct {
    FILE *file[OT_VENC_MAX_CHN_NUM];
    td_s32 venc_fd[OT_VENC_MAX_CHN_NUM];
    td_s32 maxfd;
    td_u32 picture_cnt[OT_VENC_MAX_CHN_NUM];
    ot_venc_chn venc_chn;
    td_char file_postfix[10]; /* 10 :file_postfix number */
    td_s32 chn_total;
    ot_payload_type pay_load_type[OT_VENC_MAX_CHN_NUM];
} sample_comm_venc_stream_proc_info;

static ot_payload_type change_to_mpp_format(uint32_t fcc)
{
    ot_payload_type t;

    switch (fcc) {
        case VIDEO_IMG_FORMAT_YUYV:
        case VIDEO_IMG_FORMAT_NV12:
        case VIDEO_IMG_FORMAT_NV21:
        case VIDEO_IMG_FORMAT_MJPEG:
            t = OT_PT_MJPEG;
            break;

        case VIDEO_IMG_FORMAT_H264:
            t = OT_PT_H264;
            break;

        case VIDEO_IMG_FORMAT_H265:
            t = OT_PT_H265;
            break;

        default:
            t = OT_PT_MJPEG;
            break;
    }

    return t;
}

static ot_pic_size change_to_mpp_wh(int width)
{
    ot_pic_size s;

    switch (width) {
        case 640:           /* 640: width */
            s = PIC_360P;   /* 640 x 360 */
            break;
        case 1280:          /* 1280: width */
            s = PIC_720P;
            break;
        case 1920:          /* 1920: width */
            s = PIC_1080P;
            break;
        case 3840:          /* 3840: width */
            s = PIC_3840X2160;
            break;
        default:
            s = PIC_720P;
            break;
    }

    return s;
}

static td_void set_config_format(ot_payload_type *format, int idx)
{
    encoder_property property;
    sample_uvc_get_encoder_property(&property);

    format[idx] = change_to_mpp_format(property.format);
}

static td_void set_config_wh(ot_pic_size *wh, int idx)
{
    encoder_property property;
    sample_uvc_get_encoder_property(&property);

    wh[idx] = change_to_mpp_wh(property.width);
}

td_void set_user_config_format(ot_payload_type *format, ot_pic_size *wh, int *c)
{
    set_config_format(format, 0);
    set_config_wh(wh, 0);
    *c = 1;
}

int is_channel_yuv(int channel)
{
    encoder_property property;
    sample_uvc_get_encoder_property(&property);

    if ((channel == 1) &&
        ((property.format == VIDEO_IMG_FORMAT_YUYV) ||
        (property.format == VIDEO_IMG_FORMAT_NV12) ||
        (property.format == VIDEO_IMG_FORMAT_NV21))) {
        return 1;
    }

    return 0;
}

