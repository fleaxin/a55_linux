/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef PCIV_MSG_H
#define PCIV_MSG_H

#ifndef __KERNEL__
#include <sys/time.h>
#endif

#include "ot_common.h"
#include "ot_type.h"
#include "ot_common_pciv.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PCIV_MSG_BASE_PORT    100 /* we use msg port above this value */
#define PCIV_MSG_MAX_PORT_NUM 262 /* max msg port count, also you can change it */
#define PCIV_MSG_MAX_PORT     ((PCIV_MSG_BASE_PORT) + (PCIV_MSG_MAX_PORT_NUM))

#define PCIV_MSGPORT_COMM_CMD PCIV_MSG_BASE_PORT /* common msg port, used for general command */

#define SAMPLE_PCIV_MSG_HEADLEN sizeof(sample_pciv_msghead)
#define SAMPLE_PCIV_MSG_MAXLEN  1036

#define pciv_check_expr_return(expr)     \
do {                                   \
    if (!(expr)) {                    \
        printf("\nFailure at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",  \
            __FUNCTION__, __LINE__, #expr); \
        return TD_FAILURE; \
    } \
} while (0)

typedef struct {
    td_u32 target;   /* the target PCI chip ID. follow PCIV definition */
    td_u32 msg_type; /* message type that you defined */
    td_u32 msg_len;  /* length of message BODY (exclude header) */
    td_s32 ret_val;  /* return value from target chip */
} sample_pciv_msghead;

typedef struct {
    sample_pciv_msghead msg_head;
    td_u8 c_msg_body[SAMPLE_PCIV_MSG_MAXLEN];
} sample_pciv_msg;

td_s32 pciv_wait_connect(td_s32 tgt);

td_void pciv_free_all_msg_port(td_void);
td_s32 pciv_alloc_msg_port(td_s32 *msg_port);
td_s32 pciv_open_msg_port(td_s32 tgt_id, td_s32 port);
td_s32 pciv_close_msg_port(td_s32 tgt_id, td_s32 port);
td_s32 pciv_send_msg(td_s32 tgt_id, td_s32 port, sample_pciv_msg *msg);
td_s32 pciv_read_msg(td_s32 tgt_id, td_s32 port, sample_pciv_msg *msg);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif

