/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef MKP_PCIV_H
#define MKP_PCIV_H

#include "ot_common_pciv.h"
#include "mkp_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 chip_num;
    td_s32 chip_id[OT_PCIV_MAX_CHIP_NUM];
} ot_pciv_ioctl_enum_chip;

typedef struct {
    td_phys_addr_t phys_addr;
    td_u32         size;
} ot_pciv_ioctl_malloc;

typedef struct {
    td_u32          chn_id;
    td_u32          index;
    td_phys_addr_t  phys_addr;
    td_u32          size;
} ot_pciv_ioctl_malloc_chn_buf;

typedef enum {
    IOC_NR_PCIV_CREATE = 0,
    IOC_NR_PCIV_DESTROY,
    IOC_NR_PCIV_SET_ATTR,
    IOC_NR_PCIV_GET_ATTR,
    IOC_NR_PCIV_START,
    IOC_NR_PCIV_STOP,
    IOC_NR_PCIV_MALLOC,
    IOC_NR_PCIV_FREE,
    IOC_NR_PCIV_MALLOC_CHN_BUF,
    IOC_NR_PCIV_FREE_CHN_BUF,
    IOC_NR_PCIV_ENUM_BIND_OBJ,
    IOC_NR_PCIV_GET_WINDOW_BASE,
    IOC_NR_PCIV_GET_LOCAL_ID,
    IOC_NR_PCIV_ENUM_CHIP_ID,
    IOC_NR_PCIV_DMA_TASK,
    IOC_NR_PCIV_BIND_CHN_TO_FD,
    IOC_NR_PCIV_WINDOW_VB_CREATE,
    IOC_NR_PCIV_WINDOW_VB_DESTROY,
    IOC_NR_PCIV_SHOW,
    IOC_NR_PCIV_HIDE,
} ot_ioc_nr_pciv;

#define PCIV_CREATE_CTRL            _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_CREATE, ot_pciv_attr)
#define PCIV_DESTROY_CTRL           _IO  (IOC_TYPE_PCIV, IOC_NR_PCIV_DESTROY)
#define PCIV_SET_ATTR_CTRL          _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_SET_ATTR, ot_pciv_attr)
#define PCIV_GET_ATTR_CTRL          _IOR (IOC_TYPE_PCIV, IOC_NR_PCIV_GET_ATTR, ot_pciv_attr)
#define PCIV_START_CTRL             _IO  (IOC_TYPE_PCIV, IOC_NR_PCIV_START)
#define PCIV_STOP_CTRL              _IO  (IOC_TYPE_PCIV, IOC_NR_PCIV_STOP)
#define PCIV_MALLOC_CTRL            _IOWR(IOC_TYPE_PCIV, IOC_NR_PCIV_MALLOC, ot_pciv_ioctl_malloc)
#define PCIV_FREE_CTRL              _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_FREE, td_u64)
#define PCIV_MALLOC_CHN_BUF_CTRL    _IOWR(IOC_TYPE_PCIV, IOC_NR_PCIV_MALLOC_CHN_BUF, ot_pciv_ioctl_malloc_chn_buf)
#define PCIV_FREE_CHN_BUF_CTRL      _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_FREE_CHN_BUF, td_u32)
#define PCIV_DMA_TASK_CTRL          _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_DMA_TASK, ot_pciv_dma_task)
#define PCIV_GET_WINDOW_BASE_CTRL   _IOWR(IOC_TYPE_PCIV, IOC_NR_PCIV_GET_WINDOW_BASE, ot_pciv_window_base)
#define PCIV_GET_LOCAL_ID_CTRL      _IOR (IOC_TYPE_PCIV, IOC_NR_PCIV_GET_LOCAL_ID, td_s32)
#define PCIV_ENUM_CHIP_ID_CTRL      _IOR (IOC_TYPE_PCIV, IOC_NR_PCIV_ENUM_CHIP_ID, ot_pciv_ioctl_enum_chip)
#define PCIV_BIND_CHN_TO_FD_CTRL    _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_BIND_CHN_TO_FD, ot_pciv_chn)
#define PCIV_WINDOW_VB_CREATE_CTRL  _IOW (IOC_TYPE_PCIV, IOC_NR_PCIV_WINDOW_VB_CREATE,  ot_pciv_window_vb_cfg)
#define PCIV_WINDOW_VB_DESTROY_CTRL _IO  (IOC_TYPE_PCIV, IOC_NR_PCIV_WINDOW_VB_DESTROY)
#define PCIV_SHOW_CTRL              _IO  (IOC_TYPE_PCIV, IOC_NR_PCIV_SHOW)
#define PCIV_HIDE_CTRL              _IO  (IOC_TYPE_PCIV, IOC_NR_PCIV_HIDE)

#define pciv_err_trace(format, ...)                                                                        \
    OT_ERR_TRACE(OT_ID_PCIV, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_emerg_trace(format, ...)                                                                        \
    OT_EMERG_TRACE(OT_ID_PCIV, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_warn_trace(format, ...)                                                                        \
    OT_WARN_TRACE(OT_ID_PCIV, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_notice_trace(format, ...)                                                                        \
    OT_NOTICE_TRACE(OT_ID_PCIV, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_info_trace(format, ...)                                                                        \
    OT_INFO_TRACE(OT_ID_PCIV, "[Func]:%s [Line]:%d [Info]:" format, __FUNCTION__, __LINE__, ##__VA_ARGS__) \

#define pciv_check_chn_id_return(chn_id)                               \
do {                                                            \
    if (((chn_id) < 0) || ((chn_id) >= OT_PCIV_MAX_CHN_NUM)) { \
        pciv_err_trace("invalid chn id:%d \n", chn_id);         \
        return OT_ERR_PCIV_INVALID_CHN_ID;                      \
    }                                                           \
} while (0)

#define pciv_check_ptr_return(ptr)        \
do {                                      \
    if ((ptr) == NULL) {                  \
        pciv_err_trace("PTR is NULL!\n"); \
        return OT_ERR_PCIV_NULL_PTR;      \
    }                                     \
} while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

