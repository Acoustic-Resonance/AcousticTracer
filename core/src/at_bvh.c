#include "../src/at_bvh.h"
#include "../src/at_aabb.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include <stdint.h>

AT_Result AT_minitree_create(AT_MiniTree **out_tree, AT_Triangle *triangles, uint32_t n)
{
    AT_MiniTree *mini_tree = malloc(sizeof(*mini_tree));
    mini_tree->triangles = triangles;
    mini_tree->n = n;
    mini_tree->aabb = AT_AABB_init();
    for (uint32_t i = 0; i < n; i++) {
        AT_AABB_grow(&mini_tree->aabb, triangles[i].aabb.midpoint);
    }

    *out_tree = mini_tree;
    return AT_OK;
}
