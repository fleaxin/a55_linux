/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "motionsensor.h"
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>

#include "sys_ext.h"
#include "dev_ext.h"
#include "securec.h"

#include "motionsensor_ext.h"
#include "ot_motionsensor_chip_cmd.h"
#include "ot_common_motionsensor.h"
#include "icm20690.h"
#if (defined CONFIG_OT_PROC_SHOW_SUPPORT) && (defined ICM20690_PARAM_PROC)
#include "motionsensor_chip_proc.h"
#endif

static osal_dev_t *g_msensor_chip_dev = TD_NULL;

#define MNGBUFF_ENABLE

static td_s32 g_motionsensor_init;
static td_bool g_motionsensor_start = TD_FALSE;
ot_msensor_param *g_motionsensor_param = TD_NULL;
static triger_config g_triger_data = { 0 };

#define safe_kfree(memory) do { \
    if ((memory) != TD_NULL) { \
        osal_kfree(memory); \
        memory = TD_NULL; \
    } \
} while (0)

ot_msensor_param *msensor_chip_get_param(td_void)
{
    return g_motionsensor_param;
}

#ifdef MNGBUFF_ENABLE
td_s32 msensor_chip_int_callback(ot_msensor_data *msensor_data)
{
    td_s32 ret = TD_SUCCESS;
    msensor_mng_export_func *pfn_motionsensor_mng_exp_func = TD_NULL;

    if (msensor_data == TD_NULL) {
        print_info("motionsensor_chip_data is NULL!(ret:0x%x)\n", ret);
        return ret;
    }

    pfn_motionsensor_mng_exp_func = func_entry(msensor_mng_export_func, OT_ID_MOTIONSENSOR);
    if (pfn_motionsensor_mng_exp_func == TD_NULL) {
        print_info("pfn_motionsensor_mng_exp_func is NULL!(ret:0x%x)\n", ret);
        return ret;
    }

    ret = pfn_motionsensor_mng_exp_func->pfn_chip_write_data_to_mng_buff(msensor_data);
    if (ret != TD_SUCCESS) {
        print_info("pfn_chip_write_data_to_mng_buff failed!(ret:0x%x)\n", ret);
        return ret;
    }
    return ret;
}

static td_s32 msensor_chip_init_mng_buf(ot_msensor_attr msensor_attr,
    ot_msensor_buf_attr *msensor_buf_attr, ot_msensor_config *msensor_config)
{
    td_s32 ret = TD_SUCCESS;
    msensor_mng_export_func *pfn_motionsensor_mng_exp_func = TD_NULL;

    if ((msensor_buf_attr == TD_NULL) || (msensor_config == TD_NULL)) {
        print_info("init mng buff failed!(ret:0x%x)\n", ret);
        return ret;
    }

    pfn_motionsensor_mng_exp_func = func_entry(msensor_mng_export_func, OT_ID_MOTIONSENSOR);
    if ((pfn_motionsensor_mng_exp_func == TD_NULL) || (pfn_motionsensor_mng_exp_func->pfn_init == TD_NULL)) {
        print_info("init mng buff failed!(pfn_motionsensor_mng_exp_func:0x%p)\n", pfn_motionsensor_mng_exp_func);
        return ret;
    }

    ret = pfn_motionsensor_mng_exp_func->pfn_init(&msensor_attr, msensor_buf_attr, msensor_config);
    if (ret != TD_SUCCESS) {
        print_info("init mng buff failed!(ret:0x%x)\n", ret);
        return ret;
    }
    return ret;
}

static td_s32 msensor_chip_deinit_mng_buf(td_void)
{
    td_s32 ret = TD_SUCCESS;
    msensor_mng_export_func *pfn_motionsensor_mng_exp_func = TD_NULL;

    pfn_motionsensor_mng_exp_func = func_entry(msensor_mng_export_func, OT_ID_MOTIONSENSOR);
    if ((pfn_motionsensor_mng_exp_func == TD_NULL) || (pfn_motionsensor_mng_exp_func->pfn_deinit == TD_NULL)) {
        print_info("pfn_chip_write_data_to_mng_buff failed!(pfn_motionsensor_mng_exp_func:0x%p)\n",
                   pfn_motionsensor_mng_exp_func);
        return ret;
    }

    ret = pfn_motionsensor_mng_exp_func->pfn_deinit();
    if (ret != TD_SUCCESS) {
        print_info("pfn_chip_write_data_to_mng_buff failed!(ret:0x%x)\n", ret);
        return ret;
    }
    return ret;
}

static td_s32 msensor_chip_get_cfg(ot_msensor_param *msensor_param)
{
    td_s32 ret;
    if (g_motionsensor_param == TD_NULL) {
        print_info("motionsensor_param is NULL\n");
        return TD_FAILURE;
    }

    if (msensor_param == TD_NULL) {
        print_info("input param is NULL\n");
        return TD_FAILURE;
    }

    ret = memcpy_s(msensor_param, sizeof(ot_msensor_param), g_motionsensor_param, sizeof(ot_msensor_param));
    if (ret != EOK) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 msensor_chip_get_data(td_void)
{
    td_s32 ret;

    ret = chip_get_data_for_one_frm();
    if (ret != TD_SUCCESS) {
        print_info("[error]not support MODE\n");
    }
    return ret;
}

static td_s32 msensor_chip_register_mng_callback(td_void)
{
    msensor_mng_callback callback = { 0 };

    if (ckfn_sys_entry() == TD_FALSE) {
        print_info("sys is not ready, please check it\n");
        return TD_FAILURE;
    }

    callback.pfn_get_config_from_chip = msensor_chip_get_cfg;
    callback.pfn_write_data_to_buf = msensor_chip_get_data;

    if ((func_entry(msensor_mng_export_func, OT_ID_MOTIONSENSOR) != TD_NULL) &&
        (ckfn_msensor_mng_register_call_back())) {
        call_msensor_mng_register_call_back(&callback);
    } else {
        print_info("register motionsensor callback failed!\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void msensor_chip_unregister_mng_callback(td_void)
{
    if ((func_entry(msensor_mng_export_func, OT_ID_MOTIONSENSOR) != TD_NULL) &&
        (ckfn_msensor_mng_unregister_call_back())) {
        call_msensor_mng_unregister_call_back();
    }
    return;
}
#endif

static td_s32 msensor_chip_open(td_void *private_data)
{
    ot_unused(private_data);
    print_info("motionsensor open\n");
    return TD_SUCCESS;
}

static td_s32 msensor_chip_release(td_void *private_data)
{
    ot_unused(private_data);
    print_info("motionsensor close\n");
    return TD_SUCCESS;
}

/* motionsensor chip ioctl functions */
static td_s32 msensor_chip_drv_user_start(td_uintptr_t arg)
{
    if (g_motionsensor_init == TD_FALSE) {
        print_info("motionsensor is not init!\n");
        return TD_FAILURE;
    }

    if (g_motionsensor_start == TD_TRUE) {
        print_info("motionsensor is already start!\n");
        return TD_SUCCESS;
    }

    ot_unused(arg);

    chip_fifo_data_reset();

    if (g_triger_data.triger_mode == TRIGER_TIMER) {
        chip_timer_run();
    } else if (g_triger_data.triger_mode == TRIGER_EXTERN_INTERRUPT) {
        chip_interrupt_run();
    } else {
        print_info("ERROR triger_mode!!\n");
        return TD_FAILURE;
    }

    g_motionsensor_start = TD_TRUE;

    return TD_SUCCESS;
}

static td_s32 msensor_chip_drv_user_stop(td_uintptr_t arg)
{
    if (g_motionsensor_init == TD_FALSE) {
        print_info("motionsensor is not init!\n");
        return TD_FAILURE;
    }

    if (g_motionsensor_start == TD_FALSE) {
        print_info("motionsensor is already stop!\n");
        return TD_SUCCESS;
    }

    ot_unused(arg);

    if (g_triger_data.triger_mode == TRIGER_TIMER) {
        chip_timer_stop();
    } else if (g_triger_data.triger_mode == TRIGER_EXTERN_INTERRUPT) {
        chip_interrupt_stop();
    } else {
        print_info("ERROR triger_mode!!\n");
        return TD_FAILURE;
    }

    g_motionsensor_start = TD_FALSE;

    return TD_SUCCESS;
}

static td_s32 msensor_chip_drv_user_init(td_uintptr_t arg)
{
    td_s32 ret;

    if (g_motionsensor_init == TD_TRUE) {
        print_info("motionsensor is already inited!\n");
        return TD_FAILURE;
    }

    ret = memcpy_s(g_motionsensor_param, sizeof(ot_msensor_param),
                   (td_void *)arg, sizeof(ot_msensor_param));
    if (ret != EOK) {
        return TD_FAILURE;
    }

    if (((OT_MSENSOR_DEVICE_GYRO & g_motionsensor_param->attr.device_mask) &&
        (g_motionsensor_param->config.gyro_config.odr == 0)) ||
        ((OT_MSENSOR_DEVICE_ACC & g_motionsensor_param->attr.device_mask) &&
        (g_motionsensor_param->config.acc_config.odr == 0)) ||
        ((OT_MSENSOR_DEVICE_MAGN & g_motionsensor_param->attr.device_mask) &&
        (g_motionsensor_param->config.magn_config.odr == 0))) {
        return TD_FAILURE;
    }

    g_motionsensor_param->config.gyro_config.temperature_max = MOTIONSENSOR_MAX_TEMP;
    g_motionsensor_param->config.gyro_config.temperature_min = MOTIONSENSOR_MIN_TEMP;
    g_motionsensor_param->config.acc_config.temperature_max = MOTIONSENSOR_MAX_TEMP;
    g_motionsensor_param->config.acc_config.temperature_min = MOTIONSENSOR_MIN_TEMP;
    /* init senser */
    ret = chip_dev_init(g_motionsensor_param);
    if (ret != TD_SUCCESS) {
        print_info("ot_motionsensor_param_init failed! ret=%x\n", ret);
        return TD_FAILURE;
    }

    ret = chip_get_triger_config(&g_triger_data);
    if (ret != TD_SUCCESS) {
        print_info("imu_get_triger_config failed! ret=%x\n", ret);
        return TD_FAILURE;
    }

#ifdef MNGBUFF_ENABLE
    /* buff init, fix in this */
    msensor_chip_init_mng_buf(g_motionsensor_param->attr,
        &g_motionsensor_param->buf_attr, &g_motionsensor_param->config);
#endif
    g_motionsensor_init = TD_TRUE;

    return ret;
}

static td_s32 msensor_chip_drv_user_deinit(td_uintptr_t arg)
{
    td_s32 ret;

    if (g_motionsensor_init == TD_FALSE) {
        print_info("motionsensor has not inited!\n");
        return TD_FAILURE;
    }

    if (g_motionsensor_start == TD_TRUE) {
        print_info("please stop motionsensor first!\n");
        return TD_FAILURE;
    }

    ot_unused(arg);

    g_motionsensor_init = TD_FALSE;
    chip_dev_deinit(g_motionsensor_param);
#ifdef MNGBUFF_ENABLE
    ret = msensor_chip_deinit_mng_buf();
    if (ret != TD_SUCCESS) {
        print_info("ot_motionsensorparam_init failed! ret=%x\n", ret);
        return TD_FAILURE;
    }
#endif
    return ret;
}

static td_s32 msensor_chip_drv_user_get_param(td_uintptr_t arg)
{
    td_s32 ret;
    ret = memcpy_s((ot_msensor_param *)arg, sizeof(ot_msensor_param),
                   g_motionsensor_param, sizeof(ot_msensor_param));
    if (ret != EOK) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

/* only for debug: send and save yuv */
static td_s32 msensor_chip_drv_user_mng_init(td_uintptr_t arg)
{
    td_s32 ret;

    if (g_motionsensor_init == TD_TRUE) {
        print_info("motionsensor is already inited!\n");
        return TD_FAILURE;
    }

    ret = memcpy_s(g_motionsensor_param, sizeof(ot_msensor_param),
                   (td_void *)arg, sizeof(ot_msensor_param));
    if (ret != EOK) {
        print_info("memcpy_s error\n");
        return TD_FAILURE;
    }

    if (((OT_MSENSOR_DEVICE_GYRO & g_motionsensor_param->attr.device_mask) &&
        (g_motionsensor_param->config.gyro_config.odr == 0)) ||
        ((OT_MSENSOR_DEVICE_ACC & g_motionsensor_param->attr.device_mask) &&
        (g_motionsensor_param->config.acc_config.odr == 0)) ||
        ((OT_MSENSOR_DEVICE_MAGN & g_motionsensor_param->attr.device_mask) &&
        (g_motionsensor_param->config.magn_config.odr == 0))) {
        print_info("motionsensor param error\n");
        return TD_FAILURE;
    }

    /* init senser */
    ret = chip_dev_mng_init(g_motionsensor_param);
    if (ret != TD_SUCCESS) {
        print_info("ot_motionsensor_param_init failed! ret=%x\n", ret);
        return TD_FAILURE;
    }

#ifdef MNGBUFF_ENABLE
    /* buff init, fix in this */
    msensor_chip_init_mng_buf(g_motionsensor_param->attr,
        &g_motionsensor_param->buf_attr, &g_motionsensor_param->config);
#endif
    g_motionsensor_init = TD_TRUE;

    return ret;
}

/*
 * The order of g_msensor_chip_ioctls's element must be consistent
 * with the enum value defined in "ioc_nr_msensor_chip".
 */
static const msensor_chip_ioctl_info g_msensor_chip_ioctls[] = {
    { MSENSOR_CMD_START, msensor_chip_drv_user_start },
    { MSENSOR_CMD_STOP, msensor_chip_drv_user_stop },
    { MSENSOR_CMD_INIT, msensor_chip_drv_user_init },
    { MSENSOR_CMD_DEINIT, msensor_chip_drv_user_deinit },
    { MSENSOR_CMD_GET_PARAM, msensor_chip_drv_user_get_param },
    { MSENSOR_CMD_MNG_INIT, msensor_chip_drv_user_mng_init }, /* only for debug: send and save yuv */
};

static td_slong msensor_chip_ioctl(td_u32 cmd, td_ulong arg, void *private_data)
{
    if (_IOC_NR(cmd) >= (sizeof(g_msensor_chip_ioctls) / sizeof(g_msensor_chip_ioctls[0])) ||
        cmd != g_msensor_chip_ioctls[_IOC_NR(cmd)].cmd) {
        msensor_err_trace("motionsensor chip ioctl cmd 0x%x does not exist!\n", cmd);
        return TD_FAILURE;
    }

    ot_unused(private_data);

    if (g_msensor_chip_ioctls[_IOC_NR(cmd)].func) {
        /* arg: null pointer has been checked in osal layer. */
        return g_msensor_chip_ioctls[_IOC_NR(cmd)].func((td_uintptr_t)arg);
    }

    return TD_FAILURE;
}

static osal_fileops_t g_msensor_chip_fops = {
    .open = msensor_chip_open,
    .release = msensor_chip_release,
    .unlocked_ioctl = msensor_chip_ioctl,
};

td_s32 motionsensor_chip_module_init(td_void)
{
    td_s32 ret;

    g_motionsensor_param = (ot_msensor_param *)osal_kmalloc(sizeof(ot_msensor_param), osal_gfp_kernel);
    if (g_motionsensor_param == TD_NULL) {
        print_info("could not allocate memory\n");
        return -ENOMEM;
    }

    (td_void)memset_s(g_motionsensor_param, sizeof(ot_msensor_param), 0, sizeof(ot_msensor_param));

    g_motionsensor_start = TD_FALSE;

    ret = chip_sensor_init();
    if (ret != TD_SUCCESS) {
        print_info("init failed! ret=%x\n", ret);
        goto free_chip_param;
    }

#ifdef MNGBUFF_ENABLE
    ret = msensor_chip_register_mng_callback();
    if (ret != TD_SUCCESS) {
        print_info("chip register data mng callback failed! ret=%x\n", ret);
        goto free_chip_param;
    }
#endif

    g_msensor_chip_dev = osal_createdev(MSENSOR_DEV_NAME);
    if (g_msensor_chip_dev == TD_NULL) {
        osal_printk("motionsensor: create device failed\n");
        goto free_chip_param;
    }

    g_msensor_chip_dev->fops = &g_msensor_chip_fops;
    g_msensor_chip_dev->minor = UMAP_MSENSOR_CHIP_MINOR_BASE;
    ret = osal_registerdevice(g_msensor_chip_dev);
    if (ret != TD_SUCCESS) {
        osal_printk("register motionsensor device failed!\n");
        goto destroy_chip_dev;
    }

#if (defined CONFIG_OT_PROC_SHOW_SUPPORT) && (defined ICM20690_PARAM_PROC)
    ret = mpu_proc_init();
    if (ret != TD_SUCCESS) {
        osal_deregisterdevice(g_msensor_chip_dev);
        goto destroy_chip_dev;
    }
#endif

    osal_printk("load motionsensor_chip.ko ....OK!\n");

    return TD_SUCCESS;

destroy_chip_dev:
    osal_destroydev(g_msensor_chip_dev);
free_chip_param:
    safe_kfree(g_motionsensor_param);
    return TD_FAILURE;
}

td_void motionsensor_chip_module_exit(td_void)
{
#ifdef MNGBUFF_ENABLE
    msensor_chip_unregister_mng_callback();
#endif

    if (g_motionsensor_param == TD_NULL) {
        print_info("g_motionsensor_param == NULL, nothing to exit \n");
    } else {
        chip_sensor_deinit(g_motionsensor_param);
        osal_kfree(g_motionsensor_param);
        g_motionsensor_param = TD_NULL;
    }

    osal_deregisterdevice(g_msensor_chip_dev);
    osal_destroydev(g_msensor_chip_dev);

#if (defined CONFIG_OT_PROC_SHOW_SUPPORT) && (defined ICM20690_PARAM_PROC)
    mpu_proc_exit();
#endif

    osal_printk("unload motionsensor_chip.ko ....OK!\n");
}

module_init(motionsensor_chip_module_init);
module_exit(motionsensor_chip_module_exit);

MODULE_AUTHOR("otlicon");
MODULE_DESCRIPTION("motion_sensor driver");
MODULE_LICENSE("GPL");
