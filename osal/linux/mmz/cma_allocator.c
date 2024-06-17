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

#include <asm/memory.h>
#include <linux/dma-contiguous.h>
#include <linux/dma-mapping.h>
#include <asm/memory.h>
#ifndef CONFIG_64BIT
#include <asm/highmem.h>
#include <asm/pgtable.h>
#endif
#include <asm/tlbflush.h>
#include <asm/pgtable.h>
#include <linux/vmalloc.h>
#include "securec.h"
#include "allocator.h"

struct cma_zone {
    struct device pdev;
    char name[NAME_LEN_MAX];
    unsigned long gfp;
    unsigned long phys_start;
    unsigned long nbytes;
    unsigned int alloc_type;
    unsigned long block_align;
};

long long max_malloc_size = 0x40000000UL;

#ifndef CONFIG_64BIT
static int __dma_update_pte(pte_t* pte, pgtable_t token,
                            unsigned long addr, void* data)
{
    struct page* page = virt_to_page(addr);

    pgprot_t prot = *(pgprot_t*)data;

    osal_unused(token);
    set_pte_ext(pte, mk_pte(page, prot), 0);

    return 0;
}

void __dma_clear_buffer(struct page* page, size_t size)
{
    osal_unused(page);
    osal_unused(size);
    return;
}

extern void bsp_flush_tlb_kernel_range(unsigned long start, unsigned long end);
static void __dma_remap(struct page* page, size_t size, pgprot_t prot)
{
    unsigned long start = (unsigned long)page_address(page);
    unsigned end = start + size;

    apply_to_page_range(&init_mm, start, size, __dma_update_pte, &prot);
    dsb();
    bsp_flush_tlb_kernel_range(start, end);
}
#else /* CONFIG_64BIT */


#endif /* CONFIG_64BIT */

static int do_mmb_alloc(ot_mmb_t* mmb)
{
    ot_mmb_t* p = NULL;

    /* add mmb sorted */
    osal_list_for_each_entry(p, &mmb->zone->mmb_list, list)
    {
        if (mmb->phys_addr < p->phys_addr)
            break;
        if (mmb->phys_addr == p->phys_addr)
            osal_trace(KERN_ERR "ERROR:cma allocator bad in %s(%s, %d)",
                    mmb->zone->name,  __func__, __LINE__);
    }
    osal_list_add(&mmb->list, p->list.prev);

    mmz_trace(1, OT_MMB_FMT_S, ot_mmb_fmt_arg(mmb));

    return 0;
}

static ot_mmb_t *mmb_real_alloc(ot_mmz_t *fixed_mmz, unsigned long fixed_start, unsigned long size,
    const char *name, unsigned int order)
{
    ot_mmb_t *mmb = NULL;
    errno_t err_value;
    mmb = kmalloc(sizeof(ot_mmb_t), GFP_KERNEL);
    if (mmb == NULL) {
        return NULL;
    }

    (void)memset_s(mmb, sizeof(ot_mmb_t), 0, sizeof(ot_mmb_t));

    mmb->zone      = fixed_mmz;
    mmb->phys_addr = fixed_start;
    mmb->length    = size;
    mmb->order     = order;

    if (name != NULL) {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, name, OT_MMB_NAME_LEN - 1);
    } else {
        err_value = strncpy_s(mmb->name, OT_MMB_NAME_LEN, "<null>", OT_MMB_NAME_LEN - 1);
    }

    if ((err_value != EOK) || do_mmb_alloc(mmb)) {
        kfree(mmb);
        mmb = NULL;
        return NULL;
    }

    return mmb;
}

static ot_mmb_t *__mmb_alloc(const char *name, unsigned long size, unsigned long align,
    unsigned long gfp, const char *mmz_name, const ot_mmz_t *_user_mmz)
{
    ot_mmz_t *mmz = NULL;
    ot_mmb_t *mmb = NULL;
    struct page *page = NULL;
    ot_mmz_t *fixed_mmz = NULL;

    unsigned long order, fixed_start;
    size_t count;

    /*
     * no more than 1GB
     */
    if (size == 0 || size > max_malloc_size)
        return NULL;

    if (align == 0)
        align = MMZ_GRAIN;

    size = mmz_grain_align(size);
    order = get_order(size);
    count = size >> PAGE_SHIFT;

    mmz_trace(1, "anonymous=%s,size=%luKB,align=%lu", mmz_name, size / SZ_1K, align);

    begin_list_for_each_mmz(mmz, gfp, mmz_name)

    if ((_user_mmz != NULL) && (_user_mmz != mmz))
        continue;

    page = dma_alloc_from_contiguous(mmz->cma_dev, count, order, 0);
    if (page == NULL)
        break;
    fixed_mmz = mmz;
    fixed_start = page_to_phys(page);
    break;

    end_list_for_each_mmz()

    if (fixed_mmz == NULL)
        return NULL;

    if (page != NULL) {
#ifndef CONFIG_64BIT
        __dma_clear_buffer(page, size);
#else
        (void)memset_s(page_address(page), size, 0, size);
        __flush_dcache_area(page_address(page), size);
#endif
    }

    mmb = mmb_real_alloc(fixed_mmz, fixed_start, size, name, 0);
    if (mmb == NULL) {
        goto cma_free;
    }

    return mmb;

cma_free:
    dma_release_from_contiguous(mmz->cma_dev, page, count);
    return NULL;
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

static ot_mmb_t *__mmb_alloc_v2(const char *name, unsigned long size, unsigned long align,
    unsigned long gfp, const char *mmz_name, const ot_mmz_t *_user_mmz, unsigned int order)
{
    ot_mmz_t *mmz = NULL;
    ot_mmb_t *mmb = NULL;
    struct page *page = NULL;
    ot_mmz_t *fixed_mmz = NULL;

    unsigned long cma_order;
    unsigned long fixed_start;
    size_t count = size >> PAGE_SHIFT;

    if ((size == 0) || (size > max_malloc_size))
        return NULL;

    if (align == 0) {
        align = 1;
    }

    size = mmz_grain_align(size);
    mmz_trace(1, "size=%luKB, align=%lu", size / SZ_1K, align);
    begin_list_for_each_mmz(mmz, gfp, mmz_name)
        if ((_user_mmz != NULL) && (_user_mmz != mmz))
            continue;

        if (mmz->alloc_type == SLAB_ALLOC) {
            size = mmz_align_power2(size);
        } else if (mmz->alloc_type == EQ_BLOCK_ALLOC) {
            size = mmz_align2(size, mmz->block_align);
        }

        cma_order = get_order(size);

        page = dma_alloc_from_contiguous(mmz->cma_dev, count, cma_order, 0);
        if (page == NULL) {
            return NULL;
        }
        fixed_mmz = mmz;
        fixed_start = page_to_phys(page);
        break;
    end_list_for_each_mmz()

    if (fixed_mmz == NULL) {
        return NULL;
    }

    mmb = mmb_real_alloc(fixed_mmz, fixed_start, size, name, order);
    if (mmb == NULL) {
        dma_release_from_contiguous(mmz->cma_dev, page, count);
        return NULL;
    }

    return mmb;
}

static void __mmb_free(ot_mmb_t *mmb)
{
    size_t count = mmb->length >> PAGE_SHIFT;
    struct page *page = phys_to_page(mmb->phys_addr);

    ot_mmz_t *mmz = mmb->zone;

    if (mmb->flags & OT_MMB_MAP2KERN_CACHED) {
#ifdef CONFIG_64BIT
        __flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
#else
        __cpuc_flush_dcache_area((void*)mmb->kvirt, (size_t)mmb->length);
        outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
#endif
    }

    dma_release_from_contiguous(mmz->cma_dev, page, count);

    osal_list_del(&mmb->list);
    kfree(mmb);
}

static int __mmb_unmap(ot_mmb_t* mmb);
#define MAX_KMALLOC_PAGENR  4

static void *cma_map_into_vmalloc_space(unsigned long len, struct page *phys_page, pgprot_t prot)
{
    unsigned int i;
    struct page **pages = NULL;
    unsigned int pagesnr = len / PAGE_SIZE;
    struct page *tmp = phys_page;
    unsigned int array_size = sizeof(struct page *) * pagesnr;
    void *kvirt = NULL;

    if (array_size == 0) {
        osal_trace(KERN_ERR "array_size (0x%x) is 0.\n", array_size);
        return NULL;
    }

    /*
     * Noted: mmb->length would be very large in some cases(for example:
     * more than one Giga Bytes). and array_size would be very large as
     * well. So, don't use kmalloc here.
     */
    pages = vmalloc(array_size);
    if (pages == NULL) {
        osal_trace(KERN_ERR "ptr array(0x%x) vmalloc failed.\n", array_size);
        return NULL;
    }

    for (i = 0; i < pagesnr; i++) {
        *(pages + i) = tmp;
        tmp++;
    }

    kvirt = vmap(pages, pagesnr, VM_MAP, prot);
    vfree(pages);
    pages = NULL;
    return kvirt;
}

static void *__mmb_map2kern(ot_mmb_t *mmb, int cached)
{
    pgprot_t prot;
    struct page* phys_page = phys_to_page(mmb->phys_addr);

    if (mmb->flags & OT_MMB_MAP2KERN) {
        if ((!!cached * OT_MMB_MAP2KERN_CACHED) != (mmb->flags & OT_MMB_MAP2KERN_CACHED)) {
            osal_trace(KERN_ERR "mmb<%s> has been kernel-mapped %s, can not be re-mapped as %s.",
                mmb->name, (mmb->flags & OT_MMB_MAP2KERN_CACHED) ? "cached" : "non-cached",
                (cached) ? "cached" : "non-cached");
            return NULL;
        }
        mmb->map_ref++;
        return mmb->kvirt;
    }

#ifdef CONFIG_64BIT
    if (cached) {
        mmb->flags |= OT_MMB_MAP2KERN_CACHED;
        prot = PAGE_KERNEL;
    } else {
        mmb->flags &= ~OT_MMB_MAP2KERN_CACHED;
        prot = __pgprot(PROT_NORMAL_NC);
    }
#else
    if (cached) {
        mmb->flags |= OT_MMB_MAP2KERN_CACHED;
        prot = pgprot_kernel;
    } else {
        mmb->flags &= ~OT_MMB_MAP2KERN_CACHED;
        prot = pgprot_noncached(pgprot_kernel);
    }
    __dma_remap(phys_page, mmb->length, prot);
#endif
    /*
     * Map into vmalloc space
     */
    mmb->kvirt = cma_map_into_vmalloc_space(mmb->length, phys_page, prot);
    if (mmb->kvirt == NULL) {
        /*
         * you should never get here.
         */
        osal_trace(KERN_ERR "mmb[0x%lx, 0x%lx] map to kernel failed\n",
                mmb->phys_addr, mmb->length);
        return NULL;
    }

    mmb->flags |= OT_MMB_MAP2KERN;
    mmb->map_ref++;

    return mmb->kvirt;
}

static int __mmb_unmap(ot_mmb_t *mmb)
{
    int ref;

    if (mmb->flags & OT_MMB_MAP2KERN_CACHED) {
#ifdef CONFIG_64BIT
        __flush_dcache_area((void *)mmb->kvirt, (size_t)mmb->length);
#else
        __cpuc_flush_dcache_area((void*)mmb->kvirt, (size_t)mmb->length);
        outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
#endif
    }

    if (mmb->flags & OT_MMB_MAP2KERN) {
        ref = --mmb->map_ref;
        if (mmb->map_ref != 0)
            return ref;
    }

    /*
     * unmap from vmalloc space.
     */
    {
        vunmap(mmb->kvirt);
    }
    mmb->kvirt = NULL;
    mmb->flags &= ~OT_MMB_MAP2KERN;
    mmb->flags &= ~OT_MMB_MAP2KERN_CACHED;

    if ((mmb->flags & OT_MMB_RELEASED) && (mmb->phy_ref == 0))
        __mmb_free(mmb);

    return 0;
}

static void *__mmf_map(phys_addr_t phys, int len, int cache)
{
    pgprot_t prot;
    struct page *phys_page = phys_to_page(phys);

#ifdef CONFIG_64BIT
    if (cache) {
        prot = PAGE_KERNEL;
    } else {
        prot = __pgprot(PROT_NORMAL_NC);
    }
#else
    if (cache) {
        prot = pgprot_kernel;
    } else {
        prot = pgprot_noncached(pgprot_kernel);
    }
#endif

    /*
     * Map into vmalloc space.
     */
    return cma_map_into_vmalloc_space(len, phys_page, prot);
}

/*
 * this function is added for some extra-requirements from
 * some customers, and the implementation is not strictly
 * in accordance with our early design.
 */
static void __mmf_unmap(void *virt)
{
    unsigned long vaddr = (unsigned long)(uintptr_t)virt;
    if ((vaddr >= VMALLOC_START) && (vaddr < VMALLOC_END))
        vunmap(virt);
}

static int allocator_init_parse_args(ot_mmz_t **zone, int argc, char **argv, struct cma_zone *cma_area)
{
    ot_mmz_t *mmz_zone = NULL;
    ot_mmz_create_attr mmz_create_attr = {0};
    if (argc == 4) { /* 4:Number of parameters */
        mmz_zone = ot_mmz_create("null", 0, 0, 0);
    } else if (argc == 6) { /* 6:Number of parameters */
        mmz_zone = ot_mmz_create_v2("null", &mmz_create_attr);
    } else {
        osal_trace(KERN_ERR "Input parameter num incorrect!\n");
        return -1;
    }

    if (mmz_zone == NULL) {
        return -1;
    }

    if (strncpy_s(mmz_zone->name, OT_MMZ_NAME_LEN, argv[0], OT_MMZ_NAME_LEN - 1) != EOK) {
        osal_trace("%s - strncpy_s failed!\n", __FUNCTION__);
        ot_mmz_destroy(mmz_zone);
        return -1;
    }

    osal_trace("cmz zone gfp 0x%lx, phys 0x%lx, nbytes 0x%lx\n",
        cma_area->gfp, cma_area->phys_start, cma_area->nbytes);
    mmz_zone->gfp        = cma_area->gfp;
    mmz_zone->phys_start = cma_area->phys_start;
    mmz_zone->nbytes     = cma_area->nbytes;
    mmz_zone->cma_dev    = &cma_area->pdev;
    if (mmz_zone->nbytes > max_malloc_size) {
        max_malloc_size = mmz_zone->nbytes;
    }
    if (argc == 6) { /* 6:Number of parameters */
        mmz_zone->alloc_type  = cma_area->alloc_type;
        mmz_zone->block_align = cma_area->block_align;
    }
    *zone = mmz_zone;
    return 0;
}

static int __allocator_init(char *s)
{
#ifdef CONFIG_CMA
    ot_mmz_t *zone = NULL;
    char *line = NULL;
    struct cma_zone *cma_area = NULL;
    int ret;

    while ((line = strsep(&s, ":")) != NULL) {
        int i;
        char *argv[6];
        extern struct cma_zone *get_cma_zone(const char *name);
        /*
         * We got 4 args in "line", formatted as
         * "argv[0],argv[1],argv[2],argv[3],argv[4]".
         * eg: "<mmz_name>,<gfp>,<phys_start>,<size>,<alloc_type>"
         * For more convenient, "hard code" are used such as "arg[0]", i.e.
         */
        for (i = 0; (argv[i] = strsep(&line, ",")) != NULL;) {
            if (++i == ARRAY_SIZE(argv)) break;
        }

        cma_area = get_cma_zone(argv[0]);
        if (cma_area == NULL) {
            osal_trace(KERN_ERR"can't get cma zone info:%s\n", argv[0]);
            continue;
        }

        ret = allocator_init_parse_args(&zone, i, argv, cma_area);
        if (ret != 0) {
            continue;
        }

        if (ot_mmz_register(zone)) {
            osal_trace(KERN_WARNING "Add MMZ failed: " OT_MMZ_FMT_S "\n",
                    ot_mmz_fmt_arg(zone));
            ot_mmz_destroy(zone);
        }

        zone = NULL;
    }
#endif
    return 0;
}

int cma_allocator_setopt(struct mmz_allocator *allocator)
{
    if (allocator == NULL) {
        return -1;
    }
    allocator->init = __allocator_init;
    allocator->mmb_alloc = __mmb_alloc;
    allocator->mmb_alloc_v2 = __mmb_alloc_v2;
    allocator->mmb_map2kern = __mmb_map2kern;
    allocator->mmb_unmap = __mmb_unmap;
    allocator->mmb_free = __mmb_free;
    allocator->mmf_map = __mmf_map;
    allocator->mmf_unmap = __mmf_unmap;
    return 0;
}

