/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <signal.h>

#include "ot_mpi_sys.h"
#include "mkp_isp.h"
#include "ot_isp_debug.h"
#include "isp_debug.h"
#include "isp_main.h"
#include "isp_alg.h"
#include "isp_sensor.h"
#include "isp_regcfg.h"
#include "isp_vreg.h"
#include "isp_config.h"
#include "isp_ext_config.h"
#include "ot_osal.h"
#include "isp_inner.h"
#include "ot_mpi_isp.h"

/* GLOBAL VARIABLES */
isp_usr_ctx g_isp_ctx[OT_ISP_MAX_PIPE_NUM] = {{0}};
td_s32      g_isp_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};

static td_u16 g_res_w_max[OT_ISP_MAX_PIPE_NUM] = {OT_ISP_RES_WIDTH_MAX};
static td_u16 g_res_h_max[OT_ISP_MAX_PIPE_NUM] = {OT_ISP_RES_HEIGHT_MAX};

#define isp_check_isp_wdr_cfg_return(dev)                                            \
    do {                                                                      \
        if (g_isp_ctx[dev].para_rec.wdr_cfg != TD_TRUE) {                     \
            isp_err_trace("Isp[%d] WDR mode doesn't config!\n", dev);         \
            return OT_ERR_ISP_ATTR_NOT_CFG;                                   \
        }                                                                     \
    } while (0)

#define isp_check_isp_pub_attr_cfg_return(dev)                                       \
    do {                                                                      \
        if (g_isp_ctx[dev].para_rec.pub_cfg != TD_TRUE) {                     \
            isp_err_trace("Isp[%d] pub attr doesn't config!\n", dev);         \
            return OT_ERR_ISP_ATTR_NOT_CFG;                                   \
        }                                                                     \
    } while (0)

#define isp_check_isp_init_return(dev)                                             \
    do {                                                                    \
        if (g_isp_ctx[dev].para_rec.init != TD_TRUE) {                      \
            isp_err_trace("Isp[%d] doesn't initialized!\n", dev);           \
            return OT_ERR_ISP_NOT_INIT;                                     \
        }                                                                   \
    } while (0)

/* MACRO DEFINITION */
#define ISP_VERSION_MAGIC 20130305

td_s32 isp_check_dev_open(ot_isp_dev dev)
{
    if (g_isp_fd[dev] <= 0) {
        td_u32 arg = (dev);
        g_isp_fd[dev] = open("/dev/isp_dev", O_RDONLY, S_IRUSR);
        if (g_isp_fd[dev] < 0) {
            perror("open isp device error!\n");
            return OT_ERR_ISP_NOT_INIT;
        }
        if (ioctl(g_isp_fd[dev], ISP_DEV_SET_FD, &arg)) {
            close(g_isp_fd[dev]);
            g_isp_fd[dev] = -1;
            return OT_ERR_ISP_NOT_INIT;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_check_mem_init_func(ot_isp_dev dev)
{
    if (g_isp_ctx[dev].mem_init != TD_TRUE) {
        if (ioctl(g_isp_fd[dev], ISP_MEM_INFO_GET, &g_isp_ctx[dev].mem_init)) {
            isp_err_trace("ISP[%d] get mem info failed!\n", dev);
            return OT_ERR_ISP_MEM_NOT_INIT;
        }
        if (g_isp_ctx[dev].mem_init != TD_TRUE) {
            isp_err_trace("ISP[%d] mem NOT init %d!\n", dev, g_isp_ctx[dev].mem_init);
            return OT_ERR_ISP_MEM_NOT_INIT;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_check_mem_init_func_notrace(ot_isp_dev dev)
{
    if (g_isp_ctx[dev].mem_init != TD_TRUE) {
        if (ioctl(g_isp_fd[dev], ISP_MEM_INFO_GET, &g_isp_ctx[dev].mem_init)) {
            return OT_ERR_ISP_MEM_NOT_INIT;
        }
        if (g_isp_ctx[dev].mem_init != TD_TRUE) {
            return OT_ERR_ISP_MEM_NOT_INIT;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_get_fd(ot_vi_pipe vi_pipe)
{
    return g_isp_fd[vi_pipe];
}

isp_usr_ctx *isp_get_usr_ctx(ot_vi_pipe vi_pipe)
{
    return &g_isp_ctx[vi_pipe];
}

td_bool isp_check_hrs_on(ot_vi_pipe vi_pipe)
{
    return (g_isp_ctx[vi_pipe].isp0_p2_en == TD_TRUE);
}

td_u16 isp_get_max_width(ot_vi_pipe vi_pipe)
{
    return g_res_w_max[vi_pipe];
}

td_u16 isp_get_max_height(ot_vi_pipe vi_pipe)
{
    return g_res_h_max[vi_pipe];
}

td_s32 ot_mpi_isp_set_mod_param(const ot_isp_mod_param *mod_param)
{
    isp_check_pipe_return(0);
    isp_check_pointer_return(mod_param);
    isp_check_open_return(0);

    return  ioctl(isp_get_fd(0), ISP_SET_MOD_PARAM, mod_param);
}

td_s32 ot_mpi_isp_get_mod_param(ot_isp_mod_param *mod_param)
{
    isp_check_pipe_return(0);
    isp_check_pointer_return(mod_param);
    isp_check_open_return(0);

    return ioctl(isp_get_fd(0), ISP_GET_MOD_PARAM, mod_param);
}

/*
 * prototype       : ot_mpi_isp_mem_init
 * description     : isp initial extent memory
 */
td_s32 ot_mpi_isp_mem_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    /* check status */
    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_open_return(vi_pipe);

    if (ioctl(g_isp_fd[vi_pipe], ISP_MEM_INFO_GET, &isp_ctx->mem_init)) {
        isp_err_trace("ISP[%d] get mem info failed!\n", vi_pipe);
        return OT_ERR_ISP_MEM_NOT_INIT;
    }

    if (isp_ctx->mem_init == TD_TRUE) {
        ret = isp_exit(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] exit failed!\n", vi_pipe);
            return ret;
        }
    }

    /* WDR attribute */
    ret = ioctl(g_isp_fd[vi_pipe], ISP_GET_WDR_ATTR, &isp_ctx->wdr_attr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] get WDR attr failed\n", vi_pipe);
        return ret;
    }

    isp_check_wdr_main_pipe_return(isp_ctx->wdr_attr.wdr_mode, isp_ctx->wdr_attr.is_mast_pipe); /* wdr mode abnormal */

    /* create extern registers */
    ret = vreg_init(vi_pipe, isp_vir_reg_base(vi_pipe), ISP_VREG_SIZE);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] init mem failed\n", vi_pipe);
        return ret;
    }

    ret = isp_mem_init_update_ctx(vi_pipe, isp_ctx);
    if (ret != TD_SUCCESS) {
        goto fail0;
    }

    ret = isp_check_sns_register(vi_pipe, isp_ctx);
    if (ret != TD_SUCCESS) {
        goto fail1;
    }

    ret = isp_mem_info_set(vi_pipe, isp_ctx);
    if (ret != TD_SUCCESS) {
        goto fail1;
    }

    return TD_SUCCESS;
fail1:
    isp_sensor_unreg_callback(vi_pipe);
fail0:
    vreg_exit(vi_pipe, isp_vir_reg_base(vi_pipe), ISP_VREG_SIZE);
    return ret;
}

/*
 * prototype       : ot_mpi_isp_init
 * description     : isp initial process, include extent memory, top structure,
 *                 default value, etc.
 */
td_s32 ot_mpi_isp_init(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    /* check status */
    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_open_return(vi_pipe);
    if (isp_ctx->isp_yuv_mode == TD_FALSE) {
        isp_check_sensor_register_return(vi_pipe);
    }
    isp_check_mem_init_return(vi_pipe);
    isp_check_wdr_main_pipe_return(isp_ctx->wdr_attr.wdr_mode, isp_ctx->wdr_attr.is_mast_pipe); /* wdr mode abnormal */

    isp_ctx->para_rec.wdr_cfg = ot_ext_top_wdr_cfg_read(vi_pipe);
    isp_ctx->para_rec.pub_cfg = ot_ext_top_pub_attr_cfg_read(vi_pipe);
    isp_check_isp_wdr_cfg_return(vi_pipe);
    isp_check_isp_pub_attr_cfg_return(vi_pipe);

    if (isp_ctx->para_rec.init == TD_TRUE) {
        isp_err_trace("ISP[%d] init failed!\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    return isp_init(vi_pipe, isp_ctx);
}

static td_s32 ot_isp_yuv_run_once(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    mutex_lock(isp_ctx->lock);

    ret = isp_check_mem_init_func(vi_pipe);
    if (ret != TD_SUCCESS) {
        mutex_unlock(isp_ctx->lock);
        return ret;
    }

    /* sometimes OT_MPI_ISP_Run thread is not scheduled to run before calling OT_MPI_ISP_Exit. */
    isp_ctx->sns_wdr_mode = ot_ext_system_sensor_wdr_mode_read(vi_pipe);

    isp_ctx->linkage.run_once = TD_TRUE;
    ret = ioctl(g_isp_fd[vi_pipe], ISP_YUV_RUNONCE_INFO, &isp_ctx->linkage.run_once);
    if (ret != TD_SUCCESS) {
        isp_ctx->linkage.run_once = TD_FALSE;
        isp_err_trace("ISP[%d] set runonce info failed!\n", vi_pipe);
        mutex_unlock(isp_ctx->lock);
        return ret;
    }

    isp_run(vi_pipe);

    ret = ioctl(g_isp_fd[vi_pipe], ISP_KERNEL_YUV_RUNONCE);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] kernel runonce  failed!\n", vi_pipe);
        isp_ctx->linkage.run_once = TD_FALSE;
        ioctl(g_isp_fd[vi_pipe], ISP_YUV_RUNONCE_INFO, &isp_ctx->linkage.run_once);
        mutex_unlock(isp_ctx->lock);
        return ret;
    }

    mutex_unlock(isp_ctx->lock);
    return TD_SUCCESS;
}

/* when offline mode user send raw to BE, firstly need call this function to insure parameters ready */
static td_s32 ot_isp_run_once(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    /* online mode not support */
    if ((is_online_mode(isp_ctx->block_attr.running_mode)) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        isp_err_trace("ISP[%d] run_once not support for online!\n", vi_pipe);
        return OT_ERR_ISP_NOT_SUPPORT;
    }

    mutex_lock(isp_ctx->lock);

    ret = isp_check_mem_init_func(vi_pipe);
    if (ret != TD_SUCCESS) {
        mutex_unlock(isp_ctx->lock);
        return ret;
    }

    /* sometimes OT_MPI_ISP_Run thread is not scheduled to run before calling OT_MPI_ISP_Exit. */
    /* change image mode (WDR mode or resolution) */
    ret = isp_switch_mode(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] switch mode failed!\n", vi_pipe);
        mutex_unlock(isp_ctx->lock);
        return ret;
    }

    isp_ctx->linkage.run_once = TD_TRUE;
    ret = ioctl(g_isp_fd[vi_pipe], ISP_OPT_RUNONCE_INFO, &isp_ctx->linkage.run_once);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] set runonce info failed!\n", vi_pipe);
        goto fail0;
    }

    isp_run(vi_pipe);

    ret = ioctl(g_isp_fd[vi_pipe], ISP_KERNEL_RUNONCE);
    if (ret != TD_SUCCESS) {
        goto fail0;
    }

    mutex_unlock(isp_ctx->lock);

    return TD_SUCCESS;

fail0:
    isp_ctx->linkage.run_once = TD_FALSE;
    if (ioctl(g_isp_fd[vi_pipe], ISP_OPT_RUNONCE_INFO, &isp_ctx->linkage.run_once)) {
        isp_err_trace("ISP[%d] set runonce info failed!\n", vi_pipe);
    }
    mutex_unlock(isp_ctx->lock);

    return ret;
}

td_s32 ot_mpi_isp_run_once(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_s32 ret;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    /* check status */
    isp_check_open_return(vi_pipe);
    if (isp_ctx->isp_yuv_mode == TD_FALSE) {
        isp_check_sensor_register_return(vi_pipe);
    }
    isp_check_mem_init_return(vi_pipe);
    isp_check_isp_init_return(vi_pipe);
    isp_check_wdr_main_pipe_return(isp_ctx->wdr_attr.wdr_mode, isp_ctx->wdr_attr.is_mast_pipe); /* wdr mode abnormal */

    if (isp_ctx->run_thread_status != ISP_RUN_THREAD_STATUS_EXIT) {
        isp_err_trace("ISP[%d] run failed, thread run thread status = %d!\n", vi_pipe, isp_ctx->run_thread_status);
        return TD_FAILURE;
    }

    isp_ctx->run_thread_status = ISP_RUN_THREAD_STATUS_RUN;
    if (isp_ctx->isp_yuv_mode == TD_FALSE) {
        ret = ot_isp_run_once(vi_pipe);
    } else {
        ret = ot_isp_yuv_run_once(vi_pipe);
    }
    isp_ctx->run_thread_status = ISP_RUN_THREAD_STATUS_EXIT;
    return ret;
}

/*
 * prototype       : ot_mpi_isp_run
 * description     : isp firmware recurrent task, always run in a single thread.
 */
td_s32 ot_mpi_isp_run(ot_vi_pipe vi_pipe)
{
    td_bool en;
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    /* check status */
    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_open_return(vi_pipe);
    isp_check_sensor_register_return(vi_pipe);
    isp_check_mem_init_return(vi_pipe);
    isp_check_isp_init_return(vi_pipe);
    isp_check_wdr_main_pipe_return(isp_ctx->wdr_attr.wdr_mode, isp_ctx->wdr_attr.is_mast_pipe); /* wdr mode abnormal */
    mutex_lock(isp_ctx->lock);
    /* sometimes ISP run thread is not scheduled to run before calling ISP exit. */
    /* enable interrupt */
    en = TD_TRUE;
    if (ioctl(g_isp_fd[vi_pipe], ISP_SET_INT_ENABLE, &en) < 0) {
        isp_err_trace("Enable ISP[%d] interrupt failed!\n", vi_pipe);
        mutex_unlock(isp_ctx->lock);
        return TD_FAILURE;
    }

    mutex_unlock(isp_ctx->lock);
    if (isp_ctx->run_thread_status == ISP_RUN_THREAD_STATUS_EXIT) {
        isp_ctx->run_thread_status = ISP_RUN_THREAD_STATUS_RUN;
    } else {
        isp_err_trace("run thread status is error, status = %d\n", isp_ctx->run_thread_status);
        return TD_SUCCESS;
    }

    while (isp_ctx->run_thread_status == ISP_RUN_THREAD_STATUS_RUN) {
        ret = isp_run_thread_proc(vi_pipe);
        if (ret != TD_SUCCESS) {
            break;
        }
    }

    /* disable interrupt */
    en = TD_FALSE;
    if (ioctl(g_isp_fd[vi_pipe], ISP_SET_INT_ENABLE, &en) < 0) {
        isp_err_trace("Disable ISP[%d] interrupt failed!\n", vi_pipe);
    }
    isp_ctx->run_thread_status = ISP_RUN_THREAD_STATUS_EXIT;
    return TD_SUCCESS;
}

static td_void isp_wait_run_thread_exit(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_u32 time_cnt = 0;
    const td_u32 time_out = 400; /* 400:timeout value,400*0.1ms = 40ms */
    isp_get_ctx(vi_pipe, isp_ctx);

    isp_check_pointer_void_return(isp_ctx);
    if (isp_ctx->run_thread_status == ISP_RUN_THREAD_STATUS_EXIT) {
        return;
    }

    if (isp_ctx->run_thread_status == ISP_RUN_THREAD_STATUS_RUN) {
        isp_ctx->run_thread_status = ISP_RUN_THREAD_STATUS_STOP;
    } else {
        return;
    }
    do {
        usleep(100);             /* 100:sleep time us */
        time_cnt++;
    }while ((isp_ctx->run_thread_status == ISP_RUN_THREAD_STATUS_STOP) && (time_cnt < time_out));

    if (time_cnt >= time_out) {
        isp_warn_trace("wait isp run thread time out\n");
    }
    return;
}

/*
 * prototype       : ot_mpi_isp_exit
 * description     : control isp to exit recurrent task, always run in main process.
 */
td_s32 ot_mpi_isp_exit(ot_vi_pipe vi_pipe)
{
    td_s32  ret;
    td_bool enable = TD_FALSE;
    isp_usr_ctx *isp_ctx = TD_NULL;

    /* check status */
    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_wdr_main_pipe_return(isp_ctx->wdr_attr.wdr_mode, isp_ctx->wdr_attr.is_mast_pipe); /* wdr mode abnormal */

    isp_stitch_sync_exit(vi_pipe);

    if (ioctl(g_isp_fd[vi_pipe], ISP_SET_INT_ENABLE, &enable) < 0) {
        isp_err_trace("Disable ISP[%d] interrupt failed!\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    isp_wait_run_thread_exit(vi_pipe);

    mutex_lock(isp_ctx->lock);
    isp_alg_en_exit(vi_pipe);
    ret = isp_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit failed!\n", vi_pipe);
    }

    isp_libs_unregister(vi_pipe);
    mutex_unlock(isp_ctx->lock);
    mutex_destroy(isp_ctx->lock);

    return ret;
}

td_s32 ot_mpi_isp_set_sns_slave_attr(ot_slave_dev slave_dev, const ot_isp_slave_sns_sync *sns_sync)
{
    slave_check_dev_return(slave_dev);
    isp_check_pointer_return(sns_sync);

    ot_isp_slave_mode_time_cfg_select_write(slave_dev, sns_sync->slave_mode_time);
    ot_isp_slave_mode_sync_write(sns_sync->slave_mode_time);
    ot_isp_slave_mode_configs_write(slave_dev, sns_sync->cfg.bytes);
    ot_isp_slave_mode_vstime_low_write(slave_dev, sns_sync->vs_time);
    ot_isp_slave_mode_vstime_high_write(slave_dev, 0);
    ot_isp_slave_mode_hstime_write(slave_dev, sns_sync->hs_time);
    ot_isp_slave_mode_vscyc_low_write(slave_dev, sns_sync->vs_cyc);
    ot_isp_slave_mode_vscyc_high_write(slave_dev, 0);
    ot_isp_slave_mode_hscyc_write(slave_dev, sns_sync->hs_cyc);
    ot_isp_slave_mode_hsdly_write(slave_dev, sns_sync->hs_dly_cyc);

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_get_sns_slave_attr(ot_slave_dev slave_dev, ot_isp_slave_sns_sync *sns_sync)
{
    slave_check_dev_return(slave_dev);
    isp_check_pointer_return(sns_sync);

    sns_sync->slave_mode_time = ot_isp_slave_mode_time_cfg_select_read(slave_dev);
    sns_sync->cfg.bytes = ot_isp_slave_mode_configs_read(slave_dev);
    sns_sync->vs_time = ot_isp_slave_mode_vstime_low_read(slave_dev);
    sns_sync->hs_time = ot_isp_slave_mode_hstime_read(slave_dev);
    sns_sync->vs_cyc  = ot_isp_slave_mode_vscyc_low_read(slave_dev);
    sns_sync->hs_cyc  = ot_isp_slave_mode_hscyc_read(slave_dev);
    sns_sync->hs_dly_cyc = ot_isp_slave_mode_hsdly_read(slave_dev);

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_sensor_reg_callback(ot_vi_pipe vi_pipe, ot_isp_sns_attr_info *sns_attr_info,
                                      const ot_isp_sensor_register *sns_register)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(sns_register);
    isp_check_pointer_return(sns_attr_info);

    isp_check_pointer_return(sns_register->sns_exp.pfn_cmos_sensor_init);
    isp_check_pointer_return(sns_register->sns_exp.pfn_cmos_get_isp_default);
    isp_check_pointer_return(sns_register->sns_exp.pfn_cmos_get_isp_black_level);
    isp_check_pointer_return(sns_register->sns_exp.pfn_cmos_get_sns_reg_info);
    isp_check_pointer_return(sns_register->sns_exp.pfn_cmos_set_pixel_detect);

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    if (isp_ctx->sns_reg == TD_TRUE) {
        isp_err_trace("Reg ERR! Sensor have registered to ISP[%d]!\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    ret = isp_sensor_reg_callback(vi_pipe, sns_attr_info, sns_register);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    isp_ctx->bind_attr.sensor_id = sns_attr_info->sensor_id;
    isp_ctx->sns_reg = TD_TRUE;

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_ae_lib_reg_callback(ot_vi_pipe vi_pipe, const ot_isp_3a_alg_lib *ae_lib,
                                      const ot_isp_ae_register *ae_register)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_lib_node *ae_lib_node = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    /* check null point */
    isp_check_pointer_return(ae_lib);
    isp_check_pointer_return(ae_register);

    isp_check_pointer_return(ae_register->ae_exp_func.pfn_ae_init);
    isp_check_pointer_return(ae_register->ae_exp_func.pfn_ae_run);
    isp_check_pointer_return(ae_register->ae_exp_func.pfn_ae_ctrl);
    isp_check_pointer_return(ae_register->ae_exp_func.pfn_ae_exit);

    /* whether the lib have been registered */
    ret = isp_find_lib(isp_ctx->ae_lib_info.libs, ae_lib);
    if (ret != -1) {
        isp_err_trace("Reg ERR! aelib have registered to ISP[%d].\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* whether can we register a new lib  */
    ae_lib_node = isp_search_lib(isp_ctx->ae_lib_info.libs);
    if (ae_lib_node == TD_NULL) {
        isp_err_trace("can't register aelib to ISP[%d], there is too many libs.\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* record register info */
    (td_void)memcpy_s(&ae_lib_node->alg_lib, sizeof(ot_isp_3a_alg_lib), ae_lib, sizeof(ot_isp_3a_alg_lib));
    (td_void)memcpy_s(&ae_lib_node->ae_regsiter, sizeof(ot_isp_ae_register), ae_register, sizeof(ot_isp_ae_register));
    ae_lib_node->used = TD_TRUE;

    /* set active lib */
    isp_ctx->ae_lib_info.active_lib = (td_u32)isp_find_lib(isp_ctx->ae_lib_info.libs, ae_lib);
    (td_void)memcpy_s(&isp_ctx->bind_attr.ae_lib, sizeof(ot_isp_3a_alg_lib), ae_lib, sizeof(ot_isp_3a_alg_lib));

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_awb_lib_reg_callback(ot_vi_pipe vi_pipe, const ot_isp_3a_alg_lib *awb_lib,
                                       const ot_isp_awb_register *awb_register)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_lib_node *awb_lib_node = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    /* check null point */
    isp_check_pointer_return(awb_lib);
    isp_check_pointer_return(awb_register);

    isp_check_pointer_return(awb_register->awb_exp_func.pfn_awb_init);
    isp_check_pointer_return(awb_register->awb_exp_func.pfn_awb_run);
    isp_check_pointer_return(awb_register->awb_exp_func.pfn_awb_ctrl);
    isp_check_pointer_return(awb_register->awb_exp_func.pfn_awb_exit);

    /* whether the lib have been registered */
    ret = isp_find_lib(isp_ctx->awb_lib_info.libs, awb_lib);
    if (ret != -1) {
        isp_err_trace("Reg ERR! awblib have registered to ISP[%d].\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* whether can we register a new lib  */
    awb_lib_node = isp_search_lib(isp_ctx->awb_lib_info.libs);
    if (awb_lib_node == TD_NULL) {
        isp_err_trace("can't register awblib to ISP[%d], there is too many libs.\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* record register info */
    (td_void)memcpy_s(&awb_lib_node->alg_lib, sizeof(ot_isp_3a_alg_lib), awb_lib, sizeof(ot_isp_3a_alg_lib));
    (td_void)memcpy_s(&awb_lib_node->awb_regsiter, sizeof(ot_isp_awb_register),
                      awb_register, sizeof(ot_isp_awb_register));
    awb_lib_node->used = TD_TRUE;

    /* set active lib */
    isp_ctx->awb_lib_info.active_lib = (td_u32)isp_find_lib(isp_ctx->awb_lib_info.libs, awb_lib);
    (td_void)memcpy_s(&isp_ctx->bind_attr.awb_lib, sizeof(ot_isp_3a_alg_lib), awb_lib, sizeof(ot_isp_3a_alg_lib));

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_sensor_unreg_callback(ot_vi_pipe vi_pipe, ot_sensor_id sensor_id)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_sensor_register_return(vi_pipe);

    /* check sensor id */
    if (isp_ctx->bind_attr.sensor_id != sensor_id) {
        isp_err_trace("UnReg ERR! ISP[%d] Registered sensor is %d, present sensor is %d.\n",
                      vi_pipe, isp_ctx->bind_attr.sensor_id, sensor_id);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    isp_ctx->bind_attr.sensor_id = 0;
    isp_ctx->sns_reg = TD_FALSE;

    isp_sensor_unreg_callback(vi_pipe);

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_ae_lib_unreg_callback(ot_vi_pipe vi_pipe, const ot_isp_3a_alg_lib *ae_lib)
{
    td_s32 search_id;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    /* check null point */
    isp_check_pointer_return(ae_lib);

    search_id = isp_find_lib(isp_ctx->ae_lib_info.libs, ae_lib);
    if (-1 == search_id) {
        isp_err_trace("can't find ae lib in ISP[%d].\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    (td_void)memset_s(&isp_ctx->ae_lib_info.libs[search_id], sizeof(isp_lib_node), 0, sizeof(isp_lib_node));

    /* set active lib */
    isp_ctx->ae_lib_info.active_lib = 0;

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_awb_lib_unreg_callback(ot_vi_pipe vi_pipe, const ot_isp_3a_alg_lib *awb_lib)
{
    td_s32 search_id;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    /* check null point */
    isp_check_pointer_return(awb_lib);

    search_id = isp_find_lib(isp_ctx->awb_lib_info.libs, awb_lib);
    if (search_id == -1) {
        isp_err_trace("can't find awb lib in ISP[%d].\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    (td_void)memset_s(&isp_ctx->awb_lib_info.libs[search_id], sizeof(isp_lib_node), 0, sizeof(isp_lib_node));

    /* set active lib */
    isp_ctx->awb_lib_info.active_lib = 0;

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_set_bind_attr(ot_vi_pipe vi_pipe, const ot_isp_bind_attr *bind_attr)
{
    ot_sensor_id sensor_id = 0;
    td_s32    search_id;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_s32 ret;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    isp_check_pointer_return(bind_attr);

    ret = isp_sensor_get_id(vi_pipe, &sensor_id);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Get Sensor id err\n", vi_pipe);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* check sensor id */
    if (bind_attr->sensor_id != sensor_id) {
        isp_err_trace("ISP[%d] Register sensor is %d, present sensor is %d.\n",
                      vi_pipe, sensor_id, bind_attr->sensor_id);
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* check ae lib */
    search_id = isp_find_lib(isp_ctx->ae_lib_info.libs, &bind_attr->ae_lib);
    if (search_id != -1) {
        isp_ctx->ae_lib_info.active_lib = search_id;
    } else {
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* check awb lib */
    search_id = isp_find_lib(isp_ctx->awb_lib_info.libs, &bind_attr->awb_lib);
    if (search_id != -1) {
        isp_ctx->awb_lib_info.active_lib = search_id;
    } else {
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* check af lib */
    search_id = isp_find_lib(isp_ctx->af_lib_info.libs, &bind_attr->af_lib);
    if (search_id != -1) {
        isp_ctx->af_lib_info.active_lib = search_id;
    } else {
        return OT_ERR_ISP_ILLEGAL_PARAM;
    }

    /* save global variable */
    (td_void)memcpy_s(&isp_ctx->bind_attr, sizeof(ot_isp_bind_attr), bind_attr, sizeof(ot_isp_bind_attr));

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_get_bind_attr(ot_vi_pipe vi_pipe, ot_isp_bind_attr *bind_attr)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    isp_check_pointer_return(bind_attr);

    /* get global variable */
    (td_void)memcpy_s(bind_attr, sizeof(ot_isp_bind_attr), &isp_ctx->bind_attr, sizeof(ot_isp_bind_attr));

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_get_vd_time_out(ot_vi_pipe vi_pipe, ot_isp_vd_type isp_vd_type, td_u32 milli_sec)
{
    td_s32 ret, isp_fd;
    isp_vd_timeout   isp_vd_time_out;
    isp_working_mode isp_work_mode;

    isp_check_pipe_return(vi_pipe);
    isp_check_open_return(vi_pipe);
    isp_check_mem_init_return(vi_pipe);

    isp_fd = isp_get_fd(vi_pipe);

    isp_vd_time_out.milli_sec  = milli_sec;
    isp_vd_time_out.int_status = 0x0;

    switch (isp_vd_type) {
        case OT_ISP_VD_FE_START:
            ret = ioctl(isp_fd, ISP_GET_VD_TIMEOUT, &isp_vd_time_out);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        case OT_ISP_VD_FE_END:
            ret = ioctl(isp_fd, ISP_GET_VD_END_TIMEOUT, &isp_vd_time_out);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        case OT_ISP_VD_BE_END:
            ret = ioctl(isp_fd, ISP_WORK_MODE_GET, &isp_work_mode);
            if (ret != TD_SUCCESS) {
                isp_err_trace("Get Work Mode error!\n");
                return TD_FAILURE;
            }
            if ((isp_work_mode.running_mode == ISP_MODE_RUNNING_ONLINE) ||
                (isp_work_mode.running_mode == ISP_MODE_RUNNING_SIDEBYSIDE)) {
                    isp_err_trace("Only support ISP_VD_BE_END under ISP offline mode!\n");
                    return OT_ERR_ISP_ILLEGAL_PARAM;
            }
            ret = ioctl(isp_fd, ISP_GET_VD_BEEND_TIMEOUT, &isp_vd_time_out);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        default:
            isp_err_trace("ISP[%d] Get VD type %d not support!\n", vi_pipe, isp_vd_type);
            return OT_ERR_ISP_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}
/*
 * prototype       : ot_mpi_isp_set_dcf_info
 * description     : set dcf info to isp firmware
 */
static td_s32 isp_dcf_info_buf_update(ot_vi_pipe vi_pipe, isp_usr_ctx *isp_ctx)
{
    td_u64 phy_addr_high, phy_addr_temp, buf_size;

    phy_addr_high = (td_u64)ot_ext_system_update_info_high_phyaddr_read(vi_pipe);
    phy_addr_temp = (td_u64)ot_ext_system_update_info_low_phyaddr_read(vi_pipe);
    phy_addr_temp |= (phy_addr_high << 32);  /* left shift 32bits */
    buf_size = sizeof(ot_isp_dcf_update_info) * ISP_MAX_UPDATEINFO_BUF_NUM + sizeof(ot_isp_dcf_const_info);

    isp_ctx->isp_trans_info.update_info.phy_addr = phy_addr_temp;

    isp_ctx->update_info_ctrl.isp_update_info = ot_mpi_sys_mmap(isp_ctx->isp_trans_info.update_info.phy_addr, buf_size);

    if (isp_ctx->update_info_ctrl.isp_update_info == TD_NULL) {
        isp_err_trace("isp[%d] dcf info mmap failed!\n", vi_pipe);
        return OT_ERR_ISP_NOMEM;
    }

    isp_ctx->update_info_ctrl.isp_dcf_const_info = (ot_isp_dcf_const_info *)(isp_ctx->update_info_ctrl.isp_update_info +
                                                   ISP_MAX_UPDATEINFO_BUF_NUM);

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_set_dcf_info(ot_vi_pipe vi_pipe, const ot_isp_dcf_info *isp_dcf)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_bool temp_map = TD_FALSE;
    td_s32  ret;
    td_u64  buf_size;
    isp_check_pipe_return(vi_pipe);
    isp_check_open_return(vi_pipe);
    isp_check_mem_init_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_pointer_return(isp_dcf);

    if (isp_ctx->isp_trans_info.update_info.phy_addr == 0) {
        ret = isp_dcf_info_buf_update(vi_pipe, isp_ctx);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        temp_map = TD_TRUE;
    }
    isp_check_pointer_return(isp_ctx->update_info_ctrl.isp_update_info);
    isp_check_pointer_return(isp_ctx->update_info_ctrl.isp_dcf_const_info);
    (td_void)memcpy_s(isp_ctx->update_info_ctrl.isp_dcf_const_info, sizeof(ot_isp_dcf_const_info),
                      &isp_dcf->isp_dcf_const_info, sizeof(ot_isp_dcf_const_info));
    (td_void)memcpy_s(isp_ctx->update_info_ctrl.isp_update_info, sizeof(ot_isp_dcf_update_info),
                      &isp_dcf->isp_dcf_update_info, sizeof(ot_isp_dcf_update_info));

    buf_size = sizeof(ot_isp_dcf_update_info) * ISP_MAX_UPDATEINFO_BUF_NUM + sizeof(ot_isp_dcf_const_info);
    if (temp_map) {
        ot_mpi_sys_munmap(isp_ctx->update_info_ctrl.isp_update_info, (td_u32)buf_size);

        isp_ctx->isp_trans_info.update_info.phy_addr = 0;
    }

    return TD_SUCCESS;
}

/*
 * prototype       : ot_mpi_isp_get_dcf_info
 * description     : get dcf info from isp firmware
 */
td_s32 ot_mpi_isp_get_dcf_info(ot_vi_pipe vi_pipe, ot_isp_dcf_info *isp_dcf)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_bool temp_map = TD_FALSE;
    td_s32  ret;
    td_u64  buf_size;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_pointer_return(isp_dcf);
    isp_check_open_return(vi_pipe);
    isp_check_mem_init_return(vi_pipe);

    buf_size = sizeof(ot_isp_dcf_update_info) * ISP_MAX_UPDATEINFO_BUF_NUM + sizeof(ot_isp_dcf_const_info);

    if (isp_ctx->isp_trans_info.update_info.phy_addr == 0) {
        ret = isp_dcf_info_buf_update(vi_pipe, isp_ctx);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        temp_map = TD_TRUE;
    }
    isp_check_pointer_return(isp_ctx->update_info_ctrl.isp_update_info);
    isp_check_pointer_return(isp_ctx->update_info_ctrl.isp_dcf_const_info);
    (td_void)memcpy_s(&isp_dcf->isp_dcf_const_info, sizeof(ot_isp_dcf_const_info),
                      isp_ctx->update_info_ctrl.isp_dcf_const_info, sizeof(ot_isp_dcf_const_info));
    (td_void)memcpy_s(&isp_dcf->isp_dcf_update_info, sizeof(ot_isp_dcf_update_info),
                      isp_ctx->update_info_ctrl.isp_update_info, sizeof(ot_isp_dcf_update_info));

    if (temp_map) {
        ot_mpi_sys_munmap(isp_ctx->update_info_ctrl.isp_update_info, (td_u32)buf_size);

        isp_ctx->isp_trans_info.update_info.phy_addr = 0;
    }

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_set_frame_info(ot_vi_pipe vi_pipe, const ot_isp_frame_info *isp_frame)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_pointer_return(isp_ctx->frame_info_ctrl.isp_frame);
    isp_check_pointer_return(isp_frame);
    isp_check_open_return(vi_pipe);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_FRAME_INFO_SET, isp_frame);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    (td_void)memcpy_s(isp_ctx->frame_info_ctrl.isp_frame, sizeof(ot_isp_frame_info),
                      isp_frame, sizeof(ot_isp_frame_info));

    return TD_SUCCESS;
}

td_s32 ot_mpi_isp_get_frame_info(ot_vi_pipe vi_pipe, const ot_isp_frame_info *isp_frame)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);
    isp_check_pointer_return(isp_frame);
    isp_check_open_return(vi_pipe);

    if (ioctl(isp_get_fd(vi_pipe), ISP_FRAME_INFO_GET, isp_frame)) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}
