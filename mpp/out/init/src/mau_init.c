/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <linux/of_platform.h>

#include "ot_common.h"
#include "ot_osal.h"
#include "securec.h"
#include "ot_mau_mod_init.h"

#define OT_SVP_MAU_DEV_NAME_LENGTH 8
#define OT_SVP_MAU_DEFAULT_MAX_MEM_INFO_NUM 32
#define OT_SVP_MAU_DEFAULT_MAX_NODE_NUM 512
#define OT_SVP_MAU_DEV_NUM 1

static td_u16 g_max_mem_info_num = OT_SVP_MAU_DEFAULT_MAX_MEM_INFO_NUM;
static td_u16 g_max_node_num = OT_SVP_MAU_DEFAULT_MAX_NODE_NUM;
static td_u32 g_mau_init_irq[OT_SVP_MAU_DEV_NUM] = { 0 };

module_param_named(mau_max_mem_info_num, g_max_mem_info_num, ushort, S_IRUGO);
module_param_named(mau_max_node_num, g_max_node_num, ushort, S_IRUGO);

static int ot_svp_mau_probe(struct platform_device *pf_dev)
{
    td_u32 i;
    td_s32 irq, ret;
    td_char mau_dev_name[OT_SVP_MAU_DEV_NAME_LENGTH] = { '\0' };

    ot_svp_mau_mod_param mau_mod_param;

    for (i = 0; i < OT_SVP_MAU_DEV_NUM; i++) {
        ret = snprintf_s(mau_dev_name, OT_SVP_MAU_DEV_NAME_LENGTH, OT_SVP_MAU_DEV_NAME_LENGTH - 1, "mau%u", i);
        if ((ret < 0) || (ret > (OT_SVP_MAU_DEV_NAME_LENGTH - 1))) {
            printk("snprintf_s mau%u's name failed\n", i);
            return TD_FAILURE;
        }

        irq = osal_platform_get_irq_byname(pf_dev, mau_dev_name);
        if (irq <= 0) {
            dev_err(&pf_dev->dev, "can't find mau%u IRQ\n", i);
            return TD_FAILURE;
        }
        g_mau_init_irq[i] = (td_u32)irq;
    }
    mau_mod_param.mau_power_save_en = 0;
    mau_mod_param.mau_max_mem_info_num = g_max_mem_info_num;
    mau_mod_param.mau_max_node_num = g_max_node_num;

    if (svp_mau_set_init_irq(g_mau_init_irq, OT_SVP_MAU_DEV_NUM) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    (td_void)svp_mau_set_mod_param(&mau_mod_param);
    return svp_mau_mod_init();
}

static int ot_svp_mau_remove(struct platform_device *pf_dev)
{
    ot_unused(pf_dev);
    svp_mau_mod_exit();
    return 0;
}

static const struct of_device_id g_ot_svp_mau_match[] = {
    { .compatible = "vendor,mau" },
    {},
};
MODULE_DEVICE_TABLE(of, g_ot_svp_mau_match);

static struct platform_driver g_ot_svp_mau_driver = {
    .probe = ot_svp_mau_probe,
    .remove = ot_svp_mau_remove,
    .driver = {
        .name   = "ot_mau",
        .of_match_table = g_ot_svp_mau_match,
    },
};

osal_module_platform_driver(g_ot_svp_mau_driver);

MODULE_LICENSE("Proprietary");
