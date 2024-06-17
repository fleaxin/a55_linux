/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_H
#define ISP_H

#include "ot_osal.h"
#include "mkp_isp.h"
#include "isp_ext.h"
#include "isp_vreg.h"
#include "isp_st.h"
#include "isp_block.h"
#include "vi_ext.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* vicap interrupt indication */
typedef enum {
    ISP_VICAP_INT_CHN0 = 0x1 << 8,
} isp_vicap_int;

#define isp_is_chn_int(chn_id, int_status) ((int_status) & (ISP_VICAP_INT_CHN0 << (chn_id)))


typedef struct  {
    isp_stat_info stat_info;
    struct osal_list_head list;
} isp_stat_node;

typedef struct  {
    td_bool init;
    td_phys_addr_t  phy_addr;
    td_void *vir_addr;
    td_u64  size;
    isp_stat_node node[MAX_ISP_STAT_BUF_NUM];
    isp_stat_info *act_stat;

    td_u32 user_num;
    td_u32 busy_num;
    td_u32 free_num;
    struct osal_list_head user_list;
    struct osal_list_head busy_list;
    struct osal_list_head free_list;
} isp_stat_buf;

typedef enum {
    ISP_BE_BUF_STATE_INIT = 0,
    ISP_BE_BUF_STATE_RUNNING,
    ISP_BE_BUF_STATE_FINISH,
    ISP_BE_BUF_STATE_SWITCH_START,
    ISP_BE_BUF_STATE_SWITCH,
    ISP_BE_BUF_STATE_BUTT
} isp_running_state;

typedef enum {
    ISP_BE_BUF_READY = 0,
    ISP_BE_BUF_WAITING,
    ISP_BE_BUF_EXIT,
    ISP_BE_BUF_BUTT
} isp_exit_state;

typedef enum {
    ISP_CFG_SNS_VERY_SHORT_FRAME = 0, /* main pipe */
    ISP_CFG_SNS_SHORT_FRAME = 0x10,
    ISP_CFG_SNS_MIDDLE_FRAME = 0x20,
    ISP_CFG_SNS_LONG_FRAME = 0x30,
    ISP_CFG_SNS_BUTT
} isp_cfg_sensor_time;

/* Sync Cfg */
#define EXP_RATIO_MAX          0x3FFF /* max expratio 256X */
#define EXP_RATIO_MIN          0x40  /* min expratio 1X */

#define ISP_DIGITAL_GAIN_SHIFT 8    /* ISP digital gain register shift */
#define ISP_DIGITAL_GAIN_MAX   0xFFFF  /* max ISP digital gain 256X */
#define ISP_DIGITAL_GAIN_MIN   0x1  /* min ISP digital gain 1/256X */

typedef struct  {
    td_u64 global_avg_sum_r;
    td_u64 global_avg_sum_gr;
    td_u64 global_avg_sum_gb;
    td_u64 global_avg_sum_b;
} isp_ae_global_avg_sum;

typedef struct  {
    td_u8                   buf_wr_flag;    /* FW write node */
    td_u8                   buf_rd_flag;    /* ISR read, then write to ISP reg or sensor */
    isp_sync_cfg_buf_node   sync_cfg_buf_node[ISP_SYNC_BUF_NODE_NUM];
} isp_sync_cfg_buf;

typedef struct  {
    td_u8  wdr_mode;
    td_u8  pre_wdr_mode;
    td_u8  vc_num;          /* if 3to1, vc_num = 0,1,2,0,1,...... */
    td_u8  vc_num_max;       /* if 3to1, vc_num_max = 2 */
    td_u8  vc_cfg_num;
    td_u8  cfg2_vld_dly_max;

    td_u64 pre_sns_gain;
    td_u64 cur_sns_gain;
    td_u64 pre_sns_gain_sf;
    td_u64 cur_sns_gain_sf;
    td_u32 drc_comp[CFG2VLD_DLY_LIMIT];  /* drc exposure compensation */
    td_u32 exp_ratio[OT_ISP_EXP_RATIO_NUM][CFG2VLD_DLY_LIMIT];
    td_u8  lf_mode[CFG2VLD_DLY_LIMIT];
    td_u32 wdr_gain[OT_ISP_WDR_MAX_FRAME_NUM][CFG2VLD_DLY_LIMIT];
    td_u8  alg_proc[CFG2VLD_DLY_LIMIT];
    isp_sync_cfg_buf_node  *node[CFG2VLD_DLY_LIMIT + 1];

    isp_sync_cfg_buf   sync_cfg_buf;
} isp_sync_cfg;

typedef struct  {
    td_bool mode_switch;
    td_s8   stitch_flag;
    td_u8   read_all;
    td_u8   first_read_all;
    td_u8   first_order;
    td_u8   all_init;
    td_u32  vi_callback_count;
    td_u64  pts;
} isp_sync_stitch_ctrl;

typedef struct {
    td_u64 isp_last_int_time;           /* Time of last interrupt, for debug */
    td_u64 isp_last_rate_time;          /* Time of last interrupt rate, for debug */
    td_u32 isp_int_cnt;                /* Count of interrupt, for debug */
    td_u32 isp_int_time;               /* Process time of interrupt, for debug */
    td_u32 isp_int_time_max;            /* Maximal process time of interrupt, for debug */
    td_u32 isp_int_gap_time;            /* Gap of two interrupts, for debug */
    td_u32 isp_int_gap_time_max;         /* Maximal gap of two interrupts, for debug */
    td_u32 isp_rate_int_cnt;            /* Count of interrupt rate, for debug */
    td_u32 isp_rate;                  /* Interrupt Rate, interrupt count per second, for debug */

    td_u64 pt_last_int_time;            /* Time of last interrupt, for debug */
    td_u64 pt_last_rate_time;           /* Time of last interrupt rate, for debug */
    td_u32 pt_int_cnt;                 /* Count of interrupt, for debug */
    td_u32 pt_int_time;                /* Process time of interrupt, for debug */
    td_u32 pt_int_time_max;             /* Maximal process time of interrupt, for debug */
    td_u32 pt_int_gap_time;             /* Gap of two interrupts, for debug */
    td_u32 pt_int_gap_time_max;          /* Maximal gap of two interrupts, for debug */
    td_u32 pt_rate_int_cnt;             /* Count of interrupt rate, for debug */
    td_u32 pt_rate;                   /* Interrupt Rate, interrupt count per second, for debug */

    td_u32 sensor_cfg_time;            /* Time of sensor config, for debug */
    td_u32 sensor_cfg_time_max;         /* Maximal time of sensor config, for debug */

    td_u32 isp_reset_cnt;              /* Count of ISP reset when vi width or height changed */
    td_u32 isp_be_sta_lost; /* Count of ISP BE statistics lost number When the ISP processing is not timely, for debug
                             */
} isp_drv_dbg_info;

typedef struct {
    td_u32 port_int_status;
    td_u32 isp_int_status;
    td_u32 port_int_err;
    td_u32 wch_int_status;
} isp_interrupt_sch;

typedef struct {
    isp_interrupt_sch int_sch[ISP_INTERRUPTS_SAVEINFO_MAX];
    td_u8 read_flag;
    td_u8 write_flag;
} isp_interrupt_info;

typedef struct {
    ot_vi_pipe vi_pipe_id;
    td_u32 isp_int_status;
} isp_slave_pipe_int_status;

typedef struct {
    td_bool hnr_en;
    td_bool pre_hnr_en;
    td_s32  off_switch_cnt;
    td_bool normal_blend;
} isp_hnr_info;

typedef struct {
    td_bool     print_en;
    td_u32      exposure_time[OT_ISP_WDR_MAX_FRAME_NUM]; /* exposure time (reciprocal of shutter speed), unit is us */
    td_u32      isp_dgain[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      again[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      dgain[OT_ISP_WDR_MAX_FRAME_NUM];
    td_s32      fe_id[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u64      vi_send_raw_cnt;
} isp_proc_frame_info;

typedef struct {
    td_bool                  edge;
    td_bool                  vd_start;
    td_bool                  vd_end;
    td_bool                  yuv_mode;
    td_bool                  vd_be_end;
    td_bool                  mem_init;
    td_bool                  isp_init;
    td_bool                  pub_attr_ok;
    td_bool                  stitch_sync;
    td_bool                  pro_enable;
    td_bool                  pro_start;
    td_bool                  run_once_ok;
    td_bool                  run_once_flag;
    td_bool                  yuv_run_once_ok;
    td_bool                  yuv_run_once_flag;
    td_bool                  isp_run_flag;
    td_bool                  bottom_half_cross_frame;

    td_char                  name[MAX_MMZ_NAME_LEN];
    td_u32                   int_pos;
    td_u32                   frame_cnt;
    td_u32                   status;
    td_u32                   pro_trig_flag;
    td_u32                   reg_cfg_info_flag;
    td_u8                    fpn_work_mode;
    td_u8                    *save_stt_stat;
    td_u64                   frame_pts;

    isp_chn_switch           chn_sel_attr[OT_ISP_STRIPING_MAX_NUM];
    isp_running_state        running_state;
    isp_exit_state           exit_state;
    isp_working_mode         work_mode;
    vi_stitch_attr           stitch_attr;
    vi_pipe_wdr_attr         wdr_attr;
    isp_wdr_cfg              wdr_cfg;
    isp_sync_cfg             sync_cfg;
    isp_stat_buf             statistics_buf;
    isp_kernel_reg_cfg       kernel_cfg[PING_PONG_NUM]; /* ping pong reg cfg info */
    isp_be_cfg_buf_info      be_buf_info;
    isp_be_buf_queue         be_buf_queue;
    isp_be_buf_node          *use_node;
    isp_be_sync_para         be_sync_para;
    isp_be_sync_para         be_sync_para_stitch[OT_ISP_MAX_PIPE_NUM];
    isp_be_sync_para         be_pre_sync_para_stitch[OT_ISP_MAX_PIPE_NUM];
    isp_sync_stitch_ctrl     isp_sync_ctrl;
    isp_be_sync_para         be_pre_sync_para;
    isp_be_lut2stt_attr      be_lut2stt_attr;
    isp_be_lut2stt_attr      be_pre_on_post_off_lut2stt_attr;
    isp_fe_stt_attr          fe_stt_attr;
    isp_fe_stit_stt_attr     fe_stit_stt_attr;
    isp_be_online_stt_info   be_online_stt_buf;
    isp_be_offline_stt_attr  be_off_stt_buf;
    isp_be_offline_stt_attr  be_pre_on_post_off_stt_buf;
    isp_be_stitch_stt_attr   be_off_stitch_stt_buf;
    isp_ldci_buf             ldci_read_buf_attr;
    isp_ldci_buf             ldci_write_buf_attr;
    isp_ldci_stt_addr        ldci_stt_addr;
    isp_be_offline_addr_info be_offline_addr_info;
    isp_clut_buf             clut_buf_attr;
    isp_drc_buf              drc_buf_attr;
    isp_fswdr_buf            fswdr_buf_attr;
    isp_kernel_tpr_filt_reg  bnr_tpr_filt;
    isp_bus_callback         bus_cb;
    isp_piris_callback       piris_cb;
    isp_drv_dbg_info         drv_dbg_info;
    ot_isp_pub_attr          proc_pub_info;
    isp_trans_info_buf       trans_info;
    isp_pro_info_buf         pro_info;
    isp_proc_mem             porc_mem;
    struct osal_semaphore    proc_sem;
    isp_interrupt_sch        int_sch[ISP_INTERRUPTS_SAVEINFO_MAX];   /* isp interrupt schedule */
    isp_interrupt_info       isp_int_info;
    isp_dynamic_blc_info     dyna_blc_info;
    isp_hnr_info             hnr_info;

    osal_wait_t              isp_wait;
    osal_wait_t              isp_wait_vd_start;
    osal_wait_t              isp_wait_vd_end;
    osal_wait_t              isp_wait_vd_be_end;
    osal_wait_t              isp_exit_wait;
    struct osal_semaphore    pro_info_sem;
    struct osal_semaphore    isp_sem;
    struct osal_semaphore    isp_sem_vd;
    struct osal_semaphore    isp_sem_be_vd;
    ot_isp_config_info       snap_info_save[ISP_SAVEINFO_MAX]; /* Frame end and start use index 0 and 1 respectively. */
#ifdef CONFIG_OT_SNAP_SUPPORT
    isp_snap_attr            snap_attr;
    td_u32                   start_snap_num;
    td_u32                   pro_frm_num;
#endif
    pq_ai_attr               ai_info;
    ot_dng_image_dynamic_info dng_image_dynamic_info[ISP_SAVEINFO_MAX];
    ot_frame_interrupt_attr  frame_int_attr;
    ot_isp_ir_bayer_format   isp_rgbir_format;
    ot_isp_dcf_update_info   update_info;
    ot_isp_frame_info        frame_info;
    isp_proc_frame_info      proc_frame_info;
    isp_slave_pipe_int_status slave_pipe_int_status[ISP_WDR_CHN_MAX - 1];
} isp_drv_ctx;

#ifdef CONFIG_OT_ISP_LITEOS_BOTTOM_HALF_SUPPORT
typedef struct {
    ot_vi_pipe vi_pipe;
    struct osal_work_struct worker;
    struct osal_semaphore sem;
} isp_work_queue_ctx;
#endif

typedef enum {
    ISP_CHN_SWITCH_NORMAL = 0,
    ISP_CHN_SWITCH_2LANE  = 1,
    ISP_CHN_SWITCH_3LANE  = 2,
    ISP_CHN_SWITCH_4LANE  = 3,
} isp_chn_sel;

typedef enum {
    ISP_VC_NUM_VS = 0,
    ISP_VC_NUM_S  = 1,
    ISP_VC_NUM_M  = 2,
    ISP_VC_NUM_L  = 3,
} isp_vc_num;

#define ISP_ONLINE_LDCI_TPR_BUF_NUM       2
#define ISP_STRIPING_LDCI_NORMAL_BUF_NUM  2

#define ISP_GAIN_SHIFT                    8

isp_drv_ctx *isp_drv_get_ctx(ot_vi_pipe vi_pipe);
osal_spinlock_t *isp_drv_get_lock(ot_vi_pipe vi_pipe);
osal_spinlock_t *isp_drv_get_spin_lock(ot_vi_pipe vi_pipe);
td_void isp_cal_sync_info_index(ot_vi_pipe vi_pipe, td_s32 *index);
td_s32 isp_get_actual_frame_info(ot_vi_pipe vi_pipe, ot_isp_frame_info *isp_frame);
td_u32 isp_drv_get_update_pos(ot_vi_pipe vi_pipe);
td_bool isp_drv_get_ldci_tpr_flt_en(ot_vi_pipe vi_pipe);
td_s32 isp_drv_be_end_int_proc(ot_vi_pipe vi_pipe);
int isp_int_bottom_half(int irq, void *id);
int isp_isr(int irq, void *id, td_u32 int_status);
td_s32 isp_drv_switch_be_offline_stt_info(ot_vi_pipe vi_pipe, td_u32 viproc_id);
td_s32 isp_drv_get_be_offline_stt_addr(ot_vi_pipe vi_pipe, td_u32 viproc_id, td_u64 pts,
                                       isp_be_offline_stt_buf *be_offline_stt_addr);
td_s32 isp_drv_update_hnr_be_cfg(ot_vi_pipe vi_pipe, td_void *be_node,
    td_bool hnr_en, td_bool bnr_bypass, td_bool ref_none);
td_s32 isp_drv_update_hnr_normal_blend(ot_vi_pipe vi_pipe, td_bool normal_blend);
td_s32 isp_get_frame_info(ot_vi_pipe vi_pipe, ot_isp_frame_info *isp_frame);
td_s32 isp_drv_get_dng_info(ot_vi_pipe vi_pipe, ot_isp_dng_image_static_info *dng_info);
/* Buffer control begin */
static __inline td_s32  isp_sync_buf_init(isp_sync_cfg_buf *sync_cfg_buf)
{
    (td_void)memset_s(sync_cfg_buf, sizeof(isp_sync_cfg_buf), 0, sizeof(isp_sync_cfg_buf));
    return 0;
}

static __inline td_s32  isp_sync_buf_is_full(isp_sync_cfg_buf *sync_cfg_buf)
{
    return ((sync_cfg_buf->buf_wr_flag + 1) % ISP_SYNC_BUF_NODE_NUM) == sync_cfg_buf->buf_rd_flag;
}

static __inline td_s32  isp_sync_buf_is_empty(isp_sync_cfg_buf *sync_cfg_buf)
{
    return sync_cfg_buf->buf_wr_flag == sync_cfg_buf->buf_rd_flag;
}

static __inline td_s32  isp_interrupt_buf_is_full(isp_interrupt_info *isp_int_info)
{
    return ((isp_int_info->write_flag + 1) % ISP_INTERRUPTS_SAVEINFO_MAX) == isp_int_info->read_flag;
}

static __inline td_s32  isp_interrupt_buf_is_empty(isp_interrupt_info *isp_int_info)
{
    return isp_int_info->write_flag == isp_int_info->read_flag;
}
/* only buf_rd_falg = buf_wr_flag or  buf_rd_flag + 1 = buf_wr_flag is not err */
static __inline td_s32  isp_sync_buf_is_err(isp_sync_cfg_buf *sync_cfg_buf)
{
    if (isp_sync_buf_is_empty(sync_cfg_buf)) {
        return 0;
    }
    if (((sync_cfg_buf->buf_rd_flag + 1) % ISP_SYNC_BUF_NODE_NUM) == sync_cfg_buf->buf_wr_flag) {
        return 0;
    }

    return 1;
}

static __inline td_s32  isp_sync_buf_write(isp_sync_cfg_buf *sync_cfg_buf,
    isp_sync_cfg_buf_node *sync_cfg_buf_node)
{
    if (isp_sync_buf_is_full(sync_cfg_buf)) {
        return -1;
    }

    (td_void)memcpy_s(&sync_cfg_buf->sync_cfg_buf_node[sync_cfg_buf->buf_wr_flag], sizeof(isp_sync_cfg_buf_node),
                      sync_cfg_buf_node, sizeof(isp_sync_cfg_buf_node));
    sync_cfg_buf->buf_wr_flag = (sync_cfg_buf->buf_wr_flag + 1) % ISP_SYNC_BUF_NODE_NUM;

    return 0;
}

static __inline td_s32  isp_sync_buf_read(isp_sync_cfg_buf *sync_cfg_buf, isp_sync_cfg_buf_node **sync_cfg_buf_node)
{
    if (isp_sync_buf_is_empty(sync_cfg_buf)) {
        return -1;
    }

    *sync_cfg_buf_node = &sync_cfg_buf->sync_cfg_buf_node[sync_cfg_buf->buf_rd_flag];
    sync_cfg_buf->buf_rd_flag = (sync_cfg_buf->buf_rd_flag + 1) % ISP_SYNC_BUF_NODE_NUM;

    return 0;
}

static __inline td_s32  isp_sync_buf_read2(isp_sync_cfg_buf *sync_cfg_buf,
    isp_sync_cfg_buf_node **sync_cfg_buf_node)
{
    td_u8 tmp;
    if (isp_sync_buf_is_empty(sync_cfg_buf)) {
        return -1;
    }
    /* buf_wr_flag already add one, when calc read_flag, need sub one  */
    tmp = (sync_cfg_buf->buf_wr_flag + ISP_SYNC_BUF_NODE_NUM - 1) % ISP_SYNC_BUF_NODE_NUM;

    *sync_cfg_buf_node = &sync_cfg_buf->sync_cfg_buf_node[tmp];
    sync_cfg_buf->buf_rd_flag = (tmp + 1) % ISP_SYNC_BUF_NODE_NUM;

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* ISP_H */
