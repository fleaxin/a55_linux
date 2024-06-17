/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef KAPI_KLAD_H
#define KAPI_KLAD_H

#include "ot_common_klad.h"

td_s32 kapi_klad_init(td_void);

td_void kapi_klad_deinit(td_void);

td_s32 kapi_klad_create(td_handle *klad);

td_s32 kapi_klad_destroy(td_handle klad);

td_s32 kapi_klad_attach(td_handle klad, td_handle target);

td_s32 kapi_klad_detach(td_handle klad, td_handle target);

td_s32 kapi_klad_set_attr(td_handle klad, const ot_klad_attr *attr);

td_s32 kapi_klad_get_attr(td_handle klad, ot_klad_attr *attr);

td_s32 kapi_klad_set_session_key(td_handle klad, const ot_klad_session_key *key);

td_s32 kapi_klad_set_content_key(td_handle klad, const ot_klad_content_key *key);

td_s32 kapi_klad_set_clear_key(td_handle klad, const ot_klad_clear_key *key);

td_void kapi_klad_release(td_void);
#endif /* KAPI_KLAD_H */
