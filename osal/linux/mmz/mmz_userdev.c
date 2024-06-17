/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/dma-mapping.h>
#include <linux/uaccess.h>

#ifdef CONFIG_OT_SYS_SMMU_SUPPORT
#include <linux/iommu.h>
#include "linux/vendor/sva_ext.h"
#endif

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/cacheflush.h>
#include "securec.h"
#include "osal_mmz.h"
#include "ot_osal.h"
#include "mmz.h"

#define error_mmz(s...)                                        \
    do {                                                   \
        osal_trace(KERN_ERR "mmz_userdev:%s: ", __FUNCTION__); \
        osal_trace(s);                                         \
    } while (0)
#define warning(s...)                                          \
    do {                                                       \
        osal_trace(KERN_WARNING "mmz_userdev:%s: ", __FUNCTION__); \
        osal_trace(s);                                             \
    } while (0)

struct mmz_userdev_info {
    pid_t pid;
    pid_t mmap_pid;
    struct semaphore sem;
    struct osal_list_head list;
};

static void mmz_flush_dcache_mmb_dirty(struct dirty_area *p_area)
{
#ifdef CONFIG_64BIT
    __flush_dcache_area(p_area->dirty_virt_start, p_area->dirty_size);
#else
    /* flush l1 cache, use vir addr */
    __cpuc_flush_dcache_area(p_area->dirty_virt_start, p_area->dirty_size);

#ifdef CONFIG_CACHE_L2X0
    /* flush l2 cache, use paddr */
    outer_flush_range(p_area->dirty_phys_start,
                      p_area->dirty_phys_start + p_area->dirty_size);
#endif
#endif
}

static int mmz_flush_dcache_mmb(const struct mmb_info *pmi)
{
    ot_mmb_t *mmb = NULL;

    if (pmi == NULL) {
        return -EINVAL;
    }

    mmb = pmi->mmb;
    if ((mmb == NULL) || (pmi->map_cached == 0)) {
        osal_trace("%s->%d,error!\n", __func__, __LINE__);
        return -EINVAL;
    }

#ifdef CONFIG_64BIT
    __flush_dcache_area(pmi->mapped, (size_t)pmi->size);
#else
    /* flush l1 cache, use vir addr */
    __cpuc_flush_dcache_area(pmi->mapped, (size_t)pmi->size);

#ifdef CONFIG_CACHE_L2X0
    /* flush l2 cache, use paddr */
    outer_flush_range(mmb->phys_addr, mmb->phys_addr + mmb->length);
#endif
#endif
    return 0;
}
#ifdef CONFIG_64BIT
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
void flush_cache_all(void)
{
}
#endif
#endif

/*
 * this function should never be called with local irq disabled,
 * because on_each_cpu marco will raise ipi interrupt.
 */
int mmz_flush_dcache_all(void)
{
#ifdef CONFIG_64BIT
    on_each_cpu((smp_call_func_t)flush_cache_all, NULL, 1);
#else
    on_each_cpu((smp_call_func_t)__cpuc_flush_kern_all, NULL, 1);
    outer_flush_all();
#endif /* CONFIG_64BIT */
    return 0;
}

static int mmz_userdev_open(struct inode *inode, struct file *file)
{
    struct mmz_userdev_info *pmu = NULL;

    if (!capable(CAP_SYS_RAWIO)) {
        return -EPERM;
    }

    pmu = kmalloc(sizeof(*pmu), GFP_KERNEL);
    if (pmu == NULL) {
        error_mmz("alloc mmz_userdev_info failed!\n");
        return -ENOMEM;
    }
    (void)memset_s(pmu, sizeof(*pmu), 0, sizeof(*pmu));
    pmu->pid = current->pid;
    pmu->mmap_pid = 0;
    sema_init(&pmu->sem, 1);
    OSAL_INIT_LIST_HEAD(&pmu->list);

    /* This file could be opened just for once */
    file->private_data = (void *)pmu;

    return 0;
}

static int ioctl_mmb_alloc(const struct file *file,
                           unsigned int iocmd,
                           struct mmb_info *pmi)
{
    struct mmz_userdev_info *pmu = file->private_data;
    struct mmb_info *new_mmbinfo = NULL;
    ot_mmb_t *mmb = NULL;

    mmb = ot_mmb_alloc(pmi->mmb_name, pmi->size,
                        pmi->align, pmi->gfp, pmi->mmz_name);
    if (mmb == NULL) {
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
        error_mmz("ot_mmb_alloc(%s, %llu, 0x%llx, %lu, %s) failed!\n",
              pmi->mmb_name, pmi->size, pmi->align,
              pmi->gfp, pmi->mmz_name);
#else
        error_mmz("ot_mmb_alloc(%s, %lu, 0x%lx, %lu, %s) failed!\n",
              pmi->mmb_name, pmi->size, pmi->align,
              pmi->gfp, pmi->mmz_name);
#endif
        return -ENOMEM;
    }

    new_mmbinfo = kmalloc(sizeof(*new_mmbinfo), GFP_KERNEL);
    if (new_mmbinfo == NULL) {
        ot_mmb_free(mmb);
        error_mmz("alloc mmb_info failed!\n");
        return -ENOMEM;
    }

    (void)memcpy_s(new_mmbinfo, sizeof(*new_mmbinfo), pmi, sizeof(*new_mmbinfo));
    new_mmbinfo->phys_addr = ot_mmb_phys(mmb);
    new_mmbinfo->mmb = mmb;
    new_mmbinfo->prot = PROT_READ;
    new_mmbinfo->flags = MAP_SHARED;
    osal_list_add_tail(&new_mmbinfo->list, &pmu->list);

    pmi->phys_addr = new_mmbinfo->phys_addr;

    ot_mmb_get(mmb);

    return 0;
}

static int ioctl_mmb_alloc_v2(const struct file *file,
                              unsigned int iocmd, struct mmb_info *pmi)
{
    struct mmz_userdev_info *pmu = file->private_data;
    struct mmb_info *new_mmbinfo = NULL;
    ot_mmb_t *mmb = NULL;

    mmb = ot_mmb_alloc_v2(pmi->mmb_name, pmi->size, pmi->align,
                           pmi->gfp, pmi->mmz_name, pmi->order);
    if (mmb == NULL) {
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
        error_mmz("ot_mmb_alloc(%s, %llu, 0x%llx, %lu, %s) failed!\n",
              pmi->mmb_name, pmi->size, pmi->align,
              pmi->gfp, pmi->mmz_name);
#else
        error_mmz("ot_mmb_alloc(%s, %lu, 0x%lx, %lu, %s) failed!\n",
              pmi->mmb_name, pmi->size, pmi->align,
              pmi->gfp, pmi->mmz_name);
#endif
        return -ENOMEM;
    }

    new_mmbinfo = kmalloc(sizeof(*new_mmbinfo), GFP_KERNEL);
    if (new_mmbinfo == NULL) {
        ot_mmb_free(mmb);
        error_mmz("alloc mmb_info failed!\n");
        return -ENOMEM;
    }

    (void)memcpy_s(new_mmbinfo, sizeof(*new_mmbinfo), pmi, sizeof(*pmi));
    new_mmbinfo->phys_addr = ot_mmb_phys(mmb);
    new_mmbinfo->mmb = mmb;
    new_mmbinfo->prot = PROT_READ;
    new_mmbinfo->flags = MAP_SHARED;
    osal_list_add_tail(&new_mmbinfo->list, &pmu->list);

    pmi->phys_addr = new_mmbinfo->phys_addr;

    ot_mmb_get(mmb);

    return 0;
}

static struct mmb_info *get_mmbinfo(unsigned long addr, const struct mmz_userdev_info *pmu)
{
    struct mmb_info *p = NULL;

    osal_list_for_each_entry(p, &pmu->list, list) {
        if ((addr >= p->phys_addr) && (addr < (p->phys_addr + p->size))) {
            break;
        }
    }
    if (&p->list == &pmu->list) {
        return NULL;
    }

    return p;
}

static struct mmb_info *get_mmbinfo_safe(unsigned long addr, const struct mmz_userdev_info *pmu)
{
    struct mmb_info *p = NULL;

    p = get_mmbinfo(addr, pmu);
    if (p == NULL) {
        error_mmz("mmb(0x%08lX) not found!\n", addr);
        return NULL;
    }

    return p;
}

static int ioctl_mmb_user_unmap(const struct file *file, unsigned int iocmd, struct mmb_info *pmi);

static int _usrdev_mmb_free(struct mmb_info *p)
{
    int ret;

    osal_list_del(&p->list);
    ot_mmb_put(p->mmb);
    ret = ot_mmb_free(p->mmb);
    kfree(p);

    return ret;
}

static int ioctl_mmb_free(const struct file *file,
                          unsigned int iocmd, const struct mmb_info *pmi)
{
    int ret;
    struct mmz_userdev_info *pmu = file->private_data;
    struct mmb_info *p = get_mmbinfo_safe(pmi->phys_addr, pmu);

    if ((p == NULL) || (p->mmb == NULL)) {
        return -EPERM;
    }

    if (p->delayed_free) {
        warning("mmb<%s> is delayed_free, can not free again!\n", p->mmb->name);
        return -EBUSY;
    }

    if ((p->map_ref > 0) || (p->mmb_ref > 0)) {
        warning("mmb<%s> is still in use!\n", p->mmb->name);
        p->delayed_free = 1;
        return -EBUSY;
    }

    ret = _usrdev_mmb_free(p);

    return ret;
}

static int ioctl_mmb_attr(const struct file *file,
                          unsigned int iocmd, struct mmb_info *pmi)
{
    struct mmz_userdev_info *pmu = file->private_data;
    struct mmb_info *p = NULL;

    if ((p = get_mmbinfo_safe(pmi->phys_addr, pmu)) == NULL) {
        return -EPERM;
    }

    (void)memcpy_s(pmi, sizeof(*pmi), p, sizeof(*pmi));
    return 0;
}

static int ioctl_mmb_user_remap(struct file *file, unsigned int iocmd, struct mmb_info *pmi, int cached)
{
    struct mmz_userdev_info *pmu = file->private_data;
    struct mmb_info *p = NULL;
    unsigned long addr, len, prot, flags, pgoff;

    if ((p = get_mmbinfo_safe(pmi->phys_addr, pmu)) == NULL) {
        return -EPERM;
    }
    /*
     * mmb could be remapped for more than once, but should not
     * be remapped with confusing cache type.
     */
    if ((p->mapped != NULL) && (p->mmb != NULL) && (p->map_ref > 0)) {
        if (cached != p->map_cached) {
            error_mmz("mmb<%s> already mapped as %s, cannot remap as %s.\n",
                  p->mmb->name, p->map_cached ? "cached" : "non-cached", cached ? "cached" : "non-cached");
            return -EINVAL;
        }

        p->map_ref++;
        p->mmb_ref++;

        ot_mmb_get(p->mmb);

        /*
         * pmi->phys may not always start at p->phys,
         * and may start with offset from p->phys.
         * so, we need to calculate with the offset.
         */
        pmi->mapped = p->mapped + (pmi->phys_addr - p->phys_addr);

        return 0;
    }

    if (p->phys_addr & ~PAGE_MASK) {
        return -EINVAL;
    }

    addr = 0;
    len = PAGE_ALIGN(p->size);

    prot = pmi->prot;
    flags = pmi->flags;
    if (prot == 0) {
        prot = p->prot;
    }
    if (flags == 0) {
        flags = p->flags;
    }

    pmu->mmap_pid = current->pid;
    p->map_cached = cached;

    pgoff = p->phys_addr;
    addr = vm_mmap(file, addr, len, prot, flags, pgoff);

    pmu->mmap_pid = 0;

    if (IS_ERR_VALUE((uintptr_t)addr)) {
        error_mmz("vm_mmap(file, 0, %lu, 0x%08lX, 0x%08lX, 0x%08lX) return 0x%08lX\n",
              len, prot, flags, pgoff, addr);
        return addr;
    }

    p->mapped = (void *)(uintptr_t)addr;
    p->prot = prot;
    p->flags = flags;

    p->map_ref++;
    p->mmb_ref++;
    ot_mmb_get(p->mmb);

    /*
     * pmi->phys may not always start at p->phys,
     * and may start with offset from p->phys.
     * so, we need to calculate with the offset.
     */
    pmi->mapped = p->mapped + (pmi->phys_addr - p->phys_addr);

    return 0;
}

static int ioctl_mmb_user_unmap_with_cache(const struct mmb_info *p, unsigned long addr, unsigned long len)
{
    struct mm_struct *mm = current->mm;

    mmz_map_down(mm);
    if (ot_vma_check(addr, addr + len)) {
        error_mmz("mmb<%s> vma is invalid.\n", p->mmb->name);
        mmz_map_up(mm);
        return -EPERM;
    }
#ifdef CONFIG_64BIT
    __flush_dcache_area((void *)(uintptr_t)addr, (size_t)len);
#else
    __cpuc_flush_dcache_area((void *)(uintptr_t)addr, (size_t)len);
#ifdef CONFIG_CACHE_L2X0
    outer_flush_range(p->phys_addr, p->phys_addr + len);
#endif
#endif /* CONFIG_64BIT */
    mmz_map_up(mm);

    return 0;
}

static int ioctl_mmb_user_unmap(const struct file *file, unsigned int iocmd, struct mmb_info *pmi)
{
    int ret;
    unsigned long addr, len;
    struct mmb_info *p = NULL;
    struct mmz_userdev_info *pmu = file->private_data;

    p = get_mmbinfo_safe(pmi->phys_addr, pmu);
    if (p == NULL) {
        return -EPERM;
    }

    if (p->mapped == NULL) {
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
        warning("mmb(0x%llx) isn't user-mapped!\n", p->phys_addr);
#else
        warning("mmb(0x%lx) isn't user-mapped!\n", p->phys_addr);
#endif
        pmi->mapped = NULL;
        return -EIO;
    }

    if (!((p->map_ref > 0) && (p->mmb_ref > 0))) {
        error_mmz("mmb<%s> has invalid refer: map_ref=%d, mmb_ref=%d.\n",
              p->mmb->name, p->map_ref, p->mmb_ref);
        return -EIO;
    }

    p->map_ref--;
    p->mmb_ref--;
    ot_mmb_put(p->mmb);

    if (p->map_ref > 0) {
        return 0;
    }

    addr = (unsigned long)(uintptr_t)p->mapped;
    len = PAGE_ALIGN(p->size);

    /* todo,before unmap,refresh cache manually */
    if (p->map_cached) {
        ret = ioctl_mmb_user_unmap_with_cache(p, addr, len);
        if (ret != 0) {
            return ret;
        }
    }

    ret = vm_munmap(addr, len);
    if (!IS_ERR_VALUE((uintptr_t)ret)) {
        p->mapped = NULL;
        pmi->mapped = NULL;
    }

    if (p->delayed_free && (p->map_ref == 0) && (p->mmb_ref == 0)) {
        _usrdev_mmb_free(p);
    }

    return ret;
}

static int ioctl_mmb_virt2phys(const struct file *file,
                               unsigned int iocmd, struct mmb_info *pmi)
{
    int ret = 0;
    unsigned long virt, phys;
    unsigned long offset = 0;

    virt = (unsigned long)(uintptr_t)pmi->mapped;
    phys = usr_virt_to_phys(virt);
    if (!phys) {
        ret = -ENOMEM;
    }

    if (ot_mmb_getby_phys_2(phys, &offset) == NULL) {
        error_mmz("Not mmz alloc memory[0x%lx 0x%lx]! 0x%lx\n", virt, phys, offset);
        return -EINVAL;
    }

    pmi->phys_addr = phys;

    return ret;
}

static int mmz_userdev_ioctl_m(struct file *file, unsigned int cmd, struct mmb_info *pmi)
{
    int ret;

    switch (_IOC_NR(cmd)) {
        case _IOC_NR(IOC_MMB_ALLOC):
            ret = ioctl_mmb_alloc(file, cmd, pmi);
            break;
        case _IOC_NR(IOC_MMB_ALLOC_V2):
            ret = ioctl_mmb_alloc_v2(file, cmd, pmi);
            break;
        case _IOC_NR(IOC_MMB_ATTR):
            ret = ioctl_mmb_attr(file, cmd, pmi);
            break;
        case _IOC_NR(IOC_MMB_FREE):
            ret = ioctl_mmb_free(file, cmd, pmi);
            break;

        case _IOC_NR(IOC_MMB_USER_REMAP):
            ret = ioctl_mmb_user_remap(file, cmd, pmi, 0);
            break;
        case _IOC_NR(IOC_MMB_USER_REMAP_CACHED):
            ret = ioctl_mmb_user_remap(file, cmd, pmi, 1);
            break;
        case _IOC_NR(IOC_MMB_USER_UNMAP):
            ret = ioctl_mmb_user_unmap(file, cmd, pmi);
            break;
        case _IOC_NR(IOC_MMB_VIRT_GET_PHYS):
            ret = ioctl_mmb_virt2phys(file, cmd, pmi);
            break;
        default:
            error_mmz("invalid ioctl cmd = %08X\n", cmd);
            ret = -EINVAL;
            break;
    }

    return ret;
}

static int mmz_userdev_ioctl_r(unsigned int cmd, struct mmb_info *pmi)
{
    switch (_IOC_NR(cmd)) {
        case _IOC_NR(IOC_MMB_ADD_REF):
            pmi->mmb_ref++;
            ot_mmb_get(pmi->mmb);
            break;
        case _IOC_NR(IOC_MMB_DEC_REF):
            if (pmi->mmb_ref <= 0) {
                error_mmz("mmb<%s> mmb_ref is %d!\n", pmi->mmb->name, pmi->mmb_ref);
                return -EPERM;
            }
            pmi->mmb_ref--;
            ot_mmb_put(pmi->mmb);
            if (pmi->delayed_free && (pmi->mmb_ref == 0) && (pmi->map_ref == 0)) {
                _usrdev_mmb_free(pmi);
            }
            break;
        default:
            return -EINVAL;
            break;
    }

    return 0;
}

/* just for test */
static int mmz_userdev_ioctl_t(const struct file *file, unsigned int cmd, const struct mmb_info *pmi);

static int mmz_userdev_ioctl_of_m(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mmb_info mi = { 0 };
    int ret;

    if ((_IOC_SIZE(cmd) > sizeof(mi)) || (arg == 0)) {
        error_mmz("_IOC_SIZE(cmd)=%d, arg==0x%08lX\n", _IOC_SIZE(cmd), arg);
        return -EINVAL;
    }
    (void)memset_s(&mi, sizeof(mi), 0, sizeof(mi));
    if (copy_from_user(&mi, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
        osal_trace("\nmmz_userdev_ioctl: copy_from_user error.\n");
        return -EFAULT;
    }
    mi.mmz_name[OT_MMZ_NAME_LEN - 1] = '\0';
    mi.mmb_name[OT_MMB_NAME_LEN - 1] = '\0';
    ret = mmz_userdev_ioctl_m(file, cmd, &mi);
    if (!ret && (cmd & IOC_OUT)) {
        if (copy_to_user((void *)(uintptr_t)arg, &mi, _IOC_SIZE(cmd))) {
            osal_trace("\nmmz_userdev_ioctl: copy_to_user error.\n");
            return -EFAULT;
        }
    }
    return ret;
}

static int mmz_userdev_ioctl_of_r(struct file *file, unsigned int cmd, unsigned long arg,
    struct mmz_userdev_info *pmu)
{
    struct mmb_info *pmi = NULL;

    pmi = get_mmbinfo_safe(arg, pmu);
    if (pmi == NULL) {
        return -EPERM;
    }

    return mmz_userdev_ioctl_r(cmd, pmi);
}

static int mmz_userdev_ioctl_of_c(unsigned int cmd, unsigned long arg, const struct mmz_userdev_info *pmu)
{
    struct mmb_info *pmi = NULL;
    int ret = 0;

    if (arg == 0) {
        mmz_flush_dcache_all();
        return ret;
    }

    pmi = get_mmbinfo_safe(arg, pmu);
    if (pmi == NULL) {
        return -EPERM;
    }

    switch (_IOC_NR(cmd)) {
        case _IOC_NR(IOC_MMB_FLUSH_DCACHE):
            mmz_flush_dcache_mmb(pmi);
            break;
        default:
            ret = -EINVAL;
            break;
    }
    return ret;
}

static int mmz_userdev_check_dirty_area_range(const struct dirty_area *area,
    const struct mmz_userdev_info *pmu)
{
    ot_mmb_t *mmb = NULL;
    struct mmb_info *pmi = NULL;
    unsigned long offset;

    mmb = ot_mmb_getby_phys_2(area->dirty_phys_start, &offset);
    if (mmb == NULL) {
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
        error_mmz("dirty_phys_addr=0x%llx\n", area->dirty_phys_start);
#else
        error_mmz("dirty_phys_addr=0x%lx\n", area->dirty_phys_start);
#endif
        return -EFAULT;
    }

    pmi = get_mmbinfo_safe(mmb->phys_addr, pmu);
    if (pmi == NULL) {
        return -EPERM;
    }
    if ((uintptr_t)area->dirty_virt_start != (uintptr_t)pmi->mapped + offset) {
        osal_trace(KERN_WARNING "dirty_virt_start addr was not consistent with dirty_phys_start addr!\n");
        return -EFAULT;
    }
    if (area->dirty_phys_start + area->dirty_size > mmb->phys_addr + mmb->length) {
        osal_trace(KERN_WARNING "\ndirty area overflow!\n");
        return -EFAULT;
    }

    return 0;
}

static int mmz_userdev_ioctl_of_d(unsigned int cmd, unsigned long arg, const struct mmz_userdev_info *pmu)
{
    struct dirty_area area;
    __phys_addr_type__ orig_addr;
    unsigned long virt_addr;
    struct mm_struct *mm = current->mm;
    int ret;

    if ((_IOC_SIZE(cmd) != sizeof(area)) || (arg == 0)) {
        error_mmz("_IOC_SIZE(cmd)=%d, arg==0x%08lx\n", _IOC_SIZE(cmd), arg);
        return -EINVAL;
    }
    (void)memset_s(&area, sizeof(area), 0, sizeof(area));
    if (copy_from_user(&area, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
        osal_trace(KERN_WARNING "\nmmz_userdev_ioctl: copy_from_user error.\n");
        return -EFAULT;
    }

    ret = mmz_userdev_check_dirty_area_range(&area, pmu);
    if (ret != 0) {
        return ret;
    }

    mmz_map_down(mm);

    if (ot_vma_check((uintptr_t)area.dirty_virt_start, (uintptr_t)area.dirty_virt_start + area.dirty_size)) {
        osal_trace(KERN_WARNING "\ndirty area[0x%lx,0x%lx] overflow!\n",
               (unsigned long)(uintptr_t)area.dirty_virt_start,
               (unsigned long)(uintptr_t)area.dirty_virt_start + area.dirty_size);
        mmz_map_up(mm);
        return -EFAULT;
    }

    /* cache line aligned */
    orig_addr = area.dirty_phys_start;
    area.dirty_phys_start &= ~(CACHE_LINE_SIZE - 1);
    virt_addr = (unsigned long)(uintptr_t)area.dirty_virt_start;
    virt_addr &= ~(CACHE_LINE_SIZE - 1);
    area.dirty_virt_start = (void *)(uintptr_t)virt_addr;
    area.dirty_size = (area.dirty_size + (orig_addr - area.dirty_phys_start) +
                      (CACHE_LINE_SIZE - 1)) & ~(CACHE_LINE_SIZE - 1);

    mmz_flush_dcache_mmb_dirty(&area);
    mmz_map_up(mm);
    return 0;
}

static int mmz_userdev_ioctl_of_t(const struct file *file, unsigned int cmd, unsigned long arg,
    const struct mmz_userdev_info *pmu)
{
    struct mmb_info mi;

    if ((_IOC_SIZE(cmd) != sizeof(mi)) || (arg == 0)) {
        error_mmz("_IOC_SIZE(cmd)=%d, arg==0x%08lx\n", _IOC_SIZE(cmd), arg);
        return -EINVAL;
    }

    (void)memset_s(&mi, sizeof(mi), 0, sizeof(mi));
    if (copy_from_user(&mi, (void *)(uintptr_t)arg, sizeof(mi))) {
        osal_trace("\nmmz_userdev_ioctl: copy_from_user error.\n");
        return -EFAULT;
    }

    if ((get_mmbinfo_safe(mi.phys_addr, pmu)) == NULL) {
        return -EPERM;
    }
    return mmz_userdev_ioctl_t(file, cmd, &mi);
}

static long mmz_userdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret;
    struct mmz_userdev_info *pmu = file->private_data;

    down(&pmu->sem);

    if (_IOC_TYPE(cmd) == 'm') {
        ret = mmz_userdev_ioctl_of_m(file, cmd, arg);
    } else if (_IOC_TYPE(cmd) == 'r') {
        ret = mmz_userdev_ioctl_of_r(file, cmd, arg, pmu);
    } else if (_IOC_TYPE(cmd) == 'c') {
        ret = mmz_userdev_ioctl_of_c(cmd, arg, pmu);
    } else if (_IOC_TYPE(cmd) == 'd') {
        ret = mmz_userdev_ioctl_of_d(cmd, arg, pmu);
    } else if (_IOC_TYPE(cmd) == 't') {
        ret = mmz_userdev_ioctl_of_t(file, cmd, arg, pmu);
    } else {
        ret = -EINVAL;
    }

    up(&pmu->sem);
    return ret;
}

#ifdef CONFIG_COMPAT
static long compat_mmz_userdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    return mmz_userdev_ioctl(file, cmd, (unsigned long)(uintptr_t)compat_ptr(arg));
}
#endif

static void mmz_userdev_mmap_with_file_flag(const struct file *file, struct vm_area_struct *vma,
    int mmb_cached)
{
    if (file->f_flags & O_SYNC) {
#ifdef CONFIG_64BIT
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot)
                                     | PTE_WRITE | PTE_DIRTY);
#endif
        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
    } else {
#ifdef CONFIG_64BIT
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot)
                                     | PTE_WRITE | PTE_DIRTY);
#else
        vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot)
                                     | L_PTE_PRESENT | L_PTE_YOUNG
                                     | L_PTE_DIRTY | L_PTE_MT_DEV_CACHED);
#endif
        if (mmb_cached == 0) {
            /*
             * pagetable property changes from <normal nocache> to
             * <strong order>, for enhance ddr access performance,
             */
            /*  <normal nocache> */
            vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
        }
    }
}

static void mmz_userdev_mmap_page_valid(struct vm_area_struct *vma)
{
    unsigned long start = vma->vm_start;
    unsigned long pfn = vma->vm_pgoff;
    size_t size = vma->vm_end - vma->vm_start;

    while (size) {
        if (pfn_valid(pfn)) {
            if (vm_insert_page(vma, start, pfn_to_page(pfn))) {
                error_mmz("insert page failed.\n");
                break;
            }
        } else {
#ifdef CONFIG_64BIT
            error_mmz("vm map failed for phy address(0x%llx)\n", __pfn_to_phys(pfn));
#else
            error_mmz("vm map failed for phy address(0x%x)\n", __pfn_to_phys(pfn));
#endif
        }

        start += PAGE_SIZE;
        size = (size < PAGE_SIZE) ? 0 : (size - PAGE_SIZE);
        pfn++;
    }
}

static int mmz_userdev_mmap_page_invalid(struct vm_area_struct *vma)
{
    size_t size = vma->vm_end - vma->vm_start;
    if (size == 0) {
        return -EPERM;
    }
    /*
     * Remap-pfn-range will mark the range
     * as VM_IO and VM_RESERVED
     */
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start,
                        vma->vm_page_prot)) {
        return -EAGAIN;
    }
    return 0;
}

int mmz_userdev_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct mmb_info *p = NULL;
    struct mmz_userdev_info *pmu = NULL;
    unsigned long offset;
    ot_mmb_t *mmb = NULL;
    int mmb_cached = 0;
    int ret;

    if ((file == NULL) || (file->private_data == NULL) || (vma == NULL) || (vma->vm_mm == NULL)) {
        error_mmz("%s - parameter invalid!\n", __FUNCTION__);
        return -1;
    }

    pmu = file->private_data;
    offset = vma->vm_pgoff << PAGE_SHIFT;

    p = get_mmbinfo(offset, pmu);
    if (p == NULL) {
        unsigned long mmb_offset;
        mmb = ot_mmb_getby_phys_2(offset, &mmb_offset);
        if (mmb == NULL) {
            /* Allow mmap MMZ allocated by other core. */
            if (ot_map_mmz_check_phys(offset, vma->vm_end - vma->vm_start)) {
                return -EPERM;
            }
        } else {
            mmb_cached = mmb->flags & OT_MMB_MAP2KERN_CACHED;
        }
    } else {
        if (p->mapped != NULL) {
            error_mmz("mmb(0x%08lX) have been mapped already?!\n", offset);
            return -EIO;
        }
        mmb_cached = p->map_cached;
    }

    mmz_userdev_mmap_with_file_flag(file, vma, mmb_cached);

    if (pfn_valid(vma->vm_pgoff)) {
        mmz_userdev_mmap_page_valid(vma);
    } else {
        ret = mmz_userdev_mmap_page_invalid(vma);
        if (ret != 0) {
            return ret;
        }
    }

#ifdef CONFIG_OT_SYS_SMMU_SUPPORT
    /* flush page table cache to share the data with CBBs that have sMMU */
    svm_flush_cache(vma->vm_mm, vma->vm_start, vma->vm_end - vma->vm_start);
#endif

    return 0;
}

static int mmz_userdev_release(struct inode *inode, struct file *file)
{
    struct mmz_userdev_info *pmu = file->private_data;
    struct mmb_info *p = NULL;
    struct mmb_info *n = NULL;

    list_for_each_entry_safe(p, n, &pmu->list, list) {
        error_mmz("MMB LEAK(pid=%d): 0x%lX, %lu bytes, '%s'\n",
              pmu->pid, ot_mmb_phys(p->mmb),
              ot_mmb_length(p->mmb),
              ot_mmb_name(p->mmb));

        /*
         * we do not need to release mapped-area here,
         * system will do it for us
         */
        if (p->mapped != NULL) {
#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
            warning("mmb<0x%llx> mapped to userspace 0x%pK will be unmaped!\n",
                    p->phys_addr, p->mapped);
#else
            warning("mmb<0x%lx> mapped to userspace 0x%pK will be unmaped!\n",
                    p->phys_addr, p->mapped);
#endif
        }
        for (; p->mmb_ref > 0; p->mmb_ref--) {
            ot_mmb_put(p->mmb);
        }
        _usrdev_mmb_free(p);
    }

    file->private_data = NULL;
    kfree(pmu);
    pmu = NULL;
    return 0;
}

static struct file_operations g_mmz_userdev_fops = {
    .owner = THIS_MODULE,
    .open = mmz_userdev_open,
    .release = mmz_userdev_release,
#ifdef CONFIG_COMPAT
    .compat_ioctl = compat_mmz_userdev_ioctl,
#endif
    .unlocked_ioctl = mmz_userdev_ioctl,
    .mmap = mmz_userdev_mmap,
};

static struct miscdevice g_mmz_userdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &g_mmz_userdev_fops,
    .name = "mmz_userdev"
};

int __init mmz_userdev_init(void)
{
    int ret;
    ret = misc_register(&g_mmz_userdev);
    if (ret) {
        osal_trace("register mmz dev failure!\n");
        return -1;
    }

    return 0;
}

void __exit mmz_userdev_exit(void)
{
    misc_deregister(&g_mmz_userdev);
}

/* Test func */
static int mmz_userdev_ioctl_t(const struct file *file, unsigned int cmd, const struct mmb_info *pmi)
{
    return 0;
}

