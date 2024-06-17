/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "pciv_ext.h"

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>

#include "ot_osal.h"
#include "ot_errno.h"
#include "ot_debug.h"
#include "mod_ext.h"
#include "dev_ext.h"

#include "pciv.h"
#include "pciv_drvadp.h"
#include "mkp_pciv.h"
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
#include "proc_ext.h"
#include "pciv_proc.h"
#endif

static td_s32 pciv_create_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_destroy_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_set_attr_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_get_attr_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_start_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_stop_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_malloc_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_free_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_malloc_chn_buf_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_free_chn_buf_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_get_window_base_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_get_local_id_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_enum_chip_id_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_dma_task_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_bind_chn_to_fd_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_win_vb_create_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_win_vb_destroy_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_show_ctrl(td_uintptr_t arg, const td_void *private_data);
static td_s32 pciv_hide_ctrl(td_uintptr_t arg, const td_void *private_data);

typedef enum {
    PCIV_STATE_STARTED  = 0,
    PCIV_STATE_STOPPING = 1,
    PCIV_STATE_STOPPED  = 2
} pciv_state;

typedef struct {
    td_u32 cmd;
    td_s32 (*func)(td_uintptr_t arg, const td_void *private_data);
} pciv_ioctl_info;

static osal_dev_t       *g_pciv_device = TD_NULL;
static pciv_state       g_pciv_state = PCIV_STATE_STOPPED;
static osal_atomic_t    g_pciv_user_ref = OSAL_ATOMIC_INIT(0);

static const pciv_ioctl_info g_pciv_ioctls[] = {
    { PCIV_CREATE_CTRL, pciv_create_ctrl },
    { PCIV_DESTROY_CTRL, pciv_destroy_ctrl },
    { PCIV_SET_ATTR_CTRL, pciv_set_attr_ctrl },
    { PCIV_GET_ATTR_CTRL, pciv_get_attr_ctrl },
    { PCIV_START_CTRL, pciv_start_ctrl },
    { PCIV_STOP_CTRL, pciv_stop_ctrl },
    { PCIV_MALLOC_CTRL, pciv_malloc_ctrl },
    { PCIV_FREE_CTRL, pciv_free_ctrl },
    { PCIV_MALLOC_CHN_BUF_CTRL, pciv_malloc_chn_buf_ctrl },
    { PCIV_FREE_CHN_BUF_CTRL, pciv_free_chn_buf_ctrl },
    { 0, TD_NULL },
    { PCIV_GET_WINDOW_BASE_CTRL, pciv_get_window_base_ctrl },
    { PCIV_GET_LOCAL_ID_CTRL, pciv_get_local_id_ctrl },
    { PCIV_ENUM_CHIP_ID_CTRL, pciv_enum_chip_id_ctrl },
    { PCIV_DMA_TASK_CTRL, pciv_dma_task_ctrl },
    { PCIV_BIND_CHN_TO_FD_CTRL, pciv_bind_chn_to_fd_ctrl },
    { PCIV_WINDOW_VB_CREATE_CTRL, pciv_win_vb_create_ctrl },
    { PCIV_WINDOW_VB_DESTROY_CTRL, pciv_win_vb_destroy_ctrl },
    { PCIV_SHOW_CTRL, pciv_show_ctrl },
    { PCIV_HIDE_CTRL, pciv_hide_ctrl },
};

static DEFINE_SEMAPHORE(g_pciv_ioctl_mutex);

#define pciv_ioctl_down_return()                    \
do {                                                \
    td_s32 tmp_;                                    \
    tmp_ = down_interruptible(&g_pciv_ioctl_mutex); \
    if (tmp_) {                                     \
        return tmp_;                                \
    }                                               \
} while (0)

#define pciv_ioctl_up() up(&g_pciv_ioctl_mutex)

static int pciv_open(td_void *private_data)
{
    ot_unused(private_data);
    return 0;
}

static int pciv_close(td_void *private_data)
{
    ot_unused(private_data);
    return 0;
}

static td_s32 pciv_bind_chn_to_fd_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    ot_pciv_chn *chn = (ot_pciv_chn *)arg;

    pciv_check_ptr_return(chn);
    pciv_check_chn_id_return(*chn);

    umap_set_chn(private_data, *((td_u32 *)chn));
    return TD_SUCCESS;
}

static td_s32 pciv_create_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32       ret;
    ot_pciv_attr *attr = (ot_pciv_attr *)arg;
    ot_pciv_chn  chn   = umap_get_chn(private_data);

    pciv_check_ptr_return(attr);

    ret = pciv_create(chn, attr);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV create failed !\n");
    }
    return ret;
}

static td_s32 pciv_destroy_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32      ret;
    ot_pciv_chn chn = umap_get_chn(private_data);

    ot_unused(arg);

    ret = pciv_destroy(chn);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV destroy failed !\n");
    }
    return ret;
}

static td_s32 pciv_set_attr_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32       ret;
    ot_pciv_attr *attr = (ot_pciv_attr *)arg;
    ot_pciv_chn  chn   = umap_get_chn(private_data);

    pciv_check_ptr_return(attr);

    ret = pciv_set_attr(chn, attr);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV set_attr failed !\n");
    }
    return ret;
}

static td_s32 pciv_get_attr_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32       ret;
    ot_pciv_attr *attr = (ot_pciv_attr *)arg;
    ot_pciv_chn  chn   = umap_get_chn(private_data);

    pciv_check_ptr_return(attr);

    ret = pciv_get_attr(chn, attr);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV get_attr failed !\n");
    }
    return ret;
}

static td_s32 pciv_start_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32      ret;
    ot_pciv_chn chn = umap_get_chn(private_data);

    ot_unused(arg);

    ret = pciv_start(chn);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV start failed !\n");
    }
    return ret;
}

static td_s32 pciv_stop_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32      ret;
    ot_pciv_chn chn = umap_get_chn(private_data);

    ot_unused(arg);

    ret = pciv_stop(chn);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV stop failed !\n");
    }
    return ret;
}

static td_s32 pciv_malloc_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32               ret;
    ot_pciv_ioctl_malloc *malloc_buf = (ot_pciv_ioctl_malloc *)arg;

    ot_unused(private_data);
    pciv_check_ptr_return(malloc_buf);

    ret = pciv_malloc(malloc_buf->size, &(malloc_buf->phys_addr));
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV malloc failed !\n");
    }
    return ret;
}

static td_s32 pciv_free_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32         ret;
    td_phys_addr_t *phys_addr = (td_phys_addr_t *)arg;

    ot_unused(private_data);
    pciv_check_ptr_return(phys_addr);

    ret = pciv_free(*phys_addr);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV free failed !\n");
    }
    return ret;
}

static td_s32 pciv_malloc_chn_buf_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32                       ret;
    ot_pciv_ioctl_malloc_chn_buf *malloc_chn_buf = (ot_pciv_ioctl_malloc_chn_buf *)arg;
    ot_pciv_chn                  chn = umap_get_chn(private_data);

    pciv_check_ptr_return(malloc_chn_buf);

    ret = pciv_malloc_chn_buffer(chn, malloc_chn_buf->index, malloc_chn_buf->size,
                                 &(malloc_chn_buf->phys_addr));
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV malloc_chn_buffer failed !\n");
    }
    return ret;
}

static td_s32 pciv_free_chn_buf_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32      ret;
    td_u32      *index = (td_u32 *)arg;
    ot_pciv_chn chn = umap_get_chn(private_data);

    pciv_check_ptr_return(index);

    ret = pciv_free_chn_buffer(chn, *index);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV free_chn_buffer failed !\n");
    }
    return ret;
}

static td_s32 pciv_get_window_base_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32 ret;
    ot_pciv_window_base *base = (ot_pciv_window_base *)arg;

    ot_unused(private_data);
    pciv_check_ptr_return(base);

    ret = pciv_drv_adp_get_window_base(base);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("pciv get window base err! ret:0x%x\n\n", ret);
    }
    return ret;
}

static td_s32 pciv_dma_task_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32 ret;
    td_u32 cpy_size;
    td_u32 i;
    ot_pciv_dma_task *task = (ot_pciv_dma_task *)arg;
    static ot_pciv_dma_blk dma_blk[OT_PCIV_MAX_DMA_BLK];

    ot_unused(private_data);
    pciv_check_ptr_return(task);
    pciv_check_ptr_return(task->dma_blk);

    if (task->blk_cnt > OT_PCIV_MAX_DMA_BLK || task->blk_cnt == 0) {
        pciv_err_trace("task count err,should be [%d, %d]\n", 1, OT_PCIV_MAX_DMA_BLK);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if ((task->is_read != TD_TRUE) && (task->is_read != TD_FALSE)) {
        pciv_err_trace("read:%d is wrong!\n", task->is_read);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    cpy_size = sizeof(ot_pciv_dma_blk) * task->blk_cnt;
    if (osal_copy_from_user(dma_blk, task->dma_blk, cpy_size)) {
        pciv_err_trace("copy from user failed!\n");
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    task->dma_blk = dma_blk;
    for (i = 0; i < task->blk_cnt; i++) {
        if (task->dma_blk[i].size == 0) {
            pciv_err_trace("blk_size:%d is illegal!\n\n", task->dma_blk[i].size);
            return OT_ERR_PCIV_ILLEGAL_PARAM;
        }
    }
    ret = pciv_user_dma_task(task);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV user_dma_task failed !\n");
    }
    return ret;
}

static td_s32 pciv_get_local_id_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32 *local_id = (td_s32 *)arg;

    ot_unused(private_data);
    pciv_check_ptr_return(local_id);

    *local_id = pciv_drv_adp_get_local_id();
    return TD_SUCCESS;
}

static td_s32 pciv_enum_chip_id_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32 ret;
    ot_pciv_enum_chip *chips = (ot_pciv_enum_chip *)arg;

    ot_unused(private_data);
    pciv_check_ptr_return(chips);

    ret = pciv_drv_adp_enum_chip(chips);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV drv_adp enum_chip failed !\n");
    }
    return ret;
}

static td_s32 pciv_win_vb_create_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32 ret;
    ot_pciv_window_vb_cfg *config = (ot_pciv_window_vb_cfg *)arg;

    ot_unused(private_data);
    pciv_check_ptr_return(config);

    ret = pciv_window_vb_create(config);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV window_vb_create failed !\n");
    }
    return ret;
}

static td_s32 pciv_win_vb_destroy_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32 ret;

    ot_unused(arg);
    ot_unused(private_data);

    ret = pciv_window_vb_destroy();
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV window_vb_destroy failed !\n");
    }
    return ret;
}

static td_s32 pciv_hide_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32      ret;
    ot_pciv_chn chn = umap_get_chn(private_data);

    ot_unused(arg);

    ret = pciv_hide(chn, TD_TRUE);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV show failed !\n");
    }
    return ret;
}

static td_s32 pciv_show_ctrl(td_uintptr_t arg, const td_void *private_data)
{
    td_s32      ret;
    ot_pciv_chn chn = umap_get_chn(private_data);

    ot_unused(arg);

    ret = pciv_hide(chn, TD_FALSE);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("PCIV show failed !\n");
    }
    return ret;
}

static long pciv_dev_ioctl(td_u32 cmd, td_ulong argv, const td_void *private_data)
{
    td_s32 ret = OT_ERR_PCIV_ILLEGAL_PARAM;

    /* if the system has received the notice to stop or the system has been stopped */
    if (g_pciv_state != PCIV_STATE_STARTED) {
        return OT_ERR_PCIV_NOT_READY;
    }

    pciv_ioctl_down_return();

    if (_IOC_NR(cmd) >= (sizeof(g_pciv_ioctls) / sizeof(g_pciv_ioctls[0])) ||
        cmd != g_pciv_ioctls[_IOC_NR(cmd)].cmd) {
        pciv_err_trace("Ioctl cmd 0x%x does not exist!\n", cmd);
        ret = OT_ERR_PCIV_NOT_SUPPORT;
    }

    if (g_pciv_ioctls[_IOC_NR(cmd)].func) {
        /* arg: null pointer has been checked in osal layer. */
        ret = g_pciv_ioctls[_IOC_NR(cmd)].func(argv, private_data);
    }

    pciv_ioctl_up();

    return ret;
}

static long pciv_unlocked_ioctl(td_u32 cmd, td_ulong arg, td_void *private_data)
{
    int ret;

    osal_atomic_inc_return(&g_pciv_user_ref);
    ret = pciv_dev_ioctl(cmd, arg, private_data);
    osal_atomic_dec_return(&g_pciv_user_ref);

    return ret;
}

#ifdef CONFIG_COMPAT
static long pciv_compat_ioctl(unsigned int cmd, unsigned long argv, td_void *private_data)
{
    td_uintptr_t arg = (td_uintptr_t)argv;

    switch (cmd) {
        case PCIV_DMA_TASK_CTRL: {
            ot_pciv_dma_task *dma_task = (ot_pciv_dma_task *)arg;
            OT_COMPAT_POINTER(dma_task->dma_blk, td_void *);
            break;
        }
        default: {
            break;
        }
    }

    return pciv_unlocked_ioctl(cmd, (unsigned long)arg, private_data);
}
#endif

td_s32 pciv_ext_init(td_void *p)
{
    ot_unused(p);

    /* as long as it is not in stop state,it will not need to initialize,only return success */
    if (g_pciv_state != PCIV_STATE_STOPPED) {
        return TD_SUCCESS;
    }

    if (pciv_init() != TD_SUCCESS) {
        OT_ERR_TRACE(OT_ID_PCIV, "pciv_init failed\n");
        return TD_FAILURE;
    }

    OT_INFO_TRACE(OT_ID_PCIV, "pciv_init success\n");
    g_pciv_state = PCIV_STATE_STARTED;
    return TD_SUCCESS;
}

td_void pciv_ext_exit(td_void)
{
    /* if it is stopped ,return success,else the exit function is been called */
    if (g_pciv_state == PCIV_STATE_STOPPED) {
        return;
    }
    pciv_exit();
    g_pciv_state = PCIV_STATE_STOPPED;
}

static td_void pciv_notify(mod_notice_id notice)
{
    ot_unused(notice);

    g_pciv_state = PCIV_STATE_STOPPING; /* the new IOCT is not continue received */
    /* pay attention to wake all user */
    return;
}

static td_void pciv_query_state(mod_state *state)
{
    if (osal_atomic_read(&g_pciv_user_ref) == 0) {
        *state = MOD_STATE_FREE;
    } else {
        *state = MOD_STATE_BUSY;
    }
    return;
}

static td_u32 pciv_get_ver_magic(td_void)
{
    return VERSION_MAGIC;
}

static umap_module g_pciv_module = {
    .mod_id = OT_ID_PCIV,
    .mod_name = "pciv",

    .pfn_init = pciv_ext_init,
    .pfn_exit = pciv_ext_exit,
    .pfn_query_state = pciv_query_state,
    .pfn_notify = pciv_notify,
    .pfn_ver_checker = pciv_get_ver_magic,
    .data = TD_NULL,
};

static struct osal_fileops g_pciv_fops = {
    .open = pciv_open,
    .unlocked_ioctl = pciv_unlocked_ioctl,
    .release = pciv_close,
#ifdef CONFIG_COMPAT
    .compat_ioctl = pciv_compat_ioctl
#endif
};

static int __init pciv_mod_init(td_void)
{
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_proc_entry_t *proc = TD_NULL;

    proc = osal_create_proc_entry(PROC_ENTRY_PCIV, NULL);
    if (proc == TD_NULL) {
        osal_printk("PCIV create proc error\n");
        goto proc_fail;
    }
    proc->read = pciv_proc_show;
#endif

    g_pciv_device = osal_createdev(UMAP_DEVNAME_PCIV_BASE);
    if (g_pciv_device == TD_NULL) {
        osal_printk("pciv create dev failed\n");
        goto create_fail;
    }
    g_pciv_device->fops = &g_pciv_fops;
    g_pciv_device->minor = UMAP_PCIV_MINOR_BASE;
    if (osal_registerdevice(g_pciv_device) < 0) {
        osal_printk("register pciv device err.\n");
        goto register_dev_fail;
    }

    if (cmpi_register_module(&g_pciv_module)) {
        osal_printk("register pciv module err.\n");
        goto register_mod_fail;
    }

    if (osal_atomic_init(&g_pciv_user_ref) < 0) {
        osal_printk("osal_atomic_init failed\n");
        goto atomic_init_fail;
    }
    osal_atomic_set(&g_pciv_user_ref, 0);
    osal_printk("load pciv.ko ....OK!\n");
    return TD_SUCCESS;
atomic_init_fail:
    cmpi_unregister_module(OT_ID_PCIV);
register_mod_fail:
    osal_deregisterdevice(g_pciv_device);
register_dev_fail:
    osal_destroydev(g_pciv_device);
    g_pciv_device = TD_NULL;
create_fail:
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_PCIV, NULL);
proc_fail:
#endif
    osal_printk("load pciv.ko ....fail!\n");
    return TD_FAILURE;
}

static td_void __exit pciv_mod_exit(td_void)
{
    cmpi_unregister_module(OT_ID_PCIV);
    osal_atomic_destroy(&g_pciv_user_ref);
    osal_deregisterdevice(g_pciv_device);
    osal_destroydev(g_pciv_device);
    g_pciv_device = TD_NULL;
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_PCIV, NULL);
#endif
    osal_printk("unload pciv.ko ....OK!\n");
    return;
}

module_init(pciv_mod_init);
module_exit(pciv_mod_exit);

MODULE_AUTHOR("OT_MPP GRP");
MODULE_LICENSE("GPL");
MODULE_VERSION(OT_MPP_VERSION);
