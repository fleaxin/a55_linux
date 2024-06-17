/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <math.h>
#include "isp_config.h"
#include "isp_math_utils.h"
#include "isp_inner.h"
#include "ot_mpi_sys.h"
#include "ot_math.h"

#define LSC_MEASURE_Q       1024
#define WINDOWSIZEX         20
#define WINDOWSIZEY         20
#define BAYER_CHN_2D        2
#define RLSC_NODE_NUM       129
#define RLSC_MAX_GAIN       65535
#define RLSC_MAX_PIXEL_VALUE ((1<<16) - 1)

#define LSC_MAX_PIXEL_VALUE ((1<<20) - 1)
#define LSC_ARR_LEN         400

#define BPRG    0
#define BPGR    1
#define BPGB    2
#define BPBG    3

typedef struct {
    td_u32 grid_width_step[OT_ISP_LSC_GRID_COL];
    td_u32 grid_height_step[OT_ISP_LSC_GRID_ROW];
} isp_lsc_grid_step;

typedef struct {
    td_u32 r_data[OT_ISP_LSC_GRID_POINTS];
    td_u32 gr_data[OT_ISP_LSC_GRID_POINTS];
    td_u32 gb_data[OT_ISP_LSC_GRID_POINTS];
    td_u32 b_data[OT_ISP_LSC_GRID_POINTS];
} ls_data;

typedef struct {
    td_s32 x;
    td_s32 y;
} ls_location;

static const td_float g_max_gain_array[OT_ISP_LSC_MESHSCALE_NUM] = {
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)OT_ISP_LSC_MESHSCALE0_DEF_GAIN, /* 1.9 */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)OT_ISP_LSC_MESHSCALE1_DEF_GAIN, /* 2.8 */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)OT_ISP_LSC_MESHSCALE2_DEF_GAIN, /* 3.7 */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)OT_ISP_LSC_MESHSCALE3_DEF_GAIN, /* 4.6 */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)1024 + 1,                       /* 0.10, 1024 is base gain */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)512 + 1,                        /* 1.9, 512 is base gain */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)256 + 1,                        /* 2.8, 256 is base gain */
    (td_float)OT_ISP_LSC_MAX_GAIN / (td_float)128 + 1,                        /* 3.7, 128 is base gain */
};

static const td_u8 g_bayer_patt_lut[OT_ISP_BAYER_CHN_NUM][BAYER_CHN_2D][BAYER_CHN_2D] = {
    {{BPRG, BPGR}, {BPGB, BPBG}}, /* 0 RGGB */
    {{BPGR, BPRG}, {BPBG, BPGB}}, /* 1 GRBG */
    {{BPGB, BPBG}, {BPRG, BPGR}}, /* 2 GBRG */
    {{BPBG, BPGB}, {BPGR, BPRG}}  /* 3 BGGR */
};

typedef struct {
    td_s32 square_radius;
    td_u32 window_size;
    td_u8  value;
    td_s32 true_blc_offset;
} rlsc_calc_cfg;

typedef struct {
    td_u32 r_gain[RLSC_NODE_NUM];
    td_u32 gr_gain[RLSC_NODE_NUM];
    td_u32 gb_gain[RLSC_NODE_NUM];
    td_u32 b_gain[RLSC_NODE_NUM];
} rlsc_calib_gain;

typedef struct {
    td_u32 height;
    td_u32 width;
    td_u32 stride;

    td_u32 begin_pos_x;
    td_u32 begin_pos_y;
    td_u32 end_pos_x;
    td_u32 end_pos_y;

    td_u32 window_size;
    td_u32 eliminate_pct;

    td_u32 bit_depth;
    td_u32 bayer_format;

    td_bool offset_in_en;
    td_u32 blc_offset_r;
    td_u32 blc_offset_gr;
    td_u32 blc_offset_gb;
    td_u32 blc_offset_b;

    td_u32 gain_scale;
} rlsc_calibration_cfg;

static td_u32 get_max_data(const td_u32 *data, int length)
{
    int i;

    td_u32 max_data = 0;
    for (i = 0; i < length; i++) {
        if (data[i] > max_data) {
            max_data = data[i];
        }
    }
    return max_data;
}

static td_u32 get_min_data(const td_u32 *data, int length)
{
    int i;

    td_u32 min_data = LSC_MAX_PIXEL_VALUE;
    for (i = 0; i < length; i++) {
        if (data[i] < min_data) {
            min_data = data[i];
        }
    }
    return min_data;
}

/*
 * [data_enlarge description]
 * @param data        [input] input data
 * @param max_data     [input] input target value
 * @param mesh_scale   [input] input scale value
 * generate gain value at each grid point, uniformed by input mesh_scale
 */
static td_u32 data_enlarge(td_u32 data, td_u32 max_data, td_u32 mesh_scale)
{
    td_u32 ratio;
    td_u32 lsc_q_value;
    if (mesh_scale < 4) { /* calculate mesh_scale under 4 separately */
        lsc_q_value = 1U << (9 - mesh_scale); /* 9 to get base gain for each meshscale */
        ratio = (td_u32)(((td_float)max_data / (td_float)div_0_to_1(data)) * lsc_q_value);
    } else {
        lsc_q_value = 1U << (14 - mesh_scale); /* 14 to get base gain for each meshscale */
        ratio = (td_u32)(((td_float)max_data / (td_float)div_0_to_1(data) - 1) * lsc_q_value);
    }

    return MIN2(ratio, OT_ISP_LSC_MAX_GAIN);
}

static td_u32 rlsc_data_enlarge(td_u32 data, td_u32 max_data, td_u8 scale)
{
    td_u32 ratio;
    td_u32 rlsc_q_value;
    if (scale < 7) { /* calculate scale under 7 separately */
        rlsc_q_value = 1U << (15 - scale); /* 15 to get base gain for each scale */
        ratio = (td_u32)(((td_float)max_data / (td_float)div_0_to_1(data)) * rlsc_q_value);
    } else {
        rlsc_q_value = 1U << (23 - scale); /* 23 to get base gain for each scale */
        ratio = (td_u32)(((td_float)max_data / (td_float)div_0_to_1(data) - 1) * rlsc_q_value);
    }
    return MIN2(ratio, RLSC_MAX_GAIN);
}

/*
 * [sort description]
 * @param array  [input] input array
 * @param num     [input] array length
 * sort the input array from min to max
 */
static td_void sort(td_u32 *array, td_u32 num)
{
    td_u32 i, j;
    td_u32 temp;
    for (i = 0; i < num; i++) {
        for (j = 0; j < (num - 1 - i); j++) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

static td_u32 get_point_average(const td_u16 *bayer_img, const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg,
    td_s32 blc_offset, td_u8 value, const ls_location *location)
{
    td_s32 height   = (td_s32)lsc_cali_cfg->dst_img_height / 2; /* process by channel, height is divided by 2 */
    td_s32 width    = (td_s32)lsc_cali_cfg->dst_img_width / 2; /* process by channel, width is divided by 2 */
    td_u32 stride = lsc_cali_cfg->dst_img_width;
    td_u8  bayer = lsc_cali_cfg->bayer;
    td_s32 x_start, x_end, y_start, y_end, x, y;

    td_u32 chn_index = (bayer & 0x3) ^ value;
    td_s32 w = chn_index & 0x1;
    td_s32 h = (chn_index >> 1) & 0x1;
    td_u32 coor_index, count;
    td_u32 num = 0;
    td_u32 num_temp = 0;
    td_u32 sum = 0;
    td_u32 data[LSC_ARR_LEN] = {0};

    x_start = location->x - WINDOWSIZEX / 2; /* process by channel, need to divided by 2 */
    x_end   = location->x + WINDOWSIZEX / 2; /* process by channel, need to divided by 2 */
    y_start = location->y - WINDOWSIZEY / 2; /* process by channel, need to divided by 2 */
    y_end   = location->y + WINDOWSIZEY / 2; /* process by channel, need to divided by 2 */

    if (x_start < 0) {
        x_start = 0;
    }
    if (x_end >= width) {
        x_end = width - 1;
    }
    if (y_start < 0) {
        y_start = 0;
    }
    if (y_end >= height) {
        y_end = height - 1;
    }

    for (y = y_start; y < y_end; y++) {
        for (x = x_start; x < x_end; x++) {
            coor_index = 2 * x + 2 * y * stride; /* search in the origin img, multiply by 2 */
            data[num++] = (td_u32)MAX2(0, ((td_s32)((td_u32)(bayer_img[coor_index + w + h * stride]) <<
                (20 - (td_u16)lsc_cali_cfg->raw_bit)) - blc_offset)); /* process in 20 bit */
        }
    }
    sort(data, num);

    for (count = num / 10; count < num * 9 / 10; count++) { /* drop 10% of the bottom data, 9 for 90% of the data */
        sum  += data[count];
        num_temp++;
    }

    if (num_temp) {
        return sum / num_temp;
    } else {
        return 0;
    }
}

/*
 * [get_lsc_data_channel description]
 * @param bayer_img        [input]  input image data
 * @param grid_data        [output] mesh_lsc grid data generated
 * @param lsc_cali_cfg     [input]  mesh LSC calibration configure
 * @param lsc_grid_step_xy [input]  input grid width&height information
 * @param blc_offset       [input]  input BLC value
 * @param value            [input]  indicate channel number: 0-R, 1-gr, 2-gb, 3-B
 */
static td_s32 get_lsc_data_channel(const td_u16 *bayer_img, td_u32 *grid_data,
    const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg, const isp_lsc_grid_step *lsc_grid_step_xy, td_u8 value)
{
    td_s32 true_blc_offset;
    td_u32 i, j;

    ls_location location;
    location.y = 0;

    if (bayer_img == TD_NULL || grid_data == TD_NULL) {
        isp_err_trace("%s: LINE: %d bayer_img / grid_data is TD_NULL pointer !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    switch (value) { /* input BLC is 12 bit, move to 20 bit */
        case BPRG:
            true_blc_offset = (td_s32)(((td_u32)lsc_cali_cfg->blc_offset_r) << 0x8);
            break;

        case BPGR:
            true_blc_offset = (td_s32)(((td_u32)lsc_cali_cfg->blc_offset_gr) << 0x8);
            break;

        case BPGB:
            true_blc_offset = (td_s32)(((td_u32)lsc_cali_cfg->blc_offset_gb) << 0x8);
            break;

        case BPBG:
            true_blc_offset = (td_s32)(((td_u32)lsc_cali_cfg->blc_offset_b) << 0x8);
            break;

        default:
            true_blc_offset = (td_s32)(((td_u32)lsc_cali_cfg->blc_offset_r) << 0x8);
            break;
    }

    for (i = 0; i < OT_ISP_LSC_GRID_ROW; i++) {
        location.y = location.y + lsc_grid_step_xy->grid_height_step[i];
        location.x = 0;
        for (j = 0; j < OT_ISP_LSC_GRID_COL; j++) {
            location.x = location.x + lsc_grid_step_xy->grid_width_step[j];

            grid_data[i * OT_ISP_LSC_GRID_COL + j] = get_point_average(bayer_img, lsc_cali_cfg, true_blc_offset,
                                                                       value, &location);
        }
    }

    return TD_SUCCESS;
}

/*
 * [get_ls_data description]
 * @param bayer_img        [input]  input image data
 * @param lsc_grid_data    [output] mesh_lsc grid data generated
 * @param lsc_grid_step_xy [input]  input grid width&height information
 * @param lsc_cali_cfg     [input]  mesh_lsc calibration results
 */
static td_s32 get_ls_data(const td_u16 *bayer_img, ls_data *lsc_grid_data, const isp_lsc_grid_step *lsc_grid_step_xy,
                          const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg)
{
    td_s32 ret;

    ret = get_lsc_data_channel(bayer_img, lsc_grid_data->r_data, lsc_cali_cfg, lsc_grid_step_xy, BPRG);
    if (ret != TD_SUCCESS) {
        isp_err_trace("%s: LINE: %d get_lsc_data of R channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    ret = get_lsc_data_channel(bayer_img, lsc_grid_data->gr_data, lsc_cali_cfg, lsc_grid_step_xy, BPGR);
    if (ret != TD_SUCCESS) {
        isp_err_trace("%s: LINE: %d get_lsc_data of gr channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    ret = get_lsc_data_channel(bayer_img, lsc_grid_data->gb_data, lsc_cali_cfg, lsc_grid_step_xy, BPGB);
    if (ret != TD_SUCCESS) {
        isp_err_trace("%s: LINE: %d get_lsc_data of gb channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    ret = get_lsc_data_channel(bayer_img, lsc_grid_data->b_data, lsc_cali_cfg, lsc_grid_step_xy, BPBG);
    if (ret != TD_SUCCESS) {
        isp_err_trace("%s: LINE: %d get_lsc_data of B channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

/*
 * [lc_normalization description]
 * @param ls_data          [input]  input grid data
 * @param lsc_grid_points  [input]  mesh_lsc calibration results
 * @param mesh_scale       [input]  input mesh_lsc calibration config
 * this function is used for testing if the input mesh scale is suitable for the current condition
 */
static td_void mesh_scale_test(const ls_data *ls_grid_data, td_u32 lsc_grid_points, td_u32 mesh_scale)
{
    td_u32 b_max_data, gb_max_data, gr_max_data, r_max_data;
    td_u32 b_min_data, gb_min_data, gr_min_data, r_min_data;
    td_float b_max_gain, gb_max_gain, gr_max_gain, r_max_gain;
    td_float max_gain;

    /* for b channel */
    b_max_data  = get_max_data(ls_grid_data->b_data, lsc_grid_points);
    /* for gb channel */
    gb_max_data = get_max_data(ls_grid_data->gb_data, lsc_grid_points);
    /* for gr channel */
    gr_max_data = get_max_data(ls_grid_data->gr_data, lsc_grid_points);
    /* for r channel */
    r_max_data  = get_max_data(ls_grid_data->r_data, lsc_grid_points);

    /* for b channel */
    b_min_data  = get_min_data(ls_grid_data->b_data, lsc_grid_points);
    /* for gb channel */
    gb_min_data = get_min_data(ls_grid_data->gb_data, lsc_grid_points);
    /* for gr channel */
    gr_min_data = get_min_data(ls_grid_data->gr_data, lsc_grid_points);
    /* for r channel */
    r_min_data  = get_min_data(ls_grid_data->r_data, lsc_grid_points);

    b_max_gain  = (td_float)b_max_data  / (td_float)b_min_data;
    gb_max_gain = (td_float)gb_max_data / (td_float)gb_min_data;
    gr_max_gain = (td_float)gr_max_data / (td_float)gr_min_data;
    r_max_gain  = (td_float)r_max_data  / (td_float)r_min_data;

    max_gain = MAX2(MAX3(b_max_gain, gb_max_gain, gr_max_gain), r_max_gain);
    if (max_gain > g_max_gain_array[mesh_scale]) {
        printf("WARNING:\n");
        printf("max gain = %f\n", max_gain);

        if (mesh_scale < 4) { /* under mesh_scale 4, min gain's value is 0 */
            if ((max_gain > g_max_gain_array[0]) && (max_gain <= g_max_gain_array[1])) { /* (2,4] */
                printf("please set mesh scale to %d\n", 1);
            } else if ((max_gain > g_max_gain_array[1]) && (max_gain <= g_max_gain_array[2])) { /* 2: (4,8] */
                printf("please set mesh scale to %d\n", 2); /* 2 */
            } else if ((max_gain > g_max_gain_array[2]) && (max_gain <= g_max_gain_array[3])) { /* 2, 3: (8,16] */
                printf("please set mesh scale to %d\n", 3); /* 3 */
            } else { /* >16 */
                printf("max gain is too large, please set meshscale to %d to reduce loss\n", 3); /* 3: >16 */
            }
        } else { /* min gain's value is 1 */
            if ((max_gain > g_max_gain_array[4]) && (max_gain <= g_max_gain_array[5])) { /* 4, 5: (2,3] */
                printf("please set mesh scale to %d\n", 5); /* 5 */
            } else if ((max_gain > g_max_gain_array[5]) && (max_gain <= g_max_gain_array[6])) { /* 5, 6: (3,5] */
                printf("please set mesh scale to %d\n", 6); /* 6 */
            } else if ((max_gain > g_max_gain_array[6]) && (max_gain <= g_max_gain_array[7])) { /* 6, 7: (5,9] */
                printf("please set mesh scale to %d\n", 7); /* 7 */
            } else if ((max_gain > g_max_gain_array[7]) && (max_gain <= g_max_gain_array[3])) { /* 3, 7: (9,16] */
                printf("please set mesh scale to %d\n", 3); /* 3 */
            } else {
                printf("max gain is too large, please set meshscale to %d to reduce loss\n", 3); /* 3: >16 */
            }
        }
    }

    return;
}

/*
 * [lc_normalization description]
 * @param lsc_grid_data     [input]  input grid data
 * @param lsc_cali_result   [output] mesh_lsc calibration results
 * @param lsc_cali_cfg      [input]  input mesh_lsc calibration config
 */
static td_void lc_normalization(const ls_data *lsc_grid_data, ot_isp_mesh_shading_table *lsc_cali_result,
                                const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg)
{
    td_u32 b_max_data, gb_max_data, gr_max_data, r_max_data;
    td_u32 lsc_grid_points;
    td_u32 i;

    lsc_grid_points = OT_ISP_LSC_GRID_POINTS;

    mesh_scale_test(lsc_grid_data, OT_ISP_LSC_GRID_POINTS, lsc_cali_cfg->mesh_scale);

    /* find the max data of each channel */
    /* for b channel */
    b_max_data  = get_max_data(lsc_grid_data->b_data, lsc_grid_points);
    /* for gb channel */
    gb_max_data = get_max_data(lsc_grid_data->gb_data, lsc_grid_points);
    /* for gr channel */
    gr_max_data = get_max_data(lsc_grid_data->gr_data, lsc_grid_points);
    /* for r channel */
    r_max_data  = get_max_data(lsc_grid_data->r_data, lsc_grid_points);

    for (i = 0; i < lsc_grid_points; i++) {
        /* normalization process */
        lsc_cali_result->lsc_gain_lut.b_gain[i]  = (td_u16)(MIN2(data_enlarge(lsc_grid_data->b_data[i],
            b_max_data, lsc_cali_cfg->mesh_scale), LSC_MAX_PIXEL_VALUE));
        lsc_cali_result->lsc_gain_lut.gb_gain[i] = (td_u16)(MIN2(data_enlarge(lsc_grid_data->gb_data[i],
            gb_max_data, lsc_cali_cfg->mesh_scale), LSC_MAX_PIXEL_VALUE));
        lsc_cali_result->lsc_gain_lut.gr_gain[i] = (td_u16)(MIN2(data_enlarge(lsc_grid_data->gr_data[i],
            gr_max_data, lsc_cali_cfg->mesh_scale), LSC_MAX_PIXEL_VALUE));
        lsc_cali_result->lsc_gain_lut.r_gain[i]  = (td_u16)(MIN2(data_enlarge(lsc_grid_data->r_data[i],
            r_max_data, lsc_cali_cfg->mesh_scale), LSC_MAX_PIXEL_VALUE));
    }
    lsc_cali_result->mesh_scale = lsc_cali_cfg->mesh_scale;

    return;
}

/*
 * [geometric_grid_size_y description]
 * @param lsc_cali_cfg      [input]  input mesh_lsc calibration config
 * @param lsc_grid_step_xy  [output] grid width&height info
 * @param lsc_cali_result   [output] mesh_lsc calibration results
 */
static td_void geometric_grid_size_x(const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg,
    isp_lsc_grid_step *lsc_grid_step_xy, ot_isp_mesh_shading_table *lsc_cali_result)
{
    td_u32   tmp_step_x[OT_ISP_MLSC_X_HALF_GRID_NUM];
    td_float rx[OT_ISP_MLSC_X_HALF_GRID_NUM];
    td_float sum_r;

    td_u32 i, sum;
    td_u32 half_grid_size_x = OT_ISP_MLSC_X_HALF_GRID_NUM;
    td_u32 width, diff;

    const td_float r1 = 1.0f;
    width = lsc_cali_cfg->dst_img_width / 2; /* process by channel, need to divided by 2 */

    rx[0] = 1.0f;
    for (i = 1; i < half_grid_size_x; i++) {
        rx[i] = rx[i - 1] * r1;
    }
    sum_r = 0;
    for (i = 0; i < half_grid_size_x; i++) {
        sum_r = sum_r + rx[i];
    }
    for (i = 0; i < half_grid_size_x; i++) {
        tmp_step_x[i] = (int)((((width >> 1) * 0x400 / div_0_to_1_float(sum_r)) * rx[i] + 0x200) / 0x400);
    }

    sum = 0;
    for (i = 0; i < half_grid_size_x; i++) {
        sum = sum + tmp_step_x[i];
    }
    if (sum != (width / 2)) { /* calculate only 1/2 of the width */
        if (sum > (width / 2)) { /* calculate only 1/2 of the width */
            diff = sum - width / 2; /* calculate only 1/2 of the width */
            for (i = 0; i < diff; i++) {
                tmp_step_x[i] = tmp_step_x[i] - 1;
            }
        } else {
            diff = width / 2 - sum; /* calculate only 1/2 of the width */
            for (i = 0; i < diff; i++) {
                tmp_step_x[half_grid_size_x - i - 1] = tmp_step_x[half_grid_size_x - i - 1] + 1;
            }
        }
    }

    /* return the step length value to grid_step_width and grid_step_height */
    lsc_grid_step_xy->grid_width_step[0] = 0;
    for (i = 1; i <= half_grid_size_x; i++) {
        lsc_grid_step_xy->grid_width_step[i] = tmp_step_x[i - 1];
        lsc_grid_step_xy->grid_width_step[OT_ISP_LSC_GRID_COL - i] = tmp_step_x[i - 1];
        lsc_cali_result->x_grid_width[i - 1] = lsc_grid_step_xy->grid_width_step[i];
    }

    return;
}

/*
 * [geometric_grid_size_y description]
 * @param lsc_cali_cfg      [input]  input mesh_lsc calibration config
 * @param lsc_grid_step_xy   [output]  grid width&height info
 * @param lsc_cali_result   [output] mesh_lsc calibration results
 */
static td_void geometric_grid_size_y(const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg,
    isp_lsc_grid_step *lsc_grid_step_xy, ot_isp_mesh_shading_table *lsc_cali_result)
{
    td_float ry[OT_ISP_MLSC_Y_HALF_GRID_NUM];
    td_float sum_r;
    td_u32   tmp_step_y[OT_ISP_MLSC_Y_HALF_GRID_NUM];

    td_u32 j, sum;
    td_u32 half_grid_size_y = OT_ISP_MLSC_Y_HALF_GRID_NUM;
    td_u32 height, diff;

    height = lsc_cali_cfg->dst_img_height / 2; /* process by channel, need to divided by 2 */

    const td_float r1 = 1.0f;
    ry[0] = 1.0f;

    for (j = 1; j < half_grid_size_y; j++) {
        ry[j] = ry[j - 1] * r1;
    }
    sum_r = 0;
    for (j = 0; j < half_grid_size_y; j++) {
        sum_r = sum_r + ry[j];
    }
    for (j = 0; j < half_grid_size_y; j++) {
        tmp_step_y[j] = (int)((((height >> 1) * 0x400 / div_0_to_1_float(sum_r)) * ry[j] + 0x200) / 0x400);
    }

    sum = 0;
    for (j = 0; j < half_grid_size_y; j++) {
        sum = sum + tmp_step_y[j];
    }
    if (sum != (height / 2)) { /* calculate only 1/2 of the height */
        if (sum > (height / 2)) { /* calculate only 1/2 of the height */
            diff = sum - height / 2; /* calculate only 1/2 of the height */
            for (j = 0; j < diff; j++) {
                tmp_step_y[j] = tmp_step_y[j] - 1;
            }
        } else {
            diff = height / 2 - sum; /* calculate only 1/2 of the height */
            for (j = 0; j < diff; j++) {
                tmp_step_y[half_grid_size_y - j - 1] = tmp_step_y[half_grid_size_y - j - 1] + 1;
            }
        }
    }

    /* return the step length value to grid_step_width and grid_step_height */
    lsc_grid_step_xy->grid_height_step[0] = 0;
    for (j = 1; j <= half_grid_size_y; j++) {
        lsc_grid_step_xy->grid_height_step[j] = tmp_step_y[j - 1];
        lsc_grid_step_xy->grid_height_step[OT_ISP_LSC_GRID_ROW - j] = tmp_step_y[j - 1];
        lsc_cali_result->y_grid_width[j - 1] = lsc_grid_step_xy->grid_height_step[j];
    }

    return;
}

/*
 * [lsc_generate_grid_info description]
 * @param data             [input]  input raw data that used for calibration
 * @param lsc_cali_cfg     [input]  input mesh_lsc calibration config
 * @param lsc_grid_step_xy [input]  grid width&height info
 * @param lsc_cali_result  [output] mesh_lsc calibration results
 */
static td_s32 lsc_generate_grid_info(const td_u16 *data, const ot_isp_mlsc_calibration_cfg *lsc_cali_cfg,
    const isp_lsc_grid_step *lsc_grid_step_xy, ot_isp_mesh_shading_table *lsc_cali_result)
{
    /* pass all the input params to the function */
    ls_data *ls_grid_data = TD_NULL;
    td_s32 ret;

    /* memory allocation */
    ls_grid_data = (ls_data *)isp_malloc(sizeof(ls_data));
    if (ls_grid_data == TD_NULL) {
        isp_err_trace("ls_gird_data allocation fail!\n");
        return TD_FAILURE;
    }

    (td_void)memset_s(ls_grid_data, sizeof(ls_data), 0, sizeof(ls_data));

    /* get lens correction coefficients */
    ret = get_ls_data(data, ls_grid_data, lsc_grid_step_xy, lsc_cali_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("there are some errors in get_ls_data()!\n");
        isp_free(ls_grid_data);
        return TD_FAILURE;
    }

    lc_normalization(ls_grid_data, lsc_cali_result, lsc_cali_cfg);

    isp_free(ls_grid_data);
    return ret;
}

static td_u32 get_point_mean_value(const td_u16 *bayer_img, const rlsc_calibration_cfg *rlsc_cali_cfg,
                                   const ls_location *point, const rlsc_calc_cfg *calc_cfg)
{
    td_s32 x, y, x_start, x_end, y_start, y_end, diff_x, diff_y, square_radius;
    td_s32 begin_x = (td_s32)rlsc_cali_cfg->begin_pos_x;
    td_s32 begin_y = (td_s32)rlsc_cali_cfg->begin_pos_y;
    td_u32 mean = 0;
    td_u32 coor_index, chn_index, h, w, j, num, count, num_temp, sum;
    td_u32 data[LSC_ARR_LEN] = { 0 };

    if (point->x < 0 || (td_u32)point->x >= rlsc_cali_cfg->width ||
        point->y < 0 || (td_u32)point->y >= rlsc_cali_cfg->height) {
        return mean;
    }

    x_start = (point->x - calc_cfg->window_size / 2); /* get half win size, divide by 2 */
    x_end   = (point->x + calc_cfg->window_size / 2); /* get half win size, divide by 2 */
    y_start = (point->y - calc_cfg->window_size / 2); /* get half win size, divide by 2 */
    y_end   = (point->y + calc_cfg->window_size / 2); /* get half win size, divide by 2 */

    if (x_start < 0) {
        x_start = 0;
    }
    if ((td_u32)x_end >= rlsc_cali_cfg->width) {
        x_end = rlsc_cali_cfg->width - 1;
    }
    if (y_start < 0) {
        y_start = 0;
    }
    if ((td_u32)y_end >= rlsc_cali_cfg->height) {
        y_end = rlsc_cali_cfg->height - 1;
    }

    chn_index = (rlsc_cali_cfg->bayer_format & 0x3) ^ calc_cfg->value;
    w = chn_index & 0x1;
    h = (chn_index >> 1) & 0x1;
    num = 0;
    for (y = y_start; y < y_end; y++) {
        for (x = x_start; x < x_end; x++) {
            diff_x = (x > begin_x) ? (x - begin_x) : (begin_x - x);
            diff_y = (y > begin_y) ? (y - begin_y) : (begin_y - y);
            square_radius = diff_x * diff_x + diff_y * diff_y;
            coor_index = x + y * rlsc_cali_cfg->stride;
            j = g_bayer_patt_lut[calc_cfg->value][(td_u32)y & 0x1][(td_u32)x & 0x1];

            if ((j == calc_cfg->value) && (square_radius <= calc_cfg->square_radius)) {
                data[num++] = (td_u32)clip3(((td_s32)bayer_img[coor_index + h * rlsc_cali_cfg->stride + w] -
                                             calc_cfg->true_blc_offset), 0, RLSC_MAX_PIXEL_VALUE);
            }
        }
    }
    sort(data, num);

    num_temp = 0;
    sum = 0;
    for (count = num / 10; count < num * 9 / 10; count++) { /* drop 10% of the bottom data, 9 for 90% of the data */
        sum += data[count];
        num_temp++;
    }
    if (num_temp) {
        mean = sum / num_temp;
    } else {
        mean = 0;
    }
    return mean;
}

static td_s32 get_rlsc_data_channel(const td_u16 *bayer_img, td_u32 *gain_data,
                                    const rlsc_calibration_cfg *rlsc_cali_cfg, td_u32 blc_offset, td_u8 value)
{
    if (bayer_img == TD_NULL || gain_data == TD_NULL || rlsc_cali_cfg == TD_NULL) {
        printf("%s: LINE: %d bayer_img/gain_data/rlsc_cali_cfg is TD_NULL pointer !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }
    td_s32 diff_x, diff_y, true_blc_offset;
    td_u32 square_sec_size, i, two_nodes_distance, temp;
    td_float sin_val, cos_val;

    ls_location center, delta, location1, location2, location3, location4, last_loc;
    rlsc_calc_cfg calc_cfg;

    td_u32 half_window_size = rlsc_cali_cfg->window_size >> 1;
    td_u32 last_window_size = half_window_size;
    td_u32 mean = 0;
    td_u32 count = 0;

    diff_x   = rlsc_cali_cfg->width >> 1;
    diff_y   = rlsc_cali_cfg->height >> 1;
    center.x = rlsc_cali_cfg->width >> 1;
    center.y = rlsc_cali_cfg->height >> 1;

    if (rlsc_cali_cfg->offset_in_en) {
        td_s32 bit_shift_signed = rlsc_cali_cfg->bit_depth - 12; /* compare blc to 12 bit depth */
        td_u32 bit_shift = ABS(bit_shift_signed);

        if (bit_shift_signed >= 0) {
            true_blc_offset = blc_offset << bit_shift;
        } else {
            true_blc_offset = blc_offset >> bit_shift;
        }
    } else {
        true_blc_offset = 0;
    }

    calc_cfg.window_size = half_window_size * 2; /* get full win size multiply by 2 */
    calc_cfg.value = value;
    calc_cfg.true_blc_offset = true_blc_offset;
    calc_cfg.square_radius = diff_x * diff_x + diff_y * diff_y;
    if (calc_cfg.square_radius < 131584) { /* 131584 is an empirical number for minimum square radius */
        printf("%s: LINE: %d radius is too small !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    square_sec_size = calc_cfg.square_radius / (RLSC_NODE_NUM - 1);
    sin_val = ((td_float)diff_y) / (td_float)sqrt((td_double)calc_cfg.square_radius);
    cos_val = ((td_float)diff_x) / (td_float)sqrt((td_double)calc_cfg.square_radius);
    last_loc.x = center.x;
    last_loc.y = center.y;

    gain_data[0] = get_point_mean_value(bayer_img, rlsc_cali_cfg, &center, &calc_cfg);
    for (i = 1; i < RLSC_NODE_NUM; i++) {
        delta.x = (td_u32)(cos_val * sqrt((td_float)(i * square_sec_size)));
        delta.y = (td_u32)(sin_val * sqrt((td_float)(i * square_sec_size)));
        location1.x = center.x + delta.x;
        location1.y = center.y + delta.y;
        two_nodes_distance = (td_u32)sqrt((td_float)(location1.x - last_loc.x) * (location1.x - last_loc.x) +
                                          (td_float)(location1.y - last_loc.y) * (location1.y - last_loc.y));
        if (last_window_size + half_window_size > two_nodes_distance && half_window_size > 3) { /* win distance > 3 */
            half_window_size--;
        }
        calc_cfg.window_size = half_window_size * 2; /* get full win size multiply by 2 */

        location2.x = center.x - delta.x;
        location2.y = center.y + delta.y;
        location3.x = center.x - delta.x;
        location3.y = center.y - delta.y;
        location4.x = center.x + delta.x;
        location4.y = center.y - delta.y;

        temp = get_point_mean_value(bayer_img, rlsc_cali_cfg, &location1, &calc_cfg);
        if (temp) {
            mean += temp;
            count++;
        }
        temp = get_point_mean_value(bayer_img, rlsc_cali_cfg, &location2, &calc_cfg);
        if (temp) {
            mean += temp;
            count++;
        }
        temp = get_point_mean_value(bayer_img, rlsc_cali_cfg, &location3, &calc_cfg);
        if (temp) {
            mean += temp;
            count++;
        }
        temp = get_point_mean_value(bayer_img, rlsc_cali_cfg, &location4, &calc_cfg);
        if (temp) {
            mean += temp;
            count++;
        }

        if (!count) {
            gain_data[i] = 0;
        } else {
            gain_data[i] = mean / count;
        }

        last_window_size = half_window_size;
        last_loc.x = location1.x;
        last_loc.y = location1.y;
        mean = 0;
        count = 0;
        temp = 0;
    }

    return TD_SUCCESS;
}

static td_s32 get_rlsc_data(const td_u16 *bayer_img, rlsc_calib_gain *rlsc_data,
    const rlsc_calibration_cfg *rlsc_cali_cfg)
{
    td_s32 ret;

    ret = get_rlsc_data_channel(bayer_img, rlsc_data->r_gain, rlsc_cali_cfg, rlsc_cali_cfg->blc_offset_r, BPRG);
    if (ret != TD_SUCCESS) {
        printf("%s: LINE: %d get_lsc_data of R channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    ret = get_rlsc_data_channel(bayer_img, rlsc_data->gr_gain, rlsc_cali_cfg, rlsc_cali_cfg->blc_offset_gr, BPGR);
    if (ret != TD_SUCCESS) {
        printf("%s: LINE: %d get_lsc_data of gr channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    ret = get_rlsc_data_channel(bayer_img, rlsc_data->gb_gain, rlsc_cali_cfg, rlsc_cali_cfg->blc_offset_gb, BPGB);
    if (ret != TD_SUCCESS) {
        printf("%s: LINE: %d get_lsc_data of gb channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    ret = get_rlsc_data_channel(bayer_img, rlsc_data->b_gain, rlsc_cali_cfg, rlsc_cali_cfg->blc_offset_b, BPBG);
    if (ret != TD_SUCCESS) {
        printf("%s: LINE: %d get_lsc_data of B channel failure !\n", __FUNCTION__, __LINE__);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_void rlsc_normalization(const rlsc_calib_gain *rlsc_data, rlsc_calib_gain *rlsc_gain,
                                  const rlsc_calibration_cfg *rlsc_cali_cfg)
{
    td_u32 r_max_data, gr_max_data, gb_max_data, b_max_data;
    td_u32 node_num;
    td_u32 i;
    td_u8  scale = rlsc_cali_cfg->gain_scale;

    node_num = RLSC_NODE_NUM;

    r_max_data = get_max_data(rlsc_data->r_gain, node_num);
    gr_max_data = get_max_data(rlsc_data->gr_gain, node_num);
    gb_max_data = get_max_data(rlsc_data->gb_gain, node_num);
    b_max_data = get_max_data(rlsc_data->b_gain, node_num);

    for (i = 0; i < node_num; i++) {
        rlsc_gain->r_gain[i] = rlsc_data_enlarge(rlsc_data->r_gain[i], r_max_data, scale);
        rlsc_gain->gr_gain[i] = rlsc_data_enlarge(rlsc_data->gr_gain[i], gr_max_data, scale);
        rlsc_gain->gb_gain[i] = rlsc_data_enlarge(rlsc_data->gb_gain[i], gb_max_data, scale);
        rlsc_gain->b_gain[i] = rlsc_data_enlarge(rlsc_data->b_gain[i], b_max_data, scale);
    }
    return;
}

static td_s32 rlsc_generate_gain_info(const td_u16 *data, const rlsc_calibration_cfg *rlsc_cali_cfg,
                                      ot_isp_mesh_shading_table *mlsc_table)
{
    rlsc_calib_gain *rlsc_data = TD_NULL;
    rlsc_calib_gain *rlsc_gain = TD_NULL;
    td_s32 ret;
    td_u32 i;

    rlsc_data = (rlsc_calib_gain *)isp_malloc(sizeof(rlsc_calib_gain));
    if (rlsc_data == TD_NULL) {
        isp_err_trace("rlsc_data allocation fail!\n");
        return TD_FAILURE;
    }
    rlsc_gain = (rlsc_calib_gain *)isp_malloc(sizeof(rlsc_calib_gain));
    if (rlsc_gain == TD_NULL) {
        isp_err_trace("rlsc_gain allocation fail!\n");
        isp_free(rlsc_data);
        return TD_FAILURE;
    }
    (td_void)memset_s(rlsc_data, sizeof(rlsc_calib_gain), 0, sizeof(rlsc_calib_gain));
    (td_void)memset_s(rlsc_gain, sizeof(rlsc_calib_gain), 0, sizeof(rlsc_calib_gain));

    ret = get_rlsc_data(data, rlsc_data, rlsc_cali_cfg);
    if (ret != TD_SUCCESS) {
        isp_err_trace("get rlsc data fails!\n");
        isp_free(rlsc_gain);
        isp_free(rlsc_data);
        return TD_FAILURE;
    }

    rlsc_normalization(rlsc_data, rlsc_gain, rlsc_cali_cfg);

    for (i = 0; i < RLSC_NODE_NUM; i++) {
        mlsc_table->bnr_lsc_gain_lut.r_gain[i] = rlsc_gain->r_gain[i];
        mlsc_table->bnr_lsc_gain_lut.gr_gain[i] = rlsc_gain->gr_gain[i];
        mlsc_table->bnr_lsc_gain_lut.gb_gain[i] = rlsc_gain->gb_gain[i];
        mlsc_table->bnr_lsc_gain_lut.b_gain[i] = rlsc_gain->b_gain[i];
    }

    isp_free(rlsc_gain);
    isp_free(rlsc_data);

    return TD_SUCCESS;
}

static td_void rlsc_cali_cfg_init(const ot_isp_mlsc_calibration_cfg *mlsc_cali_cfg, rlsc_calibration_cfg *rlsc_cali_cfg)
{
    rlsc_cali_cfg->height        = mlsc_cali_cfg->dst_img_height;
    rlsc_cali_cfg->width         = mlsc_cali_cfg->dst_img_width;
    rlsc_cali_cfg->stride        = mlsc_cali_cfg->dst_img_width;
    rlsc_cali_cfg->begin_pos_x   = mlsc_cali_cfg->dst_img_width >> 1;
    rlsc_cali_cfg->begin_pos_y   = mlsc_cali_cfg->dst_img_height >> 1;
    rlsc_cali_cfg->end_pos_x     = mlsc_cali_cfg->dst_img_width;
    rlsc_cali_cfg->end_pos_y     = mlsc_cali_cfg->dst_img_height;
    rlsc_cali_cfg->window_size   = 20; /* default window size 20 */
    rlsc_cali_cfg->eliminate_pct = 10; /* default 10 to eliminate 10% of the top/bottom values */
    rlsc_cali_cfg->gain_scale    = 3;  /* default gain scale 3 */
    rlsc_cali_cfg->bit_depth     = mlsc_cali_cfg->raw_bit;
    rlsc_cali_cfg->bayer_format  = mlsc_cali_cfg->bayer;
    rlsc_cali_cfg->offset_in_en  = TD_TRUE;
    rlsc_cali_cfg->blc_offset_r  = mlsc_cali_cfg->blc_offset_r;
    rlsc_cali_cfg->blc_offset_gr = mlsc_cali_cfg->blc_offset_gr;
    rlsc_cali_cfg->blc_offset_gb = mlsc_cali_cfg->blc_offset_gb;
    rlsc_cali_cfg->blc_offset_b  = mlsc_cali_cfg->blc_offset_b;
}

/*
 * [isp_mesh_shading_calibration description]
 * @param src_raw       [input]  input raw data that used for calibration
 * @param mlsc_cali_cfg [input]  input mesh_lsc calibration config
 * @param mlsc_table    [output] mesh_lsc calibration results
 */
td_s32 isp_mesh_shading_calibration(const td_u16 *src_raw,
                                    ot_isp_mlsc_calibration_cfg *mlsc_cali_cfg,
                                    ot_isp_mesh_shading_table *mlsc_table)
{
    td_s32 ret, ret1;
    td_s32 i, j;
    td_phys_addr_t phy_addr = 0;
    td_u64 size;
    td_u16 *crop_raw = TD_NULL;

    isp_lsc_grid_step lsc_grid_step_xy;
    rlsc_calibration_cfg rlsc_cali_cfg;
    rlsc_cali_cfg_init(mlsc_cali_cfg, &rlsc_cali_cfg);

    size = sizeof(td_u16) * mlsc_cali_cfg->dst_img_width * mlsc_cali_cfg->dst_img_height;
    ret = ot_mpi_sys_mmz_alloc(&phy_addr, (td_void **)(&crop_raw), "mlsc_calib", TD_NULL, size);
    if (ret != TD_SUCCESS) {
        isp_err_trace("malloc memory failed!\n");
        return TD_FAILURE;
    }
    (td_void)memset_s(crop_raw, sizeof(td_u16) * mlsc_cali_cfg->dst_img_width * mlsc_cali_cfg->dst_img_height,
                      0, sizeof(td_u16) * mlsc_cali_cfg->dst_img_width * mlsc_cali_cfg->dst_img_height);

    for (j = 0; j < mlsc_cali_cfg->dst_img_height; j++) {
        for (i = 0; i < mlsc_cali_cfg->dst_img_width; i++) {
            *(crop_raw + j * mlsc_cali_cfg->dst_img_width + i) = *(src_raw +
                (j + mlsc_cali_cfg->offset_y) * mlsc_cali_cfg->img_width + i + mlsc_cali_cfg->offset_x);
        }
    }

    /* get grid info */
    geometric_grid_size_x(mlsc_cali_cfg, &lsc_grid_step_xy, mlsc_table);

    geometric_grid_size_y(mlsc_cali_cfg, &lsc_grid_step_xy, mlsc_table);

    /* malloc memory */
    ret = lsc_generate_grid_info(crop_raw, mlsc_cali_cfg, &lsc_grid_step_xy, mlsc_table);
    if (ret == TD_FAILURE) {
        isp_err_trace("there are some errors in lsc_generate_grid_info()!\n");
        goto end;
    }

    ret = rlsc_generate_gain_info(crop_raw, &rlsc_cali_cfg, mlsc_table);
    if (ret == TD_FAILURE) {
        isp_err_trace("there are some errors in rlsc_generate_gain_info()!\n");
        goto end;
    }

end:
    ret1 = ot_mpi_sys_mmz_free(phy_addr, (td_void *)crop_raw);
    if (ret1 != TD_SUCCESS) {
        isp_err_trace("ot_mpi_sys_mmz_free err!\n");
        return ret1;
    }

    return ret;
}

