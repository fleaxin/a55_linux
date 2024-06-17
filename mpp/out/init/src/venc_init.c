/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include "ot_type.h"
#include "ot_venc_mod_init.h"

static td_u32 g_venc_max_chn_num = OT_VENC_MAX_CHN_NUM;
module_param(g_venc_max_chn_num, uint, S_IRUGO);

static int __init venc_mod_init(void)
{
    venc_set_max_chn_num(g_venc_max_chn_num);
    if (venc_module_init() != TD_SUCCESS) {
        return -1;
    }

    return 0;
}

static void __exit venc_mod_exit(void)
{
    venc_module_exit();
}

module_init(venc_mod_init);
module_exit(venc_mod_exit);

MODULE_LICENSE("Proprietary");
