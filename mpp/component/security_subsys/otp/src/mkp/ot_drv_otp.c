/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/kernel.h>
#include "ot_drv_otp.h"
#include "kapi_otp.h"

#ifdef OT_OTP_V100
td_s32 ot_drv_otp_load_key_to_klad(const td_char *key_name)
{
    return kapi_otp_load_key_to_klad(key_name);
}
EXPORT_SYMBOL(ot_drv_otp_load_key_to_klad);
#endif