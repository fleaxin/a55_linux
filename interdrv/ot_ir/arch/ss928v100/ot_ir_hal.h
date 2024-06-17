/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_IR_HAL_H__
#define __OT_IR_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_IR_DEVICE_IRQ_NO 55 /* 55 is IR Interrupt source */
#define IR_REG_BASE 0x110f0000 /* 0x110f0000 is IR reg base Address */

#define IR_CRG_ADDR 0x11014640 /* 0x11014640 is IR CRG Address */
#define IR_BIT 4 /* 4bit is IR CRG Clock enable */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
