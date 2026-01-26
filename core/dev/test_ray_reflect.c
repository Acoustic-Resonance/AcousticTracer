#include "../src/at_ray.h"
#include "acoustic/at_math.h"

#include <stdio.h>

#include "raylib.h"

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ray reflect test");

    SetTargetFPS(60);

    AT_Ray ray = AT_ray_init(&(AT_Vec3){0.0f, 0.0f, 0.0f},
                             &(AT_Vec3){4.0f, 3.0f, 0.5f}, 0.0f, 0);
    AT_Triangle triangle = (AT_Triangle){
        AT_vec3(2.0f, -1.0f, -2.0f),
        AT_vec3(2.0f, -1.0f, 5.0f),
        AT_vec3(2.0f, 5.0f, 1.0f),
    };

    AT_RayHit ray_hit = {0};
    AT_Ray new_ray = {0};
    if (AT_ray_triangle_intersect(&ray, &triangle, &ray_hit)) {
        new_ray.origin = ray_hit.position;
        new_ray.direction = AT_ray_reflect(ray.direction, ray_hit.normal);
        printf("New ray: {%.2f, %.2f, %.2f}\n", new_ray.direction.x,
               new_ray.direction.y, new_ray.direction.z);
        printf("RAY HIT!\n");
    }
    printf(
        "Hit Position: {%.2f, %.2f, %.2f} - Hit Normal: {%.2f, %.2f, %.2f} - "
        "t: %.2f\n",
        ray_hit.position.x, ray_hit.position.y, ray_hit.position.z,
        ray_hit.normal.x, ray_hit.normal.y, ray_hit.normal.z, ray_hit.t);

    Camera3D cam = {0};
    cam.position = (Vector3){-1.0f, 5.0f, 2.0f};
    cam.target =
        (Vector3){ray_hit.position.x, ray_hit.position.y, ray_hit.position.z};
    cam.up = (Vector3){0.0f, 1.0f, 0.0f};
    cam.fovy = 60.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_FREE);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(cam);
            {
                DrawRay(
                    (Ray){(Vector3){ray.origin.x, ray.origin.y, ray.origin.z},
                          (Vector3){ray.direction.x, ray.direction.y,
                                    ray.direction.z}},
                    RED);
                DrawTriangle3D(
                    (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.y},
                    (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                    (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                    GREEN);
                DrawSphere((Vector3){ray_hit.position.x, ray_hit.position.y,
                                     ray_hit.position.z},
                           0.05f, BLUE);
                DrawRay(
                    (Ray){(Vector3){new_ray.origin.x, new_ray.origin.y,
                                    new_ray.origin.z},
                          (Vector3){new_ray.direction.x, new_ray.direction.y,
                                    new_ray.direction.z}},
                    YELLOW);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
