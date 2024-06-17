/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "ot_type.h"
#include "ss_mpi_cipher.h"
#include "ot_common_cipher.h"

#include "sample_utils.h"

#define GENERATE_TIMES  10

td_s32 sample_rng(td_void)
{
    sample_log("************ test rng ************\n");
    td_s32 ret;
    td_s32 index;
    td_u32 random_number = 0;

    /* 1. cipher init */
    sample_chk_expr_return(ss_mpi_cipher_init(), TD_SUCCESS);

    for (index = 0; index < GENERATE_TIMES; index++) {
        ret = ss_mpi_cipher_get_random_num(&random_number);
        if (ret == OT_ERR_CIPHER_NO_AVAILABLE_RNG) {
            /* "there is no ramdom number available now. try again! */
            index--;
            continue;
        }

        if (ret != TD_SUCCESS) {
            sample_err("ss_mpi_cipher_get_random_num failed!\n");
            goto __CIPHER_DEINIT__;
        }
    }
    sample_log("************ test tng success ************\n");
__CIPHER_DEINIT__:
    (td_void)ss_mpi_cipher_deinit();
    return ret;
}

