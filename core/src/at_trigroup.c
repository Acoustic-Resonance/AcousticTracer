#include "../src/at_trigroup.h"
#include "../src/at_aabb.h"

AT_Result AT_trigroup_create(AT_TriGroup **out_group, AT_Triangle *triangles, uint32_t n)
{
    if (!out_group || *out_group || !triangles) return AT_ERR_INVALID_ARGUMENT;

    AT_TriGroup *tri_group = malloc(sizeof(*tri_group));
    if (!tri_group) {
        // TODO: Deal with Allocation problems
        // for now will just free triangles but later should try allocate again
        return AT_ERR_ALLOC_ERROR;
    }

    tri_group->triangles = triangles;
    tri_group->n = n;
    tri_group->aabb = AT_AABB_init();
    for (uint32_t i = 0; i < n; i++) {
        AT_AABB_grow(&tri_group->aabb, triangles[i].aabb.midpoint);
    }

    *out_group = tri_group;
    return AT_OK;
}

void AT_trigroup_destroy(AT_TriGroup *tri_group)
{
    if (!tri_group) return;

    free(tri_group);
}

/** \brief Gets the longest side of a given triangle group's AABB.
    \relates AT_TriGroup

    \param tree A pointer to a given triangle group.

    \retval char The longest axis as a single char.
 */
char get_longest_axis(const AT_TriGroup *group)
{
    float delta_x = group->aabb.max.x - group->aabb.min.x;
    float delta_y = group->aabb.max.y - group->aabb.min.y;
    float delta_z = group->aabb.max.z - group->aabb.min.z;

    if (delta_x >= delta_y && delta_x >= delta_z) {
        return 'x';
    } else if (delta_y >= delta_x && delta_y >= delta_z) {
        return 'y';
    } else {
        return 'z';
    }
}

AT_Result AT_trigroup_split(AT_TriGroup *org_group, AT_TriGroup **left_group, AT_TriGroup **right_group)
{
    if (!org_group || !left_group || *left_group || !right_group || *right_group) {
        return AT_ERR_INVALID_ARGUMENT;
    }
    // TODO: Use org_group->triangles and just shift them around to reduce space and IO operations
    // use https://youtu.be/C1H4zIiCOaI?t=692
    AT_Triangle *triangles = malloc(sizeof(*triangles) * org_group->n);
    if (!triangles) {
        // TODO: Deal with allocation problems
        return AT_ERR_ALLOC_ERROR;
    }
    // TODO: See about optimising so you don't need all the ifs
    // 1. Get longest axis
    char longest_axis = get_longest_axis(org_group);
    // 2. Get centre of longest axis
    float centre;
    if (longest_axis == 'x') {
        centre = org_group->aabb.midpoint.x;
    } else if (longest_axis == 'y') {
        centre = org_group->aabb.midpoint.y;
    } else {
        centre = org_group->aabb.midpoint.z;
    }
    // 3. Get triangles to left of axis
    // 4. Get triangles to right of axis
    uint32_t left = 0;
    uint32_t right = org_group->n - 1;
    for (uint32_t i = 0; i < org_group->n; i++) {
        AT_Triangle triangle = org_group->triangles[i];
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
    int right_n = (org_group->n - 1) - right;
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
    for (int i = org_group->n - 1; i > right_n; i--) {
        right_triangles[j] = triangles[i];
        j++;
    }
    if (AT_trigroup_create(left_group, left_triangles, left_n) != AT_OK) {
        perror("Failed to create left sub tree");
    }
    if (AT_trigroup_create(right_group, right_triangles, right_n != AT_OK)) {
        perror("Failed to create right sub tree");
    }
    // 5. Repeat for sub trees

    AT_trigroup_destroy(org_group);

    return AT_OK;
}
