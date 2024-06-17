/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __REGION_EXT_H__
#define __REGION_EXT_H__

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_region.h"
#include "mod_ext.h"

typedef td_void (*rgn_detach_chn)(td_s32 dev_id, td_s32 chn_id, td_bool detach_chn);

/* the max pixel format of region support */
#define OT_PIXEL_FORMAT_NUM_MAX    5

typedef td_bool (*rgn_check_chn_capacity)(ot_rgn_type type, const ot_mpp_chn *chn);

typedef td_bool (*rgn_check_chn_attr)(ot_rgn_type type, const ot_mpp_chn *chn, const ot_rgn_chn_attr *rgn_chn);

typedef struct {
    ot_pixel_format        pixel_format;    /* bitmap pixel format now support clut2 and clut4 */
    td_u32                 bg_color;    /* background color, pixel format depends on "pixel_format" */
    td_u32                 canvas_num;
    td_u32                 clut[OT_RGN_CLUT_NUM];      /* color look up table */
} ot_rgn_overlay_common_attr;

typedef struct {
    td_u32                         fg_alpha;
    td_u32                         bg_alpha;
    td_u32                         global_alpha;
    ot_rgn_overlay_qp_info         qp_info;
    ot_rgn_attach_dst              dst;
} ot_rgn_overlay_common_chn_attr;

typedef struct {
    ot_quad_cover                  quad;
    ot_cover_type                  type;
    ot_coord                       coord;
    td_u32                         color;
} ot_rgn_cover_common_chn_attr;

typedef struct {
    ot_mosaic_blk_size             blk_size;          /* block size of MOSAIC */
} ot_rgn_mosaic_common_chn_attr;

#ifdef CONFIG_OT_RGN_CORNER_RECT_SUPPORT
typedef struct {
    td_u32                         hor_len;
    td_u32                         ver_len;
    td_u32                         thick;
    ot_corner_rect_type            corner_rect_type;
    td_u32                         color;
} ot_rgn_corner_rect_common_chn_attr;
#endif

typedef struct {
    td_u32                         thick;
    td_u32                         color;                           /* color of line */
    ot_point                       points[OT_RGN_LINE_POINT_NUM];   /* startpoint and endpoint of line */
} ot_rgn_line_common_chn_attr;

typedef union {
    ot_rgn_overlay_common_chn_attr      overlay;
    ot_rgn_cover_common_chn_attr        cover;
    ot_rgn_line_common_chn_attr         line;
    ot_rgn_mosaic_common_chn_attr       mosaic;
#ifdef CONFIG_OT_RGN_CORNER_RECT_SUPPORT
    ot_rgn_corner_rect_common_chn_attr  corner_rect;
#endif
} ot_rgn_common_chn_attr;

typedef struct {
    td_bool                         is_show;
    ot_rgn_common_chn_attr          chn_attr;
    ot_rgn_overlay_common_attr      overlay_attr;
    ot_size                         size;
    ot_point                        point;
    td_u32                          layer;
    td_u32                          stride;
    td_phys_addr_t                  phys_addr;             /* physical address of region memory */
    td_phys_addr_t                  virt_addr;            /* virtual address of region memory */
} rgn_common;

typedef struct {
    td_u32 buff_index; /* index of buffer */
    td_u32 handle;      /* handle of buffer */
} rgn_buf_info;

typedef struct {
    rgn_buf_info buf_info;
    rgn_common *common; /* address of common information of point array */
} rgn_get_info;

typedef struct {
    td_u32 num;                  /* number of region */
    td_bool modify;                /* modify of not */
    rgn_get_info *get_info; /* address of common information of point array */
} rgn_info;

typedef struct {
    td_u32 num; /* number of region */
    rgn_buf_info *buf_info;
} rgn_put_info;

typedef struct {
    ot_mod_id mod_id;
    td_u32 dev_min; /* if no dev id, should set it 0 */
    td_u32 chn_min;
    td_u32 dev_max; /* if no dev id, should set it 1 */
    td_u32 chn_max;
    rgn_detach_chn detach_chn;
} rgn_register_info;

typedef struct {
    td_bool spt_layer;
    td_bool spt_reset;

    /* region layer [min,max] */
    td_u32 layer_min;
    td_u32 layer_max;
} rgn_capacity_layer;

typedef struct {
    td_bool spt_reset;

    ot_point point_min;
    ot_point point_max;

    /* X (start position) of pixel align number */
    td_u32 start_x_align;
    /* Y (start position) of pixel align number */
    td_u32 start_y_align;
} rgn_capacity_point;

typedef struct {
    td_bool spt_reset;

    ot_size size_min;
    ot_size size_max;

    /* region width of pixel align number */
    td_u32 width_align;
    /* region height of pixel align number */
    td_u32 height_align;
    /* maximum area of region */
    td_u32 max_area;
} rgn_capacity_size;

typedef struct {
    td_bool spt_reset;

    td_u32 min_thick;
    td_u32 max_thick;
    td_u32 thick_align;
} rgn_capacity_thick;

typedef struct {
    td_bool spt_reset;

    /* pixel format number of region */
    td_u32 pixel_format_num;
    /*
     * all pixel format type of region supported----------related item check :8.1,
     * check channel all pixel format is same or not.
     */
    ot_pixel_format pixel_format[OT_PIXEL_FORMAT_NUM_MAX];
} rgn_capacity_pixel_format;

typedef struct {
    td_bool spt_reset;
    td_bool spt_alpha;

    td_u32 alpha_max;
    td_u32 alpha_min;
} rgn_capacity_alpha;

typedef struct {
    td_bool spt_reset;
} rgn_capacity_bg_color;

typedef enum {
    OT_RGN_SORT_BY_LAYER = 0,
    OT_RGN_SORT_BY_POSITION,
    RGN_SRT_BUTT
} rgn_sort_key;

typedef struct {
    td_bool is_sort;    /* sort or not */
    rgn_sort_key key;    /* key word used in sort */
    td_bool small_to_big;    /* the sort way: 1/true: small to larger;2/true: larger to small; */
} rgn_capacity_sort;

typedef struct {
    td_bool is_common;
    td_bool spt_reset;
    td_bool spt_qp;

    td_s32 qp_abs_min;
    td_s32 qp_abs_max;

    td_s32 qp_rel_min;
    td_s32 qp_rel_max;
} rgn_capacity_qp;

typedef enum {
    OT_RGN_CAPACITY_QUAD_UNSUPPORT = 0x0,
    OT_RGN_CAPACITY_QUAD_TYPE_SOLID = 0x1,
    OT_RGN_CAPACITY_QUAD_TYPE_UNSOLID = 0x2,
    OT_RGN_CAPACITY_QUAD_TYPE_ALL = 0x3,
    OT_RGN_CAPACITY_QUAD_TYPE_BUTT = 0x4,
} rgn_capacity_quad_type;

typedef struct {
    td_bool spt_rect_abs_coord;
    td_bool spt_quad_abs_coord;
    td_bool spt_rect_ratio_coord;
    td_bool spt_quad_ratio_coord;
} rgn_capacity_coord;

typedef struct {
    td_bool                     spt_quad;
    td_bool                     spt_reset;
    rgn_capacity_coord          coord_type;              /* coord type of cover */
    rgn_capacity_quad_type      quad_type;
} rgn_capacity_cover_type;

typedef struct {
    td_bool spt_reset;

    td_u32 mosaic_blk_size_max;
    td_u32 mosaic_blk_size_min;
} rgn_capacity_mosaic_blk_size;
#ifdef CONFIG_OT_RGN_CORNER_RECT_SUPPORT
typedef struct {
    td_bool     spt_reset;

    td_u32      min_hor_len;
    td_u32      max_hor_len;
    td_u32      min_ver_len;
    td_u32      max_ver_len;
    td_u32      ver_align;
    td_u32      hor_align;
} rgn_capacity_corner_rect_len;
#endif
typedef struct {
    rgn_capacity_pixel_format pixel_format;
    rgn_capacity_alpha        fg_alpha;
    rgn_capacity_alpha        bg_alpha;
    rgn_capacity_alpha        global_alpha;
    rgn_capacity_bg_color     bg_color;
    rgn_capacity_qp           qp;                 /* capability of QP */
    rgn_capacity_size         size;               /* region size(width and height) */
    td_u32                    stride;             /* stride align */
} rgn_capacity_overlay;

typedef struct {
    rgn_capacity_cover_type     cover_type;         /* coord type of cover */
    rgn_capacity_size           size;               /* region size(width and height) */
    rgn_capacity_thick          thick;              /* line width */
} rgn_capacity_cover;

typedef struct {
    rgn_capacity_size               size;               /* region size(width and height) */
    rgn_capacity_mosaic_blk_size    blk_size;           /* blk size of mosaic */
} rgn_capacity_mosaic;

typedef struct {
    rgn_capacity_thick          thick;  /* line width */
} rgn_capacity_line;
#ifdef CONFIG_OT_RGN_CORNER_RECT_SUPPORT
typedef struct {
    rgn_capacity_thick               thick;      /* line width */
    rgn_capacity_size                size;       /* region size(width and height) */
    rgn_capacity_corner_rect_len     len;        /* horizontal and vertical length */
} rgn_capacity_corner_rect;
#endif
typedef union {
    rgn_capacity_overlay        overlay;
    rgn_capacity_cover          cover;
    rgn_capacity_mosaic         mosaic;
    rgn_capacity_line           line;
#ifdef CONFIG_OT_RGN_CORNER_RECT_SUPPORT
    rgn_capacity_corner_rect    corner_rect;
#endif
} rgn_capacity_type;

typedef struct {
    rgn_capacity_type           type;               /* rgn type capacity */
    rgn_capacity_layer          layer;              /* layer of region */
    rgn_capacity_point          point;              /* start position */
    td_u32                      rgn_num_in_chn;     /* the maximum region of channel */
    td_bool                     spt_overlap;        /* support overlap or not */
    rgn_capacity_sort           chn_sort;
    rgn_check_chn_capacity      pfn_check_chn_capacity; /* check whether channel support a region type */
    rgn_check_chn_attr          pfn_check_attr;     /* check whether attribute is legal */
} rgn_capacity;

typedef struct {
    td_s32 (*pfn_rgn_register_mod)(ot_rgn_type type,
        const rgn_capacity *capacity, const rgn_register_info *register_info);
    td_s32 (*pfn_rgn_unregister_mod)(ot_rgn_type type, ot_mod_id mod_id);
    td_s32 (*pfn_rgn_get_region)(ot_rgn_type type, const ot_mpp_chn *chn, rgn_info *info);
    td_s32 (*pfn_rgn_put_region)(ot_rgn_type type, const ot_mpp_chn *chn, const rgn_put_info *info);
    td_s32 (*pfn_rgn_set_modify_false)(ot_rgn_type type, const ot_mpp_chn *chn);
} rgn_export_func;

#define ckfn_rgn() \
    (func_entry(rgn_export_func, OT_ID_RGN) != TD_NULL)

#define ckfn_rgn_register_mod() \
    (func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_register_mod != TD_NULL)

#define call_rgn_register_mod(type, capacity, register_info) \
    func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_register_mod(type, capacity, register_info)

#define ckfn_rgn_unregister_mod() \
    (func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_unregister_mod != TD_NULL)

#define call_rgn_unregister_mod(type, mod_id) \
    func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_unregister_mod(type, mod_id)

#define ckfn_rgn_get_region() \
    (func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_get_region != TD_NULL)

#define call_rgn_get_region(type, chn, info) \
    func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_get_region(type, chn, info)

#define ckfn_rgn_put_region() \
    (func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_put_region != TD_NULL)

#define call_rgn_put_region(type, chn, info) \
    func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_put_region(type, chn, info)

#define ckfn_rgn_set_modify_false() \
    (func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_set_modify_false != TD_NULL)

#define call_rgn_set_modify_false(type, chn) \
    func_entry(rgn_export_func, OT_ID_RGN)->pfn_rgn_set_modify_false(type, chn)

#ifdef __cplusplus
extern "C" {
#endif
td_u32 rgn_get_rgn_state(td_void);

#ifdef __cplusplus
}
#endif

#endif /* __REGION_EXT_H__ */
