/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SYS_CTRL_H
#define SYS_CTRL_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    VI_ONLINE_VIDEO_NORM_VPSS_ONLINE = 0, /* OT_VI_VIDEO_MODE_NORM OT_VI_ONLINE_VPSS_ONLINE */
    VI_ONLINE_VIDEO_NORM_VPSS_OFFLINE = 1, /* OT_VI_VIDEO_MODE_NORM OT_VI_ONLINE_VPSS_OFFLINE */
    VI_ONLINE_VIDEO_ADVANCED_VPSS_OFFLINE = 2, /* OT_VI_VIDEO_MODE_ADVANCED OT_VI_ONLINE_VPSS_OFFLINE */
    VI_OFFLINE_VIDEO_NORM_VPSS_ONLINE = 3, /* OT_VI_VIDEO_MODE_NORM OT_VI_OFFLINE_VPSS_ONLINE */
    VI_OFFLINE_VIDEO_ADVANCED_VPSS_OFFLINE = 4, /* OT_VI_VIDEO_MODE_ADVANCED OT_VI_OFFLINE_VPSS_OFFLINE */

    VI_ONLINE_VIDEO_ADVANCED_VPSS_ONLINE = 5, /* OT_VI_VIDEO_MODE_ADVANCED OT_VI_ONLINE_VPSS_ONLINE */
    VI_OFFLINE_VIDEO_NORM_VPSS_OFFLINE = 6, /* OT_VI_VIDEO_MODE_NORM OT_VI_OFFLINE_VPSS_OFFLINE */
    VI_OFFLINE_VIDEO_ADVANCED_VPSS_ONLINE = 7, /* OT_VI_VIDEO_MODE_ADVANCED OT_VI_OFFLINE_VPSS_ONLINE */

    VI_VPSS_ONLINE_BUTT
} vi_vpss_online_mode;

void sys_ctl(int online_flag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SYS_CTRL_H */