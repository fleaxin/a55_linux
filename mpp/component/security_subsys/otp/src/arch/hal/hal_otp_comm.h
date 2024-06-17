/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HAL_OTP_COMM_H
#define HAL_OTP_COMM_H

#include "ot_type.h"

td_s32 hal_otp_init(td_void);

td_void hal_otp_deinit(td_void);

td_s32 hal_otp_read_word(td_u32 offset, td_u32 *value);

td_s32 hal_otp_write_word(td_u32 offset, td_u32 value);

#endif /* HAL_OTP_COMM_H */
