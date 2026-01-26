#include "../src/at_internal.h"
#include "acoustic/at_model.h"
#include "../external/raylib.h"

#include <stdio.h>

void print_model_head(const AT_Model *model)
{
    printf("Vertex count: %lu\n", model->vertex_count);
    printf("X: %f\n", model->vertices[0].x);
    printf("Y: %f\n", model->vertices[0].y);
    printf("Z: %f\n", model->vertices[0].z);

    printf("Index count: %lu\n", model->index_count);
    printf("%d, %d, %d\n", model->indices[0], model->indices[1], model->indices[2]);

    printf("Normal 1:\n");
    printf("X: %f\n", model->normals[0].x);
    printf("Y: %f\n", model->normals[0].y);
    printf("Z: %f\n", model->normals[0].z);
}

Mesh model_to_rl_mesh(const AT_Model *model)
{
    Mesh mesh = {0};

    mesh.vertexCount   = model->vertex_count;
    mesh.triangleCount = model->index_count / 3;

    // Allocate Raylib buffers
    mesh.vertices = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.normals  = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.indices  = MemAlloc(sizeof(unsigned int) * model->index_count);

    // Vertices
    for (int i = 0; i < model->vertex_count; i++)
    {
        mesh.vertices[i*3 + 0] = model->vertices[i].x;
        mesh.vertices[i*3 + 1] = model->vertices[i].y;
        mesh.vertices[i*3 + 2] = model->vertices[i].z;
    }

    // Normals
    for (int i = 0; i < model->vertex_count; i++)
    {
        mesh.normals[i*3 + 0] = model->normals[i].x;
        mesh.normals[i*3 + 1] = model->normals[i].y;
        mesh.normals[i*3 + 2] = model->normals[i].z;
    }

    for (int i = 0; i < model->index_count; i++)
    {
        mesh.indices[i] = model->indices[i];
    }

    UploadMesh(&mesh, false);
    return mesh;
}

void draw_model(const AT_Model *model)
{
    InitWindow(1280, 720, "Test");
    Camera3D camera = {
        .position = { 10.0f, 10.0f, 10.0f },
        .target = { 0.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fovy = 60.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    SetTargetFPS(60);

    Model rl_model = LoadModelFromMesh(model_to_rl_mesh(model));

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawModel(rl_model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1, GREEN);
                DrawGrid(10, 1.0f);
            EndMode3D();
            DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
}
int main()
{
    const char *filepath = "../assets/glb/box_room.gltf";
    AT_Model *model = NULL;
    if(AT_model_create(&model, filepath) != AT_OK) {
        perror("Failed to create model");
        return 1;
    }

    draw_model(model);

    AT_model_destroy(model);
    return 0;
}
