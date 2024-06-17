/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "gfbg_init.h"
#include "ot_module_param.h"

char g_tmp_video[128] = "gfbg:vram0_size:8100"; /* 128 for vram length */
char *g_layer_mmz_names[GFBG_MAX_LAYER_NUM] = { [0 ... GFBG_MAX_LAYER_NUM - 1] = TD_NULL };

int gfbg_pts_init(void)
{
    return TD_SUCCESS;
}

void gfbg_pts_exit(void)
{
    return;
}

int gfbg_vsync_init(void)
{
    return TD_SUCCESS;
}

void gfbg_vsync_exit(void)
{
    return;
}

int gfbg_vsync_notify(void)
{
    return TD_SUCCESS;
}

td_s32 gfbg_get_module_para(td_void *args)
{
    GFBG_MODULE_PARAMS_S *gfbg_module_param = (GFBG_MODULE_PARAMS_S*)args;
    if (gfbg_module_param != TD_NULL) {
        if (strncpy_s(g_tmp_video, sizeof(g_tmp_video), gfbg_module_param->video, 127) != EOK) { /* 127:for varm len */
            gfbg_error("%s:%d:strncpy_s failure\n", __FUNCTION__, __LINE__);
            return TD_FAILURE;
        }
        g_tmp_video[127] = '\0'; /* 127:for varm length */
        set_video_name(g_tmp_video);
    }
    return TD_SUCCESS;
}

char *gfbg_get_layer_mmz_names(td_u32 layer_id)
{
    if (layer_id >= 1) { /* 1: max layer num for liteos. */
        return TD_NULL;
    }
    return g_layer_mmz_names[layer_id];
}

