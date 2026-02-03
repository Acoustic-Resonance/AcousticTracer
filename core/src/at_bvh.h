#ifndef AT_BVH
#define AT_BVH

#include "acoustic/at.h"

#include <stdint.h>

typedef struct {
    uint16_t mini_tree_size;
} AT_BVHConfig;

void AT_BVH_sort_triangles(AT_Triangle *triangles, char axis);

#endif // AT_BVH
