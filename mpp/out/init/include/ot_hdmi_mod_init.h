/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef HDMI_MOD_INIT_H
#define HDMI_MOD_INIT_H

#include "ot_defines.h"
#include "ot_type.h"
#include "ot_osal.h"

td_s32 hdmi_set_reg(td_u32 id, td_char *reg);
td_s32 hdmi_set_phy(td_u32 id, char *phy);
int hdmi_drv_mod_init(void);
void hdmi_drv_mod_exit(void);

#endif /* HDMI_MOD_INIT_H */

