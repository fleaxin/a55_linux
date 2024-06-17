/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_MAU_DIST_H
#define SAMPLE_MAU_DIST_H
#include "ot_type.h"

/*
 * function : show the sample of mau cosine distance
 */
td_void sample_svp_mau_cos_dist(td_char idx_flag);

/*
 * function : show the sample of mau euclidean distance
 */
td_void sample_svp_mau_euclid_dist(td_char out_type_flag);

/*
 * function : mau_cos_dist sample signal handle
 */
td_void sample_svp_mau_cos_dist_handle_sig(td_void);

/*
 * function : mau_euclid_dist sample signal handle
 */
td_void sample_svp_mau_euclid_dist_handle_sig(td_void);

#endif
