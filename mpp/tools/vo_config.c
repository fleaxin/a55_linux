/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "ss_mpi_vo.h"

#define VO_CONFIG_ARG_NUMBER_BASE 10  /* 10:base */

#define VO_CONFIG_MAX_CSC_VAL 100
#define VO_CONFIG_MIN_CSC_VAL 0

#define VO_CONFIG_MAX_VGA_GAIN 63
#define VO_CONFIG_MIN_VGA_GAIN 0

#define VO_CONFIG_MAX_VGA_SHARPEN 255
#define VO_CONFIG_MIN_VGA_SHARPEN 0

typedef enum {
    VO_CONFIG_TYPE_LAYER = 0,   /* config layer csc */
    VO_CONFIG_TYPE_VGA   = 1,   /* config vga param */
    VO_CONFIG_TYPE_HDMI  = 2,   /* config hdmi param */
    VO_CONFIG_TYPE_RGB   = 3,   /* config rgb param */
    VO_CONFIG_TYPE_MIPI  = 4,   /* config mipi param */
    VO_CONFIG_TYPE_HDMI1 = 5,   /* config hdmi1 param */
    VO_CONFIG_TYPE_BUTT,
} vo_config_type;

typedef enum {
    VO_CONFIG_ARG_CMD = 0,               /* config param cmd */
    VO_CONFIG_ARG_TYPE = 1,              /* config param type */
    VO_CONFIG_ARG_ID = 2,                /* config param id */
    VO_CONFIG_ARG_CSC_MATRIX = 3,        /* config param csc matrix */
    VO_CONFIG_ARG_LUMA = 4,              /* config param csc luma */
    VO_CONFIG_ARG_CONTRAST = 5,          /* config param csc contrast */
    VO_CONFIG_ARG_HUE = 6,               /* config param csc hue */
    VO_CONFIG_ARG_SATURATION = 7,        /* config param csc saturation */
    VO_CONFIG_ARG_EX_CSC_EN = 8,         /* config param csc ex_csc_en */
    VO_CONFIG_ARG_GAIN = 9,              /* config param vga gain */
    VO_CONFIG_ARG_SHARPEN_STRENGTH = 10, /* config param vga sharpen strength */
    VO_CONFIG_ARG_BUTT,
} vo_config_arg_index;

#define VO_CONFIG_DEF_ARGC_NUM (VO_CONFIG_ARG_EX_CSC_EN + 1)        /* default 9 argc */
#define VO_CONFIG_VGA_ARGC_NUM (VO_CONFIG_ARG_SHARPEN_STRENGTH + 1) /* vga param have 11 argc */

#define vo_config_check_return(express, name)                                                    \
    do {                                                                                         \
        td_s32 _ret;                                                                             \
        _ret = express;                                                                          \
        if (_ret != TD_SUCCESS) {                                                                \
            printf("%s failed at %s: LINE: %d with %#x!\n", name, __FUNCTION__, __LINE__, _ret); \
            return _ret;                                                                         \
        }                                                                                        \
    } while (0)

static td_void usage(td_void)
{
    printf("notice: layer config usage: ./vo_config [0] [layer] [csc_matrix] [luma] [contrast] [hue] ");
    printf("[saturation] [ex_csc_en].\n");
    printf("\tsample: ./vo_config 0 0  9 50 50 50 50 0\n");
    printf("\tvga config usage: ./vo_config [1] [dev] [csc_matrix] [luma] [contrast] [hue] ");
    printf("[saturation] [ex_csc_en] [gain] [sharpen_strength].\n");
    printf("\tsample: ./vo_config 1 0 17 50 50 50 50 0 10 128\n");
    printf("\thdmi config usage: ./vo_config [2] [dev] [csc_matrix] [luma] [contrast] [hue] ");
    printf("[saturation] [ex_csc_en].\n");
    printf("\tsample: ./vo_config 2 0  9 50 50 50 50 0\n");
    printf("\trgb config usage: ./vo_config [3] [dev] [csc_matrix] [luma] [contrast] [hue] ");
    printf("[saturation] [ex_csc_en].\n");
    printf("\tsample: ./vo_config 3 0 17 50 50 50 50 0\n");
    printf("\tmipi config usage: ./vo_config [4] [dev] [csc_matrix] [luma] [contrast] [hue] ");
    printf("[saturation] [ex_csc_en].\n");
    printf("\tsample: ./vo_config 4 0  9 50 50 50 50 0\n");
    printf("\thdmi1 config usage: ./vo_config [2] [dev] [csc_matrix] [luma] [contrast] [hue] ");
    printf("[saturation] [ex_csc_en].\n");
    printf("\tsample: ./vo_config 5 0  9 50 50 50 50 0\n");
    printf("notice: video layer csc_matrix [0,15], gfx layer csc_matrix [24,27] ");
    printf("vga or rgb csc_matrix [16,23], hdmi or mipi csc_matrix [0,23].\n");
    printf("\tluma/contrast/hue/saturation [0,100], ex_csc_en [0,1], gain [0, 64), sharpen_strength [0, 255].\n");
}

static td_s32 vo_get_input_argc_num(vo_config_type config_type)
{
    td_s32 argc = VO_CONFIG_DEF_ARGC_NUM;

    if (config_type == VO_CONFIG_TYPE_VGA) {
        argc = VO_CONFIG_VGA_ARGC_NUM;
    }

    return argc;
}

static td_s32 vo_config_get_argv_val(char *argv[], vo_config_arg_index index, td_slong *val)
{
    td_char *end_ptr = TD_NULL;
    td_slong result;

    errno = 0;
    result = strtol(argv[index], &end_ptr, VO_CONFIG_ARG_NUMBER_BASE);
    if ((end_ptr == argv[index]) || (*end_ptr != '\0')) {
        return TD_FAILURE;
    } else if (((result == LONG_MIN) || (result == LONG_MAX)) &&
        (errno == ERANGE)) {
        return TD_FAILURE;
    }

    *val = result;
    return TD_SUCCESS;
}

static const td_char *vo_config_get_argv_name(vo_config_arg_index index)
{
    const td_char *argv_name[VO_CONFIG_ARG_BUTT] = {"vo_config", "config_type", "dev/layer", "csc_matrix",
        "luma", "contrast", "hue", "saturation", "ex_csc_en", "gain", "sharpen_strength"};

    if (index >= VO_CONFIG_ARG_BUTT) {
        return "-";
    }

    return argv_name[index];
}

static td_s32 vo_config_get_valid_argv_val(char *argv[], vo_config_arg_index index,
    td_s32 min_val, td_s32 max_val, td_s32 *val)
{
    td_s32 ret;
    td_slong result = 0;

    ret = vo_config_get_argv_val(argv, index, &result);
    if (ret != TD_SUCCESS) {
        printf("Failure: input arg index(%u) name(%s) arg is invalid!\n",
            index, vo_config_get_argv_name(index));
        usage();
        return ret;
    }

    if ((result < min_val) || (result > max_val)) {
        printf("Failure: input arg index(%u) name(%s) arg val (%ld) is wrong. should be [%d, %d]!\n",
            index, vo_config_get_argv_name(index), result, min_val, max_val);
        usage();
        return TD_FAILURE;
    }

    *val = (td_s32)result;
    return TD_SUCCESS;
}

static td_s32 vo_get_input_id_arg(vo_config_type config_type, char *argv[], td_s32 *id)
{
    td_s32 min_val = 0;
    td_s32 max_val = (config_type == VO_CONFIG_TYPE_LAYER) ? (OT_VO_MAX_LAYER_NUM - 1) : OT_VO_MAX_DEV_NUM - 1;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_ID, min_val, max_val, id),
        "vo_config_get_valid_argv_val");

    return TD_SUCCESS;
}

static td_s32 vo_get_input_csc_arg(char *argv[], ot_vo_csc *csc)
{
    td_s32 val = 0;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_CSC_MATRIX,
        OT_VO_CSC_MATRIX_BT601LIMIT_TO_BT601LIMIT, OT_VO_CSC_MATRIX_BUTT - 1, &val), "vo_config_get_argv");
    csc->csc_matrix = (ot_vo_csc_matrix)val;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_LUMA,
        VO_CONFIG_MIN_CSC_VAL, VO_CONFIG_MAX_CSC_VAL, &val), "vo_config_get_argv");
    csc->luma = (td_u32)val;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_CONTRAST,
        VO_CONFIG_MIN_CSC_VAL, VO_CONFIG_MAX_CSC_VAL, &val), "vo_config_get_argv");
    csc->contrast = (td_u32)val;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_HUE,
        VO_CONFIG_MIN_CSC_VAL, VO_CONFIG_MAX_CSC_VAL, &val), "vo_config_get_argv");
    csc->hue = (td_u32)val;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_SATURATION,
        VO_CONFIG_MIN_CSC_VAL, VO_CONFIG_MAX_CSC_VAL, &val), "vo_config_get_argv");
    csc->saturation = (td_u32)val;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_EX_CSC_EN,
        TD_FALSE, TD_TRUE, &val), "vo_config_get_argv");
    csc->ex_csc_en = (td_bool)val;

    return TD_SUCCESS;
}

static td_void vo_print_csc_config(const ot_vo_csc *csc)
{
    printf("csc_matrix %d, luma %d, contrast %d, hue %d, saturation %d, ex_csc_en %d", csc->csc_matrix, csc->luma,
        csc->contrast, csc->hue, csc->saturation, csc->ex_csc_en);
}

static td_s32 vo_get_input_arg(vo_config_type config_type, char *argv[], td_s32 *id, ot_vo_csc *csc)
{
    vo_config_check_return(vo_get_input_id_arg(config_type, argv, id), "vo_get_input_id_arg");
    vo_config_check_return(vo_get_input_csc_arg(argv, csc), "vo_get_input_csc_arg");

    return TD_SUCCESS;
}

static td_s32 vo_do_layer_config(vo_config_type config_type, char *argv[])
{
    ot_vo_layer layer = 0;
    ot_vo_csc csc = {0};

    vo_config_check_return(vo_get_input_arg(config_type, argv, &layer, &csc), "vo_get_input_arg");
    vo_config_check_return(ss_mpi_vo_set_layer_csc(layer, &csc), "ss_mpi_vo_set_layer_csc");
    vo_config_check_return(ss_mpi_vo_get_layer_csc(layer, &csc), "ss_mpi_vo_get_layer_csc");
    printf("layer config layer_id %d ", layer);
    vo_print_csc_config(&csc);
    printf("\n");
    return TD_SUCCESS;
}

static td_s32 vo_get_input_vga_param(vo_config_type config_type, char *argv[], td_s32 *id, ot_vo_vga_param *vga_param)
{
    td_s32 val = 0;

    vo_config_check_return(vo_get_input_arg(config_type, argv, id, &(vga_param->csc)), "vo_get_input_arg");

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_GAIN,
        VO_CONFIG_MIN_VGA_GAIN, VO_CONFIG_MAX_VGA_GAIN, &val), "vo_config_get_valid_argv_val");
    vga_param->gain = (td_u32)val;

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_SHARPEN_STRENGTH,
        VO_CONFIG_MIN_VGA_SHARPEN, VO_CONFIG_MAX_VGA_SHARPEN, &val), "vo_config_get_valid_argv_val");
    vga_param->sharpen_strength = (td_u32)val;

    return TD_SUCCESS;
}

static td_s32 vo_do_vga_config(vo_config_type config_type, char *argv[])
{
    ot_vo_dev dev = 0;
    ot_vo_vga_param vga_param = {0};

    vo_config_check_return(vo_get_input_vga_param(config_type, argv, &dev, &vga_param), "vo_get_input_vga_param");
    vo_config_check_return(ss_mpi_vo_set_vga_param(dev, &vga_param), "ss_mpi_vo_set_vga_param");
    vo_config_check_return(ss_mpi_vo_get_vga_param(dev, &vga_param), "ss_mpi_vo_get_vga_param");
    printf("vga config dev_id %d ", dev);
    vo_print_csc_config(&(vga_param.csc));
    printf(" gain %d, sharpen_strength %d\n", vga_param.gain, vga_param.sharpen_strength);
    return TD_SUCCESS;
}

static td_s32 vo_do_hdmi_config(vo_config_type config_type, char *argv[])
{
    ot_vo_dev dev = 0;
    ot_vo_hdmi_param hdmi_param = {0};

    vo_config_check_return(vo_get_input_arg(config_type, argv, &dev, &(hdmi_param.csc)), "vo_get_input_arg");
    vo_config_check_return(ss_mpi_vo_set_hdmi_param(dev, &hdmi_param), "ss_mpi_vo_set_hdmi_param");
    vo_config_check_return(ss_mpi_vo_get_hdmi_param(dev, &hdmi_param), "ss_mpi_vo_get_hdmi_param");
    printf("hdmi config dev_id %d ", dev);
    vo_print_csc_config(&(hdmi_param.csc));
    printf("\n");
    return TD_SUCCESS;
}

static td_s32 vo_do_rgb_config(vo_config_type config_type, char *argv[])
{
    ot_vo_dev dev = 0;
    ot_vo_rgb_param rgb_param = {0};
    ot_vo_rgb_param old_rgb_param = {0};

    vo_config_check_return(vo_get_input_arg(config_type, argv, &dev, &(rgb_param.csc)), "vo_get_input_arg");
    vo_config_check_return(ss_mpi_vo_get_rgb_param(dev, &old_rgb_param), "ss_mpi_vo_get_rgb_param");
    rgb_param.rgb_inverted_en = old_rgb_param.rgb_inverted_en;
    rgb_param.bit_inverted_en = old_rgb_param.bit_inverted_en;

    vo_config_check_return(ss_mpi_vo_set_rgb_param(dev, &rgb_param), "ss_mpi_vo_set_rgb_param");
    vo_config_check_return(ss_mpi_vo_get_rgb_param(dev, &rgb_param), "ss_mpi_vo_get_rgb_param");
    printf("rgb config dev_id %d ", dev);
    vo_print_csc_config(&(rgb_param.csc));
    printf("\n");
    return TD_SUCCESS;
}

static td_s32 vo_do_mipi_config(vo_config_type config_type, char *argv[])
{
    ot_vo_dev dev = 0;
    ot_vo_mipi_param mipi_param = {0};

    vo_config_check_return(vo_get_input_arg(config_type, argv, &dev, &(mipi_param.csc)), "vo_get_input_arg");
    vo_config_check_return(ss_mpi_vo_set_mipi_param(dev, &mipi_param), "ss_mpi_vo_set_mipi_param");
    vo_config_check_return(ss_mpi_vo_get_mipi_param(dev, &mipi_param), "ss_mpi_vo_get_mipi_param");
    printf("mipi config dev_id %d ", dev);
    vo_print_csc_config(&(mipi_param.csc));
    printf("\n");
    return TD_SUCCESS;
}

static td_s32 vo_do_hdmi1_config(vo_config_type config_type, char *argv[])
{
    ot_vo_dev dev = 0;
    ot_vo_hdmi_param hdmi1_param = {0};

    vo_config_check_return(vo_get_input_arg(config_type, argv, &dev, &(hdmi1_param.csc)), "vo_get_input_csc_param");
    vo_config_check_return(ss_mpi_vo_set_hdmi1_param(dev, &hdmi1_param), "ss_mpi_vo_set_hdmi1_param");
    vo_config_check_return(ss_mpi_vo_get_hdmi1_param(dev, &hdmi1_param), "ss_mpi_vo_get_hdmi1_param");
    printf("hdmi1 config dev_id %d ", dev);
    vo_print_csc_config(&(hdmi1_param.csc));
    printf("\n");
    return TD_SUCCESS;
}

static td_s32 vo_do_config(vo_config_type config_type, char *argv[])
{
    switch (config_type) {
        case VO_CONFIG_TYPE_LAYER:
            return vo_do_layer_config(config_type, argv);

        case VO_CONFIG_TYPE_VGA:
            return vo_do_vga_config(config_type, argv);

        case VO_CONFIG_TYPE_HDMI:
            return vo_do_hdmi_config(config_type, argv);

        case VO_CONFIG_TYPE_RGB:
            return vo_do_rgb_config(config_type, argv);

        case VO_CONFIG_TYPE_MIPI:
            return vo_do_mipi_config(config_type, argv);

        case VO_CONFIG_TYPE_HDMI1:
            return vo_do_hdmi1_config(config_type, argv);

        default:
            printf("config type %d is invalid!\n", config_type);
            return OT_ERR_VO_ILLEGAL_PARAM;
    }
}


#ifdef __LITEOS__
td_s32 vo_config(int argc, char *argv[])
#else
td_s32 main(int argc, char *argv[])
#endif
{
    td_s32 argc_num;
    td_s32 val = 0;
    vo_config_type config_type;

    printf("\nnotice: this tool only can be used for TESTING !!!\n");
#ifndef __LITEOS__
    printf("\tTo see more usage, please enter: ./vo_config -h\n\n");
#else
    printf("\tTo see more usage, please enter: vo_config -h\n\n");
#endif

    if (argc <= 1) {
        printf("Failure: too little parameters!\n");
        usage();
        return OT_ERR_VO_ILLEGAL_PARAM;
    }

    if ((strncmp(argv[1], "-h", 2)) == 0) { /* 2 chars */
        usage();
        return TD_SUCCESS;
    }

    vo_config_check_return(vo_config_get_valid_argv_val(argv, VO_CONFIG_ARG_TYPE,
        VO_CONFIG_TYPE_LAYER, VO_CONFIG_TYPE_BUTT - 1, &val), "vo_config_get_valid_argv_val");
    config_type = (vo_config_type)val;

    argc_num = vo_get_input_argc_num(config_type);
    if (argc != argc_num) {
        printf("Failure: config type %d should have %d param, current input %d param!\n",
            config_type, argc_num, argc);
        usage();
        return OT_ERR_VO_ILLEGAL_PARAM;
    }

    return vo_do_config(config_type, argv);
}

