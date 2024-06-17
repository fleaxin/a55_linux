/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_VEDU_MOD_INIT_H
#define OT_VEDU_MOD_INIT_H

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

int vpu_mod_init(void);
void vpu_mod_exit(void);
td_s32 vedu_set_irq(td_s32 vpu_id, td_s32 irq);
td_s32 jpeg_set_irq(td_s32 vpu_id, td_s32 irq);
td_s32 vedu_set_high_profile(td_bool high_profile);
td_s32 vedu_check_apll_clk(td_void);
td_void vedu_set_vedu_enable(td_u32 *vedu_en, td_u32 len);
td_void vedu_get_vedu_enable(td_u32 *vedu_en, td_u32 len);
td_void vedu_set_vedu_addr(td_s32 vpu_id, td_void *addr);
td_void jpge_set_jpge_addr(td_s32 vpu_id, td_void *addr);
td_u64 vedu_get_hal_addr(td_s32 vpu_id);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef OT_VEDU_MOD_INIT_H */
