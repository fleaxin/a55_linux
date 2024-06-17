/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/of_platform.h>

#include "ot_photo_mod_init.h"
#include "ot_common.h"

static int __init photo_mod_init(void)
{
    return photo_drv_mod_init();
}

static void __exit photo_mod_exit(void)
{
    photo_drv_mod_exit();
}

module_init(photo_mod_init);
module_exit(photo_mod_exit);

MODULE_LICENSE("Proprietary");
