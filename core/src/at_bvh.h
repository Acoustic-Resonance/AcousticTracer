#ifndef AT_BVH_H
#define AT_BVH_H

#include "acoustic/at.h"

#include <stdbool.h>
#include <stdint.h>

typedef unsigned int *AT_TriArray;
typedef struct {
    AT_Triangle *triangles_db;
    AT_TriArray *arrs; // 4 arrays, 0 - 2 are dimensional (x, y, z), and 3 is unsorted triangles
} AT_TriangleArrays;

typedef struct {
    AT_TriangleArrays *triangle_arrs;
    uint32_t start, num_tri;
    AT_AABB aabb;
} AT_TriGroup;

typedef struct {
    AT_TriGroup **groups;
    uint32_t num_groups;
} AT_TriangleGroups;

typedef struct {
    uint32_t mini_tree_size;
    float traversal_cost, intersection_cost;
} AT_BVHConfig;

typedef struct {
    AT_AABB aabb;
    int idx, left_child, right_child;
    AT_TriangleArrays *triangle_arrs;
    uint32_t start, num_tri;
    // TODO: add parent index
} AT_BVHNode;

typedef struct {
    AT_BVHNode *nodes;
    uint32_t max_node_count;
    uint32_t last_node_idx;
} AT_BVH;

typedef struct {
    int axis;
    uint32_t left_n;
} AT_SplitContext;

typedef struct {
    uint32_t spatial;
    uint32_t object;
} AT_Medians;

typedef struct {
    float left_area, right_area;
} AT_SA;

typedef bool (*AT_CompareFunc)(AT_Vec3, AT_SplitContext *);

AT_Result AT_triangle_arrays_create(AT_TriangleArrays **out_arrs, const AT_Model *model);
void AT_triangle_arrays_destroy(AT_TriangleArrays *triangle_arrs);

AT_Result AT_BVH_create(AT_BVH **out_tree, const AT_TriGroup *tri_group, const AT_BVHConfig *conf);
AT_Result AT_BVHNode_init(AT_BVH *tree, const AT_BVHNode *nodes, AT_TriangleArrays *triangle_arrs, uint32_t start, uint32_t num_tri, int index);
void AT_BVH_destroy(AT_BVH *tree);

void AT_BVH_sort_triangles(AT_TriangleArrays *triangle_arrs, uint32_t num_tri);
AT_Result AT_BVH_partition_list(AT_TriangleArrays *triangle_arrs, int array_idx, uint32_t start, uint32_t num_tri, AT_SplitContext *ctx);

AT_Medians AT_BVH_get_median_range(const AT_BVHNode *node, int axis);
float AT_BVH_get_SAH(const AT_BVHNode *node, const AT_BVHConfig *conf, uint32_t split_idx, int axis);
AT_SplitContext AT_BVH_get_optimal_split(const AT_BVHNode *node, const AT_BVHConfig *conf);
AT_Result AT_BVH_split(AT_BVH *tree, const AT_BVHConfig *conf);

// TODO: bvh pruning
// TODO: bvh merge
// TODO: bvh traversal

#endif // AT_BVH_H
