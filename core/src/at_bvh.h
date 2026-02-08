#ifndef AT_BVH_H
#define AT_BVH_H

#include "acoustic/at.h"

#include <stdint.h>

typedef struct {
    AT_Triangle *triangles;
    uint32_t n;
    AT_AABB aabb;
} AT_TriGroup;

typedef struct {
    AT_TriGroup **groups;
    uint32_t n;
} AT_TriangleGroups;

typedef struct {
    uint32_t mini_tree_size;
} AT_BVHConfig;

typedef struct {

} AT_BVH;

void AT_BVH_create(AT_BVH **out_tree);
void AT_BVH_destroy(AT_BVH *tree);

void AT_BVH_sort_triangles(AT_Triangle *triangles);
void AT_BVH_partition_lists();

void AT_BVH_get_median_range();
float AT_BVH_get_SAH(AT_BVH *tree);

#endif // AT_BVH_H
