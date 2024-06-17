/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_LUT_CONFIG_H
#define ISP_LUT_CONFIG_H

#include "ot_debug.h"
#include "ot_isp_debug.h"
#include "isp_vreg.h"
#include "isp_main.h"
#include "isp_regcfg.h"
#include "isp_lut_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* pre be apb lut */
/* upisp_wdr_snoise_profile  16 bits */
static __inline td_void isp_wdr_snoise_profile_write(isp_pre_be_lut_apb_type *pre_be_apb_lut,
    const td_u16 *snoise_profile)
{
    td_s32 i;
    for (i = 0; i < 257; i++) { /* wdr snoise lut size 257 */
        pre_be_apb_lut->isp_wdr_snoise_lut[i].u32 = snoise_profile[i] & 0xFFFF;
    }
}

/* upisp_wdr_lnoise_profile  16 bits */
static __inline td_void isp_wdr_lnoise_profile_write(isp_pre_be_lut_apb_type *pre_be_apb_lut,
    const td_u16 *lnoise_profile)
{
    int i;
    for (i = 0; i < 257; i++) { /* wdr lnoise lut size 257 */
        pre_be_apb_lut->isp_wdr_lnoise_lut[i].u32 = lnoise_profile[i] & 0xFFFF;
    }
}

/* upisp_wdr_sigma_nm_lut  12 bits */
static __inline td_void isp_wdr_sigma_nm_lut_write(isp_pre_be_lut_apb_type *pre_be_apb_lut,
    const td_u16 *wdr_sigma_nm_lut)
{
    int i;
    for (i = 0; i < 513; i++) { /* wdr normalize lut size 513 */
        pre_be_apb_lut->isp_wdr_normalize_lut[i].u32 = wdr_sigma_nm_lut[i] & 0xFFF;
    }
}

static __inline td_void isp_pre_be_ae_weight_write(isp_pre_be_lut_apb_type *pre_be_apb_lut,
    const td_u32 *upisp_ae_weight)
{
    td_s32 i;

    for (i = 0; i < 64; i++) { /* ae weight table number 64 */
        pre_be_apb_lut->isp_ae_weight[i].u32 = upisp_ae_weight[i];
    }
}

/* upisp_expander_lut  21 bits */
static __inline td_void isp_expander_lut_write(isp_pre_be_lut_apb_type *pre_be_apb_lut,
    const td_u32 *upisp_expander_lut)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_EXPANDER_NODE_NUM; i++) {
        pre_be_apb_lut->isp_expander_lut[i].u32 = upisp_expander_lut[i] & 0x1FFFFF;
    }
}

/* pre be lut2stt */
static __inline td_void isp_dpc_lut_wstt_write(isp_pre_be_lut_wstt_type *pre_be_lut2stt,
    const td_u32 *upisp_dpc_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_STATIC_DP_COUNT_NORMAL; i++) {
        pre_be_lut2stt->isp_dpc_lut_wstt[i].u32 = upisp_dpc_lut_wstt[i];
    }
}

static __inline td_void isp_dpc1_lut_wstt_write(isp_pre_be_lut_wstt_type *pre_be_lut2stt,
    const td_u32 *upisp_dpc_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_STATIC_DP_COUNT_NORMAL; i++) {
        pre_be_lut2stt->isp_dpc1_lut_wstt[i].u32 = upisp_dpc_lut_wstt[i];
    }
}

static __inline td_void isp_pregammafe_lut_wstt_write(isp_pre_be_lut_wstt_type *pre_be_lut2stt,
    const td_u32 *pregammafe_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_PREGAMMA_NODE_NUM; i++) { /* pregammafe lut size is 257 */
        pre_be_lut2stt->isp_pregammafe_lut_wstt[i].u32 = pregammafe_lut_wstt[i] & 0xFFFFF;
    }
}

static __inline td_void isp_crb_lut_wstt_write(isp_pre_be_lut_wstt_type *pre_be_lut2stt,
    const td_u16 *upisp_crb_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < 1024; i++) { /* crb lut size is 1024 */
        pre_be_lut2stt->isp_crb_lut_wstt[i].u32 = upisp_crb_lut_wstt[i] & 0x3FF;
    }
}

/* post be apb lut */
static __inline td_void isp_post_be_ae_weight_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u32 *upisp_ae_weight)
{
    td_s32 i;

    for (i = 0; i < 64; i++) { /* ae weight table number 64 */
        post_be_apb_lut->isp_ae_weight[i].u32 = upisp_ae_weight[i];
    }
}

/* upisp_demosaic_depurp_lut  4 bits */
static __inline td_void isp_demosaic_depurp_lut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u8 *upisp_demosaic_depurp_lut)
{
    td_s32 i;

    for (i = 0; i < LCAC_STRENGTH_NUM; i++) {
        post_be_apb_lut->isp_demosaic_depurplut[i].u32 = upisp_demosaic_depurp_lut[i] & 0xF;
    }
}

/* upisp_nddm_gflut  12 bits */
static __inline td_void isp_nddm_gflut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *upisp_nddm_gflut)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_DEMOSAIC_LUT_LENGTH; i++) {
        post_be_apb_lut->isp_nddm_gf_lut[i].u32 = upisp_nddm_gflut[i] & 0xFFF;
    }
}

/*  upisp_ca_lut  24 bits */
static __inline td_void isp_ca_lut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u32 *upisp_ca_lut)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_CA_YRATIO_LUT_LENGTH; i++) {
        post_be_apb_lut->isp_ca_lut[i].u32 = upisp_ca_lut[i] & 0xFFFFFF;
    }
}

/* upisp_ldci_poply1_wlut  10 bits */
static __inline td_void isp_ldci_poply1_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_poply1_wlut)
{
    u_isp_ldci_polyp_wlut o_isp_ldci_polyp_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyp_wlut.u32 = post_be_apb_lut->isp_ldci_polyp_wlut[i].u32;
        o_isp_ldci_polyp_wlut.bits.isp_ldci_poply1_wlut = ldci_poply1_wlut[i];
        post_be_apb_lut->isp_ldci_polyp_wlut[i].u32 = o_isp_ldci_polyp_wlut.u32;
    }
}

/* upisp_ldci_poply2_wlut  10 bits */
static __inline td_void isp_ldci_poply2_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_poply2_wlut)
{
    u_isp_ldci_polyp_wlut o_isp_ldci_polyp_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyp_wlut.u32 = post_be_apb_lut->isp_ldci_polyp_wlut[i].u32;
        o_isp_ldci_polyp_wlut.bits.isp_ldci_poply2_wlut = ldci_poply2_wlut[i];
        post_be_apb_lut->isp_ldci_polyp_wlut[i].u32 = o_isp_ldci_polyp_wlut.u32;
    }
}

/* upisp_ldci_poply3_wlut  10 bits */
static __inline td_void isp_ldci_poply3_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_poply3_wlut)
{
    u_isp_ldci_polyp_wlut o_isp_ldci_polyp_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyp_wlut.u32 = post_be_apb_lut->isp_ldci_polyp_wlut[i].u32;
        o_isp_ldci_polyp_wlut.bits.isp_ldci_poply3_wlut = ldci_poply3_wlut[i];
        post_be_apb_lut->isp_ldci_polyp_wlut[i].u32 = o_isp_ldci_polyp_wlut.u32;
    }
}

/* upisp_ldci_plyq0_wlut  12 bits */
static __inline td_void isp_ldci_plyq0_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_plyq0_wlut)
{
    u_isp_ldci_polyq01_wlut o_isp_ldci_polyq01_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyq01_wlut.u32 = post_be_apb_lut->isp_ldci_polyq01_wlut[i].u32;
        o_isp_ldci_polyq01_wlut.bits.isp_ldci_plyq0_wlut = ldci_plyq0_wlut[i];
        post_be_apb_lut->isp_ldci_polyq01_wlut[i].u32 = o_isp_ldci_polyq01_wlut.u32;
    }
}

/* upisp_ldci_plyq1_wlut  12 bits */
static __inline td_void isp_ldci_plyq1_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_plyq1_wlut)
{
    u_isp_ldci_polyq01_wlut o_isp_ldci_polyq01_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyq01_wlut.u32 = post_be_apb_lut->isp_ldci_polyq01_wlut[i].u32;
        o_isp_ldci_polyq01_wlut.bits.isp_ldci_plyq1_wlut = ldci_plyq1_wlut[i];
        post_be_apb_lut->isp_ldci_polyq01_wlut[i].u32 = o_isp_ldci_polyq01_wlut.u32;
    }
}

/* Input         td_u32 *upisp_ldci_plyq2_wlut  12 bits */
static __inline td_void isp_ldci_plyq2_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_plyq2_wlut)
{
    u_isp_ldci_polyq23_wlut o_isp_ldci_polyq23_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyq23_wlut.u32 = post_be_apb_lut->isp_ldci_polyq23_wlut[i].u32;
        o_isp_ldci_polyq23_wlut.bits.isp_ldci_plyq2_wlut = ldci_plyq2_wlut[i];
        post_be_apb_lut->isp_ldci_polyq23_wlut[i].u32 = o_isp_ldci_polyq23_wlut.u32;
    }
}

/* Input         td_u32 *upisp_ldci_plyq3_wlut  12 bits */
static __inline td_void isp_ldci_plyq3_wlut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_s16 *ldci_plyq3_wlut)
{
    u_isp_ldci_polyq23_wlut o_isp_ldci_polyq23_wlut;
    td_s32 i;

    for (i = 0; i < LDCI_POLY_LUT_NUM; i++) {
        o_isp_ldci_polyq23_wlut.u32 = post_be_apb_lut->isp_ldci_polyq23_wlut[i].u32;
        o_isp_ldci_polyq23_wlut.bits.isp_ldci_plyq3_wlut = ldci_plyq3_wlut[i];
        post_be_apb_lut->isp_ldci_polyq23_wlut[i].u32 = o_isp_ldci_polyq23_wlut.u32;
    }
}

/* Input         td_u32 *upisp_adrc_tmlut0_diff  14 bits */
static __inline td_void isp_adrc_tmlut0_diff_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_tmlut0_diff)
{
    u_isp_adrc_tmlut0 o_isp_drc_tmlut0;
    td_s32 i;

    for (i = 0; i < OT_ISP_DRC_TM_NODE_NUM; i++) {
        o_isp_drc_tmlut0.u32 = post_be_apb_lut->isp_adrc_tmlut0[i].u32;
        o_isp_drc_tmlut0.bits.isp_adrc_tmlut0_diff = adrc_tmlut0_diff[i];
        post_be_apb_lut->isp_adrc_tmlut0[i].u32 = o_isp_drc_tmlut0.u32;
    }
}

/* Input         td_u32 *upisp_adrc_tmlut0_value  16 bits */
static __inline td_void isp_adrc_tmlut0_value_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_tmlut0_value)
{
    u_isp_adrc_tmlut0 o_isp_drc_tmlut0;
    td_s32 i;

    for (i = 0; i < OT_ISP_DRC_TM_NODE_NUM; i++) {
        o_isp_drc_tmlut0.u32 = post_be_apb_lut->isp_adrc_tmlut0[i].u32;
        o_isp_drc_tmlut0.bits.isp_adrc_tmlut0_value = adrc_tmlut0_value[i];
        post_be_apb_lut->isp_adrc_tmlut0[i].u32 = o_isp_drc_tmlut0.u32;
    }
}

/* Input         td_u32 *upisp_drc_cclut  12 bits */
static __inline td_void isp_adrc_cclut_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *upisp_adrc_cclut)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_DRC_CC_NODE_NUM; i++) {
        post_be_apb_lut->isp_adrc_cclut[i].u32 = upisp_adrc_cclut[i] & 0xFFF;
    }
}

/* Input         td_u32 *upisp_adrc_pregamma  20 bits */
static __inline td_void isp_adrc_pregamma_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u32 *upisp_adrc_pregamma)
{
    td_s32 i;

    for (i = 0; i < 257; i++) { /* adrc pregamma size 257 */
        post_be_apb_lut->isp_adrc_pregamma[i].u32 = upisp_adrc_pregamma[i] & 0xFFFFF;
    }
}

/* Input         td_u32 *upisp_adrc_localmix_dark0  8 bits */
static __inline td_void isp_adrc_localmix_dark0_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u8 *adrc_localmix_dark0)
{
    td_s32 i;
    u_isp_adrc_localmix o_isp_adrc_localmix;

    for (i = 0; i < 33; i++) { /* adrc localmix size 33 */
        o_isp_adrc_localmix.u32 = post_be_apb_lut->isp_adrc_localmix[i].u32;
        o_isp_adrc_localmix.bits.isp_adrc_localmix_dark0 = adrc_localmix_dark0[i];
        post_be_apb_lut->isp_adrc_localmix[i].u32 = o_isp_adrc_localmix.u32;
    }
}

/* Input         td_u32 *upisp_adrc_localmix_bright0  8 bits */
static __inline td_void isp_adrc_localmix_bright0_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u8 *adrc_localmix_bright0)
{
    td_s32 i;
    u_isp_adrc_localmix o_isp_adrc_localmix;

    for (i = 0; i < 33; i++) { /* adrc localmix bright0 size 33 */
        o_isp_adrc_localmix.u32 = post_be_apb_lut->isp_adrc_localmix[i].u32;
        o_isp_adrc_localmix.bits.isp_adrc_localmix_bright0 = adrc_localmix_bright0[i];
        post_be_apb_lut->isp_adrc_localmix[i].u32 = o_isp_adrc_localmix.u32;
    }
}

/* Input         td_u32 *upisp_adrc_localmix_dark1  8 bits */
static __inline td_void isp_adrc_localmix_dark1_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u8 *adrc_localmix_dark1)
{
    td_s32 i;
    u_isp_adrc_localmix o_isp_adrc_localmix;

    for (i = 0; i < 33; i++) { /* adrc localmix dark1 size 33 */
        o_isp_adrc_localmix.u32 = post_be_apb_lut->isp_adrc_localmix[i].u32;
        o_isp_adrc_localmix.bits.isp_adrc_localmix_dark1 = adrc_localmix_dark1[i];
        post_be_apb_lut->isp_adrc_localmix[i].u32 = o_isp_adrc_localmix.u32;
    }
}

/* Input         td_u32 *upisp_adrc_localmix_bright1  8 bits */
static __inline td_void isp_adrc_localmix_bright1_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u8 *adrc_localmix_bright1)
{
    td_s32 i;
    u_isp_adrc_localmix o_isp_adrc_localmix;

    for (i = 0; i < 33; i++) { /* adrc localmix bright1 size 33 */
        o_isp_adrc_localmix.u32 = post_be_apb_lut->isp_adrc_localmix[i].u32;
        o_isp_adrc_localmix.bits.isp_adrc_localmix_bright1 = adrc_localmix_bright1[i];
        post_be_apb_lut->isp_adrc_localmix[i].u32 = o_isp_adrc_localmix.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut0_value  16 bits */
static __inline td_void isp_adrc_mlut0_value_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut0_value)
{
    u_isp_adrc_mlut0 o_isp_adrc_mlut0;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut0 size 28 */
        o_isp_adrc_mlut0.u32 = post_be_apb_lut->isp_adrc_mlut0[i].u32;
        o_isp_adrc_mlut0.bits.isp_adrc_mlut0_value = adrc_mlut0_value[i];
        post_be_apb_lut->isp_adrc_mlut0[i].u32 = o_isp_adrc_mlut0.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut0_diff  12 bits */
static __inline td_void isp_adrc_mlut0_diff_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut0_diff)
{
    u_isp_adrc_mlut0 o_isp_adrc_mlut0;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut0 size 28 */
        o_isp_adrc_mlut0.u32 = post_be_apb_lut->isp_adrc_mlut0[i].u32;
        o_isp_adrc_mlut0.bits.isp_adrc_mlut0_diff = adrc_mlut0_diff[i];
        post_be_apb_lut->isp_adrc_mlut0[i].u32 = o_isp_adrc_mlut0.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut1_value  16 bits */
static __inline td_void isp_adrc_mlut1_value_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut1_value)
{
    u_isp_adrc_mlut1 o_isp_adrc_mlut1;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut1 size 28 */
        o_isp_adrc_mlut1.u32 = post_be_apb_lut->isp_adrc_mlut1[i].u32;
        o_isp_adrc_mlut1.bits.isp_adrc_mlut1_value = adrc_mlut1_value[i];
        post_be_apb_lut->isp_adrc_mlut1[i].u32 = o_isp_adrc_mlut1.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut1_diff  12 bits */
static __inline td_void isp_adrc_mlut1_diff_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut1_diff)
{
    u_isp_adrc_mlut1 o_isp_adrc_mlut1;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut1 size 28 */
        o_isp_adrc_mlut1.u32 = post_be_apb_lut->isp_adrc_mlut1[i].u32;
        o_isp_adrc_mlut1.bits.isp_adrc_mlut1_diff = adrc_mlut1_diff[i];
        post_be_apb_lut->isp_adrc_mlut1[i].u32 = o_isp_adrc_mlut1.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut2_value  16 bits */
static __inline td_void isp_adrc_mlut2_value_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut2_value)
{
    u_isp_adrc_mlut2 o_isp_adrc_mlut2;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut2 size 28 */
        o_isp_adrc_mlut2.u32 = post_be_apb_lut->isp_adrc_mlut2[i].u32;
        o_isp_adrc_mlut2.bits.isp_adrc_mlut2_value = adrc_mlut2_value[i];
        post_be_apb_lut->isp_adrc_mlut2[i].u32 = o_isp_adrc_mlut2.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut2_diff  12 bits */
static __inline td_void isp_adrc_mlut2_diff_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut2_diff)
{
    u_isp_adrc_mlut2 o_isp_adrc_mlut2;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut2 size 28 */
        o_isp_adrc_mlut2.u32 = post_be_apb_lut->isp_adrc_mlut2[i].u32;
        o_isp_adrc_mlut2.bits.isp_adrc_mlut2_diff = adrc_mlut2_diff[i];
        post_be_apb_lut->isp_adrc_mlut2[i].u32 = o_isp_adrc_mlut2.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut3_value  16 bits */
static __inline td_void isp_adrc_mlut3_value_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut3_value)
{
    u_isp_adrc_mlut3 o_isp_adrc_mlut3;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut3 size 28 */
        o_isp_adrc_mlut3.u32 = post_be_apb_lut->isp_adrc_mlut3[i].u32;
        o_isp_adrc_mlut3.bits.isp_adrc_mlut3_value = adrc_mlut3_value[i];
        post_be_apb_lut->isp_adrc_mlut3[i].u32 = o_isp_adrc_mlut3.u32;
    }
}

/*  Input         td_u32 *upisp_adrc_mlut3_diff  12 bits */
static __inline td_void isp_adrc_mlut3_diff_write(isp_post_be_lut_apb_type *post_be_apb_lut,
    const td_u16 *adrc_mlut3_diff)
{
    u_isp_adrc_mlut3 o_isp_adrc_mlut3;
    td_s32 i;

    for (i = 0; i < 28; i++) { /* adrc mlut3 size 28 */
        o_isp_adrc_mlut3.u32 = post_be_apb_lut->isp_adrc_mlut3[i].u32;
        o_isp_adrc_mlut3.bits.isp_adrc_mlut3_diff = adrc_mlut3_diff[i];
        post_be_apb_lut->isp_adrc_mlut3[i].u32 = o_isp_adrc_mlut3.u32;
    }
}

/* post be lut2stt */
/* Input         td_u32 *upisp_clut_lut_wstt  30 bits */
static __inline td_void isp_clut_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *upisp_clut_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_CLUT_LUT_LENGTH; i++) {
        post_be_lut2stt->isp_clut_lut_wstt[i].u32 = upisp_clut_lut_wstt[i];
    }
}

/* dehaze_lut_wstt size:1280
   0~1024:dehaze prestat lut
        odd index: (prestat_lut >> 16) & 0x3ff
        even index:prestat_lut & 0x3ff
   1024~1280: dehaze strength lut */
static __inline td_void isp_dehaze_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt, td_u16 blk_num,
    const td_u32 *prestat_wstt, const td_u8 *lut_wstt)
{
    td_s32 i;

    for (i = 0; i < blk_num; i++) {
        post_be_lut2stt->isp_dehaze_lut_wstt[2 * i].bits.isp_dehaze_lut_wstt = prestat_wstt[i] & 0x3ff; /* addr 2 * i */
        post_be_lut2stt->isp_dehaze_lut_wstt[2 * i + 1].bits.isp_dehaze_lut_wstt = /* addr 2 * i + 1 */
            (prestat_wstt[i] >> 16) & 0x3ff; /* odd index  (prestat_lut >> 16) & 0x3ff */
    }

    for (i = 2 * blk_num; i < 1024; i++) { /* [2 * blk_num, 1024] */
        post_be_lut2stt->isp_dehaze_lut_wstt[i].bits.isp_dehaze_lut_wstt = 0;
    }

    for (i = 1024; i < 1280; i++) { /* [1024, 1280] */
        post_be_lut2stt->isp_dehaze_lut_wstt[i].bits.isp_dehaze_lut_wstt = lut_wstt[i - 1024]; /* sub 1024 */
    }
}

static __inline td_void isp_bnr_noisesd_lut_wstt_write(isp_post_be_lut_wstt_type *lut2stt,
    const td_u16 *isp_bnr_noisesd, const td_u16 *isp_bnr_noisesd_s)
{
    td_s32 i, idx;
    for (i = 0; i < 65; i++) { /* [0, 65) */
        lut2stt->isp_bnr_lut_wstt[4 * i + 0].u32 = (isp_bnr_noisesd_s[2 * i] << 0xF) + isp_bnr_noisesd[2 * i]; /* 4/2 */
    }

    for (i = 0; i < 64; i++) { /* [0, 64) */
        idx = 2 * i + 1;  /* 2 */
        lut2stt->isp_bnr_lut_wstt[4 * i + 1].u32 = (isp_bnr_noisesd_s[idx] << 0xF) + isp_bnr_noisesd[idx]; /* 4 */
    }
}

static __inline td_void isp_bnr_noisesd_m_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u16 *isp_bnr_noisesd_m)
{
    td_s32 i;
    for (i = 65; i < 130; i++) { /* [65, 130) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 0].u32 = isp_bnr_noisesd_m[2 * (i - 65)]; /* 65/4/2 */
    }

    for (i = 64; i < 128; i++) { /* [64, 128) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 1].u32 = isp_bnr_noisesd_m[2 * (i - 64) + 1]; /* 64/4/2 */
    }
}

static __inline td_void isp_bnr_fbratiotable_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u8 *isp_bnr_fbratiotable)
{
    td_s32 i;
    for (i = 130; i < 194; i++) { /* [130, 194) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 0].u32 = isp_bnr_fbratiotable[i - 130]; /* addr 4 * i + 0, sub 130 */
    }
}

static __inline td_void isp_bnr_noiseinvs_magidx_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u8 *isp_bnr_noiseinvs_magidx, const td_u8 *isp_bnr_noiseinvs_magidx_m)
{
    td_s32 i;
    for (i = 130; i < 259; i++) { /* [130, 259) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 1].u32 = /* addr 4 * i + 1 */
            (isp_bnr_noiseinvs_magidx_m[i - 130] << 5) + isp_bnr_noiseinvs_magidx[i - 130]; /* shift 5, sub 130 */
    }
}

static __inline td_void isp_bnr_coring_low_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u16 *isp_bnr_coring_low)
{
    td_s32 i;
    for (i = 259; i < 276; i++) { /* [259, 276) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 0].u32 = isp_bnr_coring_low[2 * (i - 259)]; /* 4/259/2 */
    }

    for (i = 259; i < 275; i++) { /* [259, 275) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 1].u32 = isp_bnr_coring_low[2 * (i - 259) + 1]; /* 4/259/2 */
    }
}

static __inline td_void isp_bnr_noiseinv_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *isp_bnr_noiseinv)
{
    td_s32 i;
    for (i = 0; i < 65; i++) { /* [0, 65) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 2].u32 = isp_bnr_noiseinv[2 * i]; /* addr 4 * i + 2 */
    }

    for (i = 0; i < 64; i++) { /* [0, 64) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 3].u32 = isp_bnr_noiseinv[2 * i + 1]; /* addr 4 * i + 3, 2 */
    }
}

static __inline td_void isp_bnr_noiseinv_m_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *isp_bnr_noiseinv_m)
{
    td_s32 i;
    for (i = 65; i < 130; i++) { /* [65, 130) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 2].u32 = isp_bnr_noiseinv_m[2 * (i - 65)]; /* 4/2/65 */
    }

    for (i = 64; i < 128; i++) { /* [64, 128) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 3].u32 = isp_bnr_noiseinv_m[2 * (i - 64) + 1]; /* 2/3/4/64 */
    }
}

static __inline td_void isp_bnr_rlsc_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u16 *rlsc_lut0_ch0, const td_u16 *rlsc_lut0_ch1, const td_u16 *rlsc_lut1_ch2, const td_u16 *rlsc_lut1_ch3)
{
    td_s32 i;
    for (i = 259; i < 389; i++) { /* [259, 389) */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 2].u32 =  /* addr 4 * i + 2 */
            (rlsc_lut0_ch1[i - 259] << 16) + rlsc_lut0_ch0[i - 259]; /* shift 16, sub 259 */
        post_be_lut2stt->isp_bnr_lut_wstt[4 * i + 3].u32 =   /* addr 4 * i + 3 */
            (rlsc_lut1_ch3[i - 259] << 16) + rlsc_lut1_ch2[i - 259]; /* shift 16, sub 259 */
    }
}

/* Input         td_u32 *upisp_gamma_lut_wstt  12 bits */
static __inline td_void isp_gamma_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u16 *gamma_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < GAMMA_REG_NODE_NUM; i++) {
        post_be_lut2stt->isp_gamma_lut_wstt[i].u32 = gamma_lut_wstt[i] & 0xFFF;
    }
}

/* Input         td_u32 *upisp_sharpen_lut_wstt  30 bits */
static __inline td_void isp_sharpen_lut_wstt_write(isp_post_be_lut_wstt_type *lut2stt, const td_u16 *mfgaind,
    const td_u16 *mfgainud, const td_u16 *hfgaind, const td_u16 *hfgainud)
{
    td_s32 i;

    for (i = 0; i < SHRP_GAIN_LUT_SIZE; i++) {
        lut2stt->sharp_lut_wstt[i * 4].bits.isp_sharpen_lut_wstt     = mfgaind[i] + (hfgaind[i] << 12);  /* 4, 12 */
        lut2stt->sharp_lut_wstt[i * 4 + 1].bits.isp_sharpen_lut_wstt = mfgainud[i] + (hfgainud[i] << 12); /* 4/1/12 */
    }
}

static __inline td_void isp_sharpen_mot_lut_wstt_write(isp_post_be_lut_wstt_type *lut2stt,
    const td_u16 *mfmotgaind, const td_u16 *mfmotgainud, const td_u16 *hfmotgaind, const td_u16 *hfmotgainud)
{
    td_s32 i;

    for (i = 0; i < SHRP_GAIN_LUT_SIZE; i++) {
        lut2stt->sharp_lut_wstt[i * 4 + 2].bits.isp_sharpen_lut_wstt =  /* i * 4 + 2 */
            mfmotgaind[i] + (hfmotgaind[i] << 12); /* shift 12bits */
        lut2stt->sharp_lut_wstt[i * 4 + 3].bits.isp_sharpen_lut_wstt =   /* i * 4 + 3 */
            mfmotgainud[i] + (hfmotgainud[i] << 12); /* shift 12bits */
    }
}

/* Input         td_u32 *upisp_lsc_lut_wstt  20 bits */
static __inline td_void isp_lsc_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *upisp_lsc_rgain_wstt, const td_u32 *upisp_lsc_grgain_wstt,
    const td_u32 *upisp_lsc_gbgain_wstt, const td_u32 *upisp_lsc_bgain_wstt)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_LSC_GRID_POINTS; i++) {
        post_be_lut2stt->isp_lsc_lut_wstt[4 * i + 0].u32 = upisp_lsc_rgain_wstt[i];  /* 4 * i  r_gain */
        post_be_lut2stt->isp_lsc_lut_wstt[4 * i + 1].u32 = upisp_lsc_grgain_wstt[i]; /* 4 * i + 1  gr_gain */
        post_be_lut2stt->isp_lsc_lut_wstt[4 * i + 2].u32 = upisp_lsc_bgain_wstt[i];  /* 4 * i + 2  b_gain */
        post_be_lut2stt->isp_lsc_lut_wstt[4 * i + 3].u32 = upisp_lsc_gbgain_wstt[i]; /* 4 * i + 3  gb_gain */
    }
}

/* ldci lut_wstt size: 65 * 4; order:
   4 * i:                 (cgain_lut << 20) + (statdrc_lut << 10) + calcdrc_lut
   4 * i + 1(for i < 33): (heneg_lut << 18) + (hepos_lut << 9) + delut_lut
   4 * i + 2(for i < 33): (ldci_usmneg_lut << 9) + ldci_usmpos_lut
   4 * i + 3            : 0 */
static __inline td_void isp_ldci_drc_cgain_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_s16 *calcdrc_lut, const td_s16 *statdrc_lut, const td_u32 *cgain_lut)
{
    td_u32 ldci_statdrc;
    td_s32 i;

    for (i = 0; i < 65; i++) { /* [0, 65) */
        ldci_statdrc = statdrc_lut[i];
        post_be_lut2stt->isp_ldci_lut_wstt[4 * i].u32 =  /* addr 4 * i */
            (cgain_lut[i] << 20) + (ldci_statdrc << 10) + calcdrc_lut[i]; /* 20~29 cgain, 10~19 statdrc */
        post_be_lut2stt->isp_ldci_lut_wstt[4 * i + 3].u32 = 0;  /* addr 4 * i + 3 */
    }
}

static __inline td_void isp_ldci_he_delut_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *delut_lut_wstt, const td_u32 *hepos_lut_wstt, const td_u32 *heneg_lut_wstt)
{
    td_s32 i;

    for (i = 0; i < 33; i++) { /* [0, 33) */
        post_be_lut2stt->isp_ldci_lut_wstt[4 * i + 1].u32 =   /* addr 4 * i + 1 */
            (heneg_lut_wstt[i] << 18) + (hepos_lut_wstt[i] << 9) + delut_lut_wstt[i]; /* 18~26 heneg, 9~17 hepos */
    }

    for (i = 33; i < 65; i++) { /* [33, 65) */
        post_be_lut2stt->isp_ldci_lut_wstt[4 * i + 1].u32 = 0;  /* addr 4 * i + 1 */
    }
}

static __inline td_void isp_ldci_usm_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *usmpos_lut, const td_u32 *usmneg_lut)
{
    td_s32 i;

    for (i = 0; i < 33; i++) { /* [0, 33) */
        post_be_lut2stt->isp_ldci_lut_wstt[4 * i + 2].u32 = (usmneg_lut[i] << 9) + usmpos_lut[i]; /* 4/2,[9,17] */
    }

    for (i = 33; i < 65; i++) { /* [33, 65) */
        post_be_lut2stt->isp_ldci_lut_wstt[4 * i + 2].u32 = 0; /* addr 4 * i + 2 */
    }
}

/* Input         td_u32 *degammafe_lut  20 bits */
static __inline td_void isp_degammafe_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *degammafe_lut)
{
    td_s32 i;

    for (i = 0; i < OT_ISP_PREGAMMA_NODE_NUM; i++) { /* pregamma lut size is 257 */
        post_be_lut2stt->isp_degammafe_lut_wstt[i].u32 = degammafe_lut[i] & 0xFFFFF;
    }
}

/*  upisp_bnrshp_lut_wstt  24 bits */
static __inline td_void isp_bnrshp_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt,
    const td_u32 *uisp_bnrshp_wstt0, const td_u32 *uisp_bnrshp_wstt1,
    const td_u32 *uisp_bnrshp_wstt2, const td_u32 *uisp_bnrshp_wstt3)
{
    td_s32 i;

    for (i = 0; i < 32; i++) { /* 32 */
        post_be_lut2stt->isp_bnrshp_lut_wstt[i * 4].u32 = uisp_bnrshp_wstt0[i]; /* addr 4 * i */
        post_be_lut2stt->isp_bnrshp_lut_wstt[i * 4 + 1].u32 = uisp_bnrshp_wstt1[i]; /* addr 4 * i + 1 */
        post_be_lut2stt->isp_bnrshp_lut_wstt[i * 4 + 2].u32 = uisp_bnrshp_wstt2[i]; /* addr 4 * i + 2 */
        post_be_lut2stt->isp_bnrshp_lut_wstt[i * 4 + 3].u32 = uisp_bnrshp_wstt3[i]; /* addr 4 * i + 3 */
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
