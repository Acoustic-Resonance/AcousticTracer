#include "../src/at_internal.h"
#include "../src/at_ray.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"
#include "raylib.h"
#include "rlgl.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <float.h>

#define MAX_RAYS 20
#define MAX_COLORS_COUNT    21

Color colors[MAX_COLORS_COUNT] = {
    DARKGRAY, MAROON, ORANGE, DARKGREEN, DARKBLUE, DARKPURPLE, DARKBROWN,
    GRAY, RED, GOLD, LIME, BLUE, VIOLET, BROWN, LIGHTGRAY, PINK, YELLOW,
    GREEN, SKYBLUE, PURPLE, BEIGE };

AT_Triangle *AT_model_get_triangles(const AT_Model *model)
{
    uint32_t triangle_count = model->index_count / 3;
    AT_Triangle *ts = (AT_Triangle*)malloc(sizeof(AT_Triangle) * triangle_count);
    for (uint32_t i = 0; i < triangle_count; i++) {
        ts[i] = (AT_Triangle){
            .v1 = model->vertices[model->indices[i*3 + 0]],
            .v2 = model->vertices[model->indices[i*3 + 1]],
            .v3 = model->vertices[model->indices[i*3 + 2]]
        };
    }
    return ts;
}

int main()
{
    const char *filepath = "../assets/glb/L_room.gltf";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Failed to create model\n");
        return 1;
    }

    AT_Ray rays[MAX_RAYS] = {0};

    //init rays
    for (uint32_t i = 0; i < MAX_RAYS; i++) {
        float t = (float)i / (MAX_RAYS - 1);
        float angle = -PI/2 + t * PI;
        rays[i] = AT_ray_init(
            (AT_Vec3){0},
            (AT_Vec3){cosf(angle), 0.1f, sinf(angle)},
            i);
    }

    uint32_t t_count = model->index_count / 3;
    AT_Triangle *ts = AT_model_get_triangles(model);

    //iterate rays
    for (uint32_t i = 0; i < MAX_RAYS; i++) {
        AT_Ray *ray = &rays[i];
        while (true) {
            AT_Ray closest = AT_ray_init((AT_Vec3){ FLT_MAX, FLT_MAX, FLT_MAX }, (AT_Vec3){0}, 0);
            bool intersects = false;
            for (uint32_t j = 0; j < t_count; j++) {
                if(AT_ray_triangle_intersect(ray, &ts[j], &closest)) intersects = true;
            }
            if (!intersects) break;
            AT_Ray *child = malloc(sizeof(AT_Ray));
            *child = closest;
            ray->child = child;
            ray = ray->child;
        }
    }

    printf("Initializing Window\n");
    InitWindow(1280, 720, "Model Ray Test");

    SetTargetFPS(90);

    Camera3D camera = {
        .position = { 10.0f, 10.0f, 10.0f },
        .target = { 0.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fovy = 60.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    rlDisableBackfaceCulling();

    uint32_t ssn = 0;

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);
        if (IsKeyPressed(KEY_T)) {
           char filename[64];
           sprintf(filename, "../assets/images/screenshot_%03d.png", ssn++);
           TakeScreenshot(filename);
        }
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
            {
                //draw all rays
                for (uint32_t i = 0; i < MAX_RAYS; i++) {
                    DrawSphere((Vector3){
                    rays[i].origin.x,
                    rays[i].origin.y,
                    rays[i].origin.z},
                    0.1, RED);

                    AT_Ray *curr = &rays[i];
                    while (curr->child) {
                        curr = curr->child;
                        DrawSphere(
                            (Vector3){
                                curr->origin.x,
                                curr->origin.y,
                                curr->origin.z,
                            }, 0.01, BLUE
                        );
                        if (curr->child) {
                            DrawLine3D((Vector3){curr->origin.x, curr->origin.y, curr->origin.z}, (Vector3){curr->child->origin.x, curr->child->origin.y, curr->child->origin.z} , (Color)colors[i%MAX_COLORS_COUNT]);
                        }
                    }

                    if (rays[i].child) {
                        DrawLine3D((Vector3){rays[i].origin.x, rays[i].origin.y, rays[i].origin.z}, (Vector3){rays[i].child->origin.x, rays[i].child->origin.y, rays[i].child->origin.z} , RED);
                    } else {
                        DrawRay((Ray){
                        (Vector3){rays[i].origin.x, rays[i].origin.y, rays[i].origin.z},
                        (Vector3){rays[i].direction.x, rays[i].direction.y, rays[i].direction.z}
                        }, RED);
                    }
                }

                for (uint32_t i = 0; i < t_count; i++) {
                    DrawTriangle3D(
                        (Vector3){ts[i].v2.x, ts[i].v2.y, ts[i].v2.z},
                        (Vector3){ts[i].v1.x, ts[i].v1.y, ts[i].v1.z},
                        (Vector3){ts[i].v3.x, ts[i].v3.y, ts[i].v3.z},
                        (Color)colors[i%MAX_COLORS_COUNT]);
                }

                DrawGrid(10, 1.0f);
            }
            EndMode3D();
            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();
    free(ts);
    AT_model_destroy(model);
    for (uint32_t i = 0; i < MAX_RAYS; i++) {
       AT_ray_destroy(rays[i].child);
    }
    return 0;
}
