/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_BASE_MOD_INIT_H
#define OT_BASE_MOD_INIT_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

int comm_init(void);

void comm_exit(void);

void vb_set_force_exit(td_u32 value);


#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */
#endif /* end of #ifndef OT_BASE_MOD_INIT_H */
