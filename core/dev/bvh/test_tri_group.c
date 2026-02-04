#include "../src/at_aabb.h"
#include "../src/at_bvh.h"
#include "../src/at_internal.h"
#include "../src/at_trigroup.h"
#include "acoustic/at.h"
#include "acoustic/at_model.h"
#include "raylib.h"
#include "rlgl.h"

#include <stdint.h>

#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 1000

int main(int argc, char *_[])
{
    AT_TriGroup *tri_group = NULL;
    AT_BVHConfig bvh_config;
    AT_Triangle *ts;
    AT_Model *model;

    if (argc > 1) {
        const char *filepath = "../assets/glb/Sponza.glb";
        model = NULL;
        if (AT_model_create(&model, filepath) != AT_OK) {
            perror("Failed to create model");
            return 1;
        }

        for (uint32_t i = 0; i < model->vertex_count; i++) {
            model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.5);
        }

        if (AT_model_get_triangles(&ts, model) != AT_OK) {
            perror("Error getting triangles from the given model");
            return 1;
        }
        bvh_config.mini_tree_size = (model->index_count / 3) / 16;

        if (AT_trigroup_create(&tri_group, ts, model->index_count / 3) != AT_OK) {
            perror("Failed to create the triangle group");
            free(ts);
            return 1;
        }
    } else {
        uint32_t triangle_count = 7000000;
        int num_groups = triangle_count / 100;
        bvh_config.mini_tree_size = triangle_count / num_groups;
        ts = (AT_Triangle *)malloc(sizeof(AT_Triangle) * triangle_count);
        for (uint32_t i = 0; i < triangle_count; i++) {
            AT_Triangle *triangle = &ts[i];
            triangle->v1 = AT_vec3(0.4f, 0.2f * i, 0.34f);
            triangle->v2 = AT_vec3(0.03f * i, 0.5f, 0.67f);
            triangle->v3 = AT_vec3(0.2f, 0.6f, 0.09f * i);
            triangle->aabb = AT_AABB_from_triangle(triangle);
        }
        if (AT_trigroup_create(&tri_group, ts, triangle_count) != AT_OK) {
            perror("Failed to create the triangle group");
            free(ts);
            return 1;
        }
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

    if (argc > 2) {
        Color colors[16] = {RED, BLUE, GREEN, PURPLE, PINK, LIME, BROWN, MAROON, MAGENTA, ORANGE, GOLD, YELLOW, DARKGREEN, SKYBLUE, DARKBLUE, VIOLET};

        AT_AABB aabb = {};
        AT_model_to_AABB(&aabb, model);

        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Triangle group testing");
        SetTargetFPS(60);

        Camera3D cam = {.position = {5.0f, 5.0f, 5.0f}, .target = {0.0f, 0.0f, 0.0f}, .up = {0.0f, 1.0f, 0.0f}, .fovy = 60.0f, .projection = CAMERA_PERSPECTIVE};

        rlDisableBackfaceCulling();
        rlSetLineWidth(1.0f);

        while (!WindowShouldClose()) {
            UpdateCamera(&cam, CAMERA_FREE);

            BeginDrawing();
            {
                ClearBackground(WHITE);
                BeginMode3D(cam);
                {
                    for (uint32_t i = 0; i < groups->n; i++) {
                        // if (groups->groups[i]->n < 40000) {
                        //     continue;
                        // }
                        Color color = colors[i % 16];
                        AT_TriGroup *group = groups->groups[i];
                        // AT_AABB aabb = group->aabb;
                        // AT_Vec3 midpoint = aabb.midpoint;
                        // DrawBoundingBox(
                        //     (BoundingBox){
                        //         (Vector3){aabb.min.x, aabb.min.y, aabb.min.z},
                        //         (Vector3){aabb.max.x, aabb.max.y, aabb.max.z}
                        //     },
                        //     color
                        // );
                        // DrawSphere(
                        //     (Vector3){
                        //         midpoint.x, midpoint.y, midpoint.z
                        //     },
                        //     0.3f,
                        //     color
                        // );
                        for (uint32_t j = 0; j < groups->groups[i]->n; j++) {
                            AT_Triangle triangle = group->triangles[j];
                            // AT_Vec3 triangle_mid = triangle.aabb.midpoint;
                            // bool is_left = triangle_mid.x <= midpoint.x ||
                            //                triangle_mid.y <= midpoint.y ||
                            //                triangle_mid.z <= midpoint.z;
                            // if (!is_left) {
                            //     color = RED;
                            // } else {
                            //     color = BLUE;
                            // }
                            color.a = 100;
                            DrawTriangle3D(
                                (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                                (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                                (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                                color
                            );
                        }
                    }
                }
                EndMode3D();
                DrawFPS(10, 10);
            }
            EndDrawing();
        }

        CloseWindow();

        AT_model_destroy(model);
    } else {
        for (uint32_t i = 0; i < groups->n; i++) {
            printf("Group %d of size %d\n", i, groups->groups[i]->n);
        }
    }

    AT_triangle_groups_destroy(groups);
    free(ts);

    return 0;
}
