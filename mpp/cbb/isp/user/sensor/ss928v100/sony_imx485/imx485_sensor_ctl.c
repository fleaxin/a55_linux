/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef OT_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ot_i2c.h"
#endif
#include "securec.h"

#include "imx485_cmos.h"

#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8

static int g_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};

int imx485_i2c_init(ot_vi_pipe vi_pipe)
{
    int ret;
    char dev_file[I2C_DEV_FILE_NUM] = {0};
    td_u8 dev_num;

    if (g_fd[vi_pipe] >= 0) {
        return TD_SUCCESS;
    }
#ifdef OT_GPIO_I2C
    g_fd[vi_pipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open gpioi2c_ex error!\n");
        return TD_FAILURE;
    }
#else
    ot_isp_sns_commbus *imx485businfo = TD_NULL;
    imx485businfo = imx485_get_bus_info(vi_pipe);
    dev_num = imx485businfo->i2c_dev;
    (td_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return TD_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_I2C_SLAVE_FORCE, (IMX485_I2C_ADDR >> 1));
    if (ret < 0) {
        isp_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif

    return TD_SUCCESS;
}

int imx485_i2c_exit(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 imx485_read_register(ot_vi_pipe vi_pipe, td_u32 addr)
{
    ot_unused(vi_pipe);
    ot_unused(addr);
    return TD_SUCCESS;
}

td_s32 imx485_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return TD_SUCCESS;
    }

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = IMX485_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = IMX485_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = IMX485_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        isp_err_trace("GPIO-I2C write failed!\n");
        return ret;
    }
#else
    td_u32 idx = 0;
    td_s32 ret;
    td_u8 buf[I2C_BUF_NUM];

    if (IMX485_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx] = (addr >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    } else {
    }

    if (IMX485_DATA_BYTE == 2) {  /* 2 byte */
    } else {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[vi_pipe], buf, IMX485_ADDR_BYTE + IMX485_DATA_BYTE);
    if (ret < 0) {
        isp_err_trace("I2C_WRITE error!\n");
        return TD_FAILURE;
    }

#endif
    return TD_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000); /* 1ms: 1000us */
    return;
}

void imx485_prog(ot_vi_pipe vi_pipe, const td_u32 *rom)
{
    ot_unused(vi_pipe);
    ot_unused(rom);
    return;
}

void imx485_standby(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x3000, 0x01);  /* STANDBY */
    ret += imx485_write_register(vi_pipe, 0x3002, 0x01);  /* XTMSTA */
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

void imx485_restart(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x3000, 0x00);  /* standby */
    delay_ms(24); /* 24ms */
    ret += imx485_write_register(vi_pipe, 0x3002, 0x00);  /* master mode start */
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

td_void imx485_blc_clamp(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp blc_clamp)
{
    td_s32 ret = TD_SUCCESS;

    imx485_set_blc_clamp_value(vi_pipe, blc_clamp.blc_clamp_en);

    if (blc_clamp.blc_clamp_en == TD_TRUE) {
        ret += imx485_write_register(vi_pipe, 0x3258, 0x01);  /* clamp on */
    } else {
        ret += imx485_write_register(vi_pipe, 0x3258, 0x00);  /* clamp off */
    }

    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}


static void imx485_wdr_2160p30_3to1_init(ot_vi_pipe vi_pipe);
static void imx485_linear_2160p30_init(ot_vi_pipe vi_pipe);

static void imx485_default_reg_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    ot_isp_sns_state *pastimx485 = TD_NULL;
    pastimx485 = imx485_get_ctx(vi_pipe);
    for (i = 0; i < pastimx485->regs_info[0].reg_num; i++) {
        ret += imx485_write_register(vi_pipe,
                                     pastimx485->regs_info[0].i2c_data[i].reg_addr,
                                     pastimx485->regs_info[0].i2c_data[i].data);
    }
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

void imx485_init(ot_vi_pipe vi_pipe)
{
    ot_wdr_mode wdr_mode;
    td_bool          init;
    td_u8            img_mode;
    td_s32 ret;
    ot_isp_sns_state *pastimx485 = TD_NULL;
    pastimx485 = imx485_get_ctx(vi_pipe);
    init       = pastimx485->init;
    wdr_mode   = pastimx485->wdr_mode;
    img_mode   = pastimx485->img_mode;

    ret = imx485_i2c_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("i2c init failed!\n");
        return;
    }
    /* When sensor first init, config all registers */
    if (init == TD_FALSE) {
        if (OT_WDR_MODE_3To1_LINE == wdr_mode) {
            if (IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE == img_mode) {  /* IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE */
                imx485_wdr_2160p30_3to1_init(vi_pipe);
            } else {
            }
        } else {
            imx485_linear_2160p30_init(vi_pipe);
        }
    } else {
        /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
        if (OT_WDR_MODE_3To1_LINE == wdr_mode) {
            if (IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE == img_mode) {  /* IMX485_SENSOR_2160P_30FPS_WDR_3TO1_MODE */
                imx485_wdr_2160p30_3to1_init(vi_pipe);
            } else {
            }
        } else {
            imx485_linear_2160p30_init(vi_pipe);
        }
    }

    pastimx485->init = TD_TRUE;
    return;
}

void imx485_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    ret = imx485_i2c_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("imx485 exit failed!\n");
    }
    return;
}

static td_s32 imx485_linear_2160p30_init_part1(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x3008, 0x7F);
    ret += imx485_write_register(vi_pipe, 0x300A, 0x5B);
    ret += imx485_write_register(vi_pipe, 0x300B, 0x50);
    ret += imx485_write_register(vi_pipe, 0x3028, 0x4C);
    ret += imx485_write_register(vi_pipe, 0x3029, 0x04);
    ret += imx485_write_register(vi_pipe, 0x3031, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3032, 0x01);
    ret += imx485_write_register(vi_pipe, 0x30A5, 0x00);
    ret += imx485_write_register(vi_pipe, 0x30dc, 0x32);
    ret += imx485_write_register(vi_pipe, 0x3114, 0x02);
    ret += imx485_write_register(vi_pipe, 0x311C, 0x9B);
    ret += imx485_write_register(vi_pipe, 0x3260, 0x22);
    ret += imx485_write_register(vi_pipe, 0x3262, 0x02);
    ret += imx485_write_register(vi_pipe, 0x3278, 0xA2);
    ret += imx485_write_register(vi_pipe, 0x3324, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3366, 0x31);
    ret += imx485_write_register(vi_pipe, 0x340C, 0x4D);
    ret += imx485_write_register(vi_pipe, 0x3416, 0x10);
    ret += imx485_write_register(vi_pipe, 0x3417, 0x13);
    ret += imx485_write_register(vi_pipe, 0x3432, 0x93);
    ret += imx485_write_register(vi_pipe, 0x34CE, 0x1E);
    ret += imx485_write_register(vi_pipe, 0x34CF, 0x1E);
    ret += imx485_write_register(vi_pipe, 0x34DC, 0x80);
    ret += imx485_write_register(vi_pipe, 0x351C, 0x03);
    ret += imx485_write_register(vi_pipe, 0x359E, 0x70);
    ret += imx485_write_register(vi_pipe, 0x35A2, 0x9C);
    ret += imx485_write_register(vi_pipe, 0x35AC, 0x08);
    ret += imx485_write_register(vi_pipe, 0x35C0, 0xFA);
    ret += imx485_write_register(vi_pipe, 0x35C2, 0x4E);
    ret += imx485_write_register(vi_pipe, 0x3608, 0x41);
    ret += imx485_write_register(vi_pipe, 0x360A, 0x47);
    ret += imx485_write_register(vi_pipe, 0x361E, 0x4A);
    ret += imx485_write_register(vi_pipe, 0x3630, 0x43);
    ret += imx485_write_register(vi_pipe, 0x3632, 0x47);
    ret += imx485_write_register(vi_pipe, 0x363C, 0x41);
    ret += imx485_write_register(vi_pipe, 0x363E, 0x4A);
    ret += imx485_write_register(vi_pipe, 0x3648, 0x41);
    ret += imx485_write_register(vi_pipe, 0x364A, 0x47);
    ret += imx485_write_register(vi_pipe, 0x3660, 0x04);
    ret += imx485_write_register(vi_pipe, 0x3676, 0x3F);
    ret += imx485_write_register(vi_pipe, 0x367A, 0x3F);
    ret += imx485_write_register(vi_pipe, 0x36A4, 0x41);
    ret += imx485_write_register(vi_pipe, 0x3798, 0x82);
    return TD_SUCCESS;
}

static td_s32 imx485_linear_2160p30_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x379A, 0x82);
    ret += imx485_write_register(vi_pipe, 0x379C, 0x82);
    ret += imx485_write_register(vi_pipe, 0x379E, 0x82);
    ret += imx485_write_register(vi_pipe, 0x3804, 0x22);
    ret += imx485_write_register(vi_pipe, 0x3888, 0xA8);
    ret += imx485_write_register(vi_pipe, 0x388C, 0xA6);
    ret += imx485_write_register(vi_pipe, 0x3914, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3915, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3916, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3917, 0x14);
    ret += imx485_write_register(vi_pipe, 0x3918, 0x14);
    ret += imx485_write_register(vi_pipe, 0x3919, 0x14);
    ret += imx485_write_register(vi_pipe, 0x391A, 0x13);
    ret += imx485_write_register(vi_pipe, 0x391B, 0x13);
    ret += imx485_write_register(vi_pipe, 0x391C, 0x13);
    ret += imx485_write_register(vi_pipe, 0x391E, 0x00);
    ret += imx485_write_register(vi_pipe, 0x391F, 0xA5);
    ret += imx485_write_register(vi_pipe, 0x3920, 0xED);
    ret += imx485_write_register(vi_pipe, 0x3921, 0x0E);
    ret += imx485_write_register(vi_pipe, 0x39A2, 0x0C);
    ret += imx485_write_register(vi_pipe, 0x39A4, 0x16);
    ret += imx485_write_register(vi_pipe, 0x39A6, 0x2B);
    ret += imx485_write_register(vi_pipe, 0x39A7, 0x01);
    ret += imx485_write_register(vi_pipe, 0x39D2, 0x2D);
    ret += imx485_write_register(vi_pipe, 0x39D3, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39D8, 0x37);
    ret += imx485_write_register(vi_pipe, 0x39D9, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39DA, 0x9B);
    ret += imx485_write_register(vi_pipe, 0x39DB, 0x01);
    ret += imx485_write_register(vi_pipe, 0x39E0, 0x28);
    ret += imx485_write_register(vi_pipe, 0x39E1, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39E2, 0x2C);
    ret += imx485_write_register(vi_pipe, 0x39E3, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39E8, 0x96);
    ret += imx485_write_register(vi_pipe, 0x39EA, 0x9A);
    ret += imx485_write_register(vi_pipe, 0x39EB, 0x01);
    ret += imx485_write_register(vi_pipe, 0x39F2, 0x27);
    ret += imx485_write_register(vi_pipe, 0x39F3, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3A00, 0x38);
    ret += imx485_write_register(vi_pipe, 0x3A01, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3A02, 0x95);
    ret += imx485_write_register(vi_pipe, 0x3A03, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3A18, 0x9B);
    ret += imx485_write_register(vi_pipe, 0x3A2A, 0x0C);
    ret += imx485_write_register(vi_pipe, 0x3A30, 0x15);
    return TD_SUCCESS;
}

static td_s32 imx485_linear_2160p30_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x3A32, 0x31);
    ret += imx485_write_register(vi_pipe, 0x3A33, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3A36, 0x4D);
    ret += imx485_write_register(vi_pipe, 0x3A3E, 0x11);
    ret += imx485_write_register(vi_pipe, 0x3A40, 0x31);
    ret += imx485_write_register(vi_pipe, 0x3A42, 0x4C);
    ret += imx485_write_register(vi_pipe, 0x3A43, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3A44, 0x47);
    ret += imx485_write_register(vi_pipe, 0x3A46, 0x4B);
    ret += imx485_write_register(vi_pipe, 0x3A4E, 0x11);
    ret += imx485_write_register(vi_pipe, 0x3A50, 0x32);
    ret += imx485_write_register(vi_pipe, 0x3A52, 0x46);
    ret += imx485_write_register(vi_pipe, 0x3A53, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3D04, 0x48);
    ret += imx485_write_register(vi_pipe, 0x3D05, 0x09);
    ret += imx485_write_register(vi_pipe, 0x3D0C, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3D18, 0x9F);
    ret += imx485_write_register(vi_pipe, 0x3D1A, 0x57);
    ret += imx485_write_register(vi_pipe, 0x3D1C, 0x57);
    ret += imx485_write_register(vi_pipe, 0x3D1E, 0x87);
    ret += imx485_write_register(vi_pipe, 0x3D20, 0x5F);
    ret += imx485_write_register(vi_pipe, 0x3D22, 0xA7);
    ret += imx485_write_register(vi_pipe, 0x3D24, 0x5F);
    ret += imx485_write_register(vi_pipe, 0x3D26, 0x9F);
    ret += imx485_write_register(vi_pipe, 0x3D28, 0x4F);
    return TD_SUCCESS;
}


/* 4K@30fps */
static void imx485_linear_2160p30_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += imx485_linear_2160p30_init_part1(vi_pipe);
    ret += imx485_linear_2160p30_init_part2(vi_pipe);
    ret += imx485_linear_2160p30_init_part3(vi_pipe);

    imx485_default_reg_init(vi_pipe);

    ret += imx485_write_register(vi_pipe, 0x3000, 0x00);  /* standby */
    delay_ms(20); /* 20ms */
    ret += imx485_write_register(vi_pipe, 0x3002, 0x00);  /* master mode start */
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
        return;
    }
    printf("===IMX485 2160P 30fps 12bit LINE Init OK!===\n");
    return;
}

static td_s32 imx485_wdr_2160p30_3to1_init_part0(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x3000, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3002, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3008, 0x7F);
    ret += imx485_write_register(vi_pipe, 0x300A, 0x5B);
    ret += imx485_write_register(vi_pipe, 0x300B, 0x50);

    ret += imx485_write_register(vi_pipe, 0x3024, 0xCA); // VMAX
    ret += imx485_write_register(vi_pipe, 0x3025, 0x08);
    ret += imx485_write_register(vi_pipe, 0x3026, 0x00);

    ret += imx485_write_register(vi_pipe, 0x3028, 0x6E);
    ret += imx485_write_register(vi_pipe, 0x3029, 0x01);
    ret += imx485_write_register(vi_pipe, 0x302C, 0x01);
    ret += imx485_write_register(vi_pipe, 0x302D, 0x02);
    ret += imx485_write_register(vi_pipe, 0x3031, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3032, 0x00);

    ret += imx485_write_register(vi_pipe, 0x3050, 0xB2); // SHR0
    ret += imx485_write_register(vi_pipe, 0x3051, 0x11);
    ret += imx485_write_register(vi_pipe, 0x3052, 0x00);

    ret += imx485_write_register(vi_pipe, 0x3054, 0x14); // SHR1
    ret += imx485_write_register(vi_pipe, 0x3055, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3056, 0x00);

    ret += imx485_write_register(vi_pipe, 0x3058, 0xD4); // SHR2
    ret += imx485_write_register(vi_pipe, 0x3059, 0x08);
    ret += imx485_write_register(vi_pipe, 0x305a, 0x00);

    ret += imx485_write_register(vi_pipe, 0x3060, 0xC6); // RHS1
    ret += imx485_write_register(vi_pipe, 0x3061, 0x08);
    ret += imx485_write_register(vi_pipe, 0x3062, 0x00);

    ret += imx485_write_register(vi_pipe, 0x3064, 0x86); // RHS2
    ret += imx485_write_register(vi_pipe, 0x3065, 0x11);
    ret += imx485_write_register(vi_pipe, 0x3066, 0x00);

    return TD_SUCCESS;
}

static td_s32 imx485_wdr_2160p30_3to1_init_part1(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += imx485_write_register(vi_pipe, 0x30A5, 0x00);
    ret += imx485_write_register(vi_pipe, 0x30CF, 0x02);
    ret += imx485_write_register(vi_pipe, 0x3114, 0x02);
    ret += imx485_write_register(vi_pipe, 0x311C, 0x80);
    ret += imx485_write_register(vi_pipe, 0x3200, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3260, 0x22);
    ret += imx485_write_register(vi_pipe, 0x3262, 0x02);
    ret += imx485_write_register(vi_pipe, 0x3278, 0xA2);
    ret += imx485_write_register(vi_pipe, 0x3324, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3366, 0x31);
    ret += imx485_write_register(vi_pipe, 0x340C, 0x4D);
    ret += imx485_write_register(vi_pipe, 0x3416, 0x10);
    ret += imx485_write_register(vi_pipe, 0x3417, 0x13);
    ret += imx485_write_register(vi_pipe, 0x3432, 0x93);
    ret += imx485_write_register(vi_pipe, 0x34CE, 0x1E);
    ret += imx485_write_register(vi_pipe, 0x34CF, 0x1E);
    ret += imx485_write_register(vi_pipe, 0x34DC, 0x80);
    ret += imx485_write_register(vi_pipe, 0x351C, 0x03);
    ret += imx485_write_register(vi_pipe, 0x359E, 0x70);
    ret += imx485_write_register(vi_pipe, 0x35A2, 0x9C);
    ret += imx485_write_register(vi_pipe, 0x35AC, 0x08);
    ret += imx485_write_register(vi_pipe, 0x35C0, 0xFA);
    ret += imx485_write_register(vi_pipe, 0x35C2, 0x4E);
    return TD_SUCCESS;
}

static td_s32 imx485_wdr_2160p30_3to1_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x3608, 0x41);
    ret += imx485_write_register(vi_pipe, 0x360A, 0x47);
    ret += imx485_write_register(vi_pipe, 0x361E, 0x4A);
    ret += imx485_write_register(vi_pipe, 0x3630, 0x43);
    ret += imx485_write_register(vi_pipe, 0x3632, 0x47);
    ret += imx485_write_register(vi_pipe, 0x363C, 0x41);
    ret += imx485_write_register(vi_pipe, 0x363E, 0x4A);
    ret += imx485_write_register(vi_pipe, 0x3648, 0x41);
    ret += imx485_write_register(vi_pipe, 0x364A, 0x47);
    ret += imx485_write_register(vi_pipe, 0x3660, 0x04);
    ret += imx485_write_register(vi_pipe, 0x3676, 0x3F);
    ret += imx485_write_register(vi_pipe, 0x367A, 0x3F);
    ret += imx485_write_register(vi_pipe, 0x36A4, 0x41);
    ret += imx485_write_register(vi_pipe, 0x3798, 0x82);
    ret += imx485_write_register(vi_pipe, 0x379A, 0x82);
    ret += imx485_write_register(vi_pipe, 0x379C, 0x82);
    ret += imx485_write_register(vi_pipe, 0x379E, 0x82);
    ret += imx485_write_register(vi_pipe, 0x3804, 0x22);
    ret += imx485_write_register(vi_pipe, 0x3888, 0xA8);
    ret += imx485_write_register(vi_pipe, 0x388C, 0xA6);
    ret += imx485_write_register(vi_pipe, 0x3914, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3915, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3916, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3917, 0x14);
    ret += imx485_write_register(vi_pipe, 0x3918, 0x14);
    ret += imx485_write_register(vi_pipe, 0x3919, 0x14);
    ret += imx485_write_register(vi_pipe, 0x391A, 0x13);
    ret += imx485_write_register(vi_pipe, 0x391B, 0x13);
    ret += imx485_write_register(vi_pipe, 0x391C, 0x13);
    ret += imx485_write_register(vi_pipe, 0x391E, 0x00);
    ret += imx485_write_register(vi_pipe, 0x391F, 0xA5);
    ret += imx485_write_register(vi_pipe, 0x3920, 0xED);
    ret += imx485_write_register(vi_pipe, 0x3921, 0x0E);
    ret += imx485_write_register(vi_pipe, 0x39A2, 0x0C);
    ret += imx485_write_register(vi_pipe, 0x39A4, 0x16);
    ret += imx485_write_register(vi_pipe, 0x39A6, 0x2B);
    ret += imx485_write_register(vi_pipe, 0x39A7, 0x01);
    ret += imx485_write_register(vi_pipe, 0x39D2, 0x2D);
    ret += imx485_write_register(vi_pipe, 0x39D3, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39D8, 0x37);
    ret += imx485_write_register(vi_pipe, 0x39D9, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39DA, 0x9B);
    ret += imx485_write_register(vi_pipe, 0x39DB, 0x01);
    ret += imx485_write_register(vi_pipe, 0x39E0, 0x28);
    ret += imx485_write_register(vi_pipe, 0x39E1, 0x00);
    ret += imx485_write_register(vi_pipe, 0x39E2, 0x2C);
    ret += imx485_write_register(vi_pipe, 0x39E3, 0x00);
    return TD_SUCCESS;
}

static td_s32 imx485_wdr_2160p30_3to1_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += imx485_write_register(vi_pipe, 0x39E8, 0x96);
    ret += imx485_write_register(vi_pipe, 0x39EA, 0x9A);
    ret += imx485_write_register(vi_pipe, 0x39EB, 0x01);
    ret += imx485_write_register(vi_pipe, 0x39F2, 0x27);
    ret += imx485_write_register(vi_pipe, 0x39F3, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3A00, 0x38);
    ret += imx485_write_register(vi_pipe, 0x3A01, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3A02, 0x95);
    ret += imx485_write_register(vi_pipe, 0x3A03, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3A18, 0x9B);
    ret += imx485_write_register(vi_pipe, 0x3A2A, 0x0C);
    ret += imx485_write_register(vi_pipe, 0x3A30, 0x15);
    ret += imx485_write_register(vi_pipe, 0x3A32, 0x31);
    ret += imx485_write_register(vi_pipe, 0x3A33, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3A36, 0x4D);
    ret += imx485_write_register(vi_pipe, 0x3A3E, 0x11);
    ret += imx485_write_register(vi_pipe, 0x3A40, 0x31);
    ret += imx485_write_register(vi_pipe, 0x3A42, 0x4C);
    ret += imx485_write_register(vi_pipe, 0x3A43, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3A44, 0x47);
    ret += imx485_write_register(vi_pipe, 0x3A46, 0x4B);
    ret += imx485_write_register(vi_pipe, 0x3A4E, 0x11);
    ret += imx485_write_register(vi_pipe, 0x3A50, 0x32);
    ret += imx485_write_register(vi_pipe, 0x3A52, 0x46);
    ret += imx485_write_register(vi_pipe, 0x3A53, 0x01);
    ret += imx485_write_register(vi_pipe, 0x3D01, 0x07);
    ret += imx485_write_register(vi_pipe, 0x3D04, 0x48);
    ret += imx485_write_register(vi_pipe, 0x3D05, 0x09);
    ret += imx485_write_register(vi_pipe, 0x3D0C, 0x00);
    ret += imx485_write_register(vi_pipe, 0x3D18, 0x8F);
    ret += imx485_write_register(vi_pipe, 0x3D1A, 0x4F);
    ret += imx485_write_register(vi_pipe, 0x3D1C, 0x47);
    ret += imx485_write_register(vi_pipe, 0x3D1E, 0x37);
    ret += imx485_write_register(vi_pipe, 0x3D20, 0x4F);
    ret += imx485_write_register(vi_pipe, 0x3D22, 0x87);
    ret += imx485_write_register(vi_pipe, 0x3D24, 0x4F);
    ret += imx485_write_register(vi_pipe, 0x3D26, 0x7F);
    ret += imx485_write_register(vi_pipe, 0x3D28, 0x3F);
    return TD_SUCCESS;
}

static void imx485_wdr_2160p30_3to1_init(ot_vi_pipe vi_pipe)
{
    /* 10bit Register */
    td_s32 ret = TD_SUCCESS;

    imx485_default_reg_init(vi_pipe);
    ret += imx485_wdr_2160p30_3to1_init_part0(vi_pipe);
    ret += imx485_wdr_2160p30_3to1_init_part1(vi_pipe);
    ret += imx485_wdr_2160p30_3to1_init_part2(vi_pipe);
    ret += imx485_wdr_2160p30_3to1_init_part3(vi_pipe);

    ret += imx485_write_register(vi_pipe, 0x3000, 0x00);  /* standby */
    delay_ms(24); /* 24ms */
    ret += imx485_write_register(vi_pipe, 0x3002, 0x00);  /* master mode start */
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
        return;
    }
    printf("===Imx485 sensor 2160P30fps 10bit 3to1 WDR(90fps->30fps) init success!=====\n");
    return;
}
