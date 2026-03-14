#include "acoustic/at.h"
#include "../src/at_internal.h"
#include "acoustic/at_model.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../external/raylib.h"

Color cols[3] = {BLACK, LIGHTGRAY, DARKGRAY};

int main()
{
    printf("Simulation Run Rays :|\n");

    const char *filepath = "../assets/glb/Sponza.glb";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Error creating model\n");
        return 1;
    }

    for (uint32_t i = 0; i < model->vertex_count; i++) {
        model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.01);
    }

    AT_Source s1 = {
        .direction = {0.2, -0.1, 0},
        .intensity = 50.0,
        .position = {1, 2, 0}
    };

    AT_Source s2 = {
        .direction = {-0.2, 0.1, 0},
        .intensity = 50.0,
        .position = {0, 2, 1}
    };

    int num_sources = 2;
    AT_Source sources[num_sources];
    sources[0] = s1;
    sources[1] = s2;

    AT_SceneConfig conf = {
        .environment = model,
        .material = AT_MATERIAL_CONCRETE,
        .num_sources = num_sources,
        .sources = sources
    };

    AT_Scene *scene = NULL;
    if (AT_scene_create(&scene, &conf) != AT_OK) {
        fprintf(stderr, "Error creating scene\n");
        return 1;
    }

    AT_Settings settings = {
        .fps = 60,
        .num_rays = 10,
        .voxel_size = 10
    };

    AT_Simulation *sim = NULL;
    if (AT_simulation_create(&sim, scene, &settings) != AT_OK) {
        //we need some way to print what kind of error it was (alloc, param..)
        fprintf(stderr, "Error creating simulation\n");
        return 1;
    }

    if (AT_simulation_run(sim) != AT_OK) {
        fprintf(stderr, "Error running simulation\n");
        return 1;
    }

    InitWindow(1280, 720, "Model Ray Test");

        SetTargetFPS(60);

        Camera3D camera = {
            .position = { 10.0f, 10.0f, 10.0f },
            .target = { 0.0f, 0.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        };

        uint32_t t_count = sim->scene->environment->index_count / 3;
        AT_Triangle *ts = NULL;
        if (AT_model_get_triangles(&ts, sim->scene->environment) != AT_OK) {
            fprintf(stderr, "Failed to get triangles from model\n");
            return 1;
        }

        while (!WindowShouldClose())
        {
            UpdateCamera(&camera, CAMERA_FREE);
            BeginDrawing();
            {
                ClearBackground(RAYWHITE);
                BeginMode3D(camera);
                {
                    //draw all rays
                    AT_Ray *rays = sim->rays;
                    for (uint32_t s = 0; s < sim->scene->num_sources; s++) {
                        for (uint32_t i = 0; i < sim->num_rays; i++) {
                            uint32_t ray_idx = s * sim->num_rays + i;
                            AT_Ray ray = rays[ray_idx];
                            DrawSphere((Vector3){
                            ray.origin.x,
                            ray.origin.y,
                            ray.origin.z},
                            0.1, RED);

                            AT_Ray *curr = &ray;
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
                                    DrawLine3D(
                                        (Vector3){curr->origin.x, curr->origin.y, curr->origin.z},
                                        (Vector3){curr->child->origin.x, curr->child->origin.y, curr->child->origin.z},
                                        PURPLE);
                                }
                            }

                            if (ray.child) {
                                DrawLine3D(
                                    (Vector3){ray.origin.x, ray.origin.y, ray.origin.z},
                                    (Vector3){ray.child->origin.x, ray.child->origin.y, ray.child->origin.z},
                                    RED);
                            } else {
                                DrawRay((Ray){
                                (Vector3){ray.origin.x, ray.origin.y, ray.origin.z},
                                (Vector3){ray.direction.x, ray.direction.y, ray.direction.z}
                                }, RED);
                            }
                        }
                    }

                    for (uint32_t i = 0; i < t_count; i++) {
                        DrawTriangle3D(
                            (Vector3){ts[i].v1.x, ts[i].v1.y, ts[i].v1.z},
                            (Vector3){ts[i].v2.x, ts[i].v2.y, ts[i].v2.z},
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
    AT_simulation_destroy(sim);
    AT_scene_destroy(scene);
    AT_model_destroy(model);

    return 0;
}
