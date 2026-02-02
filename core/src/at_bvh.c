#include "../src/at_bvh.h"
#include "../src/at_aabb.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include <stdint.h>

AT_Result AT_minitree_create(AT_MiniTree **out_tree, AT_Triangle *triangles, uint32_t n)
{
    if (!out_tree || *out_tree || !triangles) return AT_ERR_INVALID_ARGUMENT;

    AT_MiniTree *mini_tree = malloc(sizeof(*mini_tree));
    if (!mini_tree) {
        // TODO: Deal with Allocation problems
        // for now will just free triangles but later should try allocate again
        free(triangles);
        return AT_ERR_ALLOC_ERROR;
    }

    mini_tree->triangles = triangles;
    mini_tree->n = n;
    mini_tree->aabb = AT_AABB_init();
    for (uint32_t i = 0; i < n; i++) {
        AT_AABB_grow(&mini_tree->aabb, triangles[i].aabb.midpoint);
    }

    *out_tree = mini_tree;
    return AT_OK;
}

void AT_minitree_destroy(AT_MiniTree *minitree)
{
    if (!minitree) return;

    free(minitree->triangles);
    free(minitree);
}
