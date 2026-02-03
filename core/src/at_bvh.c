#include "../src/at_bvh.h"
#include "../src/at_aabb.h"

AT_Result AT_minitree_create(AT_MiniTree **out_tree, AT_Triangle *triangles, uint32_t n)
{
    if (!out_tree || *out_tree || !triangles) return AT_ERR_INVALID_ARGUMENT;

    AT_MiniTree *mini_tree = malloc(sizeof(*mini_tree));
    if (!mini_tree) {
        // TODO: Deal with Allocation problems
        // for now will just free triangles but later should try allocate again
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

    if (delta_x >= delta_y && delta_x >= delta_z) {
        return 'x';
    } else if (delta_y >= delta_x && delta_y >= delta_z) {
        return 'y';
    } else {
        return 'z';
    }
}

AT_Result AT_BVH_split_tree(AT_MiniTree *org_tree, AT_MiniTree **left_tree, AT_MiniTree **right_tree)
{
    if (!org_tree || !left_tree || *left_tree || !right_tree || *right_tree) {
        return AT_ERR_INVALID_ARGUMENT;
    }
    AT_Triangle *triangles = malloc(sizeof(*triangles) * org_tree->n);
    if (!triangles) {
        // TODO: Deal with allocation problems
        return AT_ERR_ALLOC_ERROR;
    }
    // TODO: See about optimising so you don't need all the ifs
    // 1. Get longest axis
    char longest_axis = get_longest_axis(org_tree);
    // 2. Get centre of longest axis
    float centre;
    if (longest_axis == 'x') {
        centre = org_tree->aabb.midpoint.x;
    } else if (longest_axis == 'y') {
        centre = org_tree->aabb.midpoint.y;
    } else {
        centre = org_tree->aabb.midpoint.z;
    }
    // 3. Get triangles to left of axis
    // 4. Get triangles to right of axis
    uint32_t left = 0;
    uint32_t right = org_tree->n - 1;
    for (uint32_t i = 0; i < org_tree->n; i++) {
        AT_Triangle triangle = org_tree->triangles[i];
        AT_Vec3 triangle_mid = triangle.aabb.midpoint;
        if (longest_axis == 'x') {
            if (triangle_mid.x <= centre) {
                triangles[left] = triangle;
                left++;
            } else {
                triangles[right] = triangle;
                right--;
            }
        } else if (longest_axis == 'y') {
            if (triangle_mid.y <= centre) {
                triangles[left] = triangle;
                left++;
            } else {
                triangles[right] = triangle;
                right--;
            }
        } else {
            if (triangle_mid.z <= centre) {
                triangles[left] = triangle;
                left++;
            } else {
                triangles[right] = triangle;
                right--;
            }
        }
    }
    int left_n = left;
    int right_n = (org_tree->n - 1) - right;
    AT_Triangle *left_triangles = malloc(sizeof(*left_triangles) * left_n);
    if (!left_triangles) {
        // TODO: Deal with allocation problems
        free(triangles);
        return AT_ERR_ALLOC_ERROR;
    }
    AT_Triangle *right_triangles = malloc(sizeof(*right_triangles) * right_n);
    if (!right_triangles) {
        // TODO: Deal with allocation problems
        free(triangles);
        free(right_triangles);
        return AT_ERR_ALLOC_ERROR;
    }
    for (int i = 0; i < left_n; i++) {
        left_triangles[i] = triangles[i];
    }
    int j = 0;
    for (int i = org_tree->n - 1; i > right_n; i--) {
        right_triangles[j] = triangles[i];
        j++;
    }
    if (AT_minitree_create(left_tree, left_triangles, left_n) != AT_OK) {
        perror("Failed to create left sub tree");
    }
    if (AT_minitree_create(right_tree, right_triangles, right_n != AT_OK)) {
        perror("Failed to create right sub tree");
    }
    // 5. Repeat for sub trees

    AT_minitree_destroy(org_tree);

    return AT_OK;
}
