#include "../src/at_bvh.h"
#include "../src/at_aabb.h"
#include "../src/at_internal.h"
#include "../src/at_utils.h"
#include "acoustic/at_model.h"

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

AT_AABB get_node_aabb(const AT_TriangleArrays *triangle_arrs, uint32_t axis, uint32_t start, uint32_t num_tri)
{
    AT_AABB aabb = AT_AABB_init();
    for (uint32_t i = 0; i < num_tri; i++) {
        AT_AABB_grow(&aabb, AT_get_triangle_by_arr(start, axis, i).aabb.min);
        AT_AABB_grow(&aabb, AT_get_triangle_by_arr(start, axis, i).aabb.max);
    }

    return aabb;
}

AT_Result AT_BVHNode_init(AT_BVH *tree, const AT_BVHNode *nodes, AT_TriangleArrays *triangles_arrs, uint32_t start, uint32_t num_tri, int index)
{
    if (!nodes) {
        return AT_ERR_INVALID_ARGUMENT;
    }
    AT_BVHNode *node = &nodes[index];
    node->idx = index;
    if (num_tri == 1) {
        node->left_child = -1;
        node->right_child = -1;
        // TODO: reduce last node index to fix loops
        // TODO: grow aabb to encompass full triangles
    } else {
        node->left_child = ++tree->last_node_idx;
        node->right_child = ++tree->last_node_idx;
    }
    node->start = start;
    node->num_tri = num_tri;
    node->triangle_arrs = triangles_arrs;

    node->aabb = get_node_aabb(node->triangle_arrs, 3, node->start, num_tri);

    return AT_OK;
}

AT_Result AT_BVH_create(AT_BVH **out_tree, const AT_TriGroup *tri_group, const AT_BVHConfig *conf)
{
    if (!out_tree || *out_tree) return AT_ERR_INVALID_ARGUMENT;

    AT_BVH *bvh = malloc(sizeof(*bvh));
    if (!bvh) return AT_ERR_ALLOC_ERROR;
    bvh->max_node_count = (2 * tri_group->num_tri) - 1;
    bvh->last_node_idx = 0;
    bvh->nodes = malloc(sizeof(*bvh->nodes) * bvh->max_node_count);
    if (!bvh->nodes) {
        free(bvh);
        return AT_ERR_ALLOC_ERROR;
    }
    AT_BVHNode_init(bvh, bvh->nodes, tri_group->triangle_arrs, tri_group->start, tri_group->num_tri, 0);

    AT_Result res = AT_BVH_split(bvh, conf);
    if (res != AT_OK) {
        perror("Failed to split BVH");
        free(bvh->nodes);
        free(bvh);
        return res;
    }

    *out_tree = bvh;
    return AT_OK;
}

void AT_BVH_destroy(AT_BVH *tree)
{
    if (!tree) return;

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
    bool is_neg = signbit((float)num);
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

void count_sort(AT_TriangleArrays *triangle_arrs, AT_TriArray in_buf, int cur_byte, uint32_t num_tri, AT_TriArray out_buf, int dim)
{
    int counts[256] = {0};
    int offsets[256] = {0};

    bool sorted = true;
    AT_Triangle *triangles_db = triangle_arrs->triangles_db;
    for (uint32_t i = 0; i < num_tri; i++) {
        AT_Triangle cur_triangle = triangles_db[in_buf[i]];
        if (
            (i > 0) &&
            cur_triangle.aabb.midpoint.arr[dim] < triangles_db[in_buf[i - 1]].aabb.midpoint.arr[dim]
        ) {
            sorted = false;
        }
        AT_Vec3 midpoint = cur_triangle.aabb.midpoint;
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
        AT_Triangle triangle = triangles_db[in_buf[i]];
        AT_Vec3 midpoint = triangle.aabb.midpoint;
        unsigned char byte = get_nth_byte(midpoint.arr[dim], cur_byte);
        out_buf[offsets[byte]++] = in_buf[i];
    }
}

void AT_BVH_sort_triangles(AT_TriangleArrays *triangles_arrs, uint32_t num_tri)
{
    AT_TriArray tmp_buf = malloc(sizeof(*tmp_buf) * num_tri);
    AT_TriArray res_buf = malloc(sizeof(*tmp_buf) * num_tri);
    for (uint32_t i = 0; i < num_tri; i++) {
        tmp_buf[i] = i;
        res_buf[i] = i;
    }
    AT_TriArray tmp;

    for (int dim = 0; dim < 3; dim++) {
        for (int i = 0; i < 4; i++) { // byte loop
            count_sort(triangles_arrs, tmp_buf, i, num_tri, res_buf, dim);
            if (i < 3) {
                tmp = tmp_buf;
                tmp_buf = res_buf;
                res_buf = tmp;
            }
        }
        memcpy(triangles_arrs->arrs[dim], res_buf, sizeof(*triangles_arrs->arrs[dim]) * num_tri);
    }

    free(tmp_buf);
    free(res_buf);
}

AT_Result AT_BVH_partition_list(AT_TriangleArrays *triangle_arrs, int array_idx, uint32_t start, uint32_t num_tri, AT_SplitContext *ctx)
{
    uint32_t left = 0, right = 0;
    // TODO: check for malloc error
    AT_TriArray left_tmp_buf = malloc(sizeof(*left_tmp_buf) * num_tri);
    AT_TriArray right_tmp_buf = malloc(sizeof(*right_tmp_buf) * num_tri);
    if (!left_tmp_buf || !right_tmp_buf) {
        if (!right_tmp_buf) free(left_tmp_buf);
        return AT_ERR_ALLOC_ERROR;
    }
    for (uint32_t i = 0; i < num_tri; i++) {
        bool is_left = AT_get_triangle_by_arr(start, array_idx, i).left;
        if (is_left && left < ctx->left_n) {
            left_tmp_buf[left++] = triangle_arrs->arrs[array_idx][start + i];
        } else {
            right_tmp_buf[right++] = triangle_arrs->arrs[array_idx][start + i];
        }
    }

    assert(left + right == num_tri);
    assert(left == ctx->left_n);

    memcpy(&triangle_arrs->arrs[array_idx][start], left_tmp_buf, sizeof(*left_tmp_buf) * left);
    memcpy(&triangle_arrs->arrs[array_idx][start + left], right_tmp_buf, sizeof(*right_tmp_buf) * right);

    free(left_tmp_buf);
    free(right_tmp_buf);

    return AT_OK;
}

AT_Medians AT_BVH_get_median_range(const AT_BVHNode *node, int axis)
{
    uint32_t num_tri = node->num_tri;
    // Object split
    AT_Medians median = {
        .object = (num_tri > 1) ? (num_tri / 2) - 1 : 0,
        .spatial = UINT32_MAX,
    };

    // Spatial split
    float threshold = node->aabb.midpoint.arr[axis];
    uint32_t low = 0, mid = 0;
    uint32_t high = num_tri - 1;
    float min_dist = FLT_MAX;
    while (low <= high) {
        mid = low + (high - low) / 2;
        float dist = fabsf(AT_get_triangle(node, axis, mid).aabb.midpoint.arr[axis] - threshold);

        if (dist < min_dist) {
            min_dist = dist;
            median.spatial = mid;
        }

        float midpoint = AT_get_triangle(node, axis, mid).aabb.midpoint.arr[axis];
        if (midpoint < threshold) {
            low = mid + 1;
        } else if (midpoint > threshold) {
            if (mid == 0) break;
            high = mid - 1;
        } else {
            median.spatial = mid;
            break;
        }
    }

    return median;
}

AT_SA get_node_SA(const AT_BVHNode *node, int axis, uint32_t split_idx)
{
    AT_SA area;

    AT_AABB left_aabb = get_node_aabb(node->triangle_arrs, axis, node->start, split_idx);
    AT_AABB right_aabb = get_node_aabb(node->triangle_arrs, axis, split_idx, node->num_tri - split_idx);
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

AT_SplitContext AT_BVH_get_optimal_split(const AT_BVHNode *node, const AT_BVHConfig *conf)
{
    float no_split_cost = node->aabb.SA * node->num_tri;
    float split_cost = no_split_cost;
    float new_cost;
    uint32_t left_n = node->num_tri;
    int dim = 0;
    for (int axis = 0; axis < 3; axis++) {
        AT_Medians medians = AT_BVH_get_median_range(node, axis);
        uint32_t start = AT_min(medians.object, medians.spatial);
        uint32_t end = AT_max(medians.object, medians.spatial);
        for (uint32_t i = start; i < end + 1; i++) {
            new_cost = AT_BVH_get_SAH(node, conf, i, axis);
            if (new_cost < split_cost) {
                split_cost = new_cost;
                left_n = i + 1;
                dim = axis;
            }
        }
    }

    if (left_n >= node->num_tri) {
        return (AT_SplitContext){
            .left_n = left_n
        };
    }

    for (uint32_t i = 0; i < node->num_tri; i++) {
        if (i < left_n) {
            (&AT_get_triangle(node, dim, i))->left = 1;
        } else {
            (&AT_get_triangle(node, dim, i))->left = 0;
        }
    }
    return (AT_SplitContext){
        .axis = dim,
        .left_n = left_n,
    };
}

AT_Result AT_BVH_split(AT_BVH *tree, const AT_BVHConfig *conf)
{
    if (!tree || !conf) return AT_ERR_INVALID_ARGUMENT;

    AT_BVHNode *root = &tree->nodes[0];
    if (root->num_tri == 1) {
        root->left_child = -1;
        root->right_child = -1;
        return AT_OK;
    }

    AT_BVHNode *stack[sizeof(tree->nodes) * tree->max_node_count];
    int stack_top = 0;
    stack[stack_top++] = root;
    AT_BVHNode *parent;
    int left, right;
    while (stack_top > 0) {
        parent = stack[--stack_top];
        left = parent->left_child;
        right = parent->right_child;
        AT_SplitContext split_ctx = AT_BVH_get_optimal_split(parent, conf);

        // Skip if not worth splitting
        if (split_ctx.left_n >= parent->num_tri) {
            parent->left_child = -1;
            parent->right_child = -1;
            // TODO: reduce last node index to fix loops
            // TODO: grow aabb back to full size
            continue;
        }

        if (AT_BVH_partition_list(parent->triangle_arrs, 3, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
            return AT_ERR_ALLOC_ERROR;
        }

        // TODO: figure out how to remove if statements
        // i + 1 % 3
        // 3 - 2 = 1 + 2 % 2
        // 3 - 1 = 2 + 3 % 1
        // 3 - 0 = 3 + 4 % 1
        if (split_ctx.axis == 0) {
            if (AT_BVH_partition_list(parent->triangle_arrs, 1, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
                return AT_ERR_ALLOC_ERROR;
            }
            if (AT_BVH_partition_list(parent->triangle_arrs, 2, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
                return AT_ERR_ALLOC_ERROR;
            }
        } else if (split_ctx.axis == 1) {
            if (AT_BVH_partition_list(parent->triangle_arrs, 0, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
                return AT_ERR_ALLOC_ERROR;
            }
            if (AT_BVH_partition_list(parent->triangle_arrs, 2, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
                return AT_ERR_ALLOC_ERROR;
            }
        } else {
            if (AT_BVH_partition_list(parent->triangle_arrs, 0, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
                return AT_ERR_ALLOC_ERROR;
            }
            if (AT_BVH_partition_list(parent->triangle_arrs, 1, parent->start, parent->num_tri, &split_ctx) != AT_OK) {
                return AT_ERR_ALLOC_ERROR;
            }
        }

        // Create children
        AT_BVHNode_init(tree, tree->nodes, parent->triangle_arrs, parent->start + 0, split_ctx.left_n, left);
        AT_BVHNode_init(tree, tree->nodes, parent->triangle_arrs, parent->start + split_ctx.left_n, parent->num_tri - split_ctx.left_n, right);

        // Add if not leaf
        if (tree->nodes[left].num_tri > 1) {
            stack[stack_top++] = &tree->nodes[left];
        }
        if (tree->nodes[right].num_tri > 1) {
            stack[stack_top++] = &tree->nodes[right];
        }
    }

    return AT_OK;
}
