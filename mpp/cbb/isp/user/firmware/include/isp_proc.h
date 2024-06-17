/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_PROC_H
#define ISP_PROC_H

#include "ot_common.h"
#include "isp_main.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

td_s32 isp_proc_init(ot_vi_pipe vi_pipe);
td_s32 isp_proc_write(const isp_alg_node *algs, ot_vi_pipe vi_pipe);
td_void isp_proc_exit(ot_vi_pipe vi_pipe);

#define isp_proc_printf(proc, len, fmt...)                                              \
    do {                                                                                \
        (td_void)snprintf_s((proc)->proc_buff, (proc)->buff_len, (proc)->buff_len - 1, ##fmt); \
        (proc)->write_len = strlen((proc)->proc_buff);                               \
        (proc)->proc_buff = &((proc)->proc_buff[(proc)->write_len]);                \
        (proc)->buff_len -= (proc)->write_len;                                      \
        len += (proc)->write_len;                                                     \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
