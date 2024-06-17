/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ICM20690_H
#define ICM20690_H

#include "motionsensor.h"
#include "ot_osal.h"
#include "linux/workqueue.h"
#include "linux/hrtimer.h"

#ifdef __LITEOS__
#include "gpio.h"
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#define ICM20690_DEV_ADDR 0x68
#define I2C_DEV_NUM       0x1

#define ICM20690_VALID_DATA_BIT 16

#define div_0_to_1(a) (((a) == 0) ? 1 : (a))

#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40

#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48

#define ICM20690_SLEEP_MODE           0x00
#define ICM20690_STANDBY_MODE         0x01
#define ICM20690_ACCEL_LOWPOWER_MODE  0x02
#define ICM20690_ACCEL_LOWNOISE_MODE  0x03
#define ICM20690_GYRO_LOWPOWER_MODE   0x04
#define ICM20690_GYRO_LOWNOISE_MODE   0x05
#define ICM20690_6_AXIS_LOWPOWER_MODE 0x06
#define ICM20690_6_AXIS_LOWNOISE_MODE 0x07

#define ODR_1KHZ        0x0

#define TRUE_REGISTER_VALUE  0x01
#define FALSE_REGISTER_VALUE 0x00

#define USER_CONTROL_REGISTER_ADDR       0x6A
#define POWER_MANAGEMENT_REGISTER_1_ADDR 0x6B
#define POWER_MANAGEMENT_REGISTER_2_ADDR 0x6C
#define CONFIGURATION_REGISTER_ADDR      0x1A
#define GYRO_CONFIG_REGISTER_ADDR        0x1B
#define ACCEL_CONFIG_REGISTER_1_ADDR     0x1C
#define ACCEL_CONFIG_REGISTER_2_ADDR     0x1D
#define LP_MODE_CONFIG_REGISTER_ADDR     0x1E

#define DEV_TEMPERATURE_LSB_ADDR    0x41

#define SMPLRT_DIV                  0x19
#define FIFO_ENABLE_REGISTER_ADDR   0x23
#define FIFO_WATERMARK_THRESHOLD    0x61


#define RESET_OFFSET    0x07
#define CLKSET_OFFSET   0x02
#define CLKSET_VALUE    0x01

#define FIFO_COUNTH     0x72
#define FIFO_COUNTL     0x73
#define FIFO_R_W        0x74

#define INT_PIN_CONFIGURATION 0x37
#define INTERRUPT_ENABLE      0x38
#define FIFO_WM_INT_STATUS    0x39
#define INT_STATUS            0x3A

#define ICM20690_ACCEL_UI_FULL_SCALE_SET_2G  0x00
#define ICM20690_ACCEL_UI_FULL_SCALE_SET_4G  0x01
#define ICM20690_ACCEL_UI_FULL_SCALE_SET_8G  0x02
#define ICM20690_ACCEL_UI_FULL_SCALE_SET_16G 0x03

#define ICM20690_GYRO_FULL_SCALE_SET_250DPS  0x00
#define ICM20690_GYRO_FULL_SCALE_SET_500DPS  0x01
#define ICM20690_GYRO_FULL_SCALE_SET_1000DPS 0x02
#define ICM20690_GYRO_FULL_SCALE_SET_2000DPS 0x03
#define ICM20690_GYRO_FULL_SCALE_SET_31DPS   0x05
#define ICM20690_GYRO_FULL_SCALE_SET_62DPS   0x06
#define ICM20690_GYRO_FULL_SCALE_SET_125DPS  0x07

#define ICM20690_SELFID          0x20
#define ICM20690_FIFO_MAX_RECORD 140
#define ICM20690_FIFO_R_MAX_SIZE 512
#define FIFO_FRAME_CNT           85

#define ROOMTEMP_OFFSET  (25 * (0x1 << 10))
#define GRADIENT_TEMP    (10 * (0x1 << 10))
#define TEMP_SENSITIVITY    3268

/* icm20690 interrupt pin reg */
#define INT_GPIO_CHIP       7
#define INT_GPIO_OFFSET     2

#define TIME_RECORD_CNT     10
#define DATA_RECORD_CNT (TIME_RECORD_CNT)

typedef struct {
    ot_msensor_gyro_config gyro_config;
    td_u32 band_width;
    td_u64 last_pts;
} msensor_gyro_status;

typedef struct {
    ot_msensor_acc_config acc_config;
    td_u32 band_width;
    td_u64 last_pts;
} msensor_acc_status;

typedef struct {
    ot_msensor_magn_config magn_config;
    td_u32 band_width;
    td_u64 last_pts;
} msensor_magn_status;

typedef struct {
    ot_msensor_sample_data accel_cur_data;
    ot_msensor_sample_data gyro_cur_data;
    ot_msensor_sample_data magn_data;
#ifndef __LITEOS__
    struct i2c_client *client;
    struct spi_device *ot_spi;
    struct task_struct *read_data_task;
#else
    struct gpio_descriptor gd;
    osal_task_t *read_data_task;
#endif
    struct task_struct *get_data_kthread;
    struct osal_work_struct work;
    td_u8 power_mode;
    td_u8 flag_acc_fifo_enabled;
    td_u8 flag_gyro_fifo_enabled;
    td_u8 flag_fifo_incomming;
    td_u8 *fifo_buf;
    td_u32 fifo_length;
    td_u8 record_num;
    td_u8 enable_kthread;
    td_u8 fifo_en;
    triger_config triger_data;
    struct hrtimer hrtimer;
    td_u8 thread_wakeup;
    osal_semaphore_t g_sem;
    msensor_acc_status acc_status;
    msensor_gyro_status gyro_status;
    msensor_magn_status magn_status;
    td_s32 temperature;
    td_s32 irq_num;
    td_s32 workqueue_call_times;
    osal_wait_t wait_call_stop_working;
} icm20690_dev_info;

icm20690_dev_info *chip_get_dev_info(td_void);
td_s32 chip_fifo_data_reset(td_void);
td_s32 chip_get_triger_config(triger_config *triger_data);

td_s32 chip_dev_init(ot_msensor_param *msensor_param);
td_s32 chip_dev_mng_init(ot_msensor_param *msensor_param);
td_void chip_dev_deinit(ot_msensor_param *msensor_param);

td_s32 chip_sensor_init(td_void);
td_void chip_sensor_deinit(ot_msensor_param *msensor_param);
td_s32 chip_timer_run(td_void);
td_s32 chip_interrupt_run(td_void);
td_s32 chip_timer_stop(td_void);
td_s32 chip_interrupt_stop(td_void);
td_s32 chip_get_data_for_one_frm(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif

