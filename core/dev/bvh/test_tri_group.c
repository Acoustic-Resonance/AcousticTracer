#include "../src/at_aabb.h"
#include "../src/at_bvh.h"
#include "../src/at_trigroup.h"
#include "acoustic/at.h"

#include <stdint.h>

int main()
{
    uint32_t triangle_count = 1000;
    AT_BVHConfig bvh_config = {
        .mini_tree_size = 100
    };
    AT_Triangle *ts = (AT_Triangle *)malloc(sizeof(AT_Triangle) * triangle_count);

    for (uint32_t i = 0; i < triangle_count; i++) {
        AT_Triangle *triangle = &ts[i];
        triangle->v1 = AT_vec3(0.4f, 0.2f * i, 0.34f);
        triangle->v2 = AT_vec3(0.03f * i, 0.5f, 0.67f);
        triangle->v3 = AT_vec3(0.2f, 0.6f, 0.09f * i);
        triangle->aabb = AT_AABB_from_triangle(triangle);
    }
    AT_TriGroup *tri_group = NULL;
    if (AT_trigroup_create(&tri_group, ts, triangle_count) != AT_OK) {
        perror("Failed to create the triangle group");
        free(ts);
        return 1;
    }

    // TODO: Implement groups as a DA
    AT_TriangleGroups *groups = NULL;
    if (AT_triangle_groups_create(&groups, tri_group->n) != AT_OK) {
        perror("Failed to create the triangle groups holder");
        free(ts);
        return 1;
    }
    if (AT_trigroup_split(tri_group, groups, bvh_config.mini_tree_size) != AT_OK) {
        perror("Failed to split the triangle group");
        free(ts);
        return 1;
    }

    printf("\n");
    for (uint32_t i = 0; i < groups->n; i++) {
        printf("Group %d of size %d\n", i, groups->groups[i]->n);
    }

    AT_triangle_groups_destroy(groups);
    free(ts);

    return 0;
}
