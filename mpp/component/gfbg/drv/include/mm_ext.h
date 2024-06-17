/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef MM_EXT_H
#define MM_EXT_H

#include "ot_osal.h"
#include "ot_math.h"
#include "ot_common.h"
#include "osal_mmz.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_phys_addr_t cmpi_mmz_malloc(const td_char *mmz_name, const td_char *buf_name, td_ulong ul_size);
td_phys_addr_t cmpi_mmz_malloc_fix_addr(const td_char *buf_name, td_phys_addr_t start, td_ulong size);
td_void cmpi_mmz_free(td_phys_addr_t phy_addr, td_void *vir_addr);
td_s32 cmpi_mmz_malloc_nocache(const td_char *cp_mmz_name, const td_char *buf_name,
    td_phys_addr_t *phy_addr, td_void **pp_vir_addr, td_ulong ul_len);
td_s32 cmpi_mmz_malloc_cached(const td_char *cp_mmz_name, const td_char *buf_name,
    td_phys_addr_t *phy_addr, td_void **pp_vir_addr, td_ulong ul_len);
td_void *cmpi_remap_cached(td_phys_addr_t phy_addr, td_ulong ul_size);
td_void *cmpi_remap_nocache(td_phys_addr_t phy_addr, td_ulong ul_size);
td_void cmpi_unmap(td_void *virt_addr);

static inline td_s32 cmpi_check_mmz_phy_addr(td_phys_addr_t phy_addr, td_ulong len)
{
    /* if address in mmz of current system */
    if (ot_is_phys_in_mmz((td_ulong)phy_addr, len)) {
        /* if in other system */
        if (ot_map_mmz_check_phys((td_ulong)phy_addr, len)) {
            return TD_FAILURE;
        }
    }

    return TD_SUCCESS;
}

static inline td_bool read_user_linear_space_valid(td_u8 *addr_start, td_u32 len)
{
    td_u8 check;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return TD_FALSE;
    }

    if (!osal_access_ok(OSAL_VERIFY_READ, addr_start, len)) {
        return TD_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_from_user(&check, addr_end, 1)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static inline td_bool write_user_linear_space_valid(td_u8 *addr_start, td_u32 len)
{
    td_u8 check = 0;
    td_u8 *addr_end = TD_NULL;

    if (len == 0) {
        return TD_FALSE;
    }

    if (!osal_access_ok(OSAL_VERIFY_WRITE, addr_start, len)) {
        return TD_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_to_user(addr_end, &check, 1)) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
