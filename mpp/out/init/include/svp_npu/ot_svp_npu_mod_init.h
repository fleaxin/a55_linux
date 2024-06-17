/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_SVP_NPU_MOD_INIT_H
#define OT_SVP_NPU_MOD_INIT_H
#include "ot_type.h"

typedef struct {
    td_u16 max_task_node_num;
    td_u8 power_save_en;
    td_u8 reserved;
} ot_svp_npu_mod_param;

td_s32 svp_npu_set_init_reg(td_u32 *svp_npu_reg[], td_u32 num);
td_s32 svp_npu_set_init_irq(td_u32 svp_npu_irq[], td_u32 num);
td_void svp_npu_set_mod_param(ot_svp_npu_mod_param *param);
td_s32 svp_npu_mod_init(td_void);
td_void svp_npu_mod_exit(td_void);
#endif
