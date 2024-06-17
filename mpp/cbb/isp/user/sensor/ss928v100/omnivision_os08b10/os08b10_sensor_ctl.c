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

#include "os08b10_cmos.h"

#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8

static int g_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};

int os08b10_i2c_init(ot_vi_pipe vi_pipe)
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
    ot_isp_sns_commbus *os08b10businfo = TD_NULL;
    os08b10businfo = os08b10_get_bus_info(vi_pipe);
    dev_num = os08b10businfo->i2c_dev;
    (td_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return TD_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_I2C_SLAVE_FORCE, (OS08B10_I2C_ADDR >> 1));
    if (ret < 0) {
        isp_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif

    return TD_SUCCESS;
}

int os08b10_i2c_exit(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 os08b10_read_register(ot_vi_pipe vi_pipe, td_u32 addr)
{
    ot_unused(vi_pipe);
    ot_unused(addr);
    return TD_SUCCESS;
}

td_s32 os08b10_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return TD_SUCCESS;
    }

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = OS08B10_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = OS08B10_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = OS08B10_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        isp_err_trace("GPIO-I2C write faild!\n");
        return ret;
    }
#else
    td_u32 idx = 0;
    td_s32 ret;
    td_u8 buf[I2C_BUF_NUM];

    if (OS08B10_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx] = (addr >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    } else {
    }

    if (OS08B10_DATA_BYTE == 2) {  /* 2 byte */
    } else {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[vi_pipe], buf, OS08B10_ADDR_BYTE + OS08B10_DATA_BYTE);
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

void os08b10_prog(ot_vi_pipe vi_pipe, const td_u32 *rom)
{
    ot_unused(vi_pipe);
    ot_unused(rom);
    return;
}

void os08b10_standby(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

void os08b10_restart(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

td_void os08b10_blc_clamp(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp blc_clamp)
{
    td_s32 ret = TD_SUCCESS;
    ot_wdr_mode      wdr_mode;
    ot_isp_sns_state *pastos08b10 = TD_NULL;

    pastos08b10 = os08b10_get_ctx(vi_pipe);
    wdr_mode    = pastos08b10->wdr_mode;

    os08b10_set_blc_clamp_value(vi_pipe, blc_clamp.blc_clamp_en);

    if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
        if (blc_clamp.blc_clamp_en == TD_TRUE) {
            ret += os08b10_write_register(vi_pipe, 0x4001, 0xeb);  /* clamp on */
        } else {
            ret += os08b10_write_register(vi_pipe, 0x4001, 0xea);  /* clamp off */
        }
    } else if (OT_WDR_MODE_NONE == wdr_mode) {
        if (blc_clamp.blc_clamp_en == TD_TRUE) {
            ret += os08b10_write_register(vi_pipe, 0x4001, 0x2b);  /* clamp on */
        } else {
            ret += os08b10_write_register(vi_pipe, 0x4001, 0x2a);  /* clamp off */
        }
    } else {
        isp_err_trace("wdr_mode failed!\n");
    }

    if (ret != TD_SUCCESS) {
        isp_err_trace("write blc enable register failed!\n");
    }
    return;
}

static void os08b10_linear_8m30_12bit_init(ot_vi_pipe vi_pipe);
static void os08b10_vc_wdr_2t1_8m30_12bit_init(ot_vi_pipe vi_pipe);

static void os08b10_default_reg_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    ot_isp_sns_state *pastos08b10 = TD_NULL;

    pastos08b10 = os08b10_get_ctx(vi_pipe);
    for (i = 0; i < pastos08b10->regs_info[0].reg_num; i++) {
        ret += os08b10_write_register(vi_pipe,
            pastos08b10->regs_info[0].i2c_data[i].reg_addr,
            pastos08b10->regs_info[0].i2c_data[i].data);
    }

    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }

    return;
}

void os08b10_init(ot_vi_pipe vi_pipe)
{
    ot_wdr_mode      wdr_mode;
    td_bool          init;
    td_u8            img_mode;
    td_s32           ret;
    ot_isp_sns_state *pastos08b10 = TD_NULL;

    pastos08b10 = os08b10_get_ctx(vi_pipe);
    init        = pastos08b10->init;
    wdr_mode    = pastos08b10->wdr_mode;
    img_mode    = pastos08b10->img_mode;

    ret = os08b10_i2c_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("i2c init failed!\n");
        return;
    }

    /* When sensor first init, config all registers */
    if (init == TD_FALSE) {
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
            if (OS08B10_8M_30FPS_12BIT_2TO1_VC_MODE == img_mode) {  /* OS08B10_8M_30FPS_12BIT_2TO1_VC_MODE */
                os08b10_vc_wdr_2t1_8m30_12bit_init(vi_pipe);
            } else {
            }
        } else {
            os08b10_linear_8m30_12bit_init(vi_pipe);
        }
    } else {
        /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
            if (OS08B10_8M_30FPS_12BIT_2TO1_VC_MODE == img_mode) {  /* OS08B10_8M_30FPS_12BIT_2TO1_VC_MODE */
                os08b10_vc_wdr_2t1_8m30_12bit_init(vi_pipe);
            } else {
            }
        } else {
            os08b10_linear_8m30_12bit_init(vi_pipe);
        }
    }

    pastos08b10->init = TD_TRUE;
    return;
}

void os08b10_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    ret = os08b10_i2c_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("OS08B10 exit failed!\n");
    }

    return;
}

static td_s32 os08b10_linear_8m30_12bit_init_part1(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os08b10_write_register(vi_pipe, 0x0103, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0109, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0104, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x0102, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0303, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0305, 0x3b);
    ret += os08b10_write_register(vi_pipe, 0x0306, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x0307, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0308, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x0309, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x030a, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x030c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0317, 0x4a);
    ret += os08b10_write_register(vi_pipe, 0x0322, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0323, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x0324, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0325, 0x80);
    ret += os08b10_write_register(vi_pipe, 0x0327, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x0328, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x0329, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x032a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x032c, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x032d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x032e, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x032f, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3002, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x300f, 0x11);
    ret += os08b10_write_register(vi_pipe, 0x3012, 0x41);
    ret += os08b10_write_register(vi_pipe, 0x301e, 0xb8);
    ret += os08b10_write_register(vi_pipe, 0x3026, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3027, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x302c, 0x1a);
    ret += os08b10_write_register(vi_pipe, 0x3104, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3106, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3400, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3408, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x3409, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x340a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x340c, 0x0b);
    ret += os08b10_write_register(vi_pipe, 0x340d, 0x90);
    ret += os08b10_write_register(vi_pipe, 0x3420, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3421, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3422, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3423, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3424, 0x00);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os08b10_write_register(vi_pipe, 0x3425, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3426, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3427, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3428, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3429, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342a, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342f, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3501, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3502, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3504, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3541, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3542, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3581, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3582, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3603, 0x30);
    ret += os08b10_write_register(vi_pipe, 0x3612, 0x96);
    ret += os08b10_write_register(vi_pipe, 0x3613, 0xe0);
    ret += os08b10_write_register(vi_pipe, 0x3623, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3624, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3625, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x362a, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x362b, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x362c, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x362d, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x362e, 0x17);
    ret += os08b10_write_register(vi_pipe, 0x362f, 0x2d);
    ret += os08b10_write_register(vi_pipe, 0x3630, 0x67);
    ret += os08b10_write_register(vi_pipe, 0x3631, 0x7f);
    ret += os08b10_write_register(vi_pipe, 0x3638, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3639, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x363a, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3643, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3644, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3645, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3646, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3647, 0x06);
    ret += os08b10_write_register(vi_pipe, 0x3648, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3649, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x364a, 0x0d);
    ret += os08b10_write_register(vi_pipe, 0x364b, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x364c, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x364d, 0x0e);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x364e, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x364f, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x3650, 0xf7);
    ret += os08b10_write_register(vi_pipe, 0x3651, 0x77);
    ret += os08b10_write_register(vi_pipe, 0x365a, 0xbb);
    ret += os08b10_write_register(vi_pipe, 0x365b, 0x9d);
    ret += os08b10_write_register(vi_pipe, 0x365c, 0x8e);
    ret += os08b10_write_register(vi_pipe, 0x365d, 0x86);
    ret += os08b10_write_register(vi_pipe, 0x3661, 0x07);
    ret += os08b10_write_register(vi_pipe, 0x3662, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3667, 0xd4);
    ret += os08b10_write_register(vi_pipe, 0x366f, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3671, 0x88);
    ret += os08b10_write_register(vi_pipe, 0x3673, 0x2a);
    ret += os08b10_write_register(vi_pipe, 0x3678, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3679, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3682, 0xf1);
    ret += os08b10_write_register(vi_pipe, 0x3687, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3688, 0x81);
    ret += os08b10_write_register(vi_pipe, 0x3689, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3692, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3701, 0x39);
    ret += os08b10_write_register(vi_pipe, 0x3703, 0x32);
    ret += os08b10_write_register(vi_pipe, 0x3705, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3706, 0x74);
    ret += os08b10_write_register(vi_pipe, 0x3708, 0x35);
    ret += os08b10_write_register(vi_pipe, 0x370a, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x370b, 0x4b);
    ret += os08b10_write_register(vi_pipe, 0x3712, 0x13);
    ret += os08b10_write_register(vi_pipe, 0x3714, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3715, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3716, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3719, 0x23);
    ret += os08b10_write_register(vi_pipe, 0x371c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x371d, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x373b, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x3755, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3756, 0xea);
    ret += os08b10_write_register(vi_pipe, 0x3757, 0xea);
    ret += os08b10_write_register(vi_pipe, 0x376b, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x376c, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x376d, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x376e, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x376f, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x377a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x377b, 0x01);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part4(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3785, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3790, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3792, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3796, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3797, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3798, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3799, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37bb, 0x88);
    ret += os08b10_write_register(vi_pipe, 0x37be, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x37bf, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x37c0, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37c7, 0x51);
    ret += os08b10_write_register(vi_pipe, 0x37c8, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x37c9, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37ca, 0xe4);
    ret += os08b10_write_register(vi_pipe, 0x37cc, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x37cf, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x37d0, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37d1, 0x74);
    ret += os08b10_write_register(vi_pipe, 0x37d2, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x37d3, 0x4b);
    ret += os08b10_write_register(vi_pipe, 0x37d4, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37d5, 0x74);
    ret += os08b10_write_register(vi_pipe, 0x37d6, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x37d7, 0x4b);
    ret += os08b10_write_register(vi_pipe, 0x3800, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3801, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3802, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3803, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3804, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3805, 0x1f);
    ret += os08b10_write_register(vi_pipe, 0x3806, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3807, 0x7f);
    ret += os08b10_write_register(vi_pipe, 0x3808, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3809, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x380a, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x380b, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x380c, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x380d, 0x38);
    ret += os08b10_write_register(vi_pipe, 0x380e, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x380f, 0xc8);
    ret += os08b10_write_register(vi_pipe, 0x3811, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3813, 0x05); /* vertical offset 0x8 */
    ret += os08b10_write_register(vi_pipe, 0x3814, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3815, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3816, 0x01);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part5(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3817, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x381c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3820, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3821, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3822, 0x14); /* vts add disable[4]: 1 dis; 0 enable */
    ret += os08b10_write_register(vi_pipe, 0x3823, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x382b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x382c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x383e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x384c, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x384d, 0x38);
    ret += os08b10_write_register(vi_pipe, 0x3858, 0x3c);
    ret += os08b10_write_register(vi_pipe, 0x3850, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3851, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3852, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3853, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3865, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3866, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3867, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3868, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x386a, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x386b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x386c, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x386d, 0x61);
    ret += os08b10_write_register(vi_pipe, 0x3881, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3882, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3883, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3884, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3885, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3886, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3887, 0x20);
    ret += os08b10_write_register(vi_pipe, 0x3888, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3900, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3940, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c37, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c49, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c4a, 0x9a);
    ret += os08b10_write_register(vi_pipe, 0x3c4c, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3c4d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c4e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c50, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c51, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c52, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c67, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3c69, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3c6a, 0x00);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part6(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3c6b, 0x62);
    ret += os08b10_write_register(vi_pipe, 0x3c6c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c6d, 0x62);
    ret += os08b10_write_register(vi_pipe, 0x3c70, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c71, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x3c72, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c73, 0x62);
    ret += os08b10_write_register(vi_pipe, 0x3c74, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c75, 0x62);
    ret += os08b10_write_register(vi_pipe, 0x3c76, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x3c77, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x3c79, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c7a, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c7b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cb6, 0x41);
    ret += os08b10_write_register(vi_pipe, 0x3cb9, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cc0, 0x90);
    ret += os08b10_write_register(vi_pipe, 0x3cc2, 0x90);
    ret += os08b10_write_register(vi_pipe, 0x3cc5, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cc6, 0x98);
    ret += os08b10_write_register(vi_pipe, 0x3cc7, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cc8, 0x98);
    ret += os08b10_write_register(vi_pipe, 0x3cc9, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cca, 0x98);
    ret += os08b10_write_register(vi_pipe, 0x3ccc, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3cd9, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3cda, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3cdb, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3cdc, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3d8c, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x3d8d, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x4001, 0x2b); /* OB ouput 0x6b */
    ret += os08b10_write_register(vi_pipe, 0x4004, 0x01); /* blc target 0x40 */
    ret += os08b10_write_register(vi_pipe, 0x4005, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4008, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x4009, 0x0d);
    ret += os08b10_write_register(vi_pipe, 0x400a, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x400b, 0xa8);
    ret += os08b10_write_register(vi_pipe, 0x400e, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x401a, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x402e, 0x01); /* blc target 0x40 */
    ret += os08b10_write_register(vi_pipe, 0x402f, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4030, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4031, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x4032, 0x9f);
    ret += os08b10_write_register(vi_pipe, 0x4050, 0x01);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part7(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x4051, 0x06);
    ret += os08b10_write_register(vi_pipe, 0x405d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x405e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4288, 0xcf);
    ret += os08b10_write_register(vi_pipe, 0x4289, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x428a, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x430b, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x430c, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x430d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x430e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4314, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x4500, 0x1a);
    ret += os08b10_write_register(vi_pipe, 0x4501, 0x18);
    ret += os08b10_write_register(vi_pipe, 0x4504, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4505, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4506, 0x32);
    ret += os08b10_write_register(vi_pipe, 0x4507, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x4508, 0x1a);
    ret += os08b10_write_register(vi_pipe, 0x450a, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x450b, 0x28);
    ret += os08b10_write_register(vi_pipe, 0x450c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x450f, 0x80);
    ret += os08b10_write_register(vi_pipe, 0x4513, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4516, 0x28);
    ret += os08b10_write_register(vi_pipe, 0x4603, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x460a, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x4640, 0x62);
    ret += os08b10_write_register(vi_pipe, 0x464d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4800, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x480e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4813, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4837, 0x0b);
    ret += os08b10_write_register(vi_pipe, 0x484b, 0x27);
    ret += os08b10_write_register(vi_pipe, 0x4850, 0x47);
    ret += os08b10_write_register(vi_pipe, 0x4851, 0xaa);
    ret += os08b10_write_register(vi_pipe, 0x4852, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x4853, 0x20);
    ret += os08b10_write_register(vi_pipe, 0x4854, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x4855, 0x30);
    ret += os08b10_write_register(vi_pipe, 0x4860, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4d00, 0x4d);
    ret += os08b10_write_register(vi_pipe, 0x4d01, 0x42);
    ret += os08b10_write_register(vi_pipe, 0x4d02, 0xb9);
    ret += os08b10_write_register(vi_pipe, 0x4d03, 0x94);
    ret += os08b10_write_register(vi_pipe, 0x4d04, 0x95);
    ret += os08b10_write_register(vi_pipe, 0x4d05, 0xce);

    return ret;
}

static td_s32 os08b10_linear_8m30_12bit_init_part8(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x4d09, 0x6f);
    ret += os08b10_write_register(vi_pipe, 0x5000, 0x03); /* sensor dpc 0xf */
    ret += os08b10_write_register(vi_pipe, 0x5001, 0x0d);
    ret += os08b10_write_register(vi_pipe, 0x5080, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x50c0, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5100, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5200, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x5201, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x5681, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5780, 0x53);
    ret += os08b10_write_register(vi_pipe, 0x5781, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x5782, 0x60);
    ret += os08b10_write_register(vi_pipe, 0x5783, 0xf0);
    ret += os08b10_write_register(vi_pipe, 0x5784, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5785, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x5786, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x5788, 0x60);
    ret += os08b10_write_register(vi_pipe, 0x5789, 0xf0);
    ret += os08b10_write_register(vi_pipe, 0x5791, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x5792, 0x11);
    ret += os08b10_write_register(vi_pipe, 0x5793, 0x33);

    os08b10_default_reg_init(vi_pipe);

    delay_ms(1);   /* delay 1 ms */
    ret += os08b10_write_register(vi_pipe, 0x0100, 0x01);

    return ret;
}


static void os08b10_linear_8m30_12bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_linear_8m30_12bit_init_part1(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part2(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part3(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part4(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part5(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part6(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part7(vi_pipe);
    ret += os08b10_linear_8m30_12bit_init_part8(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("os08b10 write register failed!\n");
        return;
    }

    printf("========================================================================\n");
    printf("vi_pipe:%d,== os08b10 24Mclk 8M30fps(MIPI) 12bit linear init success! ==\n", vi_pipe);
    printf("========================================================================\n");

    return;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part1(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x0103, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0109, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0104, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x0102, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0303, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0305, 0x3e);
    ret += os08b10_write_register(vi_pipe, 0x0306, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x0307, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0308, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x0309, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x030a, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x030c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x0317, 0x49);
    ret += os08b10_write_register(vi_pipe, 0x0322, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0323, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x0324, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x0325, 0x90);
    ret += os08b10_write_register(vi_pipe, 0x0327, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x0328, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x0329, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x032a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x032c, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x032d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x032e, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x032f, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3002, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x300f, 0x11);
    ret += os08b10_write_register(vi_pipe, 0x3012, 0x41);
    ret += os08b10_write_register(vi_pipe, 0x301e, 0xb8);
    ret += os08b10_write_register(vi_pipe, 0x3026, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x3027, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x302c, 0x1a);
    ret += os08b10_write_register(vi_pipe, 0x3104, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3106, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3400, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3408, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x3409, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x340a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x340c, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x340d, 0xa4);
    ret += os08b10_write_register(vi_pipe, 0x3420, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3421, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3422, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3423, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3424, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3425, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3426, 0x55);
    ret += os08b10_write_register(vi_pipe, 0x3427, 0x15);
    ret += os08b10_write_register(vi_pipe, 0x3428, 0x55);
    ret += os08b10_write_register(vi_pipe, 0x3429, 0x15);
    ret += os08b10_write_register(vi_pipe, 0x342a, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x342b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x342c, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x342d, 0x54);
    ret += os08b10_write_register(vi_pipe, 0x342e, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x342f, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x3501, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x3502, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3504, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3541, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3542, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3581, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3582, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3603, 0x30);
    ret += os08b10_write_register(vi_pipe, 0x3612, 0x96);
    ret += os08b10_write_register(vi_pipe, 0x3613, 0xe0);
    ret += os08b10_write_register(vi_pipe, 0x3623, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3624, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3625, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x362a, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x362b, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x362c, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x362d, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x362e, 0x17);
    ret += os08b10_write_register(vi_pipe, 0x362f, 0x2d);
    ret += os08b10_write_register(vi_pipe, 0x3630, 0x67);
    ret += os08b10_write_register(vi_pipe, 0x3631, 0x7f);
    ret += os08b10_write_register(vi_pipe, 0x3638, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3639, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x363a, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3643, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3644, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3645, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3646, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3647, 0x06);
    ret += os08b10_write_register(vi_pipe, 0x3648, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3649, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x364a, 0x0d);
    ret += os08b10_write_register(vi_pipe, 0x364b, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x364c, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x364d, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x364e, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x364f, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x3650, 0xf7);
    ret += os08b10_write_register(vi_pipe, 0x3651, 0x77);
    ret += os08b10_write_register(vi_pipe, 0x365a, 0xbb);
    ret += os08b10_write_register(vi_pipe, 0x365b, 0x9d);
    ret += os08b10_write_register(vi_pipe, 0x365c, 0x8e);
    ret += os08b10_write_register(vi_pipe, 0x365d, 0x86);
    ret += os08b10_write_register(vi_pipe, 0x3661, 0x07);
    ret += os08b10_write_register(vi_pipe, 0x3662, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x3667, 0x54);
    ret += os08b10_write_register(vi_pipe, 0x366f, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3671, 0x89);
    ret += os08b10_write_register(vi_pipe, 0x3673, 0x2a);
    ret += os08b10_write_register(vi_pipe, 0x3678, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x3679, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3682, 0xf1);
    ret += os08b10_write_register(vi_pipe, 0x3687, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3688, 0x81);
    ret += os08b10_write_register(vi_pipe, 0x3689, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3692, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3701, 0x39);
    ret += os08b10_write_register(vi_pipe, 0x3703, 0x32);
    ret += os08b10_write_register(vi_pipe, 0x3705, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3706, 0x3a);
    ret += os08b10_write_register(vi_pipe, 0x3708, 0x27);
    ret += os08b10_write_register(vi_pipe, 0x370a, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x370b, 0x8a);
    ret += os08b10_write_register(vi_pipe, 0x3712, 0x13);
    ret += os08b10_write_register(vi_pipe, 0x3714, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3715, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3716, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part4(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3719, 0x23);
    ret += os08b10_write_register(vi_pipe, 0x371c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x371d, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x373b, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x3755, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3756, 0xa9);
    ret += os08b10_write_register(vi_pipe, 0x3757, 0xa9);
    ret += os08b10_write_register(vi_pipe, 0x376b, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x376c, 0x32);
    ret += os08b10_write_register(vi_pipe, 0x376d, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x376e, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x376f, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x377a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x377b, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3785, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3790, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3792, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3796, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3797, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3798, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3799, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37bb, 0x88);
    ret += os08b10_write_register(vi_pipe, 0x37be, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x37bf, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x37c0, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37c7, 0x51);
    ret += os08b10_write_register(vi_pipe, 0x37c8, 0x22);
    ret += os08b10_write_register(vi_pipe, 0x37c9, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37ca, 0xb6);
    ret += os08b10_write_register(vi_pipe, 0x37cc, 0x0e);
    ret += os08b10_write_register(vi_pipe, 0x37cf, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x37d0, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37d1, 0x3a);
    ret += os08b10_write_register(vi_pipe, 0x37d2, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37d3, 0x8a);
    ret += os08b10_write_register(vi_pipe, 0x37d4, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37d5, 0x3a);
    ret += os08b10_write_register(vi_pipe, 0x37d6, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x37d7, 0x8a);
    ret += os08b10_write_register(vi_pipe, 0x3800, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3801, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part5(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3802, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3803, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3804, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3805, 0x1f);
    ret += os08b10_write_register(vi_pipe, 0x3806, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3807, 0x7f);
    ret += os08b10_write_register(vi_pipe, 0x3808, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x3809, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x380a, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x380b, 0x70); /* vertical 0x70 */
    ret += os08b10_write_register(vi_pipe, 0x380c, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x380d, 0xd0);
    ret += os08b10_write_register(vi_pipe, 0x380e, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x380f, 0x86);
    ret += os08b10_write_register(vi_pipe, 0x3811, 0x11); /* horizontal offset 0x11 */
    ret += os08b10_write_register(vi_pipe, 0x3813, 0x01); /* vertical offset 0x1 */
    ret += os08b10_write_register(vi_pipe, 0x3814, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3815, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3816, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3817, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x381c, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3820, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x3821, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3822, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x3823, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x382b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x382c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x383e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x384c, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x384d, 0xd0);
    ret += os08b10_write_register(vi_pipe, 0x3858, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3850, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3851, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3852, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3853, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x3865, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3866, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3867, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3868, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x386a, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x386b, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part6(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x386c, 0x05);
    ret += os08b10_write_register(vi_pipe, 0x386d, 0x61);
    ret += os08b10_write_register(vi_pipe, 0x3881, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x3882, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3883, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x3884, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3885, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3886, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3887, 0x20);
    ret += os08b10_write_register(vi_pipe, 0x3888, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x3c37, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c49, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c4a, 0x9a);
    ret += os08b10_write_register(vi_pipe, 0x3c4c, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3c4d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c4e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c50, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c51, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c52, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c67, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3c69, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x3c6a, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c6b, 0x52);
    ret += os08b10_write_register(vi_pipe, 0x3c6c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c6d, 0x52);
    ret += os08b10_write_register(vi_pipe, 0x3c70, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c71, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x3c72, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c73, 0x52);
    ret += os08b10_write_register(vi_pipe, 0x3c74, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c75, 0x52);
    ret += os08b10_write_register(vi_pipe, 0x3c76, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x3c77, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x3c79, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c7a, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3c7b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cb6, 0x41);
    ret += os08b10_write_register(vi_pipe, 0x3cb9, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cc0, 0x90);
    ret += os08b10_write_register(vi_pipe, 0x3cc2, 0x90);
    ret += os08b10_write_register(vi_pipe, 0x3cc5, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part7(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x3cc6, 0x98);
    ret += os08b10_write_register(vi_pipe, 0x3cc7, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cc8, 0x98);
    ret += os08b10_write_register(vi_pipe, 0x3cc9, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x3cca, 0x98);
    ret += os08b10_write_register(vi_pipe, 0x3ccc, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3cd9, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3cda, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3cdb, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3cdc, 0x7c);
    ret += os08b10_write_register(vi_pipe, 0x3d8c, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x3d8d, 0x10);
    ret += os08b10_write_register(vi_pipe, 0x4001, 0xeb);
    ret += os08b10_write_register(vi_pipe, 0x4004, 0x00); /* blc target 0x4b */
    ret += os08b10_write_register(vi_pipe, 0x4005, 0x4b);
    ret += os08b10_write_register(vi_pipe, 0x4008, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x4009, 0x0d);
    ret += os08b10_write_register(vi_pipe, 0x400a, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x400b, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x400e, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x401a, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x402e, 0x00); /* blc target 0x4b */
    ret += os08b10_write_register(vi_pipe, 0x402f, 0x4b);
    ret += os08b10_write_register(vi_pipe, 0x4030, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4031, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x4032, 0x1f);
    ret += os08b10_write_register(vi_pipe, 0x4050, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x4051, 0x06);
    ret += os08b10_write_register(vi_pipe, 0x405d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x405e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4288, 0xce);
    ret += os08b10_write_register(vi_pipe, 0x4289, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x428a, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x430b, 0x0f);
    ret += os08b10_write_register(vi_pipe, 0x430c, 0xfc);
    ret += os08b10_write_register(vi_pipe, 0x430d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x430e, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4314, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x4500, 0x1a);
    ret += os08b10_write_register(vi_pipe, 0x4501, 0x18);
    ret += os08b10_write_register(vi_pipe, 0x4504, 0x00);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part8(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x4505, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4506, 0x32);
    ret += os08b10_write_register(vi_pipe, 0x4507, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x4508, 0x1a);
    ret += os08b10_write_register(vi_pipe, 0x450a, 0x12);
    ret += os08b10_write_register(vi_pipe, 0x450b, 0x28);
    ret += os08b10_write_register(vi_pipe, 0x450c, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x450f, 0x80);
    ret += os08b10_write_register(vi_pipe, 0x4513, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4516, 0x28);
    ret += os08b10_write_register(vi_pipe, 0x4603, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x460a, 0x50);
    ret += os08b10_write_register(vi_pipe, 0x4640, 0x62);
    ret += os08b10_write_register(vi_pipe, 0x464d, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4800, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x480e, 0x04);
    ret += os08b10_write_register(vi_pipe, 0x4813, 0x84);
    ret += os08b10_write_register(vi_pipe, 0x4837, 0x0a);
    ret += os08b10_write_register(vi_pipe, 0x484b, 0x67);
    ret += os08b10_write_register(vi_pipe, 0x4850, 0x47);
    ret += os08b10_write_register(vi_pipe, 0x4851, 0xaa);
    ret += os08b10_write_register(vi_pipe, 0x4852, 0xff);
    ret += os08b10_write_register(vi_pipe, 0x4853, 0x20);
    ret += os08b10_write_register(vi_pipe, 0x4854, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x4855, 0x30);
    ret += os08b10_write_register(vi_pipe, 0x4860, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x4d00, 0x4d);
    ret += os08b10_write_register(vi_pipe, 0x4d01, 0x42);
    ret += os08b10_write_register(vi_pipe, 0x4d02, 0xb9);
    ret += os08b10_write_register(vi_pipe, 0x4d03, 0x94);
    ret += os08b10_write_register(vi_pipe, 0x4d04, 0x95);
    ret += os08b10_write_register(vi_pipe, 0x4d05, 0xce);
    ret += os08b10_write_register(vi_pipe, 0x4d09, 0x6f);
    ret += os08b10_write_register(vi_pipe, 0x5000, 0x03); /* sensor dpc 0x3 */
    ret += os08b10_write_register(vi_pipe, 0x5001, 0x0d);
    ret += os08b10_write_register(vi_pipe, 0x5080, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x50c0, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5100, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5200, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x5201, 0x70);
    ret += os08b10_write_register(vi_pipe, 0x5681, 0x02);
    ret += os08b10_write_register(vi_pipe, 0x5780, 0x53);

    return ret;
}

static td_s32 os08b10_vc_wdr_2t1_8m30_12bit_init_part9(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_write_register(vi_pipe, 0x5781, 0x03);
    ret += os08b10_write_register(vi_pipe, 0x5782, 0x18);
    ret += os08b10_write_register(vi_pipe, 0x5783, 0x3c);
    ret += os08b10_write_register(vi_pipe, 0x5784, 0x00);
    ret += os08b10_write_register(vi_pipe, 0x5785, 0x40);
    ret += os08b10_write_register(vi_pipe, 0x5786, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x5788, 0x18);
    ret += os08b10_write_register(vi_pipe, 0x5789, 0x3c);
    ret += os08b10_write_register(vi_pipe, 0x5791, 0x08);
    ret += os08b10_write_register(vi_pipe, 0x5792, 0x11);
    ret += os08b10_write_register(vi_pipe, 0x5793, 0x33);

    ret += os08b10_write_register(vi_pipe, 0x3508, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3509, 0x80);
    ret += os08b10_write_register(vi_pipe, 0x350a, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x350b, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x350c, 0x00);

    ret += os08b10_write_register(vi_pipe, 0x3548, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x3549, 0x80);
    ret += os08b10_write_register(vi_pipe, 0x354a, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x354b, 0x01);
    ret += os08b10_write_register(vi_pipe, 0x354c, 0x00);

    os08b10_default_reg_init(vi_pipe);
    delay_ms(1);   /* delay 1 ms */
    ret += os08b10_write_register(vi_pipe, 0x0100, 0x01);

    return ret;
}


static void os08b10_vc_wdr_2t1_8m30_12bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part1(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part2(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part3(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part4(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part5(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part6(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part7(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part8(vi_pipe);
    ret += os08b10_vc_wdr_2t1_8m30_12bit_init_part9(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("os08b10 write register failed!\n");
        return;
    }

    printf("========================================================================\n");
    printf("vi_pipe:%d,== os08b10 24Mclk 8M30fps(MIPI) 12bit vc-wdr init success! ==\n", vi_pipe);
    printf("========================================================================\n");

    return;
}
