/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <stdio.h>
#include "ot_isp_debug.h"
#include "isp_ext_reg_access.h"
#include "isp_main.h"
#include "isp_debug.h"
#include "isp_ext_config.h"
#include "ot_mpi_sys.h"

td_s32 isp_dbg_set(ot_vi_pipe vi_pipe, const ot_isp_debug_info *dbg_info)
{
    td_u32 size = 0;

    if (ot_ext_system_sys_debug_enable_read(vi_pipe) && dbg_info->debug_en) {
        isp_err_trace("isp has enabled debug info!\n");
        return TD_FAILURE;
    }

    if (dbg_info->debug_en) {
        if (dbg_info->phys_addr == 0) {
            isp_err_trace("isp lib's debug phys_addr is 0!\n");
            return TD_FAILURE;
        }

        if (dbg_info->depth == 0) {
            isp_err_trace("ae lib's debug depth is 0!\n");
            return TD_FAILURE;
        }
        size = sizeof(ot_isp_debug_attr) + sizeof(ot_isp_debug_status) * dbg_info->depth;
    }

    /* don't clear phyaddr and size when disable dbg info. */
    if (dbg_info->debug_en) {
        ot_ext_system_sys_debug_enable_write(vi_pipe, dbg_info->debug_en);
        ot_ext_system_sys_debug_high_addr_write(vi_pipe, ((dbg_info->phys_addr >> 32) & 0xFFFFFFFF)); /* shift 32 */
        ot_ext_system_sys_debug_low_addr_write(vi_pipe, (dbg_info->phys_addr & 0xFFFFFFFF));
        ot_ext_system_sys_debug_depth_write(vi_pipe, dbg_info->depth);
        ot_ext_system_sys_debug_size_write(vi_pipe, size);
    } else {
        ot_ext_system_sys_debug_enable_write(vi_pipe, dbg_info->debug_en);
    }

    return TD_SUCCESS;
}

td_s32 isp_dbg_get(ot_vi_pipe vi_pipe, ot_isp_debug_info *dbg_info)
{
    td_u64 phy_addr_high;
    td_u64 phy_addr_temp;

    phy_addr_high  = (td_u64)ot_ext_system_sys_debug_high_addr_read(vi_pipe);
    phy_addr_temp  = (td_u64)ot_ext_system_sys_debug_low_addr_read(vi_pipe);
    phy_addr_temp |= (phy_addr_high << 32); /* left shift 32bits */

    dbg_info->phys_addr = phy_addr_temp;
    dbg_info->debug_en = ot_ext_system_sys_debug_enable_read(vi_pipe);
    dbg_info->depth    = ot_ext_system_sys_debug_depth_read(vi_pipe);

    return TD_SUCCESS;
}

td_s32 isp_dbg_run_bgn(isp_dbg_ctrl *dbg, td_u32 frm_cnt)
{
    ot_isp_debug_status  *dbg_status = TD_NULL;

    if (!dbg->debug_en) {
        if (dbg->dbg_attr != TD_NULL) {
            ot_mpi_sys_munmap(dbg->dbg_attr, dbg->size);
            dbg->dbg_attr = TD_NULL;
            dbg->dbg_status = TD_NULL;
        }
        return TD_SUCCESS;
    }

    if ((dbg->debug_en) && (dbg->dbg_attr == TD_NULL)) {
        dbg->dbg_attr = (ot_isp_debug_attr *)ot_mpi_sys_mmap(dbg->phy_addr, dbg->size);
        if (dbg->dbg_attr == TD_NULL) {
            isp_err_trace("isp map debug buf failed!\n");
            return TD_FAILURE;
        }
        dbg->dbg_status = (ot_isp_debug_status *)(dbg->dbg_attr + 1);
    }

    dbg_status = dbg->dbg_status + (frm_cnt % div_0_to_1(dbg->depth));

    dbg_status->frame_num_begain = frm_cnt;

    return TD_SUCCESS;
}

td_s32 isp_dbg_run_end(ot_vi_pipe vi_pipe, isp_dbg_ctrl *dbg, td_u32 frm_cnt)
{
    ot_isp_debug_status *dbg_status = TD_NULL;

    if ((!dbg->debug_en) || (dbg->dbg_status == TD_NULL)) {
        return TD_SUCCESS;
    }
    dbg_status = dbg->dbg_status + (frm_cnt % div_0_to_1(dbg->depth));
    /* record status */
    dbg_status->frame_num_end = frm_cnt;

    isp_black_level_actual_value_read(vi_pipe, &dbg_status->black_level_actual[0]);

    return TD_SUCCESS;
}
