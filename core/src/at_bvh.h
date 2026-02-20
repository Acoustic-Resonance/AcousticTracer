#ifndef AT_BVH_H
#define AT_BVH_H

#include "acoustic/at.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    AT_Triangle *triangles;
    AT_Triangle **dim_arrs;
    uint32_t num_tri;
    AT_AABB aabb;
} AT_TriGroup;

typedef struct {
    AT_TriGroup **groups;
    uint32_t num_groups;
} AT_TriangleGroups;

typedef struct {
    uint32_t mini_tree_size;
} AT_BVHConfig;

typedef struct {

} AT_BVH;

typedef struct {
    float threshold;
    int axis;
    uint32_t left_n;
} AT_SplitContext;

typedef struct {
    uint32_t spatial;
    uint32_t object;
} AT_Medians;

typedef bool (*AT_CompareFunc)(AT_Vec3, AT_SplitContext *);

void AT_BVH_create(AT_BVH **out_tree);
void AT_BVH_destroy(AT_BVH *tree);

void AT_BVH_sort_triangles(AT_Triangle *triangles, uint32_t num_tri, AT_Triangle **dim_arrs);
void AT_BVH_partition_list(AT_Triangle *triangles, uint32_t num_tri, AT_SplitContext *ctx);

AT_Medians AT_BVH_get_median_range(AT_Triangle *triangles, uint32_t num_tri, int axis);

#endif // AT_BVH_H
