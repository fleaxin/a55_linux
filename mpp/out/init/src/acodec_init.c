/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_acodec_mod_init.h"

static __init int acodec_mod_init(void)
{
    return acodec_init();
}

static __exit void acodec_mod_exit(void)
{
    acodec_exit();
}

module_init(acodec_mod_init);
module_exit(acodec_mod_exit);

MODULE_LICENSE("Proprietary");

