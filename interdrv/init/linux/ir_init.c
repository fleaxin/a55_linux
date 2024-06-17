/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_ir.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <linux/of_platform.h>

#define OSDRV_MODULE_VERSION_STRING "OT_ir @OT_MPP"

static int ot_ir_probe(struct platform_device *pdev)
{
    struct resource *mem = NULL;
    uintptr_t *ot_ir_irq = NULL;
    volatile void **ot_ir_reg_base = NULL;

    ot_ir_irq = get_ir_irq();
    *ot_ir_irq = (uintptr_t)platform_get_irq(pdev, 0);
    if (*ot_ir_irq <= 0) {
        dev_err(&pdev->dev, "cannot find ir IRQ%lu. \n", *ot_ir_irq);
    }

    ot_ir_reg_base = get_ir_reg_base();
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    *ot_ir_reg_base = (volatile void *)devm_ioremap_resource(&pdev->dev, mem);
    if (IS_ERR((void *)*ot_ir_reg_base)) {
        dev_err(&pdev->dev, "ir reg map failed. \n");
    }

    return ot_ir_init();
}

static int ot_ir_remove(struct platform_device *pdev)
{
    ot_ir_unused(pdev);
    ot_ir_exit();
    return 0;
}

static const struct of_device_id g_ot_ir_match[] = {
    { .compatible = "vendor,ir" },
    {},
};

static struct platform_driver g_ot_ir_driver = {
    .probe  = ot_ir_probe,
    .remove = ot_ir_remove,
    .driver =  {
        .name = "ot_ir",
        .of_match_table = g_ot_ir_match,
    },
};

module_platform_driver(g_ot_ir_driver);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Infrared remoter(OT_IR11) Device Driver");
MODULE_VERSION("OT_VERSION=" OSDRV_MODULE_VERSION_STRING);
