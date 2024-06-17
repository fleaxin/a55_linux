/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_UAC_H__
#define __OT_UAC_H__

typedef struct ot_uac {
    int (*init)();
    int (*open)();
    int (*close)();
    int (*run)();
} ot_uac;

ot_uac *get_ot_uac();
void release_ot_uac(ot_uac *uvc);

#endif // __OT_UAC_H__
