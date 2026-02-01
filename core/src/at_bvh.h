#include "acoustic/at_math.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t uint32;

typedef struct {
    uint16_t mini_tree_size;
} AT_BVHConfig;

typedef struct {
    AT_Triangle *triangles;
    uint32 n; /**< Number of triangles in the tree. */
    AT_AABB aabb;
} AT_MiniTree;

void AT_BVH_sort_triangles(AT_Triangle *triangles, char axis);
