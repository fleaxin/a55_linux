/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_VENC_MOD_INIT_H
#define OT_VENC_MOD_INIT_H

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

int venc_module_init(void);
void venc_module_exit(void);
td_void venc_set_max_chn_num(td_u32 num);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef OT_VENC_MOD_INIT_H */
