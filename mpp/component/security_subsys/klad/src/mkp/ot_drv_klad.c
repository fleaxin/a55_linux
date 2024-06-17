/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "ot_drv_klad.h"

#include <linux/kernel.h>
#include "ot_debug_klad.h"
#include "kapi_klad.h"

#define crypto_unused(x)    ((x) = (x))

td_s32 ot_drv_klad_init(td_void)
{
    return TD_SUCCESS;
}
EXPORT_SYMBOL(ot_drv_klad_init);

td_s32 ot_drv_klad_deinit(td_void)
{
    return TD_SUCCESS;
}
EXPORT_SYMBOL(ot_drv_klad_deinit);

td_s32 ot_drv_klad_create(td_handle *klad)
{
    ot_klad_formula_fail_return(klad == TD_NULL, OT_ERR_KLAD_NULL_PTR);
    return kapi_klad_create(klad);
}
EXPORT_SYMBOL(ot_drv_klad_create);

td_s32 ot_drv_klad_destroy(td_handle klad)
{
    return kapi_klad_destroy(klad);
}
EXPORT_SYMBOL(ot_drv_klad_destroy);

td_s32 ot_drv_klad_attach(td_handle klad, td_handle target)
{
    return kapi_klad_attach(klad, target);
}
EXPORT_SYMBOL(ot_drv_klad_attach);

td_s32 ot_drv_klad_detach(td_handle klad, td_handle target)
{
    return kapi_klad_detach(klad, target);
}
EXPORT_SYMBOL(ot_drv_klad_detach);

td_s32 ot_drv_klad_set_attr(td_handle klad, const ot_klad_attr *attr)
{
    ot_klad_formula_fail_return(attr == TD_NULL, OT_ERR_KLAD_NULL_PTR);

    return kapi_klad_set_attr(klad, attr);
}
EXPORT_SYMBOL(ot_drv_klad_set_attr);

td_s32 ot_drv_klad_get_attr(td_handle klad, ot_klad_attr *attr)
{
    ot_klad_formula_fail_return(attr == TD_NULL, OT_ERR_KLAD_NULL_PTR);

    return kapi_klad_set_attr(klad, attr);
}
EXPORT_SYMBOL(ot_drv_klad_get_attr);

td_s32 ot_drv_klad_set_session_key(td_handle klad, const ot_klad_session_key *key)
{
    ot_klad_formula_fail_return(key == TD_NULL, OT_ERR_KLAD_NULL_PTR);

    return kapi_klad_set_session_key(klad, key);
}
EXPORT_SYMBOL(ot_drv_klad_set_session_key);

td_s32 ot_drv_klad_set_content_key(td_handle klad, const ot_klad_content_key *key)
{
    ot_klad_formula_fail_return(key == TD_NULL, OT_ERR_KLAD_NULL_PTR);

    return kapi_klad_set_content_key(klad, key);
}
EXPORT_SYMBOL(ot_drv_klad_set_content_key);


td_s32 ot_drv_klad_set_clear_key(td_handle klad, const ot_klad_clear_key *key)
{
    ot_klad_formula_fail_return(key == TD_NULL, OT_ERR_KLAD_NULL_PTR);

    return kapi_klad_set_clear_key(klad, key);
}
EXPORT_SYMBOL(ot_drv_klad_set_clear_key);