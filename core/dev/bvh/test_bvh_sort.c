#include "../src/at_bvh.h"
#include "../src/at_internal.h"
#include "../src/at_trigroup.h"
#include "acoustic/at.h"
#include "acoustic/at_model.h"

#include <stdio.h>
#include <stdlib.h>

unsigned char gget_nth_byte(unsigned int num, int n)
{
    int offset = 8 * n;
    return (num & (0xFF << offset)) >> offset;
}

void binprintf(char v)
{
    unsigned int mask = 1 << ((sizeof(char) << 3) - 1);
    while (mask) {
        printf("%d", (v & mask ? 1 : 0));
        mask >>= 1;
    }
}

int main(int argc, char *argv[])
{
    const char *filepath = "../assets/glb/Sponza.glb";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Failed to create model\n");
        return 1;
    }

    uint32_t num_tri = model->index_count / 3;
    AT_Triangle *ts = NULL;
    if (AT_model_get_triangles(&ts, model) != AT_OK) {
        fprintf(stderr, "Failed to load triangles from model\n");
        return 1;
    }

    AT_Triangle **dim_arrs = malloc(sizeof(*dim_arrs) * 3);
    dim_arrs[0] = malloc(sizeof(AT_Triangle) * num_tri);
    dim_arrs[1] = malloc(sizeof(AT_Triangle) * num_tri);
    dim_arrs[2] = malloc(sizeof(AT_Triangle) * num_tri);
    AT_BVH_sort_triangles(ts, num_tri, dim_arrs);

    AT_BVHConfig bvh_config = {
        .mini_tree_size = 100,
    };
    AT_TriGroup *tri_group = NULL;
    if (AT_trigroup_create(&tri_group, ts, dim_arrs, 0, num_tri) != AT_OK) {
        perror("Failed to create the triangle group");
        free(ts);
        return 1;
    }

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

    FILE *x_file = fopen("x.txt", "w");
    FILE *y_file = fopen("y.txt", "w");
    FILE *z_file = fopen("z.txt", "w");
    // for (uint32_t i = 0; i < num_tri; i++) {
    //     fprintf(x_file, "%f\n", dim_arrs[0][i].aabb.midpoint.x);
    // }
    // for (uint32_t i = 0; i < num_tri; i++) {
    //     fprintf(y_file, "%f\n", dim_arrs[1][i].aabb.midpoint.y);
    // }
    // for (uint32_t i = 0; i < num_tri; i++) {
    //     fprintf(z_file, "%f\n", dim_arrs[2][i].aabb.midpoint.z);
    // }
    AT_TriGroup *groupX = groups->groups[strtol(argv[1], NULL, 10)];
    AT_TriGroup *groupY = groups->groups[strtol(argv[2], NULL, 10)];
    AT_TriGroup *groupZ = groups->groups[strtol(argv[3], NULL, 10)];
    for (uint32_t i = 0; i < groupX->n; i++) {
        fprintf(x_file, "%f\n", groupX->dim_arrs[0][i].aabb.midpoint.x);
    }
    for (uint32_t i = 0; i < groupY->n; i++) {
        fprintf(y_file, "%f\n", groupY->dim_arrs[1][i].aabb.midpoint.y);
    }
    for (uint32_t i = 0; i < groupZ->n; i++) {
        fprintf(z_file, "%f\n", groupZ->dim_arrs[2][i].aabb.midpoint.z);
    }
    fclose(x_file);
    fclose(y_file);
    fclose(z_file);

    free(ts);
    free(dim_arrs[0]);
    free(dim_arrs[1]);
    free(dim_arrs[2]);
    free(dim_arrs);
    AT_model_destroy(model);
    return 0;
}
