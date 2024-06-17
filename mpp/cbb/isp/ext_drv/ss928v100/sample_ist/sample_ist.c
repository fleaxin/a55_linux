/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __LITEOS__

#include "sample_ist.h"
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "ot_common.h"
#include "isp_ext.h"

#define MAX_TEST_NODES 4

td_s32 sync_callback(td_u64 data);
td_s32 sync_af_calc(td_u64 data);

ot_vi_pipe g_vi_pipe = 0;
ot_isp_drv_af_statistics g_focus_stat;

ot_isp_sync_task_node g_sync_node[MAX_TEST_NODES] = {
    {
        .method = ISP_SYNC_TSK_METHOD_HW_IRQ,
        .isp_sync_tsk_callback = sync_af_calc,
        .data = 0,
        .focus_stat = &g_focus_stat,
        .sz_id = "hw_0"
    },
    {
        .method = ISP_SYNC_TSK_METHOD_HW_IRQ,
        .isp_sync_tsk_callback = sync_callback,
        .data = 1,
        .focus_stat = TD_NULL,
        .sz_id = "hw_1"
    },
    {
        .method = ISP_SYNC_TSK_METHOD_WORKQUE,
        .isp_sync_tsk_callback = sync_callback,
        .data = 3, /* 3 */
        .focus_stat = TD_NULL,
        .sz_id = "wq_0"
    },
    {
        .method = ISP_SYNC_TSK_METHOD_WORKQUE,
        .isp_sync_tsk_callback = sync_callback,
        .data = 4, /* 4 */
        .focus_stat = TD_NULL,
        .sz_id = "wq_1"
    }
};

td_s32 sync_af_calc(td_u64 data)
{
    int i, j;
    int cnt = 0;
    td_u16 stat_data;

    ot_isp_drv_be_focus_statistics *isp_focus_st = TD_NULL;
    isp_focus_st = &g_focus_stat.be_af_stat;

    ot_unused(data);

    /* get af statistics */
    if (cnt++ % 30 == 0) { /* calculate every 30 frames */
        printk("h1:\n");
        for (i = 0; i < 15; i++) { /* this is a 15 * 17 matrix */
            for (j = 0; j < 17; j++) { /* this is a 15 * 17 matrix */
                stat_data = isp_focus_st->zone_metrics[i][j].h1;
                printk("%6d", stat_data);
            }
            printk("\n");
        }
    }

    /* af algorithm */
    return 0;
}

td_s32 sync_callback(td_u64 data)
{
    int local_data = data;

    printk("%d\n", local_data);
    return 0;
}

/* file operation */
int sample_ist_open(struct inode *inode, struct file *file)
{
    ot_unused(inode);
    ot_unused(file);

    return 0;
}

int sample_ist_close(struct inode *inode, struct file *file)
{
    ot_unused(inode);
    ot_unused(file);

    return 0;
}

static long sample_ist_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int __user *argp = (int __user *)(td_uintptr_t)arg;
    int node_index = *argp;
    ot_unused(file);

    if (node_index >= MAX_TEST_NODES) {
        return -1;
    }

    switch (cmd) {
        case SAMPLE_IST_ADD_NODE:
            if (ckfn_isp_register_sync_task()) {
                call_isp_register_sync_task(g_vi_pipe, &g_sync_node[node_index]);
            } else {
                printk("register sample_ist failed!\n");
                return -1;
            }
            break;

        case SAMPLE_IST_DEL_NODE:
            if (ckfn_isp_unregister_sync_task()) {
                if (call_isp_unregister_sync_task(g_vi_pipe, &g_sync_node[node_index]) == TD_FAILURE) {
                    printk("del node err %d\n", node_index);
                }
            } else {
                printk("unregister sample_ist failed!\n");
                return -1;
            }
            break;

        default: {
            printk("invalid ioctl command!\n");
            return -ENOIOCTLCMD;
        }
    }

    return 0;
}

static struct file_operations g_sample_ist_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl = sample_ist_ioctl,
    .open       = sample_ist_open,
    .release    = sample_ist_close,
};

static struct miscdevice g_sample_ist_dev = {
    .minor   = MISC_DYNAMIC_MINOR,
    .name    = "sample_ist",
    .fops    = &g_sample_ist_fops,
};

/* module init and exit */
static int __init sample_ist_init(void)
{
    int  i, ret;

    ret = misc_register(&g_sample_ist_dev);
    if (ret != 0) {
        printk("register sample_ist device failed with %#x!\n", ret);
        return -1;
    }

    for (i = 0; i < MAX_TEST_NODES; i++) {
        if (ckfn_isp_register_sync_task()) {
            call_isp_register_sync_task(g_vi_pipe, &g_sync_node[i]);
        }
    }

    return 0;
}

static void __exit sample_ist_exit(void)
{
    int i;

    misc_deregister(&g_sample_ist_dev);

    for (i = 0; i < MAX_TEST_NODES; i++) {
        if (ckfn_isp_unregister_sync_task()) {
            call_isp_unregister_sync_task(g_vi_pipe, &g_sync_node[i]);
        }
    }
}

module_init(sample_ist_init);
module_exit(sample_ist_exit);

MODULE_DESCRIPTION("sample of isp sync task Driver");
MODULE_LICENSE("GPL");

#endif
