/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <linux/of_platform.h>

#include "ot_common_video.h"
#include "ot_common.h"
#include "ot_osal.h"
#include "ot_dpu_match_mod_init.h"

#define OT_DPU_MATCH_NAME_LENGTH 16

static int ot_dpu_match_probe(struct platform_device *dev)
{
    td_char dev_name[OT_DPU_MATCH_NAME_LENGTH] = "match";
    int irq;

    irq = osal_platform_get_irq_byname(dev, dev_name);
    if (irq <= 0) {
        dev_err(&dev->dev, "cannot find match IRQ\n");
    }

    if (dpu_match_set_init_irq(irq) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    return dpu_match_init();
}

static int ot_dpu_match_remove(struct platform_device *dev)
{
    ot_unused(dev);
    dpu_match_exit();
    return 0;
}

static const struct of_device_id g_ot_dpu_match_match[] = {
    { .compatible = "vendor,dpu_match" },
    {},
};
MODULE_DEVICE_TABLE(of, g_ot_dpu_match_match);

static struct platform_driver g_ot_dpu_match_driver = {
    .probe = ot_dpu_match_probe,
    .remove = ot_dpu_match_remove,
    .driver = {
        .name = "ot_dpu_match",
        .of_match_table = g_ot_dpu_match_match,
    },
};

osal_module_platform_driver(g_ot_dpu_match_driver);

MODULE_LICENSE("Proprietary");

