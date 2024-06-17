/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OSAL_H__
#define __OSAL_H__

void osal_proc_init(void);
void osal_proc_exit(void);
int ot_media_init(void);
void ot_media_exit(void);
int media_mem_init(void);
void media_mem_exit(void);
int mem_check_module_param(void);
void osal_device_init(void);
void osal_device_exit(void);
#endif
