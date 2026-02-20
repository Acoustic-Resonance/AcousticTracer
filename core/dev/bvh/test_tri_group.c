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

#define SAMPLE_SIZE 300

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
        // bvh_config.mini_tree_size = (model->index_count / 3) / 16;
        bvh_config.mini_tree_size = 100;

        int t_count = model->index_count / 3;
        AT_Triangle **dim_arrs = malloc(sizeof(*dim_arrs) * 3);
        dim_arrs[0] = malloc(sizeof(AT_Triangle) * t_count);
        dim_arrs[1] = malloc(sizeof(AT_Triangle) * t_count);
        dim_arrs[2] = malloc(sizeof(AT_Triangle) * t_count);
        AT_BVH_sort_triangles(ts, t_count, dim_arrs);

        if (AT_trigroup_create(&tri_group, ts, dim_arrs, 0, model->index_count / 3) != AT_OK) {
            perror("Failed to create the triangle group");
            free(ts);
            return 1;
        }
    } else {
        uint32_t triangle_count = 7000000;
        bvh_config.mini_tree_size = 100;
        ts = (AT_Triangle *)malloc(sizeof(AT_Triangle) * triangle_count);
        for (uint32_t i = 0; i < triangle_count; i++) {
            AT_Triangle *triangle = &ts[i];
            triangle->v1 = AT_vec3(0.4f, 0.2f * i, 0.34f);
            triangle->v2 = AT_vec3(0.03f * i, 0.5f, 0.67f);
            triangle->v3 = AT_vec3(0.2f, 0.6f, 0.09f * i);
            triangle->aabb = AT_AABB_from_triangle(triangle);
        }
        int t_count = triangle_count;
        AT_Triangle **dim_arrs = malloc(sizeof(*dim_arrs) * 3);
        dim_arrs[0] = malloc(sizeof(AT_Triangle) * t_count);
        dim_arrs[1] = malloc(sizeof(AT_Triangle) * t_count);
        dim_arrs[2] = malloc(sizeof(AT_Triangle) * t_count);
        AT_BVH_sort_triangles(ts, t_count, dim_arrs);

        if (AT_trigroup_create(&tri_group, ts, dim_arrs, 0, triangle_count) != AT_OK) {
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
        Color cols[4] = {BLACK, LIGHTGRAY, DARKGRAY, WHITE};
        int idx[SAMPLE_SIZE];
        for (int i = 0; i < SAMPLE_SIZE; i++) {
            idx[i] = rand() % (groups->n + 1);
        }

        // AT_AABB aabb = {};
        // AT_model_to_AABB(&aabb, model);

        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Triangle group testing");
        SetTargetFPS(60);

        Camera3D cam = {.position = {5.0f, 5.0f, 5.0f}, .target = {0.0f, 0.0f, 0.0f}, .up = {0.0f, 1.0f, 0.0f}, .fovy = 60.0f, .projection = CAMERA_PERSPECTIVE};

        rlDisableBackfaceCulling();
        rlSetLineWidth(1.0f);

        uint32_t k = 0, index = idx[k], i = 0;
        while (!WindowShouldClose()) {
            UpdateCamera(&cam, CAMERA_FREE);
            if (IsKeyPressed(KEY_T)) {
                k = (k + 1) % SAMPLE_SIZE;
                index = idx[k];
            }
            if (IsKeyPressed(KEY_R)) {
                // printf("Decrease: %d\n", -1 % 3);
                if (k == 0) {
                    k = SAMPLE_SIZE;
                }
                k = (k - 1) % SAMPLE_SIZE;
                index = idx[k];
            }
            if (IsKeyPressed(KEY_Y)) {
                index = (index + 1) % groups->n;
            }
            if (IsKeyPressed(KEY_U)) {
                if (index == 0) {
                    index = groups->n;
                }
                index = (index - 1) % groups->n;
            }
            if (IsKeyPressed(KEY_E)) {
                AT_Vec3 midpoint = groups->groups[index]->aabb.midpoint;
                cam.target = (Vector3){
                    .x = midpoint.x,
                    .y = midpoint.y,
                    .z = midpoint.z,
                };
            }
            if (IsKeyPressed(KEY_F)) {
                AT_Vec3 midpoint = groups->groups[index]->triangles[0].aabb.midpoint;
                cam.target = (Vector3){
                    .x = midpoint.x,
                    .y = midpoint.y,
                    .z = midpoint.z,
                };
                cam.position = (Vector3){
                    .x = midpoint.x - 1,
                    .y = midpoint.y - 1,
                    .z = midpoint.z - 1,
                };
            }
            if (IsKeyPressed(KEY_Q)) {
                AT_Vec3 tri_mid = groups->groups[index]->triangles[++i % groups->groups[index]->n].aabb.midpoint;
                cam.position = (Vector3){
                    .x = tri_mid.x,
                    .y = tri_mid.y,
                    .z = tri_mid.z,
                };
            }

            BeginDrawing();
            {
                ClearBackground(BLACK);
                BeginMode3D(cam);
                {
                    // for (uint32_t i = 0; i < groups->n; i++) {
                    // // if (groups->groups[i]->n < 40000) {
                    // //     continue;
                    // // }
                    // Color color = colors[i % 16];
                    // AT_TriGroup *group = groups->groups[i];
                    // if (groups->groups[i]->n > 100) {
                    // i = (i + 1) % groups->n;
                    // continue;
                    // }
                    AT_AABB aabb = groups->groups[index]->aabb;
                    // AT_Vec3 midpoint = aabb.midpoint;
                    DrawBoundingBox(
                        (BoundingBox){
                            (Vector3){aabb.min.x, aabb.min.y, aabb.min.z},
                            (Vector3){aabb.max.x, aabb.max.y, aabb.max.z}
                        },
                        BLUE
                    );
                    // // DrawSphere(
                    // //     (Vector3){
                    // //         midpoint.x, midpoint.y, midpoint.z
                    // //     },
                    // //     0.3f,
                    // //     color
                    // // );
                    for (uint32_t j = 0; j < groups->groups[index]->n; j++) {
                        AT_Triangle triangle = groups->groups[index]->triangles[j];
                        // AT_Vec3 triangle_mid = triangle.aabb.midpoint;
                        // bool is_left = triangle_mid.x <= midpoint.x ||
                        //                triangle_mid.y <= midpoint.y ||
                        //                triangle_mid.z <= midpoint.z;
                        // if (!is_left) {
                        //     color = RED;
                        // } else {
                        //     color = BLUE;
                        // }
                        // Color color = cols[j % 4];
                        Color color = colors[j % sizeof(colors)];
                        // Color color = GREEN;
                        // color.a = 100;
                        DrawTriangle3D(
                            (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                            (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                            (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                            color
                        );
                        // }
                    }
                }
                EndMode3D();
                DrawFPS(10, 10);
                char txt[50];
                sprintf(txt, "Group %d has %d triangles", index, groups->groups[index]->n);
                DrawText(txt, 10, 50, 18, GREEN);
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
