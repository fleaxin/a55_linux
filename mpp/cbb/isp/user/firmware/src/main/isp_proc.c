/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_proc.h"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ot_mpi_sys.h"
#include "mkp_isp.h"

typedef struct {
    td_u32 int_count;
    td_u32 proc_param;
    isp_proc_mem proc_mem;
} isp_proc_ctx;

isp_proc_ctx g_proc_ctx[OT_ISP_MAX_PIPE_NUM] = { { 0 } };
#define proc_get_ctx(dev, ctx) ctx = &g_proc_ctx[dev]

static td_s32 isp_update_proc_param(ot_vi_pipe vi_pipe);

td_s32 isp_proc_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_proc_ctx *proc = TD_NULL;

    isp_check_open_return(vi_pipe);
    proc_get_ctx(vi_pipe, proc);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_PARAM_GET, &proc->proc_param);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] get proc param failed!\n", vi_pipe);
        return ret;
    }

    if (proc->proc_param == 0) {
        return TD_SUCCESS;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_INIT, &proc->proc_mem);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] init proc ec %x!\n", vi_pipe, ret);
        return ret;
    }

    proc->proc_mem.virt_addr = ot_mpi_sys_mmap(proc->proc_mem.phy_addr,
                                               proc->proc_mem.size);
    if (proc->proc_mem.virt_addr == TD_NULL) {
        isp_err_trace("ISP[%d] mmap proc mem failed!\n", vi_pipe);
        ret = OT_ERR_ISP_NOMEM;
        goto freeproc;
    }
    proc->int_count = 0;

    return TD_SUCCESS;

freeproc:
    if (ioctl(isp_get_fd(vi_pipe), ISP_PROC_EXIT) != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit proc failed!\n", vi_pipe);
        return TD_FAILURE;
    }
    return ret;
}

static td_s32 isp_alg_proc_writing(const isp_alg_node *algs, ot_vi_pipe vi_pipe, isp_proc_ctx *proc)
{
    td_s32 ret, i;
    ot_isp_ctrl_proc_write proc_ctrl;

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_WRITE_ING);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] write proc failed, ec %x!\n", vi_pipe, ret);
        return ret;
    }

    proc_ctrl.proc_buff = (td_char *)proc->proc_mem.virt_addr;
    proc_ctrl.buff_len  = proc->proc_mem.size - 1;
    proc_ctrl.write_len = 0;

    for (i = 0; i < ISP_MAX_ALGS_NUM; i++) {
        if (algs[i].used != TD_TRUE) {
            continue;
        }

        if (algs[i].alg_func.pfn_alg_ctrl != TD_NULL) {
            algs[i].alg_func.pfn_alg_ctrl(vi_pipe, OT_ISP_PROC_WRITE, &proc_ctrl);
        }

        if (proc_ctrl.write_len > proc_ctrl.buff_len) {
            isp_err_trace("ISP[%d] Warning!! proc buff overflow!\n", vi_pipe);
            proc_ctrl.write_len = proc_ctrl.buff_len;
            break;
        }

        if (proc_ctrl.write_len != 0) {
            if (proc_ctrl.proc_buff[proc_ctrl.write_len - 1] != '\0') {
                isp_err_trace("ISP[%d] Warning!! alg %d's proc doesn't finished!\n", vi_pipe, algs[i].alg_type);
            }
            proc_ctrl.proc_buff[proc_ctrl.write_len - 1] = '\n';
        }

        /* update the proc ctrl */
        proc_ctrl.proc_buff = &proc_ctrl.proc_buff[proc_ctrl.write_len];
        proc_ctrl.buff_len  = proc_ctrl.buff_len - proc_ctrl.write_len;
        proc_ctrl.write_len = 0;
        if (proc_ctrl.buff_len == 0) {
            break;
        }
    }

    proc_ctrl.proc_buff[proc_ctrl.write_len] = '\0';
    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_WRITE_OK);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] write proc failed, ec %x!\n", vi_pipe, ret);
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_proc_write(const isp_alg_node *algs, ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_proc_ctx *proc = TD_NULL;

    isp_check_open_return(vi_pipe);
    proc_get_ctx(vi_pipe, proc);

    ret = isp_update_proc_param(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] isp_update_proc_param failed!\n", vi_pipe);
        return ret;
    }

    if (proc->proc_param == 0) {
        return TD_SUCCESS;
    }

    if (proc->proc_mem.virt_addr == TD_NULL) {
        isp_err_trace("ISP[%d] the proc hasn't init!\n", vi_pipe);
        return TD_FAILURE;
    }

    /* write proc info 1s a time */
    proc->int_count++;
    if (proc->int_count < proc->proc_param) {
        return TD_SUCCESS;
    }
    proc->int_count = 0;

    ret = isp_alg_proc_writing(algs, vi_pipe, proc);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_void isp_proc_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_void *virt_addr = TD_NULL;
    isp_proc_ctx *proc = TD_NULL;

    proc_get_ctx(vi_pipe, proc);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_PARAM_GET, &proc->proc_param);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] get proc param %x!\n", vi_pipe, ret);
        return;
    }

    if (proc->proc_param == 0) {
        return;
    }

    if (proc->proc_mem.virt_addr != TD_NULL) {
        virt_addr = proc->proc_mem.virt_addr;
        proc->proc_mem.virt_addr = TD_NULL;
        ot_mpi_sys_munmap(virt_addr, proc->proc_mem.size);
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_EXIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit proc ec %x!\n", vi_pipe, ret);
        return;
    }
}

static td_s32 isp_update_proc_param(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_proc_ctx *proc = TD_NULL;

    proc_get_ctx(vi_pipe, proc);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PROC_PARAM_GET, &proc->proc_param);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] get proc param %x!\n", vi_pipe, ret);
        return ret;
    }

    return TD_SUCCESS;
}

