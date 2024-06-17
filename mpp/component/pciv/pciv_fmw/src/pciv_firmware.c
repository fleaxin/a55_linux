/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "pciv_firmware.h"

#include <linux/module.h>

#include "ot_defines.h"
#include "ot_osal.h"
#include "osal_mmz.h"

#include "mm_ext.h"
#include "sys_ext.h"
#include "vpss_ext.h"
#include "region_ext.h"
#include "securec.h"
#include "pciv_firmware_comm.h"
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
#include "proc_ext.h"
#include "pciv_firmware_proc.h"
#endif

typedef enum {
    PCIVFMW_STATE_STARTED   = 0,
    PCIVFMW_STATE_STOPPING  = 1,
    PCIVFMW_STATE_STOPED    = 2,
} pciv_fmw_state;

static pciv_vb_pool g_vb_pool;

#define FMW_SEND_ING_SLEEP_TIME 10
#define FMW_OT_PCIV_TIMER_EXPIRES  (3 * OT_PCIV_TIMER_EXPIRES)

typedef struct {
    td_s32         vgs_in_vb_handle;
    td_s32         vgs_out_vb_handle;
    td_phys_addr_t in_vb_phys_addr;
    td_phys_addr_t out_vb_phys_addr;
}pciv_fmw_vgs_task;

static pciv_fmw_chn      g_fmw_pciv_chn[PCIV_FMW_MAX_CHN_NUM] = {0};
static struct timer_list g_timer_pciv_send_vdec_pic;

static osal_spinlock_t g_pciv_fmw_lock;
#define pciv_fmw_spin_lock(flags)   osal_spin_lock_irqsave(&(g_pciv_fmw_lock), &(flags))
#define pciv_fmw_spin_unlock(flags) osal_spin_unlock_irqrestore(&(g_pciv_fmw_lock), &(flags))

#define PCIV_FMW_MAX_NODE_NUM 6

static pciv_fmw_callback g_pciv_fmw_call_back;

static td_u32 g_pciv_fmw_state = PCIVFMW_STATE_STOPED;

static td_s32 g_drop_err_timeref = 1;

td_void pciv_firmware_recv_pic_free(struct timer_list *recv_timer);

pciv_fmw_chn *pciv_fmw_get_context(ot_pciv_chn pciv_chn)
{
    return &g_fmw_pciv_chn[pciv_chn];
}

static td_void pciv_firmware_wait_send_end(ot_pciv_chn pciv_chn)
{
    td_ulong flags;
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);

    while (1) {
        pciv_fmw_spin_lock(flags);
        if (fmw_chn->send_state == PCIVFMW_SEND_ING) {
            pciv_fmw_spin_unlock(flags);
            msleep(FMW_SEND_ING_SLEEP_TIME);
            continue;
        } else {
            /* if send fail, release the vdec buffer(only deal with vdec queue node exist) */
            if (fmw_chn->send_state == PCIVFMW_SEND_NOK) {
                pciv_fmw_src_pic_release(pciv_chn);
                fmw_chn->send_state = PCIVFMW_SEND_OK;
            }
            pciv_fmw_spin_unlock(flags);
            break;
        }
    }
}

td_s32 pciv_firmware_reset_chn_queue(ot_pciv_chn pciv_chn)
{
    td_ulong        flags;
    td_s32          ret;
    td_u32          busy_num;
    td_u32          i;
    pciv_pic_node   *node       = TD_NULL;
    pciv_fmw_chn    *fmw_chn    = TD_NULL;

    pciv_fmw_spin_lock(flags);
    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_start == TD_TRUE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("chn%d has not stop, please stop it first!\n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }
    pciv_fmw_spin_unlock(flags);

    /* it is in the sending process,wait for sending finished */
    pciv_firmware_wait_send_end(pciv_chn);

    pciv_fmw_spin_lock(flags);
    /* put the node in the busy queue to free queue */
    busy_num = pciv_pic_queue_get_busy_num(&fmw_chn->pic_queue);

    for (i = 0; i < busy_num; i++) {
        node = pciv_pic_queue_get_busy(&fmw_chn->pic_queue);
        if (node == TD_NULL) {
            pciv_fmw_spin_unlock(flags);
            pciv_fmw_err_trace("get busy node failed! chn%d\n", pciv_chn);
            return OT_ERR_PCIV_NOT_PERM;
        }

        if (fmw_chn->is_master == TD_FALSE) {
            ret = call_vb_user_sub(node->pciv_pic.frame_info.pool_id,
                node->pciv_pic.frame_info.video_frame.phys_addr[0],
                OT_VB_UID_PCIV);
            if (ret != TD_SUCCESS) {
                pciv_fmw_err_trace("sub user vb failed\n");
            }
        }

        pciv_pic_queue_put_free(&fmw_chn->pic_queue, node);
        node = TD_NULL;
    }

    pciv_fmw_spin_unlock(flags);

    return TD_SUCCESS;
}

td_s32 pciv_firmware_create(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id)
{
    td_ulong        flags;
    td_s32          ret;
    td_u32          node_num;
    pciv_fmw_chn    *fmw_chn = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(attr);

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_create == TD_TRUE) {
        pciv_fmw_err_trace("chn%d have already created \n", pciv_chn);
        return OT_ERR_PCIV_EXIST;
    }

    ret = pciv_firmware_set_attr(pciv_chn, attr, local_id);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("attr of chn%d is invalid \n", pciv_chn);
        return ret;
    }

    pciv_fmw_spin_lock(flags);
    pciv_firmware_chn_base_init(pciv_chn);
    pciv_fmw_spin_unlock(flags);

    /* master chip */
    if (local_id == 0) {
        node_num = attr->blk_cnt;
    } else {
        node_num = PCIV_FMW_MAX_NODE_NUM;
    }

    ret = pciv_creat_pic_queue(&fmw_chn->pic_queue, node_num);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("chn%d create pic queue failed\n", pciv_chn);
        return OT_ERR_PCIV_NO_MEM;
    }
    fmw_chn->cur_vdec_node = TD_NULL;

    pciv_fmw_spin_lock(flags);
    fmw_chn->is_create = TD_TRUE;
    if (local_id == 0) {
        fmw_chn->is_master = TD_TRUE;
    } else {
        fmw_chn->is_master = TD_FALSE;
    }
    pciv_fmw_spin_unlock(flags);
    pciv_fmw_info_trace("chn%d create ok \n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_firmware_destroy(ot_pciv_chn pciv_chn)
{
    td_ulong        flags;
    td_s32          ret;
    pciv_fmw_chn    *fmw_chn = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_create == TD_FALSE) {
        return TD_SUCCESS;
    }
    if (fmw_chn->is_start == TD_TRUE) {
        pciv_fmw_err_trace("chn%d is running,you should stop first \n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }

    ret = pciv_firmware_reset_chn_queue(pciv_chn);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("chn%d stop failed!\n", pciv_chn);
        return ret;
    }
    pciv_fmw_spin_lock(flags);
    pciv_destroy_pic_queue(&fmw_chn->pic_queue);

    fmw_chn->is_create = TD_FALSE;
    pciv_fmw_spin_unlock(flags);
    pciv_fmw_info_trace("chn%d destroy ok \n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_firmware_set_attr(ot_pciv_chn pciv_chn, const ot_pciv_attr *attr, td_s32 local_id)
{
    td_ulong        flags;
    td_s32          ret;
    pciv_fmw_chn    *fmw_chn = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(attr);

    fmw_chn = pciv_fmw_get_context(pciv_chn);

    pciv_fmw_spin_lock(flags);

    /* The channel is in the process of start,it cannot alter */
    if (fmw_chn->is_start == TD_TRUE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("chn%d is running\n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }

    /* check the valid of attr */
    ret = pciv_firmware_check_attr(attr);
    if (ret != TD_SUCCESS) {
        pciv_fmw_spin_unlock(flags);
        return ret;
    }

    ret = pciv_firmware_set_fmw_chn(pciv_chn, attr, local_id);
    if (ret != TD_SUCCESS) {
        pciv_fmw_spin_unlock(flags);
        return ret;
    }

    fmw_chn->blk_size = attr->blk_size;
    fmw_chn->count = attr->blk_cnt;

    pciv_fmw_spin_unlock(flags);

    return TD_SUCCESS;
}

td_s32 pciv_firmware_start(ot_pciv_chn pciv_chn)
{
    td_ulong        flags;
    pciv_fmw_chn    *fmw_chn = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_create != TD_TRUE) {
        pciv_fmw_err_trace("chn%d not create\n", pciv_chn);
        return OT_ERR_PCIV_UNEXIST;
    }

    pciv_fmw_spin_lock(flags);
    fmw_chn->is_start = TD_TRUE;
    pciv_fmw_spin_unlock(flags);
    pciv_fmw_info_trace("chn%d start ok \n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_firmware_stop(ot_pciv_chn pciv_chn)
{
    td_ulong flags;

    pciv_fmw_check_chn_id_return(pciv_chn);

    pciv_fmw_spin_lock(flags);
    g_fmw_pciv_chn[pciv_chn].is_start = TD_FALSE;

    if (g_fmw_pciv_chn[pciv_chn].rgn_num != 0) {
        pciv_fmw_info_trace("region number of chn%d is %d, now free the region!\n",
            pciv_chn, g_fmw_pciv_chn[pciv_chn].rgn_num);
        pciv_fmw_put_rgn(pciv_chn, OT_RGN_OVERLAYEX);
        g_fmw_pciv_chn[pciv_chn].rgn_num = 0;
    }

    pciv_fmw_spin_unlock(flags);
    pciv_fmw_info_trace("chn%d stop ok \n", pciv_chn);
    return TD_SUCCESS;
}

td_s32 pciv_firmware_window_vb_create(const ot_pciv_window_vb_cfg *cfg)
{
    td_s32  ret;
    td_u32  i;
    td_u32  pool_id;
    vb_info info = {0};

    pciv_fmw_check_ptr_return(cfg);

    if (g_vb_pool.pool_cnt != 0) {
        pciv_fmw_err_trace("video buffer pool has created\n");
        return OT_ERR_PCIV_BUSY;
    }

    for (i = 0; i < cfg->pool_cnt; i++) {
        ret = snprintf_s(info.buf_name, OT_MAX_MMZ_NAME_LEN, OT_MAX_MMZ_NAME_LEN - 1, "pciv_vb_from_window %d", i);
        if (ret < 0) {
            pciv_fmw_err_trace("get buf name failed for vb %d\n", i);
            break;
        }
        info.blk_cnt = cfg->blk_cnt[i];
        info.blk_size = cfg->blk_size[i];
        info.vb_remap_mode = OT_VB_REMAP_MODE_NONE;
        info.uid = OT_VB_UID_PCIV;
        ret = call_vb_create_pool(&pool_id, OT_RESERVE_MMZ_NAME, &info);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("create pool(index=%u, cnt=%d, size=%d) fail\n",
                i, cfg->blk_cnt[i], cfg->blk_size[i]);
            break;
        }
        g_vb_pool.pool_cnt = i + 1;
        g_vb_pool.pool_id[i] = pool_id;
        g_vb_pool.size[i] = cfg->blk_size[i];
    }

    /* if one pool created not success, then rollback */
    if (g_vb_pool.pool_cnt != cfg->pool_cnt) {
        for (i = 0; i < g_vb_pool.pool_cnt; i++) {
            ret = call_vb_destroy_pool(g_vb_pool.pool_id[i]);
            if (ret != TD_SUCCESS) {
                pciv_fmw_err_trace("destroy VB pool failed!\n");
                return OT_ERR_PCIV_NOT_PERM;
            }
            g_vb_pool.pool_id[i] = OT_VB_INVALID_POOL_ID;
        }

        g_vb_pool.pool_cnt = 0;

        return OT_ERR_PCIV_NO_MEM;
    }

    return TD_SUCCESS;
}

td_s32 pciv_firmware_window_vb_destroy(td_void)
{
    td_u32 i;
    td_u32 j    = 0;
    td_s32 flag = 0;
    td_s32 ret;

    for (i = 0; i < g_vb_pool.pool_cnt; i++) {
        ret = call_vb_destroy_pool(g_vb_pool.pool_id[i]);
        if (ret != TD_SUCCESS) {
            pciv_fmw_err_trace("destroy VB pool_id[%d]:%d failed!\n", i, g_vb_pool.pool_id[i]);
            j++;
            continue;
        }
        g_vb_pool.pool_id[i] = OT_VB_INVALID_POOL_ID;
    }

    for (i = 0; i < g_vb_pool.pool_cnt; i++) {
        flag += (g_vb_pool.pool_id[i] != OT_VB_INVALID_POOL_ID);
    }
    if (flag) {
        g_vb_pool.pool_cnt = j;
        return OT_ERR_PCIV_NOT_PERM;
    }

    g_vb_pool.pool_cnt = 0;
    return TD_SUCCESS;
}

td_s32 pciv_firmware_malloc(td_u32 size, td_s32 local_id, td_phys_addr_t *phys_addr)
{
    td_u32          i;
    td_s32          ret;
    td_u32          pool_id;
    vb_blk_handle   handle    = OT_VB_INVALID_HANDLE;
    td_char         *mmz_name = TD_NULL;

    pciv_fmw_check_ptr_return(phys_addr);

    if (size == 0) {
        pciv_fmw_err_trace("malloc fail,size:%d!\n", size);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if (local_id == 0) {
        handle = call_vb_get_blk_by_size(size, OT_VB_UID_PCIV, mmz_name);
        if (handle == OT_VB_INVALID_HANDLE) {
            pciv_fmw_err_trace("call_vb_get_blk_by_size fail,size:%d!\n", size);
            return OT_ERR_PCIV_NO_BUF;
        }

        goto get_vb;
    }

    /* if in the slave chip, then alloc buffer from special VB */
    for (i = 0; i < g_vb_pool.pool_cnt; i++) {
        if (size > g_vb_pool.size[i]) {
            continue;
        }

        handle = call_vb_get_blk_by_pool_id(g_vb_pool.pool_id[i], OT_VB_UID_PCIV);
        if (handle != OT_VB_INVALID_HANDLE) {
            break;
        }
    }

    if (handle == OT_VB_INVALID_HANDLE) {
        pciv_fmw_err_trace("call_vb_get_blk_by_size fail,size:%d!\n", size);
        return OT_ERR_PCIV_NO_BUF;
    }
get_vb:
    *phys_addr = call_vb_handle_to_phys(handle);

    pool_id = call_vb_handle_to_pool_id(handle);

    ret = call_vb_user_add(pool_id, *phys_addr, OT_VB_UID_USER);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("vb add fail!\n");
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    return ret;
}

td_s32 pciv_firmware_free(td_phys_addr_t phys_addr)
{
    td_ulong        flags;
    vb_blk_handle   vb_handle;
    td_s32          ret = TD_SUCCESS;
    td_s32          res;

    pciv_fmw_spin_lock(flags);
    vb_handle = call_vb_phy_to_handle(phys_addr);
    if (vb_handle == OT_VB_INVALID_HANDLE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("invalid physical address 0x%lx\n", (td_ulong)phys_addr);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    if (call_vb_inquire_one_user_cnt(vb_handle, OT_VB_UID_USER) > 0) {
        res = call_vb_user_sub(call_vb_handle_to_pool_id(vb_handle), phys_addr, OT_VB_UID_PCIV);
        if (res != TD_SUCCESS) {
            ret = OT_ERR_PCIV_ILLEGAL_PARAM;
        }
        res = call_vb_user_sub(call_vb_handle_to_pool_id(vb_handle), phys_addr, OT_VB_UID_USER);
        if (res != TD_SUCCESS) {
            ret = OT_ERR_PCIV_ILLEGAL_PARAM;
        }
    }
    pciv_fmw_spin_unlock(flags);

    return ret;
}

td_s32 pciv_firmware_malloc_chn_buffer(ot_pciv_chn pciv_chn, td_u32 index, td_u32 size, td_s32 local_id,
    td_phys_addr_t *phys_addr)
{
    td_ulong        flags;
    vb_blk_handle   handle;
    td_char         *mmz_name = TD_NULL;
    pciv_fmw_chn    *fmw_chn  = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(phys_addr);

    if (index >= OT_PCIV_MAX_BUF_NUM) {
        pciv_fmw_err_trace("the index(%u) of chnbuff illegal!\n", index);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    if (size == 0) {
        pciv_fmw_err_trace("call_vb_get_blk_by_size fail,size:%d!\n", size);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    pciv_fmw_spin_lock(flags);
    if (local_id != 0) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("slave chip:%d, doesn't need chn buffer!\n", local_id);
        return OT_ERR_PCIV_NOT_PERM;
    }

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_start == TD_TRUE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("chn%d, has start already!\n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }

    if (fmw_chn->phys_addr[index] != 0) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("chn%d, has malloc chn buffer already!\n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }

    handle = call_vb_get_blk_by_size(size, OT_VB_UID_PCIV, mmz_name);
    if (handle == OT_VB_INVALID_HANDLE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("call_vb_get_blk_by_size fail,size:%d!\n", size);
        return OT_ERR_PCIV_NO_BUF;
    }

    *phys_addr = call_vb_handle_to_phys(handle);
    fmw_chn->phys_addr[index] = *phys_addr;
    fmw_chn->pool_id[index] = call_vb_handle_to_pool_id(handle);
    pciv_fmw_spin_unlock(flags);

    return TD_SUCCESS;
}

td_s32 pciv_firmware_free_chn_buffer(ot_pciv_chn pciv_chn, td_u32 index, td_s32 local_id)
{
    td_ulong        flags;
    td_s32          ret;
    vb_blk_handle   vb_handle;
    pciv_fmw_chn    *fmw_chn = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);

    if (index >= OT_PCIV_MAX_BUF_NUM) {
        pciv_fmw_err_trace("the index(%u) of chnbuff illegal!\n", index);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    pciv_fmw_spin_lock(flags);
    if (local_id != 0) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("slave chip:%d, has no chn buffer, doesn't need to free chn buffer!\n", local_id);
        return OT_ERR_PCIV_NOT_PERM;
    }

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_start == TD_TRUE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("chn%d has not stopped! please stop it first!\n", pciv_chn);
        return OT_ERR_PCIV_NOT_PERM;
    }

    vb_handle = call_vb_phy_to_handle(fmw_chn->phys_addr[index]);
    if (vb_handle == OT_VB_INVALID_HANDLE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_err_trace("invalid physical address 0x%lx\n", (td_ulong)fmw_chn->phys_addr[index]);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }

    pciv_fmw_warn_trace("start to free buffer, pool_id: %d, phys_addr: 0x%lx.\n",
        call_vb_handle_to_pool_id(vb_handle), (td_ulong)fmw_chn->phys_addr[index]);
    ret = call_vb_user_sub(call_vb_handle_to_pool_id(vb_handle), fmw_chn->phys_addr[index], OT_VB_UID_PCIV);
    if (ret != TD_SUCCESS) {
        pciv_fmw_spin_unlock(flags);
        return OT_ERR_PCIV_ILLEGAL_PARAM;
    }
    pciv_fmw_warn_trace("finish to free buffer, pool_id: %d, phys_addr: 0x%lx.\n",
        call_vb_handle_to_pool_id(vb_handle), (td_ulong)fmw_chn->phys_addr[index]);
    fmw_chn->phys_addr[index] = 0;

    pciv_fmw_spin_unlock(flags);

    return ret;
}

/* After vo displaying and vpss and venc used,the function register by PCIV or FwmDccs mode is been called */
td_void pciv_firmware_recv_pic_free(struct timer_list *recv_timer)
{
    td_s32          ret;
    td_u32          i;
    pciv_pic        recv_pic;
    pciv_fmw_chn    *fmw_chn    = TD_NULL;
    td_bool         hit         = TD_FALSE;
    ot_pciv_chn     pciv_chn;

    fmw_chn = (pciv_fmw_chn*)osal_container_of(recv_timer, pciv_fmw_chn, buf_timer);
    pciv_chn = ((td_uintptr_t)fmw_chn - (td_uintptr_t)g_fmw_pciv_chn) / sizeof(pciv_fmw_chn);

    if (fmw_chn->is_start != TD_TRUE) {
        return;
    }

    for (i = 0; i < fmw_chn->count; i++) {
        if (!pciv_firmware_is_vb_can_release(pciv_chn, i)) {
            continue;
        }
        /*
         * The function register by the PCIV is called to handle the action
         * after the VO displaying or vpss using and venc coding
         */
        if (g_pciv_fmw_call_back.pf_recv_pic_free) {
            recv_pic.index = i;        /* the index of buffer can release */
            recv_pic.cnt   = 0;        /* not used */

            ret = g_pciv_fmw_call_back.pf_recv_pic_free(pciv_chn, &recv_pic);
            if (ret == TD_SUCCESS) {
                hit = TD_TRUE;
                fmw_chn->resp_cnt++;
                continue;
            }
            if (ret != OT_ERR_PCIV_BUF_EMPTY) {
                pciv_fmw_err_trace("chnchn%d pf_recv_pic_free failed with:0x%x.\n", pciv_chn, ret);
                continue;
            }
        }
    }

    /* if the buffer has not release by vo/vpss/venc,then start the time after 10ms to check */
    if (hit != TD_TRUE) {
        fmw_chn->buf_timer.function = pciv_firmware_recv_pic_free;
        mod_timer(&(fmw_chn->buf_timer), jiffies + 1);
    }
}

td_s32 pciv_firmware_recv_pic_and_send(ot_pciv_chn pciv_chn, const pciv_pic *recv_pic)
{
    td_ulong            flags;
    td_s32              ret;
    pciv_fmw_chn        *fmw_chn = TD_NULL;
    vb_blk_handle       vb_handle;
    ot_video_frame_info video_frm_info  = {0};

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(recv_pic);
    if (recv_pic->index >= OT_PCIV_MAX_BUF_NUM) {
        pciv_fmw_err_trace("index %u is over range!\n", recv_pic->index);
        return TD_FAILURE;
    }

    pciv_fmw_spin_lock(flags);
    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_start != TD_TRUE) {
        pciv_fmw_spin_unlock(flags);
        return OT_ERR_PCIV_NOT_READY;
    }
    fmw_chn->get_cnt++;

    vb_handle = call_vb_phy_to_handle(fmw_chn->phys_addr[recv_pic->index]);
    if (vb_handle == OT_VB_INVALID_HANDLE) {
        pciv_fmw_err_trace("chn%d get buffer fail!\n", pciv_chn);
        fmw_chn->lost_cnt++;
        pciv_fmw_spin_unlock(flags);
        return TD_FAILURE;
    }

    pciv_firmware_recv_set_video_frame(pciv_chn, &video_frm_info, recv_pic, vb_handle);

    ret = pciv_firmware_send_data(pciv_chn, recv_pic, &video_frm_info);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("chn%d send data fail!\n", pciv_chn);
        fmw_chn->lost_cnt++;
        pciv_fmw_spin_unlock(flags);
        return TD_FAILURE;
    }

    /*
     * when start timer, PcivFirmWareVoPicFree not have lock synchronous
     * int the PcivFirmWareVoPicFree function, need separate lock
     */
    pciv_firmware_recv_pic_free(&fmw_chn->buf_timer);
    pciv_fmw_spin_unlock(flags);

    return ret;
}

/* After transmit, release the Image buffer after VGS Zoom */
td_s32 pciv_firmware_src_pic_free(ot_pciv_chn pciv_chn, const pciv_pic *src_pic)
{
    td_s32      ret;

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(src_pic);

    g_fmw_pciv_chn[pciv_chn].resp_cnt++;

    /* if the mpp is deinit,the sys will release the buffer */
    if (g_pciv_fmw_state == PCIVFMW_STATE_STOPED) {
        return TD_SUCCESS;
    }

    pciv_fmw_debug_trace("---> addr:0x%lx\n", (td_ulong)src_pic->phys_addr);
    ret = call_vb_user_sub(src_pic->pool_id, src_pic->phys_addr, OT_VB_UID_PCIV);
    return ret;
}

static td_void pciv_fmw_set_send_state(ot_pciv_chn pciv_chn, td_s32 ret, const pciv_bind_obj *obj)
{
    td_ulong flags;
    pciv_fmw_chn    *fmw_chn = pciv_fmw_get_context(pciv_chn);

    pciv_fmw_spin_lock(flags);
    if (!obj->is_block && ret != TD_SUCCESS) {
        fmw_chn->lost_cnt++;
        ret = TD_SUCCESS;
    }
    if (ret != TD_SUCCESS) {
        fmw_chn->send_state = PCIVFMW_SEND_NOK;
    } else {
        fmw_chn->send_state = PCIVFMW_SEND_OK;
    }
    if (ret == TD_SUCCESS) {
        pciv_fmw_src_pic_release(pciv_chn);
    }
    pciv_fmw_spin_unlock(flags);
}

static td_s32 pciv_firmware_send_src_pic(ot_pciv_chn pciv_chn, const pciv_bind_obj *bind_obj,
    const ot_video_frame_info *video_frame)
{
    td_ulong        flags;
    td_s32          ret = TD_FAILURE;
    pciv_pic        src_pic;
    pciv_fmw_chn    *fmw_chn = pciv_fmw_get_context(pciv_chn);

    pciv_fmw_set_src_pic(&src_pic, bind_obj, video_frame);

    pciv_fmw_spin_lock(flags);
    if (fmw_chn->is_start != TD_TRUE) {
        pciv_fmw_info_trace("chn%d have stopped \n", pciv_chn);
        pciv_fmw_spin_unlock(flags);
        goto out;
    }
    pciv_fmw_spin_unlock(flags);

    ret = pciv_check_input_frame_size(pciv_chn, &video_frame->video_frame);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("pic from mod(%d) send to pciv failed!\n", video_frame->mod_id);
        goto out;
    }

    /* add the VB( release in pciv_firmware_src_pic_free) */
    ret = pciv_fmw_vb_user_add(video_frame);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("add user vb failed for chn%d! ret: 0x%x.\n", pciv_chn, ret);
        goto out;
    }

    /* the callback function register by the upper mode is called to send the zoom Image by VGS */
    ret = g_pciv_fmw_call_back.pf_src_send_pic(pciv_chn, &src_pic);
    if (ret != TD_SUCCESS) {
        pciv_fmw_vb_user_sub(video_frame);
        pciv_fmw_info_trace("chn%d pf_src_send_pic failed! ret: 0x%x.\n", pciv_chn, ret);
        goto out;
    }

    pciv_fmw_spin_lock(flags);
    fmw_chn->send_cnt++;
    pciv_fmw_spin_unlock(flags);
out:
    if (ret != TD_SUCCESS && (!bind_obj->is_block || bind_obj->is_vpss_send)) {
        pciv_fmw_spin_lock(flags);
        fmw_chn->lost_cnt++;
        pciv_fmw_spin_unlock(flags);
        ret = TD_SUCCESS;
    }

    return ret;
}

static td_s32 pciv_fmw_zoom_cb_check(ot_pciv_chn pciv_chn, const pciv_fmw_chn *fmw_chn,
    const vgs_task_data *vgs_task)
{
    /* In VGS interrupt,maybe the pcivchn is stopped */
    if (fmw_chn->is_start != TD_TRUE) {
        pciv_fmw_info_trace("chn%d have stopped \n", pciv_chn);
        return TD_FAILURE;
    }

    /*
     * if the task fail,maybe the action of cancel job occur in pciv owner interrupt
     * maybe out of its owner job,it need judge next step, we cannot add lock,else maybe lock itself and other abnormal
     * if the vgs task fail,then retuen with failure
     */
    if (vgs_task->finish_stat != VGS_TASK_FNSH_STAT_OK) {
        pciv_fmw_err_trace("vgs task finish status is no ok, chn%d\n", pciv_chn);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_s32 pciv_fmw_zoom_pre(ot_pciv_chn pciv_chn, vb_blk_handle *vb_handle, vgs_task_data **vgs_task,
    ot_vb_calc_cfg *vb_calc_cfg, const ot_video_frame_info *src_frame)
{
    td_s32          ret;
    vgs_export_func *pfn_vgs_exp_func = (vgs_export_func *)cmpi_get_module_func_by_id(OT_ID_VGS);

    ret = pciv_fmw_get_vb_calc_cfg(pciv_chn, vb_calc_cfg);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("pciv_fmw get vgs vb_calc_cfg failed!\n");
        return ret;
    }

    *vgs_task = pfn_vgs_exp_func->pfn_vgs_get_free_task();
    if (*vgs_task == TD_NULL) {
        pciv_fmw_err_trace("pciv get task failed\n");
        return TD_FAILURE;
    }

    ret = pciv_fmw_get_vgs_out_vb(pciv_chn, vb_handle);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("pciv_fmw get vgs out_put VB failed!\n");
        goto free_task;
    }

    (td_void)memset_s(*vgs_task, sizeof(vgs_task_data), 0, sizeof(vgs_task_data));

    /* configure the input video frame */
    ret = memcpy_s(&(*vgs_task)->img_in, sizeof(ot_video_frame_info), src_frame, sizeof(ot_video_frame_info));
    if (ret != EOK) {
        pciv_fmw_err_trace("copy frame info to img_in failed!\n");
        goto free_task;
    }
    /* the output image is same with the input image */
    ret = memcpy_s(&(*vgs_task)->img_out, sizeof(ot_video_frame_info), src_frame, sizeof(ot_video_frame_info));
    if (ret != EOK) {
        pciv_fmw_err_trace("copy frame info to img_out failed!\n");
        goto free_task;
    }
    return TD_SUCCESS;
free_task:
    pfn_vgs_exp_func->pfn_vgs_put_free_task(*vgs_task);
    return TD_FAILURE;
}

static td_s32 pciv_fmw_config_vgs_task(ot_pciv_chn pciv_chn, vgs_task_data *vgs_task, const pciv_bind_obj *obj,
    const ot_video_frame *out_frame, pciv_fmw_vgs_task *fmw_task)
{
    td_s32 ret;

    vgs_task->call_mod_id       = OT_ID_PCIV;
    vgs_task->call_dev_id       = 0;
    vgs_task->call_chn_id       = pciv_chn;
    vgs_task->private_data[0]   = obj->type;
    vgs_task->private_data[1]   = obj->is_vpss_send;
    vgs_task->private_data[2]   = obj->is_block;  /* 2: save the is_block to vgs private date 2 */

    fmw_task->vgs_in_vb_handle    = vgs_task->img_in.pool_id;
    fmw_task->vgs_out_vb_handle   = vgs_task->img_out.pool_id;
    if (out_frame->compress_mode != OT_COMPRESS_MODE_NONE) {
        fmw_task->out_vb_phys_addr = vgs_task->img_out.video_frame.header_phys_addr[0];
    } else {
        fmw_task->out_vb_phys_addr = vgs_task->img_out.video_frame.phys_addr[0];
    }
    if (vgs_task->img_in.video_frame.compress_mode != OT_COMPRESS_MODE_NONE) {
        fmw_task->in_vb_phys_addr = vgs_task->img_in.video_frame.header_phys_addr[0];
    } else {
        fmw_task->in_vb_phys_addr = vgs_task->img_in.video_frame.phys_addr[0];
    }

    ret = call_vb_user_add(fmw_task->vgs_in_vb_handle, fmw_task->in_vb_phys_addr, OT_VB_UID_PCIV);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("add user vb failed! chn%d\n", pciv_chn);
        return ret;
    }
    return TD_SUCCESS;
}

static td_void  pciv_fmw_release_vgs_vb(const pciv_fmw_vgs_task *fmw_task)
{
    call_vb_user_sub(fmw_task->vgs_out_vb_handle, fmw_task->out_vb_phys_addr, OT_VB_UID_PCIV);
    call_vb_user_sub(fmw_task->vgs_in_vb_handle, fmw_task->in_vb_phys_addr, OT_VB_UID_PCIV);
}

static td_void pciv_fmw_src_vgs_task_cb(ot_mod_id call_mod_id,
    td_u32 call_dev_id, td_u32 call_chn_id, const vgs_task_data *vgs_task)
{
    td_s32                      ret;
    td_ulong                    flags;
    ot_pciv_chn                 pciv_chn;
    pciv_bind_obj               bind_obj = {0};
    pciv_fmw_chn                *fmw_chn = TD_NULL;

    ot_unused(call_mod_id);
    ot_unused(call_dev_id);
    ot_unused(call_chn_id);

    pciv_fmw_check_ptr_void_return(vgs_task);

    pciv_chn = vgs_task->call_chn_id;
    pciv_fmw_check_chn_id_void_return(pciv_chn);

    fmw_chn = pciv_fmw_get_context(pciv_chn);

    /* the image finish used should released */
    pciv_fmw_vb_user_sub(&vgs_task->img_in);
    pciv_fmw_spin_lock(flags);
    fmw_chn->trans_cb_cnt++;
    if (fmw_chn->rgn_num != 0) {
        fmw_chn->osd_cb_cnt++;
    }
    pciv_fmw_spin_unlock(flags);

    bind_obj.type          = vgs_task->private_data[0];
    bind_obj.is_vpss_send  = vgs_task->private_data[1];
    bind_obj.is_block      = vgs_task->private_data[2]; /* 2: get the is_block from vgs private date 2 */

    ret = pciv_fmw_zoom_cb_check(pciv_chn, fmw_chn, vgs_task);
    if (ret != TD_SUCCESS) {
        goto out;
    }

    /* send the video after zoom */
    ret = pciv_firmware_send_src_pic(pciv_chn, &bind_obj, &vgs_task->img_out);
    if (ret != TD_SUCCESS) {
        goto out;
    }

out:
    pciv_fmw_put_rgn(pciv_chn, OT_RGN_OVERLAYEX);
    pciv_fmw_spin_lock(flags);
    fmw_chn->rgn_num = 0;
    pciv_fmw_spin_unlock(flags);
    if (bind_obj.type == PCIV_BIND_VDEC && bind_obj.is_vpss_send == TD_FALSE) {
        pciv_fmw_set_send_state(pciv_chn, ret, &bind_obj);
    }
    /* no matter success or not,this callback mst release the VB */
    pciv_fmw_vb_user_sub(&vgs_task->img_out);
}

static td_s32 pciv_fmw_src_vgs_task_work(ot_pciv_chn pciv_chn, const vgs_task_data *vgs_task,
    const vgs_online_opt *vgs_opt)
{
    td_s32          ret;
    vgs_job_data    job_data;
    ot_vgs_handle   vgs_handle;
    ot_mpp_chn      mpp_chn;
    vgs_export_func *pfn_vgs_exp_func = (vgs_export_func *)cmpi_get_module_func_by_id(OT_ID_VGS);
    pciv_fmw_chn    *fmw_chn    = pciv_fmw_get_context(pciv_chn);

    mpp_chn.mod_id = OT_ID_PCIV;
    mpp_chn.dev_id = 0;
    mpp_chn.chn_id = pciv_chn;

    job_data.job_type       = VGS_JOB_TYPE_NORMAL;
    job_data.job_call_back  = TD_NULL;
    /* 1.begin VGS job------------------------------------------------------ */
    ret = pfn_vgs_exp_func->pfn_vgs_begin_job(&vgs_handle, VGS_JOB_PRI_NORMAL, &mpp_chn, &job_data);
    if (ret != TD_SUCCESS) {
        fmw_chn->add_job_fail_cnt++;
        pciv_fmw_err_trace("begin vgs job failed! chn%d\n", pciv_chn);
        pfn_vgs_exp_func->pfn_vgs_put_free_task(vgs_task);
        return TD_FAILURE;
    }
    fmw_chn->add_job_suc_cnt++;

    /* 2.move the picture, add osd, scale picture-------------------------- */
    /* add task to VGS job */
    ret = pfn_vgs_exp_func->pfn_vgs_add_online_task(vgs_handle, vgs_task, vgs_opt);
    if (ret != TD_SUCCESS) {
        fmw_chn->trans_task_fail_cnt++;
        if (fmw_chn->rgn_num != 0) {
            fmw_chn->osd_task_fail_cnt++;
        }
        pciv_fmw_err_trace("create vgs task failed! chn%d\n", pciv_chn);
        pfn_vgs_exp_func->pfn_vgs_cancel_job(vgs_handle);
        return TD_FAILURE;
    }

    fmw_chn->trans_task_suc_cnt++;
    if (fmw_chn->rgn_num != 0) {
        fmw_chn->osd_task_suc_cnt++;
    }

    /* 3.end DSU job-------------------------------------------------------*/
    /* notes: if end_job failed, callback will called auto */
    ret = pfn_vgs_exp_func->pfn_vgs_end_job(vgs_handle, TD_TRUE, TD_NULL);
    if (ret != TD_SUCCESS) {
        fmw_chn->end_job_fail_cnt++;
        pciv_fmw_err_trace("end vgs job failed! chn%d\n", pciv_chn);
        pfn_vgs_exp_func->pfn_vgs_cancel_job(vgs_handle);
        return TD_FAILURE;
    }

    fmw_chn->end_job_suc_cnt++;
    return TD_SUCCESS;
}

static td_s32 pciv_fmw_src_vgs_task(ot_pciv_chn pciv_chn, const pciv_bind_obj *obj,
    const ot_video_frame_info *src_frame)
{
    td_s32                  ret;
    rgn_info                info;
    vb_blk_handle           vb_handle;
    ot_vb_calc_cfg          vb_calc_cfg;
    pciv_fmw_vgs_task       fmw_task;
    static vgs_online_opt   vgs_opt = { 0 };
    vgs_task_data           *vgs_task   = TD_NULL;
    ot_video_frame          *out_frame  = TD_NULL;
    pciv_fmw_chn            *fmw_chn    = pciv_fmw_get_context(pciv_chn);

    info.num = 0;
    ret = pciv_fmw_get_rgn(pciv_chn, OT_RGN_OVERLAYEX, &info);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    fmw_chn->rgn_num = info.num;

    /* config VGS optional */
    ret = pciv_fmw_config_vgs_opt(&info, &vgs_opt);
    if (ret != TD_SUCCESS) {
        goto put_rgn;
    }

    ret = pciv_fmw_zoom_pre(pciv_chn, &vb_handle, &vgs_task, &vb_calc_cfg, src_frame);
    if (ret != TD_SUCCESS) {
        goto put_rgn;
    }

    /* config the output video info */
    vgs_task->img_out.pool_id   = call_vb_handle_to_pool_id(vb_handle);
    out_frame                   = &vgs_task->img_out.video_frame;
    pciv_fmw_config_out_frame(pciv_chn, vb_handle, &vb_calc_cfg, out_frame);

    /* config the member or vgs task structure(sending image in VGS callback function) */
    vgs_task->call_back = pciv_fmw_src_vgs_task_cb;
    ret = pciv_fmw_config_vgs_task(pciv_chn, vgs_task, obj, out_frame, &fmw_task);
    if (ret != TD_SUCCESS) {
        goto put_rgn;
    }

    ret = pciv_fmw_src_vgs_task_work(pciv_chn, vgs_task, &vgs_opt);
    if (ret != TD_SUCCESS) {
        pciv_fmw_release_vgs_vb(&fmw_task);
        goto put_rgn;
    }
    return TD_SUCCESS;
put_rgn:
    pciv_fmw_put_rgn(pciv_chn, OT_RGN_OVERLAYEX);
    fmw_chn->rgn_num = 0;
    return ret;
}

/* be called in VIU interrupt handler or VDEC interrupt handler or vpss send handler . */
td_s32 pciv_firmware_send_vgs_pic(ot_pciv_chn pciv_chn, const pciv_bind_obj *obj, const ot_video_frame_info *pic_info)
{
    pciv_fmw_chn *fmw_chn = TD_NULL;

    /********************************************************************************************
    * the image send to PCIV from VDEC is 16-byte align,but the image send by pciv is not align.
    * then when PCIV send the image date from vdec directly, at this time, when the master chip
    * get data,it will calculate the address of UV, the err of 8 lines image data dislocation will
    * appear, so we propose to the data should through VPSS or VGS in the slave chip.it will reload
    * as the format that PCIV sending need. but,if the image data do not through VPSS, because of
    * the performance of VGS if weak, at this time, the system send spead is limited by VGS, so for
    * the difference difference of align format, PCIV cannot send the image directly from vdec, but
    * through this function handle.
    **********************************************************************************************/
    /* PCIV channel must be start */
    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_start != TD_TRUE) {
        return TD_FAILURE;
    }

    if (g_drop_err_timeref == 1) {
        /*
         * prevent out-of-order when send VI  or VO source image,
         * drop the out-of-order fram in the video(time_ref must increased)
         */
        if ((obj->type == PCIV_BIND_VI) || (obj->type == PCIV_BIND_VO)) {
            if (pic_info->video_frame.time_ref < fmw_chn->time_ref) {
                pciv_fmw_err_trace("pciv %d, time_ref err, (%d,%d)\n",
                    pciv_chn, pic_info->video_frame.time_ref, fmw_chn->time_ref);
                return TD_FAILURE;
            }
            fmw_chn->time_ref = pic_info->video_frame.time_ref;
        }
    }

    /* if need put osd to the source vide, process:move->put OSD->zoom */
    pciv_fmw_info_trace("chn%d support osd right now\n", pciv_chn);
    return pciv_fmw_src_vgs_task(pciv_chn, obj, pic_info);
}

static td_bool pciv_firmware_is_vgs_bypass(ot_pciv_chn pciv_chn, const ot_video_frame_info *frame_info)
{
    td_s32        ret;
    pciv_fmw_chn  *fmw_chn    = TD_NULL;
    rgn_info      info;

    info.num = 0;
    ret = pciv_fmw_get_rgn(pciv_chn, OT_RGN_OVERLAYEX, &info);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("get region failed\n");
        info.num = 0;
    }

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    /* the pic size or format is not the same or it needs to add osd */
    if (fmw_chn->pic_attr.width != frame_info->video_frame.width ||
        fmw_chn->pic_attr.height != frame_info->video_frame.height ||
        fmw_chn->pic_attr.compress_mode != frame_info->video_frame.compress_mode ||
        fmw_chn->pic_attr.pixel_format != frame_info->video_frame.pixel_format ||
        fmw_chn->pic_attr.dynamic_range != frame_info->video_frame.dynamic_range ||
        fmw_chn->pic_attr.video_format != frame_info->video_frame.video_format ||
        (info.num > 0)) {
        pciv_fmw_put_rgn(pciv_chn, OT_RGN_OVERLAYEX);
        return TD_FALSE;
    }
    pciv_fmw_put_rgn(pciv_chn, OT_RGN_OVERLAYEX);

    return TD_TRUE;
}

static td_s32 pciv_firmware_send_frame(ot_pciv_chn pciv_chn, pciv_bind_obj *obj,
    const ot_video_frame_info *frame_info)
{
    td_ulong     flags;
    td_s32       ret;
    pciv_fmw_chn *fmw_chn = pciv_fmw_get_context(pciv_chn);
    td_bool      is_vdec_pic = (obj->type == PCIV_BIND_VDEC && obj->is_vpss_send == TD_FALSE);

    if (pciv_firmware_is_vgs_bypass(pciv_chn, frame_info)) {
        /* send picture directly */
        obj->is_vgs_bypass = TD_TRUE;
        ret = pciv_firmware_send_src_pic(pciv_chn, obj, frame_info);
    } else {
        obj->is_vgs_bypass = TD_FALSE;
        pciv_fmw_spin_lock(flags);
        ret = pciv_firmware_send_vgs_pic(pciv_chn, obj, frame_info);
        if (is_vdec_pic && ret == TD_SUCCESS) {
            fmw_chn->send_state = PCIVFMW_SEND_ING;
        }
        pciv_fmw_spin_unlock(flags);
    }
    return ret;
}

static td_s32 pciv_send_vdec_pic_timer_state_check(ot_pciv_chn pciv_chn, pciv_fmw_chn *fmw_chn)
{
    if (fmw_chn->is_start == TD_FALSE) {
        return TD_FAILURE;
    }

    if (fmw_chn->send_state == PCIVFMW_SEND_ING) {
        return TD_FAILURE;
    }

    if (fmw_chn->send_state == PCIVFMW_SEND_OK) {
        /*
         * check the last time is success or not(the first is the success state)
         * get the new vdec image info
         */
        fmw_chn->cur_vdec_node = pciv_pic_queue_get_busy(&fmw_chn->pic_queue);
        if (fmw_chn->cur_vdec_node == TD_NULL) {
            pciv_fmw_info_trace("chn%d no vdec pic\n", pciv_chn);
            return TD_FAILURE;
        }
        return TD_SUCCESS;
    }

    if (fmw_chn->send_state == PCIVFMW_SEND_NOK) {
        if (fmw_chn->is_master == TD_TRUE) {
            /* if the last time is not success,get and send  the data of last time again */
            fmw_chn->cur_vdec_node = pciv_pic_queue_get_busy(&fmw_chn->pic_queue);
        }
        if (fmw_chn->cur_vdec_node == TD_NULL) {
            pciv_fmw_info_trace("chn%d no vdec pic\n", pciv_chn);
            return TD_FAILURE;
        }
        return TD_SUCCESS;
    }

    pciv_fmw_info_trace("chn%d send vdec pic state error %#x\n", pciv_chn, fmw_chn->send_state);
    return TD_FAILURE;
}

static td_void pciv_send_vdec_pic_timer_work(ot_pciv_chn pciv_chn, pciv_fmw_chn *fmw_chn)
{
    td_ulong            flags;
    td_s32              ret;
    pciv_bind_obj       obj     = {0};
    ot_video_frame_info *video_frame_info = TD_NULL;
    ot_mpp_chn          mpp_chn;

    pciv_fmw_check_ptr_void_return(fmw_chn);
    pciv_fmw_check_ptr_void_return(fmw_chn->cur_vdec_node);

    video_frame_info = &fmw_chn->cur_vdec_node->pciv_pic.frame_info;
    if (fmw_chn->is_master == TD_TRUE) {
        pciv_fmw_spin_lock(flags);
        fmw_chn->send_state = PCIVFMW_SEND_ING;

        /* send the image from vdec to VPSS/VENC/VO */
        pciv_fmw_warn_trace("chn%d, start: %d, start send pic to VPSS/VO.\n", pciv_chn, fmw_chn->is_start);
        call_sys_set_mpp_chn(&mpp_chn, OT_ID_PCIV, PCIV_DEV_ID, pciv_chn);
        ret = call_sys_send_data(&mpp_chn, fmw_chn->cur_vdec_node->pciv_pic.is_block,
                                 MPP_DATA_VDEC_FRAME, video_frame_info);
        if ((ret != TD_SUCCESS) &&
            (fmw_chn->cur_vdec_node->pciv_pic.is_block == TD_TRUE)) {
            pciv_fmw_warn_trace("chn%d, start: %d, finish send pic to VPSS/VO.\n",
                pciv_chn, fmw_chn->is_start);
            pciv_fmw_info_trace("chn%d send pic failed, put to queue and send again. ret: 0x%x\n",
                pciv_chn, ret);
            pciv_pic_queue_put_busy_head(&fmw_chn->pic_queue, fmw_chn->cur_vdec_node);
            fmw_chn->send_state     = PCIVFMW_SEND_NOK;
            fmw_chn->cur_vdec_node  = TD_NULL;
        } else {
            pciv_fmw_warn_trace("chn%d, start: %d, finish send pic to VPSS/VO.\n",
                pciv_chn, fmw_chn->is_start);
            pciv_fmw_info_trace("chn%d send pic ok\n", pciv_chn);
            fmw_chn->is_pciv_hold[fmw_chn->cur_vdec_node->pciv_pic.index] = TD_FALSE;
            pciv_pic_queue_put_free(&fmw_chn->pic_queue, fmw_chn->cur_vdec_node);

            fmw_chn->send_state     = PCIVFMW_SEND_OK;
            fmw_chn->cur_vdec_node  = TD_NULL;
            pciv_firmware_recv_pic_free(&fmw_chn->buf_timer);
        }
        pciv_fmw_spin_unlock(flags);
    } else {
        /* send the vdec image to PCI target */
        obj.type         = PCIV_BIND_VDEC;
        obj.is_vpss_send = TD_FALSE;
        obj.is_block     = fmw_chn->cur_vdec_node->pciv_pic.is_block;
        ret = pciv_firmware_send_frame(pciv_chn, &obj, video_frame_info);
        /* when ret is success and not vgs bypass, means starting send frame(send state is
         * PCIVFMW_SEND_ING), can't release
         */
        if (ret != TD_SUCCESS || obj.is_vgs_bypass) {
            pciv_fmw_set_send_state(pciv_chn, ret, &obj);
        }
    }
}

/* timer function of receiver(master chip): get data from pciv and send to vpss/venc/vo */
/* timer function of sender(slave chip): get data from vdec and send to pciv */
td_void pciv_send_vdec_pic_timer_func(struct timer_list *vdec_timer)
{
    td_ulong     flags;
    td_s32       ret;
    ot_pciv_chn  pciv_chn;
    pciv_fmw_chn *fmw_chn = TD_NULL;

    ot_unused(vdec_timer);

    /* timer will be restarted after 1 tick */
    g_timer_pciv_send_vdec_pic.expires = jiffies + msecs_to_jiffies(OT_PCIV_TIMER_EXPIRES);
    g_timer_pciv_send_vdec_pic.function = pciv_send_vdec_pic_timer_func;

    if (!timer_pending(&g_timer_pciv_send_vdec_pic)) {
        add_timer(&g_timer_pciv_send_vdec_pic);
    }

    for (pciv_chn = 0; pciv_chn < PCIV_FMW_MAX_CHN_NUM; pciv_chn++) {
        pciv_fmw_spin_lock(flags);
        fmw_chn = pciv_fmw_get_context(pciv_chn);
        fmw_chn->timer_cnt++;

        ret = pciv_send_vdec_pic_timer_state_check(pciv_chn, fmw_chn);
        if (ret != TD_SUCCESS) {
            pciv_fmw_spin_unlock(flags);
            continue;
        }
        pciv_fmw_spin_unlock(flags);
        pciv_send_vdec_pic_timer_work(pciv_chn, fmw_chn);
    }
}

static td_s32 pciv_firmware_send_pic_from_vi(td_s32 dev_id, ot_pciv_chn pciv_chn,
    td_bool is_block, const ot_video_frame_info *frame_info)
{
    td_s32              ret;
    pciv_bind_obj       obj         = {0};

    obj.type         = PCIV_BIND_VI;
    obj.is_vpss_send = TD_FALSE;
    obj.is_block     = is_block;

    ret = pciv_firmware_send_frame(pciv_chn, &obj, frame_info);
    if (ret != TD_SUCCESS) {
        pciv_fmw_debug_trace("dev_id:%d viu frame pic send failed\n", dev_id);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VO_VIRTDEV_SUPPORT
static td_s32 pciv_firmware_send_pic_from_vo(td_s32 dev_id, ot_pciv_chn pciv_chn,
    td_bool is_block, const ot_video_frame_info *frame_info)
{
    td_s32              ret;
    pciv_bind_obj       obj         = {0};

    obj.type         = PCIV_BIND_VO;
    obj.is_vpss_send = TD_FALSE;
    obj.is_block     = is_block;

    ret = pciv_firmware_send_frame(pciv_chn, &obj, frame_info);
    if (ret != TD_SUCCESS) {
        pciv_fmw_debug_trace("dev_id:%d virtual vou frame pic send failed\n", dev_id);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}
#endif

static td_s32 pciv_firmware_send_pic_from_vdec(td_s32 dev_id, ot_pciv_chn pciv_chn,
    td_bool is_block, const ot_video_frame_info *frame_info)
{
    td_ulong        flags;
    pciv_pic_node   *node       = TD_NULL;
    pciv_fmw_chn    *fmw_chn    = TD_NULL;
    td_s32          ret;

    pciv_fmw_spin_lock(flags);
    fmw_chn = pciv_fmw_get_context(pciv_chn);

    node = pciv_pic_queue_get_free(&fmw_chn->pic_queue);
    if (node == TD_NULL) {
        pciv_fmw_debug_trace("dev_id:%d no free node\n", dev_id);
        pciv_fmw_spin_unlock(flags);
        return TD_FAILURE;
    }

    node->pciv_pic.is_block = is_block;
    ret = memcpy_s(&node->pciv_pic.frame_info, sizeof(ot_video_frame_info), frame_info, sizeof(ot_video_frame_info));
    if (ret != EOK) {
        pciv_pic_queue_put_free(&fmw_chn->pic_queue, node);
        pciv_fmw_err_trace("dev_id:%d copy frame info failed\n", dev_id);
        pciv_fmw_spin_unlock(flags);
        return TD_FAILURE;
    }
    ret = pciv_fmw_vb_user_add(frame_info);
    if (ret != TD_SUCCESS) {
        pciv_pic_queue_put_free(&fmw_chn->pic_queue, node);
        pciv_fmw_err_trace("add user vb failed for chn%d! ret: 0x%x.\n", pciv_chn, ret);
        pciv_fmw_spin_unlock(flags);
        return ret;
    }
    pciv_pic_queue_put_busy(&fmw_chn->pic_queue, node);
    pciv_fmw_spin_unlock(flags);
    return TD_SUCCESS;
}

/* called in VIU, virtual VOU or VDEC interrupt handler */
td_s32 pciv_firmware_send_pic(td_s32 dev_id, td_s32 chn_id, td_bool is_block, mpp_data_type data_type,
    td_void *pic_info)
{
    td_ulong            flags;
    td_s32              ret         = TD_FAILURE;
    pciv_fmw_chn        *fmw_chn    = TD_NULL;
    ot_pciv_chn         pciv_chn    = chn_id;
    ot_video_frame_info *frame_info = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(pic_info);

    frame_info = (ot_video_frame_info *)pic_info;

    pciv_fmw_spin_lock(flags);
    fmw_chn = pciv_fmw_get_context(pciv_chn);
    if (fmw_chn->is_start != TD_TRUE) {
        pciv_fmw_spin_unlock(flags);
        pciv_fmw_info_trace("chn%d have stopped \n", pciv_chn);
        return TD_FAILURE;
    }
    fmw_chn->get_cnt++;
    pciv_fmw_spin_unlock(flags);

    switch (data_type) {
        case MPP_DATA_VI_FRAME:
            /* fall-through */
        case MPP_DATA_AVS_FRAME:
            /* fall-through */
        case MPP_DATA_VPSS_FRAME:
            ret = pciv_firmware_send_pic_from_vi(dev_id, pciv_chn, is_block, frame_info);
            break;
#ifdef CONFIG_OT_VO_VIRTDEV_SUPPORT
        case MPP_DATA_VOU_FRAME:
            ret = pciv_firmware_send_pic_from_vo(dev_id, pciv_chn, is_block, frame_info);
            break;
#endif
        case MPP_DATA_VDEC_FRAME:
            ret = pciv_firmware_send_pic_from_vdec(dev_id, pciv_chn, is_block, frame_info);
            break;
        default:
            pciv_fmw_debug_trace("dev_id:%d data type:%d error\n", dev_id, data_type);
    }

    if (!is_block && ret != TD_SUCCESS) {
        pciv_fmw_spin_lock(flags);
        fmw_chn->lost_cnt++;
        pciv_fmw_spin_unlock(flags);
        return TD_SUCCESS;
    } else {
        return ret;
    }
}

#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
static td_s32 pciv_fmw_vpss_query_auto(td_s32 dev_id, td_s32 chn_id,
    const vpss_query_info *query_info, const pciv_fmw_chn *fmw_chn, vpss_inst_info *inst_info)
{
    td_s32          ret;
    vb_base_info    base_info;
    vb_blk_handle   vb_handle;
    ot_vb_calc_cfg  vb_calc_cfg;
    ot_video_frame  *video_frame = TD_NULL;
    ot_mpp_chn      mpp_chn      = {0};
    td_char         *mmz_name    = TD_NULL;

    base_info.mod_id = OT_ID_PCIVFMW;
    base_info.is_3dnr_buffer = TD_FALSE;
    base_info.align = 0;

    base_info.compress_mode = fmw_chn->pic_attr.compress_mode;
    base_info.dynamic_range = fmw_chn->pic_attr.dynamic_range;
    base_info.video_format  = fmw_chn->pic_attr.video_format;
    base_info.pixel_format  = fmw_chn->pic_attr.pixel_format;
    base_info.width         = fmw_chn->pic_attr.width;
    base_info.height        = fmw_chn->pic_attr.height;

    call_sys_get_vb_cfg(&base_info, &vb_calc_cfg);

    mpp_chn.mod_id = OT_ID_PCIV;
    mpp_chn.dev_id = dev_id;
    mpp_chn.chn_id = chn_id;
    ret = call_sys_get_mmz_name(&mpp_chn, (td_void **)&mmz_name);
    if (ret != TD_SUCCESS) {
        pciv_fmw_err_trace("chn%d get mmz_name failed!\n", chn_id);
        return TD_FAILURE;
    }

    if (query_info->src_pic_info->mod_id == OT_ID_VDEC) {
        ret = g_pciv_fmw_call_back.pf_query_pciv_chn_share_buf_state(chn_id);
        if (ret != TD_SUCCESS) {
            pciv_fmw_info_trace("chn%d has no free share buf to receive pic!\n", chn_id);
            return TD_FAILURE;
        }
    }

    vb_handle = call_vb_get_blk_by_size(fmw_chn->blk_size, OT_VB_UID_VPSS, mmz_name);
    if (vb_handle == OT_VB_INVALID_HANDLE) {
        g_pciv_fmw_call_back.pf_free_pciv_chn_share_buf_state(chn_id);
        pciv_fmw_alert_trace("get VB for pic_size: %d from ddr:%s fail, for chn%d VPSS query\n",
            fmw_chn->blk_size, mmz_name, chn_id);
        return TD_FAILURE;
    }

    inst_info->dest_pic_info[0].video_frame.pool_id = call_vb_handle_to_pool_id(vb_handle);
    video_frame = &inst_info->dest_pic_info[0].video_frame.video_frame;

    pciv_fmw_vpss_query_set_video_frame(chn_id, vb_handle, &vb_calc_cfg, query_info, video_frame);

    inst_info->vpss_proc = TD_TRUE;
    inst_info->new_frame = TD_TRUE;
    return TD_SUCCESS;
}

static td_void pciv_fmw_vpss_query_old_undo(pciv_fmw_chn *fmw_chn, vpss_inst_info *inst_info)
{
    inst_info->vpss_proc     = TD_FALSE;
    inst_info->new_frame     = TD_FALSE;
    inst_info->double_frame  = TD_FALSE;
    inst_info->update_backup = TD_FALSE;
    fmw_chn->old_undo_cnt++;
}

td_s32 pciv_fmw_vpss_query(td_s32 dev_id, td_s32 chn_id,
    vpss_query_info *query_info, vpss_inst_info *inst_info)
{
    td_ulong        flags;
    td_s32          ret;
    ot_pciv_chn     pciv_chn    = chn_id;
    pciv_fmw_chn    *fmw_chn    = TD_NULL;

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(query_info);
    pciv_fmw_check_ptr_return(inst_info);

    pciv_fmw_spin_lock(flags);
    fmw_chn = pciv_fmw_get_context(pciv_chn);

    ret = pciv_fmw_state_check(pciv_chn);
    if (ret != TD_SUCCESS) {
        pciv_fmw_spin_unlock(flags);
        return ret;
    }

    ret = pciv_fmw_vpss_query_mod_check(pciv_chn, query_info);
    if (ret != TD_SUCCESS) {
        goto old_undo;
    }
    /* auto */
    if (!ckfn_sys_get_vb_cfg()) {
        pciv_fmw_err_trace("sys_get_vb_cfg is null!\n");
        pciv_fmw_spin_unlock(flags);
        return TD_FAILURE;
    }
    ret = pciv_fmw_vpss_query_auto(dev_id, chn_id, query_info, fmw_chn, inst_info);
    if (ret != TD_SUCCESS) {
        goto old_undo;
    }

    fmw_chn->new_do_cnt++;
    pciv_fmw_spin_unlock(flags);
    return TD_SUCCESS;

old_undo:
    pciv_fmw_vpss_query_old_undo(fmw_chn, inst_info);
    pciv_fmw_spin_unlock(flags);
    return TD_SUCCESS;
}
#endif

td_s32 pciv_fmw_vpss_send(td_s32 dev_id, td_s32 chn_id, vpss_send_info *send_info)
{
    td_ulong        flags;
    td_s32          ret;
    pciv_bind_obj   bind_obj;
    ot_pciv_chn     pciv_chn    = chn_id;
    pciv_fmw_chn    *fmw_chn    = TD_NULL;
    vpss_pic_info   *pic_info   = TD_NULL;

    ot_unused(dev_id);

    pciv_fmw_check_chn_id_return(pciv_chn);
    pciv_fmw_check_ptr_return(send_info);

    fmw_chn = pciv_fmw_get_context(pciv_chn);
    ret = pciv_fmw_vpss_send_state_check(pciv_chn, send_info);
    if (ret != TD_SUCCESS) {
        goto send_fail;
    }
    pciv_fmw_check_ptr_return(send_info->dest_pic_info[0]);

    pciv_fmw_spin_lock(flags);
    fmw_chn->get_cnt++;
    pciv_fmw_spin_unlock(flags);

    pciv_fmw_vpss_send_get_bind_obj(send_info, &bind_obj);

    pic_info = send_info->dest_pic_info[0];
    ret = pciv_firmware_send_frame(pciv_chn, &bind_obj, &(pic_info->video_frame));
    if (ret != TD_SUCCESS) {
        pciv_fmw_spin_lock(flags);
        fmw_chn->lost_cnt++;
        pciv_fmw_spin_unlock(flags);
        goto send_fail;
    }

    return TD_SUCCESS;
send_fail:
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
    g_pciv_fmw_call_back.pf_free_pciv_chn_share_buf_state(chn_id);
#endif
    pciv_fmw_alert_trace("vpss send pic failed! chn%d\n", pciv_chn);
    return ret;
}

td_s32 pciv_fmw_reset_call_back(td_s32 dev_id, td_s32 chn_id, td_void *pv_data)
{
    td_ulong        flags;
    td_u32          i;
    td_s32          ret;
    td_u32          busy_num;
    ot_pciv_chn     pciv_chn;
    pciv_fmw_chn    *fmw_chn = TD_NULL;
    pciv_pic_node   *node = TD_NULL;

    ot_unused(dev_id);
    ot_unused(pv_data);

    pciv_chn = chn_id;

    pciv_firmware_wait_send_end(pciv_chn);

    pciv_fmw_spin_lock(flags);
    fmw_chn = pciv_fmw_get_context(pciv_chn);
    /* put the node from busy to free */
    busy_num = pciv_pic_queue_get_busy_num(&fmw_chn->pic_queue);
    for (i = 0; i < busy_num; i++) {
        node = pciv_pic_queue_get_busy(&fmw_chn->pic_queue);
        if (node == TD_NULL) {
            pciv_fmw_spin_unlock(flags);
            pciv_fmw_err_trace("get busy node failed! chn%d\n", pciv_chn);
            return TD_FAILURE;
        }

        ret = call_vb_user_sub(node->pciv_pic.frame_info.pool_id,
                               node->pciv_pic.frame_info.video_frame.phys_addr[0], OT_VB_UID_PCIV);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("call_vb_user_sub failed! chn%d\n", pciv_chn);
        }

        pciv_pic_queue_put_free(&fmw_chn->pic_queue, node);
    }

    fmw_chn->pic_queue.max      = PCIV_FMW_MAX_NODE_NUM;
    fmw_chn->pic_queue.free_num = PCIV_FMW_MAX_NODE_NUM;
    fmw_chn->pic_queue.busy_num = 0;

    pciv_fmw_spin_unlock(flags);

    return TD_SUCCESS;
}

td_s32 pciv_firmware_register_func(const pciv_fmw_callback *call_back)
{
    td_s32 ret;

    pciv_fmw_check_ptr_return(call_back);
    pciv_fmw_check_ptr_return(call_back->pf_src_send_pic);
    pciv_fmw_check_ptr_return(call_back->pf_recv_pic_free);
#ifdef CONFIG_OT_VPSS_AUTO_SUPPORT
    pciv_fmw_check_ptr_return(call_back->pf_query_pciv_chn_share_buf_state);
    pciv_fmw_check_ptr_return(call_back->pf_free_pciv_chn_share_buf_state);
#endif

    ret = memcpy_s(&g_pciv_fmw_call_back, sizeof(pciv_fmw_callback), call_back, sizeof(pciv_fmw_callback));
    if (ret != EOK) {
        pciv_fmw_alert_trace("copy callback func failed!\n");
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 pciv_fmw_init_base(td_void)
{
    td_s32              i;
    td_s32              ret;
    bind_sender_info    sender_info;
    bind_receiver_info  receiver;

    if ((ckfn_sys_entry() == TD_FALSE) ||
        (ckfn_sys_register_sender() == TD_FALSE) || (ckfn_sys_unregister_sender() == TD_FALSE) ||
        (ckfn_sys_register_receiver() == TD_FALSE) || (ckfn_sys_unregister_receiver() == TD_FALSE)) {
        pciv_fmw_err_trace("sys is not ready!\n");
        return TD_FAILURE;
    }

    sender_info.mod_id = OT_ID_PCIV;
    sender_info.max_dev_cnt = 1;
    sender_info.max_chn_cnt = OT_PCIV_MAX_CHN_NUM;
    sender_info.give_bind_call_back = TD_NULL;
    ret = call_sys_register_sender(&sender_info);
    if (ret != TD_SUCCESS) {
        pciv_fmw_alert_trace("register sender failed\n");
        return ret;
    }

    receiver.mod_id = OT_ID_PCIV;
    receiver.max_dev_cnt = 1;
    receiver.max_chn_cnt = OT_PCIV_MAX_CHN_NUM;
    receiver.call_back = pciv_firmware_send_pic;
    receiver.reset_call_back = pciv_fmw_reset_call_back;
    ret = call_sys_register_receiver(&receiver);
    if (ret != TD_SUCCESS) {
        pciv_fmw_alert_trace("register receiver failed\n");
        call_sys_unregister_sender(OT_ID_PCIV);
        return ret;
    }

    timer_setup(&g_timer_pciv_send_vdec_pic, pciv_send_vdec_pic_timer_func, 0);
    g_timer_pciv_send_vdec_pic.expires  = jiffies + msecs_to_jiffies(FMW_OT_PCIV_TIMER_EXPIRES);
    g_timer_pciv_send_vdec_pic.function = pciv_send_vdec_pic_timer_func;
    add_timer(&g_timer_pciv_send_vdec_pic);

    g_vb_pool.pool_cnt = 0;

    ret = memset_s(g_fmw_pciv_chn, sizeof(g_fmw_pciv_chn), 0, sizeof(g_fmw_pciv_chn));
    if (ret != TD_SUCCESS) {
        pciv_fmw_alert_trace("init pciv chn failed\n");
        call_sys_unregister_receiver(OT_ID_PCIV);
        call_sys_unregister_sender(OT_ID_PCIV);
        return TD_FAILURE;
    }
    for (i = 0; i < PCIV_FMW_MAX_CHN_NUM; i++) {
        timer_setup(&g_fmw_pciv_chn[i].buf_timer, TD_NULL, 0);
    }
    return TD_SUCCESS;
}

static td_void pciv_fmw_exit_base(td_void)
{
    td_s32 i;
    td_s32 ret;

    if ((ckfn_sys_entry() == TD_FALSE) || (ckfn_sys_unregister_sender() == TD_FALSE) ||
        (ckfn_sys_unregister_receiver() == TD_FALSE)) {
        pciv_fmw_err_trace("sys is not ready!\n");
        return;
    }

    call_sys_unregister_receiver(OT_ID_PCIV);
    call_sys_unregister_sender(OT_ID_PCIV);

    for (i = 0; i < PCIV_FMW_MAX_CHN_NUM; i++) {
        /* stop channel */
        ret = pciv_firmware_stop(i);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("stop pciv channel %d failed\n", i);
        }
        /* destroy channel */
        ret = pciv_firmware_destroy(i);
        if (ret != TD_SUCCESS) {
            pciv_fmw_alert_trace("destroy pciv channel %d failed\n", i);
        }
        del_timer_sync(&g_fmw_pciv_chn[i].buf_timer);
    }

    del_timer_sync(&g_timer_pciv_send_vdec_pic);
}

td_s32 pciv_fmw_init(td_void *p)
{
    td_s32 ret;

    ot_unused(p);

    if (g_pciv_fmw_state == PCIVFMW_STATE_STARTED) {
        return TD_SUCCESS;
    }

    ret = pciv_fmw_init_base();
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = pciv_fmw_init_vpss();
    if (ret != TD_SUCCESS) {
        pciv_fmw_exit_base();
        return ret;
    }

    /* register OVERLAY to region */
    ret = pciv_fmw_init_rgn();
    if (ret != TD_SUCCESS) {
        pciv_fmw_exit_vpss();
        pciv_fmw_exit_base();
        return ret;
    }

    g_pciv_fmw_state = PCIVFMW_STATE_STARTED;
    return TD_SUCCESS;
}

td_void pciv_fmw_exit(td_void)
{
    if (g_pciv_fmw_state == PCIVFMW_STATE_STOPED) {
        return;
    }

    pciv_fmw_exit_rgn();
    pciv_fmw_exit_vpss();
    pciv_fmw_exit_base();

    g_pciv_fmw_state = PCIVFMW_STATE_STOPED;
}

static td_void pciv_fmw_notify(mod_notice_id notice)
{
    if (notice == MOD_NOTICE_STOP) {
        g_pciv_fmw_state = PCIVFMW_STATE_STOPPING;
    }
}

static td_void pciv_fmw_query_state(mod_state *state)
{
    *state = MOD_STATE_FREE;
}

static td_u32 pciv_get_ver_magic(td_void)
{
    return VERSION_MAGIC;
}

static umap_module g_pciv_fmw_module = {
    .mod_id = OT_ID_PCIVFMW,
    .mod_name = "pcivfmw",

    .pfn_init = pciv_fmw_init,
    .pfn_exit = pciv_fmw_exit,
    .pfn_query_state = pciv_fmw_query_state,
    .pfn_notify = pciv_fmw_notify,
    .pfn_ver_checker = pciv_get_ver_magic,

    .export_funcs = TD_NULL,
    .data = TD_NULL,
};

static int __init pciv_fmw_mod_init(td_void)
{
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_proc_entry_t *proc = TD_NULL;

    proc = osal_create_proc_entry(PROC_ENTRY_PCIVFMW, TD_NULL);

    if (proc == TD_NULL) {
        osal_printk("PCIV firmware create proc error\n");
        goto proc_fail;
    }

    proc->read = pciv_fmw_proc_show;
#endif

    if (cmpi_register_module(&g_pciv_fmw_module)) {
        osal_printk("register pciv firmware module err.\n");
        goto register_fail;
    }

    osal_spin_lock_init(&g_pciv_fmw_lock);

    osal_printk("load pciv_firmware.ko ....OK!\n");

    return TD_SUCCESS;
register_fail:
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_PCIVFMW, TD_NULL);
proc_fail:
#endif
    osal_printk("load pciv_firmware.ko ....fail!\n");
    return TD_FAILURE;
}

static td_void __exit pciv_fmw_mod_exit(td_void)
{
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_PCIVFMW, TD_NULL);
#endif
    cmpi_unregister_module(OT_ID_PCIVFMW);

    osal_spin_lock_destroy(&g_pciv_fmw_lock);

    osal_printk("unload pciv_firmware.ko ....OK!\n");
}

EXPORT_SYMBOL(pciv_firmware_create);
EXPORT_SYMBOL(pciv_firmware_destroy);
EXPORT_SYMBOL(pciv_firmware_set_attr);
EXPORT_SYMBOL(pciv_firmware_start);
EXPORT_SYMBOL(pciv_firmware_stop);
EXPORT_SYMBOL(pciv_firmware_window_vb_create);
EXPORT_SYMBOL(pciv_firmware_window_vb_destroy);
EXPORT_SYMBOL(pciv_firmware_malloc);
EXPORT_SYMBOL(pciv_firmware_free);
EXPORT_SYMBOL(pciv_firmware_malloc_chn_buffer);
EXPORT_SYMBOL(pciv_firmware_free_chn_buffer);
EXPORT_SYMBOL(pciv_firmware_recv_pic_and_send);
EXPORT_SYMBOL(pciv_firmware_src_pic_free);
EXPORT_SYMBOL(pciv_firmware_register_func);

module_init(pciv_fmw_mod_init);
module_exit(pciv_fmw_mod_exit);

MODULE_AUTHOR("OT_MPP GRP");
MODULE_LICENSE("GPL");
MODULE_VERSION(OT_MPP_VERSION);
