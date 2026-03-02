#include "../src/at_bvh.h"
#include "../src/at_aabb.h"
#include "../src/at_utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AT_Result AT_triangle_arrays_create(AT_TriangleArrays **out_arrs, const AT_Model *model)
{
    if (!out_arrs || *out_arrs || !model) return AT_ERR_INVALID_ARGUMENT;

    AT_TriangleArrays *tri_arrs = malloc(sizeof(*tri_arrs));
    if (!tri_arrs) return AT_ERR_ALLOC_ERROR;

    AT_Result res = AT_model_get_triangles(&tri_arrs->triangles_db, model);
    if (res != AT_OK) {
        free(tri_arrs);
        return res;
    }

    tri_arrs->arrs = malloc(sizeof(*tri_arrs->arrs) * 4);
    uint32_t num_tri = model->index_count / 3;
    for (int i = 0; i < 4; i++) {
        tri_arrs->arrs[i] = malloc(sizeof(*tri_arrs->arrs[i]) * num_tri);
        if (!tri_arrs->arrs[i]) {
            for (int j = i - 1; j >= 0; j--) {
                free(tri_arrs->arrs[j]);
            }
            free(tri_arrs->arrs);
            free(tri_arrs);
            return AT_ERR_ALLOC_ERROR;
        }
    }

    for (uint32_t i = 0; i < num_tri; i++) {
        tri_arrs->arrs[0][i] = i;
        tri_arrs->arrs[1][i] = i;
        tri_arrs->arrs[2][i] = i;
        tri_arrs->arrs[3][i] = i;
    }

    AT_BVH_sort_triangles(tri_arrs, num_tri);

    *out_arrs = tri_arrs;

    return AT_OK;
}
void AT_triangle_arrays_destroy(AT_TriangleArrays *triangle_arrs)
{
    if (!triangle_arrs) return;

    for (int i = 0; i < 4; i++) {
        free(triangle_arrs->arrs[i]);
    }
    free(triangle_arrs->arrs);
    free(triangle_arrs->triangles_db);
    free(triangle_arrs);
}

AT_Result AT_BVH_create(AT_BVH **out_tree, const AT_TriGroup *tri_group)
{
    if (!out_tree || *out_tree) return AT_ERR_INVALID_ARGUMENT;

    AT_BVH *bvh = malloc(sizeof(*bvh));
    if (!bvh) return AT_ERR_ALLOC_ERROR;
    bvh->nodes = malloc(sizeof(*bvh->nodes) * ((2 * tri_group->num_tri) - 1));
    if (!bvh->nodes) return AT_ERR_ALLOC_ERROR;
    bvh->nodes[0] = create_node(tri_group->triangles, tri_group->num_tri, tri_group->aabb, 0);

    *out_tree = bvh;
    return AT_OK;
}

void AT_BVH_destroy(AT_BVH *tree)
{
    free(tree->nodes);
    free(tree);
}

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

unsigned char get_nth_byte(float num, int n)
{
    int offset = 8 * n;
    return (flt_to_int(num) & (0xFF << offset)) >> offset;
}

void count_sort(AT_Triangle *in_buf, int cur_byte, uint32_t num_tri, AT_Triangle *out_buf, int dim)
{
    int counts[256] = {0};
    int offsets[256] = {0};

    bool sorted = true;
    for (uint32_t i = 0; i < num_tri; i++) {
        if (
            (i > 0) &&
            in_buf[i].aabb.midpoint.arr[dim] < in_buf[i - 1].aabb.midpoint.arr[dim]
        ) {
            sorted = false;
        }
        AT_Vec3 midpoint = in_buf[i].aabb.midpoint;
        counts[get_nth_byte(midpoint.arr[dim], cur_byte)]++;
    }
    if (sorted) {
        memcpy(out_buf, in_buf, sizeof(*in_buf) * num_tri);
        return;
    }

    for (int i = 1; i < 256; i++) {
        offsets[i] = offsets[i - 1] + counts[i - 1];
    }

    for (uint32_t i = 0; i < num_tri; i++) {
        AT_Triangle triangle = in_buf[i];
        AT_Vec3 midpoint = triangle.aabb.midpoint;
        unsigned char byte = get_nth_byte(midpoint.arr[dim], cur_byte);
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

AT_Medians AT_BVH_get_median_range(AT_Triangle *triangles, uint32_t num_tri, int axis)
{
    // Object split
    AT_Medians median = {
        .object = AT_max(0, (num_tri / 2) - 1),
        .spatial = UINT32_MAX,
    };

    // Spatial split
    float threshold = (triangles[num_tri - 1].aabb.midpoint.arr[axis] - triangles[0].aabb.midpoint.arr[axis]) * 0.5f;
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

AT_SA get_node_SA(AT_BVHNode *node, AT_Triangle *triangles, uint32_t split_idx, uint32_t num_tri)
{
    AT_SA area;

    AT_AABB left_aabb = AT_AABB_init();
    AT_AABB_grow(&left_aabb, node->aabb.min);
    AT_AABB right_aabb = AT_AABB_init();
    right_aabb.max = node->aabb.max;
    // TODO: check if split index is num_tri
    for (uint32_t i = 0; i < split_idx + 1; i++) {
        AT_AABB_grow(&left_aabb, triangles[i].aabb.max);
    }
    for (uint32_t i = split_idx + 1; i < num_tri; i++) {
        AT_AABB_grow(&right_aabb, triangles[i].aabb.max);
    }
    area.left_area = AT_AABB_get_SA(left_aabb);
    area.right_area = AT_AABB_get_SA(right_aabb);

    return area;
}

float AT_BVH_get_SAH(const AT_BVHNode *node, const AT_BVHConfig *conf, uint32_t split_idx, int axis)
{
    // SAH(tree) = c_t + c_i((SA(left) / SA(tree)) * N(left) + (SA(right) / SA(tree) * N(right)))
    AT_SA areas = get_node_SA(node, axis, split_idx);
    float tree_SA = 1 / node->aabb.SA;
    uint32_t left_n = split_idx;
    uint32_t right_n = node->num_tri - left_n;
    float c_t = conf->traversal_cost;
    float c_i = conf->intersection_cost;

    float left_cost = (areas.left_area * tree_SA) * left_n;
    float right_cost = (areas.right_area * tree_SA) * right_n;

    return c_t + c_i * (left_cost + right_cost);
}

uint32_t AT_BVH_get_optimal_split(const AT_BVH *tree, const AT_BVHConfig *conf)
{
    AT_BVHNode root = tree->nodes[0];
    float no_split_cost = root.aabb.SA * root.num_tri;
    float split_cost = FLT_MAX;
    uint32_t split_idx;
    int axis = 0;
    AT_Medians medians = AT_BVH_get_median_range(root.triangles, root.num_tri, axis);
    uint32_t start = AT_min(medians.object, medians.spatial);
    uint32_t end = AT_max(medians.object, medians.spatial);
    for (uint32_t i = start; i < end; i++) {
        float new_cost = AT_BVH_get_SAH(tree, conf, i);
        if (new_cost < split_cost) {
            split_cost = new_cost;
            split_idx = i;
        }
    }

    if (no_split_cost < split_cost) {
        return root.num_tri;
    }

    return split_idx;
}
