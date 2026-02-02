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

/** \brief Gets the longest side of a given mini tree
    \relates AT_MiniTree

    \param tree A pointer to a given mini tree.

    \retval char The longest axis as a single char.
 */
char get_longest_axis(const AT_MiniTree *tree)
{
    float delta_x = tree->aabb.max.x - tree->aabb.min.x;
    float delta_y = tree->aabb.max.y - tree->aabb.min.y;
    float delta_z = tree->aabb.max.z - tree->aabb.min.z;

    // TODO: See about optimising return value so don't have to subtract again later
    if (delta_x >= delta_y && delta_x >= delta_z) {
        return 'x';
    } else if (delta_y >= delta_x && delta_y >= delta_z) {
        return 'y';
    } else {
        return 'z';
    }
}

void AT_BVH_split_tree(const AT_MiniTree *org_tree, AT_MiniTree *left_tree, AT_MiniTree *right_tree)
{
    // 1. Get longest axis
    char longest_axis = get_longest_axis(org_tree);
    // 2. Get centre of longest axis
    // 3. Get triangles to left of axis
    // 4. Get triangles to right of axis
    // 5. Repeat for sub trees
}
