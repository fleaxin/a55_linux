/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <linux/device.h>
#include "osal_mmz.h"

#define NAME_LEN_MAX 64

struct mmz_allocator {
    int (*init)(char *args);
    ot_mmb_t *(*mmb_alloc)(const char *name,
                            unsigned long size,
                            unsigned long align,
                            unsigned long gfp,
                            const char *mmz_name,
                            const ot_mmz_t *_user_mmz);
    ot_mmb_t *(*mmb_alloc_v2)(const char *name,
                               unsigned long size,
                               unsigned long align,
                               unsigned long gfp,
                               const char *mmz_name,
                               const ot_mmz_t *_user_mmz,
                               unsigned int order);
    ot_mmb_t *(*mmb_alloc_v3)(const ot_mmz_alloc_para_in *para_in, ot_mmz_alloc_para_out *para_out);
    void *(*mmb_map2kern)(ot_mmb_t *mmb, int cached);
    int (*mmb_unmap)(ot_mmb_t *mmb);
    void (*mmb_free)(ot_mmb_t *mmb);
    void *(*mmf_map)(phys_addr_t phys, int len, int cache);
    void (*mmf_unmap)(void *virt);
};

int cma_allocator_setopt(struct mmz_allocator *allocator);
int ot_allocator_setopt(struct mmz_allocator *allocator);

#endif
