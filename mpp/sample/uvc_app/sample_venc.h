/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __SAMPLE_VENC_H__
#define __SAMPLE_VENC_H__

#include "ot_type.h"
#include "sample_comm.h"

td_s32 init_ot_encoder(td_void);
td_s32 start_ot_encoder(td_void);
td_s32 stop_ot_encoder(td_void);
td_s32 set_encoder_idr(td_void);

td_void set_user_config_format(ot_payload_type *format, ot_pic_size *wh, td_s32 *c);

#endif /* __SAMPLE_VENC_H__ */
