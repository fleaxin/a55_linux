/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_isp_debug.h"
#include "isp_ext_config.h"
#include "isp_math_utils.h"
#include "isp_alg.h"
#include "isp_sensor.h"
#include "isp_proc.h"
#include "isp_param_check.h"
#include "isp_ext_reg_access.h"

#define BSHP_CHROM_CTRL_DEFAULT      1
#define BSHP_LUMA_CTRL_DEFAULT       1
#define BSHP_SHTCTRL_BY_VAR_DEFAULT  1
#define BSHP_BLKCTRL_MODE_DEFAULT    1
#define BSHP_DIRDIFF_SFT_DEFAULT     10
#define BSHP_NEGMF_SFT_DEFAULT       0
#define BSHP_NEGHF_SFT_DEFAULT       0
#define BSHP_SHTVAR_SFT_DEFAULT      5
#define BSHP_RECNEG_CTRL_DEFAULT     0
#define BSHP_DIRRLY_LOW_DEFAULT      127
#define BSHP_DIRRLY_HIGH_DEFAULT     0
#define BSHP_DIRRT_LOW_DEFAULT       4
#define BSHP_DIRRT_HIGH_DEFAULT      25
#define BSHP_NEGLUMA_THD0_LOW_DEFAULT     45
#define BSHP_NEGLUMA_THD0_HIGH_DEFAULT    80
#define BSHP_NEGLUMA_THD1_LOW_DEFAULT     45
#define BSHP_NEGLUMA_THD1_HIGH_DEFAULT    80
#define BSHP_NEGLUMA_WGT0_LOW_DEFAULT     1
#define BSHP_NEGLUMA_WGT0_HIGH_DEFAULT    150
#define BSHP_NEGLUMA_WGT1_LOW_DEFAULT     35
#define BSHP_NEGLUMA_WGT1_HIGH_DEFAULT    220
#define BSHP_NEGMF_THD_LOW_DEFAULT        10
#define BSHP_NEGMF_THD_HIGH_DEFAULT       50
#define BSHP_NEGMF_WGT_LOW_DEFAULT        127
#define BSHP_NEGMF_WGT_HIGH_DEFAULT       50
#define BSHP_NEGHF_THD_LOW_DEFAULT        10
#define BSHP_NEGHF_THD_HIGH_DEFAULT       50
#define BSHP_NEGHF_WGT_LOW_DEFAULT        127
#define BSHP_NEGHF_WGT_HIGH_DEFAULT       50
#define BSHP_SHTVAR_DIFF_THD_LOW_DEFAULT   27
#define BSHP_SHTVAR_DIFF_THD_HIGH_DEFAULT  50
#define BSHP_SHTVAR_DIFF_WGT_LOW_DEFAULT   127
#define BSHP_SHTVAR_DIFF_WGT_HIGH_DEFAULT  5
#define BSHP_RECNEG_CTRL_THD_LOW_DEFAULT   20
#define BSHP_RECNEG_CTRL_THD_HIGH_DEFAULT  50
#define BSHP_RECNEG_CTRL_WGT_LOW_DEFAULT   60
#define BSHP_RECNEG_CTRL_WGT_HIGH_DEFAULT  60
#define BSHP_LUMA_WGT_VALUE_DEFAULT        31
#define BSHP_GAIN_VALUE_DEFAULT            180
#define BSHP_LMF_MF_VALUE_DEFAULT          2
#define BSHP_LMF_HF_VALUE_DEFAULT          1
#define BSHP_SEL_PIX_WGT_VALUE_DEFAULT     0
#define BSHP_OMAX_GAIN_DEFAULT             22
#define BSHP_UMAX_GAIN_DEFAULT             22
#define BSHP_OSHTAMT_DEFAULT               50
#define BSHP_USHTAMT_DEFAULT               20
#define BSHP_OMAXCHG_DEFAULT               900
#define BSHP_UMAXCHG_DEFAULT               900
#define BSHP_DETAILOSHT_AMT_DEFAULT        85
#define BSHP_DETAILUSHT_AMT_DEFAULT        100
#define BSHP_SHPONG_THD_LOW_DEFAULT        190
#define BSHP_SHPONG_THD_HIGH_DEFAULT       210
#define BSHP_SHPONG_WGT_LOW_DEFAULT        80
#define BSHP_SHPONG_WGT_HIGH_DEFAULT       127
#define BSHP_NEGLUMA_MUL_PRECS             4
#define BSHP_NEGMF_MUL_PRECS               4
#define BSHP_NEGHF_MUL_PRECS               4
#define BSHP_SHTVAR_MUL_PRECS              4
#define BSHP_RECNEG_CTRL_MUL_PRECS         4
#define BSHP_SHPONG_MUL_PRECS              4
#define OT_ISP_GAIN_LUT_SFT                3

#define BSHP_BIT_DEPTH           12
#define BSHP_VAR_BIT_DEPTH       12

#define BSHP_HSF_COEF_D_SFT      9
#define BSHP_HSF_COEF_UD_SFT     5
#define BSHP_LPF_COEF_D_SFT      5
#define BSHP_LPF_COEF_UD_SFT     5
#define BSHP_U8GAIN_THD_SEL_D    2
#define BSHP_U8GAIN_THD_SEL_UD   2
#define BSHP_U8GAIN_THD_SFT_D    0
#define BSHP_U8GAIN_THD_SFT_UD   0
#define BSHP_MF_GAIN_SFT         5
#define BSHP_HF_GAIN_SFT         5

#define BSHP_THD_NUM             4

#define BSHP_PARAM_THD_NUM       2
#define MAX16B                   65535
#define MIN16B                   (-65536)

typedef struct {
    td_bool init;
    td_bool bayershp_att_update;
    ot_isp_bayershp_manual_attr actual; /* actual param */
    ot_isp_bayershp_attr mpi_cfg;
} isp_bayershp;

static const td_s16 g_lpf_undir_coef[OT_ISP_BSHP_LPF_SIZE] = {5, 7, 8};
static const td_s16 g_lpf_dir_coef[OT_ISP_BSHP_LPF_SIZE]   = {4, 7, 10};
static const td_s16 g_hsf_undir_coef[OT_ISP_BSHP_LPF_SIZE] = {-2, 9, 18};
static const td_s16 g_hsf_0dir_coef[OT_ISP_BSHP_HSF_NUM] = {-1, -2, -3, -2, -1, 12, 27, 36, 27, 12, 31, 72, 96};
static const td_s16 g_hsf_1dir_coef[OT_ISP_BSHP_HSF_NUM] = {-2, -3, -1,  6,  6,  0, 13, 41, 47, 23, 18, 62, 92};
static const td_s16 g_hsf_2dir_coef[OT_ISP_BSHP_HSF_NUM] = {-1, -3,  6, 16, 10, -3, 10, 50, 53, 16,  6, 50, 92};
static const td_s16 g_hsf_3dir_coef[OT_ISP_BSHP_HSF_NUM] = {-2,  0, 18, 23,  6, -3, 13, 62, 47,  6, -1, 41, 92};

static const td_u16 g_omaxchg_amt[OT_ISP_AUTO_ISO_NUM] = {
    1000, 900, 800, 700, 600, 600, 400, 400, 400, 400, 200, 180, 180, 180, 180, 180
};
static const td_u16 g_umaxchg_amt[OT_ISP_AUTO_ISO_NUM] = {
    1000, 900, 800, 700, 600, 600, 400, 400, 400, 400, 200, 200, 200, 200, 200, 200
};
static const td_u16 g_detail_osht_amt[OT_ISP_AUTO_ISO_NUM] = {
    100, 100, 85, 85, 85, 85, 85, 85, 85, 127, 127, 127, 127, 127, 127, 127
};
static const td_u16 g_detail_usht_amt[OT_ISP_AUTO_ISO_NUM] = {
    100, 100, 100, 100, 100, 100, 100, 100, 100, 127, 127, 127, 127, 127, 127, 127
};
static const td_u16 g_omax_gain[OT_ISP_AUTO_ISO_NUM] = {
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 32, 32
};
static const td_u16 g_umax_gain[OT_ISP_AUTO_ISO_NUM] = {
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 32, 32
};

static const td_u16 g_sht_var_diff_thd[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {27, 29, 30, 32, 36, 38, 40, 44, 45, 46, 47, 48, 48, 48, 48, 48},
    {50, 52, 53, 55, 58, 58, 61, 62, 63, 64, 65, 65, 65, 65, 65, 65}
};

static const td_u16 g_rec_neg_ctrl_thr[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {20, 35, 45, 55, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65},
    {50, 50, 50, 50, 50, 50, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90}
};
static const td_u16 g_rec_neg_ctrl_wgt[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {60, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65},
    {60, 60, 60, 60, 60, 60, 60, 60, 60, 90, 90, 90, 90, 90, 90, 90}
};

static const td_u8 g_lmt_mf[OT_ISP_BSHP_FILTER_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[0] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[1] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[2] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[3] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[4] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[5] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[6] */
    {2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  /* lmt_mf[7] */
};

static const td_u8 g_lmt_hf[OT_ISP_BSHP_FILTER_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[0] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[1] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[2] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[3] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[4] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[5] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, /* lmt_mf[6] */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  /* lmt_mf[7] */
};

static const td_u16 g_neg_luma_thd0[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45},
    {80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80}
};
static const td_u16 g_neg_luma_wgt0[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
    {150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150}
};
static const td_u16 g_neg_luma_thd1[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45,  45},
    {80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80,  80}
};
static const td_u16 g_neg_luma_wgt1[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
    {220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220}
};

static const td_u16 g_neg_mf_thd[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM]  = {
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
    {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50}
};

static const td_u16 g_neg_mf_wgt[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM]  = {
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},
    {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50}
};

static const td_u16 g_neg_hf_thd[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM]  = {
    {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
    {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50}
};
static const td_u16 g_neg_hf_wgt[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM]  = {
    {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127},
    {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50}
};

static const td_u16 g_shp_g_thd[BSHP_PARAM_THD_NUM][OT_ISP_AUTO_ISO_NUM] = {
    {190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190},
    {210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210}
};

isp_bayershp *g_bayershp_ctx[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

#define bayershp_get_ctx(dev, ctx)   ((ctx) = g_bayershp_ctx[dev])
#define bayershp_set_ctx(dev, ctx)   (g_bayershp_ctx[dev] = (ctx))
#define bayershp_reset_ctx(dev)      (g_bayershp_ctx[dev] = TD_NULL)

static td_s32 bayershp_ctx_init(ot_vi_pipe vi_pipe)
{
    isp_bayershp *bayershp_ctx = TD_NULL;

    bayershp_get_ctx(vi_pipe, bayershp_ctx);

    /* isp_malloc memory */
    if (bayershp_ctx == TD_NULL) {
        bayershp_ctx = (isp_bayershp *)isp_malloc(sizeof(isp_bayershp));
        if (bayershp_ctx == TD_NULL) {
            isp_err_trace("isp[%d] bayershp_ctx isp_malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }
    (td_void)memset_s(bayershp_ctx, sizeof(isp_bayershp), 0, sizeof(isp_bayershp));
    bayershp_set_ctx(vi_pipe, bayershp_ctx);

    return TD_SUCCESS;
}

static td_void bayershp_ctx_exit(ot_vi_pipe vi_pipe)
{
    isp_bayershp *sharpen_ctx = TD_NULL;
    bayershp_get_ctx(vi_pipe, sharpen_ctx);
    isp_free(sharpen_ctx);
    bayershp_reset_ctx(vi_pipe);
}

/*  bayershp hardware regs that will not change  */
static td_void bshp_sta_reg_init(ot_vi_pipe vi_pipe, isp_bshp_static_cfg *static_regcfg)
{
    td_u8 i;
    static_regcfg->fil_coef_mode  = 0; /* 0: preset coeffs; 1: adjustable coeffs. */
    ot_unused(vi_pipe);

    static_regcfg->bit_depth      = BSHP_BIT_DEPTH;
    static_regcfg->var_bit_depth  = BSHP_VAR_BIT_DEPTH;
    for (i = 0; i < 3; i++) { /* fliter num is 3 */
        static_regcfg->lpf_coef_d[i]  = g_lpf_dir_coef[i];
        static_regcfg->lpf_coef_ud[i] = g_lpf_undir_coef[i];
        static_regcfg->hsf_coef_ud[i] = g_hsf_undir_coef[i];
    }
    for (i = 0; i < 13; i++) { /* filter num is 13 */
        static_regcfg->hsf_coef0_d[i] = g_hsf_0dir_coef[i];
        static_regcfg->hsf_coef1_d[i] = g_hsf_1dir_coef[i];
        static_regcfg->hsf_coef2_d[i] = g_hsf_2dir_coef[i];
        static_regcfg->hsf_coef3_d[i] = g_hsf_3dir_coef[i];
    }
    static_regcfg->hsf_coef_d_sft    = BSHP_HSF_COEF_D_SFT;
    static_regcfg->hsf_coef_ud_sft   = BSHP_HSF_COEF_UD_SFT;
    static_regcfg->lpf_coef_d_sft    = BSHP_LPF_COEF_D_SFT;
    static_regcfg->lpf_coef_ud_sft   = BSHP_LPF_COEF_UD_SFT;
    static_regcfg->u8gain_thd_sel_d  = BSHP_U8GAIN_THD_SEL_D;
    static_regcfg->u8gain_thd_sel_ud = BSHP_U8GAIN_THD_SEL_UD;
    static_regcfg->u8gain_thd_sft_d  = BSHP_U8GAIN_THD_SFT_D;
    static_regcfg->u8gain_thd_sft_ud = BSHP_U8GAIN_THD_SFT_UD;
    static_regcfg->mf_gain_sft       = BSHP_MF_GAIN_SFT;
    static_regcfg->hf_gain_sft       = BSHP_HF_GAIN_SFT;
    static_regcfg->static_resh       = TD_TRUE;
}

static td_void bshp_user_reg_init(isp_bshp_user_cfg *user_regcfg)
{
    td_u8 i;
    user_regcfg->dir_rly[0]          = BSHP_DIRRLY_LOW_DEFAULT;
    user_regcfg->sht_var_diff_wgt[0] = BSHP_SHTVAR_DIFF_WGT_LOW_DEFAULT;
    user_regcfg->dir_rly[1]          = BSHP_DIRRLY_HIGH_DEFAULT;
    user_regcfg->sht_var_diff_wgt[1] = BSHP_SHTVAR_DIFF_WGT_HIGH_DEFAULT;

    for (i = 0; i < OT_ISP_BSHP_LUMA_WGT_NUM; i++) {
        user_regcfg->luma_wgt[i] =  BSHP_LUMA_WGT_VALUE_DEFAULT;
    }

    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        user_regcfg->gain_d0[i]  =  BSHP_GAIN_VALUE_DEFAULT;
        user_regcfg->gain_ud0[i] =  BSHP_GAIN_VALUE_DEFAULT;
        user_regcfg->gain_d1[i]  =  BSHP_GAIN_VALUE_DEFAULT;
        user_regcfg->gain_ud1[i] =  BSHP_GAIN_VALUE_DEFAULT;
    }
    user_regcfg->o_sht_amt  = BSHP_OSHTAMT_DEFAULT;
    user_regcfg->u_sht_amt  = BSHP_USHTAMT_DEFAULT;
    user_regcfg->shp_on_g_wgt[0] = BSHP_SHPONG_WGT_LOW_DEFAULT;
    user_regcfg->shp_on_g_wgt[1] = BSHP_SHPONG_WGT_HIGH_DEFAULT;
    user_regcfg->user_resh = TD_TRUE;
    return;
}

static td_void bayershp_dyna_reg_init_p2(isp_bshp_dyna_cfg *dyna_regcfg)
{
    td_s32  thd_low, thd_hig, wgt_low, wgt_hig, tmp_mul;
    thd_low = BSHP_NEGLUMA_THD0_LOW_DEFAULT;
    wgt_low = BSHP_NEGLUMA_WGT0_LOW_DEFAULT;
    thd_hig = BSHP_NEGLUMA_THD0_HIGH_DEFAULT;
    wgt_hig = BSHP_NEGLUMA_WGT0_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGLUMA_MUL_PRECS);
    dyna_regcfg->neg_luma_thd0_mul = (td_s16)(clip3(tmp_mul, MIN16B, MAX16B));
    thd_low = BSHP_NEGLUMA_THD1_LOW_DEFAULT;
    wgt_low = BSHP_NEGLUMA_WGT1_LOW_DEFAULT;
    thd_hig = BSHP_NEGLUMA_THD1_HIGH_DEFAULT;
    wgt_hig = BSHP_NEGLUMA_WGT1_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGLUMA_MUL_PRECS);
    dyna_regcfg->neg_luma_thd1_mul = (td_s16)(clip3(tmp_mul, MIN16B, MAX16B));
    thd_low = BSHP_NEGMF_THD_LOW_DEFAULT;
    wgt_low = BSHP_NEGMF_WGT_LOW_DEFAULT;
    thd_hig = BSHP_NEGMF_THD_HIGH_DEFAULT;
    wgt_hig = BSHP_NEGMF_WGT_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGMF_MUL_PRECS);
    dyna_regcfg->neg_mf_mul  = (td_s16)(clip3(tmp_mul, MIN16B, MAX16B));
    thd_low = BSHP_NEGHF_THD_LOW_DEFAULT;
    wgt_low = BSHP_NEGHF_WGT_LOW_DEFAULT;
    thd_hig = BSHP_NEGHF_THD_HIGH_DEFAULT;
    wgt_hig = BSHP_NEGHF_WGT_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGHF_MUL_PRECS);
    dyna_regcfg->neg_hf_mul  = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = BSHP_SHTVAR_DIFF_THD_LOW_DEFAULT;
    wgt_low = BSHP_SHTVAR_DIFF_WGT_LOW_DEFAULT;
    thd_hig = BSHP_SHTVAR_DIFF_THD_HIGH_DEFAULT;
    wgt_hig = BSHP_SHTVAR_DIFF_WGT_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_SHTVAR_MUL_PRECS);
    dyna_regcfg->sht_var_diff_mul = (td_s16)(clip3(tmp_mul, MIN16B, MAX16B));
    thd_low = BSHP_RECNEG_CTRL_THD_LOW_DEFAULT;
    wgt_low = BSHP_RECNEG_CTRL_WGT_LOW_DEFAULT;
    thd_hig = BSHP_RECNEG_CTRL_THD_HIGH_DEFAULT;
    wgt_hig = BSHP_RECNEG_CTRL_WGT_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_RECNEG_CTRL_MUL_PRECS);
    dyna_regcfg->reg_neg_ctrl_mul  = (td_s16)(clip3(tmp_mul, MIN16B, MAX16B));
    thd_low = BSHP_SHPONG_THD_LOW_DEFAULT;
    wgt_low = BSHP_SHPONG_WGT_LOW_DEFAULT;
    thd_hig = BSHP_SHPONG_THD_HIGH_DEFAULT;
    wgt_hig = BSHP_SHPONG_WGT_HIGH_DEFAULT;
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_SHPONG_MUL_PRECS);
    dyna_regcfg->shp_on_g_mul = (td_s16)(clip3(tmp_mul, MIN16B, MAX16B));
}

static td_void bayershp_dyna_reg_init_p1(isp_bshp_dyna_cfg *dyna_regcfg)
{
    td_u8   i;
    dyna_regcfg->chr_ctrl_en        = BSHP_CHROM_CTRL_DEFAULT;
    dyna_regcfg->luma_ctrl_en       = BSHP_LUMA_CTRL_DEFAULT;
    dyna_regcfg->sht_ctrl_by_var_en = BSHP_SHTCTRL_BY_VAR_DEFAULT;
    dyna_regcfg->blk_ctrl_mode      = BSHP_BLKCTRL_MODE_DEFAULT;
    dyna_regcfg->dir_diff_sft       = BSHP_DIRDIFF_SFT_DEFAULT;
    dyna_regcfg->neg_mf_sft         = BSHP_NEGMF_SFT_DEFAULT;
    dyna_regcfg->neg_hf_sft         = BSHP_NEGHF_SFT_DEFAULT;
    dyna_regcfg->sht_var5x5_sft     = BSHP_SHTVAR_SFT_DEFAULT;
    dyna_regcfg->rec_neg_ctrl_sft   = BSHP_RECNEG_CTRL_DEFAULT;
    dyna_regcfg->dir_rt[0]          = BSHP_DIRRT_LOW_DEFAULT;
    dyna_regcfg->neg_luma_thd0[0]   = BSHP_NEGLUMA_THD0_LOW_DEFAULT;
    dyna_regcfg->neg_luma_wgt0[0]   = BSHP_NEGLUMA_WGT0_LOW_DEFAULT;
    dyna_regcfg->neg_luma_thd1[0]   = BSHP_NEGLUMA_THD1_LOW_DEFAULT;
    dyna_regcfg->neg_luma_wgt1[0]   = BSHP_NEGLUMA_WGT1_LOW_DEFAULT;
    dyna_regcfg->neg_mf_thd[0]      = BSHP_NEGMF_THD_LOW_DEFAULT;
    dyna_regcfg->neg_mf_wgt[0]      = BSHP_NEGMF_WGT_LOW_DEFAULT;
    dyna_regcfg->neg_hf_thd[0]      = BSHP_NEGHF_THD_LOW_DEFAULT;
    dyna_regcfg->neg_hf_wgt[0]      = BSHP_NEGHF_WGT_LOW_DEFAULT;
    dyna_regcfg->sht_var_diff_thd[0] = BSHP_SHTVAR_DIFF_THD_LOW_DEFAULT;
    dyna_regcfg->reg_neg_ctrl_thd[0] = BSHP_RECNEG_CTRL_THD_LOW_DEFAULT;
    dyna_regcfg->reg_neg_ctrl_wgt[0] = BSHP_RECNEG_CTRL_WGT_LOW_DEFAULT;
    dyna_regcfg->dir_rt[1]           = BSHP_DIRRT_HIGH_DEFAULT;
    dyna_regcfg->neg_luma_thd0[1]    = BSHP_NEGLUMA_THD0_HIGH_DEFAULT;
    dyna_regcfg->neg_luma_wgt0[1]    = BSHP_NEGLUMA_WGT0_HIGH_DEFAULT;
    dyna_regcfg->neg_luma_thd1[1]    = BSHP_NEGLUMA_THD1_HIGH_DEFAULT;
    dyna_regcfg->neg_luma_wgt1[1]    = BSHP_NEGLUMA_WGT1_HIGH_DEFAULT;
    dyna_regcfg->neg_mf_thd[1]       = BSHP_NEGMF_THD_HIGH_DEFAULT;
    dyna_regcfg->neg_mf_wgt[1]       = BSHP_NEGMF_WGT_HIGH_DEFAULT;
    dyna_regcfg->neg_hf_thd[1]       = BSHP_NEGHF_THD_HIGH_DEFAULT;
    dyna_regcfg->neg_hf_wgt[1]       = BSHP_NEGHF_WGT_HIGH_DEFAULT;
    dyna_regcfg->sht_var_diff_thd[1] = BSHP_SHTVAR_DIFF_THD_HIGH_DEFAULT;
    dyna_regcfg->reg_neg_ctrl_thd[1] = BSHP_RECNEG_CTRL_THD_HIGH_DEFAULT;
    dyna_regcfg->reg_neg_ctrl_wgt[1] = BSHP_RECNEG_CTRL_WGT_HIGH_DEFAULT;
    for (i = 0; i < OT_ISP_BSHP_FILTER_NUM; i++) {
        dyna_regcfg->lmf_mf[i] = BSHP_LMF_MF_VALUE_DEFAULT;
        dyna_regcfg->lmf_hf[i] = BSHP_LMF_HF_VALUE_DEFAULT;
    }
    dyna_regcfg->sel_pix_wgt = BSHP_SEL_PIX_WGT_VALUE_DEFAULT;
    dyna_regcfg->o_max_gain = BSHP_OMAX_GAIN_DEFAULT;
    dyna_regcfg->u_max_gain = BSHP_UMAX_GAIN_DEFAULT;
    dyna_regcfg->o_max_chg  = BSHP_OMAXCHG_DEFAULT;
    dyna_regcfg->u_max_chg  = BSHP_UMAXCHG_DEFAULT;
    dyna_regcfg->detail_o_sht_amt = BSHP_DETAILOSHT_AMT_DEFAULT;
    dyna_regcfg->detail_u_sht_amt = BSHP_DETAILUSHT_AMT_DEFAULT;
    dyna_regcfg->shp_on_g_thd[0] = BSHP_SHPONG_THD_LOW_DEFAULT;
    dyna_regcfg->shp_on_g_thd[1] = BSHP_SHPONG_THD_HIGH_DEFAULT;
}

static td_void bshp_dyna_reg_init(isp_bshp_dyna_cfg *dyna_regcfg)
{
    bayershp_dyna_reg_init_p1(dyna_regcfg);
    bayershp_dyna_reg_init_p2(dyna_regcfg);
    dyna_regcfg->dyna_resh = TD_TRUE;
    dyna_regcfg->buf_id = 0;
    return;
}

static td_void bayershp_ext_reg_init(ot_vi_pipe vi_pipe, isp_bayershp *bayer_sharpen)
{
    ot_ext_system_bshp_manual_mode_write(vi_pipe, bayer_sharpen->mpi_cfg.op_type);
    ot_ext_system_bshp_enable_write(vi_pipe, bayer_sharpen->mpi_cfg.en);

    isp_bayershp_manual_attr_write(vi_pipe, &bayer_sharpen->mpi_cfg.manual_attr);
    isp_bayershp_auto_attr_write(vi_pipe, &bayer_sharpen->mpi_cfg.auto_attr);
}

static td_void bayershp_regs_initialize(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_bayershp *bayer_sharpen)
{
    td_u32 i;

    for (i = 0; i < reg_cfg->cfg_num; i++) {
        reg_cfg->alg_reg_cfg[i].bshp_reg_cfg.bshp_enable = bayer_sharpen->mpi_cfg.en;
        reg_cfg->alg_reg_cfg[i].bshp_reg_cfg.lut2_stt_en  = TD_TRUE;
        bshp_sta_reg_init(vi_pipe, &reg_cfg->alg_reg_cfg[i].bshp_reg_cfg.static_reg_cfg);
        bshp_user_reg_init(&reg_cfg->alg_reg_cfg[i].bshp_reg_cfg.usr_reg_cfg);
        bshp_dyna_reg_init(&reg_cfg->alg_reg_cfg[i].bshp_reg_cfg.dyna_reg_cfg);
    }
    reg_cfg->cfg_key.bit1_bshp_cfg = 1;
}

static td_s32 bshp_check_cmos_param(const ot_isp_bayershp_attr *bshp)
{
    td_s32 ret;

    ret = isp_bshp_comm_attr_check("cmos", bshp);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_bshp_auto_attr_check("cmos", &bshp->auto_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_bshp_manual_attr_check("cmos", &bshp->manual_attr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}
static td_s32 isp_bshp_ctx_sns_init(isp_bayershp *bshp, ot_isp_cmos_default *sns_dft)
{
    td_s32 ret;
    isp_check_pointer_return(sns_dft->bshp);
    ret = bshp_check_cmos_param(sns_dft->bshp);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    (td_void)memcpy_s(&bshp->mpi_cfg, sizeof(ot_isp_bayershp_attr), sns_dft->bshp, sizeof(ot_isp_bayershp_attr));

    return TD_SUCCESS;
}

static td_void bshp_create_luma_wgt_def(ot_isp_bayershp_attr *mpi_cfg)
{
    td_u8 i;
    td_s32 ret;
    const td_u8 luma_wgt[OT_ISP_BSHP_LUMA_WGT_NUM][OT_ISP_AUTO_ISO_NUM] = {
        {24,  23,  23,  23,  23,  23,  13,  10,  15,  15,  15,  15,  15,  15,  15,  15},
        {25,  23,  23,  23,  23,  23,  13,  10,  15,  15,  15,  15,  15,  15,  15,  15},
        {26,  23,  23,  23,  23,  23,  12,  10,  15,  15,  15,  15,  15,  15,  15,  15},
        {27,  23,  23,  23,  23,  23,  12,  12,  15,  15,  15,  15,  15,  15,  15,  15},
        {28,  23,  23,  23,  23,  23,  14,  13,  15,  15,  15,  15,  15,  15,  15,  15},
        {29,  29,  29,  29,  29,  29,  17,  17,  17,  17,  17,  17,  17,  17,  17,  17},
        {29,  29,  29,  29,  29,  29,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20},
        {30,  30,  30,  30,  30,  30,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22},
        {30,  30,  30,  30,  30,  30,  25,  25,  25,  25,  25,  25,  25,  25,  25,  25},
        {30,  30,  30,  30,  30,  30,  28,  28,  28,  28,  28,  28,  28,  28,  28,  28},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31},
        {31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31}
    };
    ret = memcpy_s(mpi_cfg->auto_attr.luma_wgt, OT_ISP_BSHP_LUMA_WGT_NUM * OT_ISP_AUTO_ISO_NUM * sizeof(td_u8),
                   luma_wgt, OT_ISP_BSHP_LUMA_WGT_NUM * OT_ISP_AUTO_ISO_NUM * sizeof(td_u8));
    if (ret != EOK) {
        return;
    }

    for (i = 0; i < OT_ISP_BSHP_LUMA_WGT_NUM; i++) {
        mpi_cfg->manual_attr.luma_wgt[i] = BSHP_LUMA_WGT_VALUE_DEFAULT;
    }
}

static td_void bshp_create_edge_mf_strength_def(ot_isp_bayershp_attr *mpi_cfg)
{
    td_u16 i, size;
    td_s32 ret;
    const td_u16 edge_mf_strength[OT_ISP_BSHP_HALF_CURVE_NUM][OT_ISP_AUTO_ISO_NUM] = {
        { 75, 75, 75, 64, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 75, 75, 75, 64, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 75, 75, 75, 64, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 76, 76, 76, 66, 34, 34, 34, 38, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 77, 77, 77, 69, 37, 37, 37, 51, 33, 33, 33, 33, 33, 33, 33, 33 },
        { 78, 78, 78, 72, 41, 41, 41, 57, 35, 35, 35, 35, 35, 35, 35, 35 },
        { 79, 79, 79, 75, 45, 45, 45, 63, 37, 37, 37, 37, 37, 37, 37, 37 },
        { 80, 80, 80, 78, 48, 48, 48, 69, 40, 40, 40, 40, 40, 40, 40, 40 },
        { 82, 82, 82, 82, 55, 55, 55, 74, 42, 42, 42, 42, 42, 42, 42, 42 },
        { 86, 86, 86, 86, 65, 65, 65, 80, 45, 45, 45, 45, 45, 45, 45, 45 },
        { 90, 90, 90, 90, 75, 75, 75, 86, 47, 47, 47, 47, 47, 47, 47, 47 },
        { 94, 94, 94, 94, 85, 85, 85, 91, 49, 49, 49, 49, 49, 49, 49, 49 },
        { 98, 98, 98, 98, 95, 95, 95, 97, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 101, 101, 101, 101, 101, 101, 100, 101, 73, 73, 73, 73, 73, 73, 73, 73 },
        { 102, 102, 102, 102, 102, 102, 101, 102, 87, 87, 87, 87, 87, 87, 87, 87 },
        { 104, 104, 104, 104, 103, 103, 102, 103, 100, 100, 100, 100, 100, 100, 100, 100 },
        { 105, 105, 105, 105, 104, 104, 103, 104, 101, 101, 101, 101, 101, 101, 101, 101 },
        { 107, 107, 107, 107, 105, 105, 103, 105, 103, 103, 103, 103, 103, 103, 103, 103 },
        { 108, 108, 108, 108, 106, 106, 104, 106, 104, 104, 104, 104, 104, 104, 104, 104 },
        { 109, 109, 109, 109, 107, 107, 105, 107, 105, 105, 105, 105, 105, 105, 105, 105 },
        { 111, 111, 111, 111, 108, 108, 105, 108, 107, 107, 107, 107, 107, 107, 107, 107 },
        { 112, 112, 112, 112, 109, 109, 106, 109, 108, 108, 108, 108, 108, 108, 108, 108 },
        { 114, 114, 114, 114, 110, 110, 107, 110, 109, 109, 109, 109, 109, 109, 109, 109 },
        { 115, 115, 115, 115, 111, 111, 108, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 117, 117, 117, 117, 113, 113, 108, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 118, 118, 118, 118, 114, 114, 109, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 120, 120, 120, 120, 115, 115, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 120, 120, 120, 120, 115, 115, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 120, 120, 120, 120, 115, 115, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 120, 120, 120, 120, 115, 115, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 120, 120, 120, 120, 115, 115, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
        { 120, 120, 120, 120, 115, 115, 110, 110, 110, 110, 110, 110, 110, 110, 110, 110 },
    };
    size = OT_ISP_BSHP_HALF_CURVE_NUM * OT_ISP_AUTO_ISO_NUM * sizeof(td_u16);
    ret = memcpy_s(mpi_cfg->auto_attr.edge_mf_strength, size, edge_mf_strength, size);
    if (ret != EOK) {
        return;
    }
    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        mpi_cfg->manual_attr.edge_mf_strength[i] = edge_mf_strength[i][0];
    }
}

static td_void bshp_create_texture_mf_strength_def(ot_isp_bayershp_attr *mpi_cfg)
{
    td_u16 i, size;
    td_s32 ret;
    const td_u16 texture_mf_strength[OT_ISP_BSHP_HALF_CURVE_NUM][OT_ISP_AUTO_ISO_NUM] = {
        { 90, 85, 80, 75, 72, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 90, 85, 80, 75, 72, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 90, 85, 80, 75, 72, 60, 60, 56, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 89, 83, 78, 72, 69, 56, 56, 51, 57, 60, 60, 60, 60, 60, 60, 60 },
        { 88, 79, 74, 67, 62, 49, 49, 47, 52, 56, 56, 56, 56, 56, 56, 56 },
        { 87, 75, 70, 62, 56, 41, 41, 43, 46, 49, 49, 49, 49, 49, 49, 49 },
        { 85, 71, 66, 56, 50, 34, 34, 38, 40, 42, 42, 42, 42, 42, 42, 42 },
        { 84, 70, 65, 56, 48, 32, 32, 34, 35, 35, 35, 35, 35, 35, 35, 35 },
        { 82, 69, 65, 56, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 81, 69, 65, 57, 46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 79, 69, 65, 58, 46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 77, 68, 65, 58, 45, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 76, 68, 65, 59, 44, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 74, 67, 65, 60, 44, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 73, 67, 65, 61, 43, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 71, 67, 65, 61, 42, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 69, 66, 65, 62, 42, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 68, 66, 65, 63, 41, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 66, 65, 65, 63, 40, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 65, 65, 65, 64, 40, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 63, 63, 63, 62, 39, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 61, 61, 61, 60, 39, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 59, 59, 59, 58, 38, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 57, 57, 57, 56, 37, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 55, 55, 55, 54, 37, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 53, 53, 53, 52, 36, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 51, 51, 51, 50, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 49, 49, 49, 48, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 47, 47, 47, 46, 34, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 45, 45, 45, 44, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 43, 43, 43, 42, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 41, 41, 41, 40, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
    };

    size = OT_ISP_BSHP_HALF_CURVE_NUM * OT_ISP_AUTO_ISO_NUM * sizeof(td_u16);
    ret = memcpy_s(mpi_cfg->auto_attr.texture_mf_strength, size, texture_mf_strength, size);
    if (ret != EOK) {
        return;
    }
    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        mpi_cfg->manual_attr.texture_mf_strength[i] = texture_mf_strength[i][0];
    }
}

static td_void bshp_create_edge_hf_strength_def(ot_isp_bayershp_attr *mpi_cfg)
{
    td_u16 i, size;
    td_s32 ret;
    const td_u16 edge_hf_strength[OT_ISP_BSHP_HALF_CURVE_NUM][OT_ISP_AUTO_ISO_NUM] = {
        { 100, 96, 64, 96, 64, 64, 96, 100, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 100, 96, 64, 96, 64, 64, 96, 100, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 100, 96, 64, 96, 64, 64, 96, 100, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 100, 96, 64, 96, 64, 64, 96, 100, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 100, 96, 63, 96, 63, 63, 96, 100, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 102, 97, 61, 97, 61, 61, 97, 102, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 104, 97, 59, 97, 59, 59, 97, 104, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 105, 98, 57, 98, 57, 57, 98, 105, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 107, 99, 54, 99, 54, 54, 99, 107, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 108, 99, 52, 99, 52, 52, 99, 108, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 100, 50, 100, 50, 50, 100, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 107, 57, 107, 57, 57, 101, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 113, 63, 113, 63, 63, 103, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 120, 70, 120, 70, 70, 104, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 127, 77, 127, 77, 77, 105, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 133, 83, 133, 83, 83, 107, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 140, 90, 140, 90, 90, 108, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 110, 147, 97, 147, 97, 97, 109, 110, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 111, 151, 103, 151, 103, 103, 112, 111, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 113, 154, 109, 154, 109, 109, 115, 113, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 116, 157, 114, 157, 114, 114, 119, 116, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 118, 160, 120, 160, 120, 120, 122, 118, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 120, 163, 126, 163, 126, 126, 125, 120, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 123, 166, 131, 166, 131, 131, 129, 123, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 125, 169, 137, 169, 137, 137, 132, 125, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 127, 171, 143, 171, 143, 143, 136, 127, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 129, 174, 149, 174, 149, 149, 139, 129, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 130, 175, 150, 175, 150, 150, 140, 130, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 130, 175, 150, 175, 150, 150, 140, 130, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 130, 175, 150, 175, 150, 150, 140, 130, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 130, 175, 150, 175, 150, 150, 140, 130, 110, 110, 110, 110, 110, 110, 260, 260 },
        { 130, 175, 150, 175, 150, 150, 140, 130, 110, 110, 110, 110, 110, 110, 260, 260 },
    };

    size = OT_ISP_BSHP_HALF_CURVE_NUM * OT_ISP_AUTO_ISO_NUM * sizeof(td_u16);
    ret = memcpy_s(mpi_cfg->auto_attr.edge_hf_strength, size, edge_hf_strength, size);
    if (ret != EOK) {
        return;
    }
    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        mpi_cfg->manual_attr.edge_hf_strength[i] = edge_hf_strength[i][0];
    }
}

static td_void bshp_create_texture_hf_strength_def(ot_isp_bayershp_attr *mpi_cfg)
{
    td_u16 i, size;
    td_s32 ret;
    const td_u16 texture_hf_strength[OT_ISP_BSHP_HALF_CURVE_NUM][OT_ISP_AUTO_ISO_NUM] = {
        { 130, 90, 70, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 130, 90, 70, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 130, 90, 70, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 },
        { 125, 86, 66, 56, 56, 56, 56, 57, 57, 60, 60, 60, 60, 60, 60, 60 },
        { 114, 78, 58, 49, 49, 49, 49, 52, 52, 56, 56, 56, 56, 56, 56, 56 },
        { 103, 70, 50, 41, 41, 41, 41, 46, 46, 49, 49, 49, 49, 49, 49, 49 },
        { 93, 62, 42, 34, 34, 34, 34, 40, 40, 42, 42, 42, 42, 42, 42, 42 },
        { 88, 59, 40, 32, 32, 32, 32, 35, 35, 35, 35, 35, 35, 35, 35, 35 },
        { 86, 59, 39, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 83, 58, 38, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 81, 57, 38, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 79, 56, 37, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 76, 55, 36, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 74, 55, 36, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 71, 54, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 69, 53, 34, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 67, 52, 34, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 64, 51, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 62, 51, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 60, 50, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 58, 49, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 56, 48, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 55, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 53, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 52, 46, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 50, 45, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 48, 44, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 47, 43, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 45, 43, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 44, 42, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 42, 41, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
        { 40, 40, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 },
    };

    size = OT_ISP_BSHP_HALF_CURVE_NUM * OT_ISP_AUTO_ISO_NUM * sizeof(td_u16);
    ret = memcpy_s(mpi_cfg->auto_attr.texture_hf_strength, size, texture_hf_strength, size);
    if (ret != EOK) {
        return;
    }
    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        mpi_cfg->manual_attr.texture_hf_strength[i] = texture_hf_strength[i][0];
    }
}

static td_s32 isp_bshp_ctx_def_init(isp_bayershp *bshp)
{
    td_s32 i;
    bshp->mpi_cfg.en = TD_TRUE;

    /* auto */
    for (i = 0; i < OT_ISP_AUTO_ISO_NUM; i++) {
        bshp->mpi_cfg.auto_attr.edge_filt_strength[i] = OT_EXT_SYSTEM_BSHP_EDGE_SMOOTH_STR_DEFAULT;
        bshp->mpi_cfg.auto_attr.edge_max_gain[i]      = OT_EXT_SYSTEM_BSHP_SARPEN_GAIN_MAX_DEFAULT;
        bshp->mpi_cfg.auto_attr.texture_max_gain[i]   = OT_EXT_SYSTEM_BSHP_DETAIL_MAX_STR_DEFAULT;
        bshp->mpi_cfg.auto_attr.overshoot[i]          = OT_EXT_SYSTEM_BSHP_OVERSHOOT_DEFAULT;
        bshp->mpi_cfg.auto_attr.undershoot[i]         = OT_EXT_SYSTEM_BSHP_UNDERSHOOT_DEFAULT;
        bshp->mpi_cfg.auto_attr.g_chn_gain[i]         = OT_EXT_SYSTEM_BSHP_G_CHN_GAIN_DEFAULT;
    }

    /* manual */
    bshp->mpi_cfg.manual_attr.edge_filt_strength   = OT_EXT_SYSTEM_BSHP_EDGE_SMOOTH_STR_DEFAULT;
    bshp->mpi_cfg.manual_attr.edge_max_gain        = OT_EXT_SYSTEM_BSHP_SARPEN_GAIN_MAX_DEFAULT;
    bshp->mpi_cfg.manual_attr.texture_max_gain     = OT_EXT_SYSTEM_BSHP_DETAIL_MAX_STR_DEFAULT;
    bshp->mpi_cfg.manual_attr.overshoot            = OT_EXT_SYSTEM_BSHP_OVERSHOOT_DEFAULT;
    bshp->mpi_cfg.manual_attr.undershoot           = OT_EXT_SYSTEM_BSHP_UNDERSHOOT_DEFAULT;
    bshp->mpi_cfg.manual_attr.g_chn_gain           = OT_EXT_SYSTEM_BSHP_G_CHN_GAIN_DEFAULT;

    bshp_create_luma_wgt_def(&bshp->mpi_cfg);
    bshp_create_edge_mf_strength_def(&bshp->mpi_cfg);
    bshp_create_texture_mf_strength_def(&bshp->mpi_cfg);
    bshp_create_edge_hf_strength_def(&bshp->mpi_cfg);
    bshp_create_texture_hf_strength_def(&bshp->mpi_cfg);

    return TD_SUCCESS;
}

static td_s32 isp_bshp_ctxinit(ot_vi_pipe vi_pipe, isp_bayershp *bshp)
{
    td_s32 ret;
    ot_isp_cmos_default *sns_dft = TD_NULL;

    isp_sensor_get_default(vi_pipe, &sns_dft);

    if (sns_dft->key.bit1_bshp) {
        ret = isp_bshp_ctx_sns_init(bshp, sns_dft);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    } else {
        ret = isp_bshp_ctx_def_init(bshp);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    return TD_SUCCESS;
}

static td_s32 bayershp_proc_write(ot_vi_pipe vi_pipe, ot_isp_ctrl_proc_write *proc, const isp_bayershp *bayershp_ctx)
{
    ot_isp_ctrl_proc_write proc_tmp;
    ot_unused(vi_pipe);

    if ((proc->proc_buff == TD_NULL) || (proc->buff_len == 0)) {
        return TD_FAILURE;
    }

    proc_tmp.proc_buff = proc->proc_buff;
    proc_tmp.buff_len = proc->buff_len;

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "-----bayershp info------------------------------------------\n");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16s" "%16s" "%16s" "%16s" "%16s" "%16s" "%16s\n",
                    "enable", "edge_filt_str", "texture_max", "edge_max", "overshoot", "undershoot", "g_chn_gain");

    isp_proc_printf(&proc_tmp, proc->write_len,
                    "%16u" "%16u" "%16u" "%16u" "%16u" "%16u" "%16u\n",
                    bayershp_ctx->mpi_cfg.en, bayershp_ctx->actual.edge_filt_strength,
                    bayershp_ctx->actual.texture_max_gain, bayershp_ctx->actual.edge_max_gain,
                    bayershp_ctx->actual.overshoot, bayershp_ctx->actual.undershoot,
                    bayershp_ctx->actual.g_chn_gain);

    proc->write_len += 1;
    return TD_SUCCESS;
}

static td_s32 isp_bayershp_param_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_bayershp *bayer_sharpen)
{
    td_s32 ret;
    /* ctx value init */
    bayer_sharpen->init = TD_FALSE;
    ret = isp_bshp_ctxinit(vi_pipe, bayer_sharpen);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* regs init */
    bayershp_regs_initialize(vi_pipe, reg_cfg, bayer_sharpen);
    bayershp_ext_reg_init(vi_pipe, bayer_sharpen);

    bayer_sharpen->init = TD_TRUE;
    ot_ext_system_isp_bshp_init_status_write(vi_pipe, bayer_sharpen->init);
    return TD_SUCCESS;
}

static td_s32 isp_bayershp_init(ot_vi_pipe vi_pipe, td_void *reg_cfg)
{
    td_s32 ret;
    isp_bayershp *bayer_sharpen = TD_NULL;

    ot_ext_system_isp_bshp_init_status_write(vi_pipe, TD_FALSE);
    ret = bayershp_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    bayershp_get_ctx(vi_pipe, bayer_sharpen);
    isp_check_pointer_return(bayer_sharpen);

    return isp_bayershp_param_init(vi_pipe, (isp_reg_cfg *)reg_cfg, bayer_sharpen);
}

static td_void isp_bayershp_read_ext_regs(ot_vi_pipe vi_pipe, isp_bayershp *bshp)
{
    bshp->bayershp_att_update = ot_ext_system_bshp_attr_update_read(vi_pipe);
    if (bshp->bayershp_att_update != TD_TRUE) {
        return;
    }

    ot_ext_system_bshp_attr_update_write(vi_pipe, TD_FALSE);
    bshp->mpi_cfg.op_type = ot_ext_system_bshp_manual_mode_read(vi_pipe);

    if (bshp->mpi_cfg.op_type == OT_OP_MODE_MANUAL) {
        isp_bayershp_manual_attr_read(vi_pipe, &bshp->mpi_cfg.manual_attr);
    } else {
        isp_bayershp_auto_attr_read(vi_pipe, &bshp->mpi_cfg.auto_attr);
    }

    return;
}

static td_void bshp_k_calc(isp_bshp_user_cfg *user_cfg, isp_bshp_dyna_cfg *dyna_regcfg)
{
    td_s32 thd_low, thd_hig, wgt_low, wgt_hig, tmp_mul;
    thd_low = dyna_regcfg->neg_luma_thd0[0];
    wgt_low = dyna_regcfg->neg_luma_wgt0[0];
    thd_hig = dyna_regcfg->neg_luma_thd0[1];
    wgt_hig = dyna_regcfg->neg_luma_wgt0[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGLUMA_MUL_PRECS);
    dyna_regcfg->neg_luma_thd0_mul = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = dyna_regcfg->neg_luma_thd1[0];
    wgt_low = dyna_regcfg->neg_luma_wgt1[0];
    thd_hig = dyna_regcfg->neg_luma_thd1[1];
    wgt_hig = dyna_regcfg->neg_luma_wgt1[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGLUMA_MUL_PRECS);
    dyna_regcfg->neg_luma_thd1_mul = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = dyna_regcfg->neg_mf_thd[0];
    wgt_low = dyna_regcfg->neg_mf_wgt[0];
    thd_hig = dyna_regcfg->neg_mf_thd[1];
    wgt_hig = dyna_regcfg->neg_mf_wgt[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGMF_MUL_PRECS);
    dyna_regcfg->neg_mf_mul       = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = dyna_regcfg->neg_hf_thd[0];
    wgt_low = dyna_regcfg->neg_hf_wgt[0];
    thd_hig = dyna_regcfg->neg_hf_thd[1];
    wgt_hig = dyna_regcfg->neg_hf_wgt[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_NEGHF_MUL_PRECS);
    dyna_regcfg->neg_hf_mul       = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = dyna_regcfg->sht_var_diff_thd[0];
    wgt_low = user_cfg->sht_var_diff_wgt[0];
    thd_hig = dyna_regcfg->sht_var_diff_thd[1];
    wgt_hig = user_cfg->sht_var_diff_wgt[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_SHTVAR_MUL_PRECS);
    dyna_regcfg->sht_var_diff_mul  = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = dyna_regcfg->reg_neg_ctrl_thd[0];
    wgt_low = dyna_regcfg->reg_neg_ctrl_wgt[0];
    thd_hig = dyna_regcfg->reg_neg_ctrl_thd[1];
    wgt_hig = dyna_regcfg->reg_neg_ctrl_wgt[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_RECNEG_CTRL_MUL_PRECS);
    dyna_regcfg->reg_neg_ctrl_mul  = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    thd_low = dyna_regcfg->shp_on_g_thd[0];
    wgt_low = user_cfg->shp_on_g_wgt[0];
    thd_hig = dyna_regcfg->shp_on_g_thd[1];
    wgt_hig = user_cfg->shp_on_g_wgt[1];
    tmp_mul = calc_mul_coef(thd_low, wgt_low, thd_hig, wgt_hig, BSHP_SHPONG_MUL_PRECS);
    dyna_regcfg->shp_on_g_mul      = (td_s16)clip3(tmp_mul, MIN16B, MAX16B);
    return;
}

static td_void isp_bshp_dyna_reg_update(td_u32 vi_pipe, td_u32 iso, isp_bshp_dyna_cfg *dyna_cfg)
{
    td_u8 i, iso_u, iso_d;
    td_u32 iso1, iso2;

    ot_unused(vi_pipe);

    iso_u = get_iso_index(iso);
    iso_d = MAX2((td_s8)iso_u - 1, 0);
    iso1 = get_iso(iso_d);
    iso2 = get_iso(iso_u);
    for (i = 0; i < BSHP_PARAM_THD_NUM; i++) {
        dyna_cfg->neg_luma_thd0[i] = (td_u16)linear_inter(iso, iso1, g_neg_luma_thd0[i][iso_d],
                                                          iso2, g_neg_luma_thd0[i][iso_u]);
        dyna_cfg->neg_luma_thd1[i] = (td_u16)linear_inter(iso, iso1, g_neg_luma_thd1[i][iso_d],
                                                          iso2, g_neg_luma_thd1[i][iso_u]);
        dyna_cfg->neg_luma_wgt0[i] = (td_u16)linear_inter(iso, iso1, g_neg_luma_wgt0[i][iso_d],
                                                          iso2, g_neg_luma_wgt0[i][iso_u]);
        dyna_cfg->neg_luma_wgt1[i] = (td_u16)linear_inter(iso, iso1, g_neg_luma_wgt1[i][iso_d],
                                                          iso2, g_neg_luma_wgt1[i][iso_u]);
        dyna_cfg->neg_mf_thd[i]    = (td_u16)linear_inter(iso, iso1, g_neg_mf_thd[i][iso_d],
                                                          iso2, g_neg_mf_thd[i][iso_u]);
        dyna_cfg->neg_mf_wgt[i]    = (td_u16)linear_inter(iso, iso1, g_neg_mf_wgt[i][iso_d],
                                                          iso2, g_neg_mf_wgt[i][iso_u]);
        dyna_cfg->neg_hf_thd[i]    = (td_u16)linear_inter(iso, iso1, g_neg_hf_thd[i][iso_d],
                                                          iso2, g_neg_hf_thd[i][iso_u]);
        dyna_cfg->neg_hf_wgt[i]    = (td_u16)linear_inter(iso, iso1, g_neg_hf_wgt[i][iso_d],
                                                          iso2, g_neg_hf_wgt[i][iso_u]);
        dyna_cfg->sht_var_diff_thd[i] = (td_u16)linear_inter(iso, iso1, g_sht_var_diff_thd[i][iso_d],
                                                             iso2, g_sht_var_diff_thd[i][iso_u]);
        dyna_cfg->reg_neg_ctrl_thd[i] = (td_u16)linear_inter(iso, iso1, g_rec_neg_ctrl_thr[i][iso_d],
                                                             iso2, g_rec_neg_ctrl_thr[i][iso_u]);
        dyna_cfg->reg_neg_ctrl_wgt[i] = (td_u16)linear_inter(iso, iso1, g_rec_neg_ctrl_wgt[i][iso_d],
                                                             iso2, g_rec_neg_ctrl_wgt[i][iso_u]);
        dyna_cfg->shp_on_g_thd[i]     = (td_u16)linear_inter(iso, iso1, g_shp_g_thd[i][iso_d],
                                                             iso2, g_shp_g_thd[i][iso_u]);
    }
    dyna_cfg->o_max_gain       = (td_u16)linear_inter(iso, iso1, g_omax_gain[iso_d],
                                                      iso2, g_omax_gain[iso_u]);
    dyna_cfg->u_max_gain       = (td_u16)linear_inter(iso, iso1, g_umax_gain[iso_d],
                                                      iso2, g_umax_gain[iso_u]);
    dyna_cfg->o_max_chg        = (td_u16)linear_inter(iso, iso1, g_omaxchg_amt[iso_d],
                                                      iso2, g_omaxchg_amt[iso_u]);
    dyna_cfg->u_max_chg        = (td_u16)linear_inter(iso, iso1, g_umaxchg_amt[iso_d],
                                                      iso2, g_umaxchg_amt[iso_u]);
    dyna_cfg->detail_o_sht_amt = (td_u16)linear_inter(iso, iso1, g_detail_osht_amt[iso_d],
                                                      iso2, g_detail_osht_amt[iso_u]);
    dyna_cfg->detail_u_sht_amt = (td_u16)linear_inter(iso, iso1, g_detail_usht_amt[iso_d],
                                                      iso2, g_detail_usht_amt[iso_u]);
    for (i = 0; i < OT_ISP_BSHP_FILTER_NUM; i++) {
        dyna_cfg->lmf_mf[i] = (td_u16)linear_inter(iso, iso1, g_lmt_mf[i][iso_d], iso2, g_lmt_mf[i][iso_u]);
        dyna_cfg->lmf_hf[i] = (td_u16)linear_inter(iso, iso1, g_lmt_hf[i][iso_d], iso2, g_lmt_hf[i][iso_u]);
    }
}

static td_void bshp_user_curve_update(isp_bshp_user_cfg *user_cfg, const td_u16 *gain_mf_d_t, const td_u16 *gain_hf_d_t,
    const td_u16 *gain_mf_ud_t, const td_u16 *gain_hf_ud_t)
{
    td_s32 i;
    td_u32 gain_mf_d_, gain_hf_d_, gain_mf_ud_, gain_hf_ud_;
    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        user_cfg->gain_d0[i] = ((td_u32)gain_hf_d_t[i] << 12) + gain_mf_d_t[i]; /* shift 12 */
        user_cfg->gain_ud0[i] = ((td_u32)gain_hf_ud_t[i] << 12) + (td_u32)gain_mf_ud_t[i]; /* shift 12 */
    }

    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        int index_up  = clip3(i + 1, 0, 31); /* max is 31 plus 1 */
        gain_hf_d_  = (gain_hf_d_t[i] + gain_hf_d_t[index_up]) / 2; /* odd div 2 avg 2 */
        gain_mf_d_  = (gain_mf_d_t[i] + gain_mf_d_t[index_up]) / 2; /* odd div 2 avg 2 */
        gain_mf_ud_ = (gain_mf_ud_t[i] + gain_mf_ud_t[index_up]) / 2; /* odd div 2 avg 2 */
        gain_hf_ud_ = (gain_hf_ud_t[i] + gain_hf_ud_t[index_up]) / 2; /* odd div 2 avg 2 */
        user_cfg->gain_d1[i]  = (gain_hf_d_ << 12) + gain_mf_d_; /* shift 12 */
        user_cfg->gain_ud1[i] = (gain_hf_ud_ << 12) + gain_mf_ud_; /* shift 12 */
    }
}

static td_void isp_bshp_user_reg_update(isp_bshp_user_cfg *user_cfg, isp_bayershp *bshp_ctx)
{
    td_u8 i;

    user_cfg->dir_rly[1]          = bshp_ctx->actual.edge_filt_strength;
    user_cfg->shp_on_g_wgt[0]     = bshp_ctx->actual.g_chn_gain;
    user_cfg->sht_var_diff_wgt[0] = bshp_ctx->actual.texture_max_gain;
    user_cfg->sht_var_diff_wgt[1] = bshp_ctx->actual.edge_max_gain;
    user_cfg->o_sht_amt           = bshp_ctx->actual.overshoot;
    user_cfg->u_sht_amt           = bshp_ctx->actual.undershoot;
    for (i = 0; i < OT_ISP_BSHP_LUMA_WGT_NUM; i++) {
        user_cfg->luma_wgt[i] = bshp_ctx->actual.luma_wgt[i];
    }
    bshp_user_curve_update(user_cfg, bshp_ctx->actual.edge_mf_strength, bshp_ctx->actual.edge_hf_strength,
        bshp_ctx->actual.texture_mf_strength, bshp_ctx->actual.texture_hf_strength);

    user_cfg->user_resh = TD_TRUE;
}

static td_void isp_bayershp_fw(td_u32 iso, ot_vi_pipe vi_pipe, isp_bshp_reg_cfg *bshp_reg_cfg, isp_bayershp *bshp_ctx)
{
    isp_bshp_dyna_cfg *dyna_cfg = &bshp_reg_cfg->dyna_reg_cfg;
    isp_bshp_user_cfg *user_cfg = &bshp_reg_cfg->usr_reg_cfg;
    /* update dyna regs */
    isp_bshp_dyna_reg_update(vi_pipe, iso, dyna_cfg);
    /* update user regs */
    isp_bshp_user_reg_update(user_cfg, bshp_ctx);

    bshp_k_calc(user_cfg, dyna_cfg);

    dyna_cfg->dyna_resh = TD_TRUE;
}

static td_void isp_bayershp_get_mpi_auto_inter_result(td_u32 iso, isp_bayershp *bshp)
{
    td_u8 i, iso_u, iso_d;
    td_u32 iso1, iso2;
    ot_isp_bayershp_auto_attr   *auto_attr = &bshp->mpi_cfg.auto_attr;
    ot_isp_bayershp_manual_attr *actual    = &bshp->actual;

    iso_u = get_iso_index(iso);
    iso_d = MAX2((td_s8)iso_u - 1, 0);
    iso1 = get_iso(iso_d);
    iso2 = get_iso(iso_u);

    actual->edge_filt_strength = (td_u8)linear_inter(iso, iso1, auto_attr->edge_filt_strength[iso_d],
                                                     iso2, auto_attr->edge_filt_strength[iso_u]);
    actual->g_chn_gain         = (td_u16)linear_inter(iso, iso1, auto_attr->g_chn_gain[iso_d],
                                                      iso2, auto_attr->g_chn_gain[iso_u]);
    actual->texture_max_gain   = (td_u16)linear_inter(iso, iso1, auto_attr->texture_max_gain[iso_d],
                                                      iso2, auto_attr->texture_max_gain[iso_u]);
    actual->edge_max_gain      = (td_u16)linear_inter(iso, iso1, auto_attr->edge_max_gain[iso_d],
                                                      iso2, auto_attr->edge_max_gain[iso_u]);
    actual->overshoot          = (td_u16)linear_inter(iso, iso1, auto_attr->overshoot[iso_d],
                                                      iso2, auto_attr->overshoot[iso_u]);
    actual->undershoot         = (td_u16)linear_inter(iso, iso1, auto_attr->undershoot[iso_d],
                                                      iso2, auto_attr->undershoot[iso_u]);

    for (i = 0; i < OT_ISP_BSHP_LUMA_WGT_NUM; i++) {
        actual->luma_wgt[i] = (td_u8)linear_inter(iso, iso1, auto_attr->luma_wgt[i][iso_d],
                                                  iso2, auto_attr->luma_wgt[i][iso_u]);
    }

    for (i = 0; i < OT_ISP_BSHP_HALF_CURVE_NUM; i++) {
        actual->edge_mf_strength[i]    = (td_u16)linear_inter(iso, iso1, auto_attr->edge_mf_strength[i][iso_d],
                                                              iso2, auto_attr->edge_mf_strength[i][iso_u]);
        actual->texture_mf_strength[i] = (td_u16)linear_inter(iso, iso1, auto_attr->texture_mf_strength[i][iso_d],
                                                              iso2, auto_attr->texture_mf_strength[i][iso_u]);
        actual->edge_hf_strength[i]    = (td_u16)linear_inter(iso, iso1, auto_attr->edge_hf_strength[i][iso_d],
                                                              iso2, auto_attr->edge_hf_strength[i][iso_u]);
        actual->texture_hf_strength[i] = (td_u16)linear_inter(iso, iso1, auto_attr->texture_hf_strength[i][iso_d],
                                                              iso2, auto_attr->texture_hf_strength[i][iso_u]);
    }
}

static td_void isp_bayershp_actual_calc(td_u32 iso, isp_bayershp *bayershp_ctx)
{
    if (bayershp_ctx->mpi_cfg.op_type == OT_OP_MODE_MANUAL) {
        (td_void)memcpy_s(&bayershp_ctx->actual, sizeof(ot_isp_bayershp_manual_attr),
            &bayershp_ctx->mpi_cfg.manual_attr, sizeof(ot_isp_bayershp_manual_attr));
    } else {
        isp_bayershp_get_mpi_auto_inter_result(iso, bayershp_ctx);
    }
}

static td_void isp_bayershp_reg_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg, isp_bayershp *bayershp_ctx)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    isp_bayershp_actual_calc(isp_ctx->linkage.iso, bayershp_ctx);

    isp_bayershp_fw(isp_ctx->linkage.iso, vi_pipe, &reg_cfg->alg_reg_cfg[0].bshp_reg_cfg, bayershp_ctx);

    for (i = 1; i < isp_ctx->block_attr.block_num; i++) {
        (td_void)memcpy_s(&reg_cfg->alg_reg_cfg[i].bshp_reg_cfg, sizeof(isp_bshp_reg_cfg),
                          &reg_cfg->alg_reg_cfg[0].bshp_reg_cfg, sizeof(isp_bshp_reg_cfg));
    }
}

static __inline td_bool  check_bshp_open(const isp_bayershp *bayershp_ctx)
{
    return (bayershp_ctx->mpi_cfg.en == TD_TRUE);
}

static td_s32 isp_bayershp_run(ot_vi_pipe vi_pipe, const td_void *stat_info, td_void *reg_cfg, td_s32 rsv)
{
    td_u8 i;
    ot_isp_alg_mod alg_mod = OT_ISP_ALG_BSHP;
    isp_reg_cfg *local_reg = (isp_reg_cfg *)reg_cfg;
    isp_bayershp *bayershp_ctx = TD_NULL;
    bayershp_get_ctx(vi_pipe, bayershp_ctx);
    isp_check_pointer_return(bayershp_ctx);

    ot_unused(stat_info);
    ot_unused(rsv);

    ot_ext_system_isp_bshp_init_status_write(vi_pipe, bayershp_ctx->init);
    if (bayershp_ctx->init != TD_TRUE) {
        ioctl(isp_get_fd(vi_pipe), ISP_ALG_INIT_ERR_INFO_PRINT, &alg_mod);
        return TD_SUCCESS;
    }
    bayershp_ctx->mpi_cfg.en = ot_ext_system_bshp_enable_read(vi_pipe);
    for (i = 0; i < local_reg->cfg_num; i++) {
        local_reg->alg_reg_cfg[i].bshp_reg_cfg.bshp_enable = bayershp_ctx->mpi_cfg.en;
    }
    local_reg->cfg_key.bit1_bshp_cfg = 1;

    if (!check_bshp_open(bayershp_ctx)) {
        return TD_SUCCESS;
    }
    isp_bayershp_read_ext_regs(vi_pipe, bayershp_ctx);

    isp_bayershp_reg_update(vi_pipe, local_reg, bayershp_ctx);

    return TD_SUCCESS;
}

static td_s32 isp_bayershp_ctrl(ot_vi_pipe vi_pipe, td_u32 cmd, td_void *value)
{
    isp_bayershp *bayershp_ctx = TD_NULL;
    switch (cmd) {
        case OT_ISP_WDR_MODE_SET:
            break;

        case OT_ISP_PROC_WRITE:
            bayershp_get_ctx(vi_pipe, bayershp_ctx);
            isp_check_pointer_return(bayershp_ctx);
            bayershp_proc_write(vi_pipe, (ot_isp_ctrl_proc_write *)value, bayershp_ctx);
            break;

        default:
            break;
    }

    return TD_SUCCESS;
}

static td_s32 isp_bayershp_exit(ot_vi_pipe vi_pipe)
{
    td_u16 i;
    isp_reg_cfg_attr *regcfg = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, regcfg);
    ot_ext_system_isp_bshp_init_status_write(vi_pipe, TD_FALSE);
    for (i = 0; i < regcfg->reg_cfg.cfg_num; i++) {
        regcfg->reg_cfg.alg_reg_cfg[i].bshp_reg_cfg.bshp_enable = TD_FALSE;
    }

    regcfg->reg_cfg.cfg_key.bit1_bshp_cfg = 1;

    bayershp_ctx_exit(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_alg_register_bayershp(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_node *algs = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_alg_check_return(isp_ctx->alg_key.bit1_bshp);
    algs = isp_search_alg(isp_ctx->algs);
    isp_check_pointer_return(algs);

    algs->alg_type = OT_ISP_ALG_BSHP;
    algs->alg_func.pfn_alg_init = isp_bayershp_init;
    algs->alg_func.pfn_alg_run  = isp_bayershp_run;
    algs->alg_func.pfn_alg_ctrl = isp_bayershp_ctrl;
    algs->alg_func.pfn_alg_exit = isp_bayershp_exit;
    algs->used = TD_TRUE;

    return TD_SUCCESS;
}
