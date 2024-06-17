/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_LUT_DEFINE_H
#define ISP_LUT_DEFINE_H

/* Define the union u_isp_dpc_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc_lut_wstt_x    : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc_lut_wstt_y    : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc_lut_wstt;

/* Define the union u_isp_dpc1_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dpc1_lut_wstt_x   : 13; /* [12..0]  */
        unsigned int    reserved_0            : 3; /* [15..13]  */
        unsigned int    isp_dpc1_lut_wstt_y   : 13; /* [28..16]  */
        unsigned int    reserved_1            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dpc1_lut_wstt;

/* Define the union u_isp_prelsc_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_prelsc_lut_wstt   : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_prelsc_lut_wstt;

/* Define the union u_isp_pregammafe_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregammafe_lut_wstt : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregammafe_lut_wstt;

/* Define the union u_isp_crb_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_crb_lut_wstt      : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_crb_lut_wstt;

typedef struct {
    volatile u_isp_dpc_lut_wstt        isp_dpc_lut_wstt[2048];       /* 0 ~ 0x1FFC, dpc lut size 2048 */
    volatile u_isp_dpc1_lut_wstt       isp_dpc1_lut_wstt[2048];      /* 0x2000 ~ 0x3FFC, dpc1 lut size 2048 */
    volatile u_isp_pregammafe_lut_wstt isp_pregammafe_lut_wstt[257]; /* 0x4000 ~ 0x4400, pregammafe lut size 257 */
    volatile unsigned int              reserved_1[63];               /* 0x4404 ~ 0x44fc, reserved 63 * 4 bytes */
    volatile u_isp_crb_lut_wstt        isp_crb_lut_wstt[1024];       /* 0x4500 ~ 0x54fc, crb lut size 1024 */
} isp_pre_be_lut_wstt_type;

/* apb lut */
/* Define the union u_isp_wdr_snoise_lut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_snoise_profile : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_snoise_lut;

/* Define the union u_isp_wdr_lnoise_lut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_lnoise_profile : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_lnoise_lut;

/* Define the union u_isp_wdr_normalize_lut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_wdr_sigma_nm_lut  : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_wdr_normalize_lut;

/* Define the union u_isp_ae_weight */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ae_weight0        : 4; /* [3..0]  */
        unsigned int    reserved_0            : 4; /* [7..4]  */
        unsigned int    isp_ae_weight1        : 4; /* [11..8]  */
        unsigned int    reserved_1            : 4; /* [15..12]  */
        unsigned int    isp_ae_weight2        : 4; /* [19..16]  */
        unsigned int    reserved_2            : 4; /* [23..20]  */
        unsigned int    isp_ae_weight3        : 4; /* [27..24]  */
        unsigned int    reserved_3            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ae_weight;

/* Define the union u_isp_expander_lut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_expander_lut      : 21; /* [20..0]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_expander_lut;

/* 0x0 ~ 0x11b0 para0 */
/* 0x11c0 ~ 0x19cc para1 */
/* 0x19d0 ~ 0x1cf0 para2 */
/* 0x1d00 ~ 0x270c para3 */
typedef struct {
    volatile u_isp_wdr_snoise_lut    isp_wdr_snoise_lut[257];     /* 0x0~0x400, wdr snoise lut size 257 */
    volatile unsigned int            reserved_0[3];               /* 0x404~0x40c, reserved 3 * 4 bytes  */
    volatile u_isp_wdr_lnoise_lut    isp_wdr_lnoise_lut[257];     /* 0x410~0x810, wdr lnoise lut size 257 */
    volatile unsigned int            reserved_1[3];               /* 0x814~0x81c, reserved 3 * 4 bytes */
    volatile u_isp_wdr_normalize_lut isp_wdr_normalize_lut[513];  /* 0x820~0x1020, wdr normalize lut size 513 */
    volatile unsigned int            reserved_2[3];               /* 0x1024~0x102c, reserved 3 * 4 bytes */
    volatile u_isp_ae_weight         isp_ae_weight[64];           /* 0x1030~0x112c, ae weight lut size 64 */
    volatile unsigned int            reserved_3[36];              /* 0x1130~0x11bc, reserved 36 * 4 bytes */
    volatile u_isp_expander_lut      isp_expander_lut[257];       /* 0x11c0~0x15c0, expander lut size  257 */
    volatile unsigned int            reserved_4[1167];            /* 0x15c4~0x27fc, reserved 1167 * 4 bytes */
} isp_pre_be_lut_apb_type;

/* Define the union u_isp_clut_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_clut_lut_wstt     : 30; /* [29..0]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_clut_lut_wstt;

/* Define the union u_isp_dehaze_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_dehaze_lut_wstt   : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_dehaze_lut_wstt;

/* Define the union u_isp_bnr_lut_wstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnr_lut_wstt       : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnr_lut_wstt;
/* Define the union u_isp_gamma_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_gamma_lut_wstt    : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_gamma_lut_wstt;

/* Define the union u_isp_sharpen_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_sharpen_lut_wstt  : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_sharpen_lut_wstt;

/* Define the union u_isp_lsc_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_lsc_lut_wstt      : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_lsc_lut_wstt;

/* Define the union u_isp_ldci_lut_wstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_ldci_lut_wstt      : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_lut_wstt;
/* Define the union u_isp_pregamma_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_pregamma_lut_wstt : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_pregamma_lut_wstt;

/* Define the union u_isp_degammafe_lut_wstt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_degammafe_lut_wstt : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_degammafe_lut_wstt;

/* Define the union u_isp_bnrshp_lut_wstt */
typedef union {
    /* Define the struct bits  */
    struct {
        unsigned int isp_bnrshp_lut_wstt    : 32; /* [31..0]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_bnrshp_lut_wstt;

typedef struct {
    volatile u_isp_clut_lut_wstt      isp_clut_lut_wstt[5508];     /* 0x0 ~ 0x560c, clut lut2stt size  5508 */
    volatile unsigned int             reserved_0[60];              /* 0x5610 ~ 0x56fc, reserved 60 * 4 bytes */
    volatile u_isp_dehaze_lut_wstt    isp_dehaze_lut_wstt[1280];   /* 0x5700 ~ 0x6afc, dehaze lut2stt size  1280 */
    volatile u_isp_bnr_lut_wstt       isp_bnr_lut_wstt[1556];      /* 0x6b00 ~ 0x834c, bnr lut2stt size  1556 */
    volatile unsigned int             reserved_1[44];              /* 0x8350 ~ 0x83fc, reserved 44 * 4 bytes */
    volatile u_isp_gamma_lut_wstt     isp_gamma_lut_wstt[257];     /* 0x8400 ~ 0x8800, gamma lut2stt size  257 */
    volatile unsigned int             reserved_2[63];              /* 0x8804 ~ 0x88fc, reserved 63 * 4 bytes */
    volatile u_isp_sharpen_lut_wstt   sharp_lut_wstt[256];         /* 0x8900 ~ 0x8cfc, sharpen lut2stt size  256 */
    volatile u_isp_lsc_lut_wstt       isp_lsc_lut_wstt[4356];      /* 0x8d00 ~ 0xd10c, lsc lut2stt size  4356 */
    volatile unsigned int             reserved_3[60];              /* 0xd110 ~ 0xd1fc, reserved 60 * 4 bytes */
    volatile u_isp_ldci_lut_wstt      isp_ldci_lut_wstt[260];      /* 0xd200 ~ 0xd60c, ldci lut2stt size  260 */
    volatile unsigned int             reserved_5[60];              /* 0xd610 ~ 0xd6fc, reserved 60 * 4 bytes */
    volatile u_isp_degammafe_lut_wstt isp_degammafe_lut_wstt[257]; /* 0xd700 ~ 0xdb00, degamma lut2stt size  257 */
    volatile unsigned int             reserved_7[63];              /* 0xdb04 ~ 0xdbfc, reserved 63 * 4 bytes */
    volatile u_isp_bnrshp_lut_wstt    isp_bnrshp_lut_wstt[128];    /* 0xdc00 ~ 0xddfc, bnrshp lut2stt size  128 */
} isp_post_be_lut_wstt_type;

/* Define the union u_isp_demosaic_depurplut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_demosaic_depurp_lut : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_demosaic_depurplut;

/* Define the union u_isp_nddm_gf_lut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_nddm_gflut        : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_nddm_gf_lut;

/* Define the union u_isp_ca_lut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ca_lut            : 24; /* [23..0]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ca_lut;

/* Define the union u_isp_ldci_polyp_wlut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_poply1_wlut  : 10; /* [9..0]  */
        unsigned int    isp_ldci_poply2_wlut  : 10; /* [19..10]  */
        unsigned int    isp_ldci_poply3_wlut  : 10; /* [29..20]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyp_wlut;

/* Define the union u_isp_ldci_polyq01_wlut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_plyq0_wlut   : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ldci_plyq1_wlut   : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq01_wlut;

/* Define the union u_isp_ldci_polyq23_wlut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_ldci_plyq2_wlut   : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    isp_ldci_plyq3_wlut   : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_ldci_polyq23_wlut;

/* Define the union u_isp_adrc_tmlut0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_tmlut0_diff  : 14; /* [13..0]  */
        unsigned int    isp_adrc_tmlut0_value : 16; /* [29..14]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_tmlut0;

/* Define the union u_isp_adrc_cclut */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_cclut        : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_cclut;

/* Define the union u_isp_adrc_pregamma */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_pregamma     : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_pregamma;

/* Define the union u_isp_adrc_localmix */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_localmix_dark0 : 8; /* [7..0]  */
        unsigned int    isp_adrc_localmix_bright0 : 8; /* [15..8]  */
        unsigned int    isp_adrc_localmix_dark1 : 8; /* [23..16]  */
        unsigned int    isp_adrc_localmix_bright1 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_localmix;

/* Define the union u_isp_adrc_mlut0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_mlut0_value  : 16; /* [15..0]  */
        unsigned int    isp_adrc_mlut0_diff   : 12; /* [27..16]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut0;

/* Define the union u_isp_adrc_mlut1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_mlut1_value  : 16; /* [15..0]  */
        unsigned int    isp_adrc_mlut1_diff   : 12; /* [27..16]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut1;

/* Define the union u_isp_adrc_mlut2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_mlut2_value  : 16; /* [15..0]  */
        unsigned int    isp_adrc_mlut2_diff   : 12; /* [27..16]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut2;

/* Define the union u_isp_adrc_mlut3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    isp_adrc_mlut3_value  : 16; /* [15..0]  */
        unsigned int    isp_adrc_mlut3_diff   : 12; /* [27..16]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    u32;
} u_isp_adrc_mlut3;

typedef struct {
    volatile unsigned int             reserved_10[1036];            /* 0x0~0x102c, reserved 1036 * 4 bytes  */
    volatile u_isp_ae_weight          isp_ae_weight[64];            /* 0x1030~0x112c, ae weight lut size 64 */
    volatile u_isp_demosaic_depurplut isp_demosaic_depurplut[16];   /* 0x1130~0x116c, lcac lut size  16 */
    volatile u_isp_nddm_gf_lut        isp_nddm_gf_lut[17];          /* 0x1170~0x11b0, nddm gf lut size 17 */
    volatile unsigned int             reserved_0[263];              /* 0x11b4~0x15cc, reserved 263 * 4 bytes */
    volatile u_isp_ca_lut             isp_ca_lut[256];              /* 0x15d0~0x19cc, ca lut size  256 */
    volatile u_isp_ldci_polyp_wlut    isp_ldci_polyp_wlut[65];      /* 0x19d0~0x1ad0, ldci polyp lut size 65 */
    volatile unsigned int             reserved_1[3];                /* 0x1ad4~0x1adc, reserved 3 * 4 bytes */
    volatile u_isp_ldci_polyq01_wlut  isp_ldci_polyq01_wlut[65];    /* 0x1ae0~0x1be0, ldci polyq01 lut size 65  */
    volatile unsigned int             reserved_2[3];                /* 0x1be4~0x1bec, reserved 3 * 4 bytes */
    volatile u_isp_ldci_polyq23_wlut  isp_ldci_polyq23_wlut[65];    /* 0x1bf0~0x1cf0, ldci poly23 lut size 65 */
    volatile unsigned int             reserved_3[3];                /* 0x1cf4~0x1cfc, reserved 3 * 4 bytes */
    volatile u_isp_adrc_tmlut0        isp_adrc_tmlut0[200];         /* 0x1d00~0x201c, adrc tm lut0 size 200 */
    volatile u_isp_adrc_cclut         isp_adrc_cclut[33];           /* 0x2020~0x20a0, adrc cclut size 33 */
    volatile unsigned int             reserved_4[3];                /* 0x20a4~0x20ac, reserved 3 * 4 bytes */
    volatile u_isp_adrc_pregamma      isp_adrc_pregamma[257];       /* 0x20b0~0x24b0, adrc pregamma size 257 */
    volatile unsigned int             reserved_5[3];                /* 0x24b4~0x24bc, reserved 3 * 4 bytes */
    volatile u_isp_adrc_localmix      isp_adrc_localmix[33];        /* 0x24c0~0x2540, adrc localmix size 33 */
    volatile unsigned int             reserved_6[3];                /* 0x2544~0x254c, reserved 3 * 4 bytes */
    volatile u_isp_adrc_mlut0         isp_adrc_mlut0[28];           /* 0x2550~0x25bc, adrc mlut0 size 28 */
    volatile u_isp_adrc_mlut1         isp_adrc_mlut1[28];           /* 0x25c0~0x262c, adrc mlut1 size 28  */
    volatile u_isp_adrc_mlut2         isp_adrc_mlut2[28];           /* 0x2630~0x269c, adrc mlut2 size 28  */
    volatile u_isp_adrc_mlut3         isp_adrc_mlut3[28];           /* 0x26a0~0x270c, adrc mlut3 size 28  */
    volatile unsigned int             reserved_7[60];               /* 0x2710~0x27fc, reserved 60 * 4 bytes */
} isp_post_be_lut_apb_type;

typedef struct {
    isp_pre_be_lut_wstt_type  pre_be_lut2stt;   /* 0x0 ~ 0x54fc, size  0x5500 */
    isp_post_be_lut_wstt_type post_be_lut2stt;  /* 0x5500 ~ 0x137fc, size  0xe300 */
} isp_be_lut_wstt_type;

typedef struct {
    isp_pre_be_lut_apb_type   pre_be_apb_lut;   /* 0x0 ~ 0x27fc, size  0x2800 */
    isp_pre_be_lut_wstt_type  pre_be_lut2stt;   /* 0x2800 ~ 0x7cfc, size  0x5500 */
} isp_pre_be_lut_type;

typedef struct {
    isp_post_be_lut_apb_type   post_be_apb_lut;   /* 0x0 ~ 0x27fc, size  0x2800 */
    isp_post_be_lut_wstt_type  post_be_lut2stt;   /* 0x2800 ~ 0x105fc, size  0xde00 */
} isp_post_be_lut_type;

#endif /* ISP_LUT_DEFINE_H */
