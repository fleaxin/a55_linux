/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_MAU_MOD_INIT_H
#define OT_MAU_MOD_INIT_H
#include "ot_type.h"

typedef struct {
    td_u16 mau_max_mem_info_num;
    td_u16 mau_max_node_num;
    td_u8 mau_power_save_en;
    td_u8 reserved;
} ot_svp_mau_mod_param;

td_s32 svp_mau_set_init_reg(td_u32 *mau_reg[], td_u32 num);
td_s32 svp_mau_set_init_irq(const td_u32 mau_irq[], td_u32 num);
td_void svp_mau_set_mod_param(const ot_svp_mau_mod_param *param);
td_s32 svp_mau_mod_init(td_void);
td_void svp_mau_mod_exit(td_void);
#endif
