#ifndef AT_BVH_H
#define AT_BVH_H

#include "acoustic/at.h"
#include "at_internal.h"

#include <stdbool.h>
#include <stdint.h>

typedef unsigned int *AT_TriArray;
struct AT_TriangleArrays {
    AT_Triangle *triangles_db;
    AT_TriArray *arrs; // 4 arrays, 0 - 2 are dimensional (x, y, z), and 3 is unsorted triangles
};

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
} AT_MiniTreeNode;

struct AT_MiniTree {
    AT_MiniTreeNode *nodes;
    uint32_t max_node_count;
    uint32_t last_node_idx;
};

typedef struct {
    AT_Vec3 centroid;
    AT_AABB root_aabb;
    AT_MiniTree *mini_tree;
} AT_MiniTreeInstance;

typedef struct {
    int idx, left_child, right_child;
    AT_AABB aabb;
    AT_MiniTreeInstance *mini_tree;
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

typedef struct {
    bool intersects;
    AT_Ray out_ray;
    AT_Vec3 out_normal;
    // TODO: check if we need triangles
    uint32_t triangle_index;
} AT_IntersectContext;

typedef bool (*AT_CompareFunc)(AT_Vec3, AT_SplitContext *);

AT_Result AT_triangle_arrays_create(AT_TriangleArrays **out_arrs, const AT_Model *model);
void AT_triangle_arrays_destroy(AT_TriangleArrays *triangle_arrs);

AT_Result AT_MiniTree_create(AT_MiniTree **out_tree, const AT_TriGroup *tri_group, const AT_BVHConfig *conf);
AT_Result AT_MiniTreeNode_init(AT_MiniTree *tree, const AT_MiniTreeNode *nodes, AT_TriangleArrays *triangle_arrs, uint32_t start, uint32_t num_tri, int index);
void AT_MiniTree_destroy(AT_MiniTree *tree);

void AT_MiniTree_sort_triangles(AT_TriangleArrays *triangle_arrs, uint32_t num_tri);
AT_Result AT_MiniTree_partition_list(AT_TriangleArrays *triangle_arrs, int array_idx, uint32_t start, uint32_t num_tri, AT_SplitContext *ctx);

AT_Medians AT_MiniTree_get_median_range(const AT_MiniTreeNode *node, int axis);
float AT_MiniTree_get_SAH(const AT_MiniTreeNode *node, const AT_BVHConfig *conf, uint32_t split_idx, int axis);
AT_SplitContext AT_MiniTree_get_optimal_split(const AT_MiniTreeNode *node, const AT_BVHConfig *conf);
AT_Result AT_MiniTree_split(AT_MiniTree *minitree, const AT_BVHConfig *conf);

// TODO: bvh pruning
// TODO: bvh merge
// TODO: bvh traversal
void AT_MiniTree_intersect(AT_IntersectContext *ctx, AT_MiniTree **minitrees, uint32_t num_trees, AT_Ray *in_ray);
AT_IntersectContext AT_IntersectContext_init();

#endif // AT_BVH_H
