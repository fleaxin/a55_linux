/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "pciv_drvadp.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "ot_type.h"
#include "ot_debug.h"
#include "sys_ext.h"
#include "ot_common_pciv.h"
#include "pci_trans.h"
#include "pciv.h"
#include "ot_mcc_usrdev.h"
#include "securec.h"
#include "mkp_pciv.h"

#define PCIV_DRV_INVALID_CHIP_ID      (-1)
#define PCIV_DRV_BASE_WIN_MIN_OFFSET  0x100000
#define PCIV_DRV_BASE_WIN_MAX_OFFSET  0x800000
#define PCIV_DRV_MSG_PORT             80
#define PCIV_DRV_MAX_SEND_MSG_CNT     10
#define PCIV_DRV_SEND_MSG_DELAY       100

struct ot_mcc_handle_attr g_msg_handle_attr = { 0, PCIV_DRV_MSG_PORT, 0, { 0 } };

static td_s32 g_local_id = PCIV_DRV_INVALID_CHIP_ID;
static td_s32 g_chip_cnt = 0;

static ot_pciv_window_base g_window_base[VENDOR_MAX_MAP_DEV];

static os_mcc_handle_t *g_msg_handle[VENDOR_MAX_MAP_DEV + 1] = { TD_NULL };

static struct list_head g_list_dma_task;
static spinlock_t g_lock_dma_task;
static spinlock_t g_lock_mcc_msg;

static pciv_gap_record g_gap_record[OT_PCIV_MAX_CHN_NUM] = {0};

td_void pciv_drv_adp_dma_finish(struct pcit_dma_task *task);

/* the interupter must be lock when call this function */
td_void pciv_drv_adp_start_dma_task(td_void)
{
    pciv_send_task          *task = TD_NULL;
    struct pcit_dma_task    pci_task;

    while (!list_empty(&(g_list_dma_task))) {
        task = list_entry(g_list_dma_task.next, pciv_send_task, list);

        pci_task.dir = task->read ? OT_PCIT_DMA_READ : OT_PCIT_DMA_WRITE;
        pci_task.src = task->src_phys_addr;
        pci_task.dest = task->dst_phys_addr;
        pci_task.len = task->len;
        pci_task.finish = pciv_drv_adp_dma_finish;
        pci_task.private_data = task;   /* point to address of task */
        if (pcit_create_task(&pci_task) == TD_SUCCESS) {
            /* if create task success, create next task */
            list_del(g_list_dma_task.next);
        } else {
            /* if create task fail, wait next dma task start */
            return;
        }
    }
}

td_void pciv_drv_adp_dma_finish(struct pcit_dma_task *task)
{
    pciv_send_task *stask = (pciv_send_task *)task->private_data;

    if (stask != TD_NULL) {
        if (stask->call_back != TD_NULL) {
            stask->call_back(stask);
        }
        kfree(stask);
        stask = TD_NULL;
    }
}

td_s32 pciv_drv_adp_add_dma_task(pciv_send_task *task)
{
    pciv_send_task  *task_tmp = TD_NULL;
    td_ulong        lock_flag;
    td_s32          ret;

    if (task == TD_NULL) {
        pciv_err_trace("task is a NULL pointer!\n");
        return OT_ERR_PCIV_NULL_PTR;
    }

    if (task->len == 0 && task->call_back != TD_NULL) {
        task->call_back(task);
        return TD_SUCCESS;
    }

    task_tmp = kmalloc(sizeof(pciv_send_task), GFP_ATOMIC);
    if (task_tmp == TD_NULL) {
        pciv_err_trace("alloc memory for send_task failed!\n");
        return OT_ERR_PCIV_NO_MEM;
    }
    ret = memcpy_s(task_tmp, sizeof(pciv_send_task), task, sizeof(pciv_send_task));
    if (ret != EOK) {
        pciv_err_trace("copy task info failed!\n");
        kfree(task_tmp);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    spin_lock_irqsave(&(g_lock_dma_task), lock_flag);

    list_add_tail(&(task_tmp->list), &(g_list_dma_task));

    pciv_drv_adp_start_dma_task();

    spin_unlock_irqrestore(&(g_lock_dma_task), lock_flag);

    return TD_SUCCESS;
}

td_s32 pciv_drv_adp_addr_check(const ot_pciv_dma_blk *block, td_bool is_read)
{
    td_s32 i;
    td_phys_addr_t temp_addr;
    td_phys_addr_t temp_pf_base;

    for (i = 0; i < VENDOR_MAX_MAP_DEV; i++) {
        if (g_window_base[i].chip_id == PCIV_DRV_INVALID_CHIP_ID) {
            break;
        }
        temp_addr = is_read ? block->src_addr : block->dst_addr;
        temp_pf_base = g_window_base[i].pf_addr;
        if ((temp_addr >= (temp_pf_base + PCIV_DRV_BASE_WIN_MIN_OFFSET)) &&
            ((temp_addr + block->size) <= (temp_pf_base + PCIV_DRV_BASE_WIN_MAX_OFFSET))) {
            return TD_SUCCESS;
        }
    }
    return TD_FAILURE;
}

/* it's a half work */
td_s32 pciv_drv_adp_send_msg(const ot_pciv_remote_obj *remote_obj, pciv_msg_type type, const pciv_pic *recv_pic)
{
    static pciv_msg     msg;
    td_ulong            lock_flag;
    pciv_notify_pic_end *notify = TD_NULL;
    td_s32              ret;
    td_s32              send_len = 0;
    td_s32              this_send_cnt = 0;

    if (remote_obj->chip_id > VENDOR_MAX_MAP_DEV || remote_obj->chip_id < 0) {
        pciv_err_trace("target is invalid!\n");
        return TD_FAILURE;
    }

    spin_lock_irqsave(&(g_lock_mcc_msg), lock_flag);

    msg.target = remote_obj->chip_id;
    msg.msg_type = type;
    msg.msg_len = sizeof(pciv_notify_pic_end) + PCIV_MSG_HEAD_LEN;
    notify = (pciv_notify_pic_end *)msg.c_msg_body;

    if (g_msg_handle[msg.target] == TD_NULL) {
        pciv_err_trace("target handle is none !\n");
        spin_unlock_irqrestore(&(g_lock_mcc_msg), lock_flag);
        return TD_FAILURE;
    }

    ret = memcpy_s(&notify->pic_info, sizeof(pciv_pic), recv_pic, sizeof(pciv_pic));
    if (ret != EOK) {
        pciv_err_trace("copy pic info failed!\n");
        spin_unlock_irqrestore(&(g_lock_mcc_msg), lock_flag);
        return TD_FAILURE;
    }
    notify->pciv_chn = remote_obj->pciv_chn;
    send_len = os_mcc_sendto(g_msg_handle[msg.target], &msg, msg.msg_len);
    while (send_len != (td_s32)msg.msg_len && this_send_cnt < PCIV_DRV_MAX_SEND_MSG_CNT) {
        udelay(PCIV_DRV_SEND_MSG_DELAY);
        send_len = os_mcc_sendto(g_msg_handle[msg.target], &msg, msg.msg_len);
        this_send_cnt++;
    }

    if (send_len == (td_s32)msg.msg_len) {
        ret = TD_SUCCESS;
    } else {
        /* danger!! but I don't known how to deal with it if send fail */
        ret = TD_FAILURE;
    }

    spin_unlock_irqrestore(&(g_lock_mcc_msg), lock_flag);
    if (ret != TD_SUCCESS) {
        printk(KERN_ERR "Send Msg Error tar:%d, handle:%pK, type:%u, len:%u, ret:%d\n",
               msg.target, g_msg_handle[msg.target], type, msg.msg_len, ret);
        panic("-------------------Msg Error---------------------------\n");
    }
    return ret;
}

td_s32 pciv_drv_adp_set_gap_record(ot_pciv_chn pciv_chn, const pciv_gap_record *gap)
{
    pciv_gap_record *gap_record = &g_gap_record[pciv_chn];
    td_s32 ret;

    ret = memcpy_s(gap_record, sizeof(pciv_gap_record), gap, sizeof(pciv_gap_record));
    if (ret != EOK) {
        pciv_err_trace("copy gap record failed\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_s32 pciv_drv_adp_get_gap_record(ot_pciv_chn pciv_chn, pciv_gap_record *gap)
{
    pciv_gap_record *gap_record = &g_gap_record[pciv_chn];
    td_s32 ret;

    ret = memcpy_s(gap, sizeof(pciv_gap_record), gap_record, sizeof(pciv_gap_record));
    if (ret != EOK) {
        pciv_err_trace("copy gap record failed\n");
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

td_s32 pciv_drv_adp_dma_end_notify(const ot_pciv_remote_obj *remote_obj, const pciv_pic *recv_pic)
{
    return pciv_drv_adp_send_msg(remote_obj, PCIV_MSGTYPE_WRITEDONE, recv_pic);
}

td_s32 pciv_drv_adp_buf_free_notify(const ot_pciv_remote_obj *remote_obj, const pciv_pic *recv_pic)
{
    return pciv_drv_adp_send_msg(remote_obj, PCIV_MSGTYPE_READDONE, recv_pic);
}

static td_s32 pciv_drv_adp_isr_write_done(const pciv_notify_pic_end *notify)
{
    pciv_pic    recv_pic;

    td_u64 time = call_sys_get_time_stamp();
    ot_pciv_chn     pciv_chn    = notify->pciv_chn;
    pciv_gap_record *gap_record = &g_gap_record[pciv_chn];
    td_s32          ret;

    if (gap_record->wdone_time == 0) {
        /* initial */
        gap_record->wdone_time    = time;
        gap_record->min_wdone_gap = 0xFFFF;
    }

    gap_record->wdone_gap = (td_u32)(time - gap_record->wdone_time);

    gap_record->max_wdone_gap =
        (gap_record->wdone_gap > gap_record->max_wdone_gap) ?
        gap_record->wdone_gap : gap_record->max_wdone_gap;

    if (gap_record->wdone_gap != 0) {
        gap_record->min_wdone_gap =
            (gap_record->wdone_gap < gap_record->min_wdone_gap) ?
            gap_record->wdone_gap : gap_record->min_wdone_gap;
    }

    gap_record->wdone_time = time;

    ret = memcpy_s(&recv_pic, sizeof(pciv_pic), &notify->pic_info, sizeof(pciv_pic));
    if (ret != EOK) {
        return TD_FAILURE;
    }
    pciv_receive_pic(pciv_chn, &recv_pic);
    return TD_SUCCESS;
}

static td_s32 pciv_drv_adp_isr_read_done(const pciv_notify_pic_end *notify)
{
    td_u64 time = call_sys_get_time_stamp();

    ot_pciv_chn     pciv_chn    = notify->pciv_chn;
    pciv_gap_record *gap_record = &g_gap_record[pciv_chn];

    if (gap_record->rdone_time == 0) {
        /* initial */
        gap_record->rdone_time    = time;
        gap_record->min_rdone_gap = 0xFFFF;
    }

    gap_record->rdone_gap = (td_u32)(time - gap_record->rdone_time);

    gap_record->max_rdone_gap =
        (gap_record->rdone_gap > gap_record->max_rdone_gap) ?
        gap_record->rdone_gap : gap_record->max_rdone_gap;

    if (gap_record->rdone_gap != 0) {
        gap_record->min_rdone_gap =
            (gap_record->rdone_gap < gap_record->min_rdone_gap) ?
            gap_record->rdone_gap : gap_record->min_rdone_gap;
    }

    gap_record->rdone_time = time;

    pciv_free_share_buf(pciv_chn, notify->pic_info.index, notify->pic_info.cnt);
    return TD_SUCCESS;
}

td_s32 pciv_drv_adp_msg_isr(td_void *handle, td_void *buf, td_u32 data_len)
{
    td_s32              ret     = TD_FAILURE;
    pciv_msg            *msg    = (pciv_msg *)buf;
    pciv_notify_pic_end *notify = (pciv_notify_pic_end *)msg->c_msg_body;

    ot_unused(handle);

    if (msg->target > VENDOR_MAX_MAP_DEV) {
        pciv_err_trace("no this target %u\n", msg->target);
        return -1;
    }

    if (g_local_id != msg->target) {
        if (g_local_id != 0) {
            /* On the slave chip, only receive the master chip message */
            pciv_err_trace("who are you? target=%u\n", msg->target);
            return 0;
        }
        /* On the master chip, if the message is from other slave chip, retransmission the message */
        if (g_msg_handle[msg->target] != TD_NULL) {
            td_ulong lock_flag;

            /* Danger!! But I don't known how to deal with it if send fail */
            spin_lock_irqsave(&(g_lock_mcc_msg), lock_flag);

            (void)os_mcc_sendto(g_msg_handle[msg->target], buf, data_len);

            spin_unlock_irqrestore(&(g_lock_mcc_msg), lock_flag);
        }
        return TD_SUCCESS;
    }
    switch (msg->msg_type) {
        case PCIV_MSGTYPE_WRITEDONE: {
            /*
             * The end of sending call the DMA task interface to send the WRITEDONE message to the receive end,
             * The local receiving end affter getting the message, get the image information, send to VO to display.
             */
            ret = pciv_drv_adp_isr_write_done(notify);
            break;
        }
        case PCIV_MSGTYPE_READDONE: {
            /*
             * The end of receiving the image after display the image on VO,
             * then send the READDONE message to the end of sending
             * The local end of sending after getting the message,
             * get the buffer information from the message and update the buffer state idle
             */
            ret = pciv_drv_adp_isr_read_done(notify);
            break;
        }
        case PCIV_MSGTYPE_FREE: {
            ret = pciv_free_all_buf(notify->pciv_chn);
            break;
        }
        default: {
            pciv_err_trace("Unknown message:%u\n", msg->msg_type);
            break;
        }
    }

    if (ret != TD_SUCCESS) {
        pciv_err_trace("Unknown how to process\n");
    }
    return ret;
}

td_s32 pciv_drv_adp_get_window_base(ot_pciv_window_base *win_base)
{
    td_s32 i;
    td_s32 ret = OT_ERR_PCIV_ILLEGAL_PARAM;

    if (win_base->chip_id < 0 || win_base->chip_id > VENDOR_MAX_MAP_DEV) {
        pciv_err_trace("invalid pcie device id:%d.\n", win_base->chip_id);
        return OT_ERR_PCIV_NOT_PERM;
    }

    for (i = 0; i < VENDOR_MAX_MAP_DEV; i++) {
        if (g_window_base[i].chip_id == win_base->chip_id) {
            ret = memcpy_s(win_base, sizeof(ot_pciv_window_base), &g_window_base[i], sizeof(ot_pciv_window_base));
            if (ret != EOK) {
                pciv_err_trace("copy window base failed\n");
                return OT_ERR_PCIV_ILLEGAL_PARAM;
            }
            break;
        }
    }
    if (ret != TD_SUCCESS) {
        pciv_err_trace("illegal pcie device id:%d\n", win_base->chip_id);
    }
    return ret;
}

td_s32 pciv_drv_adp_get_local_id(td_void)
{
    return g_local_id;
}

td_s32 pciv_drv_adp_enum_chip(ot_pciv_enum_chip *chips)
{
    td_s32 i;

    chips->chip_num = g_chip_cnt;
    for (i = 0; i < g_chip_cnt; i++) {
        chips->chip_id[i] = g_window_base[i].chip_id;
    }
    chips->chip_id[g_chip_cnt] = PCIV_DRV_INVALID_CHIP_ID;
    return TD_SUCCESS;
}

static td_s32 pciv_drv_adp_host_init(ot_pciv_window_base *win_base, int n_remot_id[])
{
    td_s32                  i;
    td_s32                  ret;
    os_mcc_handle_opt_t   stopt;

    for (i = 0; i < VENDOR_MAX_MAP_DEV; i++) {
        if (n_remot_id[i] != PCIV_DRV_INVALID_CHIP_ID) {
            ret = os_mcc_check_remote(n_remot_id[i], TD_NULL);
            if (ret != 0) {
                printk("host check slave chip %d failed\n", n_remot_id[i]);
                return TD_FAILURE;
            }

            g_msg_handle_attr.target_id = n_remot_id[i];
            g_msg_handle[n_remot_id[i]] = os_mcc_open(&g_msg_handle_attr);
            if (g_msg_handle[n_remot_id[i]] == TD_NULL) {
                printk("os_mcc_open err, id:%d\n", n_remot_id[i]);
                return TD_FAILURE;
            }

            stopt.recvfrom_notify = pciv_drv_adp_msg_isr;
            os_mcc_setopt(g_msg_handle[n_remot_id[i]], &stopt);

            win_base->chip_id = n_remot_id[i];
            win_base->pf_addr = get_pf_window_base(n_remot_id[i]);
            win_base->pf_ahb_addr = 0;

            win_base++;
        }
    }
    return TD_SUCCESS;
}

static td_s32 pciv_drv_adp_slave_init(ot_pciv_window_base *base_win)
{
    os_mcc_handle_opt_t stopt;

    g_msg_handle[0] = os_mcc_open(&g_msg_handle_attr);
    if (g_msg_handle[0] == TD_NULL) {
        printk("can't open mcc device 0!\n");
        return TD_FAILURE;
    }

    stopt.recvfrom_notify = pciv_drv_adp_msg_isr;
    stopt.data = g_local_id;
    os_mcc_setopt(g_msg_handle[0], &stopt);

    base_win->chip_id = 0;
    base_win->np_addr = 0;
    base_win->pf_addr = 0;
    base_win->cfg_addr = 0;
    base_win->pf_ahb_addr = get_pf_window_base(0);
    return TD_SUCCESS;
}

td_s32 pciv_drv_adp_init(td_void)
{
    td_s32              i;
    td_s32              ret;
    td_s32              n_remot_id[VENDOR_MAX_MAP_DEV];
    ot_pciv_window_base *base_win = &g_window_base[0];

    INIT_LIST_HEAD(&(g_list_dma_task));
    spin_lock_init(&(g_lock_dma_task));
    spin_lock_init(&g_lock_mcc_msg);

    for (i = 0; i < VENDOR_MAX_MAP_DEV; i++) {
        n_remot_id[i] = PCIV_DRV_INVALID_CHIP_ID;
        g_window_base[i].chip_id = PCIV_DRV_INVALID_CHIP_ID;
    }

    g_chip_cnt = os_mcc_getremoteids(n_remot_id, TD_NULL);
    if (g_chip_cnt > VENDOR_MAX_MAP_DEV) {
        printk("The chip count %d is out of range [%d, %d]!\n", g_chip_cnt, 1, VENDOR_MAX_MAP_DEV);
        return TD_FAILURE;
    }

    /* slave chip need check host */
    if (n_remot_id[0] == 0) {
        ret = os_mcc_check_remote(0, TD_NULL);
        if (ret != 0) {
            printk("slave check host failed!\n");
            return TD_FAILURE;
        }
    }

    g_local_id = os_mcc_getlocalid(TD_NULL);
    if (g_local_id == 0) {
        /* pci host */
        ret = pciv_drv_adp_host_init(base_win, n_remot_id);
    } else {
        /* pci slave */
        ret = pciv_drv_adp_slave_init(base_win);
    }
    return ret;
}

td_void pciv_drv_adp_exit(td_void)
{
    td_s32 i;
    for (i = 0; i < VENDOR_MAX_MAP_DEV; i++) {
        if (g_msg_handle[i] != TD_NULL) {
            os_mcc_close(g_msg_handle[i]);
        }
    }
    return;
}

