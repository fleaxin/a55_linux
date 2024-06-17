/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISP_HNR_EXT_CONFIG_H
#define ISP_HNR_EXT_CONFIG_H

#include "isp_vreg.h"
#include "ot_common.h"
#include "isp_ext_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static __inline td_void ot_ext_system_hnr_en_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe), data);
}

static __inline td_bool ot_ext_system_hnr_en_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe)) & 0x1);
}

static __inline td_void ot_ext_system_hnr_sfs_write(ot_vi_pipe vi_pipe, td_u32 data)
{
    iowr_32direct(vreg_hnr_base(vi_pipe) + 0x4, data);
}

static __inline td_u32 ot_ext_system_hnr_sfs_read(ot_vi_pipe vi_pipe)
{
    return (iord_32direct(vreg_hnr_base(vi_pipe) + 0x4));
}

static __inline td_void ot_ext_system_hnr_tfs_write(ot_vi_pipe vi_pipe, td_u32 data)
{
    iowr_32direct(vreg_hnr_base(vi_pipe) + 0x8, data);
}

static __inline td_u32 ot_ext_system_hnr_tfs_read(ot_vi_pipe vi_pipe)
{
    return (iord_32direct(vreg_hnr_base(vi_pipe) + 0x8));
}

static __inline td_void ot_ext_system_hnr_eds_write(ot_vi_pipe vi_pipe, td_u32 data)
{
    iowr_32direct(vreg_hnr_base(vi_pipe) + 0xc, data);
}

static __inline td_u32 ot_ext_system_hnr_eds_read(ot_vi_pipe vi_pipe)
{
    return (iord_32direct(vreg_hnr_base(vi_pipe) + 0xc));
}

static __inline td_void ot_ext_system_hnr_mds_write(ot_vi_pipe vi_pipe, td_u32 data)
{
    iowr_32direct(vreg_hnr_base(vi_pipe) + 0x10, data);
}

static __inline td_u32 ot_ext_system_hnr_mds_read(ot_vi_pipe vi_pipe)
{
    return (iord_32direct(vreg_hnr_base(vi_pipe) + 0x10));
}

static __inline td_void ot_ext_system_hnr_task_en_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x14, data);
}

static __inline td_bool ot_ext_system_hnr_task_en_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x14) & 0x1);
}

static __inline td_void ot_ext_system_hnr_pos_front_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x18, data);
}

static __inline td_bool ot_ext_system_hnr_pos_front_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x18) & 0x1);
}

static __inline td_void ot_ext_system_hnr_en_user_write_0(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x20, data);
}

static __inline td_bool ot_ext_system_hnr_en_user_read_0(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x20) & 0x1);
}

static __inline td_void ot_ext_system_hnr_en_user_write_1(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x24, data);
}

static __inline td_bool ot_ext_system_hnr_en_user_read_1(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x24) & 0x1);
}

static __inline td_void ot_ext_system_hnr_en_user_write_2(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x28, data);
}

static __inline td_bool ot_ext_system_hnr_en_user_read_2(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x28) & 0x1);
}

static __inline td_void ot_ext_system_hnr_advanced_mode_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x2c, data);
}

static __inline td_bool ot_ext_system_hnr_advanced_mode_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x2c) & 0x1);
}

static __inline td_void ot_ext_system_hnr_bnr_bypass_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x30, data);
}

static __inline td_bool ot_ext_system_hnr_bnr_bypass_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x30) & 0x1);
}

static __inline td_void ot_ext_system_hnr_bnr_bypass_status_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x34, data);
}

static __inline td_bool ot_ext_system_hnr_bnr_bypass_status_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x34) & 0x1);
}

static __inline td_void ot_ext_system_hnr_ref_mode_write(ot_vi_pipe vi_pipe, td_u32 data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x38, data);
}

static __inline td_bool ot_ext_system_hnr_ref_mode_read(ot_vi_pipe vi_pipe)
{
    return (td_u32)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x38));
}

static __inline td_void ot_ext_system_hnr_normal_blend_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x3c, data);
}

static __inline td_bool ot_ext_system_hnr_normal_blend_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x3c) & 0x1);
}

static __inline td_void ot_ext_system_hnr_normal_blend_status_write(ot_vi_pipe vi_pipe, td_bool data)
{
    iowr_8direct(vreg_hnr_base(vi_pipe) + 0x40, data);
}

static __inline td_bool ot_ext_system_hnr_normal_blend_status_read(ot_vi_pipe vi_pipe)
{
    return (td_bool)(iord_8direct(vreg_hnr_base(vi_pipe) + 0x40) & 0x1);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* ISP_HNR_EXT_CONFIG_H */
