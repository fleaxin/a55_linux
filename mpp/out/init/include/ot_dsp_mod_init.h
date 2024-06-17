/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_DSP_MOD_INIT_H
#define OT_DSP_MOD_INIT_H
#include "ot_type.h"

typedef struct {
    td_u16 dsp_max_node_num;
    td_u16 dsp_init_mode;
} ot_svp_dsp_mod_param;

td_s32 svp_dsp_set_init_reg(td_u32 *dsp_reg[], td_u32 num);
td_void svp_dsp_set_mod_param(const ot_svp_dsp_mod_param *param);
td_s32 svp_dsp_std_mod_init(td_void);
td_void svp_dsp_std_mod_exit(td_void);
#endif
