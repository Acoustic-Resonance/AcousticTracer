#include "../src/at_internal.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"
#include "raylib.h"
#include <float.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

AT_Triangle get_triangle_n_from_model(const AT_Model *model,
                                      uint32_t i)
{
    return (AT_Triangle){{model->vertices[model->indices[i * 3]].x,
                          model->vertices[model->indices[i * 3]].y,
                          model->vertices[model->indices[i * 3]].z},
                         {model->vertices[model->indices[i * 3 + 1]].x,
                          model->vertices[model->indices[i * 3 + 1]].y,
                          model->vertices[model->indices[i * 3 + 1]].z},
                         {model->vertices[model->indices[i * 3 + 2]].x,
                          model->vertices[model->indices[i * 3 + 2]].y,
                          model->vertices[model->indices[i * 3 + 2]].z}};
}

void draw_model(const AT_Model *model)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Test");
    Camera3D camera = {.position = {10.0f, 10.0f, 10.0f},
                       .target = {0.0f, 0.0f, 0.0f},
                       .up = {0.0f, 1.0f, 0.0f},
                       .fovy = 60.0f,
                       .projection = CAMERA_PERSPECTIVE};
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        for (uint32_t i = 0; i < model->index_count / 3; i++) {
            AT_Triangle triangle = get_triangle_n_from_model(model, i);
            DrawTriangle3D(
                (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z}, GREEN);
        }
        DrawGrid(10, 1.0f);
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
}
int main()
{
    const char *filepath = "../assets/glb/L_room.gltf";
    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Failed to create model.\n");
        return 1;
    }

    AT_AABB aabb = {};
    AT_model_to_AABB(&aabb, model);
    AT_Vec3 min_vec = aabb.min;
    AT_Vec3 max_vec = aabb.max;
    printf("%.2f, %.2f, %.2f", min_vec.x, min_vec.y, min_vec.z);
    printf("%.2f, %.2f, %.2f", max_vec.x, max_vec.y, max_vec.z);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AABB testing");
    SetTargetFPS(60);

    Camera3D cam = {.position = {10.0f, 10.0f, 10.0f},
                    .target = {0.0f, 0.0f, 0.0f},
                    .up = {0.0f, 1.0f, 0.0f},
                    .fovy = 60.0f,
                    .projection = CAMERA_PERSPECTIVE};

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_FREE);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(cam);
            {
                DrawBoundingBox(
                    (BoundingBox){
                        (Vector3){min_vec.x, min_vec.y, min_vec.z},
                        (Vector3){max_vec.x, max_vec.y, max_vec.z},
                    },
                    RED);
                for (uint32_t i = 0; i < model->index_count / 3; i++) {
                    AT_Triangle triangle = get_triangle_n_from_model(model, i);
                    DrawTriangle3D(
                        (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                        (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                        (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                        GREEN);
                }
                DrawGrid(10, 1.0f);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();

    AT_model_destroy(model);

    return 0;
}
