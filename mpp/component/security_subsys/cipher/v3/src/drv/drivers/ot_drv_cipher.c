/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "ot_drv_cipher.h"

#include <linux/kernel.h>

#include "securec.h"
#include "drv_osal_lib.h"
#include "drv_cipher_kapi.h"

#define crypto_unused(x)    ((x) = (x))
#define TRNG_TIMEOUT            10000
#define HASH_RESULT_MAX_SIZE    64
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

td_s32 ot_drv_cipher_init(td_void)
{
    return TD_SUCCESS;
}
EXPORT_SYMBOL(ot_drv_cipher_init);

td_s32 ot_drv_cipher_deinit(td_void)
{
    return TD_SUCCESS;
}
EXPORT_SYMBOL(ot_drv_cipher_deinit);

td_s32 ot_drv_cipher_create(td_handle *handle, const ot_cipher_attr *cipher_attr)
{
    chk_ptr_err_return(handle);
    chk_ptr_err_return(cipher_attr);
    return kapi_symc_create(handle, cipher_attr->cipher_type);
}
EXPORT_SYMBOL(ot_drv_cipher_create);

td_s32 ot_drv_cipher_destroy(td_handle handle)
{
    return kapi_symc_destroy(handle);
}
EXPORT_SYMBOL(ot_drv_cipher_destroy);

td_s32 ot_drv_cipher_set_cfg(td_handle handle, const ot_cipher_ctrl *ctrl)
{
    symc_cfg_t cfg;
    const td_u32 *iv = TD_NULL;

    chk_ptr_err_return(ctrl);
    (td_void)memset_s(&cfg, sizeof(symc_cfg_t), 0, sizeof(symc_cfg_t));

    cfg.id = handle;
    cfg.alg = ctrl->alg;
    cfg.mode = ctrl->work_mode;
    cfg.width = OT_CIPHER_BIT_WIDTH_128BIT;
    cfg.ivlen = AES_IV_SIZE;

    if (ctrl->alg == OT_CIPHER_ALG_AES) {
        if (ctrl->work_mode == OT_CIPHER_WORK_MODE_CCM ||
            ctrl->work_mode == OT_CIPHER_WORK_MODE_GCM) {
            const ot_cipher_ctrl_aes_ccm_gcm *aes_ccm_gcm = &ctrl->aes_ccm_gcm_ctrl;
            cfg.klen = aes_ccm_gcm->key_len;
            cfg.ivlen = aes_ccm_gcm->iv_len;
            cfg.iv_usage = OT_CIPHER_IV_CHG_ONE_PACK;
            cfg.tlen = aes_ccm_gcm->tag_len;
            cfg.alen = aes_ccm_gcm->aad_len;
            addr_u64(cfg.aad) = aes_ccm_gcm->aad_phys_addr;
            iv = aes_ccm_gcm->iv;
        } else {
            const ot_cipher_ctrl_aes *aes = &ctrl->aes_ctrl;
            cfg.klen = aes->key_len;
            cfg.iv_usage = aes->chg_flags;
            cfg.width = aes->bit_width;
            iv = aes->iv;
        }
    } else if (ctrl->alg == OT_CIPHER_ALG_SM4) {
        log_error("Unsupport alg %d\n", ctrl->alg);
        return OT_ERR_CIPHER_UNSUPPORTED;
    } else if (ctrl->alg != OT_CIPHER_ALG_DMA) {
        log_error("invalid alg %d\n", ctrl->alg);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    if (iv != TD_NULL) {
        if (memcpy_s(cfg.iv, sizeof(cfg.iv), iv, cfg.ivlen) != EOK) {
            return OT_ERR_CIPHER_FAILED_SEC_FUNC;
        }
    }

    return kapi_symc_cfg(&cfg);
}
EXPORT_SYMBOL(ot_drv_cipher_set_cfg);

td_s32 ot_drv_cipher_get_cfg(td_handle handle, ot_cipher_ctrl *ctrl)
{
    chk_ptr_err_return(ctrl);
    return kapi_symc_get_cfg(handle, ctrl);
}
EXPORT_SYMBOL(ot_drv_cipher_get_cfg);

td_s32 ot_drv_cipher_encrypt(td_handle handle,
    td_phys_addr_t src_phys_addr, td_phys_addr_t dst_phys_addr, td_u32 byte_len)
{
    symc_encrypt_t crypto;

    (td_void)memset_s(&crypto, sizeof(symc_encrypt_t), 0, sizeof(symc_encrypt_t));
    crypto.id = handle;
    addr_u64(crypto.input) = src_phys_addr;
    addr_u64(crypto.output) = dst_phys_addr;
    crypto.length = byte_len;
    crypto.operation = SYMC_OPERATION_ENCRYPT;
    return kapi_symc_crypto(&crypto);
}
EXPORT_SYMBOL(ot_drv_cipher_encrypt);

td_s32 ot_drv_cipher_decrypt(td_handle handle,
    td_phys_addr_t src_phys_addr, td_phys_addr_t dst_phys_addr, td_u32 byte_len)
{
    symc_encrypt_t crypto;

    (td_void)memset_s(&crypto, sizeof(symc_encrypt_t), 0, sizeof(symc_encrypt_t));
    crypto.id = handle;
    addr_u64(crypto.input) = src_phys_addr;
    addr_u64(crypto.output) = dst_phys_addr;
    crypto.length = byte_len;
    crypto.operation = SYMC_OPERATION_DECRYPT;

    return kapi_symc_crypto(&crypto);
}
EXPORT_SYMBOL(ot_drv_cipher_decrypt);

td_s32 ot_drv_cipher_encrypt_virt(td_handle handle,
    const td_u8 *src_data, td_u8 *dst_data, td_u32 byte_len)
{
    symc_encrypt_t crypto;

    chk_ptr_err_return(src_data);
    chk_ptr_err_return(dst_data);

    (td_void)memset_s(&crypto, sizeof(symc_encrypt_t), 0, sizeof(symc_encrypt_t));
    crypto.id = handle;
    addr_via_const(crypto.input) = src_data;
    addr_via(crypto.output) = dst_data;
    crypto.length = byte_len;
    crypto.operation = SYMC_OPERATION_ENCRYPT_VIA;

    return kapi_symc_crypto_via(&crypto, TD_FALSE);
}
EXPORT_SYMBOL(ot_drv_cipher_encrypt_virt);

td_s32 ot_drv_cipher_decrypt_virt(td_handle handle,
    const td_u8 *src_data, td_u8 *dst_data, td_u32 byte_len)
{
    symc_encrypt_t crypto;

    chk_ptr_err_return(src_data);
    chk_ptr_err_return(dst_data);

    (td_void)memset_s(&crypto, sizeof(symc_encrypt_t), 0, sizeof(symc_encrypt_t));
    crypto.id = handle;
    addr_via_const(crypto.input) = src_data;
    addr_via(crypto.output) = dst_data;
    crypto.length = byte_len;
    crypto.operation = SYMC_OPERATION_DECRYPT_VIA;

    return kapi_symc_crypto_via(&crypto, TD_FALSE);
}
EXPORT_SYMBOL(ot_drv_cipher_decrypt_virt);

td_s32 ot_drv_cipher_encrypt_multi_pack(td_handle handle,
    const ot_cipher_data *data_pack, td_u32 data_pack_num)
{
    crypto_unused(handle);
    crypto_unused(data_pack);
    crypto_unused(data_pack_num);

    return OT_ERR_CIPHER_UNSUPPORTED;
}
EXPORT_SYMBOL(ot_drv_cipher_encrypt_multi_pack);

td_s32 ot_drv_cipher_decrypt_multi_pack(td_handle handle,
    const ot_cipher_data *data_pack, td_u32 data_pack_num)
{
    crypto_unused(handle);
    crypto_unused(data_pack);
    crypto_unused(data_pack_num);

    return OT_ERR_CIPHER_UNSUPPORTED;
}
EXPORT_SYMBOL(ot_drv_cipher_decrypt_multi_pack);

td_s32 ot_drv_cipher_get_tag(td_handle handle, td_u8 *tag, td_u32 tag_len)
{
    td_s32 ret;
    td_u32 tmp_tag[AEAD_TAG_SIZE_IN_WORD] = {0};
    td_u32 tmp_tag_len = sizeof(tmp_tag);

    chk_ptr_err_return(tag);

    ret = kapi_aead_get_tag(handle, tmp_tag, &tmp_tag_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_aead_get_tag, ret);
        return ret;
    }

    chk_param_err_return(tmp_tag_len > tag_len);

    ret = memcpy_s(tag, tag_len, tmp_tag, tmp_tag_len);
    if (ret != EOK) {
        print_func_errno(kapi_aead_get_tag, OT_ERR_CIPHER_FAILED_SEC_FUNC);
        return OT_ERR_CIPHER_FAILED_SEC_FUNC;
    }

    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_get_tag);

td_s32 ot_drv_cipher_hash_init(const ot_cipher_hash_attr *hash_attr, td_handle *handle)
{
    chk_ptr_err_return(hash_attr);
    chk_ptr_err_return(handle);

    return kapi_hash_start(handle, hash_attr->sha_type, hash_attr->hmac_key, hash_attr->hmac_key_len);
}
EXPORT_SYMBOL(ot_drv_cipher_hash_init);

td_s32 ot_drv_cipher_hash_update(td_handle handle, const td_u8 *in_data, td_u32 in_data_len)
{
    chk_ptr_err_return(in_data);

    return kapi_hash_update(handle, in_data, in_data_len, HASH_CHUNCK_SRC_LOCAL);
}
EXPORT_SYMBOL(ot_drv_cipher_hash_update);

td_s32 ot_drv_cipher_hash_final(td_handle handle, td_u8 *out_hash, td_u32 out_hash_len)
{
    td_s32 ret = TD_SUCCESS;
    td_u32 real_hash_len = 0;
    chk_ptr_err_return(out_hash);

    ret = kapi_hash_finish(handle, out_hash, out_hash_len, &real_hash_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_finish, ret);
        return ret;
    }

    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_hash_final);

td_s32 ot_drv_cipher_get_random_num(td_u32 *random_num)
{
    chk_ptr_err_return(random_num);
    return kapi_trng_get_random(random_num, TRNG_TIMEOUT);
}
EXPORT_SYMBOL(ot_drv_cipher_get_random_num);

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

td_s32 ot_drv_cipher_rsa_public_encrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_common_data *plain_txt, ot_cipher_common_data *cipher_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;
    cryp_rsa_crypto_data rsa_data;

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

    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));
    (td_void)memset_s(&rsa_data, sizeof(cryp_rsa_crypto_data), 0, sizeof(cryp_rsa_crypto_data));

    rsa_data.in = plain_txt->data;
    rsa_data.in_len = plain_txt->data_len;
    rsa_data.out = cipher_txt->data;
    rsa_data.out_len = cipher_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    key.public = TD_TRUE;
    key.klen = rsa_key->n_len;
    key.n = rsa_key->n;
    cipher_get_pub_exponent(&key.e, rsa_key);

    ret = kapi_rsa_encrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_encrypt, ret);
    }
    cipher_txt->data_len = rsa_data.out_len;
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_rsa_public_encrypt);


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
td_s32 ot_drv_cipher_rsa_private_decrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_common_data *cipher_txt, ot_cipher_common_data *plain_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;
    cryp_rsa_crypto_data rsa_data;

    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));
    (td_void)memset_s(&rsa_data, sizeof(cryp_rsa_crypto_data), 0, sizeof(cryp_rsa_crypto_data));

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
    rsa_data.out_len = plain_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    mpi_rsa_init_private_key(&key, rsa_key);

    ret = kapi_rsa_decrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_encrypt, ret);
    }
    plain_txt->data_len = rsa_data.out_len;
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_rsa_private_decrypt);

td_s32 ot_drv_cipher_rsa_private_encrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_common_data *plain_txt, ot_cipher_common_data *cipher_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;
    cryp_rsa_crypto_data rsa_data;

    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));
    (td_void)memset_s(&rsa_data, sizeof(cryp_rsa_crypto_data), 0, sizeof(cryp_rsa_crypto_data));

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
    rsa_data.out_len = cipher_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    mpi_rsa_init_private_key(&key, rsa_key);

    ret = kapi_rsa_encrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_encrypt, ret);
    }
    cipher_txt->data_len = rsa_data.out_len;
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_rsa_private_encrypt);

td_s32 ot_drv_cipher_rsa_public_decrypt(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_common_data *cipher_txt, ot_cipher_common_data *plain_txt)
{
    td_s32 ret;
    cryp_rsa_key key;
    ot_cipher_rsa_encrypt_scheme rsa_scheme = OT_CIPHER_RSA_ENCRYPT_SCHEME_BUTT;
    cryp_rsa_crypto_data rsa_data;

    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));
    (td_void)memset_s(&rsa_data, sizeof(cryp_rsa_crypto_data), 0, sizeof(cryp_rsa_crypto_data));

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
    rsa_data.out_len = plain_txt->data_len;

    ret = mpi_rsa_get_crypto_attr(&rsa_scheme, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_crypto_attr, ret);
        return ret;
    }

    key.public  = TD_TRUE;
    key.klen    = rsa_key->n_len;
    key.n       = rsa_key->n;
    cipher_get_pub_exponent(&key.e, rsa_key);

    ret = kapi_rsa_decrypt(rsa_scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_decrypt, ret);
    }
    plain_txt->data_len = rsa_data.out_len;
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_rsa_public_decrypt);

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

    ret = kapi_hash_start(&hash_id, sha_type, TD_NULL, 0);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_start, ret);
        return ret;
    }

    ret = kapi_hash_update(hash_id, in_data, in_len, HASH_CHUNCK_SRC_LOCAL);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_update, ret);
        return ret;
    }

    ret = kapi_hash_finish(hash_id, hash_data, hash_len, &hash_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_finish, ret);
        return ret;
    }

    return TD_SUCCESS;
}

td_s32 ot_drv_cipher_rsa_sign(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_private_key *rsa_key,
    const ot_cipher_sign_in_data *rsa_data, ot_cipher_common_data *sign_data)
{
    td_s32 ret;
    cryp_rsa_key key;
    td_u8 hash[64] = {0};
    cryp_rsa_sign_data rsa_sign;
    ot_cipher_rsa_sign_scheme rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_BUTT;

    (td_void)memset_s(&rsa_sign, sizeof(rsa_sign), 0, sizeof(rsa_sign));
    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = mpi_rsa_chk_sign_param(rsa_key, rsa_data, sign_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_sign_param, ret);
        return ret;
    }

    ret = mpi_rsa_get_sign_attr(&rsa_scheme, &rsa_sign.in_len, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_sign_attr, ret);
        return ret;
    }

    /* hash value of context, if OT_CIPHER_SIGN_TYPE_MSG, compute hash = Hash(in_data). */
    if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_MSG) {
        ret = mpi_cipher_hash(sha_type, rsa_data->input, rsa_data->input_len, hash, sizeof(hash));
        if (ret != TD_SUCCESS) {
            print_func_errno(mpi_cipher_hash, ret);
            return ret;
        }
        rsa_sign.in = hash;
    } else if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_HASH) {
        rsa_sign.in = rsa_data->input;
        if (rsa_data->input_len != rsa_sign.in_len) {
            log_error("Invalid input len %u\n", rsa_data->input_len);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        log_error("Invalid sign type %d\n", rsa_data->sign_type);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    rsa_sign.out = sign_data->data;
    rsa_sign.out_len = rsa_key->n_len;

    mpi_rsa_init_private_key(&key, rsa_key);

    ret = kapi_rsa_sign_hash(rsa_scheme, &key, &rsa_sign);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_sign_hash, ret);
    }

    sign_data->data_len = rsa_sign.out_len;
    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_rsa_sign);

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

td_s32 ot_drv_cipher_rsa_verify(ot_cipher_rsa_scheme scheme,
    ot_cipher_hash_type sha_type, const ot_cipher_rsa_public_key *rsa_key,
    const ot_cipher_sign_in_data *rsa_data, const ot_cipher_common_data *sign_data)
{
    td_s32 ret;
    cryp_rsa_key key;
    td_u8 hash[HASH_RESULT_MAX_SIZE] = {0};
    cryp_rsa_sign_data rsa_sign;
    ot_cipher_rsa_sign_scheme rsa_scheme = OT_CIPHER_RSA_SIGN_SCHEME_BUTT;

    (td_void)memset_s(&rsa_sign, sizeof(rsa_sign), 0, sizeof(rsa_sign));
    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = mpi_rsa_chk_verify_param(rsa_key, rsa_data, sign_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_chk_verify_param, ret);
        return ret;
    }

    ret = mpi_rsa_get_sign_attr(&rsa_scheme, &rsa_sign.out_len, scheme, sha_type);
    if (ret != TD_SUCCESS) {
        print_func_errno(mpi_rsa_get_sign_attr, ret);
        return ret;
    }

    /* hash value of context, if OT_CIPHER_SIGN_TYPE_MSG, compute hash = Hash(in_data). */
    if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_MSG) {
        ret = mpi_cipher_hash(sha_type, rsa_data->input, rsa_data->input_len, hash, sizeof(hash));
        if (ret != TD_SUCCESS) {
            print_func_errno(mpi_cipher_hash, ret);
            return ret;
        }
        rsa_sign.out = hash;
    } else if (rsa_data->sign_type == OT_CIPHER_SIGN_TYPE_HASH) {
        rsa_sign.out = rsa_data->input;
        if (rsa_data->input_len != rsa_sign.out_len) {
            log_error("Invalid input len %u\n", rsa_data->input_len);
            return OT_ERR_CIPHER_INVALID_PARAM;
        }
    } else {
        log_error("Invalid sign type %d\n", rsa_data->sign_type);
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    rsa_sign.in = sign_data->data;
    rsa_sign.in_len = sign_data->data_len;

    key.public  = TD_TRUE;
    key.klen    = rsa_key->n_len;
    key.n       = rsa_key->n;
    cipher_get_pub_exponent(&key.e, rsa_key);

    ret = kapi_rsa_verify_hash(rsa_scheme, &key, &rsa_sign);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_verify_hash, ret);
    }

    (td_void)memset_s(&key, sizeof(key), 0, sizeof(key));
    return ret;
}
EXPORT_SYMBOL(ot_drv_cipher_rsa_verify);

td_s32 ot_drv_cipher_sm2_encrypt(const ot_cipher_sm2_public_key *sm2_key,
    const ot_cipher_common_data *plain_txt, ot_cipher_common_data *cipher_txt)
{
    crypto_unused(sm2_key);
    crypto_unused(plain_txt);
    crypto_unused(cipher_txt);

    return OT_ERR_CIPHER_UNSUPPORTED;
}
EXPORT_SYMBOL(ot_drv_cipher_sm2_encrypt);

td_s32 ot_drv_cipher_sm2_decrypt(const ot_cipher_sm2_private_key *sm2_key,
    const ot_cipher_common_data *cipher_txt, ot_cipher_common_data *plain_txt)
{
    crypto_unused(sm2_key);
    crypto_unused(plain_txt);
    crypto_unused(cipher_txt);

    return OT_ERR_CIPHER_UNSUPPORTED;
}
EXPORT_SYMBOL(ot_drv_cipher_sm2_decrypt);

td_s32 ot_drv_cipher_sm2_sign(const ot_cipher_sm2_sign *sm2_sign,
    const ot_cipher_sign_in_data *sm2_data, ot_cipher_sm2_sign_data *sign_data)
{
    crypto_unused(sm2_sign);
    crypto_unused(sm2_data);
    crypto_unused(sign_data);

    return OT_ERR_CIPHER_UNSUPPORTED;
}
EXPORT_SYMBOL(ot_drv_cipher_sm2_sign);

td_s32 ot_drv_cipher_sm2_verify(const ot_cipher_sm2_verify *sm2_verify,
    const ot_cipher_sign_in_data *sm2_data, const ot_cipher_sm2_sign_data *sign_data)
{
    crypto_unused(sm2_verify);
    crypto_unused(sm2_data);
    crypto_unused(sign_data);

    return OT_ERR_CIPHER_UNSUPPORTED;
}
EXPORT_SYMBOL(ot_drv_cipher_sm2_verify);

td_s32 ot_drv_keyslot_create(const ot_keyslot_attr *attr, td_handle *keyslot)
{
    chk_ptr_err_return(attr);
    chk_ptr_err_return(keyslot);

    return kapi_keyslot_create(attr, keyslot);
}
EXPORT_SYMBOL(ot_drv_keyslot_create);

td_s32 ot_drv_keyslot_destroy(td_handle keyslot)
{
    return kapi_keyslot_destroy(keyslot);
}
EXPORT_SYMBOL(ot_drv_keyslot_destroy);

td_s32 ot_drv_cipher_attach(td_handle cipher, td_handle keyslot)
{
    return kapi_symc_attach(cipher, keyslot);
}
EXPORT_SYMBOL(ot_drv_cipher_attach);

td_s32 ot_drv_cipher_detach(td_handle cipher, td_handle keyslot)
{
    return kapi_symc_detach(cipher, keyslot);
}
EXPORT_SYMBOL(ot_drv_cipher_detach);
