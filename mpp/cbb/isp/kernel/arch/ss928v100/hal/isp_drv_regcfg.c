/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_drv.h"
#include "isp_drv_define.h"
#include "isp_reg_define.h"
#include "isp_stt_define.h"
#include "ot_common.h"
#include "ot_osal.h"
#include "ot_math.h"
#include "mkp_isp.h"
#include "isp.h"
#include "mm_ext.h"
#include "sys_ext.h"

#define DYNAMIC_BLC_FILTER_FIX_BIT 6

static td_u32 g_drc_cur_luma_lut[OT_ISP_DRC_SHP_LOG_CONFIG_NUM][OT_ISP_DRC_EXP_COMP_SAMPLE_NUM - 1] = {
    {1,     1,      5,      31,     180,    1023,   32767},
    {1,     3,      8,      52,     277,    1446,   38966},
    {2,     5,      15,     87,     427,    2044,   46337},
    {4,     9,      27,     144,    656,    2888,   55101},
    {7,     16,     48,     240,    1008,   4080,   65521},
    {12,    29,     85,     399,    1547,   5761,   77906},
    {23,    53,     151,    660,    2372,   8128,   92622},
    {42,    97,     267,    1090,   3628,   11458,  110100},
    {76,    175,    468,    1792,   5537,   16130,  130840},
    {138,   313,    816,    2933,   8423,   22664,  155417},
    {258,   555,    1412,   4770,   12758,  31760,  184476},
    {441,   977,    2420,   7699,   19215,  44338,  218711},
    {776,   1698,   4100,   12304,  28720,  61568,  258816},
    {1344,  2907,   6847,   19416,  42491,  84851,  305376},
    {2283,  4884,   11224,  30137,  62006,  115708, 358680},
    {3783,  8004,   17962,  45770,  88821,  155470, 418391},
};

static td_u32 g_drc_div_denom_log[OT_ISP_DRC_SHP_LOG_CONFIG_NUM] = {
    52429, 55188,  58254,  61681,  65536,  69905,  74898, 80659,
    87379, 95319, 104843, 116472, 130980, 149557, 174114, 207870
};

static td_u32 g_drc_denom_exp[OT_ISP_DRC_SHP_LOG_CONFIG_NUM] = {
    1310720, 1245184, 1179648, 1114113, 1048577, 983043, 917510, 851980,
    786455,  720942,  655452,  590008,  524657, 459488, 394682, 330589
};

static td_u8 g_drc_shp_log[OT_ISP_MAX_PIPE_NUM][OT_ISP_STRIPING_MAX_NUM] = {
    [0 ... OT_ISP_MAX_PIPE_NUM - 1] = { 8, 8, 8 }
};
static td_u8 g_drc_shp_exp[OT_ISP_MAX_PIPE_NUM][OT_ISP_STRIPING_MAX_NUM] = {
    [0 ... OT_ISP_MAX_PIPE_NUM - 1] = { 8, 8, 8 }
};

static td_s64 signed_right_shift(td_s64 value, td_u8 bit_shift)
{
    td_u64 value_pos;
    td_u64 tmp;
    value_pos = (td_u64)value;
    if (value < 0) {
        value_pos = value_pos >> bit_shift;
        tmp = (((0x1LL << (bit_shift)) - 1LL)) << (64 - bit_shift); /* left shift (64 - bit_shift)bits */
        value_pos = value_pos | tmp;
        return (td_s64)value_pos;
    } else {
        return (td_s64)(value_pos >> bit_shift);
    }
}

static td_u16 sqrt32(td_u32 arg)
{
    td_u32 mask = (td_u32)1 << 15; /* left shift 15 */
    td_u16 res = 0;
    td_u32 i;

    for (i = 0; i < 16; i++) {  /* max value 16 */
        if ((res + (mask >> i)) * (res + (mask >> i)) <= arg) {
            res = res + (mask >> i);
        }
    }

    /* rounding */
    if ((td_u32)(res * res + res) < arg) {
        ++res;
    }

    return res;
}

/* isp drv FHY regs define */
static td_void isp_drv_input_sel_write(isp_pre_be_reg_type *isp_be_regs, const td_u32 *input_sel)
{
    u_isp_be_input_mux o_isp_be_input_mux;
    isp_check_pointer_void_return(isp_be_regs);

    o_isp_be_input_mux.u32 = isp_be_regs->isp_be_input_mux.u32;
    o_isp_be_input_mux.bits.isp_input0_sel = input_sel[0]; /* array index 0 */
    o_isp_be_input_mux.bits.isp_input1_sel = input_sel[1]; /* array index 1 */
    o_isp_be_input_mux.bits.isp_input2_sel = input_sel[2]; /* array index 2 */
    o_isp_be_input_mux.bits.isp_input3_sel = input_sel[3]; /* array index 3 */
    o_isp_be_input_mux.bits.isp_input4_sel = input_sel[4]; /* array index 4 */
    isp_be_regs->isp_be_input_mux.u32 = o_isp_be_input_mux.u32;
}

static td_void isp_drv_post_be_input_sel_write(isp_post_be_reg_type *isp_be_regs, const td_u32 *input_sel)
{
    u_isp_be_input_mux o_isp_be_input_mux;
    isp_check_pointer_void_return(isp_be_regs);

    o_isp_be_input_mux.u32 = isp_be_regs->isp_be_input_mux.u32;

    o_isp_be_input_mux.bits.isp_input0_sel = 0; /* fix value channel 0 */
    o_isp_be_input_mux.bits.isp_input1_sel = 1; /* fix value channel 1 */
    o_isp_be_input_mux.bits.isp_input2_sel = 2; /* fix value channel 2 */
    o_isp_be_input_mux.bits.isp_input3_sel = 3; /* fix value channel 3 */

    o_isp_be_input_mux.bits.isp_input4_sel = input_sel[4]; /* array index 4 */
    isp_be_regs->isp_be_input_mux.u32 = o_isp_be_input_mux.u32;
}

static __inline td_void isp_drv_set_cc_coef00(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef00)
{
    u_isp_cc_coef0 o_isp_cc_coef0;
    o_isp_cc_coef0.u32 = post_be->isp_cc_coef0.u32;
    o_isp_cc_coef0.bits.isp_cc_coef00 = uisp_cc_coef00;
    post_be->isp_cc_coef0.u32 = o_isp_cc_coef0.u32;
}

static __inline td_void isp_drv_set_cc_coef01(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef01)
{
    u_isp_cc_coef0 o_isp_cc_coef0;
    o_isp_cc_coef0.u32 = post_be->isp_cc_coef0.u32;
    o_isp_cc_coef0.bits.isp_cc_coef01 = uisp_cc_coef01;
    post_be->isp_cc_coef0.u32 = o_isp_cc_coef0.u32;
}
static __inline td_void isp_drv_set_cc_coef02(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef02)
{
    u_isp_cc_coef1 o_isp_cc_coef1;
    o_isp_cc_coef1.u32 = post_be->isp_cc_coef1.u32;
    o_isp_cc_coef1.bits.isp_cc_coef02 = uisp_cc_coef02;
    post_be->isp_cc_coef1.u32 = o_isp_cc_coef1.u32;
}

static __inline td_void isp_drv_set_cc_coef10(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef10)
{
    u_isp_cc_coef1 o_isp_cc_coef1;
    o_isp_cc_coef1.u32 = post_be->isp_cc_coef1.u32;
    o_isp_cc_coef1.bits.isp_cc_coef10 = uisp_cc_coef10;
    post_be->isp_cc_coef1.u32 = o_isp_cc_coef1.u32;
}

static __inline td_void isp_drv_set_cc_coef11(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef11)
{
    u_isp_cc_coef2 o_isp_cc_coef2;
    o_isp_cc_coef2.u32 = post_be->isp_cc_coef2.u32;
    o_isp_cc_coef2.bits.isp_cc_coef11 = uisp_cc_coef11;
    post_be->isp_cc_coef2.u32 = o_isp_cc_coef2.u32;
}

static __inline td_void isp_drv_set_cc_coef12(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef12)
{
    u_isp_cc_coef2 o_isp_cc_coef2;
    o_isp_cc_coef2.u32 = post_be->isp_cc_coef2.u32;
    o_isp_cc_coef2.bits.isp_cc_coef12 = uisp_cc_coef12;
    post_be->isp_cc_coef2.u32 = o_isp_cc_coef2.u32;
}

static __inline td_void isp_drv_set_cc_coef20(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef20)
{
    u_isp_cc_coef3 o_isp_cc_coef3;
    o_isp_cc_coef3.u32 = post_be->isp_cc_coef3.u32;
    o_isp_cc_coef3.bits.isp_cc_coef20 = uisp_cc_coef20;
    post_be->isp_cc_coef3.u32 = o_isp_cc_coef3.u32;
}

static __inline td_void isp_drv_set_cc_coef21(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef21)
{
    u_isp_cc_coef3 o_isp_cc_coef3;
    o_isp_cc_coef3.u32 = post_be->isp_cc_coef3.u32;
    o_isp_cc_coef3.bits.isp_cc_coef21 = uisp_cc_coef21;
    post_be->isp_cc_coef3.u32 = o_isp_cc_coef3.u32;
}

static __inline td_void isp_drv_set_cc_coef22(isp_post_be_reg_type *post_be, td_u32 uisp_cc_coef22)
{
    u_isp_cc_coef4 o_isp_cc_coef4;
    o_isp_cc_coef4.u32 = post_be->isp_cc_coef4.u32;
    o_isp_cc_coef4.bits.isp_cc_coef22 = uisp_cc_coef22;
    post_be->isp_cc_coef4.u32 = o_isp_cc_coef4.u32;
}

static td_void isp_drv_set_ccm(isp_post_be_reg_type *isp_be_regs, td_u16 *be_cc)
{
    td_u8 i;
    for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
        if ((be_cc[i] >> 12) < 0x8) { /* valid bit 12, sign bit 0x8 */
            be_cc[i] = MIN2(be_cc[i], 0xFFF); /* max positive value 0xFFF */
        } else if ((be_cc[i] >> 12) >= 0x8) { /* valid bit 12, sign bit 0x8 */
            be_cc[i] = MIN2(be_cc[i], 0x8FFF); /* max negative value 0x8FFF */
        }
    }

    isp_drv_set_cc_coef00(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[0]))); /* array index 0 */
    isp_drv_set_cc_coef01(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[1]))); /* array index 1 */
    isp_drv_set_cc_coef02(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[2]))); /* array index 2 */
    isp_drv_set_cc_coef10(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[3]))); /* array index 3 */
    isp_drv_set_cc_coef11(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[4]))); /* array index 4 */
    isp_drv_set_cc_coef12(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[5]))); /* array index 5 */
    isp_drv_set_cc_coef20(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[6]))); /* array index 6 */
    isp_drv_set_cc_coef21(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[7]))); /* array index 7 */
    isp_drv_set_cc_coef22(isp_be_regs, ccm_convert(ccm_convert_pre(be_cc[8]))); /* array index 8 */
}

static __inline td_void isp_drv_set_awb_gain(isp_post_be_reg_type *isp_be_regs, const td_u32 *wb_gain)
{
    isp_be_regs->isp_wb_gain1.u32 = (wb_gain[OT_ISP_CHN_R] << 16) + wb_gain[OT_ISP_CHN_GR]; /* left shift 16 */
    isp_be_regs->isp_wb_gain2.u32 = (wb_gain[OT_ISP_CHN_B] << 16) + wb_gain[OT_ISP_CHN_GB]; /* left shift 16 */
}
static __inline td_void isp_drv_set_be_dgain(isp_post_be_reg_type *isp_be_regs, td_u32 isp_dgain)
{
    isp_be_regs->isp_dg_gain1.u32 = (isp_dgain << 16) + isp_dgain; /* left shift 16 */
    isp_be_regs->isp_dg_gain2.u32 = (isp_dgain << 16) + isp_dgain; /* left shift 16 */
}

static __inline td_void isp_drv_set_fe_dgain(isp_fe_reg_type *isp_fe_regs, td_u32 isp_dgain)
{
    u_isp_dg2_gain1 o_isp_dg_gain1;
    u_isp_dg2_gain2 o_isp_dg_gain2;

    isp_check_pointer_void_return(isp_fe_regs);

    o_isp_dg_gain1.u32 = (isp_dgain << 16) + isp_dgain; /* left shift 16 */
    isp_fe_regs->isp_dg2_gain1.u32 = o_isp_dg_gain1.u32;

    o_isp_dg_gain2.u32 = (isp_dgain << 16) + isp_dgain; /* left shift 16 */
    isp_fe_regs->isp_dg2_gain2.u32 = o_isp_dg_gain2.u32;
}

static __inline td_void isp_drv_set_isp_4dgain0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp4_dgain0)
{
    isp_be_regs->isp_4dg_0_gain1.u32 = (isp4_dgain0 << 16) + isp4_dgain0; /* left shift 16 */
    isp_be_regs->isp_4dg_0_gain2.u32 = (isp4_dgain0 << 16) + isp4_dgain0; /* left shift 16 */
}

static __inline td_void isp_drv_set_isp_4dgain1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp4_dgain1)
{
    isp_be_regs->isp_4dg_1_gain1.u32 = (isp4_dgain1 << 16) + isp4_dgain1; /* left shift 16 */
    isp_be_regs->isp_4dg_1_gain2.u32 = (isp4_dgain1 << 16) + isp4_dgain1; /* left shift 16 */
}

static __inline td_void isp_drv_set_isp_4dgain2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp4_dgain2)
{
    isp_be_regs->isp_4dg_2_gain1.u32 = (isp4_dgain2 << 16) + isp4_dgain2; /* left shift 16 */
    isp_be_regs->isp_4dg_2_gain2.u32 = (isp4_dgain2 << 16) + isp4_dgain2; /* left shift 16 */
}

static __inline td_void isp_drv_set_isp_4dgain3(isp_pre_be_reg_type *isp_be_regs, td_u32 isp4_dgain3)
{
    isp_be_regs->isp_4dg_3_gain1.u32 = (isp4_dgain3 << 16) + isp4_dgain3; /* left shift 16 */
    isp_be_regs->isp_4dg_3_gain2.u32 = (isp4_dgain3 << 16) + isp4_dgain3; /* left shift 16 */
}

static __inline td_void isp_drv_set_wdr_exporratio0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_exporatio0)
{
    u_isp_wdr_exporratio o_isp_wdr_exporatio;

    o_isp_wdr_exporatio.u32 = isp_be_regs->isp_wdr_exporratio.u32;
    o_isp_wdr_exporatio.bits.isp_wdr_exporratio0 = isp_wdr_exporatio0;
    isp_be_regs->isp_wdr_exporratio.u32 = o_isp_wdr_exporatio.u32;
}

static __inline td_void isp_drv_set_wdr_exporratio1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_exporatio1)
{
    u_isp_wdr_exporratio o_isp_wdr_exporatio;

    o_isp_wdr_exporatio.u32 = isp_be_regs->isp_wdr_exporratio.u32;
    o_isp_wdr_exporatio.bits.isp_wdr_exporratio1 = isp_wdr_exporatio1;
    isp_be_regs->isp_wdr_exporratio.u32 = o_isp_wdr_exporatio.u32;
}

static __inline td_void isp_drv_set_wdr_exporratio2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_exporatio2)
{
    u_isp_wdr_exporratio o_isp_wdr_exporatio;

    o_isp_wdr_exporatio.u32 = isp_be_regs->isp_wdr_exporratio.u32;
    o_isp_wdr_exporatio.bits.isp_wdr_exporratio2 = isp_wdr_exporatio2;
    isp_be_regs->isp_wdr_exporratio.u32 = o_isp_wdr_exporatio.u32;
}

static __inline td_void isp_drv_set_wdr_expo_value0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_expovalue0)
{
    u_isp_wdr_wdr_expovalue0  o_isp_wdr_expovalue0;

    o_isp_wdr_expovalue0.u32 = isp_be_regs->isp_wdr_wdr_expovalue0.u32;
    o_isp_wdr_expovalue0.bits.isp_wdr_wdr_expovalue0 = isp_wdr_expovalue0;
    isp_be_regs->isp_wdr_wdr_expovalue0.u32 = o_isp_wdr_expovalue0.u32;
}

static __inline td_void isp_drv_set_wdr_expo_value1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_expovalue1)
{
    u_isp_wdr_wdr_expovalue1 o_isp_wdr_expovalue1;

    o_isp_wdr_expovalue1.u32 = isp_be_regs->isp_wdr_wdr_expovalue1.u32;
    o_isp_wdr_expovalue1.bits.isp_wdr_wdr_expovalue1 = isp_wdr_expovalue1;
    isp_be_regs->isp_wdr_wdr_expovalue1.u32 = o_isp_wdr_expovalue1.u32;
}

static __inline td_void isp_drv_set_wdr_expo_value2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_expovalue2)
{
    u_isp_wdr_wdr_expovalue2  o_isp_wdr_expovalue2;

    o_isp_wdr_expovalue2.u32 = isp_be_regs->isp_wdr_wdr_expovalue2.u32;
    o_isp_wdr_expovalue2.bits.isp_wdr_wdr_expovalue2 = isp_wdr_expovalue2;
    isp_be_regs->isp_wdr_wdr_expovalue2.u32 = o_isp_wdr_expovalue2.u32;
}

static __inline td_void isp_drv_set_wdr_expo_value3(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_expovalue3)
{
    u_isp_wdr_wdr_expovalue3 o_isp_wdr_expovalue3;

    o_isp_wdr_expovalue3.u32 = isp_be_regs->isp_wdr_wdr_expovalue3.u32;
    o_isp_wdr_expovalue3.bits.isp_wdr_wdr_expovalue3 = isp_wdr_expovalue3;
    isp_be_regs->isp_wdr_wdr_expovalue3.u32 = o_isp_wdr_expovalue3.u32;
}

static __inline td_void isp_drv_set_fusion_expo_value0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_fusion_expovalue0)
{
    u_isp_wdr_fusion_expovalue0 o_isp_wdr_fusion_expovalue0;
    o_isp_wdr_fusion_expovalue0.u32 = isp_be_regs->isp_wdr_fusion_expovalue0.u32;
    o_isp_wdr_fusion_expovalue0.bits.isp_wdr_fusion_expovalue0 = isp_fusion_expovalue0;
    isp_be_regs->isp_wdr_fusion_expovalue0.u32 = o_isp_wdr_fusion_expovalue0.u32;
}

static __inline td_void isp_drv_set_fusion_expo_value1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_fusion_expovalue1)
{
    u_isp_wdr_fusion_expovalue1 o_isp_wdr_fusion_expovalue1;
    o_isp_wdr_fusion_expovalue1.u32 = isp_be_regs->isp_wdr_fusion_expovalue1.u32;
    o_isp_wdr_fusion_expovalue1.bits.isp_wdr_fusion_expovalue1 = isp_fusion_expovalue1;
    isp_be_regs->isp_wdr_fusion_expovalue1.u32 = o_isp_wdr_fusion_expovalue1.u32;
}

static __inline td_void isp_drv_set_fusion_expo_value2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_fusion_expovalue2)
{
    u_isp_wdr_fusion_expovalue2 o_isp_wdr_fusion_expovalue2;
    o_isp_wdr_fusion_expovalue2.u32 = isp_be_regs->isp_wdr_fusion_expovalue2.u32;
    o_isp_wdr_fusion_expovalue2.bits.isp_wdr_fusion_expovalue2 = isp_fusion_expovalue2;
    isp_be_regs->isp_wdr_fusion_expovalue2.u32 = o_isp_wdr_fusion_expovalue2.u32;
}

static __inline td_void isp_drv_set_fusion_expo_value3(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_fusion_expovalue3)
{
    u_isp_wdr_fusion_expovalue3 o_isp_wdr_fusion_expovalue3;
    o_isp_wdr_fusion_expovalue3.u32 = isp_be_regs->isp_wdr_fusion_expovalue3.u32;
    o_isp_wdr_fusion_expovalue3.bits.isp_wdr_fusion_expovalue3 = isp_fusion_expovalue3;
    isp_be_regs->isp_wdr_fusion_expovalue3.u32 = o_isp_wdr_fusion_expovalue3.u32;
}

static __inline td_void isp_drv_set_flick_exporatio0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_flick_exporatio)
{
    u_isp_flick_exporatio0 o_isp_flick_exporatio0;

    o_isp_flick_exporatio0.u32 = isp_be_regs->isp_flick_exporatio0.u32;
    o_isp_flick_exporatio0.bits.isp_flick_exporatio0 = isp_flick_exporatio;
    isp_be_regs->isp_flick_exporatio0.u32 = o_isp_flick_exporatio0.u32;
}

static __inline td_void isp_drv_set_flick_exporatio1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_flick_exporatio)
{
    u_isp_flick_exporatio0 o_isp_flick_exporatio0;

    o_isp_flick_exporatio0.u32 = isp_be_regs->isp_flick_exporatio0.u32;
    o_isp_flick_exporatio0.bits.isp_flick_exporatio1 = isp_flick_exporatio;
    isp_be_regs->isp_flick_exporatio0.u32 = o_isp_flick_exporatio0.u32;
}

static __inline td_void isp_drv_set_flick_exporatio2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_flick_exporatio)
{
    u_isp_flick_exporatio1 o_isp_flick_exporatio1;

    o_isp_flick_exporatio1.u32 = isp_be_regs->isp_flick_exporatio1.u32;
    o_isp_flick_exporatio1.bits.isp_flick_exporatio2 = isp_flick_exporatio;
    isp_be_regs->isp_flick_exporatio1.u32 = o_isp_flick_exporatio1.u32;
}

static __inline td_void isp_drv_set_wdr_blc_comp0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_blc_comp0)
{
    u_isp_wdr_blc_comp0 o_isp_wdr_blc_comp0;

    o_isp_wdr_blc_comp0.u32 = isp_be_regs->isp_wdr_blc_comp0.u32;
    o_isp_wdr_blc_comp0.bits.isp_wdr_blc_comp0 = isp_wdr_blc_comp0;
    isp_be_regs->isp_wdr_blc_comp0.u32 = o_isp_wdr_blc_comp0.u32;
}

static __inline td_void isp_drv_set_wdr_blc_comp1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_blc_comp1)
{
    u_isp_wdr_blc_comp1 o_isp_wdr_blc_comp1;

    o_isp_wdr_blc_comp1.u32 = isp_be_regs->isp_wdr_blc_comp1.u32;
    o_isp_wdr_blc_comp1.bits.isp_wdr_blc_comp1 = isp_wdr_blc_comp1;
    isp_be_regs->isp_wdr_blc_comp1.u32 = o_isp_wdr_blc_comp1.u32;
}

static __inline td_void isp_drv_set_wdr_blc_comp2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_blc_comp2)
{
    u_isp_wdr_blc_comp2 o_isp_wdr_blc_comp2;

    o_isp_wdr_blc_comp2.u32 = isp_be_regs->isp_wdr_blc_comp2.u32;
    o_isp_wdr_blc_comp2.bits.isp_wdr_blc_comp2 = isp_wdr_blc_comp2;
    isp_be_regs->isp_wdr_blc_comp2.u32 = o_isp_wdr_blc_comp2.u32;
}

static __inline td_void isp_drv_set_wdr_max_ratio(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_maxratio)
{
    u_isp_wdr_wdr_maxratio o_isp_wdr_maxratio;

    o_isp_wdr_maxratio.u32 = isp_be_regs->isp_wdr_wdr_maxratio.u32;
    o_isp_wdr_maxratio.bits.isp_wdr_wdr_maxratio = isp_wdr_maxratio;
    isp_be_regs->isp_wdr_wdr_maxratio.u32 = o_isp_wdr_maxratio.u32;
}

static __inline td_void isp_drv_set_fusion_max_ratio(isp_pre_be_reg_type *isp_be_regs, td_u32 uisp_fusion_maxratio)
{
    u_isp_wdr_fusion_maxratio o_isp_fusion_maxratio;

    o_isp_fusion_maxratio.u32 = isp_be_regs->isp_wdr_fusion_maxratio.u32;
    o_isp_fusion_maxratio.bits.isp_wdr_fusion_maxratio = uisp_fusion_maxratio;
    isp_be_regs->isp_wdr_fusion_maxratio.u32 = o_isp_fusion_maxratio.u32;
}

static __inline td_void isp_drv_set_wdr_long_thr0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_long_thr)
{
    u_isp_wdr_wgtidx_thr0 o_isp_wdr_wgtidx_thr0;

    o_isp_wdr_wgtidx_thr0.u32 = isp_be_regs->isp_wdr_wgtidx_thr0.u32;
    o_isp_wdr_wgtidx_thr0.bits.isp_wdr_long_thr0  = isp_wdr_long_thr;
    isp_be_regs->isp_wdr_wgtidx_thr0.u32 = o_isp_wdr_wgtidx_thr0.u32;
}

static __inline td_void isp_drv_set_wdr_short_thr0(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_short_thr)
{
    u_isp_wdr_wgtidx_thr0 o_isp_wdr_wgtidx_thr0;

    o_isp_wdr_wgtidx_thr0.u32 = isp_be_regs->isp_wdr_wgtidx_thr0.u32;
    o_isp_wdr_wgtidx_thr0.bits.isp_wdr_short_thr0  = isp_wdr_short_thr;
    isp_be_regs->isp_wdr_wgtidx_thr0.u32 = o_isp_wdr_wgtidx_thr0.u32;
}

static __inline td_void isp_drv_set_wdr_long_thr1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_long_thr)
{
    u_isp_wdr_wgtidx_thr1 o_isp_wdr_wgtidx_thr1;

    o_isp_wdr_wgtidx_thr1.u32 = isp_be_regs->isp_wdr_wgtidx_thr1.u32;
    o_isp_wdr_wgtidx_thr1.bits.isp_wdr_long_thr1  = isp_wdr_long_thr;
    isp_be_regs->isp_wdr_wgtidx_thr1.u32 = o_isp_wdr_wgtidx_thr1.u32;
}

static __inline td_void isp_drv_set_wdr_short_thr1(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_short_thr)
{
    u_isp_wdr_wgtidx_thr1 o_isp_wdr_wgtidx_thr1;

    o_isp_wdr_wgtidx_thr1.u32 = isp_be_regs->isp_wdr_wgtidx_thr1.u32;
    o_isp_wdr_wgtidx_thr1.bits.isp_wdr_short_thr1  = isp_wdr_short_thr;
    isp_be_regs->isp_wdr_wgtidx_thr1.u32 = o_isp_wdr_wgtidx_thr1.u32;
}

static __inline td_void isp_drv_set_wdr_long_thr2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_long_thr)
{
    u_isp_wdr_wgtidx_thr2 o_isp_wdr_wgtidx_thr2;

    o_isp_wdr_wgtidx_thr2.u32 = isp_be_regs->isp_wdr_wgtidx_thr2.u32;
    o_isp_wdr_wgtidx_thr2.bits.isp_wdr_long_thr2  = isp_wdr_long_thr;
    isp_be_regs->isp_wdr_wgtidx_thr2.u32 = o_isp_wdr_wgtidx_thr2.u32;
}

static __inline td_void isp_drv_set_wdr_short_thr2(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_short_thr)
{
    u_isp_wdr_wgtidx_thr2 o_isp_wdr_wgtidx_thr2;

    o_isp_wdr_wgtidx_thr2.u32 = isp_be_regs->isp_wdr_wgtidx_thr2.u32;
    o_isp_wdr_wgtidx_thr2.bits.isp_wdr_short_thr2  = isp_wdr_short_thr;
    isp_be_regs->isp_wdr_wgtidx_thr2.u32 = o_isp_wdr_wgtidx_thr2.u32;
}

static __inline td_void isp_drv_set_wdr_mdt_en(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_mdt_en)
{
    u_isp_wdr_ctrl o_isp_wdr_ctrl;

    o_isp_wdr_ctrl.u32 = isp_be_regs->isp_wdr_ctrl.u32;
    o_isp_wdr_ctrl.bits.isp_wdr_mdt_en = isp_wdr_mdt_en;
    isp_be_regs->isp_wdr_ctrl.u32      = o_isp_wdr_ctrl.u32;
}

static __inline td_void isp_drv_set_wdr_fusion_mode(isp_pre_be_reg_type *isp_be_regs, td_u32 isp_wdr_fusion_mode)
{
    u_isp_wdr_ctrl o_isp_wdr_ctrl;

    o_isp_wdr_ctrl.u32 = isp_be_regs->isp_wdr_ctrl.u32;
    o_isp_wdr_ctrl.bits.isp_wdr_fusionmode = isp_wdr_fusion_mode;
    isp_be_regs->isp_wdr_ctrl.u32     = o_isp_wdr_ctrl.u32;
}

static __inline td_void isp_drv_set_wdr_saturate_thr(isp_pre_be_reg_type *isp_be_reg, td_u32 uisp_wdr_saturate_thr)
{
    u_isp_wdr_saturate_thr o_isp_wdr_saturate_thr;
    o_isp_wdr_saturate_thr.u32 = isp_be_reg->isp_wdr_saturate_thr.u32;
    o_isp_wdr_saturate_thr.bits.isp_wdr_saturate_thr = uisp_wdr_saturate_thr;
    isp_be_reg->isp_wdr_saturate_thr.u32 = o_isp_wdr_saturate_thr.u32;
}

static __inline td_void isp_drv_set_wdr_fusion_saturate_thr(isp_pre_be_reg_type *isp_be_reg, td_u32 fusion_saturate_thd)
{
    u_isp_wdr_fusion_sat_thd o_isp_wdr_fusion_sat_thd;
    o_isp_wdr_fusion_sat_thd.u32 = isp_be_reg->isp_wdr_fusion_sat_thd.u32;
    o_isp_wdr_fusion_sat_thd.bits.isp_wdr_fusion_saturate_thd = fusion_saturate_thd;
    isp_be_reg->isp_wdr_fusion_sat_thd.u32 = o_isp_wdr_fusion_sat_thd.u32;
}

static __inline td_void isp_drv_set_fpn_offset(isp_pre_be_reg_type *isp_be_regs, td_u32 offset)
{
    u_isp_fpn_corr0 isp_fpn_corr0;

    isp_fpn_corr0.u32 = isp_be_regs->isp_fpn_corr0.u32;
    isp_fpn_corr0.bits.isp_fpn_offset0 = offset;
    isp_be_regs->isp_fpn_corr0.u32 = isp_fpn_corr0.u32;
}

static __inline td_void isp_drv_set_fpn1_offset(isp_pre_be_reg_type *isp_be_regs, td_u32 offset)
{
    u_isp_fpn1_corr0 isp_fpn_corr0;

    isp_fpn_corr0.u32 = isp_be_regs->isp_fpn1_corr0.u32;
    isp_fpn_corr0.bits.isp_fpn1_offset0 = offset;
    isp_be_regs->isp_fpn1_corr0.u32 = isp_fpn_corr0.u32;
}

static __inline td_void isp_drv_set_fpn_correct_en(isp_pre_be_reg_type *isp_be_regs, td_bool enable)
{
    u_isp_fpn_corr_cfg isp_fpn_corr_cfg;

    isp_fpn_corr_cfg.u32 = isp_be_regs->isp_fpn_corr_cfg.u32;
    isp_fpn_corr_cfg.bits.isp_fpn_correct0_en = enable;
    isp_be_regs->isp_fpn_corr_cfg.u32 = isp_fpn_corr_cfg.u32;
}

static __inline td_void isp_drv_set_fpn1_correct_en(isp_pre_be_reg_type *isp_be_regs, td_bool enable)
{
    u_isp_fpn_corr_cfg isp_fpn_corr_cfg;

    isp_fpn_corr_cfg.u32 = isp_be_regs->isp_fpn1_corr_cfg.u32;
    isp_fpn_corr_cfg.bits.isp_fpn_correct0_en = enable;
    isp_be_regs->isp_fpn1_corr_cfg.u32 = isp_fpn_corr_cfg.u32;
}

static __inline td_void isp_drv_set_ldci_stat_evratio(isp_post_be_reg_type *isp_be_regs, td_u32 isp_ldci_stat_evratio)
{
    u_isp_ldci_stat_evratio o_isp_ldci_stat_evratio;

    o_isp_ldci_stat_evratio.u32 = isp_be_regs->isp_ldci_stat_evratio.u32;
    o_isp_ldci_stat_evratio.bits.isp_ldci_stat_evratio = isp_ldci_stat_evratio;
    isp_be_regs->isp_ldci_stat_evratio.u32 = o_isp_ldci_stat_evratio.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma0(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_0)
{
    u_isp_adrc_prev_luma_0 o_isp_drc_prev_luma_0;

    o_isp_drc_prev_luma_0.u32 = isp_be_regs->isp_adrc_prev_luma_0.u32;
    o_isp_drc_prev_luma_0.bits.isp_adrc_prev_luma_0 = isp_drc_prev_luma_0;
    isp_be_regs->isp_adrc_prev_luma_0.u32 = o_isp_drc_prev_luma_0.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma1(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_1)
{
    u_isp_adrc_prev_luma_1 o_isp_drc_prev_luma_1;

    o_isp_drc_prev_luma_1.u32 = isp_be_regs->isp_adrc_prev_luma_1.u32;
    o_isp_drc_prev_luma_1.bits.isp_adrc_prev_luma_1 = isp_drc_prev_luma_1;
    isp_be_regs->isp_adrc_prev_luma_1.u32 = o_isp_drc_prev_luma_1.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma2(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_2)
{
    u_isp_adrc_prev_luma_2 o_isp_drc_prev_luma_2;

    o_isp_drc_prev_luma_2.u32 = isp_be_regs->isp_adrc_prev_luma_2.u32;
    o_isp_drc_prev_luma_2.bits.isp_adrc_prev_luma_2 = isp_drc_prev_luma_2;
    isp_be_regs->isp_adrc_prev_luma_2.u32 = o_isp_drc_prev_luma_2.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma3(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_3)
{
    u_isp_adrc_prev_luma_3 o_isp_drc_prev_luma_3;

    o_isp_drc_prev_luma_3.u32 = isp_be_regs->isp_adrc_prev_luma_3.u32;
    o_isp_drc_prev_luma_3.bits.isp_adrc_prev_luma_3 = isp_drc_prev_luma_3;
    isp_be_regs->isp_adrc_prev_luma_3.u32 = o_isp_drc_prev_luma_3.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma4(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_4)
{
    u_isp_adrc_prev_luma_4 o_isp_drc_prev_luma_4;

    o_isp_drc_prev_luma_4.u32 = isp_be_regs->isp_adrc_prev_luma_4.u32;
    o_isp_drc_prev_luma_4.bits.isp_adrc_prev_luma_4 = isp_drc_prev_luma_4;
    isp_be_regs->isp_adrc_prev_luma_4.u32 = o_isp_drc_prev_luma_4.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma5(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_5)
{
    u_isp_adrc_prev_luma_5 o_isp_drc_prev_luma_5;

    o_isp_drc_prev_luma_5.u32 = isp_be_regs->isp_adrc_prev_luma_5.u32;
    o_isp_drc_prev_luma_5.bits.isp_adrc_prev_luma_5 = isp_drc_prev_luma_5;
    isp_be_regs->isp_adrc_prev_luma_5.u32 = o_isp_drc_prev_luma_5.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma6(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_6)
{
    u_isp_adrc_prev_luma_6 o_isp_drc_prev_luma_6;

    o_isp_drc_prev_luma_6.u32 = isp_be_regs->isp_adrc_prev_luma_6.u32;
    o_isp_drc_prev_luma_6.bits.isp_adrc_prev_luma_6 = isp_drc_prev_luma_6;
    isp_be_regs->isp_adrc_prev_luma_6.u32 = o_isp_drc_prev_luma_6.u32;
}

static __inline td_void isp_drv_set_drc_prev_luma7(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_prev_luma_7)
{
    u_isp_adrc_prev_luma_7 o_isp_drc_prev_luma_7;

    o_isp_drc_prev_luma_7.u32 = isp_be_regs->isp_adrc_prev_luma_7.u32;
    o_isp_drc_prev_luma_7.bits.isp_adrc_prev_luma_7 = isp_drc_prev_luma_7;
    isp_be_regs->isp_adrc_prev_luma_7.u32 = o_isp_drc_prev_luma_7.u32;
}

static __inline td_void isp_drv_set_drc_shp_cfg(isp_post_be_reg_type *isp_be_regs,
                                                td_u32 isp_drc_shp_log_luma, td_u32 isp_drc_shp_exp_luma)
{
    u_isp_adrc_shp_cfg_luma o_isp_adrc_shp_cfg_luma;

    o_isp_adrc_shp_cfg_luma.u32 = isp_be_regs->isp_adrc_shp_cfg_luma.u32;
    o_isp_adrc_shp_cfg_luma.bits.isp_adrc_shp_log_luma = isp_drc_shp_log_luma;
    o_isp_adrc_shp_cfg_luma.bits.isp_adrc_shp_exp_luma = isp_drc_shp_exp_luma;
    isp_be_regs->isp_adrc_shp_cfg_luma.u32 = o_isp_adrc_shp_cfg_luma.u32;
}

static __inline td_void isp_drv_set_drc_div_denom_log(isp_post_be_reg_type *isp_be_regs,
                                                      td_u32 isp_drc_div_denom_log_luma)
{
    u_isp_adrc_div_denom_log_luma o_isp_adrc_div_denom_log_luma;

    o_isp_adrc_div_denom_log_luma.u32 = isp_be_regs->isp_adrc_div_denom_log_luma.u32;
    o_isp_adrc_div_denom_log_luma.bits.isp_adrc_div_denom_log_luma = isp_drc_div_denom_log_luma;
    isp_be_regs->isp_adrc_div_denom_log_luma.u32 = o_isp_adrc_div_denom_log_luma.u32;
}

static __inline td_void isp_drv_set_drc_denom_exp(isp_post_be_reg_type *isp_be_regs, td_u32 isp_drc_denom_exp)
{
    u_isp_adrc_denom_exp_luma o_isp_adrc_denom_exp_luma;

    o_isp_adrc_denom_exp_luma.u32 = isp_be_regs->isp_adrc_denom_exp_luma.u32;
    o_isp_adrc_denom_exp_luma.bits.isp_adrc_denom_exp_luma = isp_drc_denom_exp;
    isp_be_regs->isp_adrc_denom_exp_luma.u32 = o_isp_adrc_denom_exp_luma.u32;
}

static __inline td_void isp_drv_set_be_regup(isp_viproc_reg_type *viproc_reg, td_u32 reg_up)
{
    u_viproc_isp_ispbe_regup o_viproc_regup;
    o_viproc_regup.u32 = viproc_reg->viproc_ispbe_regup.u32;
    o_viproc_regup.bits.ispbe_reg_up = reg_up;
    viproc_reg->viproc_ispbe_regup.u32 = o_viproc_regup.u32;
}

/* Fe Blc */
static __inline td_void isp_drv_fe_dg_offset_write(isp_fe_reg_type *fe_reg, const td_u16 *dg_blc)
{
    fe_reg->isp_dg2_blc_offset1.u32 = ((td_u32)dg_blc[OT_ISP_CHN_R] << 16) + dg_blc[OT_ISP_CHN_GR]; /* bit16~30: r */
    fe_reg->isp_dg2_blc_offset2.u32 = ((td_u32)dg_blc[OT_ISP_CHN_B] << 16) + dg_blc[OT_ISP_CHN_GB]; /* bit16~30: b */
}

static __inline td_void isp_drv_fe_wb_offset_write(isp_fe_reg_type *fe_reg, const td_u16 *wb_blc)
{
    fe_reg->isp_wb1_blc_offset1.u32 = ((td_u32)wb_blc[OT_ISP_CHN_R] << 16) + wb_blc[OT_ISP_CHN_GR]; /* bit16~30: r */
    fe_reg->isp_wb1_blc_offset2.u32 = ((td_u32)wb_blc[OT_ISP_CHN_B] << 16) + wb_blc[OT_ISP_CHN_GB]; /* bit16~30: b */
}

static __inline td_void isp_drv_fe_ae_offset_write(isp_fe_reg_type *fe_reg, const td_u16 *ae_blc)
{
    fe_reg->isp_ae_offset_r_gr.u32 = ((td_u32)ae_blc[OT_ISP_CHN_GR] << 16) + ae_blc[OT_ISP_CHN_R]; /* bit16~30: gr */
    fe_reg->isp_ae_offset_gb_b.u32 = ((td_u32)ae_blc[OT_ISP_CHN_B] << 16) + ae_blc[OT_ISP_CHN_GB]; /* bit16~30: b */
}

static __inline td_void isp_drv_fe_af_offset_write(isp_fe_reg_type *fe_reg, const td_u16 *af_blc)
{
    fe_reg->isp_af_offset.u32 = ((td_u32)af_blc[OT_ISP_CHN_GB] << 16) + af_blc[OT_ISP_CHN_GR]; /* bit16~30: gb */
}

static __inline td_void isp_drv_fe_blc_offset_write(isp_fe_reg_type *fe_reg, const td_u16 *fe_blc)
{
    fe_reg->isp_blc1_offset1.u32 = ((td_u32)fe_blc[OT_ISP_CHN_R] << 16) + fe_blc[OT_ISP_CHN_GR]; /* bit16~30: r_blc */
    fe_reg->isp_blc1_offset2.u32 = ((td_u32)fe_blc[OT_ISP_CHN_B] << 16) + fe_blc[OT_ISP_CHN_GB]; /* bit16~30: b_blc */
}

static __inline td_void isp_drv_be_f0_4dg_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f0_4dg_blc)
{
    be_reg->isp_4dg_0_blc_offset1.u32 = ((td_u32)f0_4dg_blc[OT_ISP_CHN_R] << 16) + /* bit16~30: r_blc */
                                        f0_4dg_blc[OT_ISP_CHN_GR];

    be_reg->isp_4dg_0_blc_offset2.u32 = ((td_u32)f0_4dg_blc[OT_ISP_CHN_B] << 16) + /* bit16~30: b_blc */
                                        f0_4dg_blc[OT_ISP_CHN_GB];
}

static __inline td_void isp_drv_be_f1_4dg_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f1_4dg_blc)
{
    be_reg->isp_4dg_1_blc_offset1.u32 = ((td_u32)f1_4dg_blc[OT_ISP_CHN_R] << 16) + /* bit16~30: r_blc */
                                        f1_4dg_blc[OT_ISP_CHN_GR];

    be_reg->isp_4dg_1_blc_offset2.u32 = ((td_u32)f1_4dg_blc[OT_ISP_CHN_B] << 16) + /* bit16~30: b_blc */
                                        f1_4dg_blc[OT_ISP_CHN_GB];
}

static __inline td_void isp_drv_be_f2_4dg_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f2_4dg_blc)
{
    be_reg->isp_4dg_2_blc_offset1.u32 = ((td_u32)f2_4dg_blc[OT_ISP_CHN_R] << 16) + /* bit16~30: r_blc */
                                        f2_4dg_blc[OT_ISP_CHN_GR];

    be_reg->isp_4dg_2_blc_offset2.u32 = ((td_u32)f2_4dg_blc[OT_ISP_CHN_B] << 16) + /* bit16~30: b_blc */
                                        f2_4dg_blc[OT_ISP_CHN_GB];
}

static __inline td_void isp_drv_be_f3_4dg_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f3_4dg_blc)
{
    be_reg->isp_4dg_3_blc_offset1.u32 = ((td_u32)f3_4dg_blc[OT_ISP_CHN_R] << 16) + /* bit16~30: r_blc */
                                        f3_4dg_blc[OT_ISP_CHN_GR];

    be_reg->isp_4dg_3_blc_offset2.u32 = ((td_u32)f3_4dg_blc[OT_ISP_CHN_B] << 16) + /* bit16~30: b_blc */
                                        f3_4dg_blc[OT_ISP_CHN_GB];
}

static __inline td_void isp_drv_be_ge0_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *ge0_blc)
{
    be_reg->isp_ge0_blc_offset.u32 = ((td_u32)ge0_blc[OT_ISP_CHN_GR] << 16) + /* bit16~30: gr_blc */
                                        ge0_blc[OT_ISP_CHN_GB];
}

static __inline td_void isp_drv_be_ge1_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *ge1_blc)
{
    be_reg->isp_ge1_blc_offset.u32 = ((td_u32)ge1_blc[OT_ISP_CHN_GR] << 16) + /* bit16~30: gr_blc */
                                        ge1_blc[OT_ISP_CHN_GB];
}

static __inline td_void isp_drv_wdr_out_blc_write(isp_pre_be_reg_type *be_reg, td_u16 out_blc)
{
    u_isp_wdr_outblc o_isp_wdr_outblc;

    o_isp_wdr_outblc.u32 = be_reg->isp_wdr_outblc.u32;
    o_isp_wdr_outblc.bits.isp_wdr_outblc = out_blc;
    be_reg->isp_wdr_outblc.u32 = o_isp_wdr_outblc.u32;
}

static __inline td_void isp_drv_be_f0_wdr_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f0_wdr_blc)
{
    be_reg->isp_wdr_f0_inblc0.u32 = ((td_u32)f0_wdr_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                    f0_wdr_blc[OT_ISP_CHN_GR];

    be_reg->isp_wdr_f0_inblc1.u32 = ((td_u32)f0_wdr_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                    f0_wdr_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f1_wdr_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f1_wdr_blc)
{
    be_reg->isp_wdr_f1_inblc0.u32 = ((td_u32)f1_wdr_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                    f1_wdr_blc[OT_ISP_CHN_GR];

    be_reg->isp_wdr_f1_inblc1.u32 = ((td_u32)f1_wdr_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                    f1_wdr_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f2_wdr_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f2_wdr_blc)
{
    be_reg->isp_wdr_f2_inblc0.u32 = ((td_u32)f2_wdr_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                    f2_wdr_blc[OT_ISP_CHN_GR];

    be_reg->isp_wdr_f2_inblc1.u32 = ((td_u32)f2_wdr_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                    f2_wdr_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f3_wdr_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f3_wdr_blc)
{
    be_reg->isp_wdr_f3_inblc0.u32 = ((td_u32)f3_wdr_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                    f3_wdr_blc[OT_ISP_CHN_GR];

    be_reg->isp_wdr_f3_inblc1.u32 = ((td_u32)f3_wdr_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                    f3_wdr_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f0_flick_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f0_flick_blc)
{
    be_reg->isp_flick_f0_inblc0.u32 = ((td_u32)f0_flick_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                      f0_flick_blc[OT_ISP_CHN_GR];

    be_reg->isp_flick_f0_inblc1.u32 = ((td_u32)f0_flick_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                      f0_flick_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f1_flick_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f1_flick_blc)
{
    be_reg->isp_flick_f1_inblc0.u32 = ((td_u32)f1_flick_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                      f1_flick_blc[OT_ISP_CHN_GR];

    be_reg->isp_flick_f1_inblc1.u32 = ((td_u32)f1_flick_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                      f1_flick_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f2_flick_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f2_flick_blc)
{
    be_reg->isp_flick_f2_inblc0.u32 = ((td_u32)f2_flick_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                      f2_flick_blc[OT_ISP_CHN_GR];

    be_reg->isp_flick_f2_inblc1.u32 = ((td_u32)f2_flick_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                      f2_flick_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_f3_flick_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *f3_flick_blc)
{
    be_reg->isp_flick_f3_inblc0.u32 = ((td_u32)f3_flick_blc[OT_ISP_CHN_R] << 16) +  /* bit16~30: r_blc */
                                      f3_flick_blc[OT_ISP_CHN_GR];

    be_reg->isp_flick_f3_inblc1.u32 = ((td_u32)f3_flick_blc[OT_ISP_CHN_GB] << 16) + /* bit16~30: gb_blc */
                                      f3_flick_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_bnr_offset_write(isp_post_be_reg_type *be_reg, td_u16 bnr_rlmt_blc)
{
    u_isp_bnr_flt_ctrl2 o_isp_bnr_flt_ctrl2;
    o_isp_bnr_flt_ctrl2.u32 = be_reg->isp_bnr_flt_ctrl2.u32;
    o_isp_bnr_flt_ctrl2.bits.isp_bnr_blc = bnr_rlmt_blc;
    be_reg->isp_bnr_flt_ctrl2.u32 = o_isp_bnr_flt_ctrl2.u32;
}

static __inline td_void isp_drv_be_lsc_offset_write(isp_post_be_reg_type *be_reg, const td_u16 *lsc_blc)
{
    be_reg->isp_lsc_blc0.u32 = ((td_u32)lsc_blc[OT_ISP_CHN_GR] << 16) + lsc_blc[OT_ISP_CHN_R]; /* bit16~30: gr_blc */
    be_reg->isp_lsc_blc1.u32 = ((td_u32)lsc_blc[OT_ISP_CHN_GB] << 16) + lsc_blc[OT_ISP_CHN_B]; /* bit16~30: gb_blc */
}

static __inline td_void isp_drv_be_dgain_offset_write(isp_post_be_reg_type *be_reg, const td_u16 *dg_blc)
{
    be_reg->isp_dg_blc_offset1.u32 = ((td_u32)dg_blc[OT_ISP_CHN_R] << 16) + dg_blc[OT_ISP_CHN_GR]; /* bit16~30: r */
    be_reg->isp_dg_blc_offset2.u32 = ((td_u32)dg_blc[OT_ISP_CHN_B] << 16) + dg_blc[OT_ISP_CHN_GB]; /* bit16~30: b */
}

static __inline td_void isp_drv_be_ae_offset_write(isp_post_be_reg_type *be_reg, const  td_u16 *ae_blc)
{
    be_reg->isp_ae_offset_r_gr.u32 = ((td_u32)ae_blc[OT_ISP_CHN_GR] << 16) + ae_blc[OT_ISP_CHN_R]; /* bit16~30: gr */
    be_reg->isp_ae_offset_gb_b.u32 = ((td_u32)ae_blc[OT_ISP_CHN_B] << 16) + ae_blc[OT_ISP_CHN_GB]; /* bit16~30: b */
}
static __inline td_void isp_drv_be_af_offset_write(isp_post_be_reg_type *be_reg, const td_u16 *af_blc)
{
    be_reg->isp_af_offset.u32 = ((td_u32)af_blc[OT_ISP_CHN_GB] << 16) + af_blc[OT_ISP_CHN_GR]; /* bit16~30: gb */
}

static __inline td_void isp_drv_be_la_offset_write(isp_post_be_reg_type *be_reg, const td_u16 *la_blc)
{
    be_reg->isp_la_offset_r.u32  = la_blc[OT_ISP_CHN_R];
    be_reg->isp_la_offset_gr.u32 = la_blc[OT_ISP_CHN_GR];
    be_reg->isp_la_offset_gb.u32 = la_blc[OT_ISP_CHN_GB];
    be_reg->isp_la_offset_b.u32  = la_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_wb_offset_write(isp_post_be_reg_type *be_reg, const td_u16 *wb_blc)
{
    be_reg->isp_wb_blc_offset1.u32  = ((td_u32)wb_blc[OT_ISP_CHN_R] << 16) + wb_blc[OT_ISP_CHN_GR]; /* bit16~30: r */
    be_reg->isp_wb_blc_offset2.u32  = ((td_u32)wb_blc[OT_ISP_CHN_B] << 16) + wb_blc[OT_ISP_CHN_GB]; /* bit16~30: b */
}

static __inline td_void isp_drv_be_expander_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *expander_blc)
{
    be_reg->isp_expander_offsetr.u32  = expander_blc[OT_ISP_CHN_R];
    be_reg->isp_expander_offsetgr.u32 = expander_blc[OT_ISP_CHN_GR];
    be_reg->isp_expander_offsetgb.u32 = expander_blc[OT_ISP_CHN_GB];
    be_reg->isp_expander_offsetb.u32  = expander_blc[OT_ISP_CHN_B];
}

static __inline td_void isp_drv_be_rgbir_offset_write(isp_pre_be_reg_type *be_reg, const td_u16 *rgbir_blc)
{
    be_reg->isp_rgbir_blc_offset_r.u32  = rgbir_blc[0]; /* index 0 */
    be_reg->isp_rgbir_blc_offset_g.u32  = rgbir_blc[1]; /* index 1 */
    be_reg->isp_rgbir_blc_offset_b.u32  = rgbir_blc[2]; /* index 2 */
    be_reg->isp_rgbir_blc_offset_ir.u32 = rgbir_blc[3]; /* index 3 */
}

static __inline td_void isp_drv_be_format_write(isp_post_be_reg_type *be_reg, isp_be_format be_format)
{
    u_isp_be_format o_isp_be_format;
    o_isp_be_format.u32 = be_reg->isp_be_format.u32;
    o_isp_be_format.bits.isp_format = be_format;
    be_reg->isp_be_format.u32 = o_isp_be_format.u32;
}

/* just called by isp_drv_reg_config_chn_sel */
static td_void isp_drv_reg_config_chn_sel_subfunc0(td_u32 channel_sel, isp_drv_ctx *drv_ctx,  td_u32 *chn_switch)
{
    switch (channel_sel & 0x3) {
        case ISP_CHN_SWITCH_NORMAL:
            chn_switch[0] = (drv_ctx->sync_cfg.vc_num_max - drv_ctx->sync_cfg.vc_num) %
                            div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
            chn_switch[1] = (chn_switch[0] + 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 1 */
            chn_switch[2] = (chn_switch[0] + 2) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 2 */
            chn_switch[3] = (chn_switch[0] + 3) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 3 */
            break;

        case ISP_CHN_SWITCH_2LANE:
            chn_switch[1] = (drv_ctx->sync_cfg.vc_num_max - drv_ctx->sync_cfg.vc_num) %
                            div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
            chn_switch[0] = (chn_switch[1] + 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 0 & 1 */
            chn_switch[2] = (chn_switch[1] + 2) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 2 & 1 */
            chn_switch[3] = (chn_switch[1] + 3) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 3 & 1 */
            break;

        case ISP_CHN_SWITCH_3LANE:
            chn_switch[2] = (drv_ctx->sync_cfg.vc_num_max - drv_ctx->sync_cfg.vc_num) %  /* index 2 */
                            div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
            chn_switch[1] = (chn_switch[2] + 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 1 & 2 */
            chn_switch[0] = (chn_switch[2] + 2) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 0 & 2 */
            chn_switch[3] = (chn_switch[2] + 3) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 3 & 2 */
            break;

        case ISP_CHN_SWITCH_4LANE:
            chn_switch[3] = (drv_ctx->sync_cfg.vc_num_max - drv_ctx->sync_cfg.vc_num) %  /* index 3 */
                            div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
            chn_switch[2] = (chn_switch[3] + 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 2 & 3+1 */
            chn_switch[1] = (chn_switch[3] + 2) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 1 & 3+2 */
            chn_switch[0] = (chn_switch[3] + 3) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1); /* index 0 & 3+3 */
            break;
        default:
            break;
    }
}

static td_void isp_drv_reg_config_chn_sel_subfunc1(td_u32 channel_sel, isp_drv_ctx *drv_ctx,
    td_u32 *chn_switch, td_u8 length)
{
    ot_unused(length);

    switch (channel_sel & 0x3) {
        case ISP_CHN_SWITCH_NORMAL:
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0]; /* array index 0 */
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1]; /* array index 1 */
            chn_switch[2] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[2]; /* array index 2 */
            chn_switch[3] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[3]; /* array index 3 */
            break;

        case ISP_CHN_SWITCH_2LANE:
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0]; /* array index 1 */
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1]; /* array index 0 */
            chn_switch[2] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[2]; /* array index 2 */
            chn_switch[3] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[3]; /* array index 3 */
            break;

        case ISP_CHN_SWITCH_3LANE:
            chn_switch[2] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0]; /* array index 2 */
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1]; /* array index 1 */
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[2]; /* array index 0 & 2 */
            chn_switch[3] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[3]; /* array index 3 */
            break;

        case ISP_CHN_SWITCH_4LANE:
            chn_switch[3] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[0]; /* array index 3 */
            chn_switch[2] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[1]; /* array index 2 */
            chn_switch[1] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[2]; /* array index 1 & 2 */
            chn_switch[0] = drv_ctx->chn_sel_attr[0].wdr_chn_sel[3]; /* array index 0 & 3 */
            break;
        default:
            break;
    }
}

static td_void isp_drv_set_pre_be_input_sel(ot_vi_pipe vi_pipe, const td_u32 *input_sel, td_u8 length)
{
    td_u8 k, blk_dev;
    td_s32 ret;
    isp_pre_be_reg_type *be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_unused(length);

    ret = isp_drv_get_pre_be_reg_virt_addr(vi_pipe, be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    blk_dev = drv_ctx->work_mode.block_dev;

    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_drv_input_sel_write(be_reg[k + blk_dev], input_sel);
    }
    return;
}

static td_void isp_drv_set_post_be_input_sel(ot_vi_pipe vi_pipe, const td_u32 *input_sel, td_u8 length)
{
    td_u8 k, blk_dev;
    td_s32 ret;
    isp_post_be_reg_type *be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    isp_drv_ctx *drv_ctx = TD_NULL;
    ot_unused(length);

    ret = isp_drv_get_post_be_reg_virt_addr(vi_pipe, be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    blk_dev = drv_ctx->work_mode.block_dev;

    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_drv_post_be_input_sel_write(be_reg[k + blk_dev], input_sel);
    }
    return;
}


td_void isp_drv_set_input_sel(ot_vi_pipe vi_pipe, td_u32 *input_sel, td_u8 length)
{
    isp_drv_set_pre_be_input_sel(vi_pipe, input_sel, length);
    isp_drv_set_post_be_input_sel(vi_pipe, input_sel, length);

    return;
}

td_s32 isp_drv_reg_config_chn_sel(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u32  chn_switch[ISP_CHN_SWITCH_NUM]  = {0};
    td_u32  channel_sel;

    isp_check_pointer_return(drv_ctx);

    channel_sel = drv_ctx->chn_sel_attr[0].channel_sel;
    chn_switch[4] = (drv_ctx->yuv_mode == TD_TRUE) ? 1 : 0; /* array index 4 */

    if (is_full_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_drv_reg_config_chn_sel_subfunc0(channel_sel, drv_ctx, &chn_switch[0]);
        isp_drv_set_input_sel(vi_pipe, &chn_switch[0], sizeof(chn_switch));
    } else if ((is_line_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) ||
               (is_half_wdr_mode(drv_ctx->sync_cfg.wdr_mode))) {
        isp_drv_reg_config_chn_sel_subfunc1(channel_sel, drv_ctx, &chn_switch[0], sizeof(chn_switch));

        /* offline mode: isp BE buffer poll, so chn switch need each frame refres */
        if (is_offline_mode(drv_ctx->work_mode.running_mode) ||
            (is_striping_mode(drv_ctx->work_mode.running_mode)) ||
            (is_pre_online_post_offline(drv_ctx->work_mode.running_mode))) {
            isp_drv_set_input_sel(vi_pipe, &chn_switch[0], sizeof(chn_switch));
        }
    } else {
    }

    return TD_SUCCESS;
}

static td_void isp_drv_set_pre_be_blc_reg(isp_pre_be_reg_type *pre_be, const isp_be_blc_dyna_cfg *be_blc_reg_cfg)
{
    /* 4dg */
    isp_drv_be_f0_4dg_offset_write(pre_be, be_blc_reg_cfg->wdr_dg_blc[0].blc); /* wdr0 */
    isp_drv_be_f1_4dg_offset_write(pre_be, be_blc_reg_cfg->wdr_dg_blc[1].blc); /* wdr1 */
    isp_drv_be_f2_4dg_offset_write(pre_be, be_blc_reg_cfg->wdr_dg_blc[2].blc); /* wdr2 */
    isp_drv_be_f3_4dg_offset_write(pre_be, be_blc_reg_cfg->wdr_dg_blc[3].blc); /* wdr3 */

    /* ge */
    isp_drv_be_ge0_offset_write(pre_be, be_blc_reg_cfg->ge_blc[0].blc); /* wdr0 */
    isp_drv_be_ge1_offset_write(pre_be, be_blc_reg_cfg->ge_blc[1].blc); /* wdr1 */

    /* wdr */
    isp_drv_wdr_out_blc_write(pre_be, be_blc_reg_cfg->wdr_blc[0].out_blc);
    isp_drv_be_f0_wdr_offset_write(pre_be, be_blc_reg_cfg->wdr_blc[0].blc); /* wdr0 */
    isp_drv_be_f1_wdr_offset_write(pre_be, be_blc_reg_cfg->wdr_blc[1].blc); /* wdr1 */
    isp_drv_be_f2_wdr_offset_write(pre_be, be_blc_reg_cfg->wdr_blc[2].blc); /* wdr2 */
    isp_drv_be_f3_wdr_offset_write(pre_be, be_blc_reg_cfg->wdr_blc[3].blc); /* wdr3 */

    /* flicker */
    isp_drv_be_f0_flick_offset_write(pre_be, be_blc_reg_cfg->flicker_blc[0].blc); /* wdr0 */
    isp_drv_be_f1_flick_offset_write(pre_be, be_blc_reg_cfg->flicker_blc[1].blc); /* wdr1 */
    isp_drv_be_f2_flick_offset_write(pre_be, be_blc_reg_cfg->flicker_blc[2].blc); /* wdr2 */
    isp_drv_be_f3_flick_offset_write(pre_be, be_blc_reg_cfg->flicker_blc[3].blc); /* wdr3 */
    isp_drv_be_expander_offset_write(pre_be, be_blc_reg_cfg->expander_blc.blc); /* expander */
    isp_drv_be_rgbir_offset_write(pre_be, be_blc_reg_cfg->rgbir_blc.blc); /* rgbir */
}

static td_void isp_drv_set_post_be_blc_reg(isp_post_be_reg_type *post_be, const isp_be_blc_dyna_cfg *be_blc_reg_cfg)
{
    isp_drv_be_bnr_offset_write(post_be, be_blc_reg_cfg->bnr_blc.blc[0] >> 2); /* shift 2bits */
    isp_drv_be_lsc_offset_write(post_be, be_blc_reg_cfg->lsc_blc.blc); /* lsc */
    isp_drv_be_dgain_offset_write(post_be, be_blc_reg_cfg->dg_blc.blc); /* isp_dgain */
    isp_drv_be_ae_offset_write(post_be, be_blc_reg_cfg->ae_blc.blc); /* ae */
    isp_drv_be_la_offset_write(post_be, be_blc_reg_cfg->mg_blc.blc); /* la */
    isp_drv_be_wb_offset_write(post_be, be_blc_reg_cfg->wb_blc.blc); /* wb */
    isp_drv_be_af_offset_write(post_be, be_blc_reg_cfg->af_blc.blc); /* ae */
}

/* dynamic mode: read ob stats and cfg fe_blc in frame_delay or frame_end interrupt */
static td_void isp_drv_calc_noarmal_blc_delay(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u8 fe_cfg_node_idx)
{
    ot_isp_black_level_mode pre_black_level_mode, cur_black_level_mode;

    cur_black_level_mode = drv_ctx->sync_cfg.node[0]->dynamic_blc_cfg.black_level_mode;
    if (drv_ctx->sync_cfg.node[1] != TD_NULL) {
        pre_black_level_mode = drv_ctx->sync_cfg.node[1]->dynamic_blc_cfg.black_level_mode;
    } else {
        pre_black_level_mode = cur_black_level_mode;
    }

    if ((pre_black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) &&
        (cur_black_level_mode != OT_ISP_BLACK_LEVEL_MODE_DYNAMIC)) {
        drv_ctx->dyna_blc_info.fe_blc_mode_change      = TD_TRUE;
        drv_ctx->dyna_blc_info.pre_be_blc_mode_change  = TD_TRUE;
        drv_ctx->dyna_blc_info.post_be_blc_mode_change = TD_TRUE;
        drv_ctx->dyna_blc_info.fe_delay_cnt      = fe_cfg_node_idx;
        drv_ctx->dyna_blc_info.pre_be_delay_cnt  = MIN2(isp_drv_get_pre_be_sync_index(vi_pipe, drv_ctx),
                                                        CFG2VLD_DLY_LIMIT - 1);
        drv_ctx->dyna_blc_info.post_be_delay_cnt = MIN2(isp_drv_get_be_sync_index(vi_pipe, drv_ctx),
                                                        CFG2VLD_DLY_LIMIT - 1);
        drv_ctx->dyna_blc_info.sync_cfg.black_level_mode = cur_black_level_mode;
    }
}

static td_void isp_drv_update_dyna_blc_info(ot_vi_pipe vi_pipe, isp_dynamic_blc_info *dyna_blc_info,
    const isp_drv_ctx *drv_ctx)
{
    td_u8 delay_cnt, cur_idx;

    if (isp_drv_get_ob_stats_update_pos(vi_pipe) == OT_ISP_UPDATE_OB_STATS_FE_FRAME_START) {
        delay_cnt = 2; /* delay cnt 2 */
    } else {
        delay_cnt = 1;
    }
    if (drv_ctx->run_once_flag == TD_TRUE) {
        delay_cnt = 3; /* runonce delay 3 */
    }

    (td_void)memcpy_s(&dyna_blc_info->sync_cfg, sizeof(isp_dynamic_blc_sync_cfg),
                      &drv_ctx->sync_cfg.node[0]->dynamic_blc_cfg, sizeof(isp_dynamic_blc_sync_cfg));
    (td_void)memcpy_s(&dyna_blc_info->fpn_cfg[0], sizeof(isp_fpn_sync_cfg),
                      &drv_ctx->sync_cfg.node[0]->fpn_cfg, sizeof(isp_fpn_sync_cfg));

    cur_idx = delay_cnt - 1;
    if (drv_ctx->sync_cfg.node[cur_idx] == TD_NULL) {
        dyna_blc_info->ob_stats_read_en = TD_FALSE;
        return;
    }

    dyna_blc_info->sync_cfg.black_level_mode = drv_ctx->sync_cfg.node[cur_idx]->dynamic_blc_cfg.black_level_mode;

    if (dyna_blc_info->sync_cfg.black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
        dyna_blc_info->ob_stats_read_en = TD_TRUE;

        if (dyna_blc_info->pre_black_level_mode == OT_ISP_BLACK_LEVEL_MODE_BUTT) {
            dyna_blc_info->is_first_frame = TD_TRUE;
        } else if (dyna_blc_info->pre_black_level_mode != OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
            dyna_blc_info->is_first_frame = TD_TRUE;
        } else {
            dyna_blc_info->is_first_frame = TD_FALSE;
        }
    }

    dyna_blc_info->pre_black_level_mode = dyna_blc_info->sync_cfg.black_level_mode;
}

static td_bool isp_drv_calc_cfg_fe_blc_en(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u8 fe_cfg_node_idx)
{
    td_bool cfg_blc_en = TD_TRUE;
    isp_dynamic_blc_info *dyna_blc_info = TD_NULL;

    dyna_blc_info = &drv_ctx->dyna_blc_info;

    isp_drv_update_dyna_blc_info(vi_pipe, dyna_blc_info, drv_ctx);
    isp_drv_calc_noarmal_blc_delay(vi_pipe, drv_ctx, fe_cfg_node_idx);

    if (dyna_blc_info->sync_cfg.black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
        return TD_FALSE;
    }

    if (dyna_blc_info->fe_blc_mode_change != TD_TRUE) {
        return TD_TRUE;
    }

    if (dyna_blc_info->fe_delay_cnt != 0) {
        cfg_blc_en = TD_FALSE;
        dyna_blc_info->fe_delay_cnt -= 1;
    }

    if (dyna_blc_info->fe_delay_cnt == 0) {
        dyna_blc_info->fe_blc_mode_change = TD_FALSE;
    }

    return cfg_blc_en;
}

static td_bool isp_drv_calc_cfg_post_be_blc_en(isp_drv_ctx *drv_ctx)
{
    td_bool cfg_blc_en = TD_TRUE;
    isp_dynamic_blc_info *dyna_blc_info = &drv_ctx->dyna_blc_info;

    if (dyna_blc_info->sync_cfg.black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
        return TD_FALSE;
    }

    if (dyna_blc_info->post_be_blc_mode_change != TD_TRUE) {
        return TD_TRUE;
    }

    if (dyna_blc_info->post_be_delay_cnt != 0) {
        cfg_blc_en = TD_FALSE;
        dyna_blc_info->post_be_delay_cnt -= 1;
    }

    if (dyna_blc_info->post_be_delay_cnt == 0) {
        dyna_blc_info->post_be_blc_mode_change = TD_FALSE;
    }

    return cfg_blc_en;
}

static td_bool isp_drv_calc_cfg_pre_be_blc_en(isp_drv_ctx *drv_ctx)
{
    td_bool cfg_blc_en = TD_TRUE;
    isp_dynamic_blc_info *dyna_blc_info = &drv_ctx->dyna_blc_info;

    if (dyna_blc_info->sync_cfg.black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
        return TD_FALSE;
    }

    if (dyna_blc_info->pre_be_blc_mode_change != TD_TRUE) {
        return TD_TRUE;
    }

    if (dyna_blc_info->pre_be_delay_cnt != 0) {
        cfg_blc_en = TD_FALSE;
        dyna_blc_info->pre_be_delay_cnt -= 1;
    }

    if (dyna_blc_info->pre_be_delay_cnt == 0) {
        dyna_blc_info->pre_be_blc_mode_change = TD_FALSE;
    }

    return cfg_blc_en;
}

static td_void isp_drv_offline_be_blc_reg(isp_drv_ctx *drv_ctx, const isp_sync_cfg_buf_node *cfg_node)
{
    td_u8 stitch_idx = 0;
    ot_vi_pipe stitch_pipe;
    isp_drv_ctx *drv_ctx_stitch_pipe = TD_NULL;
    td_bool cfg_blc_en = TD_TRUE;

    (td_void)memcpy_s(&drv_ctx->be_sync_para.fpn_cfg, sizeof(isp_fpn_sync_cfg),
                      &cfg_node->fpn_cfg, sizeof(isp_fpn_sync_cfg));
    if (drv_ctx->stitch_attr.stitch_enable == TD_FALSE) {
        cfg_blc_en = isp_drv_calc_cfg_post_be_blc_en(drv_ctx);
        if (cfg_blc_en == TD_FALSE) {
            return;
        }
        (td_void)memcpy_s(&drv_ctx->be_sync_para.be_blc, sizeof(isp_be_blc_dyna_cfg),
                          &cfg_node->be_blc_reg_cfg, sizeof(isp_be_blc_dyna_cfg));

        return;
    }

    if (drv_ctx->stitch_attr.main_pipe != TD_TRUE) {
        return;
    }

    for (stitch_idx = 0; stitch_idx < drv_ctx->stitch_attr.stitch_pipe_num; stitch_idx++) {
        stitch_pipe = drv_ctx->stitch_attr.stitch_bind_id[stitch_idx];
        drv_ctx_stitch_pipe = isp_drv_get_ctx(stitch_pipe);

        cfg_blc_en = isp_drv_calc_cfg_post_be_blc_en(drv_ctx_stitch_pipe);
        if (cfg_blc_en == TD_FALSE) {
            continue;
        }

        (td_void)memcpy_s(&drv_ctx_stitch_pipe->be_sync_para.be_blc, sizeof(isp_be_blc_dyna_cfg),
                          &cfg_node->be_blc_reg_cfg_stitch[stitch_idx], sizeof(isp_be_blc_dyna_cfg));
        /* for main pipe */
        (td_void)memcpy_s(&drv_ctx->be_sync_para_stitch[stitch_idx].be_blc, sizeof(isp_be_blc_dyna_cfg),
                          &cfg_node->be_blc_reg_cfg_stitch[stitch_idx], sizeof(isp_be_blc_dyna_cfg));
    }

    return;
}

static td_void isp_drv_get_be_blc_online(isp_drv_ctx *drv_ctx, const isp_sync_cfg_buf_node *cfg_node)
{
    td_bool cfg_blc_en;

    cfg_blc_en = isp_drv_calc_cfg_post_be_blc_en(drv_ctx);
    if (cfg_blc_en == TD_FALSE) {
        return;
    }
    (td_void)memcpy_s(&drv_ctx->be_sync_para.be_blc, sizeof(isp_be_blc_dyna_cfg),
                      &cfg_node->be_blc_reg_cfg, sizeof(isp_be_blc_dyna_cfg));
    (td_void)memcpy_s(&drv_ctx->be_sync_para.fpn_cfg, sizeof(isp_fpn_sync_cfg),
                      &cfg_node->fpn_cfg, sizeof(isp_fpn_sync_cfg));
}

static td_void isp_drv_get_post_be_blc_pre_online_post_offline(isp_drv_ctx *drv_ctx,
    const isp_sync_cfg_buf_node *cfg_node)
{
    td_bool cfg_blc_en;
    td_u16  blc_dyna_size = sizeof(isp_blc_dyna_cfg);
    isp_be_blc_dyna_cfg *be_blc_sync = TD_NULL;
    const isp_be_blc_dyna_cfg *be_blc_cfg = TD_NULL;

    cfg_blc_en = isp_drv_calc_cfg_post_be_blc_en(drv_ctx);
    if (cfg_blc_en == TD_FALSE) {
        return;
    }
    be_blc_sync = &drv_ctx->be_sync_para.be_blc;
    be_blc_cfg  = &cfg_node->be_blc_reg_cfg;
    (td_void)memcpy_s(&be_blc_sync->lsc_blc, blc_dyna_size, &be_blc_cfg->lsc_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->dg_blc, blc_dyna_size, &be_blc_cfg->dg_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->wb_blc, blc_dyna_size, &be_blc_cfg->wb_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->ae_blc, blc_dyna_size, &be_blc_cfg->ae_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->mg_blc, blc_dyna_size, &be_blc_cfg->mg_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->bnr_blc, blc_dyna_size, &be_blc_cfg->bnr_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->raw_blc, blc_dyna_size, &be_blc_cfg->raw_blc, blc_dyna_size);
}

static td_void isp_drv_get_pre_be_blc_pre_online_post_offline(isp_drv_ctx *drv_ctx,
    isp_sync_cfg_buf_node *pre_be_cfg_node)
{
    td_bool cfg_blc_en;
    td_u16  blc_dyna_size = sizeof(isp_blc_dyna_cfg);
    isp_be_blc_dyna_cfg *be_blc_sync = TD_NULL;
    isp_be_blc_dyna_cfg *be_blc_cfg = TD_NULL;

    cfg_blc_en = isp_drv_calc_cfg_pre_be_blc_en(drv_ctx);
    if (cfg_blc_en == TD_FALSE) {
        return;
    }
    be_blc_sync = &drv_ctx->be_sync_para.be_blc;
    be_blc_cfg  = &pre_be_cfg_node->be_blc_reg_cfg;

    (td_void)memcpy_s(&be_blc_sync->rgbir_blc, blc_dyna_size, &be_blc_cfg->rgbir_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->ge_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &be_blc_cfg->ge_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->wdr_dg_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &be_blc_cfg->wdr_dg_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->wdr_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &be_blc_cfg->wdr_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->flicker_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &be_blc_cfg->flicker_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->expander_blc, blc_dyna_size, &be_blc_cfg->expander_blc, blc_dyna_size);
    (td_void)memcpy_s(&drv_ctx->be_sync_para.fpn_cfg, sizeof(isp_fpn_sync_cfg),
                      &pre_be_cfg_node->fpn_cfg, sizeof(isp_fpn_sync_cfg));
}

static td_void isp_drv_get_be_blc_pre_online_post_offline(isp_drv_ctx *drv_ctx, const isp_sync_cfg_buf_node *cfg_node,
    isp_sync_cfg_buf_node *pre_be_cfg_node)
{
    isp_drv_get_post_be_blc_pre_online_post_offline(drv_ctx, cfg_node);
    isp_drv_get_pre_be_blc_pre_online_post_offline(drv_ctx, pre_be_cfg_node);
}

td_s32 isp_drv_reg_config_be_blc(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_sync_cfg_buf_node *cfg_node,
    isp_sync_cfg_buf_node *pre_be_cfg_node)
{
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);
    isp_check_pointer_return(cfg_node);

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        isp_drv_get_be_blc_online(drv_ctx, cfg_node);
    } else if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        isp_drv_get_be_blc_pre_online_post_offline(drv_ctx, cfg_node, pre_be_cfg_node);
    } else {
        isp_drv_offline_be_blc_reg(drv_ctx, cfg_node);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_set_fe_blc_reg(ot_vi_pipe vi_pipe_bind, td_u8 k, isp_fe_reg_type *fe_reg,
    const isp_fe_blc_dyna_cfg *fe_blc_reg_cfg)
{
    td_u8  af_en[OT_ISP_MAX_PHY_PIPE_NUM] = { ISP_PIPE_FEAF_MODULE_ENABLE };
    isp_check_pointer_void_return(fe_reg);
    isp_check_pointer_void_return(fe_blc_reg_cfg);

    if (fe_blc_reg_cfg->resh_dyna == TD_TRUE) {
        isp_drv_fe_dg_offset_write(fe_reg, fe_blc_reg_cfg->fe_dg_blc[k].blc);
        isp_drv_fe_wb_offset_write(fe_reg, fe_blc_reg_cfg->fe_wb_blc[k].blc);
        isp_drv_fe_ae_offset_write(fe_reg, fe_blc_reg_cfg->fe_ae_blc[k].blc);
        isp_drv_fe_blc_offset_write(fe_reg, fe_blc_reg_cfg->fe_blc[k].blc);
        if (af_en[vi_pipe_bind] == 1) {
            isp_drv_fe_af_offset_write(fe_reg, fe_blc_reg_cfg->fe_af_blc[k].blc);
        }
    }
}

static td_s32 isp_drv_reg_config_fe_blc_all_wdr_pipe(td_u8 chn_num_max, const isp_drv_ctx *drv_ctx,
    const isp_fe_blc_dyna_cfg  *fe_blc_reg_cfg)
{
    td_u8 k;
    ot_vi_pipe vi_pipe_bind;
    isp_fe_reg_type *fe_reg = TD_NULL;

    for (k = 0; k < chn_num_max; k++) {
        vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];
        isp_check_vir_pipe_return(vi_pipe_bind);
        isp_drv_fereg_ctx(vi_pipe_bind, fe_reg);
        isp_drv_set_fe_blc_reg(vi_pipe_bind, k, fe_reg, fe_blc_reg_cfg);
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_reg_config_fe_blc_stitch(ot_vi_pipe vi_pipe, const isp_drv_ctx *drv_ctx, td_u8 fe_cfg_node_idx,
    td_u8 chn_num_max)
{
    td_u8 i;
    td_u8 stitch_idx = 0;
    isp_drv_ctx *drv_ctx_main_pipe = TD_NULL;
    isp_sync_cfg_buf_node *cfg_node_main_pipe = TD_NULL;

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
            stitch_idx = i;
            break;
        }
    }

    drv_ctx_main_pipe  = isp_drv_get_ctx(drv_ctx->stitch_attr.stitch_bind_id[0]);
    cfg_node_main_pipe = drv_ctx_main_pipe->sync_cfg.node[fe_cfg_node_idx];
    if (cfg_node_main_pipe == TD_NULL) {
        return TD_SUCCESS;
    }

    return isp_drv_reg_config_fe_blc_all_wdr_pipe(chn_num_max, drv_ctx,
                                                  &cfg_node_main_pipe->fe_blc_reg_cfg_stitch[stitch_idx]);
}

td_s32 isp_drv_reg_config_fe_blc(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_bool cfg_blc_en;
    td_u8 fe_cfg_node_idx, cfg_node_vc, chn_num_max;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_vir_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);

    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        return TD_SUCCESS;
    }

    fe_cfg_node_idx = isp_drv_get_fe_sync_index(drv_ctx);
    fe_cfg_node_idx = MIN2(fe_cfg_node_idx, CFG2VLD_DLY_LIMIT - 1);

    if (is_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        cfg_node_vc  = (drv_ctx->sync_cfg.cfg2_vld_dly_max - 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
    } else {
        cfg_node_vc  = 0;
    }

    chn_num_max = clip3(drv_ctx->wdr_attr.pipe_num, 1, ISP_WDR_CHN_MAX);
    cfg_node    = drv_ctx->sync_cfg.node[fe_cfg_node_idx];
    if (cfg_node == TD_NULL) {
        return TD_SUCCESS;
    }
    if (drv_ctx->sync_cfg.vc_cfg_num != cfg_node_vc) {
        return TD_SUCCESS;
    }

    cfg_blc_en = isp_drv_calc_cfg_fe_blc_en(vi_pipe, drv_ctx, fe_cfg_node_idx);
    if (cfg_blc_en != TD_TRUE) {
        return TD_SUCCESS;
    }
    drv_ctx->dyna_blc_info.actual_info.is_ready = TD_FALSE;
    if (drv_ctx->stitch_attr.stitch_enable == TD_FALSE) {
        return isp_drv_reg_config_fe_blc_all_wdr_pipe(chn_num_max, drv_ctx, &cfg_node->fe_blc_reg_cfg);
    }

    return isp_drv_reg_config_fe_blc_stitch(vi_pipe, drv_ctx, fe_cfg_node_idx, chn_num_max);
}

td_s32 isp_drv_reg_config_fe_dgain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 k, fe_cfg_node_idx, cfg_node_vc, chn_num_max;
    ot_vi_pipe vi_pipe_bind;
    td_u32 wdr_gain;
    isp_fe_reg_type     *fe_reg   = TD_NULL;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;

    isp_check_vir_pipe_return(vi_pipe);
    isp_check_pointer_return(drv_ctx);

    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        return TD_SUCCESS;
    }

    fe_cfg_node_idx = isp_drv_get_fe_sync_index(drv_ctx);
    fe_cfg_node_idx = MIN2(fe_cfg_node_idx, CFG2VLD_DLY_LIMIT - 1);

    if (is_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        cfg_node_vc  = (drv_ctx->sync_cfg.cfg2_vld_dly_max - 1) % div_0_to_1(drv_ctx->sync_cfg.vc_num_max + 1);
    } else {
        cfg_node_vc  = 0;
    }

    chn_num_max = clip3(drv_ctx->wdr_attr.pipe_num, 1, ISP_WDR_CHN_MAX);
    cfg_node    = drv_ctx->sync_cfg.node[fe_cfg_node_idx];

    if (cfg_node != TD_NULL) {
        if (drv_ctx->sync_cfg.vc_cfg_num == cfg_node_vc) {
            for (k = 0; k < chn_num_max; k++) {
                vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];
                isp_check_vir_pipe_return(vi_pipe_bind);
                isp_drv_fereg_ctx(vi_pipe_bind, fe_reg);

                wdr_gain = drv_ctx->sync_cfg.wdr_gain[k][fe_cfg_node_idx];
                wdr_gain = (cfg_node->ae_reg_cfg.isp_dgain * wdr_gain) >> 0x8;
                wdr_gain = clip3(wdr_gain, ISP_DIGITAL_GAIN_MIN, ISP_DIGITAL_GAIN_MAX);
                isp_drv_set_fe_dgain(fe_reg, wdr_gain);
            }
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_drv_reg_config_pre_be_regup(ot_vi_pipe vi_pipe)
{
    td_u8 i, blk_dev;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_viproc_reg_type  *pre_viproc[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = isp_drv_get_pre_viproc_reg_virt_addr(vi_pipe, pre_viproc);
    isp_check_return(vi_pipe, ret, "isp_drv_get_pre_viproc_reg_virt_addr");

    blk_dev = drv_ctx->work_mode.block_dev;
    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        isp_drv_set_be_regup(pre_viproc[i + blk_dev], TD_TRUE);
    }
    return TD_SUCCESS;
}

static td_s32 isp_drv_reg_config_post_be_regup(ot_vi_pipe vi_pipe)
{
    td_u8 i, blk_dev;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_pipe_return(vi_pipe);
    drv_ctx = isp_drv_get_ctx(vi_pipe);

    ret = isp_drv_get_post_viproc_reg_virt_addr(vi_pipe, post_viproc);
    isp_check_return(vi_pipe, ret, "isp_drv_get_post_viproc_reg_virt_addr");

    blk_dev = drv_ctx->work_mode.block_dev;
    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        isp_drv_set_be_regup(post_viproc[i + blk_dev], TD_TRUE);
    }
    return TD_SUCCESS;
}

td_s32 isp_drv_reg_config_be_regup(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    ret = isp_drv_reg_config_pre_be_regup(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    ret = isp_drv_reg_config_post_be_regup(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    return TD_SUCCESS;
}

static td_void isp_drv_set_wdr_sync_cfg(isp_pre_be_reg_type *pre_be, const isp_be_sync_para *be_sync_para)
{
    isp_drv_set_wdr_exporratio0(pre_be, be_sync_para->wdr_exp_ratio[0]);
    isp_drv_set_wdr_exporratio1(pre_be, be_sync_para->wdr_exp_ratio[1]);
    isp_drv_set_wdr_exporratio2(pre_be, be_sync_para->wdr_exp_ratio[2]);  /* 2 */
    isp_drv_set_flick_exporatio0(pre_be, be_sync_para->flick_exp_ratio[0]);
    isp_drv_set_flick_exporatio1(pre_be, be_sync_para->flick_exp_ratio[1]);
    isp_drv_set_flick_exporatio2(pre_be, be_sync_para->flick_exp_ratio[2]); /* 2 */
    isp_drv_set_wdr_expo_value0(pre_be, be_sync_para->wdr_exp_val[0]); /* index 0 */
    isp_drv_set_wdr_expo_value1(pre_be, be_sync_para->wdr_exp_val[1]); /* index 1 */
    isp_drv_set_wdr_expo_value2(pre_be, be_sync_para->wdr_exp_val[2]); /* index 2 */
    isp_drv_set_wdr_expo_value3(pre_be, be_sync_para->wdr_exp_val[3]); /* index 3 */
    isp_drv_set_fusion_expo_value0(pre_be, be_sync_para->fusion_exp_val[0]); /* index 0 */
    isp_drv_set_fusion_expo_value1(pre_be, be_sync_para->fusion_exp_val[1]); /* index 1 */
    isp_drv_set_fusion_expo_value2(pre_be, be_sync_para->fusion_exp_val[2]); /* index 2 */
    isp_drv_set_fusion_expo_value3(pre_be, be_sync_para->fusion_exp_val[3]); /* index 3 */
    isp_drv_set_wdr_blc_comp0(pre_be, be_sync_para->wdr_blc_comp[0]);
    isp_drv_set_wdr_blc_comp1(pre_be, be_sync_para->wdr_blc_comp[1]);
    isp_drv_set_wdr_blc_comp2(pre_be, be_sync_para->wdr_blc_comp[2]); /* 2 */
    isp_drv_set_wdr_mdt_en(pre_be, be_sync_para->wdr_mdt_en);
    isp_drv_set_wdr_fusion_mode(pre_be, be_sync_para->fusion_mode);
    isp_drv_set_wdr_short_thr0(pre_be, be_sync_para->short_thr[0]);
    isp_drv_set_wdr_short_thr1(pre_be, be_sync_para->short_thr[1]);
    isp_drv_set_wdr_short_thr2(pre_be, be_sync_para->short_thr[2]); /* 2 */
    isp_drv_set_wdr_long_thr0(pre_be, be_sync_para->long_thr[0]);
    isp_drv_set_wdr_long_thr1(pre_be, be_sync_para->long_thr[1]);
    isp_drv_set_wdr_long_thr2(pre_be, be_sync_para->long_thr[2]); /* 2 */
    isp_drv_set_wdr_max_ratio(pre_be, be_sync_para->wdr_max_ratio);
    isp_drv_set_fusion_max_ratio(pre_be, be_sync_para->fusion_max_ratio);
}

static td_void isp_drv_reg_config_wdr_saturate_thr(td_u16 blc_value, isp_be_sync_para *be_sync_para)
{
    td_u32 saturate_low, saturate_hig;
    td_s32 m_max_value_in = isp_bitmask(WDR_BITDEPTH);

    saturate_hig = ((td_u32)(m_max_value_in - (blc_value)));
    saturate_low = sqrt32(saturate_hig);
    be_sync_para->saturate_thr        = (td_u16)(saturate_hig - saturate_low);
    be_sync_para->fusion_saturate_thr = be_sync_para->saturate_thr;
}

static td_void isp_drv_wdr_saturate_thr_update(isp_drv_ctx *drv_ctx)
{
    td_u16 blc_value = drv_ctx->be_sync_para.be_blc.wdr_blc[0].blc[0];
    if (is_2to1_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        blc_value = drv_ctx->be_sync_para.be_blc.wdr_blc[1].blc[0]; /* long frame index:1 */
    } else if (is_3to1_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        blc_value = drv_ctx->be_sync_para.be_blc.wdr_blc[2].blc[0]; /* long frame index:2 */
    }

    isp_drv_reg_config_wdr_saturate_thr(blc_value, &drv_ctx->be_sync_para);
}

static td_void isp_drv_reg_config_wdr_2to1(isp_drv_ctx *drv_ctx, isp_fswdr_sync_cfg *wdr_reg_cfg, td_u32 *ratio)
{
    td_u8  i;
    td_u16  offset0 = drv_ctx->be_sync_para.be_blc.wdr_blc[0].blc[1];
    const td_u8  bit_shift = 0;
    td_u32 expo_value[OT_ISP_WDR_MAX_FRAME_NUM] = { 0 }; /* exposure max 4 */
    td_u32 blc_comp[OT_ISP_WDR_MAX_FRAME_NUM - 1] = { 0 };
    isp_be_sync_para *be_sync_para = &drv_ctx->be_sync_para;

    be_sync_para->wdr_exp_ratio[0] = MIN2((isp_bitmask(10) * EXP_RATIO_MIN / div_0_to_1(ratio[0])), 0x3FF); /* 10 */
    be_sync_para->wdr_exp_ratio[1] = 0;
    be_sync_para->wdr_exp_ratio[2] = 0; /* wdr_index 2 */

    be_sync_para->flick_exp_ratio[0] = MIN2(ratio[0], 0X3FFF);
    be_sync_para->flick_exp_ratio[1] = 0;
    be_sync_para->flick_exp_ratio[2] = 0; /* wdr_index 2 */

    if (wdr_reg_cfg->fusion_mode == 0) {
        expo_value[0] = MIN2(ratio[0], isp_bitmask(18)); /* const value 18 */
        expo_value[1] = MIN2(EXP_RATIO_MIN, isp_bitmask(18)); /* const value 18 */
    } else {
        expo_value[0] = MIN2((ratio[0] + EXP_RATIO_MIN), isp_bitmask(18)); /* const value 18 */
        expo_value[1] = MIN2(EXP_RATIO_MIN, isp_bitmask(18)); /* const value 18 */
    }

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        be_sync_para->wdr_exp_val[i]    = expo_value[i];
        be_sync_para->fusion_exp_val[i] = expo_value[i];
    }

    blc_comp[0] = ((expo_value[0] - expo_value[1]) * offset0) >> bit_shift;
    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM - 1; i++) {
        be_sync_para->wdr_blc_comp[i] = blc_comp[i];
    }

    isp_drv_reg_config_wdr_saturate_thr(drv_ctx->be_sync_para.be_blc.wdr_blc[1].blc[0], be_sync_para);
}

static td_void isp_drv_reg_config_wdr_3to1(isp_drv_ctx *drv_ctx, isp_fswdr_sync_cfg *wdr_reg_cfg, td_u32 *ratio)
{
    td_u8  i;
    td_u16  offset0 = drv_ctx->be_sync_para.be_blc.wdr_blc[0].blc[1];
    td_u16  offset1 = drv_ctx->be_sync_para.be_blc.wdr_blc[1].blc[1];
    const td_u8  bit_shift = 0;
    td_u32 expo_value[OT_ISP_WDR_MAX_FRAME_NUM] = { 0 }; /* exposure max 4 */
    td_u32 blc_comp[OT_ISP_WDR_MAX_FRAME_NUM - 1] = { 0 };
    isp_be_sync_para *be_sync_para = &drv_ctx->be_sync_para;
    be_sync_para->wdr_exp_ratio[0] = MIN2((isp_bitmask(10) * EXP_RATIO_MIN / div_0_to_1(ratio[0])), 0x3FF); /* 10 */
    be_sync_para->wdr_exp_ratio[1] = MIN2((isp_bitmask(10) * EXP_RATIO_MIN / div_0_to_1(ratio[1])), 0x3FF);
    be_sync_para->wdr_exp_ratio[2] = 0; /* wdr_index 2 */

    be_sync_para->flick_exp_ratio[0] = MIN2(ratio[0], 0X3FFF);
    be_sync_para->flick_exp_ratio[1] = MIN2(ratio[1], 0X3FFF);
    be_sync_para->flick_exp_ratio[2] = 0; /* wdr_index 2 */

    if (wdr_reg_cfg->fusion_mode == 0) {
        expo_value[2] = MIN2(EXP_RATIO_MIN, isp_bitmask(18)); /* const value 18, index[2] */
        expo_value[1] = MIN2(ratio[1], isp_bitmask(18)); /* const value 18, index[1] */
        expo_value[0] = MIN2((ratio[1] * ratio[0]) >> 6, isp_bitmask(18)); /* shift 6,and const 18 */
    } else {
        expo_value[2] = MIN2(EXP_RATIO_MIN, isp_bitmask(18)); /* const value 18, index[2] */
        expo_value[1] = MIN2(((ratio[1] * ratio[0] / div_0_to_1(ratio[0] + EXP_RATIO_MIN)) + EXP_RATIO_MIN),
            isp_bitmask(18)); /* const value 18 */
        expo_value[0] = MIN2((EXP_RATIO_MIN + ((ratio[1] * ratio[0]) >> 6) + ratio[0]), isp_bitmask(18)); /* 6, 18 */
    }

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        be_sync_para->wdr_exp_val[i]    = expo_value[i];
        be_sync_para->fusion_exp_val[i] = expo_value[i];
    }

    blc_comp[0] = ((expo_value[0] - expo_value[1]) * offset0) >> bit_shift; /* array index 1 */
    blc_comp[1] = ((expo_value[1] - expo_value[2]) * offset1) >> bit_shift; /* array index 2 */
    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM - 1; i++) {
        be_sync_para->wdr_blc_comp[i] = blc_comp[i];
    }

    isp_drv_reg_config_wdr_saturate_thr(drv_ctx->be_sync_para.be_blc.wdr_blc[2].blc[0], be_sync_para); /* long idx:2 */
}

static td_void isp_drv_reg_config_wdr_thr(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_fswdr_sync_cfg *wdr_reg_cfg)
{
    td_bool wdr_mdt_en;
    td_u8   cfg_dly_max;
    td_u8   lf_mode, i;
    td_u16  long_thr[OT_ISP_WDR_MAX_FRAME_NUM - 1] = {0x3FFF, 0x3FFF, 0x3FFF};
    td_u16  short_thr[OT_ISP_WDR_MAX_FRAME_NUM - 1] = {0x3FFF, 0x3FFF, 0x3FFF};

    cfg_dly_max = isp_drv_get_pre_be_sync_index(vi_pipe, drv_ctx);
    cfg_dly_max = MIN2(cfg_dly_max, CFG2VLD_DLY_LIMIT - 1);
    lf_mode = drv_ctx->sync_cfg.lf_mode[cfg_dly_max];

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM - 1; i++) {
        long_thr[i]  = wdr_reg_cfg->long_thr[i];
        short_thr[i] = wdr_reg_cfg->short_thr[i];
    }

    wdr_mdt_en = wdr_reg_cfg->wdr_mdt_en;
    if ((lf_mode != 0) && (drv_ctx->be_sync_para.wdr_exp_val[0] < 0x44)) {
        long_thr[0]  = 0x3FFF;
        short_thr[0] = 0x3FFF;
        long_thr[1]  = 0x3FFF;
        short_thr[1] = 0x3FFF;
        long_thr[2]  = 0x3FFF; /* 2 */
        short_thr[2] = 0x3FFF; /* 2 */
        wdr_mdt_en   = 0;
    }

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM - 1; i++) {
        drv_ctx->be_sync_para.long_thr[i]  = long_thr[i];
        drv_ctx->be_sync_para.short_thr[i] = short_thr[i];
    }
    drv_ctx->be_sync_para.wdr_mdt_en  = wdr_mdt_en;
}

td_s32 isp_drv_reg_config_wdr(ot_vi_pipe vi_pipe, isp_fswdr_sync_cfg *wdr_reg_cfg, td_u32 *ratio)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_sync_para *be_sync_para = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(wdr_reg_cfg);
    isp_check_pointer_return(ratio);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (is_linear_mode(drv_ctx->sync_cfg.wdr_mode) || is_built_in_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        return TD_SUCCESS;
    }

    if (is_2to1_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_drv_reg_config_wdr_2to1(drv_ctx, wdr_reg_cfg, ratio);
    } else if (is_3to1_wdr_mode(drv_ctx->sync_cfg.wdr_mode)) {
        isp_drv_reg_config_wdr_3to1(drv_ctx, wdr_reg_cfg, ratio);
    }
    be_sync_para = &drv_ctx->be_sync_para;
    be_sync_para->wdr_max_ratio = ((1 << 26) - 1) / div_0_to_1(be_sync_para->wdr_exp_val[0]); /* 26 */
    be_sync_para->fusion_max_ratio = be_sync_para->wdr_max_ratio;
    be_sync_para->fusion_mode      = wdr_reg_cfg->fusion_mode;
    isp_drv_reg_config_wdr_thr(vi_pipe, drv_ctx, wdr_reg_cfg);

    return TD_SUCCESS;
}

static td_void isp_drv_reg_config_pq_ai_ldci(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 ldci_comp)
{
    isp_drv_ctx *ai_drv_ctx = TD_NULL;

    if (vi_pipe == drv_ctx->ai_info.base_pipe_id) {
        if (drv_ctx->ai_info.ai_pipe_id < 0 || drv_ctx->ai_info.ai_pipe_id >= OT_ISP_MAX_PIPE_NUM) {
            isp_err_trace("Err ai pipe %d!\n", drv_ctx->ai_info.ai_pipe_id);
            return;
        }

        ai_drv_ctx = isp_drv_get_ctx(drv_ctx->ai_info.ai_pipe_id);
        ai_drv_ctx->be_sync_para.ldci_comp = ldci_comp;
    }
}

td_s32 isp_drv_reg_config_ldci(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u32  ldci_comp;
    td_u32  ldci_comp_index;

    isp_check_pointer_return(drv_ctx);
    if ((drv_ctx->ai_info.pq_ai_en == TD_TRUE) && (vi_pipe == drv_ctx->ai_info.ai_pipe_id)) {
        return TD_SUCCESS;
    }

    ldci_comp_index = isp_drv_get_be_sync_index(vi_pipe, drv_ctx);
    if (ldci_comp_index >= 1) {
        ldci_comp_index = ldci_comp_index - 1; /* ldci compensate is earlier tham drc one frame */
    } else {
        ldci_comp_index = 0;
    }
    ldci_comp_index = MIN2(ldci_comp_index, CFG2VLD_DLY_LIMIT - 1);

    ldci_comp = drv_ctx->sync_cfg.drc_comp[ldci_comp_index];
    ldci_comp = sqrt32(ldci_comp << DRC_COMP_SHIFT);
    ldci_comp = MIN2(ldci_comp, 0xFFFF);

    drv_ctx->be_sync_para.ldci_comp = ldci_comp;
    if (drv_ctx->ai_info.pq_ai_en == TD_TRUE) {
        isp_drv_reg_config_pq_ai_ldci(vi_pipe, drv_ctx, ldci_comp);
    }

    return TD_SUCCESS;
}

static td_bool isp_drv_get_bnr_is_initial(const isp_drv_ctx *drv_ctx)
{
    td_bool is_init = TD_FALSE;
    td_bool pre_tnr_en, cur_tnr_en;

    cur_tnr_en = drv_ctx->bnr_tpr_filt.cur.tnr_en && drv_ctx->bnr_tpr_filt.cur.nr_en;
    pre_tnr_en = drv_ctx->bnr_tpr_filt.pre.tnr_en && drv_ctx->bnr_tpr_filt.pre.nr_en;
    if ((cur_tnr_en == TD_TRUE) && (pre_tnr_en == TD_FALSE)) {
        is_init = TD_TRUE;
    }

    return is_init;
}
static td_void isp_drv_sharpen_mot_en_write(isp_post_be_reg_type *be_reg, td_bool mot_en)
{
    u_isp_sharpen_mot0 o_isp_sharpen_mot0;

    o_isp_sharpen_mot0.u32 = be_reg->isp_sharpen_mot0.u32;
    o_isp_sharpen_mot0.bits.isp_sharpen_mot_en = mot_en;
    be_reg->isp_sharpen_mot0.u32 = o_isp_sharpen_mot0.u32;
}

static td_void isp_drv_bnr_entmpnr_write(isp_post_be_reg_type *be_reg, td_bool tnr_en, td_bool is_init)
{
    u_isp_bnr_cfg isp_bnr_cfg;
    isp_bnr_cfg.u32 = be_reg->isp_bnr_cfg.u32;
    isp_bnr_cfg.bits.isp_bnr_entmpnr   = tnr_en;
    isp_bnr_cfg.bits.isp_bnr_isinitial = is_init;
    be_reg->isp_bnr_cfg.u32 = isp_bnr_cfg.u32;
}

static td_void isp_drv_reg_config_bnr_online(isp_post_be_reg_type *be_reg[], isp_viproc_reg_type *viproc_reg[],
    isp_drv_ctx *drv_ctx)
{
    td_bool is_init, mot_en, tnr_en;
    td_u8 k, blk_num, blk_dev;
    isp_kernel_tpr_filt_reg  *bnr_tpr_filt = TD_NULL;

    if (is_offline_mode(drv_ctx->work_mode.running_mode) ||
        is_striping_mode(drv_ctx->work_mode.running_mode) ||
        is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        return;
    }

    blk_num = drv_ctx->work_mode.block_num;
    blk_dev = drv_ctx->work_mode.block_dev;

    bnr_tpr_filt = &drv_ctx->bnr_tpr_filt;

    is_init = isp_drv_get_bnr_is_initial(drv_ctx);
    mot_en = bnr_tpr_filt->cur.tnr_en && bnr_tpr_filt->cur.nr_en && bnr_tpr_filt->sharpen_mot_en;
    tnr_en = bnr_tpr_filt->cur.tnr_en && bnr_tpr_filt->cur.nr_en;
    for (k = 0; k < blk_num; k++) {
        isp_drv_bnr_entmpnr_write(be_reg[k + blk_dev], tnr_en, is_init);
        isp_drv_sharpen_mot_en_write(be_reg[k + blk_dev], mot_en);

        /* bnr enable */
        viproc_reg[k + blk_dev]->viproc_ispbe_ctrl0.bits.isp_bnr_en = bnr_tpr_filt->cur.nr_en;
    }

    (td_void)memcpy_s(&bnr_tpr_filt->pre, sizeof(isp_bnr_temporal_filt),
                      &bnr_tpr_filt->cur, sizeof(isp_bnr_temporal_filt));
}

td_void isp_drv_reg_config_bnr_offline(isp_be_wo_reg_cfg *be_cfg, isp_drv_ctx *drv_ctx)
{
    td_bool is_init, mot_en, tnr_en;
    td_u8 k;

    is_init = isp_drv_get_bnr_is_initial(drv_ctx);

    mot_en = drv_ctx->bnr_tpr_filt.cur.tnr_en && drv_ctx->bnr_tpr_filt.cur.nr_en &&
             drv_ctx->bnr_tpr_filt.sharpen_mot_en;
    tnr_en = drv_ctx->bnr_tpr_filt.cur.tnr_en && drv_ctx->bnr_tpr_filt.cur.nr_en;

    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_drv_bnr_entmpnr_write(&be_cfg->be_reg_cfg[k].post_be, tnr_en, is_init);
        isp_drv_sharpen_mot_en_write(&be_cfg->be_reg_cfg[k].post_be, mot_en);
        /* bnr enable */
        be_cfg->be_reg_cfg[k].post_viproc.viproc_ispbe_ctrl0.bits.isp_bnr_en = drv_ctx->bnr_tpr_filt.cur.nr_en;
    }

    (td_void)memcpy_s(&drv_ctx->bnr_tpr_filt.pre, sizeof(isp_bnr_temporal_filt),
                      &drv_ctx->bnr_tpr_filt.cur, sizeof(isp_bnr_temporal_filt));
}

static td_void isp_drv_calc_drc_exp_ratio(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 *exp_ratio)
{
    td_u8 cfg_dly_max;
    td_u32 drc_exp_ratio;
    isp_drv_ctx *base_drv_ctx = TD_NULL;

    cfg_dly_max = isp_drv_get_be_sync_index(vi_pipe, drv_ctx);
    cfg_dly_max = MIN2(cfg_dly_max, CFG2VLD_DLY_LIMIT - 1);

    if ((drv_ctx->ai_info.pq_ai_en == TD_TRUE) && (vi_pipe == drv_ctx->ai_info.ai_pipe_id)) {
        if (drv_ctx->ai_info.base_pipe_id < 0 || drv_ctx->ai_info.base_pipe_id >= OT_ISP_MAX_PIPE_NUM) {
            isp_err_trace("Err base pipe %d!\n", drv_ctx->ai_info.base_pipe_id);
            return;
        }
        base_drv_ctx = isp_drv_get_ctx(drv_ctx->ai_info.base_pipe_id);
        drc_exp_ratio = base_drv_ctx->sync_cfg.drc_comp[cfg_dly_max];
    } else {
        drc_exp_ratio = drv_ctx->sync_cfg.drc_comp[cfg_dly_max];
    }

    if (drc_exp_ratio != 0x1000) { /* do division only when drc_exp_ratio != 4096 */
        drc_exp_ratio = div_0_to_1(drc_exp_ratio);
        drc_exp_ratio = osal_div_u64((1 << (DRC_COMP_SHIFT + DRC_COMP_SHIFT)), drc_exp_ratio);
        drc_exp_ratio = MIN2(drc_exp_ratio, (15 << DRC_COMP_SHIFT)); /* Maximum supported ratio is 15 */
    }
    *exp_ratio = drc_exp_ratio;
}

td_void isp_drv_calc_drc_prev_luma(isp_drc_sync_cfg *drc_reg_cfg, td_bool update_log_param,
                                   td_u8 drc_shp_log, td_u32 drc_exp_ratio, td_u32 *drc_prev_luma)
{
    td_u32 i;

    /* Compensate on PrevLuma when ShpLog/ShpExp is modified, but no compensation under offline repeat mode */
    if (update_log_param && (!drc_reg_cfg->is_offline_repeat_mode)) {
        for (i = 0; i < OT_ISP_DRC_EXP_COMP_SAMPLE_NUM - 1; i++) {
            drc_prev_luma[i] =
                (td_u32)((td_s32)g_drc_cur_luma_lut[drc_shp_log][i] + drc_reg_cfg->prev_luma_delta[i]);
        }
    } else {
        for (i = 0; i < OT_ISP_DRC_EXP_COMP_SAMPLE_NUM - 1; i++) {
            drc_prev_luma[i] = g_drc_cur_luma_lut[drc_shp_log][i];
        }
    }
    drc_prev_luma[OT_ISP_DRC_EXP_COMP_SAMPLE_NUM - 1] = (1 << 20); /* left shift 20 */

    if ((drc_exp_ratio != 0x1000) && (!drc_reg_cfg->is_offline_repeat_mode)) {
        for (i = 0; i < OT_ISP_DRC_EXP_COMP_SAMPLE_NUM; i++) {
            drc_prev_luma[i] = (td_u32)(((td_u64)drc_exp_ratio * drc_prev_luma[i]) >> DRC_COMP_SHIFT);
        }
    }
}

td_s32 isp_drv_reg_config_drc(ot_vi_pipe vi_pipe, isp_drc_sync_cfg *drc_reg_cfg)
{
    td_u8  i;
    td_u8  blk_dev;
    td_u32 drc_div_denom_log, drc_denom_exp;
    td_u32 drc_exp_ratio = 0x1000;
    td_u32 drc_prev_luma[OT_ISP_DRC_EXP_COMP_SAMPLE_NUM] = {0};
    td_bool update_log_param;
    isp_drv_ctx *drv_ctx = TD_NULL;

    isp_check_pointer_return(drc_reg_cfg);

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    blk_dev = drv_ctx->work_mode.block_dev;

    isp_drv_calc_drc_exp_ratio(vi_pipe, drv_ctx, &drc_exp_ratio);

    update_log_param = (g_drc_shp_log[vi_pipe][blk_dev] != drc_reg_cfg->shp_log ||
                        g_drc_shp_exp[vi_pipe][blk_dev] != drc_reg_cfg->shp_exp) ? TD_TRUE : TD_FALSE;

    g_drc_shp_log[vi_pipe][blk_dev] = drc_reg_cfg->shp_log;
    g_drc_shp_exp[vi_pipe][blk_dev] = drc_reg_cfg->shp_exp;

    isp_drv_calc_drc_prev_luma(drc_reg_cfg, update_log_param,
                               g_drc_shp_log[vi_pipe][blk_dev], drc_exp_ratio, drc_prev_luma);

    drc_div_denom_log = g_drc_div_denom_log[g_drc_shp_log[vi_pipe][blk_dev]];
    drc_denom_exp    = g_drc_denom_exp[g_drc_shp_exp[vi_pipe][blk_dev]];

    for (i = 0; i < OT_ISP_DRC_EXP_COMP_SAMPLE_NUM; i++) {
        drv_ctx->be_sync_para.drc_prev_luma[i] = drc_prev_luma[i];
    }
    drv_ctx->be_sync_para.drc_shp_log       = g_drc_shp_log[vi_pipe][blk_dev];
    drv_ctx->be_sync_para.drc_div_denom_log = drc_div_denom_log;
    drv_ctx->be_sync_para.drc_denom_exp     = drc_denom_exp;

    return TD_SUCCESS;
}

td_s32 isp_drv_reg_config_dgain(ot_vi_pipe vi_pipe, td_u32 isp_dgain)
{
    td_u8 k, blk_dev;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_post_be_reg_type *be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    ret = isp_drv_get_post_be_reg_virt_addr(vi_pipe, be_reg);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    blk_dev = drv_ctx->work_mode.block_dev;
    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_check_pointer_return(be_reg[k + blk_dev]);
        isp_drv_set_be_dgain(be_reg[k + blk_dev], isp_dgain);
    }

    return TD_SUCCESS;
}

td_void isp_drv_reg_config_sync_ccm(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u16 *color_matrix)
{
    td_s32 i;
    td_u8  stitch_idx = 0;
    ot_vi_pipe stitch_main_pipe;
    isp_drv_ctx *drv_ctx_main_pipe = TD_NULL;
    td_u16  ccm[OT_ISP_CCM_MATRIX_SIZE] = { 0 };

    for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
        ccm[i] = color_matrix[i];
    }

    for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
        drv_ctx->be_sync_para.ccm[i] = ccm[i];
    }
    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        stitch_main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx_main_pipe = isp_drv_get_ctx(stitch_main_pipe);

        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
                stitch_idx = i;
                break;
            }
        }

        for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
            drv_ctx_main_pipe->be_sync_para_stitch[stitch_idx].ccm[i] = ccm[i];
        }
    }
}

static td_void isp_drv_reg_config_pq_ai_wb_gain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, td_u32 *wb_dgain)
{
    td_u8 i;
    isp_drv_ctx *ai_drv_ctx = TD_NULL;
    if ((drv_ctx->ai_info.pq_ai_en == TD_TRUE) && (vi_pipe == drv_ctx->ai_info.ai_pipe_id)) {
        return;
    }

    if (vi_pipe == drv_ctx->ai_info.base_pipe_id) {
        if (drv_ctx->ai_info.ai_pipe_id < 0 || drv_ctx->ai_info.ai_pipe_id >= OT_ISP_MAX_PIPE_NUM) {
            isp_err_trace("Err ai pipe %d!\n", drv_ctx->ai_info.ai_pipe_id);
            return;
        }

        ai_drv_ctx = isp_drv_get_ctx(drv_ctx->ai_info.ai_pipe_id);
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            ai_drv_ctx->be_sync_para.wb_gain[i] = wb_dgain[i];
        }
    }
}

static td_s32 isp_drv_reg_config_awb_gain(ot_vi_pipe vi_pipe,
    isp_drv_ctx *drv_ctx, const isp_awb_reg_cfg_2 *awb_reg_cfg)
{
    td_u8  i;
    td_u8  stitch_idx = 0;
    ot_vi_pipe stitch_main_pipe;
    td_u32 wb_gain[OT_ISP_BAYER_CHN_NUM] = {0};
    isp_drv_ctx *drv_ctx_main_pipe = TD_NULL;

    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        wb_gain[i] = awb_reg_cfg->be_white_balance_gain[i];
    }

    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        drv_ctx->be_sync_para.wb_gain[i] = wb_gain[i];
    }
    isp_drv_reg_config_pq_ai_wb_gain(vi_pipe, drv_ctx, wb_gain);

    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        stitch_main_pipe = drv_ctx->stitch_attr.stitch_bind_id[0];
        drv_ctx_main_pipe = isp_drv_get_ctx(stitch_main_pipe);

        for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
            if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
                stitch_idx = i;
                break;
            }
        }

        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            drv_ctx_main_pipe->be_sync_para_stitch[stitch_idx].wb_gain[i] = wb_gain[i];
        }
    }

    return TD_SUCCESS;
}

static td_void isp_drv_reg_config_sync_awb_ccm_stitch(const isp_drv_ctx *drv_ctx, td_u8 cfg_node_idx, td_u8 cfg_node_vc)
{
    td_u8 i, j, pipe_id;
    ot_vi_pipe stitch_pipe;
    isp_drv_ctx *stitch_drv_ctx = TD_NULL;
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    isp_sync_cfg_buf_node *ccm_cfg_node = TD_NULL;

    if (drv_ctx->stitch_attr.main_pipe != TD_TRUE) {
        return;
    }

    cfg_node     = drv_ctx->sync_cfg.node[cfg_node_idx];
    ccm_cfg_node = drv_ctx->sync_cfg.node[0];
    isp_check_pointer_void_return(cfg_node);

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        stitch_pipe = drv_ctx->stitch_attr.stitch_bind_id[i];
        stitch_drv_ctx = isp_drv_get_ctx(stitch_pipe);

        isp_drv_reg_config_sync_ccm(stitch_pipe, stitch_drv_ctx, ccm_cfg_node->awb_reg_cfg_stitch[i].color_matrix);
        isp_drv_reg_config_awb_gain(stitch_pipe, stitch_drv_ctx, &cfg_node->awb_reg_cfg_stitch[i]);
    }

    for (i = 1; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        stitch_pipe = drv_ctx->stitch_attr.stitch_bind_id[i];
        stitch_drv_ctx = isp_drv_get_ctx(stitch_pipe);

        for (pipe_id = 0; pipe_id < OT_ISP_MAX_PIPE_NUM; pipe_id++) {
            for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
                stitch_drv_ctx->be_sync_para_stitch[pipe_id].wb_gain[j] =
                    drv_ctx->be_sync_para_stitch[pipe_id].wb_gain[j];
            }

            for (j = 0; j < OT_ISP_CCM_MATRIX_SIZE; j++) {
                stitch_drv_ctx->be_sync_para_stitch[pipe_id].ccm[j] = drv_ctx->be_sync_para_stitch[pipe_id].ccm[j];
            }
        }
    }
}

static td_void isp_drv_reg_config_sync_awb_ccm_normal(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    td_u8 cfg_node_idx, td_u8 cfg_node_vc)
{
    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    isp_sync_cfg_buf_node *ccm_cfg_node = TD_NULL;
    cfg_node     = drv_ctx->sync_cfg.node[cfg_node_idx];
    ccm_cfg_node = drv_ctx->sync_cfg.node[0];
    isp_check_pointer_void_return(cfg_node);
    if (drv_ctx->sync_cfg.vc_cfg_num == cfg_node_vc) {
        isp_drv_reg_config_sync_ccm(vi_pipe, drv_ctx, ccm_cfg_node->awb_reg_cfg.color_matrix);
        isp_drv_reg_config_awb_gain(vi_pipe, drv_ctx, &cfg_node->awb_reg_cfg);
    }

    return;
}

td_void isp_drv_reg_config_sync_awb_ccm(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    td_u8 cfg_node_idx, td_u8 cfg_node_vc)
{
    isp_check_pointer_void_return(drv_ctx);
    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        return isp_drv_reg_config_sync_awb_ccm_stitch(drv_ctx, cfg_node_idx, cfg_node_vc);
    }

    return isp_drv_reg_config_sync_awb_ccm_normal(vi_pipe, drv_ctx, cfg_node_idx, cfg_node_vc);
}


td_s32 isp_drv_reg_config_4dgain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_sync_4dgain_cfg *sync_4dgain_cfg)
{
    td_u32 *wdr_gain = TD_NULL;

    wdr_gain = sync_4dgain_cfg->wdr_gain;

    drv_ctx->be_sync_para.wdr_gain[0] = wdr_gain[0];
    drv_ctx->be_sync_para.wdr_gain[1] = wdr_gain[1];
    drv_ctx->be_sync_para.wdr_gain[2] = wdr_gain[2]; /* 2: config channel 2 dgain value */
    return TD_SUCCESS;
}

td_void isp_drv_reg_config_vi_fpn_offline(isp_be_wo_reg_cfg *be_cfg, const isp_drv_ctx *drv_ctx)
{
    td_bool fpn_cor_en;
    td_u8 k;
    isp_check_pointer_void_return(be_cfg);
    isp_check_pointer_void_return(drv_ctx);
    if (drv_ctx->fpn_work_mode != FPN_MODE_CORRECTION) {
        return;
    }
    fpn_cor_en = drv_ctx->be_sync_para.fpn_cfg.fpn_cor_en;

    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        be_cfg->be_reg_cfg[k].pre_viproc.viproc_ispbe_ctrl0.bits.isp_fpn_en   = fpn_cor_en;
        if (fpn_cor_en == TD_TRUE) {
            be_cfg->be_reg_cfg[k].pre_viproc.viproc_ispbe_ctrl0.bits.isp_fpn_mode = 0; /* 0: cor mode */
        }
    }
}

static td_s32 isp_drv_set_vi_fpn_correction_en_online(ot_vi_pipe vi_pipe, td_bool fpn_cor_en,
                                                      const isp_drv_ctx *drv_ctx)
{
    td_u8  k, blk_dev;
    td_s32 ret;
    isp_viproc_reg_type *pre_viproc[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    if (ckfn_vi_set_fpn_correct_en() != TD_NULL) {
        ret = call_vi_set_fpn_correct_en(vi_pipe, fpn_cor_en);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    } else {
        return TD_FAILURE;
    }

    ret = isp_drv_get_pre_viproc_reg_virt_addr(vi_pipe, pre_viproc);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    blk_dev = drv_ctx->work_mode.block_dev;
    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        pre_viproc[k + blk_dev]->viproc_ispbe_ctrl0.bits.isp_fpn_en = fpn_cor_en;
        if (fpn_cor_en == TD_TRUE) {
            pre_viproc[k + blk_dev]->viproc_ispbe_ctrl0.bits.isp_fpn_mode = 0; /* 0: cor mode */
        }
    }

    return TD_SUCCESS;
}

static td_void isp_drv_set_fpn_cfg(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    isp_pre_be_reg_type *pre_be, const isp_fpn_sync_cfg *fpn_cfg)
{
    td_s32 ret;

    if (drv_ctx->fpn_work_mode != FPN_MODE_CORRECTION) {
        return;
    }

    if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode) ||
        is_online_mode(drv_ctx->work_mode.running_mode)) {
        ret = isp_drv_set_vi_fpn_correction_en_online(vi_pipe, fpn_cfg->fpn_cor_en, drv_ctx);
        if (ret != TD_SUCCESS) {
            return;
        }
    }

    isp_drv_set_fpn_offset(pre_be, fpn_cfg->add_offset[0]);
    isp_drv_set_fpn1_offset(pre_be, fpn_cfg->add_offset[1]);
    isp_drv_set_fpn_correct_en(pre_be, fpn_cfg->fpn_cor_en);
    if (is_2to1_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        isp_drv_set_fpn1_correct_en(pre_be, fpn_cfg->fpn_cor_en);
    }
}

static td_void isp_drv_set_pre_be_sync_para(ot_vi_pipe vi_pipe, isp_pre_be_reg_type *pre_be, isp_drv_ctx *drv_ctx,
    const isp_be_sync_para *be_sync_para)
{
    td_bool cfg_blc_normal = TD_TRUE;
    if ((drv_ctx->dyna_blc_info.sync_cfg.black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) &&
        (isp_drv_get_ob_stats_update_pos(vi_pipe) == OT_ISP_UPDATE_OB_STATS_FE_FRAME_END)) {
        if (is_online_mode(drv_ctx->work_mode.running_mode) ||
            is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
            cfg_blc_normal = TD_FALSE;
        }
    }
    /* isp 4dgain */
    isp_drv_set_isp_4dgain0(pre_be, be_sync_para->wdr_gain[0]);
    isp_drv_set_isp_4dgain1(pre_be, be_sync_para->wdr_gain[1]);
    isp_drv_set_isp_4dgain2(pre_be, be_sync_para->wdr_gain[2]); /* 2: config channel 2 dgain value */
    /* wdr */
    isp_drv_set_wdr_sync_cfg(pre_be, be_sync_para);
    if (cfg_blc_normal == TD_TRUE) {
        isp_drv_set_pre_be_blc_reg(pre_be, &be_sync_para->be_blc);
        isp_drv_set_wdr_saturate_thr(pre_be, be_sync_para->saturate_thr);
        isp_drv_set_wdr_fusion_saturate_thr(pre_be, be_sync_para->fusion_saturate_thr);
        isp_drv_set_fpn_cfg(vi_pipe, drv_ctx, pre_be, &be_sync_para->fpn_cfg);
    }
}

static td_void isp_drv_set_post_be_sync_para(ot_vi_pipe vi_pipe, isp_post_be_reg_type *post_be, isp_drv_ctx *drv_ctx,
                                             isp_be_sync_para *be_sync_para)
{
    td_bool cfg_blc_normal = TD_TRUE;
    if ((drv_ctx->dyna_blc_info.sync_cfg.black_level_mode == OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) &&
        (isp_drv_get_ob_stats_update_pos(vi_pipe) == OT_ISP_UPDATE_OB_STATS_FE_FRAME_END)) {
        if (is_online_mode(drv_ctx->work_mode.running_mode)) {
            cfg_blc_normal = TD_FALSE;
        }
    }
    /* ldci */
    isp_drv_set_ldci_stat_evratio(post_be, be_sync_para->ldci_comp);

    /* drc */
    isp_drv_set_drc_shp_cfg(post_be, be_sync_para->drc_shp_log, be_sync_para->drc_shp_log);
    isp_drv_set_drc_div_denom_log(post_be, be_sync_para->drc_div_denom_log);
    isp_drv_set_drc_denom_exp(post_be, be_sync_para->drc_denom_exp);
    isp_drv_set_drc_prev_luma0(post_be, be_sync_para->drc_prev_luma[0]); /* isp_drc_prev_luma_0 */
    isp_drv_set_drc_prev_luma1(post_be, be_sync_para->drc_prev_luma[1]); /* isp_drc_prev_luma_1 */
    isp_drv_set_drc_prev_luma2(post_be, be_sync_para->drc_prev_luma[2]); /* isp_drc_prev_luma_2 */
    isp_drv_set_drc_prev_luma3(post_be, be_sync_para->drc_prev_luma[3]); /* isp_drc_prev_luma_3 */
    isp_drv_set_drc_prev_luma4(post_be, be_sync_para->drc_prev_luma[4]); /* isp_drc_prev_luma_4 */
    isp_drv_set_drc_prev_luma5(post_be, be_sync_para->drc_prev_luma[5]); /* isp_drc_prev_luma_5 */
    isp_drv_set_drc_prev_luma6(post_be, be_sync_para->drc_prev_luma[6]); /* isp_drc_prev_luma_6 */
    isp_drv_set_drc_prev_luma7(post_be, be_sync_para->drc_prev_luma[7]); /* isp_drc_prev_luma_7 */

    if (cfg_blc_normal == TD_TRUE) {
        /* be blc */
        isp_drv_set_post_be_blc_reg(post_be, &be_sync_para->be_blc);
        /* isp dgain */
        isp_drv_set_be_dgain(post_be, be_sync_para->isp_dgain[0]);
    }

    /* awb */
    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        isp_drv_set_awb_gain(post_be, be_sync_para->wb_gain);
        isp_drv_set_ccm(post_be, be_sync_para->ccm);
    }
}

static td_void isp_drv_set_hnr_be_cfg(isp_post_be_reg_type *post_be, td_u32 frame_flag, isp_be_sync_para *be_sync_para)
{
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
    td_u16 black_offset = be_sync_para->be_blc.raw_blc.blc[1] >> 2; /* 2 */
    td_u32 gain_coef;
    td_u16 isp_dgain = 0x100;
    td_bool dgain_bypass;

    dgain_bypass = (frame_flag & OT_FRAME_FLAG_DGAIN_BYPASS) ? TD_TRUE : TD_FALSE;
    if (dgain_bypass == TD_FALSE) {
        return;
    }

    gain_coef = clip3(0xFFF * 0x100 / div_0_to_1(0xFFF - black_offset) + 1, 0x100, 0x200);
    isp_dgain = (td_u32)(((td_u64)isp_dgain * gain_coef) >> 8); /* 8 */

    isp_drv_set_be_dgain(post_be, isp_dgain);
#endif
}

static td_void isp_drv_set_wdr_gain_bypass(isp_drv_ctx *drv_ctx, td_u32 frame_flag, isp_be_sync_para *be_sync_para)
{
#ifdef CONFIG_OT_ISP_HNR_SUPPORT
    td_bool dgain_bypass = (frame_flag & OT_FRAME_FLAG_DGAIN_BYPASS) ? TD_TRUE : TD_FALSE;
    if (dgain_bypass == TD_FALSE) {
        return;
    }

    if (drv_ctx->work_mode.video_mode != OT_VI_VIDEO_MODE_NORM) {
        return;
    }
    if (drv_ctx->hnr_info.normal_blend == TD_FALSE) {
        be_sync_para->wdr_gain[0] = 0x100;
    }
#endif
}

td_s32 isp_drv_set_be_sync_para_offline(ot_vi_pipe vi_pipe, td_void *be_node, isp_be_sync_para *be_sync_para,
                                        td_u32 frame_flag)
{
    td_u8 i;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_be_wo_reg_cfg *be_reg = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_check_pointer_return(be_node);
    isp_check_pointer_return(be_sync_para);
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if ((isp_drv_get_alg_run_select(vi_pipe) == OT_ISP_ALG_RUN_FE_ONLY) && (drv_ctx->yuv_mode != TD_TRUE)) {
        return TD_SUCCESS;
    }

    if (drv_ctx->isp_init != TD_TRUE) {
        return TD_FAILURE;
    }
    be_reg  = (isp_be_wo_reg_cfg *)be_node;

    isp_drv_set_wdr_gain_bypass(drv_ctx, frame_flag, be_sync_para);

    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        isp_drv_set_pre_be_sync_para(vi_pipe, &be_reg->be_reg_cfg[i].pre_be, drv_ctx, be_sync_para);
        isp_drv_set_post_be_sync_para(vi_pipe, &be_reg->be_reg_cfg[i].post_be, drv_ctx, be_sync_para);

        isp_drv_set_hnr_be_cfg(&be_reg->be_reg_cfg[i].post_be, frame_flag, be_sync_para);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_set_be_format_offline(isp_be_wo_reg_cfg *be_node, isp_drv_ctx *drv_ctx, isp_be_format be_format)
{
    td_u8 i;
    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        isp_drv_be_format_write(&be_node->be_reg_cfg[i].post_be, be_format);
    }
}

static td_void isp_drv_set_be_format_online(ot_vi_pipe vi_pipe, isp_be_format be_format)
{
    td_u8 i, block_dev;
    td_s32 ret;
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_post_be_reg_type *post_be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    ret = isp_drv_get_post_be_reg_virt_addr(vi_pipe, post_be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }
    drv_ctx = isp_drv_get_ctx(vi_pipe);
    block_dev = drv_ctx->work_mode.block_dev;
    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        isp_drv_be_format_write(post_be_reg[i + block_dev], be_format);
    }
}

td_s32 isp_drv_set_be_format(ot_vi_pipe vi_pipe, td_void *be_node, isp_be_format be_format)
{
    isp_drv_ctx *drv_ctx = TD_NULL;
    isp_check_pipe_return(vi_pipe);
    if (be_format >= ISP_BE_FORMAT_BUTT) {
        isp_err_trace("Err be_format!\n");
        return TD_FAILURE;
    }

    drv_ctx = isp_drv_get_ctx(vi_pipe);
    if (drv_ctx->isp_init != TD_TRUE) {
        return TD_FAILURE;
    }
    if (is_offline_mode(drv_ctx->work_mode.running_mode) ||
        is_striping_mode(drv_ctx->work_mode.running_mode) ||
        is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        isp_check_pointer_return(be_node);
        isp_drv_set_be_format_offline((isp_be_wo_reg_cfg *)be_node, drv_ctx, be_format);
    } else if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        isp_drv_set_be_format_online(vi_pipe, be_format);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_set_isp_pre_be_sync_param_online(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 k, blk_dev;
    td_s32 ret;
    isp_pre_be_reg_type *be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    isp_viproc_reg_type *pre_viproc[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    ret = isp_drv_get_pre_be_reg_virt_addr(vi_pipe, be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }

    ret = isp_drv_get_pre_viproc_reg_virt_addr(vi_pipe, pre_viproc);
    if (ret != TD_SUCCESS) {
        return;
    }

    blk_dev = drv_ctx->work_mode.block_dev;
    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_drv_set_pre_be_sync_para(vi_pipe, be_reg[k + blk_dev], drv_ctx, &drv_ctx->be_sync_para);
        isp_drv_set_be_regup(pre_viproc[k + blk_dev], TD_TRUE);
    }
}

static td_void isp_drv_set_isp_post_be_sync_param_online(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8   i, block_dev;
    td_s32  ret;
    isp_viproc_reg_type *post_viproc[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    isp_post_be_reg_type *post_be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    ret = isp_drv_get_post_be_reg_virt_addr(vi_pipe, post_be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }
    ret = isp_drv_get_post_viproc_reg_virt_addr(vi_pipe, post_viproc);
    if (ret != TD_SUCCESS) {
        return;
    }
    isp_drv_reg_config_bnr_online(post_be_reg, post_viproc, drv_ctx);

    block_dev = drv_ctx->work_mode.block_dev;
    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        isp_drv_set_post_be_sync_para(vi_pipe, post_be_reg[i + block_dev], drv_ctx, &drv_ctx->be_sync_para);
        isp_drv_set_be_regup(post_viproc[i + block_dev], TD_TRUE);
    }
}

td_s32 isp_drv_set_isp_be_sync_param_online(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 cfg_node_idx;
    isp_check_pointer_return(drv_ctx);
    cfg_node_idx = MIN2(isp_drv_get_be_sync_index(vi_pipe, drv_ctx), CFG2VLD_DLY_LIMIT - 1);
    if (drv_ctx->sync_cfg.node[cfg_node_idx] == TD_NULL) {
        return TD_SUCCESS;
    }

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        isp_drv_set_isp_pre_be_sync_param_online(vi_pipe, drv_ctx);
        isp_drv_set_isp_post_be_sync_param_online(vi_pipe, drv_ctx);
    } else if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        isp_drv_set_isp_pre_be_sync_param_online(vi_pipe, drv_ctx);
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_ISP_HNR_SUPPORT
#define ISP_HNR_OFF_BNR_SPECIAL_PROCESS_CNT 5

static td_void isp_drv_set_bnr_en_ai_md(isp_post_be_reg_type *post_be, td_bool ai_mode)
{
    if (ai_mode == TD_TRUE) {
        post_be->isp_bnr_cfg7.bits.isp_bnr_b_en_ai_md = 1;
    } else {
        post_be->isp_bnr_cfg7.bits.isp_bnr_b_en_ai_md = 0;
    }
}

static td_void isp_drv_set_hnr_bnr_snr_cfg(isp_post_be_reg_type *post_be, isp_drv_ctx *drv_ctx)
{
    if ((drv_ctx->hnr_info.hnr_en == TD_FALSE) && (post_be->isp_bnr_cfg.bits.isp_bnr_ensptnr == 0)) {
        post_be->isp_bnr_cfg.bits.isp_bnr_ensptnr     = 1;
    }

    isp_drv_set_bnr_en_ai_md(post_be, drv_ctx->hnr_info.hnr_en);
}

static td_void isp_drv_hnr_set_bnr_bypass(isp_be_all_reg_type *be_reg_cfg, td_bool bnr_bypass)
{
    td_bool bnr_en = bnr_bypass ? TD_FALSE : TD_TRUE;

    if (bnr_bypass == TD_FALSE) {
        return;
    }

    be_reg_cfg->post_be.isp_bnr_cfg.bits.isp_bnr_entmpnr = bnr_en;
    isp_drv_sharpen_mot_en_write(&(be_reg_cfg->post_be), bnr_en);
    /* bnr enable */
    be_reg_cfg->post_viproc.viproc_ispbe_ctrl0.bits.isp_bnr_en = bnr_en;
}

td_void isp_drv_set_bnr_lut_cfg(isp_post_be_lut_wstt_type *lut2stt)
{
    td_s32 i;

    for (i = 0; i < 65; i++) { /* [0, 65) */
        lut2stt->isp_bnr_lut_wstt[4 * i + 2].u32 = 300; /* 4 * i + 2, 300 */
    }

    for (i = 0; i < 64; i++) { /* [0, 64) */
        lut2stt->isp_bnr_lut_wstt[4 * i + 3].u32 = 300; /* 4 * i + 3, 300 */
    }
}

td_void isp_drv_set_hnr_bnr_cfg(isp_be_wo_reg_cfg *be_reg, isp_drv_ctx *drv_ctx, td_bool bnr_bypass, td_bool ref_none)
{
    td_u8 i;
    td_bool ai_mode = TD_FALSE;

    if (drv_ctx->hnr_info.normal_blend == TD_TRUE && drv_ctx->hnr_info.hnr_en == TD_TRUE) {
        ai_mode = TD_TRUE;
    }

    if (drv_ctx->work_mode.video_mode == OT_VI_VIDEO_MODE_NORM) {
        for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
            isp_drv_hnr_set_bnr_bypass(&be_reg->be_reg_cfg[i], bnr_bypass);
            isp_drv_set_bnr_en_ai_md(&be_reg->be_reg_cfg[i].post_be, ai_mode);
        }
        return;
    }

    for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
        /* snr */
        isp_drv_set_hnr_bnr_snr_cfg(&be_reg->be_reg_cfg[i].post_be, drv_ctx);
    }

    if (ref_none == TD_TRUE) { /* ref none mode no need transition */
        return;
    }

    if ((drv_ctx->hnr_info.hnr_en == TD_FALSE) && (drv_ctx->hnr_info.pre_hnr_en == TD_TRUE)) {
        drv_ctx->hnr_info.off_switch_cnt = ISP_HNR_OFF_BNR_SPECIAL_PROCESS_CNT;
    }

    if (drv_ctx->hnr_info.off_switch_cnt > 0) {
        for (i = 0; i < drv_ctx->work_mode.block_num; i++) {
            /* tnr */
            isp_drv_set_bnr_lut_cfg(&be_reg->be_reg_cfg[i].post_be.post_be_lut.post_be_lut2stt);
        }

        drv_ctx->hnr_info.off_switch_cnt--;
    }

    drv_ctx->hnr_info.pre_hnr_en = drv_ctx->hnr_info.hnr_en;
}
#endif
static td_void isp_drv_dynamic_blc_post_sync_info_init(isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    for (i = 1; i < DYNAMIC_BLC_BUF_NUM; i++) {
        (td_void)memcpy_s(&drv_ctx->dyna_blc_info.be_blc_sync[i], sizeof(isp_be_blc_dyna_cfg),
                          &drv_ctx->be_sync_para.be_blc, sizeof(isp_be_blc_dyna_cfg));
    }
}

static td_void isp_drv_fpn_sync_info_init(isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    for (i = 0; i < DYNAMIC_BLC_BUF_NUM; i++) {
        (td_void)memcpy_s(&drv_ctx->dyna_blc_info.fpn_cfg[i], sizeof(isp_fpn_sync_cfg),
                          0, sizeof(isp_fpn_sync_cfg));
    }
}

static td_void isp_drv_dynamic_be_blc_post_sync_info_update(isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    for (i = DYNAMIC_BLC_BUF_NUM - 1; i >= 1; i--) {
        (td_void)memcpy_s(&drv_ctx->dyna_blc_info.be_blc_sync[i], sizeof(isp_be_blc_dyna_cfg),
                          &drv_ctx->dyna_blc_info.be_blc_sync[i - 1], sizeof(isp_be_blc_dyna_cfg));
    }
}

static td_void isp_drv_fpn_sync_info_update(isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    for (i = DYNAMIC_BLC_BUF_NUM - 1; i >= 1; i--) {
        (td_void)memcpy_s(&drv_ctx->dyna_blc_info.fpn_cfg[i], sizeof(isp_fpn_sync_cfg),
                          &drv_ctx->dyna_blc_info.fpn_cfg[i - 1], sizeof(isp_fpn_sync_cfg));
    }
}

static td_s32 isp_drv_dynamic_blc_get_index(td_u8 wdr_idx, td_s16 *ag_blc_offset, isp_drv_ctx *drv_ctx)
{
    td_u8 blc_cfg_node_idx;

    isp_sync_cfg_buf_node *cfg_node = TD_NULL;
    if (wdr_idx >= OT_ISP_WDR_MAX_FRAME_NUM) {
        return TD_FALSE;
    }

    blc_cfg_node_idx = isp_drv_get_fe_sync_index(drv_ctx);

    blc_cfg_node_idx = MIN2(blc_cfg_node_idx, CFG2VLD_DLY_LIMIT - 1);

    cfg_node    = drv_ctx->sync_cfg.node[blc_cfg_node_idx];

    if (cfg_node == TD_NULL) {
        return TD_FALSE;
    }

    *ag_blc_offset = cfg_node->dynamic_blc_cfg.ag_offset;

    return TD_SUCCESS;
}

static td_void isp_drv_dynamic_blc_calc_separate(td_u8 wdr_idx, const isp_fe_reg_type *fe_reg, isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    td_s32 cur_read[OT_ISP_BAYER_CHN_NUM];
    td_s32 filt_res[OT_ISP_BAYER_CHN_NUM];
    td_s32 wgt_last, wgt_cur;
    td_s32 offset, blc_res, blc_diff;
    isp_dynamic_blc_info *dyna_blc_info = &drv_ctx->dyna_blc_info;

    cur_read[OT_ISP_CHN_R]  = fe_reg->isp_blc_dynamic_reg4.bits.isp_blc_dynamic_value0;
    cur_read[OT_ISP_CHN_GR] = fe_reg->isp_blc_dynamic_reg4.bits.isp_blc_dynamic_value1;
    cur_read[OT_ISP_CHN_GB] = fe_reg->isp_blc_dynamic_reg5.bits.isp_blc_dynamic_value2;
    cur_read[OT_ISP_CHN_B]  = fe_reg->isp_blc_dynamic_reg5.bits.isp_blc_dynamic_value3;
    if (wdr_idx >= OT_ISP_WDR_MAX_FRAME_NUM) {
        return;
    }
    if (dyna_blc_info->is_first_frame == TD_TRUE) {
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            dyna_blc_info->last_filt_res[wdr_idx][i] = cur_read[i];
        }
        isp_drv_dynamic_blc_post_sync_info_init(drv_ctx);
        isp_drv_fpn_sync_info_init(drv_ctx);
    }

    wgt_last = dyna_blc_info->sync_cfg.filter_strength;
    wgt_cur = (1 << DYNAMIC_BLC_FILTER_FIX_BIT) - wgt_last;
    offset  = dyna_blc_info->sync_cfg.offset;
    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        blc_diff = cur_read[i] - dyna_blc_info->last_filt_res[wdr_idx][i];
        if (blc_diff > dyna_blc_info->sync_cfg.filter_thr) {
            dyna_blc_info->last_filt_res[wdr_idx][i] = cur_read[i];
        }
    }

    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        filt_res[i] = (td_s32)signed_right_shift(
            (dyna_blc_info->last_filt_res[wdr_idx][i] * wgt_last + cur_read[i] * wgt_cur), DYNAMIC_BLC_FILTER_FIX_BIT);
        dyna_blc_info->last_filt_res[wdr_idx][i] = filt_res[i];

        blc_res = clip3(filt_res[i] + offset, 0x0, 0x3FFF);
        blc_diff = blc_res - dyna_blc_info->black_level[wdr_idx][i];
        if (ABS(blc_diff) > dyna_blc_info->sync_cfg.tolerance) {
            dyna_blc_info->black_level[wdr_idx][i] = (td_u16)blc_res;
        }
    }
}

static td_void isp_drv_dynamic_blc_calc_not_separate(td_u8 wdr_idx, const isp_fe_reg_type *fe_reg, isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    td_u16 blc_r, blc_gr, blc_gb, blc_b;
    td_s32 cur_read, filt_res;
    td_s32 wgt_last, wgt_cur;
    td_s32 offset, blc_res, blc_diff;

    isp_dynamic_blc_info *dyna_blc_info = &drv_ctx->dyna_blc_info;
    if (wdr_idx >= OT_ISP_WDR_MAX_FRAME_NUM) {
        return;
    }

    blc_r  = fe_reg->isp_blc_dynamic_reg4.bits.isp_blc_dynamic_value0;
    blc_gr = fe_reg->isp_blc_dynamic_reg4.bits.isp_blc_dynamic_value1;
    blc_gb = fe_reg->isp_blc_dynamic_reg5.bits.isp_blc_dynamic_value2;
    blc_b  = fe_reg->isp_blc_dynamic_reg5.bits.isp_blc_dynamic_value3;

    cur_read = (blc_r + blc_gr + blc_gb + blc_b) >> 2; /* right shift 2 */

    if (dyna_blc_info->is_first_frame == TD_TRUE) {
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            dyna_blc_info->last_filt_res[wdr_idx][i] = cur_read;
        }
        isp_drv_dynamic_blc_post_sync_info_init(drv_ctx);
        isp_drv_fpn_sync_info_init(drv_ctx);
    }

    wgt_last = dyna_blc_info->sync_cfg.filter_strength;
    wgt_cur  = (1 << DYNAMIC_BLC_FILTER_FIX_BIT) - wgt_last;
    offset   = dyna_blc_info->sync_cfg.offset;

    blc_diff = cur_read - dyna_blc_info->last_filt_res[wdr_idx][0];

    if (blc_diff > dyna_blc_info->sync_cfg.filter_thr) {
        dyna_blc_info->last_filt_res[wdr_idx][0] = cur_read;
    }

    filt_res = signed_right_shift(dyna_blc_info->last_filt_res[wdr_idx][0] * wgt_last + cur_read * wgt_cur,
                                  DYNAMIC_BLC_FILTER_FIX_BIT);
    dyna_blc_info->last_filt_res[wdr_idx][0] = filt_res;

    blc_res = clip3(filt_res + offset, 0x0, 0x3FFF);
    blc_diff = blc_res - dyna_blc_info->black_level[wdr_idx][0];
    if (ABS(blc_diff) > dyna_blc_info->sync_cfg.tolerance) {
        dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_R] = (td_u16)blc_res;
        dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_GR] = (td_u16)blc_res;
        dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_GB] = (td_u16)blc_res;
        dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_B] = (td_u16)blc_res;
    }
}

static td_void isp_drv_dynamic_blc_add_offset(td_u8 wdr_idx, isp_drv_ctx *drv_ctx)
{
    isp_dynamic_blc_info *dyna_blc_info = &drv_ctx->dyna_blc_info;
    td_s32 ret;
    td_s16 ag_blc_offset;

    ag_blc_offset = 0;

    ret = isp_drv_dynamic_blc_get_index(wdr_idx, &ag_blc_offset, drv_ctx);
    if (ret != TD_SUCCESS) {
        return;
    }

    dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_R] = clip3(dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_R]
                                                              + ag_blc_offset, 0x0, 0x3FFF);
    dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_GR] = clip3(dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_GR]
                                                               + ag_blc_offset, 0x0, 0x3FFF);
    dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_GB] = clip3(dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_GB]
                                                               + ag_blc_offset, 0x0, 0x3FFF);
    dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_B] = clip3(dyna_blc_info->black_level[wdr_idx][OT_ISP_CHN_B]
                                                              + ag_blc_offset, 0x0, 0x3FFF);
}

static td_s32 isp_drv_dynamic_blc_calc(isp_drv_ctx *drv_ctx)
{
    td_u8 k;
    ot_vi_pipe vi_pipe_bind;
    isp_fe_reg_type *fe_reg = TD_NULL;

    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        return TD_SUCCESS;
    }
    for (k = 0; k < drv_ctx->wdr_attr.pipe_num; k++) {
        vi_pipe_bind = drv_ctx->wdr_attr.pipe_id[k];
        isp_check_vir_pipe_return(vi_pipe_bind);
        isp_drv_fereg_ctx(vi_pipe_bind, fe_reg);

        if (drv_ctx->dyna_blc_info.sync_cfg.separate_en == TD_TRUE) {
            isp_drv_dynamic_blc_calc_separate(k, fe_reg, drv_ctx);
        } else {
            isp_drv_dynamic_blc_calc_not_separate(k, fe_reg, drv_ctx);
        }

        isp_drv_dynamic_blc_add_offset(k, drv_ctx);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_dynamic_update_isp_black_level(isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    td_u16 blc_size = OT_ISP_WDR_MAX_FRAME_NUM * OT_ISP_BAYER_CHN_NUM * sizeof(td_u16);
    td_s16 diff;
    isp_dynamic_blc_info *dyna_blc_info = &drv_ctx->dyna_blc_info;
    if (drv_ctx->dyna_blc_info.sync_cfg.user_black_level_en == TD_TRUE) { /* user black level */
        (td_void)memcpy_s(dyna_blc_info->isp_black_level, blc_size, dyna_blc_info->sync_cfg.user_black_level, blc_size);
    } else {  /* sns black level */
        (td_void)memcpy_s(dyna_blc_info->isp_black_level, blc_size, dyna_blc_info->black_level, blc_size);
    }

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            diff = (td_s16)dyna_blc_info->black_level[i][j] - (td_s16)dyna_blc_info->isp_black_level[i][j];
            dyna_blc_info->diff_black_level[i][j] = clip3(diff, -16383, 16383); /* range[-16383, 16383] */
        }
    }
}

static td_s32 isp_drv_dynamic_fe_blc_calc_reg(isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    isp_fe_blc_dyna_cfg dyna_blc;

    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        return TD_SUCCESS;
    }
    isp_drv_dynamic_update_isp_black_level(drv_ctx);

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            dyna_blc.fe_blc[i].blc[j]    = drv_ctx->dyna_blc_info.diff_black_level[i][j];
            dyna_blc.fe_dg_blc[i].blc[j] = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* Fe Dg */
            dyna_blc.fe_wb_blc[i].blc[j] = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* Fe WB */
            dyna_blc.fe_ae_blc[i].blc[j] = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* Fe AE */
            dyna_blc.fe_af_blc[i].blc[j] = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* Fe AF */
        }
    }

    dyna_blc.resh_dyna = TD_TRUE;
    isp_drv_reg_config_fe_blc_all_wdr_pipe(drv_ctx->wdr_attr.pipe_num, drv_ctx, &dyna_blc);

    return TD_SUCCESS;
}

static td_void isp_drv_ge_blc_amend(isp_be_blc_dyna_cfg *dyna_blc, const isp_dynamic_blc_info *dyna_blc_info)
{
    td_bool is_blc_same[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u8 i, j;
    td_s16 blc_r, blc_gr, blc_gb, blc_b;
    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        if (dyna_blc_info->isp_black_level[i][OT_ISP_CHN_GR] == dyna_blc_info->isp_black_level[i][OT_ISP_CHN_GB]) {
            is_blc_same[i] = TD_TRUE;
        } else {
            is_blc_same[i] = TD_FALSE;
        }
    }

    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        if (is_blc_same[i]) {
            for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
                dyna_blc->ge_blc[i].blc[j] = 0;
            }
        } else {
            blc_gr = -(dyna_blc_info->isp_black_level[i][OT_ISP_CHN_GB]);
            blc_gb = -(dyna_blc_info->isp_black_level[i][OT_ISP_CHN_GR]);
            blc_r = -(dyna_blc_info->isp_black_level[i][OT_ISP_CHN_R]);
            blc_b = -(dyna_blc_info->isp_black_level[i][OT_ISP_CHN_B]);
            dyna_blc->ge_blc[i].blc[OT_ISP_CHN_R] = blc_r;
            dyna_blc->ge_blc[i].blc[OT_ISP_CHN_GR] = blc_gr;
            dyna_blc->ge_blc[i].blc[OT_ISP_CHN_GB] = blc_gb;
            dyna_blc->ge_blc[i].blc[OT_ISP_CHN_B] = blc_b;
        }
    }
}

static td_void isp_drv_dynamic_be_blc_calc_linear(isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    isp_be_blc_dyna_cfg *dyna_blc = &drv_ctx->dyna_blc_info.be_blc_sync[0];

    for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
        for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
            dyna_blc->wdr_dg_blc[i].blc[j]  = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* 4DG */
            dyna_blc->wdr_blc[i].blc[j]     = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* WDR */
            dyna_blc->wdr_blc[i].out_blc    = 0;                       /* WDR */
            dyna_blc->flicker_blc[i].blc[j] = 0;                       /* flicker */
        }

        dyna_blc->expander_blc.blc[j] = 0;                            /* expander */
        dyna_blc->lsc_blc.blc[j]      = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* lsc */
        dyna_blc->dg_blc.blc[j]       = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* Dg */
        dyna_blc->ae_blc.blc[j]       = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* AE */
        dyna_blc->mg_blc.blc[j]       = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* MG */
        dyna_blc->wb_blc.blc[j]       = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* WB */
        dyna_blc->rgbir_blc.blc[j]    = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* RGBIR */
        dyna_blc->raw_blc.blc[j]      = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* pipeline blc */
        dyna_blc->af_blc.blc[j]       = 0; /* AF */
    }
    /* bnr */
    dyna_blc->bnr_blc.blc[0]  = drv_ctx->dyna_blc_info.isp_black_level[0][0] >> 2; /* shift 2bits to 12bits */

    isp_drv_ge_blc_amend(dyna_blc, &drv_ctx->dyna_blc_info);
}

static td_void isp_drv_dynamic_be_blc_calc_wdr(isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    td_u16 wdr_out_blc;
    isp_be_blc_dyna_cfg *dyna_blc = TD_NULL;

    if (drv_ctx->dyna_blc_info.sync_cfg.wdr_en == TD_FALSE) {
        isp_drv_dynamic_be_blc_calc_linear(drv_ctx);
        return;
    }

    dyna_blc = &drv_ctx->dyna_blc_info.be_blc_sync[0];
    wdr_out_blc = 0; /* WDR outblc, shift by 6 - 4 */
    for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
        for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
            dyna_blc->wdr_dg_blc[i].blc[j]  = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* 4DG */
            dyna_blc->wdr_blc[i].blc[j]     = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* WDR */
            dyna_blc->wdr_blc[i].out_blc    = wdr_out_blc;                /* WDR */
            dyna_blc->flicker_blc[i].blc[j] = drv_ctx->dyna_blc_info.isp_black_level[i][j]; /* flicker */
        }

        dyna_blc->expander_blc.blc[j] = 0;              /* expander */
        dyna_blc->lsc_blc.blc[j]      = wdr_out_blc >> 10; /* lsc,shift by 10 */
        dyna_blc->dg_blc.blc[j]       = wdr_out_blc >> 10; /* Dg,shift by 10 */
        dyna_blc->ae_blc.blc[j]       = wdr_out_blc >> 10; /* AE,shift by 10 */
        dyna_blc->mg_blc.blc[j]       = wdr_out_blc >> 10; /* MG,shift by 10 */
        dyna_blc->wb_blc.blc[j]       = wdr_out_blc >> 10; /* WB,shift by 10 */
        dyna_blc->rgbir_blc.blc[j]    = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* RGBIR */
        dyna_blc->raw_blc.blc[j]      = drv_ctx->dyna_blc_info.isp_black_level[0][j]; /* pipeline blc */
        dyna_blc->af_blc.blc[j]       = 0; /* AF */
    }

    /* bnr */
    dyna_blc->bnr_blc.blc[0]  = wdr_out_blc >> 8; /* 12bits, shift by 8 bits */
    /* ge */
    isp_drv_ge_blc_amend(dyna_blc, &drv_ctx->dyna_blc_info);
}

static td_void isp_drv_dynamic_pre_be_blc_reg(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx, isp_be_blc_dyna_cfg *be_blc)
{
    td_u8 k, blk_dev;
    td_s32 ret;
    isp_pre_be_reg_type *be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    ret = isp_drv_get_pre_be_reg_virt_addr(vi_pipe, be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }

    blk_dev = drv_ctx->work_mode.block_dev;
    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_drv_set_pre_be_blc_reg(be_reg[k + blk_dev], be_blc);
        isp_drv_set_wdr_saturate_thr(be_reg[k + blk_dev], drv_ctx->be_sync_para.saturate_thr);
        isp_drv_set_wdr_fusion_saturate_thr(be_reg[k + blk_dev], drv_ctx->be_sync_para.fusion_saturate_thr);
        isp_drv_set_fpn_cfg(vi_pipe, drv_ctx, be_reg[k + blk_dev], &drv_ctx->be_sync_para.fpn_cfg);
    }
}

static td_void isp_drv_dynamic_post_be_blc_reg(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
                                               const isp_be_blc_dyna_cfg *be_blc)
{
    td_u8 k, blk_dev;
    td_s32 ret;
    isp_post_be_reg_type *be_reg[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    ret = isp_drv_get_post_be_reg_virt_addr(vi_pipe, be_reg);
    if (ret != TD_SUCCESS) {
        return;
    }

    blk_dev = drv_ctx->work_mode.block_dev;
    for (k = 0; k < drv_ctx->work_mode.block_num; k++) {
        isp_drv_set_post_be_blc_reg(be_reg[k + blk_dev], be_blc);
        isp_drv_set_be_dgain(be_reg[k + blk_dev], drv_ctx->be_sync_para.isp_dgain[0]);
    }
}

static td_void isp_drv_dynamic_blc_update_stitch_sync(ot_vi_pipe vi_pipe, const isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    td_u8 stitch_idx = 0;
    isp_drv_ctx *drv_ctx_main_pipe = TD_NULL;

    for (i = 0; i < drv_ctx->stitch_attr.stitch_pipe_num; i++) {
        if (vi_pipe == drv_ctx->stitch_attr.stitch_bind_id[i]) {
            stitch_idx = i;
            break;
        }
    }

    drv_ctx_main_pipe = isp_drv_get_ctx(drv_ctx->stitch_attr.stitch_bind_id[0]);

    (td_void)memcpy_s(&drv_ctx_main_pipe->be_sync_para_stitch[stitch_idx].be_blc, sizeof(isp_be_blc_dyna_cfg),
                      &drv_ctx->be_sync_para.be_blc, sizeof(isp_be_blc_dyna_cfg));
}

static td_void isp_drv_dynamic_blc_update_dgain(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i;
    td_u16 black_offset;
    td_u32 isp_dgain, gain_coef;

    black_offset = drv_ctx->be_sync_para.be_blc.raw_blc.blc[1] >> 2; /* 2 */
    gain_coef = clip3(0xFFF * 0x100 / div_0_to_1(0xFFF - black_offset) + 1, 0x100, 0x200);
    isp_dgain = drv_ctx->be_sync_para.isp_dgain_no_blc[0];
    isp_dgain = (td_u32)(((td_u64)isp_dgain * gain_coef) >> 8); /* 8 */
    for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
        drv_ctx->be_sync_para.isp_dgain[i] = isp_dgain;
    }
}
static td_void isp_drv_dynamic_be_blc_calc_by_wdr_mode(isp_drv_ctx *drv_ctx)
{
    if (is_linear_mode(drv_ctx->wdr_attr.wdr_mode)) {
        isp_drv_dynamic_be_blc_calc_linear(drv_ctx);
    } else if (is_fs_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        isp_drv_dynamic_be_blc_calc_wdr(drv_ctx);
    }
}

static td_void isp_drv_dynamic_be_blc_get_idx(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    td_u32 *pre_be_idx, td_u32 *post_be_idx)
{
    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        *pre_be_idx = 0;
        *post_be_idx = 0;
        return;
    }

    if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        *pre_be_idx = 0;
        if (isp_drv_get_ob_stats_update_pos(vi_pipe) == OT_ISP_UPDATE_OB_STATS_FE_FRAME_START) {
            *post_be_idx = 2; /* delay 2 */
        } else {
            *post_be_idx = 1;
        }
        return;
    }

    if (is_offline_mode(drv_ctx->work_mode.running_mode) ||
        is_striping_mode(drv_ctx->work_mode.running_mode)) {
        if (isp_drv_get_ob_stats_update_pos(vi_pipe) == OT_ISP_UPDATE_OB_STATS_FE_FRAME_START) {
            *post_be_idx = 2; /* delay 2 */
            *pre_be_idx = 2; /* delay 2 */
        } else {
            *post_be_idx = 1; /* delay 1 */
            *pre_be_idx = 1; /* delay 1 */
        }
    }

    return;
}

static td_void isp_drv_update_be_blc_sync_param(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx,
    isp_be_blc_dyna_cfg *pre_be_blc, isp_be_blc_dyna_cfg *post_be_blc)
{
    td_u16  blc_dyna_size = sizeof(isp_blc_dyna_cfg);
    isp_be_blc_dyna_cfg *be_blc_sync = &drv_ctx->be_sync_para.be_blc;
    (td_void)memcpy_s(be_blc_sync, sizeof(isp_be_blc_dyna_cfg), post_be_blc, sizeof(isp_be_blc_dyna_cfg));

    (td_void)memcpy_s(&be_blc_sync->rgbir_blc, blc_dyna_size, &pre_be_blc->rgbir_blc, blc_dyna_size);
    (td_void)memcpy_s(&be_blc_sync->ge_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &pre_be_blc->ge_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->wdr_dg_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &pre_be_blc->wdr_dg_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->wdr_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &pre_be_blc->wdr_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->flicker_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM,
                      &pre_be_blc->flicker_blc[0], blc_dyna_size * OT_ISP_WDR_MAX_FRAME_NUM);
    (td_void)memcpy_s(&be_blc_sync->expander_blc, blc_dyna_size, &pre_be_blc->expander_blc, blc_dyna_size);

    if (drv_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        isp_drv_dynamic_blc_update_stitch_sync(vi_pipe, drv_ctx);
    }
}

static td_s32 isp_drv_dynamic_be_blc_calc_reg(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    isp_be_blc_dyna_cfg *pre_be_blc = TD_NULL;
    isp_be_blc_dyna_cfg *post_be_blc = TD_NULL;
    td_u32 pre_be_idx = 0;
    td_u32 post_be_idx = 0;
    isp_check_vir_pipe_return(vi_pipe);
    if (drv_ctx->wdr_attr.is_mast_pipe == TD_FALSE) {
        return TD_SUCCESS;
    }

    isp_drv_dynamic_be_blc_get_idx(vi_pipe, drv_ctx, &pre_be_idx, &post_be_idx);

    isp_drv_dynamic_be_blc_calc_by_wdr_mode(drv_ctx);

    pre_be_blc = &drv_ctx->dyna_blc_info.be_blc_sync[pre_be_idx];
    post_be_blc = &drv_ctx->dyna_blc_info.be_blc_sync[post_be_idx];

    isp_drv_update_be_blc_sync_param(vi_pipe, drv_ctx, pre_be_blc, post_be_blc);

    isp_drv_dynamic_be_blc_post_sync_info_update(drv_ctx);
    isp_drv_fpn_sync_info_update(drv_ctx);
    isp_drv_dynamic_blc_update_dgain(vi_pipe, drv_ctx);
    isp_drv_wdr_saturate_thr_update(drv_ctx);
    (td_void)memcpy_s(&drv_ctx->be_sync_para.fpn_cfg, sizeof(isp_fpn_sync_cfg),
                      &drv_ctx->dyna_blc_info.fpn_cfg[pre_be_idx], sizeof(isp_fpn_sync_cfg));

    if (isp_drv_get_ob_stats_update_pos(vi_pipe) != OT_ISP_UPDATE_OB_STATS_FE_FRAME_END) {
        return TD_SUCCESS;
    }

    if (is_online_mode(drv_ctx->work_mode.running_mode)) {
        isp_drv_dynamic_pre_be_blc_reg(vi_pipe, drv_ctx, pre_be_blc);
        isp_drv_dynamic_post_be_blc_reg(vi_pipe, drv_ctx, post_be_blc);
    } else if (is_pre_online_post_offline(drv_ctx->work_mode.running_mode)) {
        isp_drv_dynamic_pre_be_blc_reg(vi_pipe, drv_ctx, pre_be_blc);
    }

    return TD_SUCCESS;
}

static td_void isp_drv_dynamic_blc_update_actucal_info(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    td_u8 i, j;
    td_u8 wdr_merge_frame = 1;
    isp_blc_actual_info *actual_info = &drv_ctx->dyna_blc_info.actual_info;

    if (is_2to1_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        wdr_merge_frame = 2; /* 2to1 wdr */
    } else if (is_3to1_wdr_mode(drv_ctx->wdr_attr.wdr_mode)) {
        wdr_merge_frame = 3; /* 3to1 wdr */
    }

    for (i = 0; i < wdr_merge_frame; i++) {
        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            actual_info->isp_black_level[i][j] = drv_ctx->dyna_blc_info.isp_black_level[i][j];
            actual_info->sns_black_level[i][j] = drv_ctx->dyna_blc_info.black_level[i][j];
        }
    }

    for (i = wdr_merge_frame; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            actual_info->isp_black_level[i][j] = 0;
            actual_info->sns_black_level[i][j] = 0;
        }
    }

    actual_info->is_ready = TD_TRUE;
}

static td_void isp_drv_reg_config_dynamic_blc_single_pipe(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    if (drv_ctx->dyna_blc_info.ob_stats_read_en != TD_TRUE) {
        return;
    }
    isp_drv_dynamic_blc_calc(drv_ctx);

    isp_drv_dynamic_fe_blc_calc_reg(drv_ctx);

    isp_drv_dynamic_be_blc_calc_reg(vi_pipe, drv_ctx);

    isp_drv_dynamic_blc_update_actucal_info(vi_pipe, drv_ctx);

    drv_ctx->dyna_blc_info.ob_stats_read_en = TD_FALSE;
}

td_s32 isp_drv_reg_config_dynamic_blc(ot_vi_pipe vi_pipe, isp_drv_ctx *drv_ctx)
{
    isp_check_pointer_return(drv_ctx);
    if (drv_ctx->dyna_blc_info.sync_cfg.black_level_mode != OT_ISP_BLACK_LEVEL_MODE_DYNAMIC) {
        return TD_SUCCESS;
    }

    isp_drv_reg_config_dynamic_blc_single_pipe(vi_pipe, drv_ctx);
    return TD_SUCCESS;
}
