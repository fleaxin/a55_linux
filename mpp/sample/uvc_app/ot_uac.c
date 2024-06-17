/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <pthread.h>
#include <unistd.h>
#include "ot_uac.h"
#include "ot_audio.h"

static int __init()
{
    return 0;
}

static int __open()
{
    return 0;
}

static int __close()
{
    ot_audio_shutdown();
    return 0;
}

static int __run()
{
    ot_audio_init();
    ot_audio_startup();

    return 0;
}

/* ---------------------------------------------------------------------- */

static ot_uac g_ot_uac = {
    .init = __init,
    .open = __open,
    .close = __close,
    .run = __run,
};

ot_uac *get_ot_uac()
{
    return &g_ot_uac;
}

void release_ot_uac(ot_uac *uvc) {}
