/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <linux/of_platform.h>
#include <asm/io.h>
#include "ot_common.h"
#include "ot_osal.h"
#include "mod_ext.h"
#include "securec.h"
#include "ot_svp_npu_mod_init.h"

#define SVP_NPU_DEV_NAME_LENGTH 8
#define SVP_NPU_DEFAULT_TASK_NODE_NUM 512
#define SVP_NPU_DEV_NUM 1

static td_u8 g_power_save_en = 1;
static td_u16 g_max_task_node_num = SVP_NPU_DEFAULT_TASK_NODE_NUM;
static td_u32 *g_svp_npu_init_reg[SVP_NPU_DEV_NUM] = { TD_NULL };
static td_u32 g_svp_npu_init_irq[SVP_NPU_DEV_NUM] = { 0 };

module_param_named(svp_npu_save_power, g_power_save_en, byte, S_IRUGO);
module_param_named(svp_npu_max_task_node_num, g_max_task_node_num, ushort, S_IRUGO);

static int svp_npu_probe(struct platform_device *pf_dev)
{
    td_s32 irq, ret;
    td_char svp_npu_dev_name[SVP_NPU_DEV_NAME_LENGTH] = { '\0' };
    struct resource *mem = TD_NULL;
    ot_svp_npu_mod_param mod_param;
    td_void *reg = TD_NULL;

    if (cmpi_get_module_name(OT_ID_PQP) != TD_NULL) {
        printk("Attention: svp_npu cannot run because the pqp.ko has been loaded!\n");
        return TD_FAILURE;
    }

    ret = snprintf_s(svp_npu_dev_name, SVP_NPU_DEV_NAME_LENGTH, SVP_NPU_DEV_NAME_LENGTH - 1, "svp_npu");
    if ((ret < 0) || (ret > (SVP_NPU_DEV_NAME_LENGTH - 1))) {
        printk("snprintf_s npu name failed\n");
        return TD_FAILURE;
    }
    mem = osal_platform_get_resource_byname(pf_dev, IORESOURCE_MEM, svp_npu_dev_name);
    reg = devm_ioremap_resource(&pf_dev->dev, mem);
    if (IS_ERR(reg)) {
        printk("devm_ioremap_resource failed\n");
        return PTR_ERR(reg);
    }

    irq = osal_platform_get_irq_byname(pf_dev, "svp_npu_ns");
    if (irq <= 0) {
        dev_err(&pf_dev->dev, "can't find npu IRQ\n");
        return TD_FAILURE;
    }
    g_svp_npu_init_irq[0] = (td_u32)irq;
    g_svp_npu_init_reg[0] = (td_u32 *)reg;

    mod_param.power_save_en = g_power_save_en;
    mod_param.max_task_node_num = g_max_task_node_num;
    if (svp_npu_set_init_reg(g_svp_npu_init_reg, SVP_NPU_DEV_NUM) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    if (svp_npu_set_init_irq(g_svp_npu_init_irq, SVP_NPU_DEV_NUM) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    (td_void)svp_npu_set_mod_param(&mod_param);
    return svp_npu_mod_init();
}

static int svp_npu_remove(struct platform_device *pf_dev)
{
    td_u32 i;

    ot_unused(pf_dev);
    svp_npu_mod_exit();
    for (i = 0; i < SVP_NPU_DEV_NUM; i++) {
        g_svp_npu_init_reg[i] = TD_NULL;
    }
    return 0;
}

static const struct of_device_id g_svp_npu_match[] = {
    { .compatible = "vendor,svp_npu" },
    {},
};
MODULE_DEVICE_TABLE(of, g_svp_npu_match);

static struct platform_driver g_svp_npu_driver = {
    .probe          = svp_npu_probe,
    .remove         = svp_npu_remove,
    .driver         = {
        .name   = "svp_npu",
        .of_match_table = g_svp_npu_match,
    },
};

osal_module_platform_driver(g_svp_npu_driver);

MODULE_LICENSE("Proprietary");
