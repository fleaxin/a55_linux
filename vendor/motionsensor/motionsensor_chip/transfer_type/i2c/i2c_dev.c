/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

/* include <liteos/i2c.h> */
#include "i2c_dev.h"
#include "linux/i2c.h"
#include "motionsensor.h"

struct i2c_client g_i2c_client_obj; /* i2c control struct */
#define SLAVE_ADDR     0x34         /* i2c dev addr */
#define SLAVE_REG_ADDR 0x300f       /* i2c dev register */

/* client initial */
static td_s32 i2c_client_init(struct i2c_client **ot_i2c_client)
{
    td_s32 ret;

    /* struct i2c_client * i2c_client0 is &g_i2c_client_obj */
    /* i2c_client0->addr is SLAVE_ADDR >> 1 */
    ot_i2c_client->addr = ICM20690_DEV_ADDR;
    ret = client_attach(*ot_i2c_client, 0);
    if (ret != TD_SUCCESS) {
        dprintf("fail to attach client!\n");
        return -1;
    }
    return 0;
}

static td_s32 sample_i2c_write(struct i2c_client *ot_i2c_client, td_u8 reg_addr, const td_u8 *reg_data, td_u32 cnt)
{
    td_s32 ret;

    /* struct i2c_client * i2c_client0 is & g_i2c_client_obj */
    td_char buf[4] = { 0 }; /* 4 buf */
    /* i2c_client_init */
    buf[0] = reg_addr & 0xff;
    ret = memcpy_s(&buf[1], cnt, reg_data, cnt);
    if (ret != EOK) {
        return ret;
    }
    /* buf[1] is (SLAVE_REG_ADDR >> 8) & 0xff */
    /* buf[2] is 0x03   write value to i2c */
    /* call I2C standard function drv to write */
    ret = i2c_master_send(ot_i2c_client, &buf, cnt + 1);
    return ret;
}

static td_s32 sample_i2c_read(struct i2c_client *ot_i2c_client, td_u8 reg_addr, td_u8 *reg_data, td_u32 cnt)
{
    td_s32 ret = TD_SUCCESS;

    /* struct i2c_client *i2c_client0 is & g_i2c_client_obj */
    struct i2c_rdwr_ioctl_data rdwr;
    struct i2c_msg msg[2]; /* 2 msg */
    td_u8 recvbuf[4]; /* 4 bytes */
    (td_void)memset_s(recvbuf, sizeof(recvbuf), 0x0, sizeof(recvbuf));
    /* i2c_client_init */
    msg[0].addr = ot_i2c_client->addr;
    msg[0].flags = ot_i2c_client->flags & I2C_M_TEN;
    msg[0].len = 1;
    msg[0].buf = reg_addr;
    msg[1].addr = ot_i2c_client->addr;
    msg[1].flags = ot_i2c_client->flags & I2C_M_TEN;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = cnt;
    msg[1].buf = reg_data;
    /* rdwr.msgs = &msg[0] */
    rdwr.nmsgs = 2; /* 2 msg */
    recvbuf[0] = SLAVE_REG_ADDR & 0xff;
    recvbuf[1] = (SLAVE_REG_ADDR >> 8) & 0xff; /* 8 bits */
    i2c_transfer(ot_i2c_client->adapter, msg, rdwr.nmsgs);
    /* dprintf("val is 0x%x\n",recvbuf[0])  buf[0] save the value read from i2c dev */
    return ret;
}

td_u8 motionsersor_i2c_write(struct i2c_client *ot_i2c_client,
                             td_u8 reg_addr, const td_u8 *reg_data, td_u32 cnt)
{
    return sample_i2c_write(ot_i2c_client, reg_addr, reg_data, cnt);
}

td_u8 motionsersor_i2c_read(struct i2c_client *ot_i2c_client,
                            td_u8 reg_addr, td_u8 *reg_data, td_u32 cnt)
{
    return sample_i2c_read(ot_i2c_client, reg_addr, reg_data, cnt);
}
