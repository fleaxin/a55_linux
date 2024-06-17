/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "drv_lib.h"
#ifdef KLAD_SWITCH_CPU
#include "sys_ext.h"
#include "ot_debug_klad.h"
#include "ot_common_klad.h"

static td_bool s_klad_secure_cpu = TD_FALSE;
#endif

td_s32 klad_get_cpu_secure_sta(td_void)
{
#ifdef KLAD_SWITCH_CPU
    if (!ckfn_sys_entry()) {
        ot_klad_error("no sys ko\n");
        return OT_ERR_KLAD_FAILED_INIT;
    }

    if (ckfn_sys_is_tee_enable_otp() && !call_sys_is_tee_enable_otp()) {
        s_klad_secure_cpu = TD_TRUE;
    } else {
        s_klad_secure_cpu = TD_FALSE;
    }
#endif
    return TD_SUCCESS;
}

/* KLAD_SECURE_CPU force to tee cpu
 * KLAD_SWITCH_CPU can switch to ree or tee cpu
 * else default ree cpu
 */
td_bool klad_is_secure_cpu(td_void)
{
#if defined(KLAD_SECURE_CPU)
    return TD_TRUE;
#elif defined(KLAD_SWITCH_CPU)
    return s_klad_secure_cpu;
#else
    return TD_FALSE;
#endif
}

