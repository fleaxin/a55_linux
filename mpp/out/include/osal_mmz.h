/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef _OSAL_MMZ_H
#define _OSAL_MMZ_H

#include "ot_osal.h"

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE            0x40
#endif

#define OT_MMZ_NAME_LEN           32
#define OT_MMB_NAME_LEN           32

#define MMZ_REGION_FREE        0
#define MMZ_REGION_BUSY       -1
#define MMZ_REGION_NOT_ENOUGH -2
#define MMZ_REGION_OUT_RANGE  -3


/*xb: ot_media_memory_zone描述了一个mmz区域的所有信息，可以有多个mmz区域，通过链表连接在一起。*/
struct ot_media_memory_zone {
    char name[OT_MMZ_NAME_LEN];

    unsigned long gfp;              //xb:区域标识

    unsigned long phys_start;       //xb:mmz区域起始物理地址
    unsigned long nbytes;           //xb:mmz区域大小

    struct osal_list_head list;     //xb:mmz链表
    union {
        struct device *cma_dev;
        unsigned char *bitmap;      //xb:位图
    };
    struct osal_list_head mmb_list; //xb:mmz区域的mmb链表，存放所有申请到的物理内存

    unsigned int alloc_type;
    unsigned long block_align;

    void (*destructor)(const void *);
};
typedef struct ot_media_memory_zone ot_mmz_t;

#define OT_MMZ_FMT_S              "PHYS(0x%08lX, 0x%08lX), GFP=%lu, nBYTES=%luKB,    NAME=\"%s\""
#define ot_mmz_fmt_arg(p) (p)->phys_start, (p)->phys_start + (p)->nbytes - 1, (p)->gfp, (p)->nbytes / SZ_1K, (p)->name

/* ot_media_memory_block描述了从mmz区域申请一块内存，同一个mmz区域内的所有mmb通过链表连接。 */
struct ot_media_memory_block {
#ifndef MMZ_V2_SUPPORT
    unsigned int id;
#endif
    char name[OT_MMB_NAME_LEN];         //xb:该mmb模块使用者名字
    struct ot_media_memory_zone *zone;  //xb:指向mmb所属的mmz区域
    struct osal_list_head list;         //xb:mmb链表

    unsigned long phys_addr;            //xb:申请到的mmb起始物理地址
    void *kvirt;                        //xb:对应内核虚拟地址
    unsigned long length;               //申请的mmb大小

    unsigned long flags;                //标识

    unsigned int order;

    int phy_ref;                        //引用计数
    int map_ref;                        //引用计数
};
typedef struct ot_media_memory_block ot_mmb_t;

#define ot_mmb_kvirt(p) ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb==NULL); __mmb->kvirt; })
#define ot_mmb_phys(p) ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb==NULL); __mmb->phys_addr; })
#define ot_mmb_length(p) ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb==NULL); __mmb->length; })
#define ot_mmb_name(p) ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb==NULL); __mmb->name; })
#define ot_mmb_zone(p) ({ot_mmb_t *__mmb=(p); OSAL_BUG_ON(__mmb==NULL); __mmb->zone; })

#define OT_MMB_MAP2KERN           (1 << 0)
#define OT_MMB_MAP2KERN_CACHED    (1 << 1)
#define OT_MMB_RELEASED           (1 << 2)

#define OT_MMB_FMT_S              "phys(0x%08lX, 0x%08lX), kvirt=0x%08lX, flags=0x%08lX, length=%luKB,    name=\"%s\""
#define ot_mmb_fmt_arg(p) (p)->phys_addr, mmz_grain_align((p)->phys_addr + (p)->length) - 1, \
    (unsigned long)(uintptr_t)((p)->kvirt), (p)->flags, (p)->length / SZ_1K, (p)->name

#define DEFAULT_ALLOC              0
#define SLAB_ALLOC                 1
#define EQ_BLOCK_ALLOC             2

#define LOW_TO_HIGH                0
#define HIGH_TO_LOW                1

#define MMZ_DBG_LEVEL              0x0
#define MMZ_INFO_LEVEL             0x1

#define mmz_trace(level, s, params...)                                             \
    do {                                                                           \
        if (level & MMZ_DBG_LEVEL)                                                 \
            printk(KERN_INFO "[%s, %d]: " s "\n", __FUNCTION__, __LINE__, params); \
    } while (0)

#define MMZ_GRAIN                  PAGE_SIZE
#define mmz_bitmap_size(p) (mmz_align2(mmz_length2grain((p)->nbytes), 8) / 8)

#define mmz_get_bit(p, n) (((p)->bitmap[(n) / 8] >> ((n) & 0x7)) & 0x1)
#define mmz_set_bit(p, n) ((p)->bitmap[(n) / 8] |= 1 << ((n) & 0x7))
#define mmz_clr_bit(p, n) ((p)->bitmap[(n) / 8] &= ~(1 << ((n) & 0x7)))

#define mmz_pos2phy_addr(p, n) ((p)->phys_start + (n) * MMZ_GRAIN)
#define mmz_phy_addr2pos(p, a) (((a) - (p)->phys_start) / MMZ_GRAIN)

#define mmz_align2low(x, g) (((g) == 0) ? (0) : (((x) / (g)) * (g)))
#define mmz_align2(x, g) (((g) == 0) ? (0) : ((((x) + (g) - 1) / (g)) * (g)))
#define mmz_grain_align(x) mmz_align2(x, MMZ_GRAIN)
#define mmz_length2grain(len) (mmz_grain_align(len) / MMZ_GRAIN)

#define begin_list_for_each_mmz(p, gfp, mmz_name)        \
    list_for_each_entry(p, ot_mmz_get_mmz_list(), list)   \
    {                                                    \
        if ((gfp) == 0 ? 0 : (p)->gfp != (gfp)) {        \
            continue;                                       \
        }                                                   \
        if (((mmz_name) == NULL) || (*(mmz_name) == '\0')) { \
            if (ot_mmz_get_anony() == 1) {                \
                if (strcmp("anonymous", (p)->name)) {    \
                    continue;                            \
                }                                        \
            } else {                                     \
                break;                                   \
            }                                            \
        } else {                                         \
            if (strcmp(mmz_name, (p)->name)) {           \
                continue;                                \
            }                                            \
        }                                                \
        mmz_trace(1, OT_MMZ_FMT_S, ot_mmz_fmt_arg(p));
#define end_list_for_each_mmz() }

#if defined(KERNEL_BIT_64) && defined(USER_BIT_32)
#define __phys_addr_type__         unsigned long long
#define __phys_len_type__          unsigned long long
#define __phys_addr_align__        __attribute__((aligned(8)))
#else
#define __phys_addr_type__         unsigned long
#define __phys_len_type__          unsigned long
#define __phys_addr_align__        __attribute__((aligned(sizeof(long))))
#endif

struct mmb_info {
    __phys_addr_type__ phys_addr; /* phys-memory address */
    __phys_addr_type__ __phys_addr_align__ align; /* if you need your phys-memory have special align size */
    __phys_len_type__ __phys_addr_align__ size; /* length of memory you need, in bytes */
    unsigned int __phys_addr_align__ order;

    void *__phys_addr_align__ mapped; /* userspace mapped ptr */

    union {
        struct {
            unsigned long prot : 8; /* PROT_READ or PROT_WRITE */
            unsigned long flags : 12; /* MAP_SHARED or MAP_PRIVATE */

#ifdef __KERNEL__
            unsigned long reserved : 8; /* reserved, do not use */
            unsigned long delayed_free : 1;
            unsigned long map_cached : 1;
#endif
        };
        unsigned long w32_stuf;
    } __phys_addr_align__;

    char mmb_name[OT_MMB_NAME_LEN];
    char mmz_name[OT_MMZ_NAME_LEN];
    unsigned long __phys_addr_align__ gfp; /* reserved, do set to 0 */

#ifdef __KERNEL__
    int map_ref;
    int mmb_ref;

    struct osal_list_head list;
    ot_mmb_t *mmb;
#endif
} __attribute__((aligned(8)));

struct dirty_area {
    __phys_addr_type__ dirty_phys_start; /* dirty physical address */
    void *__phys_addr_align__ dirty_virt_start; /* dirty virtual  address,
                       must be coherent with dirty_phys_addr */
    __phys_len_type__ __phys_addr_align__ dirty_size;
} __phys_addr_align__;

#define IOC_MMB_ALLOC              _IOWR('m', 10, struct mmb_info)
#define IOC_MMB_ATTR               _IOR('m', 11, struct mmb_info)
#define IOC_MMB_FREE               _IOW('m', 12, struct mmb_info)
#define IOC_MMB_ALLOC_V2           _IOWR('m', 13, struct mmb_info)

#define IOC_MMB_USER_REMAP         _IOWR('m', 20, struct mmb_info)
#define IOC_MMB_USER_REMAP_CACHED  _IOWR('m', 21, struct mmb_info)
#define IOC_MMB_USER_UNMAP         _IOWR('m', 22, struct mmb_info)

#define IOC_MMB_VIRT_GET_PHYS      _IOWR('m', 23, struct mmb_info)

#define IOC_MMB_ADD_REF            _IO('r', 30) /* ioctl(file, cmd, arg), arg is mmb_addr */
#define IOC_MMB_DEC_REF            _IO('r', 31) /* ioctl(file, cmd, arg), arg is mmb_addr */

#define IOC_MMB_FLUSH_DCACHE       _IO('c', 40)

#define IOC_MMB_FLUSH_DCACHE_DIRTY _IOW('d', 50, struct dirty_area)
#define IOC_MMB_TEST_CACHE         _IOW('t', 11, struct mmb_info)

#define MMZ_SETUP_CMDLINE_LEN      256
typedef struct {
    char mmz[MMZ_SETUP_CMDLINE_LEN];
    char map_mmz[MMZ_SETUP_CMDLINE_LEN];
    int anony;
} mmz_module_params_s;

typedef struct {
    unsigned long gfp;
    unsigned long phys_start;
    unsigned long nbytes;
    unsigned long block_align;
    unsigned int alloc_type;
} ot_mmz_create_attr;

typedef struct {
    const char *mmz_name;
    const char *buf_name;
    unsigned long start;
    unsigned long size;
    unsigned long align;
    unsigned long gfp;
    unsigned int order;
} ot_mmz_alloc_para_in;

typedef struct {
    int erro_num;
} ot_mmz_alloc_para_out;


/*
 * APIs
 */
ot_mmz_t *ot_mmz_create(const char *name, unsigned long gfp, unsigned long phys_start, unsigned long nbytes);
ot_mmz_t *ot_mmz_create_v2(const char *name, const ot_mmz_create_attr *mmz_create_attr);

int ot_mmz_destroy(const ot_mmz_t *zone);

int ot_mmz_register(ot_mmz_t *zone);
int ot_mmz_unregister(ot_mmz_t *zone);
ot_mmz_t *ot_mmz_find(unsigned long gfp, const char *mmz_name);

ot_mmb_t *ot_mmb_alloc(const char *name, unsigned long size, unsigned long align,
    unsigned long gfp, const char *mmz_name);
ot_mmb_t *ot_mmb_alloc_v2(const char *name, unsigned long size, unsigned long align,
    unsigned long gfp, const char *mmz_name, unsigned int order);
ot_mmb_t *ot_mmb_alloc_v3(const ot_mmz_alloc_para_in *para_in, ot_mmz_alloc_para_out *para_out);
int ot_mmb_free(ot_mmb_t *mmb);
ot_mmb_t *ot_mmb_getby_phys(unsigned long addr);
ot_mmb_t *ot_mmb_getby_phys_2(unsigned long addr, unsigned long *out_offset);
ot_mmb_t *ot_mmb_getby_kvirt(const void *virt);
unsigned long usr_virt_to_phys(unsigned long virt);

int ot_map_mmz_check_phys(unsigned long addr_start, unsigned long addr_len);

int ot_is_phys_in_mmz(unsigned long addr_start, unsigned long addr_len);

int ot_vma_check(unsigned long vm_start, unsigned long vm_end);
int ot_mmb_flush_dcache_byaddr_safe(void *kvirt, unsigned long phys_addr, unsigned long length);

unsigned long ot_mmz_get_phys(const char *zone_name);

ot_mmb_t *ot_mmb_alloc_in(const char *name, unsigned long size, unsigned long align,
    const ot_mmz_t *_user_mmz);
ot_mmb_t *ot_mmb_alloc_in_v2(const char *name, unsigned long size, unsigned long align,
    const ot_mmz_t *_user_mmz, unsigned int order);

#define ot_mmb_freeby_phys(phys_addr) ot_mmb_free(ot_mmb_getby_phys(phys_addr))
#define ot_mmb_freeby_kvirt(kvirt) ot_mmb_free(ot_mmb_getby_kvirt(kvirt))

void *ot_mmb_map2kern(ot_mmb_t *mmb);
void *ot_mmb_map2kern_cached(ot_mmb_t *mmb);

int ot_mmb_flush_dcache_byaddr(void *kvirt, unsigned long phys_addr, unsigned long length);
int ot_mmb_invalid_cache_byaddr(void *kvirt, unsigned long phys_addr, unsigned long length);

int ot_mmb_unmap(ot_mmb_t *mmb);
int ot_mmb_get(ot_mmb_t *mmb);
int ot_mmb_put(ot_mmb_t *mmb);

void *ot_mmf_map2kern_nocache(unsigned long phys, int len);
void *ot_mmf_map2kern_cache(unsigned long phys, int len);
void ot_mmf_unmap(void *virt);
int ot_mmz_get_anony(void);
struct osal_list_head *ot_mmz_get_mmz_list(void);

/* for mmz userdev */
int mmz_userdev_init(void);
void mmz_userdev_exit(void);
int mmz_flush_dcache_all(void);

#endif
