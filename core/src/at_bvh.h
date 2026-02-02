#include "acoustic/at.h"
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

/** \brief AT_MiniTree constructor for a given list of triangles.
    \relates AT_MiniTree

    \param out_tree Pointer to an empty initialised AT_MiniTree.
    \param triangles Array of triangles.
    \param n The number of triangles associated with the tree.

    \retval AT_Result Saves the created mini tree at the location of the pointer, returning a result enum value.
 */
AT_Result AT_minitree_create(AT_MiniTree **out_tree, AT_Triangle *triangles, uint32_t n);

/** \brief Destroys an allocated AT_MiniTree.
    \relates AT_MiniTree

    \param minitree Pointer to an initialised mini tree.

    \retval void
 */
void AT_minitree_destroy(AT_MiniTree *minitree);

void AT_BVH_sort_triangles(AT_Triangle *triangles, char axis);
