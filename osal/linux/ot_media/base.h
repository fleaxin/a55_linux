/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef _OT_MEDIA_BASE_H_
#define _OT_MEDIA_BASE_H_

#include "ot_media.h"

int ot_media_bus_init(void);
void ot_media_bus_exit(void);

int ot_media_device_register(struct ot_media_device *pdev);

void ot_media_device_unregister(struct ot_media_device *pdev);

struct ot_media_driver *ot_media_driver_register(const char *name, struct module *owner);

void ot_media_driver_unregister(struct ot_media_driver *pdrv);

#endif
