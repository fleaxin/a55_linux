/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/module.h>
#include <linux/of_platform.h>
#include "securec.h"
#include "ot_osal.h"
#include "ot_npudev_mod_init.h"

static int ot_npudev_probe(struct platform_device *pdev)
{
    struct resource *mem = NULL;
    void *ioremap_addr = NULL;
    unsigned int reg_num;
    unsigned int irq_num;
    int irq_no;
    unsigned int i;

    if (pdev == NULL) {
        osal_printk("%s, %d, dev is NULL!\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    reg_num = npudev_get_reg_num();
    for (i = 0; i < reg_num; i++) {
        mem = osal_platform_get_resource(pdev, IORESOURCE_MEM, i);
        if (mem == NULL) {
            osal_printk("%s, %d, platform_get_resource fail. i = %d.\n", __FUNCTION__, __LINE__, i);
            return TD_FAILURE;
        }

        ioremap_addr = osal_ioremap(mem->start, mem->end - mem->start);
        if (IS_ERR(ioremap_addr)) {
            return PTR_ERR(ioremap_addr);
        }
        npudev_set_npu_reg_addr(i, ioremap_addr);
    }

    irq_num = npudev_get_irq_num();
    for (i = 0; i < irq_num; i++) {
        irq_no = osal_platform_get_irq(pdev, i);
        if (irq_no < 0) {
            osal_printk("%s, %d, get irq fail. i = %d.\n", __FUNCTION__, __LINE__, i);
            return TD_FAILURE;
        }
        npudev_set_npu_irq_no(i, irq_no);
    }

    if (npudev_module_init() != TD_SUCCESS) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static int ot_npudev_remove(struct platform_device *pdev)
{
    unsigned int reg_num;
    unsigned int i;
    void *ioremap_addr = NULL;

    ot_unused(pdev);

    npudev_module_exit();

    reg_num = npudev_get_reg_num();
    for (i = 0; i < reg_num; i++) {
        ioremap_addr = npudev_get_npu_reg_addr(i);
        if (ioremap_addr != NULL) {
            osal_iounmap(ioremap_addr);
        }
    }

    return TD_SUCCESS;
}

static const struct of_device_id g_ot_npudev_match[] = {
    { .compatible = "vendor,npu" },
    {},
};

MODULE_DEVICE_TABLE(of, g_ot_npudev_match);

static struct platform_driver g_ot_npudev_driver = {
    .probe = ot_npudev_probe,
    .remove = ot_npudev_remove,
    .driver = {
        .name = "ot_npudev",
        .of_match_table = g_ot_npudev_match,
    },
};

osal_module_platform_driver(g_ot_npudev_driver);

MODULE_LICENSE("Proprietary");

