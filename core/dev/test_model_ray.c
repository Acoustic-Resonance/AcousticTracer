#include "../src/at_internal.h"
#include "../src/at_ray.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"
#include "../external/raylib.h"
#include "raylib.h"

#include <stdint.h>
#include <stdio.h>
#include <float.h>

#define MAX_RAYS 50
#define MAX_COLORS_COUNT    21

const char *colorNames[MAX_COLORS_COUNT] = {
     "DARKGRAY", "MAROON", "ORANGE", "DARKGREEN", "DARKBLUE", "DARKPURPLE",
     "DARKBROWN", "GRAY", "RED", "GOLD", "LIME", "BLUE", "VIOLET", "BROWN",
     "LIGHTGRAY", "PINK", "YELLOW", "GREEN", "SKYBLUE", "PURPLE", "BEIGE" };

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

AT_RayHit AT_ray_get_closest_hit(const AT_Ray *ray)
{
    AT_RayHit closest_hit = {{0}, {0}, FLT_MAX};
    float smallest_dist = FLT_MAX;
    for (uint32_t i = 0; i < ray->hits.count; i++) {
        float curr_dist = AT_vec3_distance_sq(ray->origin, ray->hits.items[i].position);
        if (curr_dist < smallest_dist) {
            smallest_dist = curr_dist;
            closest_hit = ray->hits.items[i];
        }
    }
    //*dist = smallest_dist;
    return closest_hit;

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
        rays[i] = AT_ray_init(
            (AT_Vec3){(float)i*0.05-1, 0.5, 10.0f},
            (AT_Vec3){i*-0.001, 0.0f, -1.0f},
            0);
    }

    uint32_t t_count = model->index_count / 3;
    AT_Triangle *ts = AT_model_get_triangles(model);

    //iterate rays
    for (uint32_t i = 0; i < MAX_RAYS; i++) {
        AT_RayHit hit = {{0}, {0}, FLT_MAX};
        AT_RayHitList hits;
        AT_da_init(&hits);
        for (uint32_t j = 0; j < t_count; j++) {
            printf("Checking Ray %i\n", i);
            if (AT_ray_triangle_intersect(&rays[i], &ts[j], &hit)) {
                //AT_ray_add_hit(&rays[i], hit);
                AT_da_append(&hits, hit);
                printf("HIT! Ray %i {%.2f, %.2f, %.2f}\n",
                    i,
                    hit.position.x,
                    hit.position.y,
                    hit.position.z);
            }
        }
        bool min_found = false;
        AT_RayHit closest_hit;
        for (uint32_t k = 0; k < hits.count; k++) {
            float min_dist = FLT_MAX;

            float curr_dist = AT_vec3_distance_sq(closest_hit.position, hits.items[k].position);
            if (curr_dist < min_dist) {
                min_dist = curr_dist;
                closest_hit = hits.items[k];
                min_found = true;
            }
        }
        if (min_found) {
            printf("ADDING HIT FOR RAY %i {%.2f, %.2f, %.2f}\n", i,
                closest_hit.position.x, closest_hit.position.y, closest_hit.position.z);
            AT_ray_add_hit(&rays[i], closest_hit);
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

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);
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

                    DrawRay((Ray){
                    (Vector3){rays[i].origin.x, rays[i].origin.y, rays[i].origin.z},
                    (Vector3){rays[i].direction.x, rays[i].direction.y, rays[i].direction.z}
                    }, RED);

                    for (uint32_t j = 0; j < rays[i].hits.count; j++) {
                        DrawSphere((Vector3){
                                rays[i].hits.items[j].position.x,
                                rays[i].hits.items[j].position.y,
                                rays[i].hits.items[j].position.z},
                                0.1, RED);
                    }
                }

                for (uint32_t i = 0; i < t_count; i++) {
                    DrawTriangle3D(
                        (Vector3){ts[i].v1.x, ts[i].v1.y, ts[i].v1.z},
                        (Vector3){ts[i].v2.x, ts[i].v2.y, ts[i].v2.z},
                        (Vector3){ts[i].v3.x, ts[i].v3.y, ts[i].v3.z},
                        GREEN);
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
    return 0;
}
