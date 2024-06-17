/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_AVS_MOD_INIT_H
#define OT_AVS_MOD_INIT_H

#include "ot_defines.h"
#include "ot_type.h"
#include "ot_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

td_void avs_set_init_avs_reg(td_void *reg);
td_void avs_set_init_avs_irq(unsigned int irq);
int avs_mod_init(void);
void avs_mod_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef OT_AVS_MOD_INIT_H */

