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

#define MAX_RAYS 10
#define MAX_COLORS_COUNT    21

Color colors[MAX_COLORS_COUNT] = {
    DARKGRAY, MAROON, ORANGE, DARKGREEN, DARKBLUE, DARKPURPLE, DARKBROWN,
    GRAY, RED, GOLD, LIME, BLUE, VIOLET, BROWN, LIGHTGRAY, PINK, YELLOW,
    GREEN, SKYBLUE, PURPLE, BEIGE };

Color cols[3] = {BLACK, LIGHTGRAY, DARKGRAY};

int main()
{
    const char *filepath = "../assets/glb/Sponza.glb";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Failed to create model\n");
        return 1;
    }

    for (uint32_t i = 0; i < model->vertex_count; i++) {
        model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.01);
    }

    AT_Ray rays[MAX_RAYS] = {0};

    //init rays
    for (uint32_t i = 0; i < MAX_RAYS; i++) {
        rays[i] = AT_ray_init(
            (AT_Vec3){0},
            (AT_Vec3){i*(1.0f/MAX_RAYS), 0.1f, -1.0f},
            i);
    }

    uint32_t t_count = model->index_count / 3;
    AT_Triangle *ts = NULL;
    if (AT_model_get_triangles(&ts, model) != AT_OK) {
        fprintf(stderr, "Failed to load triangles from model\n");
        return 1;
    }

    //iterate rays
    for (uint32_t i = 0; i < MAX_RAYS; i++) {
        uint32_t count = 0;
        AT_Ray *ray = &rays[i];
        while (count++ < 5) {
            AT_Ray closest = AT_ray_init((AT_Vec3){ FLT_MAX, FLT_MAX, FLT_MAX }, (AT_Vec3){0}, 0);
            bool intersects = false;
            for (uint32_t j = 0; j < t_count; j++) {
                if(AT_ray_triangle_intersect(ray, &ts[j], &closest)) intersects = true;
            }
            if (!intersects) break;
            AT_Ray *child = malloc(sizeof(AT_Ray));
            *child = closest;
            child->child = NULL;
            ray->child = child;
            ray = ray->child;
        }
    }

    printf("Initializing Window\n");
    InitWindow(1280, 720, "Model Ray Test");

    SetTargetFPS(60);

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
                            DrawLine3D((Vector3){curr->origin.x, curr->origin.y, curr->origin.z}, (Vector3){curr->child->origin.x, curr->child->origin.y, curr->child->origin.z} , PURPLE);
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
                        (Color)cols[i%3]);
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
