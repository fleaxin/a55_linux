/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "isp_drv_vreg.h"
#include "ot_osal.h"
#include "mm_ext.h"
#include "isp_vreg.h"
#include "securec.h"

static drv_vreg_args g_vreg[OT_ISP_MAX_PIPE_NUM][OT_ISP_VREG_MAX_NUM] = {0};
drv_vreg_args *vreg_drv_search(ot_vi_pipe vi_pipe, td_phys_addr_t base_addr)
{
    td_s32  i;

    for (i = 0; i < OT_ISP_VREG_MAX_NUM; i++) {
        if ((g_vreg[vi_pipe][i].phy_addr != 0) &&
            (base_addr == g_vreg[vi_pipe][i].base_addr)) {
            return &g_vreg[vi_pipe][i];
        }
    }

    return TD_NULL;
}

drv_vreg_args *vreg_drv_query(td_phys_addr_t base_addr)
{
    td_s32  i, j;

    for (j = 0; j < OT_ISP_MAX_PIPE_NUM; j++) {
        for (i = 0; i < OT_ISP_VREG_MAX_NUM; i++) {
            if ((g_vreg[j][i].phy_addr != 0) &&
                (base_addr == g_vreg[j][i].base_addr)) {
                return &g_vreg[j][i];
            }
        }
    }

    return TD_NULL;
}

td_s32 vreg_drv_init(ot_vi_pipe vi_pipe, td_phys_addr_t base_addr, td_u64 size)
{
    td_s32 ret;
    td_s32  i;
    td_phys_addr_t phy_addr;
    td_u8 *vir_addr = TD_NULL;
    td_char name[MAX_VREG_NAME_LEN] = {0};
    drv_vreg_args *vreg_args = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    /* check param */
    if (size == 0) {
        isp_err_trace("The vreg's size is 0!\n");
        return TD_FAILURE;
    }

    vreg_args = vreg_drv_search(vi_pipe, base_addr);
    if (vreg_args != TD_NULL) {
        isp_err_trace("The vreg of base_addr 0x%lx has registered!\n", (td_ulong)base_addr);
        return TD_FAILURE;
    }

    /* search pos */
    for (i = 0; i < OT_ISP_VREG_MAX_NUM; i++) {
        if (g_vreg[vi_pipe][i].phy_addr == 0) {
            vreg_args = &g_vreg[vi_pipe][i];
            break;
        }
    }

    if (vreg_args == TD_NULL) {
        isp_err_trace("The vreg is too many, can't register!\n");
        return TD_FAILURE;
    }

    /* Mmz malloc memory */
    ret =  snprintf_s(name, sizeof(name), sizeof(name) - 1, "ISP[%d].Vreg[%d]", vi_pipe, i);
    if (ret < 0) {
        return TD_FAILURE;
    }

    ret = cmpi_mmz_malloc_nocache(TD_NULL, name, &phy_addr, (td_void **)&vir_addr, size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("alloc virt regs buf err\n");
        return TD_FAILURE;
    }

    (td_void)memset_s(vir_addr, size, 0, size);

    vreg_args->phy_addr  = phy_addr;
    vreg_args->virt_addr   = (td_void *)vir_addr;
    vreg_args->base_addr = base_addr;

    return TD_SUCCESS;
}

td_s32 vreg_drv_exit(ot_vi_pipe vi_pipe, td_phys_addr_t base_addr)
{
    drv_vreg_args *vreg_args = TD_NULL;

    isp_check_pipe_return(vi_pipe);

    vreg_args = vreg_drv_search(vi_pipe, base_addr);
    if (vreg_args == TD_NULL) {
        isp_warn_trace("The vreg of base_addr 0x%lx has not registered!\n", (td_ulong)base_addr);
        return TD_FAILURE;
    }

    if (vreg_args->phy_addr != 0) {
        cmpi_mmz_free(vreg_args->phy_addr, vreg_args->virt_addr);
        vreg_args->phy_addr  = 0;
        vreg_args->size     = 0;
        vreg_args->base_addr = 0;
        vreg_args->virt_addr   = TD_NULL;
    }

    return TD_SUCCESS;
}

td_s32 vreg_drv_get_addr(ot_vi_pipe vi_pipe, td_phys_addr_t base_addr, td_phys_addr_t *phy_addr)
{
    drv_vreg_args *vreg_args = TD_NULL;
    ot_unused(vi_pipe);

    vreg_args = vreg_drv_query(base_addr);
    if (vreg_args == TD_NULL) {
        isp_warn_trace("The vreg of base_addr 0x%lx has not registered!\n", (td_ulong)base_addr);
        return TD_FAILURE;
    }

    *phy_addr = vreg_args->phy_addr;

    return TD_SUCCESS;
}

td_s32 vreg_drv_release_all(ot_vi_pipe vi_pipe)
{
    td_s32  i;

    isp_check_pipe_return(vi_pipe);

    for (i = 0; i < OT_ISP_VREG_MAX_NUM; i++) {
        if (g_vreg[vi_pipe][i].phy_addr != 0) {
            cmpi_mmz_free(g_vreg[vi_pipe][i].phy_addr, g_vreg[vi_pipe][i].virt_addr);
            g_vreg[vi_pipe][i].phy_addr  = 0;
            g_vreg[vi_pipe][i].base_addr = 0;
            g_vreg[vi_pipe][i].size     = 0;
            g_vreg[vi_pipe][i].virt_addr   = TD_NULL;
        }
    }

    return TD_SUCCESS;
}

long vreg_drv_ioctl(unsigned int cmd, unsigned long arg, void *private_data)
{
    ot_vi_pipe vi_pipe;
    unsigned int *argp = (unsigned int *)(td_uintptr_t)arg;
    drv_vreg_args *vreg_args = TD_NULL;
    vi_pipe = isp_get_dev(private_data);

    switch (cmd) {
        case VREG_DRV_FD:
            isp_check_pointer_return(argp);
            *((td_u32 *)(private_data)) = *(td_u32 *)argp;

            return TD_SUCCESS;

        /* malloc memory for vregs, and record information in kernel. */
        case VREG_DRV_INIT:
            isp_check_pointer_return(argp);
            vreg_args = (drv_vreg_args *)argp;

            return vreg_drv_init(vi_pipe, vreg_args->base_addr, vreg_args->size);

        /* free the memory of vregs, and clean information in kernel. */
        case VREG_DRV_EXIT:
            isp_check_pointer_return(argp);
            vreg_args = (drv_vreg_args *)argp;

            return vreg_drv_exit(vi_pipe, vreg_args->base_addr);

        /* free the memory of vregs, and clean information in kernel. */
        case VREG_DRV_RELEASE_ALL:
            isp_check_pointer_return(argp);
            vreg_args = (drv_vreg_args *)argp;

            if ((vreg_args->size == ISP_VREG_SIZE) &&
                (vreg_args->base_addr == ISP_VREG_BASE)) {
                vreg_drv_release_all(vi_pipe);
            }

            return TD_SUCCESS;

        /* get the mapping relation between vreg addr and physical addr. */
        case VREG_DRV_GETADDR:
            isp_check_pointer_return(argp);
            vreg_args = (drv_vreg_args *)argp;

            return vreg_drv_get_addr(vi_pipe, vreg_args->base_addr, &vreg_args->phy_addr);

        default:
            return TD_FAILURE;
    }

    return TD_SUCCESS;
}

