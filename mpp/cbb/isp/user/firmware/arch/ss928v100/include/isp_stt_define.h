/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_STT_DEFINE_H
#define ISP_STT_DEFINE_H

/* Define the union u_isp_ae_total_stat_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_total_pixels_stt : 27; /* [26..0]  */
        unsigned int    reserved_0            : 5; /* [31..27]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_stat_rstt;

/* Define the union u_isp_ae_count_stat_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_count_pixels_stt : 31; /* [30..0]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_count_stat_rstt;

/* Define the union u_isp_ae_total_r_aver_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_r_aver_stt : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_r_aver_rstt;
/* Define the union u_isp_ae_total_gr_aver_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_gr_aver_stt : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_gr_aver_rstt;
/* Define the union u_isp_ae_total_gb_aver_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_gb_aver_stt : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_gb_aver_rstt;
/* Define the union u_isp_ae_total_b_aver_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_b_aver_stt : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_b_aver_rstt;
/* Define the union u_isp_ae_hist_high_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_hist_high_stt   : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist_high_rstt;
/* Define the union u_isp_ae_total_ir_aver_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ae_total_ir_aver_stt : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_total_ir_aver_rstt;

/* Define the union u_isp_ae_hist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_hist           : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_hist;

/* Define the union u_isp_ae_aver_r_gr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_aver_gr        : 16; /* [15..0]  */
        unsigned int    isp_ae_aver_r         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_r_gr;

/* Define the union u_isp_ae_aver_gb_b */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_aver_b         : 16; /* [15..0]  */
        unsigned int    isp_ae_aver_gb        : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_gb_b;

/* Define the union u_isp_ae_ir_hist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_ir_hist        : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ir_hist;

/* Define the union u_isp_ae_aver_ir */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_aver_ir        : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_aver_ir;

/* Define the union u_isp_ae_ai_hist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_ai_hist        : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_hist;

/* Define the union u_isp_ae_ai_aver */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_ai_aver        : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_ai_aver;

/* Define the union u_isp_awb_avg_r_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_avg_r_stt     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_avg_r_rstt;

/* Define the union u_isp_awb_avg_g_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_avg_g_stt     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_avg_g_rstt;

/* Define the union u_isp_awb_avg_b_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_avg_b_stt     : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_avg_b_rstt;

/* Define the union u_isp_awb_cnt_all_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awb_count_all_stt : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_cnt_all_rstt;

/* Define the union u_isp_awb_stat */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awb_stat           : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awb_stat;
/* Define the union u_isp_af_stat */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_af_stat_out        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_af_stat;
/* Define the union u_isp_la_aver */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_la_aver_b         : 8; /* [7..0]  */
        unsigned int    isp_la_aver_gb        : 8; /* [15..8]  */
        unsigned int    isp_la_aver_gr        : 8; /* [23..16]  */
        unsigned int    isp_la_aver_r         : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_la_aver;

/* Define the union u_isp_flick_countover_cur_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_countover_cur_rstt : 26; /* [25..0]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_countover_cur_rstt;
/* Define the union u_isp_flick_gr_diff_cur_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gr_diff_cur_rstt : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gr_diff_cur_rstt;

/* Define the union u_isp_flick_gb_diff_cur_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gb_diff_cur_rstt : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gb_diff_cur_rstt;

/* Define the union u_isp_flick_gr_abs_cur_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gr_abs_cur_rstt : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gr_abs_cur_rstt;

/* Define the union u_isp_flick_gb_abs_cur_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gb_abs_cur_rstt : 23; /* [22..0]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gb_abs_cur_rstt;

/* Define the union u_isp_flick_gmean */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_flick_gbmean      : 11; /* [10..0]  */
        unsigned int    isp_flick_grmean      : 11; /* [21..11]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_flick_gmean;

/* Define the union u_isp_fpn_stat_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_fpn_busy_rstt     : 1; /* [0]  */
        unsigned int    reserved_0            : 7; /* [7..1]  */
        unsigned int    isp_fpn_vcnt_rstt     : 6; /* [13..8]  */
        unsigned int    reserved_1            : 2; /* [15..14]  */
        unsigned int    isp_fpn_hcnt_rstt     : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_stat_rstt;

/* Define the union u_isp_fpn_sum0_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpn_sum0_rstt      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_sum0_rstt;
/* Define the union u_isp_fpn_sum1_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_fpn_sum1_rstt      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_fpn_sum1_rstt;
/* Define the union u_isp_dpc_bpt_calib_number_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_bpt_calib_number_rstt : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_calib_number_rstt;

/* Define the union u_isp_dpc_bpt_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_posx_rstt     : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc_posy_rstt     : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_bpt_rstt;

/* Define the union U_ISP_LDCI_GROBAL_MAP_RSTT */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_grobal_luma_rstt : 10; /* [9..0]  */
        unsigned int    isp_ldci_grobal_poply2_rstt : 10; /* [19..10]  */
        unsigned int    isp_ldci_grobal_poply3_rstt : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_grobal_map_rstt;

/* Define the union u_isp_ldci_lpf_map_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_lpflitmap_rstt : 10; /* [9..0]  */
        unsigned int    isp_ldci_lpfplyp2map_rstt : 10; /* [19..10]  */
        unsigned int    isp_ldci_lpfplyp3map_rstt : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lpf_map_rstt;

/* Define the union u_isp_dehaze_minstat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_minstat_l  : 10; /* [9..0]  */
        unsigned int    reserved_0            : 6; /* [15..10]  */
        unsigned int    isp_dehaze_minstat_h  : 10; /* [25..16]  */
        unsigned int    reserved_1            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_minstat;

/* Define the union u_isp_dehaze_maxstat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_maxstat    : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_maxstat;

/* Define the union u_isp_y_sum0_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_y_sum0             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_y_sum0_rstt;
/* Define the union u_isp_y_sum1_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_y_sum1             : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_y_sum1_rstt;

/* Define the union u_isp_awblsc_avg_r_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_avg_r_stt  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_avg_r_rstt;

/* Define the union u_isp_awblsc_avg_g_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_avg_g_stt  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_avg_g_rstt;

/* Define the union u_isp_awblsc_avg_b_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_avg_b_stt  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_avg_b_rstt;

/* Define the union u_isp_awblsc_cnt_all_rstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_awblsc_count_all_stt : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_cnt_all_rstt;

/* Define the union u_isp_awblsc_stat */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_awblsc_stat        : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_awblsc_stat;

/* Define the union u_isp_rgbir_ir_sum_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rgbir_ir_sum       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_ir_sum_rstt;
/* Define the union u_isp_rgbir_auto_gain_o_rstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_rgbir_auto_gain_o  : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_rgbir_auto_gain_o_rstt;

typedef struct {
    volatile u_isp_ae_total_stat_rstt     isp_ae_total_stat_rstt;    /* 0x0 */
    volatile u_isp_ae_count_stat_rstt     isp_ae_count_stat_rstt;    /* 0x4 */
    volatile u_isp_ae_total_r_aver_rstt   isp_ae_total_r_aver_rstt;  /* 0x8 */
    volatile u_isp_ae_total_gr_aver_rstt  isp_ae_total_gr_aver_rstt; /* 0xc */
    volatile u_isp_ae_total_gb_aver_rstt  isp_ae_total_gb_aver_rstt; /* 0x10 */
    volatile u_isp_ae_total_b_aver_rstt   isp_ae_total_b_aver_rstt;  /* 0x14 */
    volatile u_isp_ae_hist_high_rstt      isp_ae_hist_high_rstt;     /* 0x18 */
    volatile u_isp_ae_total_ir_aver_rstt  isp_ae_total_ir_aver_rstt; /* 0x1c */
    volatile u_isp_ae_hist                isp_ae_hist[1024];         /* 0x20~0x101c, ae hist size: 1024 */
    volatile u_isp_ae_aver_r_gr           isp_ae_aver_r_gr[1024];    /* 0x1020~0x201c, ae aver size: 1024(only 256) */
    volatile u_isp_ae_aver_gb_b           isp_ae_aver_gb_b[1024];    /* 0x2020~0x301c, ae aver size: 1024(only 256) */
    volatile u_isp_ae_ir_hist             isp_ae_ir_hist[1024];    /* 0x3020~0x401c, ae ir hist size: 1024, only fe0 */
    volatile u_isp_ae_aver_ir             isp_ae_aver_ir[1024];    /* 0x4020~0x501c, ae aver ir size: 256, only fe0 */
    volatile u_isp_ae_ai_hist             isp_ae_ai_hist[128];  /* 0x5020~0x521c, ae ai hist size: 128, not support */
    volatile u_isp_ae_ai_aver             isp_ae_ai_aver[16];   /* 0x5220~0x525c, ae ai aver size: 16, not support */
} isp_be_offline_ae_stat_type;

typedef struct {
    isp_be_offline_ae_stat_type              isp_ae_atat;                   /* 0x0 ~ 0x0x525c */
    volatile unsigned int                    reserved_0[3328];              /* 0x5260~0x865c, reserved 3328 * 4 bytes */
    volatile u_isp_flick_countover_cur_rstt  isp_flick_countover_cur_rstt;  /* 0x8660 */
    volatile u_isp_flick_gr_diff_cur_rstt    isp_flick_gr_diff_cur_rstt;    /* 0x8664 */
    volatile u_isp_flick_gb_diff_cur_rstt    isp_flick_gb_diff_cur_rstt;    /* 0x8668 */
    volatile u_isp_flick_gr_abs_cur_rstt     isp_flick_gr_abs_cur_rstt;     /* 0x866c */
    volatile u_isp_flick_gb_abs_cur_rstt     isp_flick_gb_abs_cur_rstt;     /* 0x8670 */
    volatile u_isp_flick_gmean               isp_flick_gmean[512];          /* 0x8674~0x8e70, flicker gmean size: 512 */
    volatile u_isp_fpn_stat_rstt             isp_fpn_stat_rstt;             /* 0x8e74 */
    volatile u_isp_fpn_sum0_rstt             isp_fpn_sum0_rstt;             /* 0x8e78 */
    volatile u_isp_fpn_sum1_rstt             isp_fpn_sum1_rstt;             /* 0x8e7c */
    volatile u_isp_dpc_bpt_calib_number_rstt isp_dpc_bpt_calib_number_rstt; /* 0x8e80 */
    volatile u_isp_dpc_bpt_rstt              isp_dpc_bpt_rstt[2048];     /* 0x8e84~0xae80, dpfect pixel number: 2048 */
    volatile unsigned int                    reserved_1[2561];              /* 0xae84~0xd684, reserved 2561*4 bytes  */
    volatile u_isp_y_sum0_rstt               isp_y_sum0_rstt;               /* 0xd688 */
    volatile u_isp_y_sum1_rstt               isp_y_sum1_rstt;               /* 0xd68c */
    volatile unsigned int                    reserved_2[2052];              /* 0xd690~0xf69c, reserved 2052 * 4 bytes */
    volatile u_isp_rgbir_ir_sum_rstt         isp_rgbir_ir_sum_rstt;         /* 0xf6a0 */
    volatile u_isp_rgbir_auto_gain_o_rstt    isp_rgbir_auto_gain_o_rstt;    /* 0xf6a4 */
    volatile unsigned int                    reserved_3[22];                /* 0xf6a8 ~ 0xf6fc, reserved 22 * 4 bytes */
} isp_pre_be_offline_stat_type;

typedef struct {
    isp_be_offline_ae_stat_type          isp_ae_atat;                 /* 0x0 ~ 0x0x525c */
    volatile u_isp_awb_avg_r_rstt        isp_awb_avg_r_rstt;          /* 0x5260 */
    volatile u_isp_awb_avg_g_rstt        isp_awb_avg_g_rstt;          /* 0x5264 */
    volatile u_isp_awb_avg_b_rstt        isp_awb_avg_b_rstt;          /* 0x5268 */
    volatile u_isp_awb_cnt_all_rstt      isp_awb_cnt_all_rstt;        /* 0x526c */
    volatile u_isp_awb_stat              isp_awb_stat[2048];          /* 0x5270~0x726c, awb stat lut size: 2048 */
    volatile u_isp_af_stat               isp_af_stat[1020];           /* 0x7270~0x825c, af stat lut size: 1020 */
    volatile u_isp_la_aver               isp_la_aver[256];            /* 0x8260~0x865c, la aver size: 256 */
    volatile unsigned int                reserved_0[2569];            /* 0x8660~0xae80, reserved 2569 * 4 bytes */
    volatile u_isp_ldci_grobal_map_rstt  isp_ldci_grobal_map_rstt;    /* 0xae84 */
    volatile u_isp_ldci_lpf_map_rstt     isp_ldci_lpf_map_rstt[1024]; /* 0xae88~0xbe84, ldci lpf map size:1024 */
    volatile u_isp_dehaze_minstat        isp_dehaze_minstat[512];     /* 0xbe88~0xc684, dehaze min stat size: 512 */
    volatile u_isp_dehaze_maxstat        isp_dehaze_maxstat[1024];    /* 0xc688~0xd684, dehaze max stat size: 1024 */
    volatile u_isp_y_sum0_rstt           isp_y_sum0_rstt;             /* 0xd688 */
    volatile u_isp_y_sum1_rstt           isp_y_sum1_rstt;             /* 0xd68c */
    volatile u_isp_awblsc_avg_r_rstt     isp_awblsc_avg_r_rstt;       /* 0xd690 */
    volatile u_isp_awblsc_avg_g_rstt     isp_awblsc_avg_g_rstt;       /* 0xd694 */
    volatile u_isp_awblsc_avg_b_rstt     isp_awblsc_avg_b_rstt;       /* 0xd698 */
    volatile u_isp_awblsc_cnt_all_rstt   isp_awblsc_cnt_all_rstt;     /* 0xd69c */
    volatile u_isp_awblsc_stat           isp_awblsc_stat[2048];       /* 0xd6a0~0xf69c, awblsc stat lut size: 2048 */
} isp_post_be_offline_stat_type;

/* Define the global struct */
typedef struct {
    isp_pre_be_offline_stat_type  pre_be_stat;   /* align 256 */
    isp_post_be_offline_stat_type post_be_stat;
} isp_be_offline_stat_reg_type;

/* Define the global struct */
typedef struct {
    isp_be_offline_ae_stat_type          isp_ae_atat;             /* 0x0 ~ 0x0x525c */
    volatile u_isp_awb_avg_r_rstt        isp_awb_avg_r_rstt;      /* 0x5260 */
    volatile u_isp_awb_avg_g_rstt        isp_awb_avg_g_rstt;      /* 0x5264 */
    volatile u_isp_awb_avg_b_rstt        isp_awb_avg_b_rstt;      /* 0x5268 */
    volatile u_isp_awb_cnt_all_rstt      isp_awb_cnt_all_rstt;    /* 0x526c */
    volatile u_isp_awb_stat              isp_awb_stat[2048];      /* 0x5270~0x726c, awb stat lut size: 2048 */
} isp_stitch_stat_reg_type;

typedef struct {
    volatile u_isp_ae_hist           isp_ae_hist[1024];           /* 0x0~0xffc, ae hist size: 1024 */
    volatile u_isp_ae_aver_r_gr      isp_ae_aver_r_gr[256];       /* 0x1000~0x13fc, ae aver size: 256  */
    volatile u_isp_ae_aver_gb_b      isp_ae_aver_gb_b[256];       /* 0x1400~0x17fc, ae aver size: 256  */
    volatile u_isp_af_stat           isp_af_stat[1020];           /* 0x1800 ~ 0x27ec, af stat size:1020 */
    volatile unsigned int            reserved_1[4];               /* 0x27ef0 ~ 0x27fc, reserved 4 * 4 bytes */
    volatile u_isp_awb_stat          isp_awb_stat[2048];          /* 0x2800~0x47fc, awb stat lut size: 2048  */
    volatile u_isp_la_aver           isp_la_aver[256];            /* 0x4800~0x4bfc, la aver lut size: 256  */
    volatile u_isp_dehaze_minstat    isp_dehaze_minstat[512];     /* 0x4c00~0x53fc, dehaze min stat size: 512 */
    volatile u_isp_dehaze_maxstat    isp_dehaze_maxstat[1024];    /* 0x5400~0x63fc, dehaze max stat size: 1024 */
    volatile u_isp_ldci_lpf_map_rstt isp_ldci_lpf_map_rstt[1024]; /* 0x6400~0x73fc, ldci lpf map size:1024  */
    volatile u_isp_awblsc_stat       isp_awblsc_stat[2048];       /* 0x7400 ~ 0x93fc , awblsc lut size: 2048 */
} isp_be_online_stat_reg_type;

typedef struct {
    volatile u_isp_ae_hist          isp_ae_hist[1024];      /* 0x0 ~ 0xffc, ae hist size: 1024 */
    volatile u_isp_ae_aver_r_gr     isp_ae_aver_r_gr[256];  /* 0x1000 ~ 0x13fc, ae aver size: 256 */
    volatile u_isp_ae_aver_gb_b     isp_ae_aver_gb_b[256];  /* 0x1400 ~ 0x17fc, ae aver size: 256 */
    volatile u_isp_ae_ir_hist       isp_ae_ir_hist[1024];   /* 0x1800 ~ 0x27fc, ae ir hist size: 1024, only fe0 */
    volatile u_isp_ae_aver_ir       isp_ae_aver_ir[256];    /* 0x2800 ~ 0x2bfc, ae aver ir size: 256, only fe0 */
    volatile u_isp_af_stat          isp_af_stat[1020];      /* 0x2c00 ~ 0x3bec, af stat size:1020 */
    volatile unsigned int           reserved_1[4];          /* 0x3bf0 ~ 0x3bfc, reserved 4 * 4 bytes */
} isp_fe_stat_reg_type;

#endif /* ISP_STT_DEFINE_H */
