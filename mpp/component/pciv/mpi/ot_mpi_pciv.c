/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>

#include "ot_mpi_pciv.h"

#include "ot_type.h"
#include "ot_common_pciv.h"
#include "mkp_pciv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static td_s32 g_pciv_fd[OT_PCIV_MAX_CHN_NUM] = { [0 ...(OT_PCIV_MAX_CHN_NUM - 1)] = -1 };

/*
 * If the ioctl is interrupt by the sema, it will called again
 * if in the program the signal is been capture,but not set SA_RESTART
 * attribute,then when the xx_interruptible in the kernel interrupt,
 * it will not re-called by the system,So The best way to avoid this
 * is encapsule again in the user mode
 */
static pthread_mutex_t g_pciv_mutex = PTHREAD_MUTEX_INITIALIZER;

#define pciv_mutex_lock()                       \
do {                                            \
    (td_void)pthread_mutex_lock(&g_pciv_mutex); \
} while (0)

#define pciv_mutex_unlock()                       \
do {                                              \
    (td_void)pthread_mutex_unlock(&g_pciv_mutex); \
} while (0)

static td_void mkpi_pciv_show_ioctl_err_msg(td_s32 errcode)
{
    if (errcode == -1) {
        perror("ioctl error");
        printf("func:%s,line:%d,err_no:0x%x\n", __FUNCTION__, __LINE__, errcode);
    }
    return;
}

static td_s32 pciv_ioctl(td_s32 fd, td_u32 cmd, td_uintptr_t arg)
{
    td_s32 ret;

    do {
        if (arg == TD_NULL) {
            ret = ioctl(fd, cmd);
        } else {
            ret = ioctl(fd, cmd, (td_void *)arg);
        }
    } while ((ret == -1) && (errno == EINTR));
    mkpi_pciv_show_ioctl_err_msg(ret);
    return ret;
}

static td_s32 mkpi_pciv_check_chn_open(ot_pciv_chn chn)
{
    td_s32 ret;

    pciv_mutex_lock();
    if (g_pciv_fd[chn] < 0) {
        g_pciv_fd[chn] = open("/dev/pciv", O_RDONLY);
        if (g_pciv_fd[chn] < 0) {
            perror("open PCIV error");
            pciv_mutex_unlock();
            return OT_ERR_PCIV_NOT_READY;
        }
        ret = pciv_ioctl(g_pciv_fd[chn], PCIV_BIND_CHN_TO_FD_CTRL, (td_uintptr_t)&chn);
        if (ret != TD_SUCCESS) {
            close(g_pciv_fd[chn]);
            g_pciv_fd[chn] = -1;
            pciv_mutex_unlock();
            return OT_ERR_PCIV_NOT_READY;
        }
    }
    pciv_mutex_unlock();
    return TD_SUCCESS;
}

#define pciv_check_open_return(id)                    \
do {                                                  \
    if (mkpi_pciv_check_chn_open(id) != TD_SUCCESS) { \
        return OT_ERR_PCIV_NOT_READY;                 \
    }                                                 \
} while (0)                                           \

td_s32 ot_mpi_pciv_malloc_chn_buf(ot_pciv_chn chn, td_u32 blk_size, td_u32 blk_cnt, td_phys_addr_t phys_addr[])
{
    td_s32 ret;
    td_s32 cnt;
    ot_pciv_ioctl_malloc_chn_buf malloc_chn_buf;

    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);
    pciv_check_ptr_return(phys_addr);

    malloc_chn_buf.chn_id = chn;
    if (blk_size == 0) {
        pciv_err_trace("blk_size:%d is illegal!\n", blk_size);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((blk_cnt == 0) || (blk_cnt > OT_PCIV_MAX_BUF_NUM)) {
        pciv_err_trace("blk_cnt:%d is illegal!\n", blk_cnt);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    for (cnt = 0; (td_u32)cnt < blk_cnt; cnt++) {
        malloc_chn_buf.index = cnt;
        malloc_chn_buf.size = blk_size;
        malloc_chn_buf.phys_addr = 0;
        ret = pciv_ioctl(g_pciv_fd[chn], PCIV_MALLOC_CHN_BUF_CTRL, (td_uintptr_t)&malloc_chn_buf);
        if (ret != TD_SUCCESS) {
            break;
        }

        phys_addr[cnt] = malloc_chn_buf.phys_addr;
    }

    /* if one block malloc failure then free all the memory */
    if (ret != TD_SUCCESS) {
        cnt--;
        for (; cnt >= 0; cnt--) {
            (td_void)pciv_ioctl(g_pciv_fd[chn], PCIV_FREE_CHN_BUF_CTRL, (td_uintptr_t)&cnt);
            phys_addr[cnt] = 0;
        }
    }

    return ret;
}

td_s32 ot_mpi_pciv_free_chn_buf(ot_pciv_chn chn, td_u32 blk_cnt)
{
    td_u32 i;
    td_s32 ret;

    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);

    if ((blk_cnt == 0) || (blk_cnt > OT_PCIV_MAX_BUF_NUM)) {
        pciv_err_trace("blk_cnt:%d is illegal!\n", blk_cnt);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    for (i = 0; i < blk_cnt; i++) {
        ret = pciv_ioctl(g_pciv_fd[chn], PCIV_FREE_CHN_BUF_CTRL, (td_uintptr_t)&i);
        if (ret != TD_SUCCESS) {
            break;
        }
    }

    return ret;
}

td_s32 ot_mpi_pciv_create_chn(ot_pciv_chn chn, const ot_pciv_attr *attr)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);
    pciv_check_ptr_return(attr);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_CREATE_CTRL, (td_uintptr_t)attr);
}

td_s32 ot_mpi_pciv_destroy_chn(ot_pciv_chn chn)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_DESTROY_CTRL, TD_NULL);
}

td_s32 ot_mpi_pciv_start_chn(ot_pciv_chn chn)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_START_CTRL, TD_NULL);
}

td_s32 ot_mpi_pciv_stop_chn(ot_pciv_chn chn)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_STOP_CTRL, TD_NULL);
}

td_s32 ot_mpi_pciv_set_chn_attr(ot_pciv_chn chn, const ot_pciv_attr *attr)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);
    pciv_check_ptr_return(attr);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_SET_ATTR_CTRL, (td_uintptr_t)attr);
}

td_s32 ot_mpi_pciv_get_chn_attr(ot_pciv_chn chn, ot_pciv_attr *attr)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);
    pciv_check_ptr_return(attr);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_GET_ATTR_CTRL, (td_uintptr_t)attr);
}

td_s32 ot_mpi_pciv_show_chn(ot_pciv_chn chn)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_SHOW_CTRL, TD_NULL);
}

td_s32 ot_mpi_pciv_hide_chn(ot_pciv_chn chn)
{
    pciv_check_chn_id_return(chn);
    pciv_check_open_return(chn);

    return pciv_ioctl(g_pciv_fd[chn], PCIV_HIDE_CTRL, TD_NULL);
}

td_s32 ot_mpi_pciv_dma_task(const ot_pciv_dma_task *task)
{
    pciv_check_open_return(0);
    pciv_check_ptr_return(task);
    pciv_check_ptr_return(task->dma_blk);

    return pciv_ioctl(g_pciv_fd[0], PCIV_DMA_TASK_CTRL, (td_uintptr_t)task);
}

td_s32 ot_mpi_pciv_create_window_vb(const ot_pciv_window_vb_cfg *cfg)
{
    pciv_check_open_return(0);
    pciv_check_ptr_return(cfg);

    return pciv_ioctl(g_pciv_fd[0], PCIV_WINDOW_VB_CREATE_CTRL, (td_uintptr_t)cfg);
}

td_s32 ot_mpi_pciv_destroy_window_vb(td_void)
{
    pciv_check_open_return(0);

    return pciv_ioctl(g_pciv_fd[0], PCIV_WINDOW_VB_DESTROY_CTRL, TD_NULL);
}

td_s32 ot_mpi_pciv_malloc_window_buf(td_u32 blk_size, td_u32 blk_cnt, td_phys_addr_t phys_addr[])
{
    td_s32 ret;
    td_s32 cnt;
    ot_pciv_ioctl_malloc malloc_buf;

    pciv_check_open_return(0);
    pciv_check_ptr_return(phys_addr);

    if (blk_cnt == 0) {
        pciv_err_trace("blk_cnt:%d is illegal!\n", blk_cnt);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if (blk_size == 0) {
        pciv_err_trace("blk_size:%d is illegal!\n", blk_size);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    for (cnt = 0; (td_u32)cnt < blk_cnt; cnt++) {
        malloc_buf.size = blk_size;
        malloc_buf.phys_addr = 0;
        ret = pciv_ioctl(g_pciv_fd[0], PCIV_MALLOC_CTRL, (td_uintptr_t)&malloc_buf);
        if (ret != TD_SUCCESS) {
            break;
        }

        phys_addr[cnt] = malloc_buf.phys_addr;
    }
    /* If one block malloc failure then free all the memory */
    if ((ret != TD_SUCCESS) && (cnt != 0)) {
        cnt--;
        for (; cnt >= 0; cnt--) {
            (td_void)pciv_ioctl(g_pciv_fd[0], PCIV_FREE_CTRL, (td_uintptr_t)&phys_addr[cnt]);
        }
    }

    return ret;
}

td_s32 ot_mpi_pciv_free_window_buf(td_u32 blk_cnt, const td_phys_addr_t phys_addr[])
{
    td_u32 i;
    td_s32 ret;

    pciv_check_open_return(0);
    pciv_check_ptr_return(phys_addr);

    if (blk_cnt == 0) {
        pciv_err_trace("blk_cnt:%d is illegal!\n", blk_cnt);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    for (i = 0; i < blk_cnt; i++) {
        ret = pciv_ioctl(g_pciv_fd[0], PCIV_FREE_CTRL, (td_uintptr_t)&phys_addr[i]);
        if (ret != TD_SUCCESS) {
            break;
        }
    }

    return ret;
}

td_s32 ot_mpi_pciv_get_local_id(td_s32 *id)
{
    pciv_check_open_return(0);
    pciv_check_ptr_return(id);

    return pciv_ioctl(g_pciv_fd[0], PCIV_GET_LOCAL_ID_CTRL, (td_uintptr_t)id);
}

td_s32 ot_mpi_pciv_enum_chip(ot_pciv_enum_chip *chips)
{
    pciv_check_open_return(0);
    pciv_check_ptr_return(chips);

    return pciv_ioctl(g_pciv_fd[0], PCIV_ENUM_CHIP_ID_CTRL, (td_uintptr_t)chips);
}

td_s32 ot_mpi_pciv_get_window_base(td_s32 chip_id, ot_pciv_window_base *base)
{
    pciv_check_open_return(0);
    pciv_check_ptr_return(base);

    base->chip_id = chip_id;
    return pciv_ioctl(g_pciv_fd[0], PCIV_GET_WINDOW_BASE_CTRL, (td_uintptr_t)base);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

