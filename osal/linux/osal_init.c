/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include "ot_osal.h"
#include "osal.h"

static int __init osal_init(void)
{
    if (mem_check_module_param() == -1) {
        return -1;
    }

    osal_device_init();
    osal_proc_init();
    if (ot_media_init() != 0) {
        goto failed;
    }
    if (media_mem_init() != 0) {
        ot_media_exit();
        goto failed;
    }
    osal_trace("load ot_osal %s ....OK!\n", OT_OSAL_VERSION);
    return 0;

failed:
    osal_proc_exit();
    osal_trace("load ot_osal %s failed!\n", OT_OSAL_VERSION);
    return -1;
}

static void __exit osal_exit(void)
{
    media_mem_exit();
    ot_media_exit();
    osal_proc_exit();
    osal_device_exit();
    osal_trace("unload ot_osal %s ....OK!\n", OT_OSAL_VERSION);
}

module_init(osal_init);
module_exit(osal_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
