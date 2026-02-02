#include "../src/at_aabb.h"
#include "../src/at_bvh.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include <stdint.h>
#include <string.h>

int main()
{
    uint32_t triangle_count = 15;
    AT_Triangle *ts = (AT_Triangle *)malloc(sizeof(AT_Triangle) * triangle_count);

    for (uint32_t i = 0; i < triangle_count; i++) {
        AT_Triangle *triangle = &ts[i];
        triangle->v1 = AT_vec3(0.4f, 0.2f * i, 0.34f);
        triangle->v2 = AT_vec3(0.03f * i, 0.5f, 0.67f);
        triangle->v3 = AT_vec3(0.2f, 0.6f, 0.09f * i);
        triangle->aabb = AT_AABB_from_triangle(triangle);
    }
    AT_MiniTree *mini_tree = NULL;
    if (AT_minitree_create(&mini_tree, ts, triangle_count) != AT_OK) {
        perror("Failed to create mini tree");
        return 1;
    }

    for (uint32_t i = 0; i < mini_tree->n; i++) {
        AT_Triangle triangle = mini_tree->triangles[i];
        AT_Vec3 v1 = triangle.v1;
        AT_Vec3 v2 = triangle.v2;
        AT_Vec3 v3 = triangle.v3;
        printf("Triangle %d: {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}\n", i, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
    }

    printf("\n");
    AT_Vec3 mini_tree_midpoint = mini_tree->aabb.midpoint;
    AT_MiniTree *left = NULL;
    AT_MiniTree *right = NULL;
    AT_BVH_split_tree(mini_tree, &left, &right);
    printf("Left triangle: {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f} {%f, %f, %f}\n", left->triangles[left->n - 1].v1.x, left->triangles[left->n - 1].v1.y, left->triangles[left->n - 1].v1.z, left->triangles[left->n - 1].v2.x, left->triangles[left->n - 1].v2.y, left->triangles[left->n - 1].v2.z, left->triangles[left->n - 1].v3.x, left->triangles[left->n - 1].v3.y, left->triangles[left->n - 1].v3.z, left->triangles[left->n - 1].aabb.midpoint.x, left->triangles[left->n - 1].aabb.midpoint.y, left->triangles[left->n - 1].aabb.midpoint.z);
    printf("Midpoint: {%f, %f, %f}\n", mini_tree_midpoint.x, mini_tree_midpoint.y, mini_tree_midpoint.z);
    printf("Right triangle: {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f} {%f, %f, %f}\n", right->triangles[0].v1.x, right->triangles[0].v1.y, right->triangles[0].v1.z, right->triangles[0].v2.x, right->triangles[0].v2.y, right->triangles[0].v2.z, right->triangles[0].v3.x, right->triangles[0].v3.y, right->triangles[0].v3.z, right->triangles[0].aabb.midpoint.x, right->triangles[0].aabb.midpoint.y, right->triangles[0].aabb.midpoint.z);

    AT_minitree_destroy(left);
    AT_minitree_destroy(right);

    return 0;
}
