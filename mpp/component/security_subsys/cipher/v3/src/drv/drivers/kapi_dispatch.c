/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "drv_osal_lib.h"
#include "cryp_symc.h"

/* max package numher of symc multi encrypt */
#define SYMC_MULTI_MAX_PKG      0x1000

#define RSA_PUBLIC_BUFFER_NUM   0x03
#define RSA_PRIVATE_BUFFER_NUM  0x07

typedef td_s32 (*ot_drv_func)(td_void *param);

typedef struct {
    const char *name;
    ot_drv_func func;
    td_u32 cmd;
} crypto_dispatch_func;

typedef struct {
    td_u8 *buf;
    td_u32 buf_size;
    td_u32 offset;
} kapi_rsa_buf;

static td_s32 _kapi_check_rsa_key(const cryp_rsa_key *key)
{
    /* 1. check if n is even number */
    if ((key->n[key->klen - 1] & (0x1)) == 0) {
        log_error("invalid n, n is even number.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }
    return TD_SUCCESS;
}

/* ****************************** API Code **************************** */
static td_s32 dispatch_symc_create_handle(td_void *argp)
{
    td_s32 ret;
    symc_create_t *symc_create = argp;

    func_enter();

    /* allocate a aes channel */
    ret = kapi_symc_create(&symc_create->id, symc_create->type);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_create, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_symc_destroy_handle(td_void *argp)
{
    td_s32 ret;
    symc_destroy_t *destroy = argp;

    func_enter();

    ret = kapi_symc_destroy(destroy->id);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_destroy, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_symc_cfg(td_void *argp)
{
    td_s32 ret;
    symc_cfg_t *cfg = argp;

    func_enter();

    ret = kapi_symc_cfg(cfg);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_cfg, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_symc_encrypt(td_void *argp)
{
    td_s32 ret;
    symc_encrypt_t *encrypt = argp;

    func_enter();

    if ((encrypt->operation == SYMC_OPERATION_ENCRYPT) || (encrypt->operation == SYMC_OPERATION_DECRYPT)) {
        ret = cipher_check_mmz_phy_addr((td_phys_addr_t)addr_u64(encrypt->input), encrypt->length);
        if (ret != TD_SUCCESS) {
            print_func_errno(cipher_check_mmz_phy_addr, ret);
            return ret;
        }

        ret = cipher_check_mmz_phy_addr((td_phys_addr_t)addr_u64(encrypt->output), encrypt->length);
        if (ret != TD_SUCCESS) {
            print_func_errno(cipher_check_mmz_phy_addr, ret);
            return ret;
        }

        ret = kapi_symc_crypto(encrypt);
        if (ret != TD_SUCCESS) {
            print_func_errno(kapi_symc_crypto, ret);
            return ret;
        }
    } else if ((encrypt->operation == SYMC_OPERATION_ENCRYPT_VIA) ||
        (encrypt->operation == SYMC_OPERATION_DECRYPT_VIA)) {
        ret = kapi_symc_crypto_via(encrypt, TD_TRUE);
        if (ret != TD_SUCCESS) {
            print_func_errno(kapi_symc_crypto_via, ret);
            return ret;
        }
    } else {
        log_error("encrypt operation(0x%x) is unsupported.\n", encrypt->operation);
        return OT_ERR_CIPHER_UNSUPPORTED;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_symc_encrypt_multi(td_void *argp)
{
    td_s32 ret;
    symc_encrypt_multi_t *encrypt_multi = argp;

    func_enter();

    log_dbg("operation %u\n", encrypt_multi->operation);
    ret = kapi_symc_crypto_multi(encrypt_multi->id,
        addr_via(encrypt_multi->pack), encrypt_multi->pack_num, encrypt_multi->operation, TD_TRUE);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_crypto_multi, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_symc_get_tag(td_void *argp)
{
#ifdef CHIP_AES_CCM_GCM_SUPPORT
    td_s32 ret;
    aead_tag_t *aead_tag = argp;

    func_enter();

    ret = kapi_aead_get_tag(aead_tag->id, aead_tag->tag, &aead_tag->taglen);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_aead_get_tag, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
#else
    log_error("Unsupported get tag\n");
    return TD_FAILURE;
#endif
}

static td_s32 dispatch_symc_get_cfg(td_void *argp)
{
    td_s32 ret;
    symc_get_cfg_t *get_config = argp;

    func_enter();

    ret = kapi_symc_get_cfg(get_config->id, &get_config->ctrl);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_get_cfg, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_hash_start(td_void *argp)
{
    td_s32 ret;
    hash_start_t *start = argp;
    td_u8 *key = TD_NULL;

    func_enter();

    if (start->type == OT_CIPHER_HASH_TYPE_HMAC_SHA256 ||
        start->type == OT_CIPHER_HASH_TYPE_HMAC_SHA384 ||
        start->type == OT_CIPHER_HASH_TYPE_HMAC_SHA512) {
        chk_param_err_return(start->keylen == 0 || start->keylen > MAX_MALLOC_BUF_SIZE);
        chk_ptr_err_return(addr_via(start->key));

        key = (td_u8 *)crypto_calloc(1, start->keylen);
        if (key == TD_NULL) {
            print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
            return OT_ERR_CIPHER_FAILED_MEM;
        }

        chk_func_err_goto(crypto_copy_from_user(key, start->keylen, addr_via(start->key), start->keylen));
    }

    chk_func_err_goto(kapi_hash_start(&start->id, start->type, key, start->keylen));

exit__:
    if (key != TD_NULL) {
        (td_void)memset_s(key, start->keylen, 0, start->keylen);
        crypto_free(key);
    }

    func_exit();

    return ret;
}

static td_s32 dispatch_hash_update(td_void *argp)
{
    td_s32 ret;
    hash_update_t *update = argp;

    func_enter();

    chk_ptr_err_return(addr_via(update->input));

    update->src = HASH_CHUNCK_SRC_USER;
    ret = kapi_hash_update(update->id, addr_via(update->input), update->length, update->src);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_update, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_hash_finish(td_void *argp)
{
    td_s32 ret;
    hash_finish_t *finish = argp;

    func_enter();

    ret = kapi_hash_finish(finish->id, (td_u8 *)finish->hash, sizeof(finish->hash), &finish->hashlen);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_finish, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

#if (defined(CHIP_RSA_SUPPORT) || defined(SOFT_RSA_SUPPORT))
static td_s32 rsa_buf_chk_info_param(const rsa_info_t *rsa_info)
{
    chk_ptr_err_return(rsa_info);
    chk_ptr_err_return(addr_via(rsa_info->in));
    chk_ptr_err_return(addr_via(rsa_info->out));

    if (rsa_info->public == TD_FALSE) {
        chk_param_err_return((addr_via(rsa_info->d) == TD_NULL) &&
            ((addr_via(rsa_info->p) == TD_NULL) ||
             (addr_via(rsa_info->q) == TD_NULL)  ||
             (addr_via(rsa_info->dp) == TD_NULL) ||
             (addr_via(rsa_info->dq) == TD_NULL) ||
             (addr_via(rsa_info->qp) == TD_NULL)));
    }

    chk_param_err_return(rsa_info->inlen > rsa_info->klen);
    chk_param_err_return(rsa_info->outlen > rsa_info->klen);
    chk_param_err_return(rsa_info->klen < RSA_KEY_BITWIDTH_1024);
    chk_param_err_return(rsa_info->klen > RSA_KEY_BITWIDTH_4096);

    return TD_SUCCESS;
}

static td_s32 rsa_pub_alloc(cryp_rsa_key *key, const rsa_info_t *rsa_info, td_u8 **in, td_u8 **out)
{
    td_s32 ret;
    td_u32 size;
    td_u8 *buf = TD_NULL;
    td_u32 klen = rsa_info->klen;

    /* buffer size of key, input and output */
    size = rsa_info->klen * RSA_PUBLIC_BUFFER_NUM;
    buf = crypto_calloc(1, size);
    if (buf == TD_NULL) {
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    key->n = buf;
    buf += klen;
    *in  = buf;
    buf += klen;
    *out = buf;
    buf += klen;
    key->bufsize = size;

    chk_func_err_goto(crypto_copy_from_user(key->n, klen, addr_via(rsa_info->n), klen));
    chk_func_err_goto(crypto_copy_from_user(*in, klen, addr_via(rsa_info->in), rsa_info->inlen));
    key->e = rsa_info->e;

    return TD_SUCCESS;

exit__:
    if (key->n != TD_NULL) {
        (td_void)memset_s(key->n, key->bufsize, 0, key->bufsize);
        crypto_free(key->n);
    }

    log_error("error, copy rsa key from user failed\n");
    print_errno(OT_ERR_CIPHER_FAILED_MEM);
    return OT_ERR_CIPHER_FAILED_MEM;
}

static td_void rsa_private_set_key_param(cryp_rsa_key *key, kapi_rsa_buf *rsa_buf, td_u32 klen, td_u32 e)
{
    td_u8 *ptr = rsa_buf->buf;

    key->n  = ptr;
    ptr += klen;
    rsa_buf->offset += klen;

    key->d  = ptr;
    ptr += klen;
    rsa_buf->offset += klen;

    key->p  = ptr;
    ptr += klen / MUL_VAL_2;
    rsa_buf->offset += klen / MUL_VAL_2;

    key->q  = ptr;
    ptr += klen / MUL_VAL_2;
    rsa_buf->offset += klen / MUL_VAL_2;

    key->dp = ptr;
    ptr += klen / MUL_VAL_2;
    rsa_buf->offset += klen / MUL_VAL_2;

    key->dq = ptr;
    ptr += klen / MUL_VAL_2;
    rsa_buf->offset += klen / MUL_VAL_2;

    key->qp = ptr;
    ptr += klen / MUL_VAL_2;
    rsa_buf->offset += klen / MUL_VAL_2;

    key->e = e;
    key->bufsize = rsa_buf->buf_size;
}

static td_s32 rsa_private_get_cfg(cryp_rsa_key *key,
    const rsa_info_t *rsa_info, td_u8 **in, td_u8 **out, td_u8 *buf)
{
    td_s32 ret = TD_FAILURE;
    td_u8 *ptr = buf;
    td_u32 klen = rsa_info->klen;

    if (addr_via(rsa_info->n) != TD_NULL) {
        chk_func_err_goto(crypto_copy_from_user(key->n, klen, addr_via(rsa_info->n), klen));
    }

    if (addr_via(rsa_info->d) != TD_NULL) {
        chk_func_err_goto(crypto_copy_from_user(key->d, klen, addr_via(rsa_info->d), klen));
    } else {
        chk_func_err_goto(crypto_copy_from_user(
            key->p, klen / MUL_VAL_2, addr_via(rsa_info->p), klen / MUL_VAL_2));
        chk_func_err_goto(crypto_copy_from_user(
            key->q, klen / MUL_VAL_2, addr_via(rsa_info->q), klen / MUL_VAL_2));
        chk_func_err_goto(crypto_copy_from_user(
            key->dp, klen / MUL_VAL_2, addr_via(rsa_info->dp), klen / MUL_VAL_2));
        chk_func_err_goto(crypto_copy_from_user(
            key->dq, klen / MUL_VAL_2, addr_via(rsa_info->dq), klen / MUL_VAL_2));
        chk_func_err_goto(crypto_copy_from_user(
            key->qp, klen / MUL_VAL_2, addr_via(rsa_info->qp), klen / MUL_VAL_2));
        key->d = TD_NULL;
    }

    *in  = ptr;
    ptr += klen;
    *out = ptr;
    ptr += klen;

    if (addr_via(rsa_info->in) != TD_NULL) {
        chk_func_err_goto(crypto_copy_from_user(*in, klen, addr_via(rsa_info->in), rsa_info->inlen));
    }

    return TD_SUCCESS;
exit__:
    return ret;
}

static td_s32 rsa_private_alloc(cryp_rsa_key *key, const rsa_info_t *rsa_info, td_u8 **in, td_u8 **out)
{
    td_s32 ret;
    td_u32 size;
    td_u8 *ptr = TD_NULL;
    td_u8 *buf = TD_NULL;
    td_u32 klen = rsa_info->klen;
    kapi_rsa_buf rsa_buf;

    /* n + d or n + p + q + dP + dQ + qp
     * the length of n/d is klen,
     * the length of p/q/dP/dQ/qp is klen/2,
     * the length of input is klen
     * the length of output is klen
     */
    (td_void)memset_s(&rsa_buf, sizeof(rsa_buf), 0, sizeof(rsa_buf));

    size = klen * RSA_PRIVATE_BUFFER_NUM;
    buf = crypto_calloc(MUL_VAL_1, size);
    if (buf == TD_NULL) {
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }
    ptr = buf;

    rsa_buf.buf = buf;
    rsa_buf.buf_size = size;
    rsa_private_set_key_param(key, &rsa_buf, klen, rsa_info->e);

    ptr += rsa_buf.offset; /* set buf for in and out */
    chk_func_err_goto(rsa_private_get_cfg(key, rsa_info, in, out, ptr));
    return TD_SUCCESS;

exit__:
    if (buf != TD_NULL) {
        (td_void)memset_s(buf, size, 0, size);
        crypto_free(buf);
    }

    log_error("error, copy rsa key from user failed\n");
    print_errno(OT_ERR_CIPHER_FAILED_MEM);
    return OT_ERR_CIPHER_FAILED_MEM;
}

static td_s32 rsa_alloc_buffer(cryp_rsa_key *key, const rsa_info_t *rsa_info, td_u8 **in, td_u8 **out)
{
    td_s32 ret;
    td_u32 klen;

    func_enter();

    ret = rsa_buf_chk_info_param(rsa_info);
    if (ret != TD_SUCCESS) {
        print_func_errno(rsa_buf_chk_info_param, ret);
        return ret;
    }

    (td_void)memset_s(key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    klen = rsa_info->klen;
    key->klen = klen;
    key->public = rsa_info->public;

    if (rsa_info->public) {
        ret = rsa_pub_alloc(key, rsa_info, in, out);
        if (ret != TD_SUCCESS) {
            print_func_errno(rsa_pub_alloc, ret);
            return ret;
        }
    } else {
        ret = rsa_private_alloc(key, rsa_info, in, out);
        if (ret != TD_SUCCESS) {
            print_func_errno(rsa_private_alloc, ret);
            return ret;
        }
    }

    func_exit();
    return TD_SUCCESS;
}

static td_void rsa_free_buffer(cryp_rsa_key *key)
{
    func_enter();

    if (key->n != TD_NULL) {
        (td_void)memset_s(key->n, key->bufsize, 0, key->bufsize);
        crypto_free(key->n);
    }

    func_exit();
    return;
}
#endif

static td_s32 dispatch_rsa_encrypt(td_void *argp)
{
#if (defined(CHIP_RSA_SUPPORT) || defined(SOFT_RSA_SUPPORT))
    td_s32 ret;
    rsa_info_t *rsa_info = (rsa_info_t *)argp;
    cryp_rsa_key key;
    cryp_rsa_crypto_data rsa_data;

    func_enter();

    (td_void)memset_s(&rsa_data, sizeof(rsa_data), 0, sizeof(rsa_data));
    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &rsa_data.in, &rsa_data.out);
    if (ret != TD_SUCCESS) {
        print_func_errno(rsa_alloc_buffer, ret);
        return ret;
    }

    /* check the validation of key */
    ret = _kapi_check_rsa_key(&key);
    if (ret != TD_SUCCESS) {
        print_func_errno(_kapi_check_rsa_key, ret);
        goto exit;
    }
    rsa_data.in_len  = rsa_info->inlen;
    rsa_data.out_len = rsa_info->outlen;

    ret = kapi_rsa_encrypt(rsa_info->scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_encrypt, ret);
        goto exit;
    }

    ret = crypto_copy_to_user(addr_via(rsa_info->out), rsa_info->outlen, rsa_data.out, rsa_data.out_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(crypto_copy_to_user, ret);
        goto exit;
    }

    rsa_info->outlen = rsa_data.out_len;

exit:
    rsa_free_buffer(&key);
    func_exit();
    return ret;
#else
    log_error("Unsupported rsa encrypt\n");
    return TD_FAILURE;
#endif
}

static td_s32 dispatch_rsa_decrypt(td_void *argp)
{
#if (defined(CHIP_RSA_SUPPORT) || defined(SOFT_RSA_SUPPORT))
    td_s32 ret;
    rsa_info_t *rsa_info = (rsa_info_t *)argp;
    cryp_rsa_key key;
    cryp_rsa_crypto_data rsa_data;

    func_enter();

    (td_void)memset_s(&rsa_data, sizeof(rsa_data), 0, sizeof(rsa_data));
    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &rsa_data.in, &rsa_data.out);
    if (ret != TD_SUCCESS) {
        print_func_errno(rsa_alloc_buffer, ret);
        return ret;
    }
    /* check the validation of key */
    ret = _kapi_check_rsa_key(&key);
    if (ret != TD_SUCCESS) {
        print_func_errno(_kapi_check_rsa_key, ret);
        goto exit;
    }
    rsa_data.in_len  = rsa_info->inlen;
    rsa_data.out_len = rsa_info->outlen;

    ret = kapi_rsa_decrypt(rsa_info->scheme, &key, &rsa_data);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_decrypt, ret);
        goto exit;
    }

    ret = crypto_copy_to_user(addr_via(rsa_info->out), rsa_info->outlen, rsa_data.out, rsa_data.out_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(crypto_copy_to_user, ret);
        goto exit;
    }

    rsa_info->outlen = rsa_data.out_len;

exit:
    rsa_free_buffer(&key);
    func_exit();
    return ret;
#else
    log_error("Unsupported rsa decrypt\n");
    return TD_FAILURE;
#endif
}

static td_s32 dispatch_rsa_sign_hash(td_void *argp)
{
#if (defined(CHIP_RSA_SUPPORT) || defined(SOFT_RSA_SUPPORT))
    td_s32 ret;
    rsa_info_t *rsa_info = (rsa_info_t *)argp;
    cryp_rsa_key key;
    cryp_rsa_sign_data rsa;

    func_enter();

    (td_void)memset_s(&rsa, sizeof(rsa), 0, sizeof(rsa));
    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &rsa.in, &rsa.out);
    if (ret != TD_SUCCESS) {
        print_func_errno(rsa_alloc_buffer, ret);
        return ret;
    }
    /* check the validation of key */
    ret = _kapi_check_rsa_key(&key);
    if (ret != TD_SUCCESS) {
        print_func_errno(_kapi_check_rsa_key, ret);
        goto exit;
    }
    rsa.in_len  = rsa_info->inlen;
    rsa.out_len = rsa_info->outlen;

    ret = kapi_rsa_sign_hash(rsa_info->scheme, &key, &rsa);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_sign_hash, ret);
        goto exit;
    }

    ret = crypto_copy_to_user(addr_via(rsa_info->out), rsa_info->outlen, rsa.out, rsa.out_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(crypto_copy_to_user, ret);
        goto exit;
    }

    rsa_info->outlen = rsa.out_len;
exit:
    rsa_free_buffer(&key);
    func_exit();
    return ret;
#else
    log_error("Unsupported rsa sign\n");
    return TD_FAILURE;
#endif
}

static td_s32 dispatch_rsa_verify_hash(td_void *argp)
{
#if (defined(CHIP_RSA_SUPPORT) || defined(SOFT_RSA_SUPPORT))
    td_s32 ret;
    rsa_info_t *rsa_info = (rsa_info_t *)argp;
    cryp_rsa_key key;
    cryp_rsa_sign_data rsa;

    func_enter();

    (td_void)memset_s(&rsa, sizeof(rsa), 0, sizeof(rsa));
    (td_void)memset_s(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &rsa.in, &rsa.out);
    if (ret != TD_SUCCESS) {
        print_func_errno(rsa_alloc_buffer, ret);
        return ret;
    }
    /* check the validation of key */
    ret = _kapi_check_rsa_key(&key);
    if (ret != TD_SUCCESS) {
        print_func_errno(_kapi_check_rsa_key, ret);
        goto exit;
    }
    rsa.in_len  = rsa_info->inlen;
    rsa.out_len = rsa_info->outlen;

    /* copy hash value from user */
    ret = crypto_copy_from_user(rsa.out, rsa.out_len, addr_via(rsa_info->out), rsa_info->outlen);
    if (ret != TD_SUCCESS) {
        print_func_errno(crypto_copy_from_user, ret);
        goto exit;
    }

    ret = kapi_rsa_verify_hash(rsa_info->scheme, &key, &rsa);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_verify_hash, ret);
        goto exit;
    }

exit:
    rsa_free_buffer(&key);
    func_exit();
    return ret;
#else
    log_error("Unsupported rsa verify\n");
    return TD_FAILURE;
#endif
}

static td_s32 dispatch_trng_get_random(td_void *argp)
{
    trng_t *trng = (trng_t *)argp;
    td_s32 ret;

    func_enter();

    ret = kapi_trng_get_random(&trng->randnum, trng->timeout);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

static td_s32 dispatch_sm2_sign(td_void *argp)
{
#ifdef CHIP_SM2_SUPPORT
    td_s32 ret;
    sm2_sign_t *sign = (sm2_sign_t *)argp;
    td_u8 *user_id = TD_NULL;

    func_enter();

    chk_ptr_err_return(sign);
    chk_param_err_return((sign->id_len + 1) > SM2_ID_MAX_LEN);

    user_id = crypto_calloc(1, sign->id_len + 1);
    if (user_id == TD_NULL) {
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    ret = crypto_copy_from_user(user_id, sign->id_len + 1, sign->id, sign->id_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(crypto_copy_from_user, ret);
        crypto_free(user_id);
        return ret;
    }

    ret = kapi_sm2_sign(sign, user_id, sign->id_len, HASH_CHUNCK_SRC_USER);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_sm2_sign, ret);
    }

    func_exit();
    crypto_free(user_id);
    return ret;
#else
    crypto_unused(argp);
    log_error("Unsupported SM2\n");
    return OT_ERR_CIPHER_UNSUPPORTED;
#endif
}

static td_s32 dispatch_sm2_verify(td_void *argp)
{
#ifdef CHIP_SM2_SUPPORT
    td_s32 ret;
    sm2_verify_t *verify = (sm2_verify_t *)argp;
    td_u8 *user_id = TD_NULL;

    func_enter();

    chk_ptr_err_return(verify);
    chk_param_err_return((verify->id_len + 1) > SM2_ID_MAX_LEN);

    user_id = crypto_calloc(1, (verify->id_len + 1));
    if (user_id == TD_NULL) {
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    ret = crypto_copy_from_user(user_id, verify->id_len + 1, verify->id, verify->id_len);
    if (ret != TD_SUCCESS) {
        print_func_errno(crypto_copy_from_user, ret);
        crypto_free(user_id);
        return ret;
    }

    ret = kapi_sm2_verify(verify, user_id, verify->id_len, HASH_CHUNCK_SRC_USER);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_sm2_verify, ret);
    }

    func_exit();
    crypto_free(user_id);
    return ret;
#else
    crypto_unused(argp);
    log_error("Unsupported SM2\n");
    return OT_ERR_CIPHER_UNSUPPORTED;
#endif
}

static td_s32 dispatch_sm2_encrypt(td_void *argp)
{
#ifdef CHIP_SM2_SUPPORT
    td_s32 ret;
    sm2_encrypt_t *encrypt = (sm2_encrypt_t *)argp;
    ot_cipher_common_data plain_txt, cipher_txt;

    chk_ptr_err_return(encrypt);
    chk_param_err_return((encrypt->plain_data == TD_NULL) || (encrypt->cipher_data == TD_NULL));
    chk_param_err_return((encrypt->plain_len == 0) || (encrypt->plain_len > MAX_MALLOC_BUF_SIZE));

    (td_void)memset_s(&plain_txt, sizeof(ot_cipher_common_data), 0, sizeof(ot_cipher_common_data));
    (td_void)memset_s(&cipher_txt, sizeof(ot_cipher_common_data), 0, sizeof(ot_cipher_common_data));
    plain_txt.data_len = encrypt->plain_len;
    cipher_txt.data_len = encrypt->plain_len + SM2_ENCRYPT_PAD_LEN;
    if (encrypt->cipher_len < cipher_txt.data_len) {
        log_error("cipher length is overflow.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    plain_txt.data = crypto_calloc(1, plain_txt.data_len);
    if (plain_txt.data == TD_NULL) {
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    cipher_txt.data = crypto_calloc(1, cipher_txt.data_len);
    if (cipher_txt.data == TD_NULL) {
        crypto_free(plain_txt.data);
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    chk_func_err_goto(crypto_copy_from_user(
        plain_txt.data, plain_txt.data_len, encrypt->plain_data, encrypt->plain_len));

    chk_func_err_goto(kapi_sm2_encrypt(&encrypt->pub_key, &plain_txt, &cipher_txt));

    chk_func_err_goto(crypto_copy_to_user(
        encrypt->cipher_data, encrypt->cipher_len, cipher_txt.data, cipher_txt.data_len));
    encrypt->cipher_len = cipher_txt.data_len;

exit__:
    crypto_free(plain_txt.data);
    crypto_free(cipher_txt.data);
    return ret;
#else
    crypto_unused(argp);
    log_error("Unsupported SM2\n");
    return OT_ERR_CIPHER_UNSUPPORTED;
#endif
}

static td_s32 dispatch_sm2_decrypt(td_void *argp)
{
#ifdef CHIP_SM2_SUPPORT
    td_s32 ret;
    sm2_decrypt_t *decrypt = (sm2_decrypt_t *)argp;
    ot_cipher_common_data cipher_txt, plain_txt;

    chk_ptr_err_return(decrypt);
    chk_param_err_return((decrypt->plain_data == TD_NULL) || (decrypt->cipher_data == TD_NULL));
    chk_param_err_return(decrypt->cipher_len < SM2_ENCRYPT_PAD_LEN);
    chk_param_err_return(decrypt->cipher_len > MAX_MALLOC_BUF_SIZE);

    (td_void)memset_s(&plain_txt, sizeof(ot_cipher_common_data), 0, sizeof(ot_cipher_common_data));
    (td_void)memset_s(&cipher_txt, sizeof(ot_cipher_common_data), 0, sizeof(ot_cipher_common_data));
    cipher_txt.data_len = decrypt->cipher_len;
    plain_txt.data_len = decrypt->cipher_len - SM2_ENCRYPT_PAD_LEN;
    if (decrypt->plain_len < plain_txt.data_len) {
        log_error("cipher length is overflow.\n");
        return OT_ERR_CIPHER_INVALID_PARAM;
    }

    plain_txt.data = crypto_calloc(1, plain_txt.data_len);
    if (plain_txt.data == TD_NULL) {
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    cipher_txt.data = crypto_calloc(1, cipher_txt.data_len);
    if (cipher_txt.data == TD_NULL) {
        crypto_free(plain_txt.data);
        print_func_errno(crypto_calloc, OT_ERR_CIPHER_FAILED_MEM);
        return OT_ERR_CIPHER_FAILED_MEM;
    }

    chk_func_err_goto(crypto_copy_from_user(
        cipher_txt.data, cipher_txt.data_len, decrypt->cipher_data, decrypt->cipher_len));

    chk_func_err_goto(kapi_sm2_decrypt(&decrypt->pri_key, &cipher_txt, &plain_txt));

    chk_func_err_goto(crypto_copy_to_user(
        decrypt->plain_data, decrypt->plain_len, plain_txt.data, plain_txt.data_len));
    decrypt->plain_len = plain_txt.data_len;

exit__:
    crypto_free(plain_txt.data);
    crypto_free(cipher_txt.data);
    return ret;
#else
    crypto_unused(argp);
    log_error("Unsupported SM2\n");
    return OT_ERR_CIPHER_UNSUPPORTED;
#endif
}

static td_s32 dispatch_keyslot_create(td_void *argp)
{
    td_s32 ret;
    keyslot_create_t *create = (keyslot_create_t *)argp;

    func_enter();

    chk_ptr_err_return(create);

    ret = kapi_keyslot_create(&create->attr, &create->keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_keyslot_create, ret);
        return ret;
    }

    func_exit();

    return TD_SUCCESS;
}

static td_s32 dispatch_keyslot_destroy(td_void *argp)
{
    td_s32 ret;
    keyslot_destroy_t *destroy = (keyslot_destroy_t *)argp;

    chk_ptr_err_return(destroy);

    ret = kapi_keyslot_destroy(destroy->keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_keyslot_destroy, ret);
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 dispatch_symc_attach(td_void *argp)
{
    td_s32 ret;
    symc_keyslot_t *symc = (symc_keyslot_t *)argp;

    func_enter();

    chk_ptr_err_return(symc);

    ret = kapi_symc_attach(symc->cipher, symc->keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_attach, ret);
        return ret;
    }
    func_exit();

    return TD_SUCCESS;
}

static td_s32 dispatch_symc_detach(td_void *argp)
{
    td_s32 ret;
    symc_keyslot_t *symc = (symc_keyslot_t *)argp;

    func_enter();

    chk_ptr_err_return(symc);

    ret = kapi_symc_detach(symc->cipher, symc->keyslot);
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_detach, ret);
        return ret;
    }

    func_exit();

    return TD_SUCCESS;
}

static crypto_dispatch_func g_dispatch_func[CRYPTO_CMD_COUNT] = {
    {"CreateHandle",   dispatch_symc_create_handle,    CRYPTO_CMD_SYMC_CREATE},
    {"DestroyHandle",  dispatch_symc_destroy_handle,   CRYPTO_CMD_SYMC_DESTROY},
    {"ConfigChn",      dispatch_symc_cfg,              CRYPTO_CMD_SYMC_SET_CFG},
    {"GetSymcConfig",  dispatch_symc_get_cfg,          CRYPTO_CMD_SYMC_GET_CFG},
    {"Encrypt",        dispatch_symc_encrypt,          CRYPTO_CMD_SYMC_ENCRYPT},
    {"EncryptMulti",   dispatch_symc_encrypt_multi,    CRYPTO_CMD_SYMC_ENCRYPT_MULTI},
    {"GetTag",         dispatch_symc_get_tag,          CRYPTO_CMD_SYMC_GET_TAG},
    {"HashStart",      dispatch_hash_start,            CRYPTO_CMD_HASH_START},
    {"HashUpdate",     dispatch_hash_update,           CRYPTO_CMD_HASH_UPDATE},
    {"HashFinish",     dispatch_hash_finish,           CRYPTO_CMD_HASH_FINISH},
    {"TRNG",           dispatch_trng_get_random,       CRYPTO_CMD_TRNG},
    {"RsaEncrypt",     dispatch_rsa_encrypt,           CRYPTO_CMD_RSA_ENCRYPT},
    {"RsaDecrypt",     dispatch_rsa_decrypt,           CRYPTO_CMD_RSA_DECRYPT},
    {"RsaSign",        dispatch_rsa_sign_hash,         CRYPTO_CMD_RSA_SIGN},
    {"RsaVerify",      dispatch_rsa_verify_hash,       CRYPTO_CMD_RSA_VERIFY},
    {"Sm2Encrypt",     dispatch_sm2_encrypt,           CRYPTO_CMD_SM2_ENCRYPT},
    {"Sm2Decrypt",     dispatch_sm2_decrypt,           CRYPTO_CMD_SM2_DECRYPT},
    {"Sm2Sign",        dispatch_sm2_sign,              CRYPTO_CMD_SM2_SIGN},
    {"Sm2Verify",      dispatch_sm2_verify,            CRYPTO_CMD_SM2_VERIFY},
    {"KeyslotCreate",  dispatch_keyslot_create,        CRYPTO_CMD_KEYSLOT_CREATE},
    {"KeyslotDestroy", dispatch_keyslot_destroy,       CRYPTO_CMD_KEYSLOT_DESTROY},
    {"SymcAttach",     dispatch_symc_attach,           CRYPTO_CMD_SYMC_ATTACH},
    {"SymcDetach",     dispatch_symc_detach,           CRYPTO_CMD_SYMC_DETACH},
};

td_s32 crypto_ioctl(td_u32 cmd, td_void *argp)
{
    td_u32 nr;
    td_s32 ret;

    func_enter();

    nr = _IOC_NR(cmd);

    chk_ptr_err_return(argp);
    chk_param_err_return(nr >= CRYPTO_CMD_COUNT);
    chk_param_err_return(g_dispatch_func[nr].cmd != cmd);
    chk_ptr_err_return(g_dispatch_func[nr].func);

    log_dbg("cmd 0x%x, nr %d, size %d, local cmd 0x%x\n",
        cmd, nr, _IOC_SIZE(cmd), g_dispatch_func[nr].cmd);

    ret = g_dispatch_func[nr].func(argp);
    if (ret != TD_SUCCESS) {
        /* TRNG may be empty in FIFO, don't report error, try to read it again */
        if (cmd != CRYPTO_CMD_TRNG) {
            print_func_errno(crypto_dispatch_func, ret);
        }
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 crypto_entry(td_void)
{
    td_s32 ret;

    crypto_mem_init();

    ret = module_addr_map();
    if (ret != TD_SUCCESS) {
        print_func_errno(module_addr_map, ret);
        return ret;
    }

    ret = kapi_trng_init();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_trng_init, ret);
        goto error;
    }

    ret = kapi_symc_init();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_init, ret);
        goto error1;
    }

    ret = kapi_hash_init();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_init, ret);
        goto error2;
    }

#if (defined(CHIP_RSA_SUPPORT) || defined(SOFT_RSA_SUPPORT))
    ret = kapi_rsa_init();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_init, ret);
        kapi_hash_deinit();
        goto error2;
    }
#endif

#ifdef CHIP_SM2_SUPPORT
    ret = kapi_sm2_init();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_sm2_init, ret);
        kapi_rsa_deinit();
        kapi_hash_deinit();
        goto error2;
    }
#endif
    return TD_SUCCESS;

error2:
    kapi_symc_deinit();
error1:
    kapi_trng_deinit();
error:
    module_addr_unmap();
    return ret;
}

td_s32 crypto_exit(td_void)
{
    td_s32 ret;

    func_enter();

    ret = kapi_symc_deinit();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_deinit, ret);
        return ret;
    }

    ret = kapi_hash_deinit();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_deinit, ret);
        return ret;
    }

    ret = kapi_rsa_deinit();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_rsa_deinit, ret);
        return ret;
    }

#ifdef CHIP_SM2_SUPPORT
    ret = kapi_sm2_deinit();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_sm2_deinit, ret);
        return ret;
    }
#endif

    ret = kapi_trng_deinit();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_trng_deinit, ret);
        return ret;
    }

    ret = module_addr_unmap();
    if (ret != TD_SUCCESS) {
        print_func_errno(module_addr_unmap, ret);
        return ret;
    }

    func_exit();
    return TD_SUCCESS;
}

td_s32 crypto_release(td_void)
{
    td_s32 ret;

    ret = kapi_symc_release();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_symc_release, ret);
        return ret;
    }

    ret = kapi_hash_release();
    if (ret != TD_SUCCESS) {
        print_func_errno(kapi_hash_release, ret);
        return ret;
    }

    return TD_SUCCESS;
}

/** @} */ /** <!-- ==== Structure Definition end ==== */

