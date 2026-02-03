#ifndef AT_BVH
#define AT_BVH

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

void AT_BVH_sort_triangles(AT_Triangle *triangles, char axis);

#endif // AT_BVH
