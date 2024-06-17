/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_VIPROC_REG_DEFINE_H
#define ISP_VIPROC_REG_DEFINE_H

/* Define the union u_viproc_isp_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    out0_crop_en          : 1; /* [0]  */
        unsigned int    isp_bas_en            : 1; /* [1]  */
        unsigned int    bas_in_crop_en        : 1; /* [2]  */
        unsigned int    bas_out_crop_en       : 1; /* [3]  */
        unsigned int    map_interp_en         : 1; /* [4]  */
        unsigned int    map_crop_en           : 1; /* [5]  */
        unsigned int    ratio_crop_en         : 1; /* [6]  */
        unsigned int    reserved_0            : 6; /* [12..7]  */
        unsigned int    isp_be_en             : 1; /* [13]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    img_pro_mode          : 2; /* [17..16]  */
        unsigned int    vcpin_sfifo_en        : 1; /* [18]  */
        unsigned int    dsf_en                : 1; /* [19]  */
        unsigned int    wdr_fpn_mux           : 1; /* [20]  */
        unsigned int    dcg_fpn_sel           : 1; /* [21]  */
        unsigned int    ch0_mux               : 1; /* [22]  */
        unsigned int    reserved_2            : 2; /* [24..23]  */
        unsigned int    fpn_out_crop_en       : 1; /* [25]  */
        unsigned int    isp_be_bypass_mode    : 3; /* [28..26]  */
        unsigned int    wdr_mode              : 1; /* [29]  */
        unsigned int    reserved_3            : 1; /* [30]  */
        unsigned int    video_en              : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_ctrl0;

/* Define the union u_viproc_isp_regup */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reg_up                : 1; /* [0]  */
        unsigned int    reserved_0            : 1; /* [1]  */
        unsigned int    reserved_1            : 1; /* [2]  */
        unsigned int    reserved_2            : 5; /* [7..3]  */
        unsigned int    manual_reg_up         : 1; /* [8]  */
        unsigned int    reserved_3            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_regup;

/* Define the union u_viproc_isp_ispbe_regup */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ispbe_reg_up          : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_ispbe_regup;

/* Define the union u_viproc_isp_ispbe_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_fpn_en            : 1; /* [1]  pre_be only */
        unsigned int    isp_fpn_mode          : 1; /* [2]  pre_be only */
        unsigned int    isp_rgbir_en          : 1; /* [3]  pre_be only */
        unsigned int    isp_dpc_en            : 1; /* [4]  pre_be only */
        unsigned int    isp_dpc_stat_en       : 1; /* [5]  pre_be only */
        unsigned int    isp_ge_en             : 1; /* [6]  pre_be only */
        unsigned int    isp_dpc1_en           : 1; /* [7]  pre_be only */
        unsigned int    isp_dpc1_stat_en      : 1; /* [8]  pre_be only */
        unsigned int    isp_ge1_en            : 1; /* [9]  pre_be only */
        unsigned int    isp_4dg_en            : 1; /* [10]  pre_be only */
        unsigned int    isp_flicker_en        : 1; /* [11]  pre_be only */
        unsigned int    isp_wdr_en            : 1; /* [12]  pre_be only */
        unsigned int    isp_expander_en       : 1; /* [13]  pre_be only */
        unsigned int    isp_crb_en            : 1; /* [14]  pre_be only */
        unsigned int    isp_bcom_en           : 1; /* [15]  pre_be only */
        unsigned int    reserved_1            : 1; /* [16]  */
        unsigned int    isp_pregammafe_en     : 1; /* [17]  pre_be only */
        unsigned int    isp_degammafe_en      : 1; /* [18]  post_be only */
        unsigned int    reserved_2            : 1; /* [19]  */
        unsigned int    isp_bnr_en            : 1; /* [20]  post_be only */
        unsigned int    isp_bdec_en           : 1; /* [21]  post_be only */
        unsigned int    isp_awblsc_en         : 1; /* [22]  post_be only */
        unsigned int    isp_lsc_en            : 1; /* [23]  post_be only */
        unsigned int    isp_dg_en             : 1; /* [24]  post_be only */
        unsigned int    isp_af_en             : 1; /* [25]  post_be only */
        unsigned int    isp_awb_en            : 1; /* [26]  post_be only */
        unsigned int    isp_ae_en             : 1; /* [27]  post_be + pre_be */
        unsigned int    reserved_3            : 1; /* [28]  */
        unsigned int    isp_wb_en             : 1; /* [29]  post_be only */
        unsigned int    isp_blc_en            : 1; /* [30]  post_be only */
        unsigned int    reserved_4            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ctrl0;

/* Define the union u_viproc_isp_ispbe_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    isp_adrc_en           : 1; /* [1]  post_be only */
        unsigned int    isp_la_en             : 1; /* [2]  post_be only */
        unsigned int    reserved_1            : 1; /* [3]  */
        unsigned int    isp_dmnr_nddm_en      : 1; /* [4]  post_be only */
        unsigned int    isp_dmnr_vhdm_en      : 1; /* [5]  post_be only */
        unsigned int    reserved_2            : 1; /* [6]  */
        unsigned int    reserved_3            : 1; /* [7]  */
        unsigned int    isp_cc_en             : 1; /* [8]  post_be only */
        unsigned int    isp_clut_en           : 1; /* [9]  post_be only */
        unsigned int    reserved_4            : 1; /* [10]  */
        unsigned int    isp_gamma_en          : 1; /* [11]  post_be only */
        unsigned int    isp_dehaze_en         : 1; /* [12]  post_be only */
        unsigned int    isp_csc_en            : 1; /* [13]  post_be only */
        unsigned int    isp_sumy_en           : 1; /* [14]  post_be + pre_be */
        unsigned int    isp_ldci_en           : 1; /* [15]  post_be only */
        unsigned int    isp_ca_en             : 1; /* [16]  post_be only */
        unsigned int    isp_split_en          : 1; /* [17]  post_be only */
        unsigned int    isp_sharpen_en        : 1; /* [18]  post_be only */
        unsigned int    isp_hcds_en           : 1; /* [19]  post_be only */
        unsigned int    reserved_5            : 1; /* [20]  */
        unsigned int    reserved_6            : 1; /* [21]  */
        unsigned int    isp_vcds_en           : 1; /* [22]  post_be only */
        unsigned int    reserved_7            : 1; /* [23]  */
        unsigned int    isp_wbf_en            : 1; /* [24]  pre_be only */
        unsigned int    reserved_8            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ctrl1;

/* Define the union u_viproc_isp_ispbe_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_be_rggb_cfg       : 2; /* [1..0]  */
        unsigned int    isp_be_rgbir_cfg      : 5; /* [6..2]  pre_be only */
        unsigned int    reserved_0            : 9; /* [15..7]  */
        unsigned int    isp_be_timing_cfg     : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_cfg;

/* Define the union u_viproc_ratio_crop0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ratio_center_hor      : 16; /* [15..0]  */
        unsigned int    ratio_center_ver      : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_ratio_crop0;

/* Define the union u_viproc_ratio_crop1 */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int ratio_square_of_radius : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_ratio_crop1;

/* Define the union u_viproc_isp_para_drc_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int para_drc_addr_low      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_para_drc_addr_low;
/* Define the union u_viproc_isp_para_drc_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int para_drc_addr_high     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_para_drc_addr_high;
/* Define the union u_viproc_isp_para_dci_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int para_dci_addr_low      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_para_dci_addr_low;
/* Define the union u_viproc_isp_para_dci_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int para_dci_addr_high     : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_para_dci_addr_high;
/* Define the union u_viproc_isp_out_para_drc_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int out_para_drc_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_out_para_drc_addr_low;
/* Define the union u_viproc_isp_out_para_drc_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int out_para_drc_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_out_para_drc_addr_high;
/* Define the union u_viproc_isp_out_para_dci_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int out_para_dci_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_out_para_dci_addr_low;
/* Define the union u_viproc_isp_out_para_dci_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int out_para_dci_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_out_para_dci_addr_high;
/* Define the union u_viproc_isp_be_ae_hist_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_hist_addr_low;
/* Define the union u_viproc_isp_be_ae_hist_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_hist_addr_high;
/* Define the union u_viproc_isp_be_ae_aver_r_gr_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_r_gr_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_aver_r_gr_addr_low;
/* Define the union u_viproc_isp_be_ae_aver_r_gr_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_r_gr_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_aver_r_gr_addr_high;
/* Define the union u_viproc_isp_be_ae_aver_gb_b_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_gb_b_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_aver_gb_b_addr_low;
/* Define the union u_viproc_isp_be_ae_aver_gb_b_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_aver_gb_b_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_aver_gb_b_addr_high;
/* Define the union u_viproc_isp_be_ae_ir_hist_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ir_hist_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ir_hist_addr_low;
/* Define the union u_viproc_isp_be_ae_ir_hist_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ir_hist_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ir_hist_addr_high;
/* Define the union u_viproc_isp_be_ae_ir_aver_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ir_aver_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ir_aver_addr_low;
/* Define the union u_viproc_isp_be_ae_ir_aver_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ir_aver_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ir_aver_addr_high;
/* Define the union u_viproc_isp_be_ae_ai_hist_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_hist_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ai_hist_addr_low;
/* Define the union u_viproc_isp_be_ae_ai_hist_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_hist_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ai_hist_addr_high;
/* Define the union u_viproc_isp_be_ae_ai_aver_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_aver_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ai_aver_addr_low;
/* Define the union u_viproc_isp_be_ae_ai_aver_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_ai_aver_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ae_ai_aver_addr_high;
/* Define the union u_viproc_isp_be_awb_stat_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awb_stat_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_awb_stat_addr_low;
/* Define the union u_viproc_isp_be_awb_stat_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awb_stat_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_awb_stat_addr_high;
/* Define the union u_viproc_isp_be_af_stat_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_af_stat_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_af_stat_addr_low;
/* Define the union u_viproc_isp_be_af_stat_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_af_stat_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_af_stat_addr_high;

/* Define the union u_viproc_isp_be_la_aver_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_la_aver_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_la_aver_addr_low;
/* Define the union u_viproc_isp_be_la_aver_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_la_aver_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_la_aver_addr_high;

/* Define the union u_viproc_isp_be_dehaze_min_stat_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_min_stat_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dehaze_min_stat_addr_low;
/* Define the union u_viproc_isp_be_hehaze_min_stat_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_min_stat_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_hehaze_min_stat_addr_high;
/* Define the union u_viproc_isp_be_dehaze_max_stat_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_max_stat_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dehaze_max_stat_addr_low;
/* Define the union u_viproc_isp_be_hehaze_max_stat_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_max_stat_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_hehaze_max_stat_addr_high;

/* Define the union u_viproc_isp_be_ldci_lpf_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_lpf_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ldci_lpf_addr_low;
/* Define the union u_viproc_isp_be_ldci_lpf_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_lpf_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ldci_lpf_addr_high;
/* Define the union u_viproc_isp_be_abwlsc_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awblsc_addr_low    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_abwlsc_addr_low;
/* Define the union u_viproc_isp_be_abwlsc_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awblsc_addr_high   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_abwlsc_addr_high;
/* Define the union u_viproc_isp_out1_finish_line */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    out1_finish_line      : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_out1_finish_line;

/* Define the union u_viproc_isp_be_dcg_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dcg_lut_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dcg_lut_addr_low;
/* Define the union u_viproc_isp_be_dcg_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dcg_lut_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dcg_lut_addr_high;
/* Define the union u_viproc_isp_be_dcg_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dcg_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dcg_lut_width_word;

/* Define the union u_viproc_isp_be_dcg1_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dcg1_lut_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dcg1_lut_addr_low;
/* Define the union u_viproc_isp_be_dcg1_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dcg1_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dcg1_lut_addr_high;
/* Define the union u_viproc_isp_be_dcg1_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dcg1_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dcg1_lut_width_word;

/* Define the union u_viproc_isp_be_bnr_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_lut_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_bnr_lut_addr_low;
/* Define the union u_viproc_isp_be_bnr_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_lut_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_bnr_lut_addr_high;
/* Define the union u_viproc_isp_be_bnr_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_bnr_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_bnr_lut_width_word;

/* Define the union u_viproc_isp_be_lsc_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_lut_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_lsc_lut_addr_low;
/* Define the union u_viproc_isp_be_lsc_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_lsc_lut_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_lsc_lut_addr_high;
/* Define the union u_viproc_isp_be_lsc_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_lsc_lut_width_word;

/* Define the union u_viproc_isp_be_clut_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_clut_lut_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_clut_lut_addr_low;
/* Define the union u_viproc_isp_be_clut_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_clut_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_clut_lut_addr_high;
/* Define the union u_viproc_isp_be_clut_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_clut_lut_width_word;

/* Define the union u_viproc_isp_be_gamma_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_gamma_lut_addr_low;
/* Define the union u_viproc_isp_be_gamma_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_gamma_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_gamma_lut_addr_high;
/* Define the union u_viproc_isp_be_gamma_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_gamma_lut_width_word;

/* Define the union u_viproc_isp_be_dehaze_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_lut_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dehaze_lut_addr_low;
/* Define the union u_viproc_isp_be_dehaze_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dehaze_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dehaze_lut_addr_high;
/* Define the union u_viproc_isp_be_dehaze_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dehaze_lut_width_word;

/* Define the union u_viproc_isp_be_ldci_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_lut_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ldci_lut_addr_low;
/* Define the union u_viproc_isp_be_ldci_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ldci_lut_addr_high;
/* Define the union u_viproc_isp_be_ldci_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_ldci_lut_width_word;

/* Define the union u_viproc_isp_be_sharpen_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_lut_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_sharpen_lut_addr_low;
/* Define the union u_viproc_isp_be_sharpen_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_sharpen_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_sharpen_lut_addr_high;
/* Define the union u_viproc_isp_be_sharpen_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_sharpen_lut_width_word;

/* Define the union u_viproc_isp_be_pregammafe_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregammafe_lut_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_pregammafe_lut_addr_low;
/* Define the union u_viproc_isp_be_pregammafe_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_pregammafe_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_pregammafe_lut_addr_high;
/* Define the union u_viproc_isp_be_pregammafe_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_pregammafe_lut_width_word;

/* Define the union u_viproc_isp_be_degammafe_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_degammafe_lut_addr_low : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_degammafe_lut_addr_low;
/* Define the union u_viproc_isp_be_degammafe_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_degammafe_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_degammafe_lut_addr_high;
/* Define the union u_viproc_isp_be_degammafe_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_degammafe_lut_width_word;

/* Define the union u_viproc_isp_be_dmnr_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dmnr_lut_addr_low  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dmnr_lut_addr_low;
/* Define the union u_viproc_isp_be_dmnr_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_dmnr_lut_addr_high : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dmnr_lut_addr_high;
/* Define the union u_viproc_isp_be_dmnr_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dmnr_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_dmnr_lut_width_word;

/* Define the union u_viproc_isp_be_crb_lut_addr_low */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_crb_lut_addr_low   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_crb_lut_addr_low;
/* Define the union u_viproc_isp_be_crb_lut_addr_high */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_crb_lut_addr_high  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_crb_lut_addr_high;
/* Define the union u_viproc_isp_be_crb_lut_width_word */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_lut_width_word : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_viproc_isp_be_crb_lut_width_word;

typedef struct {
    volatile u_viproc_isp_ctrl0                 viproc_ctrl0; /* 0x0 */
    volatile unsigned int                       reserved_0[3]; /* 0x4~0xc, reserved 3 * 4 bytes */
    volatile u_viproc_isp_regup                 viproc_regup; /* 0x10 */
    volatile u_viproc_isp_ispbe_regup           viproc_ispbe_regup; /* 0x14 */
    volatile unsigned int                       reserved_1[2]; /* 0x18~0x1c, reserved 2 * 4 bytes */
    volatile u_viproc_isp_be_ctrl0              viproc_ispbe_ctrl0; /* 0x20 */
    volatile u_viproc_isp_be_ctrl1              viproc_ispbe_ctrl1; /* 0x24 */
    volatile unsigned int                       reserved_2; /* 0x28 */
    volatile u_viproc_isp_be_cfg                viproc_ispbe_cfg; /* 0x2c */
    volatile unsigned int                       reserved_3[20]; /* 0x30~0x7c, reserved 20 * 4 bytes */
    volatile u_viproc_isp_ratio_crop0           viproc_ratio_crop0; /* 0x80 */
    volatile u_viproc_isp_ratio_crop1           viproc_ratio_crop1; /* 0x84 */
    volatile unsigned int                       reserved_30[277]; /* 0x88~0x4d8, reserved 277 * 4 bytes */
    volatile u_viproc_isp_para_drc_addr_low     viproc_para_drc_addr_low; /* 0x4dc */
    volatile u_viproc_isp_para_drc_addr_high    viproc_para_drc_addr_high; /* 0x4e0 */
    volatile u_viproc_isp_para_dci_addr_low     viproc_para_dci_addr_low; /* 0x4e4 */
    volatile u_viproc_isp_para_dci_addr_high    viproc_para_dci_addr_high; /* 0x4e8 */
    volatile u_viproc_isp_out_para_drc_addr_low   viproc_out_para_drc_addr_low; /* 0x4ec */
    volatile u_viproc_isp_out_para_drc_addr_high   viproc_out_para_drc_addr_high; /* 0x4f0 */
    volatile u_viproc_isp_out_para_dci_addr_low   viproc_out_para_dci_addr_low; /* 0x4f4 */
    volatile u_viproc_isp_out_para_dci_addr_high   viproc_out_para_dci_addr_high; /* 0x4f8 */
    /* stat addr */
    volatile u_viproc_isp_be_ae_hist_addr_low   ae_hist_addr_low; /* 0x4fc, post_be only */
    volatile u_viproc_isp_be_ae_hist_addr_high   ae_hist_addr_high; /* 0x500, post_be only */
    volatile u_viproc_isp_be_ae_aver_r_gr_addr_low   ae_aver_r_gr_addr_low; /* 0x504, post_be only */
    volatile u_viproc_isp_be_ae_aver_r_gr_addr_high   ae_aver_r_gr_addr_high; /* 0x508, post_be only */
    volatile u_viproc_isp_be_ae_aver_gb_b_addr_low   ae_aver_gb_b_addr_low; /* 0x50c, post_be only */
    volatile u_viproc_isp_be_ae_aver_gb_b_addr_high   ae_aver_gb_b_addr_high; /* 0x510, post_be only */
    volatile u_viproc_isp_be_ae_ir_hist_addr_low   ae_ir_hist_addr_low; /* 0x514, not support */
    volatile u_viproc_isp_be_ae_ir_hist_addr_high   ae_ir_hist_addr_high; /* 0x518, not support  */
    volatile u_viproc_isp_be_ae_ir_aver_addr_low   ae_ir_aver_addr_low; /* 0x51c, not support  */
    volatile u_viproc_isp_be_ae_ir_aver_addr_high   ae_ir_aver_addr_high; /* 0x520, not support  */
    volatile u_viproc_isp_be_ae_ai_hist_addr_low   ae_ai_hist_addr_low; /* 0x524, not support  */
    volatile u_viproc_isp_be_ae_ai_hist_addr_high   ae_ai_hist_addr_high; /* 0x528, not support  */
    volatile u_viproc_isp_be_ae_ai_aver_addr_low   ae_ai_aver_addr_low; /* 0x52c, not support  */
    volatile u_viproc_isp_be_ae_ai_aver_addr_high   ae_ai_aver_addr_high; /* 0x530, not support  */
    volatile u_viproc_isp_be_awb_stat_addr_low   awb_stat_addr_low; /* 0x534, post_be only */
    volatile u_viproc_isp_be_awb_stat_addr_high   awb_stat_addr_high; /* 0x538, post_be only  */
    volatile u_viproc_isp_be_af_stat_addr_low   af_stat_addr_low; /* 0x53c, post_be only  */
    volatile u_viproc_isp_be_af_stat_addr_high   af_stat_addr_high; /* 0x540, post_be only  */
    volatile unsigned int                       reserved_4[4]; /* 0x544 ~ 0x550, reserved 4 * 4 bytes */
    volatile u_viproc_isp_be_la_aver_addr_low   la_aver_addr_low; /* 0x554, post_be only  */
    volatile u_viproc_isp_be_la_aver_addr_high  la_aver_addr_high; /* 0x558, post_be only  */
    volatile unsigned int                       reserved_5[4]; /* 0x55c ~ 0x568, reserved 4 * 4 bytes */
    volatile u_viproc_isp_be_dehaze_min_stat_addr_low  dehaze_min_stat_addr_low; /* 0x56c, post_be only */
    volatile u_viproc_isp_be_hehaze_min_stat_addr_high dehaze_min_stat_addr_high; /* 0x570, post_be only */
    volatile u_viproc_isp_be_dehaze_max_stat_addr_low  dehaze_max_stat_addr_low; /* 0x574, post_be only */
    volatile u_viproc_isp_be_hehaze_max_stat_addr_high dehaze_max_stat_addr_high; /* 0x578, post_be only */
    volatile unsigned int                       reserved_6[2]; /* 0x57c ~ 0x580, reserved 2 * 4 bytes */
    volatile u_viproc_isp_be_ldci_lpf_addr_low   ldci_lpf_addr_low; /* 0x584, post_be only */
    volatile u_viproc_isp_be_ldci_lpf_addr_high   ldci_lpf_addr_high; /* 0x588, post_be only */
    volatile u_viproc_isp_be_abwlsc_addr_low   awblsc_addr_low; /* 0x58c, post_be only */
    volatile u_viproc_isp_be_abwlsc_addr_high   awblsc_addr_high; /* 0x590, post_be only */
    volatile u_viproc_isp_out1_finish_line   viproc_out1_finish_line; /* 0x594 */
    /* lut2stt addr */
    volatile u_viproc_isp_be_dcg_lut_addr_low   viproc_isp_dcg_lut_addr_low; /* 0x598, pre_be only */
    volatile u_viproc_isp_be_dcg_lut_addr_high   viproc_isp_dcg_lut_addr_high; /* 0x59c, pre_be only */
    volatile u_viproc_isp_be_dcg_lut_width_word   viproc_isp_dcg_lut_width_word; /* 0x5a0, pre_be only */
    volatile u_viproc_isp_be_dcg1_lut_addr_low   viproc_isp_dcg1_lut_addr_low; /* 0x5a4, pre_be only */
    volatile u_viproc_isp_be_dcg1_lut_addr_high   viproc_isp_dcg1_lut_addr_high; /* 0x5a8, pre_be only */
    volatile u_viproc_isp_be_dcg1_lut_width_word   viproc_isp_dcg1_lut_width_word; /* 0x5ac, pre_be only */
    volatile u_viproc_isp_be_bnr_lut_addr_low   viproc_isp_bnr_lut_addr_low; /* 0x5b0, post_be only */
    volatile u_viproc_isp_be_bnr_lut_addr_high   viproc_isp_bnr_lut_addr_high; /* 0x5b4, post_be only */
    volatile u_viproc_isp_be_bnr_lut_width_word   viproc_isp_bnr_lut_width_word; /* 0x5b8, post_be only */
    volatile u_viproc_isp_be_lsc_lut_addr_low   viproc_isp_lsc_lut_addr_low; /* 0x5bc, post_be only */
    volatile u_viproc_isp_be_lsc_lut_addr_high   viproc_isp_lsc_lut_addr_high; /* 0x5c0 , post_be only */
    volatile u_viproc_isp_be_lsc_lut_width_word   viproc_isp_lsc_lut_width_word; /* 0x5c4, post_be only */
    volatile u_viproc_isp_be_clut_lut_addr_low   viproc_isp_clut_lut_addr_low; /* 0x5c8, post_be only */
    volatile u_viproc_isp_be_clut_lut_addr_high   viproc_isp_clut_lut_addr_high; /* 0x5cc, post_be only */
    volatile u_viproc_isp_be_clut_lut_width_word   viproc_isp_clut_lut_width_word; /* 0x5d0, post_be only */
    volatile u_viproc_isp_be_gamma_lut_addr_low   viproc_isp_gamma_lut_addr_low; /* 0x5d4, post_be only */
    volatile u_viproc_isp_be_gamma_lut_addr_high   viproc_isp_gamma_lut_addr_high; /* 0x5d8, post_be only */
    volatile u_viproc_isp_be_gamma_lut_width_word   viproc_isp_gamma_lut_width_word; /* 0x5dc, post_be only */
    volatile u_viproc_isp_be_dehaze_lut_addr_low   viproc_isp_dehaze_lut_addr_low; /* 0x5e0, post_be only */
    volatile u_viproc_isp_be_dehaze_lut_addr_high   viproc_isp_dehaze_lut_addr_high; /* 0x5e4, post_be only */
    volatile u_viproc_isp_be_dehaze_lut_width_word   viproc_isp_dehaze_lut_width_word; /* 0x5e8, post_be only */
    volatile u_viproc_isp_be_ldci_lut_addr_low   viproc_isp_ldci_lut_addr_low; /* 0x5ec, post_be only */
    volatile u_viproc_isp_be_ldci_lut_addr_high   viproc_isp_ldci_lut_addr_high; /* 0x5f0, post_be only */
    volatile u_viproc_isp_be_ldci_lut_width_word   viproc_isp_ldci_lut_width_word; /* 0x5f4, post_be only */
    volatile unsigned int                       reserved_7[3]; /* 0x5f8 ~ 0x600, reserved 3 * 4 bytes */
    volatile u_viproc_isp_be_sharpen_lut_addr_low   viproc_isp_sharpen_lut_addr_low; /* 0x604, post_be only */
    volatile u_viproc_isp_be_sharpen_lut_addr_high   viproc_isp_sharpen_lut_addr_high; /* 0x608, post_be only */
    volatile u_viproc_isp_be_sharpen_lut_width_word   viproc_isp_sharpen_lut_width_word; /* 0x60c, post_be only */
    volatile unsigned int                       reserved_8[3]; /* 0x610 ~ 0x618, reserved 3 * 4 bytes */
    volatile u_viproc_isp_be_pregammafe_lut_addr_low   viproc_isp_pregammafe_lut_addr_low; /* 0x61c, pre_be only  */
    volatile u_viproc_isp_be_pregammafe_lut_addr_high   viproc_isp_pregammafe_lut_addr_high; /* 0x620, pre_be only  */
    volatile u_viproc_isp_be_pregammafe_lut_width_word viproc_isp_pregammafe_lut_width_word; /* 0x624, pre_be only  */
    volatile u_viproc_isp_be_degammafe_lut_addr_low   viproc_isp_degammafe_lut_addr_low; /* 0x628, post_be only  */
    volatile u_viproc_isp_be_degammafe_lut_addr_high   viproc_isp_degammafe_lut_addr_high; /* 0x62c , post_be only */
    volatile u_viproc_isp_be_degammafe_lut_width_word  viproc_isp_degammafe_lut_width_word; /* 0x630, post_be only  */
    volatile u_viproc_isp_be_dmnr_lut_addr_low   viproc_isp_bnrshp_lut_addr_low; /* 0x634 , post_be only  */
    volatile u_viproc_isp_be_dmnr_lut_addr_high   viproc_isp_bnrshp_lut_addr_high; /* 0x638 , post_be only  */
    volatile u_viproc_isp_be_dmnr_lut_width_word   viproc_isp_bnrshp_lut_width_word; /* 0x63c , post_be only  */
    volatile u_viproc_isp_be_crb_lut_addr_low   viproc_isp_crb_lut_addr_low; /* 0x640, pre_be only   */
    volatile u_viproc_isp_be_crb_lut_addr_high   viproc_isp_crb_lut_addr_high; /* 0x644, pre_be only   */
    volatile u_viproc_isp_be_crb_lut_width_word   viproc_isp_crb_lut_width_word; /* 0x648, pre_be only   */
    volatile unsigned int                       reserved_9[45]; /* 0x64c ~ 0x6fc, reserved 45 * 4 bytes */
} isp_viproc_reg_type;

#endif /* ISP_VIPROC_REG_DEFINE_H */
