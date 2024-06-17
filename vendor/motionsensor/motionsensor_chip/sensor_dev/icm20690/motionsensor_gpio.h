/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef MOTIONSENSOR_GPIO_H
#define MOTIONSENSOR_GPIO_H

#include "ot_type.h"
#ifndef __LITEOS__
#include<linux/gpio.h>
#else
#include "gpio.h"
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#define gpio_num(gpio_chip_num, gpio_offset_num)  ((gpio_chip_num) * 8 + (gpio_offset_num))

#ifndef __LITEOS__
td_s32 gpio_init(td_void);
td_s32 gpio_deinit(td_void);
#else
td_s32 gpio_init(struct gpio_descriptor *gd);
td_s32 gpio_deinit(struct gpio_descriptor *gd);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
