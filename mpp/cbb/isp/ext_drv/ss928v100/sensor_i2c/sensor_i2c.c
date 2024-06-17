/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/i2c.h>

#ifdef __LITEOS__
#include <i2c.h>
#include "fcntl.h"
#endif
#include <linux/delay.h>
#include "isp_ext.h"
#include "securec.h"

#define I2C_MAX_NUM 6
#define ISP_MAX_DEV 12
#ifndef __LITEOS__
static struct i2c_board_info g_ot_info = {
    I2C_BOARD_INFO("sensor_i2c", (0x6c >> 1)),
};
#endif

static struct i2c_client *g_sensor_client[I2C_MAX_NUM] = { TD_NULL };

#ifdef __LITEOS__
static int ot_sensor_i2c_write(unsigned char i2c_dev, unsigned char dev_addr, unsigned int reg_addr,
                               unsigned int reg_addr_num, unsigned int data, unsigned int data_byte_num)
{
    unsigned char tmp_buf[8]; /* tmp buf size 8 */
    int ret;
    int idx = 0;
    struct i2c_client client;
    td_u32 tries = 0;

    if (i2c_dev >= I2C_MAX_NUM) {
        return TD_FAILURE;
    }

    if (g_sensor_client[i2c_dev] == TD_NULL) {
        return TD_FAILURE;
    }

    (td_void)memcpy_s(&client, sizeof(struct i2c_client), g_sensor_client[i2c_dev], sizeof(struct i2c_client));

    client.addr = ((dev_addr >> 1) & 0xff);

    /* reg_addr config */
    if (reg_addr_num == 1) {
        client.flags &= ~I2C_M_16BIT_REG;
        tmp_buf[idx++] = reg_addr & 0xff;
    } else {
        client.flags |= I2C_M_16BIT_REG;
        tmp_buf[idx++] = (reg_addr >> 8) & 0xff; /* reg_addr shift by 8 */
        tmp_buf[idx++] = reg_addr & 0xff;
    }

    /* data config */
    if (data_byte_num == 1) {
        client.flags &= ~I2C_M_16BIT_DATA;
        tmp_buf[idx++] = data & 0xff;
    } else {
        client.flags |= I2C_M_16BIT_DATA;
        tmp_buf[idx++] = (data >> 8) & 0xff; /* reg_addr shift by 8 */
        tmp_buf[idx++] = data & 0xff;
    }

    while (1) {
        ret = ot_i2c_master_send(&client, (const char *)tmp_buf, idx);
        if (ret == idx) {
            break;
        } else if ((ret == -EAGAIN)) {
            tries++;
            if (tries > 5) { /* threshold 5 */
                return TD_FAILURE;
            }
        } else {
            osal_printk("[%s %d] ot_i2c_master_send error, ret=%d. \n", __func__, __LINE__, ret);
            return ret;
        }
    }

    return 0;
}
#else
#ifndef ISP_WRITE_I2C_THROUGH_MUL_REG
static int ot_sensor_i2c_write(unsigned char i2c_dev, unsigned char dev_addr, unsigned int reg_addr,
                               unsigned int reg_addr_num, unsigned int data, unsigned int data_byte_num)
{
    unsigned char tmp_buf[8]; /* tmp buf size 8 */
    int ret;
    int idx = 0;
    struct i2c_client client;
    td_u32 tries = 0;

    if (i2c_dev >= I2C_MAX_NUM) {
        return TD_FAILURE;
    }

    if (g_sensor_client[i2c_dev] == TD_NULL) {
        return TD_FAILURE;
    }

    (td_void)memcpy_s(&client, sizeof(struct i2c_client), g_sensor_client[i2c_dev], sizeof(struct i2c_client));

    client.addr = (dev_addr >> 1);

    /* reg_addr config */
    if (reg_addr_num == 1) {
        tmp_buf[idx++] = reg_addr & 0xff;
    } else {
        tmp_buf[idx++] = (reg_addr >> 8) & 0xff; /* reg_addr shift by 8 */
        tmp_buf[idx++] = reg_addr & 0xff;
    }

    /* data config */
    if (data_byte_num == 1) {
        tmp_buf[idx++] = data & 0xff;
    } else {
        tmp_buf[idx++] = (data >> 8) & 0xff; /* reg_addr shift by 8 */
        tmp_buf[idx++] = data & 0xff;
    }

    while (1) {
        ret = ot_i2c_master_send(&client, (const char *)tmp_buf, idx);
        if (ret == idx) {
            break;
        } else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled())) {
            tries++;
            if (tries > 5) { /* threshold 5 */
                return TD_FAILURE;
            }
        } else {
            osal_printk("[%s %d] ot_i2c_master_send error, ret=%d. \n", __func__, __LINE__, ret);
            return ret;
        }
    }

    return 0;
}
#else
static int ot_sensor_i2c_write(unsigned char i2c_dev, unsigned char dev_addr, signed char* data_buf,
    unsigned char data_buf_len, unsigned char data_len, unsigned int reg_addr_num, unsigned int data_byte_num)
{
    int ret;
    unsigned char tries = 0;
    unsigned char byte_num;
    struct i2c_client client;

    if (data_buf == TD_NULL) {
        return TD_FAILURE;
    }
    if (data_len >= data_buf_len) {
        return TD_FAILURE;
    }
    if (i2c_dev >= I2C_MAX_NUM) {
        return TD_FAILURE;
    }

    if (g_sensor_client[i2c_dev] == TD_NULL) {
        return TD_FAILURE;
    }

    (td_void)memcpy_s(&client, sizeof(struct i2c_client), g_sensor_client[i2c_dev], sizeof(struct i2c_client));

    client.addr = (dev_addr >> 1);
    byte_num = reg_addr_num + data_byte_num;
    while (1) {
        ret = bsp_i2c_master_send_mul_reg(&client, (const char *)data_buf, data_len, byte_num);
        if (ret == data_len) {
            break;
        } else if ((ret == -EAGAIN) && (in_atomic() || irqs_disabled())) {
            tries++;
            if (tries > 5) { /* threshold 5 */
                return TD_FAILURE;
            }
        } else {
            osal_printk("[%s %d] bsp_i2c_master_send_mul_reg error, ret=%d. \n", __func__, __LINE__, ret);
            return ret;
        }
    }
    return 0;
}
#endif
#endif

static int ot_dev_isp_register(void)
{
    int i;
    isp_bus_callback bus_cb = {0};

    bus_cb.pfn_isp_write_i2c_data = ot_sensor_i2c_write;
    if (ckfn_isp() && (ckfn_isp_register_bus_callback())) {
        for (i = 0; i < ISP_MAX_DEV; i++) {
            call_isp_register_bus_callback(i, ISP_BUS_TYPE_I2C, &bus_cb);
        }
    } else {
        osal_printk("register i2c_write_callback to isp failed, ot_i2c init is failed!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

#ifdef __LITEOS__
static struct i2c_client g_sensor_client_obj[I2C_MAX_NUM];
static struct i2c_client *ot_sensor_i2c_client_init(int i2c_dev)
{
    int ret;
    struct i2c_client *i2c_client0 = &g_sensor_client_obj[i2c_dev];
    i2c_client0->addr = 0x6c >> 1;
    ret = client_attach(i2c_client0, i2c_dev);
    if (ret) {
        dprintf("Fail to attach client!\n");
        return TD_NULL;
    }
    return &g_sensor_client_obj[i2c_dev];
}

int ot_dev_init(void)
{
    int i;
    int ret;

    for (i = 0; i < I2C_MAX_NUM; i++) {
        g_sensor_client[i] = ot_sensor_i2c_client_init(i);
    }

    ret = ot_dev_isp_register();
    if (ret != TD_SUCCESS) {
        printf("ot_dev_init failed !\n");
        return -1;
    }

    return 0;
}

void ot_dev_exit(void)
{
}

#else
static int __init ot_dev_init(void)
{
    int i;
    struct i2c_adapter *i2c_adap = TD_NULL;

    for (i = 0; i < I2C_MAX_NUM; i++) {
        i2c_adap = i2c_get_adapter(i);
        if (i2c_adap != TD_NULL) {
            g_sensor_client[i] = i2c_new_device(i2c_adap, &g_ot_info);

            i2c_put_adapter(i2c_adap);
        } else {
            osal_printk("i2c:%d get adapter error!\n", i);
        }
    }

    ot_dev_isp_register();

    return 0;
}
static void __exit ot_dev_exit(void)
{
    int i;

    for (i = 0; i < I2C_MAX_NUM; i++) {
        if (g_sensor_client[i] != TD_NULL) {
            i2c_unregister_device(g_sensor_client[i]);
        }
    }
}

#ifndef __LITEOS__
module_init(ot_dev_init);
#else
module_init(((void *)ot_dev_init));
#endif
module_exit(ot_dev_exit);
MODULE_LICENSE("GPL");

#endif
