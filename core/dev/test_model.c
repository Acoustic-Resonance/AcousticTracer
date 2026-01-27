#include "../src/at_internal.h"
#include "acoustic/at_model.h"
#include "../external/raylib.h"

#include <math.h>
#include <stdio.h>

void move_camera(Camera3D *camera)
{
    const float rotationSpeed = 0.03f;
    const float zoomSpeed = 0.5f;

    float dx = camera->position.x - camera->target.x;
    float dz = camera->position.z - camera->target.z;

    float radius = sqrtf(dx*dx + dz*dz);
    float angle = atan2f(dz, dx);

    if (IsKeyDown(KEY_A)) {
        angle -= rotationSpeed;
    } else if (IsKeyDown(KEY_D)) {
        angle += rotationSpeed;
    } else if (IsKeyDown(KEY_S)) {
        radius += zoomSpeed;
    } else if (IsKeyDown(KEY_W)) {
        radius -= zoomSpeed;
    }

    camera->position.x = camera->target.x + cosf(angle) * radius;
    camera->position.z = camera->target.z + sinf(angle) * radius;
}

Mesh at_model_to_raylib_mesh(const AT_Model *model)
{
    Mesh mesh = {0};

    mesh.vertexCount   = model->vertex_count;
    mesh.triangleCount = model->index_count / 3;

    // Allocate Raylib buffers
    mesh.vertices = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.normals  = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.indices  = MemAlloc(sizeof(unsigned int) * model->index_count);

    // Vertices
    for (int i = 0; i < model->vertex_count; i++) {
        mesh.vertices[i*3 + 0] = model->vertices[i].x;
        mesh.vertices[i*3 + 1] = model->vertices[i].y;
        mesh.vertices[i*3 + 2] = model->vertices[i].z;
    }

    // Normals
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

    Model rl_model = LoadModelFromMesh(at_model_to_raylib_mesh(model));

    while(!WindowShouldClose()) {
        move_camera(&camera);
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
    const char *filepath = "../assets/glb/L_room.gltf";
    AT_Model *model = NULL;
    if(AT_model_create(&model, filepath) != AT_OK) {
        perror("Failed to create model");
        return 1;
    }

    draw_model(model);

    AT_model_destroy(model);
    return 0;
}
