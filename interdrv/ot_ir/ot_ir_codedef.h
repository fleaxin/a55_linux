/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_IR_CODEDEF_H
#define OT_IR_CODEDEF_H
#include "ot_ir.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static ot_ir_dev_param g_static_dev_param[] = {
    /* NEC with simple repeat code : uPD6121G */
    { 828, 972, 414, 486, 45, 67, 135, 203, 180, 270, 32, 0, OT_IR_DEFAULT_FREQ },

    /* NEC with simple repeat code : D6121/BU5777/D1913 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 207, 243, 32, 0, OT_IR_DEFAULT_FREQ },

    /* NEC with simple repeat code : LC7461M-C13 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 207, 243, 42, 0, OT_IR_DEFAULT_FREQ },

    /* NEC with simple repeat code : AEHA */
    { 270, 405, 135, 203, 34, 51, 101, 152, 270, 405, 48, 0, OT_IR_DEFAULT_FREQ },

    /* TC9012 : TC9012F/9243 */
    { 414, 486, 414, 486, 45, 67, 135, 203, 0, 0, 32, 1, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : uPD6121G */
    { 828, 972, 414, 486, 45, 67, 135, 203, 0, 0, 32, 2, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : LC7461M-C13 */
    { 828, 972, 414, 486, 45, 67, 135, 203, 0, 0, 42, 2, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : MN6024-C5D6 */
    { 270, 405, 270, 405, 68, 101, 203, 304, 0, 0, 22, 2, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : MN6014-C6D6 */
    { 279, 419, 279, 419, 70, 105, 140, 210, 0, 0, 24, 2, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : MATNEW */
    { 279, 419, 300, 449, 35, 52, 105, 157, 0, 0, 48, 2, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : MN6030 */
    { 279, 419, 279, 419, 70, 105, 210, 314, 0, 0, 22, 2, OT_IR_DEFAULT_FREQ },

    /* NEC with full repeat code : PANASONIC */
    { 282, 422, 282, 422, 70, 106, 211, 317, 0, 0, 22, 2, OT_IR_DEFAULT_FREQ },

    /* SONY-D7C5 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 12, 3, OT_IR_DEFAULT_FREQ },

    /* SONY-D7C6 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 13, 3, OT_IR_DEFAULT_FREQ },

    /* SONY-D7C8 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 15, 3, OT_IR_DEFAULT_FREQ },

    /* SONY-D7C13 */
    { 192, 288, 48, 72, 48, 72, 96, 144, 0, 0, 20, 3, OT_IR_DEFAULT_FREQ },
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_IR_CODEDEF_H */
