/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "pciv.h"

#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include "ot_osal.h"
#include "osal_mmz.h"
#include "pciv_drvadp.h"
#include "pciv_fmwext.h"
#include "mkp_pciv.h"
#include "sys_ext.h"
#include "securec.h"

#define PCIV_MAX_DMA_TASK (1 << 5)
#define PCIV_MAX_WINDOW_BUFF_SIZE (7 * 1024 * 1024)

#define PCIV_MIN_BUF_NUM        1
#define PCIV_SCHEDULE_TIMEOUT   2
#define PCIV_SLEEP_TIME         10
#define PCIV_WAIT_ENENT_TIMEOUT 200

#define PCIV_SPIN_LOCK   spin_lock_irqsave(&g_pciv_lock, flags)
#define PCIV_SPIN_UNLOCK spin_unlock_irqrestore(&g_pciv_lock, flags)

#define pciv_mutex_down_return(sem)    \
do {                                   \
    if (down_interruptible(&(sem))) {  \
        return 0 - ERESTARTSYS;        \
    }                                  \
} while (0)

#define pciv_mutex_up(sem) up(&(sem))

typedef struct {
    struct list_head    list;
    wait_queue_head_t   wq_dma_done;
    td_bool             dma_done;
} pciv_user_dma_node;

pciv_chn_ctx                g_chn_ctx[OT_PCIV_MAX_CHN_NUM] = { 0 };
static pciv_user_dma_node   g_user_dma_pool[PCIV_MAX_DMA_TASK] = { 0 };
static struct list_head     g_list_head_user_dma;

static spinlock_t g_pciv_lock;

td_s32 pciv_check_attr(const ot_pciv_attr *attr)
{
    td_u32 i;

    /* check the number of the buffer is valid */
    if ((attr->blk_cnt < PCIV_MIN_BUF_NUM) || (attr->blk_cnt > OT_PCIV_MAX_BUF_NUM)) {
        pciv_err_trace("buffer count(%u) not invalid,should in [1,%u]\n", attr->blk_cnt, OT_PCIV_MAX_BUF_NUM);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    /* check the physical address */
    for (i = 0; i < attr->blk_cnt; i++) {
        if (!attr->phys_addr[i]) {
            pciv_err_trace("attr->phys_addr[%d]:0x%lx invalid\n", i, (td_ulong)attr->phys_addr[i]);
            return OT_ERR_PCIV_ILLEGAL_PARAM;
        }
    }
    /* check the valid of the remote device */
    if ((attr->remote_obj.chip_id < 0) ||
        (attr->remote_obj.chip_id >= OT_PCIV_MAX_CHIP_NUM) ||
        (attr->remote_obj.pciv_chn < 0) ||
        (attr->remote_obj.pciv_chn >= OT_PCIV_MAX_CHN_NUM)) {
        pciv_err_trace("invalid remote object(%d,%d)\n", attr->remote_obj.chip_id, attr->remote_obj.pciv_chn);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 pciv_is_support(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id)
{
    td_s32 i;

    if (local_id != 0) {
        /* slave chip can not bind slave chip */
        if (attr->remote_obj.chip_id != 0) {
            pciv_err_trace("a slave chip bind the other slave chip or it self, chip_id of src:%d, chip_id of dst:%d\n",
                local_id, attr->remote_obj.chip_id);
            return OT_ERR_PCIV_NOT_SUPPORT;
        }

        /* slave's different chn bind host's same chn is not support */
        for (i = 0; i < OT_PCIV_MAX_CHN_NUM; i++) {
            if (g_chn_ctx[i].is_create == TD_FALSE || pciv_chn == i) {
                continue;
            }
            if (attr->remote_obj.pciv_chn == g_chn_ctx[i].pciv_attr.remote_obj.pciv_chn) {
                pciv_err_trace("two slave's chns bind the host's same chn, chn%d, attr->remote_obj.chn%d\n",
                    pciv_chn, attr->remote_obj.pciv_chn);
                return OT_ERR_PCIV_NOT_SUPPORT;
            }
        }
    }

    /* host and slave remote_obj.chip_id cannot be itsself */
    if (attr->remote_obj.chip_id == local_id) {
        pciv_err_trace("remote_obj chip_id can not be itself! remote_obj.chip_id:%d local_id:%d\n",
            attr->remote_obj.chip_id, local_id);
        return OT_ERR_PCIV_NOT_SUPPORT;
    }
    return TD_SUCCESS;
}

static td_s32 pciv_reset_chn(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, pciv_chn_ctx *chn)
{
    td_ulong        flags;
    td_s32          ret;
    td_u32          i;
    pciv_gap_record gap_record;

    PCIV_SPIN_LOCK;

    for (i = 0; i < OT_PCIV_MAX_BUF_NUM; i++) {
        chn->buf_status[i] = BUFF_BUSY;
    }

    for (i = 0; i < attr->blk_cnt; i++) {
        if (attr->phys_addr[i] != 0) {
            chn->buf_status[i] = BUFF_FREE;
        }
    }

    ret = memcpy_s(&chn->pciv_attr, sizeof(ot_pciv_attr), attr, sizeof(ot_pciv_attr));
    if (ret != EOK) {
        pciv_err_trace("copy pciv_attr failed!\n");
        PCIV_SPIN_UNLOCK;
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    chn->is_start   = TD_FALSE;
    chn->is_config  = TD_TRUE;
    chn->is_create  = TD_TRUE;
    chn->can_recv   = TD_FALSE;
    chn->is_hide    = TD_FALSE; /* reset to show */
    chn->get_cnt    = 0;
    chn->send_cnt   = 0;
    chn->resp_cnt   = 0;
    chn->lost_cnt   = 0;
    chn->notify_cnt = 0;

    (td_void)memset_s(&gap_record, sizeof(pciv_gap_record), 0, sizeof(pciv_gap_record));
    ret = pciv_drv_adp_set_gap_record(pciv_chn,  &gap_record);
    if (ret != TD_SUCCESS) {
        PCIV_SPIN_UNLOCK;
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    PCIV_SPIN_UNLOCK;
    return TD_SUCCESS;
}

td_s32 pciv_create(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr)
{
    td_ulong        flags;
    td_s32          ret;
    td_s32          local_id;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    pciv_mutex_down_return(g_chn_ctx[pciv_chn].pciv_mutex);

    PCIV_SPIN_LOCK;
    chn = &g_chn_ctx[pciv_chn];
    if (chn->is_create == TD_TRUE) {
        pciv_err_trace("chn%d has been created\n", pciv_chn);
        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return OT_ERR_PCIV_EXIST;
    }

    if (pciv_check_attr(attr) != TD_SUCCESS) {
        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    local_id = pciv_drv_adp_get_local_id();
    PCIV_SPIN_UNLOCK;

    ret = pciv_is_support(pciv_chn, attr, local_id);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }

    ret = pciv_firmware_create(pciv_chn, attr, local_id);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }

    ret = pciv_reset_chn(pciv_chn, attr, chn);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }

    pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);

    pciv_info_trace("chn%d create ok\n", pciv_chn);
    return ret;
}

td_s32 pciv_destroy(ot_pciv_chn pciv_chn)
{
    td_s32          ret;
    td_ulong        flags;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    pciv_mutex_down_return(g_chn_ctx[pciv_chn].pciv_mutex);
    PCIV_SPIN_LOCK;
    chn = &g_chn_ctx[pciv_chn];
    if (chn->is_create == TD_FALSE) {
        pciv_notice_trace("chn%d has not been created\n", pciv_chn);
        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return TD_SUCCESS;
    }
    if (chn->is_start == TD_TRUE) {
        pciv_err_trace("chn%d should stop first then destroy \n", pciv_chn);
        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return OT_ERR_PCIV_NOT_PERM;
    }
    PCIV_SPIN_UNLOCK;

    ret = pciv_firmware_destroy(pciv_chn);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }
    PCIV_SPIN_LOCK;
    chn->is_create = TD_FALSE;
    PCIV_SPIN_UNLOCK;
    pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);

    pciv_info_trace("chn%d destroy ok\n", pciv_chn);
    return TD_SUCCESS;
}

static td_s32 pciv_check_state_and_attr(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, const pciv_chn_ctx *chn)
{
    if (chn->is_create == TD_FALSE) {
        pciv_err_trace("chn%d has not been created\n", pciv_chn);
        return OT_ERR_PCIV_UNEXIST;
    }

    if (chn->is_start == TD_TRUE) {
        pciv_err_trace("chn%d is running\n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }

    if (pciv_check_attr(attr) != TD_SUCCESS) {
        pciv_err_trace("chn%d attribute error\n", pciv_chn);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

td_s32 pciv_set_attr(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr)
{
    td_ulong        flags;
    td_s32          ret;
    td_s32          local_id;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    pciv_mutex_down_return(g_chn_ctx[pciv_chn].pciv_mutex);

    chn = &g_chn_ctx[pciv_chn];

    ret = pciv_check_state_and_attr(pciv_chn, attr, chn);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }

    local_id = pciv_drv_adp_get_local_id();

    ret = pciv_is_support(pciv_chn, attr, local_id);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }

    ret = pciv_firmware_set_attr(pciv_chn, attr, local_id);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }
    PCIV_SPIN_LOCK;
    ret = memcpy_s(&chn->pciv_attr, sizeof(ot_pciv_attr), attr, sizeof(ot_pciv_attr));
    if (ret != EOK) {
        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        pciv_err_trace("copy pciv_attr failed!\n");
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    chn->is_config = TD_TRUE;
    PCIV_SPIN_UNLOCK;

    pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);

    pciv_info_trace("chn%d set attr ok\n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_get_attr(ot_pciv_chn pciv_chn, ot_pciv_attr *attr)
{
    td_s32 ret;
    pciv_chn_ctx *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    chn = &g_chn_ctx[pciv_chn];

    if (chn->is_create == TD_FALSE) {
        pciv_err_trace("chn%d has not been created\n", pciv_chn);

        return OT_ERR_PCIV_UNEXIST;
    }

    if (chn->is_config != TD_TRUE) {
        pciv_err_trace("attr of chn%d has not been set\n", pciv_chn);

        return OT_ERR_PCIV_NOT_PERM;
    }

    ret = memcpy_s(attr, sizeof(ot_pciv_attr), &chn->pciv_attr, sizeof(ot_pciv_attr));
    if (ret != EOK) {
        pciv_err_trace("copy pciv_attr failed!\n");
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 pciv_start(ot_pciv_chn pciv_chn)
{
    td_ulong            flags;
    td_u32              i;
    td_s32              ret;
    td_s32              local_id;
    ot_pciv_remote_obj  remote_obj;
    pciv_pic            recv_pic;
    pciv_chn_ctx        *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    pciv_mutex_down_return(g_chn_ctx[pciv_chn].pciv_mutex);
    PCIV_SPIN_LOCK;

    chn = &g_chn_ctx[pciv_chn];
    if (chn->is_create != TD_TRUE) {
        pciv_err_trace("chn%d not create\n", pciv_chn);

        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return OT_ERR_PCIV_UNEXIST;
    }

    if (chn->is_start == TD_TRUE) {
        pciv_info_trace("chn%d is running\n", pciv_chn);

        PCIV_SPIN_UNLOCK;
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return TD_SUCCESS;
    }

    remote_obj.pciv_chn = chn->pciv_attr.remote_obj.pciv_chn;
    remote_obj.chip_id = 0;
    recv_pic.filed = -1;
    local_id = pciv_drv_adp_get_local_id();
    if (local_id != 0) {
        for (i = 0; i < chn->pciv_attr.blk_cnt; i++) {
            chn->buf_status[i] = BUFF_FREE;
        }
        pciv_drv_adp_send_msg(&remote_obj, PCIV_MSGTYPE_FREE, &recv_pic);
    }
    chn->can_recv = TD_TRUE;

    PCIV_SPIN_UNLOCK;
    ret = pciv_firmware_start(pciv_chn);
    if (ret != TD_SUCCESS) {
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }
    PCIV_SPIN_LOCK;
    chn->is_start = TD_TRUE;
    PCIV_SPIN_UNLOCK;
    pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);

    pciv_info_trace("chn%d start ok\n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_stop(ot_pciv_chn pciv_chn)
{
    td_ulong        flags;
    td_s32          ret;
    td_s32          local_id;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    pciv_mutex_down_return(g_chn_ctx[pciv_chn].pciv_mutex);

    chn = &g_chn_ctx[pciv_chn];
    if (chn->is_create != TD_TRUE) {
        pciv_err_trace("chn%d not create\n", pciv_chn);

        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return OT_ERR_PCIV_UNEXIST;
    }
    if (chn->is_start != TD_TRUE) {
        pciv_info_trace("chn%d has stopped\n", pciv_chn);

        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return TD_SUCCESS;
    }

    PCIV_SPIN_LOCK;
    /* first set the stop flag */
    chn->is_start = TD_FALSE;
    PCIV_SPIN_UNLOCK;

    /* wait for the PCI task finished */
    local_id = pciv_drv_adp_get_local_id();
    if (local_id != 0) {
        while (chn->send_cnt != chn->resp_cnt) {
            pciv_drv_adp_start_dma_task();
            set_current_state(TASK_INTERRUPTIBLE);
            (td_void)schedule_timeout(PCIV_SCHEDULE_TIMEOUT);
            continue;
        }
    }

    /* then stop the media related work */
    ret = pciv_firmware_stop(pciv_chn);
    if (ret) {
        PCIV_SPIN_LOCK;
        chn->is_start = TD_TRUE;
        PCIV_SPIN_UNLOCK;

        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return ret;
    }

    PCIV_SPIN_LOCK;
    chn->can_recv = TD_FALSE;
    PCIV_SPIN_UNLOCK;

    pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);

    pciv_info_trace("chn%d stop ok\n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_hide(ot_pciv_chn pciv_chn, td_bool hide)
{
    pciv_chn_ctx *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    pciv_mutex_down_return(g_chn_ctx[pciv_chn].pciv_mutex);

    chn = &g_chn_ctx[pciv_chn];
    if (chn->is_create != TD_TRUE) {
        pciv_err_trace("chn%d not created\n", pciv_chn);
        pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
        return OT_ERR_PCIV_UNEXIST;
    }

    chn->is_hide = hide;

    pciv_info_trace("chn%d hide%d ok\n", pciv_chn, hide);

    pciv_mutex_up(g_chn_ctx[pciv_chn].pciv_mutex);
    return TD_SUCCESS;
}

/* only when  the slave chip reciver data or image, we should config the window vb */
td_s32 pciv_window_vb_create(const ot_pciv_window_vb_cfg *cfg)
{
    ot_pciv_window_vb_cfg vb_cfg;
    td_u32                i, j, size, cnt;
    td_s32                local_id;
    td_s32                ret;

    /* on the host chip, the action creating the special region is not supported */
    local_id = pciv_drv_adp_get_local_id();
    if (local_id == 0) {
        pciv_err_trace("master chip does not support!\n");
        return OT_ERR_PCIV_NOT_SUPPORT;
    }

    if (cfg->pool_cnt > OT_PCIV_MAX_VB_CNT || cfg->pool_cnt == 0) {
        pciv_err_trace("pool_cnt:%d is illegal,which should between [1, %d]!\n",
            cfg->pool_cnt, OT_PCIV_MAX_VB_CNT);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    /* rank the pool in accordance with the size, in convenient of the used of follow-up */
    ret = memcpy_s(&vb_cfg, sizeof(ot_pciv_window_vb_cfg), cfg, sizeof(ot_pciv_window_vb_cfg));
    if (ret != EOK) {
        pciv_err_trace("copy vb_cfg failed!\n");
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    for (i = 0; i < cfg->pool_cnt; i++) {
        if ((cfg->blk_size[i] == 0) || (cfg->blk_size[i] > PCIV_MAX_WINDOW_BUFF_SIZE)) {
            pciv_err_trace("blk_size[%d]:%d is illegal\n", i, cfg->blk_size[i]);
            return OT_ERR_PCIV_ILLEGAL_PARAM;
        }
        if (cfg->blk_cnt[i] == 0) {
            pciv_err_trace("blk_cnt[%d]:%d is illegal\n",
                i, cfg->blk_cnt[i]);
            return OT_ERR_PCIV_ILLEGAL_PARAM;
        }
    }
    for (i = 0; i < cfg->pool_cnt - 1; i++) {
        for (j = i + 1; j < cfg->pool_cnt; j++) {
            if (vb_cfg.blk_size[j] < vb_cfg.blk_size[i]) {
                size = vb_cfg.blk_size[i];
                cnt = vb_cfg.blk_cnt[i];

                vb_cfg.blk_size[i] = vb_cfg.blk_size[j];
                vb_cfg.blk_cnt[i] = vb_cfg.blk_cnt[j];

                vb_cfg.blk_size[j] = size;
                vb_cfg.blk_cnt[j] = cnt;
            }
        }
    }

    return pciv_firmware_window_vb_create(&vb_cfg);
}

td_s32 pciv_window_vb_destroy(td_void)
{
    td_s32 local_id;

    /* in the master chip,not support destroy the special area */
    local_id = pciv_drv_adp_get_local_id();
    if (local_id == 0) {
        pciv_err_trace("master chip does not support!\n");
        return OT_ERR_PCIV_NOT_SUPPORT;
    }

    return pciv_firmware_window_vb_destroy();
}

td_s32 pciv_malloc(td_u32 size, td_phys_addr_t *phys_addr)
{
    td_s32 local_id = pciv_drv_adp_get_local_id();

    return pciv_firmware_malloc(size, local_id, phys_addr);
}

td_s32 pciv_free(td_phys_addr_t phys_addr)
{
    return pciv_firmware_free(phys_addr);
}

td_s32 pciv_malloc_chn_buffer(ot_pciv_chn pciv_chn, td_u32 index, td_u32 size, td_phys_addr_t *phys_addr)
{
    td_s32 local_id = pciv_drv_adp_get_local_id();

    return pciv_firmware_malloc_chn_buffer(pciv_chn, index, size, local_id, phys_addr);
}

td_s32 pciv_free_chn_buffer(ot_pciv_chn pciv_chn, td_u32 index)
{
    td_s32 local_id = pciv_drv_adp_get_local_id();

    return pciv_firmware_free_chn_buffer(pciv_chn, index, local_id);
}

td_void pciv_user_dma_done(pciv_send_task *task)
{
    pciv_user_dma_node *user_dma_node = TD_NULL;

    /* assert the finished DMA task is the last one */
    ot_assert((task->prv_data[0] + 1) == task->prv_data[1]);

    user_dma_node = (pciv_user_dma_node *)(td_uintptr_t)task->prv_data[2]; /* 2: save the pciv dma node info */
    user_dma_node->dma_done = TD_TRUE;
    wake_up(&user_dma_node->wq_dma_done);
}

static td_s32 pciv_user_dma_add_task(const ot_pciv_dma_task *task)
{
    td_ulong            flags;
    td_u32              i;
    pciv_send_task      pci_task;
    td_s32              ret;
    pciv_user_dma_node  *user_dma_node  = TD_NULL;

    PCIV_SPIN_LOCK;
    user_dma_node = list_entry(g_list_head_user_dma.next, pciv_user_dma_node, list);
    list_del(g_list_head_user_dma.next);
    PCIV_SPIN_UNLOCK;

    user_dma_node->dma_done = TD_FALSE;

    for (i = 0; i < task->blk_cnt; i++) {
        pci_task.src_phys_addr = task->dma_blk[i].src_addr;
        pci_task.dst_phys_addr = task->dma_blk[i].dst_addr;
        pci_task.len    = task->dma_blk[i].size;
        pci_task.read   = task->is_read;
        pci_task.prv_data[0] = i;
        pci_task.prv_data[1] = task->blk_cnt;
        pci_task.prv_data[2] = (td_u64)(td_uintptr_t)user_dma_node; /* 2:prv_data[2] save the pciv dma node info */
        pci_task.call_back = TD_NULL;

        /* if this is the last task node, we set the callback */
        if ((i + 1) == task->blk_cnt) {
            pci_task.call_back = pciv_user_dma_done;
        }
        ret = pciv_drv_adp_add_dma_task(&pci_task);
        if (ret != TD_SUCCESS) {
            break;
        }
    }

    if (ret == TD_SUCCESS) {
        td_s32 time_left;
        time_left = wait_event_timeout(user_dma_node->wq_dma_done,
            (user_dma_node->dma_done == TD_TRUE), PCIV_WAIT_ENENT_TIMEOUT);
        if (time_left == 0) {
            pciv_warn_trace("wait_event_timeout \n");
            ret = OT_ERR_PCIV_TIMEOUT;
        }
    }

    PCIV_SPIN_LOCK;
    list_add_tail(&user_dma_node->list, &g_list_head_user_dma);
    PCIV_SPIN_UNLOCK;
    return ret;
}

td_s32 pciv_user_dma_task(const ot_pciv_dma_task *task)
{
    td_u32 i;
    td_s32 ret;
    td_s32 local_id;

    pciv_check_ptr_return(task);
    pciv_check_ptr_return(task->dma_blk);

    if (list_empty(&g_list_head_user_dma)) {
        return OT_ERR_PCIV_BUSY;
    }

    if ((task->is_read != TD_TRUE) && (task->is_read != TD_FALSE)) {
        pciv_err_trace("DMA size is illeage! is_read:%d\n", task->is_read);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    local_id = pciv_drv_adp_get_local_id();
    if (local_id == 0) {
        for (i = 0; i < task->blk_cnt; i++) {
            if (pciv_drv_adp_addr_check(&task->dma_blk[i], task->is_read) != TD_SUCCESS) {
                pciv_err_trace("Error slave addr and size! task->dma_blk[%d]: \
                    src_addr:0x%lx, dst_addr:0x%lx, size:0x%x\n",
                    i,
                    (td_ulong)task->dma_blk[i].src_addr,
                    (td_ulong)task->dma_blk[i].dst_addr,
                    task->dma_blk[i].size);
                return OT_ERR_PCIV_ILLEGAL_PARAM;
            }
        }
    }

    for (i = 0; i < task->blk_cnt; i++) {
        if ((task->dma_blk[i].src_addr == 0) || (task->dma_blk[i].dst_addr == 0) ||
            ((task->dma_blk[i].src_addr & 0x3) != 0) || ((task->dma_blk[i].dst_addr & 0x3) != 0)) {
            pciv_err_trace(" src_addr:0x%lx dst_addr:0x%lx is illeage! \n",
                (td_ulong)task->dma_blk[i].src_addr, (td_ulong)task->dma_blk[i].dst_addr);
            return OT_ERR_PCIV_ILLEGAL_PARAM;
        }
    }

    ret = pciv_user_dma_add_task(task);
    return ret;
}

static td_void pciv_check_notify_cnt(ot_pciv_chn pciv_chn, td_u32 index, td_u32 cnt)
{
    pciv_chn_ctx *chn = &g_chn_ctx[pciv_chn];

    if (cnt == 0) {
        chn->notify_cnt = 0;
    } else {
        chn->notify_cnt++;
        if (cnt != chn->notify_cnt) {
            pciv_warn_trace("warnning: chn%d, read_done msg_seq -> (%u,%u),bufindex:%u \n",
                pciv_chn, cnt, chn->notify_cnt, index);
        }
    }
}

/*
 * when received the message of release the shared memory,
 * this interface is been called to set the memory flag to idle
 */
td_s32 pciv_free_share_buf(ot_pciv_chn pciv_chn, td_u32 index, td_u32 cnt)
{
    pciv_chn_ctx    *chn = TD_NULL;
    td_ulong        flags;

    pciv_check_chn_id_return(pciv_chn);

    chn = &g_chn_ctx[pciv_chn];

    PCIV_SPIN_LOCK;
    if (chn->is_start != TD_TRUE) {
        pciv_err_trace("chn%d not start!\n", pciv_chn);
        PCIV_SPIN_UNLOCK;
        return OT_ERR_PCIV_UNEXIST;
    }

    if (index >= OT_PCIV_MAX_BUF_NUM) {
        pciv_err_trace("buffer index %u is too larger!\n", index);
        PCIV_SPIN_UNLOCK;
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    ot_assert(index < chn->pciv_attr.blk_cnt);

    /* check the serial number of the message is same or not with the local serial number */
    pciv_check_notify_cnt(pciv_chn, index, cnt);

    /* set the buffer flag to idle */
    chn->buf_status[index] = BUFF_FREE;
    PCIV_SPIN_UNLOCK;

    return TD_SUCCESS;
}

td_s32 pciv_free_all_buf(ot_pciv_chn pciv_chn)
{
    td_u32          i;
    td_ulong        flags;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    chn = &g_chn_ctx[pciv_chn];

    PCIV_SPIN_LOCK;
    if (chn->is_create != TD_TRUE) {
        PCIV_SPIN_UNLOCK;
        return TD_SUCCESS;
    }

    for (i = 0; i < chn->pciv_attr.blk_cnt; i++) {
        chn->buf_status[i] = BUFF_FREE;
    }

    chn->can_recv = TD_TRUE;
    PCIV_SPIN_UNLOCK;
    return TD_SUCCESS;
}

/* when start DMA transmission, the interface must be called first to get a valid shared buffer */
static td_s32 pciv_get_share_buf(ot_pciv_chn pciv_chn, td_u32 *cur_index)
{
    td_u32          i;
    pciv_chn_ctx    *chn = TD_NULL;

    chn = &g_chn_ctx[pciv_chn];
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
    for (i = 0; i < chn->pciv_attr.blk_cnt; i++) {
        if (chn->buf_status[i] == BUFF_HOLD) {
            *cur_index = i;
            return TD_SUCCESS;
        }
    }
#endif

    for (i = 0; i < chn->pciv_attr.blk_cnt; i++) {
        if (chn->buf_status[i] == BUFF_FREE) {
            *cur_index = i;
            return TD_SUCCESS;
        }
    }

    return TD_FAILURE;
}

#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
static td_s32 pciv_get_share_buf_state(ot_pciv_chn pciv_chn)
{
    td_ulong        flags;
    td_u32          i;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    PCIV_SPIN_LOCK;
    chn = &g_chn_ctx[pciv_chn];
    for (i = 0; i < chn->pciv_attr.blk_cnt; i++) {
        if (chn->buf_status[i] == BUFF_FREE) {
            chn->buf_status[i] = BUFF_HOLD;
            PCIV_SPIN_UNLOCK;
            return TD_SUCCESS;
        }
    }
    PCIV_SPIN_UNLOCK;
    return TD_FAILURE;
}

static td_s32 pciv_set_share_buf_state(ot_pciv_chn pciv_chn)
{
    td_ulong        flags;
    td_u32          i;
    pciv_chn_ctx    *chn = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);

    PCIV_SPIN_LOCK;
    chn = &g_chn_ctx[pciv_chn];
    for (i = 0; i < chn->pciv_attr.blk_cnt; i++) {
        if (chn->buf_status[i] == BUFF_HOLD) {
            chn->buf_status[i] = BUFF_FREE;
            break;
        }
    }
    PCIV_SPIN_UNLOCK;
    return TD_SUCCESS;
}
#endif

td_void pciv_src_pic_send_done(pciv_send_task *task)
{
    td_s32          ret;
    ot_pciv_chn     pciv_chn;
    pciv_chn_ctx    *chn = TD_NULL;
    pciv_pic        recv_pic;
    pciv_pic        src_pic;

    pciv_chn = (ot_pciv_chn)task->prv_data[0];
    ot_assert((pciv_chn >= 0) && (pciv_chn < OT_PCIV_MAX_CHN_NUM));

    chn = &g_chn_ctx[pciv_chn];

    ret = memcpy_s(&recv_pic, sizeof(pciv_pic), (pciv_pic *)task->pv_prv_data, sizeof(pciv_pic));
    if (ret != EOK) {
        /* Release all resources even if the copy failed */
        pciv_err_trace("copy pciv_pic data failed!\n");
    } else {
        if (chn->is_start == TD_TRUE) {
            (void)pciv_drv_adp_dma_end_notify(&chn->pciv_attr.remote_obj, &recv_pic);
        } else {
            chn->buf_status[recv_pic.index] = BUFF_FREE;
        }
    }

    kfree(task->pv_prv_data);
    task->pv_prv_data = TD_NULL;

    src_pic.phys_addr = task->src_phys_addr;
    src_pic.pool_id = (td_u32)task->prv_data[1];
    (td_void)pciv_src_pic_free(pciv_chn, &src_pic);

    chn->resp_cnt++;

    return;
}

static td_s32 pciv_src_pic_send_pre(ot_pciv_chn pciv_chn, const pciv_pic *src_pic,
    td_u32 *cur_index, pciv_pic **recv_pic)
{
    td_s32          ret;
    pciv_chn_ctx    *chn = &g_chn_ctx[pciv_chn];

    if (chn->is_start != TD_TRUE) {
        pciv_err_trace("chn: %d is not enable!\n", pciv_chn);
        return TD_FAILURE;
    }

    ot_assert(src_pic->src_type < PCIV_BIND_BUTT);
    ot_assert(src_pic->filed < OT_VIDEO_FIELD_BUTT);

    chn->get_cnt++;

    ret = pciv_get_share_buf(pciv_chn, cur_index);
    if (ret != TD_SUCCESS) {
        chn->lost_cnt++;
        pciv_err_trace("no free buf, chn%d,src_type:%d\n", pciv_chn, src_pic->src_type);
        return OT_ERR_PCIV_NO_BUF;
    }

    *recv_pic = (pciv_pic *)kmalloc(sizeof(pciv_pic), GFP_ATOMIC);
    if (*recv_pic == TD_NULL) {
        pciv_emerg_trace("kmalloc pciv_recvpic err! chn%d\n", pciv_chn);
        chn->lost_cnt++;
        return OT_ERR_PCIV_NO_MEM;
    }
    return TD_SUCCESS;
}

static td_s32 pciv_set_recv_pic_info(const pciv_chn_ctx *chn, td_s32 cur_index,
    const pciv_pic *src_pic, pciv_pic *recv_pic)
{
    td_s32 ret;

    ret = memcpy_s(recv_pic, sizeof(pciv_pic), src_pic, sizeof(pciv_pic));
    if (ret != EOK) {
        pciv_err_trace("copy pciv_pic failed!\n");
        return TD_FAILURE;
    }
    recv_pic->phys_addr = 0;
    recv_pic->pool_id   = 0;
    recv_pic->index     = cur_index;
    recv_pic->cnt       = chn->send_cnt;
    recv_pic->pts       = src_pic->pts;
    return TD_SUCCESS;
}

/* after dealing with the source image,the interface is auto called to send
the image to the PCI target by PCI DMA mode */
td_s32 pciv_src_pic_send(ot_pciv_chn pciv_chn, const pciv_pic *src_pic)
{
    td_ulong        flags;
    td_s32          ret;
    td_u32          cur_index;
    pciv_send_task  pci_task;
    pciv_chn_ctx    *chn        = TD_NULL;
    pciv_pic        *recv_pic   = TD_NULL;

    pciv_check_chn_id_return(pciv_chn);
    pciv_check_ptr_return(src_pic);

    /* pay attention to the possibility of the pciv and vfwm called each other */
    PCIV_SPIN_LOCK;
    ret = pciv_src_pic_send_pre(pciv_chn, src_pic, &cur_index, &recv_pic);
    if (ret != TD_SUCCESS) {
        PCIV_SPIN_UNLOCK;
        return ret;
    }

    chn = &g_chn_ctx[pciv_chn];
    ret = pciv_set_recv_pic_info(chn, cur_index, src_pic, recv_pic);
    if (ret != TD_SUCCESS) {
        kfree(recv_pic);
        chn->lost_cnt++;
        PCIV_SPIN_UNLOCK;
        return ret;
    }

    /* hide the channel,that is the PCIV channel will not send the image to the
    target by DMA mode, only go on message-based communication */
    pci_task.len = chn->is_hide ? 0 : chn->pciv_attr.blk_size;
    pci_task.src_phys_addr = src_pic->phys_addr;
    pci_task.dst_phys_addr = chn->pciv_attr.phys_addr[cur_index];
    pci_task.read = TD_FALSE;
    pci_task.prv_data[0] = pciv_chn;            /* channel num */
    pci_task.prv_data[1] = src_pic->pool_id; /* src image pool_id */
    pci_task.pv_prv_data = (td_void *)recv_pic;
    pci_task.call_back = pciv_src_pic_send_done; /* register PCI DMA finished callback */
    ret = pciv_drv_adp_add_dma_task(&pci_task);
    if (ret != TD_SUCCESS) {
        pciv_emerg_trace("DMA task err! chn%d\n", pciv_chn);
        kfree(recv_pic);
        chn->lost_cnt++;
        PCIV_SPIN_UNLOCK;
        return ret;
    }

    /* set the serial number of the buffer not idle state */
    ot_assert((chn->buf_status[cur_index] == BUFF_HOLD) || (chn->buf_status[cur_index] == BUFF_FREE));
    chn->buf_status[cur_index] = BUFF_BUSY;
    chn->send_cnt++;
    PCIV_SPIN_UNLOCK;

    return TD_SUCCESS;
}

/* after the PCV DMA task finished,this interface is been called to release the image buffer */
td_s32 pciv_src_pic_free(ot_pciv_chn pciv_chn, const pciv_pic *src_pic)
{
    return pciv_firmware_src_pic_free(pciv_chn, src_pic);
}

/*
 * after receiving the image through the chn, this interface is been called to send the image
 * to VO for display or VPSS for using or VENC for coding
 */
td_s32 pciv_receive_pic(ot_pciv_chn pciv_chn, pciv_pic *recv_pic)
{
    td_s32          ret;
    td_ulong        flags;
    pciv_chn_ctx    *chn = TD_NULL;

    ot_assert(pciv_chn < OT_PCIV_MAX_CHN_NUM);
    ot_assert(recv_pic->filed < OT_VIDEO_FIELD_BUTT);
    ot_assert(recv_pic->src_type < PCIV_BIND_BUTT);
    ot_assert(recv_pic->index < g_chn_ctx[pciv_chn].pciv_attr.blk_cnt);

    PCIV_SPIN_LOCK;
    chn = &g_chn_ctx[pciv_chn];

    /*
     * when the master chip and slave chip re-creat and re-destroy, it is possible the message
     * is store in the buffer memory, it will lead that when master chip is not booted or just
     * created, at the same time, it received the slave chip image before destroying, it will
     * occupy the buffer, but the buffer-index idle-flag is true, in this situation, will appear
     * that it will used the buffer-index after the slave chip  re-creat, but the  master buffer-index
     * is been occupied, then an assert will occur, so we introduce the can_recv flag, when the slave
     * chip is re-start, it will send a message to the master chip,notice the master chip to release
     * all vb to keep synchronous with the slave chip, because the mechanism of receiving message is
     * trig by software interrupt, but in double core system, can_recv is needed to do mutual
     * exclusion on the double core system.
     */
    if ((chn->is_start != TD_TRUE) || (chn->can_recv != TD_TRUE)) {
        chn->buf_status[recv_pic->index] = BUFF_BUSY;
        PCIV_SPIN_UNLOCK;
        pciv_recv_pic_free(pciv_chn, recv_pic);
        pciv_err_trace("chn%d hasn't be ready to receive pic, start: %d, can_recv: %d\n",
            pciv_chn, chn->is_start, chn->can_recv);
        return OT_ERR_PCIV_NOT_READY; /* the vdec situation must be in our consideration */
    }
    chn->get_cnt++;

    /*
     * before this, the image buffer-flag is idle, pay attention to this situation,
     * if the called order is not reasonable, and the upper do not assure that master chip
     * and slave chip re-creat when return the TD_SUCCESS,at this time ,it is possible occur assert
     */
    ot_assert(chn->buf_status[recv_pic->index] == BUFF_FREE);

    /* in spite of sending to vo display success or not, here must set the buffer-flag to idle */
    chn->buf_status[recv_pic->index] = BUFF_BUSY;
    PCIV_SPIN_UNLOCK;

    /* the firmware interface is been called to send the image to VO display */
    ret = pciv_firmware_recv_pic_and_send(pciv_chn, recv_pic);
    PCIV_SPIN_LOCK;
    if (ret != TD_SUCCESS) {
        chn->lost_cnt++;
        PCIV_SPIN_UNLOCK;
        pciv_recv_pic_free(pciv_chn, recv_pic);
        pciv_err_trace("pciv_firmware_recv_pic_and_send err,chn%d, return value: 0x%x \n", pciv_chn, ret);
        return ret;
    }

    chn->send_cnt++;
    PCIV_SPIN_UNLOCK;
    return TD_SUCCESS;
}

/* after used by VO or VPSS or VENC, in the firmware, this interface is been called auto to return the image buffer */
td_s32 pciv_recv_pic_free(ot_pciv_chn pciv_chn, pciv_pic *recv_pic)
{
    td_s32          ret;
    pciv_chn_ctx    *chn    = TD_NULL;
    td_ulong        flags;

    pciv_check_chn_id_return(pciv_chn);
    pciv_check_ptr_return(recv_pic);

    PCIV_SPIN_LOCK;
    chn = &g_chn_ctx[pciv_chn];

    ot_assert(recv_pic->index < chn->pciv_attr.blk_cnt);

    /* only when the buffer state is been set to used, the buffer release action  occur */
    if (chn->buf_status[recv_pic->index] != BUFF_BUSY) {
        pciv_warn_trace("Buffer is not been used, chn%d\n", pciv_chn);
        PCIV_SPIN_UNLOCK;
        return OT_ERR_PCIV_BUF_EMPTY;
    }

    /* buffer state is set to idle */
    chn->buf_status[recv_pic->index] = BUFF_FREE;

    recv_pic->cnt = chn->resp_cnt;
    PCIV_SPIN_UNLOCK;
    /* the READDONE message is send to the sender to notice that do free source related action */
    ret = pciv_drv_adp_buf_free_notify(&chn->pciv_attr.remote_obj, recv_pic);
    if (ret != TD_SUCCESS) {
        pciv_err_trace("pciv_drv_adp_buf_free_notify err,chn%d\n", pciv_chn);
        return ret;
    }

    if (chn->can_recv == TD_TRUE && chn->is_start == TD_TRUE) {
        chn->resp_cnt++;
    }

    return TD_SUCCESS;
}

pciv_chn_ctx *pciv_get_context(ot_pciv_chn pciv_chn)
{
    return &g_chn_ctx[pciv_chn];
}

td_s32 pciv_init(void)
{
    td_s32              i;
    pciv_fmw_callback   firmware_call_back;

    spin_lock_init(&g_pciv_lock);
    INIT_LIST_HEAD(&g_list_head_user_dma);
    for (i = 0; i < PCIV_MAX_DMA_TASK; i++) {
        init_waitqueue_head(&g_user_dma_pool[i].wq_dma_done);
        g_user_dma_pool[i].dma_done = TD_TRUE;
        list_add_tail(&g_user_dma_pool[i].list, &g_list_head_user_dma);
    }

    (td_void)memset_s(g_chn_ctx, sizeof(g_chn_ctx), 0, sizeof(g_chn_ctx));
    for (i = 0; i < OT_PCIV_MAX_CHN_NUM; i++) {
        g_chn_ctx[i].is_create = TD_FALSE;
        g_chn_ctx[i].pciv_attr.remote_obj.chip_id = -1;
        g_chn_ctx[i].pciv_attr.remote_obj.pciv_chn = -1;

        sema_init(&g_chn_ctx[i].pciv_mutex, 1);
    }

    firmware_call_back.pf_src_send_pic = pciv_src_pic_send;
    firmware_call_back.pf_recv_pic_free = pciv_recv_pic_free;
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
    firmware_call_back.pf_query_pciv_chn_share_buf_state = pciv_get_share_buf_state;
    firmware_call_back.pf_free_pciv_chn_share_buf_state = pciv_set_share_buf_state;
#else
    firmware_call_back.pf_query_pciv_chn_share_buf_state = TD_NULL;
    firmware_call_back.pf_free_pciv_chn_share_buf_state = TD_NULL;
#endif
    (td_void)pciv_firmware_register_func(&firmware_call_back);

    return pciv_drv_adp_init();
}

td_void pciv_exit(void)
{
    td_s32 i, ret;

    for (i = 0; i < OT_PCIV_MAX_CHN_NUM; i++) {
        if (g_chn_ctx[i].is_create != TD_TRUE) {
            msleep(PCIV_SLEEP_TIME);
            continue;
        }

        ret = pciv_stop(i);
        if (ret != TD_SUCCESS) {
            pciv_err_trace("pciv_stop err,chn%d\n", i);
            return;
        }

        ret = pciv_destroy(i);
        if (ret != TD_SUCCESS) {
            pciv_err_trace("pciv_destroy err,chn%d\n", i);
            return;
        }
    }

    pciv_drv_adp_exit();
    return;
}

