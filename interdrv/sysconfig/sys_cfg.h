/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SYS_CFG_H
#define SYS_CFG_H

#include <linux/printk.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define sys_config_print(format, args...) \
    printk("%s,%d: "format, __FUNCTION__, __LINE__, ##args) \

#define sys_writel(addr, value) ((*((volatile unsigned int *)(addr))) = (value))
#define sys_read(addr) (*((volatile int *)(addr)))
void write_reg32(unsigned long addr, unsigned long value, unsigned long mask);

void *sys_config_get_reg_crg(void);
void *sys_config_get_reg_sys(void);
void *sys_config_get_reg_ddr(void);
void *sys_config_get_reg_misc(void);
void *sys_config_get_reg_iocfg(void);
void *sys_config_get_reg_iocfg2(void);
void *sys_config_get_reg_gpio(void);
void *sys_config_get_reg_mipi_tx(void);

#define CHIP_SS928V100 0x0
#define VO_INTF_NAME_MIPI_TX "mipi_tx"

int sys_config_get_main_board_type(void);
int sys_config_get_chip_type(void);
int sys_config_get_vi_vpss_mode(void);
int sys_config_get_vo_intf_type(void);
int sys_config_get_vi_intf_type(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* SYS_CFG_H */
