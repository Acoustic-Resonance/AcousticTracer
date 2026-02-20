#include "../src/at_aabb.h"
#include "../src/at_bvh.h"

#include <stdlib.h>

int main()
{
    AT_AABB aabb = AT_AABB_init();
    int num_tri = 150;
    AT_Triangle *ts = malloc(sizeof(*ts) * num_tri);
    for (int i = 0; i < num_tri; i++) {
        ts[i] = (AT_Triangle){
            .v1 = {{0, 0, 0}},
            .v2 = {{2 * i, 0, 0}},
            .v3 = {{0, 0, 3 * i}},
        };
        ts[i].aabb = AT_AABB_from_triangle(&ts[i]);
        AT_AABB_grow(&aabb, ts[i].aabb.midpoint);
    }

    float delta_x = aabb.max.x - aabb.min.x;
    float delta_y = aabb.max.y - aabb.min.y;
    float delta_z = aabb.max.z - aabb.min.z;

    int axis;
    if (delta_x >= delta_y && delta_x >= delta_z) {
        axis = 0;
    } else if (delta_y >= delta_x && delta_y >= delta_z) {
        axis = 1;
    } else {
        axis = 2;
    }
    
    AT_Medians medians = AT_BVH_get_median_range(ts, num_tri, axis);
    printf("Object: %d; Spatial: %d\n", medians.object, medians.spatial);
    float start_tri_mid = ts[0].aabb.midpoint.arr[axis];
    float mid_tri_mid = ts[medians.spatial].aabb.midpoint.arr[axis];
    float finish_tri_mid = ts[num_tri - 1].aabb.midpoint.arr[axis];
    printf("Starting point: %f\n", start_tri_mid);
    printf("Middle point: %f\n", mid_tri_mid);
    printf("Finishing point: %f\n", finish_tri_mid);
}
