/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "ot_h265e_mod_init.h"
#include <linux/module.h>

static int __init h265e_mod_init(void)
{
    return h265e_module_init();
}
static void __exit h265e_mod_exit(void)
{
    h265e_module_exit();
}

module_init(h265e_mod_init);
module_exit(h265e_mod_exit);

MODULE_LICENSE("Proprietary");
