#include "../src/at_bvh.h"
#include "../src/at_internal.h"
#include "../src/at_trigroup.h"
#include "../src/at_utils.h"
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
    AT_BVHConfig bvh_config;

    const char *filepath = "../assets/glb/Sponza.glb";
    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        perror("Failed to create model");
        return 1;
    }

    for (uint32_t i = 0; i < model->vertex_count; i++) {
        model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.05);
    }

    AT_TriangleArrays *triangle_arrs = NULL;
    if (AT_triangle_arrays_create(&triangle_arrs, model) != AT_OK) {
        perror("Failed to create triangle arrays");
        AT_model_destroy(model);
        return 1;
    }

    // bvh_config.mini_tree_size = (model->index_count / 3) / 16;
    bvh_config.mini_tree_size = 100;

    uint32_t num_tri = model->index_count / 3;

    AT_TriangleGroups *groups = NULL;
    if (AT_triangle_groups_create(&groups, num_tri) != AT_OK) {
        perror("Failed to create the triangle groups holder");
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }
    if (AT_trigroup_split(triangle_arrs, num_tri, groups, bvh_config.mini_tree_size) != AT_OK) {
        perror("Failed to split the triangle group");
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }

    if (argc > 2) {
        Color colors[16] = {RED, BLUE, GREEN, PURPLE, PINK, LIME, BROWN, MAROON, MAGENTA, ORANGE, GOLD, YELLOW, DARKGREEN, SKYBLUE, DARKBLUE, VIOLET};
        Color cols[4] = {BLACK, LIGHTGRAY, DARKGRAY, WHITE};
        int idx[SAMPLE_SIZE];
        for (int i = 0; i < SAMPLE_SIZE; i++) {
            idx[i] = rand() % (groups->num_groups);
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
                index = (index + 1) % groups->num_groups;
            }
            if (IsKeyPressed(KEY_U)) {
                if (index == 0) {
                    index = groups->num_groups;
                }
                index = (index - 1) % groups->num_groups;
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
                AT_Vec3 midpoint = AT_get_triangle(groups->groups[index], 3, 0).aabb.midpoint;
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
                AT_Vec3 tri_mid = AT_get_triangle(groups->groups[index], 3, ++i % groups->groups[index]->num_tri).aabb.midpoint;
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
                    for (uint32_t j = 0; j < groups->groups[index]->num_tri; j++) {
                        AT_Triangle triangle = AT_get_triangle(groups->groups[index], 3, j);
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
                        Color color = colors[j % (sizeof(colors) / sizeof(colors[0]))];
                        // Color color = GREEN;
                        // color.a = 100;
                        DrawTriangle3D(
                            (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                            (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                            (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                            color
                        );
                        DrawSphere(
                            (Vector3){triangle.aabb.midpoint.x, triangle.aabb.midpoint.y, triangle.aabb.midpoint.z},
                            0.05f,
                            RED
                        );
                        // }
                    }
                }
                EndMode3D();
                DrawFPS(10, 10);
                char txt[50];
                sprintf(txt, "Group %d has %d triangles", index, groups->groups[index]->num_tri);
                DrawText(txt, 10, 50, 18, GREEN);
            }
            EndDrawing();
        }

        CloseWindow();

        AT_model_destroy(model);
    } else {
        for (uint32_t i = 0; i < groups->num_groups; i++) {
            printf("Group %d of size %d\n", i, groups->groups[i]->num_tri);
        }
    }

    AT_triangle_groups_destroy(groups);
    AT_triangle_arrays_destroy(triangle_arrs);

    return 0;
}
