/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef NPUDEV_MOD_INIT_H
#define NPUDEV_MOD_INIT_H

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

td_u32 npudev_get_reg_num(td_void);
td_u32 npudev_get_irq_num(td_void);

td_void npudev_set_npu_reg_addr(td_u32 index, td_void *addr);
td_void npudev_set_npu_irq_no(td_u32 index, td_s32 irq);

td_void *npudev_get_npu_reg_addr(td_u32 index);

td_s32 npudev_module_init(td_void);
td_void npudev_module_exit(td_void);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef NPUDEV_MOD_INIT_H */
