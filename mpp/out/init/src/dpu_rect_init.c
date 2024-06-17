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
#include "ot_dpu_rect_mod_init.h"

#define DPU_RECT_NAME_LENGTH 16

static int ot_dpu_rect_probe(struct platform_device *dev)
{
    td_char dev_name[DPU_RECT_NAME_LENGTH] = "rect";
    int irq;

    irq = osal_platform_get_irq_byname(dev, dev_name);
    if (irq <= 0) {
        dev_err(&dev->dev, "cannot find rect IRQ\n");
    }
    if (dpu_rect_set_init_irq(irq) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    return dpu_rect_init();
}

static int ot_dpu_rect_remove(struct platform_device *dev)
{
    ot_unused(dev);
    dpu_rect_exit();
    return 0;
}

static const struct of_device_id g_ot_dpu_rect_match[] = {
    { .compatible = "vendor,dpu_rect" },
    {},
};
MODULE_DEVICE_TABLE(of, g_ot_dpu_rect_match);

static struct platform_driver g_ot_dpu_rect_driver = {
    .probe = ot_dpu_rect_probe,
    .remove = ot_dpu_rect_remove,
    .driver = {
        .name = "ot_dpu_rect",
        .of_match_table = g_ot_dpu_rect_match,
    },
};

osal_module_platform_driver(g_ot_dpu_rect_driver);

MODULE_LICENSE("Proprietary");

