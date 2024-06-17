/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_mpi_cipher.h"
#include "sys_cipher.h"
#include "securec.h"
#include "drv_cipher_debug.h"

#define BYTE_BITS               8
#define CIPHER_MAX_MULTIPAD_NUM 5000
#define TRNG_TIMEOUT            10000

/*
 * Read E in public key from array to U32,
 * so only use last byte0~byte3, others are zero
 */
#define cipher_get_pub_exponent(pub_e, pub_rsa)                 \
    do {                                                        \
        td_u8 *_buf = (pub_rsa)->e;                             \
        td_u8 *_pub = (td_u8 *)(pub_e);                         \
        td_u32 _len = (pub_rsa)->e_len;                         \
        td_u32 _i;                                              \
        for (_i = 0; _i < crypto_min(WORD_WIDTH, _len); _i++) { \
            _pub[WORD_WIDTH - _i - 1] = _buf[_len - _i - 1];    \
        }                                                       \
    } while (0)

/*
 * brief  Init the cipher device.
 */
td_s32 ot_mpi_cipher_init(td_void)
{
    return sys_symc_init();
}

/*
 * brief  Deinit the cipher device.
 */
td_s32 ot_mpi_cipher_deinit(td_void)
{
    return sys_symc_deinit();
}

/*
 * brief Obtain a cipher handle for encryption and decryption.
 */
td_s32 ot_mpi_cipher_create(td_handle *handle, const ot_cipher_attr *cipher_attr)
{
    td_s32 ret;
    td_u32 id = 0;

    func_enter();

    chk_ptr_err_return(handle);
    chk_ptr_err_return(cipher_attr);
    chk_param_err_return(cipher_attr->cipher_type >= OT_CIPHER_TYPE_BUTT);

    ret = sys_symc_create(&id, cipher_attr->cipher_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_create, ret);
        return ret;
    }

    *handle = (td_handle)id;

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Destroy the existing cipher handle.
 */
td_s32 ot_mpi_cipher_destroy(td_handle handle)
{
    td_s32 ret;

    func_enter();

    ret = sys_symc_destroy(handle);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_destroy, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Configures the cipher control information.
 */
td_s32 ot_mpi_cipher_set_cfg(td_handle handle, const ot_cipher_ctrl *ctrl)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(ctrl);

    ret = sys_symc_config(handle, ctrl);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_config, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_get_cfg(td_handle handle, ot_cipher_ctrl *ctrl)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(ctrl);

    ret = sys_symc_get_config(handle, ctrl);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_get_config, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Performs encryption.
 */
td_s32 ot_mpi_cipher_encrypt(td_handle handle,
    td_phys_addr_t src_phys_addr, td_phys_addr_t dst_phys_addr, td_u32 byte_len)
{
    td_s32 ret;
    compat_addr in;
    compat_addr out;

    func_enter();

    addr_u64(in) = src_phys_addr;
    addr_u64(out) = dst_phys_addr;

    ret = sys_symc_crypto(handle, in, out, byte_len, SYMC_OPERATION_ENCRYPT);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_crypto, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Performs description.
 */
td_s32 ot_mpi_cipher_decrypt(td_handle handle,
    td_phys_addr_t src_phys_addr, td_phys_addr_t dst_phys_addr, td_u32 byte_len)
{
    td_s32 ret;
    compat_addr in;
    compat_addr out;

    func_enter();

    addr_u64(in) = src_phys_addr;
    addr_u64(out) = dst_phys_addr;

    ret = sys_symc_crypto(handle, in, out, byte_len, SYMC_OPERATION_DECRYPT);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_crypto, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Performs encryption.
 */
td_s32 ot_mpi_cipher_encrypt_virt(td_handle handle, const td_u8 *src_data, td_u8 *dst_data, td_u32 byte_len)
{
    td_s32 ret;
    compat_addr in;
    compat_addr out;

    func_enter();

    chk_ptr_err_return(src_data);
    chk_ptr_err_return(dst_data);

    addr_via_const(in) = src_data;
    addr_via(out) = dst_data;

    ret = sys_symc_crypto(handle, in, out, byte_len, SYMC_OPERATION_ENCRYPT_VIA);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_crypto, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Performs decryption.
 */
td_s32 ot_mpi_cipher_decrypt_virt(td_handle handle, const td_u8 *src_data, td_u8 *dst_data, td_u32 byte_len)
{
    td_s32 ret;
    compat_addr in;
    compat_addr out;

    func_enter();

    chk_ptr_err_return(src_data);
    chk_ptr_err_return(dst_data);

    addr_via_const(in) = src_data;
    addr_via(out) = dst_data;

    ret = sys_symc_crypto(handle, in, out, byte_len, SYMC_OPERATION_DECRYPT_VIA);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_crypto, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Encrypt multiple packaged data.
 */
td_s32 ot_mpi_cipher_encrypt_multi_pack(td_handle handle, const ot_cipher_data *data_pack, td_u32 data_pack_num)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(data_pack);
    chk_param_err_return(data_pack_num == 0x00);
    chk_param_err_return(data_pack_num >= CIPHER_MAX_MULTIPAD_NUM);

    ret = sys_symc_crypto_multi(handle, data_pack, data_pack_num, SYMC_OPERATION_ENCRYPT);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_crypto_multi, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

/*
 * brief Encrypt multiple packaged data.
 */
td_s32 ot_mpi_cipher_decrypt_multi_pack(td_handle handle, const ot_cipher_data *data_pack, td_u32 data_pack_num)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(data_pack);
    chk_param_err_return(data_pack_num == 0x00);
    chk_param_err_return(data_pack_num >= CIPHER_MAX_MULTIPAD_NUM);

    ret = sys_symc_crypto_multi(handle, data_pack, data_pack_num, SYMC_OPERATION_DECRYPT);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_symc_crypto_multi, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_get_tag(td_handle handle, td_u8 *tag, td_u32 tag_len)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(tag);

    ret = sys_aead_get_tag(handle, tag, tag_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_aead_get_tag, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_get_random_num(td_u32 *random_num)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(random_num);

    ret = sys_trng_get_random(random_num, TRNG_TIMEOUT);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_trng_get_random, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_hash_init(const ot_cipher_hash_attr *hash_attr, td_handle *handle)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(hash_attr);
    chk_ptr_err_return(handle);

    ret = sys_hash_start(handle, hash_attr->sha_type, hash_attr->hmac_key, hash_attr->hmac_key_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_hash_start, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_hash_update(td_handle handle, const td_u8 *in_data, td_u32 in_data_len)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(in_data);

    ret = sys_hash_update(handle, in_data, in_data_len, HASH_CHUNCK_SRC_USER);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_hash_update, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_hash_final(td_handle handle, td_u8 *out_hash, td_u32 out_hash_len)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(out_hash);
    chk_param_err_return(out_hash_len == 0);
    chk_param_err_return(handle == (td_handle)OT_INVALID_HANDLE);

    ret = sys_hash_finish(handle, out_hash, out_hash_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_hash_finish, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 mpi_rsa_chk_private_key(const ot_cipher_rsa_private_key *rsa_key)
{
    chk_ptr_err_return(rsa_key);
    chk_ptr_err_return(rsa_key->n);
    chk_param_err_return(rsa_key->n_len < RSA_MIN_KEY_LEN);
    chk_param_err_return(rsa_key->n_len > RSA_MAX_KEY_LEN);

    if (rsa_key->d == TD_NULL) {
        chk_ptr_err_return(rsa_key->p);
        chk_ptr_err_return(rsa_key->q);
        chk_ptr_err_return(rsa_key->dp);
        chk_ptr_err_return(rsa_key->dq);
        chk_ptr_err_return(rsa_key->qp);
        chk_param_err_return((rsa_key->n_len >> 1) != rsa_key->p_len);
        chk_param_err_return((rsa_key->n_len >> 1) != rsa_key->q_len);
        chk_param_err_return((rsa_key->n_len >> 1) != rsa_key->dp_len);
        chk_param_err_return((rsa_key->n_len >> 1) != rsa_key->dq_len);
        chk_param_err_return((rsa_key->n_len >> 1) != rsa_key->qp_len);
    } else {
        chk_param_err_return(rsa_key->n_len != rsa_key->d_len);
    }

    return TD_SUCCESS;
}

static td_void mpi_rsa_init_private_key(cryp_rsa_key *key, const ot_cipher_rsa_private_key *rsa_key)
{
    (td_void)memset_s(key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));
    key->public  = TD_FALSE;
    key->klen    = rsa_key->n_len;
    key->n       = rsa_key->n;
    key->d       = rsa_key->d;
    key->p       = rsa_key->p;
    key->q       = rsa_key->q;
    key->dp      = rsa_key->dp;
    key->dq      = rsa_key->dq;
    key->qp      = rsa_key->qp;
}

static td_s32 mpi_rsa_get_crypto_attr(ot_cipher_rsa_encrypt_scheme *rsa_scheme,
    ot_cipher_rsa_scheme scheme, ot_cipher_hash_type sha_type)
{
    if (scheme == OT_CIPHER_RSA_SCHEME_PKCS1_V15) {
        *rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_PKCS1_V1_5;
    } else if (scheme == OT_CIPHER_RSA_SCHEME_PKCS1_V21) {
        switch (sha_type) {
            case OT_CIPHER_HASH_TYPE_SHA256:
                *rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA256;
                break;
            case OT_CIPHER_HASH_TYPE_SHA384:
                *rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA384;
                break;
            case OT_CIPHER_HASH_TYPE_SHA512:
                *rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_RSAES_OAEP_SHA512;
                break;
            default:
                log_error("Invalid sha type %d\n", sha_type);
                return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        log_error("Invalid scheme %d\n", scheme);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_rsa_public_encrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_common_data *plain_txt, ot_cipher_common_data *cipher_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    sys_rsa_crypto_data rsa_data = {0};
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;

    chk_ptr_err_return(rsa_key);
    chk_ptr_err_return(rsa_key->n);
    chk_ptr_err_return(rsa_key->e);
    chk_param_err_return((rsa_key->n_len < RSA_MIN_KEY_LEN) || (rsa_key->n_len > RSA_MAX_KEY_LEN));
    chk_param_err_return(rsa_key->n_len < rsa_key->e_len);

    chk_ptr_err_return(plain_txt);
    chk_ptr_err_return(plain_txt->data);
    chk_param_err_return(plain_txt->data_len == 0);

    chk_ptr_err_return(cipher_txt);
    chk_ptr_err_return(cipher_txt->data);
    chk_param_err_return((cipher_txt->data_len == 0) || (cipher_txt->data_len < rsa_key->n_len));

    rsa_data.in = plain_txt->data;
    rsa_data.in_len = plain_txt->data_len;
    rsa_data.out = cipher_txt->data;
    rsa_data.out_buf_len = rsa_key->n_len;
    rsa_data.out_len = &cipher_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    key.public = TD_TRUE;
    key.klen   = rsa_key->n_len;
    key.n      = rsa_key->n;

    cipher_get_pub_exponent(&key.e, rsa_key);

    ret = sys_rsa_encrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_rsa_encrypt, ret);
    }
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}

td_s32 ot_mpi_cipher_rsa_private_decrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_common_data *cipher_txt, ot_cipher_common_data *plain_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    sys_rsa_crypto_data rsa_data = {0};
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;

    ret = mpi_rsa_chk_private_key(rsa_key);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_private_key, ret);
        return ret;
    }

    chk_ptr_err_return(plain_txt);
    chk_ptr_err_return(plain_txt->data);
    chk_param_err_return(plain_txt->data_len == 0);

    chk_ptr_err_return(cipher_txt);
    chk_ptr_err_return(cipher_txt->data);
    chk_param_err_return((cipher_txt->data_len == 0) || (cipher_txt->data_len != rsa_key->n_len));

    rsa_data.in = cipher_txt->data;
    rsa_data.in_len = cipher_txt->data_len;
    rsa_data.out = plain_txt->data;
    rsa_data.out_buf_len = (plain_txt->data_len < rsa_key->n_len) ? plain_txt->data_len : rsa_key->n_len;
    rsa_data.out_len = &plain_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    mpi_rsa_init_private_key(&key, rsa_key);

    ret = sys_rsa_decrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_rsa_decrypt, ret);
    }
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}

td_s32 ot_mpi_cipher_rsa_private_encrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_common_data *plain_txt, ot_cipher_common_data *cipher_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    sys_rsa_crypto_data rsa_data = {0};
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;

    ret = mpi_rsa_chk_private_key(rsa_key);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_private_key, ret);
        return ret;
    }

    chk_ptr_err_return(plain_txt);
    chk_ptr_err_return(plain_txt->data);
    chk_param_err_return(plain_txt->data_len == 0);

    chk_ptr_err_return(cipher_txt);
    chk_ptr_err_return(cipher_txt->data);
    chk_param_err_return((cipher_txt->data_len == 0) || (cipher_txt->data_len < rsa_key->n_len));

    rsa_data.in = plain_txt->data;
    rsa_data.in_len = plain_txt->data_len;
    rsa_data.out = cipher_txt->data;
    rsa_data.out_buf_len = rsa_key->n_len;
    rsa_data.out_len = &cipher_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    mpi_rsa_init_private_key(&key, rsa_key);

    ret = sys_rsa_encrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_rsa_encrypt, ret);
    }
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}

td_s32 ot_mpi_cipher_rsa_public_decrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_common_data *cipher_txt, ot_cipher_common_data *plain_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    sys_rsa_crypto_data rsa_data = {0};
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;

    chk_ptr_err_return(rsa_key);
    chk_ptr_err_return(rsa_key->n);
    chk_ptr_err_return(rsa_key->e);
    chk_param_err_return((rsa_key->n_len < RSA_MIN_KEY_LEN) || (rsa_key->n_len > RSA_MAX_KEY_LEN));
    chk_param_err_return(rsa_key->n_len < rsa_key->e_len);

    chk_ptr_err_return(plain_txt);
    chk_ptr_err_return(plain_txt->data);
    chk_param_err_return(plain_txt->data_len == 0);

    chk_ptr_err_return(cipher_txt);
    chk_ptr_err_return(cipher_txt->data);
    chk_param_err_return((cipher_txt->data_len == 0) || (cipher_txt->data_len != rsa_key->n_len));

    rsa_data.in = cipher_txt->data;
    rsa_data.in_len = cipher_txt->data_len;
    rsa_data.out = plain_txt->data;
    rsa_data.out_buf_len = (plain_txt->data_len < rsa_key->n_len) ? plain_txt->data_len : rsa_key->n_len;
    rsa_data.out_len = &plain_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    key.public  = TD_TRUE;
    key.klen    = rsa_key->n_len;
    key.n       = rsa_key->n;
    cipher_get_pub_exponent(&key.e, rsa_key);

    ret = sys_rsa_decrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_rsa_decrypt, ret);
    }
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}

static td_s32 mpi_rsa_chk_sign_param(const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_sign_in_data *rsa_data, const ot_cipher_common_data *sign_data)
{
    td_s32 ret;
    ret = mpi_rsa_chk_private_key(rsa_key);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_private_key, ret);
        return ret;
    }

    /* check sign data */
    chk_ptr_err_return(rsa_data);
    chk_ptr_err_return(rsa_data->input);
    chk_param_err_return(rsa_data->sign_type >= OT_CIPHER_SIGN_TYPE_BUTT);
    chk_param_err_return(rsa_data->input_len == 0);

    chk_ptr_err_return(sign_data);
    chk_ptr_err_return(sign_data->data);
    chk_param_err_return((sign_data->data_len == 0) || (sign_data->data_len < rsa_key->n_len));

    return TD_SUCCESS;
}

static td_s32 mpi_rsa_get_sign_attr(ot_cipher_rsa_sign_scheme *rsa_scheme,
    td_u32 *hash_len, ot_cipher_rsa_scheme scheme, ot_cipher_hash_type sha_type)
{
    if (scheme == OT_CIPHER_RSA_SCHEME_PKCS1_V15) {
        switch (sha_type) {
            case OT_CIPHER_HASH_TYPE_SHA256:
                *rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256;
                *hash_len = SHA256_RESULT_SIZE;
                break;
            case OT_CIPHER_HASH_TYPE_SHA384:
                *rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384;
                *hash_len = SHA384_RESULT_SIZE;
                break;
            case OT_CIPHER_HASH_TYPE_SHA512:
                *rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512;
                *hash_len = SHA512_RESULT_SIZE;
                break;
            default:
                log_error("Invalid sha type %d\n", sha_type);
                return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else if (scheme == OT_CIPHER_RSA_SCHEME_PKCS1_V21) {
        switch (sha_type) {
            case OT_CIPHER_HASH_TYPE_SHA256:
                *rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256;
                *hash_len = SHA256_RESULT_SIZE;
                break;
            case OT_CIPHER_HASH_TYPE_SHA384:
                *rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384;
                *hash_len = SHA384_RESULT_SIZE;
                break;
            case OT_CIPHER_HASH_TYPE_SHA512:
                *rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512;
                *hash_len = SHA512_RESULT_SIZE;
                break;
            default:
                log_error("Invalid sha type %d\n", sha_type);
                return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        log_error("Invalid scheme %d\n", scheme);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    return TD_SUCCESS;
}

static td_s32 mpi_cipher_hash(ot_cipher_hash_type sha_type,
    const td_u8 *in_data, td_u32 in_len, td_u8 *hash_data, td_u32 hash_len)
{
    td_s32 ret;
    td_handle hash_id;

    ret = sys_hash_start(&hash_id, sha_type, TD_NULL, 0);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_hash_start, ret);
        return ret;
    }

    ret = sys_hash_update(hash_id, in_data, in_len, HASH_CHUNCK_SRC_USER);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_hash_update, ret);
        return ret;
    }

    ret = sys_hash_finish(hash_id, hash_data, hash_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_hash_finish, ret);
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_rsa_sign(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_sign_in_data *rsa_data, ot_cipher_common_data *sign_data)
{
    td_s32 ret;
    cryp_rsa_key key;
    td_u8 hash[HASH_RESULT_MAX_SIZE] = {0};
    sys_rsa_sign_data sys_data;
    ot_cipher_rsa_sign_scheme rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_BUTT;

    ret = mpi_rsa_chk_sign_param(rsa_key, rsa_data, sign_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_sign_param, ret);
        return ret;
    }

    (td_void)memset_s(&sys_data, sizeof(sys_data), 0, sizeof(sys_data));
    ret = mpi_rsa_get_sign_attr(&rsa_scheme, &sys_data.hash_len, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_sign_attr, ret);
        return ret;
    }

    /* hash value of context, if OT_CIPHER_SIGN_TYPE_HASH, compute hash = Hash(in_data). */
    if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_MSG) {
        ret = mpi_cipher_hash(sha_type, rsa_data->input, rsa_data->input_len, hash, sizeof(hash));
        if (ret != TD_SUCCESS) {
            print_func_errno(mpi_cipher_hash, ret);
            return ret;
        }
        sys_data.hash = hash;
    } else if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_HASH) {
        sys_data.hash = rsa_data->input;
        if (rsa_data->input_len != sys_data.hash_len) {
            log_error("Invalid input len %u\n", rsa_data->input_len);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        log_error("Invalid sign type %d\n", rsa_data->sign_type);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    sys_data.sign = sign_data->data;
    sys_data.sign_len = rsa_key->n_len;

    mpi_rsa_init_private_key(&key, rsa_key);

    ret = sys_rsa_sign_hash(rsa_scheme, &key, &sys_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_rsa_sign_hash, ret);
    }
    sign_data->data_len = sys_data.sign_len;
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}

static td_s32 mpi_rsa_chk_verify_param(const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_sign_in_data *rsa_data, const ot_cipher_common_data *sign_data)
{
    /* check public key */
    chk_ptr_err_return(rsa_key);
    chk_ptr_err_return(rsa_key->n);
    chk_ptr_err_return(rsa_key->e);
    chk_param_err_return((rsa_key->n_len < RSA_MIN_KEY_LEN) || (rsa_key->n_len > RSA_MAX_KEY_LEN));
    chk_param_err_return(rsa_key->n_len < rsa_key->e_len);

    /* check verify data */
    chk_ptr_err_return(rsa_data);
    chk_ptr_err_return(rsa_data->input);
    chk_param_err_return(rsa_data->sign_type >= OT_CIPHER_SIGN_TYPE_BUTT);
    chk_param_err_return(rsa_data->input_len == 0);

    chk_ptr_err_return(sign_data);
    chk_ptr_err_return(sign_data->data);
    chk_param_err_return((sign_data->data_len == 0) || (sign_data->data_len != rsa_key->n_len));

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_rsa_verify(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_sign_in_data *rsa_data, const ot_cipher_common_data *sign_data)
{
    td_s32 ret;
    cryp_rsa_key key;
    td_u8 hash[HASH_RESULT_MAX_SIZE] = {0};
    sys_rsa_sign_data sys_data;
    ot_cipher_rsa_sign_scheme rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_BUTT;

    ret = mpi_rsa_chk_verify_param(rsa_key, rsa_data, sign_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_verify_param, ret);
        return ret;
    }

    (td_void)memset_s(&sys_data, sizeof(sys_data), 0, sizeof(sys_data));
    ret = mpi_rsa_get_sign_attr(&rsa_scheme, &sys_data.hash_len, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_sign_attr, ret);
        return ret;
    }

    /* hash value of context, if OT_CIPHER_SIGN_TYPE_HASH, compute hash = Hash(in_data). */
    if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_MSG) {
        ret = mpi_cipher_hash(sha_type, rsa_data->input, rsa_data->input_len, hash, sizeof(hash));
        if (ret != TD_SUCCESS) {
            print_func_errno(mpi_cipher_hash, ret);
            return ret;
        }
        sys_data.hash = hash;
    } else if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_HASH) {
        sys_data.hash = rsa_data->input;
        if (rsa_data->input_len != sys_data.hash_len) {
            log_error("Invalid input len %u\n", rsa_data->input_len);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        log_error("Invalid sign type %d\n", rsa_data->sign_type);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    sys_data.sign = sign_data->data;
    sys_data.sign_len = sign_data->data_len;

    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    key.public = TD_TRUE;
    key.klen   = rsa_key->n_len;
    key.n      = rsa_key->n;
    cipher_get_pub_exponent(&key.e, rsa_key);

    ret = sys_rsa_verify_hash(rsa_scheme, &key, &sys_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_rsa_verify_hash, ret);
    }

    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}

td_s32 ot_mpi_cipher_sm2_sign(const ot_cipher_sm2_sign *sm2_sign,
    const ot_cipher_sign_in_data *sm2_data, ot_cipher_sm2_sign_data *sign_data)
{
    td_s32 ret;
    ot_cipher_common_data msg;

    func_enter();

    chk_ptr_err_return(sm2_sign);
    chk_ptr_err_return(sm2_sign->id);
    chk_param_err_return(sm2_sign->id_len > SM2_ID_MAX_LEN);

    chk_ptr_err_return(sm2_data);
    chk_ptr_err_return(sm2_data->input);
    chk_param_err_return(sm2_data->sign_type != OT_CIPHER_SIGN_TYPE_MSG);
    chk_param_err_return(sm2_data->input_len == 0);

    chk_ptr_err_return(sign_data);

    msg.data = sm2_data->input;
    msg.data_len = sm2_data->input_len;

    ret = sys_sm2_sign(sm2_sign, &msg, sign_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_sm2_sign, ret);
        return ret;
    }

    func_exit();
    return ret;
}

td_s32 ot_mpi_cipher_sm2_verify(const ot_cipher_sm2_verify *sm2_verify,
    const ot_cipher_sign_in_data *sm2_data, const ot_cipher_sm2_sign_data *sign_data)
{
    td_s32 ret;
    ot_cipher_common_data msg;

    func_enter();

    chk_ptr_err_return(sm2_verify);
    chk_ptr_err_return(sm2_verify->id);
    chk_param_err_return(sm2_verify->id_len > SM2_ID_MAX_LEN);

    chk_ptr_err_return(sm2_data);
    chk_ptr_err_return(sm2_data->input);
    chk_param_err_return(sm2_data->sign_type != OT_CIPHER_SIGN_TYPE_MSG);
    chk_param_err_return(sm2_data->input_len == 0);

    chk_ptr_err_return(sign_data);

    msg.data = sm2_data->input;
    msg.data_len = sm2_data->input_len;

    ret = sys_sm2_verify(sm2_verify, &msg, sign_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_sm2_verify, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_sm2_encrypt(const ot_cipher_sm2_public_key *sm2_key,
    const ot_cipher_common_data *plain_txt, ot_cipher_common_data *cipher_txt)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(sm2_key);

    chk_ptr_err_return(plain_txt);
    chk_ptr_err_return(plain_txt->data);
    chk_param_err_return(plain_txt->data_len == 0);

    chk_ptr_err_return(cipher_txt);
    chk_ptr_err_return(cipher_txt->data);
    chk_param_err_return(cipher_txt->data_len == 0);

    ret = sys_sm2_encrypt(sm2_key, plain_txt, cipher_txt);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_sm2_encrypt, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_sm2_decrypt(const ot_cipher_sm2_private_key *sm2_key,
    const ot_cipher_common_data *cipher_txt, ot_cipher_common_data *plain_txt)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(sm2_key);

    chk_ptr_err_return(plain_txt);
    chk_ptr_err_return(plain_txt->data);
    chk_param_err_return(plain_txt->data_len == 0);

    chk_ptr_err_return(cipher_txt);
    chk_ptr_err_return(cipher_txt->data);
    chk_param_err_return(cipher_txt->data_len == 0);

    ret = sys_sm2_decrypt(sm2_key, cipher_txt, plain_txt);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_sm2_decrypt, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 ot_mpi_keyslot_create(const ot_keyslot_attr *attr, td_handle *keyslot)
{
    td_s32 ret;

    func_enter();

    chk_ptr_err_return(attr);
    chk_ptr_err_return(keyslot);

    ret = sys_keyslot_create(attr, keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_keyslot_create, ret);
        return ret;
    }

    func_exit();

    return TD_SUCCESS;
}

td_s32 ot_mpi_keyslot_destroy(td_handle keyslot)
{
    td_s32 ret;

    func_enter();

    ret = sys_keyslot_destroy(keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_keyslot_destroy, ret);
        return ret;
    }

    func_exit();

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_attach(td_handle cipher, td_handle keyslot)
{
    td_s32 ret;

    func_enter();

    ret = sys_cipher_attach(cipher, keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_cipher_attach, ret);
        return ret;
    }

    func_exit();

    return TD_SUCCESS;
}

td_s32 ot_mpi_cipher_detach(td_handle cipher, td_handle keyslot)
{
    td_s32 ret;

    func_enter();

    ret = sys_cipher_detach(cipher, keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(sys_cipher_detach, ret);
        return ret;
    }

    func_exit();

    return TD_SUCCESS;
}

/** @} */ /** <!-- ==== Compat Code end ==== */
