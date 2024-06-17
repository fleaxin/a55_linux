/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "icm20690.h"
#include <linux/slab.h>
#ifndef __LITEOS__
#include <linux/kernel.h>
#include <asm/dma.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#else
#include "linux/kernel.h"
#include "asm/dma.h"
#include "linux/delay.h"
#include "linux/interrupt.h"
#include <linux/module.h>
#include <linux/kernel.h>
#endif

#include "sys_ext.h"
#include "securec.h"

#include "ot_common_motionsensor.h"
#include "motionsensor_ext.h"
#include "motionsensor_gpio.h"
#include "motionsensor_spi.h"

#ifdef TRANSFER_I2C
#include "i2c_dev.h"
#elif (defined TRANSFER_SPI) || (defined __LITEOS__)
#include "spi_dev.h"
#endif

static const td_u32 g_motionsensor_spi_num = 1;

static icm20690_dev_info *g_icm20690_dev = TD_NULL;
static ot_msensor_data g_imu_data;

static osal_spinlock_t g_lock_get_data;

static td_u8 g_fifo_data_len;
static ot_msensor_attr g_msensor_mode;
static td_u64 g_time_backup[TIME_RECORD_CNT] = { 0 };
static td_u32 g_every_data_cnt_backup[DATA_RECORD_CNT] = { 0 };
static td_u8 g_time_cnt;
static td_u8 g_data_count;

/* for debug */
msensor_triger_mode g_msensor_triger_mode = TRIGER_TIMER;
td_u8 g_msensor_fifo_en = TD_TRUE;
EXPORT_SYMBOL_GPL(g_msensor_triger_mode);
EXPORT_SYMBOL_GPL(g_msensor_fifo_en);

#define FIFO_LEN 1024

#ifdef __LITEOS__
static gpio_groupbit_info g_group_bit_info;
#endif

td_u32 g_data_num = 0;
td_u32 g_i_thread = 0;
td_u64 g_pts_now;

#define IMU_ACC_DATA_X      g_imu_data.acc_buf.acc_data[g_imu_data.acc_buf.data_num].x
#define IMU_ACC_DATA_Y      g_imu_data.acc_buf.acc_data[g_imu_data.acc_buf.data_num].y
#define IMU_ACC_DATA_Z      g_imu_data.acc_buf.acc_data[g_imu_data.acc_buf.data_num].z
#define IMU_ACC_DATA_TEMP   g_imu_data.acc_buf.acc_data[g_imu_data.acc_buf.data_num].temperature
#define IMU_ACC_DATA_PTS    g_imu_data.acc_buf.acc_data[g_imu_data.acc_buf.data_num].pts

#define IMU_GYRO_DATA_X     g_imu_data.gyro_buf.gyro_data[g_imu_data.gyro_buf.data_num].x
#define IMU_GYRO_DATA_Y     g_imu_data.gyro_buf.gyro_data[g_imu_data.gyro_buf.data_num].y
#define IMU_GYRO_DATA_Z     g_imu_data.gyro_buf.gyro_data[g_imu_data.gyro_buf.data_num].z
#define IMU_GYRO_DATA_TEMP  g_imu_data.gyro_buf.gyro_data[g_imu_data.gyro_buf.data_num].temperature
#define IMU_GYRO_DATA_PTS   g_imu_data.gyro_buf.gyro_data[g_imu_data.gyro_buf.data_num].pts

#define get_fifo_data(high, low) (td_s16)(((td_s16)(g_icm20690_dev->fifo_buf[high]) << 8) & 0xff00) | \
    (g_icm20690_dev->fifo_buf[low] & 0xff)

/* get mod */
#define remainder(a, b) ((a) - (b) * ((a) / (b)))

#define safe_kfree(memory) do { \
    if ((memory) != TD_NULL) { \
        osal_kfree(memory); \
        memory = TD_NULL; \
    } \
} while (0)

#ifndef __LITEOS__
/* linux */
static td_s32 icm20690_transfer_read(td_u8 reg_addr, td_u8 *reg_data, td_u32 cnt, td_bool fifo_mode)
{
    td_s32 ret;
#ifdef TRANSFER_I2C
    ret = motionsersor_i2c_read(g_icm20690_dev->client, reg_addr, reg_data, cnt);
#elif defined TRANSFER_SPI
    ret = ot_motionsensor_ssp_read_alt(g_motionsensor_spi_num, reg_addr, reg_data, cnt, fifo_mode);
#else
    ret = TD_FAILURE;
#endif
    return ret;
}

static td_s32 icm20690_transfer_write(td_u8 reg_addr, const td_u8 *reg_data, td_u32 cnt)
{
    td_s32 ret;
#ifdef TRANSFER_I2C
    ret = motionsersor_i2c_write(g_icm20690_dev->client, reg_addr, reg_data, cnt);
#elif defined TRANSFER_SPI
    ot_unused(cnt);
    ret = ot_motionsensor_ssp_write_alt(g_motionsensor_spi_num, reg_addr, reg_data);
#else
    ret = TD_FAILURE;
#endif
    return ret;
}

#else
/* liteos */
static td_s32 icm20690_transfer_read(td_u8 reg_addr, td_u8 *reg_data, td_u32 cnt, td_bool fifo_mode)
{
    td_s32 ret;
    ret = motionsersor_spi_read(reg_addr, reg_data, cnt, g_motionsensor_spi_num);
    return ret;
}

static td_s32 icm20690_transfer_write(td_u8 reg_addr, const td_u8 *reg_data, td_u32 cnt)
{
    td_s32 ret;
    ret = motionsersor_spi_write(reg_addr, reg_data, cnt, g_motionsensor_spi_num);
    return ret;
}
#endif

#ifdef TRANSFER_SPI
static td_s32 icm20690_i2c_disable(td_void)
{
    td_u8 ret;
    td_u8 register_value;
    register_value = 0x10;
    ret = icm20690_transfer_write(USER_CONTROL_REGISTER_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("disable i2c failed(%d)\n", ret);
        return -EAGAIN;
    }
    return TD_SUCCESS;
}
#endif

icm20690_dev_info *chip_get_dev_info(td_void)
{
    return g_icm20690_dev;
}

static td_s32 icm20690_set_clk(td_void)
{
    td_u8 ret;
    td_u8 register_value;

    ret = icm20690_transfer_read(POWER_MANAGEMENT_REGISTER_1_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("ICM20690 set clk failed\n");
        return -EAGAIN;
    }

    register_value = CLKSET_VALUE;
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(POWER_MANAGEMENT_REGISTER_1_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("ICM20690 set clk failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_reset(td_void)
{
    td_u8 ret;
    td_u8 register_value;

    ret = icm20690_transfer_read(POWER_MANAGEMENT_REGISTER_1_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("reset ICM20690 failed\n");
        return -EAGAIN;
    }

    register_value |= TRUE_REGISTER_VALUE << RESET_OFFSET;
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(POWER_MANAGEMENT_REGISTER_1_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("reset ICM20690 failed\n");
        return -EAGAIN;
    }

    msleep(100); /* 100ms */
#ifdef TRANSFER_SPI
    ret = icm20690_i2c_disable();
    if (ret != TD_SUCCESS) {
        print_info("disable i2c failed\n");
        return -EAGAIN;
    }
#endif
    /* enable PLL, CLKSEL  1 */
    ret = icm20690_set_clk();
    if (ret != TD_SUCCESS) {
        print_info("icm20690 set clk failed\n");
        return -EAGAIN;
    }
    msleep(30); /* 30ms */
    return TD_SUCCESS;
}

static td_s32 icm20690_set_axis_mode(td_u32 dev_mode)
{
    td_u8 ret;
    td_u8 register_value = 0xFF;

    if (dev_mode & OT_MSENSOR_DEVICE_GYRO) {
        register_value &= 0x38; /* 0x38 for acc disable */
    }

    if (dev_mode & OT_MSENSOR_DEVICE_ACC) {
        register_value &= 0x07; /* 0x07 for gyro disable */
    }

    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(POWER_MANAGEMENT_REGISTER_2_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 axis set failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_sample_rate(td_u64 sample_rate)
{
    td_u8 ret;
    td_u8 register_value;

    if ((sample_rate == 0) || osal_div64_u64_rem(1000, sample_rate)) { /* remainder of 1000 */
        print_info("sample_rate must be divisible by 1000,  %lld  \n", sample_rate);
        return -EAGAIN;
    }

    register_value = osal_div64_u64(1000, sample_rate) - 1; /* dividend 1000 */
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(SMPLRT_DIV, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 set sample rate failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_gyro_dlfp_cfg(td_u8 dlfp_cfg)
{
    td_u8 ret;
    td_u8 register_value;

    if (dlfp_cfg > 7) { /* dlfp_cfg max value 7 */
        print_info("icm20690 gyro dlfp cfg is invalid\n");
        return -EINVAL;
    }

    ret = icm20690_transfer_read(CONFIGURATION_REGISTER_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro dlfp cfg set failed\n");
        return -EAGAIN;
    }

    register_value &= ~0x7; /* 0x7 for bit 1, 2, 3 */
    register_value |= dlfp_cfg;
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(CONFIGURATION_REGISTER_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro dlfp cfg set failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_accel_dlfp_cfg(td_u8 dlfp_cfg)
{
    td_u8 ret;
    td_u8 register_value;

    if (dlfp_cfg > 7) { /* dlfp_cfg max value 7 */
        print_info("icm20690 acc dlfp cfg is invalid\n");
        return -EINVAL;
    }

    ret = icm20690_transfer_read(ACCEL_CONFIG_REGISTER_2_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 acc dlfp cfg set failed\n");
        return -EAGAIN;
    }

    register_value &= ~0x7; /* 0x7 for bit 1, 2, 3 */
    register_value |= dlfp_cfg;
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(ACCEL_CONFIG_REGISTER_2_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 acc dlfp cfg set failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_gryo_fchoice_b(td_u8 fchoice_b)
{
    td_u8 ret;
    td_u8 register_value;

    if (fchoice_b > 3) { /* fchoice_b max value 3 */
        print_info("icm20690 gyro fchoice_b is invalid\n");
        return -EINVAL;
    }

    ret = icm20690_transfer_read(GYRO_CONFIG_REGISTER_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro fchoice_b set failed\n");
        return -EAGAIN;
    }

    register_value &= ~0x3; /* 0x03 for bit 1, 2 */
    register_value |= fchoice_b;
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(GYRO_CONFIG_REGISTER_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro fchoice_b set failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_accel_fchoice_b(td_u8 fchoice_b)
{
    td_u8 ret;
    td_u8 register_value;

    if (fchoice_b > 1) { /* fchoice_b max value 1 */
        print_info("icm20690 acc fchoice_b is invalid\n");
        return -EINVAL;
    }

    ret = icm20690_transfer_read(ACCEL_CONFIG_REGISTER_2_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 acc fchoice_b set failed\n");
        return -EAGAIN;
    }

    register_value &= ~(0x1 << 3); /* 3 bits */
    register_value |= fchoice_b;
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(ACCEL_CONFIG_REGISTER_2_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 acc fchoice_b set failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_gyro_set_full_scale_range(td_u8 fs_sel)
{
    td_u8 ret;
    td_u8 register_value;

    ret = icm20690_transfer_read(GYRO_CONFIG_REGISTER_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro full scale range failed set\n");
        return -EAGAIN;
    }

    register_value &= ~(0x3 << 2); /* 0x3 left shift 2 bits */
    register_value |= (fs_sel << 2); /* 2 bits */
    print_info("register_value = %x\n", register_value);

    ret = icm20690_transfer_write(GYRO_CONFIG_REGISTER_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro full scale range failed set\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_ui_set_accel_full_scale_range(td_u8 fs_sel)
{
    td_u8 ret;
    td_u8 register_value;

    ret = icm20690_transfer_read(ACCEL_CONFIG_REGISTER_1_ADDR, &register_value, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 acc full scale range set failed\n");
        return -EAGAIN;
    }

    register_value &= ~(0x7 << 3); /* 0x7 left shift 3 bits */
    register_value |= (fs_sel << 3); /* 3 bits */

    ret = icm20690_transfer_write(ACCEL_CONFIG_REGISTER_1_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 acc full scale range set failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_gyro_odr_and_bw(const msensor_gyro_status *gyro_status)
{
    if (gyro_status->gyro_config.odr == GYRO_OUTPUT_DATA_RATE_32KHZ) {
        if (gyro_status->band_width == GYRO_BAND_WIDTH_8800HZ) {
            icm20690_set_gryo_fchoice_b(0x01);
        } else if (gyro_status->band_width == GYRO_BAND_WIDTH_3600HZ) {
            icm20690_set_gryo_fchoice_b(0x02);
        } else {
            print_info("band_width and ODR is not match\n");
            return -EINVAL;
        }
    } else if (gyro_status->gyro_config.odr == GYRO_OUTPUT_DATA_RATE_8KHZ) {
        if (gyro_status->band_width == GYRO_BAND_WIDTH_250HZ) {
            icm20690_set_gyro_dlfp_cfg(0x0);
            icm20690_set_gryo_fchoice_b(0x0);
        } else if (gyro_status->band_width == GYRO_BAND_WIDTH_3600HZ) {
            icm20690_set_gyro_dlfp_cfg(0x7);
            icm20690_set_gryo_fchoice_b(0x0);
        } else {
            print_info("band_width and ODR is not match\n");
            return -EINVAL;
        }
    } else if (gyro_status->gyro_config.odr <= 1000) { /* output data rate 1000 */
        switch (gyro_status->band_width) {
            case GYRO_BAND_WIDTH_5HZ:
                icm20690_set_gyro_dlfp_cfg(0x6);
                break;
            case GYRO_BAND_WIDTH_10HZ:
                icm20690_set_gyro_dlfp_cfg(0x5);
                break;
            case GYRO_BAND_WIDTH_20HZ:
                icm20690_set_gyro_dlfp_cfg(0x4);
                break;
            case GYRO_BAND_WIDTH_41HZ:
                icm20690_set_gyro_dlfp_cfg(0x3);
                break;
            case GYRO_BAND_WIDTH_92HZ:
                icm20690_set_gyro_dlfp_cfg(0x2);
                break;
            case GYRO_BAND_WIDTH_184HZ:
                icm20690_set_gyro_dlfp_cfg(0x1);
                break;
            default:
                print_info("band_width and ODR is not match\n");
                break;
        }
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_set_gyro_attr(msensor_gyro_status gyro_status)
{
    td_s32 ret;
    /* set gyro odr and BW */
    ret = icm20690_set_gyro_odr_and_bw(&gyro_status);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    /* set gyro FSR */
    print_info("gyro_status.range is %d\n", gyro_status.gyro_config.fsr);

    switch (gyro_status.gyro_config.fsr) {
        case GYRO_FULL_SCALE_RANGE_250DPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_250DPS);
            break;
        case GYRO_FULL_SCALE_RANGE_500DPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_500DPS);
            break;
        case GYRO_FULL_SCALE_RANGE_1KDPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_1000DPS);
            break;
        case GYRO_FULL_SCALE_RANGE_2KDPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_2000DPS);
            break;
        case GYRO_FULL_SCALE_RANGE_31DPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_31DPS);
            break;
        case GYRO_FULL_SCALE_RANGE_62DPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_62DPS);
            break;
        case GYRO_FULL_SCALE_RANGE_125DPS:
            icm20690_gyro_set_full_scale_range(ICM20690_GYRO_FULL_SCALE_SET_125DPS);
            break;
        default:
            print_info("icm20690 gyro attr is invalid\n");
            return -EINVAL;
    }
    return TD_SUCCESS;
}

static td_void icm20690_set_acc_odr_and_bw(const msensor_acc_status *acc_status)
{
    if (acc_status->acc_config.odr == ACCEL_OUTPUT_DATA_RATE_4KHZ) {
        icm20690_set_accel_fchoice_b(0x01);
    } else if (acc_status->acc_config.odr == ACCEL_OUTPUT_DATA_RATE_1KHZ) {
        icm20690_set_accel_fchoice_b(0x0);
        icm20690_set_accel_dlfp_cfg(0x7);
    } else if (acc_status->acc_config.odr < 1000) { /* output data rate 1000 */
        switch (acc_status->band_width) {
            case ACCEL_BAND_WIDTH_5HZ:
                icm20690_set_accel_dlfp_cfg(0x6);
                break;
            case ACCEL_BAND_WIDTH_10HZ:
                icm20690_set_accel_dlfp_cfg(0x5);
                break;
            case ACCEL_BAND_WIDTH_21HZ:
                icm20690_set_accel_dlfp_cfg(0x4);
                break;
            case ACCEL_BAND_WIDTH_44HZ:
                icm20690_set_accel_dlfp_cfg(0x3);
                break;
            case ACCEL_BAND_WIDTH_99HZ:
                icm20690_set_accel_dlfp_cfg(0x2);
                break;
            case ACCEL_BAND_WIDTH_218HZ:
                icm20690_set_accel_dlfp_cfg(0x1);
                break;
            default:
                print_info("band_width and ODR is not match\n");
                break;
        }
    }
    return;
}

static td_s32 icm20690_set_accel_attr(msensor_acc_status acc_status)
{
    /* set ACCEL odr and BW */
    icm20690_set_acc_odr_and_bw(&acc_status);

    /* set ACCEL FSR */
    print_info("accel_status.range is %d\n", acc_status.acc_config.fsr);

    switch (acc_status.acc_config.fsr) {
        case ACCEL_UI_FULL_SCALE_SET_2G:
            icm20690_ui_set_accel_full_scale_range(ICM20690_ACCEL_UI_FULL_SCALE_SET_2G);
            break;
        case ACCEL_UI_FULL_SCALE_SET_4G:
            icm20690_ui_set_accel_full_scale_range(ICM20690_ACCEL_UI_FULL_SCALE_SET_4G);
            break;
        case ACCEL_UI_FULL_SCALE_SET_8G:
            icm20690_ui_set_accel_full_scale_range(ICM20690_ACCEL_UI_FULL_SCALE_SET_8G);
            break;
        case ACCEL_UI_FULL_SCALE_SET_16G:
            icm20690_ui_set_accel_full_scale_range(ICM20690_ACCEL_UI_FULL_SCALE_SET_16G);
            break;
        default:
            print_info("icm20690 acc attr is invalid\n");
            return -EINVAL;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_gyro_low_power_mode_disable(td_void)
{
    td_u8 ret;
    td_u8 register_value = 0x00;

    ret = icm20690_transfer_write(LP_MODE_CONFIG_REGISTER_ADDR, &register_value, 1);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro low power mode disable failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_reset_fifo(td_void)
{
    /* reset FIFO , in case the FIFO is overflow, 00000100 -> 0x6A, set 00000100 to USER_CTRL (reset the FIFO) */
    td_u8 ch, ret;

    ret = icm20690_transfer_read(USER_CONTROL_REGISTER_ADDR, &ch, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("reset fifo failed\n");
        return -EAGAIN;
    }

    ch |= 0x04;

    ret = icm20690_transfer_write(USER_CONTROL_REGISTER_ADDR, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("reset fifo failed\n");
        return -EAGAIN;
    }

    g_icm20690_dev->flag_fifo_incomming = 0;
    return TD_SUCCESS;
}

td_s32 chip_fifo_data_reset(td_void)
{
    td_s32 ret;

    if (g_icm20690_dev->fifo_en == 0) {
        return TD_SUCCESS;
    }

    ret = icm20690_reset_fifo();
    if (ret != TD_SUCCESS) {
        print_info("reset fifo failed\n");
        return -EAGAIN;
    }

    return ret;
}

static td_s32 fifo_set_acc_size(td_void)
{
    td_u8 ch, ret;

    ret = icm20690_transfer_read(ACCEL_CONFIG_REGISTER_2_ADDR, &ch, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("enable the FIFO size to 1024 failed\n");
        return -EAGAIN;
    }

    ch |= 0xC0;
    print_info("register = %x\n", ch);
    ret = icm20690_transfer_write(ACCEL_CONFIG_REGISTER_2_ADDR, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("enable the FIFO size to 1024 failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 fifo_record_mode_enable(td_void)
{
    td_u8 ch, ret;

    ret = icm20690_transfer_read(CONFIGURATION_REGISTER_ADDR, &ch, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("enable FIFO mode and record mode failed\n");
        return -EAGAIN;
    }

    ch |= 0x80;
    print_info("CONFIGURATION_REGISTER_ADDR:%x\n", ch);
    ret = icm20690_transfer_write(CONFIGURATION_REGISTER_ADDR, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("enable FIFO mode and record mode failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 fifo_acc_and_gyro_enable(td_u32 dev_mode)
{
    td_u8 ch, ret;

    ret = icm20690_transfer_read(FIFO_ENABLE_REGISTER_ADDR, &ch, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("enable ACC and GYRO in FIFO failed\n");
        return -EAGAIN;
    }

    if (OT_MSENSOR_DEVICE_ACC & dev_mode) {
        ch |= 0x08;
        g_icm20690_dev->flag_acc_fifo_enabled = 1;
    }

    if (OT_MSENSOR_DEVICE_GYRO & dev_mode) {
        ch |= 0x70;
        g_icm20690_dev->flag_gyro_fifo_enabled = 1;
    }

    print_info("FIFO_ENABLE_REGISTER_ADDR:0x%x\n", ch);

    ret = icm20690_transfer_write(FIFO_ENABLE_REGISTER_ADDR, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("enable ACC and GYRO in FIFO failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_ui_fifo_mode_enable(td_u32 dev_mode)
{
    td_u8 ch, ret;

    ret = icm20690_reset_fifo();
    if (ret != TD_SUCCESS) {
        print_info("reset fifo failed\n");
        return -EAGAIN;
    }

    /* enable fifo */
    ch = 0x40;
    ret = icm20690_transfer_write(USER_CONTROL_REGISTER_ADDR, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("enable the FIFO operation mode failed(0x%x)\n", ret);
        return -EAGAIN;
    }

    /* set acc fifo size to 1024 bytes */
    ret = fifo_set_acc_size();
    if (ret != TD_SUCCESS) {
        return -EAGAIN;
    }

    /* enable FIFO mode and record mode */
    ret = fifo_record_mode_enable();
    if (ret != TD_SUCCESS) {
        return -EAGAIN;
    }

    /* enable ACC and GYRO in FIFO */
    ret = fifo_acc_and_gyro_enable(dev_mode);
    if (ret != TD_SUCCESS) {
        return -EAGAIN;
    }

    /* set fifo watermark threshold */
    ch = g_icm20690_dev->triger_data.triger_info.extern_interrupt_config.interrupt_num /
         (g_icm20690_dev->flag_acc_fifo_enabled + g_icm20690_dev->flag_gyro_fifo_enabled);
    ret = icm20690_transfer_write(FIFO_WATERMARK_THRESHOLD, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("enable the FIFO operation mode failed\n");
        return -EAGAIN;
    }

    /* set fifo data len */
    g_fifo_data_len =
        (g_icm20690_dev->flag_acc_fifo_enabled + g_icm20690_dev->flag_gyro_fifo_enabled) * 6; /* data length 6 */
    ret = icm20690_reset_fifo();
    if (ret != TD_SUCCESS) {
        print_info("reset fifo failed\n");
        return -EAGAIN;
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_int_config(td_void)
{
    td_u8 ch, ret;

    /* set interrupt config.. bit7:INT_LEVEL. bit6:INT_OPEN.bit5:LATCH_INT_EN.bit4:INT_RD_CLEAR */
    ch = 0xA0;
    ret = icm20690_transfer_write(INT_PIN_CONFIGURATION, &ch, 1);
    if (ret != TD_SUCCESS) {
        print_info("set INT config failed\n");
        return -EAGAIN;
    }

    /* set interrupt type  fifo watermark INT or data ready INT */
    if (g_icm20690_dev->fifo_en != 0) {
        ret = icm20690_transfer_read(FIFO_WM_INT_STATUS, &ch, 1, TD_FALSE);
        if (ret != TD_SUCCESS) {
            print_info("set INT enable failed\n");
            return -EAGAIN;
        }
    } else {
        /* data ready interrupt enable set */
        ch = 0x01;

        ret = icm20690_transfer_write(INTERRUPT_ENABLE, &ch, 1);
        if (ret != TD_SUCCESS) {
            print_info("set INT enable failed\n");
            return -EAGAIN;
        }

        icm20690_transfer_read(INTERRUPT_ENABLE, &ch, 1, TD_FALSE);
        print_info("INTERRUPT_ENABLE = %x\n", ch);
    }
    return ret;
}

static td_s32 icm20690_axis_set_attr(td_u32 dev_mode)
{
    td_s32 ret;

    if (g_icm20690_dev->triger_data.triger_mode == TRIGER_EXTERN_INTERRUPT) {
        /* interrupt config */
        ret = icm20690_int_config();
        if (ret != TD_SUCCESS) {
            print_info("INT config failed\n");
            return TD_FAILURE;
        }
    }

    if (OT_MSENSOR_DEVICE_GYRO & dev_mode) {
        /* 5. 250dps, 8K ODR, 250_hz BW for gyro */
        ret = icm20690_set_gyro_attr(g_icm20690_dev->gyro_status);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 set gyro attr failed\n");
            return TD_FAILURE;
        }

        msleep(5); /* 5ms */
    }

    if (OT_MSENSOR_DEVICE_ACC & dev_mode) {
        /* 6. accel FSR setting: UI accel to 4G, OIS accel to 2G */
        ret = icm20690_set_accel_attr(g_icm20690_dev->acc_status);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 set accel attr failed\n");
            return TD_FAILURE;
        }

        msleep(5); /* 5ms */
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_axis_set_sample_rate_1khz(td_u32 dev_mode)
{
    td_s32 ret;

    if (OT_MSENSOR_DEVICE_GYRO & dev_mode) {
        if (g_icm20690_dev->gyro_status.gyro_config.odr <= 1000) { /* odr 1000 */
            print_info("g_icm20690_dev->gyro_status.odr = %d\n", g_icm20690_dev->gyro_status.gyro_config.odr);
            ret = icm20690_set_sample_rate(g_icm20690_dev->gyro_status.gyro_config.odr);
            if (ret != TD_SUCCESS) {
                print_info("icm20690 set sample rate failed\n");
                return -EAGAIN;
            }

            msleep(5); /* 5ms */
        }
    } else {
        if (g_icm20690_dev->acc_status.acc_config.odr <= 1000) { /* odr 1000 */
            ret = icm20690_set_sample_rate(g_icm20690_dev->acc_status.acc_config.odr);
            if (ret != TD_SUCCESS) {
                print_info("icm20690 set sample rate failed\n");
                return -EAGAIN;
            }
            msleep(5); /* 5ms */
        }
    }
    return TD_SUCCESS;
}

static td_s32 icm20690_axis_fifo_sensor_init(td_u32 dev_mode)
{
    td_s32 ret;

    /* 1. reset ICM20690 and enable clk */
    ret = icm20690_reset();
    if (ret != TD_SUCCESS) {
        print_info("reset icm20690 failed\n");
        return -EAGAIN;
    }

    msleep(100); /* 100ms */

    /* 2. enable  gyro, accel */
    ret = icm20690_set_axis_mode(dev_mode);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 set axis mode failed\n");
        return -EAGAIN;
    }

    msleep(30); /* 30ms */

    /* 3. set sample rate to 1khz,1000/(1+0) */
    ret = icm20690_axis_set_sample_rate_1khz(dev_mode);
    if (ret != TD_SUCCESS) {
        return -EAGAIN;
    }

    if (g_icm20690_dev->fifo_en != 0) {
        /* 4.disable FIFO_MODE, DLFP_CFG = 0 */
        ret = icm20690_ui_fifo_mode_enable(dev_mode);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 ui fifo mode enable failed(%d)\n", ret);
            return -EAGAIN;
        }

        msleep(5); /* 5ms */

        g_icm20690_dev->fifo_buf = osal_kmalloc(FIFO_LEN, osal_gfp_kernel);
        if (g_icm20690_dev->fifo_buf == TD_NULL) {
            print_info("kzalloc fifo_buf failed\n");
            return -ENOMEM;
        }

        (td_void)memset_s(g_icm20690_dev->fifo_buf, FIFO_LEN, 0, FIFO_LEN);
    }

    /* icm20690 set attr */
    ret = icm20690_axis_set_attr(dev_mode);
    if (ret != TD_SUCCESS) {
        goto err_kzalloc;
    }

    /* 7. disable gyro low power mode */
    ret = icm20690_gyro_low_power_mode_disable();
    if (ret != TD_SUCCESS) {
        print_info("icm20690 gyro low power mode disable failed\n");
        goto err_kzalloc;
    }

    return TD_SUCCESS;
err_kzalloc:
    safe_kfree(g_icm20690_dev->fifo_buf);
    return -EAGAIN;
}

static td_s32 icm20690_sensor_init(td_u32 dev_mode)
{
    td_s32 ret;

    if ((dev_mode & OT_MSENSOR_DEVICE_GYRO) || (dev_mode & OT_MSENSOR_DEVICE_ACC)) {
        ret = icm20690_axis_fifo_sensor_init(dev_mode);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 sensor init failed! ret=%x\n", ret);
        }
    } else {
        print_info("icm20690 sensor init not support this mode : %d\n", dev_mode);
        ret = TD_FAILURE;
    }
    return ret;
}

/* get data */
static td_void icm20690_set_gyro_and_acc_buf_data(td_s32 i, td_ulong time_inter)
{
    IMU_ACC_DATA_X = get_fifo_data(i * g_fifo_data_len + 0, i * g_fifo_data_len + 1); /* offset 0, 1 */
    IMU_ACC_DATA_Y = get_fifo_data(i * g_fifo_data_len + 2, i * g_fifo_data_len + 3); /* offset 2, 3 */
    IMU_ACC_DATA_Z = get_fifo_data(i * g_fifo_data_len + 4, i * g_fifo_data_len + 5); /* offset 4, 5 */
    IMU_ACC_DATA_TEMP = g_icm20690_dev->temperature;
    IMU_ACC_DATA_PTS = g_icm20690_dev->gyro_status.last_pts + time_inter;
    g_icm20690_dev->acc_status.last_pts = IMU_ACC_DATA_PTS;
    g_imu_data.acc_buf.data_num++;

    IMU_GYRO_DATA_X = get_fifo_data(i * g_fifo_data_len + 6, i * g_fifo_data_len + 7); /* offset 6, 7 */
    IMU_GYRO_DATA_Y = get_fifo_data(i * g_fifo_data_len + 8, i * g_fifo_data_len + 9); /* offset 8, 9 */
    IMU_GYRO_DATA_Z = get_fifo_data(i * g_fifo_data_len + 10, i * g_fifo_data_len + 11); /* offset 10, 11 */
    IMU_GYRO_DATA_TEMP = g_icm20690_dev->temperature;
    IMU_GYRO_DATA_PTS = g_icm20690_dev->gyro_status.last_pts + time_inter;
    g_icm20690_dev->gyro_status.last_pts = IMU_GYRO_DATA_PTS;
    g_imu_data.gyro_buf.data_num++;
    return;
}

static td_void icm20690_set_gyro_buf_data(td_s32 i, td_ulong time_inter)
{
    IMU_GYRO_DATA_X = get_fifo_data(i * g_fifo_data_len + 0, i * g_fifo_data_len + 1); /* offset 0, 1 */
    IMU_GYRO_DATA_Y = get_fifo_data(i * g_fifo_data_len + 2, i * g_fifo_data_len + 3); /* offset 2, 3 */
    IMU_GYRO_DATA_Z = get_fifo_data(i * g_fifo_data_len + 4, i * g_fifo_data_len + 5); /* offset 4, 5 */
    IMU_GYRO_DATA_TEMP = g_icm20690_dev->temperature;
    IMU_GYRO_DATA_PTS = g_pts_now - (g_icm20690_dev->record_num - i - 1) * time_inter;
    g_icm20690_dev->gyro_status.last_pts = IMU_GYRO_DATA_PTS;
    g_imu_data.gyro_buf.data_num++;
    return;
}

static td_void icm20690_set_acc_buf_data(td_s32 i, td_ulong time_inter)
{
    IMU_ACC_DATA_X = get_fifo_data(i * g_fifo_data_len + 0, i * g_fifo_data_len + 1); /* offset 0, 1 */
    IMU_ACC_DATA_Y = get_fifo_data(i * g_fifo_data_len + 2, i * g_fifo_data_len + 3); /* offset 2, 3 */
    IMU_ACC_DATA_Z = get_fifo_data(i * g_fifo_data_len + 4, i * g_fifo_data_len + 5); /* offset 4, 5 */
    IMU_ACC_DATA_TEMP = g_icm20690_dev->temperature;
    IMU_ACC_DATA_PTS = g_pts_now - (g_icm20690_dev->record_num - i - 1) * time_inter;
    g_icm20690_dev->acc_status.last_pts = IMU_ACC_DATA_PTS;
    g_imu_data.acc_buf.data_num++;
    return;
}

static td_void icm20690_get_time_inter(td_ulong *time_inter)
{
    td_s32 i;
    td_u32 adddatacnt = 0;

    if (g_data_count < DATA_RECORD_CNT) {
        g_every_data_cnt_backup[g_data_count] = g_icm20690_dev->record_num;
    } else {
        for (i = 0; i < DATA_RECORD_CNT - 1; i++) {
            g_every_data_cnt_backup[i] = g_every_data_cnt_backup[i + 1];
        }
        g_every_data_cnt_backup[DATA_RECORD_CNT - 1] = g_icm20690_dev->record_num;
    }
    if (g_time_cnt < TIME_RECORD_CNT) {
        g_time_backup[g_time_cnt] = g_pts_now;

        for (i = 0; i <= g_data_count; i++) {
            adddatacnt += g_every_data_cnt_backup[i];
        }
        *time_inter = osal_div_u64(g_time_backup[g_time_cnt] - g_time_backup[0], div_0_to_1(adddatacnt));
    } else {
        for (i = 0; i < TIME_RECORD_CNT - 1; i++) {
            g_time_backup[i] = g_time_backup[i + 1];
        }

        g_time_backup[TIME_RECORD_CNT - 1] = g_pts_now;
        for (i = 1; i < DATA_RECORD_CNT; i++) {
            adddatacnt += g_every_data_cnt_backup[i];
        }
        *time_inter = osal_div_u64(g_time_backup[TIME_RECORD_CNT - 1] - g_time_backup[0], div_0_to_1(adddatacnt));
    }
    if (g_data_count < DATA_RECORD_CNT) {
        g_data_count++;
    }
    return;
}

static td_s32 icm20690_save_data_mode_dof_fifo(ot_msensor_attr attr)
{
    td_s32 i;
    td_s32 ret;
    td_ulong time_inter = 0;

    if (((attr.device_mask & OT_MSENSOR_DEVICE_GYRO) == 0) &&
        ((attr.device_mask & OT_MSENSOR_DEVICE_ACC) == 0)) {
        print_info("[error]not support MODE\n");
        return TD_FAILURE;
    }

    icm20690_get_time_inter(&time_inter);

    ret = memcpy_s(&g_imu_data.attr, sizeof(ot_msensor_attr), &attr, sizeof(ot_msensor_attr));
    if (ret != EOK) {
        return ret;
    }

    for (i = 0; i < g_icm20690_dev->record_num; i++) {
        if ((g_imu_data.acc_buf.data_num >= OT_MSENSOR_MAX_DATA_NUM) ||
            (g_imu_data.gyro_buf.data_num >= OT_MSENSOR_MAX_DATA_NUM)) {
            break;
        }

        if (attr.device_mask == (OT_MSENSOR_DEVICE_GYRO | OT_MSENSOR_DEVICE_ACC)) {
            icm20690_set_gyro_and_acc_buf_data(i, time_inter);
        } else if (attr.device_mask == OT_MSENSOR_DEVICE_GYRO) {
            icm20690_set_gyro_buf_data(i, time_inter);
        } else if (attr.device_mask == OT_MSENSOR_DEVICE_ACC) {
            icm20690_set_acc_buf_data(i, time_inter);
        }
    }

    if (g_time_cnt < TIME_RECORD_CNT) {
        g_time_backup[g_time_cnt] = g_icm20690_dev->gyro_status.last_pts;
        g_time_cnt++;
    } else {
        g_time_backup[TIME_RECORD_CNT - 1] = g_icm20690_dev->gyro_status.last_pts;
    }
    return ret;
}

static td_s32 icm20690_fifo_data_update(td_void)
{
    td_s32 ret;
    td_u32 i;

    if (g_icm20690_dev->fifo_length > 1024) { /* fifo max length 1024 */
        print_info("FIFO is over flow !!, NUM  = %d\n\n", g_icm20690_dev->record_num);
        ret = icm20690_reset_fifo(); /* fifo reset , over flow */
        if (ret != TD_SUCCESS) {
            print_info("reset fifo failed\n");
            return -EAGAIN;
        }

        return TD_SUCCESS;
    }

    /* begin to read fifo in several sections */
    for (i = 0; i < g_icm20690_dev->fifo_length / ICM20690_FIFO_R_MAX_SIZE; i++) {
        ret = icm20690_transfer_read(FIFO_R_W, g_icm20690_dev->fifo_buf + i * ICM20690_FIFO_R_MAX_SIZE,
                                     ICM20690_FIFO_R_MAX_SIZE, TD_TRUE);
        if (ret != TD_SUCCESS) {
            print_info("read FIFO in several sections failed\n");
            return -EAGAIN;
        }
    }

    ret = icm20690_transfer_read(FIFO_R_W, g_icm20690_dev->fifo_buf + i * ICM20690_FIFO_R_MAX_SIZE,
                                 remainder(g_icm20690_dev->fifo_length, ICM20690_FIFO_R_MAX_SIZE), TD_TRUE);
    if (ret != TD_SUCCESS) {
        print_info("read FIFO in last_data failed\n");
        return -EAGAIN;
    }

    g_icm20690_dev->flag_fifo_incomming = 1;

    return TD_SUCCESS;
}

static td_s32 icm20690_get_temperature(td_s32 *temperature)
{
    td_u32 ret;
    td_u8 register_value[2]; /* 2 register value */

    ret = icm20690_transfer_read(DEV_TEMPERATURE_LSB_ADDR, &register_value[0], 2, TD_FALSE); /* read count 2 */
    if (ret != TD_SUCCESS) {
        print_info("icm20690 get temperature failed\n");
        return -EAGAIN;
    }

    *temperature = ((td_s32)((td_s8)(register_value[0]) << 8)) | register_value[1]; /* 8 bts */
    /* 1024 times of celsius temperature magnification */
    *temperature = ROOMTEMP_OFFSET + ((*temperature) * GRADIENT_TEMP / TEMP_SENSITIVITY);
    return TD_SUCCESS;
}

static td_s32 icm20690_read_accel_data_xyz(ot_msensor_sample_data *acc_data)
{
    td_u8 buff[6] = { 0, 0, 0, 0, 0, 0 }; /* 6 byte buf */
    td_s32 ret;

    ret = icm20690_transfer_read(ACCEL_XOUT_H, &buff[0], 6, TD_FALSE); /* read count 6 */
    if (ret != TD_SUCCESS) {
        print_info("icm20690 read accel xyz failed\n");
        return -EAGAIN;
    }

    acc_data->x = ((buff[0] << 8) & 0xff00) | (buff[1] & 0xff); /* high 8 bits(0xff00) : 0, low 8 bits(0xff) : 1 */
    acc_data->y = ((buff[2] << 8) & 0xff00) | (buff[3] & 0xff); /* high 8 bits(0xff00) : 2, low 8 bits(0xff) : 3 */
    acc_data->z = ((buff[4] << 8) & 0xff00) | (buff[5] & 0xff); /* high 8 bits(0xff00) : 4, low 8 bits(0xff) : 5 */
    acc_data->temperature = g_icm20690_dev->temperature;
    acc_data->pts = g_pts_now;

    return TD_SUCCESS;
}

static td_s32 icm20690_read_gyro_data_xyz(ot_msensor_sample_data *gyro_data)
{
    td_u8 buff[6] = { 0, 0, 0, 0, 0, 0 }; /* 6 byte buf */
    td_s32 ret;

    ret = icm20690_transfer_read(GYRO_XOUT_H, &buff[0], 6, TD_FALSE); /* read count 6 */
    if (ret != TD_SUCCESS) {
        print_info("icm20690 read accel x_h failed\n");
        return -EAGAIN;
    }

    gyro_data->x = ((buff[0] << 8) & 0xff00) | (buff[1] & 0xff); /* high 8 bits(0xff00) : 0, low 8 bits(0xff) : 1 */
    gyro_data->y = ((buff[2] << 8) & 0xff00) | (buff[3] & 0xff); /* high 8 bits(0xff00) : 2, low 8 bits(0xff) : 3 */
    gyro_data->z = ((buff[4] << 8) & 0xff00) | (buff[5] & 0xff); /* high 8 bits(0xff00) : 4, low 8 bits(0xff) : 5 */
    gyro_data->temperature = g_icm20690_dev->temperature;
    gyro_data->pts = g_pts_now;

    return TD_SUCCESS;
}

static td_void icm20690_sample_data_assign(ot_msensor_sample_data *dst, const ot_msensor_sample_data *src)
{
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->temperature = src->temperature;
    dst->pts = src->pts;
    return;
}

static td_s32 icm20690_get_data_directly(td_void)
{
    td_s32 ret;

    (td_void)memset_s(&g_icm20690_dev->accel_cur_data, sizeof(ot_msensor_sample_data),
                      0x0, sizeof(ot_msensor_sample_data));
    (td_void)memset_s(&g_icm20690_dev->gyro_cur_data, sizeof(ot_msensor_sample_data),
                      0x0, sizeof(ot_msensor_sample_data));

    if (g_i_thread % 50 == 0) { /* remainder of 50 */
        ret = icm20690_get_temperature(&g_icm20690_dev->temperature);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 get temp failed! ret=%x\n", ret);
            return ret;
        }
    }

    if (g_msensor_mode.device_mask & OT_MSENSOR_DEVICE_ACC) {
        ret = icm20690_read_accel_data_xyz(&g_icm20690_dev->accel_cur_data);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 read accel data xyz failed\n");
            return -ENODATA;
        }
    }

    if (g_msensor_mode.device_mask & OT_MSENSOR_DEVICE_GYRO) {
        ret = icm20690_read_gyro_data_xyz(&g_icm20690_dev->gyro_cur_data);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 read gyro data xyz failed\n");
            return -ENODATA;
        }
    }

    ret = memcpy_s(&g_imu_data.attr, sizeof(ot_msensor_attr), &g_msensor_mode, sizeof(ot_msensor_attr));
    if (ret != EOK) {
        return ret;
    }

    if (g_msensor_mode.device_mask == (OT_MSENSOR_DEVICE_GYRO | OT_MSENSOR_DEVICE_ACC)) {
        /* accel data handler, pls fix this */
        g_imu_data.acc_buf.data_num = 1;
        icm20690_sample_data_assign(&(g_imu_data.acc_buf.acc_data[0]), &(g_icm20690_dev->accel_cur_data));

        /* gyro data handler*, pls fix this */
        g_imu_data.gyro_buf.data_num = 1;
        icm20690_sample_data_assign(&(g_imu_data.gyro_buf.gyro_data[0]), &(g_icm20690_dev->gyro_cur_data));

        /* PTS  handle */
        g_icm20690_dev->gyro_status.last_pts = g_pts_now;
    } else if (g_msensor_mode.device_mask == OT_MSENSOR_DEVICE_GYRO) {
        g_imu_data.gyro_buf.data_num = 1;
        icm20690_sample_data_assign(&(g_imu_data.gyro_buf.gyro_data[0]), &(g_icm20690_dev->gyro_cur_data));
    } else if (g_msensor_mode.device_mask == OT_MSENSOR_DEVICE_ACC) {
        g_imu_data.acc_buf.data_num = 1;
        icm20690_sample_data_assign(&(g_imu_data.acc_buf.acc_data[0]), &(g_icm20690_dev->accel_cur_data));
    }

    g_i_thread++;
    return TD_SUCCESS;
}

static td_s32 icm20690_get_data_from_fifo(td_void)
{
    td_s32 ret;

    if (g_icm20690_dev->record_num == 0) {
        return TD_SUCCESS;
    }

    if ((g_msensor_mode.temperature_mask & OT_MSENSOR_TEMP_GYRO) ||
        (g_msensor_mode.temperature_mask & OT_MSENSOR_TEMP_ACC)) {
        if (g_i_thread % 50 == 0) { /* 50 per time */
            ret = icm20690_get_temperature(&g_icm20690_dev->temperature);
            if (ret != TD_SUCCESS) {
                print_info("get_temp failed! ret=%x\n", ret);
                return ret;
            }
        }
    }

    ret = icm20690_fifo_data_update();
    if (ret != TD_SUCCESS) {
        print_info("ICM20690_UI_FIFO_DATA_UPDATE failed\n");
        return TD_FAILURE;
    }

    ret = icm20690_save_data_mode_dof_fifo(g_msensor_mode);
    if (ret != TD_SUCCESS) {
        print_info("motionsensor get data failed! ret=%x\n", ret);
        icm20690_reset_fifo();
        return TD_FAILURE;
    }

    g_i_thread++;
    return TD_SUCCESS;
}

static td_s32 icm20690_get_fifo_length_and_count(td_void)
{
    td_u32 record_length;
    td_u8 ret;
    td_u8 buff[2]; /* 2 byte buf */
    (td_void)memset_s(buff, sizeof(buff), 0, sizeof(buff));

    ret = icm20690_transfer_read(FIFO_COUNTH, buff, 2, TD_FALSE); /* read count 2 */
    if (ret != TD_SUCCESS) {
        print_info("get icm20690 fifo count length failed\n");
        return -EAGAIN;
    }

    g_icm20690_dev->record_num = (td_s16)((((td_s16)buff[0]) << 8) | buff[1]); /* 8 bits */

    record_length =
        ((g_icm20690_dev->flag_acc_fifo_enabled + g_icm20690_dev->flag_gyro_fifo_enabled) * 6); /* fifo length 6 */
    g_icm20690_dev->fifo_length = g_icm20690_dev->record_num * record_length;
    return TD_SUCCESS;
}

static td_u8 g_timer_frist_flag = 0;
static td_void icm20690_get_data_attr(td_void)
{
    td_s32 ret;

    if ((g_icm20690_dev->enable_kthread == TD_TRUE) && (g_data_num < 1000)) { /* max data num 1000 */
        /* get PTS */
        if (g_timer_frist_flag == 0) {
            td_u64 time_now = call_sys_get_time_stamp();
            g_icm20690_dev->gyro_status.last_pts = time_now;
            g_icm20690_dev->acc_status.last_pts = time_now;
            g_icm20690_dev->magn_status.last_pts = time_now;
            g_timer_frist_flag = TD_TRUE;
            g_pts_now = time_now;
            g_time_backup[g_time_cnt] = g_pts_now;
            g_time_cnt++;

            chip_fifo_data_reset();
            return;
        }

        if (g_icm20690_dev->fifo_en != 0) {
            /* get fifo count */
            ret = icm20690_get_fifo_length_and_count();
            if (ret != TD_SUCCESS) {
                print_info("get_icm20690_fifo_length_and_count failed\n");
                return;
            }
        }

        g_pts_now = call_sys_get_time_stamp();
    }
    return;
}

/* vi interrupt */
td_s32 chip_get_data_for_one_frm()
{
    td_s32 ret;
    td_ulong flags;
#ifndef __LITEOS__
    /* ktime_t stime */
    /* stime is ktime_set(0, g_icm20690_dev->triger_data.u_triger_info.timer_config.interval * NSEC_PER_USEC) */
#else
    union ktime stime;

    stime.tv.sec = 0;
    stime.tv.usec = g_icm20690_dev->triger_data.triger_info.timer_config.interval;
#endif

    osal_spin_lock_irqsave(&g_lock_get_data, &flags);
    icm20690_get_data_attr();

    if (g_icm20690_dev->fifo_en == 0) {
        ret = icm20690_get_data_directly();
        if (ret != TD_SUCCESS) {
            print_info("read data failed: %d\n", ret);
            osal_spin_unlock_irqrestore(&g_lock_get_data, &flags);
            return TD_FAILURE;
        }
    } else {
        ret = icm20690_get_data_from_fifo();
        if (ret != TD_SUCCESS) {
            print_info("read data failed: %d\n", ret);
            osal_spin_unlock_irqrestore(&g_lock_get_data, &flags);
            return TD_FAILURE;
        }
    }

    msensor_chip_int_callback(&g_imu_data);

    (td_void)memset_s(&g_imu_data, sizeof(ot_msensor_data), 0, sizeof(ot_msensor_data));
    osal_spin_unlock_irqrestore(&g_lock_get_data, &flags);
#ifndef __LITEOS__
#else
    hrtimer_forward(&g_icm20690_dev->hrtimer, stime);
#endif

    return TD_SUCCESS;
}

/* bottom half */
static td_void icm20690_work(struct osal_work_struct *work)
{
    td_s32 ret;
    td_ulong flags;

    ot_unused(work);

    osal_spin_lock_irqsave(&g_lock_get_data, &flags);
    if (g_icm20690_dev->enable_kthread != 0) {
        icm20690_get_data_attr();
        if ((g_icm20690_dev->fifo_en == TD_FALSE) && (g_data_num < 1000)) { /* max data num 1000 */
            ret = icm20690_get_data_directly();
            if (ret != TD_SUCCESS) {
                print_info("read_data failed: %d\n", ret);
                goto err0;
            }
        } else {
            ret = icm20690_get_data_from_fifo();
            if (ret != TD_SUCCESS) {
                print_info("get data handle failed: %d\n", ret);
                goto err0;
            }
        }
    }

    if ((g_imu_data.acc_buf.data_num > 40) || (g_imu_data.gyro_buf.data_num > 40)) { /* data num 40 */
        msensor_chip_int_callback(&g_imu_data);
        (td_void)memset_s(&g_imu_data, sizeof(ot_msensor_data), 0, sizeof(ot_msensor_data));
    }

err0:
    g_icm20690_dev->workqueue_call_times--;
    osal_wakeup(&g_icm20690_dev->wait_call_stop_working);
    osal_spin_unlock_irqrestore(&g_lock_get_data, &flags);
}

/* external interrupt */
static td_s32 icm20690_clear_irq(td_void)
{
    td_u8 ch, ret;
    /* read INT status */
    ret = icm20690_transfer_read(INT_STATUS, &ch, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("read INT status failed\n");
        return TD_FAILURE;
    }

    /* read watermark status */
    if (g_icm20690_dev->fifo_en != 0) {
        ret = icm20690_transfer_read(FIFO_WM_INT_STATUS, &ch, 1, TD_FALSE);
        if (ret != TD_SUCCESS) {
            print_info("read INT status failed\n");
            return TD_FAILURE;
        }
    }
    return TD_SUCCESS;
}

#ifndef __LITEOS__
static irqreturn_t icm20690_irq(int irq, td_void *data)
#else
static td_void icm20690_irq(td_u32 irq, td_void *data)
#endif
{
    td_s32 ret;

    ot_unused(irq);
    ot_unused(data);

    ret = icm20690_clear_irq();
    if (ret != TD_SUCCESS) {
        print_info("clear irq status failed\n");
#ifndef __LITEOS__
        return TD_FAILURE;
#else
        return;
#endif
    }

    icm20690_get_data_attr();
    osal_schedule_work(&g_icm20690_dev->work);

#ifndef __LITEOS__
    return IRQ_HANDLED;
#endif
}

td_s32 chip_interrupt_run(td_void)
{
    td_s32 ret;

    g_icm20690_dev->enable_kthread = TD_TRUE;
    ret = osal_init_work(&g_icm20690_dev->work, icm20690_work);
    if (ret != TD_SUCCESS) {
        return ret;
    }

#ifndef __LITEOS__
    g_icm20690_dev->irq_num = gpio_to_irq(gpio_num(INT_GPIO_CHIP, INT_GPIO_OFFSET));
    ret = request_threaded_irq(g_icm20690_dev->irq_num, TD_NULL,
        icm20690_irq, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, "MotionSensor", g_icm20690_dev->client);
#else
    g_group_bit_info.groupnumber = g_icm20690_dev->gd.group_num;
    g_group_bit_info.bitnumber = g_icm20690_dev->gd.bit_num;
    g_group_bit_info.direction = GPIO_DIR_IN;
    g_group_bit_info.irq_type = IRQ_TYPE_EDGE_FALLING;
    g_group_bit_info.irq_handler = icm20690_irq;
    g_group_bit_info.irq_enable = GPIO_IRQ_ENABLE;
    g_group_bit_info.data = &g_msensor_mode;

    print_info("gpio:%d_%d\n", g_group_bit_info.groupnumber, g_group_bit_info.bitnumber);
    ret = gpio_direction_input(&g_group_bit_info);
    ret |= gpio_irq_register(&g_group_bit_info);
    ret |= gpio_set_irq_type(&g_group_bit_info);
    ret |= gpio_irq_enable(&g_group_bit_info);
#endif
    if (ret != TD_SUCCESS) {
        print_info("request irq failed: %d\n", ret);
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 chip_interrupt_stop(td_void)
{
#ifndef __LITEOS__
    if (g_icm20690_dev->irq_num != 0) {
        free_irq(g_icm20690_dev->irq_num, g_icm20690_dev->client);
        g_icm20690_dev->enable_kthread = TD_FALSE;
#else
    if (g_group_bit_info.irq_enable != 0) {
        g_group_bit_info.irq_enable = GPIO_IRQ_DISABLE;
        gpio_irq_enable(&g_group_bit_info);
        g_icm20690_dev->enable_kthread = TD_FALSE;
#endif
    } else {
        print_info("irq has already free!!\n");
    }

    osal_destroy_work(&g_icm20690_dev->work);
    return TD_SUCCESS;
}

/* timer */
static td_s32 icm20690_wait_stop_working_call_back(const td_void *param)
{
    ot_unused(param);

    if (g_icm20690_dev->workqueue_call_times == 0) {
        return 1;
    }
    return 0;
}

static td_void icm20690_timer_start(td_void)
{
#ifndef __LITEOS__
    ktime_t stime;
    stime = ktime_set(0, g_icm20690_dev->triger_data.triger_info.timer_config.interval * NSEC_PER_USEC);
#else
    union ktime stime;

    stime.tv.sec = 0;
    stime.tv.usec = g_icm20690_dev->triger_data.triger_info.timer_config.interval;
#endif
    hrtimer_start(&g_icm20690_dev->hrtimer, stime, HRTIMER_MODE_REL);
}

static enum hrtimer_restart timer_hr_interrupt(struct hrtimer *timer)
{
    td_s32 ret;

#ifndef __LITEOS__
    ktime_t stime;
    stime = ktime_set(0, g_icm20690_dev->triger_data.triger_info.timer_config.interval * NSEC_PER_USEC);
    hrtimer_forward_now(timer, stime);
#endif

    ret = osal_schedule_work(&g_icm20690_dev->work);
    if (ret == TD_FALSE) {
        return TD_FAILURE;
    }
    g_icm20690_dev->workqueue_call_times++;
    return HRTIMER_RESTART;
}

static td_s32 icm20690_timer_init(td_void)
{
#ifdef __LITEOS__
    td_s32 ret;
    union ktime time;

    time.tv.sec = 0;

    time.tv.usec = g_icm20690_dev->triger_data.triger_info.timer_config.interval;

    ret = hrtimer_create(&g_icm20690_dev->hrtimer, time, timer_hr_interrupt);
    if (ret != TD_SUCCESS) {
        print_info("create tiemr failed!(%d)\n", ret);
    }

#else
    hrtimer_init(&g_icm20690_dev->hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    g_icm20690_dev->hrtimer.function = timer_hr_interrupt;
#endif
    return TD_SUCCESS;
}

static td_s32 icm20690_timer_deinit(td_void)
{
    hrtimer_cancel(&g_icm20690_dev->hrtimer);
    return TD_SUCCESS;
}

td_s32 chip_timer_run(td_void)
{
    td_s32 ret;
    g_icm20690_dev->enable_kthread = TD_TRUE;
    ret = icm20690_timer_init();
    if (ret != TD_SUCCESS) {
        print_info("motionsensor timer init failed\n");
        return TD_FAILURE;
    }
    osal_init_work(&g_icm20690_dev->work, icm20690_work);
    g_time_cnt = 0;
    g_data_count = 0;
    g_timer_frist_flag = TD_FALSE;
    g_icm20690_dev->record_num = 0;

    icm20690_timer_start();
    return TD_SUCCESS;
}

td_s32 chip_timer_stop(td_void)
{
    td_ulong flags;
    td_s32 ret;

    ret = icm20690_timer_deinit();
    if (ret != TD_SUCCESS) {
        print_info("motionsensor timer deinit failed\n");
        return TD_FAILURE;
    }

    osal_wait_event_uninterruptible(&g_icm20690_dev->wait_call_stop_working,
        icm20690_wait_stop_working_call_back, TD_NULL);

    osal_spin_lock_irqsave(&g_lock_get_data, &flags);
    g_icm20690_dev->enable_kthread = TD_FALSE;
    osal_destroy_work(&g_icm20690_dev->work);
    osal_spin_unlock_irqrestore(&g_lock_get_data, &flags);
    (td_void)memset_s(g_time_backup, sizeof(g_time_backup), 0, sizeof(g_time_backup));
    (td_void)memset_s(g_every_data_cnt_backup, sizeof(g_every_data_cnt_backup), 0, sizeof(g_every_data_cnt_backup));
    return TD_SUCCESS;
}

/* user init */
static td_s32 icm20690_set_gyro_param(ot_msensor_param *msensor_param)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 sample_rate, value;

    /* set gyro_odr_config */
    value = msensor_param->config.gyro_config.odr / OT_MSENSOR_GRADIENT;
    sample_rate = value;

    if (value == 0) {
        return TD_FAILURE;
    }

    if ((sample_rate != GYRO_OUTPUT_DATA_RATE_32KHZ) && (sample_rate != GYRO_OUTPUT_DATA_RATE_8KHZ)) {
        if (((1000 % value) != 0) || (value > 1000)) { /* odr 1000 */
            value = GYRO_OUTPUT_DATA_RATE_BUTT;
        } else {
            value = GYRO_OUTPUT_DATA_RATE_UNDER_1KHZ;
        }
    }

    switch (value) {
        case GYRO_OUTPUT_DATA_RATE_UNDER_1KHZ:
        case GYRO_OUTPUT_DATA_RATE_32KHZ:
        case GYRO_OUTPUT_DATA_RATE_8KHZ:
            msensor_param->config.gyro_config.odr = sample_rate;
            break;

        case GYRO_OUTPUT_DATA_RATE_BUTT:
            print_info("not support gyro ODR!\n");
            ret = TD_FAILURE;
            break;

        default:
            print_info("out of gyro range!!!\n");
            ret = TD_FAILURE;
            break;
    }

    /* set gyro valid data bit */
    msensor_param->config.gyro_config.data_width = ICM20690_VALID_DATA_BIT;
    return ret;
}

static td_s32 icm20690_set_acc_param(ot_msensor_param *msensor_param)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 sample_rate, value;

    /* set accel_odr_config */
    value = msensor_param->config.acc_config.odr / OT_MSENSOR_GRADIENT;
    sample_rate = value;

    if (value == 0) {
        return TD_FAILURE;
    }

    if ((sample_rate != ACCEL_OUTPUT_DATA_RATE_4KHZ) && (sample_rate != ACCEL_OUTPUT_DATA_RATE_1KHZ)) {
        if (((1000 % value) != 0) || (value > 4000)) { /* odr 1000 4000 */
            value = ACCEL_OUTPUT_DATA_RATE_BUTT;
        } else {
            value = ACCEL_OUTPUT_DATA_RATE_UNDER_1KHZ;
        }
    }

    switch (value) {
        case ACCEL_OUTPUT_DATA_RATE_UNDER_1KHZ:
        case ACCEL_OUTPUT_DATA_RATE_4KHZ:
        case ACCEL_OUTPUT_DATA_RATE_1KHZ:
            msensor_param->config.acc_config.odr = sample_rate;
            break;

        default:
            print_info("not support ACCEL ODR!\n");
            ret = TD_FAILURE;
            break;
    }
    /* set accel valid data bit */
    msensor_param->config.acc_config.data_width = ICM20690_VALID_DATA_BIT;
    return ret;
}

static td_s32 icm20690_set_param(ot_msensor_param *msensor_param)
{
    td_s32 ret;

    if (OT_MSENSOR_DEVICE_GYRO & msensor_param->attr.device_mask) {
        ret = icm20690_set_gyro_param(msensor_param);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    if (OT_MSENSOR_DEVICE_ACC & msensor_param->attr.device_mask) {
        ret = icm20690_set_acc_param(msensor_param);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    ret = memcpy_s(&g_msensor_mode, sizeof(ot_msensor_attr), &msensor_param->attr, sizeof(ot_msensor_attr));
    if (ret != EOK) {
        return ret;
    }

    return ret;
}

static td_s32 icm20690_config_to_param(ot_msensor_param *msensor_param)
{
    td_s32 ret;

    if (msensor_param == TD_NULL) {
        print_info("motion_sensor_status is NULL\n");
        return TD_FAILURE;
    }

    if ((msensor_param->attr.device_mask & OT_MSENSOR_DEVICE_GYRO) ||
        (msensor_param->attr.device_mask & OT_MSENSOR_DEVICE_ACC)) {
        ret = icm20690_set_param(msensor_param);
        if (ret != TD_SUCCESS) {
            print_info("icm20690 set param failed! ret=%x\n", ret);
        }
    } else {
        print_info("not support mode!\n");
        ret = TD_FAILURE;
    }
    return ret;
}

static td_s32 icm20690_gyro_param_init(ot_msensor_param *msensor_param)
{
    td_s32 ret = TD_SUCCESS;

    /* gyro attr set */
    if ((OT_MSENSOR_DEVICE_GYRO & msensor_param->attr.device_mask) == 0) {
        return ret;
    }

    if (msensor_param->config.gyro_config.odr == GYRO_OUTPUT_DATA_RATE_32KHZ) {
        /* default :GYRO_BAND_WIDTH_3600HZ */
        g_icm20690_dev->gyro_status.band_width = GYRO_BAND_WIDTH_3600HZ;
        g_icm20690_dev->gyro_status.gyro_config.odr = msensor_param->config.gyro_config.odr;
    } else if (msensor_param->config.gyro_config.odr == GYRO_OUTPUT_DATA_RATE_8KHZ) {
        /* default :GYRO_BAND_WIDTH_250HZ */
        g_icm20690_dev->gyro_status.band_width = GYRO_BAND_WIDTH_250HZ;
        g_icm20690_dev->gyro_status.gyro_config.odr = msensor_param->config.gyro_config.odr;
    } else if (msensor_param->config.gyro_config.odr <= 100) { /* 100 odr */
        g_icm20690_dev->gyro_status.band_width = GYRO_BAND_WIDTH_20HZ;
        g_icm20690_dev->gyro_status.gyro_config.odr = msensor_param->config.gyro_config.odr;
    } else if (msensor_param->config.gyro_config.odr <= 300) { /* 300 odr */
        g_icm20690_dev->gyro_status.band_width = GYRO_BAND_WIDTH_41HZ;
        g_icm20690_dev->gyro_status.gyro_config.odr = msensor_param->config.gyro_config.odr;
    } else if (msensor_param->config.gyro_config.odr <= 500) { /* 500 odr */
        g_icm20690_dev->gyro_status.band_width = GYRO_BAND_WIDTH_92HZ;
        g_icm20690_dev->gyro_status.gyro_config.odr = msensor_param->config.gyro_config.odr;
    } else if (msensor_param->config.gyro_config.odr <= 1000) { /* 1000 odr */
        g_icm20690_dev->gyro_status.band_width = GYRO_BAND_WIDTH_184HZ;
        g_icm20690_dev->gyro_status.gyro_config.odr = msensor_param->config.gyro_config.odr;
    } else {
        print_info("icm20690 param init failed! gyro_odr:%d not found !\n",
                   msensor_param->config.gyro_config.odr);
        ret = TD_FAILURE;
    }

    switch (msensor_param->config.gyro_config.fsr) {
        case GYRO_FULL_SCALE_SET_2KDPS:
        case GYRO_FULL_SCALE_SET_1KDPS:
        case GYRO_FULL_SCALE_SET_500DPS:
        case GYRO_FULL_SCALE_SET_250DPS:
        case GYRO_FULL_SCALE_SET_31DPS:
        case GYRO_FULL_SCALE_SET_62DPS:
        case GYRO_FULL_SCALE_SET_125DPS:
            g_icm20690_dev->gyro_status.gyro_config.fsr = msensor_param->config.gyro_config.fsr;
            break;

        default:
            print_info("icm20690 param init failed! gyro_fsr:%d not found !\n",
                       msensor_param->config.gyro_config.fsr);
            ret = TD_FAILURE;
            break;
    }

    return ret;
}

static td_s32 icm20690_acc_param_init(ot_msensor_param *msensor_param)
{
    td_s32 ret = TD_SUCCESS;

    /* accel attr set */
    if ((OT_MSENSOR_DEVICE_ACC & msensor_param->attr.device_mask) == 0) {
        return ret;
    }

    if (msensor_param->config.acc_config.odr == ACCEL_OUTPUT_DATA_RATE_4KHZ) {
        /* default :ACCEL_BAND_WIDTH_1046HZ */
        g_icm20690_dev->acc_status.band_width = ACCEL_BAND_WIDTH_1046HZ;
        g_icm20690_dev->acc_status.acc_config.odr = msensor_param->config.acc_config.odr;
    } else if (msensor_param->config.acc_config.odr == ACCEL_OUTPUT_DATA_RATE_1KHZ) {
        /* default :ACCEL_BAND_WIDTH_420HZ */
        g_icm20690_dev->acc_status.band_width = ACCEL_BAND_WIDTH_420HZ;
        g_icm20690_dev->acc_status.acc_config.odr = msensor_param->config.acc_config.odr;
    } else if (msensor_param->config.acc_config.odr < 100) { /* 100 odr */
        /* default :ACCEL_BAND_WIDTH_218HZ */
        g_icm20690_dev->acc_status.band_width = ACCEL_BAND_WIDTH_21HZ;
        g_icm20690_dev->acc_status.acc_config.odr = msensor_param->config.acc_config.odr;
    } else if (msensor_param->config.acc_config.odr < 300) { /* 300 odr */
        /* default :ACCEL_BAND_WIDTH_218HZ */
        g_icm20690_dev->acc_status.band_width = ACCEL_BAND_WIDTH_44HZ;
        g_icm20690_dev->acc_status.acc_config.odr = msensor_param->config.acc_config.odr;
    } else if (msensor_param->config.acc_config.odr < 500) { /* 500 odr */
        /* default :ACCEL_BAND_WIDTH_218HZ */
        g_icm20690_dev->acc_status.band_width = ACCEL_BAND_WIDTH_99HZ;
        g_icm20690_dev->acc_status.acc_config.odr = msensor_param->config.acc_config.odr;
    } else if (msensor_param->config.acc_config.odr < 1000) { /* 1000 odr */
        /* default :ACCEL_BAND_WIDTH_218HZ */
        g_icm20690_dev->acc_status.band_width = ACCEL_BAND_WIDTH_218HZ;
        g_icm20690_dev->acc_status.acc_config.odr = msensor_param->config.acc_config.odr;
    } else {
        print_info("icm20690 param init failed! gyro_odr:%d not found !\n",
                   msensor_param->config.acc_config.odr);
        ret = TD_FAILURE;
    }

    switch (msensor_param->config.acc_config.fsr) {
        case ACCEL_UI_FULL_SCALE_SET_2G:
        case ACCEL_UI_FULL_SCALE_SET_4G:
        case ACCEL_UI_FULL_SCALE_SET_8G:
        case ACCEL_UI_FULL_SCALE_SET_16G:
            g_icm20690_dev->acc_status.acc_config.fsr = msensor_param->config.acc_config.fsr;
            break;

        default:
            print_info("icm20690 param init failed! accel_range:%d not found !\n",
                       msensor_param->config.acc_config.fsr);
            ret = TD_FAILURE;
            break;
    }

    return ret;
}

static td_s32 icm20690_param_init(ot_msensor_param msensor_param)
{
    td_s32 ret;

    ret = icm20690_gyro_param_init(&msensor_param);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = icm20690_acc_param_init(&msensor_param);

    print_info("gyro odr:%d,acc odr:%d\n", msensor_param.config.gyro_config.odr,
               msensor_param.config.acc_config.odr);
    return ret;
}

static td_void icm20690_triger_mode_init(ot_msensor_param *msensor_param)
{
    g_icm20690_dev->triger_data.triger_mode = g_msensor_triger_mode;
    g_icm20690_dev->fifo_en = g_msensor_fifo_en;

    if (g_icm20690_dev->triger_data.triger_mode == TRIGER_TIMER) {
        if (msensor_param->config.gyro_config.odr <= 50) {           /* 50 odr */
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 1400000; /* 1400000 : 70000000 / 50 */
        } else if (msensor_param->config.gyro_config.odr <= 200) {   /* 200 odr */
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 350000;  /* 350000 : 70000000 / 200 */
        } else if (msensor_param->config.gyro_config.odr <= 500) {   /* 500 odr */
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 140000;  /* 140000 : 70000000 / 500 */
        } else if (msensor_param->config.gyro_config.odr <= 1000) {  /* 1000 odr */
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 50000;   /* 50000 : 70000000 / 1400 */
        } else if (msensor_param->config.gyro_config.odr == 8000) {  /* 8000 odr */
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 8750;    /* 8750 : 70000000 / 8000 */
        } else if (msensor_param->config.gyro_config.odr == 32000) { /* 32000 odr */
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 2188;    /* 2188 : 70000000 / 32000 */
        } else {
            g_icm20690_dev->triger_data.triger_info.timer_config.interval = 70000;   /* 70000 : 70000000 / 1000 */
        }
    } else if (g_icm20690_dev->triger_data.triger_mode == TRIGER_EXTERN_INTERRUPT) {
        g_icm20690_dev->triger_data.triger_info.extern_interrupt_config.interrupt_num = ICM20690_FIFO_MAX_RECORD;
    }
    return;
}

td_s32 chip_get_triger_config(triger_config *triger_data)
{
    td_s32 ret = TD_SUCCESS;
    triger_data->triger_mode = g_icm20690_dev->triger_data.triger_mode;
    triger_data->triger_info.timer_config.interval =
        g_icm20690_dev->triger_data.triger_info.timer_config.interval;
    return ret;
}

static td_s32 icm20690_dev_init(ot_msensor_param *msensor_param)
{
    td_s32 ret;

    osal_printk("######fun:%s line:%d motion_sensor_status:%p######\n", __func__, __LINE__, msensor_param);

    ret = icm20690_config_to_param(msensor_param);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 config to param failed! ret=%x\n", ret);
        goto err_init;
    }

    osal_printk("######fun:%s line:%d ######\n", __func__, __LINE__);

    ret = icm20690_param_init(*msensor_param);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 param init failed! ret=%x\n", ret);
        goto err_init;
    }

    icm20690_triger_mode_init(msensor_param);
    ret = icm20690_sensor_init(msensor_param->attr.device_mask);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 sensor init failed! ret=%x\n", ret);
        goto err_init;
    }
    return TD_SUCCESS;

err_init:
    return TD_FAILURE;
}

static td_u8 motionsensor_dev_id_read(td_void)
{
    td_s32 ret;
    td_u8 dev_id = 0;

    ret = icm20690_transfer_read(0x75, &dev_id, 1, TD_FALSE);
    if (ret != TD_SUCCESS) {
        print_info("read dev failed\n");
        return -EAGAIN;
    }

    osal_printk("func:%s,dev info :0x%x\n", __func__, dev_id);
    return dev_id;
}

td_s32 chip_dev_init(ot_msensor_param *msensor_param)
{
    td_s32 ret;
    td_u8 dev_id;

    dev_id = motionsensor_dev_id_read();
    if (dev_id != ICM20690_SELFID) {
        print_info("ICM20690 device abnormal!!\n");
        return -ENODEV;
    }
    ret = icm20690_dev_init(msensor_param);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 dev init failed\n");
        return -ENODEV;
    }

    g_timer_frist_flag = TD_FALSE;
    g_time_cnt = 0;
    g_data_count = 0;

    return TD_SUCCESS;
}

/* for send and save yuv only */
td_s32 chip_dev_mng_init(ot_msensor_param *msensor_param)
{
    td_s32 ret;

    osal_printk("######fun:%s line:%d motion_sensor_status:%p######\n", __func__, __LINE__, msensor_param);

    if (msensor_param == TD_NULL) {
        return TD_FAILURE;
    }

    ret = icm20690_config_to_param(msensor_param);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 config to param failed! ret=%x\n", ret);
        return TD_FAILURE;
    }

    osal_printk("######fun:%s line:%d ######\n", __func__, __LINE__);

    ret = icm20690_param_init(*msensor_param);
    if (ret != TD_SUCCESS) {
        print_info("icm20690 param init failed! ret=%x\n", ret);
        return TD_FAILURE;
    }

    icm20690_triger_mode_init(msensor_param);

    return TD_SUCCESS;
}

static td_void icm20690_axis_fifo_sensor_deinit(td_void)
{
    safe_kfree(g_icm20690_dev->fifo_buf);
    return;
}

static td_void icm20690_sensor_deinit(td_u32 axis_mode)
{
    if ((axis_mode & OT_MSENSOR_DEVICE_GYRO) || (axis_mode & OT_MSENSOR_DEVICE_ACC)) {
        icm20690_axis_fifo_sensor_deinit();
    } else {
        print_info("icm20690 sensor deinit not support this mode : %u\n", axis_mode);
    }
    return;
}

void chip_dev_deinit(ot_msensor_param *msensor_param)
{
    /* exit may need to be forced out when deinit is not called */
    g_icm20690_dev->record_num = 0;
    icm20690_sensor_deinit(msensor_param->attr.device_mask);
}

#ifdef TRANSFER_I2C
static struct i2c_board_info g_ot_icm20690_info = {
    I2C_BOARD_INFO("ICM20690", ICM20690_DEV_ADDR),
};
#endif

/* module init */
td_s32 chip_sensor_init(td_void)
{
    /* 1.malloc a ICM20690 dev */
    g_icm20690_dev = osal_kmalloc(sizeof(icm20690_dev_info), osal_gfp_kernel);
    if (g_icm20690_dev == TD_NULL) {
        print_info("could not allocate memory\n");
        return -ENOMEM;
    }

    (td_void)memset_s(g_icm20690_dev, sizeof(icm20690_dev_info), 0, sizeof(icm20690_dev_info));

#ifndef __LITEOS__
    gpio_init();
#else
    if (motionsersor_spi_init() != TD_SUCCESS) {
        print_info("spidev_init failed\n");
        safe_kfree(g_icm20690_dev);
        return TD_FAILURE;
    }

    gpio_init(&g_icm20690_dev->gd);
#endif

    osal_spin_lock_init(&g_lock_get_data);
    osal_wait_init(&g_icm20690_dev->wait_call_stop_working);

    return TD_SUCCESS;
}

void chip_sensor_deinit(ot_msensor_param *msensor_param)
{
    ot_unused(msensor_param);
    osal_wait_destroy(&g_icm20690_dev->wait_call_stop_working);
#ifndef __LITEOS__
    gpio_deinit();
#else
    gpio_deinit(&g_icm20690_dev->gd);
    motionsersor_spi_deinit();
#endif

    osal_spin_lock_destroy(&g_lock_get_data);
    safe_kfree(g_icm20690_dev);
}
