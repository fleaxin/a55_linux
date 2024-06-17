/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_avs_mod_init.h"
#include <linux/module.h>
#include <linux/of_platform.h>
#include "ot_common.h"

static int ot_avs_probe(struct platform_device *pdev)
{
    struct resource *mem = TD_NULL;
    int avs_irq;
    void *avs_reg = TD_NULL;

    if (pdev == TD_NULL) {
        osal_printk("%s, line: %d, dev is null!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    mem = osal_platform_get_resource_byname(pdev, IORESOURCE_MEM, "avs");

    avs_reg = devm_ioremap_resource(&pdev->dev, mem);
    if (IS_ERR(avs_reg)) {
        return PTR_ERR(avs_reg);
    }

    avs_set_init_avs_reg(avs_reg);

    avs_irq = osal_platform_get_irq_byname(pdev, "avs");
    if (avs_irq <= 0) {
        dev_err(&pdev->dev, "cannot find avs IRQ\n");
        return -1;
    }

    avs_set_init_avs_irq((unsigned int)avs_irq);

    if (avs_mod_init()) {
        return -1;
    }

    return 0;
}

static int ot_avs_remove(struct platform_device *pdev)
{
    ot_unused(pdev);
    avs_mod_exit();
    avs_set_init_avs_reg(TD_NULL);
    return 0;
}

static const struct of_device_id ot_avs_match[] = {
    { .compatible = "vendor,avs" },
    {},
};

MODULE_DEVICE_TABLE(of, ot_avs_match);

static struct platform_driver ot_avs_driver = {
    .probe = ot_avs_probe,
    .remove = ot_avs_remove,
    .driver = {
        .name = "ot_avs",
        .of_match_table = ot_avs_match,
    },
};

osal_module_platform_driver(ot_avs_driver);

MODULE_LICENSE("Proprietary");
