/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef MKP_ISP_H
#define MKP_ISP_H

#include "ot_type.h"
#include "ot_common_3a.h"
#include "ot_common.h"
#include "ot_defines.h"
#include "isp_reg_define.h"
#include "isp_stt_define.h"
#ifdef CONFIG_OT_SNAP_SUPPORT
#include "ot_common_snap.h"
#endif
#include "ot_common_isp.h"
#include "isp_block.h"
#include "isp_define_inner.h"
#ifdef CONFIG_OT_ISP_PQ_FOR_AI_SUPPORT
#include "ot_comm_pq_ai.h"
#endif

#define IOC_TYPE_ISP                    'I'

#define ISP_BE0_PIPE_ID                 0
#define ISP_BE1_PIPE_ID                 8
#define VI_PROC_IP_NUM                  2
#define ISP_MAX_BE_NUM                  1
#define ISP_WDR_CHN_MAX                 4
#define ISP_WDR_FILTER_NUM          5
#define ISP_WDR_SEG_NUM             8
#define SYNC_EXP_RATIO_NUM          3

#define MAX_ISP_STAT_BUF_NUM            2
#define ISP_LUT2STT_SYNC_BUF_MAX_NUM    2
#define MAX_ISP_STITCH_STAT_BUF_NUM     2   /* save 2frame stitch stats */

#define ISP_VER_PRIX                    "_ISP_V"
#define ISP_VERSION                     OT_CHIP_NAME ISP_VER_PRIX MK_VERSION(VER_X, VER_Y, VER_Z, VER_P, VER_B) VER_D

#define MAX_MMZ_NAME_LEN                16
#define ISP_INIT_HAND_SIGNAL            0x66778899
#define ISP_EXIT_HAND_SIGNAL            0x99887766
#define VIPROC_FRAME_START_INT          (1 << 10)
#define ISP_PROC_SIZE                   0x3000 /* 8KB */
#define BAYERNR_LMT_LUT_NUM             129
#define OT_ISP_BAYERNR_FBRTO_LUT_SIZE   64

#define GAMMA_REG_NODE_NUM              257
#define GAMMA_OUTSEG_NUM                8

#define DEFOG_ZONE_NUM                  1024
#define DEFOG_LUT_NUM                   256
#define DEFOG_REG_NUM                   2
#define GE_MAX_CHN_NUM                  4
#define FPN_CHN_NUM                     2

#define DEHAZE_HBLK_DEFAULT             32
#define DEHAZE_VBLK_DEFAULT             32
#define HRS_FILTER_ELEMENT_NUM          6

#define ISP_DRC_BUF_SIZE                (47 * 25 * 8 * 4)

#define WDR_STAT_HOR_NUM                32
#define WDR_STAT_VER_NUM                32
#define OT_ISP_BSHP_THD_NUM             2

#define OT_ISP_BSHP_LPF_SIZE            3
#define OT_ISP_BSHP_HSF_NUM             13
#define OT_ISP_BSHP_FILTER_NUM          8
#define ccm_convert_pre(value)  (((value) & 0x8000) | ((value) << 2))
#define ccm_convert(value)      (((value) & 0x8000) ? ((~((value) & 0x7FFF)) + 1) : (value))
#define U32_UP                          0x7fffffff
#define S32_UP                          0x7fffffff
#define S32_LOW                         (-0x7fffffff)

typedef enum {
    IOC_NR_ISP_SET_FD = 0,
    IOC_NR_ISP_GET_FRAME_EDGE,
    IOC_NR_ISP_GET_VD_TIMEOUT,
    IOC_NR_ISP_GET_VD_END_TIMEOUT,
    IOC_NR_ISP_GET_VD_BEEND_TIMEOUT,
    IOC_NR_ISP_SET_INT_ENABLE,
    IOC_NR_ISP_GET_VERSION,

    IOC_NR_ISP_STAT_BUF_INIT,
    IOC_NR_ISP_STAT_BUF_EXIT,
    IOC_NR_ISP_STAT_BUF_GET,
    IOC_NR_ISP_STAT_BUF_PUT,
    IOC_NR_ISP_STAT_ACT_GET,
    IOC_NR_ISP_REG_CFG_INIT,
    IOC_NR_ISP_REG_CFG_SET,
    IOC_NR_ISP_BE_CFG_BUF_INIT,
    IOC_NR_ISP_GET_BE_BUF_FIRST,
    IOC_NR_ISP_BE_FREE_BUF_GET,
    IOC_NR_ISP_BE_LAST_BUF_GET,
    IOC_NR_ISP_BE_CFG_BUF_EXIT,
    IOC_NR_ISP_BE_CFG_BUF_CTL,
    IOC_NR_ISP_BE_CFG_BUF_RUNNING,
    IOC_NR_ISP_BE_ALL_BUF_INIT,
    IOC_NR_ISP_PROC_INIT,
    IOC_NR_ISP_PROC_WRITE_ING,
    IOC_NR_ISP_PROC_WRITE_OK,
    IOC_NR_ISP_PROC_EXIT,
    IOC_NR_ISP_PROC_PARAM_GET,
    IOC_NR_ISP_TRANS_BUF_INIT,
    IOC_NR_ISP_TRANS_BUF_EXIT,
    IOC_NR_ISP_PRO_BUF_INIT,
    IOC_NR_ISP_PRO_BUF_EXIT,
    IOC_NR_ISP_SYNC_CFG_SET,
    IOC_NR_ISP_SYNC_STITCH_PARAM_INIT,
    IOC_NR_WDR_SYNC_CFG_SET,
    IOC_NR_ISP_MEM_INFO_SET,
    IOC_NR_ISP_MEM_INFO_GET,
    IOC_NR_ISP_P2EN_INFO_GET,
    IOC_NR_ISP_INIT_INFO_SET,
    IOC_NR_ISP_SYNC_INIT_SET,
    IOC_NR_ISP_RUN_STATE_SET,
    IOC_NR_ISP_RESET_CTX,
    IOC_NR_ISP_CONFIG_INFO_SET,
    IOC_NR_ISP_SNAP_INFO_GET,
    IOC_NR_ISP_PRO_EN_GET,
    IOC_NR_ISP_UPDATE_POS_GET,
    IOC_NR_ISP_FRAME_CNT_GET,
    IOC_NR_ISP_SNAP_ATTR_GET,
    IOC_NR_ISP_MCF_EN_GET,
    IOC_NR_ISP_PWM_NUM_GET,
    IOC_NR_ISP_RES_SWITCH_SET,
    IOC_NR_ISP_CHN_SELECT_CFG,
    IOC_NR_ISP_BE_SWITCH_FINISH_STATE_SET,

    IOC_NR_ISP_FRAME_INFO_SET,
    IOC_NR_ISP_FRAME_INFO_GET,
    IOC_NR_ISP_DNG_INFO_SET,
    IOC_NR_ISP_DNG_INFO_GET,
    IOC_NR_ISP_SET_MOD_PARAM,
    IOC_NR_ISP_GET_MOD_PARAM,
    IOC_NR_ISP_SET_CTRL_PARAM,
    IOC_NR_ISP_GET_CTRL_PARAM,

    IOC_NR_ISP_MODE_INFO,
    IOC_NR_ISP_WORK_MODE_INIT,
    IOC_NR_ISP_WORK_MODE_GET,
    IOC_NR_ISP_WORK_MODE_EXIT,
    IOC_NR_ISP_PRE_BLK_NUM_UPDATE,
    IOC_NR_ISP_GET_HDR_ATTR,
    IOC_NR_ISP_GET_WDR_ATTR,
    IOC_NR_ISP_GET_STITCH_ATTR,
    IOC_NR_ISP_GET_FUSION_ATTR,
    IOC_NR_ISP_GET_PIPE_SIZE,
    IOC_NR_ISP_PUB_ATTR_INFO,
    IOC_NR_ISP_CLUT_BUF_INIT,
    IOC_NR_ISP_CLUT_BUF_EXIT,
    IOC_NR_ISP_CLUT_BUF_GET,
    IOC_NR_ISP_STT_BUF_INIT,
    IOC_NR_ISP_STT_BUF_EXIT,
    IOC_NR_ISP_STT_ADDR_INIT,
    IOC_NR_ISP_BE_LUT_STT_BUF_GET,
    IOC_NR_ISP_BE_PRE_LUT_STT_BUF_GET,
    IOC_NR_ISP_KERNEL_RUNONCE,
    IOC_NR_ISP_YUV_RUNONCE_INFO,
    IOC_NR_ISP_KERNEL_YUV_RUNONCE,
    IOC_NR_ISP_OPT_RUNONCE_INFO,
    IOC_NR_ISP_SET_PROCALCDONE,
    IOC_NR_ISP_SPECAWB_BUF_INIT,
    IOC_NR_ISP_SPECAWB_BUF_EXIT,
    IOC_NR_ISP_SPECAWB_BUF_GET,
    IOC_NR_ISP_SET_RGBIR_FORMAT,
    IOC_NR_ISP_UPDATE_INFO_SET,
    IOC_NR_ISP_BE_SYNC_PARAM_INIT,
    IOC_NR_ISP_LDCI_BUF_INIT,
    IOC_NR_ISP_LDCI_BUF_EXIT,
    IOC_NR_ISP_LDCI_READ_STT_BUF_GET,
    IOC_NR_ISP_BE_BUF_NUM_GET,
    IOC_NR_ISP_PQ_AI_GROUP_ATTR_SET,
    IOC_NR_ISP_PQ_AI_GROUP_ATTR_GET,
    IOC_NR_ISP_PQ_AI_POST_NR_ATTR_SET,
    IOC_NR_ISP_PQ_AI_POST_NR_ATTR_GET,
    IOC_NR_ISP_DRC_BUF_INIT,
    IOC_NR_ISP_DRC_BUF_EXIT,
    IOC_NR_ISP_FSWDR_BUF_INIT,
    IOC_NR_ISP_FSWDR_BUF_EXIT,
    IOC_NR_ISP_BNR_TEMPORAL_FILT_CFG_SET,
    IOC_NR_ISP_MOT_CFG_SET,
    IOC_NR_ISP_ALG_INIT_ERR_INFO_PRINT,
    IOC_NR_ISP_DYNAMIC_ACTUAL_INFO_GET,
    IOC_NR_ISP_FPN_WORK_MODE_SET,
    IOC_BAS_CROP_ATTR,
    IOC_NR_ISP_SNAP_ATTR_SET,
    IOC_NR_ISP_STAGGER_ATTR_GET,
    IOC_NR_ISP_HNR_EN_SET,
    IOC_NR_ISP_BUTT,
} ioc_nr_isp;

typedef struct {
    unsigned int cmd;
    td_uintptr_t arg;
    void *private_data;
} isp_ioctl_para;

typedef struct {
    unsigned int cmd;
    td_char *cmd_name;
} isp_ioctl_cmd;

typedef struct {
    td_u32  milli_sec;     /* RW, Range: [0x0, 0xFFFFFFFF], the number of the */
    td_u32  int_status;    /* RO, when there is ISP interrupt, then the int_status is 1 */
} isp_vd_timeout;

#define ISP_VERSION_NUM_MAX 4
typedef struct {
    td_u8 mpp_version[ISP_VERSION_NUM_MAX];
    td_u32 magic;
} isp_version;

/* the statistics of isp */
typedef struct {
    td_u16  defect_pixel_count[OT_ISP_STRIPING_MAX_NUM];
    td_u32  defect_pixel_lut[OT_ISP_STRIPING_MAX_NUM][OT_ISP_STATIC_DP_COUNT_NORMAL];
} isp_dp_stat;

typedef struct {
    td_u32  auto_gain;
    td_u32  ir_sum_info;
} isp_rgbir_stat;

#define ISP_FLICKER_G_MEAN_LENGTH       512
typedef struct {
    td_u32 cur_diff_gr[OT_ISP_STRIPING_MAX_NUM];                            /* s24.0, current frame gr sum */
    td_u32 cur_diff_gb[OT_ISP_STRIPING_MAX_NUM];                            /* s24.0, current frame gb sum */
    td_u32 cur_abs_gr[OT_ISP_STRIPING_MAX_NUM];                             /* u23.0 */
    td_u32 cur_abs_gb[OT_ISP_STRIPING_MAX_NUM];                             /* u23.0 */
    td_u32 g_mean[OT_ISP_STRIPING_MAX_NUM][ISP_FLICKER_G_MEAN_LENGTH];    /* u22.0 */
} isp_flicker_stat;

typedef struct {
    td_u32  max_stat_dout[OT_ISP_STRIPING_MAX_NUM][DEFOG_ZONE_NUM];
    td_u32  min_dout[OT_ISP_STRIPING_MAX_NUM][DEFOG_ZONE_NUM / DEFOG_REG_NUM];
} isp_dehaze_stat;

typedef struct {
    td_u16  zone_avg[OT_ISP_MG_ZONE_ROW][OT_ISP_MG_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM];
} isp_mg_stat;

typedef struct {
    td_u32  global_map[LDCI_STAT_WND_Y][LDCI_STAT_WND_X];
} isp_ldci_stat;

typedef struct {
    td_u16  white_balance_gain[OT_ISP_BAYER_CHN_NUM];
} isp_comm_stat;

typedef struct {
    ot_isp_be_ae_stat_1  be_ae_stat1;
    ot_isp_be_ae_stat_2  be_ae_stat2;
    ot_isp_be_ae_stat_3  be_ae_stat3;
    ot_isp_awb_stat_1    awb_stat1;
    ot_isp_awb_stat_2    awb_stat2;
    ot_isp_be_af_stat    af_stat;
    isp_dp_stat          dp_stat;
    isp_comm_stat        comm_stat;
    isp_dehaze_stat      dehaze_stat;
    isp_mg_stat          mg_stat;
} isp_be_stat;

typedef struct {
    ot_isp_fe_ae_stat_1  fe_ae_stat1;
    ot_isp_fe_ae_stat_2  fe_ae_stat2;
    ot_isp_fe_ae_stitch_stat_3  fe_ae_stat3;
    ot_isp_be_ae_stat_1  be_ae_stat1;
    ot_isp_be_ae_stat_2  be_ae_stat2;
    ot_isp_be_ae_stitch_stat_3 be_ae_stat3;
    ot_isp_awb_stat_stitch awb_stat2;
} isp_stitch_stat;

typedef struct {
    td_bool fusion_mode;
    td_u16  short_thr[OT_ISP_WDR_MAX_FRAME_NUM - 1];
    td_u16  long_thr[OT_ISP_WDR_MAX_FRAME_NUM - 1];
    td_u16  blacklevel[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  exp_ratio[OT_ISP_WDR_MAX_FRAME_NUM - 1];
} isp_be_stats_calc_info;

typedef struct {
    ot_isp_fe_ae_stat_1  fe_ae_stat1;
    ot_isp_fe_ae_stat_2  fe_ae_stat2;
    ot_isp_fe_ae_stat_3  fe_ae_stat3;
    ot_isp_be_ae_stat_1  be_ae_stat1;
    ot_isp_be_ae_stat_2  be_ae_stat2;
    ot_isp_be_ae_stat_3  be_ae_stat3;
    ot_isp_awb_stat_1    awb_stat1;
    ot_isp_awb_stat_2    awb_stat2;
    ot_isp_awb_stat_1    awblsc_stat1;
    ot_isp_awb_stat_2    awblsc_stat2;
    ot_isp_fe_af_stat    fe_af_stat;
    ot_isp_be_af_stat    be_af_stat;
    isp_dp_stat          dp_stat;
    isp_comm_stat        comm_stat;
    isp_dehaze_stat      dehaze_stat;
    isp_mg_stat          mg_stat;
    isp_flicker_stat     flicker_stat;
    isp_stitch_stat      stitch_stat;
    isp_ldci_stat        ldci_stat;
    isp_rgbir_stat       rgbir_stat;
    isp_be_stats_calc_info be_stats_calc_info;
    td_bool              be_update;
    td_u64               frame_pts;
} isp_stat;

#define DP_STAT_KEY_BIT                 14
#define FLICKER_STAT_KEY_BIT            18

typedef union {
    td_u64  key;
    struct {
        td_u64  bit1_fe_ae_glo_stat      : 1;   /* [0] */
        td_u64  bit1_fe_ae_loc_stat      : 1;   /* [1] */
        td_u64  bit1_fe_ae_sti_glo_stat  : 1;   /* [2] */
        td_u64  bit1_fe_ae_sti_loc_stat  : 1;   /* [3] */
        td_u64  bit1_be_ae_glo_stat      : 1;   /* [4] */
        td_u64  bit1_be_ae_loc_stat      : 1;   /* [5] */
        td_u64  bit1_be_ae_sti_glo_stat  : 1;   /* [6] */
        td_u64  bit1_be_ae_sti_loc_stat  : 1;   /* [7] */
        td_u64  bit1_awb_stat1           : 1;   /* [8] */
        td_u64  bit1_awb_stat2           : 1;   /* [9] */
        td_u64  bit1_awb_stat3           : 1;   /* [10] */
        td_u64  bit1_awb_stat4           : 1;   /* [11] */
        td_u64  bit1_fe_af_stat          : 1;   /* [12] */
        td_u64  bit1_be_af_stat          : 1;   /* [13] */
        td_u64  bit1_dp_stat             : 1;   /* [14] */
        td_u64  bit1_comm_stat           : 1;   /* [15] */
        td_u64  bit1_dehaze              : 1;   /* [16] */
        td_u64  bit1_mg_stat             : 1;   /* [17] */
        td_u64  bit1_flicker             : 1;   /* [18] */
        td_u64  bit1_awblsc_stat1        : 1;   /* [19] */
        td_u64  bit1_awblsc_stat2        : 1;   /* [20] */
        td_u64  bit1_rgbir               : 1;   /* [21] */
        td_u64  bit1_agamma_cfg          : 1;   /* [22] */
        td_u64  bit1_adgamma_cfg         : 1;   /* [23] */
        td_u64  bit8_rsv                 : 8;   /* [24:31] */
        td_u64  bit32_isr_access         : 32;   /* [32:63] */
    };
} isp_stat_key;

typedef struct {
    isp_stat_key  stat_key;
    td_phys_addr_t   phy_addr;
    td_void *virt_addr;                 /* (isp_stat *) */
} isp_stat_info;

typedef struct {
    td_phys_addr_t  phy_addr;
    td_void *virt_addr;
    td_u32 size;
    td_bool usr_access;
} isp_stat_shadow_mem;

/* the register config of isp */
typedef struct {
    td_u8   metering_hist_thresh[OT_ISP_AE_PLANE_NUM]; /* 4 plane mode */
    td_u8   weight_table[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN];
} isp_ae_reg_cfg_1;

typedef struct {
    td_u64  exposure;
    td_u64  exposure_sf;
    td_u32  int_time[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  isp_dgain;
    td_u32  wdr_gain[OT_ISP_WDR_MAX_FRAME_NUM];

    td_bool piris_valid;
    td_s32  piris_pos;

    ot_isp_fswdr_mode fs_wdr_mode;
} isp_ae_reg_cfg_2;

typedef struct {
    td_u8 fe_enable;
    td_u8 be_enable;

    /* Overlap Configs */
    td_u16 fe_crop_pos_x;
    td_u16 fe_crop_pos_y;
    td_u16 fe_crop_out_width;
    td_u16 fe_crop_out_height;

    td_u16 be_crop_pos_x;
    td_u16 be_crop_pos_y;
    td_u16 be_crop_out_width;
    td_u16 be_crop_out_height;
} isp_ae_static_cfg;

typedef struct {
    /* ISP MPI Configs */
    td_u8 fe_four_plane_mode;
    td_u8 fe_hist_offset_x;
    td_u8 fe_hist_offset_y;
    td_u8 fe_hist_skip_x;
    td_u8 fe_hist_skip_y;

    td_u8 be_ae_sel;
    td_u8 be_four_plane_mode;
    td_u8 be_hist_offset_x;
    td_u8 be_hist_offset_y;
    td_u8 be_hist_skip_x;
    td_u8 be_hist_skip_y;

    /* WDR Configs */
    td_u8 fe_gamma_limit;
    td_u8 fe_hist_gamma_mode;
    td_u8 fe_aver_gamma_mode;
    td_u8 fe_bit_move;

    td_u8 be_gamma_limit;
    td_u8 be_hist_gamma_mode;
    td_u8 be_aver_gamma_mode;
    td_u8 be_bit_move;

    /* Weight Table Configs */
    td_u8 be_weight_table_update;
    td_u8 be_weight_table_width;
    td_u8 be_weight_table_height;
    td_u8 be_weight_table[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN];
    td_u32 combine_wgt[64]; /* ae weight table number 64 */

    td_u8 fe_weight_table_update;
    td_u8 fe_weight_table_width;
    td_u8 fe_weight_table_height;
    td_u8 fe_weight_table[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN];
} isp_ae_dyna_cfg;

typedef struct {
    isp_ae_static_cfg static_reg_cfg;
    isp_ae_dyna_cfg   dyna_reg_cfg;
} isp_ae_reg_cfg;

typedef struct {
    td_bool resh;
    td_u8   enable;
    td_u8   rs_enable;
    td_s16  hrs_filter_lut0[HRS_FILTER_ELEMENT_NUM];
    td_s16  hrs_filter_lut1[HRS_FILTER_ELEMENT_NUM];
    td_u16  width;
    td_u16  height;
} isp_hrs_static_cfg;

typedef struct {
    td_u32 reserved;
} isp_hrs_dyna_cfg;

typedef struct {
    isp_hrs_static_cfg static_reg_cfg;
    isp_hrs_dyna_cfg dyna_reg_cfg;
} isp_hrs_reg_cfg;

typedef struct {
    td_u8 enable;

    /* Overlap Configs */
    td_u16 crop_pos_x;
    td_u16 crop_pos_y;
    td_u16 crop_out_width;
    td_u16 crop_out_height;
} isp_mg_static_cfg;

typedef struct {
    /* WDR Configs */
    td_u8 gamma_limit;
    td_u8 gamma_mode;
    td_u8 bit_move;

    /* Zone Configs */
    td_u8 zone_width;
    td_u8 zone_height;
} isp_mg_dyna_cfg;

typedef struct {
    isp_mg_static_cfg static_reg_cfg;
    isp_mg_dyna_cfg dyna_reg_cfg;
} isp_mg_reg_cfg;

typedef struct {
    td_bool resh;
} isp_dg_static_cfg;

typedef struct {
    td_bool resh;
    td_u16  gain_r;
    td_u16  gain_gr;
    td_u16  gain_gb;
    td_u16  gain_b;
    td_u32  clip_value;
} isp_dg_dyna_cfg;

typedef struct {
    td_bool resh;
    td_u16  gain_r[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  gain_gr[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  gain_gb[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u16  gain_b[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32  clip_value;
} isp_fe_dg_dyna_cfg;

typedef struct {
    td_bool dg_en;
    isp_dg_static_cfg static_reg_cfg;
    isp_dg_dyna_cfg   dyna_reg_cfg;
} isp_dg_reg_cfg;

typedef struct {
    td_bool dg_en;
    isp_dg_static_cfg static_reg_cfg;
    isp_fe_dg_dyna_cfg   dyna_reg_cfg;
} isp_fe_dg_reg_cfg;

typedef struct {
    td_bool resh;
    td_u16 gain_r0;
    td_u16 gain_gr0;
    td_u16 gain_gb0;
    td_u16 gain_b0;
    td_u16 gain_r1;
    td_u16 gain_gr1;
    td_u16 gain_gb1;
    td_u16 gain_b1;
    td_u16 gain_r2;
    td_u16 gain_gr2;
    td_u16 gain_gb2;
    td_u16 gain_b2;
    td_u16 gain_r3;
    td_u16 gain_gr3;
    td_u16 gain_gb3;
    td_u16 gain_b3;
} isp_4dg_static_cfg;

typedef struct {
    td_bool resh;
    td_u32 clip_value0;
    td_u32 clip_value1;
    td_u32 clip_value2;
    td_u32 clip_value3;
} isp_4dg_dyna_cfg;

typedef struct {
    td_bool enable;
    isp_4dg_static_cfg static_reg_cfg;
    isp_4dg_dyna_cfg   dyna_reg_cfg;
} isp_4dg_reg_cfg;

typedef struct {
    /* BE DYNAMIC */
    td_u32  be_white_balance_gain[OT_ISP_BAYER_CHN_NUM];
    td_u16  be_color_matrix[OT_ISP_CCM_MATRIX_SIZE];
    td_u16  be_metering_white_level_awb;
    td_u16  be_metering_black_level_awb;
    td_u16  be_metering_cr_ref_max_awb;
    td_u16  be_metering_cb_ref_max_awb;
    td_u16  be_metering_cr_ref_min_awb;
    td_u16  be_metering_cb_ref_min_awb;
    td_u8   be_wb_work_en;
    td_u8   be_cc_en;
    td_bool be_cc_colortone_en;
    td_u16  be_cc_r_gain;
    td_u16  be_cc_g_gain;
    td_u16  be_cc_b_gain;

    td_u32 be_crop_pos_x;
    td_u32 be_crop_pos_y;
    td_u32 be_width;
    td_u32 be_height;
    td_u32 be_crop_out_height;
    td_u32 be_crop_out_width;

    /* FE DYNAMIC */
    td_u32 fe_white_balance_gain[OT_ISP_BAYER_CHN_NUM];
    td_u8 fe_wb_work_en;
    td_u32 wdr_wb_gain[OT_ISP_BAYER_CHN_NUM];
} isp_awb_reg_dyn_cfg;

typedef struct {
    /* BESTATIC */
    td_bool be_awb_sta_cfg;
    td_u8   be_awb_bitmove;
    td_u8   be_awb_work_en;
    td_u8   be_wb_work_en;

    td_u32 be_awb_stat_raddr;
    td_u8  be_cc_colortone_en;
    td_u8  be_cc_en;

    td_u32 be_top_k;
    td_u32 be_top_b;
    td_u32 be_bot_k;
    td_u32 be_bot_b;

    td_u32 be_cc_in_dc0;
    td_u32 be_cc_in_dc1;
    td_u32 be_cc_in_dc2;
    td_u32 be_cc_out_dc0;
    td_u32 be_cc_out_dc1;
    td_u32 be_cc_out_dc2;
    td_u32 be_wb_clip_value;

    td_u16 be_awb_offset_comp;

    /* FESTATIC */
    td_bool fe_awb_sta_cfg;
    td_u8   fe_wb_work_en;

    td_u32  fe_clip_value;
} isp_awb_reg_sta_cfg;

typedef struct {
    /* BE USER */
    td_bool resh;
    td_u32 update_index;
    ot_isp_awb_switch be_awb_switch;
    td_u16 be_zone_row;
    td_u16 be_zone_col;
    td_u16 be_zone_bin;
    td_u16 be_metering_bin_hist0;
    td_u16 be_metering_bin_hist1;
    td_u16 be_metering_bin_hist2;
    td_u16 be_metering_bin_hist3;
} isp_awb_reg_usr_cfg;

typedef struct {
    isp_awb_reg_dyn_cfg        awb_reg_dyn_cfg;
    isp_awb_reg_sta_cfg        awb_reg_sta_cfg;
    isp_awb_reg_usr_cfg        awb_reg_usr_cfg;
} isp_awb_reg_cfg;

typedef struct {
    td_u32  be_white_balance_gain[OT_ISP_BAYER_CHN_NUM];
    td_u16  color_matrix[OT_ISP_CCM_MATRIX_SIZE];
    ot_isp_awb_switch be_awb_switch;
} isp_awb_sync_cfg;

typedef isp_awb_sync_cfg isp_awb_reg_cfg_2;

typedef struct {
    td_u16  metering_white_level_awb;
    td_u16  metering_black_level_awb;
    td_u16  metering_cr_ref_max_awb;
    td_u16  metering_cb_ref_max_awb;
    td_u16  metering_cr_ref_min_awb;
    td_u16  metering_cb_ref_min_awb;

    td_u32 crop_pos_x;
    td_u32 crop_pos_y;
    td_u32 width;
    td_u32 height;
    td_u32 crop_out_height;
    td_u32 crop_out_width;
} isp_awblsc_reg_dyn_cfg;

typedef struct {
    td_bool awb_sta_cfg;
    td_u8   awb_bitmove;
    td_u8   awb_work_en;

    td_u32 awb_stat_raddr;
    td_u16 awb_offset_comp;

    td_u16 zone_row;
    td_u16 zone_col;
} isp_awblsc_reg_sta_cfg;

typedef struct {
    isp_awblsc_reg_dyn_cfg   awblsc_reg_dyn_cfg;
    isp_awblsc_reg_sta_cfg   awblsc_reg_sta_cfg;
} isp_awblsc_reg_cfg;

typedef struct {
    td_u32  wdr_gain[OT_ISP_WDR_MAX_FRAME_NUM];
} isp_sync_4dgain_cfg;

typedef struct {
    td_u16 threshold_write;
    td_u8 metrics_shift;
    td_u8 np_offset;
    td_bool af_enable;
    td_bool iir0_enable0;
    td_bool iir0_enable1;
    td_bool iir0_enable2;
    td_bool iir1_enable0;
    td_bool iir1_enable1;
    td_bool iir1_enable2;
    ot_isp_af_peak_mode peak_mode;
    ot_isp_af_square_mode squ_mode;
    td_bool crop_enable;
    td_bool fe_crop_enable;
    td_bool lpf_enable;
    td_bool mean_enable;
    td_bool raw_mode;
    td_bool iir0_ds_enable;
    td_bool iir1_ds_enable;
    td_bool fir0_lpf_enable;
    td_bool fir1_lpf_enable;
    td_bool iir0_ldg_enable;
    td_bool iir1_ldg_enable;
    td_bool fir0_ldg_enable;
    td_bool fir1_ldg_enable;
    td_bool update;

    td_u8 bayer_mode;
    td_u8 gamma;
    td_u8 gain_limit;
    td_u8 iir_plg_group0;
    td_u8 iir_pls_group0;
    td_u8 iir_plg_group1;
    td_u8 iir_pls_group1;
    td_u8 cnt_shift0_h;
    td_u8 cnt_shift1_h;
    td_u8 cnt_shift0_v;
    td_u8 cnt_shift1_v;
    td_u8 cnt_shift_y;
    td_u8 iir_dilate0;
    td_u8 iir_dilate1;
    td_u8 highlight_thre;
    td_u8 af_pos_sel;

    td_u16 mean_thres;
    td_u16 window_vnum;
    td_u16 window_hnum;
    td_u16 iir_thre0_coring;
    td_u16 iir_thre1_coring;
    td_u16 iir_peak0_coring;
    td_u16 iir_peak1_coring;
    td_u16 fir_thre0_coring;
    td_u16 fir_thre1_coring;
    td_u16 fir_peak0_coring;
    td_u16 fir_peak1_coring;
    td_u16 iir_slope0_coring;
    td_u16 iir_slope1_coring;
    td_u16 fir_slope0_coring;
    td_u16 fir_slope1_coring;
    td_u16 iir0_shift_group0;
    td_u16 iir1_shift_group0;
    td_u16 iir2_shift_group0;
    td_u16 iir3_shift_group0;
    td_u16 iir0_shift_group1;
    td_u16 iir1_shift_group1;
    td_u16 iir2_shift_group1;
    td_u16 iir3_shift_group1;
    td_u16 iir_thre0_low;
    td_u16 iir_thre0_high;
    td_u16 iir_thre1_low;
    td_u16 iir_thre1_high;
    td_u16 iir_gain0_low;
    td_u16 iir_gain0_high;
    td_u16 iir_gain1_low;
    td_u16 iir_gain1_high;
    td_u16 iir_slope0_low;
    td_u16 iir_slope0_high;
    td_u16 iir_slope1_low;
    td_u16 iir_slope1_high;
    td_u16 fir_thre0_low;
    td_u16 fir_thre0_high;
    td_u16 fir_thre1_low;
    td_u16 fir_thre1_high;
    td_u16 fir_gain0_low;
    td_u16 fir_gain0_high;
    td_u16 fir_gain1_low;
    td_u16 fir_gain1_high;
    td_u16 fir_slope0_low;
    td_u16 fir_slope0_high;
    td_u16 fir_slope1_low;
    td_u16 fir_slope1_high;
    td_u16 acc_shift0_h;
    td_u16 acc_shift1_h;
    td_u16 acc_shift0_v;
    td_u16 acc_shift1_v;
    td_u16 acc_shift_y;
    td_u16 crop_pos_y;
    td_u16 crop_pos_x;
    td_u16 shift_count_y;
    td_u16 crop_v_size;
    td_u16 crop_h_size;
    td_u16 fe_crop_pos_y;
    td_u16 fe_crop_pos_x;
    td_u16 fe_crop_v_size;
    td_u16 fe_crop_h_size;

    td_s16 iir_gain0_group0;
    td_s16 iir_gain0_group1;
    td_s16 iir_gain1_group0;
    td_s16 iir_gain1_group1;
    td_s16 iir_gain2_group0;
    td_s16 iir_gain2_group1;
    td_s16 iir_gain3_group0;
    td_s16 iir_gain3_group1;
    td_s16 iir_gain4_group0;
    td_s16 iir_gain4_group1;
    td_s16 iir_gain5_group0;
    td_s16 iir_gain5_group1;
    td_s16 iir_gain6_group0;
    td_s16 iir_gain6_group1;
    td_s16 fir_h_gain0_group0;
    td_s16 fir_h_gain0_group1;
    td_s16 fir_h_gain1_group0;
    td_s16 fir_h_gain1_group1;
    td_s16 fir_h_gain2_group0;
    td_s16 fir_h_gain2_group1;
    td_s16 fir_h_gain3_group0;
    td_s16 fir_h_gain3_group1;
    td_s16 fir_h_gain4_group0;
    td_s16 fir_h_gain4_group1;

    td_u32 stat_read_addr;
    td_u32 stat_read_data;
    td_u32 update_index;
} isp_af_reg_cfg;

typedef struct {
    td_u16  black_level_offset[OT_ISP_BAYER_CHN_NUM];
} isp_offset_reg_cfg;

#define DRC_COMM_LUT_NODE_NUM 16
#define DRC_HW_FLTX_NODE_NUM 4

typedef struct {
    td_bool resh;

    td_bool rdstat_en;
    td_bool wrstat_en;

    td_bool monochrome_mode;
    td_bool wgt_box_tri_sel;

    td_u8   bit_out;
    td_u8   bin_num_z;

    td_u8   r_wgt;
    td_u8   g_wgt;
    td_u8   b_wgt;

    td_u8   r_wgt_tm;
    td_u8   g_wgt_tm;
    td_u8   b_wgt_tm;

    td_bool gain_clip_mode;

    td_bool cc_ctrl_a;
    td_bool cc_ctrl_b;
    td_bool tm_ctrl;

    td_bool tm_idx_sel;
    td_u8   seg_idx_base[DRC_TM_SEG_NUM];
    td_u8   seg_max_val[DRC_TM_SEG_NUM];

    td_bool kernel_sel_r0;
    td_bool kernel_sel_e0;
    td_bool kernel_sel_ds;

    td_u32  raw_eps;
    td_u8   rg_slo;
    td_u8   bg_slo;
    td_u8   rg_thr_lut[OT_ISP_DRC_RG_BG_CTR_NODE_NUM];
    td_u8   bg_thr_lut[OT_ISP_DRC_RG_BG_CTR_NODE_NUM];
    td_u8   rg_ctr_lut[OT_ISP_DRC_RG_BG_CTR_NODE_NUM];
    td_u8   bg_ctr_lut[OT_ISP_DRC_RG_BG_CTR_NODE_NUM];

    td_bool wgt_m_en;
    td_u8   wgt_m_sft;
    td_u8   wgt_m_pd_thr;
    td_u8   wgt_m_pd_sft;
    td_u8   wgt_m_pd_slo;

    td_u8   pd_bg_slo;
    td_u8   pd_bg_thr;
    td_u8   pd_rg_slo;
    td_u8   pd_rg_wid;
    td_u8   pd_rg_crt;
    td_u8   pdw_high_slo;
    td_u8   pdw_low_thr;
    td_u8   pdw_low_slo;

    td_bool mask_ada_en;
    td_u8   mask_ada_sft;

    td_u8   update_mode;
    td_u16  timing_stat;
    td_u8   drc_update;

    td_bool first_frame;
    td_u8   shp_log;
    td_u8   shp_exp;
    td_u32  div_denom_log;
    td_u32  denom_exp;
    td_u32  prev_luma[OT_ISP_DRC_EXP_COMP_SAMPLE_NUM];
} isp_drc_static_cfg;

typedef struct {
    td_bool resh;
    td_bool img_size_changed;
    td_bool vbiflt_en;
    td_bool cc_en;
    td_bool lut_update;

    td_u16  strength_bias;
    td_u16  strength;
    td_u16  lut_mix_ctrl;
    td_u16  auto_strength_max;
    td_u16  auto_strength_min;

    td_u16  tone_mapping_value[OT_ISP_DRC_TM_NODE_NUM];
    td_u16  tone_mapping_diff[OT_ISP_DRC_TM_NODE_NUM];

    td_u8   bin_scale;
    td_u8   m_num_max;
    td_u8   bit_out;

    td_u8   block_v_num;
    td_u8   block_h_num;
    td_u8   block_chk_y;
    td_u8   block_chk_x;
    td_u16  block_v_size;
    td_u16  block_h_size;

    td_u16  div_y0;
    td_u16  div_x0;
    td_u16  div_y1;
    td_u16  div_x1;

    td_u8   pdw_high_thr;

    td_u8   big_x_init;
    td_u8   idx_x_init;
    td_u16  cnt_x_init;
    td_u16  acc_x_init;
    td_u16  wgt_x_init;
    td_u16  total_width;
    td_u16  stat_width;
} isp_drc_dyna_cfg;

typedef struct {
    td_bool resh;

    td_u8   ysft1;
    td_u8   yval1;
    td_u8   ysft2;
    td_u8   yval2;

    td_u8   csft1;
    td_u8   cval1;
    td_u8   csft2;
    td_u8   cval2;

    td_u8   sft;
    td_u8   val;

    td_u8   gain_clip_knee;
    td_u8   gain_clip_step;

    td_u8   grad_rev_shift;
    td_u8   grad_rev_slope;
    td_u8   grad_rev_max;
    td_u8   grad_rev_thr;

    td_u8   var_spa_fine;

    td_u8   var_rng_fine;

    td_u8   flt_spa_fine;
    td_u8   flt_rng_fine;

    td_u16  cc_lut[OT_ISP_DRC_CC_NODE_NUM];

    td_u8   mixing_bright0[OT_ISP_DRC_LMIX_NODE_NUM];
    td_u8   mixing_dark0[OT_ISP_DRC_LMIX_NODE_NUM];
    td_u8   mixing_bright1[OT_ISP_DRC_LMIX_NODE_NUM];
    td_u8   mixing_dark1[OT_ISP_DRC_LMIX_NODE_NUM];

    td_u8   write_out_bit[DRC_HW_FLTX_NODE_NUM];
    td_u8   log_alpha_dec[DRC_HW_FLTX_NODE_NUM];

    td_u8   luma_clip_low[DRC_HW_FLTX_NODE_NUM];
    td_u8   luma_clip_hig[DRC_HW_FLTX_NODE_NUM];

    td_bool m_en[DRC_HW_FLTX_NODE_NUM];

    td_bool m_lut0_update;
    td_u16  m_lut0_value[OT_ISP_DRC_MLUT_NODE_NUM];
    td_u16  m_lut0_diff[OT_ISP_DRC_MLUT_NODE_NUM];

    td_bool m_lut1_update;
    td_u16  m_lut1_value[OT_ISP_DRC_MLUT_NODE_NUM];
    td_u16  m_lut1_diff[OT_ISP_DRC_MLUT_NODE_NUM];

    td_bool m_lut2_update;
    td_u16  m_lut2_value[OT_ISP_DRC_MLUT_NODE_NUM];
    td_u16  m_lut2_diff[OT_ISP_DRC_MLUT_NODE_NUM];

    td_bool m_lut3_update;
    td_u16  m_lut3_value[OT_ISP_DRC_MLUT_NODE_NUM];
    td_u16  m_lut3_diff[OT_ISP_DRC_MLUT_NODE_NUM];

    td_s8   detail_adjust_coef0;
    td_u8   detail_adjust_coef1;

    td_u8   detail_sup_bright_max0;
    td_u8   detail_sup_bright_min0;
    td_u8   detail_sup_bright_thr0;
    td_u8   detail_sup_bright_slo0;
    td_u8   detail_sup_dark_max0;
    td_u8   detail_sup_dark_min0;
    td_u8   detail_sup_dark_thr0;
    td_u8   detail_sup_dark_slo0;

    td_u8   detail_sup1;
    td_u8   detail_boost1;

    td_u8   detail_sup_bright_max2;
    td_u8   detail_sup_bright_min2;
    td_u8   detail_sup_bright_thr2;
    td_u8   detail_sup_bright_slo2;
    td_u8   detail_sup_dark_max2;
    td_u8   detail_sup_dark_min2;
    td_u8   detail_sup_dark_thr2;
    td_u8   detail_sup_dark_slo2;

    td_u8   bld_l_shift;
    td_u8   bld_l_bright_max;
    td_u8   bld_l_bright_min;
    td_u8   bld_l_bright_thr;
    td_u8   bld_l_bright_slo;
    td_u8   bld_l_dark_max;
    td_u8   bld_l_dark_min;
    td_u8   bld_l_dark_thr;
    td_u8   bld_l_dark_slo;

    td_u8   bld_d_shift;
    td_u8   bld_d_bright_max;
    td_u8   bld_d_bright_min;
    td_u8   bld_d_bright_thr;
    td_u8   bld_d_bright_slo;
    td_u8   bld_d_dark_max;
    td_u8   bld_d_dark_min;
    td_u8   bld_d_dark_thr;
    td_u8   bld_d_dark_slo;

    td_u8   shp_log_raw;
    td_u8   shp_exp_raw;
    td_u32  div_denom_log_raw;
    td_u32  denom_exp_raw;

    td_bool tm_ctrl;
    td_bool cc_ctrl;
    td_u16  raw_blc;

    td_u8   wgt_m_lut[DRC_COMM_LUT_NODE_NUM];
    td_u8   mask_ada_coef[DRC_COMM_LUT_NODE_NUM];

    td_u8   pw_curve_slo_sft;
    td_u16  pw_curve_bias;
    td_u16  luma_avg_manual;
    td_u16  pw_curve_seg[DRC_COMM_LUT_NODE_NUM];
    td_s16  pw_curve_slo[DRC_COMM_LUT_NODE_NUM];

    td_bool tm_gain_skip;
    td_bool npu_wrt_sel;
    td_bool npu_wrt_en;
    td_bool mask_ada_en;

    td_u8   npu_wrt_ds;
    td_u8   mask_ada_sft;

    td_u8   stat_coef_sft0;
    td_u8   stat_coef_sft1;
    td_u8   stat_flt_type;
    td_u8   stat_format;

    td_u32  update_index;
} isp_drc_usr_cfg;

typedef struct {
    td_u8   shp_log;
    td_u8   shp_exp;
    td_bool is_offline_repeat_mode;
    td_s32  prev_luma_delta[OT_ISP_DRC_EXP_COMP_SAMPLE_NUM - 1];
} isp_drc_sync_cfg;

typedef struct {
    td_bool            enable;
    isp_drc_static_cfg static_reg_cfg;
    isp_drc_dyna_cfg   dyna_reg_cfg;
    isp_drc_usr_cfg    usr_reg_cfg;
    isp_drc_sync_cfg   sync_reg_cfg;
} isp_drc_reg_cfg;

typedef struct {
    td_bool resh;
    td_u8   seg_idx_base[PREGAMMA_SEG_NUM];
    td_u16  seg_max_val[PREGAMMA_SEG_NUM];
} isp_pregamma_static_cfg;

typedef struct {
    td_bool resh;
    td_bool lut_update;
    td_u32  pregamma_lut[OT_ISP_PREGAMMA_NODE_NUM];
    td_u32  update_index;
} isp_pregamma_dyna_cfg;

typedef struct {
    td_bool enable;
    isp_pregamma_dyna_cfg   dyna_reg_cfg;
    isp_pregamma_static_cfg static_reg_cfg;
} isp_pregamma_reg_cfg;
typedef struct {
    td_bool resh;
    td_u8   seg_idx_base[AGAMMA_SEG_NUM];
    td_u16  seg_max_val[AGAMMA_SEG_NUM];
} isp_agamma_static_cfg;

typedef struct {
    td_u8   buf_id;
    td_u8   resh;
    td_u32  agamma_lut[OT_ISP_AGAMMA_NODE_NUM];
} isp_agamma_dyna_cfg;

typedef struct {
    td_bool enable;
    isp_agamma_dyna_cfg   dyna_reg_cfg;
    isp_agamma_static_cfg static_reg_cfg;
} isp_agamma_reg_cfg;

typedef struct {
    td_bool resh;
    td_u8   seg_idx_base[AGAMMA_SEG_NUM];
    td_u16  seg_max_val[AGAMMA_SEG_NUM];
} isp_adgamma_static_cfg;

typedef struct {
    td_u8   buf_id;
    td_u8   resh;
    td_u32  adgamma_lut[OT_ISP_AGAMMA_NODE_NUM];
} isp_adgamma_dyna_cfg;

typedef struct {
    td_bool enable;
    isp_adgamma_dyna_cfg   dyna_reg_cfg;
    isp_adgamma_static_cfg static_reg_cfg;
} isp_adgamma_reg_cfg;

typedef enum {
    ISP_DIS_DATA_TYPE_YUV = 0,
    ISP_DIS_DATA_TYPE_RAW = 1,
    ISP_DIS_DATA_TYPE_BUTT
} isp_dis_data_type;

#define DIS_STATS_NUM                   9
typedef struct {
    td_s32 h_delta[DIS_STATS_NUM];
    td_s32 h_sad[DIS_STATS_NUM];
    td_s32 h_mv[DIS_STATS_NUM];
    td_s32 v_delta[DIS_STATS_NUM];
    td_s32 v_sad[DIS_STATS_NUM];
    td_s32 v_mv[DIS_STATS_NUM];
    td_u32 h_motion;
    td_u32 v_motion;
    td_u32 h_offset;
    td_u32 v_offset;
} isp_dis_stats;

typedef struct {
    td_bool resh;
    td_u8  max_mode;
    td_u16 dehaze_thld;

    td_u16 dehaze_blthld;
    td_u8  dehaze_neg_mode;

    td_u16 block_sum;
    td_u8   dchnum;
    td_u8   dcvnum;

    td_u8   offline_mode;
    td_u8   update_mode;
    td_u8   dehaze_update;
    td_u16   f_hblank;
    td_u16   b_hblank;
    td_u16   f_vblank;
    td_u16   b_vblank;
} isp_dehaze_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   buf_id;
    td_u8   strength;
    td_u16  air_r;
    td_u16  air_g;
    td_u16  air_b;
    td_u8   lut[DEFOG_LUT_NUM];
    td_u32  prestat[DEFOG_ZONE_NUM / DEFOG_REG_NUM];
    td_u32  update;
    td_u32  lut_update;

    td_u32  width;
    td_u32  height;
    td_u16  blockhsize;
    td_u16  blockvsize;
    td_u32  phasex;
    td_u32  phasey;

    td_u32 sml_map_offset; /* 20bit */
    td_u32 stat_start_x;
    td_u32 stat_end_x;
    td_u8  statnum_h;
    td_u8  statnum_v;
} isp_dehaze_dyna_cfg;

typedef struct {
    td_bool   dehaze_en;
    td_bool   lut2_stt_en;
    isp_dehaze_dyna_cfg   dyna_reg_cfg;
    isp_dehaze_static_cfg static_reg_cfg;
} isp_dehaze_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_bool static_resh1;
    td_u8   dpcc_output_mode;      /* U4.0 */
    td_u32  dpcc_bpt_ctrl;         /* U12.0 */
} isp_dpc_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   buf_id;
    td_u16  dpcc_bpt_number;
    td_u32  update_index;
    td_u32  dpcc_bp_table[OT_ISP_STATIC_DP_COUNT_NORMAL];
    td_bool resh1;
    td_u8   buf_id1;
} isp_dpc_usr_static_cor_cfg;

typedef struct {
    td_bool resh;
    td_bool resh1;
    td_bool dpcc_hard_thr_en;
    td_s8   dpcc_sup_twinkle_thr_max;
    td_s8   dpcc_sup_twinkle_thr_min;
    td_u16  dpcc_rake_ratio;
} isp_dpc_usr_dyna_cor_cfg;

typedef struct {
    isp_dpc_usr_dyna_cor_cfg   usr_dyna_cor_reg_cfg;
    isp_dpc_usr_static_cor_cfg usr_sta_cor_reg_cfg;
} isp_dpc_usr_cfg;

typedef struct {
    td_bool resh;
    td_bool resh1;
    td_bool dpc_stat_en;
    td_u8   dpcc_set_use;                           /* U4.0 */
    td_u8   dpcc_line_kerdiff_fac;
    td_u8   dpcc_blend_mode;
    td_u8   dpcc_bit_depth_sel;
    td_u16  dpcc_line_std_thr[ISP_DPC_LCH_SET_NUM];
    td_u8   dpcc_line_diff_thr[ISP_DPC_LCH_SET_NUM];
    td_u8   dpcc_line_aver_fac[ISP_DPC_LCH_SET_NUM];
    td_u16  dpcc_mode;                              /* U10.0 */
    td_u16  dpcc_methods_set1;                      /* U13.0 */
    td_u16  dpcc_methods_set2;                      /* U13.0 */
    td_u16  dpcc_methods_set3;                      /* U13.0 */
    td_u16  dpcc_line_thr[ISP_DPC_SET_NUM];         /* U16.0 */
    td_u16  dpcc_line_mad_fac[ISP_DPC_SET_NUM];     /* U14.0 */
    td_u16  dpcc_pg_fac[ISP_DPC_SET_NUM];           /* U14.0 */
    td_u16  dpcc_rnd_thr[ISP_DPC_SET_NUM];          /* U16.0 */
    td_u32  dpcc_rnd_thr_mtp[ISP_DPC_SET_NUM];      /* U32.0 */
    td_u16  dpcc_rg_fac[ISP_DPC_SET_NUM];           /* U14.0 */
    td_u32  dpcc_rg_fac_mtp[ISP_DPC_SET_NUM];       /* U32.0 */
    td_u16  dpcc_ro_limits;                         /* U12.0 */
    td_u16  dpcc_rnd_offs;                          /* U12.0 */
    td_u32  dpcc_bad_thresh;                        /* U32.0 */
    td_u32  dpcc_alpha;                             /* U32.0 */
    td_u16  dpcc_amp_coef_k;                        /* U4.8  */
    td_u16  dpcc_amp_coef_min;                      /* U1.8  */
    td_bool dpc_eigh_ref_en;
    td_bool dpc_quad_ref_en;
    td_u16  dpc_wb_rgain_recp;                      /* U4.8  */
    td_u16  dpc_wb_bgain_recp;                      /* U4.8  */
    td_u8   dpc_offset;                             /* U8.0  */
    td_u8   dpc_pgfac_eigh;                         /* U4.4  */
    td_u8   dpc_pgthr_eigh;                         /* U8.0  */
    td_u8   dpc_ro_eigh;                            /* U2.0  */
} isp_dpc_dyna_cfg;

typedef struct {
    td_bool dpc_en[ISP_DPC_MAX_CHN_NUM];
    td_bool lut2_stt_en;
    td_u8   chn_num;
    isp_dpc_static_cfg    static_reg_cfg;
    isp_dpc_dyna_cfg      dyna_reg_cfg;
    isp_dpc_usr_cfg       usr_reg_cfg;
} isp_dpc_reg_cfg;

typedef struct {
    td_bool  static_resh;
    td_u8    gain_thd_sel_d;
    td_u8    dir_var_scale;
    td_u8    dir_rly[2]; /* dir rly size 2 */

    td_u8    max_var_clip_min; /* 8 bit */
    td_u16   o_max_chg;
    td_u16   u_max_chg;
    td_u8    sht_var_wgt1;
    td_u8    sht_var_diff_wgt0;
    td_u8    sht_var_thd0;
    td_u8    sht_var_sft;
    td_u8    lmt_mf[ISP_SHARPEN_FREQ_CORING_LENGTH];
    td_u8    lmt_hf[ISP_SHARPEN_FREQ_CORING_LENGTH];

    td_u8    skin_src_sel;
    td_u8    skin_cnt_thd[2];   /* skin cnt thd size 2 */
    td_u8    skin_edge_sft;
    td_u8    skin_edge_thd[2]; /* skin edg thd size 2 */

    td_u8    chr_r_var_sft;
    td_u8    chr_r_ori_cb;    /* U8.0 */
    td_u8    chr_r_ori_cr;    /* U8.0 */
    td_u8    chr_r_sft[4];    /* chr r sft size 4 */
    td_u8    chr_r_thd[2];    /* chr r thd size 2 */
    td_u8    chr_g_ori_cb;    /* U8.0 */
    td_u8    chr_g_ori_cr;    /* U8.0 */
    td_u8    chr_g_sft[4];    /* chr g sft size 4 */
    td_u8    chr_g_thd[2];    /* chr g thd size 2 */
    td_u8    chr_b_var_sft;
    td_u8    chr_b_ori_cb;    /* U8.0 */
    td_u8    chr_b_ori_cr;    /* U8.0 */
    td_u8    chr_b_sft[4];    /* chr b sft size 4 */
    td_u8    chr_b_thd[2];    /* chr b thd size 2 */

    /* filter */
    td_s8    lpf_coef_ud[3];  /* S8.0  lpf coef ud size  3  */
    td_s8    lpf_coef_d[3];   /* S8.0  lpf coef  d size  3  */
    td_s8    hsf_coef_ud[3];  /* S8.0  hsf coef ud size  3  */
    td_s8    hsf_coef_d0[13]; /* S8.0  hsf coef d0 size 13  */
    td_s8    hsf_coef_d1[13]; /* S8.0  hsf coef d1 size 13  */
    td_s8    hsf_coef_d2[13]; /* S8.0  hsf coef d2 size 13  */
    td_s8    hsf_coef_d3[13]; /* S8.0  hsf coef d3 size 13  */

    td_u8    lpf_sft_ud;     /* U4.0--[3, 15] */
    td_u8    lpf_sft_d;      /* U4.0--[3, 15] */
    td_u8    hsf_sft_ud;     /* U4.0--[3, 15] */
    td_u8    hsf_sft_d;      /* U4.0--[3, 15] */

    td_u8    en_shp8_dir;

    /* shoot ctrl */
    td_u8    lf_gain_wgt;
    td_u8    hf_gain_sft;
    td_u8    mf_gain_sft;
    td_u8    sht_var_sel;
    td_u8    sht_var5x5_sft;
    td_u8    detail_thd_sel;
    td_u8    detail_thd_sft;

    td_u8    skin_cnt_mul;
} isp_sharpen_static_reg_cfg;

typedef struct {
    td_bool  resh;
    td_u8    gain_thd_sft_d;
    td_u8    gain_thd_sel_ud;
    td_u8    gain_thd_sft_ud;
    td_u8    dir_var_sft;

    td_u8    dir_rly_thr_low;
    td_u8    dir_rly_thrhigh;

    td_u8    sel_pix_wgt;
    td_u8    sht_var_wgt0;
    td_u8    sht_var_diff_thd[2]; /* sht var diff thd size 2 */
    td_u8    sht_var_diff_wgt1;

    td_u8    rmf_gain_scale;
    td_u8    bmf_gain_scale;
    td_s16   sht_var_diff_mul;    /* convert division to multiplication.            12-bit signed integer    S8.4 */
} isp_sharpen_default_dyna_reg_cfg;

typedef struct {
    td_bool  resh;
    td_bool  switch_mode;
    td_u8    buf_id;
    td_u16   mf_gain_d[SHRP_GAIN_LUT_SIZE];
    td_u16   mf_gain_ud[SHRP_GAIN_LUT_SIZE];
    td_u16   hf_gain_d[SHRP_GAIN_LUT_SIZE];
    td_u16   hf_gain_ud[SHRP_GAIN_LUT_SIZE];
    td_u8    osht_amt;
    td_u8    usht_amt;

    td_u16   mf_mot_gain_d[SHRP_GAIN_LUT_SIZE];
    td_u16   mf_mot_gain_ud[SHRP_GAIN_LUT_SIZE];
    td_u16   hf_mot_gain_d[SHRP_GAIN_LUT_SIZE];
    td_u16   hf_mot_gain_ud[SHRP_GAIN_LUT_SIZE];
    td_u8    osht_mot_amt;
    td_u8    usht_mot_amt;
    td_bool  mot_enable;
    td_u8    mot_thd0;   /* Range: [0, 15] */
    td_u8    mot_thd1;   /* Range: [0, 15] */
    td_u16   mot_gain0;   /* Range: [0, 256] */
    td_u16   mot_gain1;   /* Range: [0, 256] */
    td_u16   mot_mul;
    td_u8    print_sel;

    td_u8    en_chr_ctrl;
    td_u8    chr_r_gain;       /* U1.5  [0 32] */
    td_u16   chr_g_gain;       /* U1.5  [0 32] */
    td_u16   chr_gmf_gain;     /* U1.5  [0 32] */
    td_u8    chr_b_gain;       /* U1.5  [0 32] */
    td_u8    en_skin_ctrl;
    td_u8    skin_edge_wgt[2]; /* U0.5  [0 31] skin edge wgt size 2  */

    td_u8    en_weak_detail_adj;
    td_u8    weak_detail_gain;

    td_u8    en_sht_ctrl_by_var;
    td_u8    sht_bld_rt;
    td_u8    sht_var_thd1;
    td_u8    en_luma_ctrl;
    td_bool  en_detail_ctrl;
    td_u8    detail_osht_amt;
    td_u8    detail_usht_amt;
    td_u8    luma_wgt[OT_ISP_SHARPEN_LUMA_NUM];
    td_u16   sht_var_mul;     /* convert division to multiplication.            11-bit unsigned integer  U7.4 */
    td_s16   detail_osht_mul;
    td_s16   detail_usht_mul;
    td_u8    dir_diff_sft;
    td_u8    dir_rt[2];     /* dir rt size 2 */
    td_s16   chr_r_mul;      /* 9-bit signed integer      S5.4 */
    td_s16   chr_g_mul;      /* s10.4 */
    td_s16   chr_gmf_mul;    /* s10.4 */
    td_s16   chr_b_mul;      /* 9-bit signed integer      S5.4 */
    td_s16   skin_edge_mul;
    td_u8    skin_max_u;
    td_u8    skin_min_u;
    td_u8    skin_max_v;
    td_u8    skin_min_v;
    td_u16   o_max_gain;   /* 11bit */
    td_u16   u_max_gain;   /* 11bit */
    td_u8    detail_osht_thr[2]; /* detail osht thr size 2 */
    td_u8    detail_usht_thr[2]; /* detail usht thr size 2 */
    td_u32   update_index;
} isp_sharpen_mpi_dyna_reg_cfg;

typedef struct {
    isp_sharpen_mpi_dyna_reg_cfg          mpi_dyna_reg_cfg;
    isp_sharpen_default_dyna_reg_cfg      default_dyna_reg_cfg;
} isp_sharpen_dyna_reg_cfg;

typedef struct {
    td_bool  enable;
    td_bool  lut2_stt_en;
    isp_sharpen_dyna_reg_cfg          dyna_reg_cfg;
    isp_sharpen_static_reg_cfg        static_reg_cfg;
} isp_sharpen_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_bool ge_gr_gb_en;          /* u1.0 */
    td_bool ge_gr_en;            /* u1.0 */
    td_bool ge_gb_en;            /* u1.0 */
} isp_ge_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   ge_ct_slope1[GE_MAX_CHN_NUM];   /* u4.0    0x9 */
    td_u8   ge_ct_slope2[GE_MAX_CHN_NUM];   /* u4.0    0x9 */
    td_u16  ge_ct_th2[GE_MAX_CHN_NUM];     /* u14.0   0x200 */
} isp_ge_usr_cfg;

typedef struct {
    td_bool resh;
    td_u16  ge_ct_th1[GE_MAX_CHN_NUM];     /* u14.0   0x400 */
    td_u16  ge_ct_th3[GE_MAX_CHN_NUM];     /* u14.0   0x200 */
    td_u16  ge_strength;      /* u9.0  0x80 */
} isp_ge_dyna_cfg;

typedef struct {
    td_bool ge_en[GE_MAX_CHN_NUM];
    td_u8   chn_num;
    isp_ge_static_cfg     static_reg_cfg;
    isp_ge_dyna_cfg       dyna_reg_cfg;
    isp_ge_usr_cfg        usr_reg_cfg;
} isp_ge_reg_cfg;

typedef struct {
    td_bool resh;
    td_u32  update_index;
    td_u8   bit_depth_in;
    td_u8   bit_depth_out;
    td_u32  lut[OT_ISP_EXPANDER_NODE_NUM];
} isp_expander_usr_cfg;

typedef struct {
    td_bool enable;
    isp_expander_usr_cfg usr_cfg;
} isp_expander_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_s16  h_coef[8];      /* h coef size 8 */
    td_s8   v_coef[2];      /* v coef size 2 */
    td_u16  coring_limit;
    td_u8   midf_bldr;
} isp_mcds_static_reg_cfg;

typedef struct {
    td_bool dyna_resh;
} isp_mcds_dyna_reg_cfg;

typedef struct {
    td_bool hcds_en;                        /* UV Horizontal DS enable; 0: close; 1: open */
    isp_mcds_static_reg_cfg static_reg_cfg;
    isp_mcds_dyna_reg_cfg   dyna_reg_cfg;
} isp_mcds_reg_cfg;

#define  WDR_FRAME0        0
#define  WDR_FRAME1        1
#define  WDR_FRAME2        2
#define  WDR_FRAME3        3

#define  WDR_CHL_RG        0
#define  WDR_CHL_GR        1
#define  WDR_CHL_GB        2
#define  WDR_CHL_BG        3
#define  WDR_NOISE_LUT_NUM 257
#define  WDR_SIGMA_LUT_NUM 513

#define WDR_BITDEPTH        14
#define WDR_SQRT_OUT_BITS   8

typedef struct {
    td_bool first_frame;
    td_bool resh;
    td_bool gray_scale_mode;

    td_bool save_blc;
    td_bool force_long_smooth_en;                    /* u1.0,[0,1] */

    td_u8   bldr_lhf_idx;                           /* u5.0, [0,16] */
    td_u8   mask_similar_thr;                       /* u6.0, [0,63], */
    td_u8   mask_similar_cnt;                       /* u3.0, [0,7] */
    td_u16  expo_value_lut[OT_ISP_WDR_MAX_FRAME_NUM];             /* u8.6, [0,16383] */
    td_u16  flick_exp_ratio[OT_ISP_EXP_RATIO_NUM];
    td_u32  max_ratio;                            /* u17.0, [0,131071] */
    td_u16  dft_wgt_fl;                            /* u9.0, [0,256] */
    td_u16  expo_r_ratio_lut[OT_ISP_EXP_RATIO_NUM];        /* u10.0,[0,1023] */
    td_u32  blc_comp_lut[OT_ISP_WDR_MAX_FRAME_NUM - 1];           /* s27.0,[0,67108863] */

    td_u16  saturate_thr;             /* u14.0,[0,16383] */
    td_u16  fusion_saturate_thr;       /* u14.0,[0,16383] */

    td_u8   seg_max_val[ISP_WDR_SEG_NUM];
    td_u8   seg_idx_base[ISP_WDR_SEG_NUM];

    td_u8   filter_coef_h[ISP_WDR_FILTER_NUM];
    td_u8   filter_coef_v[ISP_WDR_FILTER_NUM];
} isp_fswdr_static_cfg;

typedef struct {
    td_bool resh;
    td_bool fusion_mode;                /* u1.0,  fusion mode */
    td_bool short_expo_chk;              /* u1.0, [0,1] */
    td_u8   mdt_l_bld;                  /* u8.0, [0,254],the amount of long-expo is added in motion mask */
    td_u8   bnr_full_mdt_thr;            /* u8.0, [0,254] */

    td_u8   mdt_still_thr;
    td_u8   mdt_full_thr;
    td_u32  update_index;

    td_u8   short_sigma_gwgt;          /* U4.0  [0,15] */
    td_u8   short_sigma_cwgt;          /* U4.0 [0,15] */

    td_bool sfnr;                     /* u1.0 */

    td_u16  pixel_avg_max_diff; /* u11.0,[0,4095] */

    td_u8   short_sigma_l1_g_weight;           /* u4.4,[0.255] */
    td_u8   short_sigma_l2_g_weight;           /* u4.4,[0.255] */
    td_u8   short_sigma_l1_c_weight;           /* u4.4,[0.255] */
    td_u8   short_sigma_l2_c_weight;           /* u4.4,[0.255] */

    td_bool fusion_smooth;                 /* u1.0, [0,1] */
    td_u32  fusion_smooth_diff_thd;        /* u16.0, [0,65535] */
} isp_fswdr_usr_cfg;

typedef struct {
    td_bool resh;
    td_bool wdr_mdt_en;                         /* u1.0,  motion detection switch */
    td_bool avg_mode_select;
    td_bool sigma_mode_select;
    td_bool wdr_revmode;
    td_u8   md_thr_low_gain[ISP_WDR_CHN_MAX - 1];                    /* u4.2, [0,63] */
    td_u8   md_thr_hig_gain[ISP_WDR_CHN_MAX - 1];                    /* u4.2, [0,63] */
    td_u8   sqrt_again_g;                       /* u3.0, [0,6] */
    td_u8   sqrt_dgain_g;                       /* u3.0, [0,6] */
    td_u8   mdt_nos_floor;                      /* u7.0, [0,127] */
    td_u16  short_thr[OT_ISP_WDR_MAX_FRAME_NUM - 1]; /* u14.0,[0,16383],the thr of beginning to adopt short exposure */
    td_u16  long_thr[OT_ISP_WDR_MAX_FRAME_NUM - 1];
    td_u16  still_thr_lut[ISP_WDR_CHN_MAX - 1];     /* u14.0,[0,16383] */

    td_u16  t_nos_floor;                        /* u12.0,[0,4095] */
    td_bool erosion_en;
    td_u16  fusion_thr_r_lut[ISP_WDR_CHN_MAX];     /* u14.0,[0,16383] */
    td_u16  fusion_thr_g_lut[ISP_WDR_CHN_MAX];     /* u14.0,[0,16383] */
    td_u16  fusion_thr_b_lut[ISP_WDR_CHN_MAX];     /* u14.0,[0,16383] */

    td_bool force_long;         /* u1.0,[0,1] */
    td_u16  force_long_low_thr; /* u11.0,[0,4095] */
    td_u16  force_long_hig_thr; /* u11.0,[0,4095] */
    td_u16  short_check_thd; /* u11.0,[0,4095] */

    td_bool bcom_en;
    td_bool bdec_en;
    td_u8   bcom_alpha;
    td_u8   bdec_alpha;
    td_u8   frm_merge;
    td_bool fusion_data_mode;

    td_u16  wdr_bginfo_low_thd0;
    td_u16  wdr_bginfo_high_thd0;

    td_u16  wdr_bginfo_low_thd1;
    td_u16  wdr_bginfo_high_thd1;

    td_bool bg_info_en;

    td_u8   wdr_ref_stat_hblk_num;
    td_u8   wdr_reg_stat_vblk_num;

    td_u8   wdr_cur_calc_hblk_num;
    td_u8   wdr_cur_calc_vblk_num;

    td_u8   wdr_stat_shift;
    td_u8   wdr_blk_idx;
    td_u8   wdr_split_num;

    td_u16  wdr_kx;
    td_u16  wdr_ky;

    td_u32  wdr_smlmap_offset;

    td_u8   wdr_lpf_coef[9];  /* 9 */
    td_u8   wdr_lpf_shift;

    td_u16  wdr_stat_startx;
    td_u16  wdr_stat_endx;

    td_u16  sigma_nml_lut[WDR_SIGMA_LUT_NUM]; /* u12.0 */
    td_u16  noise_profile[2][WDR_NOISE_LUT_NUM]; /* u16.0 */
} isp_fswdr_dyna_cfg;

typedef struct {
    td_bool fusion_mode;
    td_bool wdr_mdt_en;
    td_u16  saturate_thr;
    td_u16  fusion_saturate_thr;
    td_u16  short_thr[ISP_WDR_CHN_MAX - 1];
    td_u16  long_thr[ISP_WDR_CHN_MAX - 1];
} isp_fswdr_sync_cfg;

typedef struct {
    td_bool wdr_en;
    isp_fswdr_static_cfg static_reg_cfg;
    isp_fswdr_dyna_cfg   dyna_reg_cfg;
    isp_fswdr_usr_cfg    usr_reg_cfg;
    isp_fswdr_sync_cfg   sync_reg_cfg;
} isp_wdr_reg_cfg;

typedef struct {
    td_bool resh;
} isp_flicker_static_cfg;

typedef struct {
    td_bool resh;
    td_u16  over_thr;         /* U10 */
    td_u16  exp_ratio0;       /* U8.6      isp.c */
    td_u16  exp_ratio1;       /* U8.6      isp.c */
    td_u16  exp_ratio2;       /* U8.6      isp.c */
    td_s16  gr_avg_pre;        /* S11       rw */
    td_s16  gb_avg_pre;        /* S11       rw */
} isp_flicker_dyna_cfg;

typedef struct {
    td_bool enable;
    isp_flicker_static_cfg static_reg_cfg;
    isp_flicker_dyna_cfg   dyna_reg_cfg;
} isp_flicker_reg_cfg;

typedef struct {
    td_bool usr_resh;
    td_s16  center_ver_coor;
    td_s16  center_hor_coor;
    td_u32  square_radius;
} isp_rc_usr_cfg;

typedef struct {
    td_bool rc_en;
    isp_rc_usr_cfg    usr_reg_cfg;
} isp_radial_crop_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_u8   win_num_h;
    td_u8   win_num_v;
} isp_lsc_static_cfg;

typedef struct {
    td_bool coef_update;
    td_bool lut_update;
    td_bool switch_lut2_stt_reg_new;
    td_u8   switch_reg_new_cnt;
    td_u8   buf_id;
    td_u8   mesh_scale;
    td_u16  weight;
    td_u16  mesh_str;
    td_u16  width_offset;
    td_u16  delta_x[OT_ISP_LSC_GRID_COL - 1];
    td_u16  delta_y[OT_ISP_MLSC_Y_HALF_GRID_NUM];
    td_u16  inv_x[OT_ISP_LSC_GRID_COL - 1];
    td_u16  inv_y[OT_ISP_MLSC_Y_HALF_GRID_NUM];
    td_u32  r_gain[OT_ISP_LSC_GRID_POINTS];
    td_u32  gr_gain[OT_ISP_LSC_GRID_POINTS];
    td_u32  gb_gain[OT_ISP_LSC_GRID_POINTS];
    td_u32  b_gain[OT_ISP_LSC_GRID_POINTS];
    td_u32  update_index;
} isp_lsc_usr_cfg;

typedef struct {
    td_bool lsc_en;
    td_bool lut2_stt_en;
    isp_lsc_usr_cfg       usr_reg_cfg;
    isp_lsc_static_cfg    static_reg_cfg;
} isp_lsc_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_u16  node_num;
} isp_rlsc_static_cfg;

typedef struct {
    td_bool usr_resh;
    td_bool coef_update;
    td_bool lut_update;
    td_bool switch_lut2_stt_reg_new;
    td_u8   switch_reg_new_cnt;
    td_u8   buf_id;
    td_u16  width_offset;
    td_u16  center_rx;
    td_u16  center_ry;
    td_u16  center_gr_x;
    td_u16  center_gr_y;
    td_u16  center_gb_x;
    td_u16  center_gb_y;
    td_u16  center_bx;
    td_u16  center_by;
    td_u16  off_center_r;
    td_u16  off_center_gr;
    td_u16  off_center_gb;
    td_u16  off_center_b;
    td_u16  gain_str;
    td_u8   gain_scale;
    td_u16  gain_lut[OT_ISP_BAYER_CHN_NUM][OT_ISP_RLSC_POINTS + 1];
    td_u32  update_index;
} isp_rlsc_usr_cfg;

typedef struct {
    td_bool rlsc_en;
    td_bool lut2_stt_en;
    isp_rlsc_usr_cfg      usr_reg_cfg;
    isp_rlsc_static_cfg   static_reg_cfg;
} isp_rlsc_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_bool nddm_cac_blend_en;
    td_bool new_lcac_proc;
    td_u8   r_counter_thr;
    td_u8   g_counter_thr;
    td_u8   b_counter_thr;
    td_u8   de_purple_str[LCAC_STRENGTH_NUM];
    td_u16  satu_thr;
    td_u16  nddm_cac_blend_rate;
    td_s16  cb_cr_ratio_lmt_high2;
    td_s16  cb_cr_ratio_lmt_low2;
} isp_local_cac_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   luma_high_cnt_thr;    /* u8.0, [0,153] */
    td_u8   cb_cnt_high_thr;      /* u7.0, [0,68] */
    td_u8   cb_cnt_low_thr;       /* u7.0, [0,68] */
    td_u8   bld_avg_cur;         /* u4.0, [0, 8] */
    td_u16  cb_thr;            /* u12.0, [0,4095] */
    td_u16  var_thr;
    td_s16  cb_cr_ratio_lmt_high;
    td_u32  update_index;
} isp_local_cac_usr_cfg;

typedef struct {
    td_bool resh;
    td_u8   de_purple_ctr_cr;
    td_u8   de_purple_ctr_cb;
    td_u16  luma_thr;          /* u12.0, [0,4095] */
    td_u16  r_luma_thr;
    td_u16  g_luma_thr;
    td_u16  b_luma_thr;
    td_s16  cb_cr_ratio_lmt_low;
} isp_local_cac_dyna_cfg;

typedef struct {
    td_bool local_cac_en;
    isp_local_cac_usr_cfg    usr_reg_cfg;
    isp_local_cac_dyna_cfg   dyna_reg_cfg;
    isp_local_cac_static_cfg static_reg_cfg;
} isp_lcac_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_u8   cac_norm_factor;
    td_u16  cac_tao;
} isp_acac_static_cfg;

typedef struct {
    td_bool usr_resh;
    td_bool cac_link_en;
    td_u8  cac_det_mode;
    td_u16 cac_str;
    td_u16 lamda_thd0;
    td_u16 lamda_thd1;
    td_u16 lamda_mul;
    td_u16 alpha_r;
    td_u16 alpha_b;
    td_u16 cor_var_thr;
    td_u16 color_dif_thd;
    td_u16 grey_thd;
    td_s16 fcf_cr_cb_ratio_high_limit2;
    td_s16 fcf_cr_cb_ratio_high_limit;
    td_s16 fcf_cr_cb_ratio_low_limit;
    td_s16 fcf_cr_cb_ratio_low_limit2;
    td_u8  purple_alpha;
    td_u8  edge_alpha;
    td_u16 det_satu_thr;
    td_u32 update_index;
} isp_acac_usr_cfg;

typedef struct {
    td_bool dyna_resh;
    td_u16  edge_thd_mul;
    td_u16  edge_thd0;
    td_u16  edge_thd1;
    td_u16  var_thd;
} isp_acac_dyna_cfg;

typedef struct {
    td_bool acac_en;
    isp_acac_usr_cfg    usr_reg_cfg;
    isp_acac_dyna_cfg   dyna_reg_cfg;
    isp_acac_static_cfg static_reg_cfg;
} isp_acac_reg_cfg;

/* bayer_sharpen register - static */
typedef struct {
    td_bool static_resh;
    td_bool fil_coef_mode;
    td_u8   bit_depth;
    td_u8   var_bit_depth;
    td_s16  hsf_coef0_d[OT_ISP_BSHP_HSF_NUM];
    td_s16  hsf_coef1_d[OT_ISP_BSHP_HSF_NUM];
    td_s16  hsf_coef2_d[OT_ISP_BSHP_HSF_NUM];
    td_s16  hsf_coef3_d[OT_ISP_BSHP_HSF_NUM];
    td_s16  lpf_coef_d[OT_ISP_BSHP_LPF_SIZE];
    td_s16  lpf_coef_ud[OT_ISP_BSHP_LPF_SIZE];
    td_s16  hsf_coef_ud[OT_ISP_BSHP_LPF_SIZE];
    td_u8   hsf_coef_d_sft;
    td_u8   hsf_coef_ud_sft;
    td_u8   lpf_coef_d_sft;
    td_u8   lpf_coef_ud_sft;
    td_u8   mf_gain_sft;
    td_u8   hf_gain_sft;
    td_u8   u8gain_thd_sel_ud;
    td_u8   u8gain_thd_sel_d;
    td_u8   u8gain_thd_sft_ud;
    td_u8   u8gain_thd_sft_d;
} isp_bshp_static_cfg;

/* bayer_sharpen register - dynamic */
typedef struct {
    td_bool dyna_resh;
    td_u8   buf_id;
    td_s16  neg_luma_thd0_mul;
    td_s16  neg_luma_thd1_mul;
    td_s16  neg_mf_mul;
    td_s16  neg_hf_mul;
    td_s16  sht_var_diff_mul;
    td_s16  reg_neg_ctrl_mul;
    td_s16  shp_on_g_mul;
    td_bool chr_ctrl_en;
    td_bool luma_ctrl_en;
    td_bool sht_ctrl_by_var_en;
    td_bool blk_ctrl_mode;
    td_u8   dir_diff_sft;
    td_u16  dir_rt[OT_ISP_BSHP_THD_NUM];
    td_u16  neg_luma_thd0[OT_ISP_BSHP_THD_NUM];
    td_u16  neg_luma_thd1[OT_ISP_BSHP_THD_NUM];
    td_u16  neg_luma_wgt0[OT_ISP_BSHP_THD_NUM];
    td_u16  neg_luma_wgt1[OT_ISP_BSHP_THD_NUM];
    td_u8   neg_mf_sft;
    td_u16  neg_mf_thd[OT_ISP_BSHP_THD_NUM];
    td_u16  neg_mf_wgt[OT_ISP_BSHP_THD_NUM];
    td_u8   neg_hf_sft;
    td_u16  neg_hf_thd[OT_ISP_BSHP_THD_NUM];
    td_u16  neg_hf_wgt[OT_ISP_BSHP_THD_NUM];
    td_u16  lmf_mf[OT_ISP_BSHP_FILTER_NUM];
    td_u16  lmf_hf[OT_ISP_BSHP_FILTER_NUM];
    td_u8   sht_var5x5_sft;
    td_u16  sht_var_diff_thd[OT_ISP_BSHP_THD_NUM];
    td_u8   rec_neg_ctrl_sft;
    td_u16  reg_neg_ctrl_thd[OT_ISP_BSHP_THD_NUM];
    td_u16  reg_neg_ctrl_wgt[OT_ISP_BSHP_THD_NUM];
    td_u8   sel_pix_wgt;
    td_u16  o_max_gain;
    td_u16  u_max_gain;
    td_u16  o_max_chg;
    td_u16  u_max_chg;
    td_u16  detail_u_sht_amt;
    td_u16  detail_o_sht_amt;
    td_u16  shp_on_g_thd[OT_ISP_BSHP_THD_NUM];
} isp_bshp_dyna_cfg;

/* bayer_sharpen register - user */
typedef struct {
    td_bool user_resh;
    td_u32  gain_d0[OT_ISP_BSHP_HALF_CURVE_NUM];  /* even gain dir 0xfff000->hfgaind, 0x000fff->mfgaind */
    td_u32  gain_d1[OT_ISP_BSHP_HALF_CURVE_NUM];  /* odd  gain dir 0xfff000->hfgaind, 0x000fff->mfgaind */
    td_u32  gain_ud0[OT_ISP_BSHP_HALF_CURVE_NUM]; /* even gain un_dir 0xfff000->hfgaind, 0x000fff->mfgaind */
    td_u32  gain_ud1[OT_ISP_BSHP_HALF_CURVE_NUM]; /* odd  gain un_dir 0xfff000->hfgaind, 0x000fff->mfgaind */
    td_u16  dir_rly[OT_ISP_BSHP_THD_NUM];
    td_u16  luma_wgt[OT_ISP_BSHP_LUMA_WGT_NUM];
    td_u16  sht_var_diff_wgt[OT_ISP_BSHP_THD_NUM];
    td_u16  o_sht_amt;
    td_u16  u_sht_amt;
    td_u16  shp_on_g_wgt[OT_ISP_BSHP_THD_NUM];
} isp_bshp_user_cfg;

typedef struct {
    td_bool bshp_enable;
    td_bool lut2_stt_en;
    isp_bshp_static_cfg static_reg_cfg;
    isp_bshp_dyna_cfg   dyna_reg_cfg;
    isp_bshp_user_cfg   usr_reg_cfg;
} isp_bshp_reg_cfg;

typedef struct {
    td_bool ccm_prot_en;
    td_u32 data_cc_thd0;
    td_u32 data_cc_thd1;
    td_u16 cc_prot_ratio;
    td_u32 data_rr_thd0;
    td_u32 data_rr_thd1;
    td_u32 data_gg_thd0;
    td_u32 data_gg_thd1;
    td_u32 data_bb_thd0;
    td_u32 data_bb_thd1;
    td_u32 max_rgb_thd;
    td_u16 rgb_prot_ratio;
} isp_demosaic_darkregion_prot;

typedef struct {
    td_bool resh;
    td_bool de_sat_enable;       /* u1.0 */
    td_bool ahd_enable;         /* u1.0 */
    td_bool de_fake_enable;      /* u1.0 */
    td_bool g_intp_ctrl_enable;   /* u1.0 */
    td_u8   lpff3;            /* u8.0, */
    td_u8   hv_blend_limit1;    /* u8.0, */
    td_u8   hv_blend_limit2;    /* u8.0, */
    td_u8   hv_color_ratio;     /* u8.0, */
    td_u8   hv_selection;      /* u2.0 */
    td_u8   g_clip_bit_sft;      /* u4.0, */
    td_u8   dither_mask;       /* u8.0, */
    td_u8   dither_ratio;      /* u8.0, */
    td_u8   cx_var_max_rate;     /* u4.0, */
    td_u8   cx_var_min_rate;     /* u4.0, */
    td_u16  de_sat_thresh1;    /* u10.0, */
    td_u16  de_sat_thresh2;    /* u10.0, */
    td_u16  de_sat_hig;        /* u9.0, */
    td_u16  de_sat_prot_sl;     /* u10.0 */
    td_u16  ahd_part1;        /* u10.0, */
    td_u16  ahd_part2;        /* u8.0, */
    td_u16  gf_th_low;         /* u9.0,   gf low-fix strength */
    td_u16  gf_th_hig;         /* u9.0,   gf high-fix strength */
    td_u16  cb_cr_avg_thr;      /* u12.0, */
} isp_demosaic_static_cfg;

typedef struct {
    td_bool resh;
    td_bool update_gf;
    td_u8   de_f_color_cr;
    td_u8   de_f_color_cb;
    td_u8   cc_hf_max_ratio;      /* u5.0, */
    td_u8   cc_hf_min_ratio;      /* u5.0, */
    td_u8   lpff0;             /* s4.0, */
    td_u8   lpff1;             /* u5.0, */
    td_u8   lpff2;             /* u5.0, */
    td_u8   dither_max;         /* u8.0, */
    td_u8   fcr_gf_gain;         /* s3.2,   fcr control */
    td_u8   awb_gf_gain_low;      /* u3.4, */
    td_u8   awb_gf_gain_hig;      /* u3.4, */
    td_u8   awb_gf_gain_max;      /* u4.0, */
    td_u8   ehc_gray;           /* u3.4, */
    td_u16  fake_cr_var_thr_low;
    td_u16  fake_cr_var_thr_high;
    td_u16  de_sat_low;         /* u9.0, */
    td_u16  de_sat_prot_th;      /* u10.0 */
    td_u16  hf_intp_blur_th_low;  /* u10.0, */
    td_u16  hf_intp_blur_th_hig;  /* u10.0, */
    td_u16  fcr_det_low;        /* u12.0,  fcr det thresh */
    td_u16  gf_blur_luts[OT_ISP_DEMOSAIC_LUT_LENGTH]; /* u12.0, */
    td_u8   color_noise_thd_y;    /* u8.0 */
    td_u8   color_noise_str_y;    /* u8.0 */
    isp_demosaic_darkregion_prot pro_dark_region;
} isp_demosaic_dyna_cfg;

typedef struct {
    td_bool vhdm_enable;       /* u1.0, */
    td_bool nddm_enable;       /* u1.0, */
    isp_demosaic_static_cfg static_reg_cfg;
    isp_demosaic_dyna_cfg   dyna_reg_cfg;
} isp_demosaic_reg_cfg;

typedef struct {
    td_bool resh;
    td_u16  fcr_limit1;      /* u12.0, */
    td_u16  fcr_limit2;      /* u12.0, */
    td_u16  fcr_thr;         /* u12.0, */
} isp_antifalsecolor_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   fcr_gain;         /* u5.0, */
    td_u8   fcr_ratio;        /* u6.0, */
    td_u8   fcr_gray_ratio;    /* u5.0, */
    td_u8   fcr_cmax_sel;      /* u4.0, */
    td_u8   fcr_detg_sel;      /* u4.0, */
    td_u16  fcr_hf_thresh_low; /* u10.0,  fcr hf thresh low */
    td_u16  fcr_hf_thresh_hig; /* u10.0,  fcr hf thresh high */
} isp_antifalsecolor_dyna_cfg;

typedef struct {
    td_bool fcr_enable;        /* u1.0, */
    isp_antifalsecolor_static_cfg static_reg_cfg;
    isp_antifalsecolor_dyna_cfg   dyna_reg_cfg;
} isp_antifalsecolor_reg_cfg;

typedef struct {
    td_bool resh;
    td_bool ca_lut_update_en; /* u1.0 */
    td_bool ca_cp_en;
    td_u8   buf_id;
    td_u16  ca_luma_thr_high; /* u10.0 */
    td_u16  ca_luma_ratio_high; /* U1.10   high lightness thresh' ratio in Y-alpha */
    td_u32  y_ratio_lut[OT_ISP_CA_YRATIO_LUT_LENGTH]; /* U1.10 */
    td_u32  update_index;
} isp_ca_usr_cfg;

typedef struct {
    td_bool resh;
    td_u16  ca_iso_ratio; /* 1.10bit  ISO ratio  for UV,max = 2047 FW limi */
} isp_ca_dyna_cfg;

typedef struct {
    td_bool static_resh;
    td_bool ca_llhc_proc_en;  /* u1.0 */
    td_bool ca_skin_proc_en;  /* u1.0 */
    td_bool ca_satu_adj_en;   /* u1.0 */
    td_u16  ca_luma_thr_low;        /* u10.0 */
    td_u16  ca_dark_chroma_thr_low;  /* u10.0 */
    td_u16  ca_dark_chroma_thr_high; /* u10.0 */
    td_u16  ca_s_dark_chroma_thr_low; /* u10.0 */
    td_u16  ca_s_dark_chroma_thr_high; /* u10.0 */
    td_u16  ca_luma_ratio_low;      /* U1.10 */

    td_s16  ca_yuv2_rgb_coef[OT_ISP_CSC_COEF_NUM];   /* s5.10 */
    td_s16  ca_yuv2_rgb_in_dc[OT_ISP_CSC_DC_NUM];   /* S11.0 */
    td_s16  ca_yuv2_rgb_out_dc[OT_ISP_CSC_DC_NUM];  /* S11.0 */

    td_s16  ca_yuv2_rgb_in_dc0;      /* S11.0 */
    td_s16  ca_yuv2_rgb_in_dc1;      /* S11.0 */
    td_s16  ca_yuv2_rgb_in_dc2;      /* S11.0 */
    td_s16  ca_yuv2_rgb_out_dc0;     /* S11.0 */
    td_s16  ca_yuv2_rgb_out_dc1;     /* S11.0 */
    td_s16  ca_yuv2_rgb_out_dc2;     /* S11.0 */
    td_u16  ca_skin_low_luma_min_u;   /* u10 */
    td_u16  ca_skin_low_luma_max_u;   /* u10 */
    td_u16  ca_skin_low_luma_min_uy;  /* u10 */
    td_u16  ca_skin_low_luma_max_uy;  /* u10 */
    td_u16  ca_skin_high_luma_min_u;  /* u10 */
    td_u16  ca_skin_high_luma_max_u;  /* u10 */
    td_u16  ca_skin_high_luma_min_uy; /* u10 */
    td_u16  ca_skin_high_luma_max_uy; /* u10 */
    td_u16  ca_skin_low_luma_min_v;   /* u10 */
    td_u16  ca_skin_low_luma_max_v;   /* u10 */
    td_u16  ca_skin_low_luma_min_vy;  /* u10 */
    td_u16  ca_skin_low_luma_max_vy;  /* u10 */
    td_u16  ca_skin_high_luma_min_v;  /* u10 */
    td_u16  ca_skin_high_luma_max_v;  /* u10 */
    td_u16  ca_skin_high_luma_min_vy; /* u10 */
    td_u16  ca_skin_high_luma_max_vy; /* u10 */
    td_s16  ca_skin_uv_diff;        /* S11 */
    td_u16  ca_skin_ratio_thr_low;   /* u12.0 */
    td_u16  ca_skin_ratio_thr_mid;   /* u12.0 */
    td_u16  ca_skin_ratio_thr_high;  /* u12.0 */
} isp_ca_static_cfg;

typedef struct {
    td_bool ca_en;
    td_bool lut2_stt_en;
    isp_ca_static_cfg static_reg_cfg;
    isp_ca_dyna_cfg   dyna_reg_cfg;
    isp_ca_usr_cfg    usr_reg_cfg;
} isp_ca_reg_cfg;

typedef struct {
    td_bool resh;
    td_u8   buf_id;
    td_bool crb_lut_update_en;
    td_u16 crb_lut[OT_ISP_CRB_LUT_LENGTH];
    td_u16 rgain;                /* R; range: [0x0, 0x7FF]; format:11.0; gain of R color channel */
    td_u16 bgain;                /* R; range: [0x0, 0x7FF]; format:11.0; gain of B color channel */
    td_u32 update_index;
} isp_crb_usr_cfg;

typedef struct {
    td_bool resh;
    td_u32  phasex;
    td_u32  phasey;
    td_u32  stat_start_x;
    td_u32  map_offset; /* 20bit */
    td_u32  awbgain0;
    td_u32  awbgain1;
    td_u8   wdr_enable;                         /* u2.0 */
    td_u8   wdr_fusion;                       /* u1.0 */
    td_bool step_third_en;           /* RW; range: [0, 1];  format:1.0; if disabled, step 3 will be bypassed */
} isp_crb_dyna_cfg;

typedef struct {
    td_bool resh;
    td_bool mode;
    td_bool step_first_en;           /* RW; range: [0, 1];  format:1.0; if disabled, step 1 will be bypassed */
    td_bool step_second_en;          /* RW; range: [0, 1];  format:1.0; if disabled, step 2 will be bypassed */
    td_u8   dchnum;
    td_u8   dcvnum;
    td_u32  maxlevel;
} isp_crb_static_cfg;

typedef struct {
    td_bool crb_en;
    isp_crb_static_cfg static_reg_cfg;
    isp_crb_dyna_cfg   dyna_reg_cfg;
    isp_crb_usr_cfg    usr_reg_cfg;
} isp_crb_reg_cfg;

typedef struct {
    td_bool resh;
    td_u8   mix_enable;
    td_u8   jnlm_sel;
    td_u8   nlm_winsize;
} isp_bayernr_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   buf_id;
    td_bool  snr_enable;
    td_bool  tnr_enable;
    td_bool  tnr2nd_enable;
    td_bool  is_initial;
    td_u8    md_size;
    td_bool  coring_enable;
    td_bool  coring_mot_adp;
    td_u8    coring_motth;
    td_u8    coring_data_mode;

    td_u8    md_ratio;
    td_u8    md_cor_level;
    td_u16   mdcoefdiffy;     /* range:[0, 4095] */
    td_u16   mdcoefsatd;      /* range:[0, 1023] */
    td_bool  satd_en;

    td_bool  en_ai_md;
    td_bool  en_ir_md;
    td_bool  qtr_size_ir;
    td_u8    mix_ratio_rgb;

    td_u8    alha_max_st;
    td_bool  tfr_en;
    td_u8    tfr_thresh;
    td_u16   tfr_thresh1;
    td_bool  tfr_luma;
    td_u8    tp_alpha_gain2nd;
    td_bool  rfr_mode;

    td_u8    bnrlsc_en;
    td_u8    bnrlscratio1;
    td_u8    bnrlscratio2;

    td_u8    tnr_rnt_th;
    td_u8    tnr_rnt_wid;
    td_u16   tnr_nr_strength_st;
    td_u16   tnr_nr_strength_mv;
    td_s16   tnr_nr_strength_slope;

    td_u8   u8bnrblc;
    td_u16  u16expratio;
    td_u8   bitdepth_ir_in;

    td_u8   wdrlong_low;
    td_u8   wdrlong_hig;
    td_u16  wdrlong_slp;
    td_u8   wdrmid_low;
    td_u8   wdrmid_hig;
    td_u16  wdrmid_slp;

    td_u8   wdr_ghost_nrstr;
    td_u8   wdr_ghost_thres;

    td_bool  enable_mix_lut;
    td_u8    mix_target;
    td_u8    mix_gain;
    td_u8    mix_gain_r;
    td_u8    mix_gain_b;

    td_u8   wdr_enable;
    td_u8   wdr_en_fusion;
    td_bool center_wgt_en;
    td_bool bnr_lut_update_en;
    td_u8   wdr_map_flt_mod;
    td_u16  jnlm_max_wt_coef;
    td_u8   jnlm_sym_coef;
    td_u8   jnlm_sad_ratio;
    td_u8   jnlm_gain_m;
    td_u8   jnlm_gain_s;
    td_u8   wdr_map_gain;
    td_bool edge_flt_en;
    td_bool dst_flt_en;
    td_bool sadi_en;
    td_bool dmap_en;
    td_bool nlm_edgectrl;
    td_bool nlm_enable;
    td_bool bm1d_lmt_enable;
    td_u8   bm1d_strength_s;

    td_u8  nois_var;
    td_u8  nlm_nshift;
    td_u8  bm1d_n8_coef;
    td_bool sht_mode;
    td_bool sht_mode1;

    td_u16  edge_dir_slp;
    td_u8   edge_dgain_max;
    td_u8   edge_dgain_slp;
    td_u8   edge_ratio_th;
    td_u8   edge_dmap_coef;
    td_u8   tss;
    td_u8   movtmp_ratio;

    td_u8    sfm_mode;

    td_u8    sfm_noise_l1;
    td_u8    sfm_noise_m1;
    td_u8    sfm_noise_s1;
    td_u8    sfm_noise_l2;
    td_u8    sfm_noise_m2;
    td_u8    sfm_noise_s2;
    td_u8    sfm_offset_m;
    td_u8    sfm_offset_s;
    td_u8    sfm_offset_l;

    td_u8    sfm_dmap_th;
    td_u8    sfm_dmap_s1;
    td_u8    sfm_dmap_s2;
    td_u8    sfm_dmap_s3;
    td_u8    sfm_dmap_s4;

    td_u8    sfm_blend_min1;
    td_u8    sfm_blend_max1;
    td_u8    sfm_blend_th1;
    td_u16   sfm_blend_slp1;

    td_u8    sfm_blend_min2;
    td_u8    sfm_blend_max2;
    td_u8    sfm_blend_th2;
    td_u16   sfm_blend_slp2;

    td_u8    sfm_blend_lmt;
    td_bool  sfm_lmt_adp_en;
    td_u8    sfm_edge_ctrl;
    td_u8    sfm_edge_sad_gain;
    td_bool  sfm_dis_ctrl;

    td_u8    sfm0_sym_coef;
    td_u32   sfm0_r_sad_max;

    td_bool  sfm0_adaptive;
    td_u8    sfm0_nlimit_adp_th0;
    td_u8    sfm0_nlimit_adp_th1;
    td_u8    sfm0_nlimit_adp_str;
    td_u16   sfm0_nlimit_adp_thr;

    td_u8    sfm0_adap_mode;
    td_u8    sfm0_dstgain_max;
    td_u8    sfm0_dstgain_slp;
    td_u8    sfm1_dstgain_max;
    td_u8    sfm1_dstgain_slp;
    td_u8    sfm0_flt_mode;
    td_u8    sfm0_dis_adp_coef;
    td_u8    sfm0_adp_coef;
    td_u8    sfm1_adp_coef;

    td_u8    sfm0_adp_th;
    td_u8    sfm0_dis_adp_th;
    td_u8    sfm_adp_th;

    td_u8    sfm0_adp_th1;
    td_u8    sfm0_dis_adp_th1;
    td_u8    sfm_adp_th1;

    td_u16  shot_ratio;
    td_u16  jnlm_coring_hig;
    td_u16  r_lmt_rgain;
    td_u16  r_lmt_bgain;
    td_u16  lmt_offset;
    td_u32  edge_det_thr;
    td_u8   mix_lut[32]; /* mix_lut:32 */
    td_u8   ds[16]; /* ds:16 */
    td_u8   dp[16]; /* dp:16 */
    td_u8   tsr0;
    td_u8   tsr1;
    td_u8   nshift;
    td_u8   edge_tp1;
    td_u8   edge_tp0;

    td_u16  u16noise_sd_lut[BAYERNR_LMT_LUT_NUM];
    td_u16  u16noise_sd_lut_m[BAYERNR_LMT_LUT_NUM];
    td_u16  u16noise_sd_lut_s[BAYERNR_LMT_LUT_NUM];
    td_u32  u32noise_inv_lut[BAYERNR_LMT_LUT_NUM];
    td_u32  u32noise_inv_lut_m[BAYERNR_LMT_LUT_NUM];
    td_u8   u8noise_inv_s_mag_idx[BAYERNR_LMT_LUT_NUM];
    td_u8   u8noise_inv_m_mag_idx[BAYERNR_LMT_LUT_NUM];
    td_u8   u8fb_ratio_table[OT_ISP_BAYERNR_FBRTO_LUT_SIZE];

    td_u16  jnlm_coring_low_lut[OT_ISP_BAYERNR_LUT_LENGTH];
    td_u32  jnlm_limit_mult_gain[OT_ISP_BAYER_CHN_NUM];
    td_u32  jnlm_limit_mult_gain_s[OT_ISP_BAYER_CHN_NUM];
} isp_bayernr_dyna_cfg;

typedef struct {
    td_bool resh;
} isp_bayernr_usr_cfg;

typedef struct {
    td_bool bnr_enable;
    isp_bayernr_static_cfg    static_reg_cfg;
    isp_bayernr_dyna_cfg      dyna_reg_cfg;
    isp_bayernr_usr_cfg       usr_reg_cfg;
} isp_bayernr_reg_cfg;

typedef struct {
    td_bool nr_en;
    td_bool tnr_en;
} isp_bnr_temporal_filt;

typedef struct {
    td_bool sharpen_mot_en;
    isp_bnr_temporal_filt cur;
    isp_bnr_temporal_filt pre;
} isp_kernel_tpr_filt_reg;

typedef struct {
    td_bool enable;
    td_bool calc_drc_enable;
    td_bool stat_drc_enable;
    td_bool de_enable;
    td_bool calc_de_ref;
    td_bool calc_pf_lmt_enable;
    td_bool pre_drc_lut_update;
    td_bool static_resh;

    td_u32  calc_luma_sel;
    td_u32  deh_lpf_sft;
    td_s16  deh_lpf_coef_h[LDCI_DE_LPF_H_COEF_NUM];
    td_s16  deh_lpf_coef_v[LDCI_DE_LPF_V_COEF_NUM];
    td_u32  calc_pfmul;
    td_u32  calc_pf_ori[LDCI_PF_PARA_NUM];
    td_s16  calc_pf_sft[LDCI_PF_SFT_NUM];
    td_u32  calc_pf_thd[LDCI_PF_PARA_NUM];
    td_u32  calc_pf_rly[LDCI_PF_PARA_NUM];
    td_u32  global_he_wgt;
    td_u32  lpf_sft;
    td_u32  chr_pos_damp;
    td_u32  chr_neg_damp;
    td_s16  stat_poly_p2[LDCI_POLY_LUT_NUM];
    td_s16  stat_poly_p3[LDCI_POLY_LUT_NUM];
    td_s16  poly_p1[LDCI_POLY_LUT_NUM];
    td_s16  poly_q0[LDCI_POLY_LUT_NUM];
    td_s16  poly_q1[LDCI_POLY_LUT_NUM];
    td_s16  poly_q2[LDCI_POLY_LUT_NUM];
    td_s16  poly_q3[LDCI_POLY_LUT_NUM];
    td_s16  stat_dyn_rng_cmp_lut[LDCI_POLY_LUT_NUM];
    td_s16  calc_dyn_rng_cmp_lut[LDCI_POLY_LUT_NUM];
} isp_ldci_static_cfg;

typedef struct {
    td_bool enable;
    td_bool wrstat_en;
    td_bool rdstat_en;
    td_bool calc_enable;
    td_bool pre_calc_lut_reg_new;
    td_bool calc_lut_update;
    td_bool lut2_stt_en;
    td_u8   buf_id;

    td_u32  calc_blc_ctrl;
    td_u32  he_pos_lut[LDCI_HE_LUT_SIZE];
    td_u32  he_neg_lut[LDCI_HE_LUT_SIZE];
    td_u32  usm_pos_lut[LDCI_DE_USM_LUT_SIZE];
    td_u32  usm_neg_lut[LDCI_DE_USM_LUT_SIZE];
    td_u32  color_gain_lut[LDCI_COLOR_GAIN_LUT_SIZE];
    td_u32  de_lut[LDCI_DE_USM_LUT_SIZE];
    td_u32  lpf_coef[LDCI_LPF_LUT_SIZE];

    td_u32  calc_map_offset_x;
    td_u32  calc_sml_map_stride;
    td_u32  calc_sml_map_height;
    td_u32  calc_total_zone;
    td_u32  calc_scale_x;
    td_u32  calc_scale_y;
    td_u32  blk_sml_map_width[LDCI_SM_WIDTH_NUM];
    td_u32  stat_v_start;
    td_u32  stat_v_end;
    td_u32  stat_h_start;
    td_u32  stat_h_end;
    td_u32  stat_sml_map_width;
    td_u32  stat_sml_map_height;
    td_u32  stat_total_zone;
    isp_ldci_stat tpr_stat;
} isp_ldci_dyna_cfg;

typedef struct {
    isp_ldci_static_cfg static_reg_cfg;
    isp_ldci_dyna_cfg   dyna_reg_cfg;
} isp_ldci_reg_cfg;

typedef struct {
} isp_fpn_static_cfg;

typedef struct {
    td_bool isp_fpn_enable;

    td_u32  isp_fpn_line_frame;
    td_u32  isp_fpn_calib_corr;
    td_u32  isp_fpn_frame_calib_shift;
    td_u32  isp_fpn_out_shift;
    td_u32  isp_fpn_in_shift;
    td_u32  isp_fpn_shift;
    td_u32  isp_fpn_max_o;
    td_u32  isp_fpn_overflow_thr;
    td_u32  isp_fpn_offset[FPN_CHN_NUM];
    td_u32  isp_fpn_strength[FPN_CHN_NUM];
    td_u32  isp_fpn_correct_en_id[FPN_CHN_NUM];
} isp_fpn_dyna_cfg;

typedef struct {
    td_bool fpn_cor_en;
    td_bool hnr_en;
    td_bool shift_en; /* for 16BPP, shift to 12bit offset, others do not shift */
    td_u32  strength;
    td_u16  black_frame_offset[FPN_CHN_NUM];
    td_u16  add_offset[FPN_CHN_NUM];
} isp_fpn_sync_cfg;

typedef struct {
    isp_fpn_static_cfg static_reg_cfg;
    isp_fpn_dyna_cfg   dyna_reg_cfg;
    isp_fpn_sync_cfg   sync_cfg;
} isp_fpn_reg_cfg;

typedef struct {
    td_bool  resh;
    td_u32   size;
} isp_clut_static_cfg;

typedef struct {
    td_bool resh;
    td_u8   buf_id;
    td_u32  update_index;
    td_u32  lu_stt[OT_ISP_CLUT_LUT_LENGTH];
} isp_clut_usr_coef_cfg;

typedef struct {
    td_bool resh;
    td_bool enable;
    /* 0-debug closed; 1-debug open, the left screen is original video and the right screen is CLUT-processed video */
    td_bool demo_enable;
    td_bool demo_mode;
    td_u32  demo_pos;
    td_u32  gain_r; /* [0,512] */
    td_u32  gain_g; /* [0,512] */
    td_u32  gain_b; /* [0,512] */
} isp_clut_usr_ctrl_cfg;

typedef struct {
    td_bool static_resh;
    td_bool dynablc_dpstat_en;
    td_u16  dynablc_raw_array;
} isp_dynamicblc_static_cfg;

typedef struct {
    td_bool dyna_resh;
    td_u16  dynablc_value0;
    td_u16  dynablc_value1;
    td_u16  dynablc_value2;
    td_u16  dynablc_value3;
} isp_dynamicblc_dyna_cfg;

typedef struct {
    td_bool usr_resh;
    td_u16  dynablc_pattern;
    td_u16  dynablc_start_ypos;
    td_u16  dynablc_start_xpos;
    td_u16  dynablc_end_ypos;
    td_u16  dynablc_end_xpos;
    td_u16  dynablc_low_threshold;
    td_u16  dynablc_high_threshold;
} isp_dynamicblc_usr_cfg;

typedef struct {
    td_bool wdr_en;
    td_bool user_black_level_en;
    ot_isp_black_level_mode black_level_mode;
    td_bool separate_en;
    td_u8   filter_strength;
    td_s16  offset;
    td_u16  tolerance;
    td_s16  ag_offset;
    td_u16  user_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];
    td_u16  filter_thr;
} isp_dynamic_blc_sync_cfg;

typedef struct {
    td_bool    dynablc_enable;
    isp_dynamicblc_static_cfg static_reg_cfg;
    isp_dynamicblc_dyna_cfg   dyna_reg_cfg;
    isp_dynamicblc_usr_cfg    usr_reg_cfg;
    isp_dynamic_blc_sync_cfg  sync_cfg;
} isp_dynamic_blc_regcfg;

typedef struct {
    isp_clut_usr_coef_cfg     clut_usr_coef_cfg;
    isp_clut_usr_ctrl_cfg     clut_usr_ctrl_cfg;
} isp_clut_usr_cfg;

typedef struct {
    td_bool enable;
    isp_clut_static_cfg    static_reg_cfg;
    isp_clut_usr_cfg       usr_reg_cfg;
} isp_clut_reg_cfg;

typedef struct {
    td_bool  static_resh;
} isp_gamma_static_cfg;

typedef struct {
    td_bool resh;
    td_bool gamma_lut_update_en;
    td_bool switch_lut2_stt_reg_new;
    td_bool switch_mode;
    td_u8   switch_reg_new_cnt;
    td_u8   buf_id;
    td_u16  gamma_lut[GAMMA_REG_NODE_NUM];  /* u12.0 */
    td_u8   gamma_step[GAMMA_OUTSEG_NUM];
    td_u16  gamma_in_seg[GAMMA_OUTSEG_NUM];
    td_u16  gamma_pos[GAMMA_OUTSEG_NUM];
    td_u32  update_index;
} isp_gamma_usr_cfg;

typedef struct {
    td_bool gamma_en;
    td_bool lut2_stt_en;
    isp_gamma_static_cfg static_reg_cfg;
    isp_gamma_usr_cfg    usr_reg_cfg;
} isp_gamma_reg_cfg;

typedef struct {
    td_bool static_resh;
    td_u8   thres_v;
    td_u8   thres_h;
    td_u16  crosstalk_lut[OT_ISP_RGBIR_LUT_NUM];
} isp_rgbir_static_cfg;

typedef struct {
    td_bool usr_resh;
    td_bool smooth_enable;
    td_u8   in_pattern;
    td_u8   out_pattern;
    td_u8   in_bayer_pattern;

    td_u16  exp_ctrl1;
    td_u16  exp_ctrl2;
    td_u16  reci_exp1;
    td_u16  reci_exp2;

    td_u16  gain_r;
    td_u16  gain_b;

    td_u16  thd_alpha;
    td_u16  thd_beta;
    td_s16  cvt_matrix[OT_ISP_RGBIR_CVTMAT_NUM];

    td_u32  update_index;
    td_u8   mode; /* 0: RGBIR mode; 1: Ir remove mode */
    td_u8   out_type;
    td_bool ir_rm_enable;
    td_u32  ir_rm_ratio[OT_ISP_RGBIR_CROSSTALK_NUM];
    td_u32  ir_sum;

    td_u16  wb_ctrl[OT_ISP_RGBIR_CTRL_NUM];
    td_bool is_ir_upscale;
} isp_rgbir_usr_cfg;

typedef struct {
    td_bool dyna_resh;
    td_bool auto_gain_enable;
    td_u16  auto_gain;
    td_u16  wb_gain[OT_ISP_RGBIR_CTRL_NUM];
} isp_rgbir_dyna_cfg;

typedef struct {
    td_bool rgbir_enable;
    isp_rgbir_static_cfg static_reg_cfg;
    isp_rgbir_usr_cfg    usr_reg_cfg;
    isp_rgbir_dyna_cfg   dyna_reg_cfg;
} isp_rgbir_reg_cfg;

typedef struct {
    /* multiplier coefficients */
    td_bool resh;
    td_s16  csc_coef[OT_ISP_CSC_COEF_NUM];  /* s5.10 */
    /* DC components */
    td_s16  csc_in_dc[OT_ISP_CSC_DC_NUM];  /* s11.0 */
    td_s16  csc_out_dc[OT_ISP_CSC_DC_NUM]; /* s11.0 */
} isp_csc_dyna_cfg;

typedef struct {
    td_bool enable;       /* u1.0 */
    isp_csc_dyna_cfg    dyna_reg_cfg;
} isp_csc_reg_cfg;

typedef struct {
    td_bool blc_in;
    td_bool blc_out;
} isp_blc_static_cfg;

typedef struct {
    td_u16 blc[OT_ISP_BAYER_CHN_NUM];
    td_u16 out_blc;
} isp_blc_dyna_cfg;

typedef struct {
    td_bool resh_static;
    isp_blc_static_cfg lsc_blc;
    isp_blc_static_cfg dg_blc;
    isp_blc_static_cfg wdr_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_static_cfg expander_blc;
    isp_blc_static_cfg wb_blc;
    isp_blc_static_cfg wdr_dg_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_static_cfg ge_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_static_cfg ae_blc;
    isp_blc_static_cfg mg_blc;
    isp_blc_static_cfg af_blc;
    isp_blc_static_cfg flicker_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_static_cfg bnr_blc;
    isp_blc_static_cfg rgbir_blc;
} isp_be_blc_static_cfg;

typedef struct {
    td_bool resh_dyna;
    ot_isp_black_level_mode black_level_mode;
    isp_blc_dyna_cfg lsc_blc;
    isp_blc_dyna_cfg dg_blc;
    isp_blc_dyna_cfg wdr_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg expander_blc;
    isp_blc_dyna_cfg wb_blc;
    isp_blc_dyna_cfg wdr_dg_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg ge_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg ae_blc;
    isp_blc_dyna_cfg mg_blc;
    isp_blc_dyna_cfg af_blc;
    isp_blc_dyna_cfg flicker_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg bnr_blc;
    isp_blc_dyna_cfg rgbir_blc;
    isp_blc_dyna_cfg raw_blc;
} isp_be_blc_dyna_cfg;

typedef struct {
    td_bool resh_dyna_init;
    isp_be_blc_static_cfg static_blc;
    isp_be_blc_dyna_cfg   dyna_blc;
} isp_be_blc_cfg;

typedef struct {
    td_bool resh_static;
    /* FE module */
    isp_blc_static_cfg fe_dg_blc;
    isp_blc_static_cfg fe_wb_blc;
    isp_blc_static_cfg fe_ae_blc;
    isp_blc_static_cfg fe_af_blc;
    isp_blc_static_cfg fe_blc;
} isp_fe_blc_static_cfg;

typedef struct {
    td_bool resh_dyna;
    /* FE module */
    isp_blc_dyna_cfg fe_dg_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg fe_wb_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg fe_ae_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg fe_af_blc[OT_ISP_WDR_MAX_FRAME_NUM];
    isp_blc_dyna_cfg fe_blc[OT_ISP_WDR_MAX_FRAME_NUM];
} isp_fe_blc_dyna_cfg;

typedef struct {
    td_bool resh_dyna_init;
    isp_fe_blc_static_cfg static_blc;
    isp_fe_blc_dyna_cfg   dyna_blc;
} isp_fe_blc_cfg;

typedef struct {
    td_bool ae1_lut_update;
} isp_fe_lut_update_reg_cfg;

typedef struct {
    td_bool ae_lut_update;
    td_bool drc_tm_lut_update;
    td_bool lcac_lut_update;
    td_bool nddm_gf_lut_update;
    td_bool ldci_drc_lut_update;
    td_bool ldci_calc_lut_update;
    td_bool lsc_lut_update;
    td_bool bnr_lut_update;
    td_bool de_lut_update;
    td_bool ca_lut_update;
    td_bool bshp_lut_update;
} isp_be_lut_update_reg_cfg;

typedef struct {
    td_bool gamma_stt2lut_regnew;
    td_bool clut_stt2lut_regnew;
    td_bool bnr_lsc_stt2lut_regnew;
    td_bool lsc_stt2lut_regnew;
    td_bool dpc_stt2lut_regnew;
    td_bool dpc1_stt2lut_regnew;
    td_bool pre_gamma_stt2lut_regnew;
    td_bool bshp_stt2lut_regnew;

    /* pingpong */
    td_bool dehaze_stt2lut_regnew;    /* every frame */
    td_bool ldci_stt2lut_regnew;      /* every frame */
    td_bool sharpen_stt2lut_regnew;   /* every frame */
} isp_be_stt2lut_regnew_reg_cfg;

typedef struct {
    td_u16  air_r;
    td_u16  air_g;
    td_u16  air_b;
    td_u8   strength;
} isp_dehaze_lut2_stt_sync_reg_cfg;

typedef struct {
    td_bool resh;
    td_u8   mesh_scale;
} isp_lsc_lut2stt_sync_reg_cfg;

typedef struct {
    td_bool        resh;
    td_u32          gain_r;
    td_u32          gain_g;
    td_u32          gain_b;
} isp_clut_lut2stt_sync_reg_cfg;

typedef struct {
    td_u32    calc_blc_ctrl;
    td_u32    lpf_coef[LDCI_LPF_LUT_SIZE];
} isp_ldci_lut2stt_sync_reg_cfg;

typedef struct {
    isp_sharpen_mpi_dyna_reg_cfg          mpi_dyna_reg_cfg;
    isp_sharpen_default_dyna_reg_cfg      default_dyna_reg_cfg;
} isp_sharpen_lut2stt_sync_reg_cfg;

typedef struct {
    td_u16  ca_luma_thr_high;
    td_u16  ca_luma_ratio_high;
    td_u16  ca_iso_ratio;
} isp_ca_lut2stt_sync_reg_cfg;

typedef struct {
    td_bool nry0_en;
    td_bool ae_en;
    td_bool la_en;
    td_bool awb_en;
    td_bool awblsc_en;
    td_bool wb_en;
    td_bool ccm_en;
    td_bool ccm_color_tone_en;
    td_bool af_en;
    td_bool sharpen_en;
    td_bool print_sel_en;
    td_bool vhdm_en;
    td_bool nddm_en;
    td_bool ldci_en;
    td_bool lcac_en;
    td_bool acac_en;
    td_bool bshp_en;
    td_bool fcr_en;
    td_bool dpc_en[ISP_DPC_MAX_CHN_NUM];
    td_bool ge_en[GE_MAX_CHN_NUM];
    td_bool lsc_en;
    td_bool gamma_en;
    td_bool csc_en;
    td_bool ca_en;
    td_bool wdr_en;
    td_bool drc_en;
    td_bool dehaze_en;
    td_bool bnr_en;
    td_u8   bnr_lsc_en;
    td_bool dg_en;
    td_bool four_dg_en;
    td_bool pregamma_en;
    td_bool clut_en;
    td_bool wdr_split_en;
    td_bool de_en;
    td_bool expander_en;
    td_bool rgbir_en;
    td_bool crb_en;
    isp_dehaze_lut2_stt_sync_reg_cfg dehaze_sync_cfg;
    isp_lsc_lut2stt_sync_reg_cfg    lsc_sync_cfg;
    isp_clut_lut2stt_sync_reg_cfg   clut_sync_cfg;
    isp_ldci_lut2stt_sync_reg_cfg   ldci_sync_cfg;
    isp_sharpen_lut2stt_sync_reg_cfg  sharpen_sync_cfg;
    isp_ca_lut2stt_sync_reg_cfg ca_sync_cfg;
} isp_lut2stt_sync_reg_cfg;

typedef union {
    td_u32  key;
    struct {
        td_u32  bit1_awb_cfg     : 1;   /* [0] */
        td_u32 bit1_be_af_cfg : 1; /* [1] */
        td_u32 bit30_rsv : 30; /* [2:31] */
    };
} isp_reg_kernel_cfg_key;

typedef  struct {
    td_u32 column;
    td_u32 row;
    td_u32 zone_bin;
} awb_zone_cfg;

typedef struct {
    td_u32 column;
    td_u32 row;
} af_zone_cfg;

typedef struct {
    awb_zone_cfg              be_awb_zone_cfg;
    awb_zone_cfg              be_awblsc_zone_cfg;
    af_zone_cfg               be_af_zone_cfg;
} isp_reg_kernel_cfg;

typedef struct {
    isp_reg_kernel_cfg_key    cfg_key;
    isp_reg_kernel_cfg        alg_kernel_cfg[OT_ISP_STRIPING_MAX_NUM];
} isp_kernel_reg_cfg;

typedef union {
    td_u64  key;
    struct {
        td_u64  bit1_ae_cfg1      : 1;   /* [0] */
        td_u64  bit1_ae_cfg2      : 1;   /* [1] */
        td_u64  bit1_awb_dyn_cfg  : 1;   /* [2] */
        td_u64  bit1_awb_cfg      : 1;   /* [3] */
        td_u64  bit1_af_fe_cfg    : 1;    /* [4] */
        td_u64  bit1_drc_cfg      : 1;   /* [5] */
        td_u64  bit1_dp_cfg       : 1;   /* [6] */
        td_u64  bit1_sharpen_cfg  : 1;   /* [7] */
        td_u64  bit1_ge_cfg       : 1;   /* [8] */
        td_u64  bit1_wdr_cfg      : 1;   /* [9] */
        td_u64  bit1_dehaze_cfg   : 1;   /* [10] */
        td_u64  bit1_awb_cfg3     : 1;   /* [11] */
        td_u64  bit1_lsc_cfg      : 1;   /* [12] */
        td_u64  bit1_dem_cfg      : 1;   /* [13] */
        td_u64  bit1_fcr_cfg       : 1;   /* [14] */
        td_u64  bit1_mcds_cfg      : 1;   /* [15] */
        td_u64  bit1_rc_cfg        : 1;   /* [16] */
        td_u64  bit1_fs_wdr_cfg    : 1;   /* [17] */
        td_u64  bit1_bayernr_cfg   : 1;   /* [18] */
        td_u64  bit1_flicker_cfg   : 1;   /* [19] */
        td_u64  bit1_ca_cfg        : 1;   /* [20] */
        td_u64  bit1_nddm_cfg      : 1;   /* [21] */
        td_u64  bit1_local_cac_cfg : 1;   /* [22] */
        td_u64  bit1_acac_cfg      : 1;   /* [23] */
        td_u64  bit1_dci_cfg       : 1;   /* [24] */
        td_u64  bit1_ldci_cfg      : 1;   /* [25] */
        td_u64  bit1_csc_cfg       : 1;   /* [26] */
        td_u64  bit1_gamma_cfg     : 1;   /* [27] */
        td_u64  bit1_pregamma_cfg  : 1;   /* [28] */
        td_u64  bit1_split_cfg     : 1;   /* [29] */
        td_u64  bit1_hrs_cfg       : 1;   /* [30] */
        td_u64  bit1_dg_cfg        : 1;   /* [31] */
        td_u64  bit1_wdr_dg_cfg    : 1;   /* [32] */
        td_u64  bit1_fe_dg_cfg    : 1;   /* [33] */
        td_u64  bit1_af_be_cfg    : 1;    /* [34] */
        td_u64  bit1_fe_blc_cfg   : 1;   /* [35] */
        td_u64  bit1_be_blc_cfg   : 1;   /* [36] */
        td_u64  bit1_clut_cfg     : 1;   /* [37] */
        td_u64  bit1_rlsc_cfg     : 1;   /* [38] */
        td_u64  bit1_fpn_cfg      : 1;   /* [39] */
        td_u64  bit1_expander_cfg : 1;   /* [40] */
        td_u64  bit1_awblsc_cfg   : 1;   /* [41] */
        td_u64  bit1_dp1_cfg      : 1;   /* [42] */
        td_u64  bit1_rgbir_cfg    : 1;   /* [43] */
        td_u64  bit1_dynablc_cfg  : 1;   /* [44] */
        td_u64  bit1_bshp_cfg     : 1;   /* [45] */
        td_u64  bit1_crb_cfg      : 1;   /* [46] */
        td_u64  bit1_agamma_cfg   : 1;   /* [47] */
        td_u64  bit1_adgamma_cfg  : 1;   /* [48] */
        td_u64  bit15_rsv         : 15;   /* [49:63] */
    };
} isp_reg_cfg_key;

typedef struct {
    isp_ae_reg_cfg_2             ae_reg_cfg2;
    isp_ae_reg_cfg               ae_reg_cfg;
    isp_mg_reg_cfg               mg_reg_cfg;
    isp_dg_reg_cfg               dg_reg_cfg;
    isp_4dg_reg_cfg              four_dg_reg_cfg;
    isp_fe_dg_reg_cfg            fe_dg_reg_cfg;
    isp_hrs_reg_cfg              hrs_reg_cfg;
    isp_awb_reg_cfg              awb_reg_cfg;
    isp_awblsc_reg_cfg           awblsc_reg_cfg;

    isp_af_reg_cfg               fe_af_reg_cfg;
    isp_af_reg_cfg               be_af_reg_cfg;
    isp_dpc_reg_cfg              dp_reg_cfg;
    isp_drc_reg_cfg              drc_reg_cfg;
    isp_pregamma_reg_cfg         pregamma_reg_cfg;
    isp_ge_reg_cfg               ge_reg_cfg;
    isp_sharpen_reg_cfg          sharpen_reg_cfg;
    isp_wdr_reg_cfg              wdr_reg_cfg;
    isp_dehaze_reg_cfg           dehaze_reg_cfg;
    isp_mcds_reg_cfg             mcds_reg_cfg;
    isp_radial_crop_reg_cfg      rc_reg_cfg;
    isp_lsc_reg_cfg              lsc_reg_cfg;
    isp_rlsc_reg_cfg             rlsc_reg_cfg;
    isp_demosaic_reg_cfg         dem_reg_cfg;
    isp_antifalsecolor_reg_cfg   anti_false_color_reg_cfg;
    isp_bayernr_reg_cfg          bnr_reg_cfg;
    isp_flicker_reg_cfg          flicker_reg_cfg;
    isp_expander_reg_cfg         expander_cfg;
    isp_ca_reg_cfg               ca_reg_cfg;
    isp_lcac_reg_cfg             lcac_reg_cfg;
    isp_acac_reg_cfg             acac_reg_cfg;
    isp_bshp_reg_cfg             bshp_reg_cfg;
    isp_ldci_reg_cfg             ldci_reg_cfg;
    isp_fpn_reg_cfg              fpn_reg_cfg;
    isp_be_blc_cfg               be_blc_cfg;
    isp_fe_blc_cfg               fe_blc_cfg;
    isp_csc_reg_cfg              csc_cfg;
    isp_agamma_reg_cfg           agamma_reg_cfg;
    isp_adgamma_reg_cfg          adgamma_reg_cfg;
    isp_gamma_reg_cfg            gamma_cfg;
    isp_rgbir_reg_cfg            rgbir_reg_cfg;
    isp_clut_reg_cfg             clut_cfg;
    isp_crb_reg_cfg              crb_reg_cfg;
    isp_fe_lut_update_reg_cfg    fe_lut_update_cfg;
    isp_be_lut_update_reg_cfg    be_lut_update_cfg;
    isp_lut2stt_sync_reg_cfg     lut2stt_sync_cfg[ISP_LUT2STT_SYNC_BUF_MAX_NUM];
    isp_be_stt2lut_regnew_reg_cfg stt2lut_regnew_cfg;
    isp_dynamic_blc_regcfg       dynamic_blc_reg_cfg;
} isp_alg_reg_cfg;

typedef struct {
    td_u8 cfg_num;
    isp_reg_cfg_key       cfg_key;
    isp_alg_reg_cfg       alg_reg_cfg[OT_ISP_STRIPING_MAX_NUM];
    isp_kernel_reg_cfg    kernel_reg_cfg;
} isp_reg_cfg;

typedef struct {
    isp_be_all_reg_type   be_reg_cfg[OT_ISP_STRIPING_MAX_NUM];
} isp_be_wo_reg_cfg;

typedef struct {
    isp_be_online_stat_reg_type be_online_stt_reg[ISP_MAX_BE_NUM];
} isp_be_rw_online_stt_reg;

typedef struct {
    isp_dpc_reg_cfg       dp_reg_cfg;
} isp_ro_reg_cfg;

typedef struct {
    td_phys_addr_t  phy_addr;
    td_u64  size;
    td_void ATTRIBUTE *vir_addr;
} isp_mmz_buf_ex;

typedef struct {
    td_phys_addr_t  phy_addr;
    td_u64  size;
    td_void ATTRIBUTE *vir_addr;

    td_phys_addr_t  pre_phy_addr[OT_ISP_STRIPING_MAX_NUM];
    td_u64  pre_size;
    td_void ATTRIBUTE *pre_vir_addr[OT_ISP_STRIPING_MAX_NUM];

    td_phys_addr_t  post_phy_addr[OT_ISP_STRIPING_MAX_NUM];
    td_u64  post_size;
    td_void ATTRIBUTE *post_vir_addr[OT_ISP_STRIPING_MAX_NUM];

    td_void ATTRIBUTE *pre_viproc_vir_addr[OT_ISP_STRIPING_MAX_NUM];
    td_void ATTRIBUTE *post_viproc_vir_addr[OT_ISP_STRIPING_MAX_NUM];
} isp_be_wo_cfg_buf;

typedef struct {
    td_bool             init;
    td_s32              use_cnt;
    isp_mmz_buf_ex      be_buf_haddr;
} isp_be_cfg_buf_info;

typedef struct {
    td_u8   block_num;
    td_u8   block_dev;
} isp_be_blk_attr;

#define PING_PONG_NUM    2

typedef struct {
    isp_mmz_buf_ex lut_stt_buf[PING_PONG_NUM];   /* ping pong be lut stt info */
} isp_be_lut2stt_buf;

typedef struct {
    td_bool            init;
    isp_be_lut2stt_buf be_lut_stt_buf[ISP_MAX_BE_NUM];
} isp_be_lut2stt_attr;

typedef struct {
    td_bool init;
    td_bool get_first_stat_info_flag;
    td_u32  cur_write_flag;
    isp_mmz_buf_ex be_stt_buf[PING_PONG_NUM];   /* ping pong be online stt info */
} isp_be_online_stt_info;

typedef struct {
    td_bool first_frame;
    td_u32  cur_write_flag;
    isp_mmz_buf_ex fe_stt_buf[PING_PONG_NUM];   /* ping pong fe stt info */
} isp_fe_stt_info;

typedef struct {
    td_bool         init;
    isp_fe_stt_info fe_stt[ISP_WDR_CHN_MAX];
} isp_fe_stt_attr;

typedef struct {
    td_bool                init;
    isp_fe_stat_reg_type  *save_stt_stat[OT_ISP_MAX_STITCH_NUM][ISP_WDR_CHN_MAX];
    isp_fe_stt_info        fe_stt[OT_ISP_MAX_STITCH_NUM][ISP_WDR_CHN_MAX];
} isp_fe_stit_stt_attr;

/* offline be stt stats switch in isp */
typedef struct {
    td_phys_addr_t            phys_addr[OT_ISP_STRIPING_MAX_NUM];
    td_void ATTRIBUTE *virt_addr[OT_ISP_STRIPING_MAX_NUM];
} isp_be_offline_stt_buf;

typedef struct {
    td_bool                init;
    td_s32                 working_be_stt_idx[VI_PROC_IP_NUM];
    td_s32                 last_ready_be_stt_idx[VI_PROC_IP_NUM];
    td_u64                 pts[VI_PROC_IP_NUM][PING_PONG_NUM];
    isp_be_offline_stt_buf be_stt_buf[PING_PONG_NUM];
} isp_be_offline_stt_attr;

typedef struct {
    td_bool finish[MAX_ISP_STITCH_STAT_BUF_NUM];
    td_u64  pts[MAX_ISP_STITCH_STAT_BUF_NUM];
    td_u64  phys_addr[MAX_ISP_STITCH_STAT_BUF_NUM];
    isp_stitch_stat_reg_type *stitch_stt_reg[MAX_ISP_STITCH_STAT_BUF_NUM];
} isp_drv_stitch_be_stt_buf;

/* only for post_be(AE/AWB) */
typedef struct {
    td_bool init;
    td_bool is_ready[2]; /* 2 flag to roll */
    td_bool lost_frame[OT_ISP_MAX_STITCH_NUM];
    td_s32  sync_ok_index[2][OT_ISP_MAX_STITCH_NUM]; /* 2 flag to roll */
    td_s32  working_idx;
    isp_drv_stitch_be_stt_buf sync_be_buf[OT_ISP_MAX_STITCH_NUM];
} isp_be_stitch_stt_attr;

typedef struct {
    isp_stitch_stat_reg_type *sync_be_stt[OT_ISP_MAX_STITCH_NUM];
} isp_stitch_sync_be_stats;

typedef struct {
    td_bool         init;
    td_u8           buf_idx;
    td_u8           buf_num;
    isp_mmz_buf_ex  ldci_buf[OT_ISP_BE_BUF_NUM_MAX];
} isp_ldci_buf;

typedef struct {
    isp_ldci_stat ldci_stat;
} isp_ldci_stt_addr;

typedef struct {
    td_u8  buf_num;
    td_u8  buf_idx;
    td_u64 head_phy_addr;
    isp_mmz_buf_ex read_buf[OT_ISP_BE_BUF_NUM_MAX];
} isp_ldci_read_stt_buf;

typedef struct {
    td_bool         init;
    isp_mmz_buf_ex  clut_buf;
} isp_clut_buf;

typedef struct {
    td_bool         init;
    isp_mmz_buf_ex  spec_awb_buf;
} isp_spec_awb_buf;

typedef struct {
    td_bool         init;
    isp_mmz_buf_ex  drc_buf;
} isp_drc_buf;

typedef struct {
    td_bool         init;
    td_u8           cur_write_idx;
    isp_mmz_buf_ex  fswdr_buf[PING_PONG_NUM];
} isp_fswdr_buf;

#define ISP_SYNC_BUF_NODE_NUM           8
#define CFG2VLD_DLY_LIMIT               8
#define WDR_EXP_RATIO_SHIFT             6
#define DRC_COMP_SHIFT                  12

#define ISP_MAX_UPDATEINFO_BUF_NUM      (CFG2VLD_DLY_LIMIT)
#define ISP_MAX_FRAMEINFO_BUF_NUM       (CFG2VLD_DLY_LIMIT)
#define ISP_SAVEINFO_MAX                (CFG2VLD_DLY_LIMIT)
#define ISP_INTERRUPTS_SAVEINFO_MAX     (OT_ISP_MAX_PHY_PIPE_NUM * 4)
#define DYNAMIC_BLC_BUF_NUM              3
typedef struct {
    td_bool is_ready;
    td_u16  isp_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* Note:14bits */
    td_u16  sns_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* Note:14bits */
} isp_blc_actual_info;

typedef struct {
    td_bool ob_stats_read_en; /* Indicate ob statistics is ready, can run dynamic_blc_preocess */
    td_bool is_first_frame; /* Indicate first frame of switching to dynamic from auto/manual */
    ot_isp_black_level_mode  pre_black_level_mode; /* record last black_level_mode */
    td_bool fe_blc_mode_change; /* flag to indicate switch dynamic to auto/manual */
    td_bool pre_be_blc_mode_change;
    td_bool post_be_blc_mode_change;
    td_s8   fe_delay_cnt; /* switch dynamic to auto/manual, wait fe_delay_cnt to update fe_blc reg
                             use newest manual/auto cfg */
    td_s8   pre_be_delay_cnt;
    td_s8   post_be_delay_cnt;
    isp_dynamic_blc_sync_cfg sync_cfg; /* dynamic mpi cfg */
    isp_fpn_sync_cfg    fpn_cfg[DYNAMIC_BLC_BUF_NUM];
    td_u16  black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];
    td_u16  isp_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];
    td_u16  diff_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* sns - isp */
    td_u16  last_filt_res[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];
    isp_blc_actual_info actual_info;
    isp_be_blc_dyna_cfg be_blc_sync[DYNAMIC_BLC_BUF_NUM]; /* special for pre_online post_offline */
} isp_dynamic_blc_info;

typedef struct {
    td_bool valid;

    ot_isp_sns_regs_info sns_regs_info;
    isp_ae_reg_cfg_2     ae_reg_cfg;
    isp_fswdr_sync_cfg   wdr_reg_cfg;
    isp_drc_sync_cfg     drc_reg_cfg;
    isp_awb_reg_cfg_2    awb_reg_cfg;
    isp_awb_reg_cfg_2    awb_reg_cfg_stitch[OT_ISP_MAX_PIPE_NUM];
    isp_fe_blc_dyna_cfg  fe_blc_reg_cfg;
    isp_be_blc_dyna_cfg  be_blc_reg_cfg;
    isp_fe_blc_dyna_cfg  fe_blc_reg_cfg_stitch[OT_ISP_MAX_PIPE_NUM];
    isp_be_blc_dyna_cfg  be_blc_reg_cfg_stitch[OT_ISP_MAX_PIPE_NUM];
    isp_dynamic_blc_sync_cfg dynamic_blc_cfg;
    isp_fpn_sync_cfg         fpn_cfg;
} isp_sync_cfg_buf_node;

typedef struct {
    td_bool init;
    isp_reg_cfg reg_cfg;
    isp_sync_cfg_buf_node sync_cfg_node;
} isp_reg_cfg_attr;

#define ISP_CHN_SWITCH_NUM 5
typedef struct {
    td_u32  channel_sel;
    td_u32  wdr_chn_sel[ISP_CHN_SWITCH_NUM];
} isp_chn_switch;

typedef struct {
    td_u8 wdr_mode;
    td_u32 exp_ratio[OT_ISP_EXP_RATIO_NUM];
    isp_fswdr_sync_cfg wdr_reg_cfg;
} isp_wdr_cfg;

typedef struct {
    td_u16   width;
    td_u16   height;
    td_float fps;
    td_u8    sns_mode;
} isp_res_switch_mode;

typedef struct {
    td_bool init;
    td_u32  size;
    td_phys_addr_t  phy_addr;
    td_void *virt_addr;
} isp_proc_mem;

typedef struct {
    td_u64  phy_addr;
    td_void *vir_addr;
} isp_mmz_buf;

typedef struct {
    td_bool     init;
    isp_mmz_buf dng_info;
    isp_mmz_buf atta_info;
    isp_mmz_buf color_gammut_info;
    isp_mmz_buf frame_info;
    isp_mmz_buf update_info;
} isp_trans_info_buf;

typedef struct {
    td_bool     init;
    isp_mmz_buf pro_nr_param_info;
    isp_mmz_buf pro_shp_param_info;
} isp_pro_info_buf;

#ifdef CONFIG_OT_SNAP_SUPPORT
typedef struct {
    ot_op_mode operation_mode;
    td_u32 pro_frame_num;
    ot_snap_pro_auto_param   auto_param;
    ot_snap_pro_manual_param manual_param;
} isp_pro_param;

/*
 * 0 = default mode:no snap
 * 1 = only preview
 * 2 = only picture
 * 3 = preview and picture is the same pipe
 */
typedef enum {
    ISP_SNAP_NONE            = 0,
    ISP_SNAP_PREVIEW,
    ISP_SNAP_PICTURE,
    ISP_SNAP_PREVIEW_PICTURE,
    ISP_SNAP_BUTT
} isp_snap_pipe_mode;

typedef struct {
    ot_snap_type snap_type;
    td_s32 preview_pipe_id; /* -1,none snap */
    td_s32 picture_pipe_id; /* -1,none snap */
    isp_pro_param pro_param;
    td_bool load_ccm;    /* TD_TRUE:use CCM of snap_isp_info, TD_FALSE: algorithm calculate */
} isp_snap_attr;
#endif

typedef struct {
    td_bool    pq_ai_en;
    ot_vi_pipe ai_pipe_id;
    ot_vi_pipe base_pipe_id;
} pq_ai_attr;

typedef struct {
    td_bool stagger_en;
    td_u8   merge_frame_num;
    ot_crop_info crop_info;
} isp_stagger_attr;

typedef struct {
    td_u32 width_word;
    td_phys_addr_t phys_addr;
} isp_lut2stt_reg_info;

typedef struct {
    td_bool en;
    td_u16  bit_width;    /* 0x10 */
    td_u16  width;        /* stat hblk num */
    td_u16  height;       /* stat vblk num */
    td_u16  stride;       /* (width * 32 + 127) / 128 * 16 */
    td_phys_addr_t  phys_addr;
} isp_wdr_stat_info;

typedef struct {
    td_phys_addr_t be0_cfg_addr;
    td_phys_addr_t be1_cfg_addr;

    td_phys_addr_t be_param0_addr;
    td_phys_addr_t be_param1_addr;
    td_phys_addr_t be_param2_addr;
    td_phys_addr_t be_param3_addr;

    isp_wdr_stat_info wdr_sti;
    isp_wdr_stat_info wdr_sto;

    isp_lut2stt_reg_info dpc;
    isp_lut2stt_reg_info dpc1;
    isp_lut2stt_reg_info pregammafe;
    isp_lut2stt_reg_info crb;
} isp_pre_be_offline_addr_info;

typedef struct {
    td_phys_addr_t be0_cfg_addr;
    td_phys_addr_t be1_cfg_addr;

    td_phys_addr_t be_param0_addr;
    td_phys_addr_t be_param1_addr;
    td_phys_addr_t be_param2_addr;
    td_phys_addr_t be_param3_addr;

    td_phys_addr_t ldci_write_stat_addr;
    td_phys_addr_t ldci_read_stat_addr;

    td_phys_addr_t drc_write_stat_addr;
    td_phys_addr_t drc_read_stat_addr;

    isp_lut2stt_reg_info bnr;
    isp_lut2stt_reg_info lsc;
    isp_lut2stt_reg_info clut;
    isp_lut2stt_reg_info gamma;
    isp_lut2stt_reg_info dehaze;
    isp_lut2stt_reg_info ldci;
    isp_lut2stt_reg_info sharpen;
    isp_lut2stt_reg_info degamma_fe;
    isp_lut2stt_reg_info dmnr; /* bnr sharpen */
} isp_post_be_offline_addr_info;

typedef struct {
    isp_pre_be_offline_addr_info pre_be[OT_ISP_STRIPING_MAX_NUM];
    isp_post_be_offline_addr_info post_be[OT_ISP_STRIPING_MAX_NUM];
} isp_be_offline_addr_info;

#define ISP_1ST_INT                     0x1
#define ISP_2ND_INT                     (1 << 4)
#define CROSS_FRAME_FLAG                0x3

#define ISP_DEV_SET_FD                  _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SET_FD, td_s32)
#define ISP_GET_FRAME_EDGE              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_GET_FRAME_EDGE, td_u32)
#define ISP_GET_VD_TIMEOUT              _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_VD_TIMEOUT, isp_vd_timeout)
#define ISP_GET_VD_END_TIMEOUT          _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_VD_END_TIMEOUT, isp_vd_timeout)
#define ISP_GET_VD_BEEND_TIMEOUT        _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_VD_BEEND_TIMEOUT, isp_vd_timeout)
#define ISP_SET_INT_ENABLE              _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SET_INT_ENABLE, td_bool)
#define ISP_GET_VERSION                 _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_VERSION, isp_version)
#define ISP_STAT_BUF_INIT               _IOR(IOC_TYPE_ISP, IOC_NR_ISP_STAT_BUF_INIT, td_u64)
#define ISP_STAT_BUF_EXIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_STAT_BUF_EXIT)
#define ISP_STAT_BUF_GET                _IOR(IOC_TYPE_ISP, IOC_NR_ISP_STAT_BUF_GET, isp_stat_info)
#define ISP_STAT_BUF_PUT                _IOW(IOC_TYPE_ISP, IOC_NR_ISP_STAT_BUF_PUT, isp_stat_info)
#define ISP_STAT_ACT_GET                _IOR(IOC_TYPE_ISP, IOC_NR_ISP_STAT_ACT_GET, isp_stat_info)
#define ISP_REG_CFG_INIT                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_REG_CFG_INIT, isp_reg_cfg)
#define ISP_REG_CFG_SET                 _IOW(IOC_TYPE_ISP, IOC_NR_ISP_REG_CFG_SET, isp_kernel_reg_cfg)

#define ISP_BE_CFG_BUF_INIT             _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_BE_CFG_BUF_INIT, td_u64)
#define ISP_GET_BE_BUF_FIRST            _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_BE_BUF_FIRST, td_u64)
#define ISP_BE_FREE_BUF_GET             _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_BE_FREE_BUF_GET, isp_be_wo_cfg_buf)
#define ISP_BE_LAST_BUF_GET             _IOR(IOC_TYPE_ISP, IOC_NR_ISP_BE_LAST_BUF_GET, td_u64)
#define ISP_BE_CFG_BUF_EXIT             _IO(IOC_TYPE_ISP, IOC_NR_ISP_BE_CFG_BUF_EXIT)
#define ISP_BE_CFG_BUF_CTL              _IOW(IOC_TYPE_ISP, IOC_NR_ISP_BE_CFG_BUF_CTL, isp_be_wo_cfg_buf)
#define ISP_BE_CFG_BUF_RUNNING          _IO(IOC_TYPE_ISP, IOC_NR_ISP_BE_CFG_BUF_RUNNING)
#define ISP_BE_ALL_BUF_INIT             _IO(IOC_TYPE_ISP, IOC_NR_ISP_BE_ALL_BUF_INIT)
#define ISP_BE_SWITCH_FINISH_STATE_SET  _IO(IOC_TYPE_ISP, IOC_NR_ISP_BE_SWITCH_FINISH_STATE_SET)

#define ISP_MEM_INFO_SET                _IOW(IOC_TYPE_ISP, IOC_NR_ISP_MEM_INFO_SET, td_bool)
#define ISP_MEM_INFO_GET                _IOR(IOC_TYPE_ISP, IOC_NR_ISP_MEM_INFO_GET, td_bool)
#define ISP_P2EN_INFO_GET               _IOR(IOC_TYPE_ISP, IOC_NR_ISP_P2EN_INFO_GET, td_bool)
#define ISP_BE_BUF_NUM_GET              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_BE_BUF_NUM_GET, td_u8)
#define ISP_INIT_INFO_SET               _IOW(IOC_TYPE_ISP, IOC_NR_ISP_INIT_INFO_SET, td_bool)
#define ISP_SYNC_INIT_SET               _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SYNC_INIT_SET, td_bool)
#define ISP_RUN_STATE_SET               _IOW(IOC_TYPE_ISP, IOC_NR_ISP_RUN_STATE_SET, td_u64)
#define ISP_RESET_CTX                   _IO(IOC_TYPE_ISP, IOC_NR_ISP_RESET_CTX)
#define ISP_CONFIG_INFO_SET             _IOW(IOC_TYPE_ISP, IOC_NR_ISP_CONFIG_INFO_SET, ot_isp_config_info)
#define ISP_PRO_TRIGGER_GET             _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PRO_EN_GET, td_bool)
#define ISP_UPDATE_POS_GET              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_UPDATE_POS_GET, td_u32)
#define ISP_FRAME_CNT_GET               _IOR(IOC_TYPE_ISP, IOC_NR_ISP_FRAME_CNT_GET, td_u32)
#define ISP_SNAP_ATTR_GET               _IOR(IOC_TYPE_ISP, IOC_NR_ISP_SNAP_ATTR_GET, isp_snap_attr)

#define ISP_PROC_INIT                   _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PROC_INIT, isp_proc_mem)
#define ISP_PROC_WRITE_ING              _IO(IOC_TYPE_ISP, IOC_NR_ISP_PROC_WRITE_ING)
#define ISP_PROC_WRITE_OK               _IO(IOC_TYPE_ISP, IOC_NR_ISP_PROC_WRITE_OK)
#define ISP_PROC_EXIT                   _IO(IOC_TYPE_ISP, IOC_NR_ISP_PROC_EXIT)
#define ISP_PROC_PARAM_GET              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PROC_PARAM_GET, td_u32)
#define ISP_TRANS_BUF_INIT              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_TRANS_BUF_INIT, isp_trans_info_buf)
#define ISP_TRANS_BUF_EXIT              _IO(IOC_TYPE_ISP, IOC_NR_ISP_TRANS_BUF_EXIT)
#define ISP_PRO_BUF_INIT                _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PRO_BUF_INIT, isp_pro_info_buf)
#define ISP_PRO_BUF_EXIT                _IO(IOC_TYPE_ISP,  IOC_NR_ISP_PRO_BUF_EXIT)

#define ISP_UPDATE_INFO_SET             _IOW(IOC_TYPE_ISP, IOC_NR_ISP_UPDATE_INFO_SET, ot_isp_dcf_update_info)
#define ISP_FRAME_INFO_SET              _IOW(IOC_TYPE_ISP, IOC_NR_ISP_FRAME_INFO_SET, ot_isp_frame_info)
#define ISP_FRAME_INFO_GET              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_FRAME_INFO_GET, ot_isp_frame_info)
#define ISP_DNG_INFO_SET                _IOW(IOC_TYPE_ISP, IOC_NR_ISP_DNG_INFO_SET, ot_dng_image_dynamic_info)
#define ISP_DNG_INFO_GET                _IOR(IOC_TYPE_ISP, IOC_NR_ISP_DNG_INFO_GET, ot_isp_dng_image_static_info)
#define ISP_SYNC_CFG_SET                _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SYNC_CFG_SET, isp_sync_cfg_buf_node)
#define ISP_WDR_CFG_SET                 _IOW(IOC_TYPE_ISP, IOC_NR_WDR_SYNC_CFG_SET, isp_wdr_cfg)
#define ISP_BE_SYNC_PARAM_INIT          _IOW(IOC_TYPE_ISP, IOC_NR_ISP_BE_SYNC_PARAM_INIT, isp_be_sync_para)
#define ISP_RES_SWITCH_SET              _IO(IOC_TYPE_ISP, IOC_NR_ISP_RES_SWITCH_SET)
#define ISP_CHN_SELECT_CFG              _IOW(IOC_TYPE_ISP, IOC_NR_ISP_CHN_SELECT_CFG, td_u32)

#define ISP_MCF_EN_GET                  _IOR(IOC_TYPE_ISP, IOC_NR_ISP_MCF_EN_GET, td_bool)
#define ISP_PWM_NUM_GET                 _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PWM_NUM_GET, td_u32)
#define ISP_WORK_MODE_INIT              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_WORK_MODE_INIT, isp_block_attr)
#define ISP_WORK_MODE_GET               _IOR(IOC_TYPE_ISP, IOC_NR_ISP_WORK_MODE_GET, isp_working_mode)
#define ISP_WORK_MODE_EXIT              _IO(IOC_TYPE_ISP, IOC_NR_ISP_WORK_MODE_EXIT)
#define ISP_SET_MOD_PARAM               _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SET_MOD_PARAM, ot_isp_mod_param)
#define ISP_GET_MOD_PARAM               _IOR(IOC_TYPE_ISP, IOC_NR_ISP_GET_MOD_PARAM, ot_isp_mod_param)
#define ISP_SET_CTRL_PARAM              _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SET_CTRL_PARAM, ot_isp_ctrl_param)
#define ISP_GET_CTRL_PARAM              _IOR(IOC_TYPE_ISP, IOC_NR_ISP_GET_CTRL_PARAM, ot_isp_ctrl_param)
#define ISP_PRE_BLK_NUM_UPDATE          _IOW(IOC_TYPE_ISP, IOC_NR_ISP_PRE_BLK_NUM_UPDATE, td_u8)

#define ISP_GET_HDR_ATTR                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_HDR_ATTR, vi_pipe_hdr_attr)
#define ISP_GET_WDR_ATTR                _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_WDR_ATTR, vi_pipe_wdr_attr)
#define ISP_GET_BAS_CROP_ATTR           _IOWR(IOC_TYPE_ISP, IOC_BAS_CROP_ATTR, vi_blc_crop_info)
#define ISP_GET_STITCH_ATTR             _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_STITCH_ATTR, vi_stitch_attr)
#define ISP_GET_PIPE_SIZE               _IOWR(IOC_TYPE_ISP, IOC_NR_ISP_GET_PIPE_SIZE, ot_size)
#define ISP_PUB_ATTR_INFO               _IOW(IOC_TYPE_ISP, IOC_NR_ISP_PUB_ATTR_INFO, ot_isp_pub_attr)
#define ISP_KERNEL_RUNONCE              _IO(IOC_TYPE_ISP, IOC_NR_ISP_KERNEL_RUNONCE)
#define ISP_OPT_RUNONCE_INFO            _IOW(IOC_TYPE_ISP, IOC_NR_ISP_OPT_RUNONCE_INFO, td_bool)
#define ISP_SET_PROCALCDONE             _IO(IOC_TYPE_ISP, IOC_NR_ISP_SET_PROCALCDONE)
#define ISP_YUV_RUNONCE_INFO            _IOW(IOC_TYPE_ISP, IOC_NR_ISP_YUV_RUNONCE_INFO, td_bool)
#define ISP_KERNEL_YUV_RUNONCE          _IO(IOC_TYPE_ISP,  IOC_NR_ISP_KERNEL_YUV_RUNONCE)
#define ISP_SYNC_STITCH_PARAM_INIT      _IO(IOC_TYPE_ISP, IOC_NR_ISP_SYNC_STITCH_PARAM_INIT)
#define ISP_CLUT_BUF_INIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_CLUT_BUF_INIT)
#define ISP_CLUT_BUF_EXIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_CLUT_BUF_EXIT)
#define ISP_CLUT_BUF_GET                _IOR(IOC_TYPE_ISP, IOC_NR_ISP_CLUT_BUF_GET, td_u64)
#define ISP_LDCI_BUF_INIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_LDCI_BUF_INIT)
#define ISP_LDCI_BUF_EXIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_LDCI_BUF_EXIT)
#define ISP_LDCI_READ_STT_BUF_GET       _IOR(IOC_TYPE_ISP, IOC_NR_ISP_LDCI_READ_STT_BUF_GET, isp_ldci_read_stt_buf)
#define ISP_BNR_TEMPORAL_FILT_CFG_SET   _IOW(IOC_TYPE_ISP, IOC_NR_ISP_BNR_TEMPORAL_FILT_CFG_SET, isp_bnr_temporal_filt)
#define ISP_MOT_CFG_SET                 _IOW(IOC_TYPE_ISP, IOC_NR_ISP_MOT_CFG_SET, td_bool)

#define ISP_DRC_BUF_INIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_DRC_BUF_INIT)
#define ISP_DRC_BUF_EXIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_DRC_BUF_EXIT)
#define ISP_FSWDR_BUF_INIT             _IO(IOC_TYPE_ISP, IOC_NR_ISP_FSWDR_BUF_INIT)
#define ISP_FSWDR_BUF_EXIT             _IO(IOC_TYPE_ISP, IOC_NR_ISP_FSWDR_BUF_EXIT)

#define ISP_STT_BUF_INIT                _IO(IOC_TYPE_ISP, IOC_NR_ISP_STT_BUF_INIT)
#define ISP_STT_BUF_EXIT                _IO(IOC_TYPE_ISP, IOC_NR_ISP_STT_BUF_EXIT)
#define ISP_STT_ADDR_INIT               _IO(IOC_TYPE_ISP, IOC_NR_ISP_STT_ADDR_INIT)
#define ISP_BE_LUT_STT_BUF_GET          _IOR(IOC_TYPE_ISP, IOC_NR_ISP_BE_LUT_STT_BUF_GET, td_u64)
#define ISP_BE_PRE_LUT_STT_BUF_GET      _IOR(IOC_TYPE_ISP, IOC_NR_ISP_BE_PRE_LUT_STT_BUF_GET, td_u64)
#define ISP_DYNAMIC_ACTUAL_INFO_GET  _IOR(IOC_TYPE_ISP, IOC_NR_ISP_DYNAMIC_ACTUAL_INFO_GET, isp_blc_actual_info)
#define ISP_FPN_WORK_MODE_SET        _IOW(IOC_TYPE_ISP, IOC_NR_ISP_FPN_WORK_MODE_SET, td_u8)

#define ISP_SET_RGBIR_FORMAT            _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SET_RGBIR_FORMAT, ot_isp_ir_bayer_format)
#define ISP_ALG_INIT_ERR_INFO_PRINT     _IOW(IOC_TYPE_ISP, IOC_NR_ISP_ALG_INIT_ERR_INFO_PRINT, ot_isp_alg_mod)
#ifdef CONFIG_OT_ISP_PQ_FOR_AI_SUPPORT
#define ISP_PQ_AI_GROUP_ATTR_SET    _IOW(IOC_TYPE_ISP, IOC_NR_ISP_PQ_AI_GROUP_ATTR_SET, pq_ai_attr)
#define ISP_PQ_AI_GROUP_ATTR_GET    _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PQ_AI_GROUP_ATTR_GET, pq_ai_attr)
#define ISP_PQ_AI_POST_NR_ATTR_SET  _IOW(IOC_TYPE_ISP, IOC_NR_ISP_PQ_AI_POST_NR_ATTR_SET, ot_pq_ai_noiseness_post_attr)
#define ISP_PQ_AI_POST_NR_ATTR_GET  _IOR(IOC_TYPE_ISP, IOC_NR_ISP_PQ_AI_POST_NR_ATTR_GET, ot_pq_ai_noiseness_post_attr)
#endif
#ifdef CONFIG_OT_SNAP_SUPPORT
#define ISP_SNAP_ATTR_SET           _IOW(IOC_TYPE_ISP, IOC_NR_ISP_SNAP_ATTR_SET, isp_snap_attr)
#endif

#ifdef CONFIG_OT_ISP_HNR_SUPPORT
#define ISP_HNR_EN_SET              _IOW(IOC_TYPE_ISP, IOC_NR_ISP_HNR_EN_SET, td_bool)
#endif

#define ISP_STAGGER_ATTR_GET        _IOR(IOC_TYPE_ISP, IOC_NR_ISP_STAGGER_ATTR_GET, isp_stagger_attr)
#define isp_get_dev(f)             (*((td_u32*)(f)))

#define isp_check_pipe_return(pipe)                                   \
    do {                                                       \
        if (((pipe) < 0) || ((pipe) >= OT_ISP_MAX_PIPE_NUM)) {    \
            isp_err_trace("Err isp pipe %d!\n", pipe); \
            return OT_ERR_ISP_ILLEGAL_PARAM;                   \
        }                                                      \
    } while (0)

#define isp_check_pipe_void_return(pipe)                                   \
    do {                                                       \
        if (((pipe) < 0) || ((pipe) >= OT_ISP_MAX_PIPE_NUM)) {    \
            isp_err_trace("Err isp pipe %d!\n", pipe); \
            return;                   \
        }                                                      \
    } while (0)

#define isp_check_viproc_id_return(mode, viproc_id) \
    do { \
        if ((is_offline_mode(mode) || is_striping_mode(mode)) &&(((viproc_id) != 0) && ((viproc_id) != 1))) { \
            return TD_FAILURE; \
        } else if (is_pre_online_post_offline(mode) && ((viproc_id) != 1)) { \
            return TD_FAILURE; \
        } \
    } while (0)

#define isp_check_vir_pipe_return(pipe) \
    do { \
        if (((pipe) < OT_ISP_MAX_PIPE_NUM) && ((pipe) >= OT_ISP_MAX_PHY_PIPE_NUM)) { \
            return TD_SUCCESS;                                                 \
        }                                                                      \
    } while (0)

#define isp_check_pointer_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_err_trace("Null Pointer!\n"); \
            return OT_ERR_ISP_NULL_PTR; \
        } \
    } while (0)

#define isp_check_pointer_void_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_err_trace("Null Pointer!\n"); \
            return; \
        } \
    } while (0)

#define isp_check_bool_return(bool) \
    do { \
        if (((bool) != TD_TRUE) && ((bool) != TD_FALSE)) { \
            isp_err_trace("Invalid ISP Bool Type %d!\n", bool); \
            return OT_ERR_ISP_ILLEGAL_PARAM; \
        } \
    } while (0)

#define isp_check_buf_init_return(pipe, init) \
    do { \
        if ((init) != TD_FALSE) { \
            isp_err_trace("ISP[%d] buffer is initialized, Please exit ISP!!!\n", pipe); \
            return TD_FAILURE; \
        } \
    } while (0)

#define isp_check_buf_exit_return(pipe, init) \
    do { \
        if ((init) != TD_TRUE) { \
            return TD_SUCCESS; \
        } \
    } while (0)

#define isp_check_bebuf_init_return(pipe, init) \
    do { \
        if ((init) != TD_TRUE) { \
            isp_err_trace("ISP[%d] BeBuf doesn't initialized!\n", pipe); \
            return OT_ERR_ISP_NOT_INIT; \
        } \
    } while (0)

#define isp_check_stabuf_init_return(pipe, init) \
    do { \
        if ((init) != TD_TRUE) { \
            isp_err_trace("ISP[%d] StaBuf doesn't initialized!\n", pipe); \
            return OT_ERR_ISP_NOT_INIT; \
        } \
    } while (0)

#define isp_check_tranbuf_init_return(pipe, init) \
    do { \
        if ((init) != TD_TRUE) { \
            isp_warn_trace("ISP[%d] TransBuf doesn't initialized!\n", pipe); \
            return OT_ERR_ISP_NOT_INIT; \
        } \
    } while (0)

#define isp_check_exit_state_return(pipe, isp_run_flag) \
    do { \
        if ((isp_run_flag) == TD_TRUE) { \
            isp_err_trace("Isp[%d] can't exit!\n", pipe); \
            return TD_FAILURE; \
        } \
    } while (0)

#define isp_check_online_mode_return(pipe, running_mode) \
    do { \
        if ((is_online_mode(running_mode)) ||  \
            (is_sidebyside_mode(running_mode))) { \
            return TD_SUCCESS; \
        } \
    } while (0)

#define isp_dbg(en, fmt...) \
    do { \
        if (en) { \
            printf(fmt); \
        } \
    } while (0)

#define slave_check_dev_return(dev) \
    do { \
        if (((dev) < 0) || ((dev) >= CAP_SLAVE_MAX_NUM)) { \
            isp_err_trace("Err slave dev %d!\n", dev); \
            return OT_ERR_ISP_ILLEGAL_PARAM; \
        } \
    } while (0)

#define isp_alg_check_return(alg_key) \
    do { \
        if ((alg_key) == 0) { \
            return TD_SUCCESS; \
        } \
    } while (0)

#define ISP_STATISTICS_KEY              0xFFFBBF77
#define ISP_PIPE_FEAF_MODULE_ENABLE     1, 0, 0, 0

#define isp_check_feaf_module_return(dev) \
    do { \
        td_u8 fe_have_af_mod[] = {ISP_PIPE_FEAF_MODULE_ENABLE}; \
        if (!fe_have_af_mod[dev]) { \
            isp_err_trace("Invalid Af Fe Pipe %d!\n", dev); \
            return OT_ERR_ISP_ILLEGAL_PARAM; \
        } \
    } while (0)

#define isp_check_return(pipe, ret, fmt, ...) \
    do { \
        if (ret != TD_SUCCESS) { \
            isp_err_trace("Pipe[%d] " fmt " err 0x%x!\n", pipe, ret); \
            return ret; \
        } \
    } while (0)

#define isp_check_success_return(pipe, ret, fmt, ...) \
    do { \
        if (ret != TD_SUCCESS) { \
            isp_warn_trace("Pipe[%d] " fmt " warn 0x%x!\n", pipe, ret); \
            return TD_SUCCESS; \
        } \
    } while (0)

#define isp_check_ret_continue(pipe, ret, fmt, ...) \
    do { \
        if (ret != TD_SUCCESS) { \
            isp_warn_trace("Pipe[%d] " fmt " warn 0x%x!\n", pipe, ret); \
        } \
    } while (0)

#define isp_check_pointer_success_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_warn_trace("Null Pointer!\n"); \
            return TD_SUCCESS; \
        } \
    } while (0)

#define isp_check_ret_goto(ret, err, failed, fmt, ...) \
    do {                                                 \
        if (ret != TD_SUCCESS) {                        \
            isp_err_trace(fmt, ##__VA_ARGS__);          \
            ret = err;                                  \
            goto failed;                              \
        }                                               \
    } while (0)

#define isp_check_pointer_goto(ptr, ret, err, failed, fmt, ...)   \
    do {                                                            \
        if ((ptr) == TD_NULL) {                                     \
            isp_err_trace(fmt, ##__VA_ARGS__);                      \
            ret = err;                                              \
            goto failed;                                          \
        }                                                           \
    } while (0)

#define isp_check_max_param_return(x, max, fmt, ...)   \
    do {                                            \
        if ((x) > (max)) {                          \
            isp_err_trace(fmt, ##__VA_ARGS__);      \
            return OT_ERR_ISP_ILLEGAL_PARAM;        \
        }                                           \
    } while (0)

#define isp_mul_u32_limit(result, a, b) \
    do { \
        if ((a) > 0 && (b) > (U32_UP / (a))) { \
            isp_err_trace( "%u * %u upper limit of the multiplie\n", (td_u32)(a), (td_u32)(b)); \
            (result) = U32_UP; \
        } else { \
            (result) = (a) * (b); \
        } \
    } while (0)

#define isp_mul_s32_limit(result, a, b) \
    do { \
        if (((a) > 0 && (b) > 0 && (b) > (S32_UP / (a))) || ((a) < 0 && (b) < 0 && (b) < (S32_UP / (a))))  { \
            (result) = S32_UP; \
        } else if (((a) < 0 && (b) > 0 && (b) > (S32_LOW / (a))) || ((a) > 0 && (b) < 0 && (b) < (S32_LOW / (a)))) { \
            (result) = S32_LOW; \
        } else { \
            (result) = (a) * (b); \
        } \
    } while (0)

#endif
