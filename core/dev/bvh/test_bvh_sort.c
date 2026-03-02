#include "../src/at_bvh.h"
#include "../src/at_internal.h"
#include "../src/at_trigroup.h"
#include "../src/at_utils.h"
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

    AT_TriangleArrays *triangle_arrs = NULL;
    if (AT_triangle_arrays_create(&triangle_arrs, model) != AT_OK) {
        perror("Failed to create triangle arrays");
        AT_model_destroy(model);
        return 1;
    }

    uint32_t num_tri = model->index_count / 3;

    AT_BVHConfig bvh_config = {
        .mini_tree_size = 100,
    };

    AT_TriangleGroups *tri_groups = NULL;
    if (AT_triangle_groups_create(&tri_groups, num_tri) != AT_OK) {
        perror("Failed to create the triangle groups holder");
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }
    if (AT_trigroup_split(triangle_arrs, num_tri, tri_groups, bvh_config.mini_tree_size) != AT_OK) {
        perror("Failed to split the triangle group");
        AT_triangle_groups_destroy(tri_groups);
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
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
    AT_TriGroup *groupX = tri_groups->groups[strtol(argv[1], NULL, 10)];
    AT_TriGroup *groupY = tri_groups->groups[strtol(argv[2], NULL, 10)];
    AT_TriGroup *groupZ = tri_groups->groups[strtol(argv[3], NULL, 10)];
    for (uint32_t i = 0; i < groupX->num_tri; i++) {
        fprintf(x_file, "%f\n", AT_get_triangle(groupX, 0, i).aabb.midpoint.x);
    }
    for (uint32_t i = 0; i < groupY->num_tri; i++) {
        fprintf(y_file, "%f\n", AT_get_triangle(groupY, 1, i).aabb.midpoint.y);
    }
    for (uint32_t i = 0; i < groupZ->num_tri; i++) {
        fprintf(z_file, "%f\n", AT_get_triangle(groupZ, 2, i).aabb.midpoint.z);
    }
    fclose(x_file);
    fclose(y_file);
    fclose(z_file);

    AT_triangle_groups_destroy(tri_groups);
    AT_triangle_arrays_destroy(triangle_arrs);
    AT_model_destroy(model);
    return 0;
}
