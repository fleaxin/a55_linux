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
#include "ot_common_dsp.h"
#include "ot_dsp_mod_init.h"

#define OT_SVP_DSP_DEV_NAME_LENGTH 8
#define OT_SVP_DSP_DEFULT_NODE_NUM 30
static td_u32 *g_dsp_reg[OT_SVP_DSP_ID_BUTT] = { TD_NULL, TD_NULL };
static td_u16 g_svp_dsp_max_node_num = OT_SVP_DSP_DEFULT_NODE_NUM;
static td_u16 g_svp_dsp_init_mode = 0;

module_param_named(dsp_max_node_num, g_svp_dsp_max_node_num, ushort, S_IRUGO);
module_param_named(dsp_init_mode, g_svp_dsp_init_mode, ushort, S_IRUGO);

static int ot_svp_dsp_probe(struct platform_device *dev)
{
    unsigned int i;
    char dsp_dev_name[OT_SVP_DSP_DEV_NAME_LENGTH] = {'\0'};
    struct resource *mem = TD_NULL;
    ot_svp_dsp_mod_param dsp_mod_param;
    td_void *reg = TD_NULL;
    td_s32 ret;

    for (i = 0; i < OT_SVP_DSP_ID_BUTT; i++) {
        ret = snprintf_s(dsp_dev_name, OT_SVP_DSP_DEV_NAME_LENGTH, OT_SVP_DSP_DEV_NAME_LENGTH - 1, "dsp%u", i);
        if ((ret < 0) || (ret > (OT_SVP_DSP_DEV_NAME_LENGTH - 1))) {
            printk("snprintf_s dsp%u's name failed\n", i);
            return TD_FAILURE;
        }
        mem = osal_platform_get_resource_byname(dev, IORESOURCE_MEM, dsp_dev_name);
        reg = devm_ioremap_resource(&dev->dev, mem);
        if (IS_ERR(reg)) {
            printk("Line:%d,Func:%s,CoreId(%u)\n", __LINE__, __FUNCTION__, i);
            return PTR_ERR((const void *)g_dsp_reg[i]);
        }
        g_dsp_reg[i] = (td_u32 *)reg;
        /* do not must get irq */
    }
    if (svp_dsp_set_init_reg(g_dsp_reg, OT_SVP_DSP_ID_BUTT) != TD_SUCCESS) {
        return TD_FAILURE;
    }
    dsp_mod_param.dsp_max_node_num = g_svp_dsp_max_node_num;
    dsp_mod_param.dsp_init_mode = g_svp_dsp_init_mode;
    svp_dsp_set_mod_param(&dsp_mod_param);
    return svp_dsp_std_mod_init();
}

static int ot_svp_dsp_remove(struct platform_device *dev)
{
    unsigned int i;
    svp_dsp_std_mod_exit();

    for (i = 0; i < OT_SVP_DSP_ID_BUTT; i++) {
        g_dsp_reg[i] = TD_NULL;
    }
    return 0;
}

static const struct of_device_id g_ot_svp_dsp_match[] = {
    { .compatible = "vendor,dsp" },
    {},
};
MODULE_DEVICE_TABLE(of, g_ot_svp_dsp_match);

static struct platform_driver g_ot_svp_dsp_driver = {
    .probe = ot_svp_dsp_probe,
    .remove = ot_svp_dsp_remove,
    .driver = {
        .name = "ot_dsp",
        .of_match_table = g_ot_svp_dsp_match,
    },
};

osal_module_platform_driver(g_ot_svp_dsp_driver);

MODULE_LICENSE("Proprietary");

