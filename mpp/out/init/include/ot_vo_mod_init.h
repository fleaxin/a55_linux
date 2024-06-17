/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_VO_MOD_INIT_H
#define OT_VO_MOD_INIT_H

#include "ot_defines.h"
#include "ot_type.h"
#include "ot_osal.h"
#include "ot_vo_export.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

td_void vo_set_init_vo_reg(td_void *reg);
td_void vo_set_init_vo_irq(td_u32 vo_irq);
td_s32 vo_mod_init(td_void);
td_void vo_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef OT_VO_MOD_INIT_H */
