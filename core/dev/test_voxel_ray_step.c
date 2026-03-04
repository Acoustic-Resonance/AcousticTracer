#include "acoustic/at.h"
#include "acoustic/at_model.h"
#include "acoustic/at_result.h"
#include "../src/at_voxel.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../external/raylib.h"
#include "rlgl.h"

Color cols[3] = {BLACK, LIGHTGRAY, DARKGRAY};

static float AT_voxel_get_energy_sum(AT_Voxel *voxel, uint32_t index)
{
    float sum = 0.0f;
    for (size_t i = 0; i <= index; i++) {
        if (i >= voxel->count) break;
        sum += voxel->items[i];
    }
    return sum;
}

static float AT_voxel_get_energy_curr(AT_Voxel *voxel, uint32_t index)
{
    if (index >= voxel->count) return 0.0f;
    return voxel->items[index];
}

int main()
{
    printf("Voxel Ray Step\n");

    const char *filepath = "../assets/glb/Sponza.glb";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Error creating model\n");
        return 1;
    }

    // for (uint32_t i = 0; i < model->vertex_count; i++) {
    //      model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.05f);
    // }

    int num_sources = 1;
    AT_Source s1 = {
        .direction = {{0.2f, -0.05f, -0.1f}},
        .intensity = 1000.0f,
        .position = {{0}}
    };

    AT_SceneConfig conf = {
        .environment = model,
        .material = AT_MATERIAL_CONCRETE,
        .num_sources = num_sources,
        .sources = &s1
    };

    AT_Scene *scene = NULL;
    if (AT_scene_create(&scene, &conf) != AT_OK) {
        fprintf(stderr, "Error creating scene\n");
        return 1;
    }

    // scene->world_AABB.min = AT_vec3_scale(scene->world_AABB.min, 2);
    // scene->world_AABB.max = AT_vec3_scale(scene->world_AABB.max, 2);

    AT_Settings settings = {
        .fps = 60,
        .num_rays = 1000000,
        .voxel_size = 0.1f
    };

    AT_Simulation *sim = NULL;
    if (AT_simulation_create(&sim, scene, &settings) != AT_OK) {
        fprintf(stderr, "Error creating simulation\n");
        return 1;
    }

    if (AT_simulation_run(sim) != AT_OK) {
        fprintf(stderr, "Error running simulation\n");
        return 1;
    }

    uint32_t bin_count = AT_voxel_get_num_bins(sim);
    uint32_t curr_bin = 0;
    printf("BIN COUNT: %i\n", bin_count);

    printf("VOXEL COUNT: %i\n", sim->num_voxels);

    InitWindow(1280, 720, "Voxel Ray Test");

        SetTargetFPS(60);

        Camera3D camera = {
            .position = { 0.0f, 15.0f, 30.f },
            .target = { 0.0f, 0.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        };
        rlDisableBackfaceCulling();
        rlDisableDepthMask();

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
                    /*
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
                            while (curr) {
                                if (curr->child) {
                                    DrawLine3D(
                                        (Vector3){curr->origin.x, curr->origin.y, curr->origin.z},
                                        (Vector3){curr->child->origin.x, curr->child->origin.y, curr->child->origin.z},
                                        PURPLE
                                    );
                                } else if (!curr->has_died) {
                                    DrawRay((Ray){
                                        (Vector3){curr->origin.x, curr->origin.y, curr->origin.z},
                                        (Vector3){curr->direction.x, curr->direction.y, curr->direction.z}
                                    }, GREEN);
                                }
                                curr = curr->child;
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
                                }, BLUE);
                            }

                        }
                    }
                    */

                    DrawBoundingBox((BoundingBox){
                        (Vector3){
                            scene->world_AABB.min.x,
                            scene->world_AABB.min.y,
                            scene->world_AABB.min.z
                        },
                        (Vector3){
                            scene->world_AABB.max.x,
                            scene->world_AABB.max.y,
                            scene->world_AABB.max.z
                        }},
                        RED);

                    for (uint32_t i = 0; i < t_count; i++) {
                            Vector3 v1 = {ts[i].v1.x, ts[i].v1.y, ts[i].v1.z};
                            Vector3 v2 = {ts[i].v2.x, ts[i].v2.y, ts[i].v2.z};
                            Vector3 v3 = {ts[i].v3.x, ts[i].v3.y, ts[i].v3.z};
                            DrawLine3D(v1, v2, BLACK);
                            DrawLine3D(v2, v3, BLACK);
                            DrawLine3D(v3, v1, BLACK);
                        }

                    if (IsKeyDown(KEY_L)) curr_bin++;
                    if (IsKeyDown(KEY_K) && curr_bin > 0) curr_bin--;

                    //draw voxels
                    for (uint32_t z = 0; z < sim->grid_dimensions.z; z++) {
                        for (uint32_t y = 0; y < sim->grid_dimensions.y; y++) {
                            for (uint32_t x = 0; x < sim->grid_dimensions.x; x++) {
                                uint32_t i =
                                    z * sim->grid_dimensions.y * sim->grid_dimensions.x +
                                    y * sim->grid_dimensions.x +
                                    x;

                                AT_Voxel *v = &sim->voxel_grid[i];

                                //printf("CURR BIN(%i): %i\n", curr_bin, curr_bin%bin_count);
                                //float energy = AT_voxel_get_energy_curr(v, curr_bin%bin_count);
                                float energy = AT_voxel_get_energy_curr(v, curr_bin%bin_count);
                                //float energy = AT_voxel_get_energy(v, 2);
                                //printf("ENERGY: %f\n", energy);
                                //float energy = AT_voxel_get_energy(v, 2);

                                Vector3 pos = {
                                    sim->origin.x + (x + 0.5f) * sim->voxel_size,
                                    sim->origin.y + (y + 0.5f) * sim->voxel_size,
                                    sim->origin.z + (z + 0.5f) * sim->voxel_size,
                                };

                                if (energy > 0.0f) {
                                    float normalized_energy = energy * sim->num_rays;
                                    float alpha = fminf(normalized_energy, 1.0f);
                                    //printf("VOXEL (%i): %f\n", i, v->items[curr_bin%bin_count]);
                                    DrawCubeV(
                                        pos,
                                        (Vector3){sim->voxel_size, sim->voxel_size, sim->voxel_size},
                                        Fade(RED, alpha)
                                    );
                                    //AT_voxel_print(v);
                                    //printf("Voxel (%i): Num Bins: %zu Energy: %f\t\n", i, v->count, energy);
                                    continue;

                                 } // else {
                                //     DrawCubeV(
                                //          pos,
                                //          (Vector3){sim->voxel_size, sim->voxel_size, sim->voxel_size},
                                //          Fade(BLUE, 1.0f/100)
                                //     );
                                // }

                            }
                        }
                    }
                    DrawGrid(sim->voxel_size, 1.0f);
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
