#include "../src/at_internal.h"
#include "../src/at_ray.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "acoustic/at_model.h"
#include "../external/raylib.h"
#include "raylib.h"

#include <stdint.h>
#include <stdio.h>

Mesh model_to_rl_mesh(const AT_Model *model)
{
    Mesh mesh = {0};

    mesh.vertexCount   = model->vertex_count;
    mesh.triangleCount = model->index_count / 3;

    mesh.vertices = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.normals  = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.texcoords = MemAlloc(sizeof(float) * 2 * mesh.vertexCount);
    mesh.indices  = MemAlloc(sizeof(unsigned int) * model->index_count);

    for (int i = 0; i < model->vertex_count; i++) {
        mesh.vertices[i*3 + 0] = model->vertices[i].x;
        mesh.vertices[i*3 + 1] = model->vertices[i].y;
        mesh.vertices[i*3 + 2] = model->vertices[i].z;
    }

    for (int i = 0; i < model->vertex_count; i++) {
        mesh.normals[i*3 + 0] = model->normals[i].x;
        mesh.normals[i*3 + 1] = model->normals[i].y;
        mesh.normals[i*3 + 2] = model->normals[i].z;
    }

    for (int i = 0; i < model->index_count; i++) {
        mesh.indices[i] = model->indices[i];
    }

    UploadMesh(&mesh, false);
    return mesh;
}

AT_Triangle *AT_model_get_triangles(const AT_Model *model)
{
    uint32_t triangle_count = model->index_count / 3;
    AT_Triangle *ts = (AT_Triangle*)malloc(sizeof(AT_Triangle) * triangle_count);
    for (uint32_t i = 0; i < triangle_count; i++) {
        ts[i] = (AT_Triangle){
            .v1 = model->vertices[i*3 + 0],
            .v2 = model->vertices[i*3 + 1],
            .v3 = model->vertices[i*3 + 2]
        };
    }
    return ts;
}

int main()
{
    const char *filepath = "../assets/glb/box_room.gltf";

    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        fprintf(stderr, "Failed to create model\n");
        return 1;
    }

    AT_Ray ray = AT_ray_init(
        (AT_Vec3){0.5f, 0.5, 10.0f},
        (AT_Vec3){0.0f, 0.0f, -1.0f},
        0
    );

    uint32_t t_count = model->index_count / 3;
    AT_Triangle *ts = AT_model_get_triangles(model);

    AT_RayHit hit = {0};

    for (uint32_t i = 0; i < t_count; i++) {
        if (AT_ray_triangle_intersect(&ray, &ts[i], &hit)) {
            AT_ray_add_hit(&ray, hit);
            printf("HIT!\n");
        }
    }

    printf("Initializing Window\n");
    InitWindow(1280, 720, "Model Ray Test");

    printf("Loading rl_model\n");
    Model rl_model = LoadModelFromMesh(model_to_rl_mesh(model));

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
                DrawRay((Ray){
                (Vector3){ray.origin.x, ray.origin.y, ray.origin.z},
                (Vector3){ray.direction.x, ray.direction.y, ray.direction.z}
                }, RED);
                DrawModel(rl_model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1, GREEN);
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
