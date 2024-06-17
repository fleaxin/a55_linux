/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_USER_H__
#define __OT_USER_H__

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <linux/pagemap.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <uapi/linux/sched/types.h>

#include "ot_vdec_export.h"
#include "ot_vo_export.h"
#include "ot_ao_export.h"
#include "ot_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VDEC_NOTIFY   0
#ifdef __LITEOS__
#define VDEC_SET_SCHEDULER 1
#else
#define VDEC_SET_SCHEDULER 0
#endif

#define AO_NOTIFY     0
#define VO_NOTIFY     0

#ifdef __cplusplus
}
#endif

#endif
