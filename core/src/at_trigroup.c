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

AT_Result AT_triangle_groups_create(AT_TriangleGroups **out_group, int num_ts)
{
    if (!out_group || *out_group || num_ts <= 0) {
        return AT_ERR_INVALID_ARGUMENT;
    }
    // TODO: Implement groups as a DA
    AT_TriGroup **groups_arr = malloc(sizeof(AT_TriGroup *) * num_ts);
    if (!groups_arr) return AT_ERR_ALLOC_ERROR;
    AT_TriangleGroups *groups = malloc(sizeof(*groups));
    if (!groups) {
        // TODO: Deal with allocation problems
        return AT_ERR_ALLOC_ERROR;
    }
    groups->groups = groups_arr;
    groups->n = 0;

    *out_group = groups;
    return AT_OK;
}

void AT_triangle_groups_destroy(AT_TriangleGroups *tri_groups)
{
    for (uint32_t i = 0; i < tri_groups->n; i++) {
        if (!tri_groups->groups[i]) break;
        AT_trigroup_destroy(tri_groups->groups[i]);
    }
    free(tri_groups->groups);
}

/** \brief Gets the longest side of a given triangle group's AABB.
    \relates AT_TriGroup

    \param tree A pointer to a given triangle group.

    \retval AT_Vec3 A minimum vector with the longest axis value set to the midpoint.
 */
AT_Vec3 get_longest_axis_mid(const AT_TriGroup *group, int num_axis)
{
    float delta_x = group->aabb.max.x - group->aabb.min.x;
    float delta_y = group->aabb.max.y - group->aabb.min.y;
    float delta_z = group->aabb.max.z - group->aabb.min.z;

    AT_Vec3 midpoint = (AT_Vec3){
        .x = FLT_MIN,
        .y = FLT_MIN,
        .z = FLT_MIN,
    };
    int axis_order[3];
    if (delta_x >= delta_y && delta_x >= delta_z) {
        axis_order[0] = 0;
        axis_order[1] = (delta_y > delta_z) ? 1 : 2;
        axis_order[2] = (delta_z > delta_y) ? 2 : 1;
    } else if (delta_y >= delta_x && delta_y >= delta_z) {
        axis_order[0] = 1;
        axis_order[1] = (delta_x > delta_z) ? 0 : 2;
        axis_order[2] = (delta_z > delta_x) ? 2 : 0;
    } else {
        axis_order[0] = 2;
        axis_order[1] = (delta_y > delta_x) ? 1 : 0;
        axis_order[2] = (delta_x > delta_y) ? 0 : 1;
    }

    for (int i = 0; i < num_axis; i++) {
        int axis = axis_order[i];
        if (axis == 0) {
            midpoint.x = (group->aabb.max.x + group->aabb.min.x) / 2;
        } else if (axis == 1) {
            midpoint.y = (group->aabb.max.y + group->aabb.min.y) / 2;
        } else {
            midpoint.z = (group->aabb.max.z + group->aabb.min.z) / 2;
        }
    }

    return midpoint;
}

AT_Result split_group(const AT_TriGroup *parent_group, AT_TriGroup **left_group, AT_TriGroup **right_group)
{
    if (!parent_group || !left_group || *left_group || !right_group || *right_group) {
        return AT_ERR_INVALID_ARGUMENT;
    }

    uint32_t left_n = 0;
    uint32_t right_n = 0;
    AT_Triangle *triangles;
    uint32_t left;
    int num_axis = 1;
    do {
        // 1. Get longest axis
        // 2. Get centre of longest axis
        AT_Vec3 midpoint = get_longest_axis_mid(parent_group, num_axis);

        // 3. Get triangles to left of axis
        // 4. Get triangles to right of axis
        triangles = parent_group->triangles;
        left = 0;
        AT_Triangle temp;
        for (uint32_t i = 0; i < parent_group->n; i++) {
            AT_Vec3 triangle_mid = parent_group->triangles[i].aabb.midpoint;
            bool is_left = (midpoint.x == FLT_MIN || triangle_mid.x <= midpoint.x) &&
                           (midpoint.y == FLT_MIN || triangle_mid.y <= midpoint.y) &&
                           (midpoint.z == FLT_MIN || triangle_mid.z <= midpoint.z);
            if (is_left) {
                if (left < i) {
                    temp = triangles[left];
                    triangles[left] = triangles[i];
                    triangles[i] = temp;
                }
                left++;
            }
        }
        left_n = left;
        right_n = parent_group->n - left;
    } while (
        (left_n == parent_group->n || right_n == parent_group->n) &&
        num_axis++ < 3
    );
    AT_Result res;
    res = AT_trigroup_create(left_group, &triangles[0], left_n);
    if (res != AT_OK) {
        perror("Failed to create left sub group");
        return res;
    }
    res = AT_trigroup_create(right_group, &triangles[left_n], right_n);
    if (res != AT_OK) {
        perror("Failed to create right sub group");
        return res;
    }

    return AT_OK;
}

AT_Result AT_trigroup_split(AT_TriGroup *org_group, AT_TriangleGroups *groups, uint32_t N)
{
    if (!org_group || !groups) return AT_ERR_INVALID_ARGUMENT;

    // 5. Repeat for sub trees
    AT_TriGroup *stack[(int)ceil(log2(org_group->n))];
    int stack_top = 0;
    stack[stack_top] = org_group;
    stack_top++;
    AT_TriGroup *left;
    AT_TriGroup *right;
    AT_TriGroup *parent_group;
    while (stack_top > 0) {
        left = NULL;
        right = NULL;
        stack_top--;
        parent_group = stack[stack_top];
        AT_Result res = split_group(parent_group, &left, &right);
        if (res != AT_OK) {
            perror("Failed to split the tri group");
            return res;
        }
        if ((left->n == 0 && right->n == parent_group->n) ||
            (right->n == 0 && left->n == parent_group->n)) {
            groups->groups[groups->n] = parent_group;
            groups->n++;
            free(left);
            free(right);
            continue; // Don't destroy parent_group
        }
        if (left->n <= N) {
            groups->groups[groups->n] = left;
            groups->n++;
        } else {
            stack[stack_top] = left;
            stack_top++;
        }
        if (right->n <= N) {
            groups->groups[groups->n] = right;
            groups->n++;
        } else {
            stack[stack_top] = right;
            stack_top++;
        }

        AT_trigroup_destroy(parent_group);
    }

    return AT_OK;
}
