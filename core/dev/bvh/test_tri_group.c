#include "../src/at_aabb.h"
#include "../src/at_trigroup.h"
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
    AT_TriGroup *tri_group = NULL;
    if (AT_trigroup_create(&tri_group, ts, triangle_count) != AT_OK) {
        perror("Failed to create the triangle group");
        free(ts);
        return 1;
    }

    for (uint32_t i = 0; i < tri_group->n; i++) {
        AT_Triangle triangle = tri_group->triangles[i];
        AT_Vec3 v1 = triangle.v1;
        AT_Vec3 v2 = triangle.v2;
        AT_Vec3 v3 = triangle.v3;
        printf("Triangle %d: {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}\n", i, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
    }

    printf("\n");
    AT_Vec3 tri_group_midpoint = tri_group->aabb.midpoint;
    AT_TriGroup *left = NULL;
    AT_TriGroup *right = NULL;
    if (AT_trigroup_split(tri_group, &left, &right) != AT_OK) {
        perror("Failed to split the triangle group");
        return 1;
    }
    printf("Left triangle: {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f} {%f, %f, %f}\n", left->triangles[left->n - 1].v1.x, left->triangles[left->n - 1].v1.y, left->triangles[left->n - 1].v1.z, left->triangles[left->n - 1].v2.x, left->triangles[left->n - 1].v2.y, left->triangles[left->n - 1].v2.z, left->triangles[left->n - 1].v3.x, left->triangles[left->n - 1].v3.y, left->triangles[left->n - 1].v3.z, left->triangles[left->n - 1].aabb.midpoint.x, left->triangles[left->n - 1].aabb.midpoint.y, left->triangles[left->n - 1].aabb.midpoint.z);
    printf("Midpoint: {%f, %f, %f}\n", tri_group_midpoint.x, tri_group_midpoint.y, tri_group_midpoint.z);
    printf("Right triangle: {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f} {%f, %f, %f}\n", right->triangles[0].v1.x, right->triangles[0].v1.y, right->triangles[0].v1.z, right->triangles[0].v2.x, right->triangles[0].v2.y, right->triangles[0].v2.z, right->triangles[0].v3.x, right->triangles[0].v3.y, right->triangles[0].v3.z, right->triangles[0].aabb.midpoint.x, right->triangles[0].aabb.midpoint.y, right->triangles[0].aabb.midpoint.z);

    AT_trigroup_destroy(left);
    AT_trigroup_destroy(right);

    free(ts);

    return 0;
}
