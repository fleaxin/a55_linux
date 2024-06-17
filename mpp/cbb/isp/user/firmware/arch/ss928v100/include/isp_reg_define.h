/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_REG_DEFINE_H
#define ISP_REG_DEFINE_H

#include "isp_lut_define.h"
#include "isp_viproc_reg_define.h"

/* vicap_ch  vicap_ch vicap_ch */
/* Define the union u_isp_ch_reg_newer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_reg_newer          : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_reg_newer;

/* Define the union u_isp_ch_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_update             : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_update;

/* Define the union u_isp_ch_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ch_int_fstart         : 1; /* [0]  */
        unsigned int    ch_int_cc_int         : 1; /* [1]  */
        unsigned int    ch_int_buf_ovf        : 1; /* [2]  */
        unsigned int    ch_int_field_throw    : 1; /* [3]  */
        unsigned int    ch_int_update_cfg     : 1; /* [4]  */
        unsigned int    reserved_0            : 9; /* [13..5]  */
        unsigned int    ch_int_fstart_dly1    : 1; /* [14]  */
        unsigned int    ch_int_fstart_dly     : 1; /* [15]  */
        unsigned int    ch_stt_ae_finish      : 1; /* [16]  */
        unsigned int    ch_stt_af_finish      : 1; /* [17]  */
        unsigned int    ch_stt_em_finish      : 1; /* [18]  */
        unsigned int    ch_stt_int_buf_ovf    : 1; /* [19]  */
        unsigned int    ch_em_int_buf_ovf     : 1; /* [20]  */
        unsigned int    ch_tunl_line_int      : 1; /* [21]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_int;

/* Define the union u_isp_ch_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mask_fstart           : 1; /* [0]  */
        unsigned int    mask_cc_int           : 1; /* [1]  */
        unsigned int    mask_buf_ovf          : 1; /* [2]  */
        unsigned int    mask_field_throw      : 1; /* [3]  */
        unsigned int    mask_update_cfg       : 1; /* [4]  */
        unsigned int    reserved_0            : 9; /* [13..5]  */
        unsigned int    mask_fstart_dly1      : 1; /* [14]  */
        unsigned int    mask_fstart_dly       : 1; /* [15]  */
        unsigned int    mask_stt_ae_finish    : 1; /* [16]  */
        unsigned int    mask_stt_af_finish    : 1; /* [17]  */
        unsigned int    mask_stt_em_finish    : 1; /* [18]  */
        unsigned int    mask_stt_buf_ovf      : 1; /* [19]  */
        unsigned int    mask_em_buf_ovf       : 1; /* [20]  */
        unsigned int    mask_tunl_line_int    : 1; /* [21]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_int_mask;

/* Define the union u_isp_ch_wch_stt_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 17; /* [16..0]  */
        unsigned int    ch_wch_stt_finish_mode : 1; /* [17]  */
        unsigned int    reserved_1            : 12; /* [29..18]  */
        unsigned int    ch_wch_stt_32b_align  : 1; /* [30]  */
        unsigned int    reserved_2            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_cfg;

/* Define the union u_isp_ch_wch_stt_ae_hist_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_hist_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_hist_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_hist_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_hist_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_hist_addr_l;
/* Define the union u_isp_ch_wch_stt_ae_aver_r_gr_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_aver_r_gr_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_aver_r_gr_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_aver_r_gr_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_aver_r_gr_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_aver_r_gr_addr_l;
/* Define the union u_isp_ch_wch_stt_ae_aver_gb_b_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_aver_gb_b_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_aver_gb_b_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_aver_gb_b_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_aver_gb_b_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_aver_gb_b_addr_l;
/* Define the union u_isp_ch_wch_stt_af_stat_h1_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_af_stat_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_af_stat_addr_h;
/* Define the union u_isp_ch_wch_stt_af_stat_h1_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_af_stat_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_af_stat_addr_l;

/* Define the union u_isp_ch_wch_stt_ae_ir_hist_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ir_hist_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ir_hist_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_ir_hist_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ir_hist_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ir_hist_addr_l;
/* Define the union u_isp_ch_wch_stt_ae_ir_aver_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ir_aver_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ir_aver_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_ir_aver_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ir_aver_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ir_aver_addr_l;
/* Define the union u_isp_ch_wch_stt_ae_ai_hist_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ai_hist_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ai_hist_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_ai_hist_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ai_hist_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ai_hist_addr_l;
/* Define the union u_isp_ch_wch_stt_ae_ai_aver_addr_h */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ai_aver_addr_h : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ai_aver_addr_h;
/* Define the union u_isp_ch_wch_stt_ae_ai_aver_addr_l */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ch_wch_stt_ae_ai_aver_addr_l : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ch_wch_stt_ae_ai_aver_addr_l;

/* Define the global struct */
typedef struct {
    volatile unsigned int                         reserved_0;                      /* 0x10000 */
    volatile u_isp_ch_reg_newer                   ch_reg_newer;                    /* 0x10004 */
    volatile u_isp_ch_update                      ch_update;                       /* 0x10008 */
    volatile unsigned int                         reserved_1[57]; /* 0x1000c~0x100ec, reserved 57 * 4 bytes */
    volatile u_isp_ch_int                         ch_int;                          /* 0x100f0 */
    volatile unsigned int                         reserved_2;                      /* 0x100f4 */
    volatile u_isp_ch_int_mask                    ch_int_mask;                     /* 0x100f8 */
    volatile unsigned int                         reserved_3[65]; /* 0x100fc ~ 0x101fc, reserved 65 * 4 bytes */
    volatile u_isp_ch_wch_stt_cfg                 ch_wch_stt_cfg;                  /* 0x10200 */
    volatile u_isp_ch_wch_stt_ae_hist_addr_h      ch_wch_stt_ae_hist_addr_h;       /* 0x10204 */
    volatile u_isp_ch_wch_stt_ae_hist_addr_l      ch_wch_stt_ae_hist_addr_l;       /* 0x10208 */
    volatile u_isp_ch_wch_stt_ae_aver_r_gr_addr_h ch_wch_stt_ae_aver_r_gr_addr_h;  /* 0x1020c */
    volatile u_isp_ch_wch_stt_ae_aver_r_gr_addr_l ch_wch_stt_ae_aver_r_gr_addr_l;  /* 0x10210 */
    volatile u_isp_ch_wch_stt_ae_aver_gb_b_addr_h ch_wch_stt_ae_aver_gb_b_addr_h;  /* 0x10214 */
    volatile u_isp_ch_wch_stt_ae_aver_gb_b_addr_l ch_wch_stt_ae_aver_gb_b_addr_l;  /* 0x10218 */
    volatile u_isp_ch_wch_stt_af_stat_addr_h      ch_wch_stt_af_stat_addr_h;       /* 0x1021c */
    volatile u_isp_ch_wch_stt_af_stat_addr_l      ch_wch_stt_af_stat_addr_l;       /* 0x10220 */
    volatile unsigned int                         reserved_4[8];          /* 0x10224 ~ 0x10240, reserved 8 * 4 bytes */
    volatile u_isp_ch_wch_stt_ae_ir_hist_addr_h   ch_wch_stt_ae_ir_hist_addr_h;    /* 0x10244 */
    volatile u_isp_ch_wch_stt_ae_ir_hist_addr_l   ch_wch_stt_ae_ir_hist_addr_l;    /* 0x10248 */
    volatile u_isp_ch_wch_stt_ae_ir_aver_addr_h   ch_wch_stt_ae_ir_aver_addr_h;    /* 0x1024c */
    volatile u_isp_ch_wch_stt_ae_ir_aver_addr_l   ch_wch_stt_ae_ir_aver_addr_l;    /* 0x10250 */
    volatile u_isp_ch_wch_stt_ae_ai_hist_addr_h   ch_wch_stt_ae_smart_hist_addr_h; /* 0x10254 */
    volatile u_isp_ch_wch_stt_ae_ai_hist_addr_l   ch_wch_stt_ae_smart_hist_addr_l; /* 0x10258 */
    volatile u_isp_ch_wch_stt_ae_ai_aver_addr_h   ch_wch_stt_ae_smart_aver_addr_h; /* 0x1025c */
    volatile u_isp_ch_wch_stt_ae_ai_aver_addr_l   ch_wch_stt_ae_smart_aver_addr_l; /* 0x10260 */
    volatile unsigned int                         reserved_7[39];        /* 0x10264~0x102fc, reserved 39 * 4 bytes */
} isp_vicap_ch_reg_type;

/* FE  FE  FE FE  FE  FE */
/* Define the union u_isp_fe_version */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_patch          : 8; /* [7..0]  */
        unsigned int    isp_fe_build          : 8; /* [15..8]  */
        unsigned int    isp_fe_release        : 8; /* [23..16]  */
        unsigned int    isp_fe_version        : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_version;

/* Define the union u_isp_fe_date */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_date            : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_date;
/* Define the union u_isp_fe_fpga_date */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_fpga_date       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_fpga_date;
/* Define the union u_isp_fe_module_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 16; /* [15..0]  */
        unsigned int    reserved_1            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_module_pos;

/* Define the union u_isp_fe_fstart */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_fstart;
/* Define the union u_isp_fe_user_define0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_user_define0    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_user_define0;
/* Define the union u_isp_fe_user_define1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_user_define1    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_user_define1;
/* Define the union u_isp_fe_startup */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_fcnt            : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_startup;
/* Define the union u_isp_fe_format */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_format          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_format;
/* Define the union u_isp_fe_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_int_fstart     : 1; /* [0]  */
        unsigned int    isp_fe_int_update_cfg : 1; /* [1]  */
        unsigned int    isp_fe_int_cfg_loss   : 1; /* [2]  */
        unsigned int    isp_fe_int_int_delay  : 1; /* [3]  */
        unsigned int    isp_fe_int_ae1_int    : 1; /* [4]  */
        unsigned int    isp_fe_int_dynblc_int : 1; /* [5]  */
        unsigned int    isp_fe_int_af1_int    : 1; /* [6]  */
        unsigned int    isp_fe_int_dis1_int   : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_int;

/* Define the union u_isp_fe_int_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_int_fstart_masked : 1; /* [0]  */
        unsigned int    isp_fe_int_update_cfg_masked : 1; /* [1]  */
        unsigned int    isp_fe_int_cfg_loss_masked : 1; /* [2]  */
        unsigned int    isp_fe_int_int_delay_masked : 1; /* [3]  */
        unsigned int    isp_fe_int_ae1_int_masked : 1; /* [4]  */
        unsigned int    isp_fe_int_dynblc_int_masked : 1; /* [5]  */
        unsigned int    isp_fe_int_af1_int_masked : 1; /* [6]  */
        unsigned int    isp_fe_int_dis1_int_masked : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_int_state;

/* Define the union u_isp_fe_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_int_mask_fstart : 1; /* [0]  */
        unsigned int    isp_fe_int_mask_update_cfg : 1; /* [1]  */
        unsigned int    isp_fe_int_mask_cfg_loss : 1; /* [2]  */
        unsigned int    isp_fe_int_mask_int_delay : 1; /* [3]  */
        unsigned int    isp_fe_int_mask_ae1_int : 1; /* [4]  */
        unsigned int    isp_fe_int_mask_dynblc_int : 1; /* [5]  */
        unsigned int    isp_fe_int_mask_af1_int : 1; /* [6]  */
        unsigned int    isp_fe_int_mask_dis1_int : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_int_mask;

/* Define the union u_isp_fe_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 2; /* [1..0]  */
        unsigned int    isp_colorbar_en       : 1; /* [2]  */
        unsigned int    isp_crop_en           : 1; /* [3]  */
        unsigned int    isp_blc1_en           : 1; /* [4]  */
        unsigned int    isp_rc_en             : 1; /* [5]  */
        unsigned int    isp_hrs_en            : 1; /* [6]  */
        unsigned int    reserved_1            : 1; /* [7]  */
        unsigned int    isp_dg2_en            : 1; /* [8]  */
        unsigned int    reserved_2            : 1; /* [9]  */
        unsigned int    isp_af1_en            : 1; /* [10]  */
        unsigned int    isp_wb1_en            : 1; /* [11]  */
        unsigned int    reserved_3            : 1; /* [12]  */
        unsigned int    isp_ae1_en            : 1; /* [13]  */
        unsigned int    reserved_4            : 1; /* [14]  */
        unsigned int    isp_blc_dyn_en        : 1; /* [15]  */
        unsigned int    reserved_5            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_ctrl;

/* Define the union u_isp_fe_manual_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_manual_update  : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_manual_update;

/* Define the union u_isp_fe_adapter_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fe_hsync_mode         : 1; /* [0]  */
        unsigned int    fe_vsync_mode         : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_adapter_cfg;

/* Define the union u_isp_fe_out_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fe_out_width          : 16; /* [15..0]  */
        unsigned int    fe_out_height         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_out_size;

/* Define the union u_isp_fe_fstart_delay */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_delay           : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_fstart_delay;
/* Define the union u_isp_fe_ctrl_f */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_rggb_cfg       : 2; /* [1..0]  */
        unsigned int    isp_fe_rgbir_cfg      : 5; /* [6..2]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_ctrl_f;

/* Define the union u_isp_fe_ctrl_i */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_update_mode    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_ctrl_i;

/* Define the union u_isp_fe_timing_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_fe_fix_timing     : 13; /* [13..1]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_timing_cfg;

/* Define the union u_isp_fe_reg_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_update         : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_reg_update;

/* Define the union u_isp_fe_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_width          : 16; /* [15..0]  */
        unsigned int    isp_fe_height         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_size;

/* Define the union u_isp_fe_in_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_csc_sum_en     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_in_sum_cfg;

/* Define the union u_isp_fe_out_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_yuv444_sum_en  : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_out_sum_cfg;

/* Define the union u_isp_fe_blk_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_blk_width      : 16; /* [15..0]  */
        unsigned int    isp_fe_blk_height     : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_blk_size;

/* Define the union u_isp_fe_blk_hblank */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_blk_b_hblank   : 16; /* [15..0]  */
        unsigned int    isp_fe_blk_f_hblank   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_blk_hblank;

/* Define the union u_isp_fe_blk_vblank */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fe_blk_b_vblank   : 16; /* [15..0]  */
        unsigned int    isp_fe_blk_f_vblank   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_blk_vblank;

/* Define the union u_isp_fe_in_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_y_sum0          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_in_sum0;
/* Define the union u_isp_fe_in_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_y_sum1          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_in_sum1;
/* Define the union u_isp_fe_out_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_yuv444_y_sum0   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_out_sum0;
/* Define the union u_isp_fe_out_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fe_yuv444_y_sum1   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fe_out_sum1;
/* Define the union u_isp_blc1_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc1_version;
/* Define the union u_isp_blc1_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc1_ofsgr        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_blc1_ofsr         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc1_offset1;

/* Define the union u_isp_blc1_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc1_ofsgb        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_blc1_ofsb         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc1_offset2;

/* Define the union u_isp_dg2_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_en_in         : 1; /* [0]  */
        unsigned int    isp_dg2_en_out        : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_cfg;

/* Define the union u_isp_dg2_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_version;
/* Define the union u_isp_dg2_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_grgain        : 16; /* [15..0]  */
        unsigned int    isp_dg2_rgain         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_gain1;

/* Define the union u_isp_dg2_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_gbgain        : 16; /* [15..0]  */
        unsigned int    isp_dg2_bgain         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_gain2;

/* Define the union u_isp_dg2_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_ofsgr         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dg2_ofsr          : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_offset1;

/* Define the union u_isp_dg2_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_ofsgb         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dg2_ofsb          : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_offset2;

/* Define the union u_isp_dg2_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_clip_value    : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_clip_value;

/* Define the union u_isp_dg2_gain3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_irgain        : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_gain3;

/* Define the union u_isp_dg2_blc_offset3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg2_ofsir         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg2_blc_offset3;

/* Define the union u_isp_hrs_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_hrs_ds_en         : 1; /* [1]  */
        unsigned int    reserved_1            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_cfg;

/* Define the union u_isp_hrs_filterlut0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hrs_filterlut0_0  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_hrs_filterlut0_1  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_filterlut0;

/* Define the union u_isp_hrs_filterlut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hrs_filterlut1_0  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_hrs_filterlut1_1  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_filterlut1;

/* Define the union u_isp_hrs_filterlut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hrs_filterlut2_0  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_hrs_filterlut2_1  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_filterlut2;

/* Define the union u_isp_hrs_filterlut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hrs_filterlut3_0  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_hrs_filterlut3_1  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_filterlut3;

/* Define the union u_isp_hrs_filterlut4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hrs_filterlut4_0  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_hrs_filterlut4_1  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_filterlut4;

/* Define the union u_isp_hrs_filterlut5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hrs_filterlut5_0  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_hrs_filterlut5_1  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hrs_filterlut5;

/* Define the union u_isp_wb1_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_en_in         : 1; /* [0]  */
        unsigned int    isp_wb1_en_out        : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_cfg;

/* Define the union u_isp_wb1_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_version;
/* Define the union u_isp_wb1_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_grgain        : 16; /* [15..0]  */
        unsigned int    isp_wb1_rgain         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_gain1;

/* Define the union u_isp_wb1_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_gbgain        : 16; /* [15..0]  */
        unsigned int    isp_wb1_bgain         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_gain2;

/* Define the union u_isp_wb1_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_ofsgr         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wb1_ofsr          : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_offset1;

/* Define the union u_isp_wb1_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_ofsgb         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wb1_ofsb          : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_offset2;

/* Define the union u_isp_wb1_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_clip_value    : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_clip_value;

/* Define the union u_isp_wb1_gain3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_irgain        : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_gain3;

/* Define the union u_isp_wb1_blc_offset3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb1_ofsir         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb1_blc_offset3;

/* Define the union u_isp_ae_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_version         : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_version;
/* Define the union u_isp_ae_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_hnum           : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_ae_vnum           : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_zone;

/* Define the union u_isp_ae_skip_crg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_skip_x         : 3; /* [2..0]  */
        unsigned int    isp_ae_offset_x       : 1; /* [3]  */
        unsigned int    isp_ae_skip_y         : 3; /* [6..4]  */
        unsigned int    isp_ae_offset_y       : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_skip_crg;

/* Define the union u_isp_ae_total_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_total_pixels   : 27; /* [26..0]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_stat;

/* Define the union u_isp_ae_count_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_count_pixels   : 31; /* [30..0]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_count_stat;

/* Define the union u_isp_ae_total_r_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_r_aver    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_r_aver;
/* Define the union u_isp_ae_total_gr_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_gr_aver   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_gr_aver;
/* Define the union u_isp_ae_total_gb_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_gb_aver   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_gb_aver;
/* Define the union u_isp_ae_total_b_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_b_aver    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_b_aver;
/* Define the union u_isp_ae_hist_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_high       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist_high;
/* Define the union u_isp_ae_total_ir_aver */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_ir_aver   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_ir_aver;
/* Define the union u_isp_ae_bitmove */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_bitmove        : 5; /* [4..0]  */
        unsigned int    isp_ae_hist_gamma_mode : 2; /* [6..5]  */
        unsigned int    isp_ae_aver_gamma_mode : 2; /* [8..7]  */
        unsigned int    isp_ae_blc_en         : 1; /* [9]  */
        unsigned int    isp_ae_gamma_limit    : 4; /* [13..10]  */
        unsigned int    isp_ae_fourplanemode  : 1; /* [14]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_offset_ir      : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_bitmove;

/* Define the union u_isp_ae_offset_r_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_offset_r       : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_offset_gr      : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_offset_r_gr;

/* Define the union u_isp_ae_offset_gb_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_offset_gb      : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_offset_b       : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_offset_gb_b;

/* Define the union u_isp_ae_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_lut_update     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_lut_update;

/* Define the union u_isp_ae_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_stt_size       : 16; /* [15..0]  */
        unsigned int    isp_ae_stt_bst        : 4; /* [19..16]  */
        unsigned int    isp_ae_stt_en         : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_stt_bst;

/* Define the union u_isp_ae_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_crop_pos_x     : 16; /* [15..0]  */
        unsigned int    isp_ae_crop_pos_y     : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_crop_pos;

/* Define the union u_isp_ae_crop_outsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_crop_out_width : 16; /* [15..0]  */
        unsigned int    isp_ae_crop_out_height : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_crop_outsize;

/* Define the union u_isp_ae_hist_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist_waddr;
/* Define the union u_isp_ae_hist_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist_wdata;
/* Define the union u_isp_ae_hist_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_raddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist_raddr;
/* Define the union u_isp_ae_hist_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_rdata      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist_rdata;
/* Define the union u_isp_ae_aver_r_gr_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_r_gr_waddr;
/* Define the union u_isp_ae_aver_r_gr_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_r_gr_wdata;
/* Define the union u_isp_ae_aver_r_gr_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_r_gr_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_r_gr_raddr;
/* Define the union u_isp_ae_aver_r_gr_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_r_gr_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_r_gr_rdata;
/* Define the union u_isp_ae_aver_gb_b_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_gb_b_waddr;
/* Define the union u_isp_ae_aver_gb_b_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_gb_b_wdata;
/* Define the union u_isp_ae_aver_gb_b_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_gb_b_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_gb_b_raddr;
/* Define the union u_isp_ae_aver_gb_b_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_gb_b_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_gb_b_rdata;
/* Define the union u_isp_ae_weight_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_wei_waddr       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_weight_waddr;
/* Define the union u_isp_ae_weight_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_wei_wdata       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_weight_wdata;
/* Define the union u_isp_ae_weight_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_weight_raddr;
/* Define the union u_isp_ae_weight_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_weight_rdata;
/* Define the union u_isp_ae_ir_hist_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ir_hist_waddr;
/* Define the union u_isp_ae_ir_hist_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ir_hist_wdata;
/* Define the union u_isp_ae_ir_hist_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ir_hist_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ir_hist_raddr;
/* Define the union u_isp_ae_ir_hist_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ir_hist_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ir_hist_rdata;
/* Define the union u_isp_ae_aver_ir_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_ir_waddr;
/* Define the union u_isp_ae_aver_ir_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_ir_wdata;
/* Define the union u_isp_ae_aver_ir_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_ir_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_ir_raddr;
/* Define the union u_isp_ae_aver_ir_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_ir_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_ir_rdata;
/* Define the union u_isp_ae_ai_hist_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_hist_waddr;
/* Define the union u_isp_ae_ai_hist_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_hist_wdata;
/* Define the union u_isp_ae_ai_hist_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_hist_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_hist_raddr;
/* Define the union u_isp_ae_ai_hist_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_hist_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_hist_rdata;
/* Define the union u_isp_ae_ai_aver_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_aver_waddr;
/* Define the union u_isp_ae_ai_aver_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_aver_wdata;
/* Define the union u_isp_ae_ai_aver_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_aver_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_aver_raddr;
/* Define the union u_isp_ae_ai_aver_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_aver_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_aver_rdata;
/* Define the union u_isp_ae_smart_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart_en       : 1; /* [0]  */
        unsigned int    isp_ae_smart_obj_num  : 5; /* [5..1]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart_ctrl;

/* Define the union u_isp_ae_smart0_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart0_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart0_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart0_pos;

/* Define the union u_isp_ae_smart0_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart0_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart0_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart0_size;

/* Define the union u_isp_ae_smart1_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart1_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart1_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart1_pos;

/* Define the union u_isp_ae_smart1_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart1_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart1_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart1_size;

/* Define the union u_isp_ae_smart2_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart2_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart2_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart2_pos;

/* Define the union u_isp_ae_smart2_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart2_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart2_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart2_size;

/* Define the union u_isp_ae_smart3_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart3_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart3_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart3_pos;

/* Define the union u_isp_ae_smart3_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart3_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart3_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart3_size;

/* Define the union u_isp_ae_smart4_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart4_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart4_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart4_pos;

/* Define the union u_isp_ae_smart4_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart4_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart4_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart4_size;

/* Define the union u_isp_ae_smart5_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart5_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart5_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart5_pos;

/* Define the union u_isp_ae_smart5_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart5_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart5_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart5_size;

/* Define the union u_isp_ae_smart6_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart6_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart6_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart6_pos;

/* Define the union u_isp_ae_smart6_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart6_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart6_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart6_size;

/* Define the union u_isp_ae_smart7_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart7_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart7_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart7_pos;

/* Define the union u_isp_ae_smart7_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart7_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart7_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart7_size;

/* Define the union u_isp_ae_smart8_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart8_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart8_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart8_pos;

/* Define the union u_isp_ae_smart8_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart8_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart8_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart8_size;

/* Define the union u_isp_ae_smart9_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart9_objx    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart9_objy    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart9_pos;

/* Define the union u_isp_ae_smart9_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart9_objw    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart9_objh    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart9_size;

/* Define the union u_isp_ae_smart10_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart10_objx   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart10_objy   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart10_pos;

/* Define the union u_isp_ae_smart10_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart10_objw   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart10_objh   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart10_size;

/* Define the union u_isp_ae_smart11_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart11_objx   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart11_objy   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart11_pos;

/* Define the union u_isp_ae_smart11_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart11_objw   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart11_objh   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart11_size;

/* Define the union u_isp_ae_smart12_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart12_objx   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart12_objy   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart12_pos;

/* Define the union u_isp_ae_smart12_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart12_objw   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart12_objh   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart12_size;

/* Define the union u_isp_ae_smart13_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart13_objx   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart13_objy   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart13_pos;

/* Define the union u_isp_ae_smart13_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart13_objw   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart13_objh   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart13_size;

/* Define the union u_isp_ae_smart14_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart14_objx   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart14_objy   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart14_pos;

/* Define the union u_isp_ae_smart14_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart14_objw   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart14_objh   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart14_size;

/* Define the union u_isp_ae_smart15_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart15_objx   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart15_objy   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart15_pos;

/* Define the union u_isp_ae_smart15_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_smart15_objw   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ae_smart15_objh   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_smart15_size;

/* Define the union u_isp_af_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_af_iir0_en0       : 1; /* [1]  */
        unsigned int    isp_af_iir0_en1       : 1; /* [2]  */
        unsigned int    isp_af_iir0_en2       : 1; /* [3]  */
        unsigned int    isp_af_iir1_en0       : 1; /* [4]  */
        unsigned int    isp_af_iir1_en1       : 1; /* [5]  */
        unsigned int    isp_af_iir1_en2       : 1; /* [6]  */
        unsigned int    isp_af_peak_mode      : 1; /* [7]  */
        unsigned int    isp_af_squ_mode       : 1; /* [8]  */
        unsigned int    isp_af_offset_en      : 1; /* [9]  */
        unsigned int    isp_af_crop_en        : 1; /* [10]  */
        unsigned int    isp_af_lpf_en         : 1; /* [11]  */
        unsigned int    isp_af_mean_en        : 1; /* [12]  */
        unsigned int    reserved_1            : 1; /* [13]  */
        unsigned int    isp_af_raw_mode       : 1; /* [14]  */
        unsigned int    isp_af_bayer_mode     : 2; /* [16..15]  */
        unsigned int    isp_af_iir0_ds_en     : 1; /* [17]  */
        unsigned int    isp_af_iir1_ds_en     : 1; /* [18]  */
        unsigned int    isp_af_fir0_lpf_en    : 1; /* [19]  */
        unsigned int    isp_af_fir1_lpf_en    : 1; /* [20]  */
        unsigned int    isp_af_iir0_ldg_en    : 1; /* [21]  */
        unsigned int    isp_af_iir1_ldg_en    : 1; /* [22]  */
        unsigned int    isp_af_fir0_ldg_en    : 1; /* [23]  */
        unsigned int    isp_af_fir1_ldg_en    : 1; /* [24]  */
        unsigned int    isp_af_gamma          : 3; /* [27..25]  */
        unsigned int    isp_af_gain_lmt       : 3; /* [30..28]  */
        unsigned int    reserved_2            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_cfg;

/* Define the union u_isp_af_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_af_version         : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_version;
/* Define the union u_isp_af_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_hnum           : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_af_vnum           : 5; /* [12..8]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_zone;

/* Define the union u_isp_af_crop_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_pos_x          : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_af_pos_y          : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_crop_start;

/* Define the union u_isp_af_crop_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_crop_hsize     : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_af_crop_vsize     : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_crop_size;

/* Define the union u_isp_af_mean_thres */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_mean_thres     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_mean_thres;

/* Define the union u_isp_af_iirg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg0_0        : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_af_iirg0_1        : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg0;

/* Define the union u_isp_af_iirg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg1_0        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_af_iirg1_1        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg1;

/* Define the union u_isp_af_iirg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg2_0        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_af_iirg2_1        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg2;

/* Define the union u_isp_af_iirg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg3_0        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_af_iirg3_1        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg3;

/* Define the union u_isp_af_iirg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg4_0        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_af_iirg4_1        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg4;

/* Define the union u_isp_af_iirg5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg5_0        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_af_iirg5_1        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg5;

/* Define the union u_isp_af_iirg6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirg6_0        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_af_iirg6_1        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirg6;

/* Define the union u_isp_af_iirpl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirplg_0       : 8; /* [7..0]  */
        unsigned int    isp_af_iirpls_0       : 3; /* [10..8]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_af_iirplg_1       : 8; /* [23..16]  */
        unsigned int    isp_af_iirpls_1       : 3; /* [26..24]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirpl;

/* Define the union u_isp_af_shift */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iirshift0_0    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    isp_af_iirshift0_1    : 3; /* [6..4]  */
        unsigned int    reserved_1            : 1; /* [7]  */
        unsigned int    isp_af_iirshift0_2    : 3; /* [10..8]  */
        unsigned int    reserved_2            : 1; /* [11]  */
        unsigned int    isp_af_iirshift0_3    : 3; /* [14..12]  */
        unsigned int    reserved_3            : 1; /* [15]  */
        unsigned int    isp_af_iirshift1_0    : 3; /* [18..16]  */
        unsigned int    reserved_4            : 1; /* [19]  */
        unsigned int    isp_af_iirshift1_1    : 3; /* [22..20]  */
        unsigned int    reserved_5            : 1; /* [23]  */
        unsigned int    isp_af_iirshift1_2    : 3; /* [26..24]  */
        unsigned int    reserved_6            : 1; /* [27]  */
        unsigned int    isp_af_iirshift1_3    : 3; /* [30..28]  */
        unsigned int    reserved_7            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_shift;

/* Define the union u_isp_af_firh0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_firh0_0        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_af_firh0_1        : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firh0;

/* Define the union u_isp_af_firh1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_firh1_0        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_af_firh1_1        : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firh1;

/* Define the union u_isp_af_firh2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_firh2_0        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_af_firh2_1        : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firh2;

/* Define the union u_isp_af_firh3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_firh3_0        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_af_firh3_1        : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firh3;

/* Define the union u_isp_af_firh4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_firh4_0        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_af_firh4_1        : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firh4;

/* Define the union u_isp_af_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_stt_size       : 16; /* [15..0]  */
        unsigned int    isp_af_stt_bst        : 4; /* [19..16]  */
        unsigned int    isp_af_stt_en         : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_stt_bst;

/* Define the union u_isp_af_stt_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_stt_info       : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_af_stt_clr        : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_stt_abn;

/* Define the union u_isp_af_acc_shift */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_acc_shift0_h   : 4; /* [3..0]  */
        unsigned int    isp_af_acc_shift1_h   : 4; /* [7..4]  */
        unsigned int    isp_af_acc_shift0_v   : 4; /* [11..8]  */
        unsigned int    isp_af_acc_shift1_v   : 4; /* [15..12]  */
        unsigned int    isp_af_acc_shift_y    : 4; /* [19..16]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_acc_shift;

/* Define the union u_isp_af_cnt_shift */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_cnt_shift0_h   : 4; /* [3..0]  */
        unsigned int    isp_af_cnt_shift1_h   : 4; /* [7..4]  */
        unsigned int    isp_af_cnt_shift0_v   : 4; /* [11..8]  */
        unsigned int    isp_af_cnt_shift1_v   : 4; /* [15..12]  */
        unsigned int    isp_af_cnt_shift_y    : 4; /* [19..16]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_cnt_shift;

/* Define the union u_isp_af_stat_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_af_stat_raddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_stat_raddr;
/* Define the union u_isp_af_stat_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_af_stat_rdata      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_stat_rdata;
/* Define the union u_isp_af_iirthre */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_thre0_l    : 8; /* [7..0]  */
        unsigned int    isp_af_iir_thre0_h    : 8; /* [15..8]  */
        unsigned int    isp_af_iir_thre1_l    : 8; /* [23..16]  */
        unsigned int    isp_af_iir_thre1_h    : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirthre;

/* Define the union u_isp_af_iirgain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_gain0_l    : 8; /* [7..0]  */
        unsigned int    isp_af_iir_gain0_h    : 8; /* [15..8]  */
        unsigned int    isp_af_iir_gain1_l    : 8; /* [23..16]  */
        unsigned int    isp_af_iir_gain1_h    : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirgain;

/* Define the union u_isp_af_iirslope */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_slope0_l   : 4; /* [3..0]  */
        unsigned int    isp_af_iir_slope0_h   : 4; /* [7..4]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_af_iir_slope1_l   : 4; /* [19..16]  */
        unsigned int    isp_af_iir_slope1_h   : 4; /* [23..20]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirslope;

/* Define the union u_isp_af_iirdilate */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_dilate0    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 5; /* [7..3]  */
        unsigned int    isp_af_iir_dilate1    : 3; /* [10..8]  */
        unsigned int    reserved_1            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirdilate;

/* Define the union u_isp_af_firthre */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_fir_thre0_l    : 8; /* [7..0]  */
        unsigned int    isp_af_fir_thre0_h    : 8; /* [15..8]  */
        unsigned int    isp_af_fir_thre1_l    : 8; /* [23..16]  */
        unsigned int    isp_af_fir_thre1_h    : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firthre;

/* Define the union u_isp_af_firgain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_fir_gain0_l    : 8; /* [7..0]  */
        unsigned int    isp_af_fir_gain0_h    : 8; /* [15..8]  */
        unsigned int    isp_af_fir_gain1_l    : 8; /* [23..16]  */
        unsigned int    isp_af_fir_gain1_h    : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firgain;

/* Define the union u_isp_af_firslope */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_fir_slope0_l   : 4; /* [3..0]  */
        unsigned int    isp_af_fir_slope0_h   : 4; /* [7..4]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_af_fir_slope1_l   : 4; /* [19..16]  */
        unsigned int    isp_af_fir_slope1_h   : 4; /* [23..20]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firslope;

/* Define the union u_isp_af_iirthre_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_thre0_c    : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_af_iir_thre1_c    : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirthre_coring;

/* Define the union u_isp_af_iirpeak_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_peak0_c    : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_af_iir_peak1_c    : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirpeak_coring;

/* Define the union u_isp_af_iirslope_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_iir_slope0_c   : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_af_iir_slope1_c   : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_iirslope_coring;

/* Define the union u_isp_af_firthre_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_fir_thre0_c    : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_af_fir_thre1_c    : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firthre_coring;

/* Define the union u_isp_af_firpeak_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_fir_peak0_c    : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_af_fir_peak1_c    : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firpeak_coring;

/* Define the union u_isp_af_firslope_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_fir_slope0_c   : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_af_fir_slope1_c   : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_firslope_coring;

/* Define the union u_isp_af_highlight */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_highlight        : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_highlight;

/* Define the union u_isp_af_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_af_offset_gr      : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_af_offset_gb      : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_offset;

/* Define the union u_isp_rc_blcen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_blc_in_en      : 1; /* [0]  */
        unsigned int    reserved_0            : 7; /* [7..1]  */
        unsigned int    isp_rc_blc_out_en     : 1; /* [8]  */
        unsigned int    reserved_1            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_blcen;

/* Define the union u_isp_rc_blc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_blc_r          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_rc_blc_gr         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_blc0;

/* Define the union u_isp_rc_blc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_blc_b          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_rc_blc_gb         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_blc1;

/* Define the union u_isp_rc_sqradius */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rc_sqradius        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_sqradius;
/* Define the union u_isp_rc_center_coor */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rc_cenhor_coor    : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_rc_cenver_coor    : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rc_center_coor;

/* Define the union u_isp_colorbar_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_colorbar_version   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_version;
/* Define the union u_isp_colorbar_pattern */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_pattern  : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_pattern;

/* Define the union u_isp_colorbar_rbackgnd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_rbackgnd : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_rbackgnd;

/* Define the union u_isp_colorbar_gbackgnd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_gbackgnd : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_gbackgnd;

/* Define the union u_isp_colorbar_bbackgnd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_bbackgnd : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_bbackgnd;

/* Define the union u_isp_colorbar_rforegnd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_rforegnd : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_rforegnd;

/* Define the union u_isp_colorbar_gforegnd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_gforegnd : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_gforegnd;

/* Define the union u_isp_colorbar_bforegnd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_bforegnd : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_bforegnd;

/* Define the union u_isp_colorbar_init */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_rgbinit  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_init;

/* Define the union u_isp_colorbar_grad */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_rgbgrad  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_grad;

/* Define the union u_isp_colorbar_pos1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_pos_x1   : 16; /* [15..0]  */
        unsigned int    isp_colorbar_pos_y1   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_pos1;

/* Define the union u_isp_colorbar_pos2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_colorbar_pos_x2   : 16; /* [15..0]  */
        unsigned int    isp_colorbar_pos_y2   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_colorbar_pos2;

/* Define the union u_isp_crop_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_crop_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crop_version;
/* Define the union u_isp_crop_sizeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crop_width_out    : 16; /* [15..0]  */
        unsigned int    isp_crop_height_out   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crop_sizeout;

/* Define the union u_isp_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crop_pos_x        : 16; /* [15..0]  */
        unsigned int    isp_crop_pos_y        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crop_pos;

/* Define the union u_isp_blc_dynamic_reg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_dynamic_bis2x2pattern : 1; /* [0]  */
        unsigned int    isp_blc_dynamic_raw_array : 4; /* [4..1]  */
        unsigned int    isp_blc_dynamic_dp_stat_en : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_dynamic_reg0;

/* Define the union u_isp_blc_dynamic_reg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_dynamic_start_xpos : 16; /* [15..0]  */
        unsigned int    isp_blc_dynamic_start_ypos : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_dynamic_reg1;

/* Define the union u_isp_blc_dynamic_reg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_dynamic_end_xpos : 16; /* [15..0]  */
        unsigned int    isp_blc_dynamic_end_ypos : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_dynamic_reg2;

/* Define the union u_isp_blc_dynamic_reg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_dynamic_hot_pxl_thd : 16; /* [15..0]  */
        unsigned int    isp_blc_dynamic_dead_pxl_thd : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_dynamic_reg3;

/* Define the union u_isp_blc_dynamic_reg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_dynamic_value0 : 16; /* [15..0]  */
        unsigned int    isp_blc_dynamic_value1 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_dynamic_reg4;

/* Define the union u_isp_blc_dynamic_reg5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_dynamic_value2 : 16; /* [15..0]  */
        unsigned int    isp_blc_dynamic_value3 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_dynamic_reg5;

/* Define the global struct */
typedef struct {
    volatile unsigned int           reserved_0[32]; /* 0x0~0x7c, reserved 32 * 4 bytes */
    volatile u_isp_fe_version       isp_fe_version; /* 0x0080 */
    volatile u_isp_fe_date          isp_fe_date; /* 0x0084 */
    volatile u_isp_fe_fpga_date     isp_fe_fpga_date; /* 0x0088 */
    volatile unsigned int           reserved_1; /* 0x008c */
    volatile u_isp_fe_module_pos    isp_fe_module_pos; /* 0x0090 */
    volatile u_isp_fe_fstart        isp_fe_fstart; /* 0x0094 */
    volatile unsigned int           reserved_2[2]; /* 0x0098~0x009c, reserved 2 * 4 bytes */
    volatile u_isp_fe_user_define0   isp_fe_user_define0; /* 0x00a0 */
    volatile u_isp_fe_user_define1   isp_fe_user_define1; /* 0x00a4 */
    volatile unsigned int           reserved_3[2]; /* 0x00a8~0x00ac, reserved 2 * 4 bytes */
    volatile u_isp_fe_startup       isp_fe_startup; /* 0x00b0 */
    volatile u_isp_fe_format        isp_fe_format; /* 0x00b4 */
    volatile unsigned int           reserved_4[14]; /* 0x00b8~0x00ec, reserved 14 * 4 bytes */
    volatile u_isp_fe_int           isp_fe_int; /* 0x00f0 */
    volatile u_isp_fe_int_state     isp_fe_int_state; /* 0x00f4 */
    volatile u_isp_fe_int_mask      isp_fe_int_mask; /* 0x00f8 */
    volatile unsigned int           reserved_6; /* 0x00fc */
    volatile u_isp_fe_ctrl          isp_fe_ctrl; /* 0x0100 */
    volatile unsigned int           reserved_7[3]; /* 0x0104~0x010c, reserved 3 * 4 bytes */
    volatile u_isp_fe_manual_update isp_fe_manual_update; /* 0x0110 */
    volatile u_isp_fe_adapter_cfg   isp_fe_adapter_cfg; /* 0x0114 */
    volatile u_isp_fe_out_size      isp_fe_out_size; /* 0x0118 */
    volatile unsigned int           reserved_8[30]; /* 0x011c~0x0190, reserved 30 * 4 bytes */
    volatile u_isp_fe_fstart_delay   isp_fe_fstart_delay; /* 0x0194 */
    volatile unsigned int           reserved_9[18]; /* 0x0198~0x01dc, reserved 18 * 4 bytes */
    volatile u_isp_fe_ctrl_f        isp_fe_ctrl_f; /* 0x01e0 */
    volatile u_isp_fe_ctrl_i        isp_fe_ctrl_i; /* 0x01e4 */
    volatile u_isp_fe_timing_cfg    isp_fe_timing_cfg; /* 0x01e8 */
    volatile u_isp_fe_reg_update    isp_fe_reg_update; /* 0x01ec */
    volatile u_isp_fe_size          isp_fe_size; /* 0x01f0 */
    volatile unsigned int           reserved_10[395]; /* 0x01f4~0x081c, reserved 395 * 4 bytes */
    volatile u_isp_fe_in_sum_cfg    isp_fe_in_sum_cfg; /* 0x0820 */
    volatile u_isp_fe_out_sum_cfg   isp_fe_out_sum_cfg; /* 0x0824 */
    volatile unsigned int           reserved_11[19]; /* 0x0828~0x0870, reserved 19 * 4 bytes */
    volatile u_isp_fe_blk_size      isp_fe_blk_size; /* 0x0874 */
    volatile u_isp_fe_blk_hblank    isp_fe_blk_hblank; /* 0x0878 */
    volatile u_isp_fe_blk_vblank    isp_fe_blk_vblank; /* 0x087c */
    volatile unsigned int           reserved_12[4]; /* 0x0880~0x088c , reserved 4 * 4 bytes */
    volatile u_isp_fe_in_sum0       isp_fe_in_sum0; /* 0x0890 */
    volatile u_isp_fe_in_sum1       isp_fe_in_sum1; /* 0x0894 */
    volatile unsigned int           reserved_13[2]; /* 0x0898~0x089c, reserved 2 * 4 bytes */
    volatile u_isp_fe_out_sum0      isp_fe_out_sum0; /* 0x08a0 */
    volatile u_isp_fe_out_sum1      isp_fe_out_sum1; /* 0x08a4 */
    volatile unsigned int           reserved_14[537]; /* 0x08a8~0x1108, reserved 537 * 4 bytes */
    volatile u_isp_blc1_version     isp_blc1_version; /* 0x110c */
    volatile u_isp_blc1_offset1     isp_blc1_offset1; /* 0x1110 */
    volatile u_isp_blc1_offset2     isp_blc1_offset2; /* 0x1114 */
    volatile unsigned int           reserved_15[763]; /* 0x1118~0x1d00, reserved 763 * 4 bytes */
    volatile u_isp_dg2_blc_cfg      isp_dg2_blc_cfg; /* 0x1d04 */
    volatile unsigned int           reserved_16; /* 0x1d08 */
    volatile u_isp_dg2_version      isp_dg2_version; /* 0x1d0c */
    volatile u_isp_dg2_gain1        isp_dg2_gain1; /* 0x1d10 */
    volatile u_isp_dg2_gain2        isp_dg2_gain2; /* 0x1d14 */
    volatile u_isp_dg2_blc_offset1   isp_dg2_blc_offset1; /* 0x1d18 */
    volatile u_isp_dg2_blc_offset2   isp_dg2_blc_offset2; /* 0x1d1c */
    volatile u_isp_dg2_clip_value   isp_dg2_clip_value; /* 0x1d20 */
    volatile u_isp_dg2_gain3        isp_dg2_gain3; /* 0x1d24 */
    volatile u_isp_dg2_blc_offset3   isp_dg2_blc_offset3; /* 0x1d28 */
    volatile unsigned int           reserved_17[53]; /* 0x1d2c~0x1dfc, reserved 53 * 4 bytes */
    volatile u_isp_hrs_cfg          isp_hrs_cfg; /* 0x1e00 */
    volatile unsigned int           reserved_18[3]; /* 0x1e04~0x1e0c, reserved 3 * 4 bytes */
    volatile u_isp_hrs_filterlut0   isp_hrs_filterlut0; /* 0x1e10 */
    volatile u_isp_hrs_filterlut1   isp_hrs_filterlut1; /* 0x1e14 */
    volatile u_isp_hrs_filterlut2   isp_hrs_filterlut2; /* 0x1e18 */
    volatile u_isp_hrs_filterlut3   isp_hrs_filterlut3; /* 0x1e1c */
    volatile u_isp_hrs_filterlut4   isp_hrs_filterlut4; /* 0x1e20 */
    volatile u_isp_hrs_filterlut5   isp_hrs_filterlut5; /* 0x1e24 */
    volatile unsigned int           reserved_19[55]; /* 0x1e28~0x1f00, reserved 55 * 4 bytes */
    volatile u_isp_wb1_blc_cfg      isp_wb1_blc_cfg; /* 0x1f04 */
    volatile unsigned int           reserved_20; /* 0x1f08 */
    volatile u_isp_wb1_version      isp_wb1_version; /* 0x1f0c */
    volatile u_isp_wb1_gain1        isp_wb1_gain1; /* 0x1f10 */
    volatile u_isp_wb1_gain2        isp_wb1_gain2; /* 0x1f14 */
    volatile u_isp_wb1_blc_offset1   isp_wb1_blc_offset1; /* 0x1f18 */
    volatile u_isp_wb1_blc_offset2   isp_wb1_blc_offset2; /* 0x1f1c */
    volatile u_isp_wb1_clip_value   isp_wb1_clip_value; /* 0x1f20 */
    volatile u_isp_wb1_gain3        isp_wb1_gain3; /* 0x1f24 */
    volatile u_isp_wb1_blc_offset3   isp_wb1_blc_offset3; /* 0x1f28 */
    volatile unsigned int           reserved_21[56]; /* 0x1f2c~0x2008, reserved 56 * 4 bytes */
    volatile u_isp_ae_version       isp_ae_version; /* 0x200c */
    volatile u_isp_ae_zone          isp_ae_zone; /* 0x2010 */
    volatile u_isp_ae_skip_crg      isp_ae_skip_crg; /* 0x2014 */
    volatile u_isp_ae_total_stat    isp_ae_total_stat; /* 0x2018 */
    volatile u_isp_ae_count_stat    isp_ae_count_stat; /* 0x201c */
    volatile u_isp_ae_total_r_aver   isp_ae_total_r_aver; /* 0x2020 */
    volatile u_isp_ae_total_gr_aver   isp_ae_total_gr_aver; /* 0x2024 */
    volatile u_isp_ae_total_gb_aver   isp_ae_total_gb_aver; /* 0x2028 */
    volatile u_isp_ae_total_b_aver   isp_ae_total_b_aver; /* 0x202c */
    volatile u_isp_ae_hist_high     isp_ae_hist_high; /* 0x2030 */
    volatile u_isp_ae_total_ir_aver   isp_ae_total_ir_aver; /* 0x2034 */
    volatile unsigned int           reserved_22[2]; /* 0x2038~0x203c, reserved 2 * 4 bytes */
    volatile u_isp_ae_bitmove       isp_ae_bitmove; /* 0x2040 */
    volatile u_isp_ae_offset_r_gr   isp_ae_offset_r_gr; /* 0x2044 */
    volatile u_isp_ae_offset_gb_b   isp_ae_offset_gb_b; /* 0x2048 */
    volatile unsigned int           reserved_23[6]; /* 0x204c~0x2060, reserved 6 * 4 bytes */
    volatile u_isp_ae_lut_update    isp_ae_lut_update; /* 0x2064 */
    volatile u_isp_ae_stt_bst       isp_ae_stt_bst; /* 0x2068 */
    volatile unsigned int           reserved_24[2]; /* 0x206c~0x2070, reserved 2 * 4 bytes */
    volatile u_isp_ae_crop_pos      isp_ae_crop_pos; /* 0x2074 */
    volatile u_isp_ae_crop_outsize   isp_ae_crop_outsize; /* 0x2078 */
    volatile unsigned int           reserved_25; /* 0x207c */
    volatile u_isp_ae_hist_waddr    isp_ae_hist_waddr; /* 0x2080 */
    volatile u_isp_ae_hist_wdata    isp_ae_hist_wdata; /* 0x2084 */
    volatile u_isp_ae_hist_raddr    isp_ae_hist_raddr; /* 0x2088 */
    volatile u_isp_ae_hist_rdata    isp_ae_hist_rdata; /* 0x208c */
    volatile u_isp_ae_aver_r_gr_waddr   isp_ae_aver_r_gr_waddr; /* 0x2090 */
    volatile u_isp_ae_aver_r_gr_wdata   isp_ae_aver_r_gr_wdata; /* 0x2094 */
    volatile u_isp_ae_aver_r_gr_raddr   isp_ae_aver_r_gr_raddr; /* 0x2098 */
    volatile u_isp_ae_aver_r_gr_rdata   isp_ae_aver_r_gr_rdata; /* 0x209c */
    volatile u_isp_ae_aver_gb_b_waddr   isp_ae_aver_gb_b_waddr; /* 0x20a0 */
    volatile u_isp_ae_aver_gb_b_wdata   isp_ae_aver_gb_b_wdata; /* 0x20a4 */
    volatile u_isp_ae_aver_gb_b_raddr   isp_ae_aver_gb_b_raddr; /* 0x20a8 */
    volatile u_isp_ae_aver_gb_b_rdata   isp_ae_aver_gb_b_rdata; /* 0x20ac */
    volatile u_isp_ae_weight_waddr   isp_ae_weight_waddr; /* 0x20b0 */
    volatile u_isp_ae_weight_wdata   isp_ae_weight_wdata; /* 0x20b4 */
    volatile u_isp_ae_weight_raddr   isp_ae_weight_raddr; /* 0x20b8 */
    volatile u_isp_ae_weight_rdata   isp_ae_weight_rdata; /* 0x20bc */
    volatile u_isp_ae_ir_hist_waddr   isp_ae_ir_hist_waddr; /* 0x20c0 */
    volatile u_isp_ae_ir_hist_wdata   isp_ae_ir_hist_wdata; /* 0x20c4 */
    volatile u_isp_ae_ir_hist_raddr   isp_ae_ir_hist_raddr; /* 0x20c8 */
    volatile u_isp_ae_ir_hist_rdata   isp_ae_ir_hist_rdata; /* 0x20cc */
    volatile u_isp_ae_aver_ir_waddr   isp_ae_aver_ir_waddr; /* 0x20d0 */
    volatile u_isp_ae_aver_ir_wdata   isp_ae_aver_ir_wdata; /* 0x20d4 */
    volatile u_isp_ae_aver_ir_raddr   isp_ae_aver_ir_raddr; /* 0x20d8 */
    volatile u_isp_ae_aver_ir_rdata   isp_ae_aver_ir_rdata; /* 0x20dc */
    volatile u_isp_ae_ai_hist_waddr   isp_ae_ai_hist_waddr; /* 0x20e0 */
    volatile u_isp_ae_ai_hist_wdata   isp_ae_ai_hist_wdata; /* 0x20e4 */
    volatile u_isp_ae_ai_hist_raddr   isp_ae_ai_hist_raddr; /* 0x20e8 */
    volatile u_isp_ae_ai_hist_rdata   isp_ae_ai_hist_rdata; /* 0x20ec */
    volatile u_isp_ae_ai_aver_waddr   isp_ae_ai_aver_waddr; /* 0x20f0 */
    volatile u_isp_ae_ai_aver_wdata   isp_ae_ai_aver_wdata; /* 0x20f4 */
    volatile u_isp_ae_ai_aver_raddr   isp_ae_ai_aver_raddr; /* 0x20f8 */
    volatile u_isp_ae_ai_aver_rdata   isp_ae_ai_aver_rdata; /* 0x20fc */
    volatile u_isp_ae_smart_ctrl    isp_ae_smart_ctrl; /* 0x2100 */
    volatile u_isp_ae_smart0_pos    isp_ae_smart0_pos; /* 0x2104 */
    volatile u_isp_ae_smart0_size   isp_ae_smart0_size; /* 0x2108 */
    volatile u_isp_ae_smart1_pos    isp_ae_smart1_pos; /* 0x210c */
    volatile u_isp_ae_smart1_size   isp_ae_smart1_size; /* 0x2110 */
    volatile u_isp_ae_smart2_pos    isp_ae_smart2_pos; /* 0x2114 */
    volatile u_isp_ae_smart2_size   isp_ae_smart2_size; /* 0x2118 */
    volatile u_isp_ae_smart3_pos    isp_ae_smart3_pos; /* 0x211c */
    volatile u_isp_ae_smart3_size   isp_ae_smart3_size; /* 0x2120 */
    volatile u_isp_ae_smart4_pos    isp_ae_smart4_pos; /* 0x2124 */
    volatile u_isp_ae_smart4_size   isp_ae_smart4_size; /* 0x2128 */
    volatile u_isp_ae_smart5_pos    isp_ae_smart5_pos; /* 0x212c */
    volatile u_isp_ae_smart5_size   isp_ae_smart5_size; /* 0x2130 */
    volatile u_isp_ae_smart6_pos    isp_ae_smart6_pos; /* 0x2134 */
    volatile u_isp_ae_smart6_size   isp_ae_smart6_size; /* 0x2138 */
    volatile u_isp_ae_smart7_pos    isp_ae_smart7_pos; /* 0x213c */
    volatile u_isp_ae_smart7_size   isp_ae_smart7_size; /* 0x2140 */
    volatile u_isp_ae_smart8_pos    isp_ae_smart8_pos; /* 0x2144 */
    volatile u_isp_ae_smart8_size   isp_ae_smart8_size; /* 0x2148 */
    volatile u_isp_ae_smart9_pos    isp_ae_smart9_pos; /* 0x214c */
    volatile u_isp_ae_smart9_size   isp_ae_smart9_size; /* 0x2150 */
    volatile u_isp_ae_smart10_pos   isp_ae_smart10_pos; /* 0x2154 */
    volatile u_isp_ae_smart10_size   isp_ae_smart10_size; /* 0x2158 */
    volatile u_isp_ae_smart11_pos   isp_ae_smart11_pos; /* 0x215c */
    volatile u_isp_ae_smart11_size   isp_ae_smart11_size; /* 0x2160 */
    volatile u_isp_ae_smart12_pos   isp_ae_smart12_pos; /* 0x2164 */
    volatile u_isp_ae_smart12_size   isp_ae_smart12_size; /* 0x2168 */
    volatile u_isp_ae_smart13_pos   isp_ae_smart13_pos; /* 0x216c */
    volatile u_isp_ae_smart13_size   isp_ae_smart13_size; /* 0x2170 */
    volatile u_isp_ae_smart14_pos   isp_ae_smart14_pos; /* 0x2174 */
    volatile u_isp_ae_smart14_size   isp_ae_smart14_size; /* 0x2178 */
    volatile u_isp_ae_smart15_pos   isp_ae_smart15_pos; /* 0x217c */
    volatile u_isp_ae_smart15_size   isp_ae_smart15_size; /* 0x2180 */
    volatile unsigned int           reserved_26[31]; /* 0x2184~0x21fc, reserved 31 * 4 bytes */
    volatile u_isp_af_cfg           isp_af_cfg; /* 0x2200 */
    volatile unsigned int           reserved_27[2]; /* 0x2204~0x2208, reserved 2 * 4 bytes */
    volatile u_isp_af_version       isp_af_version; /* 0x220c */
    volatile u_isp_af_zone          isp_af_zone; /* 0x2210 */
    volatile u_isp_af_crop_start    isp_af_crop_start; /* 0x2214 */
    volatile u_isp_af_crop_size     isp_af_crop_size; /* 0x2218 */
    volatile u_isp_af_mean_thres    isp_af_mean_thres; /* 0x221c */
    volatile u_isp_af_iirg0         isp_af_iirg0; /* 0x2220 */
    volatile u_isp_af_iirg1         isp_af_iirg1; /* 0x2224 */
    volatile u_isp_af_iirg2         isp_af_iirg2; /* 0x2228 */
    volatile u_isp_af_iirg3         isp_af_iirg3; /* 0x222c */
    volatile u_isp_af_iirg4         isp_af_iirg4; /* 0x2230 */
    volatile u_isp_af_iirg5         isp_af_iirg5; /* 0x2234 */
    volatile u_isp_af_iirg6         isp_af_iirg6; /* 0x2238 */
    volatile u_isp_af_iirpl         isp_af_iirpl; /* 0x223c */
    volatile u_isp_af_shift         isp_af_shift; /* 0x2240 */
    volatile unsigned int           reserved_28[3]; /* 0x2244~0x224c, reserved 3 * 4 bytes */
    volatile u_isp_af_firh0         isp_af_firh0; /* 0x2250 */
    volatile u_isp_af_firh1         isp_af_firh1; /* 0x2254 */
    volatile u_isp_af_firh2         isp_af_firh2; /* 0x2258 */
    volatile u_isp_af_firh3         isp_af_firh3; /* 0x225c */
    volatile u_isp_af_firh4         isp_af_firh4; /* 0x2260 */
    volatile u_isp_af_stt_bst       isp_af_stt_bst; /* 0x2264 */
    volatile u_isp_af_stt_abn       isp_af_stt_abn; /* 0x2268 */
    volatile unsigned int           reserved_29[3]; /* 0x226c~0x2274, reserved 3 * 4 bytes */
    volatile u_isp_af_acc_shift     isp_af_acc_shift; /* 0x2278 */
    volatile u_isp_af_cnt_shift     isp_af_cnt_shift; /* 0x227c */
    volatile unsigned int           reserved_30[2]; /* 0x2280~0x2284, reserved 2 * 4 bytes */
    volatile u_isp_af_stat_raddr    isp_af_stat_raddr; /* 0x2288 */
    volatile u_isp_af_stat_rdata    isp_af_stat_rdata; /* 0x228c */
    volatile unsigned int           reserved_31[28]; /* 0x2290~0x22fc, reserved 28 * 4 bytes */
    volatile u_isp_af_iirthre       isp_af_iirthre; /* 0x2300 */
    volatile u_isp_af_iirgain       isp_af_iirgain; /* 0x2304 */
    volatile u_isp_af_iirslope      isp_af_iirslope; /* 0x2308 */
    volatile u_isp_af_iirdilate     isp_af_iirdilate; /* 0x230c */
    volatile u_isp_af_firthre       isp_af_firthre; /* 0x2310 */
    volatile u_isp_af_firgain       isp_af_firgain; /* 0x2314 */
    volatile u_isp_af_firslope      isp_af_firslope; /* 0x2318 */
    volatile unsigned int           reserved_32; /* 0x231c */
    volatile u_isp_af_iirthre_coring   isp_af_iirthre_coring; /* 0x2320 */
    volatile u_isp_af_iirpeak_coring   isp_af_iirpeak_coring; /* 0x2324 */
    volatile u_isp_af_iirslope_coring   isp_af_iirslope_coring; /* 0x2328 */
    volatile unsigned int           reserved_33; /* 0x232c */
    volatile u_isp_af_firthre_coring   isp_af_firthre_coring; /* 0x2330 */
    volatile u_isp_af_firpeak_coring   isp_af_firpeak_coring; /* 0x2334 */
    volatile u_isp_af_firslope_coring   isp_af_firslope_coring; /* 0x2338 */
    volatile unsigned int           reserved_34; /* 0x233c */
    volatile u_isp_af_highlight       isp_af_highlight; /* 0x2340 */
    volatile u_isp_af_offset        isp_af_offset; /* 0x2344 */
    volatile unsigned int           reserved_35[1202]; /* 0x2348~0x360c, reserved 1202 * 4 bytes */
    volatile u_isp_rc_blcen         isp_rc_blcen; /* 0x3610 */
    volatile u_isp_rc_blc0          isp_rc_blc0; /* 0x3614 */
    volatile u_isp_rc_blc1          isp_rc_blc1; /* 0x3618 */
    volatile unsigned int           reserved_36; /* 0x361c */
    volatile u_isp_rc_sqradius      isp_rc_sqradius; /* 0x3620 */
    volatile u_isp_rc_center_coor   isp_rc_center_coor; /* 0x3624 */
    volatile unsigned int           reserved_37[7801]; /* 0x3628~0xb008, reserved 7801 * 4 bytes */
    volatile u_isp_colorbar_version   isp_colorbar_version; /* 0xb00c */
    volatile u_isp_colorbar_pattern   isp_colorbar_pattern; /* 0xb010 */
    volatile u_isp_colorbar_rbackgnd   isp_colorbar_rbackgnd; /* 0xb014 */
    volatile u_isp_colorbar_gbackgnd   isp_colorbar_gbackgnd; /* 0xb018 */
    volatile u_isp_colorbar_bbackgnd   isp_colorbar_bbackgnd; /* 0xb01c */
    volatile u_isp_colorbar_rforegnd   isp_colorbar_rforegnd; /* 0xb020 */
    volatile u_isp_colorbar_gforegnd   isp_colorbar_gforegnd; /* 0xb024 */
    volatile u_isp_colorbar_bforegnd   isp_colorbar_bforegnd; /* 0xb028 */
    volatile u_isp_colorbar_init    isp_colorbar_init; /* 0xb02c */
    volatile u_isp_colorbar_grad    isp_colorbar_grad; /* 0xb030 */
    volatile u_isp_colorbar_pos1    isp_colorbar_pos1; /* 0xb034 */
    volatile u_isp_colorbar_pos2    isp_colorbar_pos2; /* 0xb038 */
    volatile unsigned int           reserved_38[52]; /* 0xb03c~0xb108, reserved 52 * 4 bytes */
    volatile u_isp_crop_version     isp_crop_version; /* 0xb10c */
    volatile u_isp_crop_sizeout     isp_crop_sizeout; /* 0xb110 */
    volatile u_isp_crop_pos         isp_crop_pos; /* 0xb114 */
    volatile unsigned int           reserved_39[58]; /* 0xb118~0xb1fc, reserved 58 * 4 bytes */
    volatile u_isp_blc_dynamic_reg0   isp_blc_dynamic_reg0; /* 0xb200 */
    volatile u_isp_blc_dynamic_reg1   isp_blc_dynamic_reg1; /* 0xb204 */
    volatile u_isp_blc_dynamic_reg2   isp_blc_dynamic_reg2; /* 0xb208 */
    volatile u_isp_blc_dynamic_reg3   isp_blc_dynamic_reg3; /* 0xb20c */
    volatile u_isp_blc_dynamic_reg4   isp_blc_dynamic_reg4; /* 0xb210 */
    volatile u_isp_blc_dynamic_reg5   isp_blc_dynamic_reg5; /* 0xb214 */
} isp_fe_reg_type;

/* pre be */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_patch             : 8; /* [7..0]  */
        unsigned int    isp_build             : 8; /* [15..8]  */
        unsigned int    isp_release           : 8; /* [23..16]  */
        unsigned int    isp_version           : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_version;

/* Define the union u_isp_be_date */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_date               : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_date;
/* Define the union u_isp_be_fpga_date */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpga_date          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_fpga_date;
/* Define the union u_isp_be_module_pos */
/* isp_ae_sel 0/1/2 for post be  3/4 for pre be */
/* isp_rgbir_sel for pre be */
/* isp_clut_sel for post be */
/* isp_dis_sel for post be */
/* isp_af_sel for post be */
/* isp_awb_sel 0/2 for post be  not support 1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 2; /* [1..0]  */
        unsigned int    isp_awb_sel           : 2; /* [3..2]  */
        unsigned int    isp_af_sel            : 2; /* [5..4]  */
        unsigned int    isp_dis_sel           : 1; /* [6]  */
        unsigned int    isp_clut_sel          : 1; /* [7]  */
        unsigned int    isp_rgbir_sel         : 1; /* [8]  */
        unsigned int    reserved_1            : 3; /* [11..9]  */
        unsigned int    isp_ae_sel            : 3; /* [14..12]  */
        unsigned int    reserved_2            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_module_pos;

/* Define the union u_isp_be_fstart_delay */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_delay              : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_fstart_delay;
/* Define the union u_isp_be_input_mux */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_input0_sel        : 2; /* [1..0]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    isp_input1_sel        : 2; /* [5..4]  */
        unsigned int    reserved_1            : 2; /* [7..6]  */
        unsigned int    isp_input2_sel        : 2; /* [9..8]  */
        unsigned int    reserved_2            : 2; /* [11..10]  */
        unsigned int    isp_input3_sel        : 2; /* [13..12]  */
        unsigned int    reserved_3            : 2; /* [15..14]  */
        unsigned int    isp_input4_sel        : 1; /* [16]  */
        unsigned int    reserved_4            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_input_mux;

/* Define the union u_isp_be_sys_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_stt_en            : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_sys_ctrl;

/* Define the union u_isp_be_user_define0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_user_define0       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_user_define0;
/* Define the union u_isp_be_user_define1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_user_define1       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_user_define1;
/* Define the union u_isp_be_startup */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fcnt               : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_startup;
/* Define the union u_isp_be_format */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_format            : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_be_format;

/* Define the union u_isp_clip_y_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clip_y_min        : 16; /* [15..0]  */
        unsigned int    isp_clip_y_max        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clip_y_cfg;

/* Define the union u_isp_clip_c_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clip_c_min        : 16; /* [15..0]  */
        unsigned int    isp_clip_c_max        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clip_c_cfg;

/* Define the union u_isp_csc_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_csc_sum_en        : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_sum_cfg;

/* Define the union u_isp_yuv444_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_yuv444_sum_en     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_sum_cfg;

/* Define the union u_isp_yuv422_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_yuv422_sum_en     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv422_sum_cfg;

/* Define the union u_isp_wdr_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_sum_en        : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_sum_cfg;

/* Define the union u_isp_demosaic_sum_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_sum_en   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_sum_cfg;

/* Define the union u_isp_dmnr_dither */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dmnr_dither_en    : 1; /* [0]  */
        unsigned int    isp_dmnr_dither_round : 2; /* [2..1]  */
        unsigned int    isp_dmnr_dither_spatial_mode : 1; /* [3]  */
        unsigned int    isp_dmnr_dither_out_bits : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dmnr_dither;

/* Define the union u_isp_acm_dither */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_acm_dither_en     : 1; /* [0]  */
        unsigned int    isp_acm_dither_round  : 2; /* [2..1]  */
        unsigned int    isp_acm_dither_spatial_mode : 1; /* [3]  */
        unsigned int    isp_acm_dither_out_bits : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_acm_dither;

/* Define the union u_isp_drc_dither */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_drc_dither_en     : 1; /* [0]  */
        unsigned int    isp_drc_dither_round  : 2; /* [2..1]  */
        unsigned int    isp_drc_dither_spatial_mode : 1; /* [3]  */
        unsigned int    isp_drc_dither_out_bits : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_drc_dither;

/* Define the union u_isp_sqrt1_dither */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sqrt1_dither_en   : 1; /* [0]  */
        unsigned int    isp_sqrt1_dither_round : 2; /* [2..1]  */
        unsigned int    isp_sqrt1_dither_spatial_mode : 1; /* [3]  */
        unsigned int    isp_sqrt1_dither_out_bits : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sqrt1_dither;

/* Define the union u_isp_sharpen_dither */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dither_en : 1; /* [0]  */
        unsigned int    isp_sharpen_dither_round : 2; /* [2..1]  */
        unsigned int    isp_sharpen_dither_spatial_mode : 1; /* [3]  */
        unsigned int    isp_sharpen_dither_out_bits : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dither;

/* Define the union u_isp_blk_hblank */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blk_b_hblank      : 16; /* [15..0]  */
        unsigned int    isp_blk_f_hblank      : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blk_hblank;

/* Define the union u_isp_blk_vblank */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blk_b_vblank      : 16; /* [15..0]  */
        unsigned int    isp_blk_f_vblank      : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blk_vblank;

/* Define the union u_isp_y_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_y_sum0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_y_sum0;
/* Define the union u_isp_y_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_y_sum1             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_y_sum1;
/* Define the union u_isp_yuv444_y_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv444_y_sum0      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_y_sum0;
/* Define the union u_isp_yuv444_y_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv444_y_sum1      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_y_sum1;
/* Define the union u_isp_yuv444_u_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv444_u_sum0      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_u_sum0;
/* Define the union u_isp_yuv444_u_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv444_u_sum1      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_u_sum1;
/* Define the union u_isp_yuv444_v_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv444_v_sum0      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_v_sum0;
/* Define the union u_isp_yuv444_v_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv444_v_sum1      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv444_v_sum1;
/* Define the union u_isp_yuv422_y_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv422_y_sum0      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv422_y_sum0;
/* Define the union u_isp_yuv422_y_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv422_y_sum1      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv422_y_sum1;
/* Define the union u_isp_yuv422_c_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv422_c_sum0      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv422_c_sum0;
/* Define the union u_isp_yuv422_c_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_yuv422_c_sum1      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_yuv422_c_sum1;
/* Define the union u_isp_wdr0_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr0_sum0          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr0_sum0;
/* Define the union u_isp_wdr0_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr0_sum1          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr0_sum1;
/* Define the union u_isp_wdr1_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr1_sum0          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr1_sum0;
/* Define the union u_isp_wdr1_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr1_sum1          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr1_sum1;
/* Define the union u_isp_wdr2_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr2_sum0          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr2_sum0;
/* Define the union u_isp_wdr2_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr2_sum1          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr2_sum1;
/* Define the union u_isp_wdr3_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr3_sum0          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr3_sum0;
/* Define the union u_isp_wdr3_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr3_sum1          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr3_sum1;
/* Define the union u_isp_demosaic_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_demosaic_sum0      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_sum0;
/* Define the union u_isp_demosaic_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_demosaic_sum1      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_sum1;
/* Define the union u_isp_4dg_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg_en_in         : 1; /* [0]  */
        unsigned int    isp_4dg_en_out        : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_blc_cfg;

/* Define the union u_isp_4dg_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_4dg_version        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_version;
/* Define the union u_isp_4dg_0_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg0_grgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg0_rgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_0_gain1;

/* Define the union u_isp_4dg_0_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg0_gbgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg0_bgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_0_gain2;

/* Define the union u_isp_4dg_0_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg0_ofsgr        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg0_ofsr         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_0_blc_offset1;

/* Define the union u_isp_4dg_0_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg0_ofsgb        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg0_ofsb         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_0_blc_offset2;

/* Define the union u_isp_4dg_1_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg1_grgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg1_rgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_1_gain1;

/* Define the union u_isp_4dg_1_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg1_gbgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg1_bgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_1_gain2;

/* Define the union u_isp_4dg_1_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg1_ofsgr        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg1_ofsr         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_1_blc_offset1;

/* Define the union u_isp_4dg_1_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg1_ofsgb        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg1_ofsb         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_1_blc_offset2;

/* Define the union u_isp_4dg_2_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg2_grgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg2_rgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_2_gain1;

/* Define the union u_isp_4dg_2_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg2_gbgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg2_bgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_2_gain2;

/* Define the union u_isp_4dg_2_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg2_ofsgr        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg2_ofsr         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_2_blc_offset1;

/* Define the union u_isp_4dg_2_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg2_ofsgb        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg2_ofsb         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_2_blc_offset2;

/* Define the union u_isp_4dg_3_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg3_grgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg3_rgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_3_gain1;

/* Define the union u_isp_4dg_3_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg3_gbgain       : 16; /* [15..0]  */
        unsigned int    isp_4dg3_bgain        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_3_gain2;

/* Define the union u_isp_4dg_3_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg3_ofsgr        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg3_ofsr         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_3_blc_offset1;

/* Define the union u_isp_4dg_3_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg3_ofsgb        : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_4dg3_ofsb         : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_3_blc_offset2;

/* Define the union u_isp_4dg_0_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg0_clip_value   : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_0_clip_value;

/* Define the union u_isp_4dg_1_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg1_clip_value   : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_1_clip_value;

/* Define the union u_isp_4dg_2_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg2_clip_value   : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_2_clip_value;

/* Define the union u_isp_4dg_3_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_4dg3_clip_value   : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_4dg_3_clip_value;

/* Define the union u_isp_flick_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 28; /* [27..0]  */
        unsigned int    isp_flick_mergeframe  : 3; /* [30..28]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_ctrl;

/* Define the union u_isp_flick_f0_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f0_inblc_gr : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f0_inblc_r  : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f0_inblc0;

/* Define the union u_isp_flick_f0_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f0_inblc_b  : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f0_inblc_gb : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f0_inblc1;

/* Define the union u_isp_flick_f1_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f1_inblc_gr : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f1_inblc_r  : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f1_inblc0;

/* Define the union u_isp_flick_f1_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f1_inblc_b  : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f1_inblc_gb : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f1_inblc1;

/* Define the union u_isp_flick_f2_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f2_inblc_gr : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f2_inblc_r  : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f2_inblc0;

/* Define the union u_isp_flick_f2_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f2_inblc_b  : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f2_inblc_gb : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f2_inblc1;

/* Define the union u_isp_flick_f3_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f3_inblc_gr : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f3_inblc_r  : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f3_inblc0;

/* Define the union u_isp_flick_f3_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_f3_inblc_b  : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_flick_f3_inblc_gb : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_f3_inblc1;

/* Define the union u_isp_flick_exporatio0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_exporatio1  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_flick_exporatio0  : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_exporatio0;

/* Define the union u_isp_flick_exporatio1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_exporatio2  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_exporatio1;

/* Define the union u_isp_flick_overth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_overth      : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_overth;

/* Define the union u_isp_flick_overcountth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_overcountth : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_overcountth;

/* Define the union u_isp_flick_gavg_pre */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gbavg_pre   : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_flick_gravg_pre   : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gavg_pre;

/* Define the union u_isp_flick_countover_pre */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_countover_pre : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_countover_pre;

/* Define the union u_isp_flick_countover_cur */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_countover_cur : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_countover_cur;

/* Define the union u_isp_flick_gr_diff_cur */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gr_diff_cur : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gr_diff_cur;

/* Define the union u_isp_flick_gb_diff_cur */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gb_diff_cur : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gb_diff_cur;

/* Define the union u_isp_flick_gr_abs_cur */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gr_abs_cur  : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gr_abs_cur;

/* Define the union u_isp_flick_gb_abs_cur */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gb_abs_cur  : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gb_abs_cur;

/* Define the union u_isp_flick_gmean_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gmean_waddr;
/* Define the union u_isp_flick_gmean_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gmean_wdata;
/* Define the union u_isp_flick_gmean_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gmean_raddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gmean_raddr;

/* Define the union u_isp_flick_gmean_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gbmean_rdata : 11; /* [10..0]  */
        unsigned int    isp_flick_grmean_rdata : 11; /* [21..11]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gmean_rdata;

/* Define the union u_isp_fpn_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 10; /* [9..0]  */
        unsigned int    isp_fpn_calib_offline : 1; /* [10]  */
        unsigned int    reserved_1            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_cfg;

/* Define the union u_isp_fpn_calib_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_calib_start   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_calib_start;

/* Define the union u_isp_fpn_corr_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_correct0_en   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_corr_cfg;

/* Define the union u_isp_fpn_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_busy          : 1; /* [0]  */
        unsigned int    reserved_0            : 7; /* [7..1]  */
        unsigned int    isp_fpn_vcnt          : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_fpn_hcnt          : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_stat;

/* Define the union u_isp_fpn_white_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_white_level   : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_white_level;

/* Define the union u_isp_fpn_divcoef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_divcoef       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_divcoef;

/* Define the union u_isp_fpn_framelog2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_framelog2     : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_framelog2;

/* Define the union u_isp_fpn_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpn_sum0           : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_sum0;
/* Define the union u_isp_fpn_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpn_sum1           : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_sum1;
/* Define the union u_isp_fpn_corr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_offset0       : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_fpn_strength0     : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_corr0;

/* Define the union u_isp_fpn_shift */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_shift         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_fpn_in_shift      : 4; /* [11..8]  */
        unsigned int    reserved_1            : 4; /* [15..12]  */
        unsigned int    isp_fpn_out_shift     : 4; /* [19..16]  */
        unsigned int    reserved_2            : 4; /* [23..20]  */
        unsigned int    isp_fpn_frame_calib_shift : 4; /* [27..24]  */
        unsigned int    reserved_3            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_shift;

/* Define the union u_isp_fpn_max_o */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_max_o         : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_max_o;

/* Define the union u_isp_fpn_overflowthr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_overflowthr   : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_overflowthr;

/* Define the union u_isp_fpn_black_frame_out_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_data0_out_sel : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_black_frame_out_sel;

/* Define the union u_isp_fpn_overlap_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_overlap_width_l : 16; /* [15..0]  */
        unsigned int    isp_fpn_overlap_width_r : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_overlap_cfg;

/* Define the union u_isp_fpn_calib_offline_set */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_calib_offline_set : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_calib_offline_set;

/* Define the union u_isp_fpn_calib_stat_clear */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_calib_stat_clear : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_calib_stat_clear;

/* Define the union u_isp_fpn1_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 10; /* [9..0]  */
        unsigned int    isp_fpn1_calib_offline : 1; /* [10]  */
        unsigned int    reserved_1            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_cfg;

/* Define the union u_isp_fpn1_calib_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_calib_start  : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_calib_start;

/* Define the union u_isp_fpn1_corr_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_correct0_en  : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_corr_cfg;

/* Define the union u_isp_fpn1_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_busy         : 1; /* [0]  */
        unsigned int    reserved_0            : 7; /* [7..1]  */
        unsigned int    isp_fpn1_vcnt         : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_fpn1_hcnt         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_stat;

/* Define the union u_isp_fpn1_white_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_white_level  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_white_level;

/* Define the union u_isp_fpn1_divcoef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_divcoef      : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_divcoef;

/* Define the union u_isp_fpn1_framelog2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_framelog2    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_framelog2;

/* Define the union u_isp_fpn1_sum0 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpn1_sum0          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_sum0;
/* Define the union u_isp_fpn1_sum1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpn1_sum1          : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_sum1;
/* Define the union u_isp_fpn1_corr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_offset0      : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_fpn1_strength0    : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_corr0;

/* Define the union u_isp_fpn1_shift */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_shift        : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_fpn1_in_shift     : 4; /* [11..8]  */
        unsigned int    reserved_1            : 4; /* [15..12]  */
        unsigned int    isp_fpn1_out_shift    : 4; /* [19..16]  */
        unsigned int    reserved_2            : 4; /* [23..20]  */
        unsigned int    isp_fpn1_frame_calib_shift : 4; /* [27..24]  */
        unsigned int    reserved_3            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_shift;

/* Define the union u_isp_fpn1_max_o */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_max_o        : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_max_o;

/* Define the union u_isp_fpn1_overflowthr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_overflowthr  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_overflowthr;

/* Define the union u_isp_fpn1_black_frame_out_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_data0_out_sel : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_black_frame_out_sel;

/* Define the union u_isp_fpn1_overlap_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_overlap_width_l : 16; /* [15..0]  */
        unsigned int    isp_fpn1_overlap_width_r : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_overlap_cfg;

/* Define the union u_isp_fpn1_calib_offline_set */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_calib_offline_set : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_calib_offline_set;

/* Define the union u_isp_fpn1_calib_stat_clear */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn1_calib_stat_clear : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn1_calib_stat_clear;

/* Define the union u_isp_dpc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_dpc_dpc_en1       : 1; /* [1]  */
        unsigned int    reserved_1            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_cfg;

/* Define the union u_isp_dpc_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_alpha1_g      : 8; /* [7..0]  */
        unsigned int    isp_dpc_alpha1_rb     : 8; /* [15..8]  */
        unsigned int    isp_dpc_alpha0_g      : 8; /* [23..16]  */
        unsigned int    isp_dpc_alpha0_rb     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_alpha;

/* Define the union u_isp_dpc_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dpc_version        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_version;
/* Define the union u_isp_dpc_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_det_en        : 1; /* [0]  */
        unsigned int    isp_dpc_cor_en        : 1; /* [1]  */
        unsigned int    isp_dpc_six_det_en    : 1; /* [2]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    isp_dpc_grayscale_mode : 1; /* [4]  */
        unsigned int    isp_dpc_bpt_cor_en    : 1; /* [5]  */
        unsigned int    isp_dpc_dpt_det_sel   : 1; /* [6]  */
        unsigned int    isp_dpc_dp_hightlight_en : 1; /* [7]  */
        unsigned int    isp_dpc_ir_position   : 1; /* [8]  */
        unsigned int    isp_dpc_ir_channel    : 1; /* [9]  */
        unsigned int    reserved_1            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_mode;

/* Define the union u_isp_dpc_output_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_stage1_incl_gr_center : 1; /* [0]  */
        unsigned int    isp_dpc_stage1_incl_rb_center : 1; /* [1]  */
        unsigned int    isp_dpc_stage1_incl_g_3x3 : 1; /* [2]  */
        unsigned int    isp_dpc_stage1_incl_rb_3x3 : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_output_mode;

/* Define the union u_isp_dpc_set_use */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_stage1_use_set1 : 1; /* [0]  */
        unsigned int    isp_dpc_stage1_use_set2 : 1; /* [1]  */
        unsigned int    isp_dpc_stage1_use_set3 : 1; /* [2]  */
        unsigned int    isp_dpc_stage1_use_fix_set : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_set_use;

/* Define the union u_isp_dpc_methods_set_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pg_green1_enable : 1; /* [0]  */
        unsigned int    isp_dpc_lc_green1_enable : 1; /* [1]  */
        unsigned int    isp_dpc_ro_green1_enable : 1; /* [2]  */
        unsigned int    isp_dpc_rnd_green1_enable : 1; /* [3]  */
        unsigned int    isp_dpc_rg_green1_enable : 1; /* [4]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_dpc_pg_red_blue1_enable : 1; /* [8]  */
        unsigned int    isp_dpc_lc_red_blue1_enable : 1; /* [9]  */
        unsigned int    isp_dpc_ro_red_blue1_enable : 1; /* [10]  */
        unsigned int    isp_dpc_rnd_red_blue1_enable : 1; /* [11]  */
        unsigned int    isp_dpc_rg_red_blue1_enable : 1; /* [12]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_methods_set_1;

/* Define the union u_isp_dpc_methods_set_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pg_green2_enable : 1; /* [0]  */
        unsigned int    isp_dpc_lc_green2_enable : 1; /* [1]  */
        unsigned int    isp_dpc_ro_green2_enable : 1; /* [2]  */
        unsigned int    isp_dpc_rnd_green2_enable : 1; /* [3]  */
        unsigned int    isp_dpc_rg_green2_enable : 1; /* [4]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_dpc_pg_red_blue2_enable : 1; /* [8]  */
        unsigned int    isp_dpc_lc_red_blue2_enable : 1; /* [9]  */
        unsigned int    isp_dpc_ro_red_blue2_enable : 1; /* [10]  */
        unsigned int    isp_dpc_rnd_red_blue2_enable : 1; /* [11]  */
        unsigned int    isp_dpc_rg_red_blue2_enable : 1; /* [12]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_methods_set_2;

/* Define the union u_isp_dpc_methods_set_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pg_green3_enable : 1; /* [0]  */
        unsigned int    isp_dpc_lc_green3_enable : 1; /* [1]  */
        unsigned int    isp_dpc_ro_green3_enable : 1; /* [2]  */
        unsigned int    isp_dpc_rnd_green3_enable : 1; /* [3]  */
        unsigned int    isp_dpc_rg_green3_enable : 1; /* [4]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_dpc_pg_red_blue3_enable : 1; /* [8]  */
        unsigned int    isp_dpc_lc_red_blue3_enable : 1; /* [9]  */
        unsigned int    isp_dpc_ro_red_blue3_enable : 1; /* [10]  */
        unsigned int    isp_dpc_rnd_red_blue3_enable : 1; /* [11]  */
        unsigned int    isp_dpc_rg_red_blue3_enable : 1; /* [12]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_methods_set_3;

/* Define the union u_isp_dpc_line_thresh_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_thr1_g   : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_thr1_rb  : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_thresh_1;

/* Define the union u_isp_dpc_line_mad_fac_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_mad_fac_1_g : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_line_mad_fac_1_rb : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_mad_fac_1;

/* Define the union u_isp_dpc_pg_fac_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pg_fac_1_g    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_pg_fac_1_rb   : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_pg_fac_1;

/* Define the union u_isp_dpc_rnd_thresh_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_thr1_g    : 8; /* [7..0]  */
        unsigned int    isp_dpc_rnd_thr1_rb   : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_thresh_1;

/* Define the union u_isp_dpc_rg_fac_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rg_fac_1_g    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_rg_fac_1_rb   : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rg_fac_1;

/* Define the union u_isp_dpc_line_thresh_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_thr2_g   : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_thr2_rb  : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_thresh_2;

/* Define the union u_isp_dpc_line_mad_fac_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_mad_fac_2_g : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_line_mad_fac_2_rb : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_mad_fac_2;

/* Define the union u_isp_dpc_pg_fac_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pg_fac_2_g    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_pg_fac_2_rb   : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_pg_fac_2;

/* Define the union u_isp_dpc_rnd_thresh_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_thr2_g    : 8; /* [7..0]  */
        unsigned int    isp_dpc_rnd_thr2_rb   : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_thresh_2;

/* Define the union u_isp_dpc_rg_fac_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rg_fac_2_g    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_rg_fac_2_rb   : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rg_fac_2;

/* Define the union u_isp_dpc_line_thresh_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_thr3_g   : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_thr3_rb  : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_thresh_3;

/* Define the union u_isp_dpc_line_mad_fac_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_mad_fac_3_g : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_line_mad_fac_3_rb : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_mad_fac_3;

/* Define the union u_isp_dpc_pg_fac_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pg_fac_3_g    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_pg_fac_3_rb   : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_pg_fac_3;

/* Define the union u_isp_dpc_rnd_thresh_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_thr3_g    : 8; /* [7..0]  */
        unsigned int    isp_dpc_rnd_thr3_rb   : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_thresh_3;

/* Define the union u_isp_dpc_rg_fac_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rg_fac_3_g    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc_rg_fac_3_rb   : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rg_fac_3;

/* Define the union u_isp_dpc_ro_limits */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_ro_1_g        : 2; /* [1..0]  */
        unsigned int    isp_dpc_ro_1_rb       : 2; /* [3..2]  */
        unsigned int    isp_dpc_ro_2_g        : 2; /* [5..4]  */
        unsigned int    isp_dpc_ro_2_rb       : 2; /* [7..6]  */
        unsigned int    isp_dpc_ro_3_g        : 2; /* [9..8]  */
        unsigned int    isp_dpc_ro_3_rb       : 2; /* [11..10]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_ro_limits;

/* Define the union u_isp_dpc_rnd_offs */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_offs_1_g  : 2; /* [1..0]  */
        unsigned int    isp_dpc_rnd_offs_1_rb : 2; /* [3..2]  */
        unsigned int    isp_dpc_rnd_offs_2_g  : 2; /* [5..4]  */
        unsigned int    isp_dpc_rnd_offs_2_rb : 2; /* [7..6]  */
        unsigned int    isp_dpc_rnd_offs_3_g  : 2; /* [9..8]  */
        unsigned int    isp_dpc_rnd_offs_3_rb : 2; /* [11..10]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_offs;

/* Define the union u_isp_dpc_bpt_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 4; /* [3..0]  */
        unsigned int    isp_dpc_bpt_use_set_1 : 1; /* [4]  */
        unsigned int    isp_dpc_bpt_use_set_2 : 1; /* [5]  */
        unsigned int    isp_dpc_bpt_use_set_3 : 1; /* [6]  */
        unsigned int    isp_dpc_bpt_use_fix_set : 1; /* [7]  */
        unsigned int    isp_dpc_bpt_incl_green_center : 1; /* [8]  */
        unsigned int    isp_dpc_bpt_incl_rb_center : 1; /* [9]  */
        unsigned int    isp_dpc_g_3x3         : 1; /* [10]  */
        unsigned int    isp_dpc_rb_3x3        : 1; /* [11]  */
        unsigned int    reserved_1            : 4; /* [15..12]  */
        unsigned int    isp_dpc_bp_data       : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_ctrl;

/* Define the union u_isp_dpc_bpt_number */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_bpt_number    : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_number;

/* Define the union u_isp_dpc_bpt_calib_number */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_bpt_calib_number : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_calib_number;

/* Define the union u_isp_dpc_bpt_thrd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_dev_dead_thresh : 8; /* [7..0]  */
        unsigned int    isp_dpc_abs_dead_thresh : 8; /* [15..8]  */
        unsigned int    isp_dpc_dev_hot_thresh : 8; /* [23..16]  */
        unsigned int    isp_dpc_abs_hot_thresh : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_thrd;

/* Define the union u_isp_dpc_bpt_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dpc_bpt_waddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_waddr;
/* Define the union u_isp_dpc_bpt_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_bpt_posx_wdata : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc_bpt_posy_wdata : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_wdata;

/* Define the union u_isp_dpc_bpt_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dpc_bpt_raddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_raddr;
/* Define the union u_isp_dpc_bpt_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_bpt_posx_rdata : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc_bpt_posy_rdata : 13; /* [28..16]  */
        unsigned int    isp_dpc_bpt_rdata     : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_rdata;

/* Define the union u_isp_dpc_soft_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_ex_soft_thr_min : 8; /* [7..0]  */
        unsigned int    isp_dpc_ex_soft_thr_max : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_soft_thr;

/* Define the union u_isp_dpc_bhardthr_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_ex_hard_thr_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bhardthr_en;

/* Define the union u_isp_dpc_rakeratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_ex_rake_ratio : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rakeratio;

/* Define the union u_isp_dpc_lch_thr_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_aver_fac_1 : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_diff_thr_1 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc_line_std_thr_1 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_lch_thr_1;

/* Define the union u_isp_dpc_lch_thr_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_aver_fac_2 : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_diff_thr_2 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc_line_std_thr_2 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_lch_thr_2;

/* Define the union u_isp_dpc_lch_thr_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_aver_fac_3 : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_diff_thr_3 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc_line_std_thr_3 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_lch_thr_3;

/* Define the union u_isp_dpc_lch_thr_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_aver_fac_4 : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_diff_thr_4 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc_line_std_thr_4 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_lch_thr_4;

/* Define the union u_isp_dpc_lch_thr_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_aver_fac_5 : 8; /* [7..0]  */
        unsigned int    isp_dpc_line_diff_thr_5 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc_line_std_thr_5 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_lch_thr_5;

/* Define the union u_isp_dpc_line_kerdiff_fac */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_line_kerdiff_fac : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_line_kerdiff_fac;

/* Define the union u_isp_dpc_blend_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_blend_mode    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_blend_mode;

/* Define the union u_isp_dpc_bit_depth_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_bit_depth_sel : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bit_depth_sel;

/* Define the union u_isp_dpc_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_stt2lut_en    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_stt2lut_cfg;

/* Define the union u_isp_dpc_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_stt2lut_regnew;

/* Define the union u_isp_dpc_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_stt2lut_info  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_dpc_stt2lut_clr   : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_stt2lut_abn;

/* Define the union u_isp_dpc_rnd_thresh_1_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_thr1_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc_rnd_thr1_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_thresh_1_mtp;

/* Define the union u_isp_dpc_rnd_thresh_2_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_thr2_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc_rnd_thr2_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_thresh_2_mtp;

/* Define the union u_isp_dpc_rnd_thresh_3_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rnd_thr3_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc_rnd_thr3_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rnd_thresh_3_mtp;

/* Define the union u_isp_dpc_rg_fac_1_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rg_fac_1_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc_rg_fac_1_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rg_fac_1_mtp;

/* Define the union u_isp_dpc_rg_fac_2_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rg_fac_2_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc_rg_fac_2_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rg_fac_2_mtp;

/* Define the union u_isp_dpc_rg_fac_3_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_rg_fac_3_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc_rg_fac_3_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_rg_fac_3_mtp;

/* Define the union u_isp_dpc_amp_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_amp_coef_k    : 12; /* [11..0]  */
        unsigned int    isp_dpc_amp_coef_min  : 9; /* [20..12]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_amp_coef;

/* Define the union u_isp_dpc_eith_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_wbbgainrecp   : 16; /* [15..0]  */
        unsigned int    isp_dpc_wbrgainrecp   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_eith_1;

/* Define the union u_isp_dpc_eith_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_offset        : 8; /* [7..0]  */
        unsigned int    isp_dpc_quad_refen    : 1; /* [8]  */
        unsigned int    isp_dpc_eigh_refen    : 1; /* [9]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_eith_2;

/* Define the union u_isp_dpc_eith_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_pgfac_eigh    : 8; /* [7..0]  */
        unsigned int    isp_dpc_pgthr_eigh    : 8; /* [15..8]  */
        unsigned int    isp_dpc_ro_eigh       : 2; /* [17..16]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_eith_3;

/* Define the union u_isp_dpc1_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_dpc1_dpc_en1      : 1; /* [1]  */
        unsigned int    reserved_1            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_cfg;

/* Define the union u_isp_dpc1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_alpha1_g     : 8; /* [7..0]  */
        unsigned int    isp_dpc1_alpha1_rb    : 8; /* [15..8]  */
        unsigned int    isp_dpc1_alpha0_g     : 8; /* [23..16]  */
        unsigned int    isp_dpc1_alpha0_rb    : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_alpha;

/* Define the union u_isp_dpc1_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dpc1_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_version;
/* Define the union u_isp_dpc1_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_det_en       : 1; /* [0]  */
        unsigned int    isp_dpc1_cor_en       : 1; /* [1]  */
        unsigned int    isp_dpc1_six_det_en   : 1; /* [2]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    isp_dpc1_grayscale_mode : 1; /* [4]  */
        unsigned int    isp_dpc1_bpt_cor_en   : 1; /* [5]  */
        unsigned int    isp_dpc1_dpt_det_sel  : 1; /* [6]  */
        unsigned int    isp_dpc1_dp_hightlight_en : 1; /* [7]  */
        unsigned int    isp_dpc1_ir_position  : 1; /* [8]  */
        unsigned int    isp_dpc1_ir_channel   : 1; /* [9]  */
        unsigned int    reserved_1            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_mode;

/* Define the union u_isp_dpc1_output_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_stage1_incl_gr_center : 1; /* [0]  */
        unsigned int    isp_dpc1_stage1_incl_rb_center : 1; /* [1]  */
        unsigned int    isp_dpc1_stage1_incl_g_3x3 : 1; /* [2]  */
        unsigned int    isp_dpc1_stage1_incl_rb_3x3 : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_output_mode;

/* Define the union u_isp_dpc1_set_use */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_stage1_use_set1 : 1; /* [0]  */
        unsigned int    isp_dpc1_stage1_use_set2 : 1; /* [1]  */
        unsigned int    isp_dpc1_stage1_use_set3 : 1; /* [2]  */
        unsigned int    isp_dpc1_stage1_use_fix_set : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_set_use;

/* Define the union u_isp_dpc1_methods_set_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pg_green1_enable : 1; /* [0]  */
        unsigned int    isp_dpc1_lc_green1_enable : 1; /* [1]  */
        unsigned int    isp_dpc1_ro_green1_enable : 1; /* [2]  */
        unsigned int    isp_dpc1_rnd_green1_enable : 1; /* [3]  */
        unsigned int    isp_dpc1_rg_green1_enable : 1; /* [4]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_dpc1_pg_red_blue1_enable : 1; /* [8]  */
        unsigned int    isp_dpc1_lc_red_blue1_enable : 1; /* [9]  */
        unsigned int    isp_dpc1_ro_red_blue1_enable : 1; /* [10]  */
        unsigned int    isp_dpc1_rnd_red_blue1_enable : 1; /* [11]  */
        unsigned int    isp_dpc1_rg_red_blue1_enable : 1; /* [12]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_methods_set_1;

/* Define the union u_isp_dpc1_methods_set_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pg_green2_enable : 1; /* [0]  */
        unsigned int    isp_dpc1_lc_green2_enable : 1; /* [1]  */
        unsigned int    isp_dpc1_ro_green2_enable : 1; /* [2]  */
        unsigned int    isp_dpc1_rnd_green2_enable : 1; /* [3]  */
        unsigned int    isp_dpc1_rg_green2_enable : 1; /* [4]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_dpc1_pg_red_blue2_enable : 1; /* [8]  */
        unsigned int    isp_dpc1_lc_red_blue2_enable : 1; /* [9]  */
        unsigned int    isp_dpc1_ro_red_blue2_enable : 1; /* [10]  */
        unsigned int    isp_dpc1_rnd_red_blue2_enable : 1; /* [11]  */
        unsigned int    isp_dpc1_rg_red_blue2_enable : 1; /* [12]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_methods_set_2;

/* Define the union u_isp_dpc1_methods_set_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pg_green3_enable : 1; /* [0]  */
        unsigned int    isp_dpc1_lc_green3_enable : 1; /* [1]  */
        unsigned int    isp_dpc1_ro_green3_enable : 1; /* [2]  */
        unsigned int    isp_dpc1_rnd_green3_enable : 1; /* [3]  */
        unsigned int    isp_dpc1_rg_green3_enable : 1; /* [4]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_dpc1_pg_red_blue3_enable : 1; /* [8]  */
        unsigned int    isp_dpc1_lc_red_blue3_enable : 1; /* [9]  */
        unsigned int    isp_dpc1_ro_red_blue3_enable : 1; /* [10]  */
        unsigned int    isp_dpc1_rnd_red_blue3_enable : 1; /* [11]  */
        unsigned int    isp_dpc1_rg_red_blue3_enable : 1; /* [12]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_methods_set_3;

/* Define the union u_isp_dpc1_line_thresh_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_thr1_g  : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_thr1_rb : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_thresh_1;

/* Define the union u_isp_dpc1_line_mad_fac_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_mad_fac_1_g : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_line_mad_fac_1_rb : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_mad_fac_1;

/* Define the union u_isp_dpc1_pg_fac_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pg_fac_1_g   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_pg_fac_1_rb  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_pg_fac_1;

/* Define the union u_isp_dpc1_rnd_thresh_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_thr1_g   : 8; /* [7..0]  */
        unsigned int    isp_dpc1_rnd_thr1_rb  : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_thresh_1;

/* Define the union u_isp_dpc1_rg_fac_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rg_fac_1_g   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_rg_fac_1_rb  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rg_fac_1;

/* Define the union u_isp_dpc1_line_thresh_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_thr2_g  : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_thr2_rb : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_thresh_2;

/* Define the union u_isp_dpc1_line_mad_fac_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_mad_fac_2_g : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_line_mad_fac_2_rb : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_mad_fac_2;

/* Define the union u_isp_dpc1_pg_fac_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pg_fac_2_g   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_pg_fac_2_rb  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_pg_fac_2;

/* Define the union u_isp_dpc1_rnd_thresh_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_thr2_g   : 8; /* [7..0]  */
        unsigned int    isp_dpc1_rnd_thr2_rb  : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_thresh_2;

/* Define the union u_isp_dpc1_rg_fac_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rg_fac_2_g   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_rg_fac_2_rb  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rg_fac_2;

/* Define the union u_isp_dpc1_line_thresh_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_thr3_g  : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_thr3_rb : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_thresh_3;

/* Define the union u_isp_dpc1_line_mad_fac_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_mad_fac_3_g : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_line_mad_fac_3_rb : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_mad_fac_3;

/* Define the union u_isp_dpc1_pg_fac_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pg_fac_3_g   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_pg_fac_3_rb  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_pg_fac_3;

/* Define the union u_isp_dpc1_rnd_thresh_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_thr3_g   : 8; /* [7..0]  */
        unsigned int    isp_dpc1_rnd_thr3_rb  : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_thresh_3;

/* Define the union u_isp_dpc1_rg_fac_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rg_fac_3_g   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_dpc1_rg_fac_3_rb  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rg_fac_3;

/* Define the union u_isp_dpc1_ro_limits */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_ro_1_g       : 2; /* [1..0]  */
        unsigned int    isp_dpc1_ro_1_rb      : 2; /* [3..2]  */
        unsigned int    isp_dpc1_ro_2_g       : 2; /* [5..4]  */
        unsigned int    isp_dpc1_ro_2_rb      : 2; /* [7..6]  */
        unsigned int    isp_dpc1_ro_3_g       : 2; /* [9..8]  */
        unsigned int    isp_dpc1_ro_3_rb      : 2; /* [11..10]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_ro_limits;

/* Define the union u_isp_dpc1_rnd_offs */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_offs_1_g : 2; /* [1..0]  */
        unsigned int    isp_dpc1_rnd_offs_1_rb : 2; /* [3..2]  */
        unsigned int    isp_dpc1_rnd_offs_2_g : 2; /* [5..4]  */
        unsigned int    isp_dpc1_rnd_offs_2_rb : 2; /* [7..6]  */
        unsigned int    isp_dpc1_rnd_offs_3_g : 2; /* [9..8]  */
        unsigned int    isp_dpc1_rnd_offs_3_rb : 2; /* [11..10]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_offs;

/* Define the union u_isp_dpc1_bpt_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 4; /* [3..0]  */
        unsigned int    isp_dpc1_bpt_use_set_1 : 1; /* [4]  */
        unsigned int    isp_dpc1_bpt_use_set_2 : 1; /* [5]  */
        unsigned int    isp_dpc1_bpt_use_set_3 : 1; /* [6]  */
        unsigned int    isp_dpc1_bpt_use_fix_set : 1; /* [7]  */
        unsigned int    isp_dpc1_bpt_incl_green_center : 1; /* [8]  */
        unsigned int    isp_dpc1_bpt_incl_rb_center : 1; /* [9]  */
        unsigned int    isp_dpc1_g_3x3        : 1; /* [10]  */
        unsigned int    isp_dpc1_rb_3x3       : 1; /* [11]  */
        unsigned int    reserved_1            : 4; /* [15..12]  */
        unsigned int    isp_dpc1_bp_data      : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_ctrl;

/* Define the union u_isp_dpc1_bpt_number */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_bpt_number   : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_number;

/* Define the union u_isp_dpc1_bpt_calib_number */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_bpt_calib_number : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_calib_number;

/* Define the union u_isp_dpc1_bpt_thrd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_dev_dead_thresh : 8; /* [7..0]  */
        unsigned int    isp_dpc1_abs_dead_thresh : 8; /* [15..8]  */
        unsigned int    isp_dpc1_dev_hot_thresh : 8; /* [23..16]  */
        unsigned int    isp_dpc1_abs_hot_thresh : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_thrd;

/* Define the union u_isp_dpc1_bpt_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dpc1_bpt_waddr     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_waddr;
/* Define the union u_isp_dpc1_bpt_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_bpt_posx_wdata : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc1_bpt_posy_wdata : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_wdata;

/* Define the union u_isp_dpc1_bpt_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dpc1_bpt_raddr     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_raddr;
/* Define the union u_isp_dpc1_bpt_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_bpt_posx_rdata : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc1_bpt_posy_rdata : 13; /* [28..16]  */
        unsigned int    isp_dpc1_bpt_rdata    : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bpt_rdata;

/* Define the union u_isp_dpc1_soft_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_ex_soft_thr_min : 8; /* [7..0]  */
        unsigned int    isp_dpc1_ex_soft_thr_max : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_soft_thr;

/* Define the union u_isp_dpc1_bhardthr_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_ex_hard_thr_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bhardthr_en;

/* Define the union u_isp_dpc1_rakeratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_ex_rake_ratio : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rakeratio;

/* Define the union u_isp_dpc1_lch_thr_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_aver_fac_1 : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_diff_thr_1 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc1_line_std_thr_1 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_lch_thr_1;

/* Define the union u_isp_dpc1_lch_thr_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_aver_fac_2 : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_diff_thr_2 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc1_line_std_thr_2 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_lch_thr_2;

/* Define the union u_isp_dpc1_lch_thr_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_aver_fac_3 : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_diff_thr_3 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc1_line_std_thr_3 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_lch_thr_3;

/* Define the union u_isp_dpc1_lch_thr_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_aver_fac_4 : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_diff_thr_4 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc1_line_std_thr_4 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_lch_thr_4;

/* Define the union u_isp_dpc1_lch_thr_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_aver_fac_5 : 8; /* [7..0]  */
        unsigned int    isp_dpc1_line_diff_thr_5 : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dpc1_line_std_thr_5 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_lch_thr_5;

/* Define the union u_isp_dpc1_line_kerdiff_fac */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_line_kerdiff_fac : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_line_kerdiff_fac;

/* Define the union u_isp_dpc1_blend_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_blend_mode   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_blend_mode;

/* Define the union u_isp_dpc1_bit_depth_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_bit_depth_sel : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_bit_depth_sel;

/* Define the union u_isp_dpc1_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_stt2lut_en   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_stt2lut_cfg;

/* Define the union u_isp_dpc1_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_stt2lut_regnew;

/* Define the union u_isp_dpc1_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_dpc1_stt2lut_clr  : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_stt2lut_abn;

/* Define the union u_isp_dpc1_rnd_thresh_1_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_thr1_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc1_rnd_thr1_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_thresh_1_mtp;

/* Define the union u_isp_dpc1_rnd_thresh_2_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_thr2_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc1_rnd_thr2_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_thresh_2_mtp;

/* Define the union u_isp_dpc1_rnd_thresh_3_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rnd_thr3_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc1_rnd_thr3_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rnd_thresh_3_mtp;

/* Define the union u_isp_dpc1_rg_fac_1_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rg_fac_1_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc1_rg_fac_1_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rg_fac_1_mtp;

/* Define the union u_isp_dpc1_rg_fac_2_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rg_fac_2_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc1_rg_fac_2_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rg_fac_2_mtp;

/* Define the union u_isp_dpc1_rg_fac_3_mtp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_rg_fac_3_g_mtp : 10; /* [9..0]  */
        unsigned int    isp_dpc1_rg_fac_3_rb_mtp : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_rg_fac_3_mtp;

/* Define the union u_isp_dpc1_amp_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_amp_coef_k   : 12; /* [11..0]  */
        unsigned int    isp_dpc1_amp_coef_min : 9; /* [20..12]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_amp_coef;

/* Define the union u_isp_dpc1_eith_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_wbbgainrecp  : 16; /* [15..0]  */
        unsigned int    isp_dpc1_wbrgainrecp  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_eith_1;

/* Define the union u_isp_dpc1_eith_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_offset       : 8; /* [7..0]  */
        unsigned int    isp_dpc1_quad_refen   : 1; /* [8]  */
        unsigned int    isp_dpc1_eigh_refen   : 1; /* [9]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_eith_2;

/* Define the union u_isp_dpc1_eith_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_pgfac_eigh   : 8; /* [7..0]  */
        unsigned int    isp_dpc1_pgthr_eigh   : 8; /* [15..8]  */
        unsigned int    isp_dpc1_ro_eigh      : 2; /* [17..16]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_eith_3;

/* Define the union u_isp_rgbir_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_blc_in_en   : 1; /* [0]  */
        unsigned int    isp_rgbir_blc_out_en  : 1; /* [1]  */
        unsigned int    isp_rgbir_ir_pattern_in : 3; /* [4..2]  */
        unsigned int    isp_rgbir_pattern_out : 2; /* [6..5]  */
        unsigned int    isp_rgbir_smooth_enable : 1; /* [7]  */
        unsigned int    isp_rgbir_bitdepth    : 4; /* [11..8]  */
        unsigned int    isp_rgbir_out_type    : 1; /* [12]  */
        unsigned int    isp_rgbir_ir_remove_en : 1; /* [13]  */
        unsigned int    isp_rgbir_upscale_en  : 1; /* [14]  */
        unsigned int    isp_rgbir_bayer_pattern_in : 4; /* [18..15]  */
        unsigned int    isp_rgbir_mode        : 1; /* [19]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cfg;

/* Define the union u_isp_rgbir_thre */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_th          : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_rgbir_tv          : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_thre;

/* Define the union u_isp_rgbir_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rgbir_version      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_version;
/* Define the union u_isp_rgbir_exp_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_exp_thr1    : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_rgbir_exp_thr2    : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_exp_thr;

/* Define the union u_isp_rgbir_reci_ctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_reci_ctl1   : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_rgbir_reci_ctl2   : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_reci_ctl;

/* Define the union u_isp_rgbir_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_gain_r      : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_rgbir_gain_b      : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_gain;

/* Define the union u_isp_rgbir_cvt01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_matrix0     : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_matrix1     : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cvt01;

/* Define the union u_isp_rgbir_cvt23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_matrix2     : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_matrix3     : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cvt23;

/* Define the union u_isp_rgbir_cvt45 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_matrix4     : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_matrix5     : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cvt45;

/* Define the union u_isp_rgbir_cvt67 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_matrix6     : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_matrix7     : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cvt67;

/* Define the union u_isp_rgbir_cvt89 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_matrix8     : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_matrix9     : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cvt89;

/* Define the union u_isp_rgbir_cvt1011 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_matrix10    : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_matrix11    : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_cvt1011;

/* Define the union u_isp_rgbir_blc_offset_r */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_blc_offset_r : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_blc_offset_r;

/* Define the union u_isp_rgbir_blc_offset_g */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_blc_offset_g : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_blc_offset_g;

/* Define the union u_isp_rgbir_blc_offset_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_blc_offset_b : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_blc_offset_b;

/* Define the union u_isp_rgbir_blc_offset_ir */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_blc_offset_ir : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_blc_offset_ir;

/* Define the union u_isp_rgbir_ir_sum */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rgbir_ir_sum       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_ir_sum;
/* Define the union u_isp_rgbir_thd_grad */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_thd1_grad   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_rgbir_thd0_grad   : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_thd_grad;

/* Define the union u_isp_rgbir_lut0_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_1       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_0       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut0_1;

/* Define the union u_isp_rgbir_lut2_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_3       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_2       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut2_3;

/* Define the union u_isp_rgbir_lut4_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_5       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_4       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut4_5;

/* Define the union u_isp_rgbir_lut6_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_7       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_6       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut6_7;

/* Define the union u_isp_rgbir_lut8_9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_9       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_8       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut8_9;

/* Define the union u_isp_rgbir_lut10_11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_11      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_10      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut10_11;

/* Define the union u_isp_rgbir_lut12_13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_13      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_12      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut12_13;

/* Define the union u_isp_rgbir_lut14_15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_15      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_14      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut14_15;

/* Define the union u_isp_rgbir_lut16_17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_17      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_16      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut16_17;

/* Define the union u_isp_rgbir_lut18_19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_19      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_18      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut18_19;

/* Define the union u_isp_rgbir_lut20_21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_21      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_20      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut20_21;

/* Define the union u_isp_rgbir_lut22_23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_23      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_22      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut22_23;

/* Define the union u_isp_rgbir_lut24_25 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_25      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_24      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut24_25;

/* Define the union u_isp_rgbir_lut26_27 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_27      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_26      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut26_27;

/* Define the union u_isp_rgbir_lut28_29 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_29      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_28      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut28_29;

/* Define the union u_isp_rgbir_lut30_31 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_31      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_lut_30      : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut30_31;

/* Define the union u_isp_rgbir_lut32 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_lut_32      : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_lut32;

/* Define the union u_isp_rgbir_ct_ratio_r_g */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_ct_ratio_g  : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_ct_ratio_r  : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_ct_ratio_r_g;

/* Define the union u_isp_rgbir_ct_ratio_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_ct_ratio_b  : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_ct_ratio_b;

/* Define the union u_isp_rgbir_awb_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_awb_bgain   : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_rgbir_awb_rgain   : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_awb_gain;

/* Define the union u_isp_rgbir_ctl_str */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_b_ctl_str   : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_rgbir_r_ctl_str   : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_ctl_str;

/* Define the union u_isp_rgbir_auto_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_rgbir_auto_gain   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_rgbir_auto_gain_enable : 1; /* [16]  */
        unsigned int    reserved_1            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_auto_gain;

/* Define the union u_isp_rgbir_auto_gain_o */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rgbir_auto_gain_o  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_auto_gain_o;
/* Define the union u_isp_ge_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_ge_ge1_en         : 1; /* [1]  */
        unsigned int    reserved_1            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge_cfg;

/* Define the union u_isp_ge_blc_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge0_blc_offset_en : 1; /* [0]  */
        unsigned int    isp_ge1_blc_offset_en : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge_blc_en;

/* Define the union u_isp_ge_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ge_version         : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge_version;
/* Define the union u_isp_ge0_ct_th1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge0_ct_th1     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge0_ct_th1;

/* Define the union u_isp_ge0_ct_th2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge0_ct_th2     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge0_ct_th2;

/* Define the union u_isp_ge0_ct_th3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge0_ct_th3     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge0_ct_th3;

/* Define the union u_isp_ge0_ct_slope */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge0_ct_slope1  : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_ge_ge0_ct_slope2  : 5; /* [12..8]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge0_ct_slope;

/* Define the union u_isp_ge1_ct_th1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge1_ct_th1     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ct_th1;

/* Define the union u_isp_ge1_ct_th2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge1_ct_th2     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ct_th2;

/* Define the union u_isp_ge1_ct_th3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge1_ct_th3     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ct_th3;

/* Define the union u_isp_ge1_ct_slope */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_ge1_ct_slope1  : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_ge_ge1_ct_slope2  : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ct_slope;

/* Define the union u_isp_ge0_blc_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge0_blc_offset_gb : 16; /* [15..0]  */
        unsigned int    isp_ge0_blc_offset_gr : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge0_blc_offset;

/* Define the union u_isp_ge1_blc_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_blc_offset_gb : 16; /* [15..0]  */
        unsigned int    isp_ge1_blc_offset_gr : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_blc_offset;

/* Define the union u_isp_ge_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_gr_en          : 1; /* [0]  */
        unsigned int    isp_ge_gb_en          : 1; /* [1]  */
        unsigned int    isp_ge_gr_gb_en       : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge_mode;

/* Define the union u_isp_ge_strength */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_strength       : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge_strength;

/* Define the union u_isp_ge_bit_depth_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge_bit_depth_sel  : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge_bit_depth_sel;

/* Define the union u_isp_ge1_ge_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_ge1_ge_ge1_en     : 1; /* [1]  */
        unsigned int    reserved_1            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge_cfg;

/* Define the union u_isp_ge1_ge_blc_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge0_blc_offset_en : 1; /* [0]  */
        unsigned int    isp_ge1_ge1_blc_offset_en : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge_blc_en;

/* Define the union u_isp_ge1_ge_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ge1_ge_version     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge_version;
/* Define the union u_isp_ge1_ge0_ct_th1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge0_ct_th1 : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge0_ct_th1;

/* Define the union u_isp_ge1_ge0_ct_th2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge0_ct_th2 : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge0_ct_th2;

/* Define the union u_isp_ge1_ge0_ct_th3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge0_ct_th3 : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge0_ct_th3;

/* Define the union u_isp_ge1_ge0_ct_slope */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge0_ct_slope1 : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_ge1_ge_ge0_ct_slope2 : 5; /* [12..8]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge0_ct_slope;

/* Define the union u_isp_ge1_ge1_ct_th1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge1_ct_th1 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge1_ct_th1;

/* Define the union u_isp_ge1_ge1_ct_th2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge1_ct_th2 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge1_ct_th2;

/* Define the union u_isp_ge1_ge1_ct_th3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge1_ct_th3 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge1_ct_th3;

/* Define the union u_isp_ge1_ge1_ct_slope */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_ge1_ct_slope1 : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_ge1_ge_ge1_ct_slope2 : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge1_ct_slope;

/* Define the union u_isp_ge1_ge0_blc_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge0_blc_offset_gb : 16; /* [15..0]  */
        unsigned int    isp_ge1_ge0_blc_offset_gr : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge0_blc_offset;

/* Define the union u_isp_ge1_ge1_blc_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge1_blc_offset_gb : 16; /* [15..0]  */
        unsigned int    isp_ge1_ge1_blc_offset_gr : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge1_blc_offset;

/* Define the union u_isp_ge1_ge_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_gr_en      : 1; /* [0]  */
        unsigned int    isp_ge1_ge_gb_en      : 1; /* [1]  */
        unsigned int    isp_ge1_ge_gr_gb_en   : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge_mode;

/* Define the union u_isp_ge1_ge_strength */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_strength   : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge_strength;

/* Define the union u_isp_ge1_ge_bit_depth_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ge1_ge_bit_depth_sel : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ge1_ge_bit_depth_sel;

/* Define the union u_isp_wdr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_stat_read_enable : 1; /* [0]  */
        unsigned int    isp_wdr_fusion_data_mode : 2; /* [2..1]  */
        unsigned int    isp_wdr_mask_erosion_en : 1; /* [3]  */
        unsigned int    isp_wdr_fusionmode    : 1; /* [4]  */
        unsigned int    isp_wdr_revmode       : 1; /* [5]  */
        unsigned int    isp_wdr_sigma_mode_select : 1; /* [6]  */
        unsigned int    isp_wdr_avg_mode_select : 1; /* [7]  */
        unsigned int    reserved_0            : 9; /* [16..8]  */
        unsigned int    isp_wdr_blend_mode    : 2; /* [18..17]  */
        unsigned int    isp_wdr_bginfo_en     : 1; /* [19]  */
        unsigned int    isp_wdr_mdt_en        : 1; /* [20]  */
        unsigned int    isp_wdr_merge_frame   : 3; /* [23..21]  */
        unsigned int    isp_wdr_grayscale_mode : 1; /* [24]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_ctrl;

/* Define the union u_isp_wdr_f0_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f0_inblc_gr   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f0_inblc_r    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f0_inblc0;

/* Define the union u_isp_wdr_f0_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f0_inblc_b    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f0_inblc_gb   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f0_inblc1;

/* Define the union u_isp_wdr_f1_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f1_inblc_gr   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f1_inblc_r    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f1_inblc0;

/* Define the union u_isp_wdr_f1_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f1_inblc_b    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f1_inblc_gb   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f1_inblc1;

/* Define the union u_isp_wdr_f2_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f2_inblc_gr   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f2_inblc_r    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f2_inblc0;

/* Define the union u_isp_wdr_f2_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f2_inblc_b    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f2_inblc_gb   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f2_inblc1;

/* Define the union u_isp_wdr_f3_inblc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f3_inblc_gr   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f3_inblc_r    : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f3_inblc0;

/* Define the union u_isp_wdr_f3_inblc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f3_inblc_b    : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_f3_inblc_gb   : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_f3_inblc1;

/* Define the union u_isp_wdr_outblc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_outblc        : 24; /* [23..0]  */
        unsigned int    isp_wdr_bsaveblc      : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_outblc;

/* Define the union u_isp_wdr_wdr_expovalue0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_wdr_expovalue0 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wdr_expovalue0;

/* Define the union u_isp_wdr_wdr_expovalue1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_wdr_expovalue1 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wdr_expovalue1;

/* Define the union u_isp_wdr_wdr_expovalue2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_wdr_expovalue2 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wdr_expovalue2;

/* Define the union u_isp_wdr_wdr_expovalue3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_wdr_expovalue3 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wdr_expovalue3;

/* Define the union u_isp_wdr_fusion_expovalue0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_expovalue0 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_expovalue0;

/* Define the union u_isp_wdr_fusion_expovalue1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_expovalue1 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_expovalue1;

/* Define the union u_isp_wdr_fusion_expovalue2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_expovalue2 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_expovalue2;

/* Define the union u_isp_wdr_fusion_expovalue3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_expovalue3 : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_expovalue3;

/* Define the union u_isp_wdr_exporratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_exporratio2   : 10; /* [9..0]  */
        unsigned int    isp_wdr_exporratio1   : 10; /* [19..10]  */
        unsigned int    isp_wdr_exporratio0   : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_exporratio;

/* Define the union u_isp_wdr_blc_comp0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_blc_comp0     : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_blc_comp0;

/* Define the union u_isp_wdr_blc_comp1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_blc_comp1     : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_blc_comp1;

/* Define the union u_isp_wdr_blc_comp2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_blc_comp2     : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_blc_comp2;

/* Define the union u_isp_wdr_wdr_maxratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_wdr_maxratio  : 21; /* [20..0]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wdr_maxratio;

/* Define the union u_isp_wdr_fusion_maxratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_maxratio : 21; /* [20..0]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_maxratio;

/* Define the union u_isp_wdr_saturate_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_saturate_thr  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_saturate_thr;

/* Define the union u_isp_wdr_still_thr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f0_still_thr  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_still_thr0;

/* Define the union u_isp_wdr_still_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f1_still_thr  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_still_thr1;

/* Define the union u_isp_wdr_still_thr2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_f2_still_thr  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_still_thr2;

/* Define the union u_isp_wdr_mdtlbld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mdtlbld       : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mdtlbld;

/* Define the union u_isp_wdr_mdt_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mdt_full_thr  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_wdr_mdt_still_thr : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mdt_thr;

/* Define the union u_isp_wdr_forcelong_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_forcelong_high_thd : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_forcelong_low_thd : 14; /* [29..16]  */
        unsigned int    isp_wdr_forcelong_smooth_en : 1; /* [30]  */
        unsigned int    isp_wdr_forcelong_en  : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_forcelong_para;

/* Define the union u_isp_wdr_mdt_noisefloor */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mdt_nosfloor  : 7; /* [6..0]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mdt_noisefloor;

/* Define the union u_isp_wdr_mdthr_gain0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mdthr_hig_gain0 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_wdr_mdthr_low_gain0 : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mdthr_gain0;

/* Define the union u_isp_wdr_mdthr_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mdthr_hig_gain1 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_wdr_mdthr_low_gain1 : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mdthr_gain1;

/* Define the union u_isp_wdr_mdthr_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mdthr_hig_gain2 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_wdr_mdthr_low_gain2 : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mdthr_gain2;

/* Define the union u_isp_wdr_shortexpo_chk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_shortchk_thd  : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_shortexpo_chk : 1; /* [16]  */
        unsigned int    reserved_1            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_shortexpo_chk;

/* Define the union u_isp_wdr_pix_avg_diff */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_pixel_avg_max_diff : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_pix_avg_diff;

/* Define the union u_isp_wdr_mask_similar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_mask_similar_cnt : 4; /* [3..0]  */
        unsigned int    reserved_0            : 12; /* [15..4]  */
        unsigned int    isp_wdr_mask_similar_thr : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_mask_similar;

/* Define the union u_isp_wdr_wgtidx_blendratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_bldrlhfidx    : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wgtidx_blendratio;

/* Define the union u_isp_wdr_wgtidx_thr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_long_thr0     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_short_thr0    : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wgtidx_thr0;

/* Define the union u_isp_wdr_wgtidx_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_long_thr1     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_short_thr1    : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wgtidx_thr1;

/* Define the union u_isp_wdr_wgtidx_thr2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_long_thr2     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_short_thr2    : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_wgtidx_thr2;

/* Define the union u_isp_wdr_dftwgt_fl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_dftwgt_fl     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_dftwgt_fl;

/* Define the union u_isp_wdr_sensor_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_sqrt_dgain_g  : 3; /* [2..0]  */
        unsigned int    reserved_0            : 13; /* [15..3]  */
        unsigned int    isp_wdr_sqrt_again_g  : 3; /* [18..16]  */
        unsigned int    reserved_1            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_sensor_gain;

/* Define the union u_isp_wdr_fusion_thr_r0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_f1_thr_r : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_fusion_f0_thr_r : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_thr_r0;

/* Define the union u_isp_wdr_fusion_thr_r1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_f3_thr_r : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_fusion_f2_thr_r : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_thr_r1;

/* Define the union u_isp_wdr_fusion_thr_b0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_f1_thr_b : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_fusion_f0_thr_b : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_thr_b0;

/* Define the union u_isp_wdr_fusion_thr_b1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_f3_thr_b : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_fusion_f2_thr_b : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_thr_b1;

/* Define the union u_isp_wdr_fusion_thr_g0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_f1_thr_g : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_fusion_f0_thr_g : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_thr_g0;

/* Define the union u_isp_wdr_fusion_thr_g1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_f3_thr_g : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_fusion_f2_thr_g : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_thr_g1;

/* Define the union u_isp_wdr_fusion_sat_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_fusion_saturate_thd : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_fusion_sat_thd;

/* Define the union u_isp_wdr_stat_blk_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_ref_stat_vblk_num : 6; /* [5..0]  */
        unsigned int    isp_wdr_ref_stat_hblk_num : 6; /* [11..6]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_stat_blk_num;

/* Define the union u_isp_wdr_clac_blk_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_cur_clac_vblk_num : 6; /* [5..0]  */
        unsigned int    isp_wdr_cur_clac_hblk_num : 6; /* [11..6]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_clac_blk_num;

/* Define the union u_isp_wdr_bginfo_thr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_bginfo_hig_thd0 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_bginfo_low_thd0 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_bginfo_thr0;

/* Define the union u_isp_wdr_bginfo_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_bginfo_hig_thd1 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_bginfo_low_thd1 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_bginfo_thr1;

/* Define the union u_isp_wdr_split_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_split_num     : 2; /* [1..0]  */
        unsigned int    isp_wdr_blk_idx       : 3; /* [4..2]  */
        unsigned int    isp_wdr_stat_shift    : 6; /* [10..5]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_split_info;

/* Define the union u_isp_wdr_awb_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_b_gain        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_wdr_r_gain        : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_awb_gain;

/* Define the union u_isp_wdr_upscale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_ky            : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wdr_kx            : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_upscale;

/* Define the union u_isp_wdr_smallmap_offset */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_wdr_smlmap_offset  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_smallmap_offset;
/* Define the union u_isp_wdr_statistic */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_day_time      : 1; /* [0]  */
        unsigned int    isp_wdr_dilation_en   : 1; /* [1]  */
        unsigned int    isp_wdr_erosion_en    : 1; /* [2]  */
        unsigned int    isp_wdr_similar_cnt   : 3; /* [5..3]  */
        unsigned int    isp_wdr_erosion_diffthr : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_statistic;

/* Define the union u_isp_wdr_stat_still_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_thr_offset    : 14; /* [13..0]  */
        unsigned int    isp_wdr_merg_wgt_thr  : 9; /* [22..14]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_stat_still_thr;

/* Define the union u_isp_wdr_ginter_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_beta          : 10; /* [9..0]  */
        unsigned int    isp_wdr_ahpha1        : 10; /* [19..10]  */
        unsigned int    isp_wdr_ahpha0        : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_ginter_thr;

/* Define the union u_isp_wdr_statistic_flt_thr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lpf_cef3      : 8; /* [7..0]  */
        unsigned int    isp_wdr_lpf_cef2      : 8; /* [15..8]  */
        unsigned int    isp_wdr_lpf_cef1      : 8; /* [23..16]  */
        unsigned int    isp_wdr_lpf_cef0      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_statistic_flt_thr0;

/* Define the union u_isp_wdr_statistic_flt_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lpf_cef7      : 8; /* [7..0]  */
        unsigned int    isp_wdr_lpf_cef6      : 8; /* [15..8]  */
        unsigned int    isp_wdr_lpf_cef5      : 8; /* [23..16]  */
        unsigned int    isp_wdr_lpf_cef4      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_statistic_flt_thr1;

/* Define the union u_isp_wdr_statistic_flt_thr2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lpf_cef8      : 8; /* [7..0]  */
        unsigned int    isp_wdr_lpf_stf       : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_statistic_flt_thr2;

/* Define the union u_isp_wdr_stat_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_stat_endx     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_stat_startx   : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_stat_pos;

/* Define the union u_isp_wdr_blend_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_blend_rgain   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_wdr_blend_bgain   : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_blend_gain;

/* Define the union u_isp_wdr_gwgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_gwgt_0        : 8; /* [7..0]  */
        unsigned int    isp_wdr_gwgt_1        : 8; /* [15..8]  */
        unsigned int    isp_wdr_gwgt_2        : 8; /* [23..16]  */
        unsigned int    isp_wdr_gwgt_3        : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_gwgt;

/* Define the union u_isp_wdr_gwgt_thr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_gwgt_thr0     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_wdr_gwgt_thr1     : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_gwgt_thr0;

/* Define the union u_isp_wdr_gwgt_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_gwgt_thr2     : 14; /* [13..0]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_gwgt_thr1;

/* Define the union u_isp_wdr_lut_seg_idxbase0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_seg_idx_base2 : 10; /* [9..0]  */
        unsigned int    isp_wdr_seg_idx_base1 : 10; /* [19..10]  */
        unsigned int    isp_wdr_seg_idx_base0 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_seg_idxbase0;

/* Define the union u_isp_wdr_lut_seg_idxbase1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_seg_idx_base5 : 10; /* [9..0]  */
        unsigned int    isp_wdr_seg_idx_base4 : 10; /* [19..10]  */
        unsigned int    isp_wdr_seg_idx_base3 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_seg_idxbase1;

/* Define the union u_isp_wdr_lut_seg_idxbase2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_seg_idx_base7 : 10; /* [9..0]  */
        unsigned int    isp_wdr_seg_idx_base6 : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_seg_idxbase2;

/* Define the union u_isp_wdr_lut_seg_maxval0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_seg_maxval2   : 10; /* [9..0]  */
        unsigned int    isp_wdr_seg_maxval1   : 10; /* [19..10]  */
        unsigned int    isp_wdr_seg_maxval0   : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_seg_maxval0;

/* Define the union u_isp_wdr_lut_seg_maxval1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_seg_maxval5   : 10; /* [9..0]  */
        unsigned int    isp_wdr_seg_maxval4   : 10; /* [19..10]  */
        unsigned int    isp_wdr_seg_maxval3   : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_seg_maxval1;

/* Define the union u_isp_wdr_lut_seg_maxval2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_seg_maxval7   : 10; /* [9..0]  */
        unsigned int    isp_wdr_seg_maxval6   : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_seg_maxval2;

/* Define the union u_isp_wdr_filter_coefh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_filter_coefh4 : 5; /* [4..0]  */
        unsigned int    isp_wdr_filter_coefh3 : 5; /* [9..5]  */
        unsigned int    isp_wdr_filter_coefh2 : 5; /* [14..10]  */
        unsigned int    isp_wdr_filter_coefh1 : 5; /* [19..15]  */
        unsigned int    isp_wdr_filter_coefh0 : 5; /* [24..20]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_filter_coefh;

/* Define the union u_isp_wdr_filter_coefv */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_filter_coefv4 : 5; /* [4..0]  */
        unsigned int    isp_wdr_filter_coefv3 : 5; /* [9..5]  */
        unsigned int    isp_wdr_filter_coefv2 : 5; /* [14..10]  */
        unsigned int    isp_wdr_filter_coefv1 : 5; /* [19..15]  */
        unsigned int    isp_wdr_filter_coefv0 : 5; /* [24..20]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_filter_coefv;

/* Define the union u_isp_wdr_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_snoise_profile_update : 1; /* [0]  */
        unsigned int    isp_wdr_lnoise_profile_update : 1; /* [1]  */
        unsigned int    isp_wdr_normalize_update : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lut_update;

/* Define the union u_isp_wdr_snoise_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_snoise_lut_waddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_snoise_lut_waddr;

/* Define the union u_isp_wdr_snoise_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_snoise_lut_wdata : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_snoise_lut_wdata;

/* Define the union u_isp_wdr_snoise_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_snoise_lut_raddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_snoise_lut_raddr;

/* Define the union u_isp_wdr_snoise_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_snoise_lut_rdata : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_snoise_lut_rdata;

/* Define the union u_isp_wdr_lnoise_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lnoise_lut_waddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lnoise_lut_waddr;

/* Define the union u_isp_wdr_lnoise_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lnoise_lut_wdata : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lnoise_lut_wdata;

/* Define the union u_isp_wdr_lnoise_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lnoise_lut_raddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lnoise_lut_raddr;

/* Define the union u_isp_wdr_lnoise_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lnoise_lut_rdata : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lnoise_lut_rdata;

/* Define the union u_isp_wdr_normalize_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_normalize_lut_waddr : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_normalize_lut_waddr;

/* Define the union u_isp_wdr_normalize_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_normalize_lut_wdata : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_normalize_lut_wdata;

/* Define the union u_isp_wdr_normalize_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_normalize_lut_raddr : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_normalize_lut_raddr;

/* Define the union u_isp_wdr_normalize_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_normalize_lut_rdata : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_normalize_lut_rdata;

/* Define the union u_isp_expander_bitw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_expander_bitw_out : 5; /* [4..0]  */
        unsigned int    reserved_0            : 11; /* [15..5]  */
        unsigned int    isp_expander_bitw_in  : 5; /* [20..16]  */
        unsigned int    reserved_1            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_bitw;

/* Define the union u_isp_expander_offsetr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_expander_offset_r : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_offsetr;

/* Define the union u_isp_expander_offsetgr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_expander_offset_gr : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_offsetgr;

/* Define the union u_isp_expander_offsetgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_expander_offset_gb : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_offsetgb;

/* Define the union u_isp_expander_offsetb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_expander_offset_b : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_offsetb;

/* Define the union u_isp_expander_lut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_expander_lut_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_lut_waddr;
/* Define the union u_isp_expander_lut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_expander_lut_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_lut_wdata;
/* Define the union u_isp_expander_lut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_expander_lut_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_lut_raddr;
/* Define the union u_isp_expander_lut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_expander_lut_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_lut_rdata;
/* Define the union u_isp_bcom_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bcom_cfg;

/* Define the union u_isp_bcom_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bcom_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bcom_version;
/* Define the union u_isp_bcom_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bcom_alpha        : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bcom_alpha;

/* Define the union u_isp_crb_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_mode          : 1; /* [0]  */
        unsigned int    isp_crb_rccmode       : 1; /* [1]  */
        unsigned int    isp_crb_stepfirstenable : 1; /* [2]  */
        unsigned int    isp_crb_stepsecondenable : 1; /* [3]  */
        unsigned int    isp_crb_stepthirdenable : 1; /* [4]  */
        unsigned int    isp_crb_wdr_fusion_en : 1; /* [5]  */
        unsigned int    isp_crb_wdr_enable    : 2; /* [7..6]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_cfg;

/* Define the union u_isp_crb_dc_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_dc_numv       : 5; /* [4..0]  */
        unsigned int    isp_crb_dc_numh       : 5; /* [9..5]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_dc_size;

/* Define the union u_isp_crb_kx */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_kx            : 22; /* [21..0]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_kx;

/* Define the union u_isp_crb_ky */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_ky            : 22; /* [21..0]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_ky;

/* Define the union u_isp_crb_smlmapoffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_smlmapoffset  : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_smlmapoffset;

/* Define the union u_isp_crb_ccrcc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_ccrcc1        : 11; /* [10..0]  */
        unsigned int    isp_crb_ccrcc0        : 11; /* [21..11]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_ccrcc;

/* Define the union u_isp_crb_awbgain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_awbgain1      : 12; /* [11..0]  */
        unsigned int    isp_crb_awbgain0      : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_awbgain;

/* Define the union u_isp_crb_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_gr1           : 12; /* [11..0]  */
        unsigned int    isp_crb_gr0           : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_gr;

/* Define the union u_isp_crb_cchigh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_cchigh        : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_cchigh;

/* Define the union u_isp_crb_mix0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_mix0          : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_mix0;

/* Define the union u_isp_crb_maxlevel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_maxlevel      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_maxlevel;

/* Define the union u_isp_crb_highlevel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_highlevel     : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_highlevel;

/* Define the union u_isp_crb_cc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_cc1           : 11; /* [10..0]  */
        unsigned int    isp_crb_cc0           : 11; /* [21..11]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_cc;

/* Define the union u_isp_crb_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_stt2lut_en    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_stt2lut_cfg;

/* Define the union u_isp_crb_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_stt2lut_regnew;

/* Define the union u_isp_crb_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_stt2lut_info  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_crb_stt2lut_clr   : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_stt2lut_abn;

/* Define the union u_isp_crb_inblc_r */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_inblc_r       : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_inblc_r;

/* Define the union u_isp_crb_inblc_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_inblc_gr      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_inblc_gr;

/* Define the union u_isp_crb_inblc_gb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_inblc_gb      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_inblc_gb;

/* Define the union u_isp_crb_inblc_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_inblc_b       : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_inblc_b;

/* Define the union u_isp_crb_outblc_r */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_outblc_r      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_outblc_r;

/* Define the union u_isp_crb_outblc_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_outblc_gr     : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_outblc_gr;

/* Define the union u_isp_crb_outblc_gb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_outblc_gb     : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_outblc_gb;

/* Define the union u_isp_crb_outblc_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_outblc_b      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_outblc_b;

/* Define the union u_isp_crb_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_lut_waddr     : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_lut_waddr;

/* Define the union u_isp_crb_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_lut_wdata     : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_lut_wdata;

/* Define the union u_isp_crb_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_lut_raddr     : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_lut_raddr;

/* Define the union u_isp_crb_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_lut_rdata     : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_lut_rdata;

/* Define the union u_isp_pregammafe_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_cfg;

/* Define the union u_isp_pregammafe_idxbase0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_idxbase0 : 8; /* [7..0]  */
        unsigned int    isp_pregammafe_idxbase1 : 8; /* [15..8]  */
        unsigned int    isp_pregammafe_idxbase2 : 8; /* [23..16]  */
        unsigned int    isp_pregammafe_idxbase3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_idxbase0;

/* Define the union u_isp_pregammafe_idxbase1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_idxbase4 : 8; /* [7..0]  */
        unsigned int    isp_pregammafe_idxbase5 : 8; /* [15..8]  */
        unsigned int    isp_pregammafe_idxbase6 : 8; /* [23..16]  */
        unsigned int    isp_pregammafe_idxbase7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_idxbase1;

/* Define the union u_isp_pregammafe_maxval0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_maxval0 : 8; /* [7..0]  */
        unsigned int    isp_pregammafe_maxval1 : 8; /* [15..8]  */
        unsigned int    isp_pregammafe_maxval2 : 8; /* [23..16]  */
        unsigned int    isp_pregammafe_maxval3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_maxval0;

/* Define the union u_isp_pregammafe_maxval1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_maxval4 : 8; /* [7..0]  */
        unsigned int    isp_pregammafe_maxval5 : 8; /* [15..8]  */
        unsigned int    isp_pregammafe_maxval6 : 8; /* [23..16]  */
        unsigned int    isp_pregammafe_maxval7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_maxval1;

/* Define the union u_isp_pregammafe_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_stt2lut_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_stt2lut_cfg;

/* Define the union u_isp_pregammafe_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_stt2lut_regnew;

/* Define the union u_isp_pregammafe_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_pregammafe_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_stt2lut_abn;

/* Define the union u_isp_pregammafe_lut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregammafe_lut_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_lut_waddr;
/* Define the union u_isp_pregammafe_lut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregammafe_lut_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_lut_wdata;
/* Define the union u_isp_pregammafe_lut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregammafe_lut_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_lut_raddr;
/* Define the union u_isp_pregammafe_lut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregammafe_lut_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_lut_rdata;

/* Define the global struct */
typedef struct {
    volatile unsigned int           reserved_0[32]; /* 0x0~0x007c, reserved 32 * 4 bytes */
    volatile u_isp_be_version       isp_be_version; /* 0x0080 */
    volatile u_isp_be_date          isp_be_date; /* 0x0084 */
    volatile u_isp_be_fpga_date     isp_be_fpga_date; /* 0x0088 */
    volatile unsigned int           reserved_31; /* 0x008c */
    volatile u_isp_be_module_pos    isp_be_module_pos; /* 0x0090 */
    volatile u_isp_be_fstart_delay   isp_be_fstart_delay; /* 0x0094 */
    volatile u_isp_be_input_mux     isp_be_input_mux; /* 0x0098 */
    volatile u_isp_be_sys_ctrl      isp_be_sys_ctrl; /* 0x009c */
    volatile u_isp_be_user_define0   isp_be_user_define0; /* 0x00a0 */
    volatile u_isp_be_user_define1   isp_be_user_define1; /* 0x00a4 */
    volatile unsigned int           reserved_32[2]; /* 0x00a8~0x00ac, reserved 2 * 4 bytes */
    volatile u_isp_be_startup       isp_be_startup; /* 0x00b0 */
    volatile u_isp_be_format        isp_be_format; /* 0x00b4 */ /* 1 */
    volatile unsigned int           reserved_33[466]; /* 0x00b8~0x07fc, reserved 466 * 4 bytes */
    volatile u_isp_clip_y_cfg       isp_clip_y_cfg; /* 0x0800 */
    volatile u_isp_clip_c_cfg       isp_clip_c_cfg; /* 0x0804 */
    volatile unsigned int           reserved_34[9]; /* 0x0808~0x0828, reserved 9 * 4 bytes */
    volatile unsigned int           reserved_35; /* 0x082c */
    volatile u_isp_wdr_sum_cfg      isp_wdr_sum_cfg; /* 0x0830 */
    volatile unsigned int           reserved_36[3]; /* 0x0834~0x083c, reserved 3 * 4 bytes */
    volatile unsigned int           reserved_37[4]; /* 0x0840~0x084c, reserved 4 * 4 bytes */
    volatile unsigned int           reserved_38[4]; /* 0x0850~0x085c, reserved 4 * 4 bytes */
    volatile unsigned int           reserved_39[6]; /* 0x0860~0x0874, reserved 6 * 4 bytes */
    volatile u_isp_blk_hblank       isp_blk_hblank; /* 0x0878 */
    volatile u_isp_blk_vblank       isp_blk_vblank; /* 0x087c */
    volatile u_isp_y_sum0           isp_y_sum0; /* 0x0880 */
    volatile u_isp_y_sum1           isp_y_sum1; /* 0x0884 */
    volatile unsigned int           reserved_40[2]; /* 0x0888~0x088c, reserved 2 * 4 bytes */
    volatile u_isp_yuv444_y_sum0    isp_yuv444_y_sum0; /* 0x0890 */
    volatile u_isp_yuv444_y_sum1    isp_yuv444_y_sum1; /* 0x0894 */
    volatile u_isp_yuv444_u_sum0    isp_yuv444_u_sum0; /* 0x0898 */
    volatile u_isp_yuv444_u_sum1    isp_yuv444_u_sum1; /* 0x089c */
    volatile u_isp_yuv444_v_sum0    isp_yuv444_v_sum0; /* 0x08a0 */
    volatile u_isp_yuv444_v_sum1    isp_yuv444_v_sum1; /* 0x08a4 */
    volatile unsigned int           reserved_41[2]; /* 0x08a8~0x08ac, reserved 2 * 4 bytes */
    volatile u_isp_yuv422_y_sum0    isp_yuv422_y_sum0; /* 0x08b0 */
    volatile u_isp_yuv422_y_sum1    isp_yuv422_y_sum1; /* 0x08b4 */
    volatile u_isp_yuv422_c_sum0    isp_yuv422_c_sum0; /* 0x08b8 */
    volatile u_isp_yuv422_c_sum1    isp_yuv422_c_sum1; /* 0x08bc */
    volatile u_isp_wdr0_sum0        isp_wdr0_sum0; /* 0x08c0 */
    volatile u_isp_wdr0_sum1        isp_wdr0_sum1; /* 0x08c4 */
    volatile u_isp_wdr1_sum0        isp_wdr1_sum0; /* 0x08c8 */
    volatile u_isp_wdr1_sum1        isp_wdr1_sum1; /* 0x08cc */
    volatile u_isp_wdr2_sum0        isp_wdr2_sum0; /* 0x08d0 */
    volatile u_isp_wdr2_sum1        isp_wdr2_sum1; /* 0x08d4 */
    volatile u_isp_wdr3_sum0        isp_wdr3_sum0; /* 0x08d8 */
    volatile u_isp_wdr3_sum1        isp_wdr3_sum1; /* 0x08dc */
    volatile u_isp_demosaic_sum0    isp_demosaic_sum0; /* 0x08e0 */
    volatile u_isp_demosaic_sum1    isp_demosaic_sum1; /* 0x08e4 */
    volatile unsigned int           reserved_42[1223]; /* 0x08e8~0x1c00, reserved 1223 * 4 bytes */
    volatile u_isp_4dg_blc_cfg      isp_4dg_blc_cfg; /* 0x1c04 */
    volatile unsigned int           reserved_43; /* 0x1c08 */
    volatile u_isp_4dg_version      isp_4dg_version; /* 0x1c0c */
    volatile u_isp_4dg_0_gain1      isp_4dg_0_gain1; /* 0x1c10 */
    volatile u_isp_4dg_0_gain2      isp_4dg_0_gain2; /* 0x1c14 */
    volatile u_isp_4dg_0_blc_offset1   isp_4dg_0_blc_offset1; /* 0x1c18 */
    volatile u_isp_4dg_0_blc_offset2   isp_4dg_0_blc_offset2; /* 0x1c1c */
    volatile u_isp_4dg_1_gain1      isp_4dg_1_gain1; /* 0x1c20 */
    volatile u_isp_4dg_1_gain2      isp_4dg_1_gain2; /* 0x1c24 */
    volatile u_isp_4dg_1_blc_offset1   isp_4dg_1_blc_offset1; /* 0x1c28 */
    volatile u_isp_4dg_1_blc_offset2   isp_4dg_1_blc_offset2; /* 0x1c2c */
    volatile u_isp_4dg_2_gain1      isp_4dg_2_gain1; /* 0x1c30 */
    volatile u_isp_4dg_2_gain2      isp_4dg_2_gain2; /* 0x1c34 */
    volatile u_isp_4dg_2_blc_offset1   isp_4dg_2_blc_offset1; /* 0x1c38 */
    volatile u_isp_4dg_2_blc_offset2   isp_4dg_2_blc_offset2; /* 0x1c3c */
    volatile u_isp_4dg_3_gain1      isp_4dg_3_gain1; /* 0x1c40 */
    volatile u_isp_4dg_3_gain2      isp_4dg_3_gain2; /* 0x1c44 */
    volatile u_isp_4dg_3_blc_offset1   isp_4dg_3_blc_offset1; /* 0x1c48 */
    volatile u_isp_4dg_3_blc_offset2   isp_4dg_3_blc_offset2; /* 0x1c4c */
    volatile u_isp_4dg_0_clip_value   isp_4dg_0_clip_value; /* 0x1c50 */
    volatile u_isp_4dg_1_clip_value   isp_4dg_1_clip_value; /* 0x1c54 */
    volatile u_isp_4dg_2_clip_value   isp_4dg_2_clip_value; /* 0x1c58 */
    volatile u_isp_4dg_3_clip_value   isp_4dg_3_clip_value; /* 0x1c5c */
    volatile unsigned int           reserved_44[620]; /* 0x1c60~0x260c, reserved 620 * 4 bytes */
    volatile u_isp_flick_ctrl       isp_flick_ctrl; /* 0x2610 */
    volatile u_isp_flick_f0_inblc0   isp_flick_f0_inblc0; /* 0x2614 */
    volatile u_isp_flick_f0_inblc1   isp_flick_f0_inblc1; /* 0x2618 */
    volatile u_isp_flick_f1_inblc0   isp_flick_f1_inblc0; /* 0x261c */
    volatile u_isp_flick_f1_inblc1   isp_flick_f1_inblc1; /* 0x2620 */
    volatile u_isp_flick_f2_inblc0   isp_flick_f2_inblc0; /* 0x2624 */
    volatile u_isp_flick_f2_inblc1   isp_flick_f2_inblc1; /* 0x2628 */
    volatile u_isp_flick_f3_inblc0   isp_flick_f3_inblc0; /* 0x262c */
    volatile u_isp_flick_f3_inblc1   isp_flick_f3_inblc1; /* 0x2630 */
    volatile u_isp_flick_exporatio0   isp_flick_exporatio0; /* 0x2634 */
    volatile u_isp_flick_exporatio1   isp_flick_exporatio1; /* 0x2638 */
    volatile u_isp_flick_overth     isp_flick_overth; /* 0x263c */
    volatile u_isp_flick_overcountth   isp_flick_overcountth; /* 0x2640 */
    volatile u_isp_flick_gavg_pre   isp_flick_gavg_pre; /* 0x2644 */
    volatile u_isp_flick_countover_pre   isp_flick_countover_pre; /* 0x2648 */
    volatile u_isp_flick_countover_cur   isp_flick_countover_cur; /* 0x264c */
    volatile u_isp_flick_gr_diff_cur   isp_flick_gr_diff_cur; /* 0x2650 */
    volatile u_isp_flick_gb_diff_cur   isp_flick_gb_diff_cur; /* 0x2654 */
    volatile u_isp_flick_gr_abs_cur   isp_flick_gr_abs_cur; /* 0x2658 */
    volatile u_isp_flick_gb_abs_cur   isp_flick_gb_abs_cur; /* 0x265c */
    volatile unsigned int           reserved_45[8]; /* 0x2660~0x267c, reserved 8 * 4 bytes */
    volatile u_isp_flick_gmean_waddr   isp_flick_gmean_waddr; /* 0x2680 */
    volatile u_isp_flick_gmean_wdata   isp_flick_gmean_wdata; /* 0x2684 */
    volatile u_isp_flick_gmean_raddr   isp_flick_gmean_raddr; /* 0x2688 */
    volatile u_isp_flick_gmean_rdata   isp_flick_gmean_rdata; /* 0x268c */
    volatile unsigned int           reserved_46[189]; /* 0x2690~0x2980, reserved 189 * 4 bytes */
    volatile unsigned int           reserved_51[1055]; /* 0x2984~0x39fc, reserved 1055 * 4 bytes */
    volatile u_isp_fpn_cfg          isp_fpn_cfg; /* 0x3a00 */
    volatile u_isp_fpn_calib_start   isp_fpn_calib_start; /* 0x3a04 */
    volatile u_isp_fpn_corr_cfg     isp_fpn_corr_cfg; /* 0x3a08 */
    volatile u_isp_fpn_stat         isp_fpn_stat; /* 0x3a0c */
    volatile u_isp_fpn_white_level   isp_fpn_white_level; /* 0x3a10 */
    volatile unsigned int           reserved_52; /* 0x3a14 */
    volatile u_isp_fpn_divcoef      isp_fpn_divcoef; /* 0x3a18 */
    volatile u_isp_fpn_framelog2    isp_fpn_framelog2; /* 0x3a1c */
    volatile u_isp_fpn_sum0         isp_fpn_sum0; /* 0x3a20 */
    volatile u_isp_fpn_sum1         isp_fpn_sum1; /* 0x3a24 */
    volatile unsigned int           reserved_53[2]; /* 0x3a28~0x3a2c, reserved 2 * 4 bytes */
    volatile u_isp_fpn_corr0        isp_fpn_corr0; /* 0x3a30 */
    volatile unsigned int           reserved_54[3]; /* 0x3a34~0x3a3c, reserved 3 * 4 bytes */
    volatile u_isp_fpn_shift        isp_fpn_shift; /* 0x3a40 */
    volatile unsigned int           reserved_55[3]; /* 0x3a44~0x3a4c, reserved 3 * 4 bytes */
    volatile u_isp_fpn_max_o        isp_fpn_max_o; /* 0x3a50 */
    volatile u_isp_fpn_overflowthr   isp_fpn_overflowthr; /* 0x3a54 */
    volatile unsigned int           reserved_56[2]; /* 0x3a58~0x3a5c, reserved 2 * 4 bytes */
    volatile u_isp_fpn_black_frame_out_sel   isp_fpn_black_frame_out_sel; /* 0x3a60 */
    volatile u_isp_fpn_overlap_cfg   isp_fpn_overlap_cfg; /* 0x3a64 */
    volatile u_isp_fpn_calib_offline_set   isp_fpn_calib_offline_set; /* 0x3a68 */
    volatile u_isp_fpn_calib_stat_clear   isp_fpn_calib_stat_clear; /* 0x3a6c */
    volatile unsigned int           reserved_57[36]; /* 0x3a70~0x3afc, reserved 36 * 4 bytes */
    volatile u_isp_fpn1_cfg         isp_fpn1_cfg; /* 0x3b00 */
    volatile u_isp_fpn1_calib_start   isp_fpn1_calib_start; /* 0x3b04 */
    volatile u_isp_fpn1_corr_cfg    isp_fpn1_corr_cfg; /* 0x3b08 */
    volatile u_isp_fpn1_stat        isp_fpn1_stat; /* 0x3b0c */
    volatile u_isp_fpn1_white_level   isp_fpn1_white_level; /* 0x3b10 */
    volatile unsigned int           reserved_58; /* 0x3b14 */
    volatile u_isp_fpn1_divcoef     isp_fpn1_divcoef; /* 0x3b18 */
    volatile u_isp_fpn1_framelog2   isp_fpn1_framelog2; /* 0x3b1c */
    volatile u_isp_fpn1_sum0        isp_fpn1_sum0; /* 0x3b20 */
    volatile u_isp_fpn1_sum1        isp_fpn1_sum1; /* 0x3b24 */
    volatile unsigned int           reserved_59[2]; /* 0x3b28~0x3b2c, reserved 2 * 4 bytes */
    volatile u_isp_fpn1_corr0       isp_fpn1_corr0; /* 0x3b30 */
    volatile unsigned int           reserved_60[3]; /* 0x3b34~0x3b3c, reserved 3 * 4 bytes */
    volatile u_isp_fpn1_shift       isp_fpn1_shift; /* 0x3b40 */
    volatile unsigned int           reserved_61[3]; /* 0x3b44~0x3b4c, reserved 3 * 4 bytes */
    volatile u_isp_fpn1_max_o       isp_fpn1_max_o; /* 0x3b50 */
    volatile u_isp_fpn1_overflowthr   isp_fpn1_overflowthr; /* 0x3b54 */
    volatile unsigned int           reserved_62[2]; /* 0x3b58~0x3b5c, reserved 2 * 4 bytes */
    volatile u_isp_fpn1_black_frame_out_sel   isp_fpn1_black_frame_out_sel; /* 0x3b60 */
    volatile u_isp_fpn1_overlap_cfg   isp_fpn1_overlap_cfg; /* 0x3b64 */
    volatile u_isp_fpn1_calib_offline_set   isp_fpn1_calib_offline_set; /* 0x3b68 */
    volatile u_isp_fpn1_calib_stat_clear   isp_fpn1_calib_stat_clear; /* 0x3b6c */
    volatile unsigned int           reserved_63[36]; /* 0x3b70~0x3bfc, reserved 36 * 4 bytes */
    volatile u_isp_dpc_cfg          isp_dpc_cfg; /* 0x3c00 */
    volatile unsigned int           reserved_64; /* 0x3c04 */
    volatile u_isp_dpc_alpha        isp_dpc_alpha; /* 0x3c08 */
    volatile u_isp_dpc_version      isp_dpc_version; /* 0x3c0c */
    volatile u_isp_dpc_mode         isp_dpc_mode; /* 0x3c10 */
    volatile u_isp_dpc_output_mode   isp_dpc_output_mode; /* 0x3c14 */
    volatile u_isp_dpc_set_use      isp_dpc_set_use; /* 0x3c18 */
    volatile u_isp_dpc_methods_set_1   isp_dpc_methods_set_1; /* 0x3c1c */
    volatile u_isp_dpc_methods_set_2   isp_dpc_methods_set_2; /* 0x3c20 */
    volatile u_isp_dpc_methods_set_3   isp_dpc_methods_set_3; /* 0x3c24 */
    volatile u_isp_dpc_line_thresh_1   isp_dpc_line_thresh_1; /* 0x3c28 */
    volatile u_isp_dpc_line_mad_fac_1   isp_dpc_line_mad_fac_1; /* 0x3c2c */
    volatile u_isp_dpc_pg_fac_1     isp_dpc_pg_fac_1; /* 0x3c30 */
    volatile u_isp_dpc_rnd_thresh_1   isp_dpc_rnd_thresh_1; /* 0x3c34 */
    volatile u_isp_dpc_rg_fac_1     isp_dpc_rg_fac_1; /* 0x3c38 */
    volatile u_isp_dpc_line_thresh_2   isp_dpc_line_thresh_2; /* 0x3c3c */
    volatile u_isp_dpc_line_mad_fac_2   isp_dpc_line_mad_fac_2; /* 0x3c40 */
    volatile u_isp_dpc_pg_fac_2     isp_dpc_pg_fac_2; /* 0x3c44 */
    volatile u_isp_dpc_rnd_thresh_2   isp_dpc_rnd_thresh_2; /* 0x3c48 */
    volatile u_isp_dpc_rg_fac_2     isp_dpc_rg_fac_2; /* 0x3c4c */
    volatile u_isp_dpc_line_thresh_3   isp_dpc_line_thresh_3; /* 0x3c50 */
    volatile u_isp_dpc_line_mad_fac_3   isp_dpc_line_mad_fac_3; /* 0x3c54 */
    volatile u_isp_dpc_pg_fac_3     isp_dpc_pg_fac_3; /* 0x3c58 */
    volatile u_isp_dpc_rnd_thresh_3   isp_dpc_rnd_thresh_3; /* 0x3c5c */
    volatile u_isp_dpc_rg_fac_3     isp_dpc_rg_fac_3; /* 0x3c60 */
    volatile u_isp_dpc_ro_limits    isp_dpc_ro_limits; /* 0x3c64 */
    volatile u_isp_dpc_rnd_offs     isp_dpc_rnd_offs; /* 0x3c68 */
    volatile u_isp_dpc_bpt_ctrl     isp_dpc_bpt_ctrl; /* 0x3c6c */
    volatile u_isp_dpc_bpt_number   isp_dpc_bpt_number; /* 0x3c70 */
    volatile u_isp_dpc_bpt_calib_number   isp_dpc_bpt_calib_number; /* 0x3c74 */
    volatile unsigned int           reserved_65; /* 0x3c78 */
    volatile u_isp_dpc_bpt_thrd     isp_dpc_bpt_thrd; /* 0x3c7c */
    volatile u_isp_dpc_bpt_waddr    isp_dpc_bpt_waddr; /* 0x3c80 */
    volatile u_isp_dpc_bpt_wdata    isp_dpc_bpt_wdata; /* 0x3c84 */
    volatile u_isp_dpc_bpt_raddr    isp_dpc_bpt_raddr; /* 0x3c88 */
    volatile u_isp_dpc_bpt_rdata    isp_dpc_bpt_rdata; /* 0x3c8c */
    volatile unsigned int           reserved_66[32]; /* 0x3c90~0x3d0c, reserved 32 * 4 bytes */
    volatile u_isp_dpc_soft_thr     isp_dpc_soft_thr; /* 0x3d10 */
    volatile u_isp_dpc_bhardthr_en   isp_dpc_bhardthr_en; /* 0x3d14 */
    volatile u_isp_dpc_rakeratio    isp_dpc_rakeratio; /* 0x3d18 */
    volatile unsigned int           reserved_67; /* 0x3d1c */
    volatile u_isp_dpc_lch_thr_1    isp_dpc_lch_thr_1; /* 0x3d20 */
    volatile u_isp_dpc_lch_thr_2    isp_dpc_lch_thr_2; /* 0x3d24 */
    volatile u_isp_dpc_lch_thr_3    isp_dpc_lch_thr_3; /* 0x3d28 */
    volatile u_isp_dpc_lch_thr_4    isp_dpc_lch_thr_4; /* 0x3d2c */
    volatile u_isp_dpc_lch_thr_5    isp_dpc_lch_thr_5; /* 0x3d30 */
    volatile u_isp_dpc_line_kerdiff_fac   isp_dpc_line_kerdiff_fac; /* 0x3d34 */
    volatile u_isp_dpc_blend_mode   isp_dpc_blend_mode; /* 0x3d38 */
    volatile u_isp_dpc_bit_depth_sel   isp_dpc_bit_depth_sel; /* 0x3d3c */
    volatile u_isp_dpc_stt2lut_cfg   isp_dpc_stt2lut_cfg; /* 0x3d40 */
    volatile u_isp_dpc_stt2lut_regnew   isp_dpc_stt2lut_regnew; /* 0x3d44 */
    volatile u_isp_dpc_stt2lut_abn   isp_dpc_stt2lut_abn; /* 0x3d48 */
    volatile u_isp_dpc_rnd_thresh_1_mtp   isp_dpc_rnd_thresh_1_mtp; /* 0x3d4c */
    volatile u_isp_dpc_rnd_thresh_2_mtp   isp_dpc_rnd_thresh_2_mtp; /* 0x3d50 */
    volatile u_isp_dpc_rnd_thresh_3_mtp   isp_dpc_rnd_thresh_3_mtp; /* 0x3d54 */
    volatile u_isp_dpc_rg_fac_1_mtp   isp_dpc_rg_fac_1_mtp; /* 0x3d58 */
    volatile u_isp_dpc_rg_fac_2_mtp   isp_dpc_rg_fac_2_mtp; /* 0x3d5c */
    volatile u_isp_dpc_rg_fac_3_mtp   isp_dpc_rg_fac_3_mtp; /* 0x3d60 */
    volatile u_isp_dpc_amp_coef     isp_dpc_amp_coef; /* 0x3d64 */
    volatile u_isp_dpc_eith_1       isp_dpc_eith_1; /* 0x3d68 */
    volatile u_isp_dpc_eith_2       isp_dpc_eith_2; /* 0x3d6c */
    volatile u_isp_dpc_eith_3       isp_dpc_eith_3; /* 0x3d70 */
    volatile unsigned int           reserved_68[35]; /* 0x3d74~0x3dfc, reserved 35 * 4 bytes */
    volatile u_isp_dpc1_cfg         isp_dpc1_cfg; /* 0x3e00 */
    volatile unsigned int           reserved_69; /* 0x3e04 */
    volatile u_isp_dpc1_alpha       isp_dpc1_alpha; /* 0x3e08 */
    volatile u_isp_dpc1_version     isp_dpc1_version; /* 0x3e0c */
    volatile u_isp_dpc1_mode        isp_dpc1_mode; /* 0x3e10 */
    volatile u_isp_dpc1_output_mode   isp_dpc1_output_mode; /* 0x3e14 */
    volatile u_isp_dpc1_set_use     isp_dpc1_set_use; /* 0x3e18 */
    volatile u_isp_dpc1_methods_set_1   isp_dpc1_methods_set_1; /* 0x3e1c */
    volatile u_isp_dpc1_methods_set_2   isp_dpc1_methods_set_2; /* 0x3e20 */
    volatile u_isp_dpc1_methods_set_3   isp_dpc1_methods_set_3; /* 0x3e24 */
    volatile u_isp_dpc1_line_thresh_1   isp_dpc1_line_thresh_1; /* 0x3e28 */
    volatile u_isp_dpc1_line_mad_fac_1   isp_dpc1_line_mad_fac_1; /* 0x3e2c */
    volatile u_isp_dpc1_pg_fac_1    isp_dpc1_pg_fac_1; /* 0x3e30 */
    volatile u_isp_dpc1_rnd_thresh_1   isp_dpc1_rnd_thresh_1; /* 0x3e34 */
    volatile u_isp_dpc1_rg_fac_1    isp_dpc1_rg_fac_1; /* 0x3e38 */
    volatile u_isp_dpc1_line_thresh_2   isp_dpc1_line_thresh_2; /* 0x3e3c */
    volatile u_isp_dpc1_line_mad_fac_2   isp_dpc1_line_mad_fac_2; /* 0x3e40 */
    volatile u_isp_dpc1_pg_fac_2    isp_dpc1_pg_fac_2; /* 0x3e44 */
    volatile u_isp_dpc1_rnd_thresh_2   isp_dpc1_rnd_thresh_2; /* 0x3e48 */
    volatile u_isp_dpc1_rg_fac_2    isp_dpc1_rg_fac_2; /* 0x3e4c */
    volatile u_isp_dpc1_line_thresh_3   isp_dpc1_line_thresh_3; /* 0x3e50 */
    volatile u_isp_dpc1_line_mad_fac_3   isp_dpc1_line_mad_fac_3; /* 0x3e54 */
    volatile u_isp_dpc1_pg_fac_3    isp_dpc1_pg_fac_3; /* 0x3e58 */
    volatile u_isp_dpc1_rnd_thresh_3   isp_dpc1_rnd_thresh_3; /* 0x3e5c */
    volatile u_isp_dpc1_rg_fac_3    isp_dpc1_rg_fac_3; /* 0x3e60 */
    volatile u_isp_dpc1_ro_limits   isp_dpc1_ro_limits; /* 0x3e64 */
    volatile u_isp_dpc1_rnd_offs    isp_dpc1_rnd_offs; /* 0x3e68 */
    volatile u_isp_dpc1_bpt_ctrl    isp_dpc1_bpt_ctrl; /* 0x3e6c */
    volatile u_isp_dpc1_bpt_number   isp_dpc1_bpt_number; /* 0x3e70 */
    volatile u_isp_dpc1_bpt_calib_number   isp_dpc1_bpt_calib_number; /* 0x3e74 */
    volatile unsigned int           reserved_70; /* 0x3e78 */
    volatile u_isp_dpc1_bpt_thrd    isp_dpc1_bpt_thrd; /* 0x3e7c */
    volatile u_isp_dpc1_bpt_waddr   isp_dpc1_bpt_waddr; /* 0x3e80 */
    volatile u_isp_dpc1_bpt_wdata   isp_dpc1_bpt_wdata; /* 0x3e84 */
    volatile u_isp_dpc1_bpt_raddr   isp_dpc1_bpt_raddr; /* 0x3e88 */
    volatile u_isp_dpc1_bpt_rdata   isp_dpc1_bpt_rdata; /* 0x3e8c */
    volatile unsigned int           reserved_71[32]; /* 0x3e90~0x3f0c, reserved 32 * 4 bytes */
    volatile u_isp_dpc1_soft_thr    isp_dpc1_soft_thr; /* 0x3f10 */
    volatile u_isp_dpc1_bhardthr_en   isp_dpc1_bhardthr_en; /* 0x3f14 */
    volatile u_isp_dpc1_rakeratio   isp_dpc1_rakeratio; /* 0x3f18 */
    volatile unsigned int           reserved_72; /* 0x3f1c */
    volatile u_isp_dpc1_lch_thr_1   isp_dpc1_lch_thr_1; /* 0x3f20 */
    volatile u_isp_dpc1_lch_thr_2   isp_dpc1_lch_thr_2; /* 0x3f24 */
    volatile u_isp_dpc1_lch_thr_3   isp_dpc1_lch_thr_3; /* 0x3f28 */
    volatile u_isp_dpc1_lch_thr_4   isp_dpc1_lch_thr_4; /* 0x3f2c */
    volatile u_isp_dpc1_lch_thr_5   isp_dpc1_lch_thr_5; /* 0x3f30 */
    volatile u_isp_dpc1_line_kerdiff_fac   isp_dpc1_line_kerdiff_fac; /* 0x3f34 */
    volatile u_isp_dpc1_blend_mode   isp_dpc1_blend_mode; /* 0x3f38 */
    volatile u_isp_dpc1_bit_depth_sel   isp_dpc1_bit_depth_sel; /* 0x3f3c */
    volatile u_isp_dpc1_stt2lut_cfg   isp_dpc1_stt2lut_cfg; /* 0x3f40 */
    volatile u_isp_dpc1_stt2lut_regnew   isp_dpc1_stt2lut_regnew; /* 0x3f44 */
    volatile u_isp_dpc1_stt2lut_abn   isp_dpc1_stt2lut_abn; /* 0x3f48 */
    volatile u_isp_dpc1_rnd_thresh_1_mtp   isp_dpc1_rnd_thresh_1_mtp; /* 0x3f4c */
    volatile u_isp_dpc1_rnd_thresh_2_mtp   isp_dpc1_rnd_thresh_2_mtp; /* 0x3f50 */
    volatile u_isp_dpc1_rnd_thresh_3_mtp   isp_dpc1_rnd_thresh_3_mtp; /* 0x3f54 */
    volatile u_isp_dpc1_rg_fac_1_mtp   isp_dpc1_rg_fac_1_mtp; /* 0x3f58 */
    volatile u_isp_dpc1_rg_fac_2_mtp   isp_dpc1_rg_fac_2_mtp; /* 0x3f5c */
    volatile u_isp_dpc1_rg_fac_3_mtp   isp_dpc1_rg_fac_3_mtp; /* 0x3f60 */
    volatile u_isp_dpc1_amp_coef    isp_dpc1_amp_coef; /* 0x3f64 */
    volatile u_isp_dpc1_eith_1      isp_dpc1_eith_1; /* 0x3f68 */
    volatile u_isp_dpc1_eith_2      isp_dpc1_eith_2; /* 0x3f6c */
    volatile u_isp_dpc1_eith_3      isp_dpc1_eith_3; /* 0x3f70 */
    volatile unsigned int           reserved_73[291]; /* 0x3f74~0x43fc, reserved 291 * 4 bytes */
    volatile u_isp_rgbir_cfg        isp_rgbir_cfg; /* 0x4400 */
    volatile u_isp_rgbir_thre       isp_rgbir_thre; /* 0x4404 */
    volatile unsigned int           reserved_74; /* 0x4408 */
    volatile u_isp_rgbir_version    isp_rgbir_version; /* 0x440c */
    volatile u_isp_rgbir_exp_thr    isp_rgbir_exp_thr; /* 0x4410 */
    volatile u_isp_rgbir_reci_ctl   isp_rgbir_reci_ctl; /* 0x4414 */
    volatile u_isp_rgbir_gain       isp_rgbir_gain; /* 0x4418 */
    volatile unsigned int           reserved_75; /* 0x441c */
    volatile u_isp_rgbir_cvt01      isp_rgbir_cvt01; /* 0x4420 */
    volatile u_isp_rgbir_cvt23      isp_rgbir_cvt23; /* 0x4424 */
    volatile u_isp_rgbir_cvt45      isp_rgbir_cvt45; /* 0x4428 */
    volatile u_isp_rgbir_cvt67      isp_rgbir_cvt67; /* 0x442c */
    volatile u_isp_rgbir_cvt89      isp_rgbir_cvt89; /* 0x4430 */
    volatile u_isp_rgbir_cvt1011    isp_rgbir_cvt1011; /* 0x4434 */
    volatile unsigned int           reserved_76[2]; /* 0x4438~0x443c, reserved 2 * 4 bytes */
    volatile u_isp_rgbir_blc_offset_r   isp_rgbir_blc_offset_r; /* 0x4440 */
    volatile u_isp_rgbir_blc_offset_g   isp_rgbir_blc_offset_g; /* 0x4444 */
    volatile u_isp_rgbir_blc_offset_b   isp_rgbir_blc_offset_b; /* 0x4448 */
    volatile u_isp_rgbir_blc_offset_ir   isp_rgbir_blc_offset_ir; /* 0x444c */
    volatile u_isp_rgbir_ir_sum     isp_rgbir_ir_sum; /* 0x4450 */
    volatile u_isp_rgbir_thd_grad   isp_rgbir_thd_grad; /* 0x4454 */
    volatile u_isp_rgbir_lut0_1     isp_rgbir_lut0_1; /* 0x4458 */
    volatile u_isp_rgbir_lut2_3     isp_rgbir_lut2_3; /* 0x445c */
    volatile u_isp_rgbir_lut4_5     isp_rgbir_lut4_5; /* 0x4460 */
    volatile u_isp_rgbir_lut6_7     isp_rgbir_lut6_7; /* 0x4464 */
    volatile u_isp_rgbir_lut8_9     isp_rgbir_lut8_9; /* 0x4468 */
    volatile u_isp_rgbir_lut10_11   isp_rgbir_lut10_11; /* 0x446c */
    volatile u_isp_rgbir_lut12_13   isp_rgbir_lut12_13; /* 0x4470 */
    volatile u_isp_rgbir_lut14_15   isp_rgbir_lut14_15; /* 0x4474 */
    volatile u_isp_rgbir_lut16_17   isp_rgbir_lut16_17; /* 0x4478 */
    volatile u_isp_rgbir_lut18_19   isp_rgbir_lut18_19; /* 0x447c */
    volatile u_isp_rgbir_lut20_21   isp_rgbir_lut20_21; /* 0x4480 */
    volatile u_isp_rgbir_lut22_23   isp_rgbir_lut22_23; /* 0x4484 */
    volatile u_isp_rgbir_lut24_25   isp_rgbir_lut24_25; /* 0x4488 */
    volatile u_isp_rgbir_lut26_27   isp_rgbir_lut26_27; /* 0x448c */
    volatile u_isp_rgbir_lut28_29   isp_rgbir_lut28_29; /* 0x4490 */
    volatile u_isp_rgbir_lut30_31   isp_rgbir_lut30_31; /* 0x4494 */
    volatile u_isp_rgbir_lut32      isp_rgbir_lut32; /* 0x4498 */
    volatile u_isp_rgbir_ct_ratio_r_g   isp_rgbir_ct_ratio_r_g; /* 0x449c */
    volatile u_isp_rgbir_ct_ratio_b   isp_rgbir_ct_ratio_b; /* 0x44a0 */
    volatile u_isp_rgbir_awb_gain   isp_rgbir_awb_gain; /* 0x44a4 */
    volatile u_isp_rgbir_ctl_str    isp_rgbir_ctl_str; /* 0x44a8 */
    volatile u_isp_rgbir_auto_gain   isp_rgbir_auto_gain; /* 0x44ac */
    volatile u_isp_rgbir_auto_gain_o   isp_rgbir_auto_gain_o; /* 0x44b0 */
    volatile unsigned int           reserved_77[83]; /* 0x44b4~0x45fc, reserved 83 * 4 bytes */
    volatile u_isp_ge_cfg           isp_ge_cfg; /* 0x4600 */
    volatile u_isp_ge_blc_en        isp_ge_blc_en; /* 0x4604 */
    volatile unsigned int           reserved_78; /* 0x4608 */
    volatile u_isp_ge_version       isp_ge_version; /* 0x460c */
    volatile u_isp_ge0_ct_th1       isp_ge0_ct_th1; /* 0x4610 */
    volatile u_isp_ge0_ct_th2       isp_ge0_ct_th2; /* 0x4614 */
    volatile u_isp_ge0_ct_th3       isp_ge0_ct_th3; /* 0x4618 */
    volatile u_isp_ge0_ct_slope     isp_ge0_ct_slope; /* 0x461c */
    volatile u_isp_ge1_ct_th1       isp_ge1_ct_th1; /* 0x4620 */
    volatile u_isp_ge1_ct_th2       isp_ge1_ct_th2; /* 0x4624 */
    volatile u_isp_ge1_ct_th3       isp_ge1_ct_th3; /* 0x4628 */
    volatile u_isp_ge1_ct_slope     isp_ge1_ct_slope; /* 0x462c */
    volatile u_isp_ge0_blc_offset   isp_ge0_blc_offset; /* 0x4630 */
    volatile u_isp_ge1_blc_offset   isp_ge1_blc_offset; /* 0x4634 */
    volatile unsigned int           reserved_79[7]; /* 0x4638~0x4650, reserved 7 * 4 bytes */
    volatile u_isp_ge_mode          isp_ge_mode; /* 0x4654 */
    volatile u_isp_ge_strength      isp_ge_strength; /* 0x4658 */
    volatile unsigned int           reserved_80[2]; /* 0x465c~0x4660, reserved 2 * 4 bytes */
    volatile u_isp_ge_bit_depth_sel   isp_ge_bit_depth_sel; /* 0x4664 */
    volatile unsigned int           reserved_81[38]; /* 0x4668~0x46fc, reserved 38 * 4 bytes */
    volatile u_isp_ge1_ge_cfg       isp_ge1_ge_cfg; /* 0x4700 */
    volatile u_isp_ge1_ge_blc_en    isp_ge1_ge_blc_en; /* 0x4704 */
    volatile unsigned int           reserved_82; /* 0x4708 */
    volatile u_isp_ge1_ge_version   isp_ge1_ge_version; /* 0x470c */
    volatile u_isp_ge1_ge0_ct_th1   isp_ge1_ge0_ct_th1; /* 0x4710 */
    volatile u_isp_ge1_ge0_ct_th2   isp_ge1_ge0_ct_th2; /* 0x4714 */
    volatile u_isp_ge1_ge0_ct_th3   isp_ge1_ge0_ct_th3; /* 0x4718 */
    volatile u_isp_ge1_ge0_ct_slope   isp_ge1_ge0_ct_slope; /* 0x471c */
    volatile u_isp_ge1_ge1_ct_th1   isp_ge1_ge1_ct_th1; /* 0x4720 */
    volatile u_isp_ge1_ge1_ct_th2   isp_ge1_ge1_ct_th2; /* 0x4724 */
    volatile u_isp_ge1_ge1_ct_th3   isp_ge1_ge1_ct_th3; /* 0x4728 */
    volatile u_isp_ge1_ge1_ct_slope   isp_ge1_ge1_ct_slope; /* 0x472c */
    volatile u_isp_ge1_ge0_blc_offset   isp_ge1_ge0_blc_offset; /* 0x4730 */
    volatile u_isp_ge1_ge1_blc_offset   isp_ge1_ge1_blc_offset; /* 0x4734 */
    volatile unsigned int           reserved_83[7]; /* 0x4738~0x4750, reserved 7 * 4 bytes */
    volatile u_isp_ge1_ge_mode      isp_ge1_ge_mode; /* 0x4754 */
    volatile u_isp_ge1_ge_strength   isp_ge1_ge_strength; /* 0x4758 */
    volatile unsigned int           reserved_84[2]; /* 0x475c~0x4760, reserved 2 * 4 bytes */
    volatile u_isp_ge1_ge_bit_depth_sel   isp_ge1_ge_bit_depth_sel; /* 0x4764 */
    volatile unsigned int           reserved_85[1578]; /* 0x4768~0x600c, reserved 1578 * 4 bytes */
    volatile u_isp_wdr_ctrl         isp_wdr_ctrl; /* 0x6010 */
    volatile u_isp_wdr_f0_inblc0    isp_wdr_f0_inblc0; /* 0x6014 */
    volatile u_isp_wdr_f0_inblc1    isp_wdr_f0_inblc1; /* 0x6018 */
    volatile u_isp_wdr_f1_inblc0    isp_wdr_f1_inblc0; /* 0x601c */
    volatile u_isp_wdr_f1_inblc1    isp_wdr_f1_inblc1; /* 0x6020 */
    volatile u_isp_wdr_f2_inblc0    isp_wdr_f2_inblc0; /* 0x6024 */
    volatile u_isp_wdr_f2_inblc1    isp_wdr_f2_inblc1; /* 0x6028 */
    volatile u_isp_wdr_f3_inblc0    isp_wdr_f3_inblc0; /* 0x602c */
    volatile u_isp_wdr_f3_inblc1    isp_wdr_f3_inblc1; /* 0x6030 */
    volatile u_isp_wdr_outblc       isp_wdr_outblc; /* 0x6034 */
    volatile u_isp_wdr_wdr_expovalue0   isp_wdr_wdr_expovalue0; /* 0x6038 */
    volatile u_isp_wdr_wdr_expovalue1   isp_wdr_wdr_expovalue1; /* 0x603c */
    volatile u_isp_wdr_wdr_expovalue2   isp_wdr_wdr_expovalue2; /* 0x6040 */
    volatile u_isp_wdr_wdr_expovalue3   isp_wdr_wdr_expovalue3; /* 0x6044 */
    volatile u_isp_wdr_fusion_expovalue0   isp_wdr_fusion_expovalue0; /* 0x6048 */
    volatile u_isp_wdr_fusion_expovalue1   isp_wdr_fusion_expovalue1; /* 0x604c */
    volatile u_isp_wdr_fusion_expovalue2   isp_wdr_fusion_expovalue2; /* 0x6050 */
    volatile u_isp_wdr_fusion_expovalue3   isp_wdr_fusion_expovalue3; /* 0x6054 */
    volatile u_isp_wdr_exporratio   isp_wdr_exporratio; /* 0x6058 */
    volatile u_isp_wdr_blc_comp0    isp_wdr_blc_comp0; /* 0x605c */
    volatile u_isp_wdr_blc_comp1    isp_wdr_blc_comp1; /* 0x6060 */
    volatile u_isp_wdr_blc_comp2    isp_wdr_blc_comp2; /* 0x6064 */
    volatile u_isp_wdr_wdr_maxratio   isp_wdr_wdr_maxratio; /* 0x6068 */
    volatile u_isp_wdr_fusion_maxratio   isp_wdr_fusion_maxratio; /* 0x606c */
    volatile u_isp_wdr_saturate_thr   isp_wdr_saturate_thr; /* 0x6070 */
    volatile u_isp_wdr_still_thr0   isp_wdr_still_thr0; /* 0x6074 */
    volatile u_isp_wdr_still_thr1   isp_wdr_still_thr1; /* 0x6078 */
    volatile u_isp_wdr_still_thr2   isp_wdr_still_thr2; /* 0x607c */
    volatile u_isp_wdr_mdtlbld      isp_wdr_mdtlbld; /* 0x6080 */
    volatile u_isp_wdr_mdt_thr      isp_wdr_mdt_thr; /* 0x6084 */
    volatile u_isp_wdr_forcelong_para   isp_wdr_forcelong_para; /* 0x6088 */
    volatile u_isp_wdr_mdt_noisefloor   isp_wdr_mdt_noisefloor; /* 0x608c */
    volatile u_isp_wdr_mdthr_gain0   isp_wdr_mdthr_gain0; /* 0x6090 */
    volatile u_isp_wdr_mdthr_gain1   isp_wdr_mdthr_gain1; /* 0x6094 */
    volatile u_isp_wdr_mdthr_gain2   isp_wdr_mdthr_gain2; /* 0x6098 */
    volatile u_isp_wdr_shortexpo_chk   isp_wdr_shortexpo_chk; /* 0x609c */
    volatile u_isp_wdr_pix_avg_diff   isp_wdr_pix_avg_diff; /* 0x60a0 */
    volatile u_isp_wdr_mask_similar   isp_wdr_mask_similar; /* 0x60a4 */
    volatile u_isp_wdr_wgtidx_blendratio   isp_wdr_wgtidx_blendratio; /* 0x60a8 */
    volatile u_isp_wdr_wgtidx_thr0   isp_wdr_wgtidx_thr0; /* 0x60ac */
    volatile u_isp_wdr_wgtidx_thr1   isp_wdr_wgtidx_thr1; /* 0x60b0 */
    volatile u_isp_wdr_wgtidx_thr2   isp_wdr_wgtidx_thr2; /* 0x60b4 */
    volatile u_isp_wdr_dftwgt_fl    isp_wdr_dftwgt_fl; /* 0x60b8 */
    volatile u_isp_wdr_sensor_gain   isp_wdr_sensor_gain; /* 0x60bc */
    volatile u_isp_wdr_fusion_thr_r0   isp_wdr_fusion_thr_r0; /* 0x60c0 */
    volatile u_isp_wdr_fusion_thr_r1   isp_wdr_fusion_thr_r1; /* 0x60c4 */
    volatile u_isp_wdr_fusion_thr_b0   isp_wdr_fusion_thr_b0; /* 0x60c8 */
    volatile u_isp_wdr_fusion_thr_b1   isp_wdr_fusion_thr_b1; /* 0x60cc */
    volatile u_isp_wdr_fusion_thr_g0   isp_wdr_fusion_thr_g0; /* 0x60d0 */
    volatile u_isp_wdr_fusion_thr_g1   isp_wdr_fusion_thr_g1; /* 0x60d4 */
    volatile u_isp_wdr_fusion_sat_thd   isp_wdr_fusion_sat_thd; /* 0x60d8 */
    volatile u_isp_wdr_stat_blk_num   isp_wdr_stat_blk_num; /* 0x60dc */
    volatile u_isp_wdr_clac_blk_num   isp_wdr_clac_blk_num; /* 0x60e0 */
    volatile u_isp_wdr_bginfo_thr0   isp_wdr_bginfo_thr0; /* 0x60e4 */
    volatile u_isp_wdr_bginfo_thr1   isp_wdr_bginfo_thr1; /* 0x60e8 */
    volatile u_isp_wdr_split_info   isp_wdr_split_info; /* 0x60ec */
    volatile u_isp_wdr_awb_gain     isp_wdr_awb_gain; /* 0x60f0 */
    volatile u_isp_wdr_upscale      isp_wdr_upscale; /* 0x60f4 */
    volatile u_isp_wdr_smallmap_offset   isp_wdr_smallmap_offset; /* 0x60f8 */
    volatile u_isp_wdr_statistic    isp_wdr_statistic; /* 0x60fc */
    volatile u_isp_wdr_stat_still_thr   isp_wdr_stat_still_thr; /* 0x6100 */
    volatile u_isp_wdr_ginter_thr   isp_wdr_ginter_thr; /* 0x6104 */
    volatile u_isp_wdr_statistic_flt_thr0   isp_wdr_statistic_flt_thr0; /* 0x6108 */
    volatile u_isp_wdr_statistic_flt_thr1   isp_wdr_statistic_flt_thr1; /* 0x610c */
    volatile u_isp_wdr_statistic_flt_thr2   isp_wdr_statistic_flt_thr2; /* 0x6110 */
    volatile u_isp_wdr_stat_pos     isp_wdr_stat_pos; /* 0x6114 */
    volatile u_isp_wdr_blend_gain   isp_wdr_blend_gain; /* 0x6118 */
    volatile u_isp_wdr_gwgt         isp_wdr_gwgt; /* 0x611c */
    volatile u_isp_wdr_gwgt_thr0    isp_wdr_gwgt_thr0; /* 0x6120 */
    volatile u_isp_wdr_gwgt_thr1    isp_wdr_gwgt_thr1; /* 0x6124 */
    volatile u_isp_wdr_lut_seg_idxbase0   isp_wdr_lut_seg_idxbase0; /* 0x6128 */
    volatile u_isp_wdr_lut_seg_idxbase1   isp_wdr_lut_seg_idxbase1; /* 0x612c */
    volatile u_isp_wdr_lut_seg_idxbase2   isp_wdr_lut_seg_idxbase2; /* 0x6130 */
    volatile u_isp_wdr_lut_seg_maxval0   isp_wdr_lut_seg_maxval0; /* 0x6134 */
    volatile u_isp_wdr_lut_seg_maxval1   isp_wdr_lut_seg_maxval1; /* 0x6138 */
    volatile u_isp_wdr_lut_seg_maxval2   isp_wdr_lut_seg_maxval2; /* 0x613c */
    volatile u_isp_wdr_filter_coefh   isp_wdr_filter_coefh; /* 0x6140 */
    volatile u_isp_wdr_filter_coefv   isp_wdr_filter_coefv; /* 0x6144 */
    volatile u_isp_wdr_lut_update   isp_wdr_lut_update; /* 0x6148 */
    volatile unsigned int           reserved_999[13]; /* 0x614c~0x617c, reserved 13 * 4 bytes */
    volatile u_isp_wdr_snoise_lut_waddr   isp_wdr_snoise_lut_waddr; /* 0x6180 */
    volatile u_isp_wdr_snoise_lut_wdata   isp_wdr_snoise_lut_wdata; /* 0x6184 */
    volatile u_isp_wdr_snoise_lut_raddr   isp_wdr_snoise_lut_raddr; /* 0x6188 */
    volatile u_isp_wdr_snoise_lut_rdata   isp_wdr_snoise_lut_rdata; /* 0x618c */
    volatile u_isp_wdr_lnoise_lut_waddr   isp_wdr_lnoise_lut_waddr; /* 0x6190 */
    volatile u_isp_wdr_lnoise_lut_wdata   isp_wdr_lnoise_lut_wdata; /* 0x6194 */
    volatile u_isp_wdr_lnoise_lut_raddr   isp_wdr_lnoise_lut_raddr; /* 0x6198 */
    volatile u_isp_wdr_lnoise_lut_rdata   isp_wdr_lnoise_lut_rdata; /* 0x619c */
    volatile u_isp_wdr_normalize_lut_waddr   isp_wdr_normalize_lut_waddr; /* 0x61a0 */
    volatile u_isp_wdr_normalize_lut_wdata   isp_wdr_normalize_lut_wdata; /* 0x61a4 */
    volatile u_isp_wdr_normalize_lut_raddr   isp_wdr_normalize_lut_raddr; /* 0x61a8 */
    volatile u_isp_wdr_normalize_lut_rdata   isp_wdr_normalize_lut_rdata; /* 0x61ac */
    volatile unsigned int           reserved_87[408]; /* 0x61b0~0x680c, reserved 408 * 4 bytes */
    volatile u_isp_expander_bitw    isp_expander_bitw; /* 0x6810 */
    volatile u_isp_expander_offsetr   isp_expander_offsetr; /* 0x6814 */
    volatile u_isp_expander_offsetgr   isp_expander_offsetgr; /* 0x6818 */
    volatile u_isp_expander_offsetgb   isp_expander_offsetgb; /* 0x681c */
    volatile u_isp_expander_offsetb   isp_expander_offsetb; /* 0x6820 */
    volatile unsigned int           reserved_887[23]; /* 0x6824~0x687c, reserved 23 * 4 bytes */
    volatile u_isp_expander_lut_waddr   isp_expander_lut_waddr; /* 0x6880 */
    volatile u_isp_expander_lut_wdata   isp_expander_lut_wdata; /* 0x6884 */
    volatile u_isp_expander_lut_raddr   isp_expander_lut_raddr; /* 0x6888 */
    volatile u_isp_expander_lut_rdata   isp_expander_lut_rdata; /* 0x688c */
    volatile unsigned int           reserved_88[220]; /* 0x6890~0x6bfc, reserved 220 * 4 bytes */
    volatile u_isp_bcom_cfg         isp_bcom_cfg; /* 0x6c00 */
    volatile unsigned int           reserved_89[2]; /* 0x6c04~0x6c08, reserved 2 * 4 bytes */
    volatile u_isp_bcom_version     isp_bcom_version; /* 0x6c0c */
    volatile u_isp_bcom_alpha       isp_bcom_alpha; /* 0x6c10 */
    volatile unsigned int           reserved_90[187]; /* 0x6c14~0x6efc, reserved 187 * 4 bytes */
    volatile u_isp_crb_cfg          isp_crb_cfg; /* 0x6f00 */
    volatile u_isp_crb_dc_size      isp_crb_dc_size; /* 0x6f04 */
    volatile u_isp_crb_kx           isp_crb_kx; /* 0x6f08 */
    volatile u_isp_crb_ky           isp_crb_ky; /* 0x6f0c */
    volatile u_isp_crb_smlmapoffset   isp_crb_smlmapoffset; /* 0x6f10 */
    volatile u_isp_crb_ccrcc        isp_crb_ccrcc; /* 0x6f14 */
    volatile u_isp_crb_awbgain      isp_crb_awbgain; /* 0x6f18 */
    volatile u_isp_crb_gr           isp_crb_gr; /* 0x6f1c */
    volatile u_isp_crb_cchigh       isp_crb_cchigh; /* 0x6f20 */
    volatile u_isp_crb_mix0         isp_crb_mix0; /* 0x6f24 */
    volatile u_isp_crb_maxlevel     isp_crb_maxlevel; /* 0x6f28 */
    volatile u_isp_crb_highlevel    isp_crb_highlevel; /* 0x6f2c */
    volatile u_isp_crb_cc           isp_crb_cc; /* 0x6f30 */
    volatile unsigned int           reserved_91[3]; /* 0x6f34~0x6f3c, reserved 3 * 4 bytes */
    volatile u_isp_crb_stt2lut_cfg   isp_crb_stt2lut_cfg; /* 0x6f40 */
    volatile u_isp_crb_stt2lut_regnew   isp_crb_stt2lut_regnew; /* 0x6f44 */
    volatile u_isp_crb_stt2lut_abn   isp_crb_stt2lut_abn; /* 0x6f48 */
    volatile u_isp_crb_inblc_r      isp_crb_inblc_r; /* 0x6f4c */
    volatile u_isp_crb_inblc_gr     isp_crb_inblc_gr; /* 0x6f50 */
    volatile u_isp_crb_inblc_gb     isp_crb_inblc_gb; /* 0x6f54 */
    volatile u_isp_crb_inblc_b      isp_crb_inblc_b; /* 0x6f58 */
    volatile u_isp_crb_outblc_r     isp_crb_outblc_r; /* 0x6f5c */
    volatile u_isp_crb_outblc_gr    isp_crb_outblc_gr; /* 0x6f60 */
    volatile u_isp_crb_outblc_gb    isp_crb_outblc_gb; /* 0x6f64 */
    volatile u_isp_crb_outblc_b     isp_crb_outblc_b; /* 0x6f68 */
    volatile unsigned int           reserved_992[5]; /* 0x6f6c~0x6f7c, reserved 5 * 4 bytes */
    volatile u_isp_crb_lut_waddr    isp_crb_lut_waddr; /* 0x6f80 */
    volatile u_isp_crb_lut_wdata    isp_crb_lut_wdata; /* 0x6f84 */
    volatile u_isp_crb_lut_raddr    isp_crb_lut_raddr; /* 0x6f88 */
    volatile u_isp_crb_lut_rdata    isp_crb_lut_rdata; /* 0x6f8c */
    volatile unsigned int           reserved_93[1308]; /* 0x6f90~0x83fc, reserved 1308 * 4 bytes */
    volatile u_isp_pregammafe_cfg   isp_pregammafe_cfg; /* 0x8400 */
    volatile unsigned int           reserved_94[3]; /* 0x8404~0x840c, reserved 3 * 4 bytes */
    volatile u_isp_pregammafe_idxbase0   isp_pregammafe_idxbase0; /* 0x8410 */
    volatile u_isp_pregammafe_idxbase1   isp_pregammafe_idxbase1; /* 0x8414 */
    volatile u_isp_pregammafe_maxval0   isp_pregammafe_maxval0; /* 0x8418 */
    volatile u_isp_pregammafe_maxval1   isp_pregammafe_maxval1; /* 0x841c */
    volatile u_isp_pregammafe_stt2lut_cfg   isp_pregammafe_stt2lut_cfg; /* 0x8420 */
    volatile u_isp_pregammafe_stt2lut_regnew   isp_pregammafe_stt2lut_regnew; /* 0x8424 */
    volatile u_isp_pregammafe_stt2lut_abn   isp_pregammafe_stt2lut_abn; /* 0x8428 */
    volatile unsigned int           reserved_95[21]; /* 0x842c~0x847c, reserved 21 * 4 bytes */
    volatile u_isp_pregammafe_lut_waddr   isp_pregammafe_lut_waddr; /* 0x8480 */
    volatile u_isp_pregammafe_lut_wdata   isp_pregammafe_lut_wdata; /* 0x8484 */
    volatile u_isp_pregammafe_lut_raddr   isp_pregammafe_lut_raddr; /* 0x8488 */
    volatile u_isp_pregammafe_lut_rdata   isp_pregammafe_lut_rdata; /* 0x848c */
    volatile unsigned int           reserved_96[100]; /* 0x8490~0x861c, reserved 100 * 4 bytes */
    volatile unsigned int           reserved_97[12]; /* 0x8490~0x861c, reserved 100 * 4 bytes */
    volatile unsigned int            reserved_98[44]; /* 0x8650 ~ 0x86fc, reserved 44 * 4 bytes */

    isp_pre_be_lut_type              pre_be_lut;                         /* 0x8700 ~ 0x103fc */
} isp_pre_be_reg_type;

/* post */
/* Define the union u_isp_blc_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_version;
/* Define the union u_isp_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_ofsgr         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_blc_ofsr          : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_offset1;

/* Define the union u_isp_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_blc_ofsgb         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_blc_ofsb          : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_blc_offset2;

/* Define the union u_isp_wb_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_en_in          : 1; /* [0]  */
        unsigned int    isp_wb_en_out         : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_blc_cfg;

/* Define the union u_isp_wb_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_version;
/* Define the union u_isp_wb_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_grgain         : 16; /* [15..0]  */
        unsigned int    isp_wb_rgain          : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_gain1;

/* Define the union u_isp_wb_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_gbgain         : 16; /* [15..0]  */
        unsigned int    isp_wb_bgain          : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_gain2;

/* Define the union u_isp_wb_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_ofsgr          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wb_ofsr           : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_blc_offset1;

/* Define the union u_isp_wb_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_ofsgb          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_wb_ofsb           : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_blc_offset2;

/* Define the union u_isp_wb_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_clip_value     : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_clip_value;

/* Define the union u_isp_wb_gain3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_irgain         : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_gain3;

/* Define the union u_isp_wb_blc_offset3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wb_ofsir          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wb_blc_offset3;

/* Define the union u_isp_dg_blc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_en_in          : 1; /* [0]  */
        unsigned int    isp_dg_en_out         : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_cfg;

/* Define the union u_isp_dg_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_version;
/* Define the union u_isp_dg_gain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_grgain         : 16; /* [15..0]  */
        unsigned int    isp_dg_rgain          : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_gain1;

/* Define the union u_isp_dg_gain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_gbgain         : 16; /* [15..0]  */
        unsigned int    isp_dg_bgain          : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_gain2;

/* Define the union u_isp_dg_blc_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_ofsgr          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dg_ofsr           : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_offset1;

/* Define the union u_isp_dg_blc_offset2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_ofsgb          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_dg_ofsb           : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_offset2;

/* Define the union u_isp_dg_clip_value */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_clip_value     : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_clip_value;

/* Define the union u_isp_dg_gain3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_irgain         : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_gain3;

/* Define the union u_isp_dg_blc_offset3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dg_ofsir          : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dg_blc_offset3;

/* Define the union u_isp_cc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 2; /* [1..0]  */
        unsigned int    isp_cc_colortone_en   : 1; /* [2]  */
        unsigned int    isp_cc_recover_en     : 1; /* [3]  */
        unsigned int    reserved_1            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_cfg;

/* Define the union u_isp_cc_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_cc_version         : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_version;
/* Define the union u_isp_cc_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_cc_coef00         : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_cc_coef01         : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_coef0;

/* Define the union u_isp_cc_coef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_cc_coef02         : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_cc_coef10         : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_coef1;

/* Define the union u_isp_cc_coef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_cc_coef11         : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_cc_coef12         : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_coef2;

/* Define the union u_isp_cc_coef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_cc_coef20         : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_cc_coef21         : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_coef3;

/* Define the union u_isp_cc_coef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_cc_coef22         : 15; /* [15..1]  */
        unsigned int    reserved_1            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_coef4;

/* Define the union u_isp_cc_rcv_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_soft_clip0_step : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_cc_soft_clip1_step : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    reserved_2            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_rcv_ctrl0;

/* Define the union u_isp_cc_rcv_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_darkprev       : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_cc_peaksupp_sat   : 8; /* [15..8]  */
        unsigned int    isp_cc_peaksupp_max   : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_rcv_ctrl1;

/* Define the union u_isp_cc_lumafact */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_luma_coefr     : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_cc_luma_coefb     : 5; /* [12..8]  */
        unsigned int    reserved_1            : 3; /* [15..13]  */
        unsigned int    isp_cc_luma_coefr_up  : 5; /* [20..16]  */
        unsigned int    reserved_2            : 3; /* [23..21]  */
        unsigned int    isp_cc_luma_coefb_up  : 5; /* [28..24]  */
        unsigned int    reserved_3            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_lumafact;

/* Define the union u_isp_cc_colortone_rb_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_b_gain         : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_cc_r_gain         : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_colortone_rb_gain;

/* Define the union u_isp_cc_colortone_g_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_g_gain         : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_colortone_g_gain;

/* Define the union u_isp_cc_in_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_in_dc0         : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_in_dc0;

/* Define the union u_isp_cc_in_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_in_dc1         : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_in_dc1;

/* Define the union u_isp_cc_in_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_in_dc2         : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_in_dc2;

/* Define the union u_isp_cc_out_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_out_dc0        : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_out_dc0;

/* Define the union u_isp_cc_out_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_out_dc1        : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_out_dc1;

/* Define the union u_isp_cc_out_dc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_cc_out_dc2        : 13; /* [12..0]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_cc_out_dc2;

/* Define the union u_isp_csc_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_csc_version        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_version;
/* Define the union u_isp_csc_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_csc_coef00        : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_csc_coef01        : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_coef0;

/* Define the union u_isp_csc_coef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_csc_coef02        : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_csc_coef10        : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_coef1;

/* Define the union u_isp_csc_coef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_csc_coef11        : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_csc_coef12        : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_coef2;

/* Define the union u_isp_csc_coef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_csc_coef20        : 15; /* [15..1]  */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_csc_coef21        : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_coef3;

/* Define the union u_isp_csc_coef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_csc_coef22        : 15; /* [15..1]  */
        unsigned int    reserved_1            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_coef4;

/* Define the union u_isp_csc_in_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 5; /* [4..0]  */
        unsigned int    isp_csc_in_dc0        : 11; /* [15..5]  */
        unsigned int    reserved_1            : 5; /* [20..16]  */
        unsigned int    isp_csc_in_dc1        : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_in_dc0;

/* Define the union u_isp_csc_in_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 5; /* [4..0]  */
        unsigned int    isp_csc_in_dc2        : 11; /* [15..5]  */
        unsigned int    reserved_1            : 5; /* [20..16]  */
        unsigned int    reserved_2            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_in_dc1;

/* Define the union u_isp_csc_out_dc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 5; /* [4..0]  */
        unsigned int    isp_csc_out_dc0       : 11; /* [15..5]  */
        unsigned int    reserved_1            : 5; /* [20..16]  */
        unsigned int    isp_csc_out_dc1       : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_out_dc0;

/* Define the union u_isp_csc_out_dc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 5; /* [4..0]  */
        unsigned int    isp_csc_out_dc2       : 11; /* [15..5]  */
        unsigned int    reserved_1            : 5; /* [20..16]  */
        unsigned int    reserved_2            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_csc_out_dc1;

/* Define the union u_isp_hcds_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_hcds_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hcds_version;
/* Define the union u_isp_hcds_coefh0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hcds_coefh1       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_hcds_coefh0       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hcds_coefh0;

/* Define the union u_isp_hcds_coefh1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hcds_coefh3       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_hcds_coefh2       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hcds_coefh1;

/* Define the union u_isp_hcds_coefh2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hcds_coefh5       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_hcds_coefh4       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hcds_coefh2;

/* Define the union u_isp_hcds_coefh3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_hcds_coefh7       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_hcds_coefh6       : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_hcds_coefh3;

/* Define the union u_isp_awb_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awb_version        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_version;
/* Define the union u_isp_awb_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_hnum          : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_awb_vnum          : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_zone;

/* Define the union u_isp_awb_bitmove */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_bitmove       : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_bitmove;

/* Define the union u_isp_awb_thd_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_threshold_min : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_thd_min;

/* Define the union u_isp_awb_thd_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_threshold_max : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_thd_max;

/* Define the union u_isp_awb_cr_mm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_cr_ref_max    : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_awb_cr_ref_min    : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_cr_mm;

/* Define the union u_isp_awb_cb_mm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_cb_ref_max    : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_awb_cb_ref_min    : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_cb_mm;

/* Define the union u_isp_awb_offset_comp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_offset_comp   : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_offset_comp;

/* Define the union u_isp_awb_avg_r */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_avg_r         : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_avg_r;

/* Define the union u_isp_awb_avg_g */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_avg_g         : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_avg_g;

/* Define the union u_isp_awb_avg_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_avg_b         : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_avg_b;

/* Define the union u_isp_awb_cnt_all */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_count_all     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_cnt_all;

/* Define the union u_isp_awb_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_stt_size      : 16; /* [15..0]  */
        unsigned int    isp_awb_stt_bst       : 4; /* [19..16]  */
        unsigned int    isp_awb_stt_en        : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stt_bst;

/* Define the union u_isp_awb_stt_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_stt_info      : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_awb_stt_clr       : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stt_abn;

/* Define the union u_isp_awb_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_crop_pos_x    : 16; /* [15..0]  */
        unsigned int    isp_awb_crop_pos_y    : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_crop_pos;

/* Define the union u_isp_awb_crop_outsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_crop_out_width : 16; /* [15..0]  */
        unsigned int    isp_awb_crop_out_height : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_crop_outsize;

/* Define the union u_isp_awb_stat_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stat_waddr;
/* Define the union u_isp_awb_stat_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stat_wdata;
/* Define the union u_isp_awb_stat_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awb_stat_raddr     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stat_raddr;
/* Define the union u_isp_awb_stat_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awb_stat_rdata     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stat_rdata;

/* Define the union u_isp_la_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_la_version         : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_version;
/* Define the union u_isp_la_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_hnum           : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_la_vnum           : 5; /* [12..8]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_zone;

/* Define the union u_isp_la_bitmove */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_bitmove        : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_la_gamma_en       : 1; /* [8]  */
        unsigned int    isp_la_blc_en         : 1; /* [9]  */
        unsigned int    reserved_1            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_bitmove;

/* Define the union u_isp_la_offset_r */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_offset_r       : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_offset_r;

/* Define the union u_isp_la_offset_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_offset_gr      : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_offset_gr;

/* Define the union u_isp_la_offset_gb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_offset_gb      : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_offset_gb;

/* Define the union u_isp_la_offset_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_offset_b       : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_offset_b;

/* Define the union u_isp_la_gamma_limit */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_gamma_limit    : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_gamma_limit;

/* Define the union u_isp_la_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_crop_pos_x     : 16; /* [15..0]  */
        unsigned int    isp_la_crop_pos_y     : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_crop_pos;

/* Define the union u_isp_la_crop_outsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_crop_out_width : 16; /* [15..0]  */
        unsigned int    isp_la_crop_out_height : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_crop_outsize;

/* Define the union u_isp_la_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_stt_size       : 16; /* [15..0]  */
        unsigned int    isp_la_stt_bst        : 4; /* [19..16]  */
        unsigned int    isp_la_stt_en         : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_stt_bst;

/* Define the union u_isp_la_stt_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_stt_info       : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_la_stt_clr        : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_stt_abn;

/* Define the union u_isp_la_aver_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_aver_waddr;
/* Define the union u_isp_la_aver_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_aver_wdata;
/* Define the union u_isp_la_aver_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_la_aver_raddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_aver_raddr;
/* Define the union u_isp_la_aver_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_la_aver_rdata      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_aver_rdata;
/* Define the union u_isp_awblsc_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awblsc_version     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_version;
/* Define the union u_isp_awblsc_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_hnum       : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_awblsc_vnum       : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_zone;

/* Define the union u_isp_awblsc_bitmove */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_bitmove    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_bitmove;

/* Define the union u_isp_awblsc_thd_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_threshold_min : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_thd_min;

/* Define the union u_isp_awblsc_thd_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_threshold_max : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_thd_max;

/* Define the union u_isp_awblsc_cr_mm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_cr_ref_max : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_awblsc_cr_ref_min : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_cr_mm;

/* Define the union u_isp_awblsc_cb_mm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_cb_ref_max : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_awblsc_cb_ref_min : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_cb_mm;

/* Define the union u_isp_awblsc_offset_comp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_offset_comp : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_offset_comp;

/* Define the union u_isp_awblsc_avg_r */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_avg_r      : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_avg_r;

/* Define the union u_isp_awblsc_avg_g */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_avg_g      : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_avg_g;

/* Define the union u_isp_awblsc_avg_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_avg_b      : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_avg_b;

/* Define the union u_isp_awblsc_cnt_all */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_count_all  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_cnt_all;

/* Define the union u_isp_awblsc_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_stt_size   : 16; /* [15..0]  */
        unsigned int    isp_awblsc_stt_bst    : 4; /* [19..16]  */
        unsigned int    isp_awblsc_stt_en     : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stt_bst;

/* Define the union u_isp_awblsc_stt_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_stt_info   : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_awblsc_stt_clr    : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stt_abn;

/* Define the union u_isp_awblsc_crop_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_crop_pos_x : 16; /* [15..0]  */
        unsigned int    isp_awblsc_crop_pos_y : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_crop_pos;

/* Define the union u_isp_awblsc_crop_outsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_crop_out_width : 16; /* [15..0]  */
        unsigned int    isp_awblsc_crop_out_height : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_crop_outsize;

/* Define the union u_isp_awblsc_stat_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stat_waddr;
/* Define the union u_isp_awblsc_stat_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stat_wdata;
/* Define the union u_isp_awblsc_stat_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awblsc_stat_raddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stat_raddr;
/* Define the union u_isp_awblsc_stat_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awblsc_stat_rdata  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stat_rdata;

/* Define the union u_isp_lsc_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_stt2lut_en    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_stt2lut_cfg;

/* Define the union u_isp_lsc_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_stt2lut_regnew;

/* Define the union u_isp_lsc_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_stt2lut_info  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_lsc_stt2lut_clr   : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_stt2lut_abn;

/* Define the union u_isp_lsc_winnum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_numh          : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_lsc_numv          : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_winnum;

/* Define the union u_isp_lsc_winx_0_23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_deltax       : 13; /* [12..0]  */
        unsigned int    reserved_0           : 3; /* [15..13]  */
        unsigned int    isp_lsc_invx         : 12; /* [27..16]  */
        unsigned int    reserved_1           : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_winx0_23;

/* Define the union u_isp_lsc_width_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_width_offset  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_width_offset;

/* Define the union u_isp_lsc_mesh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_mesh_str      : 16; /* [15..0]  */
        unsigned int    isp_lsc_mesh_scale    : 3; /* [18..16]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_mesh;

/* Define the union u_isp_lsc_weight */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_mesh_weight   : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_weight;

/* Define the union u_isp_lsc_rgain_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_rgain_waddr    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_rgain_waddr;
/* Define the union u_isp_lsc_rgain_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_rgain0_wdata  : 10; /* [9..0]  */
        unsigned int    isp_lsc_rgain1_wdata  : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_rgain_wdata;

/* Define the union u_isp_lsc_rgain_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_rgain_raddr    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_rgain_raddr;
/* Define the union u_isp_lsc_rgain_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_rgain0_rdata  : 10; /* [9..0]  */
        unsigned int    isp_lsc_rgain1_rdata  : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_rgain_rdata;

/* Define the union u_isp_lsc_grgain_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_grgain_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_grgain_waddr;
/* Define the union u_isp_lsc_grgain_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_grgain0_wdata : 10; /* [9..0]  */
        unsigned int    isp_lsc_grgain1_wdata : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_grgain_wdata;

/* Define the union u_isp_lsc_grgain_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_grgain_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_grgain_raddr;
/* Define the union u_isp_lsc_grgain_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_grgain0_rdata : 10; /* [9..0]  */
        unsigned int    isp_lsc_grgain1_rdata : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_grgain_rdata;

/* Define the union u_isp_lsc_bgain_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_bgain_waddr    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_bgain_waddr;
/* Define the union u_isp_lsc_bgain_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_bgain0_wdata  : 10; /* [9..0]  */
        unsigned int    isp_lsc_bgain1_wdata  : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_bgain_wdata;

/* Define the union u_isp_lsc_bgain_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_bgain_raddr    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_bgain_raddr;
/* Define the union u_isp_lsc_bgain_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_bgain0_rdata  : 10; /* [9..0]  */
        unsigned int    isp_lsc_bgain1_rdata  : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_bgain_rdata;

/* Define the union u_isp_lsc_gbgain_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_gbgain_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_gbgain_waddr;
/* Define the union u_isp_lsc_gbgain_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_gbgain0_wdata : 10; /* [9..0]  */
        unsigned int    isp_lsc_gbgain1_wdata : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_gbgain_wdata;

/* Define the union u_isp_lsc_gbgain_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_gbgain_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_gbgain_raddr;
/* Define the union u_isp_lsc_gbgain_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_gbgain0_rdata : 10; /* [9..0]  */
        unsigned int    isp_lsc_gbgain1_rdata : 10; /* [19..10]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_gbgain_rdata;

/* Define the union u_isp_lsc_winx24_31 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_deltax      : 13; /* [12..0]  */
        unsigned int    reserved_0          : 3; /* [15..13]  */
        unsigned int    isp_lsc_invx        : 12; /* [27..16]  */
        unsigned int    reserved_1          : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_winx24_31;

/* Define the union u_isp_lsc_winy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_deltay       : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_lsc_invy         : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_winy;

/* Define the union u_isp_lsc_blcen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_blc_in_en     : 1; /* [0]  */
        unsigned int    reserved_0            : 7; /* [7..1]  */
        unsigned int    isp_lsc_blc_out_en    : 1; /* [8]  */
        unsigned int    reserved_1            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_blcen;

/* Define the union u_isp_lsc_blc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_blc_r         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_lsc_blc_gr        : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_blc0;

/* Define the union u_isp_lsc_blc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_blc_b         : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_lsc_blc_gb        : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_blc1;

/* Define the union u_isp_gcac_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_en           : 1; /* [0]  */
        unsigned int    isp_gcac_lcac_link_en : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_cfg;

/* Define the union u_isp_gcac_edge_fac */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_edgemode     : 2; /* [1..0]  */
        unsigned int    isp_gcac_cacnormfactor : 4; /* [5..2]  */
        unsigned int    isp_gcac_cacglobalstr : 11; /* [16..6]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_edge_fac;

/* Define the union u_isp_gcac_edge_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cacthrb1     : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_gcac_cacthrb2     : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_edge_thd;

/* Define the union u_isp_gcac_lamda_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_caclamdath1  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_gcac_caclamdath2  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_lamda_thd;

/* Define the union u_isp_gcac_lamda_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cacthrbmul   : 16; /* [15..0]  */
        unsigned int    isp_gcac_caclamdamul  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_lamda_mul;

/* Define the union u_isp_gcac_edge_str_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cacedgestrthd1 : 9; /* [8..0]  */
        unsigned int    isp_gcac_cacedgestrthd2 : 9; /* [17..9]  */
        unsigned int    isp_gcac_cacedgestrmul : 12; /* [29..18]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_edge_str_thd;

/* Define the union u_isp_gcac_crcb_adjust */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cac_alphar   : 10; /* [9..0]  */
        unsigned int    isp_gcac_cac_alphab   : 10; /* [19..10]  */
        unsigned int    isp_gcac_cac_tao      : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_crcb_adjust;

/* Define the union u_isp_gcac_calw_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cac_greythd  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_gcac_cac_colordifthd : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_calw_thd;

/* Define the union u_isp_gcac_var_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cac_varthd   : 12; /* [11..0]  */
        unsigned int    reserved_0            : 16; /* [27..12]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_var_thd;

/* Define the union u_isp_gcac_purple */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_det_satuthr  : 12; /* [11..0]  */
        unsigned int    isp_gcac_purplealpha  : 7; /* [18..12]  */
        unsigned int    isp_gcac_edgealpha    : 7; /* [25..19]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_purple;

/* Define the union u_isp_gcac_cbcr_ratio_limit */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cbcr_ratio_low_limit : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_gcac_cbcr_ratio_high_limit : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_cbcr_ratio_limit;

/* Define the union u_isp_gcac_cbcr_ratio_limit2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gcac_cbcr_ratio_low_limit2 : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_gcac_cbcr_ratio_high_limit2 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gcac_cbcr_ratio_limit2;

/* Define the union u_isp_demosaic_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_ahd_en   : 1; /* [0]  */
        unsigned int    isp_demosaic_bnrshp_en : 1; /* [1]  */
        unsigned int    reserved_0            : 14; /* [15..2]  */
        unsigned int    isp_demosaic_newlcacproc : 1; /* [16]  */
        unsigned int    isp_demosaic_local_cac_en : 1; /* [17]  */
        unsigned int    isp_demosaic_gcac_blend_select : 1; /* [18]  */
        unsigned int    isp_demosaic_fcr_en   : 1; /* [19]  */
        unsigned int    reserved_1            : 1; /* [20]  */
        unsigned int    isp_demosaic_de_fake_en : 1; /* [21]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_demosaic_desat_enable : 1; /* [24]  */
        unsigned int    reserved_3            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cfg1;

/* Define the union u_isp_demosaic_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_bld_limit2 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_demosaic_bld_limit1 : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_coef0;

/* Define the union u_isp_demosaic_coef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fcr_limit2 : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_demosaic_fcr_limit1 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_coef2;

/* Define the union u_isp_demosaic_coef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_ahd_par2 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_demosaic_ahd_par1 : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_coef3;

/* Define the union u_isp_demosaic_coef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fcr_gain : 5; /* [4..0]  */
        unsigned int    reserved_0            : 11; /* [15..5]  */
        unsigned int    isp_demosaic_fcr_thr  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_coef4;

/* Define the union u_isp_demosaic_coef5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_hv_ratio : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_coef5;

/* Define the union u_isp_demosaic_coef6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fcr_ratio : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_coef6;

/* Define the union u_isp_demosaic_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_hv_sel   : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_sel;

/* Define the union u_isp_demosaic_lcac_cnt_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_r_counter_thr : 5; /* [4..0]  */
        unsigned int    isp_demosaic_b_counter_thr : 5; /* [9..5]  */
        unsigned int    isp_demosaic_g_counter_thr : 6; /* [15..10]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_lcac_cnt_thr;

/* Define the union u_isp_demosaic_lcac_luma_rb_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_r_luma_thr : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_demosaic_b_luma_thr : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_lcac_luma_rb_thr;

/* Define the union u_isp_demosaic_lcac_luma_g_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_g_luma_thr : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_lcac_luma_g_thr;

/* Define the union u_isp_demosaic_purple_var_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_purple_var_thr : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_purple_var_thr;

/* Define the union u_isp_demosaic_fake_cr_var_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fake_cr_var_thr_low : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_demosaic_fake_cr_var_thr_high : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_fake_cr_var_thr;

/* Define the union u_isp_demosaic_depurplectr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurplectrcb : 4; /* [3..0]  */
        unsigned int    reserved_0            : 12; /* [15..4]  */
        unsigned int    isp_demosaic_depurplectrcr : 4; /* [19..16]  */
        unsigned int    reserved_1            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_depurplectr;

/* Define the union u_isp_demosaic_lpf_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_lpf_f0   : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_demosaic_lpf_f1   : 5; /* [12..8]  */
        unsigned int    reserved_1            : 3; /* [15..13]  */
        unsigned int    isp_demosaic_lpf_f2   : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_demosaic_lpf_f3   : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_lpf_coef;

/* Define the union u_isp_demosaic_g_intp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_g_clip_sft_bit : 4; /* [3..0]  */
        unsigned int    reserved_0            : 12; /* [15..4]  */
        unsigned int    isp_demosaic_g_intp_control : 1; /* [16]  */
        unsigned int    reserved_1            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_g_intp_ctrl;

/* Define the union u_isp_demosaic_cbcravgthld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cbcr_avg_thld : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cbcravgthld;

/* Define the union u_isp_demosaic_cc_hf_ratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cc_hf_min_ratio : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_demosaic_cc_hf_max_ratio : 5; /* [12..8]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cc_hf_ratio;

/* Define the union u_isp_demosaic_gvar_blend_thld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_varthrforblend : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_gvar_blend_thld;

/* Define the union u_isp_demosaic_satu_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_satu_thr : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_satu_thr;

/* Define the union u_isp_demosaic_cbcr_ratio_limit */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cbcr_ratio_low_limit : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_demosaic_cbcr_ratio_high_limit : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cbcr_ratio_limit;

/* Define the union u_isp_demosaic_fcr_gray_ratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fcr_gray_ratio : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_fcr_gray_ratio;

/* Define the union u_isp_demosaic_fcr_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fcr_detg_sel : 4; /* [3..0]  */
        unsigned int    isp_demosaic_fcr_cmax_sel : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_fcr_sel;

/* Define the union u_isp_demosaic_cx_var_rate */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cx_var_min_rate : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_demosaic_cx_var_max_rate : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cx_var_rate;

/* Define the union u_isp_demosaic_depurplut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurplut_waddr : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_depurplut_waddr;

/* Define the union u_isp_demosaic_depurplut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurplut_wdata : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_depurplut_wdata;

/* Define the union u_isp_demosaic_depurplut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurplut_raddr : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_depurplut_raddr;

/* Define the union u_isp_demosaic_depurplut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurplut_rdata : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_depurplut_rdata;

/* Define the union u_isp_demosaic_hf_intp_blur_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_hf_intp_blur_th1 : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_demosaic_hf_intp_blur_th2 : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_hf_intp_blur_th;

/* Define the union u_isp_demosaic_cac_cbcr_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cb_thr   : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_demosaic_luma_thr : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cac_cbcr_thr;

/* Define the union u_isp_demosaic_cac_luma_high_cnt_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cac_luma_high_cnt_thr : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cac_luma_high_cnt_thr;

/* Define the union u_isp_demosaic_cac_cnt_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cac_cb_cnt_low_thr : 7; /* [6..0]  */
        unsigned int    reserved_0            : 9; /* [15..7]  */
        unsigned int    isp_demosaic_cac_cb_cnt_high_thr : 7; /* [22..16]  */
        unsigned int    reserved_1            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cac_cnt_cfg;

/* Define the union u_isp_demosaic_defcolor_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_defcolor_cr : 4; /* [3..0]  */
        unsigned int    reserved_0            : 12; /* [15..4]  */
        unsigned int    isp_demosaic_defcolor_cb : 4; /* [19..16]  */
        unsigned int    reserved_1            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_defcolor_coef;

/* Define the union u_isp_demosaic_cac_bld_avg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaci_cac_bld_avg_cur : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cac_bld_avg;

/* Define the union u_isp_demosaic_fcr_hf_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_fcr_thresh1 : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_demosaic_fcr_thresh2 : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_fcr_hf_thr;

/* Define the union u_isp_demosaic_desat_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_desat_thresh1 : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_demosaic_desat_thresh2 : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_desat_thr;

/* Define the union u_isp_demosaic_desat_bldr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_desat_hig : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_demosaic_desat_low : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_desat_bldr;

/* Define the union u_isp_demosaic_desat_protect */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_desat_protect_sl : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_demosaic_desat_protect_th : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_desat_protect;

/* Define the union u_isp_demosaic_hlc_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_hlc_thresh : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_hlc_th;

/* Define the union u_isp_demosaic_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurplut_update : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_lut_update;

/* Define the union u_isp_demosaic_cbcr_ratio_limit2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cbcr_ratio_low_limit2 : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_demosaic_cbcr_ratio_high_limit2 : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cbcr_ratio_limit2;

/* Define the union u_isp_demosaic_cbcr_ratio_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_cbcr_ratio_low_mul : 15; /* [14..0]  */
        unsigned int    reserved_0            : 2; /* [16..15]  */
        unsigned int    isp_demosaic_cbcr_ratio_high_mul : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_cbcr_ratio_mul;

/* Define the union u_isp_bnrshp_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_stt2lut_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_stt2lut_cfg;

/* Define the union u_isp_bnrshp_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_stt2lut_regnew;

/* Define the union u_isp_bnrshp_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_bnrshp_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_stt2lut_abn;

/* Define the union u_isp_bnrshp_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_dirdiffsft : 4; /* [3..0]  */
        unsigned int    isp_bnrshp_varbitdepth : 4; /* [7..4]  */
        unsigned int    isp_bnrshp_bitdepth   : 5; /* [12..8]  */
        unsigned int    isp_bnrshp_bfilecoefmode : 1; /* [13]  */
        unsigned int    isp_bnrshp_shtbyvar_en : 1; /* [14]  */
        unsigned int    isp_bnrshp_shtvar5x5sft : 3; /* [17..15]  */
        unsigned int    isp_bnrshp_bblkctrlmode : 1; /* [18]  */
        unsigned int    isp_bnrshp_recnegctrlsft : 3; /* [21..19]  */
        unsigned int    isp_bnrshp_luma_ctrl_en : 1; /* [22]  */
        unsigned int    isp_bnrshp_chrctrl_en : 1; /* [23]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_cfg;

/* Define the union u_isp_bnrshp_dir_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_dirrt0     : 8; /* [7..0]  */
        unsigned int    isp_bnrshp_dirrt1     : 8; /* [15..8]  */
        unsigned int    isp_bnrshp_dirrly0    : 8; /* [23..16]  */
        unsigned int    isp_bnrshp_dirrly1    : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_dir_mul;

/* Define the union u_isp_bnrshp_hsfcoef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef0d0 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef0d1 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef0d2 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef0;

/* Define the union u_isp_bnrshp_hsfcoef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef0d3 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef0d4 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef0d5 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef1;

/* Define the union u_isp_bnrshp_hsfcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef0d6 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef0d7 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef0d8 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef2;

/* Define the union u_isp_bnrshp_hsfcoef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef0d9 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef0d10 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef0d11 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef3;

/* Define the union u_isp_bnrshp_hsfcoef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef0d12 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef1d0 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef1d1 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef4;

/* Define the union u_isp_bnrshp_hsfcoef5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef1d2 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef1d3 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef1d4 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef5;

/* Define the union u_isp_bnrshp_hsfcoef6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef1d5 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef1d6 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef1d7 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef6;

/* Define the union u_isp_bnrshp_hsfcoef7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef1d8 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef1d9 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef1d10 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef7;

/* Define the union u_isp_bnrshp_hsfcoef8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef1d11 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef1d12 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef2d0 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef8;

/* Define the union u_isp_bnrshp_hsfcoef9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef2d1 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef2d2 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef2d3 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef9;

/* Define the union u_isp_bnrshp_hsfcoef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef2d4 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef2d5 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef2d6 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef10;

/* Define the union u_isp_bnrshp_hsfcoef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef2d7 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef2d8 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef2d9 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef11;

/* Define the union u_isp_bnrshp_hsfcoef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef2d10 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef2d11 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef2d12 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef12;

/* Define the union u_isp_bnrshp_hsfcoef13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef3d0 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef3d1 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef3d2 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef13;

/* Define the union u_isp_bnrshp_hsfcoef14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef3d3 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef3d4 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef3d5 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef14;

/* Define the union u_isp_bnrshp_hsfcoef15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef3d6 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef3d7 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef3d8 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef15;

/* Define the union u_isp_bnrshp_hsfcoef16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef3d9 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_hsfcoef3d10 : 10; /* [19..10]  */
        unsigned int    isp_bnrshp_hsfcoef3d11 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef16;

/* Define the union u_isp_bnrshp_lpfcoef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoef3d12 : 10; /* [9..0]  */
        unsigned int    isp_bnrshp_lpfsftud   : 3; /* [12..10]  */
        unsigned int    isp_bnrshp_lpfsftd    : 3; /* [15..13]  */
        unsigned int    isp_bnrshp_hsfsftud   : 3; /* [18..16]  */
        unsigned int    isp_bnrshp_hsfsftd    : 4; /* [22..19]  */
        unsigned int    isp_bnrshp_lpfcoefd2  : 7; /* [29..23]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lpfcoef0;

/* Define the union u_isp_bnrshp_lpfcoef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_lpfcoefud0 : 6; /* [5..0]  */
        unsigned int    isp_bnrshp_lpfcoefud1 : 6; /* [11..6]  */
        unsigned int    isp_bnrshp_lpfcoefud2 : 6; /* [17..12]  */
        unsigned int    isp_bnrshp_lpfcoefd0  : 7; /* [24..18]  */
        unsigned int    isp_bnrshp_lpfcoefd1  : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lpfcoef1;

/* Define the union u_isp_bnrshp_hsfcoef17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_hsfcoefud0 : 7; /* [6..0]  */
        unsigned int    isp_bnrshp_hsfcoefud1 : 7; /* [13..7]  */
        unsigned int    isp_bnrshp_hsfcoefud2 : 7; /* [20..14]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_hsfcoef17;

/* Define the union u_isp_bnrshp_luma_thr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neglumathd00 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_neglumathd01 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_thr0;

/* Define the union u_isp_bnrshp_luma_wgt0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neglumawgt00 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_neglumawgt01 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_wgt0;

/* Define the union u_isp_bnrshp_luma_thr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neglumathd10 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_neglumathd11 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_thr1;

/* Define the union u_isp_bnrshp_luma_wgt1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neglumawgt10 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_neglumawgt11 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_wgt1;

/* Define the union u_isp_bnrshp_luma_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neglumathd0mul : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_neglumathd1mul : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_mul;

/* Define the union u_isp_bnrshp_gain_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainthdsftd : 2; /* [1..0]  */
        unsigned int    isp_bnrshp_gainthdseld : 2; /* [3..2]  */
        unsigned int    isp_bnrshp_gainthdsftud : 2; /* [5..4]  */
        unsigned int    isp_bnrshp_gainthdselud : 2; /* [7..6]  */
        unsigned int    isp_bnrshp_mfgainsft  : 3; /* [10..8]  */
        unsigned int    isp_bnrshp_hfgainsft  : 3; /* [13..11]  */
        unsigned int    isp_bnrshp_negmfshft  : 3; /* [16..14]  */
        unsigned int    isp_bnrshp_negmfwgt0  : 12; /* [28..17]  */
        unsigned int    reserved_0            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gain_ctrl;

/* Define the union u_isp_bnrshp_mf_cac */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_negmfwgt1  : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_negmfmul   : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_mf_cac;

/* Define the union u_isp_bnrshp_gaind0_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind0_waddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind0_waddr;

/* Define the union u_isp_bnrshp_gaind0_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind0_wdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind0_wdata;

/* Define the union u_isp_bnrshp_gaind0_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind0_raddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind0_raddr;

/* Define the union u_isp_bnrshp_gaind0_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind0_rdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind0_rdata;

/* Define the union u_isp_bnrshp_gaind1_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind1_waddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind1_waddr;

/* Define the union u_isp_bnrshp_gaind1_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind1_wdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind1_wdata;

/* Define the union u_isp_bnrshp_gaind1_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind1_raddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind1_raddr;

/* Define the union u_isp_bnrshp_gaind1_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gaind1_rdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gaind1_rdata;

/* Define the union u_isp_bnrshp_gainud0_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud0_waddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud0_waddr;

/* Define the union u_isp_bnrshp_gainud0_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud0_wdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud0_wdata;

/* Define the union u_isp_bnrshp_gainud0_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud0_raddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud0_raddr;

/* Define the union u_isp_bnrshp_gainud0_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud0_rdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud0_rdata;

/* Define the union u_isp_bnrshp_gainud1_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud1_waddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud1_waddr;

/* Define the union u_isp_bnrshp_gainud1_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud1_wdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud1_wdata;

/* Define the union u_isp_bnrshp_gainud1_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud1_raddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud1_raddr;

/* Define the union u_isp_bnrshp_gainud1_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_gainud1_rdata : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_gainud1_rdata;

/* Define the union u_isp_bnrshp_luma_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_luma_waddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_waddr;

/* Define the union u_isp_bnrshp_luma_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_luma_wdata : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_wdata;

/* Define the union u_isp_bnrshp_luma_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_luma_raddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_raddr;

/* Define the union u_isp_bnrshp_luma_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_luma_rdata : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_luma_rdata;

/* Define the union u_isp_bnrshp_neghf_wgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neghfshft  : 3; /* [2..0]  */
        unsigned int    isp_bnrshp_neghfwgt0  : 12; /* [14..3]  */
        unsigned int    isp_bnrshp_neghfwgt1  : 12; /* [26..15]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_neghf_wgt;

/* Define the union u_isp_bnrshp_neghf_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neghfmul   : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_shtvardiffmul : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_neghf_mul;

/* Define the union u_isp_bnrshp_lmtmf_lut0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_lmtmf0     : 7; /* [6..0]  */
        unsigned int    isp_bnrshp_lmtmf1     : 7; /* [13..7]  */
        unsigned int    isp_bnrshp_lmtmf2     : 7; /* [20..14]  */
        unsigned int    isp_bnrshp_lmtmf3     : 7; /* [27..21]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lmtmf_lut0;

/* Define the union u_isp_bnrshp_lmtmf_lut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_lmtmf4     : 7; /* [6..0]  */
        unsigned int    isp_bnrshp_lmtmf5     : 7; /* [13..7]  */
        unsigned int    isp_bnrshp_lmtmf6     : 7; /* [20..14]  */
        unsigned int    isp_bnrshp_lmtmf7     : 7; /* [27..21]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lmtmf_lut1;

/* Define the union u_isp_bnrshp_lmthf_lut0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_lmthf0     : 7; /* [6..0]  */
        unsigned int    isp_bnrshp_lmthf1     : 7; /* [13..7]  */
        unsigned int    isp_bnrshp_lmthf2     : 7; /* [20..14]  */
        unsigned int    isp_bnrshp_lmthf3     : 7; /* [27..21]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lmthf_lut0;

/* Define the union u_isp_bnrshp_lmthf_lut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_lmthf4     : 7; /* [6..0]  */
        unsigned int    isp_bnrshp_lmthf5     : 7; /* [13..7]  */
        unsigned int    isp_bnrshp_lmthf6     : 7; /* [20..14]  */
        unsigned int    isp_bnrshp_lmthf7     : 7; /* [27..21]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lmthf_lut1;

/* Define the union u_isp_bnrshp_var_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_shtvardiffthd0 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_shtvardiffthd1 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_var_thd;

/* Define the union u_isp_bnrshp_var_wgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_shtvardiffwgt0 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_shtvardiffwgt1 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_var_wgt;

/* Define the union u_isp_bnrshp_sht_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_selpixwgt  : 2; /* [1..0]  */
        unsigned int    isp_bnrshp_oshtamt    : 8; /* [9..2]  */
        unsigned int    isp_bnrshp_ushtamt    : 8; /* [17..10]  */
        unsigned int    isp_bnrshp_oshtmaxgain : 7; /* [24..18]  */
        unsigned int    isp_bnrshp_ushtmaxgain : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_sht_ctrl;

/* Define the union u_isp_bnrshp_sht_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_omaxchg    : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_umaxchg    : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_sht_ctrl1;

/* Define the union u_isp_bnrshp_losef_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_recnegctrlthr0 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_recnegctrlthr1 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_losef_thr;

/* Define the union u_isp_bnrshp_losef_wgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_recnegctrlwgt0 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_recnegctrlwgt1 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_losef_wgt;

/* Define the union u_isp_bnrshp_losef_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_recnegctrlmul : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_losef_mul;

/* Define the union u_isp_bnrshp_neghf_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_neghfthd0  : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_neghfthd1  : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_neghf_thr;

/* Define the union u_isp_bnrshp_detail_sht */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_detailoshtamt : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_detailushtamt : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_detail_sht;

/* Define the union u_isp_bnrshp_rly_g_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_shpongthd0 : 12; /* [11..0]  */
        unsigned int    isp_bnrshp_shpongthd1 : 12; /* [23..12]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_rly_g_thr;

/* Define the union u_isp_bnrshp_rly_g_wgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnrshp_shpongwgt0 : 8; /* [7..0]  */
        unsigned int    isp_bnrshp_shpongwgt1 : 8; /* [15..8]  */
        unsigned int    isp_bnrshp_shpongmul  : 8; /* [23..16]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_rly_g_wgt;

/* Define the union u_isp_sharpen_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_version    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_version;
/* Define the union u_isp_sharpen_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_benlumactrl : 1; /* [0]  */
        unsigned int    isp_sharpen_print_sel : 1; /* [1]  */
        unsigned int    isp_sharpen_benshtvar_sel : 1; /* [2]  */
        unsigned int    isp_sharpen_benshtctrlbyvar : 1; /* [3]  */
        unsigned int    isp_sharpen_benskinctrl : 1; /* [4]  */
        unsigned int    isp_sharpen_weakdetailadj : 1; /* [5]  */
        unsigned int    isp_sharpen_benchrctrl : 1; /* [6]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    reserved_1            : 1; /* [8]  */
        unsigned int    reserved_2            : 1; /* [9]  */
        unsigned int    isp_sharpen_detailthd_sel : 3; /* [12..10]  */
        unsigned int    isp_sharpen_bendetailctrl : 1; /* [13]  */
        unsigned int    reserved_3            : 1; /* [14]  */
        unsigned int    reserved_4            : 1; /* [15]  */
        unsigned int    isp_sharpen_ben8dir_sel : 1; /* [16]  */
        unsigned int    reserved_5            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_ctrl;

/* Define the union u_isp_sharpen_mhfthd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_mhfthdsftd : 4; /* [3..0]  */
        unsigned int    isp_sharpen_mhfthdseld : 2; /* [5..4]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_sharpen_mhfthdsftud : 4; /* [19..16]  */
        unsigned int    isp_sharpen_mhfthdselud : 2; /* [21..20]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfthd;

/* Define the union u_isp_sharpen_weakdetail */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dirrlythrlow : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    isp_sharpen_dirrlythrhih : 7; /* [14..8]  */
        unsigned int    reserved_1            : 1; /* [15]  */
        unsigned int    isp_sharpen_weakdetailgain : 7; /* [22..16]  */
        unsigned int    reserved_2            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_weakdetail;

/* Define the union u_isp_sharpen_dirvar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dirvarsft : 4; /* [3..0]  */
        unsigned int    isp_sharpen_dirvarscale : 4; /* [7..4]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dirrly0   : 7; /* [22..16]  */
        unsigned int    reserved_1            : 1; /* [23]  */
        unsigned int    isp_sharpen_dirrly1   : 7; /* [30..24]  */
        unsigned int    reserved_2            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dirvar;

/* Define the union u_isp_sharpen_dirdiff */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dirdiffsft : 6; /* [5..0]  */
        unsigned int    reserved_0            : 10; /* [15..6]  */
        unsigned int    isp_sharpen_dirrt0    : 5; /* [20..16]  */
        unsigned int    reserved_1            : 3; /* [23..21]  */
        unsigned int    isp_sharpen_dirrt1    : 5; /* [28..24]  */
        unsigned int    reserved_2            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dirdiff;

/* Define the union u_isp_sharpen_lumawgt0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt0  : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt1  : 6; /* [11..6]  */
        unsigned int    isp_sharpen_lumawgt2  : 6; /* [17..12]  */
        unsigned int    isp_sharpen_lumawgt3  : 6; /* [23..18]  */
        unsigned int    isp_sharpen_lumawgt4  : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt0;

/* Define the union u_isp_sharpen_lumawgt1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt5  : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt6  : 6; /* [11..6]  */
        unsigned int    isp_sharpen_lumawgt7  : 6; /* [17..12]  */
        unsigned int    isp_sharpen_lumawgt8  : 6; /* [23..18]  */
        unsigned int    isp_sharpen_lumawgt9  : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt1;

/* Define the union u_isp_sharpen_lumawgt2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt10 : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt11 : 6; /* [11..6]  */
        unsigned int    isp_sharpen_lumawgt12 : 6; /* [17..12]  */
        unsigned int    isp_sharpen_lumawgt13 : 6; /* [23..18]  */
        unsigned int    isp_sharpen_lumawgt14 : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt2;

/* Define the union u_isp_sharpen_lumawgt3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt15 : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt16 : 6; /* [11..6]  */
        unsigned int    isp_sharpen_lumawgt17 : 6; /* [17..12]  */
        unsigned int    isp_sharpen_lumawgt18 : 6; /* [23..18]  */
        unsigned int    isp_sharpen_lumawgt19 : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt3;

/* Define the union u_isp_sharpen_lumawgt4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt20 : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt21 : 6; /* [11..6]  */
        unsigned int    isp_sharpen_lumawgt22 : 6; /* [17..12]  */
        unsigned int    isp_sharpen_lumawgt23 : 6; /* [23..18]  */
        unsigned int    isp_sharpen_lumawgt24 : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt4;

/* Define the union u_isp_sharpen_lumawgt5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt25 : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt26 : 6; /* [11..6]  */
        unsigned int    isp_sharpen_lumawgt27 : 6; /* [17..12]  */
        unsigned int    isp_sharpen_lumawgt28 : 6; /* [23..18]  */
        unsigned int    isp_sharpen_lumawgt29 : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt5;

/* Define the union u_isp_sharpen_lumawgt6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lumawgt30 : 6; /* [5..0]  */
        unsigned int    isp_sharpen_lumawgt31 : 6; /* [11..6]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lumawgt6;

/* Define the union u_isp_vcds_coefv */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_vcds_coefv0       : 3; /* [2..0]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    isp_vcds_coefv1       : 3; /* [6..4]  */
        unsigned int    reserved_1            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_vcds_coefv;

/* Define the union u_isp_sharpen_shoot_amt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_selpixwgt : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_sharpen_oshtamt   : 7; /* [14..8]  */
        unsigned int    reserved_1            : 1; /* [15]  */
        unsigned int    isp_sharpen_ushtamt   : 7; /* [22..16]  */
        unsigned int    reserved_2            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_shoot_amt;

/* Define the union u_isp_sharpen_shoot_maxchg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_omaxchg   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_sharpen_umaxchg   : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_shoot_maxchg;

/* Define the union u_isp_sharpen_shtvar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_shtvarsft : 3; /* [2..0]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    isp_sharpen_shtvar5x5_sft : 4; /* [7..4]  */
        unsigned int    reserved_1            : 8; /* [15..8]  */
        unsigned int    isp_sharpen_shtbldrt  : 4; /* [19..16]  */
        unsigned int    reserved_2            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_shtvar;

/* Define the union u_isp_sharpen_mot0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_mot_en    : 1; /* [0]  */
        unsigned int    isp_sharpen_mot_thd0  : 4; /* [4..1]  */
        unsigned int    isp_sharpen_mot_thd1  : 4; /* [8..5]  */
        unsigned int    isp_sharpen_mot_mul   : 10; /* [18..9]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mot0;

/* Define the union u_isp_sharpen_oshtvar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_shtvarthd0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_shtvarthd1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_shtvarwgt0 : 7; /* [22..16]  */
        unsigned int    reserved_0            : 1; /* [23]  */
        unsigned int    isp_sharpen_shtvarwgt1 : 7; /* [30..24]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_oshtvar;

/* Define the union u_isp_sharpen_mot1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_mot_gain0 : 9; /* [8..0]  */
        unsigned int    isp_sharpen_mot_gain1 : 9; /* [17..9]  */
        unsigned int    isp_sharpen_osht_mot_amt : 7; /* [24..18]  */
        unsigned int    isp_sharpen_usht_mot_amt : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mot1;

/* Define the union u_isp_sharpen_shtvar_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_shtvarmul : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_sharpen_shtvardiffmul : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_shtvar_mul;

/* Define the union u_isp_sharpen_oshtvardiff */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_shtvardiffthd0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_shtvardiffthd1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_shtvardiffwgt0 : 7; /* [22..16]  */
        unsigned int    reserved_0            : 1; /* [23]  */
        unsigned int    isp_sharpen_shtvardiffwgt1 : 7; /* [30..24]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_oshtvardiff;

/* Define the union u_isp_sharpen_lmtmf0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lmtmf0    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_sharpen_lmtmf1    : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_lmtmf2    : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_sharpen_lmtmf3    : 6; /* [29..24]  */
        unsigned int    reserved_3            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lmtmf0;

/* Define the union u_isp_sharpen_lmtmf1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lmtmf4    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_sharpen_lmtmf5    : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_lmtmf6    : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_sharpen_lmtmf7    : 6; /* [29..24]  */
        unsigned int    reserved_3            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lmtmf1;

/* Define the union u_isp_sharpen_lmthf0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lmthf0    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_sharpen_lmthf1    : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_lmthf2    : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_sharpen_lmthf3    : 6; /* [29..24]  */
        unsigned int    reserved_3            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lmthf0;

/* Define the union u_isp_sharpen_lmthf1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lmthf4    : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_sharpen_lmthf5    : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_lmthf6    : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_sharpen_lmthf7    : 6; /* [29..24]  */
        unsigned int    reserved_3            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lmthf1;

/* Define the union u_isp_sharpen_mhfgaind_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgaind_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgaind_waddr;
/* Define the union u_isp_sharpen_mhfgaind_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgaind_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgaind_wdata;
/* Define the union u_isp_sharpen_mhfgaind_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgaind_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgaind_raddr;
/* Define the union u_isp_sharpen_mhfgaind_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgaind_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgaind_rdata;
/* Define the union u_isp_sharpen_mhfgainud_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgainud_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgainud_waddr;
/* Define the union u_isp_sharpen_mhfgainud_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgainud_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgainud_wdata;
/* Define the union u_isp_sharpen_mhfgainud_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgainud_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgainud_raddr;
/* Define the union u_isp_sharpen_mhfgainud_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfgainud_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfgainud_rdata;
/* Define the union u_isp_sharpen_mhfmotgaind_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgaind_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgaind_waddr;
/* Define the union u_isp_sharpen_mhfmotgaind_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgaind_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgaind_wdata;
/* Define the union u_isp_sharpen_mhfmotgaind_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgaind_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgaind_raddr;
/* Define the union u_isp_sharpen_mhfmotgaind_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgaind_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgaind_rdata;
/* Define the union u_isp_sharpen_mhfmotgainud_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgainud_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgainud_waddr;
/* Define the union u_isp_sharpen_mhfmotgainud_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgainud_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgainud_wdata;
/* Define the union u_isp_sharpen_mhfmotgainud_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgainud_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgainud_raddr;
/* Define the union u_isp_sharpen_mhfmotgainud_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_mhfmotgainud_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_mhfmotgainud_rdata;
/* Define the union u_isp_sharpen_skin_u */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_skinminu  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_sharpen_skinmaxu  : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_skin_u;

/* Define the union u_isp_sharpen_skin_v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_skinminv  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 8; /* [15..8]  */
        unsigned int    isp_sharpen_skinmaxv  : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_skin_v;

/* Define the union u_isp_sharpen_skin_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_skincntthd0 : 4; /* [3..0]  */
        unsigned int    isp_sharpen_skincntthd1 : 4; /* [7..4]  */
        unsigned int    isp_sharpen_skincntmul : 5; /* [12..8]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_skin_cnt;

/* Define the union u_isp_sharpen_skin_edge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_skinedgesft : 4; /* [3..0]  */
        unsigned int    reserved_0            : 12; /* [15..4]  */
        unsigned int    isp_sharpen_skinedgemul : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_skin_edge;

/* Define the union u_isp_sharpen_skin_edgethd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_skinedgethd0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_skinedgethd1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_skinedgewgt0 : 5; /* [20..16]  */
        unsigned int    reserved_0            : 3; /* [23..21]  */
        unsigned int    isp_sharpen_skinedgewgt1 : 5; /* [28..24]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_skin_edgethd;

/* Define the union u_isp_sharpen_chrr_var */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrrsft0  : 3; /* [2..0]  */
        unsigned int    isp_sharpen_chrrsft1  : 3; /* [5..3]  */
        unsigned int    isp_sharpen_chrrsft2  : 3; /* [8..6]  */
        unsigned int    isp_sharpen_chrrsft3  : 3; /* [11..9]  */
        unsigned int    isp_sharpen_chrrvarshift : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrr_var;

/* Define the union u_isp_sharpen_chrr_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrrori0  : 8; /* [7..0]  */
        unsigned int    isp_sharpen_chrrori1  : 8; /* [15..8]  */
        unsigned int    isp_sharpen_chrrthd0  : 8; /* [23..16]  */
        unsigned int    isp_sharpen_chrrthd1  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrr_thd;

/* Define the union u_isp_sharpen_chrr_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrrgain  : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_sharpen_rmfscale  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_chrrmul   : 10; /* [25..16]  */
        unsigned int    reserved_2            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrr_gain;

/* Define the union u_isp_sharpen_chrg_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrgmul   : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_chrgmfmul : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrg_mul;

/* Define the union u_isp_sharpen_chrg_sft */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrgsft0  : 3; /* [2..0]  */
        unsigned int    isp_sharpen_chrgsft1  : 3; /* [5..3]  */
        unsigned int    isp_sharpen_chrgsft2  : 3; /* [8..6]  */
        unsigned int    isp_sharpen_chrgsft3  : 3; /* [11..9]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrg_sft;

/* Define the union u_isp_sharpen_chrg_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrgori0  : 8; /* [7..0]  */
        unsigned int    isp_sharpen_chrgori1  : 8; /* [15..8]  */
        unsigned int    isp_sharpen_chrgthd0  : 8; /* [23..16]  */
        unsigned int    isp_sharpen_chrgthd1  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrg_thd;

/* Define the union u_isp_sharpen_chrg_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrggain  : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_sharpen_chrgmfgain : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrg_gain;

/* Define the union u_isp_sharpen_chrb_var */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrbsft0  : 3; /* [2..0]  */
        unsigned int    isp_sharpen_chrbsft1  : 3; /* [5..3]  */
        unsigned int    isp_sharpen_chrbsft2  : 3; /* [8..6]  */
        unsigned int    isp_sharpen_chrbsft3  : 3; /* [11..9]  */
        unsigned int    isp_sharpen_chrbvarshift : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrb_var;

/* Define the union u_isp_sharpen_chrb_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrbori0  : 8; /* [7..0]  */
        unsigned int    isp_sharpen_chrbori1  : 8; /* [15..8]  */
        unsigned int    isp_sharpen_chrbthd0  : 8; /* [23..16]  */
        unsigned int    isp_sharpen_chrbthd1  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrb_thd;

/* Define the union u_isp_sharpen_chrb_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_chrbgain  : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_sharpen_bmfscale  : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_sharpen_chrbmul   : 10; /* [25..16]  */
        unsigned int    reserved_2            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_chrb_gain;

/* Define the union u_isp_sharpen_gain_sft */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_mfgain_sft : 3; /* [2..0]  */
        unsigned int    reserved_0            : 5; /* [7..3]  */
        unsigned int    isp_sharpen_lfgainwgt : 5; /* [12..8]  */
        unsigned int    reserved_1            : 3; /* [15..13]  */
        unsigned int    isp_sharpen_hfgain_sft : 3; /* [18..16]  */
        unsigned int    reserved_2            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_gain_sft;

/* Define the union u_isp_sharpen_shoot_maxgain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_omaxgain  : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_sharpen_umaxgain  : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_shoot_maxgain;

/* Define the union u_isp_sharpen_detail_mul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_detl_oshtmul : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_sharpen_detl_ushtmul : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_detail_mul;

/* Define the union u_isp_sharpen_osht_detail */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_osht_dtl_thd0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_osht_dtl_thd1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_osht_dtl_wgt : 7; /* [22..16]  */
        unsigned int    reserved_0            : 1; /* [23]  */
        unsigned int    isp_sharpen_max_var_clip : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_osht_detail;

/* Define the union u_isp_sharpen_usht_detail */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_usht_dtl_thd0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_usht_dtl_thd1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_usht_dtl_wgt : 7; /* [22..16]  */
        unsigned int    reserved_0            : 1; /* [23]  */
        unsigned int    isp_sharpen_dtl_thdsft : 4; /* [27..24]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_usht_detail;

/* Define the union u_isp_sharpen_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_stt2lut_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_stt2lut_cfg;

/* Define the union u_isp_sharpen_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_stt2lut_regnew;

/* Define the union u_isp_sharpen_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_sharpen_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_stt2lut_abn;

/* Define the union u_isp_sharpen_dlpf_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dlpfcoef0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dlpfcoef1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dlpfcoef2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dlpfsft   : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dlpf_coef;

/* Define the union u_isp_sharpen_udlpf_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_udlpfcoef0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_udlpfcoef1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_udlpfcoef2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_udlpfsft  : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_udlpf_coef;

/* Define the union u_isp_sharpen_udhsf_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_udhsfcoef0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_udhsfcoef1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_udhsfcoef2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_udhsfsft  : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_udhsf_coef;

/* Define the union u_isp_sharpen_dhsf_2dshift */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 8; /* [7..0]  */
        unsigned int    reserved_1            : 8; /* [15..8]  */
        unsigned int    reserved_2            : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfsft   : 4; /* [27..24]  */
        unsigned int    reserved_3            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dshift;

/* Define the union u_isp_sharpen_dhsf_2dcoef0_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef0_0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef0_1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef0_2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef0_3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef0_03;

/* Define the union u_isp_sharpen_dhsf_2dcoef0_47 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef0_4 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef0_5 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef0_6 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef0_7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef0_47;

/* Define the union u_isp_sharpen_dhsf_2dcoef0_811 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef0_8 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef0_9 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef0_10 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef0_11 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef0_811;

/* Define the union u_isp_sharpen_dhsf_2dcoef0_1215 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef0_12 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef0_1215;

/* Define the union u_isp_sharpen_dhsf_2dcoef1_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef1_0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef1_1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef1_2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef1_3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef1_03;

/* Define the union u_isp_sharpen_dhsf_2dcoef1_47 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef1_4 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef1_5 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef1_6 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef1_7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef1_47;

/* Define the union u_isp_sharpen_dhsf_2dcoef1_811 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef1_8 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef1_9 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef1_10 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef1_11 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef1_811;

/* Define the union u_isp_sharpen_dhsf_2dcoef1_1215 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef1_12 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef1_1215;

/* Define the union u_isp_sharpen_dhsf_2dcoef2_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef2_0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef2_1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef2_2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef2_3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef2_03;

/* Define the union u_isp_sharpen_dhsf_2dcoef2_47 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef2_4 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef2_5 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef2_6 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef2_7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef2_47;

/* Define the union u_isp_sharpen_dhsf_2dcoef2_811 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef2_8 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef2_9 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef2_10 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef2_11 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef2_811;

/* Define the union u_isp_sharpen_dhsf_2dcoef2_1215 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef2_12 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef2_1215;

/* Define the union u_isp_sharpen_dhsf_2dcoef3_03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef3_0 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef3_1 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef3_2 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef3_3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef3_03;

/* Define the union u_isp_sharpen_dhsf_2dcoef3_47 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef3_4 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef3_5 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef3_6 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef3_7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef3_47;

/* Define the union u_isp_sharpen_dhsf_2dcoef3_811 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef3_8 : 8; /* [7..0]  */
        unsigned int    isp_sharpen_dhsfcoef3_9 : 8; /* [15..8]  */
        unsigned int    isp_sharpen_dhsfcoef3_10 : 8; /* [23..16]  */
        unsigned int    isp_sharpen_dhsfcoef3_11 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef3_811;

/* Define the union u_isp_sharpen_dhsf_2dcoef3_1215 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_dhsfcoef3_12 : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_dhsf_2dcoef3_1215;

/* Define the union u_isp_nddm_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_cac_blend_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_cfg;

/* Define the union u_isp_nddm_gf_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gf_th_low    : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_nddm_gf_th_high   : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_th;

/* Define the union u_isp_nddm_awb_gf_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_awb_gf_gn_low : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    isp_nddm_awb_gf_gn_high : 7; /* [14..8]  */
        unsigned int    reserved_1            : 1; /* [15]  */
        unsigned int    isp_nddm_awb_gf_gn_max : 4; /* [19..16]  */
        unsigned int    reserved_2            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_awb_gf_cfg;

/* Define the union u_isp_nddm_fcr_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_fcr_gf_gain  : 5; /* [4..0]  */
        unsigned int    reserved_0            : 11; /* [15..5]  */
        unsigned int    isp_nddm_fcr_det_low  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_fcr_gain;

/* Define the union u_isp_nddm_dm_bldrate */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_cac_blend_rate : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_dm_bldrate;

/* Define the union u_isp_nddm_ehc_gray */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_ehc_gray     : 7; /* [6..0]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_ehc_gray;

/* Define the union u_isp_nddm_gf_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gf_lut_update : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_lut_update;

/* Define the union u_isp_nddm_dither_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_dith_mask    : 8; /* [7..0]  */
        unsigned int    isp_nddm_dith_ratio   : 8; /* [15..8]  */
        unsigned int    isp_nddm_dith_max     : 8; /* [23..16]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_dither_cfg;

/* Define the union u_isp_nddm_gf_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gf_lut_waddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_lut_waddr;

/* Define the union u_isp_nddm_gf_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gf_lut_wdata : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_lut_wdata;

/* Define the union u_isp_nddm_gf_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gf_lut_raddr : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_lut_raddr;

/* Define the union u_isp_nddm_gf_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gf_lut_rdata : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_lut_rdata;

/* Define the union u_isp_bnr_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_ensptnr       : 1; /* [0]  */
        unsigned int    isp_bnr_entmpnr       : 1; /* [1]  */
        unsigned int    isp_bnr_en2ndtmpnr    : 1; /* [2]  */
        unsigned int    isp_bnr_mono_sensor   : 1; /* [3]  */
        unsigned int    isp_bnr_nrstructure   : 2; /* [5..4]  */
        unsigned int    isp_bnr_isinitial     : 1; /* [6]  */
        unsigned int    isp_bnr_dlymode       : 1; /* [7]  */
        unsigned int    isp_bnr_enshrinkrnt   : 1; /* [8]  */
        unsigned int    isp_bnr_jnlm_lumsel   : 2; /* [10..9]  */
        unsigned int    isp_bnr_wdr_enable    : 2; /* [12..11]  */
        unsigned int    isp_bnr_wdr_mapfltmod : 1; /* [13]  */
        unsigned int    isp_bnr_wdr_mapgain   : 5; /* [18..14]  */
        unsigned int    isp_bnr_encenterweight : 1; /* [19]  */
        unsigned int    isp_bnr_enmixing      : 2; /* [21..20]  */
        unsigned int    isp_bnr_wdrmode       : 2; /* [23..22]  */
        unsigned int    isp_bnr_bm1d_enable   : 4; /* [27..24]  */
        unsigned int    isp_bnr_dstflten      : 1; /* [28]  */
        unsigned int    isp_bnr_wdr_enfusion  : 2; /* [30..29]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg;

/* Define the union u_isp_bnr_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_version        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_version;
/* Define the union u_isp_bnr_jnlmgain0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_r0   : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain0;

/* Define the union u_isp_bnr_jnlmgain1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_gr0  : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain1;

/* Define the union u_isp_bnr_jnlmgain2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_gb0  : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain2;

/* Define the union u_isp_bnr_jnlmgain3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_b0   : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain3;

/* Define the union u_isp_bnr_jnlmgain_s0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_r_s  : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain_s0;

/* Define the union u_isp_bnr_jnlmgain_s1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_gr_s : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain_s1;

/* Define the union u_isp_bnr_jnlmgain_s2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain_gb_s : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain_s2;

/* Define the union u_isp_bnr_jnlmgain_s3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlmgain0     : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlmgain_s3;

/* Define the union u_isp_bnr_flt_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlm_winsize  : 3; /* [2..0]  */
        unsigned int    isp_bnr_jnlm_symcoef  : 8; /* [10..3]  */
        unsigned int    isp_bnr_jnlm_sadratio : 7; /* [17..11]  */
        unsigned int    isp_bnr_jnlm_maxwtcoef : 10; /* [27..18]  */
        unsigned int    isp_bnr_satd_en       : 1; /* [28]  */
        unsigned int    isp_bnr_dmap_en       : 1; /* [29]  */
        unsigned int    isp_bnr_sadi_en       : 1; /* [30]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl;

/* Define the union u_isp_bnr_flt_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlm_gain_m   : 8; /* [7..0]  */
        unsigned int    isp_bnr_coring_enable : 1; /* [8]  */
        unsigned int    isp_bnr_dstmode       : 1; /* [9]  */
        unsigned int    isp_bnr_jnlm_gain_s   : 8; /* [17..10]  */
        unsigned int    reserved_0            : 2; /* [19..18]  */
        unsigned int    isp_bnr_coring_low_motadp : 2; /* [21..20]  */
        unsigned int    isp_bnr_coring_low_motth : 8; /* [29..22]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl1;

/* Define the union u_isp_bnr_flt_ctrl2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_coring_awbgainr : 10; /* [9..0]  */
        unsigned int    isp_bnr_coring_awbgainb : 10; /* [19..10]  */
        unsigned int    isp_bnr_blc           : 5; /* [24..20]  */
        unsigned int    isp_bnr_bm1d_lmt_enable : 1; /* [25]  */
        unsigned int    isp_bnr_nlm_enable    : 1; /* [26]  */
        unsigned int    isp_bnr_nlm_edgectrl  : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl2;

/* Define the union u_isp_bnr_flt_ctrl3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_edgeflten     : 1; /* [0]  */
        unsigned int    isp_bnr_edgedirslp    : 12; /* [12..1]  */
        unsigned int    isp_bnr_edgedgainmax  : 8; /* [20..13]  */
        unsigned int    isp_bnr_edgedgainslp  : 8; /* [28..21]  */
        unsigned int    reserved_0            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl3;

/* Define the union u_isp_bnr_flt_ctrl4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mdetmixratio  : 7; /* [6..0]  */
        unsigned int    isp_bnr_mdetcorlevel  : 8; /* [14..7]  */
        unsigned int    isp_bnr_mdetsadthh    : 16; /* [30..15]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl4;

/* Define the union u_isp_bnr_flt_ctrl5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mdetsadthl    : 16; /* [15..0]  */
        unsigned int    isp_bnr_mdetsadratioh : 8; /* [23..16]  */
        unsigned int    isp_bnr_mdetsadratiol : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl5;

/* Define the union u_isp_bnr_flt_ctrl6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mdetsize      : 3; /* [2..0]  */
        unsigned int    isp_bnr_mdetsadthw    : 2; /* [4..3]  */
        unsigned int    isp_bnr_rntth         : 8; /* [12..5]  */
        unsigned int    isp_bnr_nrstrengthstint : 9; /* [21..13]  */
        unsigned int    isp_bnr_nrstrengthmvint : 9; /* [30..22]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl6;

/* Define the union u_isp_bnr_flt_ctrl7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_nrstrengthslope : 10; /* [9..0]  */
        unsigned int    isp_bnr_expratio      : 13; /* [22..10]  */
        unsigned int    isp_bnr_targetnoiseratio : 8; /* [30..23]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl7;

/* Define the union u_isp_bnr_flt_ctrl8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixgain       : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixgain_0_r   : 8; /* [15..8]  */
        unsigned int    isp_bnr_coring_hig    : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl8;

/* Define the union u_isp_bnr_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_stt2lut_en    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_stt2lut_cfg;

/* Define the union u_isp_bnr_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_stt2lut_regnew;

/* Define the union u_isp_bnr_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_stt2lut_info  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_bnr_stt2lut_clr   : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_stt2lut_abn;

/* Define the union u_isp_bnr_noisesd_even_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noisesd_even_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_even_waddr;
/* Define the union u_isp_bnr_noisesd_even_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_noisesd_even_wdata : 15; /* [14..0]  */
        unsigned int    isp_bnr_noisesd_s_even_wdata : 15; /* [29..15]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_even_wdata;

/* Define the union u_isp_bnr_noisesd_even_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noisesd_even_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_even_raddr;
/* Define the union u_isp_bnr_noisesd_even_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_noisesd_even_rdata : 15; /* [14..0]  */
        unsigned int    isp_bnr_noisesd_s_even_rdata : 15; /* [29..15]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_even_rdata;

/* Define the union u_isp_bnr_noisesd_odd_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noisesd_odd_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_odd_waddr;
/* Define the union u_isp_bnr_noisesd_odd_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_noisesd_odd_wdata : 15; /* [14..0]  */
        unsigned int    isp_bnr_noisesd_s_odd_wdata : 15; /* [29..15]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_odd_wdata;

/* Define the union u_isp_bnr_noisesd_odd_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noisesd_odd_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_odd_raddr;
/* Define the union u_isp_bnr_noisesd_odd_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_noisesd_odd_rdata : 15; /* [14..0]  */
        unsigned int    isp_bnr_noisesd_s_odd_rdata : 15; /* [29..15]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noisesd_odd_rdata;

/* Define the union u_isp_bnr_coring_low_even_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_even_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_even_waddr;
/* Define the union u_isp_bnr_coring_low_even_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_even_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_even_wdata;
/* Define the union u_isp_bnr_coring_low_even_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_even_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_even_raddr;
/* Define the union u_isp_bnr_coring_low_even_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_even_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_even_rdata;
/* Define the union u_isp_bnr_coring_low_odd_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_odd_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_odd_waddr;
/* Define the union u_isp_bnr_coring_low_odd_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_odd_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_odd_wdata;
/* Define the union u_isp_bnr_coring_low_odd_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_odd_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_odd_raddr;
/* Define the union u_isp_bnr_coring_low_odd_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_coring_low_odd_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring_low_odd_rdata;
/* Define the union u_isp_bnr_noiseinv_even_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_even_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_even_waddr;
/* Define the union u_isp_bnr_noiseinv_even_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_even_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_even_wdata;
/* Define the union u_isp_bnr_noiseinv_even_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_even_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_even_raddr;
/* Define the union u_isp_bnr_noiseinv_even_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_even_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_even_rdata;
/* Define the union u_isp_bnr_noiseinv_odd_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_odd_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_odd_waddr;
/* Define the union u_isp_bnr_noiseinv_odd_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_odd_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_odd_wdata;
/* Define the union u_isp_bnr_noiseinv_odd_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_odd_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_odd_raddr;
/* Define the union u_isp_bnr_noiseinv_odd_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinv_odd_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinv_odd_rdata;
/* Define the union u_isp_bnr_flt_ctrl9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_ds0           : 8; /* [7..0]  */
        unsigned int    isp_bnr_ds1           : 8; /* [15..8]  */
        unsigned int    isp_bnr_ds2           : 8; /* [23..16]  */
        unsigned int    isp_bnr_ds3           : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl9;

/* Define the union u_isp_bnr_flt_ctrl10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_ds4           : 8; /* [7..0]  */
        unsigned int    isp_bnr_ds5           : 8; /* [15..8]  */
        unsigned int    isp_bnr_ds6           : 8; /* [23..16]  */
        unsigned int    isp_bnr_ds7           : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl10;

/* Define the union u_isp_bnr_flt_ctrl11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_ds8           : 8; /* [7..0]  */
        unsigned int    isp_bnr_ds9           : 8; /* [15..8]  */
        unsigned int    isp_bnr_ds10          : 8; /* [23..16]  */
        unsigned int    isp_bnr_ds11          : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl11;

/* Define the union u_isp_bnr_flt_ctrl12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_ds12          : 8; /* [7..0]  */
        unsigned int    isp_bnr_ds13          : 8; /* [15..8]  */
        unsigned int    isp_bnr_ds14          : 8; /* [23..16]  */
        unsigned int    isp_bnr_ds15          : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl12;

/* Define the union u_isp_bnr_flt_ctrl13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_dp0           : 8; /* [7..0]  */
        unsigned int    isp_bnr_dp1           : 8; /* [15..8]  */
        unsigned int    isp_bnr_dp2           : 8; /* [23..16]  */
        unsigned int    isp_bnr_dp3           : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl13;

/* Define the union u_isp_bnr_flt_ctrl14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_dp4           : 8; /* [7..0]  */
        unsigned int    isp_bnr_dp5           : 8; /* [15..8]  */
        unsigned int    isp_bnr_dp6           : 8; /* [23..16]  */
        unsigned int    isp_bnr_dp7           : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl14;

/* Define the union u_isp_bnr_flt_ctrl15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_dp8           : 8; /* [7..0]  */
        unsigned int    isp_bnr_dp9           : 8; /* [15..8]  */
        unsigned int    isp_bnr_dp10          : 8; /* [23..16]  */
        unsigned int    isp_bnr_dp11          : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl15;

/* Define the union u_isp_bnr_flt_ctrl16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_dp12          : 8; /* [7..0]  */
        unsigned int    isp_bnr_dp13          : 8; /* [15..8]  */
        unsigned int    isp_bnr_dp14          : 8; /* [23..16]  */
        unsigned int    isp_bnr_dp15          : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_flt_ctrl16;

/* Define the union u_isp_bnr_jnlm_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlm_adaptive : 1; /* [0]  */
        unsigned int    isp_bnr_jnlm_rsad_max : 17; /* [17..1]  */
        unsigned int    isp_bnr_isp_bnr_bm1d_sad_mode : 1; /* [18]  */
        unsigned int    reserved_0            : 1; /* [19]  */
        unsigned int    isp_bnr_jnlm_nlimit_adp_th0 : 8; /* [27..20]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlm_ctrl0;

/* Define the union u_isp_bnr_jnlm_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_jnlm_nlimit_adp_th1 : 8; /* [7..0]  */
        unsigned int    isp_bnr_jnlm_nlimit_adp_thr : 13; /* [20..8]  */
        unsigned int    isp_bnr_jnlm_nlimit_adp_str : 4; /* [24..21]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_jnlm_ctrl1;

/* Define the union u_isp_bnr_coring */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_coring_data_mode : 5; /* [4..0]  */
        unsigned int    isp_bnr_coring_low_stdadp : 1; /* [5]  */
        unsigned int    isp_bnr_coring_low_stdth : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_coring;

/* Define the union u_isp_bnr_eddge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_edge_win_size : 2; /* [1..0]  */
        unsigned int    isp_bnr_edge_ratio_th : 6; /* [7..2]  */
        unsigned int    isp_bnr_egv_coef      : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_eddge;

/* Define the union u_isp_bnr_bm1d_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_noisetha : 5; /* [4..0]  */
        unsigned int    isp_bnr_bm1d_noisem1  : 6; /* [10..5]  */
        unsigned int    isp_bnr_bm1d_noisem2  : 6; /* [16..11]  */
        unsigned int    isp_bnr_bm1d_offsetm  : 6; /* [22..17]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_bm1d_ctrl0;

/* Define the union u_isp_bnr_bm1d_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_noises1  : 6; /* [5..0]  */
        unsigned int    isp_bnr_bm1d_noises2  : 6; /* [11..6]  */
        unsigned int    isp_bnr_bm1d_offsets  : 6; /* [17..12]  */
        unsigned int    isp_bnr_bm1d_dvalth   : 8; /* [25..18]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_bm1d_ctrl1;

/* Define the union u_isp_bnr_bm1d_ctrl2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_dvals1   : 4; /* [3..0]  */
        unsigned int    isp_bnr_bm1d_dvals2   : 4; /* [7..4]  */
        unsigned int    isp_bnr_bm1d_dvals3   : 4; /* [11..8]  */
        unsigned int    isp_bnr_bm1d_dvals4   : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_bm1d_ctrl2;

/* Define the union u_isp_bnr_bm1d_ctrl3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_blend_min1 : 7; /* [6..0]  */
        unsigned int    isp_bnr_bm1d_blend_max1 : 7; /* [13..7]  */
        unsigned int    isp_bnr_bm1d_blend_th1 : 8; /* [21..14]  */
        unsigned int    isp_bnr_bm1d_blend_slp1 : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_bm1d_ctrl3;

/* Define the union u_isp_bnr_bm1d_ctrl4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_blend_min2 : 7; /* [6..0]  */
        unsigned int    isp_bnr_bm1d_blend_max2 : 7; /* [13..7]  */
        unsigned int    isp_bnr_bm1d_blend_th2 : 8; /* [21..14]  */
        unsigned int    isp_bnr_bm1d_blend_slp2 : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_bm1d_ctrl4;

/* Define the union u_isp_bnr_bm1d_ctrl5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_blend_lmt : 8; /* [7..0]  */
        unsigned int    isp_bnr_bm1d_lmt_adp_en : 1; /* [8]  */
        unsigned int    isp_bnr_bm1d_edg_cntrl : 2; /* [10..9]  */
        unsigned int    isp_bnr_bm1d_edgsad_gain : 8; /* [18..11]  */
        unsigned int    isp_bnr_bm1d_edggain_th : 2; /* [20..19]  */
        unsigned int    isp_bnr_bm1d_edgsmth_str : 6; /* [26..21]  */
        unsigned int    isp_bnr_bm1d_output_amap : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_bm1d_ctrl5;

/* Define the union u_isp_bnr_time_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_enable_mix_lut : 1; /* [0]  */
        unsigned int    isp_bnr_movtmp_ratio  : 7; /* [7..1]  */
        unsigned int    isp_bnr_tss           : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_time_ctrl0;

/* Define the union u_isp_bnr_mix_lut0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut0       : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut1       : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut2       : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut3       : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_mix_lut0;

/* Define the union u_isp_bnr_mix_lut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut4       : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut5       : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut6       : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut7       : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_mix_lut1;

/* Define the union u_isp_bnr_mix_lut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut8       : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut9       : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut10      : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut11      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_mix_lut2;

/* Define the union u_isp_bnr_mix_lut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut12      : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut13      : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut14      : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut15      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_mix_lut3;

/* Define the union u_isp_bnr_time_edge */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_tsr0          : 8; /* [7..0]  */
        unsigned int    isp_bnr_tsr1          : 8; /* [15..8]  */
        unsigned int    isp_bnr_nshift        : 3; /* [18..16]  */
        unsigned int    isp_bnr_edge_tp0      : 3; /* [21..19]  */
        unsigned int    isp_bnr_edge_tp1      : 3; /* [24..22]  */
        unsigned int    isp_bnr_tss_mode      : 1; /* [25]  */
        unsigned int    isp_bnr_sht_mode      : 1; /* [26]  */
        unsigned int    isp_bnr_nois_var      : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_time_edge;

/* Define the union u_isp_bnr_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_dstgain_max   : 8; /* [7..0]  */
        unsigned int    isp_bnr_dstgain_slp   : 8; /* [15..8]  */
        unsigned int    isp_bnr_bm1d_dstgain_max : 8; /* [23..16]  */
        unsigned int    isp_bnr_bm1d_dstgain_slp : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg0;

/* Define the union u_isp_bnr_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_flt_mode      : 3; /* [2..0]  */
        unsigned int    isp_bnr_adap_mode     : 2; /* [4..3]  */
        unsigned int    isp_bnr_sym_adp_coef  : 5; /* [9..5]  */
        unsigned int    isp_bnr_nlm_adp_coef  : 5; /* [14..10]  */
        unsigned int    isp_bnr_nlm_dis_adp_coef : 5; /* [19..15]  */
        unsigned int    isp_bnr_bm1d_adp_coef : 5; /* [24..20]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg1;

/* Define the union u_isp_bnr_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_sym_adp_th    : 8; /* [7..0]  */
        unsigned int    isp_bnr_nlm_adp_th    : 8; /* [15..8]  */
        unsigned int    isp_bnr_nlm_dis_adp_th : 8; /* [23..16]  */
        unsigned int    isp_bnr_bm1d_adp_th   : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg2;

/* Define the union u_isp_bnr_cfg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_sym_adp_th1   : 8; /* [7..0]  */
        unsigned int    isp_bnr_nlm_adp_th1   : 8; /* [15..8]  */
        unsigned int    isp_bnr_nlm_dis_adp_th1 : 8; /* [23..16]  */
        unsigned int    isp_bnr_bm1d_adp_th1  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg3;

/* Define the union u_isp_bnr_cfg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_noisel1  : 6; /* [5..0]  */
        unsigned int    isp_bnr_bm1d_noisel2  : 6; /* [11..6]  */
        unsigned int    isp_bnr_bm1d_offsetl  : 6; /* [17..12]  */
        unsigned int    isp_bnr_bm1d_dis_cntrl : 1; /* [18]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg4;

/* Define the union u_isp_bnr_cfg5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_coefdiffy     : 12; /* [11..0]  */
        unsigned int    isp_bnr_coefsatd      : 10; /* [21..12]  */
        unsigned int    isp_bnr_mixgain_0_b   : 8; /* [29..22]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg5;

/* Define the union u_isp_bnr_cfg6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bm1d_strength_s : 8; /* [7..0]  */
        unsigned int    isp_bnr_fusion_str    : 8; /* [15..8]  */
        unsigned int    isp_bnr_lscratio1     : 4; /* [19..16]  */
        unsigned int    isp_bnr_lscratio2     : 8; /* [27..20]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg6;

/* Define the union u_isp_bnr_cfg7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_bit_depth_irin : 5; /* [4..0]  */
        unsigned int    isp_bnr_b_en_ai_md    : 1; /* [5]  */
        unsigned int    isp_bnr_b_en_irmd     : 1; /* [6]  */
        unsigned int    isp_bnr_b_qtr_size_ir : 1; /* [7]  */
        unsigned int    isp_bnr_b_disable1st_rgb_tnr : 1; /* [8]  */
        unsigned int    isp_bnr_mix_ratio_rgb : 7; /* [15..9]  */
        unsigned int    isp_bnr_sht_mode1     : 1; /* [16]  */
        unsigned int    isp_bnr_tfr_en        : 1; /* [17]  */
        unsigned int    isp_bnr_tfr_luma      : 1; /* [18]  */
        unsigned int    isp_bnr_rfr_mode      : 1; /* [19]  */
        unsigned int    isp_bnr_bm1d_n8_coef  : 8; /* [27..20]  */
        unsigned int    isp_bnr_nlm_nshift    : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg7;

/* Define the union u_isp_bnr_cfg8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_wdrlong_low   : 8; /* [7..0]  */
        unsigned int    isp_bnr_wdrlong_hig   : 8; /* [15..8]  */
        unsigned int    isp_bnr_wdrlong_slp   : 11; /* [26..16]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg8;

/* Define the union u_isp_bnr_cfg9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_wdrmid_low    : 8; /* [7..0]  */
        unsigned int    isp_bnr_wdrmid_hig    : 8; /* [15..8]  */
        unsigned int    isp_bnr_wdrmid_slp    : 11; /* [26..16]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg9;

/* Define the union u_isp_bnr_cfg10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_alpha_max_st  : 8; /* [7..0]  */
        unsigned int    isp_bnr_tp_alpha_gain2nd : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg10;

/* Define the union u_isp_bnr_cfg11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut16      : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut17      : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut18      : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut19      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg11;

/* Define the union u_isp_bnr_cfg12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut20      : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut21      : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut22      : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut23      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg12;

/* Define the union u_isp_bnr_cfg13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut24      : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut25      : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut26      : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut27      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg13;

/* Define the union u_isp_bnr_cfg14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_mixlut28      : 8; /* [7..0]  */
        unsigned int    isp_bnr_mixlut29      : 8; /* [15..8]  */
        unsigned int    isp_bnr_mixlut30      : 8; /* [23..16]  */
        unsigned int    isp_bnr_mixlut31      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg14;

/* Define the union u_isp_bnr_cfg15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_tfr_thresh    : 8; /* [7..0]  */
        unsigned int    isp_bnr_tfr_thresh1   : 10; /* [17..8]  */
        unsigned int    isp_bnr_wdr_ghost_nrstr : 5; /* [22..18]  */
        unsigned int    isp_bnr_wdr_ghost_thres : 8; /* [30..23]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_cfg15;

/* Define the union u_isp_bnr_fbratiotable_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_fbratiotable_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_fbratiotable_waddr;
/* Define the union u_isp_bnr_fbratiotable_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_fbratiotable_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_fbratiotable_wdata;
/* Define the union u_isp_bnr_fbratiotable_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_fbratiotable_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_fbratiotable_raddr;
/* Define the union u_isp_bnr_fbratiotable_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_fbratiotable_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_fbratiotable_rdata;
/* Define the union u_isp_bnr_noiseinvs_magidx_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinvs_magidx_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinvs_magidx_waddr;
/* Define the union u_isp_bnr_noiseinvs_magidx_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinvs_magidx_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinvs_magidx_wdata;
/* Define the union u_isp_bnr_noiseinvs_magidx_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinvs_magidx_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinvs_magidx_raddr;
/* Define the union u_isp_bnr_noiseinvs_magidx_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_noiseinvs_magidx_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_noiseinvs_magidx_rdata;
/* Define the union u_isp_bnr_rlsc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_en       : 2; /* [1..0]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    isp_bnr_rlsc_nodenum  : 8; /* [11..4]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_cfg;

/* Define the union u_isp_bnr_rlsc_widthoffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_widthoffset : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_widthoffset;

/* Define the union u_isp_bnr_rlsc_posr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_rx       : 16; /* [15..0]  */
        unsigned int    isp_bnr_rlsc_ry       : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_posr;

/* Define the union u_isp_bnr_rlsc_posgr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_grx      : 16; /* [15..0]  */
        unsigned int    isp_bnr_rlsc_gry      : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_posgr;

/* Define the union u_isp_bnr_rlsc_posgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_gbx      : 16; /* [15..0]  */
        unsigned int    isp_bnr_rlsc_gby      : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_posgb;

/* Define the union u_isp_bnr_rlsc_posb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_bx       : 16; /* [15..0]  */
        unsigned int    isp_bnr_rlsc_by       : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_posb;

/* Define the union u_isp_bnr_rlsc_offcenterr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_offsetcenterr : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_offcenterr;

/* Define the union u_isp_bnr_rlsc_offcentergr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_offsetcentergr : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_offcentergr;

/* Define the union u_isp_bnr_rlsc_offcentergb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_offsetcentergb : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_offcentergb;

/* Define the union u_isp_bnr_rlsc_offcenterb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_offsetcenterb : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_offcenterb;

/* Define the union u_isp_bnr_rlsc_gain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_gainstr  : 16; /* [15..0]  */
        unsigned int    isp_bnr_rlsc_gainscale : 4; /* [19..16]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_gain;

/* Define the union u_isp_bnr_rlsc_maxgain */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_maxgain  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_maxgain;

/* Define the union u_isp_bnr_rlsc_cmp_str */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_rlsc_cmp_str  : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_cmp_str;

/* Define the union u_isp_bnr_rlsc_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_reg;

/* Define the union u_isp_bnr_rlsc_lut0_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut0_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut0_waddr;
/* Define the union u_isp_bnr_rlsc_lut0_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut0_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut0_wdata;
/* Define the union u_isp_bnr_rlsc_lut0_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut0_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut0_raddr;
/* Define the union u_isp_bnr_rlsc_lut0_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut0_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut0_rdata;
/* Define the union u_isp_bnr_rlsc_lut1_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut1_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut1_waddr;
/* Define the union u_isp_bnr_rlsc_lut1_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut1_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut1_wdata;
/* Define the union u_isp_bnr_rlsc_lut1_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut1_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut1_raddr;
/* Define the union u_isp_bnr_rlsc_lut1_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_rlsc_lut1_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_rlsc_lut1_rdata;
/* Define the union u_isp_dehaze_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_version     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_version;
/* Define the union u_isp_dehaze_blk_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_block_sizev : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_dehaze_block_sizeh : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_blk_size;

/* Define the union u_isp_dehaze_blk_sum */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_block_sum  : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_blk_sum;

/* Define the union u_isp_dehaze_dc_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_dc_numv    : 5; /* [4..0]  */
        unsigned int    isp_dehaze_dc_numh    : 5; /* [9..5]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_dc_size;

/* Define the union u_isp_dehaze_x */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_phase_x    : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_x;

/* Define the union u_isp_dehaze_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_phase_y    : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_y;

/* Define the union u_isp_dehaze_stat_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_max_mode   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stat_mode;

/* Define the union u_isp_dehaze_neg_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_neg_mode   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_neg_mode;

/* Define the union u_isp_dehaze_air */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_air_b      : 10; /* [9..0]  */
        unsigned int    isp_dehaze_air_g      : 10; /* [19..10]  */
        unsigned int    isp_dehaze_air_r      : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_air;

/* Define the union u_isp_dehaze_thld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_thld       : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_thld;

/* Define the union u_isp_dehaze_gstrth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_gstrth     : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_gstrth;

/* Define the union u_isp_dehaze_blthld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_blthld     : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_blthld;

/* Define the union u_isp_dehaze_stt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_stt_size   : 16; /* [15..0]  */
        unsigned int    isp_dehaze_stt_bst    : 4; /* [19..16]  */
        unsigned int    isp_dehaze_stt_en     : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stt_bst;

/* Define the union u_isp_dehaze_stt_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_stt_info   : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_dehaze_stt_clr    : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stt_abn;

/* Define the union u_isp_dehaze_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_stt2lut_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stt2lut_cfg;

/* Define the union u_isp_dehaze_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stt2lut_regnew;

/* Define the union u_isp_dehaze_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_dehaze_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stt2lut_abn;

/* Define the union u_isp_dehaze_smlmapoffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_smlmapoffset : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_smlmapoffset;

/* Define the union u_isp_dehaze_stat_point */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_statstartx : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dehaze_statendx   : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stat_point;

/* Define the union u_isp_dehaze_stat_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_stat_numv  : 5; /* [4..0]  */
        unsigned int    isp_dehaze_stat_numh  : 5; /* [9..5]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_stat_num;

/* Define the union u_isp_dehaze_minstat_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_minstat_waddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_minstat_waddr;

/* Define the union u_isp_dehaze_minstat_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_minstat_wdata_l : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_dehaze_minstat_wdata_h : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_minstat_wdata;

/* Define the union u_isp_dehaze_minstat_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_minstat_raddr : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_minstat_raddr;

/* Define the union u_isp_dehaze_minstat_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_minstat_rdata_l : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_dehaze_minstat_rdata_h : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_minstat_rdata;

/* Define the union u_isp_dehaze_maxstat_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_maxstat_waddr : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_maxstat_waddr;

/* Define the union u_isp_dehaze_maxstat_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_maxstat_wdata : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_maxstat_wdata;

/* Define the union u_isp_dehaze_maxstat_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_maxstat_raddr : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_maxstat_raddr;

/* Define the union u_isp_dehaze_maxstat_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_maxstat_rdata : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_maxstat_rdata;

/* Define the union u_isp_dehaze_prestat_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_prestat_waddr : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_prestat_waddr;

/* Define the union u_isp_dehaze_prestat_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_prestat_wdata : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_prestat_wdata;

/* Define the union u_isp_dehaze_prestat_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_prestat_raddr : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_prestat_raddr;

/* Define the union u_isp_dehaze_prestat_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_prestat_rdata : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_prestat_rdata;

/* Define the union u_isp_dehaze_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_lut_waddr  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_lut_waddr;

/* Define the union u_isp_dehaze_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_lut_wdata  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_lut_wdata;

/* Define the union u_isp_dehaze_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_lut_raddr  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_lut_raddr;

/* Define the union u_isp_dehaze_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_lut_rdata  : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_lut_rdata;

/* Define the union u_isp_pregamma_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_cfg;

/* Define the union u_isp_pregamma_idxbase0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_idxbase0 : 8; /* [7..0]  */
        unsigned int    isp_pregamma_idxbase1 : 8; /* [15..8]  */
        unsigned int    isp_pregamma_idxbase2 : 8; /* [23..16]  */
        unsigned int    isp_pregamma_idxbase3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_idxbase0;

/* Define the union u_isp_pregamma_idxbase1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_idxbase4 : 8; /* [7..0]  */
        unsigned int    isp_pregamma_idxbase5 : 8; /* [15..8]  */
        unsigned int    isp_pregamma_idxbase6 : 8; /* [23..16]  */
        unsigned int    isp_pregamma_idxbase7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_idxbase1;

/* Define the union u_isp_pregamma_maxval0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_maxval0  : 8; /* [7..0]  */
        unsigned int    isp_pregamma_maxval1  : 8; /* [15..8]  */
        unsigned int    isp_pregamma_maxval2  : 8; /* [23..16]  */
        unsigned int    isp_pregamma_maxval3  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_maxval0;

/* Define the union u_isp_pregamma_maxval1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_maxval4  : 8; /* [7..0]  */
        unsigned int    isp_pregamma_maxval5  : 8; /* [15..8]  */
        unsigned int    isp_pregamma_maxval6  : 8; /* [23..16]  */
        unsigned int    isp_pregamma_maxval7  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_maxval1;

/* Define the union u_isp_pregamma_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_stt2lut_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_stt2lut_cfg;

/* Define the union u_isp_pregamma_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_stt2lut_regnew;

/* Define the union u_isp_pregamma_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_pregamma_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_stt2lut_abn;

/* Define the union u_isp_pregamma_lut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregamma_lut_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_lut_waddr;
/* Define the union u_isp_pregamma_lut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregamma_lut_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_lut_wdata;
/* Define the union u_isp_pregamma_lut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregamma_lut_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_lut_raddr;
/* Define the union u_isp_pregamma_lut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregamma_lut_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_lut_rdata;
/* Define the union u_isp_gamma_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_cfg;

/* Define the union u_isp_gamma_pos0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_pos_0       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_gamma_pos_1       : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_pos0;

/* Define the union u_isp_gamma_pos1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_pos_2       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_gamma_pos_3       : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_pos1;

/* Define the union u_isp_gamma_pos2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_pos_4       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_gamma_pos_5       : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_pos2;

/* Define the union u_isp_gamma_pos3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_pos_6       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_gamma_pos_7       : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_pos3;

/* Define the union u_isp_gamma_inseg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_inseg_0     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_gamma_inseg_1     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_inseg0;

/* Define the union u_isp_gamma_inseg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_inseg_2     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_gamma_inseg_3     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_inseg1;

/* Define the union u_isp_gamma_inseg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_inseg_4     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_gamma_inseg_5     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_inseg2;

/* Define the union u_isp_gamma_inseg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_inseg_6     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_gamma_inseg_7     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_inseg3;

/* Define the union u_isp_gamma_step */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_step0       : 3; /* [2..0]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    isp_gamma_step1       : 3; /* [6..4]  */
        unsigned int    reserved_1            : 1; /* [7]  */
        unsigned int    isp_gamma_step2       : 3; /* [10..8]  */
        unsigned int    reserved_2            : 1; /* [11]  */
        unsigned int    isp_gamma_step3       : 3; /* [14..12]  */
        unsigned int    reserved_3            : 1; /* [15]  */
        unsigned int    isp_gamma_step4       : 3; /* [18..16]  */
        unsigned int    reserved_4            : 1; /* [19]  */
        unsigned int    isp_gamma_step5       : 3; /* [22..20]  */
        unsigned int    reserved_5            : 1; /* [23]  */
        unsigned int    isp_gamma_step6       : 3; /* [26..24]  */
        unsigned int    reserved_6            : 1; /* [27]  */
        unsigned int    isp_gamma_step7       : 3; /* [30..28]  */
        unsigned int    reserved_7            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_step;

/* Define the union u_isp_gamma_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_stt2lut_en  : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_stt2lut_cfg;

/* Define the union u_isp_gamma_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_stt2lut_regnew;

/* Define the union u_isp_gamma_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_gamma_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_stt2lut_abn;

/* Define the union u_isp_gamma_lut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_waddr    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_lut_waddr;
/* Define the union u_isp_gamma_lut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_wdata    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_lut_wdata;
/* Define the union u_isp_gamma_lut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_raddr    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_lut_raddr;
/* Define the union u_isp_gamma_lut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_rdata    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_lut_rdata;
/* Define the union u_isp_bdec_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bdec_cfg;

/* Define the union u_isp_bdec_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bdec_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bdec_version;
/* Define the union u_isp_bdec_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bdec_alpha        : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bdec_alpha;

/* Define the union u_isp_ca_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_satadj_en      : 1; /* [0]  */
        unsigned int    reserved_0            : 3; /* [3..1]  */
        unsigned int    isp_ca_skinproc_en    : 1; /* [4]  */
        unsigned int    reserved_1            : 3; /* [7..5]  */
        unsigned int    isp_ca_llhcproc_en    : 1; /* [8]  */
        unsigned int    reserved_2            : 3; /* [11..9]  */
        unsigned int    isp_ca_cp_en          : 1; /* [12]  */
        unsigned int    reserved_3            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_ctrl;

/* Define the union u_isp_ca_lumath */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lumath_high    : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_lumath_low     : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lumath;

/* Define the union u_isp_ca_darkchroma_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_darkchromath_high : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_darkchromath_low : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_darkchroma_th;

/* Define the union u_isp_ca_sdarkchroma_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_sdarkchromath_high : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_sdarkchromath_low : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_sdarkchroma_th;

/* Define the union u_isp_ca_llhc_ratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lumaratio_high : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_ca_lumaratio_low  : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_llhc_ratio;

/* Define the union u_isp_ca_isoratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_isoratio       : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_isoratio;

/* Define the union u_isp_ca_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lut_update     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lut_update;

/* Define the union u_isp_ca_yuv2rgb_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_coef01 : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ca_yuv2rgb_coef00 : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_coef0;

/* Define the union u_isp_ca_yuv2rgb_coef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_coef10 : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ca_yuv2rgb_coef02 : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_coef1;

/* Define the union u_isp_ca_yuv2rgb_coef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_coef12 : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ca_yuv2rgb_coef11 : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_coef2;

/* Define the union u_isp_ca_yuv2rgb_coef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_coef21 : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ca_yuv2rgb_coef20 : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_coef3;

/* Define the union u_isp_ca_yuv2rgb_coef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_coef22 : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_coef4;

/* Define the union u_isp_ca_yuv2rgb_indc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_indc1  : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_ca_yuv2rgb_indc0  : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_indc0;

/* Define the union u_isp_ca_yuv2rgb_indc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_indc2  : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_indc1;

/* Define the union u_isp_ca_yuv2rgb_outdc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_outdc1 : 11; /* [10..0]  */
        unsigned int    reserved_0            : 5; /* [15..11]  */
        unsigned int    isp_ca_yuv2rgb_outdc0 : 11; /* [26..16]  */
        unsigned int    reserved_1            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_outdc0;

/* Define the union u_isp_ca_yuv2rgb_outdc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_yuv2rgb_outdc2 : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_yuv2rgb_outdc1;

/* Define the union u_isp_ca_skinlluma_uth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinlluma_umax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinlluma_umin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinlluma_uth;

/* Define the union u_isp_ca_skinlluma_uyth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinlluma_uymax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinlluma_uymin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinlluma_uyth;

/* Define the union u_isp_ca_skinhluma_uth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinhluma_umax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinhluma_umin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinhluma_uth;

/* Define the union u_isp_ca_skinhluma_uyth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinhluma_uymax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinhluma_uymin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinhluma_uyth;

/* Define the union u_isp_ca_skinlluma_vth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinlluma_vmax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinlluma_vmin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinlluma_vth;

/* Define the union u_isp_ca_skinlluma_vyth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinlluma_vymax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinlluma_vymin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinlluma_vyth;

/* Define the union u_isp_ca_skinhluma_vth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinhluma_vmax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinhluma_vmin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinhluma_vth;

/* Define the union u_isp_ca_skinhluma_vyth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinhluma_vymax : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ca_skinhluma_vymin : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skinhluma_vyth;

/* Define the union u_isp_ca_skin_uvdiff */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skin_uvdiff    : 11; /* [10..0]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skin_uvdiff;

/* Define the union u_isp_ca_skin_ratioth0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinratioth_mid : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ca_skinratioth_low : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skin_ratioth0;

/* Define the union u_isp_ca_skin_ratioth1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_skinratioth_high : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_skin_ratioth1;

/* Define the union u_isp_ca_lut_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lut_waddr      : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lut_waddr;

/* Define the union u_isp_ca_lut_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lut_wdata      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lut_wdata;

/* Define the union u_isp_ca_lut_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lut_raddr      : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lut_raddr;

/* Define the union u_isp_ca_lut_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lut_rdata      : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lut_rdata;

/* Define the union u_isp_clut_adj0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_gain0        : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_clut_gain1        : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_adj0;

/* Define the union u_isp_clut_adj1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_gain2        : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_adj1;

/* Define the union u_isp_clut_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_stt2lut_en   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_stt2lut_cfg;

/* Define the union u_isp_clut_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_stt2lut_regnew;

/* Define the union u_isp_clut_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_clut_stt2lut_clr  : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_stt2lut_abn;

/* Define the union u_isp_clut_lut0_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut0_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut0_waddr;

/* Define the union u_isp_clut_lut0_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut0_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut0_wdata;

/* Define the union u_isp_clut_lut0_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut0_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut0_raddr;

/* Define the union u_isp_clut_lut0_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut0_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut0_rdata;

/* Define the union u_isp_clut_lut1_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut1_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut1_waddr;

/* Define the union u_isp_clut_lut1_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut1_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut1_wdata;

/* Define the union u_isp_clut_lut1_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut1_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut1_raddr;

/* Define the union u_isp_clut_lut1_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut1_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut1_rdata;

/* Define the union u_isp_clut_lut2_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut2_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut2_waddr;

/* Define the union u_isp_clut_lut2_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut2_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut2_wdata;

/* Define the union u_isp_clut_lut2_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut2_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut2_raddr;

/* Define the union u_isp_clut_lut2_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut2_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut2_rdata;

/* Define the union u_isp_clut_lut3_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut3_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut3_waddr;

/* Define the union u_isp_clut_lut3_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut3_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut3_wdata;

/* Define the union u_isp_clut_lut3_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut3_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut3_raddr;

/* Define the union u_isp_clut_lut3_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut3_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut3_rdata;

/* Define the union u_isp_clut_lut4_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut4_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut4_waddr;

/* Define the union u_isp_clut_lut4_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut4_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut4_wdata;

/* Define the union u_isp_clut_lut4_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut4_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut4_raddr;

/* Define the union u_isp_clut_lut4_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut4_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut4_rdata;

/* Define the union u_isp_clut_lut5_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut5_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut5_waddr;

/* Define the union u_isp_clut_lut5_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut5_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut5_wdata;

/* Define the union u_isp_clut_lut5_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut5_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut5_raddr;

/* Define the union u_isp_clut_lut5_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut5_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut5_rdata;

/* Define the union u_isp_clut_lut6_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut6_waddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut6_waddr;

/* Define the union u_isp_clut_lut6_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut6_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut6_wdata;

/* Define the union u_isp_clut_lut6_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut6_raddr   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut6_raddr;

/* Define the union u_isp_clut_lut6_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut6_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut6_rdata;

/* Define the union u_isp_clut_lut7_waddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut7_waddr   : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut7_waddr;

/* Define the union u_isp_clut_lut7_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut7_wdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut7_wdata;

/* Define the union u_isp_clut_lut7_raddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut7_raddr   : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut7_raddr;

/* Define the union u_isp_clut_lut7_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut7_rdata   : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut7_rdata;

/* Define the union u_isp_ldci_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_ldci_calc_en      : 1; /* [1]  */
        unsigned int    isp_ldci_wrstat_en    : 1; /* [2]  */
        unsigned int    isp_ldci_rdstat_en    : 1; /* [3]  */
        unsigned int    reserved_1            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_cfg;

/* Define the union u_isp_ldci_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_version;
/* Define the union u_isp_ldci_calc_small_offset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_calc_map_offsetx : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_calc_small_offset;

/* Define the union u_isp_ldci_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_smlmapstride : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_ldci_smlmapheight : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_ldci_total_zone   : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_zone;

/* Define the union u_isp_ldci_glb_hewgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_glb_hewgt    : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_glb_hewgt;

/* Define the union u_isp_ldci_lpfstt_bst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfstt_size  : 16; /* [15..0]  */
        unsigned int    isp_ldci_lpfstt_bst   : 4; /* [19..16]  */
        unsigned int    isp_ldci_lpfstt_en    : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpfstt_bst;

/* Define the union u_isp_ldci_lpfstt_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfstt_info  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_ldci_lpfstt_clr   : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpfstt_abn;

/* Define the union u_isp_ldci_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_scalex       : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_ldci_scaley       : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_scale;

/* Define the union u_isp_ldci_drcen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_calc_drcen   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_drcen;

/* Define the union u_isp_ldci_luma_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_luma_sel     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_luma_sel;

/* Define the union u_isp_ldci_blc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_blc_ctrl     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_blc_ctrl;

/* Define the union u_isp_ldci_pflmt_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_pflmt_en     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_pflmt_en;

/* Define the union u_isp_ldci_pfori */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_pfori0       : 8; /* [7..0]  */
        unsigned int    isp_ldci_pfori1       : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_pfori;

/* Define the union u_isp_ldci_pfsft */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_pfsft0       : 3; /* [2..0]  */
        unsigned int    isp_ldci_pfsft1       : 3; /* [5..3]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_ldci_pfsft2       : 3; /* [10..8]  */
        unsigned int    isp_ldci_pfsft3       : 3; /* [13..11]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_pfsft;

/* Define the union u_isp_ldci_pfthd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_pfthd0       : 8; /* [7..0]  */
        unsigned int    isp_ldci_pfthd1       : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_pfthd;

/* Define the union u_isp_ldci_pfrly */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_pfrly0       : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_ldci_pfrly1       : 5; /* [12..8]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_pfrly;

/* Define the union u_isp_ldci_pfmul */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_pfmul        : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_pfmul;

/* Define the union u_isp_ldci_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_drc_lut_update : 1; /* [0]  */
        unsigned int    isp_ldci_calc_lut_update : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lut_update;

/* Define the union u_isp_ldci_lpf_lpfcoef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfcoef0     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_ldci_lpfcoef1     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_lpfcoef0;

/* Define the union u_isp_ldci_lpf_lpfcoef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfcoef2     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_ldci_lpfcoef3     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_lpfcoef1;

/* Define the union u_isp_ldci_lpf_lpfcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfcoef4     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_ldci_lpfcoef5     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_lpfcoef2;

/* Define the union u_isp_ldci_lpf_lpfcoef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfcoef6     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 7; /* [15..9]  */
        unsigned int    isp_ldci_lpfcoef7     : 9; /* [24..16]  */
        unsigned int    reserved_1            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_lpfcoef3;

/* Define the union u_isp_ldci_lpf_lpfcoef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfcoef8     : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_lpfcoef4;

/* Define the union u_isp_ldci_lpf_lpfsft */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpfsft       : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_lpfsft;

/* Define the union u_isp_ldci_drc_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_drc_waddr     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_drc_waddr;
/* Define the union u_isp_ldci_drc_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_calcdrc_wdata : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ldci_statdrc_wdata : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_drc_wdata;

/* Define the union u_isp_ldci_drc_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_drc_raddr     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_drc_raddr;
/* Define the union u_isp_ldci_drc_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_calcdrc_rdata : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_ldci_statdrc_rdata : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_drc_rdata;

/* Define the union u_isp_ldci_he_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_he_waddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_he_waddr;
/* Define the union u_isp_ldci_he_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_hepos_wdata  : 9; /* [8..0]  */
        unsigned int    isp_ldci_heneg_wdata  : 9; /* [17..9]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_he_wdata;

/* Define the union u_isp_ldci_he_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_he_raddr      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_he_raddr;
/* Define the union u_isp_ldci_he_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_hepos_rdata  : 9; /* [8..0]  */
        unsigned int    isp_ldci_heneg_rdata  : 9; /* [17..9]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_he_rdata;

/* Define the union u_isp_ldci_de_usm_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_de_usm_waddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_de_usm_waddr;
/* Define the union u_isp_ldci_de_usm_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_usmpos_wdata : 9; /* [8..0]  */
        unsigned int    isp_ldci_usmneg_wdata : 9; /* [17..9]  */
        unsigned int    isp_ldci_delut_wdata  : 9; /* [26..18]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_de_usm_wdata;

/* Define the union u_isp_ldci_de_usm_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_de_usm_raddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_de_usm_raddr;
/* Define the union u_isp_ldci_de_usm_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_usmpos_rdata : 9; /* [8..0]  */
        unsigned int    isp_ldci_usmneg_rdata : 9; /* [17..9]  */
        unsigned int    isp_ldci_delut_rdata  : 9; /* [26..18]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_de_usm_rdata;

/* Define the union u_isp_ldci_cgain_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_cgain_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_cgain_waddr;
/* Define the union u_isp_ldci_cgain_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_cgain_wdata  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_cgain_wdata;

/* Define the union u_isp_ldci_cgain_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_cgain_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_cgain_raddr;
/* Define the union u_isp_ldci_cgain_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_cgain_rdata  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_cgain_rdata;

/* Define the union u_isp_ldci_stat_hpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_hstart       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_ldci_hend         : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stat_hpos;

/* Define the union u_isp_ldci_stat_vpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_vstart       : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_ldci_vend         : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stat_vpos;

/* Define the union u_isp_ldci_stat_evratio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_stat_evratio : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stat_evratio;

/* Define the union u_isp_ldci_stat_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_stat_smlmapwidth : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_ldci_stat_smlmapheight : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_ldci_stat_total_zone : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stat_zone;

/* Define the union u_isp_ldci_bende */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_bende        : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_bende;

/* Define the union u_isp_ldci_deref */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_deref        : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_deref;

/* Define the union u_isp_ldci_deh_lpfsft */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_deh_lpfsft   : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_deh_lpfsft;

/* Define the union u_isp_ldci_lpfcoefh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_coefh0       : 9; /* [8..0]  */
        unsigned int    isp_ldci_coefh1       : 9; /* [17..9]  */
        unsigned int    isp_ldci_coefh2       : 9; /* [26..18]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpfcoefh;

/* Define the union u_isp_ldci_lpfcoefv */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_coefv0       : 9; /* [8..0]  */
        unsigned int    isp_ldci_coefv1       : 9; /* [17..9]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpfcoefv;

/* Define the union u_isp_ldci_blk_smlmapwidth0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_blk_smlmapwidth0 : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_blk_smlmapwidth0;

/* Define the union u_isp_ldci_blk_smlmapwidth1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_blk_smlmapwidth1 : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_blk_smlmapwidth1;

/* Define the union u_isp_ldci_blk_smlmapwidth2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_blk_smlmapwidth2 : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_blk_smlmapwidth2;

/* Define the union u_isp_ldci_stat_drcen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_stat_drcen   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stat_drcen;

/* Define the union u_isp_ldci_chrdamp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_chrposdamp   : 8; /* [7..0]  */
        unsigned int    isp_ldci_chrnegdamp   : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_chrdamp;

/* Define the union u_isp_ldci_grobal_map */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_grobal_luma  : 10; /* [9..0]  */
        unsigned int    isp_ldci_grobal_poply2 : 10; /* [19..10]  */
        unsigned int    isp_ldci_grobal_poply3 : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_grobal_map;

/* Define the union u_isp_ldci_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_stt2lut_en   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stt2lut_cfg;

/* Define the union u_isp_ldci_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stt2lut_regnew;

/* Define the union u_isp_ldci_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_ldci_stt2lut_clr  : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_stt2lut_abn;

/* Define the union u_isp_ldci_polyp_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_poply_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyp_waddr;
/* Define the union u_isp_ldci_polyp_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_poply1_wdata : 10; /* [9..0]  */
        unsigned int    isp_ldci_poply2_wdata : 10; /* [19..10]  */
        unsigned int    isp_ldci_poply3_wdata : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyp_wdata;

/* Define the union u_isp_ldci_polyp_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_poply_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyp_raddr;
/* Define the union u_isp_ldci_polyp_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_poply1_rdata : 10; /* [9..0]  */
        unsigned int    isp_ldci_poply2_rdata : 10; /* [19..10]  */
        unsigned int    isp_ldci_poply3_rdata : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyp_rdata;

/* Define the union u_isp_ldci_polyq01_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_plyq01_waddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq01_waddr;
/* Define the union u_isp_ldci_polyq01_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_plyq0_wdata  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ldci_plyq1_wdata  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq01_wdata;

/* Define the union u_isp_ldci_polyq01_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_plyq01_raddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq01_raddr;
/* Define the union u_isp_ldci_polyq01_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_plyq0_rdata  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ldci_plyq1_rdata  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq01_rdata;

/* Define the union u_isp_ldci_polyq23_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_plyq23_waddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq23_waddr;
/* Define the union u_isp_ldci_polyq23_wdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_plyq2_wdata  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ldci_plyq3_wdata  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq23_wdata;

/* Define the union u_isp_ldci_polyq23_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_plyq23_raddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq23_raddr;
/* Define the union u_isp_ldci_polyq23_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_plyq2_rdata  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ldci_plyq3_rdata  : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq23_rdata;

/* Define the union u_isp_ldci_lpf_map_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_map_waddr;
/* Define the union u_isp_ldci_lpf_map_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int reserved_0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_map_wdata;
/* Define the union u_isp_ldci_lpf_map_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_lpfmap_raddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_map_raddr;
/* Define the union u_isp_ldci_lpf_map_rdata */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpflitmap_rdata : 10; /* [9..0]  */
        unsigned int    isp_ldci_lpfplyp2map_rdata : 10; /* [19..10]  */
        unsigned int    isp_ldci_lpfplyp3map_rdata : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_map_rdata;

/* Define the union u_isp_ldci_face_wgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_face_loc_en  : 1; /* [0]  */
        unsigned int    isp_ldci_face_wgt_scale : 5; /* [5..1]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_face_wgt;

/* Define the union u_isp_adrc_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_adrc_wrstat_en    : 1; /* [1]  */
        unsigned int    isp_adrc_rdstat_en    : 1; /* [2]  */
        unsigned int    isp_adrc_vbiflt_en    : 1; /* [3]  */
        unsigned int    isp_adrc_color_corr_en : 1; /* [4]  */
        unsigned int    isp_adrc_pdw_sum_en   : 1; /* [5]  */
        unsigned int    isp_adrc_detail_boost_en : 1; /* [6]  */
        unsigned int    isp_adrc_mono_chroma_en : 1; /* [7]  */
        unsigned int    isp_adrc_pregamma_en  : 1; /* [8]  */
        unsigned int    reserved_1            : 1; /* [9]  */
        unsigned int    isp_adrc_m0_en        : 1; /* [10]  */
        unsigned int    isp_adrc_m1_en        : 1; /* [11]  */
        unsigned int    isp_adrc_m2_en        : 1; /* [12]  */
        unsigned int    isp_adrc_m3_en        : 1; /* [13]  */
        unsigned int    isp_adrc_tmlutidx_sel : 1; /* [14]  */
        unsigned int    isp_adrc_kernel_sel_ds : 1; /* [15]  */
        unsigned int    isp_adrc_level0_kernel_sel_r0 : 1; /* [16]  */
        unsigned int    isp_adrc_level0_kernel_sel_e0 : 1; /* [17]  */
        unsigned int    reserved_2            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_cfg;

/* Define the union u_isp_adrc_lut_update */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_lut0_update  : 1; /* [0]  */
        unsigned int    isp_adrc_lut1_update  : 1; /* [1]  */
        unsigned int    isp_adrc_cclut_update : 1; /* [2]  */
        unsigned int    isp_adrc_mixlut_update : 1; /* [3]  */
        unsigned int    isp_adrc_mlut0_update : 1; /* [4]  */
        unsigned int    isp_adrc_mlut1_update : 1; /* [5]  */
        unsigned int    isp_adrc_mlut2_update : 1; /* [6]  */
        unsigned int    isp_adrc_mlut3_update : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_lut_update;

/* Define the union u_isp_adrc_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_version       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_version;
/* Define the union u_isp_adrc_strength */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_strength     : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_strength;

/* Define the union u_isp_adrc_rgb_wgt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_r_wgt        : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_adrc_g_wgt        : 5; /* [12..8]  */
        unsigned int    reserved_1            : 3; /* [15..13]  */
        unsigned int    isp_adrc_b_wgt        : 6; /* [21..16]  */
        unsigned int    reserved_2            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rgb_wgt;

/* Define the union u_isp_adrc_rgb_slo */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bgslo        : 8; /* [7..0]  */
        unsigned int    isp_adrc_rgslo        : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rgb_slo;

/* Define the union u_isp_adrc_raw_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_raw_eps      : 20; /* [19..0]  */
        unsigned int    isp_adrc_raw_blc      : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_raw_cfg;

/* Define the union u_isp_adrc_pregamma_idxbase0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_idxbase0 : 8; /* [7..0]  */
        unsigned int    isp_adrc_pregamma_idxbase1 : 8; /* [15..8]  */
        unsigned int    isp_adrc_pregamma_idxbase2 : 8; /* [23..16]  */
        unsigned int    isp_adrc_pregamma_idxbase3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_idxbase0;

/* Define the union u_isp_adrc_pregamma_idxbase1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_idxbase4 : 8; /* [7..0]  */
        unsigned int    isp_adrc_pregamma_idxbase5 : 8; /* [15..8]  */
        unsigned int    isp_adrc_pregamma_idxbase6 : 8; /* [23..16]  */
        unsigned int    isp_adrc_pregamma_idxbase7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_idxbase1;

/* Define the union u_isp_adrc_pregamma_idxbase2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_idxbase8 : 8; /* [7..0]  */
        unsigned int    isp_adrc_pregamma_idxbase9 : 8; /* [15..8]  */
        unsigned int    isp_adrc_pregamma_idxbase10 : 8; /* [23..16]  */
        unsigned int    isp_adrc_pregamma_idxbase11 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_idxbase2;

/* Define the union u_isp_adrc_pregamma_maxval0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_maxval0 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_adrc_pregamma_maxval1 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_maxval0;

/* Define the union u_isp_adrc_pregamma_maxval1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_maxval2 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_adrc_pregamma_maxval3 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_maxval1;

/* Define the union u_isp_adrc_pregamma_maxval2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_maxval4 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_adrc_pregamma_maxval5 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_maxval2;

/* Define the union u_isp_adrc_pregamma_maxval3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_maxval6 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_adrc_pregamma_maxval7 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_maxval3;

/* Define the union u_isp_adrc_pregamma_maxval4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_maxval8 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_adrc_pregamma_maxval9 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_maxval4;

/* Define the union u_isp_adrc_pregamma_maxval5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma_maxval10 : 14; /* [13..0]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    isp_adrc_pregamma_maxval11 : 14; /* [29..16]  */
        unsigned int    reserved_1            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_maxval5;

/* Define the union u_isp_adrc_shp_cfg_raw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_shp_log_raw  : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_adrc_shp_exp_raw  : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_shp_cfg_raw;

/* Define the union u_isp_adrc_div_denom_log_raw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_div_denom_log_raw : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_div_denom_log_raw;

/* Define the union u_isp_adrc_denom_exp_raw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_denom_exp_raw : 21; /* [20..0]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_denom_exp_raw;

/* Define the union u_isp_adrc_shp_cfg_luma */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_shp_log_luma : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_adrc_shp_exp_luma : 4; /* [11..8]  */
        unsigned int    reserved_1            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_shp_cfg_luma;

/* Define the union u_isp_adrc_div_denom_log_luma */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_div_denom_log_luma : 18; /* [17..0]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_div_denom_log_luma;

/* Define the union u_isp_adrc_denom_exp_luma */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_denom_exp_luma : 21; /* [20..0]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_denom_exp_luma;

/* Define the union u_isp_adrc_idxbase0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_seg_idxbase0 : 8; /* [7..0]  */
        unsigned int    isp_adrc_seg_idxbase1 : 8; /* [15..8]  */
        unsigned int    isp_adrc_seg_idxbase2 : 8; /* [23..16]  */
        unsigned int    isp_adrc_seg_idxbase3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_idxbase0;

/* Define the union u_isp_adrc_idxbase1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_seg_idxbase4 : 8; /* [7..0]  */
        unsigned int    isp_adrc_seg_idxbase5 : 8; /* [15..8]  */
        unsigned int    isp_adrc_seg_idxbase6 : 8; /* [23..16]  */
        unsigned int    isp_adrc_seg_idxbase7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_idxbase1;

/* Define the union u_isp_adrc_maxval0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_seg_maxval0  : 8; /* [7..0]  */
        unsigned int    isp_adrc_seg_maxval1  : 8; /* [15..8]  */
        unsigned int    isp_adrc_seg_maxval2  : 8; /* [23..16]  */
        unsigned int    isp_adrc_seg_maxval3  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_maxval0;

/* Define the union u_isp_adrc_maxval1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_seg_maxval4  : 8; /* [7..0]  */
        unsigned int    isp_adrc_seg_maxval5  : 8; /* [15..8]  */
        unsigned int    isp_adrc_seg_maxval6  : 8; /* [23..16]  */
        unsigned int    isp_adrc_seg_maxval7  : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_maxval1;

/* Define the union u_isp_adrc_gain_clip */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_gain_clip_step : 4; /* [3..0]  */
        unsigned int    isp_adrc_gain_clip_knee : 4; /* [7..4]  */
        unsigned int    isp_adrc_gain_clip_mode : 1; /* [8]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_gain_clip;

/* Define the union u_isp_adrc_dark_gain_lmt_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_val1_y       : 8; /* [7..0]  */
        unsigned int    isp_adrc_sft1_y       : 4; /* [11..8]  */
        unsigned int    isp_adrc_val2_y       : 4; /* [15..12]  */
        unsigned int    isp_adrc_sft2_y       : 4; /* [19..16]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_dark_gain_lmt_y;

/* Define the union u_isp_adrc_dark_gain_lmt_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_val1_c       : 8; /* [7..0]  */
        unsigned int    isp_adrc_sft1_c       : 4; /* [11..8]  */
        unsigned int    isp_adrc_val2_c       : 4; /* [15..12]  */
        unsigned int    isp_adrc_sft2_c       : 4; /* [19..16]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_dark_gain_lmt_c;

/* Define the union u_isp_adrc_bright_gain_lmt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_val          : 8; /* [7..0]  */
        unsigned int    isp_adrc_sft          : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bright_gain_lmt;

/* Define the union u_isp_adrc_tmlut0_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_tmlut0_waddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_tmlut0_waddr;
/* Define the union u_isp_adrc_tmlut0_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_tmlut0_wdata  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_tmlut0_wdata;
/* Define the union u_isp_adrc_tmlut0_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_tmlut0_raddr  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_tmlut0_raddr;
/* Define the union u_isp_adrc_tmlut0_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_tmlut0_rdata  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_tmlut0_rdata;
/* Define the union u_isp_adrc_cclut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_cclut_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_cclut_waddr;
/* Define the union u_isp_adrc_cclut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_cclut_wdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_cclut_wdata;
/* Define the union u_isp_adrc_cclut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_cclut_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_cclut_raddr;
/* Define the union u_isp_adrc_cclut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_cclut_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_cclut_rdata;
/* Define the union u_isp_adrc_pregamma_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_pregamma_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_waddr;
/* Define the union u_isp_adrc_pregamma_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_pregamma_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_wdata;
/* Define the union u_isp_adrc_pregamma_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_pregamma_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_raddr;
/* Define the union u_isp_adrc_pregamma_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_pregamma_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma_rdata;
/* Define the union u_isp_adrc_localmix_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_localmix_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_localmix_waddr;
/* Define the union u_isp_adrc_localmix_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_localmix_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_localmix_wdata;
/* Define the union u_isp_adrc_localmix_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_localmix_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_localmix_raddr;
/* Define the union u_isp_adrc_localmix_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_localmix_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_localmix_rdata;
/* Define the union u_isp_adrc_lut_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_cc_lut_ctrl0 : 1; /* [0]  */
        unsigned int    isp_adrc_cc_lut_ctrl1 : 1; /* [1]  */
        unsigned int    isp_adrc_cc_lut_ctrl2 : 1; /* [2]  */
        unsigned int    reserved_0            : 5; /* [7..3]  */
        unsigned int    isp_adrc_tm_lut_ctrl  : 1; /* [8]  */
        unsigned int    reserved_1            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_lut_ctrl;

/* Define the union u_isp_adrc_sup_dark0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_darkmin0     : 8; /* [7..0]  */
        unsigned int    isp_adrc_darkmax0     : 8; /* [15..8]  */
        unsigned int    isp_adrc_darkthr0     : 8; /* [23..16]  */
        unsigned int    isp_adrc_darkslo0     : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_sup_dark0;

/* Define the union u_isp_adrc_sup_bright0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_brightmin0   : 8; /* [7..0]  */
        unsigned int    isp_adrc_brightmax0   : 8; /* [15..8]  */
        unsigned int    isp_adrc_brightthr0   : 8; /* [23..16]  */
        unsigned int    isp_adrc_brightslo0   : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_sup_bright0;

/* Define the union u_isp_adrc_sup1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_boostfact1   : 8; /* [7..0]  */
        unsigned int    isp_adrc_supfact1     : 8; /* [15..8]  */
        unsigned int    isp_adrc_dsubfact1    : 4; /* [19..16]  */
        unsigned int    isp_adrc_dsubfact0    : 5; /* [24..20]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_sup1;

/* Define the union u_isp_adrc_sup_dark2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_darkmin2     : 8; /* [7..0]  */
        unsigned int    isp_adrc_darkmax2     : 8; /* [15..8]  */
        unsigned int    isp_adrc_darkthr2     : 8; /* [23..16]  */
        unsigned int    isp_adrc_darkslo2     : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_sup_dark2;

/* Define the union u_isp_adrc_sup_bright2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_brightmin2   : 8; /* [7..0]  */
        unsigned int    isp_adrc_brightmax2   : 8; /* [15..8]  */
        unsigned int    isp_adrc_brightthr2   : 8; /* [23..16]  */
        unsigned int    isp_adrc_brightslo2   : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_sup_bright2;

/* Define the union u_isp_adrc_bld_darkl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_darkminl     : 8; /* [7..0]  */
        unsigned int    isp_adrc_darkmaxl     : 8; /* [15..8]  */
        unsigned int    isp_adrc_darkthrl     : 8; /* [23..16]  */
        unsigned int    isp_adrc_darkslol     : 4; /* [27..24]  */
        unsigned int    isp_adrc_bld_sftl     : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bld_darkl;

/* Define the union u_isp_adrc_bld_brightl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_brightminl   : 8; /* [7..0]  */
        unsigned int    isp_adrc_brightmaxl   : 8; /* [15..8]  */
        unsigned int    isp_adrc_brightthrl   : 8; /* [23..16]  */
        unsigned int    isp_adrc_brightslol   : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bld_brightl;

/* Define the union u_isp_adrc_bld_darkd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_darkmind     : 8; /* [7..0]  */
        unsigned int    isp_adrc_darkmaxd     : 8; /* [15..8]  */
        unsigned int    isp_adrc_darkthrd     : 8; /* [23..16]  */
        unsigned int    isp_adrc_darkslod     : 4; /* [27..24]  */
        unsigned int    isp_adrc_bld_sftd     : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bld_darkd;

/* Define the union u_isp_adrc_bld_brightd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_brightmind   : 8; /* [7..0]  */
        unsigned int    isp_adrc_brightmaxd   : 8; /* [15..8]  */
        unsigned int    isp_adrc_brightthrd   : 8; /* [23..16]  */
        unsigned int    isp_adrc_brightslod   : 4; /* [27..24]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bld_brightd;

/* Define the union u_isp_adrc_prev_luma_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_0  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_0;

/* Define the union u_isp_adrc_prev_luma_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_1  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_1;

/* Define the union u_isp_adrc_prev_luma_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_2  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_2;

/* Define the union u_isp_adrc_prev_luma_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_3  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_3;

/* Define the union u_isp_adrc_prev_luma_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_4  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_4;

/* Define the union u_isp_adrc_prev_luma_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_5  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_5;

/* Define the union u_isp_adrc_prev_luma_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_6  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_6;

/* Define the union u_isp_adrc_prev_luma_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_prev_luma_7  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_prev_luma_7;

/* Define the union u_isp_adrc_zone */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_hnum         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_adrc_vnum         : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_zone;

/* Define the union u_isp_adrc_zone_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_zone_hsize   : 9; /* [8..0]  */
        unsigned int    isp_adrc_chk_x        : 6; /* [14..9]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_adrc_zone_vsize   : 9; /* [24..16]  */
        unsigned int    isp_adrc_chk_y        : 6; /* [30..25]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_zone_size;

/* Define the union u_isp_adrc_zone_div0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_div_x0       : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_adrc_div_y0       : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_zone_div0;

/* Define the union u_isp_adrc_zone_div1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_div_x1       : 15; /* [14..0]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_adrc_div_y1       : 15; /* [30..16]  */
        unsigned int    reserved_1            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_zone_div1;

/* Define the union u_isp_adrc_bin */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bin          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bin;

/* Define the union u_isp_adrc_zone_init */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_acc_x_init   : 16; /* [15..0]  */
        unsigned int    isp_adrc_cnt_x_init   : 9; /* [24..16]  */
        unsigned int    isp_adrc_idx_x_init   : 6; /* [30..25]  */
        unsigned int    isp_adrc_big_x_init   : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_zone_init;

/* Define the union u_isp_adrc_bin_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bin_scale    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_bin_scale;

/* Define the union u_isp_adrc_outbits */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_outbits      : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_outbits;

/* Define the union u_isp_adrc_mlut0_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut0_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut0_waddr;
/* Define the union u_isp_adrc_mlut0_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut0_wdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut0_wdata;
/* Define the union u_isp_adrc_mlut0_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut0_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut0_raddr;
/* Define the union u_isp_adrc_mlut0_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut0_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut0_rdata;
/* Define the union u_isp_adrc_mlut1_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut1_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut1_waddr;
/* Define the union u_isp_adrc_mlut1_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut1_wdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut1_wdata;
/* Define the union u_isp_adrc_mlut1_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut1_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut1_raddr;
/* Define the union u_isp_adrc_mlut1_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut1_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut1_rdata;
/* Define the union u_isp_adrc_mlut2_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut2_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut2_waddr;
/* Define the union u_isp_adrc_mlut2_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut2_wdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut2_wdata;
/* Define the union u_isp_adrc_mlut2_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut2_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut2_raddr;
/* Define the union u_isp_adrc_mlut2_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut2_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut2_rdata;
/* Define the union u_isp_adrc_mlut3_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut3_waddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut3_waddr;
/* Define the union u_isp_adrc_mlut3_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut3_wdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut3_wdata;
/* Define the union u_isp_adrc_mlut3_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut3_raddr   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut3_raddr;
/* Define the union u_isp_adrc_mlut3_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_mlut3_rdata   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut3_rdata;
/* Define the union u_isp_adrc_local_edge_lmt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_local_edge_lmt : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_local_edge_lmt;

/* Define the union u_isp_adrc_stat_width */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_stat_width   : 16; /* [15..0]  */
        unsigned int    isp_adrc_total_width  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_stat_width;

/* Define the union u_isp_adrc_vbi_strength */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_var_rng_fine : 2; /* [1..0]  */
        unsigned int    reserved_0            : 10; /* [11..2]  */
        unsigned int    isp_adrc_var_spa_fine : 2; /* [13..12]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_vbi_strength;

/* Define the union u_isp_adrc_flt_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_flt_spa_fine : 4; /* [3..0]  */
        unsigned int    reserved_0            : 8; /* [11..4]  */
        unsigned int    isp_adrc_flt_rng_fine : 4; /* [15..12]  */
        unsigned int    reserved_1            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_flt_cfg;

/* Define the union u_isp_adrc_grad_rev */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_grad_rev_thres : 8; /* [7..0]  */
        unsigned int    isp_adrc_grad_rev_max : 7; /* [14..8]  */
        unsigned int    reserved_0            : 1; /* [15]  */
        unsigned int    isp_adrc_grad_rev_slope : 4; /* [19..16]  */
        unsigned int    reserved_1            : 4; /* [23..20]  */
        unsigned int    isp_adrc_grad_rev_shift : 3; /* [26..24]  */
        unsigned int    reserved_2            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_grad_rev;

/* Define the union u_isp_adrc_wgt_box_tri_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgt_box_tri_sel : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgt_box_tri_sel;

/* Define the union u_isp_adrc_vbi_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_vbi_state    : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_vbi_state;

/* Define the union u_isp_adrc_blk_wgt_init */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_blk_wgt_init : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_blk_wgt_init;

/* Define the union u_isp_adrc_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_num          : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_num;

/* Define the union u_isp_adrc_logalphadec */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_logalphadec0 : 4; /* [3..0]  */
        unsigned int    isp_adrc_logalphadec1 : 4; /* [7..4]  */
        unsigned int    isp_adrc_logalphadec2 : 4; /* [11..8]  */
        unsigned int    isp_adrc_logalphadec3 : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_logalphadec;

/* Define the union u_isp_adrc_outbitw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_outbitw0     : 5; /* [4..0]  */
        unsigned int    reserved_0            : 3; /* [7..5]  */
        unsigned int    isp_adrc_outbitw1     : 5; /* [12..8]  */
        unsigned int    reserved_1            : 3; /* [15..13]  */
        unsigned int    isp_adrc_outbitw2     : 5; /* [20..16]  */
        unsigned int    reserved_2            : 3; /* [23..21]  */
        unsigned int    isp_adrc_outbitw3     : 5; /* [28..24]  */
        unsigned int    reserved_3            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_outbitw;

/* Define the union u_isp_adrc_lumaclip_stepl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_lumaclip_stepl0 : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_adrc_lumaclip_stepl1 : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_adrc_lumaclip_stepl2 : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_adrc_lumaclip_stepl3 : 6; /* [29..24]  */
        unsigned int    reserved_3            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_lumaclip_stepl;

/* Define the union u_isp_adrc_lumaclip_steph */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_lumaclip_steph0 : 6; /* [5..0]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    isp_adrc_lumaclip_steph1 : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_adrc_lumaclip_steph2 : 6; /* [21..16]  */
        unsigned int    reserved_2            : 2; /* [23..22]  */
        unsigned int    isp_adrc_lumaclip_steph3 : 6; /* [29..24]  */
        unsigned int    reserved_3            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_lumaclip_steph;

/* Define the union u_isp_adrc_debug_info */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_adrc_debug_info    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_debug_info;

/* Define the union u_isp_adrc_wgtm_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgtm_en      : 1; /* [0]  */
        unsigned int    isp_adrc_wgtm_sft     : 4; /* [4..1]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgtm_cfg;

/* Define the union u_isp_adrc_wgtm_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgtm_0       : 8; /* [7..0]  */
        unsigned int    isp_adrc_wgtm_1       : 8; /* [15..8]  */
        unsigned int    isp_adrc_wgtm_2       : 8; /* [23..16]  */
        unsigned int    isp_adrc_wgtm_3       : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgtm_0;

/* Define the union u_isp_adrc_wgtm_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgtm_4       : 8; /* [7..0]  */
        unsigned int    isp_adrc_wgtm_5       : 8; /* [15..8]  */
        unsigned int    isp_adrc_wgtm_6       : 8; /* [23..16]  */
        unsigned int    isp_adrc_wgtm_7       : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgtm_1;

/* Define the union u_isp_adrc_wgtm_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgtm_8       : 8; /* [7..0]  */
        unsigned int    isp_adrc_wgtm_9       : 8; /* [15..8]  */
        unsigned int    isp_adrc_wgtm_10      : 8; /* [23..16]  */
        unsigned int    isp_adrc_wgtm_11      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgtm_2;

/* Define the union u_isp_adrc_wgtm_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgtm_12      : 8; /* [7..0]  */
        unsigned int    isp_adrc_wgtm_13      : 8; /* [15..8]  */
        unsigned int    isp_adrc_wgtm_14      : 8; /* [23..16]  */
        unsigned int    isp_adrc_wgtm_15      : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgtm_3;

/* Define the union u_isp_adrc_wgtm_pd_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgtm_pd_slo  : 4; /* [3..0]  */
        unsigned int    isp_adrc_wgtm_pd_sft  : 4; /* [7..4]  */
        unsigned int    isp_adrc_wgtm_pd_thr  : 6; /* [13..8]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgtm_pd_cfg;

/* Define the union u_isp_adrc_rb00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr00     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr00     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr00     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr00     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb00;

/* Define the union u_isp_adrc_rb01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr01     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr01     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr01     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr01     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb01;

/* Define the union u_isp_adrc_rb02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr02     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr02     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr02     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr02     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb02;

/* Define the union u_isp_adrc_rb03 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr03     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr03     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr03     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr03     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb03;

/* Define the union u_isp_adrc_rb04 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr04     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr04     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr04     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr04     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb04;

/* Define the union u_isp_adrc_rb05 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr05     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr05     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr05     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr05     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb05;

/* Define the union u_isp_adrc_rb06 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr06     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr06     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr06     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr06     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb06;

/* Define the union u_isp_adrc_rb07 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr07     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr07     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr07     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr07     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb07;

/* Define the union u_isp_adrc_rb08 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr08     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr08     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr08     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr08     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb08;

/* Define the union u_isp_adrc_rb09 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr09     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr09     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr09     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr09     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb09;

/* Define the union u_isp_adrc_rb10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr10     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr10     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr10     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr10     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb10;

/* Define the union u_isp_adrc_rb11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_bg_thr11     : 8; /* [7..0]  */
        unsigned int    isp_adrc_bg_ctr11     : 8; /* [15..8]  */
        unsigned int    isp_adrc_rg_thr11     : 8; /* [23..16]  */
        unsigned int    isp_adrc_rg_ctr11     : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_rb11;

/* Define the union u_isp_adrc_stat_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_statformat   : 1; /* [0]  */
        unsigned int    isp_adrc_statflttype  : 2; /* [2..1]  */
        unsigned int    isp_adrc_statcoefsft0 : 4; /* [6..3]  */
        unsigned int    isp_adrc_statcoefsft1 : 4; /* [10..7]  */
        unsigned int    reserved_0            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_stat_cfg;

/* Define the union u_isp_adrc_luma_avg_manual */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_lumavegmanual : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_luma_avg_manual;

/* Define the union u_isp_adrc_pw_curve_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_curveslosft  : 4; /* [3..0]  */
        unsigned int    reserved_0            : 12; /* [15..4]  */
        unsigned int    isp_adrc_pwcurvebias  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_cfg;

/* Define the union u_isp_adrc_pw_curve_seg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg0  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg1  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg0;

/* Define the union u_isp_adrc_pw_curve_seg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg2  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg3  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg1;

/* Define the union u_isp_adrc_pw_curve_seg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg4  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg5  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg2;

/* Define the union u_isp_adrc_pw_curve_seg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg6  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg7  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg3;

/* Define the union u_isp_adrc_pw_curve_seg4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg8  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg9  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg4;

/* Define the union u_isp_adrc_pw_curve_seg5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg10 : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg11 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg5;

/* Define the union u_isp_adrc_pw_curve_seg6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg12 : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg13 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg6;

/* Define the union u_isp_adrc_pw_curve_seg7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveseg14 : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveseg15 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_seg7;

/* Define the union u_isp_adrc_pw_curve_slo0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo0  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo1  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo0;

/* Define the union u_isp_adrc_pw_curve_slo1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo2  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo3  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo1;

/* Define the union u_isp_adrc_pw_curve_slo2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo4  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo5  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo2;

/* Define the union u_isp_adrc_pw_curve_slo3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo6  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo7  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo3;

/* Define the union u_isp_adrc_pw_curve_slo4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo8  : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo9  : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo4;

/* Define the union u_isp_adrc_pw_curve_slo5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo10 : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo11 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo5;

/* Define the union u_isp_adrc_pw_curve_slo6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo12 : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo13 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo6;

/* Define the union u_isp_adrc_pw_curve_slo7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pwcurveslo14 : 16; /* [15..0]  */
        unsigned int    isp_adrc_pwcurveslo15 : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pw_curve_slo7;

/* Define the union u_isp_adrc_pd_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pdrg_crt     : 8; /* [7..0]  */
        unsigned int    isp_adrc_pdrg_wid     : 8; /* [15..8]  */
        unsigned int    isp_adrc_pdrg_slo     : 3; /* [18..16]  */
        unsigned int    isp_adrc_pdbg_thr     : 8; /* [26..19]  */
        unsigned int    isp_adrc_pdbg_slo     : 3; /* [29..27]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pd_cfg;

/* Define the union u_isp_adrc_pwd_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pdwlowslo    : 5; /* [4..0]  */
        unsigned int    isp_adrc_pdwlowthr    : 8; /* [12..5]  */
        unsigned int    isp_adrc_pdwhighslo   : 5; /* [17..13]  */
        unsigned int    isp_adrc_pdwhighthr   : 8; /* [25..18]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pwd_cfg;

/* Define the union u_isp_adrc_tm_gain_skip */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_tmgainskip   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_tm_gain_skip;

/* Define the union u_isp_adrc_npu_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_npu_wrt_en   : 1; /* [0]  */
        unsigned int    isp_adrc_npu_wrt_sel  : 1; /* [1]  */
        unsigned int    isp_adrc_npu_wrt_ds   : 2; /* [3..2]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_npu_cfg;

/* Define the union u_isp_adrc_mask_ada_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_maskada_en   : 1; /* [0]  */
        unsigned int    isp_adrc_maskadasft   : 2; /* [2..1]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mask_ada_cfg;

/* Define the union u_isp_adrc_mask_ada_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_maskadacoef0 : 4; /* [3..0]  */
        unsigned int    isp_adrc_maskadacoef1 : 4; /* [7..4]  */
        unsigned int    isp_adrc_maskadacoef2 : 4; /* [11..8]  */
        unsigned int    isp_adrc_maskadacoef3 : 4; /* [15..12]  */
        unsigned int    isp_adrc_maskadacoef4 : 4; /* [19..16]  */
        unsigned int    isp_adrc_maskadacoef5 : 4; /* [23..20]  */
        unsigned int    isp_adrc_maskadacoef6 : 4; /* [27..24]  */
        unsigned int    isp_adrc_maskadacoef7 : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mask_ada_coef0;

/* Define the union u_isp_adrc_mask_ada_coef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_maskadacoef8 : 4; /* [3..0]  */
        unsigned int    isp_adrc_maskadacoef9 : 4; /* [7..4]  */
        unsigned int    isp_adrc_maskadacoef10 : 4; /* [11..8]  */
        unsigned int    isp_adrc_maskadacoef11 : 4; /* [15..12]  */
        unsigned int    isp_adrc_maskadacoef12 : 4; /* [19..16]  */
        unsigned int    isp_adrc_maskadacoef13 : 4; /* [23..20]  */
        unsigned int    isp_adrc_maskadacoef14 : 4; /* [27..24]  */
        unsigned int    isp_adrc_maskadacoef15 : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mask_ada_coef1;

/* Define the union u_isp_adrc_wgt_tm */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_wgt_r_tm     : 8; /* [7..0]  */
        unsigned int    isp_adrc_wgt_g_tm     : 8; /* [15..8]  */
        unsigned int    isp_adrc_wgt_b_tm     : 8; /* [23..16]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_wgt_tm;

/* Define the union u_isp_degammafe_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    reserved_1            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_cfg;

/* Define the union u_isp_degammafe_idxbase0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_idxbase0 : 8; /* [7..0]  */
        unsigned int    isp_degammafe_idxbase1 : 8; /* [15..8]  */
        unsigned int    isp_degammafe_idxbase2 : 8; /* [23..16]  */
        unsigned int    isp_degammafe_idxbase3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_idxbase0;

/* Define the union u_isp_degammafe_idxbase1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_idxbase4 : 8; /* [7..0]  */
        unsigned int    isp_degammafe_idxbase5 : 8; /* [15..8]  */
        unsigned int    isp_degammafe_idxbase6 : 8; /* [23..16]  */
        unsigned int    isp_degammafe_idxbase7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_idxbase1;

/* Define the union u_isp_degammafe_maxval0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_maxval0 : 8; /* [7..0]  */
        unsigned int    isp_degammafe_maxval1 : 8; /* [15..8]  */
        unsigned int    isp_degammafe_maxval2 : 8; /* [23..16]  */
        unsigned int    isp_degammafe_maxval3 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_maxval0;

/* Define the union u_isp_degammafe_maxval1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_maxval4 : 8; /* [7..0]  */
        unsigned int    isp_degammafe_maxval5 : 8; /* [15..8]  */
        unsigned int    isp_degammafe_maxval6 : 8; /* [23..16]  */
        unsigned int    isp_degammafe_maxval7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_maxval1;

/* Define the union u_isp_degammafe_stt2lut_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_stt2lut_en : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_stt2lut_cfg;

/* Define the union u_isp_degammafe_stt2lut_regnew */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_stt2lut_regnew : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_stt2lut_regnew;

/* Define the union u_isp_degammafe_stt2lut_abn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_stt2lut_info : 16; /* [15..0]  */
        unsigned int    reserved_0            : 15; /* [30..16]  */
        unsigned int    isp_degammafe_stt2lut_clr : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_stt2lut_abn;

/* Define the union u_isp_degammafe_lut_waddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_degammafe_lut_waddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_lut_waddr;
/* Define the union u_isp_degammafe_lut_wdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_degammafe_lut_wdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_lut_wdata;
/* Define the union u_isp_degammafe_lut_raddr */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_degammafe_lut_raddr : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_lut_raddr;
/* Define the union u_isp_degammafe_lut_rdata */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_degammafe_lut_rdata : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_lut_rdata;
/* Define the union u_isp_split_version */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_split_version      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_version;
/* Define the union u_isp_split_data_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_split_replace_y_en : 1; /* [0]  */
        unsigned int    isp_split_replace_u_en : 1; /* [1]  */
        unsigned int    isp_split_replace_v_en : 1; /* [2]  */
        unsigned int    isp_split_ex_y_en     : 1; /* [3]  */
        unsigned int    isp_split_ex_u_en     : 1; /* [4]  */
        unsigned int    isp_split_ex_v_en     : 1; /* [5]  */
        unsigned int    isp_split_ex_yuv_en   : 3; /* [8..6]  */
        unsigned int    isp_split_split_en    : 1; /* [9]  */
        unsigned int    isp_split_data_sel    : 2; /* [11..10]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_split_dc_data     : 7; /* [22..16]  */
        unsigned int    reserved_1            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_data_cfg;

/* Define the union u_isp_split_data_version */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_split_data_version : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_data_version;

/* Define the union u_isp_split_data_bitwidth */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_split_data_bitw   : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_data_bitwidth;

/* Define the union u_isp_split_data_again */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_split_data_again   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_data_again;
/* Define the union u_isp_split_data_dgain */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_split_data_dgain   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_data_dgain;
/* Define the union u_isp_split_data_shutter */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_split_data_shutter : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_split_data_shutter;

/* Define the global struct */
typedef struct {
    volatile unsigned int           reserved_0[32]; /* 0x0~0x007c, reserved 32 * 4 bytes */
    volatile u_isp_be_version       isp_be_version; /* 0x0080 */
    volatile u_isp_be_date          isp_be_date; /* 0x0084 */
    volatile u_isp_be_fpga_date     isp_be_fpga_date; /* 0x0088 */
    volatile unsigned int           reserved_31; /* 0x008c */
    volatile u_isp_be_module_pos    isp_be_module_pos; /* 0x0090 */
    volatile u_isp_be_fstart_delay   isp_be_fstart_delay; /* 0x0094 */
    volatile u_isp_be_input_mux     isp_be_input_mux; /* 0x0098 */
    volatile u_isp_be_sys_ctrl      isp_be_sys_ctrl; /* 0x009c */
    volatile u_isp_be_user_define0   isp_be_user_define0; /* 0x00a0 */
    volatile u_isp_be_user_define1   isp_be_user_define1; /* 0x00a4 */
    volatile unsigned int           reserved_32[2]; /* 0x00a8~0x00ac, reserved 2 * 4 bytes */
    volatile u_isp_be_startup       isp_be_startup; /* 0x00b0 */
    volatile u_isp_be_format        isp_be_format; /* 0x00b4 */ /* 0 1 2 */
    volatile unsigned int           reserved_33[466]; /* 0x00b8~0x07fc, reserved 466 * 4 bytes */
    volatile u_isp_clip_y_cfg       isp_clip_y_cfg; /* 0x0800 */ /* post + pre == */
    volatile u_isp_clip_c_cfg       isp_clip_c_cfg; /* 0x0804 */ /* post + pre == */
    volatile unsigned int           reserved_34[6]; /* 0x0808~0x081c, reserved 6 * 4 bytes */
    volatile u_isp_csc_sum_cfg      isp_csc_sum_cfg; /* 0x0820 */
    volatile u_isp_yuv444_sum_cfg   isp_yuv444_sum_cfg; /* 0x0824 */
    volatile u_isp_yuv422_sum_cfg   isp_yuv422_sum_cfg; /* 0x0828 */
    volatile unsigned int           reserved_35; /* 0x082c */
    volatile unsigned int           reserved_2; /* 0x0830 */
    volatile u_isp_demosaic_sum_cfg   isp_demosaic_sum_cfg; /* 0x0834 */
    volatile unsigned int           reserved_36[2]; /* 0x0838~0x083c, reserved 2 * 4 bytes */
    volatile u_isp_dmnr_dither      isp_dmnr_dither; /* 0x0840 */
    volatile unsigned int           reserved_37[3]; /* 0x0844~0x084c, reserved 3 * 4 bytes */
    volatile u_isp_acm_dither       isp_acm_dither; /* 0x0850 */
    volatile unsigned int           reserved_38[3]; /* 0x0854~0x085c, reserved 3 * 4 bytes */
    volatile u_isp_drc_dither       isp_drc_dither; /* 0x0860 */
    volatile unsigned int           reserved_39[3]; /* 0x0864~0x086c, reserved 3 * 4 bytes */
    volatile u_isp_sqrt1_dither     isp_sqrt1_dither; /* 0x0870 */
    volatile u_isp_sharpen_dither   isp_sharpen_dither; /* 0x0874 */
    volatile u_isp_blk_hblank       isp_blk_hblank; /* 0x0878 */
    volatile u_isp_blk_vblank       isp_blk_vblank; /* 0x087c */
    volatile u_isp_y_sum0           isp_y_sum0; /* 0x0880 */
    volatile u_isp_y_sum1           isp_y_sum1; /* 0x0884 */
    volatile unsigned int           reserved_40[2]; /* 0x0888~0x088c, reserved 2 * 4 bytes */
    volatile u_isp_yuv444_y_sum0    isp_yuv444_y_sum0; /* 0x0890 */
    volatile u_isp_yuv444_y_sum1    isp_yuv444_y_sum1; /* 0x0894 */
    volatile u_isp_yuv444_u_sum0    isp_yuv444_u_sum0; /* 0x0898 */
    volatile u_isp_yuv444_u_sum1    isp_yuv444_u_sum1; /* 0x089c */
    volatile u_isp_yuv444_v_sum0    isp_yuv444_v_sum0; /* 0x08a0 */
    volatile u_isp_yuv444_v_sum1    isp_yuv444_v_sum1; /* 0x08a4 */
    volatile unsigned int           reserved_41[2]; /* 0x08a8~0x08ac, reserved 2 * 4 bytes */
    volatile u_isp_yuv422_y_sum0    isp_yuv422_y_sum0; /* 0x08b0 */
    volatile u_isp_yuv422_y_sum1    isp_yuv422_y_sum1; /* 0x08b4 */
    volatile u_isp_yuv422_c_sum0    isp_yuv422_c_sum0; /* 0x08b8 */
    volatile u_isp_yuv422_c_sum1    isp_yuv422_c_sum1; /* 0x08bc */
    volatile u_isp_wdr0_sum0        isp_wdr0_sum0; /* 0x08c0 */
    volatile u_isp_wdr0_sum1        isp_wdr0_sum1; /* 0x08c4 */
    volatile u_isp_wdr1_sum0        isp_wdr1_sum0; /* 0x08c8 */
    volatile u_isp_wdr1_sum1        isp_wdr1_sum1; /* 0x08cc */
    volatile u_isp_wdr2_sum0        isp_wdr2_sum0; /* 0x08d0 */
    volatile u_isp_wdr2_sum1        isp_wdr2_sum1; /* 0x08d4 */
    volatile u_isp_wdr3_sum0        isp_wdr3_sum0; /* 0x08d8 */
    volatile u_isp_wdr3_sum1        isp_wdr3_sum1; /* 0x08dc */
    volatile u_isp_demosaic_sum0    isp_demosaic_sum0; /* 0x08e0 */
    volatile u_isp_demosaic_sum1    isp_demosaic_sum1; /* 0x08e4 */
    volatile unsigned int           reserved_42[457]; /* 0x08e8~0x1008, reserved 457 * 4 bytes */
    volatile u_isp_blc_version      isp_blc_version; /* 0x100c */
    volatile u_isp_blc_offset1      isp_blc_offset1; /* 0x1010 */
    volatile u_isp_blc_offset2      isp_blc_offset2; /* 0x1014 */
    volatile unsigned int           reserved_43[123]; /* 0x1018~0x1200, reserved 123 * 4 bytes */
    volatile u_isp_wb_blc_cfg       isp_wb_blc_cfg; /* 0x1204 */
    volatile unsigned int           reserved_44; /* 0x1208 */
    volatile u_isp_wb_version       isp_wb_version; /* 0x120c */
    volatile u_isp_wb_gain1         isp_wb_gain1; /* 0x1210 */
    volatile u_isp_wb_gain2         isp_wb_gain2; /* 0x1214 */
    volatile u_isp_wb_blc_offset1   isp_wb_blc_offset1; /* 0x1218 */
    volatile u_isp_wb_blc_offset2   isp_wb_blc_offset2; /* 0x121c */
    volatile u_isp_wb_clip_value    isp_wb_clip_value; /* 0x1220 */
    volatile u_isp_wb_gain3         isp_wb_gain3; /* 0x1224 */
    volatile u_isp_wb_blc_offset3   isp_wb_blc_offset3; /* 0x1228 */
    volatile unsigned int           reserved_45[54]; /* 0x122c~0x1300, reserved 54 * 4 bytes */
    volatile u_isp_dg_blc_cfg       isp_dg_blc_cfg; /* 0x1304 */
    volatile unsigned int           reserved_46; /* 0x1308 */
    volatile u_isp_dg_version       isp_dg_version; /* 0x130c */
    volatile u_isp_dg_gain1         isp_dg_gain1; /* 0x1310 */
    volatile u_isp_dg_gain2         isp_dg_gain2; /* 0x1314 */
    volatile u_isp_dg_blc_offset1   isp_dg_blc_offset1; /* 0x1318 */
    volatile u_isp_dg_blc_offset2   isp_dg_blc_offset2; /* 0x131c */
    volatile u_isp_dg_clip_value    isp_dg_clip_value; /* 0x1320 */
    volatile u_isp_dg_gain3         isp_dg_gain3; /* 0x1324 */
    volatile u_isp_dg_blc_offset3   isp_dg_blc_offset3; /* 0x1328 */
    volatile unsigned int           reserved_47[53]; /* 0x132c~0x13fc, reserved 53 * 4 bytes */
    volatile u_isp_cc_cfg           isp_cc_cfg; /* 0x1400 */
    volatile unsigned int           reserved_48[2]; /* 0x1404~0x1408, reserved 2 * 4 bytes */
    volatile u_isp_cc_version       isp_cc_version; /* 0x140c */
    volatile u_isp_cc_coef0         isp_cc_coef0; /* 0x1410 */
    volatile u_isp_cc_coef1         isp_cc_coef1; /* 0x1414 */
    volatile u_isp_cc_coef2         isp_cc_coef2; /* 0x1418 */
    volatile u_isp_cc_coef3         isp_cc_coef3; /* 0x141c */
    volatile u_isp_cc_coef4         isp_cc_coef4; /* 0x1420 */
    volatile unsigned int           reserved_49[3]; /* 0x1424~0x142c, reserved 3 * 4 bytes */
    volatile u_isp_cc_rcv_ctrl0     isp_cc_rcv_ctrl0; /* 0x1430 */
    volatile u_isp_cc_rcv_ctrl1     isp_cc_rcv_ctrl1; /* 0x1434 */
    volatile u_isp_cc_lumafact      isp_cc_lumafact; /* 0x1438 */
    volatile u_isp_cc_colortone_rb_gain   isp_cc_colortone_rb_gain; /* 0x143c */
    volatile u_isp_cc_colortone_g_gain   isp_cc_colortone_g_gain; /* 0x1440 */
    volatile unsigned int           reserved_50[8]; /* 0x1444~0x1460, reserved 8 * 4 bytes */
    volatile u_isp_cc_in_dc0        isp_cc_in_dc0; /* 0x1464 */
    volatile u_isp_cc_in_dc1        isp_cc_in_dc1; /* 0x1468 */
    volatile u_isp_cc_in_dc2        isp_cc_in_dc2; /* 0x146c */
    volatile u_isp_cc_out_dc0       isp_cc_out_dc0; /* 0x1470 */
    volatile u_isp_cc_out_dc1       isp_cc_out_dc1; /* 0x1474 */
    volatile u_isp_cc_out_dc2       isp_cc_out_dc2; /* 0x1478 */
    volatile unsigned int           reserved_51[36]; /* 0x147c~0x1508, reserved 36 * 4 bytes */
    volatile u_isp_csc_version      isp_csc_version; /* 0x150c */
    volatile u_isp_csc_coef0        isp_csc_coef0; /* 0x1510 */
    volatile u_isp_csc_coef1        isp_csc_coef1; /* 0x1514 */
    volatile u_isp_csc_coef2        isp_csc_coef2; /* 0x1518 */
    volatile u_isp_csc_coef3        isp_csc_coef3; /* 0x151c */
    volatile u_isp_csc_coef4        isp_csc_coef4; /* 0x1520 */
    volatile unsigned int           reserved_52[3]; /* 0x1524~0x152c, reserved 3 * 4 bytes */
    volatile u_isp_csc_in_dc0       isp_csc_in_dc0; /* 0x1530 */
    volatile u_isp_csc_in_dc1       isp_csc_in_dc1; /* 0x1534 */
    volatile unsigned int           reserved_53[2]; /* 0x1538~0x153c, reserved 2 * 4 bytes */
    volatile u_isp_csc_out_dc0      isp_csc_out_dc0; /* 0x1540 */
    volatile u_isp_csc_out_dc1      isp_csc_out_dc1; /* 0x1544 */
    volatile unsigned int           reserved_54[177]; /* 0x1548~0x1808, reserved 177 * 4 bytes */
    volatile u_isp_hcds_version     isp_hcds_version; /* 0x180c */
    volatile u_isp_hcds_coefh0      isp_hcds_coefh0; /* 0x1810 */
    volatile u_isp_hcds_coefh1      isp_hcds_coefh1; /* 0x1814 */
    volatile u_isp_hcds_coefh2      isp_hcds_coefh2; /* 0x1818 */
    volatile u_isp_hcds_coefh3      isp_hcds_coefh3; /* 0x181c */
    volatile unsigned int           reserved_55[571]; /* 0x1820~0x2108, reserved 571 * 4 bytes */
    volatile u_isp_awb_version      isp_awb_version; /* 0x210c */
    volatile u_isp_awb_zone         isp_awb_zone; /* 0x2110 */
    volatile u_isp_awb_bitmove      isp_awb_bitmove; /* 0x2114 */
    volatile u_isp_awb_thd_min      isp_awb_thd_min; /* 0x2118 */
    volatile u_isp_awb_thd_max      isp_awb_thd_max; /* 0x211c */
    volatile u_isp_awb_cr_mm        isp_awb_cr_mm; /* 0x2120 */
    volatile u_isp_awb_cb_mm        isp_awb_cb_mm; /* 0x2124 */
    volatile u_isp_awb_offset_comp   isp_awb_offset_comp; /* 0x2128 */
    volatile unsigned int           reserved_56[5]; /* 0x212c~0x213c, reserved 5 * 4 bytes */
    volatile u_isp_awb_avg_r        isp_awb_avg_r; /* 0x2140 */
    volatile u_isp_awb_avg_g        isp_awb_avg_g; /* 0x2144 */
    volatile u_isp_awb_avg_b        isp_awb_avg_b; /* 0x2148 */
    volatile u_isp_awb_cnt_all      isp_awb_cnt_all; /* 0x214c */
    volatile unsigned int           reserved_57[4]; /* 0x2150~0x215c, reserved 4 * 4 bytes */
    volatile u_isp_awb_stt_bst      isp_awb_stt_bst; /* 0x2160 */
    volatile u_isp_awb_stt_abn      isp_awb_stt_abn; /* 0x2164 */
    volatile unsigned int           reserved_58[3]; /* 0x2168~0x2170, reserved 3 * 4 bytes */
    volatile u_isp_awb_crop_pos     isp_awb_crop_pos; /* 0x2174 */
    volatile u_isp_awb_crop_outsize   isp_awb_crop_outsize; /* 0x2178 */
    volatile unsigned int           reserved_59; /* 0x217c */
    volatile u_isp_awb_stat_waddr   isp_awb_stat_waddr; /* 0x2180 */
    volatile u_isp_awb_stat_wdata   isp_awb_stat_wdata; /* 0x2184 */
    volatile u_isp_awb_stat_raddr   isp_awb_stat_raddr; /* 0x2188 */
    volatile u_isp_awb_stat_rdata   isp_awb_stat_rdata; /* 0x218c */
    volatile unsigned int           reserved_60[28]; /* 0x2190~0x21fc, reserved 28 * 4 bytes */
    volatile u_isp_af_cfg           isp_af_cfg; /* 0x2200 */
    volatile unsigned int           reserved_61[2]; /* 0x2204~0x2208, reserved 2 * 4 bytes */
    volatile u_isp_af_version       isp_af_version; /* 0x220c */
    volatile u_isp_af_zone          isp_af_zone; /* 0x2210 */
    volatile u_isp_af_crop_start    isp_af_crop_start; /* 0x2214 */
    volatile u_isp_af_crop_size     isp_af_crop_size; /* 0x2218 */
    volatile u_isp_af_mean_thres    isp_af_mean_thres; /* 0x221c */
    volatile u_isp_af_iirg0         isp_af_iirg0; /* 0x2220 */
    volatile u_isp_af_iirg1         isp_af_iirg1; /* 0x2224 */
    volatile u_isp_af_iirg2         isp_af_iirg2; /* 0x2228 */
    volatile u_isp_af_iirg3         isp_af_iirg3; /* 0x222c */
    volatile u_isp_af_iirg4         isp_af_iirg4; /* 0x2230 */
    volatile u_isp_af_iirg5         isp_af_iirg5; /* 0x2234 */
    volatile u_isp_af_iirg6         isp_af_iirg6; /* 0x2238 */
    volatile u_isp_af_iirpl         isp_af_iirpl; /* 0x223c */
    volatile u_isp_af_shift         isp_af_shift; /* 0x2240 */
    volatile unsigned int           reserved_62[3]; /* 0x2244~0x224c, reserved 3 * 4 bytes */
    volatile u_isp_af_firh0         isp_af_firh0; /* 0x2250 */
    volatile u_isp_af_firh1         isp_af_firh1; /* 0x2254 */
    volatile u_isp_af_firh2         isp_af_firh2; /* 0x2258 */
    volatile u_isp_af_firh3         isp_af_firh3; /* 0x225c */
    volatile u_isp_af_firh4         isp_af_firh4; /* 0x2260 */
    volatile u_isp_af_stt_bst       isp_af_stt_bst; /* 0x2264 */
    volatile u_isp_af_stt_abn       isp_af_stt_abn; /* 0x2268 */
    volatile unsigned int           reserved_63[3]; /* 0x226c~0x2274, reserved 3 * 4 bytes */
    volatile u_isp_af_acc_shift     isp_af_acc_shift; /* 0x2278 */
    volatile u_isp_af_cnt_shift     isp_af_cnt_shift; /* 0x227c */
    volatile unsigned int           reserved_64[2]; /* 0x2280~0x2284, reserved 2 * 4 bytes */
    volatile u_isp_af_stat_raddr    isp_af_stat_raddr; /* 0x2288 */
    volatile u_isp_af_stat_rdata    isp_af_stat_rdata; /* 0x228c */
    volatile unsigned int           reserved_65[28]; /* 0x2290~0x22fc, reserved 28 * 4 bytes */
    volatile u_isp_af_iirthre       isp_af_iirthre; /* 0x2300 */
    volatile u_isp_af_iirgain       isp_af_iirgain; /* 0x2304 */
    volatile u_isp_af_iirslope      isp_af_iirslope; /* 0x2308 */
    volatile u_isp_af_iirdilate     isp_af_iirdilate; /* 0x230c */
    volatile u_isp_af_firthre       isp_af_firthre; /* 0x2310 */
    volatile u_isp_af_firgain       isp_af_firgain; /* 0x2314 */
    volatile u_isp_af_firslope      isp_af_firslope; /* 0x2318 */
    volatile unsigned int           reserved_66; /* 0x231c */
    volatile u_isp_af_iirthre_coring   isp_af_iirthre_coring; /* 0x2320 */
    volatile u_isp_af_iirpeak_coring   isp_af_iirpeak_coring; /* 0x2324 */
    volatile u_isp_af_iirslope_coring   isp_af_iirslope_coring; /* 0x2328 */
    volatile unsigned int           reserved_67; /* 0x232c */
    volatile u_isp_af_firthre_coring   isp_af_firthre_coring; /* 0x2330 */
    volatile u_isp_af_firpeak_coring   isp_af_firpeak_coring; /* 0x2334 */
    volatile u_isp_af_firslope_coring   isp_af_firslope_coring; /* 0x2338 */
    volatile unsigned int           reserved_68; /* 0x233c */
    volatile u_isp_af_highlight       isp_af_highlight; /* 0x2340 */
    volatile u_isp_af_offset        isp_af_offset; /* 0x2344 */
    volatile unsigned int           reserved_69[113]; /* 0x2348~0x2508, reserved 113 * 4 bytes */
    volatile u_isp_la_version       isp_la_version; /* 0x250c */
    volatile u_isp_la_zone          isp_la_zone; /* 0x2510 */
    volatile unsigned int           reserved_70[11]; /* 0x2514~0x253c, reserved 11 * 4 bytes */
    volatile u_isp_la_bitmove       isp_la_bitmove; /* 0x2540 */
    volatile u_isp_la_offset_r      isp_la_offset_r; /* 0x2544 */
    volatile u_isp_la_offset_gr     isp_la_offset_gr; /* 0x2548 */
    volatile u_isp_la_offset_gb     isp_la_offset_gb; /* 0x254c */
    volatile u_isp_la_offset_b      isp_la_offset_b; /* 0x2550 */
    volatile unsigned int           reserved_71[3]; /* 0x2554~0x255c, reserved 3 * 4 bytes */
    volatile u_isp_la_gamma_limit   isp_la_gamma_limit; /* 0x2560 */
    volatile u_isp_la_crop_pos      isp_la_crop_pos; /* 0x2564 */
    volatile u_isp_la_crop_outsize   isp_la_crop_outsize; /* 0x2568 */
    volatile unsigned int           reserved_72; /* 0x256c */
    volatile u_isp_la_stt_bst       isp_la_stt_bst; /* 0x2570 */
    volatile u_isp_la_stt_abn       isp_la_stt_abn; /* 0x2574 */
    volatile unsigned int           reserved_73[6]; /* 0x2578~0x258c, reserved 6 * 4 bytes */
    volatile u_isp_la_aver_waddr    isp_la_aver_waddr; /* 0x2590 */
    volatile u_isp_la_aver_wdata    isp_la_aver_wdata; /* 0x2594 */
    volatile u_isp_la_aver_raddr    isp_la_aver_raddr; /* 0x2598 */
    volatile u_isp_la_aver_rdata    isp_la_aver_rdata; /* 0x259c */
    volatile unsigned int           reserved_74[91]; /* 0x25a0~0x2708, reserved 91 * 4 bytes */
    volatile u_isp_awblsc_version   isp_awblsc_version; /* 0x270c */
    volatile u_isp_awblsc_zone      isp_awblsc_zone; /* 0x2710 */
    volatile u_isp_awblsc_bitmove   isp_awblsc_bitmove; /* 0x2714 */
    volatile u_isp_awblsc_thd_min   isp_awblsc_thd_min; /* 0x2718 */
    volatile u_isp_awblsc_thd_max   isp_awblsc_thd_max; /* 0x271c */
    volatile u_isp_awblsc_cr_mm     isp_awblsc_cr_mm; /* 0x2720 */
    volatile u_isp_awblsc_cb_mm     isp_awblsc_cb_mm; /* 0x2724 */
    volatile u_isp_awblsc_offset_comp   isp_awblsc_offset_comp; /* 0x2728 */
    volatile unsigned int           reserved_75[5]; /* 0x272c~0x273c, reserved 5 * 4 bytes */
    volatile u_isp_awblsc_avg_r     isp_awblsc_avg_r; /* 0x2740 */
    volatile u_isp_awblsc_avg_g     isp_awblsc_avg_g; /* 0x2744 */
    volatile u_isp_awblsc_avg_b     isp_awblsc_avg_b; /* 0x2748 */
    volatile u_isp_awblsc_cnt_all   isp_awblsc_cnt_all; /* 0x274c */
    volatile unsigned int           reserved_76[4]; /* 0x2750~0x275c, reserved 4 * 4 bytes */
    volatile u_isp_awblsc_stt_bst   isp_awblsc_stt_bst; /* 0x2760 */
    volatile u_isp_awblsc_stt_abn   isp_awblsc_stt_abn; /* 0x2764 */
    volatile unsigned int           reserved_77[3]; /* 0x2768~0x2770, reserved 3 * 4 bytes */
    volatile u_isp_awblsc_crop_pos   isp_awblsc_crop_pos; /* 0x2774 */
    volatile u_isp_awblsc_crop_outsize   isp_awblsc_crop_outsize; /* 0x2778 */
    volatile unsigned int           reserved_78; /* 0x277c */
    volatile u_isp_awblsc_stat_waddr   isp_awblsc_stat_waddr; /* 0x2780 */
    volatile u_isp_awblsc_stat_wdata   isp_awblsc_stat_wdata; /* 0x2784 */
    volatile u_isp_awblsc_stat_raddr   isp_awblsc_stat_raddr; /* 0x2788 */
    volatile u_isp_awblsc_stat_rdata   isp_awblsc_stat_rdata; /* 0x278c */
    volatile unsigned int           reserved_79[31]; /* 0x2790~0x2808, reserved 31 * 4 bytes */

    volatile u_isp_ae_version       isp_ae_version; /* 0x280c */
    volatile u_isp_ae_zone          isp_ae_zone; /* 0x2810 */
    volatile u_isp_ae_skip_crg      isp_ae_skip_crg; /* 0x2814 */
    volatile u_isp_ae_total_stat    isp_ae_total_stat; /* 0x2818 */
    volatile u_isp_ae_count_stat    isp_ae_count_stat; /* 0x281c */
    volatile u_isp_ae_total_r_aver   isp_ae_total_r_aver; /* 0x2820 */
    volatile u_isp_ae_total_gr_aver   isp_ae_total_gr_aver; /* 0x2824 */
    volatile u_isp_ae_total_gb_aver   isp_ae_total_gb_aver; /* 0x2828 */
    volatile u_isp_ae_total_b_aver   isp_ae_total_b_aver; /* 0x282c */
    volatile u_isp_ae_hist_high     isp_ae_hist_high; /* 0x2830 */
    volatile u_isp_ae_total_ir_aver   isp_ae_total_ir_aver; /* 0x2834 */
    volatile unsigned int           reserved_80[2]; /* 0x2838~0x283c, reserved 2 * 4 bytes */
    volatile u_isp_ae_bitmove       isp_ae_bitmove; /* 0x2840 */
    volatile u_isp_ae_offset_r_gr   isp_ae_offset_r_gr; /* 0x2844 */
    volatile u_isp_ae_offset_gb_b   isp_ae_offset_gb_b; /* 0x2848 */
    volatile unsigned int           reserved_81[6]; /* 0x284c~0x2860, reserved 6 * 4 bytes */
    volatile u_isp_ae_lut_update    isp_ae_lut_update; /* 0x2864 */
    volatile u_isp_ae_stt_bst       isp_ae_stt_bst; /* 0x2868 */
    volatile unsigned int           reserved_82[2]; /* 0x286c~0x2870, reserved 2 * 4 bytes */
    volatile u_isp_ae_crop_pos      isp_ae_crop_pos; /* 0x2874 */
    volatile u_isp_ae_crop_outsize   isp_ae_crop_outsize; /* 0x2878 */
    volatile unsigned int           reserved_83; /* 0x287c */
    volatile u_isp_ae_hist_waddr    isp_ae_hist_waddr; /* 0x2880 */
    volatile u_isp_ae_hist_wdata    isp_ae_hist_wdata; /* 0x2884 */
    volatile u_isp_ae_hist_raddr    isp_ae_hist_raddr; /* 0x2888 */
    volatile u_isp_ae_hist_rdata    isp_ae_hist_rdata; /* 0x288c */
    volatile u_isp_ae_aver_r_gr_waddr   isp_ae_aver_r_gr_waddr; /* 0x2890 */
    volatile u_isp_ae_aver_r_gr_wdata   isp_ae_aver_r_gr_wdata; /* 0x2894 */
    volatile u_isp_ae_aver_r_gr_raddr   isp_ae_aver_r_gr_raddr; /* 0x2898 */
    volatile u_isp_ae_aver_r_gr_rdata   isp_ae_aver_r_gr_rdata; /* 0x289c */
    volatile u_isp_ae_aver_gb_b_waddr   isp_ae_aver_gb_b_waddr; /* 0x28a0 */
    volatile u_isp_ae_aver_gb_b_wdata   isp_ae_aver_gb_b_wdata; /* 0x28a4 */
    volatile u_isp_ae_aver_gb_b_raddr   isp_ae_aver_gb_b_raddr; /* 0x28a8 */
    volatile u_isp_ae_aver_gb_b_rdata   isp_ae_aver_gb_b_rdata; /* 0x28ac */
    volatile u_isp_ae_weight_waddr   isp_ae_weight_waddr; /* 0x28b0 */
    volatile u_isp_ae_weight_wdata   isp_ae_weight_wdata; /* 0x28b4 */
    volatile u_isp_ae_weight_raddr   isp_ae_weight_raddr; /* 0x28b8 */
    volatile u_isp_ae_weight_rdata   isp_ae_weight_rdata; /* 0x28bc */
    volatile u_isp_ae_ir_hist_waddr   isp_ae_ir_hist_waddr; /* 0x28c0 */
    volatile u_isp_ae_ir_hist_wdata   isp_ae_ir_hist_wdata; /* 0x28c4 */
    volatile u_isp_ae_ir_hist_raddr   isp_ae_ir_hist_raddr; /* 0x28c8 */
    volatile u_isp_ae_ir_hist_rdata   isp_ae_ir_hist_rdata; /* 0x28cc */
    volatile u_isp_ae_aver_ir_waddr   isp_ae_aver_ir_waddr; /* 0x28d0 */
    volatile u_isp_ae_aver_ir_wdata   isp_ae_aver_ir_wdata; /* 0x28d4 */
    volatile u_isp_ae_aver_ir_raddr   isp_ae_aver_ir_raddr; /* 0x28d8 */
    volatile u_isp_ae_aver_ir_rdata   isp_ae_aver_ir_rdata; /* 0x28dc */
    volatile u_isp_ae_ai_hist_waddr   isp_ae_ai_hist_waddr; /* 0x28e0 */
    volatile u_isp_ae_ai_hist_wdata   isp_ae_ai_hist_wdata; /* 0x28e4 */
    volatile u_isp_ae_ai_hist_raddr   isp_ae_ai_hist_raddr; /* 0x28e8 */
    volatile u_isp_ae_ai_hist_rdata   isp_ae_ai_hist_rdata; /* 0x28ec */
    volatile u_isp_ae_ai_aver_waddr   isp_ae_ai_aver_waddr; /* 0x28f0 */
    volatile u_isp_ae_ai_aver_wdata   isp_ae_ai_aver_wdata; /* 0x28f4 */
    volatile u_isp_ae_ai_aver_raddr   isp_ae_ai_aver_raddr; /* 0x28f8 */
    volatile u_isp_ae_ai_aver_rdata   isp_ae_ai_aver_rdata; /* 0x28fc */
    volatile u_isp_ae_smart_ctrl    isp_ae_smart_ctrl; /* 0x2900 */
    volatile u_isp_ae_smart0_pos    isp_ae_smart0_pos; /* 0x2904 */
    volatile u_isp_ae_smart0_size   isp_ae_smart0_size; /* 0x2908 */
    volatile u_isp_ae_smart1_pos    isp_ae_smart1_pos; /* 0x290c */
    volatile u_isp_ae_smart1_size   isp_ae_smart1_size; /* 0x2910 */
    volatile u_isp_ae_smart2_pos    isp_ae_smart2_pos; /* 0x2914 */
    volatile u_isp_ae_smart2_size   isp_ae_smart2_size; /* 0x2918 */
    volatile u_isp_ae_smart3_pos    isp_ae_smart3_pos; /* 0x291c */
    volatile u_isp_ae_smart3_size   isp_ae_smart3_size; /* 0x2920 */
    volatile u_isp_ae_smart4_pos    isp_ae_smart4_pos; /* 0x2924 */
    volatile u_isp_ae_smart4_size   isp_ae_smart4_size; /* 0x2928 */
    volatile u_isp_ae_smart5_pos    isp_ae_smart5_pos; /* 0x292c */
    volatile u_isp_ae_smart5_size   isp_ae_smart5_size; /* 0x2930 */
    volatile u_isp_ae_smart6_pos    isp_ae_smart6_pos; /* 0x2934 */
    volatile u_isp_ae_smart6_size   isp_ae_smart6_size; /* 0x2938 */
    volatile u_isp_ae_smart7_pos    isp_ae_smart7_pos; /* 0x293c */
    volatile u_isp_ae_smart7_size   isp_ae_smart7_size; /* 0x2940 */
    volatile u_isp_ae_smart8_pos    isp_ae_smart8_pos; /* 0x2944 */
    volatile u_isp_ae_smart8_size   isp_ae_smart8_size; /* 0x2948 */
    volatile u_isp_ae_smart9_pos    isp_ae_smart9_pos; /* 0x294c */
    volatile u_isp_ae_smart9_size   isp_ae_smart9_size; /* 0x2950 */
    volatile u_isp_ae_smart10_pos   isp_ae_smart10_pos; /* 0x2954 */
    volatile u_isp_ae_smart10_size   isp_ae_smart10_size; /* 0x2958 */
    volatile u_isp_ae_smart11_pos   isp_ae_smart11_pos; /* 0x295c */
    volatile u_isp_ae_smart11_size   isp_ae_smart11_size; /* 0x2960 */
    volatile u_isp_ae_smart12_pos   isp_ae_smart12_pos; /* 0x2964 */
    volatile u_isp_ae_smart12_size   isp_ae_smart12_size; /* 0x2968 */
    volatile u_isp_ae_smart13_pos   isp_ae_smart13_pos; /* 0x296c */
    volatile u_isp_ae_smart13_size   isp_ae_smart13_size; /* 0x2970 */
    volatile u_isp_ae_smart14_pos   isp_ae_smart14_pos; /* 0x2974 */
    volatile u_isp_ae_smart14_size   isp_ae_smart14_size; /* 0x2978 */
    volatile u_isp_ae_smart15_pos   isp_ae_smart15_pos; /* 0x297c */
    volatile u_isp_ae_smart15_size   isp_ae_smart15_size; /* 0x2980 */

    volatile unsigned int           reserved_84[415]; /* 0x2984~0x2ffc, reserved 415 * 4 bytes */
    volatile u_isp_lsc_stt2lut_cfg   isp_lsc_stt2lut_cfg; /* 0x3000 */
    volatile u_isp_lsc_stt2lut_regnew   isp_lsc_stt2lut_regnew; /* 0x3004 */
    volatile u_isp_lsc_stt2lut_abn   isp_lsc_stt2lut_abn; /* 0x3008 */
    volatile unsigned int           reserved_85; /* 0x300c */
    volatile u_isp_lsc_winnum       isp_lsc_winnum; /* 0x3010 */
    volatile u_isp_lsc_winx0_23     isp_lsc_winx_0_23[24]; /* 0x3014 ~ 0x3070, reserved 24 * 4 bytes */
    volatile u_isp_lsc_width_offset   isp_lsc_width_offset; /* 0x3074 */
    volatile u_isp_lsc_mesh         isp_lsc_mesh; /* 0x3078 */
    volatile u_isp_lsc_weight       isp_lsc_weight; /* 0x307c */
    volatile u_isp_lsc_rgain_waddr   isp_lsc_rgain_waddr; /* 0x3080 */
    volatile u_isp_lsc_rgain_wdata   isp_lsc_rgain_wdata; /* 0x3084 */
    volatile u_isp_lsc_rgain_raddr   isp_lsc_rgain_raddr; /* 0x3088 */
    volatile u_isp_lsc_rgain_rdata   isp_lsc_rgain_rdata; /* 0x308c */
    volatile u_isp_lsc_grgain_waddr   isp_lsc_grgain_waddr; /* 0x3090 */
    volatile u_isp_lsc_grgain_wdata   isp_lsc_grgain_wdata; /* 0x3094 */
    volatile u_isp_lsc_grgain_raddr   isp_lsc_grgain_raddr; /* 0x3098 */
    volatile u_isp_lsc_grgain_rdata   isp_lsc_grgain_rdata; /* 0x309c */
    volatile u_isp_lsc_bgain_waddr   isp_lsc_bgain_waddr; /* 0x30a0 */
    volatile u_isp_lsc_bgain_wdata   isp_lsc_bgain_wdata; /* 0x30a4 */
    volatile u_isp_lsc_bgain_raddr   isp_lsc_bgain_raddr; /* 0x30a8 */
    volatile u_isp_lsc_bgain_rdata   isp_lsc_bgain_rdata; /* 0x30ac */
    volatile u_isp_lsc_gbgain_waddr   isp_lsc_gbgain_waddr; /* 0x30b0 */
    volatile u_isp_lsc_gbgain_wdata   isp_lsc_gbgain_wdata; /* 0x30b4 */
    volatile u_isp_lsc_gbgain_raddr   isp_lsc_gbgain_raddr; /* 0x30b8 */
    volatile u_isp_lsc_gbgain_rdata   isp_lsc_gbgain_rdata; /* 0x30bc */
    volatile unsigned int           reserved_86[20]; /* 0x30c0~0x310c, reserved 20 * 4 bytes */
    volatile u_isp_lsc_winx24_31    isp_lsc_winx_24_31[8]; /* 0x3110 ~ 0x312c, reserved 8 * 4 bytes */
    volatile u_isp_lsc_winy         isp_lsc_winy[16]; /* 0x3130 ~ 0x316c, reserved 16 * 4 bytes */
    volatile u_isp_lsc_blcen        isp_lsc_blcen; /* 0x3170 */
    volatile u_isp_lsc_blc0         isp_lsc_blc0; /* 0x3174 */
    volatile u_isp_lsc_blc1         isp_lsc_blc1; /* 0x3178 */
    volatile unsigned int           reserved_87[33]; /* 0x317c~0x31fc, reserved 33 * 4 bytes */
    volatile u_isp_gcac_cfg         isp_gcac_cfg; /* 0x3200 */
    volatile u_isp_gcac_edge_fac    isp_gcac_edge_fac; /* 0x3204 */
    volatile u_isp_gcac_edge_thd    isp_gcac_edge_thd; /* 0x3208 */
    volatile u_isp_gcac_lamda_thd   isp_gcac_lamda_thd; /* 0x320c */
    volatile u_isp_gcac_lamda_mul   isp_gcac_lamda_mul; /* 0x3210 */
    volatile u_isp_gcac_edge_str_thd   isp_gcac_edge_str_thd; /* 0x3214 */
    volatile u_isp_gcac_crcb_adjust   isp_gcac_crcb_adjust; /* 0x3218 */
    volatile u_isp_gcac_calw_thd    isp_gcac_calw_thd; /* 0x321c */
    volatile u_isp_gcac_var_thd     isp_gcac_var_thd; /* 0x3220 */
    volatile u_isp_gcac_purple      isp_gcac_purple; /* 0x3224 */
    volatile u_isp_gcac_cbcr_ratio_limit   isp_gcac_cbcr_ratio_limit; /* 0x3228 */
    volatile u_isp_gcac_cbcr_ratio_limit2   isp_gcac_cbcr_ratio_limit2; /* 0x322c */
    volatile unsigned int           reserved_88[885]; /* 0x3230~0x4000, reserved 885 * 4 bytes */
    volatile u_isp_demosaic_cfg1    isp_demosaic_cfg1; /* 0x4004 */
    volatile unsigned int           reserved_89[2]; /* 0x4008~0x400c, reserved 2 * 4 bytes */
    volatile u_isp_demosaic_coef0   isp_demosaic_coef0; /* 0x4010 */
    volatile unsigned int           reserved_90; /* 0x4014 */
    volatile u_isp_demosaic_coef2   isp_demosaic_coef2; /* 0x4018 */
    volatile u_isp_demosaic_coef3   isp_demosaic_coef3; /* 0x401c */
    volatile u_isp_demosaic_coef4   isp_demosaic_coef4; /* 0x4020 */
    volatile u_isp_demosaic_coef5   isp_demosaic_coef5; /* 0x4024 */
    volatile u_isp_demosaic_coef6   isp_demosaic_coef6; /* 0x4028 */
    volatile unsigned int           reserved_91; /* 0x402c */
    volatile u_isp_demosaic_sel     isp_demosaic_sel; /* 0x4030 */
    volatile unsigned int           reserved_92; /* 0x4034 */
    volatile u_isp_demosaic_lcac_cnt_thr   isp_demosaic_lcac_cnt_thr; /* 0x4038 */
    volatile u_isp_demosaic_lcac_luma_rb_thr   isp_demosaic_lcac_luma_rb_thr; /* 0x403c */
    volatile u_isp_demosaic_lcac_luma_g_thr   isp_demosaic_lcac_luma_g_thr; /* 0x4040 */
    volatile u_isp_demosaic_purple_var_thr   isp_demosaic_purple_var_thr; /* 0x4044 */
    volatile u_isp_demosaic_fake_cr_var_thr   isp_demosaic_fake_cr_var_thr; /* 0x4048 */
    volatile u_isp_demosaic_depurplectr   isp_demosaic_depurplectr; /* 0x404c */
    volatile u_isp_demosaic_lpf_coef   isp_demosaic_lpf_coef; /* 0x4050 */
    volatile u_isp_demosaic_g_intp_ctrl   isp_demosaic_g_intp_ctrl; /* 0x4054 */
    volatile u_isp_demosaic_cbcravgthld   isp_demosaic_cbcravgthld; /* 0x4058 */
    volatile unsigned int           reserved_93; /* 0x405c */
    volatile u_isp_demosaic_cc_hf_ratio   isp_demosaic_cc_hf_ratio; /* 0x4060 */
    volatile u_isp_demosaic_gvar_blend_thld   isp_demosaic_gvar_blend_thld; /* 0x4064 */
    volatile u_isp_demosaic_satu_thr   isp_demosaic_satu_thr; /* 0x4068 */
    volatile u_isp_demosaic_cbcr_ratio_limit   isp_demosaic_cbcr_ratio_limit; /* 0x406c */
    volatile u_isp_demosaic_fcr_gray_ratio   isp_demosaic_fcr_gray_ratio; /* 0x4070 */
    volatile u_isp_demosaic_fcr_sel   isp_demosaic_fcr_sel; /* 0x4074 */
    volatile u_isp_demosaic_cx_var_rate   isp_demosaic_cx_var_rate; /* 0x4078 */
    volatile unsigned int           reserved_94; /* 0x407c */
    volatile u_isp_demosaic_depurplut_waddr   isp_demosaic_depurplut_waddr; /* 0x4080 */
    volatile u_isp_demosaic_depurplut_wdata   isp_demosaic_depurplut_wdata; /* 0x4084 */
    volatile u_isp_demosaic_depurplut_raddr   isp_demosaic_depurplut_raddr; /* 0x4088 */
    volatile u_isp_demosaic_depurplut_rdata   isp_demosaic_depurplut_rdata; /* 0x408c */
    volatile unsigned int           reserved_95[32]; /* 0x4090~0x410c, reserved 32 * 4 bytes */
    volatile u_isp_demosaic_hf_intp_blur_th   isp_demosaic_hf_intp_blur_th; /* 0x4110 */
    volatile unsigned int           reserved_96[9]; /* 0x4114~0x4134, reserved 9 * 4 bytes */
    volatile u_isp_demosaic_cac_cbcr_thr   isp_demosaic_cac_cbcr_thr; /* 0x4138 */
    volatile u_isp_demosaic_cac_luma_high_cnt_thr   isp_demosaic_cac_luma_high_cnt_thr; /* 0x413c */
    volatile u_isp_demosaic_cac_cnt_cfg   isp_demosaic_cac_cnt_cfg; /* 0x4140 */
    volatile u_isp_demosaic_defcolor_coef   isp_demosaic_defcolor_coef; /* 0x4144 */
    volatile u_isp_demosaic_cac_bld_avg   isp_demosaic_cac_bld_avg; /* 0x4148 */
    volatile u_isp_demosaic_fcr_hf_thr   isp_demosaic_fcr_hf_thr; /* 0x414c */
    volatile u_isp_demosaic_desat_thr   isp_demosaic_desat_thr; /* 0x4150 */
    volatile u_isp_demosaic_desat_bldr   isp_demosaic_desat_bldr; /* 0x4154 */
    volatile u_isp_demosaic_desat_protect   isp_demosaic_desat_protect; /* 0x4158 */
    volatile u_isp_demosaic_hlc_th   isp_demosaic_hlc_th; /* 0x415c */
    volatile u_isp_demosaic_lut_update   isp_demosaic_lut_update; /* 0x4160 */
    volatile u_isp_demosaic_cbcr_ratio_limit2   isp_demosaic_cbcr_ratio_limit2; /* 0x4164 */
    volatile u_isp_demosaic_cbcr_ratio_mul   isp_demosaic_cbcr_ratio_mul; /* 0x4168 */
    volatile unsigned int           reserved_97[37]; /* 0x416c~0x41fc, reserved 37 * 4 bytes */
    volatile u_isp_bnrshp_stt2lut_cfg   isp_bnrshp_stt2lut_cfg; /* 0x4200 */
    volatile u_isp_bnrshp_stt2lut_regnew   isp_bnrshp_stt2lut_regnew; /* 0x4204 */
    volatile u_isp_bnrshp_stt2lut_abn   isp_bnrshp_stt2lut_abn; /* 0x4208 */
    volatile u_isp_bnrshp_cfg       isp_bnrshp_cfg; /* 0x420c */
    volatile u_isp_bnrshp_dir_mul   isp_bnrshp_dir_mul; /* 0x4210 */
    volatile u_isp_bnrshp_hsfcoef0   isp_bnrshp_hsfcoef0; /* 0x4214 */
    volatile u_isp_bnrshp_hsfcoef1   isp_bnrshp_hsfcoef1; /* 0x4218 */
    volatile u_isp_bnrshp_hsfcoef2   isp_bnrshp_hsfcoef2; /* 0x421c */
    volatile u_isp_bnrshp_hsfcoef3   isp_bnrshp_hsfcoef3; /* 0x4220 */
    volatile u_isp_bnrshp_hsfcoef4   isp_bnrshp_hsfcoef4; /* 0x4224 */
    volatile u_isp_bnrshp_hsfcoef5   isp_bnrshp_hsfcoef5; /* 0x4228 */
    volatile u_isp_bnrshp_hsfcoef6   isp_bnrshp_hsfcoef6; /* 0x422c */
    volatile u_isp_bnrshp_hsfcoef7   isp_bnrshp_hsfcoef7; /* 0x4230 */
    volatile u_isp_bnrshp_hsfcoef8   isp_bnrshp_hsfcoef8; /* 0x4234 */
    volatile u_isp_bnrshp_hsfcoef9   isp_bnrshp_hsfcoef9; /* 0x4238 */
    volatile u_isp_bnrshp_hsfcoef10   isp_bnrshp_hsfcoef10; /* 0x423c */
    volatile u_isp_bnrshp_hsfcoef11   isp_bnrshp_hsfcoef11; /* 0x4240 */
    volatile u_isp_bnrshp_hsfcoef12   isp_bnrshp_hsfcoef12; /* 0x4244 */
    volatile u_isp_bnrshp_hsfcoef13   isp_bnrshp_hsfcoef13; /* 0x4248 */
    volatile u_isp_bnrshp_hsfcoef14   isp_bnrshp_hsfcoef14; /* 0x424c */
    volatile u_isp_bnrshp_hsfcoef15   isp_bnrshp_hsfcoef15; /* 0x4250 */
    volatile u_isp_bnrshp_hsfcoef16   isp_bnrshp_hsfcoef16; /* 0x4254 */
    volatile u_isp_bnrshp_lpfcoef0   isp_bnrshp_lpfcoef0; /* 0x4258 */
    volatile u_isp_bnrshp_lpfcoef1   isp_bnrshp_lpfcoef1; /* 0x425c */
    volatile u_isp_bnrshp_hsfcoef17   isp_bnrshp_hsfcoef17; /* 0x4260 */
    volatile u_isp_bnrshp_luma_thr0   isp_bnrshp_luma_thr0; /* 0x4264 */
    volatile u_isp_bnrshp_luma_wgt0   isp_bnrshp_luma_wgt0; /* 0x4268 */
    volatile u_isp_bnrshp_luma_thr1   isp_bnrshp_luma_thr1; /* 0x426c */
    volatile u_isp_bnrshp_luma_wgt1   isp_bnrshp_luma_wgt1; /* 0x4270 */
    volatile u_isp_bnrshp_luma_mul   isp_bnrshp_luma_mul; /* 0x4274 */
    volatile u_isp_bnrshp_gain_ctrl   isp_bnrshp_gain_ctrl; /* 0x4278 */
    volatile u_isp_bnrshp_mf_cac    isp_bnrshp_mf_cac; /* 0x427c */
    volatile u_isp_bnrshp_gaind0_waddr   isp_bnrshp_gaind0_waddr; /* 0x4280 */
    volatile u_isp_bnrshp_gaind0_wdata   isp_bnrshp_gaind0_wdata; /* 0x4284 */
    volatile u_isp_bnrshp_gaind0_raddr   isp_bnrshp_gaind0_raddr; /* 0x4288 */
    volatile u_isp_bnrshp_gaind0_rdata   isp_bnrshp_gaind0_rdata; /* 0x428c */
    volatile u_isp_bnrshp_gaind1_waddr   isp_bnrshp_gaind1_waddr; /* 0x4290 */
    volatile u_isp_bnrshp_gaind1_wdata   isp_bnrshp_gaind1_wdata; /* 0x4294 */
    volatile u_isp_bnrshp_gaind1_raddr   isp_bnrshp_gaind1_raddr; /* 0x4298 */
    volatile u_isp_bnrshp_gaind1_rdata   isp_bnrshp_gaind1_rdata; /* 0x429c */
    volatile u_isp_bnrshp_gainud0_waddr   isp_bnrshp_gainud0_waddr; /* 0x42a0 */
    volatile u_isp_bnrshp_gainud0_wdata   isp_bnrshp_gainud0_wdata; /* 0x42a4 */
    volatile u_isp_bnrshp_gainud0_raddr   isp_bnrshp_gainud0_raddr; /* 0x42a8 */
    volatile u_isp_bnrshp_gainud0_rdata   isp_bnrshp_gainud0_rdata; /* 0x42ac */
    volatile u_isp_bnrshp_gainud1_waddr   isp_bnrshp_gainud1_waddr; /* 0x42b0 */
    volatile u_isp_bnrshp_gainud1_wdata   isp_bnrshp_gainud1_wdata; /* 0x42b4 */
    volatile u_isp_bnrshp_gainud1_raddr   isp_bnrshp_gainud1_raddr; /* 0x42b8 */
    volatile u_isp_bnrshp_gainud1_rdata   isp_bnrshp_gainud1_rdata; /* 0x42bc */
    volatile u_isp_bnrshp_luma_waddr   isp_bnrshp_luma_waddr; /* 0x42c0 */
    volatile u_isp_bnrshp_luma_wdata   isp_bnrshp_luma_wdata; /* 0x42c4 */
    volatile u_isp_bnrshp_luma_raddr   isp_bnrshp_luma_raddr; /* 0x42c8 */
    volatile u_isp_bnrshp_luma_rdata   isp_bnrshp_luma_rdata; /* 0x42cc */
    volatile unsigned int           reserved_98[12]; /* 0x42d0~0x42fc, reserved 12 * 4 bytes */
    volatile u_isp_bnrshp_neghf_wgt   isp_bnrshp_neghf_wgt; /* 0x4300 */
    volatile u_isp_bnrshp_neghf_mul   isp_bnrshp_neghf_mul; /* 0x4304 */
    volatile u_isp_bnrshp_lmtmf_lut0   isp_bnrshp_lmtmf_lut0; /* 0x4308 */
    volatile u_isp_bnrshp_lmtmf_lut1   isp_bnrshp_lmtmf_lut1; /* 0x430c */
    volatile u_isp_bnrshp_lmthf_lut0   isp_bnrshp_lmthf_lut0; /* 0x4310 */
    volatile u_isp_bnrshp_lmthf_lut1   isp_bnrshp_lmthf_lut1; /* 0x4314 */
    volatile u_isp_bnrshp_var_thd   isp_bnrshp_var_thd; /* 0x4318 */
    volatile u_isp_bnrshp_var_wgt   isp_bnrshp_var_wgt; /* 0x431c */
    volatile u_isp_bnrshp_sht_ctrl   isp_bnrshp_sht_ctrl; /* 0x4320 */
    volatile u_isp_bnrshp_sht_ctrl1   isp_bnrshp_sht_ctrl1; /* 0x4324 */
    volatile u_isp_bnrshp_losef_thr   isp_bnrshp_losef_thr; /* 0x4328 */
    volatile u_isp_bnrshp_losef_wgt   isp_bnrshp_losef_wgt; /* 0x432c */
    volatile u_isp_bnrshp_losef_mul   isp_bnrshp_losef_mul; /* 0x4330 */
    volatile u_isp_bnrshp_neghf_thr   isp_bnrshp_neghf_thr; /* 0x4334 */
    volatile u_isp_bnrshp_detail_sht   isp_bnrshp_detail_sht; /* 0x4338 */
    volatile u_isp_bnrshp_rly_g_thr   isp_bnrshp_rly_g_thr; /* 0x433c */
    volatile u_isp_bnrshp_rly_g_wgt   isp_bnrshp_rly_g_wgt; /* 0x4340 */
    volatile unsigned int           reserved_99[946]; /* 0x4344~0x5208, reserved 946 * 4 bytes */
    volatile u_isp_sharpen_version   isp_sharpen_version; /* 0x520c */
    volatile u_isp_sharpen_ctrl     isp_sharpen_ctrl; /* 0x5210 */
    volatile u_isp_sharpen_mhfthd   isp_sharpen_mhfthd; /* 0x5214 */
    volatile u_isp_sharpen_weakdetail   isp_sharpen_weakdetail; /* 0x5218 */
    volatile u_isp_sharpen_dirvar   isp_sharpen_dirvar; /* 0x521c */
    volatile u_isp_sharpen_dirdiff   isp_sharpen_dirdiff; /* 0x5220 */
    volatile u_isp_sharpen_lumawgt0   isp_sharpen_lumawgt0; /* 0x5224 */
    volatile u_isp_sharpen_lumawgt1   isp_sharpen_lumawgt1; /* 0x5228 */
    volatile u_isp_sharpen_lumawgt2   isp_sharpen_lumawgt2; /* 0x522c */
    volatile u_isp_sharpen_lumawgt3   isp_sharpen_lumawgt3; /* 0x5230 */
    volatile u_isp_sharpen_lumawgt4   isp_sharpen_lumawgt4; /* 0x5234 */
    volatile u_isp_sharpen_lumawgt5   isp_sharpen_lumawgt5; /* 0x5238 */
    volatile u_isp_sharpen_lumawgt6   isp_sharpen_lumawgt6; /* 0x523c */
    volatile u_isp_vcds_coefv       isp_vcds_coefv; /* 0x5240 */
    volatile u_isp_sharpen_shoot_amt   isp_sharpen_shoot_amt; /* 0x5244 */
    volatile u_isp_sharpen_shoot_maxchg   isp_sharpen_shoot_maxchg; /* 0x5248 */
    volatile u_isp_sharpen_shtvar   isp_sharpen_shtvar; /* 0x524c */
    volatile u_isp_sharpen_mot0     isp_sharpen_mot0; /* 0x5250 */
    volatile u_isp_sharpen_oshtvar   isp_sharpen_oshtvar; /* 0x5254 */
    volatile u_isp_sharpen_mot1     isp_sharpen_mot1; /* 0x5258 */
    volatile u_isp_sharpen_shtvar_mul   isp_sharpen_shtvar_mul; /* 0x525c */
    volatile u_isp_sharpen_oshtvardiff   isp_sharpen_oshtvardiff; /* 0x5260 */
    volatile unsigned int           reserved_100[2]; /* 0x5264~0x5268, reserved 2 * 4 bytes */
    volatile u_isp_sharpen_lmtmf0   isp_sharpen_lmtmf0; /* 0x526c */
    volatile u_isp_sharpen_lmtmf1   isp_sharpen_lmtmf1; /* 0x5270 */
    volatile u_isp_sharpen_lmthf0   isp_sharpen_lmthf0; /* 0x5274 */
    volatile u_isp_sharpen_lmthf1   isp_sharpen_lmthf1; /* 0x5278 */
    volatile unsigned int           reserved_101; /* 0x527c */
    volatile u_isp_sharpen_mhfgaind_waddr   isp_sharpen_mhfgaind_waddr; /* 0x5280 */
    volatile u_isp_sharpen_mhfgaind_wdata   isp_sharpen_mhfgaind_wdata; /* 0x5284 */
    volatile u_isp_sharpen_mhfgaind_raddr   isp_sharpen_mhfgaind_raddr; /* 0x5288 */
    volatile u_isp_sharpen_mhfgaind_rdata   isp_sharpen_mhfgaind_rdata; /* 0x528c */
    volatile u_isp_sharpen_mhfgainud_waddr   isp_sharpen_mhfgainud_waddr; /* 0x5290 */
    volatile u_isp_sharpen_mhfgainud_wdata   isp_sharpen_mhfgainud_wdata; /* 0x5294 */
    volatile u_isp_sharpen_mhfgainud_raddr   isp_sharpen_mhfgainud_raddr; /* 0x5298 */
    volatile u_isp_sharpen_mhfgainud_rdata   isp_sharpen_mhfgainud_rdata; /* 0x529c */
    volatile u_isp_sharpen_mhfmotgaind_waddr   isp_sharpen_mhfmotgaind_waddr; /* 0x52a0 */
    volatile u_isp_sharpen_mhfmotgaind_wdata   isp_sharpen_mhfmotgaind_wdata; /* 0x52a4 */
    volatile u_isp_sharpen_mhfmotgaind_raddr   isp_sharpen_mhfmotgaind_raddr; /* 0x52a8 */
    volatile u_isp_sharpen_mhfmotgaind_rdata   isp_sharpen_mhfmotgaind_rdata; /* 0x52ac */
    volatile u_isp_sharpen_mhfmotgainud_waddr   isp_sharpen_mhfmotgainud_waddr; /* 0x52b0 */
    volatile u_isp_sharpen_mhfmotgainud_wdata   isp_sharpen_mhfmotgainud_wdata; /* 0x52b4 */
    volatile u_isp_sharpen_mhfmotgainud_raddr   isp_sharpen_mhfmotgainud_raddr; /* 0x52b8 */
    volatile u_isp_sharpen_mhfmotgainud_rdata   isp_sharpen_mhfmotgainud_rdata; /* 0x52bc */
    volatile unsigned int           reserved_102[17]; /* 0x52c0~0x5300, reserved 17 * 4 bytes */
    volatile u_isp_sharpen_skin_u   isp_sharpen_skin_u; /* 0x5304 */
    volatile u_isp_sharpen_skin_v   isp_sharpen_skin_v; /* 0x5308 */
    volatile u_isp_sharpen_skin_cnt   isp_sharpen_skin_cnt; /* 0x530c */
    volatile u_isp_sharpen_skin_edge   isp_sharpen_skin_edge; /* 0x5310 */
    volatile u_isp_sharpen_skin_edgethd   isp_sharpen_skin_edgethd; /* 0x5314 */
    volatile unsigned int           reserved_103[2]; /* 0x5318~0x531c, reserved 2 * 4 bytes */
    volatile u_isp_sharpen_chrr_var   isp_sharpen_chrr_var; /* 0x5320 */
    volatile unsigned int           reserved_104; /* 0x5324 */
    volatile u_isp_sharpen_chrr_thd   isp_sharpen_chrr_thd; /* 0x5328 */
    volatile u_isp_sharpen_chrr_gain   isp_sharpen_chrr_gain; /* 0x532c */
    volatile u_isp_sharpen_chrg_mul   isp_sharpen_chrg_mul; /* 0x5330 */
    volatile u_isp_sharpen_chrg_sft   isp_sharpen_chrg_sft; /* 0x5334 */
    volatile unsigned int           reserved_105; /* 0x5338 */
    volatile u_isp_sharpen_chrg_thd   isp_sharpen_chrg_thd; /* 0x533c */
    volatile u_isp_sharpen_chrg_gain   isp_sharpen_chrg_gain; /* 0x5340 */
    volatile u_isp_sharpen_chrb_var   isp_sharpen_chrb_var; /* 0x5344 */
    volatile unsigned int           reserved_106; /* 0x5348 */
    volatile u_isp_sharpen_chrb_thd   isp_sharpen_chrb_thd; /* 0x534c */
    volatile u_isp_sharpen_chrb_gain   isp_sharpen_chrb_gain; /* 0x5350 */
    volatile u_isp_sharpen_gain_sft   isp_sharpen_gain_sft; /* 0x5354 */
    volatile u_isp_sharpen_shoot_maxgain   isp_sharpen_shoot_maxgain; /* 0x5358 */
    volatile u_isp_sharpen_detail_mul   isp_sharpen_detail_mul; /* 0x535c */
    volatile u_isp_sharpen_osht_detail   isp_sharpen_osht_detail; /* 0x5360 */
    volatile u_isp_sharpen_usht_detail   isp_sharpen_usht_detail; /* 0x5364 */
    volatile unsigned int           reserved_107[2]; /* 0x5368~0x536c, reserved 2 * 4 bytes */
    volatile u_isp_sharpen_stt2lut_cfg   isp_sharpen_stt2lut_cfg; /* 0x5370 */
    volatile u_isp_sharpen_stt2lut_regnew   isp_sharpen_stt2lut_regnew; /* 0x5374 */
    volatile u_isp_sharpen_stt2lut_abn   isp_sharpen_stt2lut_abn; /* 0x5378 */
    volatile unsigned int           reserved_108[26]; /* 0x537c~0x53e0, reserved 26 * 4 bytes */
    volatile u_isp_sharpen_dlpf_coef   isp_sharpen_dlpf_coef; /* 0x53e4 */
    volatile u_isp_sharpen_udlpf_coef   isp_sharpen_udlpf_coef; /* 0x53e8 */
    volatile u_isp_sharpen_udhsf_coef   isp_sharpen_udhsf_coef; /* 0x53ec */
    volatile u_isp_sharpen_dhsf_2dshift   isp_sharpen_dhsf_2dshift; /* 0x53f0 */
    volatile unsigned int           reserved_109[3]; /* 0x53f4~0x53fc, reserved 3 * 4 bytes */
    volatile u_isp_sharpen_dhsf_2dcoef0_03   isp_sharpen_dhsf_2dcoef0_03; /* 0x5400 */
    volatile u_isp_sharpen_dhsf_2dcoef0_47   isp_sharpen_dhsf_2dcoef0_47; /* 0x5404 */
    volatile u_isp_sharpen_dhsf_2dcoef0_811   isp_sharpen_dhsf_2dcoef0_811; /* 0x5408 */
    volatile u_isp_sharpen_dhsf_2dcoef0_1215   isp_sharpen_dhsf_2dcoef0_1215; /* 0x540c */
    volatile unsigned int           reserved_110[2]; /* 0x5410~0x5414, reserved 2 * 4 bytes */
    volatile u_isp_sharpen_dhsf_2dcoef1_03   isp_sharpen_dhsf_2dcoef1_03; /* 0x5418 */
    volatile u_isp_sharpen_dhsf_2dcoef1_47   isp_sharpen_dhsf_2dcoef1_47; /* 0x541c */
    volatile u_isp_sharpen_dhsf_2dcoef1_811   isp_sharpen_dhsf_2dcoef1_811; /* 0x5420 */
    volatile u_isp_sharpen_dhsf_2dcoef1_1215   isp_sharpen_dhsf_2dcoef1_1215; /* 0x5424 */
    volatile unsigned int           reserved_111[2]; /* 0x5428~0x542c, reserved 2 * 4 bytes */
    volatile u_isp_sharpen_dhsf_2dcoef2_03   isp_sharpen_dhsf_2dcoef2_03; /* 0x5430 */
    volatile u_isp_sharpen_dhsf_2dcoef2_47   isp_sharpen_dhsf_2dcoef2_47; /* 0x5434 */
    volatile u_isp_sharpen_dhsf_2dcoef2_811   isp_sharpen_dhsf_2dcoef2_811; /* 0x5438 */
    volatile u_isp_sharpen_dhsf_2dcoef2_1215   isp_sharpen_dhsf_2dcoef2_1215; /* 0x543c */
    volatile unsigned int           reserved_112[2]; /* 0x5440~0x5444, reserved 2 * 4 bytes */
    volatile u_isp_sharpen_dhsf_2dcoef3_03   isp_sharpen_dhsf_2dcoef3_03; /* 0x5448 */
    volatile u_isp_sharpen_dhsf_2dcoef3_47   isp_sharpen_dhsf_2dcoef3_47; /* 0x544c */
    volatile u_isp_sharpen_dhsf_2dcoef3_811   isp_sharpen_dhsf_2dcoef3_811; /* 0x5450 */
    volatile u_isp_sharpen_dhsf_2dcoef3_1215   isp_sharpen_dhsf_2dcoef3_1215; /* 0x5454 */
    volatile unsigned int           reserved_113[106]; /* 0x5458~0x55fc, reserved 106 * 4 bytes */
    volatile u_isp_nddm_cfg         isp_nddm_cfg; /* 0x5600 */
    volatile u_isp_nddm_gf_th       isp_nddm_gf_th; /* 0x5604 */
    volatile u_isp_nddm_awb_gf_cfg   isp_nddm_awb_gf_cfg; /* 0x5608 */
    volatile u_isp_nddm_fcr_gain    isp_nddm_fcr_gain; /* 0x560c */
    volatile u_isp_nddm_dm_bldrate   isp_nddm_dm_bldrate; /* 0x5610 */
    volatile u_isp_nddm_ehc_gray    isp_nddm_ehc_gray; /* 0x5614 */
    volatile u_isp_nddm_gf_lut_update   isp_nddm_gf_lut_update; /* 0x5618 */
    volatile u_isp_nddm_dither_cfg   isp_nddm_dither_cfg; /* 0x561c */
    volatile unsigned int           reserved_114[24]; /* 0x5620~0x567c, reserved 24 * 4 bytes */
    volatile u_isp_nddm_gf_lut_waddr   isp_nddm_gf_lut_waddr; /* 0x5680 */
    volatile u_isp_nddm_gf_lut_wdata   isp_nddm_gf_lut_wdata; /* 0x5684 */
    volatile u_isp_nddm_gf_lut_raddr   isp_nddm_gf_lut_raddr; /* 0x5688 */
    volatile u_isp_nddm_gf_lut_rdata   isp_nddm_gf_lut_rdata; /* 0x568c */
    volatile unsigned int           reserved_115[92]; /* 0x5690~0x57fc, reserved 92 * 4 bytes */
    volatile u_isp_bnr_cfg          isp_bnr_cfg; /* 0x5800 */
    volatile unsigned int           reserved_116[2]; /* 0x5804~0x5808, reserved 2 * 4 bytes */
    volatile u_isp_bnr_version      isp_bnr_version; /* 0x580c */
    volatile unsigned int           reserved_117[5]; /* 0x5810~0x5820, reserved 5 * 4 bytes */
    volatile u_isp_bnr_jnlmgain0    isp_bnr_jnlmgain0; /* 0x5824 */
    volatile u_isp_bnr_jnlmgain1    isp_bnr_jnlmgain1; /* 0x5828 */
    volatile u_isp_bnr_jnlmgain2    isp_bnr_jnlmgain2; /* 0x582c */
    volatile u_isp_bnr_jnlmgain3    isp_bnr_jnlmgain3; /* 0x5830 */
    volatile u_isp_bnr_jnlmgain_s0   isp_bnr_jnlmgain_s0; /* 0x5834 */
    volatile u_isp_bnr_jnlmgain_s1   isp_bnr_jnlmgain_s1; /* 0x5838 */
    volatile u_isp_bnr_jnlmgain_s2   isp_bnr_jnlmgain_s2; /* 0x583c */
    volatile u_isp_bnr_jnlmgain_s3   isp_bnr_jnlmgain_s3; /* 0x5840 */
    volatile u_isp_bnr_flt_ctrl     isp_bnr_flt_ctrl; /* 0x5844 */
    volatile u_isp_bnr_flt_ctrl1    isp_bnr_flt_ctrl1; /* 0x5848 */
    volatile u_isp_bnr_flt_ctrl2    isp_bnr_flt_ctrl2; /* 0x584c */
    volatile u_isp_bnr_flt_ctrl3    isp_bnr_flt_ctrl3; /* 0x5850 */
    volatile u_isp_bnr_flt_ctrl4    isp_bnr_flt_ctrl4; /* 0x5854 */
    volatile u_isp_bnr_flt_ctrl5    isp_bnr_flt_ctrl5; /* 0x5858 */
    volatile u_isp_bnr_flt_ctrl6    isp_bnr_flt_ctrl6; /* 0x585c */
    volatile u_isp_bnr_flt_ctrl7    isp_bnr_flt_ctrl7; /* 0x5860 */
    volatile u_isp_bnr_flt_ctrl8    isp_bnr_flt_ctrl8; /* 0x5864 */
    volatile u_isp_bnr_stt2lut_cfg   isp_bnr_stt2lut_cfg; /* 0x5868 */
    volatile u_isp_bnr_stt2lut_regnew   isp_bnr_stt2lut_regnew; /* 0x586c */
    volatile u_isp_bnr_stt2lut_abn   isp_bnr_stt2lut_abn; /* 0x5870 */
    volatile unsigned int           reserved_118[3]; /* 0x5874~0x587c, reserved 3 * 4 bytes */
    volatile u_isp_bnr_noisesd_even_waddr   isp_bnr_noisesd_even_waddr; /* 0x5880 */
    volatile u_isp_bnr_noisesd_even_wdata   isp_bnr_noisesd_even_wdata; /* 0x5884 */
    volatile u_isp_bnr_noisesd_even_raddr   isp_bnr_noisesd_even_raddr; /* 0x5888 */
    volatile u_isp_bnr_noisesd_even_rdata   isp_bnr_noisesd_even_rdata; /* 0x588c */
    volatile u_isp_bnr_noisesd_odd_waddr   isp_bnr_noisesd_odd_waddr; /* 0x5890 */
    volatile u_isp_bnr_noisesd_odd_wdata   isp_bnr_noisesd_odd_wdata; /* 0x5894 */
    volatile u_isp_bnr_noisesd_odd_raddr   isp_bnr_noisesd_odd_raddr; /* 0x5898 */
    volatile u_isp_bnr_noisesd_odd_rdata   isp_bnr_noisesd_odd_rdata; /* 0x589c */
    volatile u_isp_bnr_coring_low_even_waddr   isp_bnr_coring_low_even_waddr; /* 0x58a0 */
    volatile u_isp_bnr_coring_low_even_wdata   isp_bnr_coring_low_even_wdata; /* 0x58a4 */
    volatile u_isp_bnr_coring_low_even_raddr   isp_bnr_coring_low_even_raddr; /* 0x58a8 */
    volatile u_isp_bnr_coring_low_even_rdata   isp_bnr_coring_low_even_rdata; /* 0x58ac */
    volatile u_isp_bnr_coring_low_odd_waddr   isp_bnr_coring_low_odd_waddr; /* 0x58b0 */
    volatile u_isp_bnr_coring_low_odd_wdata   isp_bnr_coring_low_odd_wdata; /* 0x58b4 */
    volatile u_isp_bnr_coring_low_odd_raddr   isp_bnr_coring_low_odd_raddr; /* 0x58b8 */
    volatile u_isp_bnr_coring_low_odd_rdata   isp_bnr_coring_low_odd_rdata; /* 0x58bc */
    volatile u_isp_bnr_noiseinv_even_waddr   isp_bnr_noiseinv_even_waddr; /* 0x58c0 */
    volatile u_isp_bnr_noiseinv_even_wdata   isp_bnr_noiseinv_even_wdata; /* 0x58c4 */
    volatile u_isp_bnr_noiseinv_even_raddr   isp_bnr_noiseinv_even_raddr; /* 0x58c8 */
    volatile u_isp_bnr_noiseinv_even_rdata   isp_bnr_noiseinv_even_rdata; /* 0x58cc */
    volatile u_isp_bnr_noiseinv_odd_waddr   isp_bnr_noiseinv_odd_waddr; /* 0x58d0 */
    volatile u_isp_bnr_noiseinv_odd_wdata   isp_bnr_noiseinv_odd_wdata; /* 0x58d4 */
    volatile u_isp_bnr_noiseinv_odd_raddr   isp_bnr_noiseinv_odd_raddr; /* 0x58d8 */
    volatile u_isp_bnr_noiseinv_odd_rdata   isp_bnr_noiseinv_odd_rdata; /* 0x58dc */
    volatile u_isp_bnr_flt_ctrl9    isp_bnr_flt_ctrl9; /* 0x58e0 */
    volatile u_isp_bnr_flt_ctrl10   isp_bnr_flt_ctrl10; /* 0x58e4 */
    volatile u_isp_bnr_flt_ctrl11   isp_bnr_flt_ctrl11; /* 0x58e8 */
    volatile u_isp_bnr_flt_ctrl12   isp_bnr_flt_ctrl12; /* 0x58ec */
    volatile u_isp_bnr_flt_ctrl13   isp_bnr_flt_ctrl13; /* 0x58f0 */
    volatile u_isp_bnr_flt_ctrl14   isp_bnr_flt_ctrl14; /* 0x58f4 */
    volatile u_isp_bnr_flt_ctrl15   isp_bnr_flt_ctrl15; /* 0x58f8 */
    volatile u_isp_bnr_flt_ctrl16   isp_bnr_flt_ctrl16; /* 0x58fc */
    volatile u_isp_bnr_jnlm_ctrl0   isp_bnr_jnlm_ctrl0; /* 0x5900 */
    volatile u_isp_bnr_jnlm_ctrl1   isp_bnr_jnlm_ctrl1; /* 0x5904 */
    volatile u_isp_bnr_coring       isp_bnr_coring; /* 0x5908 */
    volatile u_isp_bnr_eddge        isp_bnr_eddge; /* 0x590c */
    volatile u_isp_bnr_bm1d_ctrl0   isp_bnr_bm1d_ctrl0; /* 0x5910 */
    volatile u_isp_bnr_bm1d_ctrl1   isp_bnr_bm1d_ctrl1; /* 0x5914 */
    volatile u_isp_bnr_bm1d_ctrl2   isp_bnr_bm1d_ctrl2; /* 0x5918 */
    volatile u_isp_bnr_bm1d_ctrl3   isp_bnr_bm1d_ctrl3; /* 0x591c */
    volatile u_isp_bnr_bm1d_ctrl4   isp_bnr_bm1d_ctrl4; /* 0x5920 */
    volatile u_isp_bnr_bm1d_ctrl5   isp_bnr_bm1d_ctrl5; /* 0x5924 */
    volatile u_isp_bnr_time_ctrl0   isp_bnr_time_ctrl0; /* 0x5928 */
    volatile u_isp_bnr_mix_lut0     isp_bnr_mix_lut0; /* 0x592c */
    volatile u_isp_bnr_mix_lut1     isp_bnr_mix_lut1; /* 0x5930 */
    volatile u_isp_bnr_mix_lut2     isp_bnr_mix_lut2; /* 0x5934 */
    volatile u_isp_bnr_mix_lut3     isp_bnr_mix_lut3; /* 0x5938 */
    volatile u_isp_bnr_time_edge    isp_bnr_time_edge; /* 0x593c */
    volatile u_isp_bnr_cfg0         isp_bnr_cfg0; /* 0x5940 */
    volatile u_isp_bnr_cfg1         isp_bnr_cfg1; /* 0x5944 */
    volatile u_isp_bnr_cfg2         isp_bnr_cfg2; /* 0x5948 */
    volatile u_isp_bnr_cfg3         isp_bnr_cfg3; /* 0x594c */
    volatile u_isp_bnr_cfg4         isp_bnr_cfg4; /* 0x5950 */
    volatile u_isp_bnr_cfg5         isp_bnr_cfg5; /* 0x5954 */
    volatile u_isp_bnr_cfg6         isp_bnr_cfg6; /* 0x5958 */
    volatile u_isp_bnr_cfg7         isp_bnr_cfg7; /* 0x595c */
    volatile u_isp_bnr_cfg8         isp_bnr_cfg8; /* 0x5960 */
    volatile u_isp_bnr_cfg9         isp_bnr_cfg9; /* 0x5964 */
    volatile u_isp_bnr_cfg10        isp_bnr_cfg10; /* 0x5968 */
    volatile u_isp_bnr_cfg11        isp_bnr_cfg11; /* 0x596c */
    volatile u_isp_bnr_cfg12        isp_bnr_cfg12; /* 0x5970 */
    volatile u_isp_bnr_cfg13        isp_bnr_cfg13; /* 0x5974 */
    volatile u_isp_bnr_cfg14        isp_bnr_cfg14; /* 0x5978 */
    volatile u_isp_bnr_cfg15        isp_bnr_cfg15; /* 0x597c */
    volatile u_isp_bnr_fbratiotable_waddr   isp_bnr_fbratiotable_waddr; /* 0x5980 */
    volatile u_isp_bnr_fbratiotable_wdata   isp_bnr_fbratiotable_wdata; /* 0x5984 */
    volatile u_isp_bnr_fbratiotable_raddr   isp_bnr_fbratiotable_raddr; /* 0x5988 */
    volatile u_isp_bnr_fbratiotable_rdata   isp_bnr_fbratiotable_rdata; /* 0x598c */
    volatile u_isp_bnr_noiseinvs_magidx_waddr   isp_bnr_noiseinvs_magidx_waddr; /* 0x5990 */
    volatile u_isp_bnr_noiseinvs_magidx_wdata   isp_bnr_noiseinvs_magidx_wdata; /* 0x5994 */
    volatile u_isp_bnr_noiseinvs_magidx_raddr   isp_bnr_noiseinvs_magidx_raddr; /* 0x5998 */
    volatile u_isp_bnr_noiseinvs_magidx_rdata   isp_bnr_noiseinvs_magidx_rdata; /* 0x599c */
    volatile unsigned int           reserved_119[24]; /* 0x59a0~0x59fc, reserved 24 * 4 bytes */
    volatile u_isp_bnr_rlsc_cfg     isp_bnr_rlsc_cfg; /* 0x5a00 */
    volatile unsigned int           reserved_120[3]; /* 0x5a04~0x5a0c, reserved 3 * 4 bytes */
    volatile u_isp_bnr_rlsc_widthoffset   isp_bnr_rlsc_widthoffset; /* 0x5a10 */
    volatile u_isp_bnr_rlsc_posr    isp_bnr_rlsc_posr; /* 0x5a14 */
    volatile u_isp_bnr_rlsc_posgr   isp_bnr_rlsc_posgr; /* 0x5a18 */
    volatile u_isp_bnr_rlsc_posgb   isp_bnr_rlsc_posgb; /* 0x5a1c */
    volatile u_isp_bnr_rlsc_posb    isp_bnr_rlsc_posb; /* 0x5a20 */
    volatile u_isp_bnr_rlsc_offcenterr   isp_bnr_rlsc_offcenterr; /* 0x5a24 */
    volatile u_isp_bnr_rlsc_offcentergr   isp_bnr_rlsc_offcentergr; /* 0x5a28 */
    volatile u_isp_bnr_rlsc_offcentergb   isp_bnr_rlsc_offcentergb; /* 0x5a2c */
    volatile u_isp_bnr_rlsc_offcenterb   isp_bnr_rlsc_offcenterb; /* 0x5a30 */
    volatile u_isp_bnr_rlsc_gain    isp_bnr_rlsc_gain; /* 0x5a34 */
    volatile u_isp_bnr_rlsc_maxgain   isp_bnr_rlsc_maxgain; /* 0x5a38 */
    volatile u_isp_bnr_rlsc_cmp_str   isp_bnr_rlsc_cmp_str; /* 0x5a3c */
    volatile u_isp_bnr_rlsc_reg     isp_bnr_rlsc_reg; /* 0x5a40 */
    volatile unsigned int           reserved_121[15]; /* 0x5a44~0x5a7c, reserved 15 * 4 bytes */
    volatile u_isp_bnr_rlsc_lut0_waddr   isp_bnr_rlsc_lut0_waddr; /* 0x5a80 */
    volatile u_isp_bnr_rlsc_lut0_wdata   isp_bnr_rlsc_lut0_wdata; /* 0x5a84 */
    volatile u_isp_bnr_rlsc_lut0_raddr   isp_bnr_rlsc_lut0_raddr; /* 0x5a88 */
    volatile u_isp_bnr_rlsc_lut0_rdata   isp_bnr_rlsc_lut0_rdata; /* 0x5a8c */
    volatile u_isp_bnr_rlsc_lut1_waddr   isp_bnr_rlsc_lut1_waddr; /* 0x5a90 */
    volatile u_isp_bnr_rlsc_lut1_wdata   isp_bnr_rlsc_lut1_wdata; /* 0x5a94 */
    volatile u_isp_bnr_rlsc_lut1_raddr   isp_bnr_rlsc_lut1_raddr; /* 0x5a98 */
    volatile u_isp_bnr_rlsc_lut1_rdata   isp_bnr_rlsc_lut1_rdata; /* 0x5a9c */
    volatile unsigned int           reserved_122[795]; /* 0x5aa0~0x6708, reserved 795 * 4 bytes */
    volatile u_isp_dehaze_version   isp_dehaze_version; /* 0x670c */
    volatile unsigned int           reserved_123; /* 0x6710 */
    volatile u_isp_dehaze_blk_size   isp_dehaze_blk_size; /* 0x6714 */
    volatile u_isp_dehaze_blk_sum   isp_dehaze_blk_sum; /* 0x6718 */
    volatile u_isp_dehaze_dc_size   isp_dehaze_dc_size; /* 0x671c */
    volatile u_isp_dehaze_x         isp_dehaze_x; /* 0x6720 */
    volatile u_isp_dehaze_y         isp_dehaze_y; /* 0x6724 */
    volatile u_isp_dehaze_stat_mode   isp_dehaze_stat_mode; /* 0x6728 */
    volatile u_isp_dehaze_neg_mode   isp_dehaze_neg_mode; /* 0x672c */
    volatile u_isp_dehaze_air       isp_dehaze_air; /* 0x6730 */
    volatile u_isp_dehaze_thld      isp_dehaze_thld; /* 0x6734 */
    volatile u_isp_dehaze_gstrth    isp_dehaze_gstrth; /* 0x6738 */
    volatile u_isp_dehaze_blthld    isp_dehaze_blthld; /* 0x673c */
    volatile u_isp_dehaze_stt_bst   isp_dehaze_stt_bst; /* 0x6740 */
    volatile u_isp_dehaze_stt_abn   isp_dehaze_stt_abn; /* 0x6744 */
    volatile u_isp_dehaze_stt2lut_cfg   isp_dehaze_stt2lut_cfg; /* 0x6748 */
    volatile u_isp_dehaze_stt2lut_regnew   isp_dehaze_stt2lut_regnew; /* 0x674c */
    volatile u_isp_dehaze_stt2lut_abn   isp_dehaze_stt2lut_abn; /* 0x6750 */
    volatile u_isp_dehaze_smlmapoffset   isp_dehaze_smlmapoffset; /* 0x6754 */
    volatile u_isp_dehaze_stat_point   isp_dehaze_stat_point; /* 0x6758 */
    volatile unsigned int           reserved_124; /* 0x675c */
    volatile u_isp_dehaze_stat_num   isp_dehaze_stat_num; /* 0x6760 */
    volatile unsigned int           reserved_125[7]; /* 0x6764~0x677c, reserved 7 * 4 bytes */
    volatile u_isp_dehaze_minstat_waddr   isp_dehaze_minstat_waddr; /* 0x6780 */
    volatile u_isp_dehaze_minstat_wdata   isp_dehaze_minstat_wdata; /* 0x6784 */
    volatile u_isp_dehaze_minstat_raddr   isp_dehaze_minstat_raddr; /* 0x6788 */
    volatile u_isp_dehaze_minstat_rdata   isp_dehaze_minstat_rdata; /* 0x678c */
    volatile u_isp_dehaze_maxstat_waddr   isp_dehaze_maxstat_waddr; /* 0x6790 */
    volatile u_isp_dehaze_maxstat_wdata   isp_dehaze_maxstat_wdata; /* 0x6794 */
    volatile u_isp_dehaze_maxstat_raddr   isp_dehaze_maxstat_raddr; /* 0x6798 */
    volatile u_isp_dehaze_maxstat_rdata   isp_dehaze_maxstat_rdata; /* 0x679c */
    volatile u_isp_dehaze_prestat_waddr   isp_dehaze_prestat_waddr; /* 0x67a0 */
    volatile u_isp_dehaze_prestat_wdata   isp_dehaze_prestat_wdata; /* 0x67a4 */
    volatile u_isp_dehaze_prestat_raddr   isp_dehaze_prestat_raddr; /* 0x67a8 */
    volatile u_isp_dehaze_prestat_rdata   isp_dehaze_prestat_rdata; /* 0x67ac */
    volatile u_isp_dehaze_lut_waddr   isp_dehaze_lut_waddr; /* 0x67b0 */
    volatile u_isp_dehaze_lut_wdata   isp_dehaze_lut_wdata; /* 0x67b4 */
    volatile u_isp_dehaze_lut_raddr   isp_dehaze_lut_raddr; /* 0x67b8 */
    volatile u_isp_dehaze_lut_rdata   isp_dehaze_lut_rdata; /* 0x67bc */
    volatile unsigned int           reserved_126[80]; /* 0x67c0~0x68fc, reserved 80 * 4 bytes */
    volatile u_isp_pregamma_cfg     isp_pregamma_cfg; /* 0x6900 */
    volatile unsigned int           reserved_127[3]; /* 0x6904~0x690c, reserved 3 * 4 bytes */
    volatile u_isp_pregamma_idxbase0   isp_pregamma_idxbase0; /* 0x6910 */
    volatile u_isp_pregamma_idxbase1   isp_pregamma_idxbase1; /* 0x6914 */
    volatile u_isp_pregamma_maxval0   isp_pregamma_maxval0; /* 0x6918 */
    volatile u_isp_pregamma_maxval1   isp_pregamma_maxval1; /* 0x691c */
    volatile u_isp_pregamma_stt2lut_cfg   isp_pregamma_stt2lut_cfg; /* 0x6920 */
    volatile u_isp_pregamma_stt2lut_regnew   isp_pregamma_stt2lut_regnew; /* 0x6924 */
    volatile u_isp_pregamma_stt2lut_abn   isp_pregamma_stt2lut_abn; /* 0x6928 */
    volatile unsigned int           reserved_128[21]; /* 0x692c~0x697c, reserved 21 * 4 bytes */
    volatile u_isp_pregamma_lut_waddr   isp_pregamma_lut_waddr; /* 0x6980 */
    volatile u_isp_pregamma_lut_wdata   isp_pregamma_lut_wdata; /* 0x6984 */
    volatile u_isp_pregamma_lut_raddr   isp_pregamma_lut_raddr; /* 0x6988 */
    volatile u_isp_pregamma_lut_rdata   isp_pregamma_lut_rdata; /* 0x698c */
    volatile unsigned int           reserved_129[28]; /* 0x6990~0x69fc, reserved 28 * 4 bytes */
    volatile u_isp_gamma_cfg        isp_gamma_cfg; /* 0x6a00 */
    volatile unsigned int           reserved_130[7]; /* 0x6a04~0x6a1c, reserved 7 * 4 bytes */
    volatile u_isp_gamma_pos0       isp_gamma_pos0; /* 0x6a20 */
    volatile u_isp_gamma_pos1       isp_gamma_pos1; /* 0x6a24 */
    volatile u_isp_gamma_pos2       isp_gamma_pos2; /* 0x6a28 */
    volatile u_isp_gamma_pos3       isp_gamma_pos3; /* 0x6a2c */
    volatile u_isp_gamma_inseg0     isp_gamma_inseg0; /* 0x6a30 */
    volatile u_isp_gamma_inseg1     isp_gamma_inseg1; /* 0x6a34 */
    volatile u_isp_gamma_inseg2     isp_gamma_inseg2; /* 0x6a38 */
    volatile u_isp_gamma_inseg3     isp_gamma_inseg3; /* 0x6a3c */
    volatile u_isp_gamma_step       isp_gamma_step; /* 0x6a40 */
    volatile unsigned int           reserved_131[3]; /* 0x6a44~0x6a4c, reserved 3 * 4 bytes */
    volatile u_isp_gamma_stt2lut_cfg   isp_gamma_stt2lut_cfg; /* 0x6a50 */
    volatile u_isp_gamma_stt2lut_regnew   isp_gamma_stt2lut_regnew; /* 0x6a54 */
    volatile u_isp_gamma_stt2lut_abn   isp_gamma_stt2lut_abn; /* 0x6a58 */
    volatile unsigned int           reserved_132[9]; /* 0x6a5c~0x6a7c, reserved 9 * 4 bytes */
    volatile u_isp_gamma_lut_waddr   isp_gamma_lut_waddr; /* 0x6a80 */
    volatile u_isp_gamma_lut_wdata   isp_gamma_lut_wdata; /* 0x6a84 */
    volatile u_isp_gamma_lut_raddr   isp_gamma_lut_raddr; /* 0x6a88 */
    volatile u_isp_gamma_lut_rdata   isp_gamma_lut_rdata; /* 0x6a8c */
    volatile unsigned int           reserved_133[156]; /* 0x6a90~0x6cfc, reserved 156 * 4 bytes */
    volatile u_isp_bdec_cfg         isp_bdec_cfg; /* 0x6d00 */
    volatile unsigned int           reserved_134[2]; /* 0x6d04~0x6d08, reserved 2 * 4 bytes */
    volatile u_isp_bdec_version     isp_bdec_version; /* 0x6d0c */
    volatile u_isp_bdec_alpha       isp_bdec_alpha; /* 0x6d10 */
    volatile unsigned int           reserved_135[191]; /* 0x6d14~0x700c, reserved 191 * 4 bytes */
    volatile u_isp_ca_ctrl          isp_ca_ctrl; /* 0x7010 */
    volatile u_isp_ca_lumath        isp_ca_lumath; /* 0x7014 */
    volatile u_isp_ca_darkchroma_th   isp_ca_darkchroma_th; /* 0x7018 */
    volatile u_isp_ca_sdarkchroma_th   isp_ca_sdarkchroma_th; /* 0x701c */
    volatile u_isp_ca_llhc_ratio    isp_ca_llhc_ratio; /* 0x7020 */
    volatile u_isp_ca_isoratio      isp_ca_isoratio; /* 0x7024 */
    volatile u_isp_ca_lut_update    isp_ca_lut_update; /* 0x7028 */
    volatile u_isp_ca_yuv2rgb_coef0   isp_ca_yuv2rgb_coef0; /* 0x702c */
    volatile u_isp_ca_yuv2rgb_coef1   isp_ca_yuv2rgb_coef1; /* 0x7030 */
    volatile u_isp_ca_yuv2rgb_coef2   isp_ca_yuv2rgb_coef2; /* 0x7034 */
    volatile u_isp_ca_yuv2rgb_coef3   isp_ca_yuv2rgb_coef3; /* 0x7038 */
    volatile u_isp_ca_yuv2rgb_coef4   isp_ca_yuv2rgb_coef4; /* 0x703c */
    volatile u_isp_ca_yuv2rgb_indc0   isp_ca_yuv2rgb_indc0; /* 0x7040 */
    volatile u_isp_ca_yuv2rgb_indc1   isp_ca_yuv2rgb_indc1; /* 0x7044 */
    volatile u_isp_ca_yuv2rgb_outdc0   isp_ca_yuv2rgb_outdc0; /* 0x7048 */
    volatile u_isp_ca_yuv2rgb_outdc1   isp_ca_yuv2rgb_outdc1; /* 0x704c */
    volatile u_isp_ca_skinlluma_uth   isp_ca_skinlluma_uth; /* 0x7050 */
    volatile u_isp_ca_skinlluma_uyth   isp_ca_skinlluma_uyth; /* 0x7054 */
    volatile u_isp_ca_skinhluma_uth   isp_ca_skinhluma_uth; /* 0x7058 */
    volatile u_isp_ca_skinhluma_uyth   isp_ca_skinhluma_uyth; /* 0x705c */
    volatile u_isp_ca_skinlluma_vth   isp_ca_skinlluma_vth; /* 0x7060 */
    volatile u_isp_ca_skinlluma_vyth   isp_ca_skinlluma_vyth; /* 0x7064 */
    volatile u_isp_ca_skinhluma_vth   isp_ca_skinhluma_vth; /* 0x7068 */
    volatile u_isp_ca_skinhluma_vyth   isp_ca_skinhluma_vyth; /* 0x706c */
    volatile u_isp_ca_skin_uvdiff   isp_ca_skin_uvdiff; /* 0x7070 */
    volatile u_isp_ca_skin_ratioth0   isp_ca_skin_ratioth0; /* 0x7074 */
    volatile u_isp_ca_skin_ratioth1   isp_ca_skin_ratioth1; /* 0x7078 */
    volatile unsigned int           reserved_136; /* 0x707c */
    volatile u_isp_ca_lut_waddr     isp_ca_lut_waddr; /* 0x7080 */
    volatile u_isp_ca_lut_wdata     isp_ca_lut_wdata; /* 0x7084 */
    volatile u_isp_ca_lut_raddr     isp_ca_lut_raddr; /* 0x7088 */
    volatile u_isp_ca_lut_rdata     isp_ca_lut_rdata; /* 0x708c */
    volatile unsigned int           reserved_137[222]; /* 0x7090~0x7404, reserved 222 * 4 bytes */
    volatile u_isp_clut_adj0        isp_clut_adj0; /* 0x7408 */
    volatile u_isp_clut_adj1        isp_clut_adj1; /* 0x740c */
    volatile unsigned int           reserved_138; /* 0x7410 */
    volatile u_isp_clut_stt2lut_cfg   isp_clut_stt2lut_cfg; /* 0x7414 */
    volatile u_isp_clut_stt2lut_regnew   isp_clut_stt2lut_regnew; /* 0x7418 */
    volatile u_isp_clut_stt2lut_abn   isp_clut_stt2lut_abn; /* 0x741c */
    volatile unsigned int           reserved_139[24]; /* 0x7420~0x747c, reserved 24 * 4 bytes */
    volatile u_isp_clut_lut0_waddr   isp_clut_lut0_waddr; /* 0x7480 */
    volatile u_isp_clut_lut0_wdata   isp_clut_lut0_wdata; /* 0x7484 */
    volatile u_isp_clut_lut0_raddr   isp_clut_lut0_raddr; /* 0x7488 */
    volatile u_isp_clut_lut0_rdata   isp_clut_lut0_rdata; /* 0x748c */
    volatile u_isp_clut_lut1_waddr   isp_clut_lut1_waddr; /* 0x7490 */
    volatile u_isp_clut_lut1_wdata   isp_clut_lut1_wdata; /* 0x7494 */
    volatile u_isp_clut_lut1_raddr   isp_clut_lut1_raddr; /* 0x7498 */
    volatile u_isp_clut_lut1_rdata   isp_clut_lut1_rdata; /* 0x749c */
    volatile u_isp_clut_lut2_waddr   isp_clut_lut2_waddr; /* 0x74a0 */
    volatile u_isp_clut_lut2_wdata   isp_clut_lut2_wdata; /* 0x74a4 */
    volatile u_isp_clut_lut2_raddr   isp_clut_lut2_raddr; /* 0x74a8 */
    volatile u_isp_clut_lut2_rdata   isp_clut_lut2_rdata; /* 0x74ac */
    volatile u_isp_clut_lut3_waddr   isp_clut_lut3_waddr; /* 0x74b0 */
    volatile u_isp_clut_lut3_wdata   isp_clut_lut3_wdata; /* 0x74b4 */
    volatile u_isp_clut_lut3_raddr   isp_clut_lut3_raddr; /* 0x74b8 */
    volatile u_isp_clut_lut3_rdata   isp_clut_lut3_rdata; /* 0x74bc */
    volatile unsigned int           reserved_140[48]; /* 0x74c0~0x757c, reserved 48 * 4 bytes */
    volatile u_isp_clut_lut4_waddr   isp_clut_lut4_waddr; /* 0x7580 */
    volatile u_isp_clut_lut4_wdata   isp_clut_lut4_wdata; /* 0x7584 */
    volatile u_isp_clut_lut4_raddr   isp_clut_lut4_raddr; /* 0x7588 */
    volatile u_isp_clut_lut4_rdata   isp_clut_lut4_rdata; /* 0x758c */
    volatile u_isp_clut_lut5_waddr   isp_clut_lut5_waddr; /* 0x7590 */
    volatile u_isp_clut_lut5_wdata   isp_clut_lut5_wdata; /* 0x7594 */
    volatile u_isp_clut_lut5_raddr   isp_clut_lut5_raddr; /* 0x7598 */
    volatile u_isp_clut_lut5_rdata   isp_clut_lut5_rdata; /* 0x759c */
    volatile u_isp_clut_lut6_waddr   isp_clut_lut6_waddr; /* 0x75a0 */
    volatile u_isp_clut_lut6_wdata   isp_clut_lut6_wdata; /* 0x75a4 */
    volatile u_isp_clut_lut6_raddr   isp_clut_lut6_raddr; /* 0x75a8 */
    volatile u_isp_clut_lut6_rdata   isp_clut_lut6_rdata; /* 0x75ac */
    volatile u_isp_clut_lut7_waddr   isp_clut_lut7_waddr; /* 0x75b0 */
    volatile u_isp_clut_lut7_wdata   isp_clut_lut7_wdata; /* 0x75b4 */
    volatile u_isp_clut_lut7_raddr   isp_clut_lut7_raddr; /* 0x75b8 */
    volatile u_isp_clut_lut7_rdata   isp_clut_lut7_rdata; /* 0x75bc */
    volatile unsigned int           reserved_141[144]; /* 0x75c0~0x77fc, reserved 144 * 4 bytes */
    volatile u_isp_ldci_cfg         isp_ldci_cfg; /* 0x7800 */
    volatile unsigned int           reserved_142[2]; /* 0x7804~0x7808, reserved 2 * 4 bytes */
    volatile u_isp_ldci_version     isp_ldci_version; /* 0x780c */
    volatile unsigned int           reserved_143; /* 0x7810 */
    volatile u_isp_ldci_calc_small_offset   isp_ldci_calc_small_offset; /* 0x7814 */
    volatile u_isp_ldci_zone        isp_ldci_zone; /* 0x7818 */
    volatile u_isp_ldci_glb_hewgt   isp_ldci_glb_hewgt; /* 0x781c */
    volatile u_isp_ldci_lpfstt_bst   isp_ldci_lpfstt_bst; /* 0x7820 */
    volatile u_isp_ldci_lpfstt_abn   isp_ldci_lpfstt_abn; /* 0x7824 */
    volatile u_isp_ldci_scale       isp_ldci_scale; /* 0x7828 */
    volatile unsigned int           reserved_144; /* 0x782c */
    volatile u_isp_ldci_drcen       isp_ldci_drcen; /* 0x7830 */
    volatile u_isp_ldci_luma_sel    isp_ldci_luma_sel; /* 0x7834 */
    volatile u_isp_ldci_blc_ctrl    isp_ldci_blc_ctrl; /* 0x7838 */
    volatile unsigned int           reserved_145; /* 0x783c */
    volatile u_isp_ldci_pflmt_en    isp_ldci_pflmt_en; /* 0x7840 */
    volatile u_isp_ldci_pfori       isp_ldci_pfori; /* 0x7844 */
    volatile u_isp_ldci_pfsft       isp_ldci_pfsft; /* 0x7848 */
    volatile u_isp_ldci_pfthd       isp_ldci_pfthd; /* 0x784c */
    volatile u_isp_ldci_pfrly       isp_ldci_pfrly; /* 0x7850 */
    volatile u_isp_ldci_pfmul       isp_ldci_pfmul; /* 0x7854 */
    volatile u_isp_ldci_lut_update   isp_ldci_lut_update; /* 0x7858 */
    volatile unsigned int           reserved_146[2]; /* 0x785c~0x7860, reserved 2 * 4 bytes */
    volatile u_isp_ldci_lpf_lpfcoef0   isp_ldci_lpf_lpfcoef0; /* 0x7864 */
    volatile u_isp_ldci_lpf_lpfcoef1   isp_ldci_lpf_lpfcoef1; /* 0x7868 */
    volatile u_isp_ldci_lpf_lpfcoef2   isp_ldci_lpf_lpfcoef2; /* 0x786c */
    volatile u_isp_ldci_lpf_lpfcoef3   isp_ldci_lpf_lpfcoef3; /* 0x7870 */
    volatile u_isp_ldci_lpf_lpfcoef4   isp_ldci_lpf_lpfcoef4; /* 0x7874 */
    volatile u_isp_ldci_lpf_lpfsft   isp_ldci_lpf_lpfsft; /* 0x7878 */
    volatile unsigned int           reserved_147; /* 0x787c */
    volatile u_isp_ldci_drc_waddr   isp_ldci_drc_waddr; /* 0x7880 */
    volatile u_isp_ldci_drc_wdata   isp_ldci_drc_wdata; /* 0x7884 */
    volatile u_isp_ldci_drc_raddr   isp_ldci_drc_raddr; /* 0x7888 */
    volatile u_isp_ldci_drc_rdata   isp_ldci_drc_rdata; /* 0x788c */
    volatile u_isp_ldci_he_waddr    isp_ldci_he_waddr; /* 0x7890 */
    volatile u_isp_ldci_he_wdata    isp_ldci_he_wdata; /* 0x7894 */
    volatile u_isp_ldci_he_raddr    isp_ldci_he_raddr; /* 0x7898 */
    volatile u_isp_ldci_he_rdata    isp_ldci_he_rdata; /* 0x789c */
    volatile u_isp_ldci_de_usm_waddr   isp_ldci_de_usm_waddr; /* 0x78a0 */
    volatile u_isp_ldci_de_usm_wdata   isp_ldci_de_usm_wdata; /* 0x78a4 */
    volatile u_isp_ldci_de_usm_raddr   isp_ldci_de_usm_raddr; /* 0x78a8 */
    volatile u_isp_ldci_de_usm_rdata   isp_ldci_de_usm_rdata; /* 0x78ac */
    volatile u_isp_ldci_cgain_waddr   isp_ldci_cgain_waddr; /* 0x78b0 */
    volatile u_isp_ldci_cgain_wdata   isp_ldci_cgain_wdata; /* 0x78b4 */
    volatile u_isp_ldci_cgain_raddr   isp_ldci_cgain_raddr; /* 0x78b8 */
    volatile u_isp_ldci_cgain_rdata   isp_ldci_cgain_rdata; /* 0x78bc */
    volatile unsigned int           reserved_148[24]; /* 0x78c0~0x791c, reserved 24 * 4 bytes */
    volatile u_isp_ldci_stat_hpos   isp_ldci_stat_hpos; /* 0x7920 */
    volatile u_isp_ldci_stat_vpos   isp_ldci_stat_vpos; /* 0x7924 */
    volatile u_isp_ldci_stat_evratio   isp_ldci_stat_evratio; /* 0x7928 */
    volatile unsigned int           reserved_149; /* 0x792c */
    volatile u_isp_ldci_stat_zone   isp_ldci_stat_zone; /* 0x7930 */
    volatile u_isp_ldci_bende       isp_ldci_bende; /* 0x7934 */
    volatile u_isp_ldci_deref       isp_ldci_deref; /* 0x7938 */
    volatile u_isp_ldci_deh_lpfsft   isp_ldci_deh_lpfsft; /* 0x793c */
    volatile u_isp_ldci_lpfcoefh    isp_ldci_lpfcoefh; /* 0x7940 */
    volatile u_isp_ldci_lpfcoefv    isp_ldci_lpfcoefv; /* 0x7944 */
    volatile unsigned int           reserved_150[2]; /* 0x7948~0x794c, reserved 2 * 4 bytes */
    volatile u_isp_ldci_blk_smlmapwidth0   isp_ldci_blk_smlmapwidth0; /* 0x7950 */
    volatile u_isp_ldci_blk_smlmapwidth1   isp_ldci_blk_smlmapwidth1; /* 0x7954 */
    volatile u_isp_ldci_blk_smlmapwidth2   isp_ldci_blk_smlmapwidth2; /* 0x7958 */
    volatile unsigned int           reserved_151[2]; /* 0x795c~0x7960, reserved 2 * 4 bytes */
    volatile u_isp_ldci_stat_drcen   isp_ldci_stat_drcen; /* 0x7964 */
    volatile u_isp_ldci_chrdamp     isp_ldci_chrdamp; /* 0x7968 */
    volatile u_isp_ldci_grobal_map   isp_ldci_grobal_map; /* 0x796c */
    volatile u_isp_ldci_stt2lut_cfg   isp_ldci_stt2lut_cfg; /* 0x7970 */
    volatile u_isp_ldci_stt2lut_regnew   isp_ldci_stt2lut_regnew; /* 0x7974 */
    volatile u_isp_ldci_stt2lut_abn   isp_ldci_stt2lut_abn; /* 0x7978 */
    volatile unsigned int           reserved_152; /* 0x797c */
    volatile u_isp_ldci_polyp_waddr   isp_ldci_polyp_waddr; /* 0x7980 */
    volatile u_isp_ldci_polyp_wdata   isp_ldci_polyp_wdata; /* 0x7984 */
    volatile u_isp_ldci_polyp_raddr   isp_ldci_polyp_raddr; /* 0x7988 */
    volatile u_isp_ldci_polyp_rdata   isp_ldci_polyp_rdata; /* 0x798c */
    volatile u_isp_ldci_polyq01_waddr   isp_ldci_polyq01_waddr; /* 0x7990 */
    volatile u_isp_ldci_polyq01_wdata   isp_ldci_polyq01_wdata; /* 0x7994 */
    volatile u_isp_ldci_polyq01_raddr   isp_ldci_polyq01_raddr; /* 0x7998 */
    volatile u_isp_ldci_polyq01_rdata   isp_ldci_polyq01_rdata; /* 0x799c */
    volatile u_isp_ldci_polyq23_waddr   isp_ldci_polyq23_waddr; /* 0x79a0 */
    volatile u_isp_ldci_polyq23_wdata   isp_ldci_polyq23_wdata; /* 0x79a4 */
    volatile u_isp_ldci_polyq23_raddr   isp_ldci_polyq23_raddr; /* 0x79a8 */
    volatile u_isp_ldci_polyq23_rdata   isp_ldci_polyq23_rdata; /* 0x79ac */
    volatile u_isp_ldci_lpf_map_waddr   isp_ldci_lpf_map_waddr; /* 0x79b0 */
    volatile u_isp_ldci_lpf_map_wdata   isp_ldci_lpf_map_wdata; /* 0x79b4 */
    volatile u_isp_ldci_lpf_map_raddr   isp_ldci_lpf_map_raddr; /* 0x79b8 */
    volatile u_isp_ldci_lpf_map_rdata   isp_ldci_lpf_map_rdata; /* 0x79bc */
    volatile u_isp_ldci_face_wgt    isp_ldci_face_wgt; /* 0x79c0 */
    volatile unsigned int           reserved_153[399]; /* 0x79c4~0x7ffc, reserved 399 * 4 bytes */
    volatile u_isp_adrc_cfg         isp_adrc_cfg; /* 0x8000 */
    volatile u_isp_adrc_lut_update   isp_adrc_lut_update; /* 0x8004 */
    volatile unsigned int           reserved_154; /* 0x8008 */
    volatile u_isp_adrc_version     isp_adrc_version; /* 0x800c */
    volatile u_isp_adrc_strength    isp_adrc_strength; /* 0x8010 */
    volatile u_isp_adrc_rgb_wgt     isp_adrc_rgb_wgt; /* 0x8014 */
    volatile u_isp_adrc_rgb_slo     isp_adrc_rgb_slo; /* 0x8018 */
    volatile u_isp_adrc_raw_cfg     isp_adrc_raw_cfg; /* 0x801c */
    volatile u_isp_adrc_pregamma_idxbase0   isp_adrc_pregamma_idxbase0; /* 0x8020 */
    volatile u_isp_adrc_pregamma_idxbase1   isp_adrc_pregamma_idxbase1; /* 0x8024 */
    volatile u_isp_adrc_pregamma_idxbase2   isp_adrc_pregamma_idxbase2; /* 0x8028 */
    volatile u_isp_adrc_pregamma_maxval0   isp_adrc_pregamma_maxval0; /* 0x802c */
    volatile u_isp_adrc_pregamma_maxval1   isp_adrc_pregamma_maxval1; /* 0x8030 */
    volatile u_isp_adrc_pregamma_maxval2   isp_adrc_pregamma_maxval2; /* 0x8034 */
    volatile u_isp_adrc_pregamma_maxval3   isp_adrc_pregamma_maxval3; /* 0x8038 */
    volatile u_isp_adrc_pregamma_maxval4   isp_adrc_pregamma_maxval4; /* 0x803c */
    volatile u_isp_adrc_pregamma_maxval5   isp_adrc_pregamma_maxval5; /* 0x8040 */
    volatile u_isp_adrc_shp_cfg_raw   isp_adrc_shp_cfg_raw; /* 0x8044 */
    volatile u_isp_adrc_div_denom_log_raw   isp_adrc_div_denom_log_raw; /* 0x8048 */
    volatile u_isp_adrc_denom_exp_raw   isp_adrc_denom_exp_raw; /* 0x804c */
    volatile u_isp_adrc_shp_cfg_luma   isp_adrc_shp_cfg_luma; /* 0x8050 */
    volatile u_isp_adrc_div_denom_log_luma   isp_adrc_div_denom_log_luma; /* 0x8054 */
    volatile u_isp_adrc_denom_exp_luma   isp_adrc_denom_exp_luma; /* 0x8058 */
    volatile u_isp_adrc_idxbase0    isp_adrc_idxbase0; /* 0x805c */
    volatile u_isp_adrc_idxbase1    isp_adrc_idxbase1; /* 0x8060 */
    volatile u_isp_adrc_maxval0     isp_adrc_maxval0; /* 0x8064 */
    volatile u_isp_adrc_maxval1     isp_adrc_maxval1; /* 0x8068 */
    volatile u_isp_adrc_gain_clip   isp_adrc_gain_clip; /* 0x806c */
    volatile u_isp_adrc_dark_gain_lmt_y   isp_adrc_dark_gain_lmt_y; /* 0x8070 */
    volatile u_isp_adrc_dark_gain_lmt_c   isp_adrc_dark_gain_lmt_c; /* 0x8074 */
    volatile u_isp_adrc_bright_gain_lmt   isp_adrc_bright_gain_lmt; /* 0x8078 */
    volatile unsigned int           reserved_155; /* 0x807c */
    volatile u_isp_adrc_tmlut0_waddr   isp_adrc_tmlut0_waddr; /* 0x8080 */
    volatile u_isp_adrc_tmlut0_wdata   isp_adrc_tmlut0_wdata; /* 0x8084 */
    volatile u_isp_adrc_tmlut0_raddr   isp_adrc_tmlut0_raddr; /* 0x8088 */
    volatile u_isp_adrc_tmlut0_rdata   isp_adrc_tmlut0_rdata; /* 0x808c */
    volatile u_isp_adrc_cclut_waddr   isp_adrc_cclut_waddr; /* 0x8090 */
    volatile u_isp_adrc_cclut_wdata   isp_adrc_cclut_wdata; /* 0x8094 */
    volatile u_isp_adrc_cclut_raddr   isp_adrc_cclut_raddr; /* 0x8098 */
    volatile u_isp_adrc_cclut_rdata   isp_adrc_cclut_rdata; /* 0x809c */
    volatile u_isp_adrc_pregamma_waddr   isp_adrc_pregamma_waddr; /* 0x80a0 */
    volatile u_isp_adrc_pregamma_wdata   isp_adrc_pregamma_wdata; /* 0x80a4 */
    volatile u_isp_adrc_pregamma_raddr   isp_adrc_pregamma_raddr; /* 0x80a8 */
    volatile u_isp_adrc_pregamma_rdata   isp_adrc_pregamma_rdata; /* 0x80ac */
    volatile u_isp_adrc_localmix_waddr   isp_adrc_localmix_waddr; /* 0x80b0 */
    volatile u_isp_adrc_localmix_wdata   isp_adrc_localmix_wdata; /* 0x80b4 */
    volatile u_isp_adrc_localmix_raddr   isp_adrc_localmix_raddr; /* 0x80b8 */
    volatile u_isp_adrc_localmix_rdata   isp_adrc_localmix_rdata; /* 0x80bc */
    volatile unsigned int           reserved_156[16]; /* 0x80c0~0x80fc, reserved 16 * 4 bytes */
    volatile u_isp_adrc_lut_ctrl    isp_adrc_lut_ctrl; /* 0x8100 */
    volatile u_isp_adrc_sup_dark0   isp_adrc_sup_dark0; /* 0x8104 */
    volatile u_isp_adrc_sup_bright0   isp_adrc_sup_bright0; /* 0x8108 */
    volatile u_isp_adrc_sup1        isp_adrc_sup1; /* 0x810c */
    volatile u_isp_adrc_sup_dark2   isp_adrc_sup_dark2; /* 0x8110 */
    volatile u_isp_adrc_sup_bright2   isp_adrc_sup_bright2; /* 0x8114 */
    volatile u_isp_adrc_bld_darkl   isp_adrc_bld_darkl; /* 0x8118 */
    volatile u_isp_adrc_bld_brightl   isp_adrc_bld_brightl; /* 0x811c */
    volatile u_isp_adrc_bld_darkd   isp_adrc_bld_darkd; /* 0x8120 */
    volatile u_isp_adrc_bld_brightd   isp_adrc_bld_brightd; /* 0x8124 */
    volatile unsigned int           reserved_157[6]; /* 0x8128~0x813c, reserved 6 * 4 bytes */
    volatile u_isp_adrc_prev_luma_0   isp_adrc_prev_luma_0; /* 0x8140 */
    volatile u_isp_adrc_prev_luma_1   isp_adrc_prev_luma_1; /* 0x8144 */
    volatile u_isp_adrc_prev_luma_2   isp_adrc_prev_luma_2; /* 0x8148 */
    volatile u_isp_adrc_prev_luma_3   isp_adrc_prev_luma_3; /* 0x814c */
    volatile u_isp_adrc_prev_luma_4   isp_adrc_prev_luma_4; /* 0x8150 */
    volatile u_isp_adrc_prev_luma_5   isp_adrc_prev_luma_5; /* 0x8154 */
    volatile u_isp_adrc_prev_luma_6   isp_adrc_prev_luma_6; /* 0x8158 */
    volatile u_isp_adrc_prev_luma_7   isp_adrc_prev_luma_7; /* 0x815c */
    volatile u_isp_adrc_zone        isp_adrc_zone; /* 0x8160 */
    volatile u_isp_adrc_zone_size   isp_adrc_zone_size; /* 0x8164 */
    volatile u_isp_adrc_zone_div0   isp_adrc_zone_div0; /* 0x8168 */
    volatile u_isp_adrc_zone_div1   isp_adrc_zone_div1; /* 0x816c */
    volatile u_isp_adrc_bin         isp_adrc_bin; /* 0x8170 */
    volatile u_isp_adrc_zone_init   isp_adrc_zone_init; /* 0x8174 */
    volatile u_isp_adrc_bin_scale   isp_adrc_bin_scale; /* 0x8178 */
    volatile u_isp_adrc_outbits     isp_adrc_outbits; /* 0x817c */
    volatile u_isp_adrc_mlut0_waddr   isp_adrc_mlut0_waddr; /* 0x8180 */
    volatile u_isp_adrc_mlut0_wdata   isp_adrc_mlut0_wdata; /* 0x8184 */
    volatile u_isp_adrc_mlut0_raddr   isp_adrc_mlut0_raddr; /* 0x8188 */
    volatile u_isp_adrc_mlut0_rdata   isp_adrc_mlut0_rdata; /* 0x818c */
    volatile u_isp_adrc_mlut1_waddr   isp_adrc_mlut1_waddr; /* 0x8190 */
    volatile u_isp_adrc_mlut1_wdata   isp_adrc_mlut1_wdata; /* 0x8194 */
    volatile u_isp_adrc_mlut1_raddr   isp_adrc_mlut1_raddr; /* 0x8198 */
    volatile u_isp_adrc_mlut1_rdata   isp_adrc_mlut1_rdata; /* 0x819c */
    volatile u_isp_adrc_mlut2_waddr   isp_adrc_mlut2_waddr; /* 0x81a0 */
    volatile u_isp_adrc_mlut2_wdata   isp_adrc_mlut2_wdata; /* 0x81a4 */
    volatile u_isp_adrc_mlut2_raddr   isp_adrc_mlut2_raddr; /* 0x81a8 */
    volatile u_isp_adrc_mlut2_rdata   isp_adrc_mlut2_rdata; /* 0x81ac */
    volatile u_isp_adrc_mlut3_waddr   isp_adrc_mlut3_waddr; /* 0x81b0 */
    volatile u_isp_adrc_mlut3_wdata   isp_adrc_mlut3_wdata; /* 0x81b4 */
    volatile u_isp_adrc_mlut3_raddr   isp_adrc_mlut3_raddr; /* 0x81b8 */
    volatile u_isp_adrc_mlut3_rdata   isp_adrc_mlut3_rdata; /* 0x81bc */
    volatile unsigned int             reserved_158[16]; /* 0x81c0~0x81fc, reserved 16 * 4 bytes */
    volatile u_isp_adrc_local_edge_lmt   isp_adrc_local_edge_lmt; /* 0x8200 */
    volatile u_isp_adrc_stat_width   isp_adrc_stat_width; /* 0x8204 */
    volatile u_isp_adrc_vbi_strength   isp_adrc_vbi_strength; /* 0x8208 */
    volatile u_isp_adrc_flt_cfg     isp_adrc_flt_cfg; /* 0x820c */
    volatile u_isp_adrc_grad_rev    isp_adrc_grad_rev; /* 0x8210 */
    volatile u_isp_adrc_wgt_box_tri_sel   isp_adrc_wgt_box_tri_sel; /* 0x8214 */
    volatile u_isp_adrc_vbi_state   isp_adrc_vbi_state; /* 0x8218 */
    volatile u_isp_adrc_blk_wgt_init   isp_adrc_blk_wgt_init; /* 0x821c */
    volatile u_isp_adrc_num         isp_adrc_num; /* 0x8220 */
    volatile u_isp_adrc_logalphadec   isp_adrc_logalphadec; /* 0x8224 */
    volatile u_isp_adrc_outbitw     isp_adrc_outbitw; /* 0x8228 */
    volatile u_isp_adrc_lumaclip_stepl   isp_adrc_lumaclip_stepl; /* 0x822c */
    volatile u_isp_adrc_lumaclip_steph   isp_adrc_lumaclip_steph; /* 0x8230 */
    volatile u_isp_adrc_debug_info   isp_adrc_debug_info; /* 0x8234 */
    volatile u_isp_adrc_wgtm_cfg    isp_adrc_wgtm_cfg; /* 0x28238 */
    volatile u_isp_adrc_wgtm_0      isp_adrc_wgtm_0; /* 0x2823c */
    volatile u_isp_adrc_wgtm_1      isp_adrc_wgtm_1; /* 0x28240 */
    volatile u_isp_adrc_wgtm_2      isp_adrc_wgtm_2; /* 0x28244 */
    volatile u_isp_adrc_wgtm_3      isp_adrc_wgtm_3; /* 0x28248 */
    volatile u_isp_adrc_wgtm_pd_cfg   isp_adrc_wgtm_pd_cfg; /* 0x2824c */
    volatile u_isp_adrc_rb00        isp_adrc_rb00; /* 0x8250 */
    volatile u_isp_adrc_rb01        isp_adrc_rb01; /* 0x8254 */
    volatile u_isp_adrc_rb02        isp_adrc_rb02; /* 0x8258 */
    volatile u_isp_adrc_rb03        isp_adrc_rb03; /* 0x825c */
    volatile u_isp_adrc_rb04        isp_adrc_rb04; /* 0x8260 */
    volatile u_isp_adrc_rb05        isp_adrc_rb05; /* 0x8264 */
    volatile u_isp_adrc_rb06        isp_adrc_rb06; /* 0x8268 */
    volatile u_isp_adrc_rb07        isp_adrc_rb07; /* 0x826c */
    volatile u_isp_adrc_rb08        isp_adrc_rb08; /* 0x8270 */
    volatile u_isp_adrc_rb09        isp_adrc_rb09; /* 0x8274 */
    volatile u_isp_adrc_rb10        isp_adrc_rb10; /* 0x8278 */
    volatile u_isp_adrc_rb11        isp_adrc_rb11; /* 0x827c */
    volatile unsigned int           reserved_159[32]; /* 0x8280~0x82fc, reserved 32 * 4 bytes */
    volatile u_isp_adrc_stat_cfg    isp_adrc_stat_cfg; /* 0x8300 */
    volatile u_isp_adrc_luma_avg_manual   isp_adrc_luma_avg_manual; /* 0x8304 */
    volatile u_isp_adrc_pw_curve_cfg   isp_adrc_pw_curve_cfg; /* 0x8308 */
    volatile u_isp_adrc_pw_curve_seg0   isp_adrc_pw_curve_seg0; /* 0x830c */
    volatile u_isp_adrc_pw_curve_seg1   isp_adrc_pw_curve_seg1; /* 0x8310 */
    volatile u_isp_adrc_pw_curve_seg2   isp_adrc_pw_curve_seg2; /* 0x8314 */
    volatile u_isp_adrc_pw_curve_seg3   isp_adrc_pw_curve_seg3; /* 0x8318 */
    volatile u_isp_adrc_pw_curve_seg4   isp_adrc_pw_curve_seg4; /* 0x831c */
    volatile u_isp_adrc_pw_curve_seg5   isp_adrc_pw_curve_seg5; /* 0x8320 */
    volatile u_isp_adrc_pw_curve_seg6   isp_adrc_pw_curve_seg6; /* 0x8324 */
    volatile u_isp_adrc_pw_curve_seg7   isp_adrc_pw_curve_seg7; /* 0x8328 */
    volatile u_isp_adrc_pw_curve_slo0   isp_adrc_pw_curve_slo0; /* 0x832c */
    volatile u_isp_adrc_pw_curve_slo1   isp_adrc_pw_curve_slo1; /* 0x8330 */
    volatile u_isp_adrc_pw_curve_slo2   isp_adrc_pw_curve_slo2; /* 0x8334 */
    volatile u_isp_adrc_pw_curve_slo3   isp_adrc_pw_curve_slo3; /* 0x8338 */
    volatile u_isp_adrc_pw_curve_slo4   isp_adrc_pw_curve_slo4; /* 0x833c */
    volatile u_isp_adrc_pw_curve_slo5   isp_adrc_pw_curve_slo5; /* 0x8340 */
    volatile u_isp_adrc_pw_curve_slo6   isp_adrc_pw_curve_slo6; /* 0x8344 */
    volatile u_isp_adrc_pw_curve_slo7   isp_adrc_pw_curve_slo7; /* 0x8348 */
    volatile u_isp_adrc_pd_cfg      isp_adrc_pd_cfg; /* 0x834c */
    volatile u_isp_adrc_pwd_cfg     isp_adrc_pwd_cfg; /* 0x8350 */
    volatile u_isp_adrc_tm_gain_skip   isp_adrc_tm_gain_skip; /* 0x8354 */
    volatile u_isp_adrc_npu_cfg     isp_adrc_npu_cfg; /* 0x8358 */
    volatile u_isp_adrc_mask_ada_cfg   isp_adrc_mask_ada_cfg; /* 0x835c */
    volatile u_isp_adrc_mask_ada_coef0   isp_adrc_mask_ada_coef0; /* 0x8360 */
    volatile u_isp_adrc_mask_ada_coef1   isp_adrc_mask_ada_coef1; /* 0x8364 */
    volatile u_isp_adrc_wgt_tm      isp_adrc_wgt_tm; /* 0x8368 */
    volatile unsigned int           reserved_160[101]; /* 0x836c~0x84fc, reserved 101 * 4 bytes */
    volatile u_isp_degammafe_cfg    isp_degammafe_cfg; /* 0x8500 */
    volatile unsigned int           reserved_161[3]; /* 0x8504~0x850c, reserved 3 * 4 bytes */
    volatile u_isp_degammafe_idxbase0   isp_degammafe_idxbase0; /* 0x8510 */
    volatile u_isp_degammafe_idxbase1   isp_degammafe_idxbase1; /* 0x8514 */
    volatile u_isp_degammafe_maxval0   isp_degammafe_maxval0; /* 0x8518 */
    volatile u_isp_degammafe_maxval1   isp_degammafe_maxval1; /* 0x851c */
    volatile u_isp_degammafe_stt2lut_cfg   isp_degammafe_stt2lut_cfg; /* 0x8520 */
    volatile u_isp_degammafe_stt2lut_regnew   isp_degammafe_stt2lut_regnew; /* 0x8524 */
    volatile u_isp_degammafe_stt2lut_abn   isp_degammafe_stt2lut_abn; /* 0x8528 */
    volatile unsigned int           reserved_162[21]; /* 0x852c~0x857c, reserved 21 * 4 bytes */
    volatile u_isp_degammafe_lut_waddr   isp_degammafe_lut_waddr; /* 0x8580 */
    volatile u_isp_degammafe_lut_wdata   isp_degammafe_lut_wdata; /* 0x8584 */
    volatile u_isp_degammafe_lut_raddr   isp_degammafe_lut_raddr; /* 0x8588 */
    volatile u_isp_degammafe_lut_rdata   isp_degammafe_lut_rdata; /* 0x858c */
    volatile unsigned int           reserved_163[2847]; /* 0x8590~0xb208, reserved 2847 * 4 bytes */
    volatile u_isp_split_version    isp_split_version; /* 0xb20c */
    volatile u_isp_split_data_cfg   isp_split_data_cfg; /* 0xb210 */
    volatile u_isp_split_data_version   isp_split_data_version; /* 0xb214 */
    volatile u_isp_split_data_bitwidth   isp_split_data_bitwidth; /* 0xb218 */
    volatile u_isp_split_data_again   isp_split_data_again; /* 0xb21c */
    volatile u_isp_split_data_dgain   isp_split_data_dgain; /* 0xb220 */
    volatile u_isp_split_data_shutter   isp_split_data_shutter; /* 0xb224 */
    volatile unsigned int           reserved_164[54]; /* 0xb228~0xb2fc, reserved 54 * 4 bytes */

    isp_post_be_lut_type            post_be_lut;                        /* 0xb300~0x1b8fc, size 0x10600 */
} isp_post_be_reg_type;

typedef struct {
    isp_pre_be_reg_type   pre_be;          /* 0x0 ~ 0x103fc, size 0x10400 = (0x8700 + 0x7d00) */
    isp_post_be_reg_type  post_be;         /* 0x10400 ~ 0x2bcfc, size  0x1b900 = (0xb300 + 0x10600) */
    isp_viproc_reg_type   pre_viproc;      /* 0x2bd00 ~ 0x2c3fc, size  0x700 */
    isp_viproc_reg_type   post_viproc;     /* 0x2c400 ~ 0x2cafc, size  0x700 */
} isp_be_all_reg_type;

#endif /* ISP_REG_DEFINE_H */
