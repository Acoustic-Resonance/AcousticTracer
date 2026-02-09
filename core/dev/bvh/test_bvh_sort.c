#include "../src/at_bvh.h"
#include "../src/at_internal.h"
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

int main()
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
    
    AT_Triangle *x_arr = malloc(sizeof(*x_arr) * num_tri);
    AT_Triangle *y_arr = malloc(sizeof(*y_arr) * num_tri);
    AT_Triangle *z_arr = malloc(sizeof(*z_arr) * num_tri);
    AT_Triangle *dim_arrs[3] = {x_arr, y_arr, z_arr};
    AT_BVH_sort_triangles(ts, num_tri, dim_arrs);
    FILE *x_file = fopen("x.txt", "w");
    FILE *y_file = fopen("y.txt", "w");
    FILE *z_file = fopen("z.txt", "w");
    for (uint32_t i = 0; i < num_tri; i++) {
        fprintf(x_file, "%f\n", x_arr[i].aabb.midpoint.x); 
    }
    for (uint32_t i = 0; i < num_tri; i++) {
        fprintf(y_file, "%f\n", y_arr[i].aabb.midpoint.y); 
    }
    for (uint32_t i = 0; i < num_tri; i++) {
        fprintf(z_file, "%f\n", z_arr[i].aabb.midpoint.z); 
    }
    fclose(x_file);
    fclose(y_file);
    fclose(z_file);

    free(ts);
    free(x_arr);
    free(y_arr);
    free(z_arr);
    AT_model_destroy(model);
    return 0;
}
