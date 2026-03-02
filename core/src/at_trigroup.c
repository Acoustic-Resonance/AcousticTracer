#include "../src/at_trigroup.h"
#include "../src/at_aabb.h"
#include "../src/at_bvh.h"

AT_Result AT_trigroup_create(AT_TriGroup **out_group, AT_Triangle *triangles, AT_Triangle **dim_arrs, uint32_t start, uint32_t n)
{
    if (!out_group || *out_group || !triangles) return AT_ERR_INVALID_ARGUMENT;

    AT_TriGroup *tri_group = malloc(sizeof(*tri_group));
    if (!tri_group) {
        // TODO: Deal with Allocation problems
        // for now will just return err but later should try allocate again
        return AT_ERR_ALLOC_ERROR;
    }

    tri_group->triangles = &triangles[start];
    tri_group->dim_arrs = malloc(sizeof(*dim_arrs) * 3);
    for (int i = 0; i < 3; i++) {
        tri_group->dim_arrs[i] = &dim_arrs[i][start];
    }
    tri_group->num_tri = n;
    // TODO: properly fix this shit bro
    if (n == 0) {
        *out_group = tri_group;
        return AT_OK;
    }

    tri_group->aabb = AT_AABB_init();
    for (int i = 0; i < 3; i++) {
        AT_AABB_grow(&tri_group->aabb, tri_group->dim_arrs[i][0].aabb.midpoint);
        AT_AABB_grow(&tri_group->aabb, tri_group->dim_arrs[i][n - 1].aabb.midpoint);
    }

    *out_group = tri_group;
    return AT_OK;
}

void AT_trigroup_destroy(AT_TriGroup *tri_group)
{
    if (!tri_group) return;

    free(tri_group->dim_arrs);
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
    groups->num_groups = 0;

    *out_group = groups;
    return AT_OK;
}

void AT_triangle_groups_destroy(AT_TriangleGroups *tri_groups)
{
    if (!tri_groups) return;
    for (uint32_t i = 0; i < tri_groups->num_groups; i++) {
        if (!tri_groups->groups[i]) break;
        AT_trigroup_destroy(tri_groups->groups[i]);
    }
    free(tri_groups->groups);
    free(tri_groups);
}

/** \brief Gets the longest side of a given triangle group's AABB.
    \relates AT_TriGroup

    \param tree A pointer to a given triangle group.

    \retval AT_Vec3 A minimum vector with the longest axis value set to the midpoint.
 */
AT_SplitContext get_longest_axis_mid(const AT_TriGroup *group, int nth_longest)
{
    float delta_x = group->aabb.max.x - group->aabb.min.x;
    float delta_y = group->aabb.max.y - group->aabb.min.y;
    float delta_z = group->aabb.max.z - group->aabb.min.z;

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

    int axis = axis_order[nth_longest - 1];
    // float midpoint = group->aabb.midpoint.arr[axis];
    float midpoint = group->triangles[group->num_tri / 2].aabb.midpoint.arr[axis];

    AT_SplitContext ctx = {
        .threshold = midpoint,
        .axis = axis
    };
    return ctx;
}

AT_Result split_group(const AT_TriGroup *parent_group, AT_TriGroup **left_group, AT_TriGroup **right_group)
{
    if (!parent_group || !left_group || *left_group || !right_group || *right_group) {
        return AT_ERR_INVALID_ARGUMENT;
    }

    uint32_t left_n = 0;
    uint32_t right_n = 0;
    AT_Triangle *triangles = parent_group->triangles;
    AT_Triangle **dim_arrs = parent_group->dim_arrs;
    uint32_t num_tri = parent_group->num_tri;
    int nth_longest = 1; // The xth longest axis
    AT_SplitContext ctx;
    do {
        // 1. Get longest axis
        // 2. Get centre of longest axis
        ctx = get_longest_axis_mid(parent_group, nth_longest);

        // 3. Get triangles to left of axis
        // 4. Get triangles to right of axis
        left_n = 0;
        for (uint32_t i = 0; i < num_tri; i++) {
            if (triangles[i].aabb.midpoint.arr[ctx.axis] < ctx.threshold) {
                left_n++;
            }
        }

        right_n = num_tri - left_n;
        right_n = num_tri - left_n;
    } while (
        (left_n == parent_group->num_tri || right_n == parent_group->num_tri) &&
        nth_longest++ < 3
    );
    ctx.left_n = left_n;
    AT_BVH_partition_list(triangles, num_tri, &ctx);
    for (int i = 0; i < 3; i++) {
        AT_BVH_partition_list(dim_arrs[i], num_tri, &ctx);
    }
    AT_Result res;
    res = AT_trigroup_create(left_group, triangles, dim_arrs, 0, left_n);
    if (res != AT_OK) {
        perror("Failed to create left sub group");
        return res;
    }
    res = AT_trigroup_create(right_group, triangles, dim_arrs, left_n, right_n);
    if (res != AT_OK) {
        perror("Failed to create right sub group");
        return res;
    }

    return AT_OK;
}

AT_Result AT_trigroup_split(AT_Triangle *triangles, AT_Triangle **dim_arrs, uint32_t num_tri, AT_TriangleGroups *groups, uint32_t N)
{
    if (!groups) return AT_ERR_INVALID_ARGUMENT;

    AT_TriGroup *tri_group = NULL;
    if (AT_trigroup_create(&tri_group, triangles, dim_arrs, 0, num_tri) != AT_OK) {
        perror("Failed to create the triangle group");
        return 1;
    }

    // 5. Repeat for sub trees
    AT_TriGroup *stack[(int)ceil(log2(num_tri))];
    int stack_top = 0;
    stack[stack_top] = tri_group;
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
        if ((right->num_tri == parent_group->num_tri) ||
            (left->num_tri == parent_group->num_tri)) {
            groups->groups[groups->num_groups] = parent_group;
            groups->num_groups++;
            free(left);
            free(right);
            continue; // Don't destroy parent_group
        }
        if (left->num_tri <= N) {
            groups->groups[groups->num_groups] = left;
            groups->num_groups++;
        } else {
            stack[stack_top] = left;
            stack_top++;
        }
        if (right->num_tri <= N) {
            groups->groups[groups->num_groups] = right;
            groups->num_groups++;
        } else {
            stack[stack_top] = right;
            stack_top++;
        }

        AT_trigroup_destroy(parent_group);
    }

    return AT_OK;
}
