/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "sample_dsp_enca.h"
#include <string.h>
#include "ss_mpi_dsp.h"
#include "securec.h"

static td_s32 sample_svp_dsp_morph_proc(ot_svp_dsp_handle *handle, sample_svp_dsp_enca_dilate_arg *arg, td_u32 cmd)
{
    ot_svp_dsp_msg  msg = {0};
    td_u8 *tmp =  NULL;
    errno_t ret;
    const td_u32 body_len = sizeof(ot_svp_src_img) + sizeof(ot_svp_dst_img);
    /* Check parameter,But,we do not it in here */
    msg.cmd = cmd;
    msg.msg_id = 0;
    msg.body = arg->assist_buf.phys_addr;
    msg.body_len = body_len; /* SRC + DST */

    tmp = (td_u8*)(td_uintptr_t)arg->assist_buf.virt_addr;
    ret = memcpy_s(tmp, body_len, &(arg->src), sizeof(ot_svp_src_img));
    if (ret != EOK) {
        return OT_ERR_SVP_DSP_BAD_ADDR;
    }

    tmp += sizeof(ot_svp_src_img);
    ret = memcpy_s(tmp, body_len - sizeof(ot_svp_src_img), &(arg->dst), sizeof(ot_svp_dst_img));
    if (ret != EOK) {
        return OT_ERR_SVP_DSP_BAD_ADDR;
    }

    /* It must flush cache,if the buffer assist_buf.virt_addr malloc with cache! */
    return ss_mpi_svp_dsp_rpc(handle, &msg, arg->dsp_id, arg->pri);
}
/*
 * Encapsulate Dilate 3x3
 */
td_s32 sample_svp_dsp_enca_dilate_3x3(ot_svp_dsp_handle *handle, sample_svp_dsp_enca_dilate_arg *arg)
{
    return sample_svp_dsp_morph_proc(handle, arg, OT_SVP_DSP_CMD_DILATE_3X3);
}

/*
 * Encapsulate Erode 3x3
 */
td_s32 sample_svp_dsp_enca_erode_3x3(ot_svp_dsp_handle *handle, sample_svp_dsp_enca_erode_arg *arg)
{
    return sample_svp_dsp_morph_proc(handle, arg, OT_SVP_DSP_CMD_ERODE_3X3);
}