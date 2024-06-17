/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef _LINUX_OT_MEDIA_DEVICE_H_
#define _LINUX_OT_MEDIA_DEVICE_H_

#include <linux/module.h>
#include <linux/major.h>
#include <linux/device.h>
#include "osal_list.h"

#define OT_MEDIA_DEVICE_MAJOR     218
#define OT_MEDIA_DYNAMIC_MINOR    255

struct ot_media_device;

#define OT_MEDIA_MAX_DEV_NAME_LEN 32

struct ot_media_driver {
    struct device_driver driver;
    char name[OT_MEDIA_MAX_DEV_NAME_LEN];
};

#define to_ot_media_driver(drv) \
    container_of((drv), struct ot_media_driver, driver)

struct ot_media_device {
    struct osal_list_head list;

    char devfs_name[OT_MEDIA_MAX_DEV_NAME_LEN];

    unsigned int minor;

    struct device device;

    struct module *owner;

    const struct file_operations *fops;

    /* for internal use */
    struct ot_media_driver *driver;
};

#define to_ot_media_device(dev) \
    container_of((dev), struct ot_media_device, device)

int ot_media_register(struct ot_media_device *pdev);

int ot_media_unregister(struct ot_media_device *pdev);

#endif /* _LINUX_OT_MEDIA_DEVICE_H_ */
