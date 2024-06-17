/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef GFBG_INIT_H
#define GFBG_INIT_H
#include <linux/dma-buf.h>
#include "ot_type.h"
#include "gfbg_vou_graphics.h"
#include "gfbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void set_video_name(char *temp_video);

#ifndef __LITEOS__

struct dma_buf *gfbg_memblock_export(phys_addr_t base, size_t size, int flags);
#endif
char *gfbg_get_layer_mmz_names(td_u32 layer_id);

int gfbg_pts_init(void);

void gfbg_pts_exit(void);

int gfbg_vsync_init(void);

void gfbg_vsync_exit(void);

int gfbg_vsync_notify(void);

td_void gfbg_cleanup(td_void);

td_s32 gfbg_init(td_void);

#ifndef CONFIG_OT_VO_FB_SEPARATE
td_void set_global_name(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
