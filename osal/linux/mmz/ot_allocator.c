/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/list.h>
#include <asm/cacheflush.h>
#include <linux/version.h>
#include "securec.h"
#include "allocator.h"

long long g_ot_max_malloc_size = 0x40000000UL;        /* 1GB */

static unsigned long _strtoul_ex(const char *s, char **ep, unsigned int base)
{
    char *__end_p = NULL;
    unsigned long __value;

    __value = simple_strtoul(s, &__end_p, base);

    switch (*__end_p) {
        case 'm':
        case 'M':
            __value <<= 10; /* 10: 1M=1024k, left shift 10bit */
            /* fall-through */
        case 'k':
        case 'K':
            __value <<= 10; /* 10: 1K=1024Byte, left shift 10bit */
            if (ep != NULL) {
                (*ep) = __end_p + 1;
            }
            /* fall-through */
        default:
            break;
    }

    return __value;
}

static int calculate_fixed_region(unsigned long end, unsigned long start,
    unsigned long *fixed_start, unsigned long *fixed_len, unsigned long len)
{
    unsigned long blank_len;
    int ret = 0;

    blank_len = end - start;
    if ((blank_len < *fixed_len) && (blank_len >= len)) {
        *fixed_len = blank_len;
        *fixed_start = start;
        mmz_trace(MMZ_INFO_LEVEL, "fixed_region: start=0x%08lX, len=%luKB\n",
            *fixed_start, *fixed_len / SZ_1K);
        ret = 1;
    }
    return ret;
}

static unsigned long find_fit_region(unsigned long *region_len,
    ot_mmz_t *mmz, unsigned long size, unsigned long align)
{
    unsigned long start, len;
    unsigned long fixed_start = 0;
    unsigned long fixed_len = -1;
    ot_mmb_t *p = NULL;

    align = mmz_grain_align(align);
    if (align == 0) {
        align = MMZ_GRAIN;
    }
    start = mmz_align2(mmz->phys_start, align);
    len = mmz_grain_align(size);

    list_for_each_entry(p, &mmz->mmb_list, list) {
        ot_mmb_t *next = NULL;
        mmz_trace(MMZ_INFO_LEVEL, "p->phys_addr=0x%08lX p->length = %luKB \t", p->phys_addr, p->length / SZ_1K);
        next = list_entry(p->list.next, typeof(*p), list);
        mmz_trace(MMZ_INFO_LEVEL, ",next = 0x%08lX\n\n", next->phys_addr);
        /*
         * if p is the first entry or not.
         */
        if (list_first_entry(&mmz->mmb_list, typeof(*p), list) == p) {
            (void)calculate_fixed_region(p->phys_addr, start, &fixed_start, &fixed_len, len);
        }
        start = mmz_align2((p->phys_addr + p->length), align);
        /* if aglin is larger than mmz->nbytes, it would trigger the BUG_ON */
        /* if we have to alloc after the last node.  */
        if (osal_list_is_last(&p->list, &mmz->mmb_list)) {
            if (calculate_fixed_region(mmz->phys_start + mmz->nbytes, start, &fixed_start, &fixed_len, len) == 1) {
                break;
            } else {
                if (fixed_len != (unsigned long)-1) {
                    goto out;
                }
                fixed_start = 0;
                mmz_trace(MMZ_INFO_LEVEL, "fixed_region: start=0x%08lX, len=%luKB\n", fixed_start, fixed_len / SZ_1K);
                goto out;
            }
        }
        /* blank is too small */
        if ((start + len) > next->phys_addr) {
            mmz_trace(MMZ_INFO_LEVEL, "start=0x%08lX ,len=%lu,next=0x%08lX\n", start, len, next->phys_addr);
            continue;
        }

        (void)calculate_fixed_region(next->phys_addr, start, &fixed_start, &fixed_len, len);
    }

    if ((mmz_grain_align(start + len) <= (mmz->phys_start + mmz->nbytes)) &&
        (start >= mmz->phys_start) && (start < (mmz->phys_start + mmz->nbytes))) {
        fixed_start = start;
    } else {
        fixed_start = 0;
    }

    mmz_trace(MMZ_INFO_LEVEL, "fixed_region: start=0x%08lX, len=%luKB\n", fixed_start, len / SZ_1K);
out:
    *region_len = len;
    return fixed_start;
}

static unsigned long find_fixed_region_from_highaddr(unsigned long *region_len,
                                                     const ot_mmz_t *mmz, unsigned long size, unsigned long align)
{
    int j;
    unsigned int i;
    unsigned long fixed_start = 0;
    unsigned long fixed_len = ~1;

    i = mmz_length2grain(mmz->nbytes);

    for (; i > 0; i--) {
        unsigned long start;
        unsigned long len;
        unsigned long start_highaddr;

        if (mmz_get_bit(mmz, i)) {
            continue;
        }

        len = 0;
        start_highaddr = mmz_pos2phy_addr(mmz, i);
        for (; i > 0; i--) {
            if (mmz_get_bit(mmz, i)) {
                break;
            }

            len += MMZ_GRAIN;
        }

        if (len >= size) {
            j = mmz_phy_addr2pos(mmz, mmz_align2low(start_highaddr - size, align));
            start = mmz_pos2phy_addr(mmz, j);
            if ((start_highaddr - len <= start) && (start <= start_highaddr - size)) {
                fixed_len = len;
                fixed_start = start;
                break;
            }

            mmz_trace(1, "fixed_region: start=0x%08lX, len=%luKB",
                      fixed_start, fixed_len / SZ_1K);
        }
    }

    *region_len = fixed_len;

    return fixed_start;
}

static int do_mmb_alloc(ot_mmb_t *mmb)
{
    ot_mmb_t *p = NULL;

    /* add mmb sorted */
    osal_list_for_each_entry(p, &mmb->zone->mmb_list, list) {
        if (mmb->phys_addr < p->phys_addr) {
            break;
        }
        if (mmb->phys_addr == p->phys_addr) {
            osal_trace(KERN_ERR "ERROR: media-mem allocator bad in %s! (%s, %d)",
                   mmb->zone->name, __FUNCTION__, __LINE__);
        }
    }
    osal_list_add(&mmb->list, p->list.prev);

    mmz_trace(1, OT_MMB_FMT_S, ot_mmb_fmt_arg(mmb));

    return 0;
}

static ot_mmb_t *__mmb_alloc(const char *name, unsigned long size, unsigned long align, unsigned long gfp,
    const char *mmz_name, const ot_mmz_t *_user_mmz)
{
    ot_mmz_t *mmz = NULL;
    ot_mmb_t *mmb = NULL;

    unsigned long start, region_len;

    unsigned long fixed_start = 0;
    unsigned long fixed_len = ~1;
    ot_mmz_t *fixed_mmz = NULL;
    errno_t err_value;

    if ((size == 0) || (size > g_ot_max_malloc_size)) {
        return NULL;
    }
    if (align == 0) {
        align = MMZ_GRAIN;
    }

    size = mmz_grain_align(size);

    mmz_trace(1, "size=%luKB, align=%lu", size / SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)

    if ((_user_mmz != NULL) && (_user_mmz != mmz)) {
        continue;
    }

    start = find_fit_region(&region_len, mmz, size, align);
    if ((fixed_len > region_len) && (start != 0)) {
        fixed_len = region_len;
        fixed_start = start;
        fixed_mmz = mmz;
    }
    end_list_for_each_mmz()

    if (fixed_mmz == NULL) {
        return NULL;
    }

    mmb = kmalloc(sizeof(ot_mmb_t), GFP_KERNEL);
    if (mmb == NULL) {
        return NULL;
    }

    (void)memset_s(mmb, sizeof(ot_mmb_t), 0, sizeof(ot_mmb_t));
    mmb->zone = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    if (name != NULL) {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, name, OT_MMB_NAME_LEN - 1);
    } else {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, "<null>", OT_MMB_NAME_LEN - 1);
    }

    if ((err_value != EOK) || do_mmb_alloc(mmb)) {
        kfree(mmb);
        mmb = NULL;
    }

    return mmb;
}

static ot_mmb_t *do_mmb_alloc_v2(unsigned long fixed_start, ot_mmz_t *fixed_mmz, unsigned long size,
    unsigned int order, const char *name)
{
    ot_mmb_t *mmb = NULL;
    errno_t err_value;

    mmb = kmalloc(sizeof(ot_mmb_t), GFP_KERNEL);
    if (mmb == NULL) {
        return NULL;
    }

    (void)memset_s(mmb, sizeof(ot_mmb_t), 0, sizeof(ot_mmb_t));
    mmb->zone = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    mmb->order = order;

    if (name != NULL) {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, name, OT_MMB_NAME_LEN - 1);
    } else {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, "<null>", OT_MMB_NAME_LEN - 1);
    }

    if ((err_value != EOK) || do_mmb_alloc(mmb)) {
        kfree(mmb);
        return NULL;
    }

    return mmb;
}

static unsigned long mmz_align_power2(unsigned long size)
{
    unsigned int i;
    unsigned long tmp_size = size;
    if ((tmp_size - 1) & tmp_size) {
        for (i = 1; i < 32; i++) { /* 32: the max size is 2^(32-1) */
            if (!((tmp_size >> i) & ~0)) {
                tmp_size = 1 << i;
                break;
            }
        }
    }
    return tmp_size;
}

static ot_mmb_t *__mmb_alloc_v2(const char *name, unsigned long size, unsigned long align, unsigned long gfp,
    const char *mmz_name, const ot_mmz_t *_user_mmz, unsigned int order)
{
    ot_mmz_t *mmz = NULL;

    unsigned long start = 0;
    unsigned long region_len = 0;

    unsigned long fixed_start = 0;
    unsigned long fixed_len = ~1;
    ot_mmz_t *fixed_mmz = NULL;

    if ((size == 0) || (size > 0x40000000UL)) {
        return NULL;
    }
    if (align == 0) {
        align = 1;
    }

    size = mmz_grain_align(size);

    mmz_trace(1, "size=%luKB, align=%lu", size / SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)

    if ((_user_mmz != NULL) && (_user_mmz != mmz)) {
        continue;
    }

    if (mmz->alloc_type == SLAB_ALLOC) {
        size = mmz_align_power2(size);
    } else if (mmz->alloc_type == EQ_BLOCK_ALLOC) {
        size = mmz_align2(size, mmz->block_align);
    }

    if (order == LOW_TO_HIGH) {
        start = find_fit_region(&region_len, mmz, size, align);
    } else if (order == HIGH_TO_LOW) {
        start = find_fixed_region_from_highaddr(&region_len, mmz, size, align);
    }

    if ((fixed_len > region_len) && (start != 0)) {
        fixed_len = region_len;
        fixed_start = start;
        fixed_mmz = mmz;
    }

    end_list_for_each_mmz()

    if (fixed_mmz == NULL) {
        return NULL;
    }

    return do_mmb_alloc_v2(fixed_start, fixed_mmz, size, order, name);
}

static unsigned long adjust_start_addr(unsigned long begin, unsigned long end, unsigned long mark, unsigned long len)
{
    unsigned long start_addr;

    if ((begin + len) > mark) {
        start_addr = begin;
    } else if ((mark + len) > end) {
        start_addr = end - len;
    } else {
        start_addr = mark;
    }

    return start_addr;
}

static unsigned long find_fixed_region(ot_mmz_t *mmz, unsigned long start, unsigned long size,
    unsigned long align, int *error_code)
{
    unsigned long region_start, region_end;
    unsigned long mmz_start, mmz_end;
    ot_mmb_t *cur = NULL;
    ot_mmb_t *next = NULL;

    mmz_start = mmz->phys_start;
    mmz_end = mmz->phys_start + mmz->nbytes;

    if (start < mmz_start || start >= mmz_end) {
        *error_code = MMZ_REGION_OUT_RANGE;
        return 0;
    }

    if (osal_list_empty(&mmz->mmb_list) != 0) {
        *error_code = MMZ_REGION_FREE;
        region_start = mmz_align2(mmz_start, align);
        return adjust_start_addr(region_start, mmz_end, start, size);
    }

    list_for_each_entry(cur, &mmz->mmb_list, list) {
        if (start >= cur->phys_addr && start < (cur->phys_addr + cur->length)) {
            *error_code = MMZ_REGION_BUSY;
            return 0;
        }

        if (start < cur->phys_addr) {
            if (list_first_entry(&mmz->mmb_list, typeof(*cur), list) == cur) {
                region_start = mmz_align2(mmz_start, align);
                if ((region_start + size) <= cur->phys_addr) {
                    *error_code = MMZ_REGION_FREE;
                    return adjust_start_addr(region_start, cur->phys_addr, start, size);
                } else {
                    *error_code = MMZ_REGION_NOT_ENOUGH;
                    mmz_trace(MMZ_INFO_LEVEL, "Find from head fail, mmz_start=0x%lX, pos=0x%lX, mmb_begin=0x%lX\n",
                        mmz_start, start, cur->phys_addr);
                    return 0;
                }
            } else {
                mmz_trace(MMZ_INFO_LEVEL, "Invalid, mmz_start=0x%08lX, pos=0x%08lX, mmb_begin=0x%08lX\n\n",
                    mmz_start, start, cur->phys_addr);
                *error_code = MMZ_REGION_OUT_RANGE;
                return 0;
            }
        }

        region_start = mmz_align2((cur->phys_addr + cur->length), align);
        if (osal_list_is_last(&cur->list, &mmz->mmb_list)) {
            if ((region_start + size) <= mmz_end) {
                *error_code = MMZ_REGION_FREE;
                return adjust_start_addr(region_start, mmz_end, start, size);
            } else {
                *error_code = MMZ_REGION_NOT_ENOUGH;
                mmz_trace(MMZ_INFO_LEVEL, "Find from tail failed, mmb_start=0x%08lX, pos=0x%08lX, mmz_end=0x%08lX\n\n",
                    region_start, start, mmz_end);
                return 0;
            }
        }

        next = list_entry(cur->list.next, typeof(*cur), list);
        region_end = next->phys_addr;
        if (start < region_end) {
            if ((region_end - region_start) >= size) {
                *error_code = MMZ_REGION_FREE;
                return adjust_start_addr(region_start, region_end, start, size);
            } else {
                *error_code = MMZ_REGION_NOT_ENOUGH;
                mmz_trace(MMZ_INFO_LEVEL, "Find from middle fail, pre_end=0x%08lX, pos=0x%08lX, nxt_start=0x%08lX\n\n",
                    region_start, start, region_end);
                return 0;
            }
        }
    }

    mmz_trace(MMZ_INFO_LEVEL, "Invalid: mmz_start=0x%08lX, pos=0x%08lX, mmz_end=0x%08lX\n", mmz_start, start, mmz_end);

    *error_code = MMZ_REGION_OUT_RANGE;
    return 0;
}

static ot_mmb_t *__mmb_alloc_v3(const ot_mmz_alloc_para_in *para_in, ot_mmz_alloc_para_out *para_out)
{
    ot_mmz_t *mmz = NULL;
    ot_mmb_t *mmb = NULL;

    unsigned long fixed_start = 0;

    const char *mmz_name = para_in->mmz_name;
    const char *buf_name = para_in->buf_name;
    unsigned long start = para_in->start;
    unsigned long size = para_in->size;
    unsigned long align = para_in->align;
    unsigned long gfp = para_in->gfp;

    errno_t err_value;

    if ((size == 0) || (size > g_ot_max_malloc_size)) {
        return NULL;
    }

    align = mmz_grain_align(align);
    if (align == 0) {
        align = MMZ_GRAIN;
    }

    size = mmz_align2(size, align);
    start &= ~(align - 1);

    mmz_trace(MMZ_INFO_LEVEL, "start= %lx size=%lx, align=%lx", start, size, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)

    fixed_start = find_fixed_region(mmz, start, size, align, &(para_out->erro_num));
    if (para_out->erro_num != MMZ_REGION_OUT_RANGE) {
        break;
    }

    end_list_for_each_mmz()

    if (mmz == NULL || fixed_start == 0) {
        mmz_trace(MMZ_INFO_LEVEL, "find fixed region failed. fixed_start= %lx", fixed_start);
        return NULL;
    }

    mmb = kmalloc(sizeof(ot_mmb_t), GFP_KERNEL);
    if (mmb == NULL) {
        return NULL;
    }

    (void)memset_s(mmb, sizeof(ot_mmb_t), 0, sizeof(ot_mmb_t));
    mmb->zone = mmz;
    mmb->phys_addr = fixed_start;
    mmb->length = size;
    if (buf_name != NULL) {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, buf_name, OT_MMB_NAME_LEN - 1);
    } else {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, "<null>", OT_MMB_NAME_LEN - 1);
    }

    if ((err_value != EOK) || do_mmb_alloc(mmb)) {
        kfree(mmb);
        mmb = NULL;
    }

    return mmb;
}

static void *__mmb_map2kern(ot_mmb_t *mmb, int cached)
{
    /*
      * already mapped? no need to remap again,
      * just return mmb's kernel virtual address.
      */
    if (mmb->flags & OT_MMB_MAP2KERN) {
        if ((!!cached * OT_MMB_MAP2KERN_CACHED) != (mmb->flags & OT_MMB_MAP2KERN_CACHED)) {
            osal_trace(KERN_ERR "mmb<%s> has been kernel-mapped as %s, can not be re-mapped as %s.",
                   mmb->name,
                   (mmb->flags & OT_MMB_MAP2KERN_CACHED) ? "cached" : "non-cached",
                   (cached) ? "cached" : "non-cached");
            return NULL;
        }

        mmb->map_ref++;

        return mmb->kvirt;
    }

    if (cached) {
        mmb->flags |= OT_MMB_MAP2KERN_CACHED;
        mmb->kvirt = ioremap_cache(mmb->phys_addr, mmb->length);
    } else {
        mmb->flags &= ~OT_MMB_MAP2KERN_CACHED;
        /* ioremap_wc has better performance */
        mmb->kvirt = ioremap_wc(mmb->phys_addr, mmb->length);
    }

    if (mmb->kvirt) {
        mmb->flags |= OT_MMB_MAP2KERN;
        mmb->map_ref++;
    } else {
        mmb->flags &= ~OT_MMB_MAP2KERN_CACHED;
    }

    return mmb->kvirt;
}

static void __mmb_free(ot_mmb_t *mmb)
{
    if (mmb->flags & OT_MMB_MAP2KERN_CACHED) {
#ifdef CONFIG_64BIT
        __flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
#else
        __cpuc_flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
        outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
#endif
    }

    osal_list_del(&mmb->list);
    kfree(mmb);
}

static int __mmb_unmap(ot_mmb_t *mmb)
{
    int ref;

    if (mmb->flags & OT_MMB_MAP2KERN_CACHED) {
#ifdef CONFIG_64BIT
        __flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
#else
        __cpuc_flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
        outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
#endif
    }

    if (mmb->flags & OT_MMB_MAP2KERN) {
        ref = --mmb->map_ref;
        if (mmb->map_ref != 0) {
            return ref;
        }
        iounmap(mmb->kvirt);
    }

    mmb->kvirt = NULL;
    mmb->flags &= ~OT_MMB_MAP2KERN;
    mmb->flags &= ~OT_MMB_MAP2KERN_CACHED;

    if ((mmb->flags & OT_MMB_RELEASED) && (mmb->phy_ref == 0)) {
        __mmb_free(mmb);
    }

    return 0;
}

static void *__mmf_map(phys_addr_t phys, int len, int cache)
{
    void *virt = NULL;
    if (cache) {
        virt = ioremap_cache(phys, len);
    } else {
        virt = ioremap_wc(phys, len);
    }

    return virt;
}

static void __mmf_unmap(void *virt)
{
    if (virt != NULL) {
        iounmap(virt);
    }
}

static int allocator_init_parse_args(ot_mmz_t **zone, int argc, char **argv)
{
    ot_mmz_t *mmz_zone = NULL;
    ot_mmz_create_attr mmz_create_attr = {0};
    if (argc == 4) { /* 4:Number of parameters */
        mmz_zone = ot_mmz_create("null", 0, 0, 0);
    } else if (argc == 6) { /* 6:Number of parameters */
        mmz_zone = ot_mmz_create_v2("null", &mmz_create_attr);
    } else {
        osal_trace(KERN_ERR "error parameters\n");
        return -EINVAL;
    }
    if (mmz_zone == NULL) {
        return 0;
    }
    if (strncpy_s(mmz_zone->name, OT_MMZ_NAME_LEN, argv[0], OT_MMZ_NAME_LEN - 1) != EOK) {
        osal_trace("%s - strncpy_s failed!\n", __FUNCTION__);
        ot_mmz_destroy(mmz_zone);
        return -1;
    }
    mmz_zone->gfp        = _strtoul_ex(argv[1], NULL, 0); /* 1: the second args */
    mmz_zone->phys_start = _strtoul_ex(argv[2], NULL, 0); /* 2: the third args */
    mmz_zone->nbytes     = _strtoul_ex(argv[3], NULL, 0); /* 3: the fourth args */
    if (mmz_zone->nbytes > g_ot_max_malloc_size) {
        g_ot_max_malloc_size = mmz_zone->nbytes;
    }
    if (argc == 6) { /* 6: had parse six args */
        mmz_zone->alloc_type  = _strtoul_ex(argv[4], NULL, 0); /* 4: the fifth args */
        mmz_zone->block_align = _strtoul_ex(argv[5], NULL, 0); /* 5: the sixth args */
    }
    *zone = mmz_zone;
    return 1;
}

static int __allocator_init(char *s)
{
    ot_mmz_t *zone = NULL;
    char *line = NULL;
    unsigned long phys_end;
    int ret;

    while ((line = strsep(&s, ":")) != NULL) {
        int i;
        char *argv[6]; /* 6: cmdline include 6 arguments */

        for (i = 0; (argv[i] = strsep(&line, ",")) != NULL;) {
            if (++i == ARRAY_SIZE(argv)) {
                break;
            }
        }

        ret = allocator_init_parse_args(&zone, i, argv);
        if (ret == 0) {
            continue;
        } else if (ret < 0) {
            return ret;
        }

        if (ot_mmz_register(zone)) {
            osal_trace(KERN_WARNING "Add MMZ failed: " OT_MMZ_FMT_S "\n", ot_mmz_fmt_arg(zone));
            ot_mmz_destroy(zone);
            return -1;
        }

        /* if phys_end is maximum value (ex, 0xFFFFFFFF 32bit) */
        phys_end = (zone->phys_start + zone->nbytes);

        if ((phys_end == 0) && (zone->nbytes >= PAGE_SIZE)) {
            /* reserve last PAGE_SIZE memory */
            zone->nbytes = zone->nbytes - PAGE_SIZE;
        }

        /* if phys_end exceed 0xFFFFFFFF (32bit), wrapping error */
        if ((zone->phys_start > phys_end) && (phys_end != 0)) {
            osal_trace(KERN_ERR "MMZ: parameter is not correct! Address exceeds 0xFFFFFFFF\n");
            ot_mmz_unregister(zone);
            ot_mmz_destroy(zone);
            return -1;
        }

        zone = NULL;
    }

    return 0;
}

int ot_allocator_setopt(struct mmz_allocator *allocator)
{
    if (allocator == NULL) {
        osal_trace(KERN_ERR "error parameters\n");
        return -1;
    }
    allocator->init = __allocator_init;
    allocator->mmb_alloc = __mmb_alloc;
    allocator->mmb_alloc_v2 = __mmb_alloc_v2;
    allocator->mmb_alloc_v3 = __mmb_alloc_v3;
    allocator->mmb_map2kern = __mmb_map2kern;
    allocator->mmb_unmap = __mmb_unmap;
    allocator->mmb_free = __mmb_free;
    allocator->mmf_map = __mmf_map;
    allocator->mmf_unmap = __mmf_unmap;
    return 0;
}
