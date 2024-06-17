/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_UTILS_H
#define SAMPLE_UTILS_H

#include <stdio.h>

#include "sample_log.h"
#include "ot_type.h"
#include "ss_mpi_cipher.h"

#define array_size(x)       (sizeof(x) / sizeof(x[0]))

/**
* The function is used to generate random data for the following scenarios:
* 1. The key used in hmac.
* 2. The clear key and iv used in symmetric algorithm like AES.
* 3. The derivatin materials, session key, content key and iv used in klad.
*/
td_s32 get_random_data(td_u8 *buffer, td_u32 size);

typedef enum {
    RSA_TYPE_2048,
    RSA_TYPE_4096
} rsa_type_enum;

#define RSA_2048_KEY_LEN    256
#define RSA_4096_KEY_LEN    512


/**
 * The function is used to generate public-private key pair for rsa.
 */
td_s32 get_rsa_key(ot_cipher_rsa_public_key *public_key, rsa_type_enum rsa_type);

#endif