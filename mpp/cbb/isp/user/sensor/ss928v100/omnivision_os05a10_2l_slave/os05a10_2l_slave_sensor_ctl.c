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

#include "ot_mpi_isp.h"
#include "ot_sns_ctrl.h"
#include "os05a10_2l_slave_cmos.h"

#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8

static int g_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};

int os05a10_slave_i2c_init(ot_vi_pipe vi_pipe)
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
    ot_isp_sns_commbus *os05a10slavebusinfo = TD_NULL;
    os05a10slavebusinfo = os05a10_slave_get_bus_info(vi_pipe);
    dev_num = os05a10slavebusinfo->i2c_dev;
    (td_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return TD_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_I2C_SLAVE_FORCE, (OS05A10_SLAVE_I2C_ADDR >> 1));
    if (ret < 0) {
        isp_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif

    return TD_SUCCESS;
}

int os05a10_slave_i2c_exit(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 os05a10_slave_read_register(ot_vi_pipe vi_pipe, td_u32 addr)
{
    ot_unused(vi_pipe);
    ot_unused(addr);
    return TD_SUCCESS;
}

td_s32 os05a10_slave_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return TD_SUCCESS;
    }

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = OS05A10_SLAVE_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = OS05A10_SLAVE_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = OS05A10_SLAVE_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        isp_err_trace("GPIO-I2C write failed!\n");
        return ret;
    }
#else
    td_u32 idx = 0;
    td_s32 ret;
    td_u8 buf[I2C_BUF_NUM];

    if (OS05A10_SLAVE_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx] = (addr >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    } else {
    }

    if (OS05A10_SLAVE_DATA_BYTE == 2) {  /* 2 byte */
    } else {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[vi_pipe], buf, OS05A10_SLAVE_ADDR_BYTE + OS05A10_SLAVE_DATA_BYTE);
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

void os05a10_slave_prog(ot_vi_pipe vi_pipe, const td_u32 *rom)
{
    ot_unused(vi_pipe);
    ot_unused(rom);
    return;
}

void os05a10_slave_standby(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

void os05a10_slave_restart(ot_vi_pipe vi_pipe)
{
    ot_unused(vi_pipe);
    return;
}

static void os05a10_slave_vc_wdr_2t1_5m30_10bit_init(ot_vi_pipe vi_pipe);
static void os05a10_slave_linear_5m30_12bit_init(ot_vi_pipe vi_pipe);

static void os05a10_slave_default_reg_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    ot_isp_sns_state *pastos05a10slave = TD_NULL;

    pastos05a10slave = os05a10_slave_get_ctx(vi_pipe);
    for (i = 0; i < pastos05a10slave->regs_info[0].reg_num; i++) {
        ret += os05a10_slave_write_register(vi_pipe,
            pastos05a10slave->regs_info[0].i2c_data[i].reg_addr, pastos05a10slave->regs_info[0].i2c_data[i].data);
    }

    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }

    return;
}

void os05a10_set_slave_registers(ot_vi_pipe vi_pipe)
{
    td_s32  ot_ret;
    td_s32  slave_dev;
    td_u32  data;
    td_u8   img_mode;
    ot_isp_sns_state *pastos05a10slave = TD_NULL;
    os05a10_slave_video_mode_tbl *os05a10_slave_mode_tbl = TD_NULL;
    ot_isp_slave_sns_sync *os05a10_slave_sync = os05a10_get_slave_sync(vi_pipe);
    td_s32 os05a10_slave_bind_dev = os05a10_get_slave_bind_dev(vi_pipe);
    td_u32 os05a10_slave_sensor_mode_time = os05a10_get_slave_sensor_mode_time(vi_pipe);
    ot_isp_sns_state *os05a10_slave_sns_state = os05a10_slave_get_ctx(vi_pipe);

    pastos05a10slave = os05a10_slave_get_ctx(vi_pipe);
    img_mode    = pastos05a10slave->img_mode;
    slave_dev  = os05a10_slave_bind_dev;
    data       = os05a10_slave_sensor_mode_time;

    os05a10_slave_mode_tbl = os05a10_get_slave_mode_tbl(img_mode);

    check_ret(ot_mpi_isp_get_sns_slave_attr(slave_dev, os05a10_slave_sync));
    os05a10_slave_sync->cfg.bits.bit_h_enable = 0;
    os05a10_slave_sync->cfg.bits.bit_v_enable = 0;
    os05a10_slave_sync->slave_mode_time = data;
    check_ret(ot_mpi_isp_set_sns_slave_attr(slave_dev, os05a10_slave_sync));
    ot_ret = os05a10_slave_i2c_init(vi_pipe);
    if (ot_ret != TD_SUCCESS) {
        isp_err_trace("i2c init failed!\n");
        return;
    }
    check_ret(ot_mpi_isp_get_sns_slave_attr(slave_dev, os05a10_slave_sync));
    os05a10_slave_sync->hs_time = os05a10_slave_mode_tbl->inck_per_hs;

    if (os05a10_slave_sns_state->regs_info[0].slv_sync.slave_vs_time == 0) {
        os05a10_slave_sync->vs_time = os05a10_slave_mode_tbl->inck_per_vs;
    } else {
        os05a10_slave_sync->vs_time = os05a10_slave_sns_state->regs_info[0].slv_sync.slave_vs_time;
    }
    os05a10_slave_sync->cfg.bytes = 0xc0030000;
    os05a10_slave_sync->hs_cyc = 0x3;
    os05a10_slave_sync->vs_cyc = 0x3;

    check_ret(ot_mpi_isp_set_sns_slave_attr(slave_dev, os05a10_slave_sync));

    return;
}

void os05a10_slave_init(ot_vi_pipe vi_pipe)
{
    ot_wdr_mode      wdr_mode;
    td_bool          init;
    td_u8            img_mode;
    ot_isp_sns_state *pastos05a10slave = TD_NULL;
    ot_isp_slave_sns_sync *slave_sync = TD_NULL;

    pastos05a10slave = os05a10_slave_get_ctx(vi_pipe);
    init        = pastos05a10slave->init;
    wdr_mode    = pastos05a10slave->wdr_mode;
    img_mode    = pastos05a10slave->img_mode;

    os05a10_set_slave_registers(vi_pipe);
    /* When sensor first init, config all registers */
    if (init == TD_FALSE) {
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
            if (OS05A10_SLAVE_8M_30FPS_10BIT_2TO1_VC_MODE == img_mode) { /* OS05A10_8M_30FPS_10BIT_2TO1_WDR */
                os05a10_slave_vc_wdr_2t1_5m30_10bit_init(vi_pipe);
            }
        } else {
            os05a10_slave_linear_5m30_12bit_init(vi_pipe);
        }
    } else {
        /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
            if (OS05A10_SLAVE_8M_30FPS_10BIT_2TO1_VC_MODE == img_mode) { /* OS05A10_8M_30FPS_10BIT_2TO1_WDR */
                os05a10_slave_vc_wdr_2t1_5m30_10bit_init(vi_pipe);
            }
        } else {
            os05a10_slave_linear_5m30_12bit_init(vi_pipe);
        }
    }
    pastos05a10slave->init = TD_TRUE;

    slave_sync = os05a10_get_slave_sync(vi_pipe);
    slave_sync->cfg.bytes = 0xc0030001;
    check_ret(ot_mpi_isp_set_sns_slave_attr(os05a10_get_slave_bind_dev(vi_pipe), slave_sync));
    return;
}

void os05a10_slave_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    ret = os05a10_slave_i2c_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("OS05A10 exit failed!\n");
    }

    return;
}

static td_s32 os05a10_slave_linear_5m30_12bit_init_part1(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0001, 0xf2);
    ret += os05a10_slave_write_register(vi_pipe, 0x0103, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0303, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0305, 0x32);
    ret += os05a10_slave_write_register(vi_pipe, 0x0306, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0307, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0308, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x0309, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x032a, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x031e, 0x09);
    ret += os05a10_slave_write_register(vi_pipe, 0x0325, 0x48);
    ret += os05a10_slave_write_register(vi_pipe, 0x0328, 0x07);
    ret += os05a10_slave_write_register(vi_pipe, 0x300d, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x300e, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x300f, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x3026, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3027, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3010, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3012, 0x21);
    ret += os05a10_slave_write_register(vi_pipe, 0x3016, 0xf0);
    ret += os05a10_slave_write_register(vi_pipe, 0x3018, 0xf0);
    ret += os05a10_slave_write_register(vi_pipe, 0x3028, 0xf0);
    ret += os05a10_slave_write_register(vi_pipe, 0x301e, 0x98);
    ret += os05a10_slave_write_register(vi_pipe, 0x3010, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3011, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3031, 0xa9);
    ret += os05a10_slave_write_register(vi_pipe, 0x3103, 0x48);
    ret += os05a10_slave_write_register(vi_pipe, 0x3104, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3106, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3400, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3025, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x3425, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3428, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3406, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3408, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x3501, 0x09);
    ret += os05a10_slave_write_register(vi_pipe, 0x3502, 0xa0);
    ret += os05a10_slave_write_register(vi_pipe, 0x3505, 0x83);
    ret += os05a10_slave_write_register(vi_pipe, 0x3508, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3509, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x350a, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x350b, 0x00);

    return ret;
}

static td_s32 os05a10_slave_linear_5m30_12bit_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x350c, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x350d, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x350e, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x350f, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3600, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3626, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x3605, 0x50);
    ret += os05a10_slave_write_register(vi_pipe, 0x3609, 0xb5);
    ret += os05a10_slave_write_register(vi_pipe, 0x3610, 0x69);
    ret += os05a10_slave_write_register(vi_pipe, 0x360c, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3628, 0xa4);
    ret += os05a10_slave_write_register(vi_pipe, 0x3629, 0x6a);
    ret += os05a10_slave_write_register(vi_pipe, 0x362d, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3660, 0x43);
    ret += os05a10_slave_write_register(vi_pipe, 0x3661, 0x06);
    ret += os05a10_slave_write_register(vi_pipe, 0x3662, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3663, 0x28);
    ret += os05a10_slave_write_register(vi_pipe, 0x3664, 0x0d);
    ret += os05a10_slave_write_register(vi_pipe, 0x366a, 0x38);
    ret += os05a10_slave_write_register(vi_pipe, 0x366b, 0xa0);
    ret += os05a10_slave_write_register(vi_pipe, 0x366d, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x366e, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3680, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x36c0, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3621, 0x81);
    ret += os05a10_slave_write_register(vi_pipe, 0x3634, 0x31);
    ret += os05a10_slave_write_register(vi_pipe, 0x3620, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3622, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x362a, 0xd0);
    ret += os05a10_slave_write_register(vi_pipe, 0x362e, 0x8c);
    ret += os05a10_slave_write_register(vi_pipe, 0x362f, 0x98);
    ret += os05a10_slave_write_register(vi_pipe, 0x3630, 0xb0);
    ret += os05a10_slave_write_register(vi_pipe, 0x3631, 0xd7);
    ret += os05a10_slave_write_register(vi_pipe, 0x3701, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x3737, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3740, 0x18);
    ret += os05a10_slave_write_register(vi_pipe, 0x3741, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x373c, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x373b, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3705, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3706, 0x50);
    ret += os05a10_slave_write_register(vi_pipe, 0x370a, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x370b, 0xe4);

    return ret;
}

static td_s32 os05a10_slave_linear_5m30_12bit_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x3709, 0x4a);
    ret += os05a10_slave_write_register(vi_pipe, 0x3714, 0x21);
    ret += os05a10_slave_write_register(vi_pipe, 0x371c, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x371d, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x375e, 0x0e);
    ret += os05a10_slave_write_register(vi_pipe, 0x3760, 0x13);
    ret += os05a10_slave_write_register(vi_pipe, 0x3776, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3781, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3782, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3783, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3784, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3785, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3788, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3789, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3797, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3798, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3799, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3761, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3762, 0x0d);
    ret += os05a10_slave_write_register(vi_pipe, 0x3800, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3801, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3802, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3803, 0x0c);
    ret += os05a10_slave_write_register(vi_pipe, 0x3804, 0x0e);
    ret += os05a10_slave_write_register(vi_pipe, 0x3805, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x3806, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3807, 0x6f);
    ret += os05a10_slave_write_register(vi_pipe, 0x3808, 0x0a);
    ret += os05a10_slave_write_register(vi_pipe, 0x3809, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x380a, 0x05);
    ret += os05a10_slave_write_register(vi_pipe, 0x380b, 0xf0);
    ret += os05a10_slave_write_register(vi_pipe, 0x380c, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x380d, 0xd0);
    ret += os05a10_slave_write_register(vi_pipe, 0x380e, 0x09);
    ret += os05a10_slave_write_register(vi_pipe, 0x380f, 0xc0);
    ret += os05a10_slave_write_register(vi_pipe, 0x3811, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3813, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3814, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3815, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3816, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3817, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x381c, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3820, 0x00);

    return ret;
}

static td_s32 os05a10_slave_linear_5m30_12bit_init_part4(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x3821, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3822, 0x54);
    ret += os05a10_slave_write_register(vi_pipe, 0x3823, 0x18);
    ret += os05a10_slave_write_register(vi_pipe, 0x3826, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3827, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3833, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3832, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x383c, 0x48);
    ret += os05a10_slave_write_register(vi_pipe, 0x383d, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x3843, 0x20);
    ret += os05a10_slave_write_register(vi_pipe, 0x382d, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d85, 0x0b);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d84, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d8c, 0x63);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d8d, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4000, 0x78);
    ret += os05a10_slave_write_register(vi_pipe, 0x4001, 0x2b);
    ret += os05a10_slave_write_register(vi_pipe, 0x4004, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4005, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x4028, 0x2f);
    ret += os05a10_slave_write_register(vi_pipe, 0x400a, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x4010, 0x12);
    ret += os05a10_slave_write_register(vi_pipe, 0x4008, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x4009, 0x0d);
    ret += os05a10_slave_write_register(vi_pipe, 0x401a, 0x58);
    ret += os05a10_slave_write_register(vi_pipe, 0x4050, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4051, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x4052, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4053, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x4054, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4055, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x4056, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4057, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x4058, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4059, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x430b, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x430c, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x430d, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x430e, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4501, 0x18);
    ret += os05a10_slave_write_register(vi_pipe, 0x4502, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4643, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4640, 0x01);

    return ret;
}

static td_s32 os05a10_slave_linear_5m30_12bit_init_part5(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x4641, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x480e, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4813, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4815, 0x2b);
    ret += os05a10_slave_write_register(vi_pipe, 0x486e, 0x36);
    ret += os05a10_slave_write_register(vi_pipe, 0x486f, 0x84);
    ret += os05a10_slave_write_register(vi_pipe, 0x4860, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4861, 0xa0);
    ret += os05a10_slave_write_register(vi_pipe, 0x484b, 0x05);
    ret += os05a10_slave_write_register(vi_pipe, 0x4850, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4851, 0xaa);
    ret += os05a10_slave_write_register(vi_pipe, 0x4852, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x4853, 0x8a);
    ret += os05a10_slave_write_register(vi_pipe, 0x4854, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x4855, 0x30);
    ret += os05a10_slave_write_register(vi_pipe, 0x4800, 0x60);
    ret += os05a10_slave_write_register(vi_pipe, 0x4837, 0x14);
    ret += os05a10_slave_write_register(vi_pipe, 0x484a, 0x3f);
    ret += os05a10_slave_write_register(vi_pipe, 0x5000, 0xc9);
    ret += os05a10_slave_write_register(vi_pipe, 0x5001, 0x43);
    ret += os05a10_slave_write_register(vi_pipe, 0x5002, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x5211, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x5291, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x520d, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x520e, 0xfd);
    ret += os05a10_slave_write_register(vi_pipe, 0x520f, 0xa5);
    ret += os05a10_slave_write_register(vi_pipe, 0x5210, 0xa5);
    ret += os05a10_slave_write_register(vi_pipe, 0x528d, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x528e, 0xfd);
    ret += os05a10_slave_write_register(vi_pipe, 0x528f, 0xa5);
    ret += os05a10_slave_write_register(vi_pipe, 0x5290, 0xa5);
    ret += os05a10_slave_write_register(vi_pipe, 0x5004, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x5005, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x5180, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x5181, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x5182, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x5183, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x580b, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d00, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d01, 0xe9);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d02, 0xba);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d03, 0x66);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d04, 0x46);

    return ret;
}

static td_s32 os05a10_slave_linear_5m30_12bit_init_part6(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x4d05, 0xa5);
    ret += os05a10_slave_write_register(vi_pipe, 0x3603, 0x3c);
    ret += os05a10_slave_write_register(vi_pipe, 0x3703, 0x26);
    ret += os05a10_slave_write_register(vi_pipe, 0x3709, 0x49);
    ret += os05a10_slave_write_register(vi_pipe, 0x3708, 0x2d);
    ret += os05a10_slave_write_register(vi_pipe, 0x3719, 0x1c);
    ret += os05a10_slave_write_register(vi_pipe, 0x371a, 0x06);
    ret += os05a10_slave_write_register(vi_pipe, 0x4000, 0x79);
    ret += os05a10_slave_write_register(vi_pipe, 0x380c, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x380d, 0xae);
    ret += os05a10_slave_write_register(vi_pipe, 0x380e, 0x06);
    ret += os05a10_slave_write_register(vi_pipe, 0x380f, 0x54);
    ret += os05a10_slave_write_register(vi_pipe, 0x3501, 0x09);
    ret += os05a10_slave_write_register(vi_pipe, 0x3502, 0xbc);
    ret += os05a10_slave_write_register(vi_pipe, 0x4028, 0x6f);
    ret += os05a10_slave_write_register(vi_pipe, 0x4029, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x402a, 0x3f);
    ret += os05a10_slave_write_register(vi_pipe, 0x402b, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3002, 0x00);

    os05a10_slave_default_reg_init(vi_pipe);
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x01);
    delay_ms(5); /* delay 5 ms */

    ret += os05a10_slave_write_register(vi_pipe, 0x3009, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x377e, 0x0A);
    ret += os05a10_slave_write_register(vi_pipe, 0x3797, 0x84);
    ret += os05a10_slave_write_register(vi_pipe, 0x3798, 0x64);
    ret += os05a10_slave_write_register(vi_pipe, 0x3799, 0x64);
    ret += os05a10_slave_write_register(vi_pipe, 0x3818, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3819, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x381a, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x381b, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3822, 0x70);
    ret += os05a10_slave_write_register(vi_pipe, 0x3823, 0x50);
    ret += os05a10_slave_write_register(vi_pipe, 0x3824, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3825, 0x75);
    ret += os05a10_slave_write_register(vi_pipe, 0x3826, 0x06);
    ret += os05a10_slave_write_register(vi_pipe, 0x3827, 0x50);
    ret += os05a10_slave_write_register(vi_pipe, 0x3832, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3834, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3842, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x01);

    printf("OV os05a_2lane_init_5M_2688x1520_12bit_linear30 Initial OK!-------\n");
    return ret;
}

static void os05a10_slave_linear_5m30_12bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os05a10_slave_linear_5m30_12bit_init_part1(vi_pipe);
    ret += os05a10_slave_linear_5m30_12bit_init_part2(vi_pipe);
    ret += os05a10_slave_linear_5m30_12bit_init_part3(vi_pipe);
    ret += os05a10_slave_linear_5m30_12bit_init_part4(vi_pipe);
    ret += os05a10_slave_linear_5m30_12bit_init_part5(vi_pipe);
    ret += os05a10_slave_linear_5m30_12bit_init_part6(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("os05a10 slave write register failed!\n");
        return;
    }

    printf("========================================================================\n");
    printf("== os05a10 slave  24Mclk 5M30fps(MIPI) 12bit linear init success! ==\n");
    printf("========================================================================\n");
    return;
}

static td_s32 os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part1(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    // @@ Res 2592x1944 2lane MIPI0800Mbps Linear10 30fps MCLK24MHz SCLK90MHz
    // version : R1A_AM11

    ret += os05a10_slave_write_register(vi_pipe, 0x4600, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x4601, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x4603, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0103, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0303, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x0305, 0x32);
    ret += os05a10_slave_write_register(vi_pipe, 0x0306, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0307, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x0308, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x0309, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x032a, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x031e, 0x09);
    ret += os05a10_slave_write_register(vi_pipe, 0x0325, 0x48);
    ret += os05a10_slave_write_register(vi_pipe, 0x0328, 0x07);
    ret += os05a10_slave_write_register(vi_pipe, 0x300d, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x300e, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x300f, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x3010, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3012, 0x21);
    ret += os05a10_slave_write_register(vi_pipe, 0x3016, 0xf0);
    ret += os05a10_slave_write_register(vi_pipe, 0x3018, 0xf0);

    printf("-------OV os05a_2lane_init_5M_2592x1944_12bit_linear30 Initial OK!-------\n");

    return ret;
}

static td_s32 os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part2(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x360c, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3628, 0xa4);
    ret += os05a10_slave_write_register(vi_pipe, 0x362d, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3660, 0x42);
    ret += os05a10_slave_write_register(vi_pipe, 0x3661, 0x07);
    ret += os05a10_slave_write_register(vi_pipe, 0x3662, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3663, 0x28);
    ret += os05a10_slave_write_register(vi_pipe, 0x3664, 0x0d);
    ret += os05a10_slave_write_register(vi_pipe, 0x366a, 0x38);
    ret += os05a10_slave_write_register(vi_pipe, 0x366b, 0xa0);
    ret += os05a10_slave_write_register(vi_pipe, 0x366d, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x366e, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3680, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x36c0, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3701, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x373b, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x373c, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3736, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3737, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3705, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3706, 0x39);
    ret += os05a10_slave_write_register(vi_pipe, 0x370a, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x370b, 0x98);
    ret += os05a10_slave_write_register(vi_pipe, 0x3709, 0x49);
    ret += os05a10_slave_write_register(vi_pipe, 0x3714, 0x21);
    ret += os05a10_slave_write_register(vi_pipe, 0x371c, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x371d, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3740, 0x1b);
    ret += os05a10_slave_write_register(vi_pipe, 0x3741, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x375e, 0x0b);
    ret += os05a10_slave_write_register(vi_pipe, 0x3760, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3776, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x377e, 0x0a); // Slave internal counter (dacclk) enable
    ret += os05a10_slave_write_register(vi_pipe, 0x3781, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3782, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3783, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3784, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3785, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3788, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3789, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3797, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x3762, 0x11);
    ret += os05a10_slave_write_register(vi_pipe, 0x3800, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3801, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3802, 0x00);
    return ret;
}

static td_s32 os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part3(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x3803, 0x0c);
    ret += os05a10_slave_write_register(vi_pipe, 0x3804, 0x0e);
    ret += os05a10_slave_write_register(vi_pipe, 0x3805, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x3806, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3807, 0x6f);
    ret += os05a10_slave_write_register(vi_pipe, 0x3808, 0x0f);
    ret += os05a10_slave_write_register(vi_pipe, 0x3809, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x380a, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x380b, 0x70);
    ret += os05a10_slave_write_register(vi_pipe, 0x380c, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x380d, 0x0c);
    ret += os05a10_slave_write_register(vi_pipe, 0x380e, 0x09);
    ret += os05a10_slave_write_register(vi_pipe, 0x380f, 0x0a);
    ret += os05a10_slave_write_register(vi_pipe, 0x3813, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x3814, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3815, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3816, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3817, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x3818, 0x00); // Slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3819, 0x00); // Slave
    ret += os05a10_slave_write_register(vi_pipe, 0x381a, 0x00); // Slave
    ret += os05a10_slave_write_register(vi_pipe, 0x381b, 0x01); // Slave
    ret += os05a10_slave_write_register(vi_pipe, 0x381c, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3820, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3821, 0x24);
    ret += os05a10_slave_write_register(vi_pipe, 0x3823, 0x50); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3824, 0x02); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3825, 0x06); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3826, 0x09); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3827, 0x06); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x382d, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3832, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x3834, 0x04); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3833, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x383c, 0x48);
    ret += os05a10_slave_write_register(vi_pipe, 0x383d, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x3842, 0x10); // slave
    ret += os05a10_slave_write_register(vi_pipe, 0x3d85, 0x0b);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d84, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d8c, 0x63);
    ret += os05a10_slave_write_register(vi_pipe, 0x3d8d, 0xd7);
    ret += os05a10_slave_write_register(vi_pipe, 0x4000, 0xf8);
    ret += os05a10_slave_write_register(vi_pipe, 0x4001, 0x2b);
    return ret;
}

static td_s32 os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part4(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += os05a10_slave_write_register(vi_pipe, 0x4004, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4005, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x400a, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x400f, 0xa0);
    ret += os05a10_slave_write_register(vi_pipe, 0x4010, 0x12);
    ret += os05a10_slave_write_register(vi_pipe, 0x4018, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4008, 0x02);
    ret += os05a10_slave_write_register(vi_pipe, 0x4009, 0x0d);
    ret += os05a10_slave_write_register(vi_pipe, 0x401a, 0x58);
    ret += os05a10_slave_write_register(vi_pipe, 0x4050, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4051, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x4028, 0x2f);
    ret += os05a10_slave_write_register(vi_pipe, 0x4052, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4053, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x4054, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4055, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x4056, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4057, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x4058, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4059, 0x80);
    ret += os05a10_slave_write_register(vi_pipe, 0x430b, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x430c, 0xff);
    ret += os05a10_slave_write_register(vi_pipe, 0x430d, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x430e, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4501, 0x18);
    ret += os05a10_slave_write_register(vi_pipe, 0x4502, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4643, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x4640, 0x01);
    ret += os05a10_slave_write_register(vi_pipe, 0x4641, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x4800, 0x64);
    ret += os05a10_slave_write_register(vi_pipe, 0x4809, 0x2b);
    ret += os05a10_slave_write_register(vi_pipe, 0x4813, 0x98);
    ret += os05a10_slave_write_register(vi_pipe, 0x4817, 0x04);
    ret += os05a10_slave_write_register(vi_pipe, 0x4833, 0x18);
    ret += os05a10_slave_write_register(vi_pipe, 0x4837, 0x0b);
    ret += os05a10_slave_write_register(vi_pipe, 0x483b, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x484b, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x4850, 0x7c);
    ret += os05a10_slave_write_register(vi_pipe, 0x4852, 0x06);
    ret += os05a10_slave_write_register(vi_pipe, 0x4856, 0x58);
    ret += os05a10_slave_write_register(vi_pipe, 0x4857, 0xaa);
    ret += os05a10_slave_write_register(vi_pipe, 0x4862, 0x0a);
    ret += os05a10_slave_write_register(vi_pipe, 0x4869, 0x18);
    ret += os05a10_slave_write_register(vi_pipe, 0x486a, 0xaa);
    return ret;
}

static td_s32 os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part5(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os05a10_slave_write_register(vi_pipe, 0x486e, 0x07);
    ret += os05a10_slave_write_register(vi_pipe, 0x486f, 0x55);
    ret += os05a10_slave_write_register(vi_pipe, 0x4875, 0xf0);
    ret += os05a10_slave_write_register(vi_pipe, 0x5000, 0x89);
    ret += os05a10_slave_write_register(vi_pipe, 0x5001, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x5004, 0x40);
    ret += os05a10_slave_write_register(vi_pipe, 0x5005, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x5180, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x5181, 0x10);
    ret += os05a10_slave_write_register(vi_pipe, 0x580b, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d00, 0x03);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d01, 0xc9);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d02, 0xbc);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d03, 0xc6);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d04, 0x4a);
    ret += os05a10_slave_write_register(vi_pipe, 0x4d05, 0x25);
    ret += os05a10_slave_write_register(vi_pipe, 0x4700, 0x2b);
    ret += os05a10_slave_write_register(vi_pipe, 0x4e00, 0x2b);
    ret += os05a10_slave_write_register(vi_pipe, 0x3501, 0x08);
    ret += os05a10_slave_write_register(vi_pipe, 0x3502, 0xe1);
    ret += os05a10_slave_write_register(vi_pipe, 0x3511, 0x00);
    ret += os05a10_slave_write_register(vi_pipe, 0x3512, 0x20);
    ret += os05a10_slave_write_register(vi_pipe, 0x3833, 0x01);

    os05a10_slave_default_reg_init(vi_pipe);

    delay_ms(1);
    ret += os05a10_slave_write_register(vi_pipe, 0x0100, 0x01);

    return ret;
}

static void os05a10_slave_vc_wdr_2t1_5m30_10bit_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part1(vi_pipe);
    ret += os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part2(vi_pipe);
    ret += os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part3(vi_pipe);
    ret += os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part4(vi_pipe);
    ret += os05a10_slave_vc_wdr_2t1_5m30_10bit_init_part5(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("os05a10 slave write register failed!\n");
        return;
    }

    printf("========================================================================\n");
    printf("== os05a10 slave 24Mclk 5M30fps(MIPI) 10bit vc-wdr init success! ==\n");
    printf("========================================================================\n");
    return;
}
