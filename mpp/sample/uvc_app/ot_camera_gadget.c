/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <linux/usb/ch9.h>
#include "securec.h"

#include "ot_camera.h"
#include "uvc.h"
#include "uvc_venc_glue.h"
#include "ot_stream.h"
#include "ot_ctrl.h"

#undef OT_DEBUG
#ifdef OT_DEBUG
#define DEBUG printf
#else
#define DEBUG nothing
#endif

#define ERR_INFO printf
#define RIGHT_SHIFT_8BIT 8
#define UDC_PATH_LENGTH 389
#define GET_MAX_PACKET_SIZE_PATH 398

static struct ot_uvc_dev *g_ot_cd = 0;

td_void nothing() {}

static const td_char *to_string(td_u32 u32_format)
{
    switch (u32_format) {
        case VIDEO_IMG_FORMAT_H264:
            return "H264";
        case VIDEO_IMG_FORMAT_H265:
            return "H265";
        case VIDEO_IMG_FORMAT_MJPEG:
            return "MJPEG";
        case VIDEO_IMG_FORMAT_YUYV:
            return "YUYV";
        case VIDEO_IMG_FORMAT_YUV420:
            return "YUV420";
        case VIDEO_IMG_FORMAT_NV21:
            return "NV21";
        case VIDEO_IMG_FORMAT_NV12:
            return "NV12";
        default:
            return "unknown format";
    }
}

static const td_char *get_code(td_s32 s32_code)
{
    if (s32_code == 0x01) { // 0x01 == SET_CUR
        return "SET_CUR";
    } else if (s32_code == 0x81) { // 0x81 == GET_CUR
        return "GET_CUR";
    } else if (s32_code == 0x82) { // 0x82 == GET_MIN
        return "GET_MIN";
    } else if (s32_code == 0x83) { // 0x83 == GET_MAX
        return "GET_MAX";
    } else if (s32_code == 0x84) { // 0x84 == GET_RES
        return "GET_RES";
    } else if (s32_code == 0x85) { // 0x85 == GET_LEN
        return "GET_LEN";
    } else if (s32_code == 0x86) { // 0x86 == GET_INFO
        return "GET_INFO";
    } else if (s32_code == 0x87) { // 0x87 == GET_DEF
        return "GET_DEF";
    } else {
        return "UNKNOWN";
    }
}

static const td_char *get_intf_cs_string(td_s32 s32_code)
{
    if (s32_code == 0x01) {
        return "PROB_CONTROL";
    } else if (s32_code == 0x02) {
        return "COMMIT_CONTROL";
    } else {
        return "UNKNOWN";
    }
}

static struct ot_uvc_dev *ot_uvc_init(const td_char *dev_name)
{
    struct ot_uvc_dev *ot_dev = NULL;
    struct video_ability cap;
    td_s32 s32_ret;
    td_s32 s32_fd;

    s32_fd = open(dev_name, O_RDWR | O_NONBLOCK);
    if (s32_fd == -1) {
        ERR_INFO("v4l2 open failed(%s): %d\n", dev_name, errno);
        return NULL;
    }

    s32_ret = ioctl(s32_fd, VIDEO_IOCTL_QUERY_CAP, &cap);
    if (s32_ret < 0) {
        ERR_INFO("unable to query device: %d\n", errno);
        close(s32_fd);
        return NULL;
    }

    DEBUG("open succeeded(%s:caps=0x%04x)\n", dev_name, cap.dw_caps);

    if (!(cap.dw_caps & 0x02)) {
        close(s32_fd);
        return NULL;
    }

    DEBUG("device is %s on bus %s\n", cap.a_card, cap.a_bus_info);

    ot_dev = (struct ot_uvc_dev *)malloc(sizeof *ot_dev);
    if (ot_dev == NULL) {
        close(s32_fd);
        return NULL;
    }

    (td_void)memset_s(ot_dev, sizeof(*ot_dev), 0, sizeof(*ot_dev));
    ot_dev->i_fd = s32_fd;

    return ot_dev;
}

static td_s32 ot_uvc_video_process_user_ptr(struct ot_uvc_dev *ot_dev)
{
    struct video_cache v_cache;
    td_s32 s32_ret;

    DEBUG("#############ot_uvc_video_process_user_ptr()#############\n");

    (td_void)memset_s(&v_cache, sizeof(v_cache), 0, sizeof(v_cache));
    v_cache.dw_type = VIDEO_CACHE_TYPE_OPT;
    v_cache.dw_mem = VIDEO_MM_USER;

    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DQUEUE_BUF, &v_cache);
    if (s32_ret < 0) {
        return s32_ret;
    }

    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_QUEUE_BUF, &v_cache);
    if (s32_ret < 0) {
        ERR_INFO("Unable to requeue buffer(1): %d\n", errno);
        return s32_ret;
    }

    return 0;
}

static td_void ot_uvc_video_stream_user_ptr(struct ot_uvc_dev *ot_dev)
{
    struct video_cache v_cache;
    td_s32 s32_type = VIDEO_CACHE_TYPE_OPT;
    td_s32 s32_ret;

    DEBUG("%s:Starting video stream.\n", __func__);

    (td_void)memset_s(&v_cache, sizeof(v_cache), 0, sizeof(v_cache));

    v_cache.dw_index = 0;
    v_cache.dw_type = VIDEO_CACHE_TYPE_OPT;
    v_cache.dw_mem = VIDEO_MM_USER;

    usleep(100 * 1000); // need sleep 100 * 1000 us for usb when stream on.
    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_QUEUE_BUF, &v_cache);
    if (s32_ret < 0) {
        ERR_INFO("Unable to queue buffer: %d\n", errno);
        return;
    }

    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_STREAM_ON, &s32_type);
    if (s32_ret < 0) {
        ERR_INFO("Unable to stream on: %d\n", errno);
        return;
    }

    ot_dev->i_streaming = 1;
    return;
}

static td_s32 ot_uvc_video_set_fmt(struct ot_uvc_dev *ot_dev)
{
    struct video_fmt v_fmt;
    td_s32 s32_ret;

    (td_void)memset_s(&v_fmt, sizeof(v_fmt), 0, sizeof(v_fmt));
    v_fmt.dw_type = VIDEO_CACHE_TYPE_OPT;
    v_fmt.fmt.pix.dw_width = ot_dev->dw_width;
    v_fmt.fmt.pix.dw_height = ot_dev->dw_height;
    v_fmt.fmt.pix.dw_fmt = ot_dev->dw_fcc;
    v_fmt.fmt.pix.dw_fld = VIDEO_FLD_NOTHING;

    if ((ot_dev->dw_fcc == VIDEO_IMG_FORMAT_MJPEG) ||
        (ot_dev->dw_fcc == VIDEO_IMG_FORMAT_H264) ||
        (ot_dev->dw_fcc == VIDEO_IMG_FORMAT_H265)) {
        v_fmt.fmt.pix.dw_size = ot_dev->dw_img_size;
    }

    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_SET_FORMAT, &v_fmt);
    if (s32_ret < 0) {
        ERR_INFO("Unable to set format: %d\n", errno);
    }

    return s32_ret;
}

static td_void ot_uvc_stream_off(struct ot_uvc_dev *ot_dev)
{
    td_s32 s32_type = VIDEO_CACHE_TYPE_OPT;
    td_s32 s32_ret;

    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_STREAM_OFF, &s32_type);
    if (s32_ret < 0) {
        ERR_INFO("Unable to stream off: %d\n", errno);
    }

    ot_stream_shutdown();
    ot_dev->i_streaming = 0;
    DEBUG("Stopping video stream.\n");
}

static td_void ot_uvc_video_disable(struct ot_uvc_dev *ot_dev)
{
    ot_uvc_stream_off(ot_dev);
}

static td_void ot_uvc_video_enable(struct ot_uvc_dev *ot_dev)
{
    encoder_property ep;

    ot_uvc_video_disable(ot_dev);

    ep.format = ot_dev->dw_fcc;
    ep.width = ot_dev->dw_width;
    ep.height = ot_dev->dw_height;
    ep.fps = ot_dev->i_fps;
    ep.compsite = 0;

    ot_stream_set_enc_property(&ep);
    ot_stream_shutdown();
    ot_stream_startup();

    ot_uvc_video_stream_user_ptr(ot_dev);
}

static td_s32 is_contain_udc_sub_dir(const td_char *char_path, td_s32 path_size)
{
    DIR *p_dir = NULL;
    struct dirent *p_dirent = NULL;
    if (path_size < 0) {
        return -1;
    }

    p_dir = opendir(char_path);
    if (p_dir == NULL) {
        ERR_INFO("No such path: %s\n", char_path);
        return -1;
    }

    while ((p_dirent = readdir(p_dir)) != NULL) {
        if (strcmp(p_dirent->d_name, "UDC") == 0) {
            closedir(p_dir);
            return 0;
        }
    }

    closedir(p_dir);

    return -1;
}

static td_s32 read_file(const td_char *char_path, td_u32 path_size, td_char *char_dest, td_u32 u32_size)
{
    FILE *input = NULL;
    td_char *realpath_res = NULL;
    td_s32 ret;

    if (path_size < 0) {
        return -1;
    }
    realpath_res = realpath(char_path, NULL);
    if (realpath_res == NULL) {
        return -1;
    }

    input = fopen(realpath_res, "rw");
    if (input == NULL) {
        ERR_INFO("No such path: %s\n", char_path);
        if (realpath_res != NULL) {
            free(realpath_res);
            realpath_res = NULL;
        }
        return -1;
    }

    ret = fscanf_s(input, "%s", char_dest, u32_size);
    if (ret != -1) {
        ret = 0;
    }

    if (fclose(input) != 0) {
        ERR_INFO("close file failed!\n");
    }

    if (realpath_res != NULL) {
        free(realpath_res);
        realpath_res = NULL;
    }
    return ret;
}

static td_s32 get_udc_node_name(td_char *char_node_name, td_u32 u32_size)
{
    DIR *p_dir = NULL;
    struct dirent *p_dirent = NULL;
    const td_char *path_tmp = "/sys/kernel/config/usb_gadget/";
    td_char s32_tmp[UDC_PATH_LENGTH]; /* 389: 256 bytes used for d_name, 128 bytes used for path_tmp, 5 bytes for UDC */

    p_dir = opendir(path_tmp);
    if (p_dir == NULL) {
        ERR_INFO("No such path: %s\n", path_tmp);
        return -1;
    }

    while ((p_dirent = readdir(p_dir)) != NULL) {
        if (strcmp(p_dirent->d_name, ".") == 0 || strcmp(p_dirent->d_name, "..") == 0) {
            continue;
        }

        if (p_dirent->d_type == DT_DIR) {
            /* 389 = 128+256+5; 128 bytes used for path_tmp */
            strncpy_s(s32_tmp, UDC_PATH_LENGTH, path_tmp, 128);
            strncat_s(s32_tmp, UDC_PATH_LENGTH, p_dirent->d_name, 256); /* 389 = 128+256+5; 256 bytes used for d_name */

            if (is_contain_udc_sub_dir(s32_tmp, UDC_PATH_LENGTH) == 0) {
                closedir(p_dir);
                strncat_s(s32_tmp, UDC_PATH_LENGTH, "/UDC", 5); /* 389 = 128+256+5; 5 bytes for UDC */
                return read_file(s32_tmp, UDC_PATH_LENGTH, char_node_name, u32_size);
            }
        }
    }

    closedir(p_dir);

    return -1;
}

static td_s32 get_max_payload_transfer_size(const struct ot_uvc_frame_info *ot_frm_info)
{
    const td_s32 high_speed_size = 3072;
    const td_s32 full_speed_size = 1023;
    const td_char *path_tmp = "/sys/class/udc/";

    td_char char_tmp[128];
    td_char char_target_path[GET_MAX_PACKET_SIZE_PATH]; // 398=256+128+14
    td_s32 s32_result = high_speed_size;

    if (get_udc_node_name(char_tmp, 128) != 0) { /* 128 bytes used for char_tmp */
        return s32_result;
    }

    /* 398=256+128+14   256 bytes used for path_tmp */
    strncpy_s(char_target_path, GET_MAX_PACKET_SIZE_PATH, path_tmp, 256);
    /* 398=256+128+14   128 bytes used for char_tmp */
    strncat_s(char_target_path, GET_MAX_PACKET_SIZE_PATH, char_tmp, 128);
    /* 398=256+128+14   14 bytes used for current_speed */
    strncat_s(char_target_path, GET_MAX_PACKET_SIZE_PATH, "/current_speed", 14);

    if (read_file(char_target_path, GET_MAX_PACKET_SIZE_PATH, char_tmp, 128) != 0) { /* 128 bytes used for char_tmp */
        return s32_result;
    }

    if (strcmp(char_tmp, "super-speed") == 0) {
        s32_result = ot_frm_info->ss_xfersize;
    } else if (strcmp(char_tmp, "high-speed") == 0) {
        s32_result = ot_frm_info->hs_xfersize;
    } else if (strcmp(char_tmp, "full-speed") == 0) {
        s32_result = full_speed_size;
    } else {
        ERR_INFO("USB cable is not connected yet.\n");
    }

    return s32_result;
}

static td_void ot_uvc_get_buf_size(struct ot_uvc_dev *ot_dev, const struct ot_uvc_frame_info *ot_frm_info, td_u32 fcc,
    td_u32 *dw_max_video_frame_size)
{
    switch (fcc) {
        case VIDEO_IMG_FORMAT_YUYV:
            *dw_max_video_frame_size = ot_frm_info->width * ot_frm_info->height * 2; /* 2 bytes a pixel */
            break;
        case VIDEO_IMG_FORMAT_YUV420:
        case VIDEO_IMG_FORMAT_NV12:
        case VIDEO_IMG_FORMAT_NV21:
            *dw_max_video_frame_size = ot_frm_info->width * ot_frm_info->height * 1.5; /* 1.5 byte a pixel */
            break;
        case VIDEO_IMG_FORMAT_MJPEG:
        case VIDEO_IMG_FORMAT_H264:
        case VIDEO_IMG_FORMAT_H265:
            *dw_max_video_frame_size = ot_dev->dw_img_size;
            break;
        default:
            printf("format error!\n");
            break;
    }
}

static td_void ot_uvc_handle_streaming_control(struct ot_uvc_dev *ot_dev, struct ot_uvc_streaming_control *u_str_ctrl,
    td_s32 u32_i_frame, td_s32 s32_i_format)
{
    const struct ot_uvc_format_info *ot_fmt_info = NULL;
    const struct ot_uvc_frame_info *ot_frm_info = NULL;
    td_u32 s32_n_frm = 0;

    if (s32_i_format < 0) {
        s32_i_format = array_size(ot_fmt) + s32_i_format;
    }

    if ((s32_i_format < 0) || (s32_i_format >= (td_s32)array_size(ot_fmt))) {
        return;
    }

    DEBUG("s32_i_format = %d\n", s32_i_format);
    ot_fmt_info = &ot_fmt[s32_i_format];

    while (ot_fmt_info->frames[s32_n_frm].width != 0) {
        ++s32_n_frm;
    }

    if (u32_i_frame < 0) {
        u32_i_frame = s32_n_frm + u32_i_frame;
    }

    if ((u32_i_frame < 0) || (u32_i_frame >= (td_s32)s32_n_frm)) {
        return;
    }

    ot_frm_info = &ot_fmt_info->frames[u32_i_frame];

    (td_void)memset_s(u_str_ctrl, sizeof(*u_str_ctrl), 0, sizeof(*u_str_ctrl));

    u_str_ctrl->bm_hint = 1;
    u_str_ctrl->b_format_index = s32_i_format + 1;               /* Yuv: 1, Mjpeg: 2. */
    u_str_ctrl->b_frame_index = u32_i_frame + 1;                 /* 360p: 1 720p: 2. */
    u_str_ctrl->dw_frame_interval = ot_frm_info->intervals[0]; /* Corresponding to the number of frame rate. */

    ot_uvc_get_buf_size(ot_dev, ot_frm_info, ot_fmt_info->fcc, &u_str_ctrl->dw_max_video_frame_size);

    if (ot_dev->dw_bulk) {
        u_str_ctrl->dw_max_payload_transfer_size = ot_dev->dw_bulk_size; /* This should be filled by the driver. */
    } else {
        u_str_ctrl->dw_max_payload_transfer_size = get_max_payload_transfer_size(ot_frm_info);
    }
    u_str_ctrl->bm_framing_info = 3; /* 3 format mode */
    u_str_ctrl->b_prefered_version = 1;
    u_str_ctrl->b_max_version = 1;
}

static td_void ot_uvc_handle_standard_request(struct ot_uvc_dev *ot_dev, struct usb_ctrlrequest *u_ctrl_req,
    struct uvc_request_data *u_req_data)
{
    DEBUG("camera standard request\n");
    (td_void)ot_dev;
    (td_void)u_ctrl_req;
    (td_void)u_req_data;
}


static void ot_uvc_eve_undef_control(struct ot_uvc_dev *ot_dev, td_u8 u8_cs, struct uvc_request_data *u_req_data)
{
    switch (u8_cs) {
        case OT_UVC_VC_REQUEST_ERROR_CODE_CONTROL:
            u_req_data->length = ot_dev->request_error_code.length;
            u_req_data->data[0] = ot_dev->request_error_code.data[0];
            break;
        default:
            ot_dev->request_error_code.length = 1;
            ot_dev->request_error_code.data[0] = 0x06;
            break;
    }
}

static td_void ot_uvc_handle_control_request(struct ot_uvc_dev *ot_dev, td_u8 u8_req, td_u8 u8_unit_id, td_u8 u8_cs,
    struct uvc_request_data *u_req_data)
{
    switch (u8_unit_id) {
        case OT_UVC_VC_DESCRIPTOR_UNDEFINED:
            ot_uvc_eve_undef_control(ot_dev, u8_cs, u_req_data);
            break;
        case OT_UVC_VC_HEADER:
            ot_stream_event_it_control(ot_dev, u8_req, u8_unit_id, u8_cs, u_req_data);
            break;
        case OT_UVC_VC_INPUT_TERMINAL:
            ot_stream_event_pu_control(ot_dev, u8_req, u8_unit_id, u8_cs, u_req_data);
            break;
        case UNIT_XU_H264:
            ot_stream_event_eu_h264_control(ot_dev, u8_req, u8_unit_id, u8_cs, u_req_data);
            break;
        default:
            ot_dev->request_error_code.length = 1;
            ot_dev->request_error_code.data[0] = 0x06;
    }
}

static td_void ot_uvc_handle_streaming_request(struct ot_uvc_dev *ot_dev, td_u8 u8_req, td_u8 u8_cs,
    struct uvc_request_data *u_req_data)
{
    struct ot_uvc_streaming_control *u_str_ctrl = NULL;
    const struct ot_uvc_format_info *format = NULL;
    const struct ot_uvc_frame_info *frame = NULL;

    if ((u8_cs != OT_UVC_VS_PROBE_CONTROL) && (u8_cs != OT_UVC_VS_COMMIT_CONTROL)) {
        return;
    }

    u_str_ctrl = (struct ot_uvc_streaming_control *)&u_req_data->data;
    u_req_data->length = sizeof *u_str_ctrl;
    format = &ot_fmt[ot_dev->probe.b_format_index - 1];
    frame = &format->frames[ot_dev->probe.b_frame_index - 1];

    switch (u8_req) {
        case OT_UVC_SET_CUR:
            ot_dev->i_control = u8_cs;
            u_req_data->length = 0x22;
            u_str_ctrl->dw_max_payload_transfer_size = get_max_payload_transfer_size(frame);
            break;
        case OT_UVC_GET_CUR:
            if (u8_cs == OT_UVC_VS_PROBE_CONTROL) {
                (td_void)memcpy_s(u_str_ctrl, sizeof(*u_str_ctrl), &ot_dev->probe, sizeof(*u_str_ctrl));
            } else {
                (td_void)memcpy_s(u_str_ctrl, sizeof(*u_str_ctrl), &ot_dev->commit, sizeof(*u_str_ctrl));
            }
            u_str_ctrl->dw_max_payload_transfer_size = get_max_payload_transfer_size(frame);
            break;
        case OT_UVC_GET_MIN:
        case OT_UVC_GET_MAX:

        case OT_UVC_GET_DEF:
            ot_uvc_handle_streaming_control(ot_dev, u_str_ctrl, ot_dev->probe.b_frame_index - 1, \
                ot_dev->probe.b_format_index - 1);
            break;
        case OT_UVC_GET_RES:
            (td_void)memset_s(u_str_ctrl, sizeof(*u_str_ctrl), 0, sizeof(*u_str_ctrl));
            break;
        case OT_UVC_GET_LEN:
            u_req_data->data[0] = 0x00;
            u_req_data->data[1] = 0x22;
            u_req_data->length = 0x2;
            break;
        case OT_UVC_GET_INFO:
            u_req_data->data[0] = 0x03;
            u_req_data->length = 1;
            break;
        default:
            break;
    }
}

static td_void set_probe_status(struct ot_uvc_dev *ot_dev, td_s32 u32_cs, td_s32 u32_req)
{
    if (u32_cs == 0x01) {
        switch (u32_req) {
            case 0x01:
                ot_dev->probe_status.b_set = 1;
                break;
            case 0x81:
                ot_dev->probe_status.b_get = 1;
                break;
            case 0x82:
                ot_dev->probe_status.b_min = 1;
                break;
            case 0x83:
                ot_dev->probe_status.b_max = 1;
                break;
            case 0x84:
                break;
            case 0x85:
                break;
            case 0x86:
                break;
            default:
                break;
        }
    }
}

static td_void ot_uvc_handle_class_request(struct ot_uvc_dev *ot_dev, struct usb_ctrlrequest *u_ctrl_req,
    struct uvc_request_data *u_req_data)
{
    const td_u8 u8_probe_status = 1;

    if (u8_probe_status) {
        td_u8 u8_type = u_ctrl_req->bRequestType & USB_RECIP_MASK;
        switch (u8_type) {
            case USB_RECIP_INTERFACE:
                DEBUG("request u8_type :td_s32ERFACE\n");
                DEBUG("td_s32erface : %d\n", (u_ctrl_req->wIndex & 0xff));
                DEBUG("unit id : %d\n", ((u_ctrl_req->wIndex & 0xff00) >> RIGHT_SHIFT_8BIT));
                DEBUG("cs code : 0x%02x(%s)\n", (u_ctrl_req->wValue >> RIGHT_SHIFT_8BIT),
                    (td_char *)get_intf_cs_string((u_ctrl_req->wValue >> RIGHT_SHIFT_8BIT)));
                DEBUG("req code: 0x%02x(%s)\n", u_ctrl_req->bRequest, (td_char *)get_code(u_ctrl_req->bRequest));

                set_probe_status(ot_dev, (u_ctrl_req->wValue >> RIGHT_SHIFT_8BIT), u_ctrl_req->bRequest);
                break;
            case USB_RECIP_DEVICE:
                DEBUG("request type :DEVICE\n");
                break;
            case USB_RECIP_ENDPOINT:
                DEBUG("request type :ENDPOINT\n");
                break;
            case USB_RECIP_OTHER:
                DEBUG("request type :OTHER\n");
                break;
            default:
                break;
        }
    }

    if ((u_ctrl_req->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE) {
        return;
    }

    ot_dev->i_control = (u_ctrl_req->wValue >> RIGHT_SHIFT_8BIT);
    ot_dev->i_unit_id = ((u_ctrl_req->wIndex & 0xff00) >> RIGHT_SHIFT_8BIT);
    ot_dev->i_intf_id = (u_ctrl_req->wIndex & 0xff);

    switch (u_ctrl_req->wIndex & 0xff) {
        case OT_UVC_INTF_CONTROL:
            ot_uvc_handle_control_request(ot_dev, u_ctrl_req->bRequest, u_ctrl_req->wIndex >> RIGHT_SHIFT_8BIT,
                u_ctrl_req->wValue >> RIGHT_SHIFT_8BIT, u_req_data);
            break;
        case OT_UVC_INTF_STREAMING:
            ot_uvc_handle_streaming_request(ot_dev, u_ctrl_req->bRequest, u_ctrl_req->wValue >> RIGHT_SHIFT_8BIT,
                u_req_data);
            break;
        default:
            break;
    }
}

static td_void do_ot_uvc_setup_event(struct ot_uvc_dev *ot_dev, struct usb_ctrlrequest *u_ctrl_req,
    struct uvc_request_data *u_req_data)
{
    ot_dev->i_control = 0;
    ot_dev->i_unit_id = 0;
    ot_dev->i_intf_id = 0;

    switch (u_ctrl_req->bRequestType & USB_TYPE_MASK) {
        case USB_TYPE_STANDARD:
            ot_uvc_handle_standard_request(ot_dev, u_ctrl_req, u_req_data);
            break;
        case USB_TYPE_CLASS:
            ot_uvc_handle_class_request(ot_dev, u_ctrl_req, u_req_data);
            break;
        default:
            break;
    }
}

static td_void handle_control_interface_data(struct ot_uvc_dev *ot_dev, struct uvc_request_data *u_req_data)
{
    switch (ot_dev->i_unit_id) {
        case OT_UVC_VC_HEADER:
            ot_stream_event_it_data(ot_dev, ot_dev->i_unit_id, ot_dev->i_control, u_req_data);
            break;
        case OT_UVC_VC_INPUT_TERMINAL:
            ot_stream_event_pu_data(ot_dev, ot_dev->i_unit_id, ot_dev->i_control, u_req_data);
            break;
        case UNIT_XU_H264:
            ot_stream_event_eu_h264_data(ot_dev, ot_dev->i_unit_id, ot_dev->i_control, u_req_data);
            break;
        default:
            break;
    }
}

td_u32 clamp(td_u32 val, td_u32 min, td_u32 max)
{
    td_u32 res = val;
    if (res < min) {
        return min;
    }
    if (res > max) {
        return max;
    }
    return val;
}

static td_void do_ot_uvc_data_event(struct ot_uvc_dev *ot_dev, struct uvc_request_data *u_req_data)
{
    struct ot_uvc_streaming_control *u_str_target = NULL;
    struct ot_uvc_streaming_control *u_str_ctrl = NULL;
    const struct ot_uvc_format_info *ot_fmt_info = NULL;
    const struct ot_uvc_frame_info *ot_frm_info = NULL;
    const td_u32 *u32_intval = NULL;
    td_u32 u32_i_fmt, u32_i_frm;
    td_u32 u32_n_frm;

    if ((ot_dev->i_unit_id != 0) && (ot_dev->i_intf_id == OT_UVC_INTF_CONTROL)) {
        return handle_control_interface_data(ot_dev, u_req_data);
    }

    switch (ot_dev->i_control) {
        case OT_UVC_VS_PROBE_CONTROL:
            DEBUG("setting probe control, length = %d\n", u_req_data->length);
            u_str_target = &ot_dev->probe;
            break;
        case OT_UVC_VS_COMMIT_CONTROL:
            DEBUG("setting commit control, length = %d\n", u_req_data->length);
            u_str_target = &ot_dev->commit;
            break;
        default:
            DEBUG("setting unknown control, length = %d\n", u_req_data->length);
            return;
    }

    u_str_ctrl = (struct ot_uvc_streaming_control *)&u_req_data->data;

    DEBUG("u_str_ctrl->b_format_index = %d\n", (td_u32)u_str_ctrl->b_format_index);

    u32_i_fmt = clamp((td_u32)u_str_ctrl->b_format_index, 1U, (td_u32)array_size(ot_fmt));

    DEBUG("set iformat = %d \n", u32_i_fmt);

    ot_fmt_info = &ot_fmt[u32_i_fmt - 1];
    u32_n_frm = 0;

    DEBUG("ot_fmt_info->frames[u32_n_frm].width: %d\n", ot_fmt_info->frames[u32_n_frm].width);
    DEBUG("ot_fmt_info->frames[u32_n_frm].height: %d\n", ot_fmt_info->frames[u32_n_frm].height);

    while (ot_fmt_info->frames[u32_n_frm].width != 0) {
        ++u32_n_frm;
    }

    u32_i_frm = clamp((td_u32)u_str_ctrl->b_frame_index, 1U, u32_n_frm);
    ot_frm_info = &ot_fmt_info->frames[u32_i_frm - 1];
    u32_intval = ot_frm_info->intervals;

    while ((u32_intval[0] < u_str_ctrl->dw_frame_interval) && u32_intval[1]) {
        ++u32_intval;
    }

    u_str_target->b_format_index = u32_i_fmt;
    u_str_target->b_frame_index = u32_i_frm;

    switch (ot_fmt_info->fcc) {
        case VIDEO_IMG_FORMAT_YUYV:
            u_str_target->dw_max_video_frame_size = ot_frm_info->width * ot_frm_info->height * 2; /* 2 bytes a pixel */
            break;
        case VIDEO_IMG_FORMAT_NV12:
        case VIDEO_IMG_FORMAT_NV21:
        case VIDEO_IMG_FORMAT_YUV420:
            u_str_target->dw_max_video_frame_size =
                ot_frm_info->width * ot_frm_info->height * 1.5; /* 1.5 bytes a pixel */
            break;
        case VIDEO_IMG_FORMAT_MJPEG:
        case VIDEO_IMG_FORMAT_H264:
        case VIDEO_IMG_FORMAT_H265:
            if (ot_dev->dw_img_size == 0) {
                DEBUG("WARNING: MJPEG requested and no image loaded.\n");
            }

            u_str_target->dw_max_video_frame_size = ot_dev->dw_img_size;
            break;
        default:
            break;
    }

    u_str_target->dw_frame_interval = *u32_intval;

    DEBUG("set u32_intval=%d ot_fmt_info=%d ot_frm_info=%d\n", u_str_target->dw_frame_interval,
        u_str_target->b_format_index, u_str_target->b_frame_index);

    if (ot_dev->i_control == OT_UVC_VS_COMMIT_CONTROL) {
        ot_dev->dw_fcc = ot_fmt_info->fcc;
        ot_dev->dw_width = ot_frm_info->width;
        ot_dev->dw_height = ot_frm_info->height;
        ot_dev->i_fps = FRAME_INTERVAL_CALC_100NS / u_str_target->dw_frame_interval;

        DEBUG("set device format=%s width=%d height=%d\n", to_string(ot_dev->dw_fcc), ot_dev->dw_width,
            ot_dev->dw_height);

        ot_uvc_video_set_fmt(ot_dev);

        if (ot_dev->dw_bulk != 0) {
            ot_uvc_video_disable(ot_dev);
            ot_uvc_video_enable(ot_dev);
        }
    }

    if (ot_dev->i_control == OT_UVC_VS_COMMIT_CONTROL) {
        (td_void)memset_s(&ot_dev->probe_status, sizeof(ot_dev->probe_status), 0, sizeof(ot_dev->probe_status));
    }
}

static td_void do_ot_uvc_event(struct ot_uvc_dev *ot_dev)
{
    struct video_event v_event;
    struct ot_uvc_event *ot_eve = (struct ot_uvc_event *)(td_void *)&v_event.u.a_data;
    struct uvc_request_data u_req_data;
    td_s32 s32_ret;

    DEBUG("#############do_ot_uvc_event()#############\n");

    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DQUEUE_EVENT, &v_event);
    if (s32_ret < 0) {
        ERR_INFO("VIDEO_IOCTL_DQUEUE_EVENT failed: %d\n", errno);
        return;
    }

    (td_void)memset_s(&u_req_data, sizeof(u_req_data), 0, sizeof(u_req_data));
    u_req_data.length = 32; /* 32 byte data length */

    switch (v_event.dw_type) {
        case OT_UVC_EVE_CON:
            DEBUG("handle connect event\n");
            ot_uvc_handle_streaming_control(ot_dev, &ot_dev->probe, 0, 0);
            ot_uvc_handle_streaming_control(ot_dev, &ot_dev->commit, 0, 0);
            /* fall-through */
        case OT_UVC_EVE_DISCON:
            return;
        case OT_UVC_EVE_SETTING:
            do_ot_uvc_setup_event(ot_dev, &ot_eve->req, &u_req_data);
            break;
        case OT_UVC_EVE_DATA:
            do_ot_uvc_data_event(ot_dev, &ot_eve->data);
            return;
        case OT_UVC_EVE_STRON:
            if (!ot_dev->dw_bulk) {
                ot_uvc_video_enable(ot_dev);
            }
            return;
        case OT_UVC_EVE_STROFF:
            if (!ot_dev->dw_bulk) {
                ot_uvc_video_disable(ot_dev);
            }
            return;
        default:
            break;
    }

    s32_ret = ioctl(ot_dev->i_fd, OT_UVC_IOC_SEND_RESPONSE, &u_req_data);
    if (s32_ret < 0) {
        ERR_INFO("OT_UVC_IOC_S_EVENT failed: %d\n", errno);
        return;
    }
}

static td_void ot_uvc_event_register(struct ot_uvc_dev *ot_dev)
{
    struct video_event_descriptor v_event_desc;
    td_s32 s32_ret;

    (td_void)memset_s(&v_event_desc, sizeof(v_event_desc), 0, sizeof(v_event_desc));

    v_event_desc.dw_type = OT_UVC_EVE_CON;
    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DESC_EVENT, &v_event_desc);
    if (s32_ret < 0) {
        ERR_INFO("Connect event failed: %d\n", errno);
    }

    v_event_desc.dw_type = OT_UVC_EVE_SETTING;
    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DESC_EVENT, &v_event_desc);
    if (s32_ret < 0) {
        ERR_INFO("Setup event failed: %d\n", errno);
    }

    v_event_desc.dw_type = OT_UVC_EVE_DATA;
    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DESC_EVENT, &v_event_desc);
    if (s32_ret < 0) {
        ERR_INFO("Data event failed: %d\n", errno);
    }

    v_event_desc.dw_type = OT_UVC_EVE_STRON;
    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DESC_EVENT, &v_event_desc);
    if (s32_ret < 0) {
        ERR_INFO("StreamOn event failed: %d\n", errno);
    }

    v_event_desc.dw_type = OT_UVC_EVE_STROFF;
    s32_ret = ioctl(ot_dev->i_fd, VIDEO_IOCTL_DESC_EVENT, &v_event_desc);
    if (s32_ret < 0) {
        ERR_INFO("StreamOff event failed: %d\n", errno);
    }
}

td_s32 open_uvc_device(const td_char *char_dev_path)
{
    struct ot_uvc_dev *ot_dev;

    td_char *char_dev = (td_char *)char_dev_path;

    ot_dev = ot_uvc_init(char_dev);
    if (ot_dev == 0) {
        return -1;
    }

    ot_dev->dw_img_size = MAX_PAYLOAD_IMAGE_SIZE; /* 2160*3840*2 */

    DEBUG("set imagesize = %d, set bulkmode =%d, set bulksize = %d\n", ot_dev->dw_img_size, ot_dev->dw_bulk,
        ot_dev->dw_bulk_size);

    ot_uvc_event_register(ot_dev);
    g_ot_cd = ot_dev;

    return 0;
}

td_s32 close_uvc_device(td_void)
{
    if (g_ot_cd != 0) {
        ot_uvc_video_disable(g_ot_cd);
        close(g_ot_cd->i_fd);
        free(g_ot_cd);
    }

    g_ot_cd = 0;
    return 0;
}

td_s32 run_uvc_data(td_void)
{
    fd_set w_fds;
    td_s32 s32_ret;
    struct timeval t_val;

    if (g_ot_cd == NULL) {
        return -1;
    }

    t_val.tv_sec = 1;
    t_val.tv_usec = 0;

    FD_ZERO(&w_fds);

    if (g_ot_cd->i_streaming == 1) {
        FD_SET(g_ot_cd->i_fd, &w_fds);
    }

    s32_ret = select(g_ot_cd->i_fd + 1, NULL, &w_fds, NULL, &t_val);
    if (s32_ret > 0) {
        if (FD_ISSET(g_ot_cd->i_fd, &w_fds)) {
            ot_uvc_video_process_user_ptr(g_ot_cd);
        }
    }

    return s32_ret;
}

td_s32 run_uvc_device(td_void)
{
    fd_set e_fds;
    td_s32 s32_ret;
    struct timeval t_val;

    if (g_ot_cd == NULL) {
        return -1;
    }

    t_val.tv_sec = 1;
    t_val.tv_usec = 0;

    FD_ZERO(&e_fds);
    FD_SET(g_ot_cd->i_fd, &e_fds);

    s32_ret = select(g_ot_cd->i_fd + 1, NULL, NULL, &e_fds, &t_val);
    if (s32_ret > 0) {
        if (FD_ISSET(g_ot_cd->i_fd, &e_fds)) {
            do_ot_uvc_event(g_ot_cd);
        }
    }

    return s32_ret;
}
