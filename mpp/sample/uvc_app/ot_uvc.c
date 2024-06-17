/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <pthread.h>
#include <unistd.h>
#include "log.h"
#include "ot_uvc.h"
#include "uvc.h"
#include "camera.h"

static pthread_t g_stuvc_send_data_pid;

static int __init()
{
    return 0;
}

static int __open()
{
    const char *devpath = "/dev/video0";

    return open_uvc_device(devpath);
}

static int __close()
{
    return close_uvc_device();
}

void *uvc_send_data_thread(void *p)
{
    int status = 0;
    const int running = 1;

    while (running) {
        if (sample_uvc_get_quit_flag() != 0) {
            return NULL;
        }
        status = run_uvc_data();
    }

    rlog("uvc_send_data_thread exit, status: %d.\n", status);

    return NULL;
}

static int __run()
{
    int status;
    const int running = 1;

    pthread_create(&g_stuvc_send_data_pid, 0, uvc_send_data_thread, NULL);

    while (running) {
        if (sample_uvc_get_quit_flag() != 0) {
            break;
        }

        status = run_uvc_device();
        if (status < 0) {
            break;
        }
    }

    pthread_join(g_stuvc_send_data_pid, NULL);

    return 0;
}

/* ---------------------------------------------------------------------- */

static ot_uvc g_ot_uvc = {
    .init = __init,
    .open = __open,
    .close = __close,
    .run = __run,
};

ot_uvc *get_ot_uvc()
{
    return &g_ot_uvc;
}

void release_ot_uvc(ot_uvc *uvc) {}
