/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "sample_pciv_host.h"

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

#include "sample_pciv_comm.h"
#include "ss_mpi_pciv.h"
#include "securec.h"

static td_bool g_quit = TD_FALSE;

static ot_pic_size g_pic_size_main = PIC_1080P;
static ot_pic_size g_pic_size_co = PIC_360P;
static ot_pic_size g_pic_size_vdec = PIC_3840X2160;
static td_bool g_signo_flag = TD_TRUE;

static td_u32 g_co_processor_size = 640 * 360 * 3 / 2; /* 640:360:3:2 buffer size for 360P */

static sample_vo_cfg g_vo_cfg;

static td_s32 g_local_id;
static ot_pciv_enum_chip g_chips;
static td_phys_addr_t g_pf_win_base[OT_PCIV_MAX_CHIP_NUM] = { 0 };

static sample_pciv_co_thread g_all_thrad[OT_PCIV_MAX_CHIP_NUM][SAMPLE_PCIV_CHN_PER_CHIP] = { 0 };

static pthread_t   g_pciv_vdec_thread[OT_VDEC_MAX_CHN_NUM] = { 0 };

static sample_vdec_attr g_pciv_vdec_cfg[OT_VDEC_MAX_CHN_NUM] = {
    {
        .type   = OT_PT_H264,
        .mode   = OT_VDEC_SEND_MODE_FRAME,
        .width  = _4K_WIDTH,
        .height = _4K_HEIGHT,
        .sample_vdec_video.dec_mode      = OT_VIDEO_DEC_MODE_IPB,
        .sample_vdec_video.bit_width     = OT_DATA_BIT_WIDTH_8,
        .sample_vdec_video.ref_frame_num = 3, /* 3:ref_frame_num */
        .display_frame_num = 2,  /* 2:display_frame_num */
        .frame_buf_cnt     = 6,  /* 6:ref_frame_num + display_frame_num + 1 */
    },
};

static vdec_thread_param g_pciv_vdec_thread_param[OT_VDEC_MAX_CHN_NUM] = {
    {
        .chn_id        = 0,
        .type          = OT_PT_H264,
        .stream_mode   = OT_VDEC_SEND_MODE_FRAME,
        .interval_time = 1000, /* 1000:interval_time */
        .pts_init      = 0,
        .pts_increase  = 0,
        .e_thread_ctrl = THREAD_CTRL_START,
        .circle_send   = TD_TRUE,
        .milli_sec     = 0,
        .min_buf_size  = (_4K_WIDTH * _4K_HEIGHT * 3) >> 1, /* 3:buff_size */
        .c_file_path   = SAMPLE_PCIV_STREAM_PATH,
        .c_file_name   = SAMPLE_PCIV_STREAM_FILE,
        .fps           = 30, /* 30:frame rate */
    },
};

static td_s32 sample_pciv_read_msg(td_s32 chip, td_s32 port, sample_pciv_msg *msg)
{
    td_s32 ret;
    td_s32 repeat_time = 0;

    while (TD_TRUE) {
        ret = pciv_read_msg(chip, port, msg);
        if (ret != TD_SUCCESS) {
            if (repeat_time > PCIV_REPEAT_MAX_TIME) {
                pciv_printf("read msg failed more than %d times, error code:0x%x, msg_type:%u",
                    PCIV_REPEAT_MAX_TIME, ret, msg->msg_head.msg_type);
                break;
            }
            repeat_time++;
            usleep(PCIV_WAIT_MSG_TIME);
        } else {
            break;
        }
    }
    return ret;
}

static td_void sample_pciv_send_cmd(td_s32 chip_id, sample_pciv_msg *msg)
{
    td_s32 ret;

    msg->msg_head.ret_val = TD_SUCCESS;
    msg->msg_head.target  = chip_id;
    ret = pciv_send_msg(chip_id, PCIV_MSGPORT_COMM_CMD, msg);
    if (ret != TD_SUCCESS) {
        pciv_printf("send msg failed, ret value is 0x%x", ret);
        msg->msg_head.ret_val = TD_FAILURE;
        return;
    }
    printf("==========send cmd msg (target:%d, type:%d)==========\n", chip_id, msg->msg_head.msg_type);
    ret = sample_pciv_read_msg(chip_id, PCIV_MSGPORT_COMM_CMD, msg);
    if (ret != TD_SUCCESS) {
        pciv_printf("read msg failed, ret value is 0x%x", ret);
        msg->msg_head.ret_val = TD_FAILURE;
        return;
    }
    printf("==========read cmd msg (target:%d, type:%d)==========\n", chip_id, msg->msg_head.msg_type);
    if (msg->msg_head.msg_type != SAMPLE_PCIV_MSG_ECHO) {
        pciv_printf("read msg type:%d error", msg->msg_head.msg_type);
        msg->msg_head.ret_val = TD_FAILURE;
    }
    if (msg->msg_head.ret_val != TD_SUCCESS) {
        pciv_printf("slave echo msg ret value is 0x%x", ret);
    }
}

static td_s32 sample_pciv_init_mpp(td_void)
{
    td_s32    ret;
    ot_vb_cfg vb_conf;

    sample_comm_sys_exit();

    ret = memset_s(&vb_conf, sizeof(ot_vb_cfg), 0, sizeof(ot_vb_cfg));
    if (ret != EOK) {
        pciv_printf("vb_conf init failed");
        return TD_FAILURE;
    }
    vb_conf.max_pool_cnt = 2; /* 2: max pool count */

    vb_conf.common_pool[0].blk_size = 1920 * 1080 * 3 / 2; /* 1920:1080:3:2 buffer size for 1080P */
    vb_conf.common_pool[0].blk_cnt = 20; /* 20: buffer count */
    vb_conf.common_pool[1].blk_size = g_co_processor_size;
    vb_conf.common_pool[1].blk_cnt = 8; /* 8: buffer count */

    pciv_check_err_return(sample_comm_sys_init(&vb_conf));

    return TD_SUCCESS;
}

static td_s32 sample_pciv_init_media_sys(td_void)
{
    pciv_check_err_return(sample_pciv_init_mpp());
    return TD_SUCCESS;
}

static td_s32 sample_pciv_enum_chip(int *local_id, ot_pciv_enum_chip *chips)
{
    pciv_check_err_return(ss_mpi_pciv_get_local_id(local_id));
    printf("pci local id is %d \n", *local_id);

    pciv_check_err_return(ss_mpi_pciv_enum_chip(chips));
    printf("slave chip num is %d \n", chips->chip_num);
    return TD_SUCCESS;
}

static td_s32 sample_pciv_get_pf_win(td_s32 chip_id)
{
    ot_pciv_window_base pci_base_window;
    pciv_check_err_return(ss_mpi_pciv_get_window_base(chip_id, &pci_base_window));
    printf("pci device %d -> slot:%d, pf:0x%lx,np:0x%lx,cfg:0x%lx\n", chip_id, chip_id - 1,
           (td_ulong)pci_base_window.pf_addr, (td_ulong)pci_base_window.np_addr, (td_ulong)pci_base_window.cfg_addr);
    g_pf_win_base[chip_id] = pci_base_window.pf_addr;
    printf("===============winBase:%lx================\n", (td_ulong)pci_base_window.pf_addr);
    return TD_SUCCESS;
}

static td_s32 sample_pciv_init_pcie(td_s32 *local_id, ot_pciv_enum_chip *chips)
{
    td_u32 i;

    /* Get pci local id and all target id */
    pciv_check_err_return(sample_pciv_enum_chip(local_id, chips));

    for (i = 0; i < chips->chip_num; i++) {
        /* wait for pci device connect ... ... */
        pciv_check_err_return(pciv_wait_connect(chips->chip_id[i]));

        /* open pci msg port for common cmd */
        pciv_check_err_return(pciv_open_msg_port(chips->chip_id[i], PCIV_MSGPORT_COMM_CMD));

        /* Get PCI PF Window info of pci device, used for DMA trans that host to slave */
        pciv_check_err_return(sample_pciv_get_pf_win(chips->chip_id[i]));
    }
    return TD_SUCCESS;
}

static td_void sample_pciv_exit_pcie(ot_pciv_enum_chip *chips)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < chips->chip_num; i++) {
        /* close pci msg port for common cmd */
        ret = pciv_close_msg_port(chips->chip_id[i], PCIV_MSGPORT_COMM_CMD);
        if (ret != TD_SUCCESS) {
            pciv_printf("close msg port for chip %d failed", chips->chip_id[i]);
        }
    }
}

static td_s32 sample_pciv_init_vdec(td_u32 chn_num, sample_vdec_attr *attr)
{
    td_s32 ret;

    ret = sample_comm_vdec_init_vb_pool(chn_num, attr, chn_num);
    if (ret != TD_SUCCESS) {
        pciv_printf("vdec init vb_pool fail");
    }
    ret = sample_comm_vdec_start(chn_num, attr, chn_num);
    if (ret != TD_SUCCESS) {
        pciv_printf("vdec start fail");
        goto exit_pool;
    }
    return ret;
exit_pool:
    sample_comm_vdec_exit_vb_pool();
    return ret;
}

static td_void sample_pciv_exit_vdec(td_u32 chn_num)
{
    td_s32 ret;
    ret = sample_comm_vdec_stop(chn_num);
    if (ret != TD_SUCCESS) {
        printf("vdec stop fail\n");
    }
    sample_comm_vdec_exit_vb_pool();
}

static td_s32 sample_pciv_init_vpss(td_u32 vpss_grp_cnt)
{
    td_u32 i_loop;
    td_bool chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {0, 0, 1, 1};
    ot_vpss_grp_attr vpss_grp_attr = { 0 };
    ot_vpss_chn_attr vpss_chn_attr[OT_VPSS_MAX_PHYS_CHN_NUM] = { 0 };
    ot_size chn_size, co_size, grp_size;

    sample_comm_sys_get_pic_size(g_pic_size_main, &chn_size);
    sample_comm_sys_get_pic_size(g_pic_size_co, &co_size);
    sample_comm_sys_get_pic_size(g_pic_size_vdec, &grp_size);

    /* create VPSS group in master pciv */
    vpss_grp_attr.pixel_format = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vpss_grp_attr.max_width = grp_size.width;
    vpss_grp_attr.max_height = grp_size.height;
    vpss_grp_attr.frame_rate.src_frame_rate = -1;
    vpss_grp_attr.frame_rate.dst_frame_rate = -1;

    for (i_loop = 0; i_loop < OT_VPSS_MAX_PHYS_CHN_NUM; i_loop++) {
        if (chn_enable[i_loop] == TD_TRUE) {
            vpss_chn_attr[i_loop].width = chn_size.width;
            vpss_chn_attr[i_loop].height = chn_size.height;
            vpss_chn_attr[i_loop].chn_mode = OT_VPSS_CHN_MODE_USER;
            vpss_chn_attr[i_loop].compress_mode = OT_COMPRESS_MODE_NONE;
            vpss_chn_attr[i_loop].pixel_format = OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            vpss_chn_attr[i_loop].frame_rate.src_frame_rate = -1;
            vpss_chn_attr[i_loop].frame_rate.dst_frame_rate = -1;
            vpss_chn_attr[i_loop].depth = 1;
            vpss_chn_attr[i_loop].mirror_en = TD_FALSE;
            vpss_chn_attr[i_loop].flip_en = TD_FALSE;
            vpss_chn_attr[i_loop].aspect_ratio.mode = OT_ASPECT_RATIO_NONE;
        }
    }
    vpss_chn_attr[OT_VPSS_CHN3].width = co_size.width;
    vpss_chn_attr[OT_VPSS_CHN3].height = co_size.height;

    for (i_loop = 0; i_loop < vpss_grp_cnt; i_loop++) {
        pciv_check_err_return(sample_common_vpss_start(i_loop, chn_enable,
            &vpss_grp_attr, vpss_chn_attr, OT_VPSS_MAX_PHYS_CHN_NUM));
    }

    return TD_SUCCESS;
}

static td_void sample_pciv_exit_vpss(td_u32 vpss_grp_cnt)
{
    td_s32 ret;
    td_bool chn_enable[OT_VPSS_MAX_PHYS_CHN_NUM] = {0, 0, 1, 1};
    td_u32 i_loop;

    for (i_loop = 0; i_loop < vpss_grp_cnt; i_loop++) {
        ret = sample_common_vpss_stop(i_loop, chn_enable, OT_VPSS_MAX_PHYS_CHN_NUM);
        if (ret != TD_SUCCESS) {
            pciv_printf("stop vpss %u failed", i_loop);
        }
    }
}

static td_s32 sample_pciv_init_vo(td_void)
{
    sample_comm_vo_get_def_config(&g_vo_cfg);

    g_vo_cfg.vo_dev = SAMPLE_VO_DEV_DHD0;
    g_vo_cfg.vo_intf_type = OT_VO_INTF_HDMI;
    g_vo_cfg.dis_buf_len = 3; /* 3: def buf len for single */
    g_vo_cfg.intf_sync   = OT_VO_OUT_3840x2160_30;
    g_vo_cfg.vo_mode = VO_MODE_4MUX;
    g_vo_cfg.vo_part_mode     = OT_VO_PARTITION_MODE_MULTI;
    pciv_check_err_return(sample_comm_vo_start_vo(&g_vo_cfg));
    return TD_SUCCESS;
}

static td_void sample_pciv_exit_vo(td_void)
{
    td_s32 ret;

    ret = sample_comm_vo_stop_vo(&g_vo_cfg);
    if (ret != TD_SUCCESS) {
        pciv_printf("stop vo failed");
    }
}

static td_s32 sample_pciv_send_write_done_msg(td_s32 chip, td_s32 port, ot_video_frame_info *frame_info)
{
    td_s32 ret;
    sample_pciv_args_write_done  args = { 0 };
    sample_pciv_msg send_msg = {
        .msg_head = {
            .target = 0,
            .msg_type = SAMPLE_PCIV_MSG_WRITE_DONE,
            .msg_len = sizeof(sample_pciv_args_write_done),
        },
    };

    args.width    = frame_info->video_frame.width;
    args.height   = frame_info->video_frame.height;
    args.blk_size = g_co_processor_size;
    args.addr     = frame_info->video_frame.phys_addr[0]; /* notice the real phys addr, maybe not this */
    ret = memcpy_s(send_msg.c_msg_body, SAMPLE_PCIV_MSG_MAXLEN, &args, sizeof(sample_pciv_args_write_done));
    if (ret != EOK) {
        send_msg.msg_head.ret_val = TD_FAILURE;
        pciv_printf("memcpy_s send msg failed, error code:0x%x", ret);
        return ret;
    }
    ret = pciv_send_msg(chip, port, &send_msg);
    if (ret != TD_SUCCESS) {
        pciv_printf("send msg failed, error code:0x%x", ret);
        return ret;
    }
    return TD_SUCCESS;
}


static td_s32 sample_pciv_draw_rect(ot_rect *rect, ot_video_frame_info *frame, td_s32 scale)
{
    td_s32 ret;
    ot_vgs_handle handle = -1;
    ot_vgs_task_attr vgs_task;
    ot_cover cover;
    ot_point point[OT_QUAD_POINT_NUM] = {
        {rect->x * scale, rect->y * scale},
        {rect->x * scale + rect->width * scale, rect->y * scale},
        {rect->x * scale + rect->width * scale, rect->y * scale + rect->height * scale},
        {rect->x * scale, rect->y * scale + rect->height * scale}
    };

    ret = ss_mpi_vgs_begin_job(&handle);
    if (ret != TD_SUCCESS || handle == -1) {
        pciv_printf("vgs begin job failed, error code:0x%x", ret);
        return ret;
    }

    ret = memcpy_s(&vgs_task.img_in, sizeof(ot_video_frame_info), frame, sizeof(ot_video_frame_info));
    if (ret != EOK) {
        pciv_printf("copy img in failed");
        goto cancel_vgs;
    }
    ret = memcpy_s(&vgs_task.img_out, sizeof(ot_video_frame_info), frame, sizeof(ot_video_frame_info));
    if (ret != EOK) {
        pciv_printf("copy img out failed");
        goto cancel_vgs;
    }

    cover.type = OT_COVER_QUAD;
    cover.color = 0x00FF00;
    cover.quad.is_solid = TD_FALSE;
    cover.quad.thick = 2; /* 2: line width */
    ret = memcpy_s(cover.quad.point, OT_QUAD_POINT_NUM * sizeof(ot_point), point, sizeof(point));
    if (ret != EOK) {
        pciv_printf("copy quad point failed");
        goto cancel_vgs;
    }
    ret = ss_mpi_vgs_add_cover_task(handle, &vgs_task, &cover, 1);
    if (ret != TD_SUCCESS) {
        pciv_printf("mpi vgs add cover task fail, error code:0x%x", ret);
        goto cancel_vgs;
    }

    ret = ss_mpi_vgs_end_job(handle);
    if (ret != TD_SUCCESS) {
        pciv_printf("mpi vgs end job fail, error code:0x%x", ret);
        goto cancel_vgs;
    }
    return TD_SUCCESS;
cancel_vgs:
    ss_mpi_vgs_cancel_job(handle);
    return ret;
}

static td_void *sample_pciv_host_co_thread(td_void *p)
{
    sample_pciv_co_thread *co = (sample_pciv_co_thread *)p;
    td_s32 ret;
    const td_s32 milli_sec = 2400; /* 2400: get frame msec */
    ot_vpss_grp vpss_grp = co->id;
    ot_vo_chn vo_chn = co->id;
    ot_video_frame_info base_frame_info;
    ot_video_frame_info proc_frame_info;
    ot_rect *rect = TD_NULL;
    td_s32  scale;
    sample_pciv_msg recv_msg;

    prctl(PR_SET_NAME, "co_thread", 0, 0, 0);

    while (co->is_start && g_signo_flag) {
        ret = ss_mpi_vpss_get_chn_frame(vpss_grp, OT_VPSS_CHN3, &proc_frame_info, milli_sec);
        if (ret != TD_SUCCESS) {
            usleep(PCIV_WAIT_MSG_TIME);
            continue;
        }

        ret = sample_pciv_send_write_done_msg(co->chip, co->port, &proc_frame_info);
        if (ret != TD_SUCCESS) {
            goto release_proc;
        }
        ret = ss_mpi_vpss_get_chn_frame(vpss_grp, OT_VPSS_CHN2, &base_frame_info, milli_sec);
        if (ret != TD_SUCCESS) {
            pciv_printf("thread %d get vpss base frame failed, error code:0x%x", co->id, ret);
            goto release_proc;
        }
        ret = sample_pciv_read_msg(co->chip, co->port, &recv_msg);
        if (ret != TD_SUCCESS || recv_msg.msg_head.ret_val != TD_SUCCESS) {
            goto release_base;
        }

        rect = (ot_rect *)recv_msg.c_msg_body;
        scale = base_frame_info.video_frame.width / proc_frame_info.video_frame.width;
        /* do something with rect on base frame, we draw a rect... */
        ret = sample_pciv_draw_rect(rect, &base_frame_info, scale);
        if (ret != TD_SUCCESS || recv_msg.msg_head.ret_val != TD_SUCCESS) {
            goto release_base;
        }
        ret = ss_mpi_vo_send_frame(0, vo_chn, &base_frame_info, -1);
        if (ret != TD_SUCCESS) {
            pciv_printf("thread %d send vo frame failed, error code:0x%x", co->id, ret);
        }

release_base:
        ss_mpi_vpss_release_chn_frame(vpss_grp, OT_VPSS_CHN2, &base_frame_info);
release_proc:
        ss_mpi_vpss_release_chn_frame(vpss_grp, OT_VPSS_CHN3, &proc_frame_info);
    }

    return TD_NULL;
}

static td_s32 sample_pciv_init_slave_coprocessor(td_void)
{
    td_u32 i, j;
    td_s32 ret;
    td_s32 chip_id;
    sample_pciv_msg msg = { 0 };
    sample_pciv_args_co co = { 0 };

    for (j = 0; j < g_chips.chip_num; j++) {
        chip_id = g_chips.chip_id[j];
        for (i = 0; i < SAMPLE_PCIV_CHN_PER_CHIP; i++) {
            co.id = i;
            co.blk_size = g_co_processor_size;
            pciv_check_err_return(pciv_alloc_msg_port(&co.port));
            pciv_check_err_return(pciv_open_msg_port(chip_id, co.port));

            msg.msg_head.msg_type = SAMPLE_PCIV_MSG_INIT_COPROCESSOR;
            msg.msg_head.msg_len = sizeof(sample_pciv_args_co);
            ret = memcpy_s(msg.c_msg_body, SAMPLE_PCIV_MSG_MAXLEN, &co, sizeof(sample_pciv_args_co));
            if (ret != EOK) {
                pciv_printf("copy msg failed");
                return TD_FAILURE;
            }
            sample_pciv_send_cmd(chip_id, &msg);
            if (msg.msg_head.ret_val != TD_SUCCESS) {
                return TD_FAILURE;
            }
            g_all_thrad[chip_id][i].is_start = TD_TRUE;
            g_all_thrad[chip_id][i].id       = j * SAMPLE_PCIV_CHN_PER_CHIP + i;
            g_all_thrad[chip_id][i].chip     = chip_id;
            g_all_thrad[chip_id][i].blk_size = co.blk_size;
            g_all_thrad[chip_id][i].port     = co.port;
            pthread_create(&g_all_thrad[chip_id][i].pid, TD_NULL, sample_pciv_host_co_thread, &g_all_thrad[chip_id][i]);
        }
    }

    return TD_SUCCESS;
}

static td_void sample_pciv_exit_slave_coprocessor(td_void)
{
    td_u32 i, j;
    td_s32 ret;
    td_s32 chip_id;
    sample_pciv_msg msg = { 0 };
    sample_pciv_args_co co = { 0 };

    for (j = 0; j < g_chips.chip_num; j++) {
        chip_id = g_chips.chip_id[j];
        for (i = 0; i < SAMPLE_PCIV_CHN_PER_CHIP; i++) {
            co.id = i;
            if (g_all_thrad[chip_id][i].is_start == TD_TRUE) {
                g_all_thrad[chip_id][i].is_start = TD_FALSE;
                pthread_join(g_all_thrad[chip_id][i].pid, TD_NULL);
            }

            msg.msg_head.msg_type = SAMPLE_PCIV_MSG_EXIT_COPROCESSOR;
            msg.msg_head.msg_len = sizeof(sample_pciv_args_co);
            ret = memcpy_s(msg.c_msg_body, SAMPLE_PCIV_MSG_MAXLEN, &co, sizeof(sample_pciv_args_co));
            if (ret != EOK) {
                pciv_printf("copy msg failed");
                continue;
            }
            sample_pciv_send_cmd(chip_id, &msg);

            ret = pciv_close_msg_port(chip_id, g_all_thrad[chip_id][i].port);
            if (ret != TD_SUCCESS) {
                pciv_printf("pciv close msg error, err code:0x%x", ret);
            }
        }
    }
    return;
}

static td_s32 sample_pciv_coprocessor_bind_vpss(td_u32 vpss_grp_cnt)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < vpss_grp_cnt; i++) {
        ret = sample_comm_vdec_bind_vpss(i, i);
        if (ret != TD_SUCCESS) {
            pciv_printf("bind vdec %u to vpss %u error, ret:0x%x", i, i, ret);
            return ret;
        }
    }
    return TD_SUCCESS;
}
static td_void sample_pciv_coprocessor_unbind_vpss(td_u32 vpss_grp_cnt)
{
    td_u32 i;

    for (i = 0; i < vpss_grp_cnt; i++) {
        sample_comm_vdec_un_bind_vpss(i, i);
    }
}

static td_s32 sample_pciv_get_default_vdec(td_u32 chn_num)
{
    td_s32 ret;
    td_u32 i;

    g_pciv_vdec_thread_param[0].e_thread_ctrl = THREAD_CTRL_START;
    for (i = 1; i < chn_num &&  i < OT_VDEC_MAX_CHN_NUM; i++) {
        ret = memcpy_s(&g_pciv_vdec_cfg[i], sizeof(sample_vdec_attr), &g_pciv_vdec_cfg[0], sizeof(sample_vdec_attr));
        if (ret != EOK) {
            pciv_printf("copy vdec cfg failed");
            return ret;
        }
        ret = memcpy_s(&g_pciv_vdec_thread_param[i], sizeof(vdec_thread_param),
            &g_pciv_vdec_thread_param[0], sizeof(vdec_thread_param));
        if (ret != EOK) {
            pciv_printf("copy vdec thread param failed");
            return ret;
        }
        g_pciv_vdec_thread_param[i].chn_id = i;
    }
    return TD_SUCCESS;
}

static td_s32 sample_pciv_coprocessor(td_void)
{
    td_s32 ret;
    td_u32 chn_num = g_chips.chip_num * SAMPLE_PCIV_CHN_PER_CHIP;

    ret = sample_pciv_get_default_vdec(chn_num);
    if (ret != TD_SUCCESS) {
        pciv_printf("get default vdec error, ret:0x%x", ret);
        return ret;
    }
    ret = sample_pciv_init_vdec(chn_num, g_pciv_vdec_cfg);
    if (ret != TD_SUCCESS) {
        pciv_printf("init vdec error, ret:0x%x", ret);
        return ret;
    }
    ret = sample_pciv_init_vpss(chn_num);
    if (ret != TD_SUCCESS) {
        pciv_printf("init vpss error, ret:0x%x", ret);
        goto exit_vdec;
    }
    ret = sample_pciv_coprocessor_bind_vpss(chn_num);
    if (ret != TD_SUCCESS) {
        goto exit_vpss;
    }
    ret = sample_pciv_init_vo();
    if (ret != TD_SUCCESS) {
        goto unbind;
    }
    ret = sample_pciv_init_slave_coprocessor();
    if (ret != TD_SUCCESS) {
        goto processor_exit;
    }
    sample_comm_vdec_start_send_stream(chn_num, g_pciv_vdec_thread_param, g_pciv_vdec_thread, chn_num, chn_num);

    if (g_signo_flag) {
        printf("\n#################Press enter to exit########################\n");
        getchar();
    }

    sample_comm_vdec_stop_send_stream(chn_num, g_pciv_vdec_thread_param, g_pciv_vdec_thread, chn_num, chn_num);
processor_exit:
    sample_pciv_exit_slave_coprocessor();
    sample_pciv_exit_vo();
unbind:
    sample_pciv_coprocessor_unbind_vpss(chn_num);
exit_vpss:
    sample_pciv_exit_vpss(chn_num);
exit_vdec:
    sample_pciv_exit_vdec(chn_num);
    return ret;
}

static td_void sample_pciv_usage(td_void)
{
    printf("press sample command as follows!\n");
    printf("\t 1) slave chip as a coprocessor\n");
    printf("\t q) Quit\n");
    printf("sample command:");
    return;
}

static td_void sample_pciv_host_quit(td_s32 rmt_chip)
{
    sample_pciv_msg msg;

    /* send msg to slave chip to deal with ctrl+c */
    msg.msg_head.target = rmt_chip;
    msg.msg_head.msg_type = SAMPLE_PCIV_MSG_QUIT;
    msg.msg_head.msg_len = 0;
    printf("\n Send quit message to slave!\n");
    sample_pciv_send_cmd(rmt_chip, &msg);
}

static td_void sample_pciv_handle_sig(td_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_signo_flag = TD_FALSE;
    }
}

static td_s32 sample_pciv_echo_key_q()
{
    td_u32 i;
    td_s32 rmt_chip_id;

    for (i = 0; i < g_chips.chip_num; i++) {
        rmt_chip_id = g_chips.chip_id[i];
        sample_pciv_host_quit(rmt_chip_id);
    }

    return TD_SUCCESS;
}

static td_s32 sample_pciv_for_func(td_char ch)
{
    td_s32 ret = TD_SUCCESS;

    pciv_free_all_msg_port();
    switch (ch) {
        case '1': {
            ret = sample_pciv_coprocessor();
            break;
        }
        default: {
            printf("input invalid! please try again.\n");
            g_quit = TD_FALSE;
            break;
        }
    }

    return ret;
}

static td_void sample_pciv_exit(td_void)
{
    sample_pciv_exit_pcie(&g_chips);
    sample_comm_sys_exit();
}

static td_s32 sample_pciv_init(td_void)
{
    td_s32 ret;

    pciv_check_err_return(sample_pciv_init_media_sys());
    ret = sample_pciv_init_pcie(&g_local_id, &g_chips);
    if (ret != TD_SUCCESS) {
        sample_pciv_exit();
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

/******************************************************************************
* function    : main()
* Description : pciv
******************************************************************************/
int main(void)
{
    td_s32 ret;
    td_char ch;

    g_signo_flag = TD_TRUE;
    sample_sys_signal(&sample_pciv_handle_sig);

    pciv_check_err_return(sample_pciv_init());

    while (g_signo_flag) {
        sample_pciv_usage();

        ch = getchar();
        if (ch == '\n' || g_signo_flag != TD_TRUE) {
            continue;
        }
        getchar();

        g_quit = TD_FALSE;
        if (ch == '1') {
            ret = sample_pciv_for_func(ch);
        } else if (ch == 'q') {
            ret = sample_pciv_echo_key_q();
            g_quit = TD_TRUE;
        } else {
            printf("input invalid! please try again.\n");
        }

        if (ret != TD_SUCCESS) {
            g_quit = TD_TRUE;
        }

        if (g_quit) {
            break;
        }
    }

    sample_pciv_exit();
    if (!g_signo_flag || ret != TD_SUCCESS) {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        return TD_FAILURE;
    } else {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
        return TD_SUCCESS;
    }
}

