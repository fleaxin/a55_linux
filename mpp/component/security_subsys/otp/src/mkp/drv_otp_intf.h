/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef DRV_OTP_INTF_H
#define DRV_OTP_INTF_H

#include "ot_type.h"

td_s32 intf_otp_ioctl(td_u32 cmd, td_void *argp);

td_s32 otp_entry(td_void);

td_void otp_exit(td_void);

#endif /* DRV_OTP_INTF_H */
