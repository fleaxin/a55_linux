/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "motionsensor_gpio.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "icm20690.h"

#ifndef __LITEOS__
td_s32 gpio_init(td_void)
{
    td_s32 ret;

    ret = gpio_request(gpio_num(INT_GPIO_CHIP, INT_GPIO_OFFSET), "icm20690_irq");
    if (ret < 0) {
        print_info("irq GPIO request failed: %d", ret);
        return ret;
    }

    gpio_direction_input(gpio_num(INT_GPIO_CHIP, INT_GPIO_OFFSET));

    return 0;
}
td_s32 gpio_deinit(td_void)
{
    gpio_free(gpio_num(INT_GPIO_CHIP, INT_GPIO_OFFSET));
    return 0;
}
#else
td_s32 gpio_init(struct gpio_descriptor *gd)
{
    gd->group_num = INT_GPIO_CHIP;
    gd->bit_num = INT_GPIO_OFFSET;

    gpio_chip_init(gd);
    return TD_SUCCESS;
}

td_s32 gpio_deinit(struct gpio_descriptor *gd)
{
    return gpio_chip_deinit(gd);
}

#endif
