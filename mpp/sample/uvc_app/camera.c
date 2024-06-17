/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include "ot_uvc.h"
#include "ot_uac.h"
#include "ot_stream.h"
#include "ot_audio.h"
#include "camera.h"
#include "ot_type.h"
/* -------------------------------------------------------------------------- */

unsigned int g_uac = 1;
static pthread_t g_st_uvc_pid;
static pthread_t g_st_uac_pid;

static int __init()
{
    sample_venc_config();

    if (get_ot_uvc()->init() != 0 || ot_stream_init() != 0) {
        return -1;
    }

    if (g_uac) {
#ifdef OT_UAC_COMPILE
        sample_audio_config();
#endif

        if (get_ot_uac()->init() != 0 || ot_audio_init() != 0) {
            return -1;
        }
    }

    return 0;
}

static int __open()
{
#ifdef OT_UVC_COMPILE
    if (get_ot_uvc()->open() != 0) {
        return -1;
    }
#endif

    if (g_uac) {
        if (get_ot_uac()->open() != 0) {
            return -1;
        }
    }

    return 0;
}

static int __close()
{
#ifdef OT_UVC_COMPILE
    get_ot_uvc()->close();
#endif
    ot_stream_shutdown();
    if (g_uac) {
        get_ot_uac()->close();
        ot_audio_shutdown();
    }
    ot_stream_deinit();
    return 0;
}

td_void *uvc_thread(td_void *p)
{
#ifdef OT_UVC_COMPILE
    get_ot_uvc()->run();
#endif
    return NULL;
}

td_void *uac_thread(td_void *p)
{
    get_ot_uac()->run();
    return NULL;
}

static int __run()
{
    pthread_create(&g_st_uvc_pid, 0, uvc_thread, NULL);
    pthread_create(&g_st_uac_pid, 0, uac_thread, NULL);

#ifndef OT_UVC_COMPILE
    const int running = 1;

    while (running) {
        if (sample_uvc_get_quit_flag() != 0) {
            break;
        }
        usleep(10 * 1000); /* 10 * 1000us sleep */
    }
#endif

    pthread_join(g_st_uvc_pid, NULL);
    pthread_join(g_st_uac_pid, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */

static ot_camera g_ot_camera = {
    .init = __init,
    .open = __open,
    .close = __close,
    .run = __run,
};

ot_camera *get_ot_camera()
{
    return &g_ot_camera;
}

void release_ot_camera(ot_camera *camera) {}

unsigned int get_g_uac_val()
{
    return g_uac;
}
