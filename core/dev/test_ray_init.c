#include "../src/at_ray.h"
#include "acoustic/at_math.h"

#include <stdio.h>

#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int main()
{
    printf("ray init!\n");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ray test");

    AT_Ray ray = AT_ray_init(
        (AT_Vec3){0.5f, 0.5, 1.0f},
        (AT_Vec3){0.0f, 0.0f, -1.0f},
        0
    );

    AT_Triangle t1 = {
        .v1 = {0.0f, 0.0f, 0.0f},
        .v2 = {2.0f, 0.0f, 0.0f},
        .v3 = {0.0f, 2.0f, 0.0f}
    };

    AT_Ray out_ray = {0};
    if (AT_ray_triangle_intersect(&ray, &t1, &out_ray)) {
        printf("RAY HIT!\n");
    }
    printf("Hit Position: {%.2f, %.2f, %.2f} - Hit Normal: {%.2f, %.2f, %.2f} - t: %.2f\n",
        out_ray.origin.x, out_ray.origin.y, out_ray.origin.z,
        out_ray.direction.x, out_ray.direction.y, out_ray.direction.z,
        AT_vec3_distance(ray.origin, out_ray.origin));

    SetTargetFPS(60);

    Camera3D cam = {0};
    cam.position = (Vector3){ 0.0, 1.0, 2.0 };
    cam.target = (Vector3){ 1.0, 0.5, 0.0 };
    cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam.fovy = 60.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose()) {

        UpdateCamera(&cam, CAMERA_FREE);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode3D(cam);
            {
                DrawRay((Ray){
                    (Vector3){ray.origin.x, ray.origin.y, ray.origin.z},
                    (Vector3){ray.direction.x, ray.direction.y, ray.direction.z}
                    }, RED);

                DrawTriangle3D((Vector3){t1.v1.x, t1.v1.y, t1.v1.z},
                               (Vector3){t1.v2.x, t1.v2.y, t1.v2.z},
                               (Vector3){t1.v3.x, t1.v3.y, t1.v3.z},
                               GREEN);

                DrawSphere((Vector3){out_ray.origin.x, out_ray.origin.y, out_ray.origin.z},
                    0.05f, BLUE);
            }
            EndMode3D();
        }
        EndDrawing();
    }

    return 0;
}
