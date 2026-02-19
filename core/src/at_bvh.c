#include "../src/at_bvh.h"
#include "../src/at_utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;

uint32_t flt_to_int(float num)
{
    uint32_t res = *(int *)&num;
    bool is_neg = signbit(num);
    if (is_neg) {
        res ^= 0xFFFFFFFF;
    } else {
        res ^= 0x80000000;
    }

    return res;
}

float int_to_flt(uint32_t num)
{
    bool is_neg = signbit(num);
    if (is_neg) {
        num ^= 0x80000000;
    } else {
        num ^= 0xFFFFFFFF;
    }

    return *(float *)&num;
}

uchar get_nth_byte(float num, int n)
{
    int offset = 8 * n;
    return (flt_to_int(num) & (0xFF << offset)) >> offset;
}

void count_sort(AT_Triangle *in_buf, int cur_byte, uint32_t num_tri, AT_Triangle *out_buf, int dim)
{
    int counts[256] = {0};
    int offsets[256] = {0};

    for (uint32_t i = 0; i < num_tri; i++) {
        // TODO: check if array is sorted
        AT_Vec3 midpoint = in_buf[i].aabb.midpoint;
        counts[get_nth_byte(midpoint.arr[dim], cur_byte)]++;
    }

    for (int i = 1; i < 256; i++) {
        offsets[i] = offsets[i - 1] + counts[i - 1];
    }

    for (uint32_t i = 0; i < num_tri; i++) {
        AT_Triangle triangle = in_buf[i];
        AT_Vec3 midpoint = triangle.aabb.midpoint;
        uchar byte = get_nth_byte(midpoint.arr[dim], cur_byte);
        out_buf[offsets[byte]++] = triangle;
    }
}

void AT_BVH_sort_triangles(AT_Triangle *triangles, uint32_t num_tri, AT_Triangle **dim_arrs)
{
    AT_Triangle *tmp_buf = triangles;
    AT_Triangle *res_buf = malloc(sizeof(*res_buf) * num_tri);
    AT_Triangle *tmp;
    for (int dim = 0; dim < 3; dim++) {
        for (int i = 0; i < 4; i++) {
            count_sort(tmp_buf, i, num_tri, res_buf, dim);
            if (i < 3) {
                tmp = tmp_buf;
                tmp_buf = res_buf;
                res_buf = tmp;
            }
        }
        memcpy(dim_arrs[dim], res_buf, sizeof(AT_Triangle) * num_tri);
    }

    free(tmp_buf);
}

void AT_BVH_partition_list(AT_Triangle *triangles, uint32_t num_tri, AT_SplitContext *ctx)
{
    uint32_t left = 0, right = ctx->left_n;
    AT_Triangle *tmp_buf = malloc(sizeof(*tmp_buf) * num_tri);
    for (uint32_t i = 0; i < num_tri; i++) {
        bool is_left = triangles[i].aabb.midpoint.arr[ctx->axis] < ctx->threshold;
        if (is_left) {
            tmp_buf[left++] = triangles[i];
        } else {
            tmp_buf[right++] = triangles[i];
        }
    }

    memcpy(triangles, tmp_buf, sizeof(*tmp_buf) * num_tri);
    free(tmp_buf);
}

AT_Medians AT_BVH_get_median_range(AT_Triangle *triangles, uint32_t num_tri, AT_SplitContext *ctx)
{
    // Object split
    AT_Medians median = {
        .object = AT_max(0, (num_tri / 2) - 1),
        .spatial = UINT32_MAX,
    };

    // Spatial split
    int axis = ctx->axis;
    float threshold = ctx->threshold;
    int start, end;
    float prev = fabsf(threshold - triangles[median.object].aabb.midpoint.arr[axis]);
    // Start in middle
    // if threshold is larger then go right, else go left
    if (threshold >= triangles[median.object].aabb.midpoint.arr[axis]) {
        start = median.object + 1;
        end = num_tri;
        median.spatial = num_tri - 1;
    } else {
        start = -(median.object - 1);
        end = -(-1);
        median.spatial = 0;
    }
    float dist;
    for (int i = start; i < end; i++) {
        dist = fabsf(threshold - triangles[abs(i)].aabb.midpoint.arr[axis]);
        // keep going until distance is larger
        if (dist < prev) {
            prev = dist;
        } else {
            median.spatial = abs(i - 1);
            break;
        }
    }

    return median;
}
