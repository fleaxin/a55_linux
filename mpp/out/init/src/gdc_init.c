/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/module.h>
#include <linux/of_platform.h>

#include "ot_common.h"
#include "ot_osal.h"
#include "ot_gdc_mod_init.h"

static unsigned int g_max_gdc_job;
static unsigned int g_max_gdc_task;
static unsigned int g_max_gdc_node;

module_param(g_max_gdc_job,  uint, S_IRUGO);
module_param(g_max_gdc_task, uint, S_IRUGO);
module_param(g_max_gdc_node, uint, S_IRUGO);

#define GDC_DEV_NAME_LENGTH 10
#define GDC_MAX_DEV_NUM 1

static int ot_gdc_probe(struct platform_device *pdev)
{
    td_u32 i;
    td_char gdc_dev_name[GDC_DEV_NAME_LENGTH] = "gdc";
    struct resource *mem = TD_NULL;
    td_void *gdc_reg = TD_NULL;
    td_s32 gdc_irq;

    for (i = 0; i < GDC_MAX_DEV_NUM; i++) {
        mem = osal_platform_get_resource_byname(pdev, IORESOURCE_MEM, gdc_dev_name);
        gdc_reg = devm_ioremap_resource(&pdev->dev, mem);
        if (IS_ERR(gdc_reg)) {
            osal_printk("devm_ioremap_resource!\n");
            return PTR_ERR(gdc_reg);
        }

        gdc_irq = osal_platform_get_irq_byname(pdev, gdc_dev_name);
        if (gdc_irq <= 0) {
            dev_err(&pdev->dev, "cannot find gdc%d IRQ\n", i);
        }

        if (gdc_set_init_gdc_reg(gdc_reg, i) != TD_SUCCESS) {
            osal_printk("gdc_set_init_gdc_reg!\n");
            return TD_FAILURE;
        }
        if (gdc_set_init_gdc_irq(gdc_irq, i) != TD_SUCCESS) {
            osal_printk("gdc_set_init_gdc_irq!\n");
            return TD_FAILURE;
        }
    }

    if (g_max_gdc_job > 0) {
        gdc_set_max_job_num(g_max_gdc_job);
    }
    if (g_max_gdc_task > 0) {
        gdc_set_max_task_num(g_max_gdc_task);
    }
    if (g_max_gdc_node > 0) {
        gdc_set_max_node_num(g_max_gdc_node);
    }

    gdc_module_init();

    return 0;
}

static int ot_gdc_remove(struct platform_device *pdev)
{
    td_u32 i;

    gdc_module_exit();

    ot_unused(pdev);
    for (i = 0; i < OT_GDC_IP_NUM; i++) {
        gdc_set_init_gdc_reg(TD_NULL, i);
    }

    return 0;
}


static const struct of_device_id g_ot_gdc_match[] = {
    { .compatible = "vendor,gdc" },
    {},
};
MODULE_DEVICE_TABLE(of, g_ot_gdc_match);

static struct platform_driver g_ot_gdc_driver = {
    .probe          = ot_gdc_probe,
    .remove         = ot_gdc_remove,
    .driver         = {
        .name   = "ot_gdc",
        .of_match_table = g_ot_gdc_match,
    },
};

osal_module_platform_driver(g_ot_gdc_driver);

MODULE_LICENSE("Proprietary");
