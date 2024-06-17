/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PIRIS_HAL_H
#define PIRIS_HAL_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* base address */
#define PIRISI_ADRESS_BASE 0x11096000

/* PIRIS_DRV_Write REG value */
/* gpio10_3 BIN2 */
#define PIRIS_B_CASE0_REG0   0x0

#define PIRIS_B_CASE1_REG0   0x0

#define PIRIS_B_CASE2_REG0   0x8

#define PIRIS_B_CASE3_REG0   0x8

/* PIRIS_DRV_Write REG value */
/* gpio6_3(AIN1) gpio6_0(AIN2) gpio6_1(BIN1) */
#define PIRIS_A_CASE0_REG0   0xa

#define PIRIS_A_CASE1_REG0   0x3

#define PIRIS_A_CASE2_REG0   0x1

#define PIRIS_A_CASE3_REG0   0x8

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* PIRIS_HAL_H */

