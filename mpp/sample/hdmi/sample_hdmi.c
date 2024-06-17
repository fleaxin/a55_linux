/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_common.h"
#include "sample_comm.h"
#include "sample_hdmi_cmd.h"
#include "sample_hdmi_video_path.h"
#include "sample_hdmi.h"

#define CMD_LEN 128
#define sample_get_input_cmd(input_cmd) fgets((char *)(input_cmd), (sizeof(input_cmd) - 1), stdin)

static td_bool g_start_audio = TD_FALSE;
static td_bool g_sample_exit = TD_FALSE;

#ifndef __LITEOS__
static void hdmi_test_handle_sig(td_s32 signo)
{
    if (g_sample_exit == TD_TRUE) {
        return;
    }

    if (signo == SIGINT || signo == SIGTERM) {
        g_sample_exit = TD_TRUE;
    }
}
#endif

td_s32 main(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_char input_cmd[CMD_LEN] = { 0 };
    ot_vo_intf_sync format = OT_VO_OUT_1080P60;

    ot_unused(argc);
    ot_unused(argv);
#ifndef __LITEOS__
    sample_sys_signal(hdmi_test_handle_sig);
#endif

    ret = sample_hdmi_vdec_vpss_vo_start(format);
    sample_if_failure_return(ret, TD_FAILURE);
    sample_hdmi_start(format);
    ret = sample_hdmi_start_audio();
    sample_if_failure_return(ret, TD_FAILURE);

    g_start_audio = TD_TRUE;

    while (g_sample_exit == TD_FALSE) {
        printf("please input 'h' to get help or 'q' to quit!\n");
        printf("hdmi_cmd >");
        sample_get_input_cmd(input_cmd);
        if (input_cmd[0] == 'q') {
            printf("prepare to quit!\n");
            break;
        }
        hdmi_test_cmd(input_cmd, CMD_LEN);
    }

    if (g_start_audio) {
        sample_hdmi_stop_audio();
    }
    sample_hdmi_media_set(TD_FALSE);
    sample_hdmi_vdec_vpss_vo_stop();

    return 0;
}

