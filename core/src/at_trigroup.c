#include "../src/at_trigroup.h"
#include "../src/at_aabb.h"

AT_Result AT_trigroup_create(AT_TriGroup **out_group, AT_Triangle *triangles, uint32_t n)
{
    if (!out_group || *out_group || !triangles) return AT_ERR_INVALID_ARGUMENT;

    AT_TriGroup *tri_group = malloc(sizeof(*tri_group));
    if (!tri_group) {
        // TODO: Deal with Allocation problems
        // for now will just return err but later should try allocate again
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

    \retval AT_Vec3 A minimum vector with the longest axis value set to the midpoint.
 */
AT_Vec3 get_longest_axis_mid(const AT_TriGroup *group)
{
    float delta_x = group->aabb.max.x - group->aabb.min.x;
    float delta_y = group->aabb.max.y - group->aabb.min.y;
    float delta_z = group->aabb.max.z - group->aabb.min.z;

    AT_Vec3 midpoint = (AT_Vec3){
        .x = FLT_MIN,
        .y = FLT_MIN,
        .z = FLT_MIN,
    };
    if (delta_x >= delta_y && delta_x >= delta_z) {
        midpoint.x = (group->aabb.max.x + group->aabb.min.x) / 2;
    } else if (delta_y >= delta_x && delta_y >= delta_z) {
        midpoint.y = (group->aabb.max.y + group->aabb.min.y) / 2;
    } else {
        midpoint.z = (group->aabb.max.z + group->aabb.min.z) / 2;
    }

    return midpoint;
}

AT_Result AT_trigroup_split(AT_TriGroup *org_group, AT_TriGroup **left_group, AT_TriGroup **right_group)
{

    if (!org_group || !left_group || *left_group || !right_group || *right_group) {
        return AT_ERR_INVALID_ARGUMENT;
    }
    // 1. Get longest axis
    // 2. Get centre of longest axis
    AT_Vec3 midpoint = get_longest_axis_mid(org_group);

    // 3. Get triangles to left of axis
    // 4. Get triangles to right of axis
    AT_Triangle *triangles = org_group->triangles;
    uint32_t left = 0;
    AT_Triangle temp;
    for (uint32_t i = 0; i < org_group->n; i++) {
        AT_Vec3 triangle_mid = org_group->triangles[i].aabb.midpoint;
        bool is_left = triangle_mid.x <= midpoint.x ||
                       triangle_mid.y <= midpoint.y ||
                       triangle_mid.z <= midpoint.z;
        if (is_left) {
            if (left < i) {
                temp = triangles[left];
                triangles[left] = triangles[i];
                triangles[i] = temp;
            }
            left++;
        }
    }
    int left_n = left;
    int right_n = (org_group->n - 1) - left;
    if (AT_trigroup_create(left_group, &triangles[0], left_n) != AT_OK) {
        perror("Failed to create left sub tree");
    }
    if (AT_trigroup_create(right_group, &triangles[left_n], right_n != AT_OK)) {
        perror("Failed to create right sub tree");
    }

    // 5. Repeat for sub trees

    AT_trigroup_destroy(org_group);

    return AT_OK;
}
