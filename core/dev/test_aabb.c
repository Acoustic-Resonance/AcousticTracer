#include "../src/at_aabb.h"
#include "../src/at_internal.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"
#include "raylib.h"
#include <float.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

AT_Triangle get_triangle_n_from_model(const AT_Model *model, uint32_t i)
{
    AT_Triangle tri = (AT_Triangle){
        .v1 = {model->vertices[model->indices[i * 3]].x,
               model->vertices[model->indices[i * 3]].y,
               model->vertices[model->indices[i * 3]].z},
        .v2 = {model->vertices[model->indices[i * 3 + 1]].x,
               model->vertices[model->indices[i * 3 + 1]].y,
               model->vertices[model->indices[i * 3 + 1]].z},
        .v3 = {model->vertices[model->indices[i * 3 + 2]].x,
               model->vertices[model->indices[i * 3 + 2]].y,
               model->vertices[model->indices[i * 3 + 2]].z}};
    tri.aabb = AT_AABB_from_triangle(&tri);

    return tri;
}

int main()
{
    const char *filepath = "../assets/glb/L_room.glb";
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
                AT_Triangle *triangles;
                if (AT_model_get_triangles(&triangles, model) != AT_OK) {
                    perror("Error getting triangles from the given model");
                    return 1;
                }
                for (uint32_t i = 0; i < model->index_count / 3; i++) {
                    AT_Triangle triangle = triangles[i];
                    DrawTriangle3D(
                        (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                        (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                        (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                        (Color){.a = 1});
                    DrawSphere((Vector3){triangle.aabb.midpoint.x, triangle.aabb.midpoint.y, triangle.aabb.midpoint.z}, 0.05f, RED);
                    DrawBoundingBox(
                        (BoundingBox){
                            (Vector3){triangle.aabb.min.x, triangle.aabb.min.y, triangle.aabb.min.z},
                            (Vector3){triangle.aabb.max.x, triangle.aabb.max.y, triangle.aabb.max.z}},
                        BLUE);
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
