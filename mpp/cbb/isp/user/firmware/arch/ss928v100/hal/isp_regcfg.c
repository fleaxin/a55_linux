/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "mkp_isp.h"
#include "isp_config.h"
#include "isp_lut_config.h"
#include "isp_ext_config.h"
#include "isp_main.h"
#include "ot_mpi_sys.h"
#include "ot_math.h"
#include "valg_plat.h"
#include "isp_regcfg.h"

isp_be_buf            g_be_buf_ctx[OT_ISP_MAX_PIPE_NUM] = {{ 0 }};
isp_reg_cfg_attr     *g_reg_cfg_ctx[OT_ISP_MAX_PIPE_NUM] = { TD_NULL };
isp_be_lut_buf        g_be_lut_buf_ctx[OT_ISP_MAX_PIPE_NUM] = { 0 };
isp_ldci_read_stt_buf g_ldci_read_stt_buf_ctx[OT_ISP_MAX_PIPE_NUM] = { 0 };

#define isp_regcfg_set_ctx(dev, ctx) (g_reg_cfg_ctx[dev] = (ctx))
#define isp_regcfg_reset_ctx(dev) (g_reg_cfg_ctx[dev] = TD_NULL)
#define be_reg_get_ctx(dev, ctx) ctx = &g_be_buf_ctx[dev]
#define be_lut_buf_get_ctx(dev, ctx) ctx = &g_be_lut_buf_ctx[dev]
#define ldci_buf_get_ctx(dev, ctx) ctx = &g_ldci_read_stt_buf_ctx[dev]
#define STT_LUT_CONFIG_TIMES 3
#define HRS_PIPE 0
#define AE_RES_LIMIT 4096

isp_reg_cfg_attr *isp_get_regcfg_ctx(ot_vi_pipe vi_pipe)
{
    return g_reg_cfg_ctx[vi_pipe];
}

td_s32 isp_mod_param_get(ot_isp_mod_param *mod_param)
{
    isp_check_pointer_return(mod_param);

    mod_param->quick_start = TD_FALSE;

    return TD_SUCCESS;
}

td_s32 isp_clut_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if (isp_ctx->isp_yuv_mode == TD_TRUE) {
        return TD_SUCCESS;
    }
    ret = ioctl(isp_get_fd(vi_pipe), ISP_CLUT_BUF_INIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] clut buffer init failed\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_void isp_clut_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if (isp_ctx->isp_yuv_mode == TD_TRUE) {
        return;
    }
    ret = ioctl(isp_get_fd(vi_pipe), ISP_CLUT_BUF_EXIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit clut bufs failed\n", vi_pipe);
        return;
    }
}

static td_s32 isp_update_be_lut_stt_buf_ctx(ot_vi_pipe vi_pipe, td_u64 phy_addr)
{
    td_u8 i;
    td_u64 size;
    td_void *virt_addr = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        return TD_FAILURE;
    }

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    size = sizeof(isp_be_lut_wstt_type);

    virt_addr = ot_mpi_sys_mmap(phy_addr, size * 2 * ISP_MAX_BE_NUM); /* lut_stt_buf number 2  */
    if (virt_addr == TD_NULL) {
        isp_err_trace("Pipe:%d get be lut stt bufs address failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        be_lut_buf->lut_stt_buf[i].phy_addr = phy_addr + 2 * i * size;                        /* phy_addr index 2  */
        be_lut_buf->lut_stt_buf[i].vir_addr = (td_void *)((td_u8 *)virt_addr + 2 * i * size); /* lut_stt_buf 2  */
        be_lut_buf->lut_stt_buf[i].size = size;
    }

    return TD_SUCCESS;
}


static td_s32 isp_update_be_pre_online_post_offline_lut_stt_buf_ctx(ot_vi_pipe vi_pipe, td_u64 phy_addr)
{
    td_u8 i;
    td_u64 size;
    td_void *virt_addr = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return TD_FAILURE;
    }

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    size = sizeof(isp_pre_be_lut_wstt_type);

    virt_addr = ot_mpi_sys_mmap(phy_addr, size * 2 * ISP_MAX_BE_NUM); /* lut_stt_buf number 2  */
    if (virt_addr == TD_NULL) {
        isp_err_trace("Pipe:%d get be lut stt bufs address failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        be_lut_buf->lut_stt_buf[i].phy_addr = phy_addr + 2 * i * size;                        /* phy_addr index 2  */
        be_lut_buf->lut_stt_buf[i].vir_addr = (td_void *)((td_u8 *)virt_addr + 2 * i * size); /* lut_stt_buf 2  */
        be_lut_buf->lut_stt_buf[i].size = size;
    }

    return TD_SUCCESS;
}

static td_s32 isp_update_be_pre_lut_stt_buf_ctx(ot_vi_pipe vi_pipe, td_u64 phy_addr)
{
    td_u8 i;
    td_u64 size;
    td_void *virt_addr = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        return TD_FAILURE;
    }

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    size = sizeof(isp_be_lut_wstt_type);

    virt_addr = ot_mpi_sys_mmap(phy_addr, size * 2 * ISP_MAX_BE_NUM); /* lut_stt_buf number 2  */
    if (virt_addr == TD_NULL) {
        isp_err_trace("Pipe:%d get be lut stt bufs address failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    for (i = 0; i < ISP_MAX_BE_NUM; i++) {
        be_lut_buf->lut_stt_buf[i].phy_addr = phy_addr + 2 * i * size;                        /* phy_addr index 2  */
        be_lut_buf->lut_stt_buf[i].vir_addr = (td_void *)((td_u8 *)virt_addr + 2 * i * size); /* lut_stt_buf 2  */
        be_lut_buf->lut_stt_buf[i].size = size;
    }

    return TD_SUCCESS;
}

static td_s32 isp_be_pre_online_post_offline_lut_buf_addr_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_u64 phy_addr;

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_PRE_LUT_STT_BUF_GET, &phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d get be lut2stt bufs address failed%x!\n", vi_pipe, ret);
        return ret;
    }

    ot_ext_system_be_lut_stt_buffer_high_addr_write(vi_pipe, (phy_addr >> 32)); /* shitf right  32 bit  */
    ot_ext_system_be_lut_stt_buffer_low_addr_write(vi_pipe, (phy_addr & 0xFFFFFFFF));

    ret = isp_update_be_pre_online_post_offline_lut_stt_buf_ctx(vi_pipe, phy_addr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 isp_be_lut_buf_addr_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_u64 phy_addr;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return TD_SUCCESS;
    }

    if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        isp_be_pre_online_post_offline_lut_buf_addr_init(vi_pipe);
        return TD_SUCCESS;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_LUT_STT_BUF_GET, &phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d get be lut2stt bufs address failed%x!\n", vi_pipe, ret);
        return ret;
    }

    ot_ext_system_be_lut_stt_buffer_high_addr_write(vi_pipe, (phy_addr >> 32)); /* shitf right  32 bit  */
    ot_ext_system_be_lut_stt_buffer_low_addr_write(vi_pipe, (phy_addr & 0xFFFFFFFF));

    ret = isp_update_be_lut_stt_buf_ctx(vi_pipe, phy_addr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}


static td_s32 isp_be_lut_buf_addr_exit(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return TD_SUCCESS;
    }
    if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        ot_mpi_sys_munmap(be_lut_buf->lut_stt_buf[0].vir_addr,
            sizeof(isp_pre_be_lut_wstt_type) * 2 * ISP_MAX_BE_NUM); /* lut buf number is 2 */
        for (i = 0; i < ISP_MAX_BE_NUM; i++) {
            be_lut_buf->lut_stt_buf[i].vir_addr = TD_NULL;
        }
        return TD_SUCCESS;
    }

    if (be_lut_buf->lut_stt_buf[0].vir_addr != TD_NULL) {
        ot_mpi_sys_munmap(be_lut_buf->lut_stt_buf[0].vir_addr,
            sizeof(isp_be_lut_wstt_type) * 2 * ISP_MAX_BE_NUM); /* lut buf number is 2 */
        for (i = 0; i < ISP_MAX_BE_NUM; i++) {
            be_lut_buf->lut_stt_buf[i].vir_addr = TD_NULL;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_stt_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    if (ioctl(isp_get_fd(vi_pipe), ISP_STT_BUF_INIT) != TD_SUCCESS) {
        isp_err_trace("ISP[%d] stt buffer init failed\n", vi_pipe);
        return TD_FAILURE;
    }

    ret = isp_be_lut_buf_addr_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] be lut2stt buffer address init failed\n", vi_pipe);
        isp_stt_buf_exit(vi_pipe);

        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_void isp_stt_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    ret = isp_be_lut_buf_addr_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] be lut stt buffer exit failed\n", vi_pipe);
    }

    if (ioctl(isp_get_fd(vi_pipe), ISP_STT_BUF_EXIT) != TD_SUCCESS) {
        isp_err_trace("exit stt bufs failed\n");
        return;
    }
}

td_s32 isp_stt_addr_init(ot_vi_pipe vi_pipe)
{
    if (ioctl(isp_get_fd(vi_pipe), ISP_STT_ADDR_INIT) != TD_SUCCESS) {
        isp_err_trace("ISP[%d] stt address init failed\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 isp_update_ldci_read_stt_buf_ctx(ot_vi_pipe vi_pipe, td_u64 phy_addr)
{
    td_u8 i;
    td_u64 size;
    td_void *virt_addr = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_ldci_read_stt_buf *ldci_read_stt_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->ldci_tpr_flt_en == TD_FALSE) {
        return TD_SUCCESS;
    }

    ldci_buf_get_ctx(vi_pipe, ldci_read_stt_buf);

    size = sizeof(isp_ldci_stat);
    virt_addr = ot_mpi_sys_mmap(phy_addr, size * ldci_read_stt_buf->buf_num);
    if (virt_addr == TD_NULL) {
        isp_err_trace("ISP[%d]:map ldci read stt buffer failed\n", vi_pipe);
        return TD_FAILURE;
    }

    for (i = 0; i < ldci_read_stt_buf->buf_num; i++) {
        ldci_read_stt_buf->read_buf[i].phy_addr = phy_addr + i * size;
        ldci_read_stt_buf->read_buf[i].vir_addr = (td_void *)((td_u8 *)virt_addr + i * size);
        ldci_read_stt_buf->read_buf[i].size = size;
    }

    return TD_SUCCESS;
}

static td_s32 isp_ldci_read_stt_buf_addr_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_u64 phy_addr;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_ldci_read_stt_buf *ldci_read_stt_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->ldci_tpr_flt_en == TD_FALSE) {
        return TD_SUCCESS;
    }

    ldci_buf_get_ctx(vi_pipe, ldci_read_stt_buf);
    ret = ioctl(isp_get_fd(vi_pipe), ISP_LDCI_READ_STT_BUF_GET, ldci_read_stt_buf);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d]:Get ldci read stt buffer address failed\n", vi_pipe);
        return ret;
    }

    phy_addr = ldci_read_stt_buf->head_phy_addr;
    ot_ext_system_ldci_read_stt_buffer_low_addr_write(vi_pipe, phy_addr & 0xFFFFFFFF);
    ot_ext_system_ldci_read_stt_buffer_high_addr_write(vi_pipe, phy_addr >> 32); /* shitf right  32 bit  */

    ret = isp_update_ldci_read_stt_buf_ctx(vi_pipe, phy_addr);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 isp_ldci_read_stt_buf_addr_exit(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_ldci_read_stt_buf *ldci_read_stt_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->ldci_tpr_flt_en == TD_FALSE) {
        return TD_SUCCESS;
    }

    ldci_buf_get_ctx(vi_pipe, ldci_read_stt_buf);

    if (ldci_read_stt_buf->read_buf[0].vir_addr != TD_NULL) {
        ot_mpi_sys_munmap(ldci_read_stt_buf->read_buf[0].vir_addr, sizeof(isp_ldci_stat) * ldci_read_stt_buf->buf_num);

        for (i = 0; i < OT_ISP_BE_BUF_NUM_MAX; i++) {
            ldci_read_stt_buf->read_buf[i].vir_addr = TD_NULL;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_ldci_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    ot_isp_ctrl_param isp_ctrl_param = { 0 };
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_GET_CTRL_PARAM, &isp_ctrl_param);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] get ctrlparam failed\n", vi_pipe);
        return ret;
    }

    isp_ctx->ldci_tpr_flt_en = isp_ctrl_param.ldci_tpr_flt_en;

    if ((isp_ctx->ldci_tpr_flt_en == TD_FALSE) && (is_online_mode(isp_ctx->block_attr.running_mode))) {
        return TD_SUCCESS;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_LDCI_BUF_INIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] ldci buffer init failed\n", vi_pipe);
        return TD_FAILURE;
    }

    if (isp_ctx->ldci_tpr_flt_en == TD_TRUE) {
        ret = isp_ldci_read_stt_buf_addr_init(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] ldci read stt buffer address init failed\n", vi_pipe);
            isp_ldci_buf_exit(vi_pipe);
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

td_void isp_ldci_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if ((isp_ctx->ldci_tpr_flt_en == TD_FALSE) && is_online_mode(isp_ctx->block_attr.running_mode)) {
        return;
    }

    if (isp_ctx->ldci_tpr_flt_en == TD_TRUE) {
        ret = isp_ldci_read_stt_buf_addr_exit(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] exit readstt bufaddr failed\n", vi_pipe);
        }
    }

    if (ioctl(isp_get_fd(vi_pipe), ISP_LDCI_BUF_EXIT) != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit ldci bufs failed\n", vi_pipe);
        return;
    }
}

static td_s32 isp_drc_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        return TD_SUCCESS;
    }
    ret = ioctl(isp_get_fd(vi_pipe), ISP_DRC_BUF_INIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] drc buffer init failed\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void isp_drc_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        return;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_DRC_BUF_EXIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit drc bufs failed\n", vi_pipe);
        return;
    }
}

static td_s32 isp_fswdr_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        return TD_SUCCESS;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_FSWDR_BUF_INIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] wdr buffer init failed\n", vi_pipe);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

static td_void isp_fswdr_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        return;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_FSWDR_BUF_EXIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] exit wdr bufs failed\n", vi_pipe);
        return;
    }
}

td_s32 isp_alg_stats_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    ret = isp_ldci_buf_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        goto fail00;
    }

    ret = isp_drc_buf_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        goto fail01;
    }

    ret = isp_fswdr_buf_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        goto fail02;
    }
    return TD_SUCCESS;

fail02:
    isp_drc_buf_exit(vi_pipe);
fail01:
    isp_ldci_buf_exit(vi_pipe);
fail00:
    return OT_ERR_ISP_NOT_INIT;
}

td_void isp_alg_stats_buf_exit(ot_vi_pipe vi_pipe)
{
    isp_fswdr_buf_exit(vi_pipe);
    isp_drc_buf_exit(vi_pipe);
    isp_ldci_buf_exit(vi_pipe);
}

td_s32 isp_cfg_be_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_u32 be_buf_size;
    isp_be_buf *be_buf = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_offline_mode_return(vi_pipe);
    be_reg_get_ctx(vi_pipe, be_buf);
    isp_get_ctx(vi_pipe, isp_ctx);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_CFG_BUF_INIT, &be_buf->be_phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d init be config bufs failed %x!\n", vi_pipe, ret);
        return ret;
    }

    be_buf->be_virt_addr = ot_mpi_sys_mmap_cached(be_buf->be_phy_addr, sizeof(isp_be_wo_reg_cfg) * isp_ctx->be_buf_num);

    if (be_buf->be_virt_addr == TD_NULL) {
        isp_err_trace("Pipe:%d init be config bufs failed!\n", vi_pipe);
        ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_CFG_BUF_EXIT);
        if (ret != TD_SUCCESS) {
            isp_err_trace("Pipe:%d exit be config bufs failed %x!\n", vi_pipe, ret);
            return ret;
        }

        return OT_ERR_ISP_NOMEM;
    }

    be_buf->be_wo_cfg_buf.phy_addr = be_buf->be_phy_addr;

    /* Get be buffer start address & size */
    be_buf_size = sizeof(isp_be_wo_reg_cfg) * isp_ctx->be_buf_num;
    ot_ext_system_be_buffer_address_high_write(vi_pipe, (be_buf->be_phy_addr >> 32)); /* shitf right 32 bit */
    ot_ext_system_be_buffer_address_low_write(vi_pipe, (be_buf->be_phy_addr & 0xFFFFFFFF));
    ot_ext_system_be_buffer_size_write(vi_pipe, be_buf_size);

    return TD_SUCCESS;
}

static td_s8 isp_get_block_id_by_pipe(ot_vi_pipe vi_pipe)
{
    td_s8 block_id = 0;
    switch (vi_pipe) {
        case ISP_BE0_PIPE_ID:
            block_id = 0;
            break;

        case ISP_BE1_PIPE_ID:
            block_id = 1;
            break;

        default:
            return TD_FAILURE;
    }

    return block_id;
}

static td_s32 isp_update_be_buf_addr(ot_vi_pipe vi_pipe, td_void *virt_addr)
{
    td_u16 i;
    td_u64 buf_size;
    td_u8 *head_virt = TD_NULL;
    isp_running_mode isp_runing_mode;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    isp_runing_mode = isp_ctx->block_attr.running_mode;
    buf_size = sizeof(isp_be_wo_reg_cfg) / OT_ISP_STRIPING_MAX_NUM;

    switch (isp_runing_mode) {
        case ISP_MODE_RUNNING_STRIPING:
            for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
                head_virt = (td_u8 *)virt_addr + i * buf_size;
                isp_ctx->isp_pre_be_virt_addr[i] = (td_void *)head_virt;
                isp_ctx->isp_post_be_virt_addr[i] = (td_void *)(head_virt + POST_BE_OFFLINE_OFFSET);
                isp_ctx->pre_viproc_virt_addr[i] = (td_void *)(head_virt + PRE_VIPROC_OFFLINE_OFFSET);
                isp_ctx->post_viproc_virt_addr[i] = (td_void *)(head_virt + POST_VIPROC_OFFLINE_OFFSET);
            }
            break;

        case ISP_MODE_RUNNING_OFFLINE:
            for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
                if (i == 0) {
                    isp_ctx->isp_pre_be_virt_addr[i] = virt_addr;
                    isp_ctx->isp_post_be_virt_addr[i] = (td_void *)((td_u8 *)virt_addr + POST_BE_OFFLINE_OFFSET);
                    isp_ctx->pre_viproc_virt_addr[i] = (td_void *)((td_u8 *)virt_addr + PRE_VIPROC_OFFLINE_OFFSET);
                    isp_ctx->post_viproc_virt_addr[i] = (td_void *)((td_u8 *)virt_addr + POST_VIPROC_OFFLINE_OFFSET);
                } else {
                    isp_ctx->isp_pre_be_virt_addr[i] = TD_NULL;
                    isp_ctx->isp_post_be_virt_addr[i] = TD_NULL;
                    isp_ctx->pre_viproc_virt_addr[i] = TD_NULL;
                    isp_ctx->post_viproc_virt_addr[i] = TD_NULL;
                }
            }
            break;

        case ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE:
            for (i = 0; i < OT_ISP_STRIPING_MAX_NUM; i++) {
                if (i == 0) {
                    isp_ctx->isp_pre_be_virt_addr[i] = vreg_get_virt_addr_base(isp_pre_be_reg_base(i));
                    isp_ctx->isp_post_be_virt_addr[i] = (td_void *)((td_u8 *)virt_addr + POST_BE_OFFLINE_OFFSET);
                    isp_ctx->pre_viproc_virt_addr[i] = vreg_get_virt_addr_base(isp_pre_viproc_reg_base(i));
                    isp_ctx->post_viproc_virt_addr[i] = (td_void *)((td_u8 *)virt_addr + POST_VIPROC_OFFLINE_OFFSET);
                } else {
                    isp_ctx->isp_pre_be_virt_addr[i] = TD_NULL;
                    isp_ctx->isp_post_be_virt_addr[i] = TD_NULL;
                    isp_ctx->pre_viproc_virt_addr[i] = TD_NULL;
                    isp_ctx->post_viproc_virt_addr[i] = TD_NULL;
                }
            }
            break;

        default:
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_s32 isp_cfg_be_buf_mmap(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_u64 be_phy_addr;
    isp_be_buf *be_buf = TD_NULL;

    be_reg_get_ctx(vi_pipe, be_buf);

    be_phy_addr = be_buf->be_wo_cfg_buf.phy_addr;
    ot_ext_system_be_free_buffer_high_addr_write(vi_pipe, (be_phy_addr >> 32)); /* shitf right  32 bit  */
    ot_ext_system_be_free_buffer_low_addr_write(vi_pipe, (be_phy_addr & 0xFFFFFFFF));

    if (be_buf->be_virt_addr != TD_NULL) {
        be_buf->be_wo_cfg_buf.vir_addr =
            (td_void *)((td_u8 *)be_buf->be_virt_addr + (be_buf->be_wo_cfg_buf.phy_addr - be_buf->be_phy_addr));
    } else {
        be_buf->be_wo_cfg_buf.vir_addr = TD_NULL;
    }

    if (be_buf->be_wo_cfg_buf.vir_addr == TD_NULL) {
        return TD_FAILURE;
    }

    ret = isp_update_be_buf_addr(vi_pipe, be_buf->be_wo_cfg_buf.vir_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp update BE bufs failed %x!\n", vi_pipe, ret);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_get_be_buf_first(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_be_buf *be_buf = TD_NULL;

    isp_check_offline_mode_return(vi_pipe);
    be_reg_get_ctx(vi_pipe, be_buf);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_GET_BE_BUF_FIRST, &be_buf->be_wo_cfg_buf.phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d Get be free bufs failed %x!\n", vi_pipe, ret);
        return ret;
    }

    ret = isp_cfg_be_buf_mmap(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp_cfg_be_buf_mmap failed %x!\n", vi_pipe, ret);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_get_be_free_buf(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_be_buf *be_buf = TD_NULL;

    isp_check_offline_mode_return(vi_pipe);
    be_reg_get_ctx(vi_pipe, be_buf);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_FREE_BUF_GET, &be_buf->be_wo_cfg_buf);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_cfg_be_buf_mmap(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp_cfg_be_buf_mmap failed %x!\n", vi_pipe, ret);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_get_be_last_buf(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_be_buf *be_buf = TD_NULL;

    isp_check_offline_mode_return(vi_pipe);
    be_reg_get_ctx(vi_pipe, be_buf);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_LAST_BUF_GET, &be_buf->be_wo_cfg_buf.phy_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d Get be busy bufs failed %x!\n", vi_pipe, ret);
        return ret;
    }

    ret = isp_cfg_be_buf_mmap(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp_cfg_be_buf_mmap failed %x!\n", vi_pipe, ret);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_void isp_cfg_be_buf_exit(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_buf *be_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        return;
    }
    be_reg_get_ctx(vi_pipe, be_buf);

    if (be_buf->be_virt_addr != TD_NULL) {
        ot_mpi_sys_munmap(be_buf->be_virt_addr, sizeof(isp_be_wo_reg_cfg) * isp_ctx->be_buf_num);
        be_buf->be_virt_addr = TD_NULL;
    }

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_CFG_BUF_EXIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d exit be config bufs failed %x!\n", vi_pipe, ret);
        return;
    }
}

static td_s32 isp_cfg_be_buf_ctl(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_be_buf *be_buf = TD_NULL;

    isp_check_offline_mode_return(vi_pipe);
    be_reg_get_ctx(vi_pipe, be_buf);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_CFG_BUF_CTL, &be_buf->be_wo_cfg_buf);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 isp_set_cfg_be_buf_state(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    isp_check_offline_mode_return(vi_pipe);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_CFG_BUF_RUNNING);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

/* init isp be cfgs all buffer */
td_s32 isp_all_cfgs_be_buf_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    isp_check_offline_mode_return(vi_pipe);

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_ALL_BUF_INIT);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] init be all bufs Failed with ec %#x!\n", vi_pipe, ret);
        return ret;
    }

    return TD_SUCCESS;
}

static td_void *isp_vreg_cfg_buf_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev)
{
    td_u64 size;
    td_u64 phy_addr_high;
    td_u64 phy_addr_temp;
    isp_be_buf *be_buf = TD_NULL;

    be_reg_get_ctx(vi_pipe, be_buf);

    size = sizeof(isp_be_wo_reg_cfg) / OT_ISP_STRIPING_MAX_NUM;

    if (be_buf->be_wo_cfg_buf.vir_addr != TD_NULL) {
        return ((td_u8 *)be_buf->be_wo_cfg_buf.vir_addr + blk_dev * size);
    }

    phy_addr_high = (td_u64)ot_ext_system_be_free_buffer_high_addr_read(vi_pipe);
    phy_addr_temp = (td_u64)ot_ext_system_be_free_buffer_low_addr_read(vi_pipe);
    phy_addr_temp |= (phy_addr_high << 32); /* shitf left  32 bit  */

    be_buf->be_wo_cfg_buf.phy_addr = phy_addr_temp;
    be_buf->be_wo_cfg_buf.vir_addr = ot_mpi_sys_mmap_cached(be_buf->be_wo_cfg_buf.phy_addr, sizeof(isp_be_wo_reg_cfg));

    return ((td_u8 *)be_buf->be_wo_cfg_buf.vir_addr + blk_dev * size);
}

static td_s32 isp_get_reg_cfg_virt_addr_online(ot_vi_pipe vi_pipe, td_void *pre_be_virt_addr[],
    td_void *post_be_virt_addr[], td_void *pre_viproc_virt_addr[], td_void *post_viproc_virt_addr[])
{
    td_u8 k;
    td_s8 blk_dev;
    td_u8 block_id;

    blk_dev = isp_get_block_id_by_pipe(vi_pipe);
    if (blk_dev == -1) {
        isp_err_trace("ISP[%d] Online Mode Pipe Err!\n", vi_pipe);
        return TD_FAILURE;
    }

    block_id = (td_u8)blk_dev;
    for (k = 0; k < OT_ISP_STRIPING_MAX_NUM; k++) {
        if (k == 0) {
            pre_be_virt_addr[k] = vreg_get_virt_addr_base(isp_pre_be_reg_base(block_id));
            post_be_virt_addr[k] = vreg_get_virt_addr_base(isp_post_be_reg_base(block_id));
            pre_viproc_virt_addr[k] = vreg_get_virt_addr_base(isp_pre_viproc_reg_base(block_id));
            post_viproc_virt_addr[k] = vreg_get_virt_addr_base(isp_post_viproc_reg_base(block_id));
        } else {
            pre_be_virt_addr[k] = TD_NULL;
            post_be_virt_addr[k] = TD_NULL;
            pre_viproc_virt_addr[k] = TD_NULL;
            post_viproc_virt_addr[k] = TD_NULL;
        }
    }

    return TD_SUCCESS;
}

static td_void isp_get_reg_cfg_virt_addr_offline(ot_vi_pipe vi_pipe, td_void *pre_be_virt_addr[],
    td_void *post_be_virt_addr[], td_void *pre_viproc_virt_addr[], td_void *post_viproc_virt_addr[])
{
    td_u8 k;
    td_void *head_virt = TD_NULL;

    for (k = 0; k < OT_ISP_STRIPING_MAX_NUM; k++) {
        if (k == 0) {
            head_virt = (td_u8 *)isp_vreg_cfg_buf_addr(vi_pipe, (ot_blk_dev)k);
            pre_be_virt_addr[k] = head_virt;
            post_be_virt_addr[k] = (td_void *)((td_u8 *)head_virt + POST_BE_OFFLINE_OFFSET);
            pre_viproc_virt_addr[k] = (td_void *)((td_u8 *)head_virt + PRE_VIPROC_OFFLINE_OFFSET);
            post_viproc_virt_addr[k] = (td_void *)((td_u8 *)head_virt + POST_VIPROC_OFFLINE_OFFSET);
        } else {
            pre_be_virt_addr[k] = TD_NULL;
            post_be_virt_addr[k] = TD_NULL;
            pre_viproc_virt_addr[k] = TD_NULL;
            post_viproc_virt_addr[k] = TD_NULL;
        }
    }
}


static td_s32 isp_get_reg_cfg_virt_addr_vipre_online_viproc_offline(ot_vi_pipe vi_pipe, td_void *pre_be_virt_addr[],
    td_void *post_be_virt_addr[], td_void *pre_viproc_virt_addr[], td_void *post_viproc_virt_addr[])
{
    td_u8 k;
    td_void *head_virt = TD_NULL;

    for (k = 0; k < OT_ISP_STRIPING_MAX_NUM; k++) {
        if (k == 0) {
            head_virt = (td_u8 *)isp_vreg_cfg_buf_addr(vi_pipe, (ot_blk_dev)k);
            pre_be_virt_addr[k] = vreg_get_virt_addr_base(isp_pre_be_reg_base(k));
            post_be_virt_addr[k] = (td_void *)((td_u8 *)head_virt + POST_BE_OFFLINE_OFFSET);
            pre_viproc_virt_addr[k] = vreg_get_virt_addr_base(isp_pre_viproc_reg_base(k));
            post_viproc_virt_addr[k] = (td_void *)((td_u8 *)head_virt + POST_VIPROC_OFFLINE_OFFSET);
        } else {
            pre_be_virt_addr[k] = TD_NULL;
            post_be_virt_addr[k] = TD_NULL;
            pre_viproc_virt_addr[k] = TD_NULL;
            post_viproc_virt_addr[k] = TD_NULL;
        }
    }

    return TD_SUCCESS;
}

static td_void isp_get_reg_cfg_virt_addr_striping(ot_vi_pipe vi_pipe, td_void *pre_be_virt_addr[],
    td_void *post_be_virt_addr[], td_void *pre_viproc_virt_addr[], td_void *post_viproc_virt_addr[])
{
    td_u8 k;
    td_u8 *head_virt = TD_NULL;

    for (k = 0; k < OT_ISP_STRIPING_MAX_NUM; k++) {
        head_virt = (td_u8 *)isp_vreg_cfg_buf_addr(vi_pipe, (ot_blk_dev)k);

        pre_be_virt_addr[k] = (td_void *)head_virt;
        post_be_virt_addr[k] = (td_void *)(head_virt + POST_BE_OFFLINE_OFFSET);
        pre_viproc_virt_addr[k] = (td_void *)(head_virt + PRE_VIPROC_OFFLINE_OFFSET);
        post_viproc_virt_addr[k] = (td_void *)(head_virt + POST_VIPROC_OFFLINE_OFFSET);
    }
}

static td_s32 isp_get_reg_cfg_virt_addr(ot_vi_pipe vi_pipe, td_void *pre_be_virt_addr[], td_void *post_be_virt_addr[],
    td_void *pre_viproc_virt_addr[], td_void *post_viproc_virt_addr[])
{
    td_s32 ret;
    isp_working_mode isp_work_mode;

    ret = ioctl(isp_get_fd(vi_pipe), ISP_WORK_MODE_GET, &isp_work_mode);
    if (ret != TD_SUCCESS) {
        isp_err_trace("get isp work mode failed!\n");
        return ret;
    }

    switch (isp_work_mode.running_mode) {
        case ISP_MODE_RUNNING_ONLINE:
            ret = isp_get_reg_cfg_virt_addr_online(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
                post_viproc_virt_addr);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            break;
        case ISP_MODE_RUNNING_OFFLINE:
            isp_get_reg_cfg_virt_addr_offline(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
                post_viproc_virt_addr);
            break;
        case ISP_MODE_RUNNING_STRIPING:
            isp_get_reg_cfg_virt_addr_striping(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
                post_viproc_virt_addr);
            break;
        case ISP_MODE_RUNNING_PRE_ONLINE_POST_OFFLINE:
            isp_get_reg_cfg_virt_addr_vipre_online_viproc_offline(vi_pipe, pre_be_virt_addr, post_be_virt_addr,
                pre_viproc_virt_addr, post_viproc_virt_addr);
            break;
        default:
            isp_err_trace("ISP[%d] GetBe Running Mode Err!\n", vi_pipe);
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_be_vreg_addr_init(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    return isp_get_reg_cfg_virt_addr(vi_pipe, isp_ctx->isp_pre_be_virt_addr, isp_ctx->isp_post_be_virt_addr,
        isp_ctx->pre_viproc_virt_addr, isp_ctx->post_viproc_virt_addr);
}

static td_void *isp_get_ldci_read_stt_vir_addr(ot_vi_pipe vi_pipe, td_u8 buf_idx)
{
    td_s32 ret;
    td_u64 phy_addr_high, phy_addr_tmp;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_ldci_read_stt_buf *ldci_read_stt_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->ldci_tpr_flt_en == TD_FALSE) {
        return TD_NULL;
    }

    ldci_buf_get_ctx(vi_pipe, ldci_read_stt_buf);

    if (ldci_read_stt_buf->read_buf[buf_idx].vir_addr != TD_NULL) {
        return ldci_read_stt_buf->read_buf[buf_idx].vir_addr;
    }

    phy_addr_high = (td_u64)ot_ext_system_ldci_read_stt_buffer_high_addr_read(vi_pipe);
    phy_addr_tmp = (td_u64)ot_ext_system_ldci_read_stt_buffer_low_addr_read(vi_pipe);
    phy_addr_tmp |= (phy_addr_high << 32); /* shitf left  32 bit  */

    ret = isp_update_ldci_read_stt_buf_ctx(vi_pipe, phy_addr_tmp);
    if (ret != TD_SUCCESS) {
        return TD_NULL;
    }

    return ldci_read_stt_buf->read_buf[buf_idx].vir_addr;
}

td_void *isp_get_fe_vir_addr(ot_vi_pipe vi_pipe)
{
    isp_check_fe_pipe_return(vi_pipe);

    return vreg_get_virt_addr_base(isp_fe_reg_base(vi_pipe));
}

static td_void *isp_get_be_lut2stt_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev, td_u8 buf_id)
{
    td_s32 ret;
    td_u64 size;
    td_u64 phy_addr_high, phy_addr_temp;
    isp_be_lut_buf *be_lut_buf = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);
    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        return TD_NULL;
    }

    size = sizeof(isp_be_lut_wstt_type);

    if (be_lut_buf->lut_stt_buf[blk_dev].vir_addr != TD_NULL) {
        return (td_void *)((td_u8 *)be_lut_buf->lut_stt_buf[blk_dev].vir_addr + size * buf_id);
    }

    phy_addr_high = (td_u64)ot_ext_system_be_lut_stt_buffer_high_addr_read(vi_pipe);
    phy_addr_temp = (td_u64)ot_ext_system_be_lut_stt_buffer_low_addr_read(vi_pipe);
    phy_addr_temp |= (phy_addr_high << 32); /* shitf left  32 bit  */

    ret = isp_update_be_lut_stt_buf_ctx(vi_pipe, phy_addr_temp);
    if (ret != TD_SUCCESS) {
        return TD_NULL;
    }

    return (td_void *)((td_u8 *)be_lut_buf->lut_stt_buf[blk_dev].vir_addr + size * buf_id);
}


static td_void *isp_get_be_pre_lut2stt_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev, td_u8 buf_id)
{
    td_s32 ret;
    td_u64 size;
    td_u64 phy_addr_high, phy_addr_temp;
    isp_be_lut_buf *be_lut_buf = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);
    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return TD_NULL;
    }

    size = sizeof(isp_be_lut_wstt_type);

    if (be_lut_buf->lut_stt_buf[blk_dev].vir_addr != TD_NULL) {
        return (td_void *)((td_u8 *)be_lut_buf->lut_stt_buf[blk_dev].vir_addr + size * buf_id);
    }

    phy_addr_high = (td_u64)ot_ext_system_be_lut_stt_buffer_high_addr_read(vi_pipe);
    phy_addr_temp = (td_u64)ot_ext_system_be_lut_stt_buffer_low_addr_read(vi_pipe);
    phy_addr_temp |= (phy_addr_high << 32); /* shitf left  32 bit  */

    ret = isp_update_be_pre_lut_stt_buf_ctx(vi_pipe, phy_addr_temp);
    if (ret != TD_SUCCESS) {
        return TD_NULL;
    }

    return (td_void *)((td_u8 *)be_lut_buf->lut_stt_buf[blk_dev].vir_addr + size * buf_id);
}

static td_void *isp_get_pre_on_post_off_lut2stt_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev, td_u8 buf_id)
{
    td_s32 ret;
    td_u64 size;
    td_u64 phy_addr_high, phy_addr_temp;
    isp_be_lut_buf *be_lut_buf = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);
    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return TD_NULL;
    }

    size = sizeof(isp_pre_be_lut_wstt_type);

    if (be_lut_buf->lut_stt_buf[blk_dev].vir_addr != TD_NULL) {
        return (td_void *)((td_u8 *)be_lut_buf->lut_stt_buf[blk_dev].vir_addr + size * buf_id);
    }

    phy_addr_high = (td_u64)ot_ext_system_be_lut_stt_buffer_high_addr_read(vi_pipe);
    phy_addr_temp = (td_u64)ot_ext_system_be_lut_stt_buffer_low_addr_read(vi_pipe);
    phy_addr_temp |= (phy_addr_high << 32); /* shitf left  32 bit  */

    ret = isp_update_be_pre_online_post_offline_lut_stt_buf_ctx(vi_pipe, phy_addr_temp);
    if (ret != TD_SUCCESS) {
        return TD_NULL;
    }

    return (td_void *)((td_u8 *)be_lut_buf->lut_stt_buf[blk_dev].vir_addr + size * buf_id);
}

static td_void *isp_get_post_be_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_void *pre_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *pre_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->isp_post_be_virt_addr[blk_dev] != TD_NULL) {
        return isp_ctx->isp_post_be_virt_addr[blk_dev];
    }

    ret = isp_get_reg_cfg_virt_addr(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
        post_viproc_virt_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Get Be CfgAddr Failed!\n", vi_pipe);
        return TD_NULL;
    }

    isp_check_reg_nullptr_return(post_be_virt_addr[blk_dev]);

    return post_be_virt_addr[blk_dev];
}

td_void *isp_get_pre_be_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_void *pre_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *pre_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->isp_pre_be_virt_addr[blk_dev] != TD_NULL) {
        return isp_ctx->isp_pre_be_virt_addr[blk_dev];
    }

    ret = isp_get_reg_cfg_virt_addr(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
        post_viproc_virt_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Get Be CfgAddr Failed!\n", vi_pipe);
        return TD_NULL;
    }

    isp_check_reg_nullptr_return(pre_be_virt_addr[blk_dev]);

    return pre_be_virt_addr[blk_dev];
}

static td_void *isp_get_pre_vi_proc_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_void *pre_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *pre_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->pre_viproc_virt_addr[blk_dev] != TD_NULL) {
        return isp_ctx->pre_viproc_virt_addr[blk_dev];
    }

    ret = isp_get_reg_cfg_virt_addr(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
        post_viproc_virt_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Get viproc CfgAddr Failed!\n", vi_pipe);
        return TD_NULL;
    }

    isp_check_reg_nullptr_return(pre_viproc_virt_addr[blk_dev]);

    return pre_viproc_virt_addr[blk_dev];
}

static td_void *isp_get_post_vi_proc_vir_addr(ot_vi_pipe vi_pipe, ot_blk_dev blk_dev)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_void *pre_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_be_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *pre_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };
    td_void *post_viproc_virt_addr[OT_ISP_STRIPING_MAX_NUM] = { TD_NULL };

    isp_check_fe_pipe_return(vi_pipe);
    isp_check_be_dev_return(blk_dev);
    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->post_viproc_virt_addr[blk_dev] != TD_NULL) {
        return isp_ctx->post_viproc_virt_addr[blk_dev];
    }

    ret = isp_get_reg_cfg_virt_addr(vi_pipe, pre_be_virt_addr, post_be_virt_addr, pre_viproc_virt_addr,
        post_viproc_virt_addr);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Get viproc CfgAddr Failed!\n", vi_pipe);
        return TD_NULL;
    }

    isp_check_reg_nullptr_return(post_viproc_virt_addr[blk_dev]);

    return post_viproc_virt_addr[blk_dev];
}

static td_s32 isp_clut_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;
    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    offset = POST_BE_SWTT_OFFSET + ISP_CLUT_LUT_WSTT_OFFSET;
    isp_clut_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + offset + buf_id * size));
    isp_clut_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + offset + buf_id * size));
    isp_clut_lut_width_word_write(vi_proc_reg, OT_ISP_CLUT_LUT_WIDTH_WORD_DEFAULT);

    return TD_SUCCESS;
}

static td_s32 isp_sharpen_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id,
    isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, addr_offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    addr_offset = POST_BE_SWTT_OFFSET + ISP_SHARPEN_LUT_WSTT_OFFSET + buf_id * size;
    isp_sharpen_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + addr_offset));
    isp_sharpen_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + addr_offset));
    return TD_SUCCESS;
}

static td_s32 isp_dehaze_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id,
    isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, addr_offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    addr_offset = POST_BE_SWTT_OFFSET + ISP_DEHAZE_LUT_WSTT_OFFSET + buf_id * size;
    isp_dehaze_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + addr_offset));
    isp_dehaze_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + addr_offset));
    return TD_SUCCESS;
}

static td_s32 isp_gamma_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, addr_offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    addr_offset = POST_BE_SWTT_OFFSET + ISP_GAMMA_LUT_WSTT_OFFSET + buf_id * size;
    isp_gamma_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + addr_offset));
    isp_gamma_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + addr_offset));
    return TD_SUCCESS;
}

/* pre be */
static td_s32 isp_crb_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    isp_crb_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + ISP_CRB_LUT_WSTT_OFFSET + buf_id * size));
    isp_crb_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + ISP_CRB_LUT_WSTT_OFFSET + buf_id * size));
    return TD_SUCCESS;
}

static td_s32 isp_bnr_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, addr_offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    addr_offset = POST_BE_SWTT_OFFSET + ISP_BNR_LUT_WSTT_OFFSET + buf_id * size;
    isp_bnr_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + addr_offset));
    isp_bnr_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + addr_offset));
    return TD_SUCCESS;
}

static td_s32 isp_bshp_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    offset = POST_BE_SWTT_OFFSET + ISP_BNR_SHP_LUT_WSTT_OFFSET;
    isp_bshp_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + offset + buf_id * size));
    isp_bshp_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + offset + buf_id * size));

    return TD_SUCCESS;
}

static td_s32 isp_degammafe_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id,
    isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    offset = POST_BE_SWTT_OFFSET + ISP_DEGAMMAFE_LUT_WSTT_OFFSET + buf_id * size;
    isp_degammafe_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + offset));
    isp_degammafe_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + offset));
    return TD_SUCCESS;
}

static td_s32 isp_pregammafe_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id,
    isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    offset = ISP_PREGAMMAFE_LUT_WSTT_OFFSET + buf_id * size;
    isp_pregammafe_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + offset));
    isp_pregammafe_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + offset));
    return TD_SUCCESS;
}
static td_s32 isp_dpc_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *pre_viproc)
{
    td_u64 phy_addr, size;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    isp_dcg_lut_addr_low_write(pre_viproc, get_low_addr(phy_addr + ISP_DPC_LUT_WSTT_OFFSET + buf_id * size));
    isp_dcg_lut_addr_high_write(pre_viproc, get_high_addr(phy_addr + ISP_DPC_LUT_WSTT_OFFSET + buf_id * size));

    return TD_SUCCESS;
}

static td_s32 isp_dpc1_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *pre_viproc)
{
    td_u64 phy_addr, size;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    isp_dcg1_lut_addr_low_write(pre_viproc, get_low_addr(phy_addr + ISP_DPC1_LUT_WSTT_OFFSET + buf_id * size));
    isp_dcg1_lut_addr_high_write(pre_viproc, get_high_addr(phy_addr + ISP_DPC1_LUT_WSTT_OFFSET + buf_id * size));

    return TD_SUCCESS;
}

static td_s32 isp_lsc_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    offset = POST_BE_SWTT_OFFSET + ISP_LSC_LUT_WSTT_OFFSET;
    isp_lsc_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + offset + buf_id * size));
    isp_lsc_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + offset + buf_id * size));

    return TD_SUCCESS;
}

static td_s32 isp_ldci_lut_wstt_addr_write(ot_vi_pipe vi_pipe, td_u8 i, td_u8 buf_id, isp_viproc_reg_type *vi_proc_reg)
{
    td_u64 phy_addr, size, addr_offset;
    isp_be_lut_buf *be_lut_buf = TD_NULL;

    be_lut_buf_get_ctx(vi_pipe, be_lut_buf);

    phy_addr = be_lut_buf->lut_stt_buf[i].phy_addr;

    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    size = sizeof(isp_be_lut_wstt_type);
    addr_offset = POST_BE_SWTT_OFFSET + ISP_LDCI_LUT_WSTT_OFFSET + buf_id * size;
    isp_ldci_lut_addr_low_write(vi_proc_reg, get_low_addr(phy_addr + addr_offset));
    isp_ldci_lut_addr_high_write(vi_proc_reg, get_high_addr(phy_addr + addr_offset));

    return TD_SUCCESS;
}

static td_s32 isp_fe_dg_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u16 i;
    ot_vi_pipe vi_pipe_bind;
    isp_fe_dg_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    isp_check_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);

    dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[0].fe_dg_reg_cfg.dyna_reg_cfg;

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
        isp_check_vir_pipe_return(vi_pipe_bind);
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);

        if (reg_cfg_info->cfg_key.bit1_fe_dg_cfg) {
            isp_fe_dg_en_write(fe_reg, reg_cfg_info->alg_reg_cfg[0].fe_dg_reg_cfg.dg_en);

            if (dyna_reg_cfg->resh) {
                isp_fe_dg_gain_write(fe_reg, dyna_reg_cfg->gain_r[i], dyna_reg_cfg->gain_gr[i],
                    dyna_reg_cfg->gain_gb[i], dyna_reg_cfg->gain_b[i]);
                isp_fe_dg_clip_value_write(fe_reg, dyna_reg_cfg->clip_value);
            }
        }
    }

    dyna_reg_cfg->resh = TD_FALSE;
    reg_cfg_info->cfg_key.bit1_fe_dg_cfg = 0;

    return TD_SUCCESS;
}
static td_void isp_agamma_seg_reg_config(isp_pre_be_reg_type *pre_be_reg, isp_agamma_static_cfg *static_reg_cfg)
{
    isp_pregammafe_idxbase0_write(pre_be_reg, static_reg_cfg->seg_idx_base[0]); /* idxbase[0] */
    isp_pregammafe_idxbase1_write(pre_be_reg, static_reg_cfg->seg_idx_base[1]); /* idxbase[1] */
    isp_pregammafe_idxbase2_write(pre_be_reg, static_reg_cfg->seg_idx_base[2]); /* idxbase[2] */
    isp_pregammafe_idxbase3_write(pre_be_reg, static_reg_cfg->seg_idx_base[3]); /* idxbase[3] */
    isp_pregammafe_idxbase4_write(pre_be_reg, static_reg_cfg->seg_idx_base[4]); /* idxbase[4] */
    isp_pregammafe_idxbase5_write(pre_be_reg, static_reg_cfg->seg_idx_base[5]); /* idxbase[5] */
    isp_pregammafe_idxbase6_write(pre_be_reg, static_reg_cfg->seg_idx_base[6]); /* idxbase[6] */
    isp_pregammafe_idxbase7_write(pre_be_reg, static_reg_cfg->seg_idx_base[7]); /* idxbase[7] */

    isp_pregammafe_maxval0_write(pre_be_reg, static_reg_cfg->seg_max_val[0]); /* maxval[0] */
    isp_pregammafe_maxval1_write(pre_be_reg, static_reg_cfg->seg_max_val[1]); /* maxval[1] */
    isp_pregammafe_maxval2_write(pre_be_reg, static_reg_cfg->seg_max_val[2]); /* maxval[2] */
    isp_pregammafe_maxval3_write(pre_be_reg, static_reg_cfg->seg_max_val[3]); /* maxval[3] */
    isp_pregammafe_maxval4_write(pre_be_reg, static_reg_cfg->seg_max_val[4]); /* maxval[4] */
    isp_pregammafe_maxval5_write(pre_be_reg, static_reg_cfg->seg_max_val[5]); /* maxval[5] */
    isp_pregammafe_maxval6_write(pre_be_reg, static_reg_cfg->seg_max_val[6]); /* maxval[6] */
    isp_pregammafe_maxval7_write(pre_be_reg, static_reg_cfg->seg_max_val[7]); /* maxval[7] */
}
static td_s32 isp_agamma_lut_reg_config(ot_vi_pipe vi_pipe, isp_pre_be_reg_type *pre_be_reg,
    isp_agamma_dyna_cfg *dyna_reg_cfg, td_u8 i)
{
    td_u8 buf_id = 0;
    td_s32 ret;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_pre_be_lut_wstt_type *pre_lut_stt_reg = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(pre_viproc);
    isp_pregammafe_lut_width_word_write(pre_viproc, OT_ISP_PREGAMMAFE_LUT_WIDTH_WORD_DEFAULT);

    if (is_online_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        buf_id = dyna_reg_cfg->buf_id;
        if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
            pre_lut_stt_reg = (isp_pre_be_lut_wstt_type *)isp_get_pre_on_post_off_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(pre_lut_stt_reg);
            isp_pregammafe_lut_wstt_write(pre_lut_stt_reg, dyna_reg_cfg->agamma_lut);
        } else {
            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_pre_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(be_lut_stt_reg);
            isp_pregammafe_lut_wstt_write(&be_lut_stt_reg->pre_be_lut2stt, dyna_reg_cfg->agamma_lut);
        }

        ret = isp_pregammafe_lut_wstt_addr_write(vi_pipe, i, buf_id, pre_viproc);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] isp_pregammafe_lut_wstt_addr_write failed\n", vi_pipe);
            return ret;
        }
        dyna_reg_cfg->buf_id = 1 - buf_id;
    } else {
        isp_pregammafe_lut_wstt_write(&pre_be_reg->pre_be_lut.pre_be_lut2stt, dyna_reg_cfg->agamma_lut);
    }
    isp_pregammafe_stt2lut_en_write(pre_be_reg, TD_TRUE);
    isp_pregammafe_stt2lut_regnew_write(pre_be_reg, TD_TRUE);

    return TD_SUCCESS;
}

static td_s32 isp_agamma_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_agamma_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_agamma_static_cfg *static_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    td_bool offline_mode;
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_agamma_cfg) {
        pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(pre_be_reg);
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(pre_viproc);

        isp_pregammafe_en_write(pre_viproc, reg_cfg_info->alg_reg_cfg[i].agamma_reg_cfg.enable);

        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].agamma_reg_cfg.dyna_reg_cfg;
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].agamma_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->resh) {
            isp_agamma_seg_reg_config(pre_be_reg, static_reg_cfg);
            static_reg_cfg->resh = TD_FALSE;
        }

        if (dyna_reg_cfg->resh) {
            isp_agamma_lut_reg_config(vi_pipe, pre_be_reg, dyna_reg_cfg, i);
            dyna_reg_cfg->resh = offline_mode;
        }
        reg_cfg_info->cfg_key.bit1_agamma_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}
static td_void isp_adgamma_seg_reg_config(isp_post_be_reg_type *post_be_reg, isp_adgamma_static_cfg *static_reg_cfg)
{
    isp_degammafe_idxbase0_write(post_be_reg, static_reg_cfg->seg_idx_base[0]); /* idxbase[0] */
    isp_degammafe_idxbase1_write(post_be_reg, static_reg_cfg->seg_idx_base[1]); /* idxbase[1] */
    isp_degammafe_idxbase2_write(post_be_reg, static_reg_cfg->seg_idx_base[2]); /* idxbase[2] */
    isp_degammafe_idxbase3_write(post_be_reg, static_reg_cfg->seg_idx_base[3]); /* idxbase[3] */
    isp_degammafe_idxbase4_write(post_be_reg, static_reg_cfg->seg_idx_base[4]); /* idxbase[4] */
    isp_degammafe_idxbase5_write(post_be_reg, static_reg_cfg->seg_idx_base[5]); /* idxbase[5] */
    isp_degammafe_idxbase6_write(post_be_reg, static_reg_cfg->seg_idx_base[6]); /* idxbase[6] */
    isp_degammafe_idxbase7_write(post_be_reg, static_reg_cfg->seg_idx_base[7]); /* idxbase[7] */

    isp_degammafe_maxval0_write(post_be_reg, static_reg_cfg->seg_max_val[0]); /* maxval[0] */
    isp_degammafe_maxval1_write(post_be_reg, static_reg_cfg->seg_max_val[1]); /* maxval[1] */
    isp_degammafe_maxval2_write(post_be_reg, static_reg_cfg->seg_max_val[2]); /* maxval[2] */
    isp_degammafe_maxval3_write(post_be_reg, static_reg_cfg->seg_max_val[3]); /* maxval[3] */
    isp_degammafe_maxval4_write(post_be_reg, static_reg_cfg->seg_max_val[4]); /* maxval[4] */
    isp_degammafe_maxval5_write(post_be_reg, static_reg_cfg->seg_max_val[5]); /* maxval[5] */
    isp_degammafe_maxval6_write(post_be_reg, static_reg_cfg->seg_max_val[6]); /* maxval[6] */
    isp_degammafe_maxval7_write(post_be_reg, static_reg_cfg->seg_max_val[7]); /* maxval[7] */
}
static td_s32 isp_adgamma_lut_reg_config(ot_vi_pipe vi_pipe, isp_post_be_reg_type *post_be_reg,
    isp_adgamma_dyna_cfg *dyna_reg_cfg, td_u8 i)
{
    td_u8 buf_id = 0;
    td_s32 ret;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_viproc);
    isp_degammafe_lut_width_word_write(post_viproc, OT_ISP_DEGAMMAFE_LUT_WIDTH_WORD_DEFAULT);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        buf_id = dyna_reg_cfg->buf_id;

        be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_return(be_lut_stt_reg);

        isp_degammafe_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, dyna_reg_cfg->adgamma_lut);
        ret = isp_degammafe_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] isp_degammafe_lut_wstt_addr_write failed\n", vi_pipe);
            return ret;
        }
        dyna_reg_cfg->buf_id = 1 - buf_id;
    } else {
        isp_degammafe_lut_wstt_write(&post_be_reg->post_be_lut.post_be_lut2stt, dyna_reg_cfg->adgamma_lut);
    }
    isp_degammafe_stt2lut_en_write(post_be_reg, TD_TRUE);
    isp_degammafe_stt2lut_regnew_write(post_be_reg, TD_TRUE);

    return TD_SUCCESS;
}

static td_void isp_adgamma_reg_upate_online(td_bool offline_mode, isp_post_be_reg_type *post_be_reg,
    isp_adgamma_dyna_cfg *dyna_reg_cfg)
{
    td_u32 degammafe_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        degammafe_stt2lut_clr = isp_degammafe_stt2lut_clr_read(post_be_reg);
        if (degammafe_stt2lut_clr != 0) {
            dyna_reg_cfg->resh = TD_TRUE;
            isp_degammafe_stt2lut_clr_write(post_be_reg, 1);
        }
    }
}

static td_s32 isp_adgamma_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_adgamma_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_adgamma_static_cfg *static_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    td_bool offline_mode;

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_adgamma_cfg) {
        post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(post_be_reg);

        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(post_viproc);

        isp_degammafe_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].adgamma_reg_cfg.enable);

        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].adgamma_reg_cfg.dyna_reg_cfg;
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].adgamma_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->resh) {
            isp_adgamma_seg_reg_config(post_be_reg, static_reg_cfg);
            static_reg_cfg->resh = TD_FALSE;
        }

        if (dyna_reg_cfg->resh) {
            isp_adgamma_lut_reg_config(vi_pipe, post_be_reg, dyna_reg_cfg, i);
            dyna_reg_cfg->resh = offline_mode;
        }

        isp_adgamma_reg_upate_online(offline_mode, post_be_reg, dyna_reg_cfg);
        reg_cfg_info->cfg_key.bit1_adgamma_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_void isp_fe_blc_static_reg_config(ot_vi_pipe vi_pipe_bind, isp_fe_reg_type *fe_reg,
                                            const isp_fe_blc_static_cfg *static_cfg)
{
    td_u8  af_en[OT_ISP_MAX_PHY_PIPE_NUM] = { ISP_PIPE_FEAF_MODULE_ENABLE };
    if (static_cfg->resh_static) {
        isp_fe_dg_blc_en_write(fe_reg, static_cfg->fe_dg_blc.blc_in, static_cfg->fe_dg_blc.blc_out);
        isp_fe_wb_blc_en_write(fe_reg, static_cfg->fe_wb_blc.blc_in, static_cfg->fe_wb_blc.blc_out);
        isp_fe_ae_blc_en_write(fe_reg, static_cfg->fe_ae_blc.blc_in);
        isp_fe_blc1_en_write(fe_reg, static_cfg->fe_blc.blc_in);
        if (af_en[vi_pipe_bind] == 1) {
            isp_fe_af_offset_en_write(fe_reg, static_cfg->fe_af_blc.blc_in);
        }
    }
}

static td_void isp_fe_blc_dyna_reg_config(ot_vi_pipe vi_pipe_bind, td_u8 wdr_idx, isp_fe_reg_type *fe_reg,
                                          const isp_fe_blc_dyna_cfg *dyna_cfg)
{
    td_u8  af_en[OT_ISP_MAX_PHY_PIPE_NUM] = { ISP_PIPE_FEAF_MODULE_ENABLE };
    isp_fe_dg_offset_write(fe_reg, dyna_cfg->fe_dg_blc[wdr_idx].blc);
    isp_fe_wb_offset_write(fe_reg, dyna_cfg->fe_wb_blc[wdr_idx].blc);
    isp_fe_ae_offset_write(fe_reg, dyna_cfg->fe_ae_blc[wdr_idx].blc);
    isp_fe_blc1_offset_write(fe_reg, dyna_cfg->fe_blc[wdr_idx].blc);
    if (af_en[vi_pipe_bind] == 1) {
        isp_fe_af_offset_write(fe_reg, dyna_cfg->fe_af_blc[wdr_idx].blc);
    }
}

static td_s32 isp_fe_blc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u16 i;
    ot_vi_pipe vi_pipe_bind;
    isp_fe_blc_static_cfg *static_cfg = TD_NULL;
    isp_fe_blc_dyna_cfg *dyna_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    static_cfg = &reg_cfg_info->alg_reg_cfg[0].fe_blc_cfg.static_blc;
    dyna_cfg = &reg_cfg_info->alg_reg_cfg[0].fe_blc_cfg.dyna_blc;

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
        isp_check_vir_pipe_return(vi_pipe_bind);
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);

        if (reg_cfg_info->cfg_key.bit1_fe_blc_cfg) {
            isp_fe_blc_static_reg_config(vi_pipe_bind, fe_reg, static_cfg);

            if (reg_cfg_info->alg_reg_cfg[0].fe_blc_cfg.resh_dyna_init == TD_TRUE) {
                isp_fe_blc_dyna_reg_config(vi_pipe_bind, i, fe_reg, dyna_cfg);
            }
        }
    }

    static_cfg->resh_static = TD_FALSE;
    reg_cfg_info->alg_reg_cfg[0].fe_blc_cfg.resh_dyna_init = TD_FALSE;
    reg_cfg_info->cfg_key.bit1_fe_blc_cfg = 0;

    return TD_SUCCESS;
}

static td_s32 isp_dynamic_blc_reg_config_all_wdr_pipe(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u16 i;
    ot_vi_pipe vi_pipe_bind;
    isp_dynamicblc_usr_cfg *usr_cfg = TD_NULL;
    isp_dynamicblc_static_cfg *static_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    static_cfg = &reg_cfg_info->alg_reg_cfg[0].dynamic_blc_reg_cfg.static_reg_cfg;
    usr_cfg = &reg_cfg_info->alg_reg_cfg[0].dynamic_blc_reg_cfg.usr_reg_cfg;

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
        isp_check_vir_pipe_return(vi_pipe_bind);
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);

        isp_fe_blc_dyn_en_write(fe_reg, reg_cfg_info->alg_reg_cfg[0].dynamic_blc_reg_cfg.dynablc_enable);

        if (reg_cfg_info->cfg_key.bit1_dynablc_cfg) {
            if (static_cfg->static_resh) {
                isp_fe_blc_dynamic_raw_array_write(fe_reg, static_cfg->dynablc_raw_array);
                isp_fe_blc_dynamic_dp_stat_en_write(fe_reg, static_cfg->dynablc_dpstat_en);
            }

            if (usr_cfg->usr_resh) {
                isp_fe_blc_dynamic_bis2x2pattern_write(fe_reg, usr_cfg->dynablc_pattern);
                isp_fe_blc_dynamic_start_xpos_write(fe_reg, usr_cfg->dynablc_start_xpos);
                isp_fe_blc_dynamic_start_ypos_write(fe_reg, usr_cfg->dynablc_start_ypos);
                isp_fe_blc_dynamic_end_xpos_write(fe_reg, usr_cfg->dynablc_end_xpos);
                isp_fe_blc_dynamic_end_ypos_write(fe_reg, usr_cfg->dynablc_end_ypos);
                isp_fe_blc_dynamic_hot_pxl_thd_write(fe_reg, usr_cfg->dynablc_high_threshold);
                isp_fe_blc_dynamic_dead_pxl_thd_write(fe_reg, usr_cfg->dynablc_low_threshold);
            }
        }
    }

    static_cfg->static_resh = TD_FALSE;
    usr_cfg->usr_resh = TD_FALSE;
    reg_cfg_info->cfg_key.bit1_dynablc_cfg = 0;

    return TD_SUCCESS;
}

static td_s32 isp_stitch_dynamic_blc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u8 i;
    td_bool all_init = TD_TRUE;
    ot_vi_pipe stitch_pipe;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_usr_ctx *isp_stitch_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg_attr = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    for (i = 0; i < isp_ctx->stitch_attr.stitch_pipe_num; i++) {
        stitch_pipe = isp_ctx->stitch_attr.stitch_bind_id[i];
        isp_check_pipe_return(stitch_pipe);
        isp_get_ctx(stitch_pipe, isp_stitch_ctx);
        if (isp_stitch_ctx->para_rec.init != TD_TRUE) {
            all_init = TD_FALSE;
            break;
        }
    }

    if (all_init == TD_TRUE) {
        if (isp_ctx->stitch_attr.main_pipe == TD_TRUE) {
            for (i = 0; i < isp_ctx->stitch_attr.stitch_pipe_num; i++) {
                stitch_pipe = isp_ctx->stitch_attr.stitch_bind_id[i];
                isp_check_pipe_return(stitch_pipe);
                isp_regcfg_get_ctx(stitch_pipe, reg_cfg_attr);
                isp_check_pointer_return(reg_cfg_attr);
                isp_dynamic_blc_reg_config_all_wdr_pipe(stitch_pipe, &reg_cfg_attr->reg_cfg);
            }
        }
    } else {
        isp_dynamic_blc_reg_config_all_wdr_pipe(vi_pipe, reg_cfg_info);
    }

    return TD_SUCCESS;
}

static td_s32 isp_fe_dynamicblc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    if (isp_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        return isp_stitch_dynamic_blc_reg_config(vi_pipe, reg_cfg_info);
    } else {
        return isp_dynamic_blc_reg_config_all_wdr_pipe(vi_pipe, reg_cfg_info);
    }
}

static td_void isp_fe_ae_weight_config(isp_fe_reg_type *fe_reg, isp_ae_dyna_cfg *dyna_reg_cfg)
{
    td_u16 j, k;
    td_u32 table_weight_tmp;
    td_u32 combin_weight = 0;
    td_u32 combin_weight_num = 0;

    isp_fe_ae_wei_waddr_write(fe_reg, 0);

    for (j = 0; j < OT_ISP_AE_ZONE_ROW; j++) {
        for (k = 0; k < OT_ISP_AE_ZONE_COLUMN; k++) {
            table_weight_tmp = (td_u32)dyna_reg_cfg->fe_weight_table[j][k];
            combin_weight |= (table_weight_tmp << (8 * combin_weight_num)); /* weightTmp shift left 8 */
            combin_weight_num++;

            if (combin_weight_num == OT_ISP_AE_WEI_COMBIN_COUNT) {
                isp_fe_ae_wei_wdata_write(fe_reg, combin_weight);
                combin_weight_num = 0;
                combin_weight = 0;
            }
        }
    }

    if ((combin_weight_num != OT_ISP_AE_WEI_COMBIN_COUNT) && (combin_weight_num != 0)) {
        isp_fe_ae_wei_wdata_write(fe_reg, combin_weight);
    }
}

static td_s32 isp_fe_hrs_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    isp_hrs_static_cfg *hrs_static_reg_cfg = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    hrs_static_reg_cfg = &reg_cfg_info->alg_reg_cfg[0].hrs_reg_cfg.static_reg_cfg;

    if (reg_cfg_info->cfg_key.bit1_hrs_cfg) {
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(HRS_PIPE);
        isp_check_pointer_return(fe_reg);
        hrs_static_reg_cfg->rs_enable = (is_hrs_on(HRS_PIPE) ? TD_TRUE : TD_FALSE);
        isp_hrs_ds_en_write(fe_reg, hrs_static_reg_cfg->rs_enable);
        if (hrs_static_reg_cfg->resh) {
            isp_fe_hrs_en_write(fe_reg, hrs_static_reg_cfg->enable);
            isp_hrs_filterlut0_0_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut0[0]); /* array index 0 */
            isp_hrs_filterlut1_0_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut0[1]); /* array index 1 */
            isp_hrs_filterlut2_0_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut0[2]); /* array index 2 */
            isp_hrs_filterlut3_0_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut0[3]); /* array index 3 */
            isp_hrs_filterlut4_0_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut0[4]); /* array index 4 */
            isp_hrs_filterlut5_0_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut0[5]); /* array index 5 */
            isp_hrs_filterlut0_1_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut1[0]); /* array index 0 */
            isp_hrs_filterlut1_1_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut1[1]); /* array index 1 */
            isp_hrs_filterlut2_1_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut1[2]); /* array index 2 */
            isp_hrs_filterlut3_1_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut1[3]); /* array index 3 */
            isp_hrs_filterlut4_1_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut1[4]); /* array index 4 */
            isp_hrs_filterlut5_1_write(fe_reg, hrs_static_reg_cfg->hrs_filter_lut1[5]); /* array index 5 */
            hrs_static_reg_cfg->resh = TD_FALSE;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_fe_ae_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_bool lut_update_en = TD_FALSE;
    td_u16 i, crop_width, crop_x;
    ot_vi_pipe vi_pipe_bind;
    isp_ae_static_cfg *static_reg = TD_NULL;
    isp_ae_dyna_cfg *dyna_reg = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_bool fe_enable = TD_FALSE;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    if (reg_cfg_info->cfg_key.bit1_ae_cfg1) {
        for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
            vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
            isp_check_vir_pipe_return(vi_pipe_bind);

            fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
            isp_check_pointer_return(fe_reg);
            /* ae fe static */
            static_reg = &reg_cfg_info->alg_reg_cfg[0].ae_reg_cfg.static_reg_cfg;
            dyna_reg = &reg_cfg_info->alg_reg_cfg[0].ae_reg_cfg.dyna_reg_cfg;
            fe_enable = static_reg->fe_enable;

            if ((vi_pipe_bind != 0) &&
                ((static_reg->fe_crop_out_width > AE_RES_LIMIT) || (static_reg->fe_crop_out_height > AE_RES_LIMIT))) {
                fe_enable = 0;
            }

            isp_fe_ae_en_write(fe_reg, fe_enable);
            crop_x = is_hrs_on(vi_pipe_bind) ? (static_reg->fe_crop_pos_x >> 1) : static_reg->fe_crop_pos_x;
            isp_fe_ae_crop_pos_write(fe_reg, crop_x, static_reg->fe_crop_pos_y);
            crop_width = is_hrs_on(vi_pipe_bind) ? (static_reg->fe_crop_out_width >> 1) : static_reg->fe_crop_out_width;
            isp_fe_ae_crop_outsize_write(fe_reg, crop_width - 1, static_reg->fe_crop_out_height - 1);

            /* ae fe dynamic */
            isp_fe_ae_zone_write(fe_reg, dyna_reg->fe_weight_table_width, dyna_reg->fe_weight_table_height);
            isp_fe_ae_skip_crg_write(fe_reg, dyna_reg->fe_hist_skip_x, dyna_reg->fe_hist_offset_x,
                dyna_reg->fe_hist_skip_y, dyna_reg->fe_hist_offset_y);
            isp_fe_ae_bitmove_write(fe_reg, dyna_reg->fe_bit_move, dyna_reg->fe_hist_gamma_mode,
                dyna_reg->fe_aver_gamma_mode);
            isp_fe_ae_gamma_limit_write(fe_reg, dyna_reg->fe_gamma_limit);
            isp_fe_ae_fourplanemode_write(fe_reg, dyna_reg->fe_four_plane_mode);

            isp_fe_ae_weight_config(fe_reg, dyna_reg);

            lut_update_en = dyna_reg->fe_weight_table_update;
        }
    }

    reg_cfg_info->alg_reg_cfg[0].fe_lut_update_cfg.ae1_lut_update = lut_update_en;
    return TD_SUCCESS;
}

static td_void isp_combine_ae_wgt_calc(isp_ae_dyna_cfg *dyna_reg_be_cfg)
{
    td_u16 j, k, m;
    td_u32 combin_weight, combin_weight_num;

    m = 0;
    combin_weight = 0;
    combin_weight_num = 0;

    for (j = 0; j < dyna_reg_be_cfg->be_weight_table_height; j++) {
        for (k = 0; k < dyna_reg_be_cfg->be_weight_table_width; k++) {
            combin_weight |= ((td_u32)dyna_reg_be_cfg->be_weight_table[j][k] << (8 * combin_weight_num)); /* 8 */
            combin_weight_num++;

            if (combin_weight_num == OT_ISP_AE_WEI_COMBIN_COUNT) {
                dyna_reg_be_cfg->combine_wgt[m++] = combin_weight;
                combin_weight_num = 0;
                combin_weight = 0;
            }
        }
    }

    if ((combin_weight_num != OT_ISP_AE_WEI_COMBIN_COUNT) && (combin_weight_num != 0)) {
        dyna_reg_be_cfg->combine_wgt[m++] = combin_weight;
    }
}

static td_void isp_post_be_ae_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_ae_static_cfg *static_reg_be_cfg,
    isp_ae_dyna_cfg *dyna_reg_be_cfg)
{
    td_u8 m;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(be_reg);
    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(pre_be_reg);

    isp_post_be_ae_crop_pos_x_write(be_reg, static_reg_be_cfg->be_crop_pos_x);
    isp_post_be_ae_crop_pos_y_write(be_reg, static_reg_be_cfg->be_crop_pos_y);
    isp_post_be_ae_crop_out_width_write(be_reg, static_reg_be_cfg->be_crop_out_width - 1);
    isp_post_be_ae_crop_out_height_write(be_reg, static_reg_be_cfg->be_crop_out_height - 1);
    /* ae be dynamic */
    isp_post_be_ae_sel_write(be_reg, dyna_reg_be_cfg->be_ae_sel);
    isp_post_be_ae_hnum_write(be_reg, dyna_reg_be_cfg->be_weight_table_width);
    isp_post_be_ae_vnum_write(be_reg, dyna_reg_be_cfg->be_weight_table_height);
    isp_post_be_ae_skip_x_write(be_reg, dyna_reg_be_cfg->be_hist_skip_x);
    isp_post_be_ae_offset_x_write(be_reg, dyna_reg_be_cfg->be_hist_offset_x);
    isp_post_be_ae_skip_y_write(be_reg, dyna_reg_be_cfg->be_hist_skip_y);
    isp_post_be_ae_offset_y_write(be_reg, dyna_reg_be_cfg->be_hist_offset_y);
    isp_post_be_ae_bitmove_write(be_reg, dyna_reg_be_cfg->be_bit_move);
    isp_post_be_ae_hist_gamma_mode_write(be_reg, dyna_reg_be_cfg->be_hist_gamma_mode);
    isp_post_be_ae_aver_gamma_mode_write(be_reg, dyna_reg_be_cfg->be_aver_gamma_mode);
    isp_post_be_ae_gamma_limit_write(be_reg, dyna_reg_be_cfg->be_gamma_limit);
    isp_post_be_ae_fourplanemode_write(be_reg, dyna_reg_be_cfg->be_four_plane_mode);
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_post_be_ae_wei_waddr_write(be_reg, 0);

        for (m = 0; m < 64; m++) { /* ae weight table number 64 */
            isp_post_be_ae_wei_wdata_write(be_reg, dyna_reg_be_cfg->combine_wgt[m]);
        }
    } else {
        isp_post_be_ae_weight_write(&be_reg->post_be_lut.post_be_apb_lut, dyna_reg_be_cfg->combine_wgt);
    }
}

static td_void isp_la_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_mg_static_cfg *mg_static_reg_cfg = TD_NULL;
    isp_mg_dyna_cfg *mg_dyna_reg_cfg = TD_NULL;

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(be_reg);

    /* mg static */
    mg_static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].mg_reg_cfg.static_reg_cfg;
    mg_dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].mg_reg_cfg.dyna_reg_cfg;

    isp_la_crop_pos_x_write(be_reg, mg_static_reg_cfg->crop_pos_x);
    isp_la_crop_pos_y_write(be_reg, mg_static_reg_cfg->crop_pos_y);
    isp_la_crop_out_width_write(be_reg, mg_static_reg_cfg->crop_out_width - 1);
    isp_la_crop_out_height_write(be_reg, mg_static_reg_cfg->crop_out_height - 1);

    /* mg dynamic */
    isp_la_hnum_write(be_reg, mg_dyna_reg_cfg->zone_width);
    isp_la_vnum_write(be_reg, mg_dyna_reg_cfg->zone_height);
    isp_la_bitmove_write(be_reg, mg_dyna_reg_cfg->bit_move);
    isp_la_gamma_en_write(be_reg, mg_dyna_reg_cfg->gamma_mode);
    isp_la_gamma_limit_write(be_reg, mg_dyna_reg_cfg->gamma_limit);
}

static td_s32 isp_ae_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool lut_update_en = TD_FALSE;
    td_bool offline_mode;
    isp_ae_static_cfg *static_reg_cfg = TD_NULL;
    isp_ae_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_ae_cfg1) {
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(post_viproc);

        /* ae be static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ae_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ae_reg_cfg.dyna_reg_cfg;

        isp_combine_ae_wgt_calc(dyna_reg_cfg);
        isp_ae_en_write(post_viproc, static_reg_cfg->be_enable);
        isp_post_be_ae_reg_config(vi_pipe, i, static_reg_cfg, dyna_reg_cfg);

        /* la */
        isp_la_reg_config(vi_pipe, reg_cfg_info, i);
        isp_la_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].mg_reg_cfg.static_reg_cfg.enable);

        lut_update_en = dyna_reg_cfg->be_weight_table_update;
    }

    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.ae_lut_update = lut_update_en || offline_mode;

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_ISP_AF_SUPPORT
static td_void isp_fe_af_iir_gain_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_iirg0_0_write(fe_reg, af_reg_fe_cfg->iir_gain0_group0);
    isp_fe_af_iirg0_1_write(fe_reg, af_reg_fe_cfg->iir_gain0_group1);
    isp_fe_af_iirg1_0_write(fe_reg, af_reg_fe_cfg->iir_gain1_group0);
    isp_fe_af_iirg1_1_write(fe_reg, af_reg_fe_cfg->iir_gain1_group1);
    isp_fe_af_iirg2_0_write(fe_reg, af_reg_fe_cfg->iir_gain2_group0);
    isp_fe_af_iirg2_1_write(fe_reg, af_reg_fe_cfg->iir_gain2_group1);
    isp_fe_af_iirg3_0_write(fe_reg, af_reg_fe_cfg->iir_gain3_group0);
    isp_fe_af_iirg3_1_write(fe_reg, af_reg_fe_cfg->iir_gain3_group1);
    isp_fe_af_iirg4_0_write(fe_reg, af_reg_fe_cfg->iir_gain4_group0);
    isp_fe_af_iirg4_1_write(fe_reg, af_reg_fe_cfg->iir_gain4_group1);
    isp_fe_af_iirg5_0_write(fe_reg, af_reg_fe_cfg->iir_gain5_group0);
    isp_fe_af_iirg5_1_write(fe_reg, af_reg_fe_cfg->iir_gain5_group1);
    isp_fe_af_iirg6_0_write(fe_reg, af_reg_fe_cfg->iir_gain6_group0);
    isp_fe_af_iirg6_1_write(fe_reg, af_reg_fe_cfg->iir_gain6_group1);
}

static td_void isp_fe_af_iir_shift_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_iirshift0_0_write(fe_reg, af_reg_fe_cfg->iir0_shift_group0);
    isp_fe_af_iirshift0_1_write(fe_reg, af_reg_fe_cfg->iir1_shift_group0);
    isp_fe_af_iirshift0_2_write(fe_reg, af_reg_fe_cfg->iir2_shift_group0);
    isp_fe_af_iirshift0_3_write(fe_reg, af_reg_fe_cfg->iir3_shift_group0);
    isp_fe_af_iirshift1_0_write(fe_reg, af_reg_fe_cfg->iir0_shift_group1);
    isp_fe_af_iirshift1_1_write(fe_reg, af_reg_fe_cfg->iir1_shift_group1);
    isp_fe_af_iirshift1_2_write(fe_reg, af_reg_fe_cfg->iir2_shift_group1);
    isp_fe_af_iirshift1_3_write(fe_reg, af_reg_fe_cfg->iir3_shift_group1);
}

static td_void isp_fe_af_fir_gain_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_firh0_0_write(fe_reg, af_reg_fe_cfg->fir_h_gain0_group0);
    isp_fe_af_firh0_1_write(fe_reg, af_reg_fe_cfg->fir_h_gain0_group1);

    isp_fe_af_firh1_0_write(fe_reg, af_reg_fe_cfg->fir_h_gain1_group0);
    isp_fe_af_firh1_1_write(fe_reg, af_reg_fe_cfg->fir_h_gain1_group1);

    isp_fe_af_firh2_0_write(fe_reg, af_reg_fe_cfg->fir_h_gain2_group0);
    isp_fe_af_firh2_1_write(fe_reg, af_reg_fe_cfg->fir_h_gain2_group1);

    isp_fe_af_firh3_0_write(fe_reg, af_reg_fe_cfg->fir_h_gain3_group0);
    isp_fe_af_firh3_1_write(fe_reg, af_reg_fe_cfg->fir_h_gain3_group1);

    isp_fe_af_firh4_0_write(fe_reg, af_reg_fe_cfg->fir_h_gain4_group0);
    isp_fe_af_firh4_1_write(fe_reg, af_reg_fe_cfg->fir_h_gain4_group1);
}

static td_void isp_fe_af_crop_write(ot_vi_pipe vi_pipe, isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    td_u16 crop_width, crop_pos_x;

    isp_fe_af_crop_en_write(fe_reg, af_reg_fe_cfg->fe_crop_enable);
    if (af_reg_fe_cfg->fe_crop_enable) {
        crop_pos_x = is_hrs_on(vi_pipe) ? (af_reg_fe_cfg->fe_crop_pos_x >> 1) : af_reg_fe_cfg->fe_crop_pos_x;
        isp_fe_af_pos_x_write(fe_reg, crop_pos_x);
        isp_fe_af_pos_y_write(fe_reg, af_reg_fe_cfg->fe_crop_pos_y);

        crop_width = is_hrs_on(vi_pipe) ? (af_reg_fe_cfg->fe_crop_h_size >> 1) : af_reg_fe_cfg->fe_crop_h_size;
        crop_width = isp_alignup(crop_width, 8); /* align up 8 */
        isp_fe_af_crop_hsize_write(fe_reg, crop_width - 1);
        isp_fe_af_crop_vsize_write(fe_reg, af_reg_fe_cfg->fe_crop_v_size - 1);
    }
}

static td_void isp_fe_af_raw_cfg_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_raw_mode_write(fe_reg, af_reg_fe_cfg->raw_mode);
    isp_fe_af_gain_lmt_write(fe_reg, af_reg_fe_cfg->gain_limit);
    isp_fe_af_gamma_write(fe_reg, af_reg_fe_cfg->gamma);
    isp_fe_af_bayer_mode_write(fe_reg, af_reg_fe_cfg->bayer_mode);
}

static td_void isp_fe_af_level_depend_gain_cfg_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_iir0_ldg_en_write(fe_reg, af_reg_fe_cfg->iir0_ldg_enable);
    isp_fe_af_iir_thre0_l_write(fe_reg, af_reg_fe_cfg->iir_thre0_low);
    isp_fe_af_iir_thre0_h_write(fe_reg, af_reg_fe_cfg->iir_thre0_high);
    isp_fe_af_iir_slope0_l_write(fe_reg, af_reg_fe_cfg->iir_slope0_low);
    isp_fe_af_iir_slope0_h_write(fe_reg, af_reg_fe_cfg->iir_slope0_high);
    isp_fe_af_iir_gain0_l_write(fe_reg, af_reg_fe_cfg->iir_gain0_low);
    isp_fe_af_iir_gain0_h_write(fe_reg, af_reg_fe_cfg->iir_gain0_high);

    isp_fe_af_iir1_ldg_en_write(fe_reg, af_reg_fe_cfg->iir1_ldg_enable);
    isp_fe_af_iir_thre1_l_write(fe_reg, af_reg_fe_cfg->iir_thre1_low);
    isp_fe_af_iir_thre1_h_write(fe_reg, af_reg_fe_cfg->iir_thre1_high);
    isp_fe_af_iir_slope1_l_write(fe_reg, af_reg_fe_cfg->iir_slope1_low);
    isp_fe_af_iir_slope1_h_write(fe_reg, af_reg_fe_cfg->iir_slope1_high);
    isp_fe_af_iir_gain1_l_write(fe_reg, af_reg_fe_cfg->iir_gain1_low);
    isp_fe_af_iir_gain1_h_write(fe_reg, af_reg_fe_cfg->iir_gain1_high);

    isp_fe_af_fir0_ldg_en_write(fe_reg, af_reg_fe_cfg->fir0_ldg_enable);
    isp_fe_af_fir_thre0_l_write(fe_reg, af_reg_fe_cfg->fir_thre0_low);
    isp_fe_af_fir_thre0_h_write(fe_reg, af_reg_fe_cfg->fir_thre0_high);
    isp_fe_af_fir_slope0_l_write(fe_reg, af_reg_fe_cfg->fir_slope0_low);
    isp_fe_af_fir_slope0_h_write(fe_reg, af_reg_fe_cfg->fir_slope0_high);
    isp_fe_af_fir_gain0_l_write(fe_reg, af_reg_fe_cfg->fir_gain0_low);
    isp_fe_af_fir_gain0_h_write(fe_reg, af_reg_fe_cfg->fir_gain0_high);

    isp_fe_af_fir1_ldg_en_write(fe_reg, af_reg_fe_cfg->fir1_ldg_enable);
    isp_fe_af_fir_thre1_l_write(fe_reg, af_reg_fe_cfg->fir_thre1_low);
    isp_fe_af_fir_thre1_h_write(fe_reg, af_reg_fe_cfg->fir_thre1_high);
    isp_fe_af_fir_slope1_l_write(fe_reg, af_reg_fe_cfg->fir_slope1_low);
    isp_fe_af_fir_slope1_h_write(fe_reg, af_reg_fe_cfg->fir_slope1_high);
    isp_fe_af_fir_gain1_l_write(fe_reg, af_reg_fe_cfg->fir_gain1_low);
    isp_fe_af_fir_gain1_h_write(fe_reg, af_reg_fe_cfg->fir_gain1_high);
}

static td_void isp_fe_af_coring_cfg_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_iir_thre0_c_write(fe_reg, af_reg_fe_cfg->iir_thre0_coring);
    isp_fe_af_iir_slope0_c_write(fe_reg, af_reg_fe_cfg->iir_slope0_coring);
    isp_fe_af_iir_peak0_c_write(fe_reg, af_reg_fe_cfg->iir_peak0_coring);

    isp_fe_af_iir_thre1_c_write(fe_reg, af_reg_fe_cfg->iir_thre1_coring);
    isp_fe_af_iir_slope1_c_write(fe_reg, af_reg_fe_cfg->iir_slope1_coring);
    isp_fe_af_iir_peak1_c_write(fe_reg, af_reg_fe_cfg->iir_peak1_coring);

    isp_fe_af_fir_thre0_c_write(fe_reg, af_reg_fe_cfg->fir_thre0_coring);
    isp_fe_af_fir_slope0_c_write(fe_reg, af_reg_fe_cfg->fir_slope0_coring);
    isp_fe_af_fir_peak0_c_write(fe_reg, af_reg_fe_cfg->fir_peak0_coring);

    isp_fe_af_fir_thre1_c_write(fe_reg, af_reg_fe_cfg->fir_thre1_coring);
    isp_fe_af_fir_slope1_c_write(fe_reg, af_reg_fe_cfg->fir_slope1_coring);
    isp_fe_af_fir_peak1_c_write(fe_reg, af_reg_fe_cfg->fir_peak1_coring);
}

static td_void isp_fe_af_output_shift_cfg_write(isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_acc_shift0_h_write(fe_reg, af_reg_fe_cfg->acc_shift0_h);
    isp_fe_af_acc_shift1_h_write(fe_reg, af_reg_fe_cfg->acc_shift1_h);
    isp_fe_af_acc_shift0_v_write(fe_reg, af_reg_fe_cfg->acc_shift0_v);
    isp_fe_af_acc_shift1_v_write(fe_reg, af_reg_fe_cfg->acc_shift1_v);
    isp_fe_af_acc_shift_y_write(fe_reg, af_reg_fe_cfg->acc_shift_y);
    isp_fe_af_cnt_shift_y_write(fe_reg, af_reg_fe_cfg->shift_count_y);
    isp_fe_af_cnt_shift0_h_write(fe_reg, 0x0);
    isp_fe_af_cnt_shift1_h_write(fe_reg, 0x0);
    isp_fe_af_cnt_shift0_v_write(fe_reg, ISP_AF_CNT_SHIFT0_V_DEFAULT);
    isp_fe_af_cnt_shift1_v_write(fe_reg, 0x0);
}

static td_void isp_fe_af_reg_write(ot_vi_pipe vi_pipe, isp_fe_reg_type *fe_reg, isp_af_reg_cfg *af_reg_fe_cfg)
{
    isp_fe_af_en_write(fe_reg, af_reg_fe_cfg->af_enable);

    isp_fe_af_lpf_en_write(fe_reg, af_reg_fe_cfg->lpf_enable);
    isp_fe_af_fir0_lpf_en_write(fe_reg, af_reg_fe_cfg->fir0_lpf_enable);
    isp_fe_af_fir1_lpf_en_write(fe_reg, af_reg_fe_cfg->fir1_lpf_enable);
    isp_fe_af_iir0_ds_en_write(fe_reg, af_reg_fe_cfg->iir0_ds_enable);
    isp_fe_af_iir1_ds_en_write(fe_reg, af_reg_fe_cfg->iir1_ds_enable);
    isp_fe_af_iir_dilate0_write(fe_reg, af_reg_fe_cfg->iir_dilate0);
    isp_fe_af_iir_dilate1_write(fe_reg, af_reg_fe_cfg->iir_dilate1);
    isp_fe_af_iirplg_0_write(fe_reg, af_reg_fe_cfg->iir_plg_group0);
    isp_fe_af_iirpls_0_write(fe_reg, af_reg_fe_cfg->iir_pls_group0);
    isp_fe_af_iirplg_1_write(fe_reg, af_reg_fe_cfg->iir_plg_group1);
    isp_fe_af_iirpls_1_write(fe_reg, af_reg_fe_cfg->iir_pls_group1);

    isp_fe_af_iir0_en0_write(fe_reg, af_reg_fe_cfg->iir0_enable0);
    isp_fe_af_iir0_en1_write(fe_reg, af_reg_fe_cfg->iir0_enable1);
    isp_fe_af_iir0_en2_write(fe_reg, af_reg_fe_cfg->iir0_enable2);
    isp_fe_af_iir1_en0_write(fe_reg, af_reg_fe_cfg->iir1_enable0);
    isp_fe_af_iir1_en1_write(fe_reg, af_reg_fe_cfg->iir1_enable1);
    isp_fe_af_iir1_en2_write(fe_reg, af_reg_fe_cfg->iir1_enable2);
    isp_fe_af_peak_mode_write(fe_reg, af_reg_fe_cfg->peak_mode);
    isp_fe_af_squ_mode_write(fe_reg, af_reg_fe_cfg->squ_mode);
    isp_fe_af_hnum_write(fe_reg, af_reg_fe_cfg->window_hnum);
    isp_fe_af_vnum_write(fe_reg, af_reg_fe_cfg->window_vnum);

    isp_fe_af_iir_gain_write(fe_reg, af_reg_fe_cfg);
    isp_fe_af_iir_shift_write(fe_reg, af_reg_fe_cfg);
    isp_fe_af_fir_gain_write(fe_reg, af_reg_fe_cfg);

    isp_fe_af_crop_write(vi_pipe, fe_reg, af_reg_fe_cfg);
    isp_fe_af_raw_cfg_write(fe_reg, af_reg_fe_cfg);

    /* AF BE pre median filter */
    isp_fe_af_mean_en_write(fe_reg, af_reg_fe_cfg->mean_enable);
    isp_fe_af_mean_thres_write(fe_reg, 0xFFFF - af_reg_fe_cfg->mean_thres);
    isp_fe_af_level_depend_gain_cfg_write(fe_reg, af_reg_fe_cfg);
    isp_fe_af_coring_cfg_write(fe_reg, af_reg_fe_cfg);

    /* high luma counter */
    isp_fe_af_highlight_write(fe_reg, af_reg_fe_cfg->highlight_thre);

    /* AF output shift */
    isp_fe_af_output_shift_cfg_write(fe_reg, af_reg_fe_cfg);
}
#endif
static td_s32 isp_fe_af_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
#ifdef CONFIG_OT_ISP_AF_SUPPORT
    td_u8  af_en[OT_ISP_MAX_PHY_PIPE_NUM] = { ISP_PIPE_FEAF_MODULE_ENABLE };

    td_u32 i;
    ot_vi_pipe vi_pipe_bind;
    isp_fe_reg_type *fe_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    if (reg_cfg_info->cfg_key.bit1_af_fe_cfg) {
        for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
            vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
            isp_check_vir_pipe_return(vi_pipe_bind);
            if (af_en[vi_pipe_bind] != TD_TRUE) {
                continue;
            }
            fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
            isp_check_pointer_return(fe_reg);

            isp_fe_af_reg_write(vi_pipe_bind, fe_reg, &reg_cfg_info->alg_reg_cfg[0].fe_af_reg_cfg);
        }

        reg_cfg_info->cfg_key.bit1_af_fe_cfg = 0;
    }

#endif

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_ISP_AF_SUPPORT
static td_void isp_be_af_iir_gain_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_iirg0_0_write(be_reg, af_reg_be_cfg->iir_gain0_group0);
    isp_af_iirg0_1_write(be_reg, af_reg_be_cfg->iir_gain0_group1);
    isp_af_iirg1_0_write(be_reg, af_reg_be_cfg->iir_gain1_group0);
    isp_af_iirg1_1_write(be_reg, af_reg_be_cfg->iir_gain1_group1);
    isp_af_iirg2_0_write(be_reg, af_reg_be_cfg->iir_gain2_group0);
    isp_af_iirg2_1_write(be_reg, af_reg_be_cfg->iir_gain2_group1);
    isp_af_iirg3_0_write(be_reg, af_reg_be_cfg->iir_gain3_group0);
    isp_af_iirg3_1_write(be_reg, af_reg_be_cfg->iir_gain3_group1);
    isp_af_iirg4_0_write(be_reg, af_reg_be_cfg->iir_gain4_group0);
    isp_af_iirg4_1_write(be_reg, af_reg_be_cfg->iir_gain4_group1);
    isp_af_iirg5_0_write(be_reg, af_reg_be_cfg->iir_gain5_group0);
    isp_af_iirg5_1_write(be_reg, af_reg_be_cfg->iir_gain5_group1);
    isp_af_iirg6_0_write(be_reg, af_reg_be_cfg->iir_gain6_group0);
    isp_af_iirg6_1_write(be_reg, af_reg_be_cfg->iir_gain6_group1);
}

static td_void isp_be_af_iir_shift_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_iirshift0_0_write(be_reg, af_reg_be_cfg->iir0_shift_group0);
    isp_af_iirshift0_1_write(be_reg, af_reg_be_cfg->iir1_shift_group0);
    isp_af_iirshift0_2_write(be_reg, af_reg_be_cfg->iir2_shift_group0);
    isp_af_iirshift0_3_write(be_reg, af_reg_be_cfg->iir3_shift_group0);
    isp_af_iirshift1_0_write(be_reg, af_reg_be_cfg->iir0_shift_group1);
    isp_af_iirshift1_1_write(be_reg, af_reg_be_cfg->iir1_shift_group1);
    isp_af_iirshift1_2_write(be_reg, af_reg_be_cfg->iir2_shift_group1);
    isp_af_iirshift1_3_write(be_reg, af_reg_be_cfg->iir3_shift_group1);
}

static td_void isp_be_af_fir_gain_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_firh0_0_write(be_reg, af_reg_be_cfg->fir_h_gain0_group0);
    isp_af_firh0_1_write(be_reg, af_reg_be_cfg->fir_h_gain0_group1);

    isp_af_firh1_0_write(be_reg, af_reg_be_cfg->fir_h_gain1_group0);
    isp_af_firh1_1_write(be_reg, af_reg_be_cfg->fir_h_gain1_group1);

    isp_af_firh2_0_write(be_reg, af_reg_be_cfg->fir_h_gain2_group0);
    isp_af_firh2_1_write(be_reg, af_reg_be_cfg->fir_h_gain2_group1);

    isp_af_firh3_0_write(be_reg, af_reg_be_cfg->fir_h_gain3_group0);
    isp_af_firh3_1_write(be_reg, af_reg_be_cfg->fir_h_gain3_group1);

    isp_af_firh4_0_write(be_reg, af_reg_be_cfg->fir_h_gain4_group0);
    isp_af_firh4_1_write(be_reg, af_reg_be_cfg->fir_h_gain4_group1);
}

static td_void isp_be_af_crop_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_crop_en_write(be_reg, af_reg_be_cfg->crop_enable);
    if (af_reg_be_cfg->crop_enable) {
        isp_af_pos_x_write(be_reg, af_reg_be_cfg->crop_pos_x);
        isp_af_pos_y_write(be_reg, af_reg_be_cfg->crop_pos_y);
        isp_af_crop_hsize_write(be_reg, af_reg_be_cfg->crop_h_size - 1);
        isp_af_crop_vsize_write(be_reg, af_reg_be_cfg->crop_v_size - 1);
    }
}

static td_void isp_be_af_raw_cfg_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_sel_write(be_reg, af_reg_be_cfg->af_pos_sel);
    isp_af_raw_mode_write(be_reg, af_reg_be_cfg->raw_mode);
    isp_af_gain_lmt_write(be_reg, af_reg_be_cfg->gain_limit);
    isp_af_gamma_write(be_reg, af_reg_be_cfg->gamma);
    isp_af_bayer_mode_write(be_reg, af_reg_be_cfg->bayer_mode);
}

static td_void isp_be_af_level_depend_gain_cfg_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_iir0_ldg_en_write(be_reg, af_reg_be_cfg->iir0_ldg_enable);
    isp_af_iir_thre0_l_write(be_reg, af_reg_be_cfg->iir_thre0_low);
    isp_af_iir_thre0_h_write(be_reg, af_reg_be_cfg->iir_thre0_high);
    isp_af_iir_slope0_l_write(be_reg, af_reg_be_cfg->iir_slope0_low);
    isp_af_iir_slope0_h_write(be_reg, af_reg_be_cfg->iir_slope0_high);
    isp_af_iir_gain0_l_write(be_reg, af_reg_be_cfg->iir_gain0_low);
    isp_af_iir_gain0_h_write(be_reg, af_reg_be_cfg->iir_gain0_high);

    isp_af_iir1_ldg_en_write(be_reg, af_reg_be_cfg->iir1_ldg_enable);
    isp_af_iir_thre1_l_write(be_reg, af_reg_be_cfg->iir_thre1_low);
    isp_af_iir_thre1_h_write(be_reg, af_reg_be_cfg->iir_thre1_high);
    isp_af_iir_slope1_l_write(be_reg, af_reg_be_cfg->iir_slope1_low);
    isp_af_iir_slope1_h_write(be_reg, af_reg_be_cfg->iir_slope1_high);
    isp_af_iir_gain1_l_write(be_reg, af_reg_be_cfg->iir_gain1_low);
    isp_af_iir_gain1_h_write(be_reg, af_reg_be_cfg->iir_gain1_high);

    isp_af_fir0_ldg_en_write(be_reg, af_reg_be_cfg->fir0_ldg_enable);
    isp_af_fir_thre0_l_write(be_reg, af_reg_be_cfg->fir_thre0_low);
    isp_af_fir_thre0_h_write(be_reg, af_reg_be_cfg->fir_thre0_high);
    isp_af_fir_slope0_l_write(be_reg, af_reg_be_cfg->fir_slope0_low);
    isp_af_fir_slope0_h_write(be_reg, af_reg_be_cfg->fir_slope0_high);
    isp_af_fir_gain0_l_write(be_reg, af_reg_be_cfg->fir_gain0_low);
    isp_af_fir_gain0_h_write(be_reg, af_reg_be_cfg->fir_gain0_high);

    isp_af_fir1_ldg_en_write(be_reg, af_reg_be_cfg->fir1_ldg_enable);
    isp_af_fir_thre1_l_write(be_reg, af_reg_be_cfg->fir_thre1_low);
    isp_af_fir_thre1_h_write(be_reg, af_reg_be_cfg->fir_thre1_high);
    isp_af_fir_slope1_l_write(be_reg, af_reg_be_cfg->fir_slope1_low);
    isp_af_fir_slope1_h_write(be_reg, af_reg_be_cfg->fir_slope1_high);
    isp_af_fir_gain1_l_write(be_reg, af_reg_be_cfg->fir_gain1_low);
    isp_af_fir_gain1_h_write(be_reg, af_reg_be_cfg->fir_gain1_high);
}

static td_void isp_be_af_coring_cfg_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_iir_thre0_c_write(be_reg, af_reg_be_cfg->iir_thre0_coring);
    isp_af_iir_slope0_c_write(be_reg, af_reg_be_cfg->iir_slope0_coring);
    isp_af_iir_peak0_c_write(be_reg, af_reg_be_cfg->iir_peak0_coring);

    isp_af_iir_thre1_c_write(be_reg, af_reg_be_cfg->iir_thre1_coring);
    isp_af_iir_slope1_c_write(be_reg, af_reg_be_cfg->iir_slope1_coring);
    isp_af_iir_peak1_c_write(be_reg, af_reg_be_cfg->iir_peak1_coring);

    isp_af_fir_thre0_c_write(be_reg, af_reg_be_cfg->fir_thre0_coring);
    isp_af_fir_slope0_c_write(be_reg, af_reg_be_cfg->fir_slope0_coring);
    isp_af_fir_peak0_c_write(be_reg, af_reg_be_cfg->fir_peak0_coring);

    isp_af_fir_thre1_c_write(be_reg, af_reg_be_cfg->fir_thre1_coring);
    isp_af_fir_slope1_c_write(be_reg, af_reg_be_cfg->fir_slope1_coring);
    isp_af_fir_peak1_c_write(be_reg, af_reg_be_cfg->fir_peak1_coring);
}

static td_void isp_be_af_output_shift_cfg_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_acc_shift0_h_write(be_reg, af_reg_be_cfg->acc_shift0_h);
    isp_af_acc_shift1_h_write(be_reg, af_reg_be_cfg->acc_shift1_h);
    isp_af_acc_shift0_v_write(be_reg, af_reg_be_cfg->acc_shift0_v);
    isp_af_acc_shift1_v_write(be_reg, af_reg_be_cfg->acc_shift1_v);
    isp_af_acc_shift_y_write(be_reg, af_reg_be_cfg->acc_shift_y);
    isp_af_cnt_shift_y_write(be_reg, af_reg_be_cfg->shift_count_y);
    isp_af_cnt_shift0_v_write(be_reg, ISP_AF_CNT_SHIFT0_V_DEFAULT);
    isp_af_cnt_shift0_h_write(be_reg, 0x0);
    isp_af_cnt_shift1_h_write(be_reg, 0x0);
    isp_af_cnt_shift1_v_write(be_reg, 0x0);
}

static td_void isp_be_af_reg_write(isp_post_be_reg_type *be_reg, isp_af_reg_cfg *af_reg_be_cfg)
{
    isp_af_lpf_en_write(be_reg, af_reg_be_cfg->lpf_enable);
    isp_af_fir0_lpf_en_write(be_reg, af_reg_be_cfg->fir0_lpf_enable);
    isp_af_fir1_lpf_en_write(be_reg, af_reg_be_cfg->fir1_lpf_enable);
    isp_af_iir0_ds_en_write(be_reg, af_reg_be_cfg->iir0_ds_enable);
    isp_af_iir1_ds_en_write(be_reg, af_reg_be_cfg->iir1_ds_enable);
    isp_af_iir_dilate0_write(be_reg, af_reg_be_cfg->iir_dilate0);
    isp_af_iir_dilate1_write(be_reg, af_reg_be_cfg->iir_dilate1);
    isp_af_iirplg_0_write(be_reg, af_reg_be_cfg->iir_plg_group0);
    isp_af_iirpls_0_write(be_reg, af_reg_be_cfg->iir_pls_group0);
    isp_af_iirplg_1_write(be_reg, af_reg_be_cfg->iir_plg_group1);
    isp_af_iirpls_1_write(be_reg, af_reg_be_cfg->iir_pls_group1);

    isp_af_iir0_en0_write(be_reg, af_reg_be_cfg->iir0_enable0);
    isp_af_iir0_en1_write(be_reg, af_reg_be_cfg->iir0_enable1);
    isp_af_iir0_en2_write(be_reg, af_reg_be_cfg->iir0_enable2);
    isp_af_iir1_en0_write(be_reg, af_reg_be_cfg->iir1_enable0);
    isp_af_iir1_en1_write(be_reg, af_reg_be_cfg->iir1_enable1);
    isp_af_iir1_en2_write(be_reg, af_reg_be_cfg->iir1_enable2);
    isp_af_peak_mode_write(be_reg, af_reg_be_cfg->peak_mode);
    isp_af_squ_mode_write(be_reg, af_reg_be_cfg->squ_mode);
    isp_af_hnum_write(be_reg, af_reg_be_cfg->window_hnum);
    isp_af_vnum_write(be_reg, af_reg_be_cfg->window_vnum);

    isp_be_af_iir_gain_write(be_reg, af_reg_be_cfg);
    isp_be_af_iir_shift_write(be_reg, af_reg_be_cfg);
    isp_be_af_fir_gain_write(be_reg, af_reg_be_cfg);

    isp_be_af_crop_write(be_reg, af_reg_be_cfg);
    isp_be_af_raw_cfg_write(be_reg, af_reg_be_cfg);

    /* AF BE pre median filter */
    isp_af_mean_en_write(be_reg, af_reg_be_cfg->mean_enable);
    isp_af_mean_thres_write(be_reg, 0xFFFF - af_reg_be_cfg->mean_thres);
    isp_be_af_level_depend_gain_cfg_write(be_reg, af_reg_be_cfg); /* level depend gain */
    isp_be_af_coring_cfg_write(be_reg, af_reg_be_cfg);            /* AF BE coring */

    /* high luma counter */
    isp_af_highlight_write(be_reg, af_reg_be_cfg->highlight_thre);

    isp_be_af_output_shift_cfg_write(be_reg, af_reg_be_cfg);
}
#endif
static td_s32 isp_af_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
#ifdef CONFIG_OT_ISP_AF_SUPPORT
    td_bool offline_mode;
    td_bool usr_resh;
    td_bool idx_resh;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_af_reg_cfg *af_reg_be_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    af_reg_be_cfg = &reg_cfg_info->alg_reg_cfg[i].be_af_reg_cfg;
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_return(be_reg);

    idx_resh = (isp_af_update_index_read(be_reg) != af_reg_be_cfg->update_index);
    usr_resh =
        (offline_mode) ? (reg_cfg_info->cfg_key.bit1_af_be_cfg && idx_resh) : (reg_cfg_info->cfg_key.bit1_af_be_cfg);

    if (usr_resh) {
        isp_af_update_index_write(be_reg, af_reg_be_cfg->update_index);

        isp_be_af_reg_write(be_reg, af_reg_be_cfg);

        reg_cfg_info->cfg_key.bit1_af_be_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
#endif

    return TD_SUCCESS;
}

static td_s32 isp_fe_awb_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u32 i;
    ot_vi_pipe vi_pipe_bind;
    isp_awb_reg_dyn_cfg *awb_reg_dyn_cfg = TD_NULL;
    isp_awb_reg_sta_cfg *awb_reg_sta_cfg = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return TD_SUCCESS;
    }

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
        isp_check_vir_pipe_return(vi_pipe_bind);

        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);
        if (reg_cfg_info->cfg_key.bit1_awb_dyn_cfg) {
            awb_reg_dyn_cfg = &reg_cfg_info->alg_reg_cfg[0].awb_reg_cfg.awb_reg_dyn_cfg;
            isp_fe_wb_gain_write(fe_reg, awb_reg_dyn_cfg->fe_white_balance_gain);
            isp_fe_wb_en_write(fe_reg, awb_reg_dyn_cfg->fe_wb_work_en);
        }

        awb_reg_sta_cfg = &reg_cfg_info->alg_reg_cfg[0].awb_reg_cfg.awb_reg_sta_cfg;

        if (awb_reg_sta_cfg->fe_awb_sta_cfg) {
            awb_reg_sta_cfg = &reg_cfg_info->alg_reg_cfg[0].awb_reg_cfg.awb_reg_sta_cfg;
            isp_fe_wb_clip_value_write(fe_reg, awb_reg_sta_cfg->fe_clip_value);
        }
    }

    return TD_SUCCESS;
}

static td_void isp_awb_cc_set(isp_post_be_reg_type *be_reg, td_u16 *be_cc)
{
    td_u8 i;
    for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
        if ((be_cc[i] >> 12) < 0x8) {          /* valid bit 12, sign bit 0x8 */
            be_cc[i] = MIN2(be_cc[i], 0xFFF);  /* max positive value 0xFFF */
        } else if ((be_cc[i] >> 12) >= 0x8) {  /* valid bit 12, sign bit 0x8 */
            be_cc[i] = MIN2(be_cc[i], 0x8FFF); /* max negative value 0x8FFF */
        }
    }
    isp_cc_coef00_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[0]))); /* array index 0 */
    isp_cc_coef01_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[1]))); /* array index 1 */
    isp_cc_coef02_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[2]))); /* array index 2 */
    isp_cc_coef10_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[3]))); /* array index 3 */
    isp_cc_coef11_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[4]))); /* array index 4 */
    isp_cc_coef12_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[5]))); /* array index 5 */
    isp_cc_coef20_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[6]))); /* array index 6 */
    isp_cc_coef21_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[7]))); /* array index 7 */
    isp_cc_coef22_write(be_reg, ccm_convert(ccm_convert_pre(be_cc[8]))); /* array index 8 */
}

static td_void isp_awb_gain_set(isp_post_be_reg_type *be_reg, td_u32 *be_wb_gain)
{
    isp_wb_rgain_write(be_reg, be_wb_gain[0]);  /* array index 0 */
    isp_wb_grgain_write(be_reg, be_wb_gain[1]); /* array index 1 */
    isp_wb_gbgain_write(be_reg, be_wb_gain[2]); /* array index 2 */
    isp_wb_bgain_write(be_reg, be_wb_gain[3]);  /* array index 3 */
    isp_wb_irgain_write(be_reg, 0x100);
}

static td_void isp_awb_dyn_reg_config(isp_post_be_reg_type *be_reg, isp_awb_reg_dyn_cfg *awb_reg_dyn_cfg)
{
    isp_awb_threshold_max_write(be_reg, awb_reg_dyn_cfg->be_metering_white_level_awb);
    isp_awb_threshold_min_write(be_reg, awb_reg_dyn_cfg->be_metering_black_level_awb);
    isp_awb_cr_ref_max_write(be_reg, awb_reg_dyn_cfg->be_metering_cr_ref_max_awb);
    isp_awb_cr_ref_min_write(be_reg, awb_reg_dyn_cfg->be_metering_cr_ref_min_awb);
    isp_awb_cb_ref_max_write(be_reg, awb_reg_dyn_cfg->be_metering_cb_ref_max_awb);
    isp_awb_cb_ref_min_write(be_reg, awb_reg_dyn_cfg->be_metering_cb_ref_min_awb);

    isp_cc_r_gain_write(be_reg, awb_reg_dyn_cfg->be_cc_r_gain);
    isp_cc_g_gain_write(be_reg, awb_reg_dyn_cfg->be_cc_g_gain);
    isp_cc_b_gain_write(be_reg, awb_reg_dyn_cfg->be_cc_b_gain);

    isp_awb_crop_pos_x_write(be_reg, awb_reg_dyn_cfg->be_crop_pos_x);
    isp_awb_crop_pos_y_write(be_reg, awb_reg_dyn_cfg->be_crop_pos_y);
    isp_awb_crop_out_width_write(be_reg, awb_reg_dyn_cfg->be_crop_out_width - 1);
    isp_awb_crop_out_height_write(be_reg, awb_reg_dyn_cfg->be_crop_out_height - 1);
}

static td_void isp_awb_sta_reg_config(isp_post_be_reg_type *be_reg, isp_viproc_reg_type *post_vi_proc,
    isp_awb_reg_sta_cfg *awb_reg_sta_cfg)
{
    isp_awb_bitmove_write(be_reg, awb_reg_sta_cfg->be_awb_bitmove);
    isp_awb_en_write(post_vi_proc, awb_reg_sta_cfg->be_awb_work_en);
    isp_awb_stat_raddr_write(be_reg, awb_reg_sta_cfg->be_awb_stat_raddr);
    isp_cc_recover_en_write(be_reg, OT_ISP_CCM_RECOVER_EN_DEFAULT);
    isp_cc_luma_coefr_write(be_reg, OT_ISP_CCM_LUMA_COEFR_DEFAULT);
    isp_cc_luma_coefb_write(be_reg, OT_ISP_CCM_LUMA_COEFB_DEFAULT);
    isp_cc_luma_coefr_up_write(be_reg, OT_ISP_CCM_LUMA_COEFR_UP_DEFAULT);
    isp_cc_luma_coefb_up_write(be_reg, OT_ISP_CCM_LUMA_COEFB_UP_DEFAULT);
    isp_cc_soft_clip0_step_write(be_reg, OT_ISP_CCM_SOFT_CLIP0_STEP_DEFAULT);
    isp_cc_soft_clip1_step_write(be_reg, OT_ISP_CCM_SOFT_CLIP1_STEP_DEFAULT);
    isp_cc_darkprev_write(be_reg, OT_ISP_CCM_DARKPREV_DEFAULT);
    isp_cc_peaksupp_sat_write(be_reg, OT_ISP_CCM_PEAKSUPP_SAT_DEFAULT);
    isp_cc_peaksupp_max_write(be_reg, OT_ISP_CCM_PEAKSUPP_MAX_DEFAULT);

    isp_cc_in_dc0_write(be_reg, awb_reg_sta_cfg->be_cc_in_dc0);
    isp_cc_in_dc1_write(be_reg, awb_reg_sta_cfg->be_cc_in_dc1);
    isp_cc_in_dc2_write(be_reg, awb_reg_sta_cfg->be_cc_in_dc2);
    isp_cc_out_dc0_write(be_reg, awb_reg_sta_cfg->be_cc_out_dc0);
    isp_cc_out_dc1_write(be_reg, awb_reg_sta_cfg->be_cc_out_dc1);
    isp_cc_out_dc2_write(be_reg, awb_reg_sta_cfg->be_cc_out_dc2);
    isp_wb_clip_value_write(be_reg, awb_reg_sta_cfg->be_wb_clip_value);
    isp_awb_offset_comp_write(be_reg, awb_reg_sta_cfg->be_awb_offset_comp);
}

static td_void isp_awb_usr_reg_config(isp_post_be_reg_type *be_reg, isp_awb_reg_usr_cfg *awb_reg_usr_cfg)
{
    isp_awb_update_index_write(be_reg, awb_reg_usr_cfg->update_index);
    isp_awb_sel_write(be_reg, awb_reg_usr_cfg->be_awb_switch);
    isp_awb_hnum_write(be_reg, awb_reg_usr_cfg->be_zone_col);
    isp_awb_vnum_write(be_reg, awb_reg_usr_cfg->be_zone_row);
}

static td_void isp_awb_cc_and_gain_reg_config(ot_vi_pipe vi_pipe, isp_awb_reg_dyn_cfg *awb_reg_dyn_cfg, td_u8 i,
    isp_post_be_reg_type *be_reg)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    /* offline mode, cfg ccm/awb at sync param in vi module. do not have online stich mode */
    if (isp_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        isp_awb_cc_set(be_reg, awb_reg_dyn_cfg->be_color_matrix);
        isp_awb_gain_set(be_reg, awb_reg_dyn_cfg->be_white_balance_gain);
    }
}

static td_s32 isp_awb_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_awb_reg_dyn_cfg *awb_reg_dyn_cfg = TD_NULL;
    isp_awb_reg_sta_cfg *awb_reg_sta_cfg = TD_NULL;
    isp_awb_reg_usr_cfg *awb_reg_usr_cfg = TD_NULL;
    isp_viproc_reg_type *post_vi_proc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    td_bool offline_mode;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_bool idx_resh, usr_resh;

    isp_get_ctx(vi_pipe, isp_ctx);

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    post_vi_proc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(be_reg);
    isp_check_pointer_return(post_vi_proc);

    awb_reg_dyn_cfg = &reg_cfg_info->alg_reg_cfg[i].awb_reg_cfg.awb_reg_dyn_cfg;
    awb_reg_sta_cfg = &reg_cfg_info->alg_reg_cfg[i].awb_reg_cfg.awb_reg_sta_cfg;
    awb_reg_usr_cfg = &reg_cfg_info->alg_reg_cfg[i].awb_reg_cfg.awb_reg_usr_cfg;

    isp_awb_cc_and_gain_reg_config(vi_pipe, awb_reg_dyn_cfg, i, be_reg);

    if (reg_cfg_info->cfg_key.bit1_awb_dyn_cfg) {
        isp_awb_dyn_reg_config(be_reg, awb_reg_dyn_cfg);
    }

    if (awb_reg_sta_cfg->be_awb_sta_cfg) {
        isp_awb_sta_reg_config(be_reg, post_vi_proc, awb_reg_sta_cfg);
        awb_reg_sta_cfg->be_awb_sta_cfg = 0;
    }

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    idx_resh = (isp_awb_update_index_read(be_reg) != awb_reg_usr_cfg->update_index);
    usr_resh = (offline_mode) ? (awb_reg_usr_cfg->resh && idx_resh) : (awb_reg_usr_cfg->resh);

    if (usr_resh) {
        isp_awb_usr_reg_config(be_reg, awb_reg_usr_cfg);
        /* if online mode, resh=0; if offline mode, resh=1; but only index != will resh */
        awb_reg_usr_cfg->resh = offline_mode;
    }

    return TD_SUCCESS;
}

static td_s32 isp_awb_lsc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_awblsc_reg_dyn_cfg *awblsc_reg_dyn_cfg = TD_NULL;
    isp_awblsc_reg_sta_cfg *awblsc_reg_sta_cfg = TD_NULL;
    isp_viproc_reg_type *post_vi_proc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    post_vi_proc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(be_reg);
    isp_check_pointer_return(post_vi_proc);

    awblsc_reg_dyn_cfg = &reg_cfg_info->alg_reg_cfg[i].awblsc_reg_cfg.awblsc_reg_dyn_cfg;
    awblsc_reg_sta_cfg = &reg_cfg_info->alg_reg_cfg[i].awblsc_reg_cfg.awblsc_reg_sta_cfg;

    if (reg_cfg_info->cfg_key.bit1_awblsc_cfg) {
        isp_awblsc_threshold_max_write(be_reg, awblsc_reg_dyn_cfg->metering_white_level_awb);
        isp_awblsc_threshold_min_write(be_reg, awblsc_reg_dyn_cfg->metering_black_level_awb);
        isp_awblsc_cr_ref_max_write(be_reg, awblsc_reg_dyn_cfg->metering_cr_ref_max_awb);
        isp_awblsc_cr_ref_min_write(be_reg, awblsc_reg_dyn_cfg->metering_cr_ref_min_awb);
        isp_awblsc_cb_ref_max_write(be_reg, awblsc_reg_dyn_cfg->metering_cb_ref_max_awb);
        isp_awblsc_cb_ref_min_write(be_reg, awblsc_reg_dyn_cfg->metering_cb_ref_min_awb);

        isp_awblsc_crop_pos_x_write(be_reg, awblsc_reg_dyn_cfg->crop_pos_x);
        isp_awblsc_crop_pos_y_write(be_reg, awblsc_reg_dyn_cfg->crop_pos_y);
        isp_awblsc_crop_out_width_write(be_reg, awblsc_reg_dyn_cfg->crop_out_width - 1);
        isp_awblsc_crop_out_height_write(be_reg, awblsc_reg_dyn_cfg->crop_out_height - 1);
    }

    if (awblsc_reg_sta_cfg->awb_sta_cfg) {
        isp_awblsc_en_write(post_vi_proc, awblsc_reg_sta_cfg->awb_work_en);
        isp_awblsc_bitmove_write(be_reg, awblsc_reg_sta_cfg->awb_bitmove);

        isp_awblsc_stat_raddr_write(be_reg, awblsc_reg_sta_cfg->awb_stat_raddr);

        isp_awblsc_offset_comp_write(be_reg, awblsc_reg_sta_cfg->awb_offset_comp);

        isp_awblsc_hnum_write(be_reg, awblsc_reg_sta_cfg->zone_col);
        isp_awblsc_vnum_write(be_reg, awblsc_reg_sta_cfg->zone_row);

        awblsc_reg_sta_cfg->awb_sta_cfg = 0;
    }

    return TD_SUCCESS;
}

static td_void isp_sharpen_lumawgt_write(isp_post_be_reg_type *be_reg, td_u8 *luma_wgt)
{
    isp_sharpen_lumawgt0_write(be_reg, luma_wgt[0]);   /* array index 0 */
    isp_sharpen_lumawgt1_write(be_reg, luma_wgt[1]);   /* array index 1 */
    isp_sharpen_lumawgt2_write(be_reg, luma_wgt[2]);   /* array index 2 */
    isp_sharpen_lumawgt3_write(be_reg, luma_wgt[3]);   /* array index 3 */
    isp_sharpen_lumawgt4_write(be_reg, luma_wgt[4]);   /* array index 4 */
    isp_sharpen_lumawgt5_write(be_reg, luma_wgt[5]);   /* array index 5 */
    isp_sharpen_lumawgt6_write(be_reg, luma_wgt[6]);   /* array index 6 */
    isp_sharpen_lumawgt7_write(be_reg, luma_wgt[7]);   /* array index 7 */
    isp_sharpen_lumawgt8_write(be_reg, luma_wgt[8]);   /* array index 8 */
    isp_sharpen_lumawgt9_write(be_reg, luma_wgt[9]);   /* array index 9 */
    isp_sharpen_lumawgt10_write(be_reg, luma_wgt[10]); /* array index 10 */
    isp_sharpen_lumawgt11_write(be_reg, luma_wgt[11]); /* array index 11 */
    isp_sharpen_lumawgt12_write(be_reg, luma_wgt[12]); /* array index 12 */
    isp_sharpen_lumawgt13_write(be_reg, luma_wgt[13]); /* array index 13 */
    isp_sharpen_lumawgt14_write(be_reg, luma_wgt[14]); /* array index 14 */
    isp_sharpen_lumawgt15_write(be_reg, luma_wgt[15]); /* array index 15 */
    isp_sharpen_lumawgt16_write(be_reg, luma_wgt[16]); /* array index 16 */
    isp_sharpen_lumawgt17_write(be_reg, luma_wgt[17]); /* array index 17 */
    isp_sharpen_lumawgt18_write(be_reg, luma_wgt[18]); /* array index 18 */
    isp_sharpen_lumawgt19_write(be_reg, luma_wgt[19]); /* array index 19 */
    isp_sharpen_lumawgt20_write(be_reg, luma_wgt[20]); /* array index 20 */
    isp_sharpen_lumawgt21_write(be_reg, luma_wgt[21]); /* array index 21 */
    isp_sharpen_lumawgt22_write(be_reg, luma_wgt[22]); /* array index 22 */
    isp_sharpen_lumawgt23_write(be_reg, luma_wgt[23]); /* array index 23 */
    isp_sharpen_lumawgt24_write(be_reg, luma_wgt[24]); /* array index 24 */
    isp_sharpen_lumawgt25_write(be_reg, luma_wgt[25]); /* array index 25 */
    isp_sharpen_lumawgt26_write(be_reg, luma_wgt[26]); /* array index 26 */
    isp_sharpen_lumawgt27_write(be_reg, luma_wgt[27]); /* array index 27 */
    isp_sharpen_lumawgt28_write(be_reg, luma_wgt[28]); /* array index 28 */
    isp_sharpen_lumawgt29_write(be_reg, luma_wgt[29]); /* array index 29 */
    isp_sharpen_lumawgt30_write(be_reg, luma_wgt[30]); /* array index 30 */
    isp_sharpen_lumawgt31_write(be_reg, luma_wgt[31]); /* array index 31 */
}

static td_void isp_sharpen_mot_cfg_write(isp_post_be_reg_type *be_reg, isp_sharpen_mpi_dyna_reg_cfg *mpi_dyna_reg_cfg)
{
    isp_sharpen_osht_mot_amt_write(be_reg, mpi_dyna_reg_cfg->osht_mot_amt);
    isp_sharpen_usht_mot_amt_write(be_reg, mpi_dyna_reg_cfg->usht_mot_amt);
    isp_sharpen_mot_gain0_write(be_reg, mpi_dyna_reg_cfg->mot_gain0);
    isp_sharpen_mot_gain1_write(be_reg, mpi_dyna_reg_cfg->mot_gain1);
    isp_sharpen_mot_thd0_write(be_reg, mpi_dyna_reg_cfg->mot_thd0);
    isp_sharpen_mot_thd1_write(be_reg, mpi_dyna_reg_cfg->mot_thd1);
    isp_sharpen_mot_mul_write(be_reg, mpi_dyna_reg_cfg->mot_mul);
}
static td_void isp_sharpen_mpi_dyna_reg_config(isp_post_be_reg_type *be_reg,
    isp_sharpen_mpi_dyna_reg_cfg *mpi_dyna_reg_cfg)
{
    isp_sharpen_detl_oshtmul_write(be_reg, mpi_dyna_reg_cfg->detail_osht_mul);
    isp_sharpen_detl_ushtmul_write(be_reg, mpi_dyna_reg_cfg->detail_usht_mul);
    isp_sharpen_omaxgain_write(be_reg, mpi_dyna_reg_cfg->o_max_gain);
    isp_sharpen_umaxgain_write(be_reg, mpi_dyna_reg_cfg->u_max_gain);
    isp_sharpen_skinmaxu_write(be_reg, mpi_dyna_reg_cfg->skin_max_u);
    isp_sharpen_skinminu_write(be_reg, mpi_dyna_reg_cfg->skin_min_u);
    isp_sharpen_skinmaxv_write(be_reg, mpi_dyna_reg_cfg->skin_max_v);
    isp_sharpen_skinminv_write(be_reg, mpi_dyna_reg_cfg->skin_min_v);
    isp_sharpen_chrgmfmul_write(be_reg, mpi_dyna_reg_cfg->chr_gmf_mul);
    isp_sharpen_chrgmul_write(be_reg, mpi_dyna_reg_cfg->chr_g_mul);
    isp_sharpen_chrggain_write(be_reg, mpi_dyna_reg_cfg->chr_g_gain);
    isp_sharpen_chrgmfgain_write(be_reg, mpi_dyna_reg_cfg->chr_gmf_gain);

    isp_sharpen_chrbgain_write(be_reg, mpi_dyna_reg_cfg->chr_b_gain);
    isp_sharpen_chrbmul_write(be_reg, mpi_dyna_reg_cfg->chr_b_mul);
    isp_sharpen_chrrgain_write(be_reg, mpi_dyna_reg_cfg->chr_r_gain);
    isp_sharpen_chrrmul_write(be_reg, mpi_dyna_reg_cfg->chr_r_mul);
    isp_sharpen_benchrctrl_write(be_reg, mpi_dyna_reg_cfg->en_chr_ctrl);
    isp_sharpen_bendetailctrl_write(be_reg, mpi_dyna_reg_cfg->en_detail_ctrl);
    isp_sharpen_benlumactrl_write(be_reg, mpi_dyna_reg_cfg->en_luma_ctrl);
    isp_sharpen_benshtctrlbyvar_write(be_reg, mpi_dyna_reg_cfg->en_sht_ctrl_by_var);
    isp_sharpen_benskinctrl_write(be_reg, mpi_dyna_reg_cfg->en_skin_ctrl);
    isp_sharpen_weakdetailadj_write(be_reg, mpi_dyna_reg_cfg->en_weak_detail_adj);
    isp_sharpen_dirdiffsft_write(be_reg, mpi_dyna_reg_cfg->dir_diff_sft);
    isp_sharpen_dirrt0_write(be_reg, mpi_dyna_reg_cfg->dir_rt[0]); /* array index 0 */
    isp_sharpen_dirrt1_write(be_reg, mpi_dyna_reg_cfg->dir_rt[1]); /* array index 1 */
    isp_sharpen_lumawgt_write(be_reg, mpi_dyna_reg_cfg->luma_wgt);

    isp_sharpen_osht_dtl_thd0_write(be_reg, mpi_dyna_reg_cfg->detail_osht_thr[0]); /* array index 0 */
    isp_sharpen_osht_dtl_thd1_write(be_reg, mpi_dyna_reg_cfg->detail_osht_thr[1]); /* array index 1 */
    isp_sharpen_osht_dtl_wgt_write(be_reg, mpi_dyna_reg_cfg->detail_osht_amt);
    isp_sharpen_shtvarthd1_write(be_reg, mpi_dyna_reg_cfg->sht_var_thd1);
    isp_sharpen_oshtamt_write(be_reg, mpi_dyna_reg_cfg->osht_amt);
    isp_sharpen_ushtamt_write(be_reg, mpi_dyna_reg_cfg->usht_amt);
    isp_sharpen_shtbldrt_write(be_reg, mpi_dyna_reg_cfg->sht_bld_rt);
    isp_sharpen_shtvarmul_write(be_reg, mpi_dyna_reg_cfg->sht_var_mul);
    isp_sharpen_skinedgemul_write(be_reg, mpi_dyna_reg_cfg->skin_edge_mul);
    isp_sharpen_skinedgewgt0_write(be_reg, mpi_dyna_reg_cfg->skin_edge_wgt[0]);
    isp_sharpen_skinedgewgt1_write(be_reg, mpi_dyna_reg_cfg->skin_edge_wgt[1]);
    isp_sharpen_usht_dtl_thd0_write(be_reg, mpi_dyna_reg_cfg->detail_usht_thr[0]);
    isp_sharpen_usht_dtl_thd1_write(be_reg, mpi_dyna_reg_cfg->detail_usht_thr[1]);
    isp_sharpen_usht_dtl_wgt_write(be_reg, mpi_dyna_reg_cfg->detail_usht_amt);
    isp_sharpen_weakdetailgain_write(be_reg, mpi_dyna_reg_cfg->weak_detail_gain);
    /* mot */
    isp_sharpen_mot_cfg_write(be_reg, mpi_dyna_reg_cfg);
}

static td_void isp_sharpen_def_dyna_reg_config(isp_post_be_reg_type *be_reg,
    isp_sharpen_default_dyna_reg_cfg *def_dyna_reg_cfg)
{
    /* sharpen default iso */
    isp_sharpen_mhfthdsftd_write(be_reg, def_dyna_reg_cfg->gain_thd_sft_d);
    isp_sharpen_mhfthdselud_write(be_reg, def_dyna_reg_cfg->gain_thd_sel_ud);
    isp_sharpen_mhfthdsftud_write(be_reg, def_dyna_reg_cfg->gain_thd_sft_ud);
    isp_sharpen_dirvarsft_write(be_reg, def_dyna_reg_cfg->dir_var_sft);
    isp_sharpen_shtvarwgt0_write(be_reg, def_dyna_reg_cfg->sht_var_wgt0);
    isp_sharpen_shtvardiffthd0_write(be_reg, def_dyna_reg_cfg->sht_var_diff_thd[0]);
    isp_sharpen_selpixwgt_write(be_reg, def_dyna_reg_cfg->sel_pix_wgt);
    isp_sharpen_shtvardiffthd1_write(be_reg, def_dyna_reg_cfg->sht_var_diff_thd[1]);
    isp_sharpen_shtvardiffwgt1_write(be_reg, def_dyna_reg_cfg->sht_var_diff_wgt1);
    isp_sharpen_shtvardiffmul_write(be_reg, def_dyna_reg_cfg->sht_var_diff_mul);
    isp_sharpen_rmfscale_write(be_reg, def_dyna_reg_cfg->rmf_gain_scale);
    isp_sharpen_bmfscale_write(be_reg, def_dyna_reg_cfg->bmf_gain_scale);
    isp_sharpen_dirrlythrlow_write(be_reg, def_dyna_reg_cfg->dir_rly_thr_low);
    isp_sharpen_dirrlythrhih_write(be_reg, def_dyna_reg_cfg->dir_rly_thrhigh);
}

static td_void isp_sharpen_dyna_reg_config(isp_post_be_reg_type *be_reg, isp_sharpen_mpi_dyna_reg_cfg *mpi_dyna_reg_cfg,
    isp_sharpen_default_dyna_reg_cfg *def_dyna_reg_cfg)
{
    isp_sharpen_mpi_dyna_reg_config(be_reg, mpi_dyna_reg_cfg);
    isp_sharpen_def_dyna_reg_config(be_reg, def_dyna_reg_cfg);
}

static td_void isp_sharpen_dhsfcoef_d0_write(isp_post_be_reg_type *be_reg, td_s8 *hsf_coef_d0)
{
    isp_sharpen_dhsfcoef0_0_write(be_reg, hsf_coef_d0[0]);   /* array index 0 */
    isp_sharpen_dhsfcoef0_1_write(be_reg, hsf_coef_d0[1]);   /* array index 1 */
    isp_sharpen_dhsfcoef0_2_write(be_reg, hsf_coef_d0[2]);   /* array index 2 */
    isp_sharpen_dhsfcoef0_3_write(be_reg, hsf_coef_d0[3]);   /* array index 3 */
    isp_sharpen_dhsfcoef0_4_write(be_reg, hsf_coef_d0[4]);   /* array index 4 */
    isp_sharpen_dhsfcoef0_5_write(be_reg, hsf_coef_d0[5]);   /* array index 5 */
    isp_sharpen_dhsfcoef0_6_write(be_reg, hsf_coef_d0[6]);   /* array index 6 */
    isp_sharpen_dhsfcoef0_7_write(be_reg, hsf_coef_d0[7]);   /* array index 7 */
    isp_sharpen_dhsfcoef0_8_write(be_reg, hsf_coef_d0[8]);   /* array index 8 */
    isp_sharpen_dhsfcoef0_9_write(be_reg, hsf_coef_d0[9]);   /* array index 9 */
    isp_sharpen_dhsfcoef0_10_write(be_reg, hsf_coef_d0[10]); /* array index 10 */
    isp_sharpen_dhsfcoef0_11_write(be_reg, hsf_coef_d0[11]); /* array index 11 */
    isp_sharpen_dhsfcoef0_12_write(be_reg, hsf_coef_d0[12]); /* array index 12 */
}

static td_void isp_sharpen_dhsfcoef_d1_write(isp_post_be_reg_type *be_reg, td_s8 *hsf_coef_d1)
{
    isp_sharpen_dhsfcoef1_0_write(be_reg, hsf_coef_d1[0]);   /* array index 0 */
    isp_sharpen_dhsfcoef1_1_write(be_reg, hsf_coef_d1[1]);   /* array index 1 */
    isp_sharpen_dhsfcoef1_2_write(be_reg, hsf_coef_d1[2]);   /* array index 2 */
    isp_sharpen_dhsfcoef1_3_write(be_reg, hsf_coef_d1[3]);   /* array index 3 */
    isp_sharpen_dhsfcoef1_4_write(be_reg, hsf_coef_d1[4]);   /* array index 4 */
    isp_sharpen_dhsfcoef1_5_write(be_reg, hsf_coef_d1[5]);   /* array index 5 */
    isp_sharpen_dhsfcoef1_6_write(be_reg, hsf_coef_d1[6]);   /* array index 6 */
    isp_sharpen_dhsfcoef1_7_write(be_reg, hsf_coef_d1[7]);   /* array index 7 */
    isp_sharpen_dhsfcoef1_8_write(be_reg, hsf_coef_d1[8]);   /* array index 8 */
    isp_sharpen_dhsfcoef1_9_write(be_reg, hsf_coef_d1[9]);   /* array index 9 */
    isp_sharpen_dhsfcoef1_10_write(be_reg, hsf_coef_d1[10]); /* array index 10 */
    isp_sharpen_dhsfcoef1_11_write(be_reg, hsf_coef_d1[11]); /* array index 11 */
    isp_sharpen_dhsfcoef1_12_write(be_reg, hsf_coef_d1[12]); /* array index 12 */
}

static td_void isp_sharpen_dhsfcoef_d2_write(isp_post_be_reg_type *be_reg, td_s8 *hsf_coef_d2)
{
    isp_sharpen_dhsfcoef2_0_write(be_reg, hsf_coef_d2[0]);   /* array index 0 */
    isp_sharpen_dhsfcoef2_1_write(be_reg, hsf_coef_d2[1]);   /* array index 1 */
    isp_sharpen_dhsfcoef2_2_write(be_reg, hsf_coef_d2[2]);   /* array index 2 */
    isp_sharpen_dhsfcoef2_3_write(be_reg, hsf_coef_d2[3]);   /* array index 3 */
    isp_sharpen_dhsfcoef2_4_write(be_reg, hsf_coef_d2[4]);   /* array index 4 */
    isp_sharpen_dhsfcoef2_5_write(be_reg, hsf_coef_d2[5]);   /* array index 5 */
    isp_sharpen_dhsfcoef2_6_write(be_reg, hsf_coef_d2[6]);   /* array index 6 */
    isp_sharpen_dhsfcoef2_7_write(be_reg, hsf_coef_d2[7]);   /* array index 7 */
    isp_sharpen_dhsfcoef2_8_write(be_reg, hsf_coef_d2[8]);   /* array index 8 */
    isp_sharpen_dhsfcoef2_9_write(be_reg, hsf_coef_d2[9]);   /* array index 9 */
    isp_sharpen_dhsfcoef2_10_write(be_reg, hsf_coef_d2[10]); /* array index 10 */
    isp_sharpen_dhsfcoef2_11_write(be_reg, hsf_coef_d2[11]); /* array index 11 */
    isp_sharpen_dhsfcoef2_12_write(be_reg, hsf_coef_d2[12]); /* array index 12 */
}

static td_void isp_sharpen_dhsfcoef_d3_write(isp_post_be_reg_type *be_reg, td_s8 *hsf_coef_d3)
{
    isp_sharpen_dhsfcoef3_0_write(be_reg, hsf_coef_d3[0]);   /* array index 0 */
    isp_sharpen_dhsfcoef3_1_write(be_reg, hsf_coef_d3[1]);   /* array index 1 */
    isp_sharpen_dhsfcoef3_2_write(be_reg, hsf_coef_d3[2]);   /* array index 2 */
    isp_sharpen_dhsfcoef3_3_write(be_reg, hsf_coef_d3[3]);   /* array index 3 */
    isp_sharpen_dhsfcoef3_4_write(be_reg, hsf_coef_d3[4]);   /* array index 4 */
    isp_sharpen_dhsfcoef3_5_write(be_reg, hsf_coef_d3[5]);   /* array index 5 */
    isp_sharpen_dhsfcoef3_6_write(be_reg, hsf_coef_d3[6]);   /* array index 6 */
    isp_sharpen_dhsfcoef3_7_write(be_reg, hsf_coef_d3[7]);   /* array index 7 */
    isp_sharpen_dhsfcoef3_8_write(be_reg, hsf_coef_d3[8]);   /* array index 8 */
    isp_sharpen_dhsfcoef3_9_write(be_reg, hsf_coef_d3[9]);   /* array index 9 */
    isp_sharpen_dhsfcoef3_10_write(be_reg, hsf_coef_d3[10]); /* array index 10 */
    isp_sharpen_dhsfcoef3_11_write(be_reg, hsf_coef_d3[11]); /* array index 11 */
    isp_sharpen_dhsfcoef3_12_write(be_reg, hsf_coef_d3[12]); /* array index 12 */
}

static td_void isp_sharpen_lmt_hf_write(isp_post_be_reg_type *be_reg, td_u8 *lmt_hf)
{
    isp_sharpen_lmthf0_write(be_reg, lmt_hf[0]); /* array index 0 */
    isp_sharpen_lmthf1_write(be_reg, lmt_hf[1]); /* array index 1 */
    isp_sharpen_lmthf2_write(be_reg, lmt_hf[2]); /* array index 2 */
    isp_sharpen_lmthf3_write(be_reg, lmt_hf[3]); /* array index 3 */
    isp_sharpen_lmthf4_write(be_reg, lmt_hf[4]); /* array index 4 */
    isp_sharpen_lmthf5_write(be_reg, lmt_hf[5]); /* array index 5 */
    isp_sharpen_lmthf6_write(be_reg, lmt_hf[6]); /* array index 6 */
    isp_sharpen_lmthf7_write(be_reg, lmt_hf[7]); /* array index 7 */
}

static td_void isp_sharpen_lmt_mf_write(isp_post_be_reg_type *be_reg, td_u8 *lmt_mf)
{
    isp_sharpen_lmtmf0_write(be_reg, lmt_mf[0]); /* array index 0 */
    isp_sharpen_lmtmf1_write(be_reg, lmt_mf[1]); /* array index 1 */
    isp_sharpen_lmtmf2_write(be_reg, lmt_mf[2]); /* array index 2 */
    isp_sharpen_lmtmf3_write(be_reg, lmt_mf[3]); /* array index 3 */
    isp_sharpen_lmtmf4_write(be_reg, lmt_mf[4]); /* array index 4 */
    isp_sharpen_lmtmf5_write(be_reg, lmt_mf[5]); /* array index 5 */
    isp_sharpen_lmtmf6_write(be_reg, lmt_mf[6]); /* array index 6 */
    isp_sharpen_lmtmf7_write(be_reg, lmt_mf[7]); /* array index 7 */
}

static td_void isp_sharpen_chr_write(isp_post_be_reg_type *be_reg, isp_sharpen_static_reg_cfg *static_reg_cfg)
{
    isp_sharpen_chrrsft0_write(be_reg, static_reg_cfg->chr_r_sft[0]); /* array index 0 */
    isp_sharpen_chrrsft1_write(be_reg, static_reg_cfg->chr_r_sft[1]); /* array index 1 */
    isp_sharpen_chrrsft2_write(be_reg, static_reg_cfg->chr_r_sft[2]); /* array index 2 */
    isp_sharpen_chrrsft3_write(be_reg, static_reg_cfg->chr_r_sft[3]); /* array index 3 */
    isp_sharpen_chrrvarshift_write(be_reg, static_reg_cfg->chr_r_var_sft);

    isp_sharpen_chrbsft0_write(be_reg, static_reg_cfg->chr_b_sft[0]); /* array index 0 */
    isp_sharpen_chrbsft1_write(be_reg, static_reg_cfg->chr_b_sft[1]); /* array index 1 */
    isp_sharpen_chrbsft2_write(be_reg, static_reg_cfg->chr_b_sft[2]); /* array index 2 */
    isp_sharpen_chrbsft3_write(be_reg, static_reg_cfg->chr_b_sft[3]); /* array index 3 */
    isp_sharpen_chrbvarshift_write(be_reg, static_reg_cfg->chr_b_var_sft);
    isp_sharpen_chrgsft0_write(be_reg, static_reg_cfg->chr_g_sft[0]); /* array index 0 */
    isp_sharpen_chrgsft1_write(be_reg, static_reg_cfg->chr_g_sft[1]); /* array index 1 */
    isp_sharpen_chrgsft2_write(be_reg, static_reg_cfg->chr_g_sft[2]); /* array index 2 */
    isp_sharpen_chrgsft3_write(be_reg, static_reg_cfg->chr_g_sft[3]); /* array index 3 */

    isp_sharpen_chrgori0_write(be_reg, static_reg_cfg->chr_g_ori_cb);
    isp_sharpen_chrgori1_write(be_reg, static_reg_cfg->chr_g_ori_cr);
    isp_sharpen_chrgthd0_write(be_reg, static_reg_cfg->chr_g_thd[0]); /* array index 0 */
    isp_sharpen_chrgthd1_write(be_reg, static_reg_cfg->chr_g_thd[1]); /* array index 1 */
    isp_sharpen_chrrori0_write(be_reg, static_reg_cfg->chr_r_ori_cb);
    isp_sharpen_chrrori1_write(be_reg, static_reg_cfg->chr_r_ori_cr);
    isp_sharpen_chrrthd0_write(be_reg, static_reg_cfg->chr_r_thd[0]); /* array index 0 */
    isp_sharpen_chrrthd1_write(be_reg, static_reg_cfg->chr_r_thd[1]); /* array index 1 */

    isp_sharpen_chrbori0_write(be_reg, static_reg_cfg->chr_b_ori_cb);
    isp_sharpen_chrbori1_write(be_reg, static_reg_cfg->chr_b_ori_cr);
    isp_sharpen_chrbthd0_write(be_reg, static_reg_cfg->chr_b_thd[0]); /* array index 0 */
    isp_sharpen_chrbthd1_write(be_reg, static_reg_cfg->chr_b_thd[1]); /* array index 1 */
}

static td_void isp_sharpen_static_reg_config(isp_post_be_reg_type *be_reg, isp_sharpen_static_reg_cfg *static_reg_cfg)
{
    isp_sharpen_skincntthd0_write(be_reg, static_reg_cfg->skin_cnt_thd[0]); /* array index 0 */
    isp_sharpen_skincntthd1_write(be_reg, static_reg_cfg->skin_cnt_thd[1]); /* array index 1 */
    isp_sharpen_skincntmul_write(be_reg, static_reg_cfg->skin_cnt_mul);
    isp_sharpen_dhsfcoef_d0_write(be_reg, static_reg_cfg->hsf_coef_d0);
    isp_sharpen_dhsfcoef_d1_write(be_reg, static_reg_cfg->hsf_coef_d1);
    isp_sharpen_dhsfcoef_d2_write(be_reg, static_reg_cfg->hsf_coef_d2);
    isp_sharpen_dhsfcoef_d3_write(be_reg, static_reg_cfg->hsf_coef_d3);

    isp_sharpen_hfgain_sft_write(be_reg, static_reg_cfg->hf_gain_sft);
    isp_sharpen_mfgain_sft_write(be_reg, static_reg_cfg->mf_gain_sft);
    isp_sharpen_lfgainwgt_write(be_reg, static_reg_cfg->lf_gain_wgt);

    isp_sharpen_lmt_hf_write(be_reg, static_reg_cfg->lmt_hf);
    isp_sharpen_lmt_mf_write(be_reg, static_reg_cfg->lmt_mf);

    isp_sharpen_udhsfcoef0_write(be_reg, static_reg_cfg->hsf_coef_ud[0]); /* array index 0 */
    isp_sharpen_udhsfcoef1_write(be_reg, static_reg_cfg->hsf_coef_ud[1]); /* array index 1 */
    isp_sharpen_udhsfcoef2_write(be_reg, static_reg_cfg->hsf_coef_ud[2]); /* array index 2 */
    isp_sharpen_udhsfsft_write(be_reg, static_reg_cfg->hsf_sft_ud);
    isp_sharpen_omaxchg_write(be_reg, static_reg_cfg->o_max_chg);
    isp_sharpen_umaxchg_write(be_reg, static_reg_cfg->u_max_chg);
    isp_sharpen_dhsfsft_write(be_reg, static_reg_cfg->hsf_sft_d);
    isp_sharpen_chr_write(be_reg, static_reg_cfg);

    isp_sharpen_dlpfcoef0_write(be_reg, static_reg_cfg->lpf_coef_d[0]); /* array index 0 */
    isp_sharpen_dlpfcoef1_write(be_reg, static_reg_cfg->lpf_coef_d[1]); /* array index 1 */
    isp_sharpen_dlpfcoef2_write(be_reg, static_reg_cfg->lpf_coef_d[2]); /* array index 2 */
    isp_sharpen_dlpfsft_write(be_reg, static_reg_cfg->lpf_sft_d);
    isp_sharpen_udlpfcoef0_write(be_reg, static_reg_cfg->lpf_coef_ud[0]); /* array index 0 */
    isp_sharpen_udlpfcoef1_write(be_reg, static_reg_cfg->lpf_coef_ud[1]); /* array index 1 */
    isp_sharpen_udlpfcoef2_write(be_reg, static_reg_cfg->lpf_coef_ud[2]); /* array index 2 */
    isp_sharpen_udlpfsft_write(be_reg, static_reg_cfg->lpf_sft_ud);

    isp_sharpen_ben8dir_sel_write(be_reg, static_reg_cfg->en_shp8_dir);
    isp_sharpen_benshtvar_sel_write(be_reg, static_reg_cfg->sht_var_sel);
    isp_sharpen_detailthd_sel_write(be_reg, static_reg_cfg->detail_thd_sel);
    isp_sharpen_dirrly0_write(be_reg, static_reg_cfg->dir_rly[0]); /* array index 0 */
    isp_sharpen_dirrly1_write(be_reg, static_reg_cfg->dir_rly[1]); /* array index 1 */
    isp_sharpen_dirvarscale_write(be_reg, static_reg_cfg->dir_var_scale);
    isp_sharpen_mhfthdseld_write(be_reg, static_reg_cfg->gain_thd_sel_d);
    isp_sharpen_max_var_clip_write(be_reg, static_reg_cfg->max_var_clip_min);
    isp_sharpen_shtvarthd0_write(be_reg, static_reg_cfg->sht_var_thd0);
    isp_sharpen_shtvarwgt1_write(be_reg, static_reg_cfg->sht_var_wgt1);
    isp_sharpen_shtvardiffwgt0_write(be_reg, static_reg_cfg->sht_var_diff_wgt0);
    isp_sharpen_shtvar5x5_sft_write(be_reg, static_reg_cfg->sht_var5x5_sft);
    isp_sharpen_shtvarsft_write(be_reg, static_reg_cfg->sht_var_sft);
    isp_sharpen_skinedgesft_write(be_reg, static_reg_cfg->skin_edge_sft);
    isp_sharpen_skinedgethd0_write(be_reg, static_reg_cfg->skin_edge_thd[0]); /* array index 0 */
    isp_sharpen_skinedgethd1_write(be_reg, static_reg_cfg->skin_edge_thd[1]); /* array index 1 */
    isp_sharpen_dtl_thdsft_write(be_reg, static_reg_cfg->detail_thd_sft);
}

static td_s32 isp_sharpen_usr_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_post_be_reg_type *be_reg,
    isp_sharpen_mpi_dyna_reg_cfg *mpi_dyna_reg_cfg)
{
    td_u8 buf_id;
    td_s32 ret;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_viproc);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        if (mpi_dyna_reg_cfg->switch_mode != TD_TRUE) {
            /* online lut2stt regconfig */
            buf_id = mpi_dyna_reg_cfg->buf_id;
            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(be_lut_stt_reg);

            isp_sharpen_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, mpi_dyna_reg_cfg->mf_gain_d,
                mpi_dyna_reg_cfg->mf_gain_ud, mpi_dyna_reg_cfg->hf_gain_d, mpi_dyna_reg_cfg->hf_gain_ud);
            isp_sharpen_mot_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, mpi_dyna_reg_cfg->mf_mot_gain_d,
                mpi_dyna_reg_cfg->mf_mot_gain_ud, mpi_dyna_reg_cfg->hf_mot_gain_d, mpi_dyna_reg_cfg->hf_mot_gain_ud);

            ret = isp_sharpen_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
            if (ret != TD_SUCCESS) {
                isp_err_trace("ISP[%d] isp_sharpen_lut_wstt_addr_write failed\n", vi_pipe);
                return ret;
            }

            isp_sharpen_stt2lut_en_write(be_reg, TD_TRUE);
            isp_sharpen_stt2lut_regnew_write(be_reg, TD_TRUE);

            mpi_dyna_reg_cfg->buf_id = 1 - buf_id;
        }
    } else {
        isp_sharpen_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, mpi_dyna_reg_cfg->mf_gain_d,
            mpi_dyna_reg_cfg->mf_gain_ud, mpi_dyna_reg_cfg->hf_gain_d, mpi_dyna_reg_cfg->hf_gain_ud);
        isp_sharpen_mot_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, mpi_dyna_reg_cfg->mf_mot_gain_d,
            mpi_dyna_reg_cfg->mf_mot_gain_ud, mpi_dyna_reg_cfg->hf_mot_gain_d, mpi_dyna_reg_cfg->hf_mot_gain_ud);

        isp_sharpen_stt2lut_en_write(be_reg, TD_TRUE);
        isp_sharpen_stt2lut_regnew_write(be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_void isp_sharpen_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_sharpen_mpi_dyna_reg_cfg *mpi_dyna_reg_cfg)
{
    td_u32 sharpen_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        sharpen_stt2lut_clr = isp_sharpen_stt2lut_clr_read(be_reg);
        if (sharpen_stt2lut_clr != 0) {
            mpi_dyna_reg_cfg->resh = TD_TRUE;
            isp_sharpen_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_sharpen_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool usr_resh;
    td_bool idx_resh;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_sharpen_mpi_dyna_reg_cfg *mpi_dyna_reg_cfg = TD_NULL;
    isp_sharpen_static_reg_cfg *static_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_sharpen_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(post_viproc);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].sharpen_reg_cfg.static_reg_cfg;
        mpi_dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg;

        if (static_reg_cfg->static_resh) {
            isp_sharpen_lut_width_word_write(post_viproc, OT_ISP_SHARPEN_LUT_WIDTH_WORD_DEFAULT);

            isp_sharpen_static_reg_config(be_reg, static_reg_cfg);
            static_reg_cfg->static_resh = TD_FALSE;
        }

        idx_resh = (isp_sharpen_update_index_read(be_reg) != mpi_dyna_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (mpi_dyna_reg_cfg->resh && idx_resh) : (mpi_dyna_reg_cfg->resh);

        if (usr_resh) {
            isp_sharpen_update_index_write(be_reg, mpi_dyna_reg_cfg->update_index);

            ret = isp_sharpen_usr_reg_config(vi_pipe, i, be_reg, mpi_dyna_reg_cfg);
            if (ret != TD_SUCCESS) {
                return ret;
            }

            mpi_dyna_reg_cfg->resh = offline_mode;
        }

        isp_sharpen_reg_update_online(offline_mode, be_reg, mpi_dyna_reg_cfg);
        reg_cfg_info->cfg_key.bit1_sharpen_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_void isp_demosaic_lut_reg_config(const isp_usr_ctx *isp_ctx, isp_post_be_reg_type *be_reg,
    isp_demosaic_dyna_cfg *dyna_reg_cfg)
{
    td_u16 j;

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_nddm_gf_lut_waddr_write(be_reg, 0);

        for (j = 0; j < OT_ISP_DEMOSAIC_LUT_LENGTH; j++) {
            isp_nddm_gf_lut_wdata_write(be_reg, dyna_reg_cfg->gf_blur_luts[j]);
        }
    } else {
        isp_nddm_gflut_write(&be_reg->post_be_lut.post_be_apb_lut, dyna_reg_cfg->gf_blur_luts);
    }
}

static td_void isp_demosaic_static_reg_write(isp_post_be_reg_type *be_reg, isp_demosaic_static_cfg *static_reg_cfg)
{
    isp_demosaic_desat_enable_write(be_reg, static_reg_cfg->de_sat_enable);
    isp_demosaic_g_intp_control_write(be_reg, static_reg_cfg->g_intp_ctrl_enable);
    isp_demosaic_ahd_en_write(be_reg, static_reg_cfg->ahd_enable);
    isp_demosaic_de_fake_en_write(be_reg, static_reg_cfg->de_fake_enable);
    isp_demosaic_lpf_f3_write(be_reg, static_reg_cfg->lpff3);
    isp_demosaic_desat_thresh1_write(be_reg, static_reg_cfg->de_sat_thresh1);
    isp_demosaic_desat_thresh2_write(be_reg, static_reg_cfg->de_sat_thresh2);
    isp_demosaic_desat_hig_write(be_reg, static_reg_cfg->de_sat_hig);
    isp_demosaic_desat_protect_sl_write(be_reg, static_reg_cfg->de_sat_prot_sl);
    isp_demosaic_bld_limit1_write(be_reg, static_reg_cfg->hv_blend_limit1);
    isp_demosaic_bld_limit2_write(be_reg, static_reg_cfg->hv_blend_limit2);
    isp_demosaic_ahd_par1_write(be_reg, static_reg_cfg->ahd_part1);
    isp_demosaic_ahd_par2_write(be_reg, static_reg_cfg->ahd_part2);
    isp_demosaic_cx_var_max_rate_write(be_reg, static_reg_cfg->cx_var_max_rate);
    isp_demosaic_cx_var_min_rate_write(be_reg, static_reg_cfg->cx_var_min_rate);
    isp_demosaic_g_clip_sft_bit_write(be_reg, static_reg_cfg->g_clip_bit_sft);
    isp_demosaic_hv_ratio_write(be_reg, static_reg_cfg->hv_color_ratio);
    isp_demosaic_hv_sel_write(be_reg, static_reg_cfg->hv_selection);
    isp_demosaic_cbcr_avg_thld_write(be_reg, static_reg_cfg->cb_cr_avg_thr);
    isp_nddm_dith_mask_write(be_reg, static_reg_cfg->dither_mask);
    isp_nddm_dith_ratio_write(be_reg, static_reg_cfg->dither_ratio);
    isp_nddm_gf_th_low_write(be_reg, static_reg_cfg->gf_th_low);
    isp_nddm_gf_th_high_write(be_reg, static_reg_cfg->gf_th_hig);
    isp_demosaic_hlc_thresh_write(be_reg, 0x100);
}

static td_void isp_demosaic_dyna_reg_write(isp_post_be_reg_type *be_reg, isp_demosaic_dyna_cfg *dyna_reg_cfg)
{
    isp_demosaic_fake_cr_var_thr_high_write(be_reg, dyna_reg_cfg->fake_cr_var_thr_high);
    isp_demosaic_fake_cr_var_thr_low_write(be_reg, dyna_reg_cfg->fake_cr_var_thr_low);
    isp_demosaic_defcolor_cr_write(be_reg, dyna_reg_cfg->de_f_color_cr);
    isp_demosaic_defcolor_cb_write(be_reg, dyna_reg_cfg->de_f_color_cb);
    isp_demosaic_cc_hf_max_ratio_write(be_reg, dyna_reg_cfg->cc_hf_max_ratio);
    isp_demosaic_cc_hf_min_ratio_write(be_reg, dyna_reg_cfg->cc_hf_min_ratio);
    isp_demosaic_lpf_f0_write(be_reg, dyna_reg_cfg->lpff0);
    isp_demosaic_lpf_f1_write(be_reg, dyna_reg_cfg->lpff1);
    isp_demosaic_lpf_f2_write(be_reg, dyna_reg_cfg->lpff2);
    isp_demosaic_desat_low_write(be_reg, dyna_reg_cfg->de_sat_low);
    isp_demosaic_desat_protect_th_write(be_reg, dyna_reg_cfg->de_sat_prot_th);
    isp_nddm_dith_max_write(be_reg, dyna_reg_cfg->dither_max);
    isp_nddm_fcr_gf_gain_write(be_reg, dyna_reg_cfg->fcr_gf_gain);
    isp_nddm_awb_gf_gn_low_write(be_reg, dyna_reg_cfg->awb_gf_gain_low);
    isp_nddm_awb_gf_gn_high_write(be_reg, dyna_reg_cfg->awb_gf_gain_hig);
    isp_nddm_awb_gf_gn_max_write(be_reg, dyna_reg_cfg->awb_gf_gain_max);
    isp_nddm_ehc_gray_write(be_reg, dyna_reg_cfg->ehc_gray);
    isp_demosaic_hf_intp_blur_th1_write(be_reg, dyna_reg_cfg->hf_intp_blur_th_low);
    isp_demosaic_hf_intp_blur_th2_write(be_reg, dyna_reg_cfg->hf_intp_blur_th_hig);
    isp_nddm_fcr_det_low_write(be_reg, dyna_reg_cfg->fcr_det_low);
    isp_cc_soft_clip0_step_write(be_reg, dyna_reg_cfg->color_noise_str_y);
    isp_cc_darkprev_write(be_reg, OT_ISP_CCM_DARK_RANGE_MAX - dyna_reg_cfg->color_noise_thd_y);
}

static td_s32 isp_demosaic_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool gf_lut_update = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_demosaic_static_cfg *static_reg_cfg = TD_NULL;
    isp_demosaic_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_dem_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dem_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dem_reg_cfg.dyna_reg_cfg;

        if (static_reg_cfg->resh) { /* static */
            isp_demosaic_static_reg_write(be_reg, static_reg_cfg);
            reg_cfg_info->alg_reg_cfg[i].dem_reg_cfg.static_reg_cfg.resh = TD_FALSE;
        }
        if (dyna_reg_cfg->resh) { /* dynamic */
            isp_demosaic_dyna_reg_write(be_reg, dyna_reg_cfg);
            isp_demosaic_lut_reg_config(isp_ctx, be_reg, dyna_reg_cfg);
            gf_lut_update = dyna_reg_cfg->update_gf;
            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_dem_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.nddm_gf_lut_update = gf_lut_update || offline_mode;

    return TD_SUCCESS;
}

static td_s32 isp_fpn_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
#ifdef CONFIG_OT_VI_PIPE_FPN
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;

    isp_fpn_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_fpn_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].fpn_reg_cfg.dyna_reg_cfg;

        isp_fpn_overflowthr_write(be_reg, dyna_reg_cfg->isp_fpn_overflow_thr);
        isp_fpn1_overflowthr_write(be_reg, dyna_reg_cfg->isp_fpn_overflow_thr);
        isp_fpn_strength0_write(be_reg, dyna_reg_cfg->isp_fpn_strength[0]);
        isp_fpn1_strength0_write(be_reg, dyna_reg_cfg->isp_fpn_strength[1]);

        reg_cfg_info->cfg_key.bit1_fpn_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
#endif
    return TD_SUCCESS;
}

static td_s32 isp_ldci_read_stt_addr_write(isp_viproc_reg_type *post_viproc, td_u64 phy_addr)
{
    if (phy_addr == 0) {
        return TD_FAILURE;
    }

    viproc_para_dci_addr_low_write(post_viproc, get_low_addr(phy_addr));

    return TD_SUCCESS;
}

static td_s32 isp_ldci_static_lut_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_post_be_reg_type *be_reg,
    isp_ldci_static_cfg *static_reg_cfg, isp_ldci_dyna_cfg *dyna_reg_cfg)
{
    td_u16 j, k;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_ldci_poply_waddr_write(be_reg, 0);
        isp_ldci_plyq01_waddr_write(be_reg, 0);
        isp_ldci_plyq23_waddr_write(be_reg, 0);

        for (j = 0; j < 65; j++) { /* ldci lut table number 65 */
            isp_ldci_poply_wdata_write(be_reg, static_reg_cfg->poly_p1[j], static_reg_cfg->stat_poly_p2[j],
                static_reg_cfg->stat_poly_p3[j]);
            isp_ldci_plyq01_wdata_write(be_reg, static_reg_cfg->poly_q0[j], static_reg_cfg->poly_q1[j]);
            isp_ldci_plyq23_wdata_write(be_reg, static_reg_cfg->poly_q2[j], static_reg_cfg->poly_q3[j]);
        }

        for (k = 0; k < 2; k++) { /* config all 2 lut2stt buffer for static lut */
            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, k);
            isp_check_pointer_return(be_lut_stt_reg);
            isp_ldci_drc_cgain_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, static_reg_cfg->calc_dyn_rng_cmp_lut,
                static_reg_cfg->stat_dyn_rng_cmp_lut, dyna_reg_cfg->color_gain_lut);
            isp_ldci_usm_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, dyna_reg_cfg->usm_pos_lut,
                dyna_reg_cfg->usm_neg_lut);
        }
    } else {
        isp_ldci_poply1_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->poly_p1);
        isp_ldci_poply2_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->stat_poly_p2);
        isp_ldci_poply3_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->stat_poly_p3);
        isp_ldci_plyq0_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->poly_q0);
        isp_ldci_plyq1_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->poly_q1);
        isp_ldci_plyq2_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->poly_q2);
        isp_ldci_plyq3_wlut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->poly_q3);

        isp_ldci_drc_cgain_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, static_reg_cfg->calc_dyn_rng_cmp_lut,
            static_reg_cfg->stat_dyn_rng_cmp_lut, dyna_reg_cfg->color_gain_lut);
        isp_ldci_usm_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, dyna_reg_cfg->usm_pos_lut,
            dyna_reg_cfg->usm_neg_lut);
    }

    return TD_SUCCESS;
}

static td_s32 isp_ldci_tpr_flt_attr_config(ot_vi_pipe vi_pipe, td_u8 i, isp_post_be_reg_type *be_reg,
    isp_viproc_reg_type *post_viproc, isp_ldci_dyna_cfg *dyna_reg_cfg)
{
    td_bool rdstat_en;
    td_u8 read_buf_idx;
    td_u8 blk_num;
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_ldci_read_stt_buf *ldci_read_stt_buf = TD_NULL;
    isp_ldci_stat *read_stt_buf = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    blk_num = isp_ctx->block_attr.block_num;

    if (isp_ctx->ldci_tpr_flt_en == TD_TRUE) {
        ldci_buf_get_ctx(vi_pipe, ldci_read_stt_buf);
        read_buf_idx = ldci_read_stt_buf->buf_idx;

        if (i == 0) {
            read_stt_buf = (isp_ldci_stat *)isp_get_ldci_read_stt_vir_addr(vi_pipe, read_buf_idx);
            isp_check_pointer_return(read_stt_buf);
            (td_void)memcpy_s(read_stt_buf, sizeof(isp_ldci_stat), &dyna_reg_cfg->tpr_stat, sizeof(isp_ldci_stat));
        }

        if ((i + 1) == blk_num) {
            ldci_read_stt_buf->buf_idx = (read_buf_idx + 1) % div_0_to_1(ldci_read_stt_buf->buf_num);
        }

        /* Set ReadStt Addr */
        ret = isp_ldci_read_stt_addr_write(post_viproc, ldci_read_stt_buf->read_buf[read_buf_idx].phy_addr);
        rdstat_en = (ret == TD_SUCCESS) ? (dyna_reg_cfg->rdstat_en) : (TD_FALSE);
        isp_ldci_rdstat_en_write(be_reg, rdstat_en);
    } else {
        isp_ldci_rdstat_en_write(be_reg, dyna_reg_cfg->rdstat_en);
    }

    return TD_SUCCESS;
}

static td_s32 isp_ldci_dyna_lut_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_post_be_reg_type *be_reg,
    isp_viproc_reg_type *post_viproc, isp_ldci_dyna_cfg *dyna_reg_cfg)
{
    td_u8 buf_id;
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        /* online Lut2stt regconfig */
        buf_id = dyna_reg_cfg->buf_id;

        be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_return(be_lut_stt_reg);

        isp_ldci_he_delut_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, dyna_reg_cfg->de_lut,
            dyna_reg_cfg->he_pos_lut, dyna_reg_cfg->he_neg_lut);
        ret = isp_ldci_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
        if (ret != TD_SUCCESS) {
            isp_err_trace("isp[%d]_ldci_lut_wstt_addr_write failed\n", vi_pipe);
            return ret;
        }
        dyna_reg_cfg->buf_id = 1 - buf_id;
    } else {
        isp_ldci_he_delut_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, dyna_reg_cfg->de_lut,
            dyna_reg_cfg->he_pos_lut, dyna_reg_cfg->he_neg_lut);
    }

    isp_ldci_stt2lut_en_write(be_reg, TD_TRUE);
    isp_ldci_stt2lut_regnew_write(be_reg, TD_TRUE);

    return TD_SUCCESS;
}

static td_s32 isp_ldci_static_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_post_be_reg_type *be_reg)
{
    td_s32 ret;
    isp_ldci_static_cfg *static_reg_cfg = TD_NULL;
    isp_ldci_dyna_cfg *dyna_reg_cfg = TD_NULL;

    static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ldci_reg_cfg.static_reg_cfg;
    dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ldci_reg_cfg.dyna_reg_cfg;

    isp_ldci_stat_evratio_write(be_reg, 0x1000);
    isp_ldci_luma_sel_write(be_reg, static_reg_cfg->calc_luma_sel);
    isp_ldci_bende_write(be_reg, static_reg_cfg->de_enable);
    isp_ldci_deref_write(be_reg, static_reg_cfg->calc_de_ref);
    isp_ldci_deh_lpfsft_write(be_reg, static_reg_cfg->deh_lpf_sft);
    isp_ldci_coefh0_write(be_reg, static_reg_cfg->deh_lpf_coef_h[0]); /* array index 0 */
    isp_ldci_coefh1_write(be_reg, static_reg_cfg->deh_lpf_coef_h[1]); /* array index 1 */
    isp_ldci_coefh2_write(be_reg, static_reg_cfg->deh_lpf_coef_h[2]); /* array index 2 */
    isp_ldci_coefv0_write(be_reg, static_reg_cfg->deh_lpf_coef_v[0]); /* array index 0 */
    isp_ldci_coefv1_write(be_reg, static_reg_cfg->deh_lpf_coef_v[1]); /* array index 1 */
    isp_ldci_calc_drcen_write(be_reg, static_reg_cfg->calc_drc_enable);
    isp_ldci_pflmt_en_write(be_reg, static_reg_cfg->calc_pf_lmt_enable);
    isp_ldci_pfori0_write(be_reg, static_reg_cfg->calc_pf_ori[0]); /* array index 0 */
    isp_ldci_pfori1_write(be_reg, static_reg_cfg->calc_pf_ori[1]); /* array index 1 */
    isp_ldci_pfsft0_write(be_reg, static_reg_cfg->calc_pf_sft[0]); /* array index 0 */
    isp_ldci_pfsft1_write(be_reg, static_reg_cfg->calc_pf_sft[1]); /* array index 1 */
    isp_ldci_pfsft2_write(be_reg, static_reg_cfg->calc_pf_sft[2]); /* array index 2 */
    isp_ldci_pfsft3_write(be_reg, static_reg_cfg->calc_pf_sft[3]); /* array index 3 */
    isp_ldci_pfthd0_write(be_reg, static_reg_cfg->calc_pf_thd[0]); /* array index 0 */
    isp_ldci_pfthd1_write(be_reg, static_reg_cfg->calc_pf_thd[1]); /* array index 1 */
    isp_ldci_pfrly0_write(be_reg, static_reg_cfg->calc_pf_rly[0]); /* array index 0 */
    isp_ldci_pfrly1_write(be_reg, static_reg_cfg->calc_pf_rly[1]); /* array index 1 */
    isp_ldci_pfmul_write(be_reg, static_reg_cfg->calc_pfmul);
    isp_ldci_lpfsft_write(be_reg, static_reg_cfg->lpf_sft);
    isp_ldci_stat_drcen_write(be_reg, static_reg_cfg->stat_drc_enable);
    isp_ldci_chrposdamp_write(be_reg, static_reg_cfg->chr_pos_damp);
    isp_ldci_chrnegdamp_write(be_reg, static_reg_cfg->chr_neg_damp);
    isp_ldci_glb_hewgt_write(be_reg, static_reg_cfg->global_he_wgt);

    ret = isp_ldci_static_lut_reg_config(vi_pipe, i, be_reg, static_reg_cfg, dyna_reg_cfg);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 isp_ldci_dyna_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_post_be_reg_type *be_reg,
    isp_viproc_reg_type *post_viproc, isp_ldci_dyna_cfg *dyna_reg_cfg)
{
    td_s32 ret;
    ret = isp_ldci_tpr_flt_attr_config(vi_pipe, i, be_reg, post_viproc, dyna_reg_cfg);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    isp_ldci_wrstat_en_write(be_reg, dyna_reg_cfg->wrstat_en);
    isp_ldci_calc_en_write(be_reg, dyna_reg_cfg->calc_enable);
    isp_ldci_calc_map_offsetx_write(be_reg, dyna_reg_cfg->calc_map_offset_x);
    isp_ldci_smlmapstride_write(be_reg, dyna_reg_cfg->calc_sml_map_stride);
    isp_ldci_smlmapheight_write(be_reg, dyna_reg_cfg->calc_sml_map_height);
    isp_ldci_total_zone_write(be_reg, dyna_reg_cfg->calc_total_zone);
    isp_ldci_scalex_write(be_reg, dyna_reg_cfg->calc_scale_x);
    isp_ldci_scaley_write(be_reg, dyna_reg_cfg->calc_scale_y);
    isp_ldci_stat_smlmapwidth_write(be_reg, dyna_reg_cfg->stat_sml_map_width);
    isp_ldci_stat_smlmapheight_write(be_reg, dyna_reg_cfg->stat_sml_map_height);
    isp_ldci_stat_total_zone_write(be_reg, dyna_reg_cfg->stat_total_zone);
    isp_ldci_blk_smlmapwidth0_write(be_reg, dyna_reg_cfg->blk_sml_map_width[0]); /* array index 0 */
    isp_ldci_blk_smlmapwidth1_write(be_reg, dyna_reg_cfg->blk_sml_map_width[1]); /* array index 1 */
    isp_ldci_blk_smlmapwidth2_write(be_reg, dyna_reg_cfg->blk_sml_map_width[2]); /* array index 2 */
    isp_ldci_hstart_write(be_reg, dyna_reg_cfg->stat_h_start);
    isp_ldci_hend_write(be_reg, dyna_reg_cfg->stat_h_end);
    isp_ldci_vstart_write(be_reg, dyna_reg_cfg->stat_v_start);
    isp_ldci_vend_write(be_reg, dyna_reg_cfg->stat_v_end);

    ret = isp_ldci_dyna_lut_reg_config(vi_pipe, i, be_reg, post_viproc, dyna_reg_cfg);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_void isp_ldci_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_ldci_static_cfg *static_reg_cfg)
{
    td_u32 ldci_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        ldci_stt2lut_clr = isp_ldci_stt2lut_clr_read(be_reg);
        if (ldci_stt2lut_clr != 0) {
            static_reg_cfg->static_resh = TD_TRUE;
            isp_ldci_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_ldci_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool ldci_drc_lut_update = TD_FALSE;
    td_bool cur_ldci_drc_lut_reg_new = TD_FALSE;
    td_bool ldci_calc_lut_update = TD_FALSE;
    td_bool cur_ldci_calc_lut_reg_new;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_ldci_static_cfg *static_reg_cfg = TD_NULL;
    isp_ldci_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_ldci_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(post_viproc);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ldci_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ldci_reg_cfg.dyna_reg_cfg;

        if (static_reg_cfg->static_resh) {
            isp_ldci_lut_width_word_write(post_viproc, OT_ISP_LDCI_LUT_WIDTH_WORD_DEFAULT);

            ret = isp_ldci_static_reg_config(vi_pipe, reg_cfg_info, i, be_reg);
            if (ret != TD_SUCCESS) {
                return ret;
            }

            cur_ldci_drc_lut_reg_new = TD_TRUE;
            static_reg_cfg->static_resh = TD_FALSE;
        }

        ldci_drc_lut_update = static_reg_cfg->pre_drc_lut_update;
        static_reg_cfg->pre_drc_lut_update = cur_ldci_drc_lut_reg_new;

        /* dynamic */
        isp_ldci_en_write(post_viproc, dyna_reg_cfg->enable);
        ret = isp_ldci_dyna_reg_config(vi_pipe, i, be_reg, post_viproc, dyna_reg_cfg);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        cur_ldci_calc_lut_reg_new = TD_TRUE;
        ldci_calc_lut_update = dyna_reg_cfg->pre_calc_lut_reg_new;
        dyna_reg_cfg->pre_calc_lut_reg_new = cur_ldci_calc_lut_reg_new;

        isp_ldci_reg_update_online(offline_mode, be_reg, static_reg_cfg);
        reg_cfg_info->cfg_key.bit1_ldci_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.ldci_calc_lut_update = ldci_calc_lut_update || offline_mode;
    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.ldci_drc_lut_update = ldci_drc_lut_update || offline_mode;

    return TD_SUCCESS;
}

static td_void isp_lcac_lut_reg_config(td_bool *lut_update, const isp_usr_ctx *isp_ctx, isp_post_be_reg_type *be_reg,
    isp_local_cac_static_cfg *static_reg_cfg)
{
    td_u8 j;

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_demosaic_depurplut_waddr_write(be_reg, 0);

        for (j = 0; j < LCAC_STRENGTH_NUM; j++) {
            isp_demosaic_depurplut_wdata_write(be_reg, static_reg_cfg->de_purple_str[j]);
        }

        *lut_update = TD_TRUE;
    } else {
        isp_demosaic_depurp_lut_write(&be_reg->post_be_lut.post_be_apb_lut, static_reg_cfg->de_purple_str);
    }
}
static td_void isp_lcac_static_reg_write(isp_post_be_reg_type *be_reg, isp_local_cac_static_cfg *static_reg_cfg)
{
    isp_nddm_cac_blend_en_write(be_reg, static_reg_cfg->nddm_cac_blend_en);
    isp_demosaic_newlcacproc_write(be_reg, static_reg_cfg->new_lcac_proc);
    isp_nddm_cac_blend_rate_write(be_reg, static_reg_cfg->nddm_cac_blend_rate);
    isp_demosaic_r_counter_thr_write(be_reg, static_reg_cfg->r_counter_thr);
    isp_demosaic_g_counter_thr_write(be_reg, static_reg_cfg->g_counter_thr);
    isp_demosaic_b_counter_thr_write(be_reg, static_reg_cfg->b_counter_thr);
    isp_demosaic_satu_thr_write(be_reg, static_reg_cfg->satu_thr);
    isp_demosaic_cbcr_ratio_high_limit2_write(be_reg, static_reg_cfg->cb_cr_ratio_lmt_high2);
    isp_demosaic_cbcr_ratio_low_limit2_write(be_reg, static_reg_cfg->cb_cr_ratio_lmt_low2);
}

static td_void isp_lcac_user_reg_write(isp_post_be_reg_type *be_reg, isp_local_cac_usr_cfg *usr_reg_cfg)
{
    isp_lcac_update_index_write(be_reg, usr_reg_cfg->update_index);
    isp_demosaic_purple_var_thr_write(be_reg, usr_reg_cfg->var_thr);
    isp_demosaic_cb_thr_write(be_reg, usr_reg_cfg->cb_thr);
    isp_demosaic_cac_luma_high_cnt_thr_write(be_reg, usr_reg_cfg->luma_high_cnt_thr);
    isp_demosaic_cac_cb_cnt_low_thr_write(be_reg, usr_reg_cfg->cb_cnt_low_thr);
    isp_demosaic_cac_cb_cnt_high_thr_write(be_reg, usr_reg_cfg->cb_cnt_high_thr);
    isp_demosaci_cac_bld_avg_cur_write(be_reg, usr_reg_cfg->bld_avg_cur);
    isp_demosaic_cbcr_ratio_high_limit_write(be_reg, usr_reg_cfg->cb_cr_ratio_lmt_high);
}

static td_s32 isp_lcac_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool idx_resh;
    td_bool usr_resh;
    td_bool lut_update = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_local_cac_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_local_cac_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_local_cac_static_cfg *static_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));
    if (reg_cfg_info->cfg_key.bit1_local_cac_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        /* static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].lcac_reg_cfg.static_reg_cfg;
        if (static_reg_cfg->static_resh) {
            isp_lcac_static_reg_write(be_reg, static_reg_cfg);
            isp_lcac_lut_reg_config(&lut_update, isp_ctx, be_reg, static_reg_cfg);
            static_reg_cfg->static_resh = TD_FALSE;
        }
        /* Usr */
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].lcac_reg_cfg.usr_reg_cfg;
        idx_resh = (isp_lcac_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (usr_reg_cfg->resh && idx_resh) : (usr_reg_cfg->resh);
        if (usr_resh) {
            isp_lcac_user_reg_write(be_reg, usr_reg_cfg);
            usr_reg_cfg->resh = offline_mode;
        }
        /* dynamic */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].lcac_reg_cfg.dyna_reg_cfg;
        if (dyna_reg_cfg->resh) {
            isp_demosaic_r_luma_thr_write(be_reg, dyna_reg_cfg->r_luma_thr);
            isp_demosaic_g_luma_thr_write(be_reg, dyna_reg_cfg->g_luma_thr);
            isp_demosaic_b_luma_thr_write(be_reg, dyna_reg_cfg->b_luma_thr);
            isp_demosaic_luma_thr_write(be_reg, dyna_reg_cfg->luma_thr);
            isp_demosaic_cbcr_ratio_low_limit_write(be_reg, dyna_reg_cfg->cb_cr_ratio_lmt_low);
            isp_demosaic_depurplectrcr_write(be_reg, dyna_reg_cfg->de_purple_ctr_cr);
            isp_demosaic_depurplectrcb_write(be_reg, dyna_reg_cfg->de_purple_ctr_cb);
            dyna_reg_cfg->resh = offline_mode;
        }
        reg_cfg_info->cfg_key.bit1_local_cac_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.lcac_lut_update = lut_update || offline_mode;
    return TD_SUCCESS;
}

static td_s32 isp_fcr_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    isp_antifalsecolor_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_antifalsecolor_static_cfg *static_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_fcr_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].anti_false_color_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].anti_false_color_reg_cfg.dyna_reg_cfg;

        /* static */
        if (static_reg_cfg->resh) {
            isp_demosaic_fcr_limit1_write(be_reg, static_reg_cfg->fcr_limit1);
            isp_demosaic_fcr_limit2_write(be_reg, static_reg_cfg->fcr_limit2);
            isp_demosaic_fcr_thr_write(be_reg, static_reg_cfg->fcr_thr);
            static_reg_cfg->resh = TD_FALSE;
        }

        /* dynamic */
        if (dyna_reg_cfg->resh) {
            isp_demosaic_fcr_gain_write(be_reg, dyna_reg_cfg->fcr_gain);
            isp_demosaic_fcr_ratio_write(be_reg, dyna_reg_cfg->fcr_ratio);
            isp_demosaic_fcr_gray_ratio_write(be_reg, dyna_reg_cfg->fcr_gray_ratio);
            isp_demosaic_fcr_cmax_sel_write(be_reg, dyna_reg_cfg->fcr_cmax_sel);
            isp_demosaic_fcr_detg_sel_write(be_reg, dyna_reg_cfg->fcr_detg_sel);
            isp_demosaic_fcr_thresh1_write(be_reg, dyna_reg_cfg->fcr_hf_thresh_low);
            isp_demosaic_fcr_thresh2_write(be_reg, dyna_reg_cfg->fcr_hf_thresh_hig);
            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_fcr_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_void isp_acac_user_reg_write(isp_post_be_reg_type *be_reg, isp_acac_usr_cfg *usr_reg_cfg)
{
    isp_gcac_update_index_write(be_reg, usr_reg_cfg->update_index);
    isp_gcac_edgemode_write(be_reg, usr_reg_cfg->cac_det_mode);
    isp_gcac_cacglobalstr_write(be_reg, usr_reg_cfg->cac_str);
    isp_gcac_cac_alphar_write(be_reg, usr_reg_cfg->alpha_r);
    isp_gcac_cac_alphab_write(be_reg, usr_reg_cfg->alpha_b);
    isp_gcac_cac_colordifthd_write(be_reg, usr_reg_cfg->color_dif_thd);
    isp_gcac_cac_greythd_write(be_reg, usr_reg_cfg->grey_thd);
    isp_gcac_caclamdath1_write(be_reg, usr_reg_cfg->lamda_thd0);
    isp_gcac_caclamdath2_write(be_reg, usr_reg_cfg->lamda_thd1);
    isp_gcac_caclamdamul_write(be_reg, usr_reg_cfg->lamda_mul);
    isp_gcac_det_satuthr_write(be_reg, usr_reg_cfg->det_satu_thr);
    isp_gcac_cbcr_ratio_high_limit_write(be_reg, usr_reg_cfg->fcf_cr_cb_ratio_high_limit);
    isp_gcac_cbcr_ratio_low_limit_write(be_reg, usr_reg_cfg->fcf_cr_cb_ratio_low_limit);
    isp_gcac_cbcr_ratio_high_limit2_write(be_reg, usr_reg_cfg->fcf_cr_cb_ratio_high_limit2);
    isp_gcac_cbcr_ratio_low_limit2_write(be_reg, usr_reg_cfg->fcf_cr_cb_ratio_low_limit2);
    isp_gcac_edgealpha_write(be_reg, usr_reg_cfg->edge_alpha);
    isp_gcac_purplealpha_write(be_reg, usr_reg_cfg->purple_alpha);
}

static td_s32 isp_acac_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool usr_resh = TD_FALSE;
    td_u8 idx_resh;
    isp_acac_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_acac_static_cfg *static_reg_cfg = TD_NULL;
    isp_acac_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));
    if (reg_cfg_info->cfg_key.bit1_acac_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_gcac_en_write(be_reg, reg_cfg_info->alg_reg_cfg[i].acac_reg_cfg.acac_en);
        /* static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].acac_reg_cfg.static_reg_cfg;
        if (static_reg_cfg->static_resh) {
            isp_gcac_cacnormfactor_write(be_reg, static_reg_cfg->cac_norm_factor);
            isp_gcac_cac_tao_write(be_reg, static_reg_cfg->cac_tao);
            static_reg_cfg->static_resh = TD_FALSE;
        }
        /* usr */
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].acac_reg_cfg.usr_reg_cfg;
        idx_resh = (isp_gcac_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? ((td_u8)usr_reg_cfg->usr_resh & idx_resh) : (usr_reg_cfg->usr_resh);
        if (usr_resh) {
            isp_acac_user_reg_write(be_reg, usr_reg_cfg);
            usr_reg_cfg->usr_resh = offline_mode;
        }
        /* dyna */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].acac_reg_cfg.dyna_reg_cfg;
        if (dyna_reg_cfg->dyna_resh) {
            isp_gcac_cacthrbmul_write(be_reg, dyna_reg_cfg->edge_thd_mul);
            isp_gcac_cacthrb1_write(be_reg, dyna_reg_cfg->edge_thd0);
            isp_gcac_cacthrb2_write(be_reg, dyna_reg_cfg->edge_thd1);
            dyna_reg_cfg->dyna_resh = TD_FALSE;
        }
        reg_cfg_info->cfg_key.bit1_acac_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
    return TD_SUCCESS;
}


static td_void isp_bshp_static_f0to1_reg_write(isp_post_be_reg_type *be_reg, isp_bshp_static_cfg *static_reg_cfg)
{
    isp_bnrshp_hsfcoef0d0_write(be_reg, static_reg_cfg->hsf_coef0_d[0]);   /* idx 0 */
    isp_bnrshp_hsfcoef0d1_write(be_reg, static_reg_cfg->hsf_coef0_d[1]);   /* idx 1 */
    isp_bnrshp_hsfcoef0d2_write(be_reg, static_reg_cfg->hsf_coef0_d[2]);   /* idx 2 */
    isp_bnrshp_hsfcoef0d3_write(be_reg, static_reg_cfg->hsf_coef0_d[3]);   /* idx 3 */
    isp_bnrshp_hsfcoef0d4_write(be_reg, static_reg_cfg->hsf_coef0_d[4]);   /* idx 4 */
    isp_bnrshp_hsfcoef0d5_write(be_reg, static_reg_cfg->hsf_coef0_d[5]);   /* idx 5 */
    isp_bnrshp_hsfcoef0d6_write(be_reg, static_reg_cfg->hsf_coef0_d[6]);   /* idx 6 */
    isp_bnrshp_hsfcoef0d7_write(be_reg, static_reg_cfg->hsf_coef0_d[7]);   /* idx 7 */
    isp_bnrshp_hsfcoef0d8_write(be_reg, static_reg_cfg->hsf_coef0_d[8]);   /* idx 8 */
    isp_bnrshp_hsfcoef0d9_write(be_reg, static_reg_cfg->hsf_coef0_d[9]);   /* idx 9 */
    isp_bnrshp_hsfcoef0d10_write(be_reg, static_reg_cfg->hsf_coef0_d[10]); /* idx 10 */
    isp_bnrshp_hsfcoef0d11_write(be_reg, static_reg_cfg->hsf_coef0_d[11]); /* idx 11 */
    isp_bnrshp_hsfcoef0d12_write(be_reg, static_reg_cfg->hsf_coef0_d[12]); /* idx 12 */

    isp_bnrshp_hsfcoef1d0_write(be_reg, static_reg_cfg->hsf_coef1_d[0]);   /* idx 0 */
    isp_bnrshp_hsfcoef1d1_write(be_reg, static_reg_cfg->hsf_coef1_d[1]);   /* idx 1 */
    isp_bnrshp_hsfcoef1d2_write(be_reg, static_reg_cfg->hsf_coef1_d[2]);   /* idx 2 */
    isp_bnrshp_hsfcoef1d3_write(be_reg, static_reg_cfg->hsf_coef1_d[3]);   /* idx 3 */
    isp_bnrshp_hsfcoef1d4_write(be_reg, static_reg_cfg->hsf_coef1_d[4]);   /* idx 4 */
    isp_bnrshp_hsfcoef1d5_write(be_reg, static_reg_cfg->hsf_coef1_d[5]);   /* idx 5 */
    isp_bnrshp_hsfcoef1d6_write(be_reg, static_reg_cfg->hsf_coef1_d[6]);   /* idx 6 */
    isp_bnrshp_hsfcoef1d7_write(be_reg, static_reg_cfg->hsf_coef1_d[7]);   /* idx 7 */
    isp_bnrshp_hsfcoef1d8_write(be_reg, static_reg_cfg->hsf_coef1_d[8]);   /* idx 8 */
    isp_bnrshp_hsfcoef1d9_write(be_reg, static_reg_cfg->hsf_coef1_d[9]);   /* idx 9 */
    isp_bnrshp_hsfcoef1d10_write(be_reg, static_reg_cfg->hsf_coef1_d[10]); /* idx 10 */
    isp_bnrshp_hsfcoef1d11_write(be_reg, static_reg_cfg->hsf_coef1_d[11]); /* idx 11 */
    isp_bnrshp_hsfcoef1d12_write(be_reg, static_reg_cfg->hsf_coef1_d[12]); /* idx 12 */

    return;
}

static td_void isp_bshp_static_f2to3_reg_write(isp_post_be_reg_type *be_reg, isp_bshp_static_cfg *static_reg_cfg)
{
    isp_bnrshp_hsfcoef2d0_write(be_reg, static_reg_cfg->hsf_coef2_d[0]);   /* idx 0 */
    isp_bnrshp_hsfcoef2d1_write(be_reg, static_reg_cfg->hsf_coef2_d[1]);   /* idx 1 */
    isp_bnrshp_hsfcoef2d2_write(be_reg, static_reg_cfg->hsf_coef2_d[2]);   /* idx 2 */
    isp_bnrshp_hsfcoef2d3_write(be_reg, static_reg_cfg->hsf_coef2_d[3]);   /* idx 3 */
    isp_bnrshp_hsfcoef2d4_write(be_reg, static_reg_cfg->hsf_coef2_d[4]);   /* idx 4 */
    isp_bnrshp_hsfcoef2d5_write(be_reg, static_reg_cfg->hsf_coef2_d[5]);   /* idx 5 */
    isp_bnrshp_hsfcoef2d6_write(be_reg, static_reg_cfg->hsf_coef2_d[6]);   /* idx 6 */
    isp_bnrshp_hsfcoef2d7_write(be_reg, static_reg_cfg->hsf_coef2_d[7]);   /* idx 7 */
    isp_bnrshp_hsfcoef2d8_write(be_reg, static_reg_cfg->hsf_coef2_d[8]);   /* idx 8 */
    isp_bnrshp_hsfcoef2d9_write(be_reg, static_reg_cfg->hsf_coef2_d[9]);   /* idx 9 */
    isp_bnrshp_hsfcoef2d10_write(be_reg, static_reg_cfg->hsf_coef2_d[10]); /* idx 10 */
    isp_bnrshp_hsfcoef2d11_write(be_reg, static_reg_cfg->hsf_coef2_d[11]); /* idx 11 */
    isp_bnrshp_hsfcoef2d12_write(be_reg, static_reg_cfg->hsf_coef2_d[12]); /* idx 12 */

    isp_bnrshp_hsfcoef3d0_write(be_reg, static_reg_cfg->hsf_coef3_d[0]);   /* idx 0 */
    isp_bnrshp_hsfcoef3d1_write(be_reg, static_reg_cfg->hsf_coef3_d[1]);   /* idx 1 */
    isp_bnrshp_hsfcoef3d2_write(be_reg, static_reg_cfg->hsf_coef3_d[2]);   /* idx 2 */
    isp_bnrshp_hsfcoef3d3_write(be_reg, static_reg_cfg->hsf_coef3_d[3]);   /* idx 3 */
    isp_bnrshp_hsfcoef3d4_write(be_reg, static_reg_cfg->hsf_coef3_d[4]);   /* idx 4 */
    isp_bnrshp_hsfcoef3d5_write(be_reg, static_reg_cfg->hsf_coef3_d[5]);   /* idx 5 */
    isp_bnrshp_hsfcoef3d6_write(be_reg, static_reg_cfg->hsf_coef3_d[6]);   /* idx 6 */
    isp_bnrshp_hsfcoef3d7_write(be_reg, static_reg_cfg->hsf_coef3_d[7]);   /* idx 7 */
    isp_bnrshp_hsfcoef3d8_write(be_reg, static_reg_cfg->hsf_coef3_d[8]);   /* idx 8 */
    isp_bnrshp_hsfcoef3d9_write(be_reg, static_reg_cfg->hsf_coef3_d[9]);   /* idx 9 */
    isp_bnrshp_hsfcoef3d10_write(be_reg, static_reg_cfg->hsf_coef3_d[10]); /* idx 10 */
    isp_bnrshp_hsfcoef3d11_write(be_reg, static_reg_cfg->hsf_coef3_d[11]); /* idx 11 */
    isp_bnrshp_hsfcoef3d12_write(be_reg, static_reg_cfg->hsf_coef3_d[12]); /* idx 12 */

    return;
}

static td_void isp_bshp_static_reg_write(isp_post_be_reg_type *be_reg, isp_bshp_static_cfg *static_reg_cfg)
{
    isp_bnrshp_bfilecoefmode_write(be_reg, static_reg_cfg->fil_coef_mode);
    isp_bnrshp_bitdepth_write(be_reg, static_reg_cfg->bit_depth);
    isp_bnrshp_varbitdepth_write(be_reg, static_reg_cfg->var_bit_depth);

    isp_bshp_static_f0to1_reg_write(be_reg, static_reg_cfg);
    isp_bshp_static_f2to3_reg_write(be_reg, static_reg_cfg);

    isp_bnrshp_lpfcoefd0_write(be_reg, static_reg_cfg->lpf_coef_d[0]);
    isp_bnrshp_lpfcoefd1_write(be_reg, static_reg_cfg->lpf_coef_d[1]);
    isp_bnrshp_lpfcoefd2_write(be_reg, static_reg_cfg->lpf_coef_d[2]); /* idx 2 */
    isp_bnrshp_lpfcoefud0_write(be_reg, static_reg_cfg->lpf_coef_ud[0]);
    isp_bnrshp_lpfcoefud1_write(be_reg, static_reg_cfg->lpf_coef_ud[1]);
    isp_bnrshp_lpfcoefud2_write(be_reg, static_reg_cfg->lpf_coef_ud[2]); /* idx 2 */
    isp_bnrshp_hsfcoefud0_write(be_reg, static_reg_cfg->hsf_coef_ud[0]);
    isp_bnrshp_hsfcoefud1_write(be_reg, static_reg_cfg->hsf_coef_ud[1]);
    isp_bnrshp_hsfcoefud2_write(be_reg, static_reg_cfg->hsf_coef_ud[2]); /* idx 2 */
    isp_bnrshp_hsfsftd_write(be_reg, static_reg_cfg->hsf_coef_d_sft);
    isp_bnrshp_hsfsftud_write(be_reg, static_reg_cfg->hsf_coef_ud_sft);
    isp_bnrshp_lpfsftd_write(be_reg, static_reg_cfg->lpf_coef_d_sft);
    isp_bnrshp_lpfsftud_write(be_reg, static_reg_cfg->lpf_coef_ud_sft);
    isp_bnrshp_mfgainsft_write(be_reg, static_reg_cfg->mf_gain_sft);
    isp_bnrshp_hfgainsft_write(be_reg, static_reg_cfg->hf_gain_sft);
    isp_bnrshp_gainthdselud_write(be_reg, static_reg_cfg->u8gain_thd_sel_ud);
    isp_bnrshp_gainthdseld_write(be_reg, static_reg_cfg->u8gain_thd_sel_d);
    isp_bnrshp_gainthdsftud_write(be_reg, static_reg_cfg->u8gain_thd_sft_ud);
    isp_bnrshp_gainthdsftd_write(be_reg, static_reg_cfg->u8gain_thd_sft_d);
    return;
}

static td_void isp_bshp_dyna_lmt_reg_write(isp_post_be_reg_type *be_reg, isp_bshp_dyna_cfg *dyna_reg_cfg)
{
    isp_bnrshp_lmtmf1_write(be_reg, dyna_reg_cfg->lmf_mf[1]); /* idx 1 */
    isp_bnrshp_lmtmf2_write(be_reg, dyna_reg_cfg->lmf_mf[2]); /* idx 2 */
    isp_bnrshp_lmtmf3_write(be_reg, dyna_reg_cfg->lmf_mf[3]); /* idx 3 */
    isp_bnrshp_lmtmf4_write(be_reg, dyna_reg_cfg->lmf_mf[4]); /* idx 4 */
    isp_bnrshp_lmtmf5_write(be_reg, dyna_reg_cfg->lmf_mf[5]); /* idx 5 */
    isp_bnrshp_lmtmf6_write(be_reg, dyna_reg_cfg->lmf_mf[6]); /* idx 6 */
    isp_bnrshp_lmtmf7_write(be_reg, dyna_reg_cfg->lmf_mf[7]); /* idx 7 */
    isp_bnrshp_lmthf0_write(be_reg, dyna_reg_cfg->lmf_hf[0]); /* idx 0 */
    isp_bnrshp_lmthf1_write(be_reg, dyna_reg_cfg->lmf_hf[1]); /* idx 1 */
    isp_bnrshp_lmthf2_write(be_reg, dyna_reg_cfg->lmf_hf[2]); /* idx 2 */
    isp_bnrshp_lmthf3_write(be_reg, dyna_reg_cfg->lmf_hf[3]); /* idx 3 */
    isp_bnrshp_lmthf4_write(be_reg, dyna_reg_cfg->lmf_hf[4]); /* idx 4 */
    isp_bnrshp_lmthf5_write(be_reg, dyna_reg_cfg->lmf_hf[5]); /* idx 5 */
    isp_bnrshp_lmthf6_write(be_reg, dyna_reg_cfg->lmf_hf[6]); /* idx 6 */
    isp_bnrshp_lmthf7_write(be_reg, dyna_reg_cfg->lmf_hf[7]); /* idx 7 */

    return;
}

static td_void isp_bshp_dyna_reg_write(isp_post_be_reg_type *be_reg, isp_bshp_dyna_cfg *dyna_reg_cfg)
{
    isp_bnrshp_neglumathd0mul_write(be_reg, dyna_reg_cfg->neg_luma_thd0_mul);
    isp_bnrshp_neglumathd1mul_write(be_reg, dyna_reg_cfg->neg_luma_thd1_mul);
    isp_bnrshp_negmfmul_write(be_reg, dyna_reg_cfg->neg_mf_mul);
    isp_bnrshp_neghfmul_write(be_reg, dyna_reg_cfg->neg_hf_mul);
    isp_bnrshp_shtvardiffmul_write(be_reg, dyna_reg_cfg->sht_var_diff_mul);
    isp_bnrshp_recnegctrlmul_write(be_reg, dyna_reg_cfg->reg_neg_ctrl_mul);
    isp_bnrshp_shpongmul_write(be_reg, dyna_reg_cfg->shp_on_g_mul);
    isp_bnrshp_chrctrl_en_write(be_reg, dyna_reg_cfg->chr_ctrl_en);
    isp_bnrshp_luma_ctrl_en_write(be_reg, dyna_reg_cfg->luma_ctrl_en);
    isp_bnrshp_shtbyvar_en_write(be_reg, dyna_reg_cfg->sht_ctrl_by_var_en);
    isp_bnrshp_bblkctrlmode_write(be_reg, dyna_reg_cfg->blk_ctrl_mode);
    isp_bnrshp_dirdiffsft_write(be_reg, dyna_reg_cfg->dir_diff_sft);
    isp_bnrshp_dirrt0_write(be_reg, dyna_reg_cfg->dir_rt[0]);
    isp_bnrshp_dirrt1_write(be_reg, dyna_reg_cfg->dir_rt[1]);
    isp_bnrshp_neglumathd00_write(be_reg, dyna_reg_cfg->neg_luma_thd0[0]);
    isp_bnrshp_neglumathd01_write(be_reg, dyna_reg_cfg->neg_luma_thd0[1]);
    isp_bnrshp_neglumathd10_write(be_reg, dyna_reg_cfg->neg_luma_thd1[0]);
    isp_bnrshp_neglumathd11_write(be_reg, dyna_reg_cfg->neg_luma_thd1[1]);
    isp_bnrshp_neglumawgt00_write(be_reg, dyna_reg_cfg->neg_luma_wgt0[0]);
    isp_bnrshp_neglumawgt01_write(be_reg, dyna_reg_cfg->neg_luma_wgt0[1]);
    isp_bnrshp_neglumawgt10_write(be_reg, dyna_reg_cfg->neg_luma_wgt1[0]);
    isp_bnrshp_neglumawgt11_write(be_reg, dyna_reg_cfg->neg_luma_wgt1[1]);
    isp_bnrshp_negmfshft_write(be_reg, dyna_reg_cfg->neg_mf_sft);
    isp_bnrshp_negmfwgt0_write(be_reg, dyna_reg_cfg->neg_mf_wgt[0]);
    isp_bnrshp_negmfwgt1_write(be_reg, dyna_reg_cfg->neg_mf_wgt[1]);
    isp_bnrshp_neghfshft_write(be_reg, dyna_reg_cfg->neg_hf_sft);
    isp_bnrshp_neghfthd0_write(be_reg, dyna_reg_cfg->neg_hf_thd[0]);
    isp_bnrshp_neghfthd1_write(be_reg, dyna_reg_cfg->neg_hf_thd[1]);
    isp_bnrshp_neghfwgt0_write(be_reg, dyna_reg_cfg->neg_hf_wgt[0]);
    isp_bnrshp_neghfwgt1_write(be_reg, dyna_reg_cfg->neg_hf_wgt[1]);
    isp_bshp_dyna_lmt_reg_write(be_reg, dyna_reg_cfg);
    isp_bnrshp_shtvar5x5sft_write(be_reg, dyna_reg_cfg->sht_var5x5_sft);
    isp_bnrshp_shtvardiffthd0_write(be_reg, dyna_reg_cfg->sht_var_diff_thd[0]);
    isp_bnrshp_shtvardiffthd1_write(be_reg, dyna_reg_cfg->sht_var_diff_thd[1]);
    isp_bnrshp_recnegctrlsft_write(be_reg, dyna_reg_cfg->rec_neg_ctrl_sft);
    isp_bnrshp_recnegctrlthr0_write(be_reg, dyna_reg_cfg->reg_neg_ctrl_thd[0]);
    isp_bnrshp_recnegctrlthr1_write(be_reg, dyna_reg_cfg->reg_neg_ctrl_thd[1]);
    isp_bnrshp_recnegctrlwgt0_write(be_reg, dyna_reg_cfg->reg_neg_ctrl_wgt[0]);
    isp_bnrshp_recnegctrlwgt1_write(be_reg, dyna_reg_cfg->reg_neg_ctrl_wgt[1]);
    isp_bnrshp_selpixwgt_write(be_reg, dyna_reg_cfg->sel_pix_wgt);
    isp_bnrshp_oshtmaxgain_write(be_reg, dyna_reg_cfg->o_max_gain);
    isp_bnrshp_ushtmaxgain_write(be_reg, dyna_reg_cfg->u_max_gain);
    isp_bnrshp_omaxchg_write(be_reg, dyna_reg_cfg->o_max_chg);
    isp_bnrshp_umaxchg_write(be_reg, dyna_reg_cfg->u_max_chg);
    isp_bnrshp_detailushtamt_write(be_reg, dyna_reg_cfg->detail_u_sht_amt);
    isp_bnrshp_detailoshtamt_write(be_reg, dyna_reg_cfg->detail_o_sht_amt);
    isp_bnrshp_shpongthd0_write(be_reg, dyna_reg_cfg->shp_on_g_thd[0]);
    isp_bnrshp_shpongthd1_write(be_reg, dyna_reg_cfg->shp_on_g_thd[1]);
}


static td_s32 isp_bshp_user_lut_online_write(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_post_be_reg_type *be_reg, isp_viproc_reg_type *vi_proc_reg)
{
    td_s32 j, ret;
    td_u8 buf_id;
    td_u32 *bshp_stt_data0 = TD_NULL;
    td_u32 *bshp_stt_data1 = TD_NULL;
    td_u32 *bshp_stt_data2 = TD_NULL;
    td_u32 *bshp_stt_data3 = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_bshp_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_bshp_user_cfg *usr_reg_cfg = TD_NULL;
    dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.dyna_reg_cfg;
    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.usr_reg_cfg;
    bshp_stt_data0 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data0 == TD_NULL) {
        return TD_FAILURE;
    }
    bshp_stt_data1 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data1 == TD_NULL) {
        free(bshp_stt_data0);
        return TD_FAILURE;
    }
    bshp_stt_data2 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data2 == TD_NULL) {
        free(bshp_stt_data0);
        free(bshp_stt_data1);
        return TD_FAILURE;
    }
    bshp_stt_data3 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data3 == TD_NULL) {
        free(bshp_stt_data0);
        free(bshp_stt_data1);
        free(bshp_stt_data2);
        return TD_FAILURE;
    }
    for (j = 0; j < OT_ISP_BSHP_HALF_CURVE_NUM; j++) {
        bshp_stt_data0[j] = ((usr_reg_cfg->gain_d1[j] & 0x000000ff) << 24) + /* sht 24 */
            (usr_reg_cfg->gain_d0[j] & 0x00ffffff);
        bshp_stt_data1[j] = ((usr_reg_cfg->gain_ud0[j] & 0x0000ffff) << 16) + /* sht 16 */
            ((usr_reg_cfg->gain_d1[j] & 0x00ffff00) >> 8);                    /* sht 8 */
        bshp_stt_data2[j] = ((usr_reg_cfg->gain_ud1[j] & 0x00ffffff) << 8) +  /* sht 8 */
            ((usr_reg_cfg->gain_ud0[j] & 0x00ff0000) >> 16);                  /* sht 16 */
        bshp_stt_data3[j] = (td_u32)(usr_reg_cfg->luma_wgt[j] & 0x00007fff);
    }
    buf_id = dyna_reg_cfg->buf_id;
    be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
    if (be_lut_stt_reg == TD_NULL) {
        free(bshp_stt_data0);
        free(bshp_stt_data1);
        free(bshp_stt_data2);
        free(bshp_stt_data3);
        return TD_FAILURE;
    }

    isp_bnrshp_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, bshp_stt_data0, bshp_stt_data1, bshp_stt_data2,
        bshp_stt_data3);
    ret = isp_bshp_lut_wstt_addr_write(vi_pipe, i, buf_id, vi_proc_reg);
    if (ret != TD_SUCCESS) {
        free(bshp_stt_data0);
        free(bshp_stt_data1);
        free(bshp_stt_data2);
        free(bshp_stt_data3);
        return TD_FAILURE;
    }
    isp_bnrshp_stt2lut_en_write(be_reg, TD_TRUE);
    dyna_reg_cfg->buf_id = 1 - buf_id;
    free(bshp_stt_data0);
    free(bshp_stt_data1);
    free(bshp_stt_data2);
    free(bshp_stt_data3);

    return TD_SUCCESS;
}

static td_s32 isp_bshp_user_lut_offline_write(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_post_be_reg_type *be_reg, const isp_viproc_reg_type *vi_proc_reg)
{
    td_s32 j;
    td_u32 *bshp_stt_data0 = TD_NULL;
    td_u32 *bshp_stt_data1 = TD_NULL;
    td_u32 *bshp_stt_data2 = TD_NULL;
    td_u32 *bshp_stt_data3 = TD_NULL;

    isp_bshp_user_cfg *usr_reg_cfg = TD_NULL;
    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.usr_reg_cfg;

    ot_unused(vi_pipe);
    ot_unused(vi_proc_reg);

    bshp_stt_data0 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data0 == TD_NULL) {
        return TD_FAILURE;
    }
    bshp_stt_data1 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data1 == TD_NULL) {
        free(bshp_stt_data0);
        return TD_FAILURE;
    }
    bshp_stt_data2 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data2 == TD_NULL) {
        free(bshp_stt_data0);
        free(bshp_stt_data1);
        return TD_FAILURE;
    }
    bshp_stt_data3 = (td_u32 *)malloc(OT_ISP_BSHP_HALF_CURVE_NUM * sizeof(td_u32));
    if (bshp_stt_data3 == TD_NULL) {
        free(bshp_stt_data0);
        free(bshp_stt_data1);
        free(bshp_stt_data2);
        return TD_FAILURE;
    }

    for (j = 0; j < OT_ISP_BSHP_HALF_CURVE_NUM; j++) {
        bshp_stt_data0[j] = ((usr_reg_cfg->gain_d1[j] & 0x000000ff) << 24) + /* lshif 24 */
            (usr_reg_cfg->gain_d0[j] & 0x00ffffff);
        bshp_stt_data1[j] = ((usr_reg_cfg->gain_ud0[j] & 0x0000ffff) << 16) + /* lshif 16 rshif 8 */
            ((usr_reg_cfg->gain_d1[j] & 0x00ffff00) >> 8);                    /* lshif 16 rshif 8 */
        bshp_stt_data2[j] = ((usr_reg_cfg->gain_ud1[j] & 0x00ffffff) << 8) +  /* lshif 16 rshif 8 */
            ((usr_reg_cfg->gain_ud0[j] & 0x00ff0000) >> 16);                  /* lshif 8 rshif 16 */
        bshp_stt_data3[j] = (td_u32)(usr_reg_cfg->luma_wgt[j] & 0x00007fff);
    }
    isp_bnrshp_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, bshp_stt_data0, bshp_stt_data1, bshp_stt_data2,
        bshp_stt_data3);
    isp_bnrshp_stt2lut_en_write(be_reg, TD_TRUE);
    isp_bnrshp_stt2lut_regnew_write(be_reg, TD_TRUE);
    free(bshp_stt_data0);
    free(bshp_stt_data1);
    free(bshp_stt_data2);
    free(bshp_stt_data3);
    return TD_SUCCESS;
}

static td_s32 isp_bshp_user_reg_write(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_post_be_reg_type *be_reg, isp_viproc_reg_type *vi_proc_reg)
{
    td_bool stt2_lut_regnew = TD_FALSE;
    td_bool offline_mode;
    td_s32 ret;

    isp_bshp_user_cfg *usr_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.usr_reg_cfg;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    isp_bnrshp_dirrly0_write(be_reg, usr_reg_cfg->dir_rly[0]);
    isp_bnrshp_dirrly1_write(be_reg, usr_reg_cfg->dir_rly[1]);
    isp_bnrshp_shtvardiffwgt0_write(be_reg, usr_reg_cfg->sht_var_diff_wgt[0]);
    isp_bnrshp_shtvardiffwgt1_write(be_reg, usr_reg_cfg->sht_var_diff_wgt[1]);
    isp_bnrshp_oshtamt_write(be_reg, usr_reg_cfg->o_sht_amt);
    isp_bnrshp_ushtamt_write(be_reg, usr_reg_cfg->u_sht_amt);
    isp_bnrshp_shpongwgt0_write(be_reg, usr_reg_cfg->shp_on_g_wgt[0]);
    isp_bnrshp_shpongwgt1_write(be_reg, usr_reg_cfg->shp_on_g_wgt[1]);

    if (offline_mode) {
        ret = isp_bshp_user_lut_offline_write(vi_pipe, reg_cfg_info, i, be_reg, vi_proc_reg);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        stt2_lut_regnew = TD_TRUE;
    } else {
        ret = isp_bshp_user_lut_online_write(vi_pipe, reg_cfg_info, i, be_reg, vi_proc_reg);
        if (ret != TD_SUCCESS) {
            return ret;
        }
    }

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.bshp_stt2lut_regnew = stt2_lut_regnew;
    isp_bshp_lut_width_word_write(vi_proc_reg, OT_ISP_BSHP_LUT_WIDTH_WORD_DEFAULT);
    usr_reg_cfg->user_resh = offline_mode;
    return TD_SUCCESS;
}

static td_void isp_bshp_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_bshp_user_cfg *usr_reg_cfg, td_bool *lut_update)
{
    td_u32 bnrshp_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        bnrshp_stt2lut_clr = isp_bnrshp_stt2lut_clr_read(be_reg);
        if (bnrshp_stt2lut_clr != 0) {
            usr_reg_cfg->user_resh = TD_TRUE;
            *lut_update = TD_TRUE;
            isp_bnrshp_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_bshp_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    td_bool lut_update = TD_FALSE;
    td_s32 ret;
    isp_bshp_static_cfg *static_reg_cfg = TD_NULL;
    isp_bshp_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_bshp_user_cfg *usr_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *vi_proc_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));
    if (reg_cfg_info->cfg_key.bit1_bshp_cfg) {
        vi_proc_reg = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(vi_proc_reg);
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.dyna_reg_cfg;
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bshp_reg_cfg.usr_reg_cfg;
        if (static_reg_cfg->static_resh) { /* static */
            isp_bshp_static_reg_write(be_reg, static_reg_cfg);
            static_reg_cfg->static_resh = TD_FALSE;
        }
        if (dyna_reg_cfg->dyna_resh) { /* dynamic */
            isp_bshp_dyna_reg_write(be_reg, dyna_reg_cfg);
            dyna_reg_cfg->dyna_resh = TD_FALSE;
        }
        if (usr_reg_cfg->user_resh) { /* usr */
            ret = isp_bshp_user_reg_write(vi_pipe, reg_cfg_info, i, be_reg, vi_proc_reg);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            usr_reg_cfg->user_resh = offline_mode;
        }
        isp_bshp_reg_update_online(offline_mode, be_reg, usr_reg_cfg, &lut_update);
        reg_cfg_info->cfg_key.bit1_bshp_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.bshp_lut_update = (lut_update || offline_mode);

    return TD_SUCCESS;
}

static td_s32 isp_dpc_bpt_lut_reg_config(td_bool *stt2_lut_regnew, ot_vi_pipe vi_pipe, td_u8 i,
    isp_pre_be_reg_type *be_reg, isp_dpc_usr_cfg *usr_reg_cfg)
{
    td_u8 buf_id;
    td_s32 ret;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_pre_be_lut_wstt_type *pre_lut_stt_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(pre_viproc);

    if (is_online_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        /* online lut2stt regconfig */
        buf_id = usr_reg_cfg->usr_sta_cor_reg_cfg.buf_id;
        if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
            pre_lut_stt_reg = (isp_pre_be_lut_wstt_type *)isp_get_pre_on_post_off_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(pre_lut_stt_reg);
            isp_dpc_lut_wstt_write(pre_lut_stt_reg, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bp_table);
        } else {
            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_pre_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(be_lut_stt_reg);
            isp_dpc_lut_wstt_write(&be_lut_stt_reg->pre_be_lut2stt, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bp_table);
        }

        ret = isp_dpc_lut_wstt_addr_write(vi_pipe, i, buf_id, pre_viproc);
        if (ret != TD_SUCCESS) {
            isp_err_trace("isp[%d]_dpc_lut_wstt_addr_write failed\n", vi_pipe);
            return ret;
        }

        isp_dpc_stt2lut_en_write(be_reg, TD_TRUE);

        usr_reg_cfg->usr_sta_cor_reg_cfg.buf_id = 1 - buf_id;

        *stt2_lut_regnew = TD_TRUE;
    } else {
        isp_dpc_lut_wstt_write(&be_reg->pre_be_lut.pre_be_lut2stt, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bp_table);
        isp_dpc_stt2lut_en_write(be_reg, TD_TRUE);
        isp_dpc_stt2lut_regnew_write(be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_s32 isp_dpc_usr_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_pre_be_reg_type *be_reg, td_bool offline_mode)
{
    isp_dpc_usr_cfg *usr_reg_cfg = TD_NULL;
    td_bool usr_resh;
    td_bool idx_resh;
    td_s32 ret;
    td_bool stt2_lut_regnew = TD_FALSE;

    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dp_reg_cfg.usr_reg_cfg;

    if (usr_reg_cfg->usr_dyna_cor_reg_cfg.resh) {
        isp_dpc_ex_soft_thr_max_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_sup_twinkle_thr_max);
        isp_dpc_ex_soft_thr_min_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_sup_twinkle_thr_min);
        isp_dpc_ex_hard_thr_en_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_hard_thr_en);
        isp_dpc_ex_rake_ratio_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_rake_ratio);
        usr_reg_cfg->usr_dyna_cor_reg_cfg.resh = offline_mode;
    }

    idx_resh = (isp_dpc_update_index_read(be_reg) != usr_reg_cfg->usr_sta_cor_reg_cfg.update_index);
    usr_resh =
        (offline_mode) ? (usr_reg_cfg->usr_sta_cor_reg_cfg.resh && idx_resh) : (usr_reg_cfg->usr_sta_cor_reg_cfg.resh);

    if (usr_resh) {
        isp_dpc_update_index_write(be_reg, usr_reg_cfg->usr_sta_cor_reg_cfg.update_index);
        isp_dpc_bpt_number_write(be_reg, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bpt_number);

        ret = isp_dpc_bpt_lut_reg_config(&stt2_lut_regnew, vi_pipe, i, be_reg, usr_reg_cfg);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        usr_reg_cfg->usr_sta_cor_reg_cfg.resh = offline_mode;
    }

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.dpc_stt2lut_regnew = stt2_lut_regnew;

    return TD_SUCCESS;
}

static td_void isp_dpc_rgbir_dyn_reg_config(isp_pre_be_reg_type *be_reg, isp_dpc_dyna_cfg *dyna_reg_cfg,
    td_bool rgbir_en)
{
    isp_dpc_eigh_refen_write(be_reg, dyna_reg_cfg->dpc_eigh_ref_en);
    isp_dpc_wbrgainrecp_write(be_reg, dyna_reg_cfg->dpc_wb_rgain_recp);
    isp_dpc_wbbgainrecp_write(be_reg, dyna_reg_cfg->dpc_wb_bgain_recp);
    isp_dpc_offset_write(be_reg, dyna_reg_cfg->dpc_offset);
    isp_dpc_ro_eigh_write(be_reg, dyna_reg_cfg->dpc_ro_eigh);
    isp_dpc_pgfac_eigh_write(be_reg, dyna_reg_cfg->dpc_pgfac_eigh);
    isp_dpc_pgthr_eigh_write(be_reg, dyna_reg_cfg->dpc_pgthr_eigh);

    if (rgbir_en == TD_TRUE) {
        isp_dpc_output_mode_write(be_reg, OT_ISP_DPC_DEFAULT_OUTPUT_MODE_RGBIR);
        isp_dpc_quad_refen_write(be_reg, TD_FALSE);
    } else {
        isp_dpc_output_mode_write(be_reg, OT_ISP_DPC_DEFAULT_OUTPUT_MODE);
        isp_dpc_quad_refen_write(be_reg, TD_TRUE);
    }
}

static td_void isp_dpc_dyn_reg_std_config(isp_pre_be_reg_type *be_reg, isp_dpc_dyna_cfg *dyna_reg_cfg)
{
    isp_dpc_line_std_thr_1_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[0]); /* array index 0 */
    isp_dpc_line_std_thr_2_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[1]); /* array index 1 */
    isp_dpc_line_std_thr_3_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[2]); /* array index 2 */
    isp_dpc_line_std_thr_4_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[3]); /* array index 3 */
    isp_dpc_line_std_thr_5_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[4]); /* array index 4 */

    isp_dpc_line_diff_thr_1_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[0]); /* array index 0 */
    isp_dpc_line_diff_thr_2_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[1]); /* array index 1 */
    isp_dpc_line_diff_thr_3_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[2]); /* array index 2 */
    isp_dpc_line_diff_thr_4_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[3]); /* array index 3 */
    isp_dpc_line_diff_thr_5_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[4]); /* array index 4 */

    isp_dpc_line_aver_fac_1_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[0]); /* array index 0 */
    isp_dpc_line_aver_fac_2_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[1]); /* array index 1 */
    isp_dpc_line_aver_fac_3_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[2]); /* array index 2 */
    isp_dpc_line_aver_fac_4_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[3]); /* array index 3 */
    isp_dpc_line_aver_fac_5_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[4]); /* array index 4 */

    isp_dpc_rnd_thresh_1_mtp_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr_mtp[0]); /* array index 0 */
    isp_dpc_rnd_thresh_2_mtp_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr_mtp[1]); /* array index 1 */
    isp_dpc_rnd_thresh_3_mtp_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr_mtp[2]); /* array index 2 */

    isp_dpc_rg_fac_1_mtp_write(be_reg, dyna_reg_cfg->dpcc_rg_fac_mtp[0]); /* array index 0 */
    isp_dpc_rg_fac_2_mtp_write(be_reg, dyna_reg_cfg->dpcc_rg_fac_mtp[1]); /* array index 1 */
    isp_dpc_rg_fac_3_mtp_write(be_reg, dyna_reg_cfg->dpcc_rg_fac_mtp[2]); /* array index 2 */
}

static td_void isp_dpc_dyn_reg_config(isp_pre_be_reg_type *be_reg, isp_dpc_dyna_cfg *dyna_reg_cfg)
{
    isp_dpc_blend_write(be_reg, dyna_reg_cfg->dpcc_alpha);
    isp_dpc_mode_write(be_reg, dyna_reg_cfg->dpcc_mode);
    isp_dpc_set_use_write(be_reg, dyna_reg_cfg->dpcc_set_use);
    isp_dpc_methods_set_1_write(be_reg, dyna_reg_cfg->dpcc_methods_set1);
    isp_dpc_methods_set_2_write(be_reg, dyna_reg_cfg->dpcc_methods_set2);
    isp_dpc_methods_set_3_write(be_reg, dyna_reg_cfg->dpcc_methods_set3);
    isp_dpc_line_thresh_1_write(be_reg, dyna_reg_cfg->dpcc_line_thr[0]);      /* array index 0 */
    isp_dpc_line_mad_fac_1_write(be_reg, dyna_reg_cfg->dpcc_line_mad_fac[0]); /* array index 0 */
    isp_dpc_pg_fac_1_write(be_reg, dyna_reg_cfg->dpcc_pg_fac[0]);             /* array index 0 */
    isp_dpc_rnd_thresh_1_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr[0]);        /* array index 0 */
    isp_dpc_rg_fac_1_write(be_reg, dyna_reg_cfg->dpcc_rg_fac[0]);             /* array index 0 */
    isp_dpc_line_thresh_2_write(be_reg, dyna_reg_cfg->dpcc_line_thr[1]);      /* array index 1 */
    isp_dpc_line_mad_fac_2_write(be_reg, dyna_reg_cfg->dpcc_line_mad_fac[1]); /* array index 1 */
    isp_dpc_pg_fac_2_write(be_reg, dyna_reg_cfg->dpcc_pg_fac[1]);             /* array index 1 */
    isp_dpc_rnd_thresh_2_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr[1]);        /* array index 1 */
    isp_dpc_rg_fac_2_write(be_reg, dyna_reg_cfg->dpcc_rg_fac[1]);             /* array index 1 */
    isp_dpc_line_thresh_3_write(be_reg, dyna_reg_cfg->dpcc_line_thr[2]);      /* array index 2 */
    isp_dpc_line_mad_fac_3_write(be_reg, dyna_reg_cfg->dpcc_line_mad_fac[2]); /* array index 2 */
    isp_dpc_pg_fac_3_write(be_reg, dyna_reg_cfg->dpcc_pg_fac[2]);             /* array index 2 */
    isp_dpc_rnd_thresh_3_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr[2]);        /* array index 2 */
    isp_dpc_rg_fac_3_write(be_reg, dyna_reg_cfg->dpcc_rg_fac[2]);             /* array index 2 */
    isp_dpc_ro_limits_write(be_reg, dyna_reg_cfg->dpcc_ro_limits);
    isp_dpc_rnd_offs_write(be_reg, dyna_reg_cfg->dpcc_rnd_offs);
    isp_dpc_bpt_thresh_write(be_reg, dyna_reg_cfg->dpcc_bad_thresh);

    isp_dpc_line_kerdiff_fac_write(be_reg, dyna_reg_cfg->dpcc_line_kerdiff_fac);
    isp_dpc_blend_mode_write(be_reg, dyna_reg_cfg->dpcc_blend_mode);
    isp_dpc_bit_depth_sel_write(be_reg, dyna_reg_cfg->dpcc_bit_depth_sel);
    isp_dpc_amp_coef_k_write(be_reg, dyna_reg_cfg->dpcc_amp_coef_k);
    isp_dpc_amp_coef_min_write(be_reg, dyna_reg_cfg->dpcc_amp_coef_min);

    isp_dpc_dyn_reg_std_config(be_reg, dyna_reg_cfg);
}

static td_s32 isp_dpc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool rgbir_en = ot_ext_system_rgbir_enable_read(vi_pipe);
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_dpc_static_cfg *static_reg_cfg = TD_NULL;
    isp_dpc_dyna_cfg *dyna_reg_cfg = TD_NULL;

    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.dpc_stt2lut_regnew = TD_FALSE;

    if (reg_cfg_info->cfg_key.bit1_dp_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(pre_viproc);

        /* static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dp_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->static_resh) {
            isp_dpc_output_mode_write(be_reg, static_reg_cfg->dpcc_output_mode);
            isp_dpc_bpt_ctrl_write(be_reg, static_reg_cfg->dpcc_bpt_ctrl);
            isp_dcg_lut_width_word_write(pre_viproc, OT_ISP_DPC_LUT_WIDTH_WORD_DEFAULT);
            static_reg_cfg->static_resh = TD_FALSE;
        }

        /* usr */
        ret = isp_dpc_usr_reg_config(vi_pipe, reg_cfg_info, i, be_reg, offline_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        /* dynamic */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dp_reg_cfg.dyna_reg_cfg;

        if (dyna_reg_cfg->resh) {
            isp_dpc_stat_en_write(pre_viproc, dyna_reg_cfg->dpc_stat_en);
            isp_dpc_dyn_reg_config(be_reg, dyna_reg_cfg);
            dyna_reg_cfg->resh = offline_mode;
        }
        isp_dpc_rgbir_dyn_reg_config(be_reg, dyna_reg_cfg, rgbir_en);

        reg_cfg_info->cfg_key.bit1_dp_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_dpc1_bpt_lut_reg_config(td_bool *stt2_lut_regnew, ot_vi_pipe vi_pipe, td_u8 i,
    isp_pre_be_reg_type *be_reg, isp_dpc_usr_cfg *usr_reg_cfg)
{
    td_u8 buf_id;
    td_s32 ret;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_pre_be_lut_wstt_type *pre_lut_stt_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(pre_viproc);

    if (is_online_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        /* online lut2stt regconfig */
        buf_id = usr_reg_cfg->usr_sta_cor_reg_cfg.buf_id1;
        if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
            pre_lut_stt_reg = (isp_pre_be_lut_wstt_type *)isp_get_pre_on_post_off_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(pre_lut_stt_reg);
            isp_dpc1_lut_wstt_write(pre_lut_stt_reg, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bp_table);
        } else {
            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_pre_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(be_lut_stt_reg);
            isp_dpc1_lut_wstt_write(&be_lut_stt_reg->pre_be_lut2stt, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bp_table);
        }

        ret = isp_dpc1_lut_wstt_addr_write(vi_pipe, i, buf_id, pre_viproc);
        if (ret != TD_SUCCESS) {
            isp_err_trace("isp[%d]_dpc_lut_wstt_addr_write failed\n", vi_pipe);
            return ret;
        }

        isp_dpc1_stt2lut_en_write(be_reg, TD_TRUE);

        usr_reg_cfg->usr_sta_cor_reg_cfg.buf_id1 = 1 - buf_id;

        *stt2_lut_regnew = TD_TRUE;
    } else {
        isp_dpc1_lut_wstt_write(&be_reg->pre_be_lut.pre_be_lut2stt, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bp_table);
        isp_dpc1_stt2lut_en_write(be_reg, TD_TRUE);
        isp_dpc1_stt2lut_regnew_write(be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_s32 isp_dpc1_usr_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_pre_be_reg_type *be_reg, td_bool offline_mode)
{
    isp_dpc_usr_cfg *usr_reg_cfg = TD_NULL;
    td_bool usr_resh;
    td_bool idx_resh;
    td_s32 ret;
    td_bool stt2_lut_regnew = TD_FALSE;

    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dp_reg_cfg.usr_reg_cfg;

    if (usr_reg_cfg->usr_dyna_cor_reg_cfg.resh1) {
        isp_dpc1_ex_soft_thr_max_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_sup_twinkle_thr_max);
        isp_dpc1_ex_soft_thr_min_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_sup_twinkle_thr_min);
        isp_dpc1_ex_hard_thr_en_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_hard_thr_en);
        isp_dpc1_ex_rake_ratio_write(be_reg, usr_reg_cfg->usr_dyna_cor_reg_cfg.dpcc_rake_ratio);
        usr_reg_cfg->usr_dyna_cor_reg_cfg.resh1 = offline_mode;
    }

    idx_resh = (isp_dpc1_update_index_read(be_reg) != usr_reg_cfg->usr_sta_cor_reg_cfg.update_index);
    usr_resh = (offline_mode) ? (usr_reg_cfg->usr_sta_cor_reg_cfg.resh1 && idx_resh) :
                                (usr_reg_cfg->usr_sta_cor_reg_cfg.resh1);

    if (usr_resh) {
        isp_dpc1_update_index_write(be_reg, usr_reg_cfg->usr_sta_cor_reg_cfg.update_index);
        isp_dpc1_bpt_number_write(be_reg, usr_reg_cfg->usr_sta_cor_reg_cfg.dpcc_bpt_number);

        ret = isp_dpc1_bpt_lut_reg_config(&stt2_lut_regnew, vi_pipe, i, be_reg, usr_reg_cfg);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        usr_reg_cfg->usr_sta_cor_reg_cfg.resh1 = offline_mode;
    }

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.dpc1_stt2lut_regnew = stt2_lut_regnew;

    return TD_SUCCESS;
}

static td_void isp_dpc1_rgbir_dyn_reg_config(isp_pre_be_reg_type *be_reg, isp_dpc_dyna_cfg *dyna_reg_cfg,
    td_bool rgbir_en)
{
    isp_dpc1_eigh_refen_write(be_reg, dyna_reg_cfg->dpc_eigh_ref_en);
    isp_dpc1_wbrgainrecp_write(be_reg, dyna_reg_cfg->dpc_wb_rgain_recp);
    isp_dpc1_wbbgainrecp_write(be_reg, dyna_reg_cfg->dpc_wb_bgain_recp);
    isp_dpc1_offset_write(be_reg, dyna_reg_cfg->dpc_offset);
    isp_dpc1_ro_eigh_write(be_reg, dyna_reg_cfg->dpc_ro_eigh);
    isp_dpc1_pgfac_eigh_write(be_reg, dyna_reg_cfg->dpc_pgfac_eigh);
    isp_dpc1_pgthr_eigh_write(be_reg, dyna_reg_cfg->dpc_pgthr_eigh);

    if (rgbir_en == TD_TRUE) {
        isp_dpc1_output_mode_write(be_reg, OT_ISP_DPC_DEFAULT_OUTPUT_MODE_RGBIR);
        isp_dpc1_quad_refen_write(be_reg, TD_FALSE);
    } else {
        isp_dpc1_output_mode_write(be_reg, OT_ISP_DPC_DEFAULT_OUTPUT_MODE);
        isp_dpc1_quad_refen_write(be_reg, TD_TRUE);
    }
}

static td_void isp_dpc1_dyn_reg_std_config(isp_pre_be_reg_type *be_reg, isp_dpc_dyna_cfg *dyna_reg_cfg)
{
    isp_dpc1_line_std_thr_1_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[0]); /* array index 0 */
    isp_dpc1_line_std_thr_2_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[1]); /* array index 1 */
    isp_dpc1_line_std_thr_3_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[2]); /* array index 2 */
    isp_dpc1_line_std_thr_4_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[3]); /* array index 3 */
    isp_dpc1_line_std_thr_5_write(be_reg, dyna_reg_cfg->dpcc_line_std_thr[4]); /* array index 4 */

    isp_dpc1_line_diff_thr_1_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[0]); /* array index 0 */
    isp_dpc1_line_diff_thr_2_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[1]); /* array index 1 */
    isp_dpc1_line_diff_thr_3_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[2]); /* array index 2 */
    isp_dpc1_line_diff_thr_4_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[3]); /* array index 3 */
    isp_dpc1_line_diff_thr_5_write(be_reg, dyna_reg_cfg->dpcc_line_diff_thr[4]); /* array index 4 */

    isp_dpc1_line_aver_fac_1_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[0]); /* array index 0 */
    isp_dpc1_line_aver_fac_2_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[1]); /* array index 1 */
    isp_dpc1_line_aver_fac_3_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[2]); /* array index 2 */
    isp_dpc1_line_aver_fac_4_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[3]); /* array index 3 */
    isp_dpc1_line_aver_fac_5_write(be_reg, dyna_reg_cfg->dpcc_line_aver_fac[4]); /* array index 4 */

    isp_dpc1_rnd_thresh_1_mtp_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr_mtp[0]); /* array index 0 */
    isp_dpc1_rnd_thresh_2_mtp_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr_mtp[1]); /* array index 1 */
    isp_dpc1_rnd_thresh_3_mtp_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr_mtp[2]); /* array index 2 */

    isp_dpc1_rg_fac_1_mtp_write(be_reg, dyna_reg_cfg->dpcc_rg_fac_mtp[0]); /* array index 0 */
    isp_dpc1_rg_fac_2_mtp_write(be_reg, dyna_reg_cfg->dpcc_rg_fac_mtp[1]); /* array index 1 */
    isp_dpc1_rg_fac_3_mtp_write(be_reg, dyna_reg_cfg->dpcc_rg_fac_mtp[2]); /* array index 2 */
}

static td_void isp_dpc1_dyn_reg_config(isp_pre_be_reg_type *be_reg, isp_dpc_dyna_cfg *dyna_reg_cfg)
{
    isp_dpc1_blend_write(be_reg, dyna_reg_cfg->dpcc_alpha);
    isp_dpc1_mode_write(be_reg, dyna_reg_cfg->dpcc_mode);
    isp_dpc1_set_use_write(be_reg, dyna_reg_cfg->dpcc_set_use);
    isp_dpc1_methods_set_1_write(be_reg, dyna_reg_cfg->dpcc_methods_set1);
    isp_dpc1_methods_set_2_write(be_reg, dyna_reg_cfg->dpcc_methods_set2);
    isp_dpc1_methods_set_3_write(be_reg, dyna_reg_cfg->dpcc_methods_set3);
    isp_dpc1_line_thresh_1_write(be_reg, dyna_reg_cfg->dpcc_line_thr[0]);      /* array index 0 */
    isp_dpc1_line_mad_fac_1_write(be_reg, dyna_reg_cfg->dpcc_line_mad_fac[0]); /* array index 0 */
    isp_dpc1_pg_fac_1_write(be_reg, dyna_reg_cfg->dpcc_pg_fac[0]);             /* array index 0 */
    isp_dpc1_rnd_thresh_1_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr[0]);        /* array index 0 */
    isp_dpc1_rg_fac_1_write(be_reg, dyna_reg_cfg->dpcc_rg_fac[0]);             /* array index 0 */
    isp_dpc1_line_thresh_2_write(be_reg, dyna_reg_cfg->dpcc_line_thr[1]);      /* array index 1 */
    isp_dpc1_line_mad_fac_2_write(be_reg, dyna_reg_cfg->dpcc_line_mad_fac[1]); /* array index 1 */
    isp_dpc1_pg_fac_2_write(be_reg, dyna_reg_cfg->dpcc_pg_fac[1]);             /* array index 1 */
    isp_dpc1_rnd_thresh_2_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr[1]);        /* array index 1 */
    isp_dpc1_rg_fac_2_write(be_reg, dyna_reg_cfg->dpcc_rg_fac[1]);             /* array index 1 */
    isp_dpc1_line_thresh_3_write(be_reg, dyna_reg_cfg->dpcc_line_thr[2]);      /* array index 2 */
    isp_dpc1_line_mad_fac_3_write(be_reg, dyna_reg_cfg->dpcc_line_mad_fac[2]); /* array index 2 */
    isp_dpc1_pg_fac_3_write(be_reg, dyna_reg_cfg->dpcc_pg_fac[2]);             /* array index 2 */
    isp_dpc1_rnd_thresh_3_write(be_reg, dyna_reg_cfg->dpcc_rnd_thr[2]);        /* array index 2 */
    isp_dpc1_rg_fac_3_write(be_reg, dyna_reg_cfg->dpcc_rg_fac[2]);             /* array index 2 */
    isp_dpc1_ro_limits_write(be_reg, dyna_reg_cfg->dpcc_ro_limits);
    isp_dpc1_rnd_offs_write(be_reg, dyna_reg_cfg->dpcc_rnd_offs);
    isp_dpc1_bpt_thresh_write(be_reg, dyna_reg_cfg->dpcc_bad_thresh);

    isp_dpc1_line_kerdiff_fac_write(be_reg, dyna_reg_cfg->dpcc_line_kerdiff_fac);
    isp_dpc1_blend_mode_write(be_reg, dyna_reg_cfg->dpcc_blend_mode);
    isp_dpc1_bit_depth_sel_write(be_reg, dyna_reg_cfg->dpcc_bit_depth_sel);
    isp_dpc1_amp_coef_k_write(be_reg, dyna_reg_cfg->dpcc_amp_coef_k);
    isp_dpc1_amp_coef_min_write(be_reg, dyna_reg_cfg->dpcc_amp_coef_min);

    isp_dpc1_dyn_reg_std_config(be_reg, dyna_reg_cfg);
}

static td_s32 isp_dpc1_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_dpc_static_cfg *static_reg_cfg = TD_NULL;
    isp_dpc_dyna_cfg *dyna_reg_cfg = TD_NULL;

    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.dpc1_stt2lut_regnew = TD_FALSE;

    if (reg_cfg_info->cfg_key.bit1_dp1_cfg) {
        td_bool rgbir_en = ot_ext_system_rgbir_enable_read(vi_pipe);
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(pre_viproc);

        /* static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dp_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->static_resh1) {
            isp_dpc1_output_mode_write(be_reg, static_reg_cfg->dpcc_output_mode);
            isp_dpc1_bpt_ctrl_write(be_reg, static_reg_cfg->dpcc_bpt_ctrl);
            isp_dcg1_lut_width_word_write(pre_viproc, OT_ISP_DPC_LUT_WIDTH_WORD_DEFAULT);
            static_reg_cfg->static_resh1 = TD_FALSE;
        }

        /* usr */
        ret = isp_dpc1_usr_reg_config(vi_pipe, reg_cfg_info, i, be_reg, offline_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        /* dynamic */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dp_reg_cfg.dyna_reg_cfg;

        if (dyna_reg_cfg->resh1) {
            isp_dpc1_stat_en_write(pre_viproc, dyna_reg_cfg->dpc_stat_en);
            isp_dpc1_dyn_reg_config(be_reg, dyna_reg_cfg);
            dyna_reg_cfg->resh1 = offline_mode;
        }
        isp_dpc1_rgbir_dyn_reg_config(be_reg, dyna_reg_cfg, rgbir_en);

        reg_cfg_info->cfg_key.bit1_dp_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_ISP_CR_SUPPORT
static td_void isp_ge_static_reg_config(isp_pre_be_reg_type *be_reg, isp_ge_static_cfg *static_reg_cfg)
{
    if (static_reg_cfg->static_resh) {
        isp_ge_gr_en_write(be_reg, static_reg_cfg->ge_gr_en);
        isp_ge_gb_en_write(be_reg, static_reg_cfg->ge_gb_en);
        isp_ge_gr_gb_en_write(be_reg, static_reg_cfg->ge_gr_gb_en);
        isp_ge_bit_depth_sel_write(be_reg, OT_ISP_GE_BIT_DEPTH_DEFAULT);

        isp_ge1_gr_en_write(be_reg, static_reg_cfg->ge_gr_en);
        isp_ge1_gb_en_write(be_reg, static_reg_cfg->ge_gb_en);
        isp_ge1_gr_gb_en_write(be_reg, static_reg_cfg->ge_gr_gb_en);
        isp_ge1_bit_depth_sel_write(be_reg, OT_ISP_GE_BIT_DEPTH_DEFAULT);

        static_reg_cfg->static_resh = TD_FALSE;
    }
}

static td_void isp_ge_usr_reg_config(isp_pre_be_reg_type *be_reg, isp_ge_usr_cfg *usr_reg_cfg)
{
    isp_ge_ge0_ct_th2_write(be_reg, usr_reg_cfg->ge_ct_th2[0]);
    isp_ge_ge1_ct_th2_write(be_reg, usr_reg_cfg->ge_ct_th2[2]); /* ge_ct_th2[2] */
    isp_ge1_ge0_ct_th2_write(be_reg, usr_reg_cfg->ge_ct_th2[1]);
    isp_ge1_ge1_ct_th2_write(be_reg, usr_reg_cfg->ge_ct_th2[3]); /* ge_ct_th2[3] */

    isp_ge_ge0_ct_slope1_write(be_reg, usr_reg_cfg->ge_ct_slope1[0]);
    isp_ge_ge1_ct_slope1_write(be_reg, usr_reg_cfg->ge_ct_slope1[2]); /* ge_ct_slope1[2] */
    isp_ge1_ge0_ct_slope1_write(be_reg, usr_reg_cfg->ge_ct_slope1[1]);
    isp_ge1_ge1_ct_slope1_write(be_reg, usr_reg_cfg->ge_ct_slope1[3]); /* ge_ct_slope1[3] */

    isp_ge_ge0_ct_slope2_write(be_reg, usr_reg_cfg->ge_ct_slope2[0]);
    isp_ge_ge1_ct_slope2_write(be_reg, usr_reg_cfg->ge_ct_slope2[2]); /* ge_ct_slope2[2] */
    isp_ge1_ge0_ct_slope2_write(be_reg, usr_reg_cfg->ge_ct_slope2[1]);
    isp_ge1_ge1_ct_slope2_write(be_reg, usr_reg_cfg->ge_ct_slope2[3]); /* ge_ct_slope2[3] */
}

static td_void isp_ge_dyna_reg_config(isp_pre_be_reg_type *be_reg, isp_ge_dyna_cfg *dyna_reg_cfg)
{
    isp_ge_ge0_ct_th1_write(be_reg, dyna_reg_cfg->ge_ct_th1[0]);
    isp_ge_ge1_ct_th1_write(be_reg, dyna_reg_cfg->ge_ct_th1[2]); /* ge_ct_th1[2] */
    isp_ge1_ge0_ct_th1_write(be_reg, dyna_reg_cfg->ge_ct_th1[1]);
    isp_ge1_ge1_ct_th1_write(be_reg, dyna_reg_cfg->ge_ct_th1[3]); /* ge_ct_th1[3] */

    isp_ge_ge0_ct_th3_write(be_reg, dyna_reg_cfg->ge_ct_th3[0]);
    isp_ge_ge1_ct_th3_write(be_reg, dyna_reg_cfg->ge_ct_th3[2]); /* ge_ct_th3[2] */
    isp_ge1_ge0_ct_th3_write(be_reg, dyna_reg_cfg->ge_ct_th3[1]);
    isp_ge1_ge1_ct_th3_write(be_reg, dyna_reg_cfg->ge_ct_th3[3]); /* ge_ct_th3[3] */

    isp_ge_strength_write(be_reg, dyna_reg_cfg->ge_strength);
    isp_ge1_strength_write(be_reg, dyna_reg_cfg->ge_strength);
}
#endif
static td_s32 isp_ge_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
#ifdef CONFIG_OT_ISP_CR_SUPPORT
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    isp_ge_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_ge_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_ge_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        isp_ge_static_reg_config(be_reg, &reg_cfg_info->alg_reg_cfg[i].ge_reg_cfg.static_reg_cfg);

        /* usr */
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ge_reg_cfg.usr_reg_cfg;
        if (usr_reg_cfg->resh) {
            isp_ge_usr_reg_config(be_reg, usr_reg_cfg);

            usr_reg_cfg->resh = offline_mode;
        }

        /* dynamic */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ge_reg_cfg.dyna_reg_cfg;
        if (dyna_reg_cfg->resh) {
            isp_ge_dyna_reg_config(be_reg, dyna_reg_cfg);
            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_ge_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
#endif

    return TD_SUCCESS;
}

static td_s32 isp_lsc_usr_reg_config(td_bool *stt2_lut_regnew, ot_vi_pipe vi_pipe, const isp_reg_cfg *reg_cfg_info,
    td_u8 i, isp_lsc_usr_cfg *usr_reg_cfg)
{
    td_u8 buf_id;
    td_u16 j;
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_viproc);
    isp_check_pointer_return(be_reg);

    isp_lsc_width_offset_write(be_reg, usr_reg_cfg->width_offset);

    for (j = 0; j < (OT_ISP_LSC_GRID_ROW - 1) / 2; j++) { /* lsc grid row number divide by 2 */
        isp_lsc_win_y_info_write(be_reg, j, usr_reg_cfg->delta_y[j], usr_reg_cfg->inv_y[j]);
    }

    for (j = 0; j < (OT_ISP_LSC_GRID_COL - 1); j++) {
        isp_lsc_win_x_info_write(be_reg, j, usr_reg_cfg->delta_x[j], usr_reg_cfg->inv_x[j]);
    }

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        if (reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.lut2_stt_en == TD_TRUE) { /* online lut2stt regconfig */
            buf_id = usr_reg_cfg->buf_id;

            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(be_lut_stt_reg);

            isp_lsc_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, usr_reg_cfg->r_gain, usr_reg_cfg->gr_gain,
                usr_reg_cfg->gb_gain, usr_reg_cfg->b_gain);
            ret = isp_lsc_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
            if (ret != TD_SUCCESS) {
                isp_err_trace("isp[%d]_lsc_lut_wstt_addr_write failed\n", vi_pipe);
                return ret;
            }

            isp_lsc_stt2lut_en_write(be_reg, TD_TRUE);

            usr_reg_cfg->buf_id = 1 - buf_id;
            *stt2_lut_regnew = TD_TRUE;
        }
    } else {
        isp_lsc_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, usr_reg_cfg->r_gain, usr_reg_cfg->gr_gain,
            usr_reg_cfg->gb_gain, usr_reg_cfg->b_gain);
        isp_lsc_stt2lut_en_write(be_reg, TD_TRUE);
        isp_lsc_stt2lut_regnew_write(be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_void isp_lsc_static_reg_config(isp_post_be_reg_type *be_reg, isp_viproc_reg_type *post_viproc,
    isp_lsc_static_cfg *static_reg_cfg)
{
    if (static_reg_cfg->static_resh) {
        isp_lsc_numh_write(be_reg, static_reg_cfg->win_num_h);
        isp_lsc_numv_write(be_reg, static_reg_cfg->win_num_v);
        isp_lsc_lut_width_word_write(post_viproc, OT_ISP_LSC_LUT_WIDTH_WORD_DEFAULT);
        static_reg_cfg->static_resh = TD_FALSE;
    }
}

static td_void isp_lsc_usr_coef_reg_config(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_lsc_usr_cfg *usr_reg_cfg)
{
    if (usr_reg_cfg->coef_update) {
        isp_lsc_mesh_str_write(be_reg, usr_reg_cfg->mesh_str);
        isp_lsc_mesh_weight_write(be_reg, usr_reg_cfg->weight);
        usr_reg_cfg->coef_update = offline_mode;
    }
}

static td_void isp_lsc_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_lsc_usr_cfg *usr_reg_cfg, td_bool *lut_regnew)
{
    td_u32 lsc_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        lsc_stt2lut_clr = isp_lsc_stt2lut_clr_read(be_reg);
        if (lsc_stt2lut_clr != 0) {
            usr_reg_cfg->lut_update = TD_TRUE;
            *lut_regnew = TD_TRUE;
            isp_lsc_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_lsc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode, usr_resh, idx_resh;
    td_bool lut_update = TD_FALSE;
    td_bool stt2_lut_regnew = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_lsc_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_lsc_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg;

        isp_lsc_static_reg_config(be_reg, post_viproc, &reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.static_reg_cfg);
        isp_lsc_usr_coef_reg_config(offline_mode, be_reg, usr_reg_cfg);

        idx_resh = (isp_lsc_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (usr_reg_cfg->lut_update && idx_resh) : (usr_reg_cfg->lut_update);

        if (usr_resh) {
            isp_lsc_update_index_write(be_reg, usr_reg_cfg->update_index);

            ret = isp_lsc_usr_reg_config(&stt2_lut_regnew, vi_pipe, reg_cfg_info, i, usr_reg_cfg);
            if (ret != TD_SUCCESS) {
                return ret;
            }

            lut_update = TD_TRUE;

            usr_reg_cfg->lut_update = offline_mode;
        }

        isp_lsc_reg_update_online(offline_mode, be_reg, usr_reg_cfg, &stt2_lut_regnew);

        reg_cfg_info->cfg_key.bit1_lsc_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.lsc_lut_update = lut_update;
    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.lsc_stt2lut_regnew = stt2_lut_regnew;

    if (reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_lut2_stt_reg_new == TD_TRUE) {
        if (reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_reg_new_cnt < 3) { /* lsc reg config conunt 3 */
            reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.lsc_stt2lut_regnew = TD_TRUE;
            reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_reg_new_cnt++;
        } else {
            reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_lut2_stt_reg_new = TD_FALSE;
            reg_cfg_info->alg_reg_cfg[i].lsc_reg_cfg.usr_reg_cfg.switch_reg_new_cnt = 0;
        }
    }

    return TD_SUCCESS;
}
static td_s32 isp_rc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_radial_crop_reg_cfg *rc_reg_cfg = TD_NULL;

    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_viproc);

    rc_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].rc_reg_cfg;
    isp_radial_crop_en_write(post_viproc, rc_reg_cfg->rc_en);
    isp_ratio_center_hor_write(post_viproc, rc_reg_cfg->usr_reg_cfg.center_hor_coor);
    isp_ratio_center_ver_write(post_viproc, rc_reg_cfg->usr_reg_cfg.center_ver_coor);
    isp_ratio_square_of_radius_write(post_viproc, rc_reg_cfg->usr_reg_cfg.square_radius);

    return TD_SUCCESS;
}

static td_void isp_rgbir_static_reg_config(isp_pre_be_reg_type *be_reg, isp_rgbir_static_cfg *static_reg_cfg)
{
    isp_rgbir_th_write(be_reg, static_reg_cfg->thres_h);
    isp_rgbir_tv_write(be_reg, static_reg_cfg->thres_v);
    isp_rgbir_lut_0_write(be_reg, static_reg_cfg->crosstalk_lut[0]);   /* index[0] */
    isp_rgbir_lut_1_write(be_reg, static_reg_cfg->crosstalk_lut[1]);   /* index[1] */
    isp_rgbir_lut_2_write(be_reg, static_reg_cfg->crosstalk_lut[2]);   /* index[2] */
    isp_rgbir_lut_3_write(be_reg, static_reg_cfg->crosstalk_lut[3]);   /* index[3] */
    isp_rgbir_lut_4_write(be_reg, static_reg_cfg->crosstalk_lut[4]);   /* index[4] */
    isp_rgbir_lut_5_write(be_reg, static_reg_cfg->crosstalk_lut[5]);   /* index[5] */
    isp_rgbir_lut_6_write(be_reg, static_reg_cfg->crosstalk_lut[6]);   /* index[6] */
    isp_rgbir_lut_7_write(be_reg, static_reg_cfg->crosstalk_lut[7]);   /* index[7] */
    isp_rgbir_lut_8_write(be_reg, static_reg_cfg->crosstalk_lut[8]);   /* index[8] */
    isp_rgbir_lut_9_write(be_reg, static_reg_cfg->crosstalk_lut[9]);   /* index[9] */
    isp_rgbir_lut_10_write(be_reg, static_reg_cfg->crosstalk_lut[10]); /* index[10] */
    isp_rgbir_lut_11_write(be_reg, static_reg_cfg->crosstalk_lut[11]); /* index[11] */
    isp_rgbir_lut_12_write(be_reg, static_reg_cfg->crosstalk_lut[12]); /* index[12] */
    isp_rgbir_lut_13_write(be_reg, static_reg_cfg->crosstalk_lut[13]); /* index[13] */
    isp_rgbir_lut_14_write(be_reg, static_reg_cfg->crosstalk_lut[14]); /* index[14] */
    isp_rgbir_lut_15_write(be_reg, static_reg_cfg->crosstalk_lut[15]); /* index[15] */
    isp_rgbir_lut_16_write(be_reg, static_reg_cfg->crosstalk_lut[16]); /* index[16] */
    isp_rgbir_lut_17_write(be_reg, static_reg_cfg->crosstalk_lut[17]); /* index[17] */
    isp_rgbir_lut_18_write(be_reg, static_reg_cfg->crosstalk_lut[18]); /* index[18] */
    isp_rgbir_lut_19_write(be_reg, static_reg_cfg->crosstalk_lut[19]); /* index[19] */
    isp_rgbir_lut_20_write(be_reg, static_reg_cfg->crosstalk_lut[20]); /* index[20] */
    isp_rgbir_lut_21_write(be_reg, static_reg_cfg->crosstalk_lut[21]); /* index[21] */
    isp_rgbir_lut_22_write(be_reg, static_reg_cfg->crosstalk_lut[22]); /* index[22] */
    isp_rgbir_lut_23_write(be_reg, static_reg_cfg->crosstalk_lut[23]); /* index[23] */
    isp_rgbir_lut_24_write(be_reg, static_reg_cfg->crosstalk_lut[24]); /* index[24] */
    isp_rgbir_lut_25_write(be_reg, static_reg_cfg->crosstalk_lut[25]); /* index[25] */
    isp_rgbir_lut_26_write(be_reg, static_reg_cfg->crosstalk_lut[26]); /* index[26] */
    isp_rgbir_lut_27_write(be_reg, static_reg_cfg->crosstalk_lut[27]); /* index[27] */
    isp_rgbir_lut_28_write(be_reg, static_reg_cfg->crosstalk_lut[28]); /* index[28] */
    isp_rgbir_lut_29_write(be_reg, static_reg_cfg->crosstalk_lut[29]); /* index[29] */
    isp_rgbir_lut_30_write(be_reg, static_reg_cfg->crosstalk_lut[30]); /* index[30] */
    isp_rgbir_lut_31_write(be_reg, static_reg_cfg->crosstalk_lut[31]); /* index[31] */
    isp_rgbir_lut_32_write(be_reg, static_reg_cfg->crosstalk_lut[32]); /* index[32] */
}

static td_void isp_rgbir_usr_reg_config(isp_pre_be_reg_type *be_reg, isp_rgbir_usr_cfg *usr_reg_cfg)
{
    isp_rgbir_ct_ratio_r_write(be_reg, usr_reg_cfg->ir_rm_ratio[0]); /* index[0] */
    isp_rgbir_ct_ratio_g_write(be_reg, usr_reg_cfg->ir_rm_ratio[1]); /* index[1] */
    isp_rgbir_ct_ratio_b_write(be_reg, usr_reg_cfg->ir_rm_ratio[2]); /* index[2] */
    isp_rgbir_smooth_enable_write(be_reg, usr_reg_cfg->smooth_enable);
    isp_rgbir_bitdepth_write(be_reg, 12); /* 12bit */

    isp_rgbir_exp_thr1_write(be_reg, usr_reg_cfg->exp_ctrl1);
    isp_rgbir_exp_thr2_write(be_reg, usr_reg_cfg->exp_ctrl2);
    isp_rgbir_reci_ctl1_write(be_reg, usr_reg_cfg->reci_exp1);
    isp_rgbir_reci_ctl2_write(be_reg, usr_reg_cfg->reci_exp2);
    isp_rgbir_ir_pattern_in_write(be_reg, usr_reg_cfg->in_pattern);
    isp_rgbir_pattern_out_write(be_reg, usr_reg_cfg->out_pattern);
    isp_rgbir_gain_r_write(be_reg, usr_reg_cfg->gain_r);
    isp_rgbir_gain_b_write(be_reg, usr_reg_cfg->gain_b);
    isp_rgbir_thd0_grad_write(be_reg, usr_reg_cfg->thd_alpha);
    isp_rgbir_thd1_grad_write(be_reg, usr_reg_cfg->thd_beta);
    isp_rgbir_mode_write(be_reg, usr_reg_cfg->mode);
    isp_rgbir_out_type_write(be_reg, usr_reg_cfg->out_type);

    isp_rgbir_r_ctl_str_write(be_reg, usr_reg_cfg->wb_ctrl[0]); /* index[0] */
    isp_rgbir_b_ctl_str_write(be_reg, usr_reg_cfg->wb_ctrl[1]); /* index[1] */

    isp_rgbir_upscale_en_write(be_reg, usr_reg_cfg->is_ir_upscale);
    isp_rgbir_ir_remove_en_write(be_reg, usr_reg_cfg->ir_rm_enable);
    isp_rgbir_bayer_pattern_in_write(be_reg, usr_reg_cfg->in_bayer_pattern);
    isp_rgbir_matrix0_write(be_reg, usr_reg_cfg->cvt_matrix[0]);   /* index[0] */
    isp_rgbir_matrix1_write(be_reg, usr_reg_cfg->cvt_matrix[1]);   /* index[1] */
    isp_rgbir_matrix2_write(be_reg, usr_reg_cfg->cvt_matrix[2]);   /* index[2] */
    isp_rgbir_matrix3_write(be_reg, usr_reg_cfg->cvt_matrix[3]);   /* index[3] */
    isp_rgbir_matrix4_write(be_reg, usr_reg_cfg->cvt_matrix[4]);   /* index[4] */
    isp_rgbir_matrix5_write(be_reg, usr_reg_cfg->cvt_matrix[5]);   /* index[5] */
    isp_rgbir_matrix6_write(be_reg, usr_reg_cfg->cvt_matrix[6]);   /* index[6] */
    isp_rgbir_matrix7_write(be_reg, usr_reg_cfg->cvt_matrix[7]);   /* index[7] */
    isp_rgbir_matrix8_write(be_reg, usr_reg_cfg->cvt_matrix[8]);   /* index[8] */
    isp_rgbir_matrix9_write(be_reg, usr_reg_cfg->cvt_matrix[9]);   /* index[9] */
    isp_rgbir_matrix10_write(be_reg, usr_reg_cfg->cvt_matrix[10]); /* index[10] */
    isp_rgbir_matrix11_write(be_reg, usr_reg_cfg->cvt_matrix[11]); /* index[11] */
}

static td_s32 isp_rgbir_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool idx_resh;
    td_bool usr_resh;
    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx_cfg = TD_NULL;
    isp_rgbir_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_rgbir_static_cfg *static_reg_cfg = TD_NULL;
    isp_rgbir_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx_cfg);
    td_bool offline_mode = (is_offline_mode(isp_ctx_cfg->block_attr.running_mode) ||
        is_striping_mode(isp_ctx_cfg->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_rgbir_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        isp_rgbir_sel_write(be_reg, 1); /* 0: rgbir-dpc; 1: dpc-rgbir; */

        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].rgbir_reg_cfg.usr_reg_cfg;
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].rgbir_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].rgbir_reg_cfg.dyna_reg_cfg;
        if (static_reg_cfg->static_resh) {
            isp_rgbir_static_reg_config(be_reg, static_reg_cfg);
            static_reg_cfg->static_resh = TD_FALSE;
        }

        idx_resh = (isp_rgbir_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (usr_reg_cfg->usr_resh && idx_resh) : (usr_reg_cfg->usr_resh);
        if (usr_resh) {
            isp_rgbir_update_index_write(be_reg, usr_reg_cfg->update_index);
            isp_rgbir_usr_reg_config(be_reg, usr_reg_cfg);
            usr_reg_cfg->usr_resh = offline_mode;
        }
        if (dyna_reg_cfg->dyna_resh) {
            isp_rgbir_awb_rgain_write(be_reg, dyna_reg_cfg->wb_gain[0]); /* index[0] */
            isp_rgbir_awb_bgain_write(be_reg, dyna_reg_cfg->wb_gain[1]); /* index[1] */
            isp_rgbir_auto_gain_enable_write(be_reg, dyna_reg_cfg->auto_gain_enable);
            isp_rgbir_auto_gain_write(be_reg, dyna_reg_cfg->auto_gain);
            dyna_reg_cfg->dyna_resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_rgbir_cfg = offline_mode ? 1 : ((reg_cfg_info->cfg_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_gamma_lut_reg_config(td_bool *stt2_lut_regnew, ot_vi_pipe vi_pipe, td_u8 i,
    isp_post_be_reg_type *be_reg, isp_gamma_usr_cfg *usr_reg_cfg)
{
    td_u8 buf_id;
    td_s32 ret;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_viproc);

    isp_gamma_lut_width_word_write(post_viproc, OT_ISP_GAMMA_LUT_WIDTH_WORD_DEFAULT);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        if (usr_reg_cfg->switch_mode != TD_TRUE) {
            buf_id = usr_reg_cfg->buf_id;

            be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
            isp_check_pointer_return(be_lut_stt_reg);

            isp_gamma_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, usr_reg_cfg->gamma_lut);
            ret = isp_gamma_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
            if (ret != TD_SUCCESS) {
                isp_err_trace("isp[%d]_gamma_lut_wstt_addr_write failed\n", vi_pipe);
                return ret;
            }
            isp_gamma_stt2lut_en_write(be_reg, TD_TRUE);
            usr_reg_cfg->buf_id = 1 - buf_id;
            *stt2_lut_regnew = TD_TRUE;
        }
    } else {
        isp_gamma_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, usr_reg_cfg->gamma_lut);
        isp_gamma_stt2lut_en_write(be_reg, TD_TRUE);
        isp_gamma_stt2lut_regnew_write(be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_void isp_gamma_inseg_write(isp_post_be_reg_type *be_reg, td_u16 *gamma_in_seg)
{
    isp_gamma_inseg_0_write(be_reg, gamma_in_seg[0]); /* index[0] */
    isp_gamma_inseg_1_write(be_reg, gamma_in_seg[1]); /* index[1] */
    isp_gamma_inseg_2_write(be_reg, gamma_in_seg[2]); /* index[2] */
    isp_gamma_inseg_3_write(be_reg, gamma_in_seg[3]); /* index[3] */
    isp_gamma_inseg_4_write(be_reg, gamma_in_seg[4]); /* index[4] */
    isp_gamma_inseg_5_write(be_reg, gamma_in_seg[5]); /* index[5] */
    isp_gamma_inseg_6_write(be_reg, gamma_in_seg[6]); /* index[6] */
    isp_gamma_inseg_7_write(be_reg, gamma_in_seg[7]); /* index[7] */
}

static td_void isp_gamma_pos_write(isp_post_be_reg_type *be_reg, td_u16 *gamma_pos)
{
    isp_gamma_pos_0_write(be_reg, gamma_pos[0]); /* index[0] */
    isp_gamma_pos_1_write(be_reg, gamma_pos[1]); /* index[1] */
    isp_gamma_pos_2_write(be_reg, gamma_pos[2]); /* index[2] */
    isp_gamma_pos_3_write(be_reg, gamma_pos[3]); /* index[3] */
    isp_gamma_pos_4_write(be_reg, gamma_pos[4]); /* index[4] */
    isp_gamma_pos_5_write(be_reg, gamma_pos[5]); /* index[5] */
    isp_gamma_pos_6_write(be_reg, gamma_pos[6]); /* index[6] */
    isp_gamma_pos_7_write(be_reg, gamma_pos[7]); /* index[7] */
}

static td_void isp_gamma_step_write(isp_post_be_reg_type *be_reg, td_u8 *gamma_step)
{
    isp_gamma_step0_write(be_reg, gamma_step[0]); /* index[0] */
    isp_gamma_step1_write(be_reg, gamma_step[1]); /* index[1] */
    isp_gamma_step2_write(be_reg, gamma_step[2]); /* index[2] */
    isp_gamma_step3_write(be_reg, gamma_step[3]); /* index[3] */
    isp_gamma_step4_write(be_reg, gamma_step[4]); /* index[4] */
    isp_gamma_step5_write(be_reg, gamma_step[5]); /* index[5] */
    isp_gamma_step6_write(be_reg, gamma_step[6]); /* index[6] */
    isp_gamma_step7_write(be_reg, gamma_step[7]); /* index[7] */
}

static td_void isp_gamma_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_gamma_usr_cfg *usr_reg_cfg, td_bool *lut_regnew)
{
    td_u32 gamma_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        gamma_stt2lut_clr = isp_gamma_stt2lut_clr_read(be_reg);
        if (gamma_stt2lut_clr != 0) {
            usr_reg_cfg->gamma_lut_update_en = TD_TRUE;
            *lut_regnew = TD_TRUE;
            isp_gamma_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_gamma_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool usr_resh;
    td_bool idx_resh;
    td_bool stt2_lut_regnew = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_gamma_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].gamma_cfg.usr_reg_cfg;

    if (reg_cfg_info->cfg_key.bit1_gamma_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        idx_resh = (isp_gamma_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (usr_reg_cfg->gamma_lut_update_en && idx_resh) : (usr_reg_cfg->gamma_lut_update_en);

        if (usr_resh) {
            isp_gamma_update_index_write(be_reg, usr_reg_cfg->update_index);

            ret = isp_gamma_lut_reg_config(&stt2_lut_regnew, vi_pipe, i, be_reg, usr_reg_cfg);
            if (ret != TD_SUCCESS) {
                return ret;
            }

            isp_gamma_inseg_write(be_reg, usr_reg_cfg->gamma_in_seg);
            isp_gamma_pos_write(be_reg, usr_reg_cfg->gamma_pos);
            isp_gamma_step_write(be_reg, usr_reg_cfg->gamma_step);

            usr_reg_cfg->gamma_lut_update_en = offline_mode;
            usr_reg_cfg->switch_mode = TD_FALSE;
        }

        isp_gamma_reg_update_online(offline_mode, be_reg, usr_reg_cfg, &stt2_lut_regnew);

        reg_cfg_info->cfg_key.bit1_gamma_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.gamma_stt2lut_regnew = stt2_lut_regnew;

    return TD_SUCCESS;
}
static td_s32 isp_csc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    isp_csc_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_csc_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].csc_cfg.dyna_reg_cfg;

        /* dynamic */
        if (dyna_reg_cfg->resh) {
            isp_csc_coef00_write(be_reg, dyna_reg_cfg->csc_coef[0]);  /* coef0 */
            isp_csc_coef01_write(be_reg, dyna_reg_cfg->csc_coef[1]);  /* coef1 */
            isp_csc_coef02_write(be_reg, dyna_reg_cfg->csc_coef[2]);  /* coef2 */
            isp_csc_coef10_write(be_reg, dyna_reg_cfg->csc_coef[3]);  /* coef3 */
            isp_csc_coef11_write(be_reg, dyna_reg_cfg->csc_coef[4]);  /* coef4 */
            isp_csc_coef12_write(be_reg, dyna_reg_cfg->csc_coef[5]);  /* coef5 */
            isp_csc_coef20_write(be_reg, dyna_reg_cfg->csc_coef[6]);  /* coef6 */
            isp_csc_coef21_write(be_reg, dyna_reg_cfg->csc_coef[7]);  /* coef7 */
            isp_csc_coef22_write(be_reg, dyna_reg_cfg->csc_coef[8]);  /* coef8 */
            isp_csc_in_dc0_write(be_reg, dyna_reg_cfg->csc_in_dc[0]); /* in_dc0 */
            isp_csc_in_dc1_write(be_reg, dyna_reg_cfg->csc_in_dc[1]); /* in_dc1 */
            isp_csc_in_dc2_write(be_reg, dyna_reg_cfg->csc_in_dc[2]); /* in_dc2 */

            isp_csc_out_dc0_write(be_reg, dyna_reg_cfg->csc_out_dc[0]); /* out_dc0 */
            isp_csc_out_dc1_write(be_reg, dyna_reg_cfg->csc_out_dc[1]); /* out_dc1 */
            isp_csc_out_dc2_write(be_reg, dyna_reg_cfg->csc_out_dc[2]); /* out_dc2 */
            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_csc_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_ISP_CA_SUPPORT
static td_s32 isp_ca_lut_reg_config(isp_usr_ctx *isp_ctx, isp_post_be_reg_type *be_reg, isp_ca_usr_cfg *usr_reg_cfg)
{
    td_u16 j;

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_ca_lut_waddr_write(be_reg, 0);
        for (j = 0; j < OT_ISP_CA_YRATIO_LUT_LENGTH; j++) {
            isp_ca_lut_wdata_write(be_reg, usr_reg_cfg->y_ratio_lut[j]);
        }
    } else {
        isp_ca_lut_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->y_ratio_lut);
    }

    return TD_SUCCESS;
}

static td_void isp_ca_yuv2rgb_coef_write(isp_post_be_reg_type *be_reg, td_s16 *ca_yuv2_rgb_coef)
{
    isp_ca_yuv2rgb_coef00_write(be_reg, ca_yuv2_rgb_coef[0]); /* coef0 */
    isp_ca_yuv2rgb_coef01_write(be_reg, ca_yuv2_rgb_coef[1]); /* coef1 */
    isp_ca_yuv2rgb_coef02_write(be_reg, ca_yuv2_rgb_coef[2]); /* coef2 */
    isp_ca_yuv2rgb_coef10_write(be_reg, ca_yuv2_rgb_coef[3]); /* coef3 */
    isp_ca_yuv2rgb_coef11_write(be_reg, ca_yuv2_rgb_coef[4]); /* coef4 */
    isp_ca_yuv2rgb_coef12_write(be_reg, ca_yuv2_rgb_coef[5]); /* coef5 */
    isp_ca_yuv2rgb_coef20_write(be_reg, ca_yuv2_rgb_coef[6]); /* coef6 */
    isp_ca_yuv2rgb_coef21_write(be_reg, ca_yuv2_rgb_coef[7]); /* coef7 */
    isp_ca_yuv2rgb_coef22_write(be_reg, ca_yuv2_rgb_coef[8]); /* coef8 */
}

static td_void isp_ca_yuv2rgb_indc_write(isp_post_be_reg_type *be_reg, td_s16 *ca_yuv2_rgb_in_dc)
{
    isp_ca_yuv2rgb_indc0_write(be_reg, ca_yuv2_rgb_in_dc[0]); /* indc0 */
    isp_ca_yuv2rgb_indc1_write(be_reg, ca_yuv2_rgb_in_dc[1]); /* indc1 */
    isp_ca_yuv2rgb_indc2_write(be_reg, ca_yuv2_rgb_in_dc[2]); /* indc2 */
}

static td_void isp_ca_yuv2rgb_outdc_write(isp_post_be_reg_type *be_reg, td_s16 *ca_yuv2_rgb_out_dc)
{
    isp_ca_yuv2rgb_outdc0_write(be_reg, ca_yuv2_rgb_out_dc[0]); /* outdc0 */
    isp_ca_yuv2rgb_outdc1_write(be_reg, ca_yuv2_rgb_out_dc[1]); /* outdc1 */
    isp_ca_yuv2rgb_outdc2_write(be_reg, ca_yuv2_rgb_out_dc[2]); /* outdc2 */
}

static td_void isp_ca_static_reg_config(isp_post_be_reg_type *be_reg, isp_ca_static_cfg *static_reg_cfg)
{
    if (static_reg_cfg->static_resh) {
        isp_ca_llhcproc_en_write(be_reg, static_reg_cfg->ca_llhc_proc_en);
        isp_ca_skinproc_en_write(be_reg, static_reg_cfg->ca_skin_proc_en);
        isp_ca_satadj_en_write(be_reg, static_reg_cfg->ca_satu_adj_en);
        isp_ca_lumath_low_write(be_reg, static_reg_cfg->ca_luma_thr_low);
        isp_ca_darkchromath_low_write(be_reg, static_reg_cfg->ca_dark_chroma_thr_low);
        isp_ca_darkchromath_high_write(be_reg, static_reg_cfg->ca_dark_chroma_thr_high);
        isp_ca_sdarkchromath_low_write(be_reg, static_reg_cfg->ca_s_dark_chroma_thr_low);
        isp_ca_sdarkchromath_high_write(be_reg, static_reg_cfg->ca_s_dark_chroma_thr_high);
        isp_ca_lumaratio_low_write(be_reg, static_reg_cfg->ca_luma_ratio_low);
        isp_ca_yuv2rgb_coef_write(be_reg, static_reg_cfg->ca_yuv2_rgb_coef);
        isp_ca_yuv2rgb_indc_write(be_reg, static_reg_cfg->ca_yuv2_rgb_in_dc);
        isp_ca_yuv2rgb_outdc_write(be_reg, static_reg_cfg->ca_yuv2_rgb_out_dc);
        isp_ca_skinlluma_umin_write(be_reg, static_reg_cfg->ca_skin_low_luma_min_u);
        isp_ca_skinlluma_umax_write(be_reg, static_reg_cfg->ca_skin_low_luma_max_u);
        isp_ca_skinlluma_uymin_write(be_reg, static_reg_cfg->ca_skin_low_luma_min_uy);
        isp_ca_skinlluma_uymax_write(be_reg, static_reg_cfg->ca_skin_low_luma_max_uy);
        isp_ca_skinhluma_umin_write(be_reg, static_reg_cfg->ca_skin_high_luma_min_u);
        isp_ca_skinhluma_umax_write(be_reg, static_reg_cfg->ca_skin_high_luma_max_u);
        isp_ca_skinhluma_uymin_write(be_reg, static_reg_cfg->ca_skin_high_luma_min_uy);
        isp_ca_skinhluma_uymax_write(be_reg, static_reg_cfg->ca_skin_high_luma_max_uy);
        isp_ca_skinlluma_vmin_write(be_reg, static_reg_cfg->ca_skin_low_luma_min_v);
        isp_ca_skinlluma_vmax_write(be_reg, static_reg_cfg->ca_skin_low_luma_max_v);
        isp_ca_skinlluma_vymin_write(be_reg, static_reg_cfg->ca_skin_low_luma_min_vy);
        isp_ca_skinlluma_vymax_write(be_reg, static_reg_cfg->ca_skin_low_luma_max_vy);
        isp_ca_skinhluma_vmin_write(be_reg, static_reg_cfg->ca_skin_high_luma_min_v);
        isp_ca_skinhluma_vmax_write(be_reg, static_reg_cfg->ca_skin_high_luma_max_v);
        isp_ca_skinhluma_vymin_write(be_reg, static_reg_cfg->ca_skin_high_luma_min_vy);
        isp_ca_skinhluma_vymax_write(be_reg, static_reg_cfg->ca_skin_high_luma_max_vy);
        isp_ca_skin_uvdiff_write(be_reg, static_reg_cfg->ca_skin_uv_diff);
        isp_ca_skinratioth_low_write(be_reg, static_reg_cfg->ca_skin_ratio_thr_low);
        isp_ca_skinratioth_mid_write(be_reg, static_reg_cfg->ca_skin_ratio_thr_mid);
        isp_ca_skinratioth_high_write(be_reg, static_reg_cfg->ca_skin_ratio_thr_high);

        static_reg_cfg->static_resh = TD_FALSE;
    }
}

#endif

static td_s32 isp_ca_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
#ifdef CONFIG_OT_ISP_CA_SUPPORT
    td_bool offline_mode;
    td_bool usr_resh;
    td_bool idx_resh;
    td_bool lut_update = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_ca_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_ca_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);

        isp_ca_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].ca_reg_cfg.ca_en);
        /* usr */
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].ca_reg_cfg.usr_reg_cfg;
        idx_resh = (isp_ca_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (usr_reg_cfg->resh && idx_resh) : (usr_reg_cfg->resh);

        if (usr_resh) {
            isp_ca_update_index_write(be_reg, usr_reg_cfg->update_index);
            isp_ca_cp_en_write(be_reg, usr_reg_cfg->ca_cp_en);
            isp_ca_lumath_high_write(be_reg, usr_reg_cfg->ca_luma_thr_high);
            isp_ca_lumaratio_high_write(be_reg, usr_reg_cfg->ca_luma_ratio_high);

            ret = isp_ca_lut_reg_config(isp_ctx, be_reg, usr_reg_cfg);
            if (ret != TD_SUCCESS) {
                return ret;
            }
            lut_update = TD_TRUE;
            usr_reg_cfg->resh = offline_mode;
        }

        isp_ca_isoratio_write(be_reg, reg_cfg_info->alg_reg_cfg[i].ca_reg_cfg.dyna_reg_cfg.ca_iso_ratio);

        /* static */
        isp_ca_static_reg_config(be_reg, &reg_cfg_info->alg_reg_cfg[i].ca_reg_cfg.static_reg_cfg);

        reg_cfg_info->cfg_key.bit1_ca_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.ca_lut_update = lut_update;
#endif

    return TD_SUCCESS;
}

static td_s32 crb_reg_config(ot_vi_pipe vi_pipe, const isp_usr_ctx *isp_ctx, td_u8 i, isp_crb_usr_cfg *usr_reg_cfg)
{
    td_u8 buf_id;
    td_s32 ret;
    isp_pre_be_lut_wstt_type *pre_lut_stt_reg = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;

    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(pre_be_reg);
    isp_check_pointer_return(pre_viproc);

    /* online Lut2stt regconfig */
    buf_id = usr_reg_cfg->buf_id;
    if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        pre_lut_stt_reg = (isp_pre_be_lut_wstt_type *)isp_get_pre_on_post_off_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_return(pre_lut_stt_reg);
        isp_crb_lut_wstt_write(pre_lut_stt_reg, usr_reg_cfg->crb_lut);
    } else {
        be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_pre_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_return(be_lut_stt_reg);
        isp_crb_lut_wstt_write(&be_lut_stt_reg->pre_be_lut2stt, usr_reg_cfg->crb_lut);
    }

    ret = isp_crb_lut_wstt_addr_write(vi_pipe, i, buf_id, pre_viproc);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] isp_crb_lut_wstt_addr_write failed\n", vi_pipe);
        return ret;
    }
    isp_crb_stt2lut_en_write(pre_be_reg, TD_TRUE);
    isp_crb_stt2lut_regnew_write(pre_be_reg, TD_TRUE);
    usr_reg_cfg->buf_id = 1 - buf_id;

    return TD_SUCCESS;
}

static td_s32 isp_crb_usr_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i, td_bool offline_mode)
{
    td_bool usr_resh, idx_resh;

    isp_crb_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(pre_be_reg);
    isp_check_pointer_return(pre_viproc);
    usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].crb_reg_cfg.usr_reg_cfg;
    idx_resh = (isp_crb_update_index_read(pre_be_reg) != usr_reg_cfg->update_index);
    usr_resh = (offline_mode) ? (usr_reg_cfg->resh && idx_resh) : (usr_reg_cfg->resh);
    if (usr_resh == TD_FALSE) {
        return TD_SUCCESS;
    }

    isp_crb_update_index_write(pre_be_reg, usr_reg_cfg->update_index);
    isp_crb_cc0_write(pre_be_reg, usr_reg_cfg->rgain);
    isp_crb_cc1_write(pre_be_reg, usr_reg_cfg->bgain);
    isp_crb_lut_width_word_write(pre_viproc, OT_ISP_CRB_LUT_WIDTH_WORD_DEFAULT);
    if (is_online_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        crb_reg_config(vi_pipe, isp_ctx, i, usr_reg_cfg);
    } else {
        isp_crb_lut_wstt_write(&pre_be_reg->pre_be_lut.pre_be_lut2stt, usr_reg_cfg->crb_lut);
        isp_crb_stt2lut_en_write(pre_be_reg, TD_TRUE);
        isp_crb_stt2lut_regnew_write(pre_be_reg, TD_TRUE);
    }
    usr_reg_cfg->resh = offline_mode;

    return TD_SUCCESS;
}

static td_s32 isp_crb_dyna_reg_config(isp_pre_be_reg_type *pre_be_reg, isp_crb_dyna_cfg *dyna_reg_cfg,
    td_bool offline_mode)
{
    /* dynamic */
    if (dyna_reg_cfg->resh) {
        isp_crb_kx_write(pre_be_reg, dyna_reg_cfg->phasex);
        isp_crb_ky_write(pre_be_reg, dyna_reg_cfg->phasey);
        isp_crb_smlmapoffset_write(pre_be_reg, dyna_reg_cfg->map_offset);
        isp_crb_awbgain0_write(pre_be_reg, dyna_reg_cfg->awbgain0);
        isp_crb_awbgain1_write(pre_be_reg, dyna_reg_cfg->awbgain1);
        isp_crb_wdr_enable_write(pre_be_reg, dyna_reg_cfg->wdr_enable);
        isp_crb_wdr_fusion_en_write(pre_be_reg, dyna_reg_cfg->wdr_fusion);
        isp_crb_stepthirdenable_write(pre_be_reg, dyna_reg_cfg->step_third_en);
        dyna_reg_cfg->resh = offline_mode;
    }
    return TD_SUCCESS;
}

static td_s32 isp_crb_static_reg_config(isp_pre_be_reg_type *pre_be_reg, isp_crb_static_cfg *static_reg_cfg,
    td_bool offline_mode)
{
    /* static */
    if (static_reg_cfg->resh) {
        isp_crb_maxlevel_write(pre_be_reg, static_reg_cfg->maxlevel);
        isp_crb_mode_write(pre_be_reg, static_reg_cfg->mode);
        isp_crb_stepfirstenable_write(pre_be_reg, static_reg_cfg->step_first_en);
        isp_crb_stepsecondenable_write(pre_be_reg, static_reg_cfg->step_second_en);
        isp_crb_dc_numh_write(pre_be_reg, static_reg_cfg->dchnum);
        isp_crb_dc_numv_write(pre_be_reg, static_reg_cfg->dcvnum);

        isp_crb_inblc_r_write(pre_be_reg, 0);
        isp_crb_inblc_gr_write(pre_be_reg, 0);
        isp_crb_inblc_gb_write(pre_be_reg, 0);
        isp_crb_inblc_b_write(pre_be_reg, 0);
        isp_crb_outblc_r_write(pre_be_reg, 0);
        isp_crb_outblc_gr_write(pre_be_reg, 0);
        isp_crb_outblc_gb_write(pre_be_reg, 0);
        isp_crb_outblc_b_write(pre_be_reg, 0);

        isp_crb_rccmode_write(pre_be_reg, 0);
        isp_crb_ccrcc0_write(pre_be_reg, 448);                           /* parameter is 448 */
        isp_crb_ccrcc1_write(pre_be_reg, 604);                           /* parameter is 604 */
        isp_crb_gr0_write(pre_be_reg, 1024);                             /* parameter is 1024 */
        isp_crb_gr1_write(pre_be_reg, 3000);                             /* parameter is 3000 */
        isp_crb_cchigh_write(pre_be_reg, 1436);                          /* parameter is 1436 */
        isp_crb_mix0_write(pre_be_reg, 1023);                            /* parameter is 1023 */
        isp_crb_highlevel_write(pre_be_reg, ((2 << 12) - 1 - 240) * 16); /* parameter is 2 12 240 16 */

        static_reg_cfg->resh = offline_mode;
    }
    return TD_SUCCESS;
}

static td_s32 isp_crb_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_crb_static_cfg *static_reg_cfg = TD_NULL;
    isp_crb_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_crb_cfg) {
        pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(pre_be_reg);
        isp_check_pointer_return(pre_viproc);

        isp_crb_en_write(pre_viproc, reg_cfg_info->alg_reg_cfg[i].crb_reg_cfg.crb_en);
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].crb_reg_cfg.dyna_reg_cfg;
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].crb_reg_cfg.static_reg_cfg;
        /* usr */
        ret = isp_crb_usr_reg_config(vi_pipe, reg_cfg_info, i, offline_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        ret = isp_crb_dyna_reg_config(pre_be_reg, dyna_reg_cfg, offline_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        ret = isp_crb_static_reg_config(pre_be_reg, static_reg_cfg, offline_mode);
        if (ret != TD_SUCCESS) {
            return ret;
        }
        reg_cfg_info->cfg_key.bit1_crb_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
    return TD_SUCCESS;
}

static td_s32 isp_mcds_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    isp_mcds_static_reg_cfg *static_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_mcds_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);

        isp_hcds_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].mcds_reg_cfg.hcds_en);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].mcds_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->static_resh) {
            isp_hcds_coefh0_write(be_reg, static_reg_cfg->h_coef[0]); /* coefh0 */
            isp_hcds_coefh1_write(be_reg, static_reg_cfg->h_coef[1]); /* coefh1 */
            isp_hcds_coefh2_write(be_reg, static_reg_cfg->h_coef[2]); /* coefh2 */
            isp_hcds_coefh3_write(be_reg, static_reg_cfg->h_coef[3]); /* coefh3 */
            isp_hcds_coefh4_write(be_reg, static_reg_cfg->h_coef[4]); /* coefh4 */
            isp_hcds_coefh5_write(be_reg, static_reg_cfg->h_coef[5]); /* coefh5 */
            isp_hcds_coefh6_write(be_reg, static_reg_cfg->h_coef[6]); /* coefh6 */
            isp_hcds_coefh7_write(be_reg, static_reg_cfg->h_coef[7]); /* coefh7 */
            isp_vcds_coefv0_write(be_reg, static_reg_cfg->v_coef[0]);
            isp_vcds_coefv1_write(be_reg, static_reg_cfg->v_coef[1]);
            static_reg_cfg->static_resh = TD_FALSE;
        }

        reg_cfg_info->cfg_key.bit1_mcds_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_void isp_wdr_lut_def_reg_config(ot_vi_pipe vi_pipe, isp_pre_be_reg_type *pre_be_reg,
    isp_fswdr_dyna_cfg *dyna_reg_cfg)
{
    td_u16 j;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_wdr_snoise_lut_waddr_write(pre_be_reg, 0x0);
        isp_wdr_lnoise_lut_waddr_write(pre_be_reg, 0x0);
        isp_wdr_normalize_lut_waddr_write(pre_be_reg, 0x0);

        for (j = 0; j < WDR_NOISE_LUT_NUM; j++) {
            isp_wdr_snoise_lut_wdata_write(pre_be_reg, dyna_reg_cfg->noise_profile[0][j]);
        }

        for (j = 0; j < WDR_NOISE_LUT_NUM; j++) {
            isp_wdr_lnoise_lut_wdata_write(pre_be_reg, dyna_reg_cfg->noise_profile[1][j]);
        }

        for (j = 0; j < WDR_SIGMA_LUT_NUM; j++) {
            isp_wdr_normalize_lut_wdata_write(pre_be_reg, dyna_reg_cfg->sigma_nml_lut[j]);
        }
    } else {
        isp_wdr_snoise_profile_write(&pre_be_reg->pre_be_lut.pre_be_apb_lut, dyna_reg_cfg->noise_profile[0]);
        isp_wdr_lnoise_profile_write(&pre_be_reg->pre_be_lut.pre_be_apb_lut, dyna_reg_cfg->noise_profile[1]);
        isp_wdr_sigma_nm_lut_write(&pre_be_reg->pre_be_lut.pre_be_apb_lut, dyna_reg_cfg->sigma_nml_lut);
    }

    isp_wdr_snoise_profile_update_write(pre_be_reg, 0x1);
    isp_wdr_lnoise_profile_update_write(pre_be_reg, 0x1);
    isp_wdr_normalize_update_write(pre_be_reg, 0x1);
}

static td_void isp_wdr_static_reg_config_first_frame(isp_pre_be_reg_type *be_reg, isp_fswdr_static_cfg *static_reg_cfg,
    isp_fswdr_dyna_cfg *dyna_reg_cfg, isp_fswdr_usr_cfg *usr_reg_cfg, const isp_be_blc_dyna_cfg *dyna_blc)
{
    td_u32 expo_value[4]; /* 4 means the number of  max frame for wdr mode */
    const td_u8 bit_shift = 0;
    td_u16 offset0 = dyna_blc->wdr_blc[0].blc[1]; /* wdr chn0 */
    td_u16 offset1 = dyna_blc->wdr_blc[1].blc[1]; /* wdr chn1 */
    td_u16 offset2 = dyna_blc->wdr_blc[2].blc[1]; /* wdr chn2 */

    expo_value[0] = static_reg_cfg->expo_value_lut[0];                      /* array index 0  assignment */
    expo_value[1] = static_reg_cfg->expo_value_lut[1];                      /* array index 1  assignment */
    expo_value[2] = static_reg_cfg->expo_value_lut[2];                      /* array index 2  assignment */
    expo_value[3] = static_reg_cfg->expo_value_lut[3];                      /* array index 3  assignment */
    isp_wdr_expovalue0_write(be_reg, expo_value[0]);                        /* expovalue 0 */
    isp_wdr_expovalue1_write(be_reg, expo_value[1]);                        /* expovalue 1 */
    isp_wdr_expovalue2_write(be_reg, expo_value[2]);                        /* expovalue 2 */
    isp_wdr_expovalue3_write(be_reg, expo_value[3]);                        /* expovalue 3 */
    isp_wdr_fusion_expovalue0_write(be_reg, expo_value[0]);                 /* expovalue 0  */
    isp_wdr_fusion_expovalue1_write(be_reg, expo_value[1]);                 /* expovalue 1  */
    isp_wdr_fusion_expovalue2_write(be_reg, expo_value[2]);                 /* expovalue 2  */
    isp_wdr_fusion_expovalue3_write(be_reg, expo_value[3]);                 /* expovalue 3  */
    isp_wdr_exporratio0_write(be_reg, static_reg_cfg->expo_r_ratio_lut[0]); /* expo_r_ratio 0 */
    isp_wdr_exporratio1_write(be_reg, static_reg_cfg->expo_r_ratio_lut[1]); /* expo_r_ratio 1 */
    isp_wdr_exporratio2_write(be_reg, static_reg_cfg->expo_r_ratio_lut[2]); /* expo_r_ratio 2 */
    isp_flick_exporatio0_write(be_reg, static_reg_cfg->flick_exp_ratio[0]); /* expovalue 0 */
    isp_flick_exporatio1_write(be_reg, static_reg_cfg->flick_exp_ratio[1]); /* expovalue 1 */
    isp_flick_exporatio2_write(be_reg, static_reg_cfg->flick_exp_ratio[2]); /* expovalue 2 */
    isp_wdr_mdt_en_write(be_reg, dyna_reg_cfg->wdr_mdt_en);
    isp_wdr_fusionmode_write(be_reg, usr_reg_cfg->fusion_mode);
    isp_wdr_maxratio_write(be_reg, static_reg_cfg->max_ratio);
    isp_wdr_fusion_maxratio_write(be_reg, static_reg_cfg->max_ratio);
    isp_wdr_short_thr0_write(be_reg, dyna_reg_cfg->short_thr[0]);
    isp_wdr_short_thr1_write(be_reg, dyna_reg_cfg->short_thr[1]);
    isp_wdr_short_thr2_write(be_reg, dyna_reg_cfg->short_thr[2]); /* short_thr 2 */
    isp_wdr_long_thr0_write(be_reg, dyna_reg_cfg->long_thr[0]);
    isp_wdr_long_thr1_write(be_reg, dyna_reg_cfg->long_thr[1]);
    isp_wdr_long_thr2_write(be_reg, dyna_reg_cfg->long_thr[2]); /* long_thr 2 */

    static_reg_cfg->blc_comp_lut[0] = ((expo_value[0] - expo_value[1]) * offset0) >> bit_shift; /* 0,1 */
    static_reg_cfg->blc_comp_lut[1] = ((expo_value[1] - expo_value[2]) * offset1) >> bit_shift; /* 2,1 */
    static_reg_cfg->blc_comp_lut[2] = ((expo_value[2] - expo_value[3]) * offset2) >> bit_shift; /* 2,3 */

    isp_wdr_blc_comp0_write(be_reg, static_reg_cfg->blc_comp_lut[0]); /* array index 0 */
    isp_wdr_blc_comp1_write(be_reg, static_reg_cfg->blc_comp_lut[1]); /* array index 1 */
    isp_wdr_blc_comp2_write(be_reg, static_reg_cfg->blc_comp_lut[2]); /* array index 2 */

    static_reg_cfg->first_frame = TD_FALSE;
}

static td_void isp_wdr_static_reg_config(isp_pre_be_reg_type *be_reg, isp_fswdr_static_cfg *static_reg_cfg)
{
    isp_wdr_grayscale_mode_write(be_reg, static_reg_cfg->gray_scale_mode);
    isp_wdr_bsaveblc_write(be_reg, static_reg_cfg->save_blc);
    isp_wdr_mask_similar_thr_write(be_reg, static_reg_cfg->mask_similar_thr);
    isp_wdr_mask_similar_cnt_write(be_reg, static_reg_cfg->mask_similar_cnt);
    isp_wdr_dftwgt_fl_write(be_reg, static_reg_cfg->dft_wgt_fl);
    isp_wdr_bldrlhfidx_write(be_reg, static_reg_cfg->bldr_lhf_idx);
    isp_wdr_saturate_thr_write(be_reg, static_reg_cfg->saturate_thr);
    isp_wdr_fusion_saturate_thd_write(be_reg, static_reg_cfg->fusion_saturate_thr);
    isp_wdr_forcelong_smooth_en_write(be_reg, static_reg_cfg->force_long_smooth_en);
    isp_wdr_seg_idx_base0_write(be_reg, static_reg_cfg->seg_idx_base[0]);
    isp_wdr_seg_idx_base1_write(be_reg, static_reg_cfg->seg_idx_base[1]);
    isp_wdr_seg_idx_base2_write(be_reg, static_reg_cfg->seg_idx_base[2]); /* 2 */
    isp_wdr_seg_idx_base3_write(be_reg, static_reg_cfg->seg_idx_base[3]); /* 3 */
    isp_wdr_seg_idx_base4_write(be_reg, static_reg_cfg->seg_idx_base[4]); /* 4 */
    isp_wdr_seg_idx_base5_write(be_reg, static_reg_cfg->seg_idx_base[5]); /* 5 */
    isp_wdr_seg_idx_base6_write(be_reg, static_reg_cfg->seg_idx_base[6]); /* 6 */
    isp_wdr_seg_idx_base7_write(be_reg, static_reg_cfg->seg_idx_base[7]); /* 7 */

    isp_wdr_seg_maxval0_write(be_reg, static_reg_cfg->seg_max_val[0]);
    isp_wdr_seg_maxval1_write(be_reg, static_reg_cfg->seg_max_val[1]);
    isp_wdr_seg_maxval2_write(be_reg, static_reg_cfg->seg_max_val[2]); /* 2 */
    isp_wdr_seg_maxval3_write(be_reg, static_reg_cfg->seg_max_val[3]); /* 3 */
    isp_wdr_seg_maxval4_write(be_reg, static_reg_cfg->seg_max_val[4]); /* 4 */
    isp_wdr_seg_maxval5_write(be_reg, static_reg_cfg->seg_max_val[5]); /* 5 */
    isp_wdr_seg_maxval6_write(be_reg, static_reg_cfg->seg_max_val[6]); /* 6 */
    isp_wdr_seg_maxval7_write(be_reg, static_reg_cfg->seg_max_val[7]); /* 7 */

    isp_wdr_filter_coefh0_write(be_reg, static_reg_cfg->filter_coef_h[0]);
    isp_wdr_filter_coefh1_write(be_reg, static_reg_cfg->filter_coef_h[1]);
    isp_wdr_filter_coefh2_write(be_reg, static_reg_cfg->filter_coef_h[2]); /* 2 */
    isp_wdr_filter_coefh3_write(be_reg, static_reg_cfg->filter_coef_h[3]); /* 3 */
    isp_wdr_filter_coefh4_write(be_reg, static_reg_cfg->filter_coef_h[4]); /* 4 */

    isp_wdr_filter_coefv0_write(be_reg, static_reg_cfg->filter_coef_v[0]);
    isp_wdr_filter_coefv1_write(be_reg, static_reg_cfg->filter_coef_v[1]);
    isp_wdr_filter_coefv2_write(be_reg, static_reg_cfg->filter_coef_v[2]); /* 2 */
    isp_wdr_filter_coefv3_write(be_reg, static_reg_cfg->filter_coef_v[3]); /* 3 */
    isp_wdr_filter_coefv4_write(be_reg, static_reg_cfg->filter_coef_v[4]); /* 4 */
    static_reg_cfg->resh = TD_FALSE;
}

static td_void isp_wdr_dyna_reg_config_first(isp_pre_be_reg_type *be_reg, isp_fswdr_dyna_cfg *dyna_reg_cfg)
{
    isp_wdr_bginfo_en_write(be_reg, dyna_reg_cfg->bg_info_en);
    isp_wdr_avg_mode_select_write(be_reg, dyna_reg_cfg->avg_mode_select);
    isp_wdr_sigma_mode_select_write(be_reg, dyna_reg_cfg->sigma_mode_select);
    isp_wdr_revmode_write(be_reg, dyna_reg_cfg->wdr_revmode);
    isp_wdr_merge_frame_write(be_reg, dyna_reg_cfg->frm_merge);

    isp_wdr_sqrt_again_g_write(be_reg, dyna_reg_cfg->sqrt_again_g);
    isp_wdr_sqrt_dgain_g_write(be_reg, dyna_reg_cfg->sqrt_dgain_g);
    isp_wdr_mdt_nosfloor_write(be_reg, dyna_reg_cfg->mdt_nos_floor);

    isp_wdr_mdthr_low_gain0_write(be_reg, dyna_reg_cfg->md_thr_low_gain[0]);
    isp_wdr_mdthr_hig_gain0_write(be_reg, dyna_reg_cfg->md_thr_hig_gain[0]);
    isp_wdr_mdthr_low_gain1_write(be_reg, dyna_reg_cfg->md_thr_low_gain[1]);
    isp_wdr_mdthr_hig_gain1_write(be_reg, dyna_reg_cfg->md_thr_hig_gain[1]);
    isp_wdr_mdthr_low_gain2_write(be_reg, dyna_reg_cfg->md_thr_low_gain[2]); /* 2 */
    isp_wdr_mdthr_hig_gain2_write(be_reg, dyna_reg_cfg->md_thr_hig_gain[2]); /* 2 */

    isp_wdr_f0_still_thr_write(be_reg, dyna_reg_cfg->still_thr_lut[0]);
    isp_wdr_f1_still_thr_write(be_reg, dyna_reg_cfg->still_thr_lut[1]);
    isp_wdr_f2_still_thr_write(be_reg, dyna_reg_cfg->still_thr_lut[2]); /* 2 */

    isp_wdr_erosion_en_write(be_reg, dyna_reg_cfg->erosion_en);
    isp_wdr_fusion_f0_thr_r_write(be_reg, dyna_reg_cfg->fusion_thr_r_lut[0]);
    isp_wdr_fusion_f1_thr_r_write(be_reg, dyna_reg_cfg->fusion_thr_r_lut[1]);
    isp_wdr_fusion_f2_thr_r_write(be_reg, dyna_reg_cfg->fusion_thr_r_lut[2]); /* 2 */
    isp_wdr_fusion_f3_thr_r_write(be_reg, dyna_reg_cfg->fusion_thr_r_lut[3]); /* 3 */

    isp_wdr_fusion_f0_thr_g_write(be_reg, dyna_reg_cfg->fusion_thr_g_lut[0]);
    isp_wdr_fusion_f1_thr_g_write(be_reg, dyna_reg_cfg->fusion_thr_g_lut[1]);
    isp_wdr_fusion_f2_thr_g_write(be_reg, dyna_reg_cfg->fusion_thr_g_lut[2]); /* 2 */
    isp_wdr_fusion_f3_thr_g_write(be_reg, dyna_reg_cfg->fusion_thr_g_lut[3]); /* 3 */

    isp_wdr_fusion_f0_thr_b_write(be_reg, dyna_reg_cfg->fusion_thr_b_lut[0]);
    isp_wdr_fusion_f1_thr_b_write(be_reg, dyna_reg_cfg->fusion_thr_b_lut[1]);
    isp_wdr_fusion_f2_thr_b_write(be_reg, dyna_reg_cfg->fusion_thr_b_lut[2]); /* 2 */
    isp_wdr_fusion_f3_thr_b_write(be_reg, dyna_reg_cfg->fusion_thr_b_lut[3]); /* 3 */
}

static td_void isp_wdr_dyna_reg_config_second(isp_pre_be_reg_type *be_reg, isp_fswdr_dyna_cfg *dyna_reg_cfg,
    isp_post_be_reg_type *post_be_reg)
{
    isp_bcom_alpha_write(be_reg, dyna_reg_cfg->bcom_alpha);
    isp_bdec_alpha_write(post_be_reg, dyna_reg_cfg->bdec_alpha);
    isp_wdr_forcelong_en_write(be_reg, dyna_reg_cfg->force_long);
    isp_wdr_forcelong_low_thd_write(be_reg, dyna_reg_cfg->force_long_low_thr);
    isp_wdr_forcelong_high_thd_write(be_reg, dyna_reg_cfg->force_long_hig_thr);

    isp_wdr_shortchk_thd_write(be_reg, dyna_reg_cfg->short_check_thd);
    isp_wdr_fusion_data_mode_write(be_reg, dyna_reg_cfg->fusion_data_mode);
    isp_wdr_ref_stat_hblk_num_write(be_reg, dyna_reg_cfg->wdr_ref_stat_hblk_num);
    isp_wdr_ref_stat_vblk_num_write(be_reg, dyna_reg_cfg->wdr_reg_stat_vblk_num);
    isp_wdr_cur_clac_hblk_num_write(be_reg, dyna_reg_cfg->wdr_cur_calc_hblk_num);
    isp_wdr_cur_clac_vblk_num_write(be_reg, dyna_reg_cfg->wdr_cur_calc_vblk_num);

    isp_wdr_bginfo_low_thd0_write(be_reg, dyna_reg_cfg->wdr_bginfo_low_thd0);
    isp_wdr_bginfo_hig_thd0_write(be_reg, dyna_reg_cfg->wdr_bginfo_high_thd0);
    isp_wdr_bginfo_low_thd1_write(be_reg, dyna_reg_cfg->wdr_bginfo_low_thd1);
    isp_wdr_bginfo_hig_thd1_write(be_reg, dyna_reg_cfg->wdr_bginfo_high_thd1);

    isp_wdr_stat_shift_write(be_reg, dyna_reg_cfg->wdr_stat_shift);
    isp_wdr_blk_idx_write(be_reg, dyna_reg_cfg->wdr_blk_idx);
    isp_wdr_split_num_write(be_reg, dyna_reg_cfg->wdr_split_num);
    isp_wdr_kx_write(be_reg, dyna_reg_cfg->wdr_kx);
    isp_wdr_ky_write(be_reg, dyna_reg_cfg->wdr_ky);
    isp_wdr_smlmap_offset_write(be_reg, dyna_reg_cfg->wdr_smlmap_offset);

    isp_wdr_lpf_cef0_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[0]);
    isp_wdr_lpf_cef1_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[1]);
    isp_wdr_lpf_cef2_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[2]); /* 2 */
    isp_wdr_lpf_cef3_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[3]); /* 3 */
    isp_wdr_lpf_cef4_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[4]); /* 4 */
    isp_wdr_lpf_cef5_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[5]); /* 5 */
    isp_wdr_lpf_cef6_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[6]); /* 6 */
    isp_wdr_lpf_cef7_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[7]); /* 7 */
    isp_wdr_lpf_cef8_write(be_reg, dyna_reg_cfg->wdr_lpf_coef[8]); /* 8 */

    isp_wdr_lpf_stf_write(be_reg, dyna_reg_cfg->wdr_lpf_shift);
    isp_wdr_stat_startx_write(be_reg, dyna_reg_cfg->wdr_stat_startx);
    isp_wdr_stat_endx_write(be_reg, dyna_reg_cfg->wdr_stat_endx);
}

static td_void isp_wdr_user_reg_config(isp_pre_be_reg_type *be_reg, isp_fswdr_usr_cfg *usr_reg_cfg,
    td_bool offline_mode)
{
    td_bool usr_resh;
    td_bool idx_resh;

    idx_resh = (isp_wdr_update_index_read(be_reg) != usr_reg_cfg->update_index);
    usr_resh = (offline_mode) ? (usr_reg_cfg->resh && idx_resh) : (usr_reg_cfg->resh);
    if (usr_resh) {
        isp_wdr_update_index_write(be_reg, usr_reg_cfg->update_index);
        isp_wdr_shortexpo_chk_write(be_reg, usr_reg_cfg->short_expo_chk);
        isp_wdr_mdtlbld_write(be_reg, usr_reg_cfg->mdt_l_bld);
        isp_wdr_mdt_full_thr_write(be_reg, usr_reg_cfg->mdt_full_thr);
        isp_wdr_mdt_still_thr_write(be_reg, usr_reg_cfg->mdt_still_thr);
        isp_wdr_pixel_avg_max_diff_write(be_reg, usr_reg_cfg->pixel_avg_max_diff);
        usr_reg_cfg->resh = offline_mode;
    }
}

static td_void isp_wdr_module_en_write(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(pre_viproc);
    isp_check_pointer_void_return(post_viproc);

    isp_wdr_en_write(pre_viproc, reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.wdr_en);
    isp_bcom_en_write(pre_viproc, reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.dyna_reg_cfg.bcom_en);
    isp_bdec_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.dyna_reg_cfg.bdec_en);
}

static td_s32 isp_wdr_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_fswdr_static_cfg *static_reg_cfg = TD_NULL;
    isp_fswdr_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_fswdr_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));
    if (reg_cfg_info->cfg_key.bit1_fs_wdr_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_be_reg);
        isp_wdr_module_en_write(vi_pipe, reg_cfg_info, i);

        isp_bnr_wdr_enable_write(post_be_reg, reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.wdr_en);
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.static_reg_cfg;
        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.usr_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].wdr_reg_cfg.dyna_reg_cfg;

        if (static_reg_cfg->first_frame == TD_TRUE) {
            isp_wdr_static_reg_config_first_frame(be_reg, static_reg_cfg, dyna_reg_cfg, usr_reg_cfg,
                &reg_cfg_info->alg_reg_cfg[i].be_blc_cfg.dyna_blc);
        }
        if (static_reg_cfg->resh) {
            isp_wdr_static_reg_config(be_reg, static_reg_cfg);
        }

        isp_wdr_user_reg_config(be_reg, usr_reg_cfg, offline_mode);

        if (dyna_reg_cfg->resh) {
            isp_wdr_dyna_reg_config_first(be_reg, dyna_reg_cfg);
            isp_wdr_dyna_reg_config_second(be_reg, dyna_reg_cfg, post_be_reg);
            isp_wdr_lut_def_reg_config(vi_pipe, be_reg, dyna_reg_cfg);
            dyna_reg_cfg->resh = offline_mode;
        }
        if (offline_mode == TD_TRUE) {
            isp_wdr_stat_read_enable_write(be_reg, 1);
        } else {
            isp_wdr_stat_read_enable_write(be_reg, 0);
        }
        reg_cfg_info->cfg_key.bit1_fs_wdr_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
    return TD_SUCCESS;
}

static td_void isp_drc_cclut_reg_config(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_drc_usr_cfg *usr_reg_cfg)
{
    td_u16 j;

    if (offline_mode == TD_FALSE) {
        isp_adrc_cclut_waddr_write(be_reg, 0);

        for (j = 0; j < OT_ISP_DRC_CC_NODE_NUM; j++) {
            isp_adrc_cclut_wdata_write(be_reg, usr_reg_cfg->cc_lut[j]);
        }
    } else {
        isp_adrc_cclut_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->cc_lut);
    }
    isp_adrc_cclut_update_write(be_reg, 1);
}

static td_void isp_drc_tmlut_reg_config(td_bool *tm_lut_update, const isp_usr_ctx *isp_ctx,
    isp_post_be_reg_type *be_reg, isp_drc_dyna_cfg *dyna_reg_cfg)
{
    td_u16 j;

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        if (dyna_reg_cfg->lut_update) {
            isp_adrc_tmlut0_waddr_write(be_reg, 0);
            for (j = 0; j < OT_ISP_DRC_TM_NODE_NUM; j++) {
                isp_adrc_tmlut0_wdata_write(be_reg, ((dyna_reg_cfg->tone_mapping_value[j]) << 14) | /* 14 bit */
                    (dyna_reg_cfg->tone_mapping_diff[j]));
            }
            *tm_lut_update = TD_TRUE;
        }
    } else {
        isp_adrc_tmlut0_value_write(&be_reg->post_be_lut.post_be_apb_lut, dyna_reg_cfg->tone_mapping_value);
        isp_adrc_tmlut0_diff_write(&be_reg->post_be_lut.post_be_apb_lut, dyna_reg_cfg->tone_mapping_diff);
    }
}

static td_void isp_drc_mlut_online_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    td_u16 j;
    if (usr_reg_cfg->m_lut0_update) {
        isp_adrc_mlut0_waddr_write(be_reg, 0);
        for (j = 0; j < OT_ISP_DRC_MLUT_NODE_NUM; j++) {
            isp_adrc_mlut0_wdata_write(be_reg,
                ((usr_reg_cfg->m_lut0_diff[j]) << 16) | (usr_reg_cfg->m_lut0_value[j])); /* diff at high 16 bits */
        }
        isp_adrc_mlut0_update_write(be_reg, 1);
        usr_reg_cfg->m_lut0_update = TD_FALSE;
    }

    if (usr_reg_cfg->m_lut1_update) {
        isp_adrc_mlut1_waddr_write(be_reg, 0);
        for (j = 0; j < OT_ISP_DRC_MLUT_NODE_NUM; j++) {
            isp_adrc_mlut1_wdata_write(be_reg,
                ((usr_reg_cfg->m_lut1_diff[j]) << 16) | (usr_reg_cfg->m_lut1_value[j])); /* diff at high 16 bits */
        }
        isp_adrc_mlut1_update_write(be_reg, 1);
        usr_reg_cfg->m_lut1_update = TD_FALSE;
    }

    if (usr_reg_cfg->m_lut2_update) {
        isp_adrc_mlut2_waddr_write(be_reg, 0);
        for (j = 0; j < OT_ISP_DRC_MLUT_NODE_NUM; j++) {
            isp_adrc_mlut2_wdata_write(be_reg,
                ((usr_reg_cfg->m_lut2_diff[j]) << 16) | (usr_reg_cfg->m_lut2_value[j])); /* diff at high 16 bits */
        }
        isp_adrc_mlut2_update_write(be_reg, 1);
        usr_reg_cfg->m_lut2_update = TD_FALSE;
    }

    if (usr_reg_cfg->m_lut3_update) {
        isp_adrc_mlut3_waddr_write(be_reg, 0);
        for (j = 0; j < OT_ISP_DRC_MLUT_NODE_NUM; j++) {
            isp_adrc_mlut3_wdata_write(be_reg,
                ((usr_reg_cfg->m_lut3_diff[j]) << 16) | (usr_reg_cfg->m_lut3_value[j])); /* diff at high 16 bits */
        }
        isp_adrc_mlut3_update_write(be_reg, 1);
        usr_reg_cfg->m_lut3_update = TD_FALSE;
    }
}

static td_void isp_drc_mlut_reg_config(td_bool offline_mode, isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    if (offline_mode == TD_FALSE) {
        isp_drc_mlut_online_reg_config(be_reg, usr_reg_cfg);
    } else {
        isp_adrc_mlut0_value_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut0_value);
        isp_adrc_mlut0_diff_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut0_diff);
        isp_adrc_mlut0_update_write(be_reg, 1);

        isp_adrc_mlut1_value_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut1_value);
        isp_adrc_mlut1_diff_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut1_diff);
        isp_adrc_mlut1_update_write(be_reg, 1);

        isp_adrc_mlut2_value_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut2_value);
        isp_adrc_mlut2_diff_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut2_diff);
        isp_adrc_mlut2_update_write(be_reg, 1);

        isp_adrc_mlut3_value_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut3_value);
        isp_adrc_mlut3_diff_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->m_lut3_diff);
        isp_adrc_mlut3_update_write(be_reg, 1);
    }
}

static td_void isp_drc_mixlut_reg_config(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_drc_usr_cfg *usr_reg_cfg)
{
    td_u16 j;

    if (offline_mode == TD_FALSE) {
        isp_adrc_localmix_waddr_write(be_reg, 0);
        for (j = 0; j < OT_ISP_DRC_LMIX_NODE_NUM; j++) {
            isp_adrc_localmix_wdata_write(be_reg, (((td_u32)usr_reg_cfg->mixing_bright1[j] << 24) | /* from 24 bit */
                ((td_u32)usr_reg_cfg->mixing_dark1[j] << 16) |                                      /* from 16 bit */
                ((td_u32)usr_reg_cfg->mixing_bright0[j] << 8) |                                     /* from 8 bit */
                (td_u32)usr_reg_cfg->mixing_dark0[j]));
        }
        isp_adrc_mixlut_update_write(be_reg, 1);
    } else {
        isp_adrc_localmix_dark0_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->mixing_dark0);
        isp_adrc_localmix_bright0_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->mixing_bright0);
        isp_adrc_localmix_dark1_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->mixing_dark1);
        isp_adrc_localmix_bright1_write(&be_reg->post_be_lut.post_be_apb_lut, usr_reg_cfg->mixing_bright1);
        isp_adrc_mixlut_update_write(be_reg, 1);
    }
}

static td_void isp_drc_static_comm_reg_config(isp_post_be_reg_type *be_reg, isp_drc_static_cfg *static_reg_cfg)
{
    isp_adrc_wrstat_en_write(be_reg, static_reg_cfg->wrstat_en);
    isp_adrc_rdstat_en_write(be_reg, static_reg_cfg->rdstat_en);

    isp_adrc_outbits_write(be_reg, static_reg_cfg->bit_out);
    isp_adrc_r_wgt_write(be_reg, static_reg_cfg->r_wgt);
    isp_adrc_g_wgt_write(be_reg, static_reg_cfg->g_wgt);
    isp_adrc_b_wgt_write(be_reg, static_reg_cfg->b_wgt);

    isp_adrc_wgt_r_tm_write(be_reg, static_reg_cfg->r_wgt_tm);
    isp_adrc_wgt_g_tm_write(be_reg, static_reg_cfg->g_wgt_tm);
    isp_adrc_wgt_b_tm_write(be_reg, static_reg_cfg->b_wgt_tm);

    isp_adrc_mono_chroma_en_write(be_reg, static_reg_cfg->monochrome_mode);

    isp_adrc_tmlutidx_sel_write(be_reg, static_reg_cfg->tm_idx_sel);
    isp_adrc_level0_kernel_sel_r0_write(be_reg, static_reg_cfg->kernel_sel_r0);
    isp_adrc_level0_kernel_sel_e0_write(be_reg, static_reg_cfg->kernel_sel_e0);
    isp_adrc_kernel_sel_ds_write(be_reg, static_reg_cfg->kernel_sel_ds);
    isp_adrc_wgt_box_tri_sel_write(be_reg, static_reg_cfg->wgt_box_tri_sel);

    isp_adrc_bin_write(be_reg, static_reg_cfg->bin_num_z);
    isp_adrc_gain_clip_mode_write(be_reg, static_reg_cfg->gain_clip_mode);

    isp_adrc_raw_eps_write(be_reg, static_reg_cfg->raw_eps);

    isp_adrc_cc_lut_ctrl0_write(be_reg, static_reg_cfg->cc_ctrl_a);
    isp_adrc_cc_lut_ctrl2_write(be_reg, static_reg_cfg->cc_ctrl_b);

    isp_adrc_wgtm_sft_write(be_reg, static_reg_cfg->wgt_m_sft);
    isp_adrc_wgtm_en_write(be_reg, static_reg_cfg->wgt_m_en);
    isp_adrc_wgtm_pd_slo_write(be_reg, static_reg_cfg->wgt_m_pd_slo);
    isp_adrc_wgtm_pd_sft_write(be_reg, static_reg_cfg->wgt_m_pd_sft);
    isp_adrc_wgtm_pd_thr_write(be_reg, static_reg_cfg->wgt_m_pd_thr);

    isp_adrc_pdbg_slo_write(be_reg, static_reg_cfg->pd_bg_slo);
    isp_adrc_pdbg_thr_write(be_reg, static_reg_cfg->pd_bg_thr);
    isp_adrc_pdrg_slo_write(be_reg, static_reg_cfg->pd_rg_slo);
    isp_adrc_pdrg_wid_write(be_reg, static_reg_cfg->pd_rg_wid);
    isp_adrc_pdrg_crt_write(be_reg, static_reg_cfg->pd_rg_crt);
    isp_adrc_pdwhighslo_write(be_reg, static_reg_cfg->pdw_high_slo);
    isp_adrc_pdwlowthr_write(be_reg, static_reg_cfg->pdw_low_thr);
    isp_adrc_pdwlowslo_write(be_reg, static_reg_cfg->pdw_low_slo);
}

static td_void isp_drc_seg_reg_config(isp_post_be_reg_type *be_reg, isp_drc_static_cfg *static_reg_cfg)
{
    isp_adrc_seg_idxbase0_write(be_reg, static_reg_cfg->seg_idx_base[0]); /* idxbase[0] */
    isp_adrc_seg_idxbase1_write(be_reg, static_reg_cfg->seg_idx_base[1]); /* idxbase[1] */
    isp_adrc_seg_idxbase2_write(be_reg, static_reg_cfg->seg_idx_base[2]); /* idxbase[2] */
    isp_adrc_seg_idxbase3_write(be_reg, static_reg_cfg->seg_idx_base[3]); /* idxbase[3] */
    isp_adrc_seg_idxbase4_write(be_reg, static_reg_cfg->seg_idx_base[4]); /* idxbase[4] */
    isp_adrc_seg_idxbase5_write(be_reg, static_reg_cfg->seg_idx_base[5]); /* idxbase[5] */
    isp_adrc_seg_idxbase6_write(be_reg, static_reg_cfg->seg_idx_base[6]); /* idxbase[6] */
    isp_adrc_seg_idxbase7_write(be_reg, static_reg_cfg->seg_idx_base[7]); /* idxbase[7] */

    isp_adrc_seg_maxval0_write(be_reg, static_reg_cfg->seg_max_val[0]); /* maxval[0] */
    isp_adrc_seg_maxval1_write(be_reg, static_reg_cfg->seg_max_val[1]); /* maxval[1] */
    isp_adrc_seg_maxval2_write(be_reg, static_reg_cfg->seg_max_val[2]); /* maxval[2] */
    isp_adrc_seg_maxval3_write(be_reg, static_reg_cfg->seg_max_val[3]); /* maxval[3] */
    isp_adrc_seg_maxval4_write(be_reg, static_reg_cfg->seg_max_val[4]); /* maxval[4] */
    isp_adrc_seg_maxval5_write(be_reg, static_reg_cfg->seg_max_val[5]); /* maxval[5] */
    isp_adrc_seg_maxval6_write(be_reg, static_reg_cfg->seg_max_val[6]); /* maxval[6] */
    isp_adrc_seg_maxval7_write(be_reg, static_reg_cfg->seg_max_val[7]); /* maxval[7] */
}

static td_void isp_drc_rg_thr_reg_config(isp_post_be_reg_type *be_reg, td_u8 *rg_thr_lut)
{
    isp_adrc_rg_thr00_write(be_reg, rg_thr_lut[0]);  /* rg_thr_lut[0] */
    isp_adrc_rg_thr01_write(be_reg, rg_thr_lut[1]);  /* rg_thr_lut[1] */
    isp_adrc_rg_thr02_write(be_reg, rg_thr_lut[2]);  /* rg_thr_lut[2] */
    isp_adrc_rg_thr03_write(be_reg, rg_thr_lut[3]);  /* rg_thr_lut[3] */
    isp_adrc_rg_thr04_write(be_reg, rg_thr_lut[4]);  /* rg_thr_lut[4] */
    isp_adrc_rg_thr05_write(be_reg, rg_thr_lut[5]);  /* rg_thr_lut[5] */
    isp_adrc_rg_thr06_write(be_reg, rg_thr_lut[6]);  /* rg_thr_lut[6] */
    isp_adrc_rg_thr07_write(be_reg, rg_thr_lut[7]);  /* rg_thr_lut[7] */
    isp_adrc_rg_thr08_write(be_reg, rg_thr_lut[8]);  /* rg_thr_lut[8] */
    isp_adrc_rg_thr09_write(be_reg, rg_thr_lut[9]);  /* rg_thr_lut[9] */
    isp_adrc_rg_thr10_write(be_reg, rg_thr_lut[10]); /* rg_thr_lut[10] */
    isp_adrc_rg_thr11_write(be_reg, rg_thr_lut[11]); /* rg_thr_lut[11] */
}

static td_void isp_drc_bg_thr_reg_config(isp_post_be_reg_type *be_reg, td_u8 *bg_thr_lut)
{
    isp_adrc_bg_thr00_write(be_reg, bg_thr_lut[0]);  /* bg_thr_lut[0] */
    isp_adrc_bg_thr01_write(be_reg, bg_thr_lut[1]);  /* bg_thr_lut[1] */
    isp_adrc_bg_thr02_write(be_reg, bg_thr_lut[2]);  /* bg_thr_lut[2] */
    isp_adrc_bg_thr03_write(be_reg, bg_thr_lut[3]);  /* bg_thr_lut[3] */
    isp_adrc_bg_thr04_write(be_reg, bg_thr_lut[4]);  /* bg_thr_lut[4] */
    isp_adrc_bg_thr05_write(be_reg, bg_thr_lut[5]);  /* bg_thr_lut[5] */
    isp_adrc_bg_thr06_write(be_reg, bg_thr_lut[6]);  /* bg_thr_lut[6] */
    isp_adrc_bg_thr07_write(be_reg, bg_thr_lut[7]);  /* bg_thr_lut[7] */
    isp_adrc_bg_thr08_write(be_reg, bg_thr_lut[8]);  /* bg_thr_lut[8] */
    isp_adrc_bg_thr09_write(be_reg, bg_thr_lut[9]);  /* bg_thr_lut[9] */
    isp_adrc_bg_thr10_write(be_reg, bg_thr_lut[10]); /* bg_thr_lut[10] */
    isp_adrc_bg_thr11_write(be_reg, bg_thr_lut[11]); /* bg_thr_lut[11] */
}

static td_void isp_drc_rg_ctr_reg_config(isp_post_be_reg_type *be_reg, td_u8 *rg_ctr_lut)
{
    isp_adrc_rg_ctr00_write(be_reg, rg_ctr_lut[0]);  /* rg_ctr_lut[0] */
    isp_adrc_rg_ctr01_write(be_reg, rg_ctr_lut[1]);  /* rg_ctr_lut[1] */
    isp_adrc_rg_ctr02_write(be_reg, rg_ctr_lut[2]);  /* rg_ctr_lut[2] */
    isp_adrc_rg_ctr03_write(be_reg, rg_ctr_lut[3]);  /* rg_ctr_lut[3] */
    isp_adrc_rg_ctr04_write(be_reg, rg_ctr_lut[4]);  /* rg_ctr_lut[4] */
    isp_adrc_rg_ctr05_write(be_reg, rg_ctr_lut[5]);  /* rg_ctr_lut[5] */
    isp_adrc_rg_ctr06_write(be_reg, rg_ctr_lut[6]);  /* rg_ctr_lut[6] */
    isp_adrc_rg_ctr07_write(be_reg, rg_ctr_lut[7]);  /* rg_ctr_lut[7] */
    isp_adrc_rg_ctr08_write(be_reg, rg_ctr_lut[8]);  /* rg_ctr_lut[8] */
    isp_adrc_rg_ctr09_write(be_reg, rg_ctr_lut[9]);  /* rg_ctr_lut[9] */
    isp_adrc_rg_ctr10_write(be_reg, rg_ctr_lut[10]); /* rg_ctr_lut[10] */
    isp_adrc_rg_ctr11_write(be_reg, rg_ctr_lut[11]); /* rg_ctr_lut[11] */
}

static td_void isp_drc_bg_ctr_reg_config(isp_post_be_reg_type *be_reg, td_u8 *bg_ctr_lut)
{
    isp_adrc_bg_ctr00_write(be_reg, bg_ctr_lut[0]);  /* bg_ctr_lut[0] */
    isp_adrc_bg_ctr01_write(be_reg, bg_ctr_lut[1]);  /* bg_ctr_lut[1] */
    isp_adrc_bg_ctr02_write(be_reg, bg_ctr_lut[2]);  /* bg_ctr_lut[2] */
    isp_adrc_bg_ctr03_write(be_reg, bg_ctr_lut[3]);  /* bg_ctr_lut[3] */
    isp_adrc_bg_ctr04_write(be_reg, bg_ctr_lut[4]);  /* bg_ctr_lut[4] */
    isp_adrc_bg_ctr05_write(be_reg, bg_ctr_lut[5]);  /* bg_ctr_lut[5] */
    isp_adrc_bg_ctr06_write(be_reg, bg_ctr_lut[6]);  /* bg_ctr_lut[6] */
    isp_adrc_bg_ctr07_write(be_reg, bg_ctr_lut[7]);  /* bg_ctr_lut[7] */
    isp_adrc_bg_ctr08_write(be_reg, bg_ctr_lut[8]);  /* bg_ctr_lut[8] */
    isp_adrc_bg_ctr09_write(be_reg, bg_ctr_lut[9]);  /* bg_ctr_lut[9] */
    isp_adrc_bg_ctr10_write(be_reg, bg_ctr_lut[10]); /* bg_ctr_lut[10] */
    isp_adrc_bg_ctr11_write(be_reg, bg_ctr_lut[11]); /* bg_ctr_lut[11] */
}

static td_void isp_drc_rg_bg_reg_config(isp_post_be_reg_type *be_reg, isp_drc_static_cfg *static_reg_cfg)
{
    isp_drc_rg_thr_reg_config(be_reg, static_reg_cfg->rg_thr_lut);
    isp_drc_bg_thr_reg_config(be_reg, static_reg_cfg->bg_thr_lut);
    isp_drc_rg_ctr_reg_config(be_reg, static_reg_cfg->rg_ctr_lut);
    isp_drc_bg_ctr_reg_config(be_reg, static_reg_cfg->bg_ctr_lut);
    isp_adrc_rgslo_write(be_reg, static_reg_cfg->rg_slo);
    isp_adrc_bgslo_write(be_reg, static_reg_cfg->bg_slo);
}

static td_void isp_drc_prev_luma_reg_config(isp_post_be_reg_type *be_reg, isp_drc_static_cfg *static_reg_cfg)
{
    if (static_reg_cfg->first_frame) {
        isp_adrc_shp_log_luma_write(be_reg, static_reg_cfg->shp_log);
        isp_adrc_shp_exp_luma_write(be_reg, static_reg_cfg->shp_exp);
        isp_adrc_div_denom_log_luma_write(be_reg, static_reg_cfg->div_denom_log);
        isp_adrc_denom_exp_luma_write(be_reg, static_reg_cfg->denom_exp);
        isp_adrc_prev_luma_0_write(be_reg, static_reg_cfg->prev_luma[0]); /* prev_luma[0] */
        isp_adrc_prev_luma_1_write(be_reg, static_reg_cfg->prev_luma[1]); /* prev_luma[1] */
        isp_adrc_prev_luma_2_write(be_reg, static_reg_cfg->prev_luma[2]); /* prev_luma[2] */
        isp_adrc_prev_luma_3_write(be_reg, static_reg_cfg->prev_luma[3]); /* prev_luma[3] */
        isp_adrc_prev_luma_4_write(be_reg, static_reg_cfg->prev_luma[4]); /* prev_luma[4] */
        isp_adrc_prev_luma_5_write(be_reg, static_reg_cfg->prev_luma[5]); /* prev_luma[5] */
        isp_adrc_prev_luma_6_write(be_reg, static_reg_cfg->prev_luma[6]); /* prev_luma[6] */
        isp_adrc_prev_luma_7_write(be_reg, static_reg_cfg->prev_luma[7]); /* prev_luma[7] */

        static_reg_cfg->first_frame = TD_FALSE;
    }
}

static td_void isp_drc_pw_curve_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    isp_adrc_pwcurveseg0_write(be_reg, usr_reg_cfg->pw_curve_seg[0]);   /* pw_curve_seg[0] */
    isp_adrc_pwcurveseg1_write(be_reg, usr_reg_cfg->pw_curve_seg[1]);   /* pw_curve_seg[1] */
    isp_adrc_pwcurveseg2_write(be_reg, usr_reg_cfg->pw_curve_seg[2]);   /* pw_curve_seg[2] */
    isp_adrc_pwcurveseg3_write(be_reg, usr_reg_cfg->pw_curve_seg[3]);   /* pw_curve_seg[3] */
    isp_adrc_pwcurveseg4_write(be_reg, usr_reg_cfg->pw_curve_seg[4]);   /* pw_curve_seg[4] */
    isp_adrc_pwcurveseg5_write(be_reg, usr_reg_cfg->pw_curve_seg[5]);   /* pw_curve_seg[5] */
    isp_adrc_pwcurveseg6_write(be_reg, usr_reg_cfg->pw_curve_seg[6]);   /* pw_curve_seg[6] */
    isp_adrc_pwcurveseg7_write(be_reg, usr_reg_cfg->pw_curve_seg[7]);   /* pw_curve_seg[7] */
    isp_adrc_pwcurveseg8_write(be_reg, usr_reg_cfg->pw_curve_seg[8]);   /* pw_curve_seg[8] */
    isp_adrc_pwcurveseg9_write(be_reg, usr_reg_cfg->pw_curve_seg[9]);   /* pw_curve_seg[9] */
    isp_adrc_pwcurveseg10_write(be_reg, usr_reg_cfg->pw_curve_seg[10]); /* pw_curve_seg[10] */
    isp_adrc_pwcurveseg11_write(be_reg, usr_reg_cfg->pw_curve_seg[11]); /* pw_curve_seg[11] */
    isp_adrc_pwcurveseg12_write(be_reg, usr_reg_cfg->pw_curve_seg[12]); /* pw_curve_seg[12] */
    isp_adrc_pwcurveseg13_write(be_reg, usr_reg_cfg->pw_curve_seg[13]); /* pw_curve_seg[13] */
    isp_adrc_pwcurveseg14_write(be_reg, usr_reg_cfg->pw_curve_seg[14]); /* pw_curve_seg[14] */
    isp_adrc_pwcurveseg15_write(be_reg, usr_reg_cfg->pw_curve_seg[15]); /* pw_curve_seg[15] */

    isp_adrc_pwcurveslo0_write(be_reg, usr_reg_cfg->pw_curve_slo[0]);   /* pw_curve_slo[0] */
    isp_adrc_pwcurveslo1_write(be_reg, usr_reg_cfg->pw_curve_slo[1]);   /* pw_curve_slo[1] */
    isp_adrc_pwcurveslo2_write(be_reg, usr_reg_cfg->pw_curve_slo[2]);   /* pw_curve_slo[2] */
    isp_adrc_pwcurveslo3_write(be_reg, usr_reg_cfg->pw_curve_slo[3]);   /* pw_curve_slo[3] */
    isp_adrc_pwcurveslo4_write(be_reg, usr_reg_cfg->pw_curve_slo[4]);   /* pw_curve_slo[4] */
    isp_adrc_pwcurveslo5_write(be_reg, usr_reg_cfg->pw_curve_slo[5]);   /* pw_curve_slo[5] */
    isp_adrc_pwcurveslo6_write(be_reg, usr_reg_cfg->pw_curve_slo[6]);   /* pw_curve_slo[6] */
    isp_adrc_pwcurveslo7_write(be_reg, usr_reg_cfg->pw_curve_slo[7]);   /* pw_curve_slo[7] */
    isp_adrc_pwcurveslo8_write(be_reg, usr_reg_cfg->pw_curve_slo[8]);   /* pw_curve_slo[8] */
    isp_adrc_pwcurveslo9_write(be_reg, usr_reg_cfg->pw_curve_slo[9]);   /* pw_curve_slo[9] */
    isp_adrc_pwcurveslo10_write(be_reg, usr_reg_cfg->pw_curve_slo[10]); /* pw_curve_slo[10] */
    isp_adrc_pwcurveslo11_write(be_reg, usr_reg_cfg->pw_curve_slo[11]); /* pw_curve_slo[11] */
    isp_adrc_pwcurveslo12_write(be_reg, usr_reg_cfg->pw_curve_slo[12]); /* pw_curve_slo[12] */
    isp_adrc_pwcurveslo13_write(be_reg, usr_reg_cfg->pw_curve_slo[13]); /* pw_curve_slo[13] */
    isp_adrc_pwcurveslo14_write(be_reg, usr_reg_cfg->pw_curve_slo[14]); /* pw_curve_slo[14] */
    isp_adrc_pwcurveslo15_write(be_reg, usr_reg_cfg->pw_curve_slo[15]); /* pw_curve_slo[15] */

    isp_adrc_pwcurvebias_write(be_reg, usr_reg_cfg->pw_curve_bias);
    isp_adrc_curveslosft_write(be_reg, usr_reg_cfg->pw_curve_slo_sft);
}

static td_void isp_drc_usr_comm_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    isp_adrc_sft1_y_write(be_reg, usr_reg_cfg->ysft1);
    isp_adrc_val1_y_write(be_reg, usr_reg_cfg->yval1);
    isp_adrc_sft2_y_write(be_reg, usr_reg_cfg->ysft2);
    isp_adrc_val2_y_write(be_reg, usr_reg_cfg->yval2);

    isp_adrc_sft1_c_write(be_reg, usr_reg_cfg->csft1);
    isp_adrc_val1_c_write(be_reg, usr_reg_cfg->cval1);
    isp_adrc_sft2_c_write(be_reg, usr_reg_cfg->csft2);
    isp_adrc_val2_c_write(be_reg, usr_reg_cfg->cval2);

    isp_adrc_sft_write(be_reg, usr_reg_cfg->sft);
    isp_adrc_val_write(be_reg, usr_reg_cfg->val);

    isp_adrc_gain_clip_knee_write(be_reg, usr_reg_cfg->gain_clip_knee);
    isp_adrc_gain_clip_step_write(be_reg, usr_reg_cfg->gain_clip_step);

    isp_adrc_shp_log_raw_write(be_reg, usr_reg_cfg->shp_log_raw);
    isp_adrc_shp_exp_raw_write(be_reg, usr_reg_cfg->shp_exp_raw);
    isp_adrc_div_denom_log_raw_write(be_reg, usr_reg_cfg->div_denom_log_raw);
    isp_adrc_denom_exp_raw_write(be_reg, usr_reg_cfg->denom_exp_raw);

    isp_adrc_m0_en_write(be_reg, usr_reg_cfg->m_en[0]); /* m_en[0] */
    isp_adrc_m1_en_write(be_reg, usr_reg_cfg->m_en[1]); /* m_en[1] */
    isp_adrc_m2_en_write(be_reg, usr_reg_cfg->m_en[2]); /* m_en[2] */
    isp_adrc_m3_en_write(be_reg, usr_reg_cfg->m_en[3]); /* m_en[3] */

    isp_adrc_logalphadec0_write(be_reg, usr_reg_cfg->log_alpha_dec[0]); /* log_alpha_dec[0] */
    isp_adrc_logalphadec1_write(be_reg, usr_reg_cfg->log_alpha_dec[1]); /* log_alpha_dec[1] */
    isp_adrc_logalphadec2_write(be_reg, usr_reg_cfg->log_alpha_dec[2]); /* log_alpha_dec[2] */
    isp_adrc_logalphadec3_write(be_reg, usr_reg_cfg->log_alpha_dec[3]); /* log_alpha_dec[3] */

    isp_adrc_outbitw0_write(be_reg, usr_reg_cfg->write_out_bit[0]); /* write_out_bit[0] */
    isp_adrc_outbitw1_write(be_reg, usr_reg_cfg->write_out_bit[1]); /* write_out_bit[1] */
    isp_adrc_outbitw2_write(be_reg, usr_reg_cfg->write_out_bit[2]); /* write_out_bit[2] */
    isp_adrc_outbitw3_write(be_reg, usr_reg_cfg->write_out_bit[3]); /* write_out_bit[3] */

    isp_adrc_lumaclip_stepl0_write(be_reg, usr_reg_cfg->luma_clip_low[0]); /* clip_low[1] */
    isp_adrc_lumaclip_stepl1_write(be_reg, usr_reg_cfg->luma_clip_low[1]); /* clip_low[2] */
    isp_adrc_lumaclip_stepl2_write(be_reg, usr_reg_cfg->luma_clip_low[2]); /* clip_low[3] */
    isp_adrc_lumaclip_stepl3_write(be_reg, usr_reg_cfg->luma_clip_low[3]); /* clip_low[4] */

    isp_adrc_lumaclip_steph0_write(be_reg, usr_reg_cfg->luma_clip_hig[0]); /* clip_hig[0] */
    isp_adrc_lumaclip_steph1_write(be_reg, usr_reg_cfg->luma_clip_hig[1]); /* clip_hig[1] */
    isp_adrc_lumaclip_steph2_write(be_reg, usr_reg_cfg->luma_clip_hig[2]); /* clip_hig[2] */
    isp_adrc_lumaclip_steph3_write(be_reg, usr_reg_cfg->luma_clip_hig[3]); /* clip_hig[3] */

    isp_adrc_raw_blc_write(be_reg, usr_reg_cfg->raw_blc);
    isp_adrc_cc_lut_ctrl1_write(be_reg, usr_reg_cfg->cc_ctrl);
    isp_adrc_tm_lut_ctrl_write(be_reg, usr_reg_cfg->tm_ctrl);
}

static td_void isp_drc_flt_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    isp_adrc_var_spa_fine_write(be_reg, usr_reg_cfg->var_spa_fine);
    isp_adrc_var_rng_fine_write(be_reg, usr_reg_cfg->var_rng_fine);

    isp_adrc_grad_rev_shift_write(be_reg, usr_reg_cfg->grad_rev_shift);
    isp_adrc_grad_rev_slope_write(be_reg, usr_reg_cfg->grad_rev_slope);
    isp_adrc_grad_rev_max_write(be_reg, usr_reg_cfg->grad_rev_max);
    isp_adrc_grad_rev_thres_write(be_reg, usr_reg_cfg->grad_rev_thr);

    isp_adrc_flt_spa_fine_write(be_reg, usr_reg_cfg->flt_spa_fine);
    isp_adrc_flt_rng_fine_write(be_reg, usr_reg_cfg->flt_rng_fine);

    isp_adrc_dsubfact0_write(be_reg, usr_reg_cfg->detail_adjust_coef0);
    isp_adrc_dsubfact1_write(be_reg, usr_reg_cfg->detail_adjust_coef1);

    isp_adrc_brightmax0_write(be_reg, usr_reg_cfg->detail_sup_bright_max0);
    isp_adrc_brightmin0_write(be_reg, usr_reg_cfg->detail_sup_bright_min0);
    isp_adrc_brightthr0_write(be_reg, usr_reg_cfg->detail_sup_bright_thr0);
    isp_adrc_brightslo0_write(be_reg, usr_reg_cfg->detail_sup_bright_slo0);
    isp_adrc_darkmax0_write(be_reg, usr_reg_cfg->detail_sup_dark_max0);
    isp_adrc_darkmin0_write(be_reg, usr_reg_cfg->detail_sup_dark_min0);
    isp_adrc_darkthr0_write(be_reg, usr_reg_cfg->detail_sup_dark_thr0);
    isp_adrc_darkslo0_write(be_reg, usr_reg_cfg->detail_sup_dark_slo0);

    isp_adrc_supfact1_write(be_reg, usr_reg_cfg->detail_sup1);
    isp_adrc_boostfact1_write(be_reg, usr_reg_cfg->detail_boost1);

    isp_adrc_brightmax2_write(be_reg, usr_reg_cfg->detail_sup_bright_max2);
    isp_adrc_brightmin2_write(be_reg, usr_reg_cfg->detail_sup_bright_min2);
    isp_adrc_brightthr2_write(be_reg, usr_reg_cfg->detail_sup_bright_thr2);
    isp_adrc_brightslo2_write(be_reg, usr_reg_cfg->detail_sup_bright_slo2);
    isp_adrc_darkmax2_write(be_reg, usr_reg_cfg->detail_sup_dark_max2);
    isp_adrc_darkmin2_write(be_reg, usr_reg_cfg->detail_sup_dark_min2);
    isp_adrc_darkthr2_write(be_reg, usr_reg_cfg->detail_sup_dark_thr2);
    isp_adrc_darkslo2_write(be_reg, usr_reg_cfg->detail_sup_dark_slo2);

    isp_adrc_bld_sftl_write(be_reg, usr_reg_cfg->bld_l_shift);
    isp_adrc_brightmaxl_write(be_reg, usr_reg_cfg->bld_l_bright_max);
    isp_adrc_brightminl_write(be_reg, usr_reg_cfg->bld_l_bright_min);
    isp_adrc_brightthrl_write(be_reg, usr_reg_cfg->bld_l_bright_thr);
    isp_adrc_brightslol_write(be_reg, usr_reg_cfg->bld_l_bright_slo);
    isp_adrc_darkmaxl_write(be_reg, usr_reg_cfg->bld_l_dark_max);
    isp_adrc_darkminl_write(be_reg, usr_reg_cfg->bld_l_dark_min);
    isp_adrc_darkthrl_write(be_reg, usr_reg_cfg->bld_l_dark_thr);
    isp_adrc_darkslol_write(be_reg, usr_reg_cfg->bld_l_dark_slo);

    isp_adrc_bld_sftd_write(be_reg, usr_reg_cfg->bld_d_shift);
    isp_adrc_brightmaxd_write(be_reg, usr_reg_cfg->bld_d_bright_max);
    isp_adrc_brightmind_write(be_reg, usr_reg_cfg->bld_d_bright_min);
    isp_adrc_brightthrd_write(be_reg, usr_reg_cfg->bld_d_bright_thr);
    isp_adrc_brightslod_write(be_reg, usr_reg_cfg->bld_d_bright_slo);
    isp_adrc_darkmaxd_write(be_reg, usr_reg_cfg->bld_d_dark_max);
    isp_adrc_darkmind_write(be_reg, usr_reg_cfg->bld_d_dark_min);
    isp_adrc_darkthrd_write(be_reg, usr_reg_cfg->bld_d_dark_thr);
    isp_adrc_darkslod_write(be_reg, usr_reg_cfg->bld_d_dark_slo);
}

static td_void isp_drc_wgtm_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    isp_adrc_wgtm_0_write(be_reg, usr_reg_cfg->wgt_m_lut[0]);   /* wgt_m[0] */
    isp_adrc_wgtm_1_write(be_reg, usr_reg_cfg->wgt_m_lut[1]);   /* wgt_m[1] */
    isp_adrc_wgtm_2_write(be_reg, usr_reg_cfg->wgt_m_lut[2]);   /* wgt_m[2] */
    isp_adrc_wgtm_3_write(be_reg, usr_reg_cfg->wgt_m_lut[3]);   /* wgt_m[3] */
    isp_adrc_wgtm_4_write(be_reg, usr_reg_cfg->wgt_m_lut[4]);   /* wgt_m[4] */
    isp_adrc_wgtm_5_write(be_reg, usr_reg_cfg->wgt_m_lut[5]);   /* wgt_m[5] */
    isp_adrc_wgtm_6_write(be_reg, usr_reg_cfg->wgt_m_lut[6]);   /* wgt_m[6] */
    isp_adrc_wgtm_7_write(be_reg, usr_reg_cfg->wgt_m_lut[7]);   /* wgt_m[7] */
    isp_adrc_wgtm_8_write(be_reg, usr_reg_cfg->wgt_m_lut[8]);   /* wgt_m[8] */
    isp_adrc_wgtm_9_write(be_reg, usr_reg_cfg->wgt_m_lut[9]);   /* wgt_m[9] */
    isp_adrc_wgtm_10_write(be_reg, usr_reg_cfg->wgt_m_lut[10]); /* wgt_m[10] */
    isp_adrc_wgtm_11_write(be_reg, usr_reg_cfg->wgt_m_lut[11]); /* wgt_m[11] */
    isp_adrc_wgtm_12_write(be_reg, usr_reg_cfg->wgt_m_lut[12]); /* wgt_m[12] */
    isp_adrc_wgtm_13_write(be_reg, usr_reg_cfg->wgt_m_lut[13]); /* wgt_m[13] */
    isp_adrc_wgtm_14_write(be_reg, usr_reg_cfg->wgt_m_lut[14]); /* wgt_m[14] */
}

static td_void isp_drc_mask_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg)
{
    isp_adrc_maskadacoef0_write(be_reg, usr_reg_cfg->mask_ada_coef[0]);   /* mask_ada_coef[0] */
    isp_adrc_maskadacoef1_write(be_reg, usr_reg_cfg->mask_ada_coef[1]);   /* mask_ada_coef[1] */
    isp_adrc_maskadacoef2_write(be_reg, usr_reg_cfg->mask_ada_coef[2]);   /* mask_ada_coef[2] */
    isp_adrc_maskadacoef3_write(be_reg, usr_reg_cfg->mask_ada_coef[3]);   /* mask_ada_coef[3] */
    isp_adrc_maskadacoef4_write(be_reg, usr_reg_cfg->mask_ada_coef[4]);   /* mask_ada_coef[4] */
    isp_adrc_maskadacoef5_write(be_reg, usr_reg_cfg->mask_ada_coef[5]);   /* mask_ada_coef[5] */
    isp_adrc_maskadacoef6_write(be_reg, usr_reg_cfg->mask_ada_coef[6]);   /* mask_ada_coef[6] */
    isp_adrc_maskadacoef7_write(be_reg, usr_reg_cfg->mask_ada_coef[7]);   /* mask_ada_coef[7] */
    isp_adrc_maskadacoef8_write(be_reg, usr_reg_cfg->mask_ada_coef[8]);   /* mask_ada_coef[8] */
    isp_adrc_maskadacoef9_write(be_reg, usr_reg_cfg->mask_ada_coef[9]);   /* mask_ada_coef[9] */
    isp_adrc_maskadacoef10_write(be_reg, usr_reg_cfg->mask_ada_coef[10]); /* mask_ada_coef[10] */
    isp_adrc_maskadacoef11_write(be_reg, usr_reg_cfg->mask_ada_coef[11]); /* mask_ada_coef[11] */
    isp_adrc_maskadacoef12_write(be_reg, usr_reg_cfg->mask_ada_coef[12]); /* mask_ada_coef[12] */
    isp_adrc_maskadacoef13_write(be_reg, usr_reg_cfg->mask_ada_coef[13]); /* mask_ada_coef[13] */
    isp_adrc_maskadacoef14_write(be_reg, usr_reg_cfg->mask_ada_coef[14]); /* mask_ada_coef[14] */
    isp_adrc_maskadacoef15_write(be_reg, usr_reg_cfg->mask_ada_coef[15]); /* mask_ada_coef[15] */

    isp_adrc_tmgainskip_write(be_reg, usr_reg_cfg->tm_gain_skip);
    isp_adrc_npu_wrt_ds_write(be_reg, usr_reg_cfg->npu_wrt_ds);
    isp_adrc_npu_wrt_sel_write(be_reg, usr_reg_cfg->npu_wrt_sel);
    isp_adrc_npu_wrt_en_write(be_reg, usr_reg_cfg->npu_wrt_en);
    isp_adrc_maskada_en_write(be_reg, usr_reg_cfg->mask_ada_en);
    isp_adrc_maskadasft_write(be_reg, usr_reg_cfg->mask_ada_sft);
    isp_adrc_lumavegmanual_write(be_reg, usr_reg_cfg->luma_avg_manual);

    isp_adrc_statcoefsft0_write(be_reg, usr_reg_cfg->stat_coef_sft0);
    isp_adrc_statcoefsft1_write(be_reg, usr_reg_cfg->stat_coef_sft1);
    isp_adrc_statflttype_write(be_reg, usr_reg_cfg->stat_flt_type);
    isp_adrc_statformat_write(be_reg, usr_reg_cfg->stat_format);
}

static td_void isp_drc_stat_reg_config(isp_post_be_reg_type *be_reg, isp_drc_dyna_cfg *dyna_reg_cfg)
{
    isp_adrc_num_write(be_reg, dyna_reg_cfg->m_num_max);
    isp_adrc_vbiflt_en_write(be_reg, dyna_reg_cfg->vbiflt_en);

    isp_adrc_big_x_init_write(be_reg, dyna_reg_cfg->big_x_init);
    isp_adrc_idx_x_init_write(be_reg, dyna_reg_cfg->idx_x_init);
    isp_adrc_cnt_x_init_write(be_reg, dyna_reg_cfg->cnt_x_init);
    isp_adrc_acc_x_init_write(be_reg, dyna_reg_cfg->acc_x_init);
    isp_adrc_blk_wgt_init_write(be_reg, dyna_reg_cfg->wgt_x_init); /* drc v4.0 */
    isp_adrc_total_width_write(be_reg, dyna_reg_cfg->total_width - 1);
    isp_adrc_stat_width_write(be_reg, dyna_reg_cfg->stat_width - 1);

    isp_adrc_hnum_write(be_reg, dyna_reg_cfg->block_h_num);
    isp_adrc_vnum_write(be_reg, dyna_reg_cfg->block_v_num);

    isp_adrc_zone_hsize_write(be_reg, dyna_reg_cfg->block_h_size - 1);
    isp_adrc_zone_vsize_write(be_reg, dyna_reg_cfg->block_v_size - 1);
    isp_adrc_chk_x_write(be_reg, dyna_reg_cfg->block_chk_x);
    isp_adrc_chk_y_write(be_reg, dyna_reg_cfg->block_chk_y);

    isp_adrc_div_x0_write(be_reg, dyna_reg_cfg->div_x0);
    isp_adrc_div_x1_write(be_reg, dyna_reg_cfg->div_x1);
    isp_adrc_div_y0_write(be_reg, dyna_reg_cfg->div_y0);
    isp_adrc_div_y1_write(be_reg, dyna_reg_cfg->div_y1);

    isp_adrc_bin_scale_write(be_reg, dyna_reg_cfg->bin_scale);
}

static td_void isp_drc_static_reg_config(isp_post_be_reg_type *be_reg, isp_drc_static_cfg *static_reg_cfg)
{
    if (static_reg_cfg->resh) {
        isp_drc_static_comm_reg_config(be_reg, static_reg_cfg);
        isp_drc_seg_reg_config(be_reg, static_reg_cfg);
        isp_drc_rg_bg_reg_config(be_reg, static_reg_cfg);
        isp_drc_prev_luma_reg_config(be_reg, static_reg_cfg);

        static_reg_cfg->resh = TD_FALSE;
    }
}

static td_void isp_drc_usr_reg_config(isp_post_be_reg_type *be_reg, isp_drc_usr_cfg *usr_reg_cfg, td_bool offline_mode)
{
    td_bool usr_resh, idx_resh;
    /* user */
    idx_resh = (isp_drc_update_index_read(be_reg) != usr_reg_cfg->update_index);
    usr_resh = (offline_mode) ? (usr_reg_cfg->resh && idx_resh) : (usr_reg_cfg->resh);

    if (usr_resh) {
        isp_drc_update_index_write(be_reg, usr_reg_cfg->update_index);

        isp_drc_usr_comm_reg_config(be_reg, usr_reg_cfg);
        isp_drc_flt_reg_config(be_reg, usr_reg_cfg);
        isp_drc_wgtm_reg_config(be_reg, usr_reg_cfg);
        isp_drc_pw_curve_reg_config(be_reg, usr_reg_cfg);
        isp_drc_mask_reg_config(be_reg, usr_reg_cfg);

        isp_drc_cclut_reg_config(offline_mode, be_reg, usr_reg_cfg);
        isp_drc_mlut_reg_config(offline_mode, be_reg, usr_reg_cfg);
        isp_drc_mixlut_reg_config(offline_mode, be_reg, usr_reg_cfg);

        usr_reg_cfg->resh = offline_mode;
    }
}

static td_s32 isp_drc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool tm_lut_update = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_drc_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_drc_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);

        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);

        isp_adrc_en_write(post_viproc, (&reg_cfg_info->alg_reg_cfg[i])->drc_reg_cfg.enable);

        isp_drc_static_reg_config(be_reg, &reg_cfg_info->alg_reg_cfg[i].drc_reg_cfg.static_reg_cfg);
        isp_drc_usr_reg_config(be_reg, &reg_cfg_info->alg_reg_cfg[i].drc_reg_cfg.usr_reg_cfg, offline_mode);

        /* dynamic */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].drc_reg_cfg.dyna_reg_cfg;

        if (dyna_reg_cfg->resh) {
            isp_adrc_color_corr_en_write(be_reg, dyna_reg_cfg->cc_en);
            isp_adrc_pdwhighthr_write(be_reg, dyna_reg_cfg->pdw_high_thr);
            isp_drc_tmlut_reg_config(&tm_lut_update, isp_ctx, be_reg, dyna_reg_cfg);

            if (dyna_reg_cfg->img_size_changed) {
                isp_drc_stat_reg_config(be_reg, dyna_reg_cfg);
                dyna_reg_cfg->img_size_changed = offline_mode;
            }

            isp_adrc_strength_write(be_reg, dyna_reg_cfg->strength);

            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_drc_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg.drc_tm_lut_update = tm_lut_update || offline_mode;

    return TD_SUCCESS;
}

static td_void isp_dehaze_static_reg_config(isp_post_be_reg_type *be_reg, isp_dehaze_static_cfg *static_reg_cfg)
{
    /* static registers */
    if (static_reg_cfg->resh) {
        isp_dehaze_max_mode_write(be_reg, static_reg_cfg->max_mode);
        isp_dehaze_thld_write(be_reg, static_reg_cfg->dehaze_thld);
        isp_dehaze_blthld_write(be_reg, static_reg_cfg->dehaze_blthld);
        isp_dehaze_neg_mode_write(be_reg, static_reg_cfg->dehaze_neg_mode);
        isp_dehaze_block_sum_write(be_reg, static_reg_cfg->block_sum);
        isp_dehaze_dc_numh_write(be_reg, static_reg_cfg->dchnum);
        isp_dehaze_dc_numv_write(be_reg, static_reg_cfg->dcvnum);

        static_reg_cfg->resh = TD_FALSE;
    }
}

static td_s32 isp_dehaze_dyna_lut_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i,
    isp_post_be_reg_type *be_reg, isp_viproc_reg_type *post_viproc)
{
    td_u8 buf_id;
    td_u16 blk_num;
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_dehaze_static_cfg *static_reg_cfg = TD_NULL;
    isp_dehaze_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.static_reg_cfg;
    dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.dyna_reg_cfg;

    blk_num = ((static_reg_cfg->dchnum + 1) * (static_reg_cfg->dcvnum + 1) + 1) / 2; /* blk calculation 2 */
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        /* online lut2stt regconfig */
        buf_id = dyna_reg_cfg->buf_id;

        be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_return(be_lut_stt_reg);

        isp_dehaze_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, blk_num, dyna_reg_cfg->prestat, dyna_reg_cfg->lut);
        ret = isp_dehaze_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
        if (ret != TD_SUCCESS) {
            isp_err_trace("isp[%d]_dehaze_lut_wstt_addr_write failed\n", vi_pipe);
            return ret;
        }

        dyna_reg_cfg->buf_id = 1 - buf_id;
    } else {
        isp_dehaze_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, blk_num, dyna_reg_cfg->prestat,
            dyna_reg_cfg->lut);
    }

    isp_dehaze_stt2lut_en_write(be_reg, TD_TRUE);
    isp_dehaze_stt2lut_regnew_write(be_reg, TD_TRUE);

    return TD_SUCCESS;
}

static td_void isp_dehaze_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg)
{
    td_u32 dehaze_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        dehaze_stt2lut_clr = isp_dehaze_stt2lut_clr_read(be_reg);
        if (dehaze_stt2lut_clr != 0) {
            isp_dehaze_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_dehaze_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 sys_blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_dehaze_static_cfg *static_reg_cfg = TD_NULL;
    isp_dehaze_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_dehaze_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.dyna_reg_cfg;

        isp_dehaze_static_reg_config(be_reg, static_reg_cfg);
        isp_dehaze_lut_width_word_write(post_viproc, OT_ISP_DEHAZE_LUT_WIDTH_WORD_DEFAULT);

        ret = isp_dehaze_dyna_lut_reg_config(vi_pipe, reg_cfg_info, i, be_reg, post_viproc);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        isp_dehaze_block_sizeh_write(be_reg, dyna_reg_cfg->blockhsize);
        isp_dehaze_block_sizev_write(be_reg, dyna_reg_cfg->blockvsize);
        isp_dehaze_phase_x_write(be_reg, dyna_reg_cfg->phasex);
        isp_dehaze_phase_y_write(be_reg, dyna_reg_cfg->phasey);

        isp_dehaze_smlmapoffset_write(be_reg, dyna_reg_cfg->sml_map_offset);
        isp_dehaze_statstartx_write(be_reg, dyna_reg_cfg->stat_start_x);
        isp_dehaze_statendx_write(be_reg, dyna_reg_cfg->stat_end_x);

        isp_dehaze_stat_numv_write(be_reg, dyna_reg_cfg->statnum_v);
        isp_dehaze_stat_numh_write(be_reg, dyna_reg_cfg->statnum_h);

        isp_dehaze_reg_update_online(offline_mode, be_reg);
        reg_cfg_info->cfg_key.bit1_dehaze_cfg = offline_mode ? 1 : ((sys_blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_bnr_rlsc_lut_reg_config(td_u8 buf_id, ot_vi_pipe vi_pipe, td_u8 i, isp_post_be_reg_type *be_reg,
    isp_rlsc_usr_cfg *usr_reg_cfg)
{
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_viproc);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) { /* online lut2stt regconfig */
        be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_return(be_lut_stt_reg);
        isp_bnr_rlsc_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, usr_reg_cfg->gain_lut[0],
            usr_reg_cfg->gain_lut[1],                            /* 0:channel R, 1: channel Gr */
            usr_reg_cfg->gain_lut[2], usr_reg_cfg->gain_lut[3]); /* 2: channel Gb, 3: channel B */
        isp_bnr_stt2lut_en_write(be_reg, TD_TRUE);
    } else {
        isp_bnr_rlsc_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, usr_reg_cfg->gain_lut[0],
            usr_reg_cfg->gain_lut[1],                            /* 0: channel R, 1: channel Gr */
            usr_reg_cfg->gain_lut[2], usr_reg_cfg->gain_lut[3]); /* 2: channel Gb, 3: channel B */
        isp_bnr_stt2lut_en_write(be_reg, TD_TRUE);
        isp_bnr_stt2lut_regnew_write(be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_void isp_bnr_lut_wstt_write(isp_post_be_lut_wstt_type *post_be_lut2stt, isp_bayernr_dyna_cfg *dyna_reg_cfg)
{
    isp_bnr_noisesd_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->u16noise_sd_lut, dyna_reg_cfg->u16noise_sd_lut_s);

    isp_bnr_noisesd_m_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->u16noise_sd_lut_m);

    isp_bnr_fbratiotable_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->u8fb_ratio_table);

    isp_bnr_noiseinvs_magidx_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->u8noise_inv_s_mag_idx,
        dyna_reg_cfg->u8noise_inv_m_mag_idx);

    isp_bnr_coring_low_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->jnlm_coring_low_lut);

    isp_bnr_noiseinv_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->u32noise_inv_lut);

    isp_bnr_noiseinv_m_lut_wstt_write(post_be_lut2stt, dyna_reg_cfg->u32noise_inv_lut_m);
}

static td_void isp_bnr_lut_reg_config(ot_vi_pipe vi_pipe, isp_post_be_reg_type *be_reg,
    isp_bayernr_dyna_cfg *dyna_reg_cfg, isp_rlsc_usr_cfg *rlsc_usr_reg_cfg, td_u8 i)
{
    td_u8 buf_id = 0;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(post_viproc);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        buf_id = dyna_reg_cfg->buf_id;

        be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
        isp_check_pointer_void_return(be_lut_stt_reg);

        isp_bnr_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, dyna_reg_cfg);
        isp_bnr_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);

        isp_bnr_rlsc_lut_reg_config(buf_id, vi_pipe, i, be_reg, rlsc_usr_reg_cfg);

        dyna_reg_cfg->buf_id = 1 - buf_id;
    } else {
        isp_bnr_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, dyna_reg_cfg);

        isp_bnr_rlsc_lut_reg_config(buf_id, vi_pipe, i, be_reg, rlsc_usr_reg_cfg);
    }
    isp_bnr_stt2lut_en_write(be_reg, TD_TRUE);
    isp_bnr_stt2lut_regnew_write(be_reg, TD_TRUE);
    isp_bnr_lut_width_word_write(post_viproc, OT_ISP_BNR_LUT_WIDTH_WORD_DEFAULT);
}

static td_void isp_bnr_ds_write(isp_post_be_reg_type *be_reg, td_u8 *ds)
{
    isp_bnr_ds0_write(be_reg, ds[0]);   /* ds0 */
    isp_bnr_ds1_write(be_reg, ds[1]);   /* ds1 */
    isp_bnr_ds2_write(be_reg, ds[2]);   /* ds2 */
    isp_bnr_ds3_write(be_reg, ds[3]);   /* ds3 */
    isp_bnr_ds4_write(be_reg, ds[4]);   /* ds4 */
    isp_bnr_ds5_write(be_reg, ds[5]);   /* ds5 */
    isp_bnr_ds6_write(be_reg, ds[6]);   /* ds6 */
    isp_bnr_ds7_write(be_reg, ds[7]);   /* ds7 */
    isp_bnr_ds8_write(be_reg, ds[8]);   /* ds8 */
    isp_bnr_ds9_write(be_reg, ds[9]);   /* ds9 */
    isp_bnr_ds10_write(be_reg, ds[10]); /* ds10 */
    isp_bnr_ds11_write(be_reg, ds[11]); /* ds11 */
    isp_bnr_ds12_write(be_reg, ds[12]); /* ds12 */
    isp_bnr_ds13_write(be_reg, ds[13]); /* ds13 */
    isp_bnr_ds14_write(be_reg, ds[14]); /* ds14 */
    isp_bnr_ds15_write(be_reg, ds[15]); /* ds15 */
}
static td_void isp_bnr_dp_write(isp_post_be_reg_type *be_reg, td_u8 *dp)
{
    isp_bnr_dp0_write(be_reg, dp[0]);   /* dp0 */
    isp_bnr_dp1_write(be_reg, dp[1]);   /* dp1 */
    isp_bnr_dp2_write(be_reg, dp[2]);   /* dp2 */
    isp_bnr_dp3_write(be_reg, dp[3]);   /* dp3 */
    isp_bnr_dp4_write(be_reg, dp[4]);   /* dp4 */
    isp_bnr_dp5_write(be_reg, dp[5]);   /* dp5 */
    isp_bnr_dp6_write(be_reg, dp[6]);   /* dp6 */
    isp_bnr_dp7_write(be_reg, dp[7]);   /* dp7 */
    isp_bnr_dp8_write(be_reg, dp[8]);   /* dp8 */
    isp_bnr_dp9_write(be_reg, dp[9]);   /* dp9 */
    isp_bnr_dp10_write(be_reg, dp[10]); /* dp10 */
    isp_bnr_dp11_write(be_reg, dp[11]); /* dp11 */
    isp_bnr_dp12_write(be_reg, dp[12]); /* dp12 */
    isp_bnr_dp13_write(be_reg, dp[13]); /* dp13 */
    isp_bnr_dp14_write(be_reg, dp[14]); /* dp14 */
    isp_bnr_dp15_write(be_reg, dp[15]); /* dp15 */
}
static td_void isp_bnr_mixlut_write(isp_post_be_reg_type *be_reg, td_u8 *mix_lut)
{
    isp_bnr_mixlut0_write(be_reg, mix_lut[0]);   /* mix_lut0 */
    isp_bnr_mixlut1_write(be_reg, mix_lut[1]);   /* mix_lut1 */
    isp_bnr_mixlut2_write(be_reg, mix_lut[2]);   /* mix_lut2 */
    isp_bnr_mixlut3_write(be_reg, mix_lut[3]);   /* mix_lut3 */
    isp_bnr_mixlut4_write(be_reg, mix_lut[4]);   /* mix_lut4 */
    isp_bnr_mixlut5_write(be_reg, mix_lut[5]);   /* mix_lut5 */
    isp_bnr_mixlut6_write(be_reg, mix_lut[6]);   /* mix_lut6 */
    isp_bnr_mixlut7_write(be_reg, mix_lut[7]);   /* mix_lut7 */
    isp_bnr_mixlut8_write(be_reg, mix_lut[8]);   /* mix_lut8 */
    isp_bnr_mixlut9_write(be_reg, mix_lut[9]);   /* mix_lut9 */
    isp_bnr_mixlut10_write(be_reg, mix_lut[10]); /* mix_lut10 */
    isp_bnr_mixlut11_write(be_reg, mix_lut[11]); /* mix_lut11 */
    isp_bnr_mixlut12_write(be_reg, mix_lut[12]); /* mix_lut12 */
    isp_bnr_mixlut13_write(be_reg, mix_lut[13]); /* mix_lut13 */
    isp_bnr_mixlut14_write(be_reg, mix_lut[14]); /* mix_lut14 */
    isp_bnr_mixlut15_write(be_reg, mix_lut[15]); /* mix_lut15 */
    isp_bnr_mixlut16_write(be_reg, mix_lut[16]); /* mix_lut16 */
    isp_bnr_mixlut17_write(be_reg, mix_lut[17]); /* mix_lut17 */
    isp_bnr_mixlut18_write(be_reg, mix_lut[18]); /* mix_lut18 */
    isp_bnr_mixlut19_write(be_reg, mix_lut[19]); /* mix_lut19 */
    isp_bnr_mixlut20_write(be_reg, mix_lut[20]); /* mix_lut20 */
    isp_bnr_mixlut21_write(be_reg, mix_lut[21]); /* mix_lut21 */
    isp_bnr_mixlut22_write(be_reg, mix_lut[22]); /* mix_lut22 */
    isp_bnr_mixlut23_write(be_reg, mix_lut[23]); /* mix_lut23 */
    isp_bnr_mixlut24_write(be_reg, mix_lut[24]); /* mix_lut24 */
    isp_bnr_mixlut25_write(be_reg, mix_lut[25]); /* mix_lut25 */
    isp_bnr_mixlut26_write(be_reg, mix_lut[26]); /* mix_lut26 */
    isp_bnr_mixlut27_write(be_reg, mix_lut[27]); /* mix_lut27 */
    isp_bnr_mixlut28_write(be_reg, mix_lut[28]); /* mix_lut28 */
    isp_bnr_mixlut29_write(be_reg, mix_lut[29]); /* mix_lut29 */
    isp_bnr_mixlut30_write(be_reg, mix_lut[30]); /* mix_lut30 */
    isp_bnr_mixlut31_write(be_reg, mix_lut[31]); /* mix_lut31 */
}

static td_void isp_bnr_dyna_reg_config(isp_post_be_reg_type *be_reg, isp_bayernr_dyna_cfg *dyna_reg_cfg)
{
    isp_bnr_ensptnr_write(be_reg, dyna_reg_cfg->snr_enable);
    isp_bnr_en2ndtmpnr_write(be_reg, dyna_reg_cfg->tnr2nd_enable);
    isp_bnr_nrstructure_write(be_reg, 1);
    isp_bnr_enshrinkrnt_write(be_reg, 1);
    isp_bnr_coring_enable_write(be_reg, dyna_reg_cfg->coring_enable);
    isp_bnr_bm1d_enable_write(be_reg, dyna_reg_cfg->sfm_mode);
    isp_bnr_jnlm_sadratio_write(be_reg, dyna_reg_cfg->jnlm_sad_ratio);
    isp_bnr_coring_hig_write(be_reg, dyna_reg_cfg->jnlm_coring_hig);
    isp_bnr_coring_low_motadp_write(be_reg, dyna_reg_cfg->coring_mot_adp);
    isp_bnr_coring_low_motth_write(be_reg, dyna_reg_cfg->coring_motth);
    isp_bnr_coring_data_mode_write(be_reg, dyna_reg_cfg->coring_data_mode);
    isp_bnr_coring_awbgainr_write(be_reg, dyna_reg_cfg->r_lmt_rgain);
    isp_bnr_coring_awbgainb_write(be_reg, dyna_reg_cfg->r_lmt_bgain);
    isp_bnr_edgeflten_write(be_reg, dyna_reg_cfg->edge_flt_en);
    isp_bnr_dstflten_write(be_reg, dyna_reg_cfg->dst_flt_en);
    isp_bnr_edgedirslp_write(be_reg, dyna_reg_cfg->edge_dir_slp);
    isp_bnr_edgedgainmax_write(be_reg, dyna_reg_cfg->edge_dgain_max);
    isp_bnr_edgedgainslp_write(be_reg, dyna_reg_cfg->edge_dgain_slp);
    isp_bnr_sadi_en_write(be_reg, dyna_reg_cfg->sadi_en);
    isp_bnr_dmap_en_write(be_reg, dyna_reg_cfg->dmap_en);
    isp_bnr_satd_en_write(be_reg, dyna_reg_cfg->satd_en);
    isp_bnr_nlm_edgectrl_write(be_reg, dyna_reg_cfg->nlm_edgectrl);
    isp_bnr_nlm_enable_write(be_reg, dyna_reg_cfg->nlm_enable);
    isp_bnr_jnlm_nlimit_adp_th0_write(be_reg, dyna_reg_cfg->sfm0_nlimit_adp_th0);
    isp_bnr_jnlm_nlimit_adp_th1_write(be_reg, dyna_reg_cfg->sfm0_nlimit_adp_th1);
    isp_bnr_jnlm_nlimit_adp_str_write(be_reg, dyna_reg_cfg->sfm0_nlimit_adp_str);
    isp_bnr_jnlm_nlimit_adp_thr_write(be_reg, dyna_reg_cfg->sfm0_nlimit_adp_thr);
    isp_bnr_jnlm_rsad_max_write(be_reg, dyna_reg_cfg->sfm0_r_sad_max);
    isp_bnr_jnlm_adaptive_write(be_reg, dyna_reg_cfg->sfm0_adaptive);
    isp_bnr_egv_coef_write(be_reg, dyna_reg_cfg->edge_dmap_coef);
    isp_bnr_edge_ratio_th_write(be_reg, dyna_reg_cfg->edge_ratio_th);
    isp_bnr_edge_win_size_write(be_reg, 0);
    isp_bnr_bm1d_offsetm_write(be_reg, dyna_reg_cfg->sfm_offset_m);
    isp_bnr_bm1d_noisem2_write(be_reg, dyna_reg_cfg->sfm_noise_m2);
    isp_bnr_bm1d_noisem1_write(be_reg, dyna_reg_cfg->sfm_noise_m1);
    isp_bnr_bm1d_dvalth_write(be_reg, dyna_reg_cfg->sfm_dmap_th);
    isp_bnr_bm1d_offsets_write(be_reg, dyna_reg_cfg->sfm_offset_s);
    isp_bnr_bm1d_noises2_write(be_reg, dyna_reg_cfg->sfm_noise_s2);
    isp_bnr_bm1d_noises1_write(be_reg, dyna_reg_cfg->sfm_noise_s1);
    isp_bnr_bm1d_dvals4_write(be_reg, dyna_reg_cfg->sfm_dmap_s4);
    isp_bnr_bm1d_dvals3_write(be_reg, dyna_reg_cfg->sfm_dmap_s3);
    isp_bnr_bm1d_dvals2_write(be_reg, dyna_reg_cfg->sfm_dmap_s2);
    isp_bnr_bm1d_dvals1_write(be_reg, dyna_reg_cfg->sfm_dmap_s1);
    isp_bnr_bm1d_blend_slp1_write(be_reg, dyna_reg_cfg->sfm_blend_slp1);
    isp_bnr_bm1d_blend_th1_write(be_reg, dyna_reg_cfg->sfm_blend_th1);
    isp_bnr_bm1d_blend_max1_write(be_reg, dyna_reg_cfg->sfm_blend_max1);
    isp_bnr_bm1d_blend_min1_write(be_reg, dyna_reg_cfg->sfm_blend_min1);
    isp_bnr_bm1d_blend_slp2_write(be_reg, dyna_reg_cfg->sfm_blend_slp2);
    isp_bnr_bm1d_blend_th2_write(be_reg, dyna_reg_cfg->sfm_blend_th2);
    isp_bnr_bm1d_blend_max2_write(be_reg, dyna_reg_cfg->sfm_blend_max2);
    isp_bnr_bm1d_blend_min2_write(be_reg, dyna_reg_cfg->sfm_blend_min2);
    isp_bnr_bm1d_edgsad_gain_write(be_reg, dyna_reg_cfg->sfm_edge_sad_gain);
    isp_bnr_bm1d_edg_cntrl_write(be_reg, dyna_reg_cfg->sfm_edge_ctrl);
    isp_bnr_bm1d_lmt_adp_en_write(be_reg, dyna_reg_cfg->sfm_lmt_adp_en);
    isp_bnr_bm1d_blend_lmt_write(be_reg, dyna_reg_cfg->sfm_blend_lmt);
    isp_bnr_bm1d_lmt_enable_write(be_reg, dyna_reg_cfg->bm1d_lmt_enable);
    isp_bnr_tss_write(be_reg, dyna_reg_cfg->tss);
    isp_bnr_movtmp_ratio_write(be_reg, dyna_reg_cfg->movtmp_ratio);
    isp_bnr_enable_mix_lut_write(be_reg, dyna_reg_cfg->enable_mix_lut);

    isp_bnr_mdetmixratio_write(be_reg, dyna_reg_cfg->md_ratio);
    isp_bnr_mdetcorlevel_write(be_reg, dyna_reg_cfg->md_cor_level);
    isp_bnr_mdetsize_write(be_reg, dyna_reg_cfg->md_size);
    isp_bnr_rntth_write(be_reg, dyna_reg_cfg->tnr_rnt_th);
    isp_bnr_nrstrengthmvint_write(be_reg, dyna_reg_cfg->tnr_nr_strength_mv);
    isp_bnr_nrstrengthstint_write(be_reg, dyna_reg_cfg->tnr_nr_strength_st);
    isp_bnr_nrstrengthslope_write(be_reg, dyna_reg_cfg->tnr_nr_strength_slope);
    isp_bnr_expratio_write(be_reg, dyna_reg_cfg->u16expratio);
    isp_bnr_targetnoiseratio_write(be_reg, dyna_reg_cfg->mix_target);
    isp_bnr_mixgain_write(be_reg, dyna_reg_cfg->mix_gain);
    isp_bnr_mixgain_0_r_write(be_reg, dyna_reg_cfg->mix_gain_r);
    isp_bnr_mixgain_0_b_write(be_reg, dyna_reg_cfg->mix_gain_b);

    isp_bnr_wdr_enable_write(be_reg, dyna_reg_cfg->wdr_enable);
    isp_bnr_wdr_enfusion_write(be_reg, dyna_reg_cfg->wdr_en_fusion);
    isp_bnr_encenterweight_write(be_reg, dyna_reg_cfg->center_wgt_en);
    isp_bnr_wdr_mapgain_write(be_reg, dyna_reg_cfg->wdr_map_gain);
    isp_bnr_wdr_mapfltmod_write(be_reg, dyna_reg_cfg->wdr_map_flt_mod);
    isp_bnr_jnlm_symcoef_write(be_reg, dyna_reg_cfg->jnlm_sym_coef);
    isp_bnr_jnlm_gain_s_write(be_reg, dyna_reg_cfg->jnlm_gain_s);
    isp_bnr_jnlm_gain_m_write(be_reg, dyna_reg_cfg->jnlm_gain_m);

    isp_bnr_jnlmgain_r0_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain[0]);     /* 0 */
    isp_bnr_jnlmgain_gr0_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain[1]);    /* 1 */
    isp_bnr_jnlmgain_gb0_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain[2]);    /* 2 */
    isp_bnr_jnlmgain_b0_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain[3]);     /* 3 */
    isp_bnr_jnlmgain_r_s_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain_s[0]);  /* 0 */
    isp_bnr_jnlmgain_gr_s_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain_s[1]); /* 1 */
    isp_bnr_jnlmgain_gb_s_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain_s[2]); /* 2 */
    isp_bnr_jnlmgain0_write(be_reg, dyna_reg_cfg->jnlm_limit_mult_gain_s[3]);     /* 3 */

    isp_bnr_jnlm_maxwtcoef_write(be_reg, dyna_reg_cfg->jnlm_max_wt_coef); /* isp_bnr_jnlm_maxwtcoef */
    isp_bnr_blc_write(be_reg, dyna_reg_cfg->u8bnrblc);

    isp_bnr_bm1d_dstgain_slp_write(be_reg, dyna_reg_cfg->sfm1_dstgain_slp);
    isp_bnr_bm1d_dstgain_max_write(be_reg, dyna_reg_cfg->sfm1_dstgain_max);
    isp_bnr_dstgain_slp_write(be_reg, dyna_reg_cfg->sfm0_dstgain_slp);
    isp_bnr_dstgain_max_write(be_reg, dyna_reg_cfg->sfm0_dstgain_max);
    isp_bnr_bm1d_adp_coef_write(be_reg, dyna_reg_cfg->sfm1_adp_coef);
    isp_bnr_nlm_dis_adp_coef_write(be_reg, dyna_reg_cfg->sfm0_dis_adp_coef);
    isp_bnr_nlm_adp_coef_write(be_reg, dyna_reg_cfg->sfm0_adp_coef);
    isp_bnr_adap_mode_write(be_reg, dyna_reg_cfg->sfm0_adap_mode);
    isp_bnr_flt_mode_write(be_reg, dyna_reg_cfg->sfm0_flt_mode);
    isp_bnr_bm1d_adp_th_write(be_reg, dyna_reg_cfg->sfm_adp_th);
    isp_bnr_nlm_dis_adp_th_write(be_reg, dyna_reg_cfg->sfm0_dis_adp_th);
    isp_bnr_nlm_adp_th_write(be_reg, dyna_reg_cfg->sfm0_adp_th);
    isp_bnr_bm1d_adp_th1_write(be_reg, dyna_reg_cfg->sfm_adp_th1);
    isp_bnr_nlm_dis_adp_th1_write(be_reg, dyna_reg_cfg->sfm0_dis_adp_th1);
    isp_bnr_nlm_adp_th1_write(be_reg, dyna_reg_cfg->sfm0_adp_th1);
    isp_bnr_bm1d_dis_cntrl_write(be_reg, dyna_reg_cfg->sfm_dis_ctrl);
    isp_bnr_bm1d_offsetl_write(be_reg, dyna_reg_cfg->sfm_offset_l);
    isp_bnr_bm1d_noisel2_write(be_reg, dyna_reg_cfg->sfm_noise_l2);
    isp_bnr_bm1d_noisel1_write(be_reg, dyna_reg_cfg->sfm_noise_l1);
    isp_bnr_coefsatd_write(be_reg, dyna_reg_cfg->mdcoefsatd);
    isp_bnr_coefdiffy_write(be_reg, dyna_reg_cfg->mdcoefdiffy);
    isp_bnr_dstmode_write(be_reg, 1);

    isp_bnr_ds_write(be_reg, dyna_reg_cfg->ds);
    isp_bnr_dp_write(be_reg, dyna_reg_cfg->dp);
    isp_bnr_mixlut_write(be_reg, dyna_reg_cfg->mix_lut);

    isp_bnr_nois_var_write(be_reg, dyna_reg_cfg->nois_var);
    isp_bnr_sht_mode_write(be_reg, dyna_reg_cfg->sht_mode);
    isp_bnr_sht_mode1_write(be_reg, dyna_reg_cfg->sht_mode1);
    isp_bnr_edge_tp1_write(be_reg, dyna_reg_cfg->edge_tp1);
    isp_bnr_edge_tp0_write(be_reg, dyna_reg_cfg->edge_tp0);
    isp_bnr_nshift_write(be_reg, dyna_reg_cfg->nshift);
    isp_bnr_tsr1_write(be_reg, dyna_reg_cfg->tsr1);
    isp_bnr_tsr0_write(be_reg, dyna_reg_cfg->tsr0);

    isp_bnr_rlsc_en_write(be_reg, dyna_reg_cfg->bnrlsc_en);
    isp_bnr_lscratio2_write(be_reg, dyna_reg_cfg->bnrlscratio2);
    isp_bnr_lscratio1_write(be_reg, dyna_reg_cfg->bnrlscratio1);
    isp_bnr_fusion_str_write(be_reg, 16); /* 16 */
    isp_bnr_bm1d_strength_s_write(be_reg, dyna_reg_cfg->bm1d_strength_s);
    isp_bnr_nlm_nshift_write(be_reg, dyna_reg_cfg->nlm_nshift);
    isp_bnr_bm1d_n8_coef_write(be_reg, dyna_reg_cfg->bm1d_n8_coef);
    isp_bnr_rfr_mode_write(be_reg, dyna_reg_cfg->rfr_mode);
    isp_bnr_tfr_luma_write(be_reg, dyna_reg_cfg->tfr_luma);
    isp_bnr_tfr_en_write(be_reg, dyna_reg_cfg->tfr_en);
    isp_bnr_mix_ratio_rgb_write(be_reg, dyna_reg_cfg->mix_ratio_rgb);
    isp_bnr_b_qtr_size_ir_write(be_reg, dyna_reg_cfg->qtr_size_ir);
    isp_bnr_b_disable1st_rgb_tnr_write(be_reg, 1);

    isp_bnr_bit_depth_irin_write(be_reg, dyna_reg_cfg->bitdepth_ir_in);
    isp_bnr_wdrlong_slp_write(be_reg, dyna_reg_cfg->wdrlong_slp);
    isp_bnr_wdrlong_hig_write(be_reg, dyna_reg_cfg->wdrlong_hig);
    isp_bnr_wdrlong_low_write(be_reg, dyna_reg_cfg->wdrlong_low);
    isp_bnr_wdrmid_slp_write(be_reg, dyna_reg_cfg->wdrmid_slp);
    isp_bnr_wdrmid_hig_write(be_reg, dyna_reg_cfg->wdrmid_hig);
    isp_bnr_wdrmid_low_write(be_reg, dyna_reg_cfg->wdrmid_low);
    isp_bnr_tp_alpha_gain2nd_write(be_reg, dyna_reg_cfg->tp_alpha_gain2nd);
    isp_bnr_alpha_max_st_write(be_reg, dyna_reg_cfg->alha_max_st);
    isp_bnr_wdr_ghost_thres_write(be_reg, dyna_reg_cfg->wdr_ghost_thres);
    isp_bnr_wdr_ghost_nrstr_write(be_reg, dyna_reg_cfg->wdr_ghost_nrstr);
    isp_bnr_tfr_thresh1_write(be_reg, dyna_reg_cfg->tfr_thresh1);
    isp_bnr_tfr_thresh_write(be_reg, dyna_reg_cfg->tfr_thresh);
}

static td_void isp_bayer_nr_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_bayernr_dyna_cfg *dyna_reg_cfg)
{
    td_u32 bnr_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        bnr_stt2lut_clr = isp_bnr_stt2lut_clr_read(be_reg);
        if (bnr_stt2lut_clr != 0) {
            dyna_reg_cfg->resh = TD_TRUE;
            isp_bnr_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_bayer_nr_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode, mot_en;
    isp_bayernr_static_cfg *static_reg_cfg = TD_NULL;
    isp_bayernr_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_rlsc_usr_cfg *rlsc_usr_reg_cfg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_bayernr_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(post_viproc);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bnr_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].bnr_reg_cfg.dyna_reg_cfg;
        rlsc_usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg;
        mot_en = dyna_reg_cfg->tnr_enable && reg_cfg_info->alg_reg_cfg[i].bnr_reg_cfg.bnr_enable &&
            reg_cfg_info->alg_reg_cfg[i].sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg.mot_enable;
        if (static_reg_cfg->resh) { /* static */
            isp_bnr_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].bnr_reg_cfg.bnr_enable);
            isp_bnr_entmpnr_write(be_reg, dyna_reg_cfg->tnr_enable);
            isp_sharpen_mot_en_write(be_reg, mot_en);
            isp_bnr_isinitial_write(be_reg, TD_TRUE);
            isp_bnr_jnlm_lumsel_write(be_reg, static_reg_cfg->jnlm_sel);
            isp_bnr_jnlm_winsize_write(be_reg, static_reg_cfg->nlm_winsize);
            isp_bnr_enmixing_write(be_reg, static_reg_cfg->mix_enable);
            static_reg_cfg->resh = TD_FALSE;
        }

        if (dyna_reg_cfg->resh) {
            isp_bnr_dyna_reg_config(be_reg, dyna_reg_cfg);
            isp_bnr_lut_reg_config(vi_pipe, be_reg, dyna_reg_cfg, rlsc_usr_reg_cfg, i);
            dyna_reg_cfg->resh = offline_mode;
        }

        isp_bayer_nr_reg_update_online(offline_mode, be_reg, dyna_reg_cfg);
        reg_cfg_info->cfg_key.bit1_bayernr_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_void isp_bnr_lsc_usr_reg_config(td_bool offline_mode, isp_rlsc_usr_cfg *usr_reg_cfg,
    isp_post_be_reg_type *be_reg)
{
    td_bool usr_resh;
    td_bool idx_resh;

    isp_bnr_rlsc_rx_write(be_reg, usr_reg_cfg->center_rx);
    isp_bnr_rlsc_ry_write(be_reg, usr_reg_cfg->center_ry);
    isp_bnr_rlsc_grx_write(be_reg, usr_reg_cfg->center_gr_x);
    isp_bnr_rlsc_gry_write(be_reg, usr_reg_cfg->center_gr_y);
    isp_bnr_rlsc_gbx_write(be_reg, usr_reg_cfg->center_gb_x);
    isp_bnr_rlsc_gby_write(be_reg, usr_reg_cfg->center_gb_y);
    isp_bnr_rlsc_bx_write(be_reg, usr_reg_cfg->center_bx);
    isp_bnr_rlsc_by_write(be_reg, usr_reg_cfg->center_by);

    isp_bnr_rlsc_offsetcenterr_write(be_reg, usr_reg_cfg->off_center_r);
    isp_bnr_rlsc_offsetcentergr_write(be_reg, usr_reg_cfg->off_center_gr);
    isp_bnr_rlsc_offsetcentergb_write(be_reg, usr_reg_cfg->off_center_gb);
    isp_bnr_rlsc_offsetcenterb_write(be_reg, usr_reg_cfg->off_center_b);

    if (usr_reg_cfg->coef_update) {
        isp_bnr_rlsc_gainstr_write(be_reg, usr_reg_cfg->gain_str);

        usr_reg_cfg->coef_update = offline_mode;
    }

    idx_resh = (isp_rlsc_update_index_read(be_reg) != usr_reg_cfg->update_index);
    usr_resh = (offline_mode) ? (usr_reg_cfg->lut_update && idx_resh) : (usr_reg_cfg->lut_update);

    if (usr_resh) {
        isp_rlsc_update_index_write(be_reg, usr_reg_cfg->update_index);
        isp_bnr_rlsc_gainscale_write(be_reg, usr_reg_cfg->gain_scale);
        isp_bnr_rlsc_widthoffset_write(be_reg, usr_reg_cfg->width_offset);

        usr_reg_cfg->lut_update = offline_mode;
    }
}

static td_s32 isp_bnr_lsc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_rlsc_static_cfg *static_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_rlsc_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);
        /* static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].rlsc_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->static_resh) {
            isp_bnr_rlsc_nodenum_write(be_reg, static_reg_cfg->node_num);
            static_reg_cfg->static_resh = TD_FALSE;
        }

        isp_bnr_lsc_usr_reg_config(offline_mode, &reg_cfg_info->alg_reg_cfg[i].rlsc_reg_cfg.usr_reg_cfg, be_reg);

        reg_cfg_info->cfg_key.bit1_rlsc_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_detail_reg_config(ot_vi_pipe vi_pipe, const isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    ot_unused(vi_pipe);
    ot_unused(reg_cfg_info);
    ot_unused(i);

    return TD_SUCCESS;
}

static td_s32 isp_dg_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    isp_dg_static_cfg *static_reg_cfg = TD_NULL;
    isp_dg_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_dg_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);
        isp_dg_en_write(post_viproc, reg_cfg_info->alg_reg_cfg[i].dg_reg_cfg.dg_en);

        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dg_reg_cfg.static_reg_cfg;
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].dg_reg_cfg.dyna_reg_cfg;

        if (static_reg_cfg->resh) {
            isp_dg_rgain_write(be_reg, dyna_reg_cfg->gain_r);
            isp_dg_grgain_write(be_reg, dyna_reg_cfg->gain_gr);
            isp_dg_gbgain_write(be_reg, dyna_reg_cfg->gain_gb);
            isp_dg_bgain_write(be_reg, dyna_reg_cfg->gain_b);

            static_reg_cfg->resh = TD_FALSE;
        }

        if (dyna_reg_cfg->resh) {
            isp_dg_clip_value_write(be_reg, dyna_reg_cfg->clip_value);
            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_dg_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_4dg_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_bool offline_mode;
    isp_4dg_static_cfg *static_reg_cfg = TD_NULL;
    isp_4dg_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_wdr_dg_cfg) {
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(pre_viproc);
        isp_check_pointer_return(be_reg);
        if (is_linear_mode(isp_ctx->sns_wdr_mode)) {
            isp_4dg_en_write(pre_viproc, reg_cfg_info->alg_reg_cfg[i].dg_reg_cfg.dg_en);
        } else {
            isp_4dg_en_write(pre_viproc, TD_TRUE);
        }

        /* static */
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].four_dg_reg_cfg.static_reg_cfg;

        if (static_reg_cfg->resh) {
            isp_4dg0_rgain_write(be_reg, static_reg_cfg->gain_r0);
            isp_4dg0_grgain_write(be_reg, static_reg_cfg->gain_gr0);
            isp_4dg0_gbgain_write(be_reg, static_reg_cfg->gain_gb0);
            isp_4dg0_bgain_write(be_reg, static_reg_cfg->gain_b0);
            isp_4dg1_rgain_write(be_reg, static_reg_cfg->gain_r1);
            isp_4dg1_grgain_write(be_reg, static_reg_cfg->gain_gr1);
            isp_4dg1_gbgain_write(be_reg, static_reg_cfg->gain_gb1);
            isp_4dg1_bgain_write(be_reg, static_reg_cfg->gain_b1);
            isp_4dg2_rgain_write(be_reg, static_reg_cfg->gain_r2);
            isp_4dg2_grgain_write(be_reg, static_reg_cfg->gain_gr2);
            isp_4dg2_gbgain_write(be_reg, static_reg_cfg->gain_gb2);
            isp_4dg2_bgain_write(be_reg, static_reg_cfg->gain_b2);
            static_reg_cfg->resh = TD_FALSE;
        }

        /* dynamic */
        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].four_dg_reg_cfg.dyna_reg_cfg;

        if (dyna_reg_cfg->resh) {
            isp_4dg0_clip_value_write(be_reg, dyna_reg_cfg->clip_value0);
            isp_4dg1_clip_value_write(be_reg, dyna_reg_cfg->clip_value1);
            isp_4dg2_clip_value_write(be_reg, dyna_reg_cfg->clip_value2);

            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_wdr_dg_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_flick_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_flicker_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_pre_be_reg_type *be_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_flicker_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(pre_viproc);

        isp_flicker_en_write(pre_viproc, reg_cfg_info->alg_reg_cfg[i].flicker_reg_cfg.enable);

        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].flicker_reg_cfg.dyna_reg_cfg;

        if (dyna_reg_cfg->resh) {
            isp_flick_overth_write(be_reg, dyna_reg_cfg->over_thr);
            isp_flick_gravg_pre_write(be_reg, dyna_reg_cfg->gr_avg_pre);
            isp_flick_gbavg_pre_write(be_reg, dyna_reg_cfg->gb_avg_pre);

            dyna_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_flicker_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

#ifdef CONFIG_OT_ISP_PREGAMMA_SUPPORT
static td_s32 isp_pregamma_lut_reg_config(isp_usr_ctx *isp_ctx, isp_post_be_reg_type *be_reg,
    isp_pregamma_dyna_cfg *dyna_reg_cfg)
{
    td_u16 i;
    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        if (dyna_reg_cfg->lut_update) {
            isp_adrc_pregamma_waddr_write(be_reg, 0);
            for (i = 0; i < OT_ISP_PREGAMMA_NODE_NUM; i++) {
                isp_adrc_pregamma_wdata_write(be_reg, dyna_reg_cfg->pregamma_lut[i]);
            }
            isp_adrc_lut1_update_write(be_reg, 1);
            dyna_reg_cfg->lut_update = TD_FALSE;
        }
    } else {
        isp_adrc_pregamma_write(&be_reg->post_be_lut.post_be_apb_lut, dyna_reg_cfg->pregamma_lut);
        isp_adrc_lut1_update_write(be_reg, 1);
    }
    return TD_SUCCESS;
}

static td_void isp_pregamma_seg_reg_config(isp_post_be_reg_type *be_reg, isp_pregamma_static_cfg *static_reg_cfg)
{
    isp_adrc_pregamma_idxbase0_write(be_reg, static_reg_cfg->seg_idx_base[0]);   /* idxbase[0] */
    isp_adrc_pregamma_idxbase1_write(be_reg, static_reg_cfg->seg_idx_base[1]);   /* idxbase[1] */
    isp_adrc_pregamma_idxbase2_write(be_reg, static_reg_cfg->seg_idx_base[2]);   /* idxbase[2] */
    isp_adrc_pregamma_idxbase3_write(be_reg, static_reg_cfg->seg_idx_base[3]);   /* idxbase[3] */
    isp_adrc_pregamma_idxbase4_write(be_reg, static_reg_cfg->seg_idx_base[4]);   /* idxbase[4] */
    isp_adrc_pregamma_idxbase5_write(be_reg, static_reg_cfg->seg_idx_base[5]);   /* idxbase[5] */
    isp_adrc_pregamma_idxbase6_write(be_reg, static_reg_cfg->seg_idx_base[6]);   /* idxbase[6] */
    isp_adrc_pregamma_idxbase7_write(be_reg, static_reg_cfg->seg_idx_base[7]);   /* idxbase[7] */
    isp_adrc_pregamma_idxbase8_write(be_reg, static_reg_cfg->seg_idx_base[8]);   /* idxbase[8] */
    isp_adrc_pregamma_idxbase9_write(be_reg, static_reg_cfg->seg_idx_base[9]);   /* idxbase[9] */
    isp_adrc_pregamma_idxbase10_write(be_reg, static_reg_cfg->seg_idx_base[10]); /* idxbase[10] */
    isp_adrc_pregamma_idxbase11_write(be_reg, static_reg_cfg->seg_idx_base[11]); /* idxbase[11] */

    isp_adrc_pregamma_maxval0_write(be_reg, static_reg_cfg->seg_max_val[0]);   /* maxval[0] */
    isp_adrc_pregamma_maxval1_write(be_reg, static_reg_cfg->seg_max_val[1]);   /* maxval[1] */
    isp_adrc_pregamma_maxval2_write(be_reg, static_reg_cfg->seg_max_val[2]);   /* maxval[2] */
    isp_adrc_pregamma_maxval3_write(be_reg, static_reg_cfg->seg_max_val[3]);   /* maxval[3] */
    isp_adrc_pregamma_maxval4_write(be_reg, static_reg_cfg->seg_max_val[4]);   /* maxval[4] */
    isp_adrc_pregamma_maxval5_write(be_reg, static_reg_cfg->seg_max_val[5]);   /* maxval[5] */
    isp_adrc_pregamma_maxval6_write(be_reg, static_reg_cfg->seg_max_val[6]);   /* maxval[6] */
    isp_adrc_pregamma_maxval7_write(be_reg, static_reg_cfg->seg_max_val[7]);   /* maxval[7] */
    isp_adrc_pregamma_maxval8_write(be_reg, static_reg_cfg->seg_max_val[8]);   /* maxval[8] */
    isp_adrc_pregamma_maxval9_write(be_reg, static_reg_cfg->seg_max_val[9]);   /* maxval[9] */
    isp_adrc_pregamma_maxval10_write(be_reg, static_reg_cfg->seg_max_val[10]); /* maxval[10] */
    isp_adrc_pregamma_maxval11_write(be_reg, static_reg_cfg->seg_max_val[11]); /* maxval[11] */
}
#endif

static td_void isp_pregamma_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_pregamma_dyna_cfg *dyna_reg_cfg, td_bool *stt2_lut_regnew)
{
    td_u32 pregamma_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        pregamma_stt2lut_clr = isp_pregamma_stt2lut_clr_read(be_reg);
        if (pregamma_stt2lut_clr != 0) {
            dyna_reg_cfg->lut_update = TD_TRUE;
            *stt2_lut_regnew = TD_TRUE;
            isp_pregamma_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_pregamma_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
#ifdef CONFIG_OT_ISP_PREGAMMA_SUPPORT
    td_bool offline_mode;
    td_bool usr_resh;
    td_bool idx_resh;
    td_bool stt2_lut_regnew = TD_FALSE;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    td_s32 ret;
    isp_pregamma_dyna_cfg *dyna_reg_cfg = TD_NULL;
    isp_pregamma_static_cfg *static_reg_cfg = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_pregamma_cfg) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        dyna_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].pregamma_reg_cfg.dyna_reg_cfg;
        static_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].pregamma_reg_cfg.static_reg_cfg;

        /* enable pregamma */
        if (static_reg_cfg->resh) {
            isp_pregamma_seg_reg_config(be_reg, static_reg_cfg);
            static_reg_cfg->resh = TD_FALSE;
        }

        idx_resh = (isp_pregamma_update_index_read(be_reg) != dyna_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (dyna_reg_cfg->lut_update && idx_resh) : (dyna_reg_cfg->lut_update);

        /* LUT update */
        if (usr_resh) {
            isp_pregamma_update_index_write(be_reg, dyna_reg_cfg->update_index);
            ret = isp_pregamma_lut_reg_config(isp_ctx, be_reg, dyna_reg_cfg);
            if (ret != TD_SUCCESS) {
                return ret;
            }
        }
        isp_pregamma_reg_update_online(offline_mode, be_reg, dyna_reg_cfg, &stt2_lut_regnew);
        reg_cfg_info->cfg_key.bit1_pregamma_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }
    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.pre_gamma_stt2lut_regnew = stt2_lut_regnew;

#endif
    return TD_SUCCESS;
}

static td_void isp_pre_be_blc_static_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_be_blc_static_cfg *static_cfg)
{
    isp_pre_be_reg_type *be_reg = TD_NULL;

    be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(be_reg);

    /* 4Dg */
    isp_4dg_en_in_write(be_reg, static_cfg->wdr_dg_blc[0].blc_in);
    isp_4dg_en_out_write(be_reg, static_cfg->wdr_dg_blc[0].blc_out);

    /* ge */
    isp_ge0_blc_offset_en_write(be_reg, static_cfg->wdr_dg_blc[0].blc_in);
    isp_ge1_blc_offset_en_write(be_reg, static_cfg->wdr_dg_blc[1].blc_in);
    isp_ge1_ge0_blc_offset_en_write(be_reg, static_cfg->wdr_dg_blc[2].blc_in); /* index 2 */
    isp_ge1_ge1_blc_offset_en_write(be_reg, static_cfg->wdr_dg_blc[3].blc_in); /* index 3 */

    /* WDR */
    isp_wdr_bsaveblc_write(be_reg, static_cfg->wdr_blc[0].blc_out);

    /* Rgbir */
    isp_rgbir_blc_in_en_write(be_reg, static_cfg->rgbir_blc.blc_in);
    isp_rgbir_blc_out_en_write(be_reg, static_cfg->rgbir_blc.blc_out);
}

static td_void isp_post_be_blc_static_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_be_blc_static_cfg *static_cfg)
{
    isp_post_be_reg_type *be_reg = TD_NULL;

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(be_reg);

    /* lsc */
    isp_lsc_blc_in_en_write(be_reg, static_cfg->lsc_blc.blc_in);
    isp_lsc_blc_out_en_write(be_reg, static_cfg->lsc_blc.blc_out);

    /* Dg */
    isp_dg_en_in_write(be_reg, static_cfg->dg_blc.blc_in);
    isp_dg_en_out_write(be_reg, static_cfg->dg_blc.blc_out);

    /* AE */
    isp_post_be_ae_blc_en_write(be_reg, static_cfg->ae_blc.blc_in);

    /* MG */
    isp_la_blc_en_write(be_reg, static_cfg->mg_blc.blc_in);

    /* WB */
    isp_wb_en_in_write(be_reg, static_cfg->wb_blc.blc_in);
    isp_wb_en_out_write(be_reg, static_cfg->wb_blc.blc_out);
    /* AF */
    isp_af_offset_en_write(be_reg, static_cfg->af_blc.blc_in);
}

static td_void isp_4dg_blc_dyna_reg_config(isp_pre_be_reg_type *be_reg, isp_be_blc_dyna_cfg *dyna_cfg)
{
    isp_4dg0_ofsr_write(be_reg, dyna_cfg->wdr_dg_blc[0].blc[0]);  /* index 0 ,0 */
    isp_4dg0_ofsgr_write(be_reg, dyna_cfg->wdr_dg_blc[0].blc[1]); /* index 0 ,1 */
    isp_4dg0_ofsgb_write(be_reg, dyna_cfg->wdr_dg_blc[0].blc[2]); /* index 0 ,2 */
    isp_4dg0_ofsb_write(be_reg, dyna_cfg->wdr_dg_blc[0].blc[3]);  /* index 0 ,3 */

    isp_4dg1_ofsr_write(be_reg, dyna_cfg->wdr_dg_blc[1].blc[0]);  /* index 1 ,0 */
    isp_4dg1_ofsgr_write(be_reg, dyna_cfg->wdr_dg_blc[1].blc[1]); /* index 1 ,1 */
    isp_4dg1_ofsgb_write(be_reg, dyna_cfg->wdr_dg_blc[1].blc[2]); /* index 1 ,2 */
    isp_4dg1_ofsb_write(be_reg, dyna_cfg->wdr_dg_blc[1].blc[3]);  /* index 1 ,3 */

    isp_4dg2_ofsr_write(be_reg, dyna_cfg->wdr_dg_blc[2].blc[0]);  /* index 2 ,0 */
    isp_4dg2_ofsgr_write(be_reg, dyna_cfg->wdr_dg_blc[2].blc[1]); /* index 2 ,1 */
    isp_4dg2_ofsgb_write(be_reg, dyna_cfg->wdr_dg_blc[2].blc[2]); /* index 2 ,2 */
    isp_4dg2_ofsb_write(be_reg, dyna_cfg->wdr_dg_blc[2].blc[3]);  /* index 2 ,3 */

    isp_4dg3_ofsr_write(be_reg, dyna_cfg->wdr_dg_blc[3].blc[0]);  /* index 3 ,0 */
    isp_4dg3_ofsgr_write(be_reg, dyna_cfg->wdr_dg_blc[3].blc[1]); /* index 3 ,1 */
    isp_4dg3_ofsgb_write(be_reg, dyna_cfg->wdr_dg_blc[3].blc[2]); /* index 3 ,2 */
    isp_4dg3_ofsb_write(be_reg, dyna_cfg->wdr_dg_blc[3].blc[3]);  /* index 3 ,3 */
}

static td_void isp_ge_blc_dyna_reg_config(isp_pre_be_reg_type *be_reg, isp_be_blc_dyna_cfg *dyna_cfg)
{
    isp_ge0_blc_offset_gr_write(be_reg, dyna_cfg->ge_blc[0].blc[1]);     /* index 0 ,1 */
    isp_ge0_blc_offset_gb_write(be_reg, dyna_cfg->ge_blc[0].blc[2]);     /* index 0 ,2 */
    isp_ge1_blc_offset_gr_write(be_reg, dyna_cfg->ge_blc[1].blc[1]);     /* index 1 ,1 */
    isp_ge1_blc_offset_gb_write(be_reg, dyna_cfg->ge_blc[1].blc[2]);     /* index 1 ,2 */
    isp_ge1_ge0_blc_offset_gr_write(be_reg, dyna_cfg->ge_blc[2].blc[1]); /* index 2 ,1 */
    isp_ge1_ge0_blc_offset_gb_write(be_reg, dyna_cfg->ge_blc[2].blc[2]); /* index 2 ,2 */
    isp_ge1_ge1_blc_offset_gr_write(be_reg, dyna_cfg->ge_blc[3].blc[1]); /* index 3 ,1 */
    isp_ge1_ge1_blc_offset_gb_write(be_reg, dyna_cfg->ge_blc[3].blc[2]); /* index 3 ,2 */
}

static td_void isp_wdr_blc_dyna_reg_config(isp_pre_be_reg_type *be_reg, isp_be_blc_dyna_cfg *dyna_cfg)
{
    isp_wdr_outblc_write(be_reg, dyna_cfg->wdr_blc[0].out_blc);
    isp_wdr_f0_inblc_r_write(be_reg, dyna_cfg->wdr_blc[0].blc[0]);  /* index 0 ,0 */
    isp_wdr_f0_inblc_gr_write(be_reg, dyna_cfg->wdr_blc[0].blc[1]); /* index 0 ,1 */
    isp_wdr_f0_inblc_gb_write(be_reg, dyna_cfg->wdr_blc[0].blc[2]); /* index 0 ,2 */
    isp_wdr_f0_inblc_b_write(be_reg, dyna_cfg->wdr_blc[0].blc[3]);  /* index 0 ,3 */

    isp_wdr_f1_inblc_r_write(be_reg, dyna_cfg->wdr_blc[1].blc[0]);  /* index 1 ,0 */
    isp_wdr_f1_inblc_gr_write(be_reg, dyna_cfg->wdr_blc[1].blc[1]); /* index 1 ,1 */
    isp_wdr_f1_inblc_gb_write(be_reg, dyna_cfg->wdr_blc[1].blc[2]); /* index 1 ,2 */
    isp_wdr_f1_inblc_b_write(be_reg, dyna_cfg->wdr_blc[1].blc[3]);  /* index 1 ,3 */

    isp_wdr_f2_inblc_r_write(be_reg, dyna_cfg->wdr_blc[2].blc[0]);  /* index 2 ,0 */
    isp_wdr_f2_inblc_gr_write(be_reg, dyna_cfg->wdr_blc[2].blc[1]); /* index 2 ,1 */
    isp_wdr_f2_inblc_gb_write(be_reg, dyna_cfg->wdr_blc[2].blc[2]); /* index 2 ,2 */
    isp_wdr_f2_inblc_b_write(be_reg, dyna_cfg->wdr_blc[2].blc[3]);  /* index 2 ,3 */

    isp_wdr_f3_inblc_r_write(be_reg, dyna_cfg->wdr_blc[3].blc[0]);  /* index 3 ,0 */
    isp_wdr_f3_inblc_gr_write(be_reg, dyna_cfg->wdr_blc[3].blc[1]); /* index 3 ,1 */
    isp_wdr_f3_inblc_gb_write(be_reg, dyna_cfg->wdr_blc[3].blc[2]); /* index 3 ,2 */
    isp_wdr_f3_inblc_b_write(be_reg, dyna_cfg->wdr_blc[3].blc[3]);  /* index 3 ,3 */
}

static td_void isp_flicker_blc_dyna_reg_config(isp_pre_be_reg_type *be_reg, isp_be_blc_dyna_cfg *dyna_cfg)
{
    isp_flick_f0_inblc_r_write(be_reg, dyna_cfg->flicker_blc[0].blc[0]);  /* index 0 ,0 */
    isp_flick_f0_inblc_gr_write(be_reg, dyna_cfg->flicker_blc[0].blc[1]); /* index 0 ,1 */
    isp_flick_f0_inblc_gb_write(be_reg, dyna_cfg->flicker_blc[0].blc[2]); /* index 0 ,2 */
    isp_flick_f0_inblc_b_write(be_reg, dyna_cfg->flicker_blc[0].blc[3]);  /* index 0 ,3 */

    isp_flick_f1_inblc_r_write(be_reg, dyna_cfg->flicker_blc[1].blc[0]);  /* index 1 ,0 */
    isp_flick_f1_inblc_gr_write(be_reg, dyna_cfg->flicker_blc[1].blc[1]); /* index 1 ,1 */
    isp_flick_f1_inblc_gb_write(be_reg, dyna_cfg->flicker_blc[1].blc[2]); /* index 1 ,2 */
    isp_flick_f1_inblc_b_write(be_reg, dyna_cfg->flicker_blc[1].blc[3]);  /* index 1 ,3 */

    isp_flick_f2_inblc_r_write(be_reg, dyna_cfg->flicker_blc[2].blc[0]);  /* index 2 ,0 */
    isp_flick_f2_inblc_gr_write(be_reg, dyna_cfg->flicker_blc[2].blc[1]); /* index 2 ,1 */
    isp_flick_f2_inblc_gb_write(be_reg, dyna_cfg->flicker_blc[2].blc[2]); /* index 2 ,2 */
    isp_flick_f2_inblc_b_write(be_reg, dyna_cfg->flicker_blc[2].blc[3]);  /* index 2 ,3 */

    isp_flick_f3_inblc_r_write(be_reg, dyna_cfg->flicker_blc[3].blc[0]);  /* index 3 ,0 */
    isp_flick_f3_inblc_gr_write(be_reg, dyna_cfg->flicker_blc[3].blc[1]); /* index 3 ,1 */
    isp_flick_f3_inblc_gb_write(be_reg, dyna_cfg->flicker_blc[3].blc[2]); /* index 3 ,2 */
    isp_flick_f3_inblc_b_write(be_reg, dyna_cfg->flicker_blc[3].blc[3]);  /* index 3 ,3 */
}

static td_void isp_pre_be_blc_dyna_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_be_blc_dyna_cfg *dyna_cfg)
{
    isp_pre_be_reg_type *be_reg = TD_NULL;

    be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(be_reg);

    isp_4dg_blc_dyna_reg_config(be_reg, dyna_cfg);     /* 4Dg */
    isp_ge_blc_dyna_reg_config(be_reg, dyna_cfg);      /* ge */
    isp_wdr_blc_dyna_reg_config(be_reg, dyna_cfg);     /* WDR */
    isp_flicker_blc_dyna_reg_config(be_reg, dyna_cfg); /* flicker */

    /* expander */
    isp_expander_offset_r_write(be_reg, dyna_cfg->expander_blc.blc[0]);  /* array index 0 */
    isp_expander_offset_gr_write(be_reg, dyna_cfg->expander_blc.blc[1]); /* array index 1 */
    isp_expander_offset_gb_write(be_reg, dyna_cfg->expander_blc.blc[2]); /* array index 2 */
    isp_expander_offset_b_write(be_reg, dyna_cfg->expander_blc.blc[3]);  /* array index 3 */

    /* Rgbir */
    isp_rgbir_blc_offset_r_write(be_reg, dyna_cfg->rgbir_blc.blc[0]);  /* array index 0 */
    isp_rgbir_blc_offset_g_write(be_reg, dyna_cfg->rgbir_blc.blc[1]);  /* array index 1 */
    isp_rgbir_blc_offset_b_write(be_reg, dyna_cfg->rgbir_blc.blc[2]);  /* array index 2 */
    isp_rgbir_blc_offset_ir_write(be_reg, dyna_cfg->rgbir_blc.blc[3]); /* array index 3 */
}

static td_void isp_post_be_blc_dyna_reg_config(ot_vi_pipe vi_pipe, td_u8 i, isp_be_blc_dyna_cfg *dyna_cfg)
{
    isp_post_be_reg_type *be_reg = TD_NULL;

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(be_reg);

    /* bnr */
    isp_bnr_blc_write(be_reg, (dyna_cfg->bnr_blc.blc[0] >> 2)); /* index 0  shift 2 bit */

    /* lsc */
    isp_lsc_blc_r_write(be_reg, dyna_cfg->lsc_blc.blc[0]);  /* array index 0 */
    isp_lsc_blc_gr_write(be_reg, dyna_cfg->lsc_blc.blc[1]); /* array index 1 */
    isp_lsc_blc_gb_write(be_reg, dyna_cfg->lsc_blc.blc[2]); /* array index 2 */
    isp_lsc_blc_b_write(be_reg, dyna_cfg->lsc_blc.blc[3]);  /* array index 3 */

    /* Dg */
    isp_dg_ofsr_write(be_reg, dyna_cfg->dg_blc.blc[0]);  /* array index 0 */
    isp_dg_ofsgr_write(be_reg, dyna_cfg->dg_blc.blc[1]); /* array index 1 */
    isp_dg_ofsgb_write(be_reg, dyna_cfg->dg_blc.blc[2]); /* array index 2 */
    isp_dg_ofsb_write(be_reg, dyna_cfg->dg_blc.blc[3]);  /* array index 3 */

    /* AE */
    isp_post_be_ae_offset_r_write(be_reg, dyna_cfg->ae_blc.blc[0]);  /* array index 0 */
    isp_post_be_ae_offset_gr_write(be_reg, dyna_cfg->ae_blc.blc[1]); /* array index 1 */
    isp_post_be_ae_offset_gb_write(be_reg, dyna_cfg->ae_blc.blc[2]); /* array index 2 */
    isp_post_be_ae_offset_b_write(be_reg, dyna_cfg->ae_blc.blc[3]);  /* array index 3 */
    /* MG */
    isp_la_offset_r_write(be_reg, dyna_cfg->mg_blc.blc[0]);  /* array index 0 */
    isp_la_offset_gr_write(be_reg, dyna_cfg->mg_blc.blc[1]); /* array index 1 */
    isp_la_offset_gb_write(be_reg, dyna_cfg->mg_blc.blc[2]); /* array index 2 */
    isp_la_offset_b_write(be_reg, dyna_cfg->mg_blc.blc[3]);  /* array index 3 */
    /* WB */
    isp_wb_ofsr_write(be_reg, dyna_cfg->wb_blc.blc[0]);  /* array index 0 */
    isp_wb_ofsgr_write(be_reg, dyna_cfg->wb_blc.blc[1]); /* array index 1 */
    isp_wb_ofsgb_write(be_reg, dyna_cfg->wb_blc.blc[2]); /* array index 2 */
    isp_wb_ofsb_write(be_reg, dyna_cfg->wb_blc.blc[3]);  /* array index 3 */
    /* AF */
    isp_af_offset_gr_write(be_reg, dyna_cfg->af_blc.blc[OT_ISP_CHN_GR]);
    isp_af_offset_gb_write(be_reg, dyna_cfg->af_blc.blc[OT_ISP_CHN_GB]);
}

static td_s32 isp_be_blc_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_be_blc_static_cfg *static_cfg = TD_NULL;
    isp_be_blc_dyna_cfg *dyna_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_be_blc_cfg) {
        static_cfg = &reg_cfg_info->alg_reg_cfg[i].be_blc_cfg.static_blc;
        dyna_cfg = &reg_cfg_info->alg_reg_cfg[i].be_blc_cfg.dyna_blc;

        if (static_cfg->resh_static) {
            isp_pre_be_blc_static_reg_config(vi_pipe, i, static_cfg);
            isp_post_be_blc_static_reg_config(vi_pipe, i, static_cfg);
            static_cfg->resh_static = offline_mode;
        }

        if (reg_cfg_info->alg_reg_cfg[i].be_blc_cfg.resh_dyna_init == TD_TRUE) {
            isp_pre_be_blc_dyna_reg_config(vi_pipe, i, dyna_cfg);
            isp_post_be_blc_dyna_reg_config(vi_pipe, i, dyna_cfg);
            reg_cfg_info->alg_reg_cfg[i].be_blc_cfg.resh_dyna_init = TD_FALSE;
        }

        reg_cfg_info->cfg_key.bit1_be_blc_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_void isp_clut_reg_update_online(td_bool offline_mode, isp_post_be_reg_type *be_reg,
    isp_clut_usr_coef_cfg *clut_usr_coef_cfg, td_bool *stt2_lut_regnew)
{
    td_u32 clut_stt2lut_clr;

    if (offline_mode == TD_FALSE) {
        clut_stt2lut_clr = isp_clut_stt2lut_clr_read(be_reg);
        if (clut_stt2lut_clr != 0) {
            clut_usr_coef_cfg->resh = TD_TRUE;
            *stt2_lut_regnew = TD_TRUE;
            isp_clut_stt2lut_clr_write(be_reg, 1);
        }
    }
}

static td_s32 isp_clut_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode, usr_resh, idx_resh;
    td_bool stt2_lut_regnew = TD_FALSE;
    td_u8 buf_id;
    td_u8 blk_num = reg_cfg_info->cfg_num;
    isp_clut_usr_coef_cfg *clut_usr_coef_cfg = TD_NULL;
    isp_be_lut_wstt_type *be_lut_stt_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode =
        (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode));

    if ((reg_cfg_info->cfg_key.bit1_clut_cfg)) {
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);
        isp_check_pointer_return(post_viproc);

        isp_clut_sel_write(be_reg, OT_ISP_CLUT_SEL_WRITE);
        clut_usr_coef_cfg = &reg_cfg_info->alg_reg_cfg[i].clut_cfg.usr_reg_cfg.clut_usr_coef_cfg;

        idx_resh = (isp_clut_update_index_read(be_reg) != clut_usr_coef_cfg->update_index);
        usr_resh = (offline_mode) ? (clut_usr_coef_cfg->resh && idx_resh) : (clut_usr_coef_cfg->resh);

        if (usr_resh) {
            isp_clut_update_index_write(be_reg, clut_usr_coef_cfg->update_index);

            if (is_online_mode(isp_ctx->block_attr.running_mode)) {
                /* online lut2stt regconfig */
                buf_id = clut_usr_coef_cfg->buf_id;

                be_lut_stt_reg = (isp_be_lut_wstt_type *)isp_get_be_lut2stt_vir_addr(vi_pipe, i, buf_id);
                isp_check_pointer_return(be_lut_stt_reg);

                isp_clut_lut_wstt_write(&be_lut_stt_reg->post_be_lut2stt, clut_usr_coef_cfg->lu_stt);

                isp_clut_lut_wstt_addr_write(vi_pipe, i, buf_id, post_viproc);
                isp_clut_stt2lut_en_write(be_reg, TD_TRUE);

                clut_usr_coef_cfg->buf_id = 1 - buf_id;
                stt2_lut_regnew = TD_TRUE;
            } else {
                isp_clut_lut_wstt_write(&be_reg->post_be_lut.post_be_lut2stt, clut_usr_coef_cfg->lu_stt);
                isp_clut_lut_width_word_write(post_viproc, OT_ISP_CLUT_LUT_WIDTH_WORD_DEFAULT);
                isp_clut_stt2lut_en_write(be_reg, TD_TRUE);
                isp_clut_stt2lut_regnew_write(be_reg, TD_TRUE);
            }

            clut_usr_coef_cfg->resh = offline_mode;
        }

        isp_clut_reg_update_online(offline_mode, be_reg, clut_usr_coef_cfg, &stt2_lut_regnew);
        reg_cfg_info->cfg_key.bit1_clut_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg.clut_stt2lut_regnew = stt2_lut_regnew;

    return TD_SUCCESS;
}

static td_void isp_expander_lut_reg_config(const isp_usr_ctx *isp_ctx, isp_pre_be_reg_type *be_reg,
    isp_expander_usr_cfg *usr_reg_cfg)
{
    td_u16 j;

    if (is_online_mode(isp_ctx->block_attr.running_mode) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        isp_expander_lut_waddr_write(be_reg, 0);

        for (j = 0; j < OT_ISP_EXPANDER_NODE_NUM; j++) {
            isp_expander_lut_wdata_write(be_reg, usr_reg_cfg->lut[j]);
        }
    } else {
        isp_expander_lut_write(&be_reg->pre_be_lut.pre_be_apb_lut, usr_reg_cfg->lut);
    }
}

static td_s32 isp_expander_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_bool offline_mode;
    td_bool usr_resh, idx_resh;
    td_u8 blk_num = reg_cfg_info->cfg_num;

    isp_expander_usr_cfg *usr_reg_cfg = TD_NULL;
    isp_pre_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    offline_mode = (is_offline_mode(isp_ctx->block_attr.running_mode) ||
                    is_striping_mode(isp_ctx->block_attr.running_mode));

    if (reg_cfg_info->cfg_key.bit1_expander_cfg) {
        be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        isp_check_pointer_return(be_reg);

        usr_reg_cfg = &reg_cfg_info->alg_reg_cfg[i].expander_cfg.usr_cfg;

        idx_resh = (isp_expander_update_index_read(be_reg) != usr_reg_cfg->update_index);
        usr_resh = (offline_mode) ? (usr_reg_cfg->resh && idx_resh) : (usr_reg_cfg->resh);

        if (usr_resh) {
            isp_expander_update_index_write(be_reg, usr_reg_cfg->update_index);
            isp_expander_bitw_out_write(be_reg, usr_reg_cfg->bit_depth_out);
            isp_expander_bitw_in_write(be_reg, usr_reg_cfg->bit_depth_in);

            isp_expander_lut_reg_config(isp_ctx, be_reg, usr_reg_cfg);

            usr_reg_cfg->resh = offline_mode;
        }

        reg_cfg_info->cfg_key.bit1_expander_cfg = offline_mode ? 1 : ((blk_num <= (i + 1)) ? 0 : 1);
    }

    return TD_SUCCESS;
}

static td_s32 isp_fe_update_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u32 i;
    ot_vi_pipe vi_pipe_bind;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
        isp_check_vir_pipe_return(vi_pipe_bind);
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);

        isp_fe_update_mode_write(fe_reg, TD_FALSE);
        isp_fe_update_write(fe_reg, TD_TRUE);

        if (reg_cfg_info->alg_reg_cfg[0].fe_lut_update_cfg.ae1_lut_update) {
            isp_fe_ae_lut_update_write(fe_reg, reg_cfg_info->alg_reg_cfg[0].fe_lut_update_cfg.ae1_lut_update);
        }
    }

    return TD_SUCCESS;
}

static td_void isp_fe_size_reg_config(const isp_usr_ctx *isp_ctx, isp_fe_reg_type *fe_reg)
{
    td_bool isp_crop_en;
    td_s32 x, y;
    td_u32 width, height, pipe_w, pipe_h;
    td_u32 merge_frame;

    x = isp_ctx->sys_rect.x;
    y = isp_ctx->sys_rect.y;
    width = isp_ctx->sys_rect.width;
    height = isp_ctx->sys_rect.height;
    pipe_w = isp_ctx->pipe_size.width;
    pipe_h = isp_ctx->pipe_size.height;

    /* ISP crop low-power process */
    if ((x == 0) && (y == 0) && (width == pipe_w) && (height == pipe_h)) {
        isp_crop_en = TD_FALSE;
    } else {
        isp_crop_en = TD_TRUE;
    }

    isp_fe_crop_pos_write(fe_reg, x, y);
    isp_fe_crop_size_out_write(fe_reg, width - 1, height - 1);

    if (isp_ctx->stagger_attr.stagger_en == TD_TRUE) {
        if (isp_ctx->stagger_attr.crop_info.enable == TD_TRUE) {
            isp_fe_crop_en_write(fe_reg, TD_TRUE);
            isp_fe_crop_pos_write(fe_reg, isp_ctx->stagger_attr.crop_info.rect.x,
                                  isp_ctx->stagger_attr.crop_info.rect.y);
            isp_fe_crop_size_out_write(fe_reg, isp_ctx->stagger_attr.crop_info.rect.width - 1,
                                       isp_ctx->stagger_attr.crop_info.rect.height - 1);
        } else {
            isp_fe_crop_en_write(fe_reg, TD_FALSE);
        }
        merge_frame = div_0_to_1(isp_ctx->stagger_attr.merge_frame_num);
        isp_mul_u32_limit(width, pipe_w, merge_frame);
        height = (pipe_h + (merge_frame - 1)) / div_0_to_1(merge_frame);
        isp_fe_size_write(fe_reg, width - 1, height - 1);
        isp_fe_blk_size_write(fe_reg, width - 1, height - 1);
    } else {
        isp_fe_crop_en_write(fe_reg, isp_crop_en);
        isp_fe_size_write(fe_reg, pipe_w - 1, pipe_h - 1);
        isp_fe_blk_size_write(fe_reg, pipe_w - 1, pipe_h - 1);
    }

    isp_fe_delay_write(fe_reg, height >> 1); /* set fe delay interrup trigger threshold */
}

static td_s32 isp_fe_system_reg_config(ot_vi_pipe vi_pipe)
{
    td_u8 rggb_cfg;
    td_u32 i;
    ot_vi_pipe vi_pipe_bind;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;
    td_bool rgbir_en;

    isp_get_ctx(vi_pipe, isp_ctx);

    rggb_cfg = ot_ext_system_rggb_cfg_read(vi_pipe);
    rgbir_en = ot_ext_system_rgbir_enable_read(vi_pipe);

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
        isp_check_vir_pipe_return(vi_pipe_bind);
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);

        /* ISP FE/BE Set Offline Mode */
        /* isp regs uptate mode:   0: update; 1:frame */
        isp_fe_rggb_cfg_write(fe_reg, rggb_cfg);
        if (rgbir_en == TD_TRUE) {
            td_u8 rgbir_cfg = ot_ext_system_rgbir_inpattern_read(vi_pipe);
            isp_fe_rgbir_cfg_write(fe_reg, 4 + rgbir_cfg); /* 4 is RGBIR type offset */
        } else {
            isp_fe_rgbir_cfg_write(fe_reg, rggb_cfg);
        }
        isp_fe_fix_timing_write(fe_reg, OT_ISP_FE_FIX_TIMING_STAT);
        isp_fe_blk_f_hblank_write(fe_reg, 0);
        isp_fe_hsync_mode_write(fe_reg, 0);
        isp_fe_vsync_mode_write(fe_reg, 0);

        isp_fe_size_reg_config(isp_ctx, fe_reg);
    }

    return TD_SUCCESS;
}

static td_s32 isp_reg_default(ot_vi_pipe vi_pipe, const isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    ot_unused(reg_cfg_info);

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_be_reg);
    isp_check_pointer_return(pre_be_reg);
    isp_check_pointer_return(post_viproc);

    /* pre be */
    isp_pre_be_clip_y_min_write(pre_be_reg, ISP_CLIP_Y_MIN_DEFAULT);
    isp_pre_be_clip_y_max_write(pre_be_reg, ISP_CLIP_Y_MAX_DEFAULT);
    isp_pre_be_clip_c_min_write(pre_be_reg, ISP_CLIP_C_MIN_DEFAULT);
    isp_pre_be_clip_c_max_write(pre_be_reg, ISP_CLIP_C_MAX_DEFAULT);
    isp_wdr_sum_en_write(pre_be_reg, ISP_WDR_SUM_EN_DEFAULT);
    isp_pre_be_blk_f_hblank_write(pre_be_reg, OT_ISP_BLK_F_HBLANK_DEFAULT);
    isp_pre_be_blk_f_vblank_write(pre_be_reg, OT_ISP_BLK_F_VBLANK_DEFAULT);
    isp_pre_be_blk_b_hblank_write(pre_be_reg, OT_ISP_BLK_B_HBLANK_DEFAULT);
    isp_pre_be_blk_b_vblank_write(pre_be_reg, OT_ISP_BLK_B_VBLANK_DEFAULT);

    /* post be */
    isp_post_be_clip_y_min_write(post_be_reg, ISP_CLIP_Y_MIN_DEFAULT);
    isp_post_be_clip_y_max_write(post_be_reg, ISP_CLIP_Y_MAX_DEFAULT);
    isp_post_be_clip_c_min_write(post_be_reg, ISP_CLIP_C_MIN_DEFAULT);
    isp_post_be_clip_c_max_write(post_be_reg, ISP_CLIP_C_MAX_DEFAULT);
    isp_csc_sum_en_write(post_be_reg, ISP_CSC_SUM_EN_DEFAULT);
    isp_yuv444_sum_en_write(post_be_reg, ISP_YUV444_SUM_EN_DEFAULT);
    isp_yuv422_sum_en_write(post_be_reg, ISP_YUV422_SUM_EN_DEFAULT);
    isp_demosaic_sum_en_write(post_be_reg, ISP_DEMOSAIC_SUM_EN_DEFAULT);
    isp_post_be_blk_f_hblank_write(post_be_reg, OT_ISP_BLK_F_HBLANK_DEFAULT);
    isp_post_be_blk_f_vblank_write(post_be_reg, OT_ISP_BLK_F_VBLANK_DEFAULT);
    isp_post_be_blk_b_hblank_write(post_be_reg, OT_ISP_BLK_B_HBLANK_DEFAULT);
    isp_post_be_blk_b_vblank_write(post_be_reg, OT_ISP_BLK_B_VBLANK_DEFAULT);

    isp_blc_en_write(post_viproc, TD_FALSE);
    isp_split_en_write(post_viproc, TD_FALSE);

    return TD_SUCCESS;
}

static td_s32 isp_system_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u32 rggb_cfg;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    td_bool rgbir_en = ot_ext_system_rgbir_enable_read(vi_pipe);

    isp_get_ctx(vi_pipe, isp_ctx);

    rggb_cfg = ot_ext_system_rggb_cfg_read(vi_pipe);

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_be_reg);
    isp_check_pointer_return(pre_be_reg);
    isp_check_pointer_return(post_viproc);
    isp_check_pointer_return(pre_viproc);

    isp_be_rggb_cfg_write(post_viproc, rggb_cfg);
    isp_be_rggb_cfg_write(pre_viproc, rggb_cfg);

    if (rgbir_en == TD_TRUE) {
        td_u8 rgbir_cfg = ot_ext_system_rgbir_inpattern_read(vi_pipe);
        isp_be_rgbir_cfg_write(post_viproc, 4 + rgbir_cfg); /* 4 is RGBIR type offset */
        isp_be_rgbir_cfg_write(pre_viproc, 4 + rgbir_cfg);  /* 4 is RGBIR type offset */
    } else {
        isp_be_rgbir_cfg_write(post_viproc, rggb_cfg);
        isp_be_rgbir_cfg_write(pre_viproc, rggb_cfg);
    }

    /* should cfg: isp_be_rgbir_cfg_write(pre_viproc, 0) */
    if ((is_offline_mode(isp_ctx->block_attr.running_mode)) || (is_striping_mode(isp_ctx->block_attr.running_mode))) {
        isp_post_be_stt_en_write(post_be_reg, TD_TRUE);
        isp_pre_be_stt_en_write(pre_be_reg, TD_TRUE);
    } else if (is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        isp_post_be_stt_en_write(post_be_reg, TD_TRUE);
        isp_pre_be_stt_en_write(pre_be_reg, TD_FALSE);
    } else {
        isp_post_be_stt_en_write(post_be_reg, TD_FALSE);
        isp_pre_be_stt_en_write(pre_be_reg, TD_FALSE);
    }

    isp_sumy_en_write(pre_viproc, TD_TRUE);

    isp_sumy_en_write(post_viproc, TD_TRUE);

    return TD_SUCCESS;
}

static td_s32 isp_dither_reg_config(ot_vi_pipe vi_pipe, const isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_return(be_reg);
#ifdef ISP_BINARY_CONSISTENCY
    isp_drc_dither_en_write(be_reg, TD_FALSE);
    isp_dmnr_dither_en_write(be_reg, TD_FALSE);
    isp_acm_dither_en_write(be_reg, TD_FALSE);
    isp_sharpen_dither_en_write(be_reg, TD_FALSE);
#else
    /* after drc module */
    if (reg_cfg_info->alg_reg_cfg[i].drc_reg_cfg.enable == TD_TRUE) {
        isp_drc_dither_en_write(be_reg, TD_FALSE);
    } else {
        isp_drc_dither_en_write(be_reg, !(isp_ctx->hdr_attr.dynamic_range == OT_DYNAMIC_RANGE_XDR));
    }
    isp_drc_dither_out_bits_write(be_reg, ISP_DRC_DITHER_OUT_BITS_DEFAULT);
    isp_drc_dither_round_write(be_reg, ISP_DRC_DITHER_ROUND_DEFAULT);
    isp_drc_dither_spatial_mode_write(be_reg, ISP_DRC_DITHER_SPATIAL_MODE_DEFAULT);

    /* after gamma module */
    isp_dmnr_dither_en_write(be_reg, TD_TRUE);
    isp_dmnr_dither_out_bits_write(be_reg, ISP_DMNR_DITHER_OUT_BITS_DEFAULT);
    isp_dmnr_dither_round_write(be_reg, ISP_DMNR_DITHER_ROUND_DEFAULT);
    isp_dmnr_dither_spatial_mode_write(be_reg, ISP_DMNR_DITHER_SPATIAL_MODE_DEFAULT);

    /* after CA module */
    isp_acm_dither_en_write(be_reg, TD_TRUE);
    isp_acm_dither_out_bits_write(be_reg, ISP_ACM_DITHER_OUT_BITS_DEFAULT);
    isp_acm_dither_round_write(be_reg, ISP_ACM_DITHER_ROUND_DEFAULT);
    isp_acm_dither_spatial_mode_write(be_reg, ISP_ACM_DITHER_SPATIAL_MODE_DEFAULT);

    /* after sharpen module */
    isp_sharpen_dither_en_write(be_reg, TD_TRUE);
    isp_sharpen_dither_out_bits_write(be_reg, ISP_SHARPEN_DITHER_OUT_BITS_DEFAULT);
    isp_sharpen_dither_round_write(be_reg, ISP_SHARPEN_DITHER_ROUND_DEFAULT);
    isp_sharpen_dither_spatial_mode_write(be_reg, ISP_SHARPEN_DITHER_SPATIAL_MODE_DEFAULT);
#endif

    isp_sqrt1_dither_en_write(be_reg, TD_FALSE);

    return TD_SUCCESS;
}

static td_s32 isp_fe_stt_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u8 af_enbale[OT_ISP_MAX_PHY_PIPE_NUM] = { ISP_PIPE_FEAF_MODULE_ENABLE };
    td_u16 num_h, num_v;
    td_u32 k;
    td_bool stt_enable = TD_TRUE;
    ot_vi_pipe vi_pipe_bind;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    for (k = 0; k < isp_ctx->wdr_attr.pipe_num; k++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[k];
        isp_check_vir_pipe_return(vi_pipe_bind);
        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_return(fe_reg);

        if (isp_ctx->special_opt.fe_stt_update) {
            isp_fe_ae_stt_en_write(fe_reg, stt_enable);
            isp_fe_ae_stt_bst_write(fe_reg, 0xF);
            if (af_enbale[vi_pipe_bind]) {
                isp_fe_af_stt_en_write(fe_reg, stt_enable);
                isp_fe_af_stt_bst_write(fe_reg, 0xF);
            }
        }

        /* ae */
        num_h = reg_cfg_info->alg_reg_cfg[0].ae_reg_cfg.dyna_reg_cfg.fe_weight_table_width;
        num_v = reg_cfg_info->alg_reg_cfg[0].ae_reg_cfg.dyna_reg_cfg.fe_weight_table_height;
        isp_fe_ae_stt_size_write(fe_reg, (num_h * num_v + 3) / 4); /* plus 3 divide 4 is byte align */

        /* AF */
        num_h = reg_cfg_info->alg_reg_cfg[0].fe_af_reg_cfg.window_hnum;
        num_v = reg_cfg_info->alg_reg_cfg[0].fe_af_reg_cfg.window_vnum;
        isp_fe_af_stt_size_write(fe_reg, (num_h * num_v * 4 + 3) / 4); /* plus 3 divide 4 is byte align */
    }

    isp_ctx->special_opt.fe_stt_update = TD_FALSE;

    return TD_SUCCESS;
}

static td_void isp_online_stt_enable_reg_config(isp_post_be_reg_type *post_be_reg)
{
    td_bool stt_enable = TD_TRUE;
    isp_post_be_ae_stt_en_write(post_be_reg, stt_enable);
    isp_post_be_ae_stt_bst_write(post_be_reg, 0xF);

    isp_la_stt_en_write(post_be_reg, stt_enable);
    isp_la_stt_bst_write(post_be_reg, 0xF);

    isp_awb_stt_en_write(post_be_reg, stt_enable);
    isp_awb_stt_bst_write(post_be_reg, 0xF);

    isp_awblsc_stt_en_write(post_be_reg, stt_enable);
    isp_awblsc_stt_bst_write(post_be_reg, 0xF);

    isp_af_stt_en_write(post_be_reg, stt_enable);
    isp_af_stt_bst_write(post_be_reg, 0xF);

    isp_dehaze_stt_en_write(post_be_reg, stt_enable);
    isp_dehaze_stt_bst_write(post_be_reg, 0xF);

    isp_ldci_lpfstt_en_write(post_be_reg, stt_enable);
    isp_ldci_lpfstt_bst_write(post_be_reg, 0xF);
}

static td_s32 isp_online_stt_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_u16 num_h, num_v;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_post_be_reg_type *post_be_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if ((is_offline_mode(isp_ctx->block_attr.running_mode)) || (is_striping_mode(isp_ctx->block_attr.running_mode)) ||
        (is_pre_online_post_offline(isp_ctx->block_attr.running_mode))) {
        return TD_SUCCESS;
    }

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_be_reg);

    if (isp_ctx->special_opt.be_on_stt_update[i]) {
        isp_online_stt_enable_reg_config(post_be_reg);

        isp_ctx->special_opt.be_on_stt_update[i] = TD_FALSE;
    }

    /* AE */
    num_h = reg_cfg_info->alg_reg_cfg[i].ae_reg_cfg.dyna_reg_cfg.be_weight_table_width;
    num_v = reg_cfg_info->alg_reg_cfg[i].ae_reg_cfg.dyna_reg_cfg.be_weight_table_height;
    isp_post_be_ae_stt_size_write(post_be_reg, (num_h * num_v + 3) / 4); /* plus 3 divide 4 is byte align */

    /* MG */
    num_h = reg_cfg_info->alg_reg_cfg[i].mg_reg_cfg.dyna_reg_cfg.zone_width;
    num_v = reg_cfg_info->alg_reg_cfg[i].mg_reg_cfg.dyna_reg_cfg.zone_height;
    isp_la_stt_size_write(post_be_reg, (num_h * num_v + 3) / 4); /* plus 3 divide 4 is byte align */

    /* AWB */
    num_h = reg_cfg_info->alg_reg_cfg[i].awb_reg_cfg.awb_reg_usr_cfg.be_zone_col;
    num_v = reg_cfg_info->alg_reg_cfg[i].awb_reg_cfg.awb_reg_usr_cfg.be_zone_row;
    isp_awb_stt_size_write(post_be_reg, (num_h * num_v * 2 + 3) / 4); /* 2 plus 3 divide 4 is byte align */

    /* AWBLSC */
    num_h = reg_cfg_info->alg_reg_cfg[i].awblsc_reg_cfg.awblsc_reg_sta_cfg.zone_col;
    num_v = reg_cfg_info->alg_reg_cfg[i].awblsc_reg_cfg.awblsc_reg_sta_cfg.zone_row;
    isp_awblsc_stt_size_write(post_be_reg, (num_h * num_v * 2 + 3) / 4); /* 2 plus 3 divide 4 byte align */

    /* AF */
    num_h = reg_cfg_info->alg_reg_cfg[i].be_af_reg_cfg.window_hnum;
    num_v = reg_cfg_info->alg_reg_cfg[i].be_af_reg_cfg.window_vnum;
    isp_af_stt_size_write(post_be_reg, (num_h * num_v * 4 + 3) / 4); /* plus 3 divide 4 is byte align */

    /* dehaze */
    num_h = reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.static_reg_cfg.dchnum;
    num_v = reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.static_reg_cfg.dcvnum;
    isp_dehaze_stt_size_write(post_be_reg, (((num_h + 1) * (num_v + 1)) + 3) / 4); /* plus 3 divide 4 is byte align */

    /* Ldci */
    num_h = reg_cfg_info->alg_reg_cfg[i].ldci_reg_cfg.dyna_reg_cfg.stat_sml_map_width;
    num_v = reg_cfg_info->alg_reg_cfg[i].ldci_reg_cfg.dyna_reg_cfg.stat_sml_map_height;
    isp_ldci_lpfstt_size_write(post_be_reg, (num_h * num_v + 3) / 4); /* plus 3 divide 4 is byte align */

    return TD_SUCCESS;
}

static td_s32 isp_be_alg_lut2stt_reg_new_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_be_stt2lut_regnew_reg_cfg *be_stt2lut_regnew_cfg = &reg_cfg_info->alg_reg_cfg[i].stt2lut_regnew_cfg;
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_be_reg);
    isp_check_pointer_return(pre_be_reg);

    if (be_stt2lut_regnew_cfg->gamma_stt2lut_regnew) {
        isp_gamma_stt2lut_regnew_write(post_be_reg, TD_TRUE);
    }

    if ((isp_ctx->frame_cnt < STT_LUT_CONFIG_TIMES) || be_stt2lut_regnew_cfg->lsc_stt2lut_regnew) {
        isp_lsc_stt2lut_regnew_write(post_be_reg, TD_TRUE);
    }

    if ((isp_ctx->frame_cnt < STT_LUT_CONFIG_TIMES) || be_stt2lut_regnew_cfg->bnr_lsc_stt2lut_regnew) {
        isp_bnr_stt2lut_regnew_write(post_be_reg, TD_TRUE);
    }

    if ((isp_ctx->frame_cnt < STT_LUT_CONFIG_TIMES) || be_stt2lut_regnew_cfg->clut_stt2lut_regnew) {
        isp_clut_stt2lut_regnew_write(post_be_reg, TD_TRUE);
    }

    if ((isp_ctx->frame_cnt < STT_LUT_CONFIG_TIMES) || be_stt2lut_regnew_cfg->dpc_stt2lut_regnew) {
        isp_dpc_stt2lut_regnew_write(pre_be_reg, TD_TRUE);
    }

    if ((isp_ctx->frame_cnt < STT_LUT_CONFIG_TIMES) || be_stt2lut_regnew_cfg->pre_gamma_stt2lut_regnew) {
        isp_pregamma_stt2lut_regnew_write(post_be_reg, TD_TRUE);
    }

    if ((isp_ctx->frame_cnt < STT_LUT_CONFIG_TIMES) || be_stt2lut_regnew_cfg->bshp_stt2lut_regnew) {
        isp_bnrshp_stt2lut_regnew_write(post_be_reg, TD_TRUE);
    }

    return TD_SUCCESS;
}

static td_s32 isp_be_alg_lut_update_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_be_lut_update_reg_cfg *be_lut_update_cfg = &reg_cfg_info->alg_reg_cfg[i].be_lut_update_cfg;
    isp_post_be_reg_type *post_be_reg = TD_NULL;

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_be_reg);

    if (be_lut_update_cfg->ae_lut_update) {
        isp_post_be_ae_lut_update_write(post_be_reg, be_lut_update_cfg->ae_lut_update);
    }

    if (be_lut_update_cfg->drc_tm_lut_update) {
        isp_adrc_lut0_update_write(post_be_reg, be_lut_update_cfg->drc_tm_lut_update);
    }

    if (be_lut_update_cfg->lcac_lut_update) {
        isp_demosaic_depurplut_update_write(post_be_reg, be_lut_update_cfg->lcac_lut_update);
    }

    if (be_lut_update_cfg->nddm_gf_lut_update) {
        isp_nddm_gf_lut_update_write(post_be_reg, be_lut_update_cfg->nddm_gf_lut_update);
    }
    if (be_lut_update_cfg->ldci_drc_lut_update) {
        isp_ldci_drc_lut_update_write(post_be_reg, be_lut_update_cfg->ldci_drc_lut_update);
    }
    if (be_lut_update_cfg->ldci_calc_lut_update) {
        isp_ldci_calc_lut_update_write(post_be_reg, be_lut_update_cfg->ldci_calc_lut_update);
    }

    if (be_lut_update_cfg->ca_lut_update) {
        isp_ca_lut_update_write(post_be_reg, be_lut_update_cfg->ca_lut_update);
    }

    return TD_SUCCESS;
}

static td_void isp_print_sel_config(isp_post_be_reg_type *be_reg, isp_alg_reg_cfg *alg_reg_cfg)
{
    if (alg_reg_cfg->sharpen_reg_cfg.enable) {
        isp_sharpen_print_sel_write(be_reg, alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg.print_sel);
    } else {
        isp_sharpen_print_sel_write(be_reg, 0);
    }
}

static td_void isp_pre_be_cur_enable_reg_config(isp_viproc_reg_type *pre_viproc, isp_pre_be_reg_type *pre_be_reg,
    isp_alg_reg_cfg *alg_reg_cfg)
{
    isp_dpc_en_write(pre_viproc, alg_reg_cfg->dp_reg_cfg.dpc_en[0]);       /* array index 0 */
    isp_dpc_dpc_en1_write(pre_be_reg, alg_reg_cfg->dp_reg_cfg.dpc_en[2]);  /* array index 2 */
    isp_dpc1_en_write(pre_viproc, alg_reg_cfg->dp_reg_cfg.dpc_en[1]);      /* array index 1 */
    isp_dpc1_dpc_en1_write(pre_be_reg, alg_reg_cfg->dp_reg_cfg.dpc_en[3]); /* array index 3 */
    isp_expander_en_write(pre_viproc, alg_reg_cfg->expander_cfg.enable);
    isp_rgbir_en_write(pre_viproc, alg_reg_cfg->rgbir_reg_cfg.rgbir_enable);
    if (alg_reg_cfg->rgbir_reg_cfg.rgbir_enable == TD_TRUE) {
        isp_ge_en_write(pre_viproc, TD_FALSE);
        isp_ge_ge1_en_write(pre_be_reg, TD_FALSE);
        isp_ge1_en_write(pre_viproc, TD_FALSE);
        isp_ge1_ge1_en_write(pre_be_reg, TD_FALSE);
    } else {
        isp_ge_en_write(pre_viproc, alg_reg_cfg->ge_reg_cfg.ge_en[0]);		   /* array index 0 */
        isp_ge_ge1_en_write(pre_be_reg, alg_reg_cfg->ge_reg_cfg.ge_en[2]);	   /* array index 2 */
        isp_ge1_en_write(pre_viproc, alg_reg_cfg->ge_reg_cfg.ge_en[1]); 	   /* array index 1 */
        isp_ge1_ge1_en_write(pre_be_reg, alg_reg_cfg->ge_reg_cfg.ge_en[3]);    /* array index 3 */
    }
    isp_crb_en_write(pre_viproc, alg_reg_cfg->crb_reg_cfg.crb_en);
}

static td_void isp_be_cur_enable_reg_config(ot_vi_pipe vi_pipe, isp_alg_reg_cfg *alg_reg_cfg, td_u8 i)
{
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    if ((post_viproc == TD_NULL) || (pre_viproc == TD_NULL) || (post_be_reg == TD_NULL) || (pre_be_reg == TD_NULL)) {
        return;
    }

    /* pre be */
    isp_pre_be_cur_enable_reg_config(pre_viproc, pre_be_reg, alg_reg_cfg);

    /* post be */
    /* viproc part */
    isp_wb_en_write(post_viproc, alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_wb_work_en);
    isp_cc_en_write(post_viproc, alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_cc_en);
    isp_awb_en_write(post_viproc, alg_reg_cfg->awb_reg_cfg.awb_reg_sta_cfg.be_awb_work_en);
    isp_awblsc_en_write(post_viproc, alg_reg_cfg->awblsc_reg_cfg.awblsc_reg_sta_cfg.awb_work_en);
    isp_af_en_write(post_viproc, alg_reg_cfg->be_af_reg_cfg.af_enable);
    isp_sharpen_en_write(post_viproc, alg_reg_cfg->sharpen_reg_cfg.enable);
    isp_print_sel_config(post_be_reg, alg_reg_cfg);
    isp_dmnr_vhdm_en_write(post_viproc, alg_reg_cfg->dem_reg_cfg.vhdm_enable);
    isp_dmnr_nddm_en_write(post_viproc, alg_reg_cfg->dem_reg_cfg.nddm_enable);
    isp_lsc_en_write(post_viproc, alg_reg_cfg->lsc_reg_cfg.lsc_en);
    isp_gamma_en_write(post_viproc, alg_reg_cfg->gamma_cfg.gamma_en);
    isp_csc_en_write(post_viproc, alg_reg_cfg->csc_cfg.enable);
    isp_ca_en_write(post_viproc, alg_reg_cfg->ca_reg_cfg.ca_en);
    isp_dehaze_en_write(post_viproc, alg_reg_cfg->dehaze_reg_cfg.dehaze_en);
    isp_clut_en_write(post_viproc, alg_reg_cfg->clut_cfg.enable);

    /* be part */
    isp_cc_colortone_en_write(post_be_reg, alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_cc_colortone_en);
    isp_demosaic_local_cac_en_write(post_be_reg, alg_reg_cfg->lcac_reg_cfg.local_cac_en);
    isp_demosaic_bnrshp_en_write(post_be_reg, alg_reg_cfg->bshp_reg_cfg.bshp_enable);
    isp_gcac_en_write(post_be_reg, alg_reg_cfg->acac_reg_cfg.acac_en);
    isp_demosaic_fcr_en_write(post_be_reg, alg_reg_cfg->anti_false_color_reg_cfg.fcr_enable);
    isp_adrc_pregamma_en_write(post_be_reg, alg_reg_cfg->pregamma_reg_cfg.enable);
    if (alg_reg_cfg->lsc_reg_cfg.lsc_en) {
        isp_bnr_rlsc_en_write(post_be_reg, alg_reg_cfg->bnr_reg_cfg.dyna_reg_cfg.bnrlsc_en);
    } else {
        isp_bnr_rlsc_en_write(post_be_reg, 0);
    }
}

static td_s32 isp_be_cur_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_alg_reg_cfg *alg_reg_cfg = &reg_cfg_info->alg_reg_cfg[i];
    isp_post_be_reg_type *post_be_reg = TD_NULL;

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);

    isp_check_pointer_return(post_be_reg);

    /* module enable */
    isp_be_cur_enable_reg_config(vi_pipe, alg_reg_cfg, i);

    /* ldci */
    isp_ldci_blc_ctrl_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.calc_blc_ctrl);
    isp_ldci_lpfcoef0_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[0]); /* array index 0 */
    isp_ldci_lpfcoef1_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[1]); /* array index 1 */
    isp_ldci_lpfcoef2_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[2]); /* array index 2 */
    isp_ldci_lpfcoef3_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[3]); /* array index 3 */
    isp_ldci_lpfcoef4_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[4]); /* array index 4 */
    isp_ldci_lpfcoef5_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[5]); /* array index 5 */
    isp_ldci_lpfcoef6_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[6]); /* array index 6 */
    isp_ldci_lpfcoef7_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[7]); /* array index 7 */
    isp_ldci_lpfcoef8_write(post_be_reg, alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef[8]); /* array index 8 */

    /* lsc */
    if (alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.lut_update) {
        isp_lsc_mesh_scale_write(post_be_reg, alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.mesh_scale);
    }

    /* clut */
    if (alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.resh) {
        isp_clut_gain0_write(post_be_reg, alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.gain_r);
        isp_clut_gain1_write(post_be_reg, alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.gain_g);
        isp_clut_gain2_write(post_be_reg, alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.gain_b);
    }
    /* dehaze */
    isp_dehaze_air_r_write(post_be_reg, alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.air_r);
    isp_dehaze_air_g_write(post_be_reg, alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.air_g);
    isp_dehaze_air_b_write(post_be_reg, alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.air_b);
    isp_dehaze_gstrth_write(post_be_reg, alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.strength);

    /* sharpen */
    isp_sharpen_dyna_reg_config(post_be_reg, &alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg,
        &alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.default_dyna_reg_cfg);

    return TD_SUCCESS;
}

static td_void isp_pre_be_last_reg_config_enable(isp_viproc_reg_type *pre_viproc, isp_pre_be_reg_type *pre_be_reg,
    isp_lut2stt_sync_reg_cfg *lut2stt_sync_cfg)
{
    isp_dpc_en_write(pre_viproc, lut2stt_sync_cfg->dpc_en[0]);       /* array index 0 */
    isp_dpc_dpc_en1_write(pre_be_reg, lut2stt_sync_cfg->dpc_en[2]);  /* array index 2 */
    isp_dpc1_en_write(pre_viproc, lut2stt_sync_cfg->dpc_en[1]);      /* array index 1 */
    isp_dpc1_dpc_en1_write(pre_be_reg, lut2stt_sync_cfg->dpc_en[3]); /* array index 3 */
    isp_expander_en_write(pre_viproc, lut2stt_sync_cfg->expander_en);
    isp_rgbir_en_write(pre_viproc, lut2stt_sync_cfg->rgbir_en);
    if (lut2stt_sync_cfg->rgbir_en == TD_TRUE) {
        isp_ge_en_write(pre_viproc, TD_FALSE);
        isp_ge_ge1_en_write(pre_be_reg, TD_FALSE);
        isp_ge1_en_write(pre_viproc, TD_FALSE);
        isp_ge1_ge1_en_write(pre_be_reg, TD_FALSE);
    } else {
        isp_ge_en_write(pre_viproc, lut2stt_sync_cfg->ge_en[0]);	  /* array index 0 */
        isp_ge_ge1_en_write(pre_be_reg, lut2stt_sync_cfg->ge_en[2]);  /* array index 2 */
        isp_ge1_en_write(pre_viproc, lut2stt_sync_cfg->ge_en[1]);     /* array index 1 */
        isp_ge1_ge1_en_write(pre_be_reg, lut2stt_sync_cfg->ge_en[3]); /* array index 3 */
    };

    isp_crb_en_write(pre_viproc, lut2stt_sync_cfg->crb_en);
}

static td_void isp_be_last_reg_config_enable(ot_vi_pipe vi_pipe, isp_lut2stt_sync_reg_cfg *lut2stt_sync_cfg, td_u8 i)
{
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(post_be_reg);
    isp_check_pointer_void_return(pre_be_reg);
    isp_check_pointer_void_return(post_viproc);
    isp_check_pointer_void_return(pre_viproc);

    /* pre be */
    isp_pre_be_last_reg_config_enable(pre_viproc, pre_be_reg, lut2stt_sync_cfg);

    /* post be */
    /* viproc part */
    isp_adrc_pregamma_en_write(post_be_reg, lut2stt_sync_cfg->pregamma_en);
    isp_awb_en_write(post_viproc, lut2stt_sync_cfg->awb_en);
    isp_awblsc_en_write(post_viproc, lut2stt_sync_cfg->awblsc_en);

    isp_wb_en_write(post_viproc, lut2stt_sync_cfg->wb_en);
    isp_cc_en_write(post_viproc, lut2stt_sync_cfg->ccm_en);
    isp_af_en_write(post_viproc, lut2stt_sync_cfg->af_en);
    isp_sharpen_en_write(post_viproc, lut2stt_sync_cfg->sharpen_en);
    isp_sharpen_print_sel_write(post_be_reg, (lut2stt_sync_cfg->print_sel_en && lut2stt_sync_cfg->sharpen_en));
    isp_dmnr_vhdm_en_write(post_viproc, lut2stt_sync_cfg->vhdm_en);
    isp_dmnr_nddm_en_write(post_viproc, lut2stt_sync_cfg->nddm_en);
    isp_lsc_en_write(post_viproc, lut2stt_sync_cfg->lsc_en);
    isp_gamma_en_write(post_viproc, lut2stt_sync_cfg->gamma_en);
    isp_csc_en_write(post_viproc, lut2stt_sync_cfg->csc_en);
    isp_ca_en_write(post_viproc, lut2stt_sync_cfg->ca_en);
    isp_dehaze_en_write(post_viproc, lut2stt_sync_cfg->dehaze_en);
    isp_clut_en_write(post_viproc, lut2stt_sync_cfg->clut_en);
    /* be part */
    isp_cc_colortone_en_write(post_be_reg, lut2stt_sync_cfg->ccm_color_tone_en);
    isp_demosaic_local_cac_en_write(post_be_reg, lut2stt_sync_cfg->lcac_en);
    isp_demosaic_bnrshp_en_write(post_be_reg, lut2stt_sync_cfg->bshp_en);
    isp_gcac_en_write(post_be_reg, lut2stt_sync_cfg->acac_en);
    isp_demosaic_bnrshp_en_write(post_be_reg, lut2stt_sync_cfg->bshp_en);
    isp_demosaic_fcr_en_write(post_be_reg, lut2stt_sync_cfg->fcr_en);
    if (lut2stt_sync_cfg->lsc_en) {
        isp_bnr_rlsc_en_write(post_be_reg, lut2stt_sync_cfg->bnr_lsc_en);
    } else {
        isp_bnr_rlsc_en_write(post_be_reg, 0);
    }
}

static td_s32 isp_be_last_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_lut2stt_sync_reg_cfg *lut2stt_sync_cfg = &reg_cfg_info->alg_reg_cfg[i].lut2stt_sync_cfg[0];
    isp_post_be_reg_type *post_be_reg = TD_NULL;

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    isp_check_pointer_return(post_be_reg);

    /* module enable */
    isp_be_last_reg_config_enable(vi_pipe, lut2stt_sync_cfg, i);

    /* ldci */
    isp_ldci_blc_ctrl_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.calc_blc_ctrl);
    isp_ldci_lpfcoef0_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[0]); /* array index 0 */
    isp_ldci_lpfcoef1_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[1]); /* array index 1 */
    isp_ldci_lpfcoef2_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[2]); /* array index 2 */
    isp_ldci_lpfcoef3_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[3]); /* array index 3 */
    isp_ldci_lpfcoef4_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[4]); /* array index 4 */
    isp_ldci_lpfcoef5_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[5]); /* array index 5 */
    isp_ldci_lpfcoef6_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[6]); /* array index 6 */
    isp_ldci_lpfcoef7_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[7]); /* array index 7 */
    isp_ldci_lpfcoef8_write(post_be_reg, lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef[8]); /* array index 8 */

    /* lsc */
    if (lut2stt_sync_cfg->lsc_sync_cfg.resh) {
        isp_lsc_mesh_scale_write(post_be_reg, lut2stt_sync_cfg->lsc_sync_cfg.mesh_scale);
    }
    /* clut */
    if (lut2stt_sync_cfg->clut_sync_cfg.resh) {
        isp_clut_gain0_write(post_be_reg, lut2stt_sync_cfg->clut_sync_cfg.gain_r);
        isp_clut_gain1_write(post_be_reg, lut2stt_sync_cfg->clut_sync_cfg.gain_g);
        isp_clut_gain2_write(post_be_reg, lut2stt_sync_cfg->clut_sync_cfg.gain_b);
    }
    /* dehaze */
    if (reg_cfg_info->alg_reg_cfg[i].dehaze_reg_cfg.lut2_stt_en == TD_TRUE) {
        isp_dehaze_air_r_write(post_be_reg, lut2stt_sync_cfg->dehaze_sync_cfg.air_r);
        isp_dehaze_air_g_write(post_be_reg, lut2stt_sync_cfg->dehaze_sync_cfg.air_g);
        isp_dehaze_air_b_write(post_be_reg, lut2stt_sync_cfg->dehaze_sync_cfg.air_b);
        isp_dehaze_gstrth_write(post_be_reg, lut2stt_sync_cfg->dehaze_sync_cfg.strength);
    }
    /* sharpen */
    isp_sharpen_dyna_reg_config(post_be_reg, &lut2stt_sync_cfg->sharpen_sync_cfg.mpi_dyna_reg_cfg,
        &lut2stt_sync_cfg->sharpen_sync_cfg.default_dyna_reg_cfg);

    return TD_SUCCESS;
}

static td_s32 isp_be_alg_sync_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_s32 ret;
    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        ret = isp_be_cur_reg_config(vi_pipe, reg_cfg_info, i);
        if (ret != TD_SUCCESS) {
            isp_err_trace("Pipe:%d isp_be_cur_reg_config failed!\n", vi_pipe);
        }
    } else {
        ret = isp_be_last_reg_config(vi_pipe, reg_cfg_info, i);
        if (ret != TD_SUCCESS) {
            isp_err_trace("Pipe:%d isp_be_last_reg_config failed!\n", vi_pipe);
        }
    }

    return TD_SUCCESS;
}

static td_void isp_save_be_sync_enable_reg(isp_lut2stt_sync_reg_cfg *lut2stt_sync_cfg, isp_alg_reg_cfg *alg_reg_cfg)
{
    td_u8 j;
    lut2stt_sync_cfg->ae_en = alg_reg_cfg->ae_reg_cfg.static_reg_cfg.be_enable;
    lut2stt_sync_cfg->la_en = alg_reg_cfg->mg_reg_cfg.static_reg_cfg.enable;
    lut2stt_sync_cfg->awb_en = alg_reg_cfg->awb_reg_cfg.awb_reg_sta_cfg.be_awb_work_en;
    lut2stt_sync_cfg->wb_en = alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_wb_work_en;
    lut2stt_sync_cfg->awblsc_en = alg_reg_cfg->awblsc_reg_cfg.awblsc_reg_sta_cfg.awb_work_en;
    lut2stt_sync_cfg->ccm_en = alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_cc_en;
    lut2stt_sync_cfg->ccm_color_tone_en = alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_cc_colortone_en;
    lut2stt_sync_cfg->af_en = alg_reg_cfg->be_af_reg_cfg.af_enable;
    lut2stt_sync_cfg->sharpen_en = alg_reg_cfg->sharpen_reg_cfg.enable;
    lut2stt_sync_cfg->print_sel_en = alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg.print_sel;
    lut2stt_sync_cfg->vhdm_en = alg_reg_cfg->dem_reg_cfg.vhdm_enable;
    lut2stt_sync_cfg->nddm_en = alg_reg_cfg->dem_reg_cfg.nddm_enable;
    lut2stt_sync_cfg->ldci_en = alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.enable;
    lut2stt_sync_cfg->lcac_en = alg_reg_cfg->lcac_reg_cfg.local_cac_en;
    lut2stt_sync_cfg->acac_en = alg_reg_cfg->acac_reg_cfg.acac_en;
    lut2stt_sync_cfg->bshp_en = alg_reg_cfg->bshp_reg_cfg.bshp_enable;
    lut2stt_sync_cfg->fcr_en = alg_reg_cfg->anti_false_color_reg_cfg.fcr_enable;
    lut2stt_sync_cfg->lsc_en = alg_reg_cfg->lsc_reg_cfg.lsc_en;
    lut2stt_sync_cfg->gamma_en = alg_reg_cfg->gamma_cfg.gamma_en;
    lut2stt_sync_cfg->csc_en = alg_reg_cfg->csc_cfg.enable;
    lut2stt_sync_cfg->ca_en = alg_reg_cfg->ca_reg_cfg.ca_en;
    lut2stt_sync_cfg->ca_sync_cfg.ca_luma_thr_high = alg_reg_cfg->ca_reg_cfg.usr_reg_cfg.ca_luma_thr_high;
    lut2stt_sync_cfg->ca_sync_cfg.ca_luma_ratio_high = alg_reg_cfg->ca_reg_cfg.usr_reg_cfg.ca_luma_ratio_high;
    lut2stt_sync_cfg->ca_sync_cfg.ca_iso_ratio = alg_reg_cfg->ca_reg_cfg.dyna_reg_cfg.ca_iso_ratio;
    lut2stt_sync_cfg->wdr_en = alg_reg_cfg->wdr_reg_cfg.wdr_en;
    lut2stt_sync_cfg->drc_en = alg_reg_cfg->drc_reg_cfg.enable;
    lut2stt_sync_cfg->dehaze_en = alg_reg_cfg->dehaze_reg_cfg.dehaze_en;
    lut2stt_sync_cfg->bnr_lsc_en = alg_reg_cfg->bnr_reg_cfg.dyna_reg_cfg.bnrlsc_en;
    lut2stt_sync_cfg->dg_en = alg_reg_cfg->dg_reg_cfg.dg_en;
    lut2stt_sync_cfg->four_dg_en = alg_reg_cfg->four_dg_reg_cfg.enable;
    lut2stt_sync_cfg->pregamma_en = alg_reg_cfg->pregamma_reg_cfg.enable;
    lut2stt_sync_cfg->clut_en = alg_reg_cfg->clut_cfg.enable;
    lut2stt_sync_cfg->expander_en = alg_reg_cfg->expander_cfg.enable;
    lut2stt_sync_cfg->rgbir_en = alg_reg_cfg->rgbir_reg_cfg.rgbir_enable;
    lut2stt_sync_cfg->crb_en = alg_reg_cfg->crb_reg_cfg.crb_en;
    for (j = 0; j < 4; j++) { /*  dpc ge channel number is 4 */
        lut2stt_sync_cfg->dpc_en[j] = alg_reg_cfg->dp_reg_cfg.dpc_en[j];
        lut2stt_sync_cfg->ge_en[j] = alg_reg_cfg->ge_reg_cfg.ge_en[j];
    }
}

static td_s32 isp_save_be_sync_reg(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_lut2stt_sync_reg_cfg *lut2stt_sync_cfg = &reg_cfg_info->alg_reg_cfg[i].lut2stt_sync_cfg[0];
    isp_alg_reg_cfg *alg_reg_cfg = &reg_cfg_info->alg_reg_cfg[i];

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return TD_SUCCESS;
    }

    isp_save_be_sync_enable_reg(lut2stt_sync_cfg, alg_reg_cfg);

    lut2stt_sync_cfg->lsc_sync_cfg.resh = alg_reg_cfg->be_lut_update_cfg.lsc_lut_update;
    lut2stt_sync_cfg->lsc_sync_cfg.mesh_scale = alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.mesh_scale;
    lut2stt_sync_cfg->clut_sync_cfg.resh = alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.resh;
    lut2stt_sync_cfg->clut_sync_cfg.gain_r = alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.gain_r;
    lut2stt_sync_cfg->clut_sync_cfg.gain_g = alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.gain_g;
    lut2stt_sync_cfg->clut_sync_cfg.gain_b = alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.gain_b;

    lut2stt_sync_cfg->dehaze_sync_cfg.air_r = alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.air_r;
    lut2stt_sync_cfg->dehaze_sync_cfg.air_g = alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.air_g;
    lut2stt_sync_cfg->dehaze_sync_cfg.air_b = alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.air_b;
    lut2stt_sync_cfg->dehaze_sync_cfg.strength = alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.strength;

    lut2stt_sync_cfg->ldci_sync_cfg.calc_blc_ctrl = alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.calc_blc_ctrl;
    (td_void)memcpy_s(&lut2stt_sync_cfg->ldci_sync_cfg.lpf_coef, sizeof(td_u32) * LDCI_LPF_LUT_SIZE,
        &alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg.lpf_coef, sizeof(td_u32) * LDCI_LPF_LUT_SIZE);

    (td_void)memcpy_s(&lut2stt_sync_cfg->sharpen_sync_cfg.mpi_dyna_reg_cfg, sizeof(isp_sharpen_mpi_dyna_reg_cfg),
        &alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg, sizeof(isp_sharpen_mpi_dyna_reg_cfg));
    (td_void)memcpy_s(
        &lut2stt_sync_cfg->sharpen_sync_cfg.default_dyna_reg_cfg, sizeof(isp_sharpen_default_dyna_reg_cfg),
        &alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.default_dyna_reg_cfg, sizeof(isp_sharpen_default_dyna_reg_cfg));

    return TD_SUCCESS;
}

static td_void isp_be_reah_cfg_raw_domain1(isp_alg_reg_cfg *alg_reg_cfg)
{
    alg_reg_cfg->dp_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->dp_reg_cfg.usr_reg_cfg.usr_dyna_cor_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->dp_reg_cfg.usr_reg_cfg.usr_sta_cor_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->dp_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->dp_reg_cfg.static_reg_cfg.static_resh1 = TD_TRUE;
    alg_reg_cfg->dp_reg_cfg.usr_reg_cfg.usr_dyna_cor_reg_cfg.resh1 = TD_TRUE;
    alg_reg_cfg->dp_reg_cfg.usr_reg_cfg.usr_sta_cor_reg_cfg.resh1 = TD_TRUE;
    alg_reg_cfg->dp_reg_cfg.dyna_reg_cfg.resh1 = TD_TRUE;

    alg_reg_cfg->ge_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->ge_reg_cfg.usr_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->ge_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->four_dg_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->four_dg_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->wdr_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->wdr_reg_cfg.usr_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->wdr_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->rgbir_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->rgbir_reg_cfg.usr_reg_cfg.usr_resh = TD_TRUE;
    alg_reg_cfg->rgbir_reg_cfg.dyna_reg_cfg.dyna_resh = TD_TRUE;
}

static td_void isp_be_reah_cfg_raw_domain2(isp_alg_reg_cfg *alg_reg_cfg)
{
    alg_reg_cfg->expander_cfg.usr_cfg.resh = TD_TRUE;
    alg_reg_cfg->bnr_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->bnr_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->bnr_reg_cfg.usr_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->lsc_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.coef_update = TD_TRUE;
    alg_reg_cfg->lsc_reg_cfg.usr_reg_cfg.lut_update = TD_TRUE;

    alg_reg_cfg->rlsc_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->rlsc_reg_cfg.usr_reg_cfg.coef_update = TD_TRUE;
    alg_reg_cfg->rlsc_reg_cfg.usr_reg_cfg.lut_update = TD_TRUE;

    alg_reg_cfg->dg_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->dg_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->be_blc_cfg.static_blc.resh_static = TD_TRUE;
    alg_reg_cfg->be_blc_cfg.resh_dyna_init = TD_TRUE;

    alg_reg_cfg->awb_reg_cfg.awb_reg_sta_cfg.be_awb_sta_cfg = TD_TRUE;
    alg_reg_cfg->awb_reg_cfg.awb_reg_usr_cfg.resh = TD_TRUE;

    alg_reg_cfg->drc_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->drc_reg_cfg.usr_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->drc_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->dem_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->dem_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->lcac_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->lcac_reg_cfg.usr_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->lcac_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->acac_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->acac_reg_cfg.usr_reg_cfg.usr_resh = TD_TRUE;
    alg_reg_cfg->acac_reg_cfg.dyna_reg_cfg.dyna_resh = TD_TRUE;

    alg_reg_cfg->anti_false_color_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->anti_false_color_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
}

static td_void isp_be_reah_cfg_rgb_domain(isp_alg_reg_cfg *alg_reg_cfg)
{
    alg_reg_cfg->gamma_cfg.usr_reg_cfg.gamma_lut_update_en = TD_TRUE;
    alg_reg_cfg->csc_cfg.dyna_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->dehaze_reg_cfg.static_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->dehaze_reg_cfg.dyna_reg_cfg.lut_update = 1;
    alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_ctrl_cfg.resh = TD_TRUE;
    alg_reg_cfg->clut_cfg.usr_reg_cfg.clut_usr_coef_cfg.resh = TD_TRUE;
}

static td_void isp_be_reah_cfg_yuv_domain(isp_alg_reg_cfg *alg_reg_cfg)
{
    alg_reg_cfg->ldci_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->ca_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->ca_reg_cfg.dyna_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->ca_reg_cfg.usr_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->ca_reg_cfg.usr_reg_cfg.ca_lut_update_en = TD_TRUE;

    alg_reg_cfg->sharpen_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.default_dyna_reg_cfg.resh = TD_TRUE;
    alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg.resh = TD_TRUE;

    alg_reg_cfg->mcds_reg_cfg.static_reg_cfg.static_resh = TD_TRUE;
    alg_reg_cfg->mcds_reg_cfg.dyna_reg_cfg.dyna_resh = TD_TRUE;
}

static td_void isp_be_resh_cfg(isp_alg_reg_cfg *alg_reg_cfg)
{
    isp_be_reah_cfg_raw_domain1(alg_reg_cfg);

    isp_be_reah_cfg_raw_domain2(alg_reg_cfg);

    isp_be_reah_cfg_rgb_domain(alg_reg_cfg);

    isp_be_reah_cfg_yuv_domain(alg_reg_cfg);
}

td_s32 isp_reset_fe_stt_en(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    ot_vi_pipe vi_pipe_bind;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_fe_reg_type *fe_reg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->wdr_attr.is_mast_pipe) {
        for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
            vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];
            isp_check_vir_pipe_return(vi_pipe_bind);

            fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
            isp_check_pointer_return(fe_reg);

            isp_fe_ae_stt_en_write(fe_reg, TD_FALSE);
            isp_fe_af_stt_en_write(fe_reg, TD_FALSE);
            isp_fe_update_write(fe_reg, TD_TRUE);
        }
    }

    return TD_SUCCESS;
}

static td_void isp_fe_alg_en_exit(isp_usr_ctx *isp_ctx)
{
    td_u8 i;
    ot_vi_pipe vi_pipe_bind;
    isp_fe_reg_type *fe_reg = TD_NULL;

    if (isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) {
        return;
    }

    for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
        vi_pipe_bind = isp_ctx->wdr_attr.pipe_id[i];

        if ((vi_pipe_bind < 0) || (vi_pipe_bind >= OT_ISP_MAX_PHY_PIPE_NUM)) {
            continue;
        }

        fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe_bind);
        isp_check_pointer_void_return(fe_reg);

        isp_fe_ae_en_write(fe_reg, TD_FALSE);
        isp_fe_wb_en_write(fe_reg, TD_FALSE);
        isp_fe_dg_en_write(fe_reg, TD_FALSE);
        isp_fe_af_en_write(fe_reg, TD_FALSE);
        isp_fe_ae_stt_en_write(fe_reg, TD_FALSE);
        isp_fe_af_stt_en_write(fe_reg, TD_FALSE);
        isp_fe_update_write(fe_reg, TD_TRUE);
    }
}

static td_void isp_disable_tnr_reg_write(isp_post_be_reg_type *post_be_reg, isp_viproc_reg_type *post_viproc)
{
    isp_sharpen_en_write(post_viproc, TD_FALSE);
    isp_sharpen_mot_en_write(post_be_reg, TD_FALSE);

    isp_bnr_en_write(post_viproc, TD_FALSE);
    isp_bnr_ensptnr_write(post_be_reg, TD_FALSE);
}

static td_void isp_be_alg_en_exit(ot_vi_pipe vi_pipe, const isp_usr_ctx *isp_ctx)
{
    td_u8 i;
    isp_post_be_reg_type *post_be_reg = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_viproc_reg_type *pre_viproc = TD_NULL;

    if (is_offline_mode(isp_ctx->block_attr.running_mode) || is_striping_mode(isp_ctx->block_attr.running_mode)) {
        return;
    }

    for (i = 0; i < isp_ctx->block_attr.block_num; i++) {
        post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
        pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
        post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
        pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
        if ((post_be_reg == TD_NULL) || (pre_be_reg == TD_NULL) || (post_viproc == TD_NULL) ||
            (pre_viproc == TD_NULL)) {
            return;
        }

        /* stat to ddr:disable module_en */
        isp_ae_en_write(post_viproc, TD_FALSE);
        isp_la_en_write(post_viproc, TD_FALSE);
        isp_awb_en_write(post_viproc, TD_FALSE);
        isp_awblsc_en_write(post_viproc, TD_FALSE);
        isp_af_en_write(post_viproc, TD_FALSE);
        isp_dehaze_en_write(post_viproc, TD_FALSE);
        isp_ldci_en_write(post_viproc, TD_FALSE);
        isp_flicker_en_write(pre_viproc, TD_FALSE);
        isp_awblsc_en_write(post_viproc, TD_FALSE);
        isp_ldci_wrstat_en_write(post_be_reg, TD_FALSE);
        isp_ldci_rdstat_en_write(post_be_reg, TD_FALSE);

        /* lut2stt: disable stt2lut_en  */
        isp_dpc_stt2lut_en_write(pre_be_reg, TD_FALSE);
        isp_dpc1_stt2lut_en_write(pre_be_reg, TD_FALSE);
        isp_crb_stt2lut_en_write(pre_be_reg, TD_FALSE);
        isp_pregammafe_stt2lut_en_write(pre_be_reg, TD_FALSE);
        isp_sharpen_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_ldci_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_dehaze_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_gamma_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_lsc_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_bnr_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_clut_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_pregamma_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_bnrshp_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_degammafe_stt2lut_en_write(post_be_reg, TD_FALSE);
        isp_disable_tnr_reg_write(post_be_reg, post_viproc);
    }
}

#define ms_to_us(milli_sec) (1000 * (milli_sec))

static td_void isp_alg_en_exit_wait(ot_vi_pipe vi_pipe, const isp_usr_ctx *isp_ctx)
{
    td_u32 milli_sec;
    td_u32 fe_cnt_base, fe_cnt;
    td_u64 time_begin;
    isp_fe_reg_type *fe_reg = TD_NULL;

    if ((isp_ctx->wdr_attr.is_mast_pipe != TD_TRUE) || is_virt_pipe(vi_pipe)) {
        return;
    }

    fe_reg = (isp_fe_reg_type *)isp_get_fe_vir_addr(vi_pipe);
    isp_check_pointer_void_return(fe_reg);

    milli_sec = (td_u32)(2000 / div_0_to_1_float(isp_ctx->sns_image_mode.fps)); /* 2000:2 * 1000, wait 2 frame */

    fe_cnt_base = fe_reg->isp_fe_startup.bits.isp_fe_fcnt;
    fe_cnt      = fe_cnt_base;
    time_begin  = get_sys_time_by_usec();
    while ((fe_cnt - fe_cnt_base) < 2) { /* 2 for a full frame process */
        fe_cnt = fe_reg->isp_fe_startup.bits.isp_fe_fcnt;
        if ((get_sys_time_by_usec() - time_begin) >= ms_to_us(milli_sec)) {
            return;
        }
#ifdef __LITEOS__
        usleep(1); /* msleep 10 */
#else
        usleep(ms_to_us(10)); /* msleep 10 */
#endif
    }
}

td_s32 isp_alg_en_exit(ot_vi_pipe vi_pipe)
{
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_check_pointer_return(isp_ctx);

    if (isp_ctx->para_rec.init == TD_FALSE) {
        return TD_SUCCESS;
    }

    /* FE */
    isp_fe_alg_en_exit(isp_ctx);

    /* BE */
    isp_be_alg_en_exit(vi_pipe, isp_ctx);

    /* wait */
    isp_alg_en_exit_wait(vi_pipe, isp_ctx);

    return TD_SUCCESS;
}

static td_s32 isp_fe_regs_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_s64 ret = TD_SUCCESS;
    isp_usr_ctx *isp_ctx = TD_NULL;
    td_u8 i;
    isp_check_vir_pipe_return(vi_pipe);
    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->wdr_attr.is_mast_pipe) {
        /* FE alg cfgs setting to register */
        for (i = 0; i < isp_ctx->wdr_attr.pipe_num; i++) {
            if (isp_ctx->wdr_attr.pipe_id[i] == HRS_PIPE) {
                ret += isp_fe_hrs_reg_config(vi_pipe, reg_cfg_info); /* hrs */
            }
        }
        ret += isp_fe_ae_reg_config(vi_pipe, reg_cfg_info);  /* Ae */
        ret += isp_fe_awb_reg_config(vi_pipe, reg_cfg_info); /* awb */
        ret += isp_fe_af_reg_config(vi_pipe, reg_cfg_info);  /* Af */
        ret += isp_fe_dg_reg_config(vi_pipe, reg_cfg_info);  /* DG */
        ret += isp_fe_blc_reg_config(vi_pipe, reg_cfg_info); /* fe blc */
        ret += isp_fe_system_reg_config(vi_pipe);
        ret += isp_fe_stt_reg_config(vi_pipe, reg_cfg_info);
        ret += isp_fe_dynamicblc_reg_config(vi_pipe, reg_cfg_info);
        ret += isp_fe_update_reg_config(vi_pipe, reg_cfg_info);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] isp_fe_regs_config failed!\n", vi_pipe);
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static td_void isp_be_drc_param_init(isp_be_sync_para *be_sync_param, isp_drc_reg_cfg *drc_reg_cfg)
{
    td_u8 j;
    be_sync_param->drc_shp_log = drc_reg_cfg->static_reg_cfg.shp_log;
    be_sync_param->drc_div_denom_log = drc_reg_cfg->static_reg_cfg.div_denom_log;
    be_sync_param->drc_denom_exp = drc_reg_cfg->static_reg_cfg.denom_exp;

    for (j = 0; j < OT_ISP_DRC_EXP_COMP_SAMPLE_NUM; j++) {
        be_sync_param->drc_prev_luma[j] = drc_reg_cfg->static_reg_cfg.prev_luma[j];
    }
}

static td_void isp_be_wdr_param_init(isp_be_sync_para *be_sync_param, isp_wdr_reg_cfg *wdr_reg_cfg)
{
    td_u8 j;
    for (j = 0; j < OT_ISP_EXP_RATIO_NUM; j++) {
        be_sync_param->wdr_exp_ratio[j] = wdr_reg_cfg->static_reg_cfg.expo_r_ratio_lut[j];
        be_sync_param->flick_exp_ratio[j] = wdr_reg_cfg->static_reg_cfg.flick_exp_ratio[j];
    }

    for (j = 0; j < OT_ISP_WDR_MAX_FRAME_NUM; j++) {
        be_sync_param->wdr_exp_val[j] = wdr_reg_cfg->static_reg_cfg.expo_value_lut[j];
        be_sync_param->fusion_exp_val[j] = wdr_reg_cfg->static_reg_cfg.expo_value_lut[j];
        be_sync_param->wdr_gain[j] = 0x100;
        be_sync_param->isp_dgain_no_blc[j] = 0x100;
    }

    be_sync_param->wdr_mdt_en = wdr_reg_cfg->dyna_reg_cfg.wdr_mdt_en;
    be_sync_param->fusion_mode = wdr_reg_cfg->usr_reg_cfg.fusion_mode;

    for (j = 0; j < OT_ISP_WDR_MAX_FRAME_NUM - 1; j++) {
        be_sync_param->short_thr[j] = wdr_reg_cfg->dyna_reg_cfg.short_thr[j];
        be_sync_param->long_thr[j] = wdr_reg_cfg->dyna_reg_cfg.long_thr[j];
        be_sync_param->wdr_blc_comp[j] = wdr_reg_cfg->static_reg_cfg.blc_comp_lut[j];
    }
    be_sync_param->wdr_max_ratio = wdr_reg_cfg->static_reg_cfg.max_ratio;
    be_sync_param->fusion_max_ratio = wdr_reg_cfg->static_reg_cfg.max_ratio;

    be_sync_param->saturate_thr        = wdr_reg_cfg->static_reg_cfg.saturate_thr;
    be_sync_param->fusion_saturate_thr = wdr_reg_cfg->static_reg_cfg.fusion_saturate_thr;
}

static td_s32 isp_be_sync_param_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u8 j;
    td_s32 ret;
    isp_be_sync_para be_sync_param = { 0 };
    isp_alg_reg_cfg *alg_reg_cfg = &reg_cfg_info->alg_reg_cfg[0];

    /* DG */
    be_sync_param.isp_dgain[0] = alg_reg_cfg->dg_reg_cfg.dyna_reg_cfg.gain_r;  /* array 0 assignment */
    be_sync_param.isp_dgain[1] = alg_reg_cfg->dg_reg_cfg.dyna_reg_cfg.gain_gr; /* array 1 assignment */
    be_sync_param.isp_dgain[2] = alg_reg_cfg->dg_reg_cfg.dyna_reg_cfg.gain_gb; /* array 2 assignment */
    be_sync_param.isp_dgain[3] = alg_reg_cfg->dg_reg_cfg.dyna_reg_cfg.gain_b;  /* array 3 assignment */

    /* LDCI */
    be_sync_param.ldci_comp = 0x1000;

    /* DRC */
    isp_be_drc_param_init(&be_sync_param, &alg_reg_cfg->drc_reg_cfg);

    /* WDR */
    isp_be_wdr_param_init(&be_sync_param, &alg_reg_cfg->wdr_reg_cfg);

    /* AWB */
    for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
        be_sync_param.wb_gain[j] = alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_white_balance_gain[j];
    }

    /* blc */
    (td_void)memcpy_s(&be_sync_param.be_blc, sizeof(isp_be_blc_dyna_cfg), &alg_reg_cfg->be_blc_cfg.dyna_blc,
        sizeof(isp_be_blc_dyna_cfg));

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_SYNC_PARAM_INIT, &be_sync_param);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] Init BE Sync Param Failed with ec %#x!\n", vi_pipe, ret);
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 isp_set_be_dump_raw_pre_be(ot_vi_pipe vi_pipe, const isp_usr_ctx *isp_ctx, td_u8 i)
{
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_pre_be_reg_type *pre_be_reg = TD_NULL;

    if (isp_ctx->be_frame_attr.frame_pos == OT_ISP_DUMP_FRAME_POS_NORMAL) {
        return TD_SUCCESS;
    }

    pre_be_reg = (isp_pre_be_reg_type *)isp_get_pre_be_vir_addr(vi_pipe, i);
    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);

    isp_check_pointer_return(pre_be_reg);
    isp_check_pointer_return(pre_viproc);

    isp_expander_en_write(pre_viproc, TD_FALSE);
    isp_crb_en_write(pre_viproc, TD_FALSE);
    isp_bcom_en_write(pre_viproc, TD_FALSE);
    isp_pregammafe_en_write(pre_viproc, TD_FALSE);

    return TD_SUCCESS;
}

static td_s32 isp_set_be_dump_raw_post_be(ot_vi_pipe vi_pipe, td_u8 i)
{
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *post_be_reg = TD_NULL;

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);

    isp_check_pointer_return(post_be_reg);
    isp_check_pointer_return(post_viproc);

    isp_degammafe_en_write(post_viproc, TD_FALSE);
    isp_bnr_en_write(post_viproc, TD_FALSE);
    isp_bdec_en_write(post_viproc, TD_FALSE);
    isp_awblsc_en_write(post_viproc, TD_FALSE);
    isp_lsc_en_write(post_viproc, TD_FALSE);
    isp_dg_en_write(post_viproc, TD_FALSE);
    isp_ae_en_write(post_viproc, TD_FALSE);
    isp_awb_en_write(post_viproc, TD_FALSE);
    isp_af_en_write(post_viproc, TD_FALSE);
    isp_la_en_write(post_viproc, TD_FALSE);
    isp_wb_en_write(post_viproc, TD_FALSE);
    isp_adrc_en_write(post_viproc, TD_FALSE);
    isp_drc_dither_en_write(post_be_reg, TD_FALSE);
    isp_dmnr_vhdm_en_write(post_viproc, TD_FALSE);
    isp_dmnr_nddm_en_write(post_viproc, TD_FALSE);
    isp_gcac_en_write(post_be_reg, TD_FALSE);
    isp_demosaic_local_cac_en_write(post_be_reg, TD_FALSE);
    isp_demosaic_bnrshp_en_write(post_be_reg, TD_FALSE);
    isp_demosaic_fcr_en_write(post_be_reg, TD_FALSE);

    return TD_SUCCESS;
}

static td_s32 isp_set_be_dump_raw_post_be_after_demosaic(ot_vi_pipe vi_pipe, td_u8 i)
{
    isp_viproc_reg_type *post_viproc = TD_NULL;
    isp_post_be_reg_type *post_be_reg = TD_NULL;

    post_be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);

    isp_check_pointer_return(post_be_reg);
    isp_check_pointer_return(post_viproc);

    isp_cc_en_write(post_viproc, TD_FALSE);
    isp_cc_colortone_en_write(post_be_reg, TD_FALSE);
    isp_clut_en_write(post_viproc, TD_FALSE);
    isp_gamma_en_write(post_viproc, TD_FALSE);
    isp_dehaze_en_write(post_viproc, TD_FALSE);
    isp_csc_en_write(post_viproc, TD_FALSE);
    isp_ldci_en_write(post_viproc, TD_FALSE);
    isp_ca_en_write(post_viproc, TD_FALSE);
    isp_hcds_en_write(post_viproc, TD_FALSE);
    isp_sharpen_en_write(post_viproc, TD_FALSE);

    /* dither */
    isp_dmnr_dither_en_write(post_be_reg, TD_FALSE);
    isp_acm_dither_en_write(post_be_reg, TD_FALSE);
    isp_sharpen_dither_en_write(post_be_reg, TD_FALSE);

    return TD_SUCCESS;
}

static td_s32 isp_set_be_dump_raw(ot_vi_pipe vi_pipe, td_u8 i)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->be_frame_attr.frame_pos == OT_ISP_DUMP_FRAME_POS_NORMAL) {
        return TD_SUCCESS;
    }

    ret = isp_set_be_dump_raw_pre_be(vi_pipe, isp_ctx, i);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_set_be_dump_raw_post_be(vi_pipe, i);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = isp_set_be_dump_raw_post_be_after_demosaic(vi_pipe, i);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 isp_be_alg_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info, td_u8 i)
{
    td_s64 ret = TD_SUCCESS;

    ret += isp_ae_reg_config(vi_pipe, reg_cfg_info, i);       /* ae */
    ret += isp_awb_reg_config(vi_pipe, reg_cfg_info, i);      /* awb */
    ret += isp_awb_lsc_reg_config(vi_pipe, reg_cfg_info, i);  /* awb lsc */
    ret += isp_af_reg_config(vi_pipe, reg_cfg_info, i);       /* AF */
    ret += isp_sharpen_reg_config(vi_pipe, reg_cfg_info, i);  /* sharpen */
    ret += isp_demosaic_reg_config(vi_pipe, reg_cfg_info, i); /* demosaic */
    ret += isp_bshp_reg_config(vi_pipe, reg_cfg_info, i);     /* bayer sharpen */
    ret += isp_fpn_reg_config(vi_pipe, reg_cfg_info, i);      /* FPN */
    ret += isp_ldci_reg_config(vi_pipe, reg_cfg_info, i);     /* ldci */
    ret += isp_lcac_reg_config(vi_pipe, reg_cfg_info, i);     /* Local cac */
    ret += isp_acac_reg_config(vi_pipe, reg_cfg_info, i);     /* acac */
    ret += isp_fcr_reg_config(vi_pipe, reg_cfg_info, i);      /* FCR */
    ret += isp_dpc_reg_config(vi_pipe, reg_cfg_info, i);      /* dpc */
    ret += isp_dpc1_reg_config(vi_pipe, reg_cfg_info, i);     /* dpc */
    ret += isp_ge_reg_config(vi_pipe, reg_cfg_info, i);       /* ge */
    ret += isp_lsc_reg_config(vi_pipe, reg_cfg_info, i);      /* BE LSC */
    ret += isp_rgbir_reg_config(vi_pipe, reg_cfg_info, i);    /* Rgbir */
    ret += isp_gamma_reg_config(vi_pipe, reg_cfg_info, i);    /* gamma */
    ret += isp_csc_reg_config(vi_pipe, reg_cfg_info, i);      /* csc */
    ret += isp_ca_reg_config(vi_pipe, reg_cfg_info, i);       /* ca */
    ret += isp_mcds_reg_config(vi_pipe, reg_cfg_info, i);     /* mcds */
    ret += isp_wdr_reg_config(vi_pipe, reg_cfg_info, i);      /* wdr */
    ret += isp_drc_reg_config(vi_pipe, reg_cfg_info, i);      /* drc */
    ret += isp_dehaze_reg_config(vi_pipe, reg_cfg_info, i);   /* Dehaze */
    ret += isp_bayer_nr_reg_config(vi_pipe, reg_cfg_info, i); /* BayerNR */
    ret += isp_bnr_lsc_reg_config(vi_pipe, reg_cfg_info, i);  /* bayer_nr-LSC */
    ret += isp_dg_reg_config(vi_pipe, reg_cfg_info, i);       /* DG */
    ret += isp_4dg_reg_config(vi_pipe, reg_cfg_info, i);      /* 4DG */
    ret += isp_pregamma_reg_config(vi_pipe, reg_cfg_info, i); /* PreGamma */
    ret += isp_flick_reg_config(vi_pipe, reg_cfg_info, i);    /* Flicker */
    ret += isp_be_blc_reg_config(vi_pipe, reg_cfg_info, i);   /* be blc */
    ret += isp_clut_reg_config(vi_pipe, reg_cfg_info, i);     /* CLUT */
    ret += isp_detail_reg_config(vi_pipe, reg_cfg_info, i);   /* detail */
    ret += isp_expander_reg_config(vi_pipe, reg_cfg_info, i); /* expander */
    ret += isp_rc_reg_config(vi_pipe, reg_cfg_info, i);       /* radial crop */
    ret += isp_crb_reg_config(vi_pipe, reg_cfg_info, i);      /* crb */
    ret += isp_agamma_reg_config(vi_pipe, reg_cfg_info, i);   /* agamma */
    ret += isp_adgamma_reg_config(vi_pipe, reg_cfg_info, i);  /* adgamma */
    if (ret != TD_SUCCESS) {
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void isp_be_reg_up_config(ot_vi_pipe vi_pipe, td_u8 i)
{
    isp_viproc_reg_type *pre_viproc = TD_NULL;
    isp_viproc_reg_type *post_viproc = TD_NULL;

    pre_viproc = (isp_viproc_reg_type *)isp_get_pre_vi_proc_vir_addr(vi_pipe, i);
    post_viproc = (isp_viproc_reg_type *)isp_get_post_vi_proc_vir_addr(vi_pipe, i);
    isp_check_pointer_void_return(pre_viproc);
    isp_check_pointer_void_return(post_viproc);

    isp_be_reg_up_write(pre_viproc, TD_TRUE);
    isp_be_reg_up_write(post_viproc, TD_TRUE);
}

static td_s32 isp_be_regs_config_update(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_system_reg_config(vi_pipe, reg_cfg_info, i);     /* sys */
        ret += isp_dither_reg_config(vi_pipe, reg_cfg_info, i);     /* dither */
        ret += isp_online_stt_reg_config(vi_pipe, reg_cfg_info, i); /* online stt */

        /* Be alg cfgs setting to register */
        ret += isp_be_alg_reg_config(vi_pipe, reg_cfg_info, i);
    }

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_be_alg_sync_reg_config(vi_pipe, reg_cfg_info, i);
        ret += isp_be_alg_lut2stt_reg_new_reg_config(vi_pipe, reg_cfg_info, i);
        ret += isp_be_alg_lut_update_reg_config(vi_pipe, reg_cfg_info, i);
    }

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_set_be_dump_raw(vi_pipe, i);
    }

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_save_be_sync_reg(vi_pipe, reg_cfg_info, i);
    }

    return ret;
}

static td_s32 isp_be_regs_config(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_s32 ret;
    td_s32 ret1;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->alg_run_select != OT_ISP_ALG_RUN_FE_ONLY) {
        ret = isp_be_regs_config_update(vi_pipe, reg_cfg_info);
        if (ret != TD_SUCCESS) {
            isp_err_trace("ISP[%d] isp_be_regs_config failed!\n", vi_pipe);
            return TD_FAILURE;
        }
    }

    if ((is_offline_mode(isp_ctx->block_attr.running_mode)) ||
        (is_striping_mode(isp_ctx->block_attr.running_mode)) ||
        is_pre_online_post_offline(isp_ctx->block_attr.running_mode)) {
        ret1 = isp_cfg_be_buf_ctl(vi_pipe);
        if (ret1 != TD_SUCCESS) {
            isp_err_trace("ISP[%d] Be config bufs ctl failed %x!\n", vi_pipe, ret1);
            return ret;
        }
    }

    return TD_SUCCESS;
}

static td_s32 isp_be_regs_config_init(ot_vi_pipe vi_pipe, isp_reg_cfg *reg_cfg_info)
{
    td_u32 i;
    td_s64 ret = TD_SUCCESS;

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_save_be_sync_reg(vi_pipe, reg_cfg_info, i);
    }

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_reg_default(vi_pipe, reg_cfg_info, i);
        ret += isp_system_reg_config(vi_pipe, reg_cfg_info, i); /* sys */
        ret += isp_dither_reg_config(vi_pipe, reg_cfg_info, i); /* dither */
        ret += isp_online_stt_reg_config(vi_pipe, reg_cfg_info, i);
        /* Be alg cfgs setting to register */
        ret += isp_be_alg_reg_config(vi_pipe, reg_cfg_info, i);
    }

    for (i = 0; i < reg_cfg_info->cfg_num; i++) {
        ret += isp_be_alg_sync_reg_config(vi_pipe, reg_cfg_info, i);
        ret += isp_be_alg_lut2stt_reg_new_reg_config(vi_pipe, reg_cfg_info, i);
        isp_be_reg_up_config(vi_pipe, i);
        ret += isp_be_alg_lut_update_reg_config(vi_pipe, reg_cfg_info, i);
    }

    ret += isp_be_sync_param_init(vi_pipe, reg_cfg_info);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISP[%d] isp_be_regs_config_init failed!\n", vi_pipe);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 isp_reg_cfg_init(ot_vi_pipe vi_pipe)
{
    isp_reg_cfg_attr *reg_cfg_ctx = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, reg_cfg_ctx);

    if (reg_cfg_ctx == TD_NULL) {
        reg_cfg_ctx = (isp_reg_cfg_attr *)isp_malloc(sizeof(isp_reg_cfg_attr));
        if (reg_cfg_ctx == TD_NULL) {
            isp_err_trace("Isp[%d] RegCfgCtx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void)memset_s(reg_cfg_ctx, sizeof(isp_reg_cfg_attr), 0, sizeof(isp_reg_cfg_attr));

    isp_regcfg_set_ctx(vi_pipe, reg_cfg_ctx);

    return TD_SUCCESS;
}

td_void isp_reg_cfg_exit(ot_vi_pipe vi_pipe)
{
    isp_reg_cfg_attr *reg_cfg_ctx = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, reg_cfg_ctx);
    isp_free(reg_cfg_ctx);
    isp_regcfg_reset_ctx(vi_pipe);
}

td_s32 isp_get_reg_cfg_ctx(ot_vi_pipe vi_pipe, td_void **reg_cfg_info)
{
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_regcfg_get_ctx(vi_pipe, reg_cfg);
    isp_check_pointer_return(reg_cfg);

    if (!reg_cfg->init) {
        reg_cfg->reg_cfg.cfg_key.key = 0;

        reg_cfg->init = TD_TRUE;
    }

    reg_cfg->reg_cfg.cfg_num = isp_ctx->block_attr.block_num;

    *reg_cfg_info = &reg_cfg->reg_cfg;

    return TD_SUCCESS;
}

td_s32 isp_reg_cfg_info_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);
    isp_regcfg_get_ctx(vi_pipe, reg_cfg);
    isp_check_pointer_return(reg_cfg);

    ret = isp_fe_regs_config(vi_pipe, &reg_cfg->reg_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp_fe_regs_config failed!\n", vi_pipe);
    }

    if (isp_ctx->alg_run_select != OT_ISP_ALG_RUN_FE_ONLY) {
        ret = isp_be_regs_config_init(vi_pipe, &reg_cfg->reg_cfg);
        if (ret != TD_SUCCESS) {
            isp_err_trace("Pipe:%d isp_be_regs_config_init failed!\n", vi_pipe);
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_reg_cfg_info_set(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;

    isp_regcfg_get_ctx(vi_pipe, reg_cfg);
    isp_check_pointer_return(reg_cfg);

    ret = isp_fe_regs_config(vi_pipe, &reg_cfg->reg_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp_fe_regs_config failed!\n", vi_pipe);
    }

    ret = isp_be_regs_config(vi_pipe, &reg_cfg->reg_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("Pipe:%d isp_be_regs_config failed!\n", vi_pipe);
    }

    if (reg_cfg->reg_cfg.kernel_reg_cfg.cfg_key.key) {
        ret = ioctl(isp_get_fd(vi_pipe), ISP_REG_CFG_SET, &reg_cfg->reg_cfg.kernel_reg_cfg);
        if (ret != TD_SUCCESS) {
            isp_err_trace("Config ISP register Failed with ec %#x!\n", ret);
            return ret;
        }
    }

    return TD_SUCCESS;
}

static td_void isp_sns_regs_info_check(ot_vi_pipe vi_pipe, const ot_isp_sns_regs_info *sns_regs_info)
{
    ot_unused(vi_pipe);

    if ((sns_regs_info->sns_type >= OT_ISP_SNS_TYPE_BUTT)) {
        isp_err_trace("senor's regs info invalid, sns_type %d\n", sns_regs_info->sns_type);
        return;
    }

    if (sns_regs_info->reg_num > OT_ISP_MAX_SNS_REGS) {
        isp_err_trace("senor's regs info invalid, reg_num %d\n", sns_regs_info->reg_num);
        return;
    }

    return;
}

static td_void isp_get_start_and_end_vipipe(ot_vi_pipe vi_pipe, isp_usr_ctx *isp_ctx, td_s32 *pipe_st, td_s32 *pipe_ed)
{
    td_s8 stitch_main_pipe;

    if (isp_ctx->stitch_attr.stitch_enable == TD_TRUE) {
        stitch_main_pipe = isp_ctx->stitch_attr.stitch_bind_id[0];

        if (is_stitch_main_pipe(vi_pipe, stitch_main_pipe)) {
            *pipe_st = 0;
            *pipe_ed = isp_ctx->stitch_attr.stitch_pipe_num - 1;
        } else {
            *pipe_st = vi_pipe;
            *pipe_ed = vi_pipe - 1;
        }
    } else {
        *pipe_st = vi_pipe;
        *pipe_ed = vi_pipe;
    }
}

static td_void isp_normal_sync_cfg_get(ot_vi_pipe vi_pipe, ot_isp_sns_regs_info *sns_regs_info,
    isp_alg_reg_cfg *alg_reg_cfg, isp_sync_cfg_buf_node *sync_cfg_node)
{
    (td_void)memcpy_s(&sync_cfg_node->sns_regs_info, sizeof(ot_isp_sns_regs_info), sns_regs_info,
        sizeof(ot_isp_sns_regs_info));
    isp_sns_regs_info_check(vi_pipe, &sync_cfg_node->sns_regs_info);
    (td_void)memcpy_s(&sync_cfg_node->ae_reg_cfg, sizeof(isp_ae_reg_cfg_2), &alg_reg_cfg->ae_reg_cfg2,
        sizeof(isp_ae_reg_cfg_2));
    (td_void)memcpy_s(&sync_cfg_node->awb_reg_cfg.be_white_balance_gain[0], sizeof(td_u32) * OT_ISP_BAYER_CHN_NUM,
        &alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_white_balance_gain[0], sizeof(td_u32) * OT_ISP_BAYER_CHN_NUM);
    (td_void)memcpy_s(&sync_cfg_node->awb_reg_cfg.color_matrix[0], sizeof(td_u16) * OT_ISP_CCM_MATRIX_SIZE,
        &alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_color_matrix[0], sizeof(td_u16) * OT_ISP_CCM_MATRIX_SIZE);
    (td_void)memcpy_s(&sync_cfg_node->drc_reg_cfg, sizeof(isp_drc_sync_cfg), &alg_reg_cfg->drc_reg_cfg.sync_reg_cfg,
        sizeof(isp_drc_sync_cfg));
    (td_void)memcpy_s(&sync_cfg_node->wdr_reg_cfg, sizeof(isp_fswdr_sync_cfg), &alg_reg_cfg->wdr_reg_cfg.sync_reg_cfg,
        sizeof(isp_fswdr_sync_cfg));
    (td_void)memcpy_s(&sync_cfg_node->be_blc_reg_cfg, sizeof(isp_be_blc_dyna_cfg), &alg_reg_cfg->be_blc_cfg.dyna_blc,
        sizeof(isp_be_blc_dyna_cfg));
    (td_void)memcpy_s(&sync_cfg_node->fe_blc_reg_cfg, sizeof(isp_fe_blc_dyna_cfg), &alg_reg_cfg->fe_blc_cfg.dyna_blc,
        sizeof(isp_fe_blc_dyna_cfg));
    (td_void)memcpy_s(&sync_cfg_node->dynamic_blc_cfg, sizeof(isp_dynamic_blc_sync_cfg),
        &alg_reg_cfg->dynamic_blc_reg_cfg.sync_cfg, sizeof(isp_dynamic_blc_sync_cfg));
    (td_void)memcpy_s(&sync_cfg_node->fpn_cfg, sizeof(isp_fpn_sync_cfg), &alg_reg_cfg->fpn_reg_cfg.sync_cfg,
        sizeof(isp_fpn_sync_cfg));
    sync_cfg_node->awb_reg_cfg.be_awb_switch = alg_reg_cfg->awb_reg_cfg.awb_reg_usr_cfg.be_awb_switch;
}
static td_void isp_main_pipe_stitch_sync_cfg_get(const isp_usr_ctx *isp_ctx, isp_sync_cfg_buf_node *sync_cfg_node)
{
    td_u8 i, stitch_idx;
    ot_vi_pipe stitch_pipe;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;
    isp_awb_reg_dyn_cfg *awb_reg = TD_NULL;

    for (stitch_idx = 0; stitch_idx < isp_ctx->stitch_attr.stitch_pipe_num; stitch_idx++) {
        stitch_pipe = isp_ctx->stitch_attr.stitch_bind_id[stitch_idx];
        isp_regcfg_get_ctx(stitch_pipe, reg_cfg);
        if (reg_cfg == TD_NULL) {
            continue;
        }
        awb_reg = &reg_cfg->reg_cfg.alg_reg_cfg[0].awb_reg_cfg.awb_reg_dyn_cfg;
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++) {
            sync_cfg_node->awb_reg_cfg_stitch[stitch_idx].be_white_balance_gain[i] = awb_reg->be_white_balance_gain[i];
        }

        for (i = 0; i < OT_ISP_CCM_MATRIX_SIZE; i++) {
            sync_cfg_node->awb_reg_cfg_stitch[stitch_idx].color_matrix[i] = awb_reg->be_color_matrix[i];
        }

        (td_void)memcpy_s(&sync_cfg_node->fe_blc_reg_cfg_stitch[stitch_idx], sizeof(isp_fe_blc_dyna_cfg),
            &reg_cfg->reg_cfg.alg_reg_cfg[0].fe_blc_cfg.dyna_blc, sizeof(isp_fe_blc_dyna_cfg));
        (td_void)memcpy_s(&sync_cfg_node->be_blc_reg_cfg_stitch[stitch_idx], sizeof(isp_be_blc_dyna_cfg),
            &reg_cfg->reg_cfg.alg_reg_cfg[0].be_blc_cfg.dyna_blc, sizeof(isp_be_blc_dyna_cfg));
    }
}

static td_void isp_stitch_sync_cfg_get(ot_vi_pipe vi_pipe, ot_isp_sns_regs_info *sns_regs_info,
    isp_alg_reg_cfg *alg_reg_cfg, isp_sync_cfg_buf_node *sync_cfg_node)
{
    td_s8 stitch_main_pipe;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg_s = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (isp_ctx->stitch_attr.stitch_enable != TD_TRUE) {
        return;
    }

    stitch_main_pipe = isp_ctx->stitch_attr.stitch_bind_id[0];
    if (is_stitch_main_pipe(vi_pipe, stitch_main_pipe)) {
        isp_main_pipe_stitch_sync_cfg_get(isp_ctx, sync_cfg_node);
        return;
    }

    isp_regcfg_get_ctx(stitch_main_pipe, reg_cfg_s);
    if (reg_cfg_s == TD_NULL) {
        return;
    }

    (td_void)memcpy_s(&sync_cfg_node->sns_regs_info, sizeof(ot_isp_sns_regs_info),
        &reg_cfg_s->sync_cfg_node.sns_regs_info, sizeof(ot_isp_sns_regs_info));
    (td_void)memcpy_s(&sync_cfg_node->sns_regs_info.com_bus, sizeof(ot_isp_sns_commbus), &sns_regs_info->com_bus,
        sizeof(ot_isp_sns_commbus));
    (td_void)memcpy_s(&sync_cfg_node->sns_regs_info.slv_sync.slave_bind_dev, sizeof(td_u32),
        &sns_regs_info->slv_sync.slave_bind_dev, sizeof(td_u32));
    (td_void)memcpy_s(&sync_cfg_node->ae_reg_cfg, sizeof(isp_ae_reg_cfg_2),
        &reg_cfg_s->reg_cfg.alg_reg_cfg[0].ae_reg_cfg2, sizeof(isp_ae_reg_cfg_2));
    (td_void)memcpy_s(&sync_cfg_node->awb_reg_cfg.color_matrix[0], sizeof(td_u16) * OT_ISP_CCM_MATRIX_SIZE,
        &alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_color_matrix[0], sizeof(td_u16) * OT_ISP_CCM_MATRIX_SIZE);
    (td_void)memcpy_s(&sync_cfg_node->awb_reg_cfg.be_white_balance_gain[0], sizeof(td_u32) * OT_ISP_BAYER_CHN_NUM,
        &alg_reg_cfg->awb_reg_cfg.awb_reg_dyn_cfg.be_white_balance_gain[0], sizeof(td_u32) * OT_ISP_BAYER_CHN_NUM);
    sync_cfg_node->awb_reg_cfg.be_awb_switch = alg_reg_cfg->awb_reg_cfg.awb_reg_usr_cfg.be_awb_switch;
}

td_s32 isp_sync_cfg_set(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_s32 pipe_st = 0;
    td_s32 pipe_ed = 0;
    ot_vi_pipe pipe_tmp = vi_pipe;

    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;

    ot_isp_sns_regs_info *sns_regs_info = TD_NULL;

    isp_get_ctx(pipe_tmp, isp_ctx);

    isp_get_start_and_end_vipipe(pipe_tmp, isp_ctx, &pipe_st, &pipe_ed);

    while (pipe_st <= pipe_ed) {
        if (isp_ctx->stitch_attr.stitch_enable == TD_TRUE) {
            pipe_tmp = isp_ctx->stitch_attr.stitch_bind_id[pipe_st];
        } else {
            pipe_tmp = pipe_st;
        }

        isp_get_ctx(pipe_tmp, isp_ctx);
        isp_regcfg_get_ctx(pipe_tmp, reg_cfg);
        isp_check_pointer_return(reg_cfg);
        isp_check_open_return(pipe_tmp);

        if (isp_sensor_update_sns_reg(pipe_tmp) != TD_SUCCESS) {
            /* If Users need to config AE sync info themselves, they can set pfn_cmos_get_sns_reg_info
             * to TD_NULL in cmos.c
             */
            /* Then there will be NO AE sync configs in kernel of firmware */
            return TD_SUCCESS;
        }

        isp_sensor_get_sns_reg(pipe_tmp, &sns_regs_info);

        isp_normal_sync_cfg_get(pipe_tmp, sns_regs_info, &reg_cfg->reg_cfg.alg_reg_cfg[0], &reg_cfg->sync_cfg_node);
        isp_stitch_sync_cfg_get(pipe_tmp, sns_regs_info, &reg_cfg->reg_cfg.alg_reg_cfg[0], &reg_cfg->sync_cfg_node);

        reg_cfg->sync_cfg_node.valid = TD_TRUE;
        ret = ioctl(isp_get_fd(pipe_tmp), ISP_SYNC_CFG_SET, &reg_cfg->sync_cfg_node);
        if (ret != TD_SUCCESS) {
            return ret;
        }

        sns_regs_info->config = TD_TRUE;

        pipe_st++;
    }

    return TD_SUCCESS;
}
#ifdef CONFIG_OT_SNAP_SUPPORT
td_bool isp_pro_trigger_get(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    td_bool enable;

    ret = ioctl(isp_get_fd(vi_pipe), ISP_PRO_TRIGGER_GET, &enable);
    if (ret != TD_SUCCESS) {
        return TD_FALSE;
    }

    return enable;
}
#endif
td_s32 isp_reg_cfg_ctrl(ot_vi_pipe vi_pipe)
{
    td_u8 i;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;
    isp_get_ctx(vi_pipe, isp_ctx);
    isp_regcfg_get_ctx(vi_pipe, reg_cfg);
    isp_check_pointer_return(reg_cfg);
    reg_cfg->reg_cfg.cfg_key.key = 0xFFFFFFFFFFFFFFFF;
    for (i = isp_ctx->block_attr.pre_block_num; i < isp_ctx->block_attr.block_num; i++) {
        (td_void)memcpy_s(&reg_cfg->reg_cfg.alg_reg_cfg[i], sizeof(isp_alg_reg_cfg), &reg_cfg->reg_cfg.alg_reg_cfg[0],
            sizeof(isp_alg_reg_cfg));
    }

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        for (i = 0; i < MIN2(ISP_MAX_BE_NUM, isp_ctx->block_attr.block_num); i++) {
            isp_ctx->special_opt.be_on_stt_update[i] = TD_TRUE;
        }

        for (i = isp_ctx->block_attr.pre_block_num; i < isp_ctx->block_attr.block_num; i++) {
            isp_be_resh_cfg(&reg_cfg->reg_cfg.alg_reg_cfg[i]);
        }
    }

    reg_cfg->reg_cfg.cfg_num = isp_ctx->block_attr.block_num;

    return TD_SUCCESS;
}

static td_void isp_gamma_lut_apb_reg_config(isp_post_be_reg_type *be_reg, isp_gamma_usr_cfg *usr_reg_cfg)
{
    td_u16 j;
    isp_gamma_lut_waddr_write(be_reg, 0);

    for (j = 0; j < GAMMA_REG_NODE_NUM; j++) {
        isp_gamma_lut_wdata_write(be_reg, usr_reg_cfg->gamma_lut[j]);
    }
}

static td_void isp_sharpen_lut_apb_reg_config(isp_post_be_reg_type *be_reg, isp_sharpen_mpi_dyna_reg_cfg *dyna)
{
    td_u16 j;
    isp_sharpen_mhfgaind_waddr_write(be_reg, 0);
    isp_sharpen_mhfgainud_waddr_write(be_reg, 0);
    isp_sharpen_mhfmotgaind_waddr_write(be_reg, 0);
    isp_sharpen_mhfmotgainud_waddr_write(be_reg, 0);

    for (j = 0; j < SHRP_GAIN_LUT_SIZE; j++) {
        isp_sharpen_mhfgaind_wdata_write(be_reg, (dyna->hf_gain_d[j] << 12) + dyna->mf_gain_d[j]);              /* 12 */
        isp_sharpen_mhfgainud_wdata_write(be_reg, (dyna->hf_gain_ud[j] << 12) + dyna->mf_gain_ud[j]);           /* 12 */
        isp_sharpen_mhfmotgaind_wdata_write(be_reg, (dyna->hf_mot_gain_d[j] << 12) + dyna->mf_mot_gain_d[j]);   /* 12 */
        isp_sharpen_mhfmotgaind_wdata_write(be_reg, (dyna->hf_mot_gain_ud[j] << 12) + dyna->mf_mot_gain_ud[j]); /* 12 */
    }
}

static td_void isp_ldci_lut_apb_reg_config(isp_post_be_reg_type *be_reg, const isp_usr_ctx *isp_ctx,
    isp_ldci_dyna_cfg *dyna_reg_cfg, isp_ldci_static_cfg *static_reg_cfg)
{
    td_u16 j;

    isp_ldci_drc_waddr_write(be_reg, 0);
    isp_ldci_cgain_waddr_write(be_reg, 0);
    isp_ldci_de_usm_waddr_write(be_reg, 0);
    isp_ldci_he_waddr_write(be_reg, 0);

    for (j = 0; j < 65; j++) { /* ldci drc lut table number 65 */
        isp_ldci_drc_wdata_write(be_reg, static_reg_cfg->calc_dyn_rng_cmp_lut[j],
            static_reg_cfg->stat_dyn_rng_cmp_lut[j]);
    }

    for (j = 0; j < LDCI_COLOR_GAIN_LUT_SIZE; j++) {
        isp_ldci_cgain_wdata_write(be_reg, dyna_reg_cfg->color_gain_lut[j]);
    }

    for (j = 0; j < LDCI_DE_USM_LUT_SIZE; j++) {
        isp_ldci_de_usm_wdata_write(be_reg, dyna_reg_cfg->usm_pos_lut[j], dyna_reg_cfg->usm_neg_lut[j],
            dyna_reg_cfg->de_lut[j]);
    }

    for (j = 0; j < LDCI_HE_LUT_SIZE; j++) {
        isp_ldci_he_wdata_write(be_reg, dyna_reg_cfg->he_pos_lut[j], dyna_reg_cfg->he_neg_lut[j]);
    }
    if (isp_ctx->frame_cnt == 0) {
        isp_ldci_drc_lut_update_write(be_reg, TD_TRUE);
    }

    isp_ldci_calc_lut_update_write(be_reg, TD_TRUE);
}

static td_s32 isp_lut_apb_reg_config(ot_vi_pipe vi_pipe, isp_reg_cfg_attr *reg_cfg)
{
    td_u8 i;
    isp_post_be_reg_type *be_reg = TD_NULL;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_alg_reg_cfg *alg_reg_cfg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    for (i = 0; i < reg_cfg->reg_cfg.cfg_num; i++) {
        alg_reg_cfg = &reg_cfg->reg_cfg.alg_reg_cfg[i];
        be_reg = (isp_post_be_reg_type *)isp_get_post_be_vir_addr(vi_pipe, i);

        isp_check_pointer_return(be_reg);

        /* gamma */
        isp_gamma_lut_apb_reg_config(be_reg, &alg_reg_cfg->gamma_cfg.usr_reg_cfg);

        /* sharpen */
        isp_sharpen_lut_apb_reg_config(be_reg, &alg_reg_cfg->sharpen_reg_cfg.dyna_reg_cfg.mpi_dyna_reg_cfg);

        /* LDCI */
        isp_ldci_lut_apb_reg_config(be_reg, isp_ctx, &alg_reg_cfg->ldci_reg_cfg.dyna_reg_cfg,
            &alg_reg_cfg->ldci_reg_cfg.static_reg_cfg);
    }

    return TD_SUCCESS;
}

td_s32 isp_switch_reg_set(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_regcfg_get_ctx(vi_pipe, reg_cfg);
        isp_check_pointer_return(reg_cfg);
        isp_lut_apb_reg_config(vi_pipe, reg_cfg);
        ret = isp_reg_cfg_info_init(vi_pipe);
        if (ret != TD_SUCCESS) {
            isp_err_trace("vi_pipe %d isp_reg_cfg_info_init failed \n", vi_pipe);
        }
        if (reg_cfg->reg_cfg.kernel_reg_cfg.cfg_key.key) {
            ret = ioctl(isp_get_fd(vi_pipe), ISP_REG_CFG_SET, &reg_cfg->reg_cfg.kernel_reg_cfg);
            if (ret != TD_SUCCESS) {
                isp_err_trace("Config ISP register Failed with ec %#x!\n", ret);
                return ret;
            }
        }

        isp_ctx->block_attr.pre_block_num = isp_ctx->block_attr.block_num;

        return TD_SUCCESS;
    }

    /* record the register config information to fhy and kernel,and be valid in next frame. */
    ret = isp_reg_cfg_info_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    isp_ctx->para_rec.stitch_sync = TD_TRUE;
    ret = ioctl(isp_get_fd(vi_pipe), ISP_SYNC_INIT_SET, &isp_ctx->para_rec.stitch_sync);
    if (ret != TD_SUCCESS) {
        isp_ctx->para_rec.stitch_sync = TD_FALSE;
        isp_err_trace("ISP[%d] set isp stitch sync failed!\n", vi_pipe);
    }

    ret = isp_all_cfgs_be_buf_init(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("pipe:%d init all be bufs failed \n", vi_pipe);
        return ret;
    }

    isp_ctx->block_attr.pre_block_num = isp_ctx->block_attr.block_num;

    return TD_SUCCESS;
}

td_s32 isp_lut2stt_apb_reg(ot_vi_pipe vi_pipe)
{
    td_s32 ret;
    isp_usr_ctx *isp_ctx = TD_NULL;
    isp_reg_cfg_attr *reg_cfg = TD_NULL;

    isp_get_ctx(vi_pipe, isp_ctx);

    if (is_online_mode(isp_ctx->block_attr.running_mode)) {
        isp_regcfg_get_ctx(vi_pipe, reg_cfg);
        isp_check_pointer_return(reg_cfg);

        ret = isp_lut_apb_reg_config(vi_pipe, reg_cfg);
        if (ret != TD_SUCCESS) {
            isp_err_trace("pipe:%d init all be bufs failed \n", vi_pipe);
            return ret;
        }
    }

    return TD_SUCCESS;
}

td_s32 isp_switch_state_set(ot_vi_pipe vi_pipe)
{
    td_s32 ret;

    ret = ioctl(isp_get_fd(vi_pipe), ISP_BE_SWITCH_FINISH_STATE_SET);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return TD_SUCCESS;
}
