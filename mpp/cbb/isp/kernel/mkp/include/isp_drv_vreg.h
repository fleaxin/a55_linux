/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_DRV_VREG_H
#define OT_DRV_VREG_H

#include "osal_ioctl.h"
#include "ot_common_3a.h"
#include "mkp_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define IOC_TYPE_ISP         'I'

#define MAX_VREG_NAME_LEN    16

typedef enum {
    IOC_NR_VREG_FD = 200,
    IOC_NR_VREG_INIT,
    IOC_NR_VREG_EXIT,
    IOC_NR_VREG_RELEASE_ALL,
    IOC_NR_VREG_GETADDR,

    IOC_NR_VREG_BUTT,
} ioc_nr_vreg;

typedef struct {
    td_u64  size;
    td_phys_addr_t  base_addr;
    td_phys_addr_t  phy_addr;
    td_void *virt_addr;
} drv_vreg_args;

#define VREG_DRV_FD          _IOW(IOC_TYPE_ISP, IOC_NR_VREG_FD, td_s32)
#define VREG_DRV_INIT        _IOW(IOC_TYPE_ISP, IOC_NR_VREG_INIT, drv_vreg_args)
#define VREG_DRV_EXIT        _IOW(IOC_TYPE_ISP, IOC_NR_VREG_EXIT, drv_vreg_args)
#define VREG_DRV_RELEASE_ALL _IOW(IOC_TYPE_ISP, IOC_NR_VREG_RELEASE_ALL, drv_vreg_args)
#define VREG_DRV_GETADDR     _IOWR(IOC_TYPE_ISP, IOC_NR_VREG_GETADDR, drv_vreg_args)

long vreg_drv_ioctl(unsigned int cmd, unsigned long arg, void *private_data);
td_s32 vreg_drv_release_all(ot_vi_pipe vi_pipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef ISP_VREG_H */
