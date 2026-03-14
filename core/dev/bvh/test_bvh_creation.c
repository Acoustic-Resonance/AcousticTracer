#include "../src/at_bvh.h"
#include "../src/at_internal.h"
#include "../src/at_trigroup.h"
#include "../src/at_utils.h"
#include "acoustic/at_model.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 1200

int main(int argc, char *_[])
{
    const char *filepath = "../assets/glb/Sponza.glb";
    AT_Model *model = NULL;
    if (AT_model_create(&model, filepath) != AT_OK) {
        perror("Failed to create model");
        return 1;
    }

    for (uint32_t i = 0; i < model->vertex_count; i++) {
        model->vertices[i] = AT_vec3_scale(model->vertices[i], 0.05);
    }

    AT_TriangleArrays *triangle_arrs = NULL;
    if (AT_triangle_arrays_create(&triangle_arrs, model) != AT_OK) {
        perror("Failed to create triangle arrays");
        AT_model_destroy(model);
        return 1;
    }

    AT_BVHConfig bvh_config = {
        .mini_tree_size = 100,
        .intersection_cost = 1,
        .traversal_cost = 0.5,
    };

    uint32_t num_tri = model->index_count / 3;

    AT_TriangleGroups *tri_groups = NULL;
    if (AT_triangle_groups_create(&tri_groups, num_tri) != AT_OK) {
        perror("Failed to create the triangle groups holder");
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }
    if (AT_trigroup_split(triangle_arrs, num_tri, tri_groups, bvh_config.mini_tree_size) != AT_OK) {
        perror("Failed to split the triangle group");
        AT_triangle_groups_destroy(tri_groups);
        AT_triangle_arrays_destroy(triangle_arrs);
        AT_model_destroy(model);
        return 1;
    }

    uint32_t max_count = 0;
    AT_MiniTree **mini_trees = calloc(tri_groups->num_groups, sizeof(*mini_trees));
    // for (uint32_t i = 0; i < groups->num_groups; i++) {
    uint32_t num_bvh = tri_groups->num_groups;
    // uint32_t num_bvh = 10;
    for (uint32_t i = 0; i < num_bvh; i++) {
        max_count = AT_max(tri_groups->groups[i]->num_tri, max_count);
        AT_Result res = AT_MiniTree_create(&mini_trees[i], tri_groups->groups[i], &bvh_config);
        if (res != AT_OK) {
            char txt[25];
            sprintf(txt, "Failed to create BVH %d %d", i, res);
            perror(txt);
            return 1;
        }
    }

    if (argc > 1) {
        Color colors[16] = {RED, BLUE, GREEN, PURPLE, PINK, LIME, BROWN, MAROON, MAGENTA, ORANGE, GOLD, YELLOW, DARKGREEN, SKYBLUE, DARKBLUE, VIOLET};

        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Test");
        Camera3D camera = {
            .position = {-45.0f, 100.0f, 0.0f},
            .target = {40.0f, 40.0f, 0.0f},
            .up = {0.0f, 1.0f, 0.0f},
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        };
        SetTargetFPS(60);

        while (!WindowShouldClose()) {
            UpdateCamera(&camera, CAMERA_FREE);
            BeginDrawing();
            ClearBackground(WHITE);
            BeginMode3D(camera);
            // for (uint32_t grp_idx = 0; grp_idx < groups->num_groups; grp_idx++) {
            // for (uint32_t grp_idx = 0; grp_idx < 1; grp_idx++) {
            // for (uint32_t i = 0; i < groups->groups[grp_idx]->num_tri; i++) {
            for (uint32_t bvh_idx = 0; bvh_idx < num_bvh; bvh_idx++) {
                for (uint32_t node_idx = 0; node_idx < mini_trees[bvh_idx]->last_node_idx; node_idx++) {
                    Color color = colors[node_idx % (sizeof(colors) / sizeof(colors[0]))];
                    AT_MiniTreeNode *node = &mini_trees[bvh_idx]->nodes[node_idx];
                    if (node->start == node->num_tri || node->num_tri < 100) continue;
                    // printf("i: %u, j: %u, start: %u, num_tri: %u\n", bvh_idx, node_idx, node->start, node->num_tri);
                    for (uint32_t tri_idx = 0; tri_idx < node->num_tri; tri_idx++) {
                        AT_Triangle triangle = AT_get_triangle(node, 3, tri_idx);
                        DrawTriangle3D(
                            (Vector3){triangle.v1.x, triangle.v1.y, triangle.v1.z},
                            (Vector3){triangle.v2.x, triangle.v2.y, triangle.v2.z},
                            (Vector3){triangle.v3.x, triangle.v3.y, triangle.v3.z},
                            // (Color){
                            //     .r = color.r * (log2(bvhs[i]->last_node_idx)),
                            //     .g = color.g * (log2(bvhs[i]->last_node_idx)),
                            //     .b = color.b * (log2(bvhs[i]->last_node_idx)),
                            // }
                            Fade(color, 0.2f)
                        );
                    }
                }
            }
            EndMode3D();
            DrawFPS(10, 10);
            DrawGrid(10, 1.0f);
            EndDrawing();
        }
        CloseWindow();
    } else {
        // for (int i = 0; i < groups->num_groups; i++) {
        for (int i = 0; i < num_bvh; i++) {
            AT_MiniTree *bvh = mini_trees[i];
            // printf("BVH %d with %d tri.\n", i, bvh->nodes[0].num_tri);
            AT_MiniTreeNode *stack[sizeof(bvh->nodes) * bvh->max_node_count];
            int stack_top = 0;
            stack[stack_top++] = &bvh->nodes[0];
            AT_MiniTreeNode *parent;
            while (stack_top > 0) {
                parent = stack[--stack_top];
                // printf("\tParent %d: %f with %d tri.\n", parent->idx, parent->aabb.SA, parent->num_tri);
                if (parent->left_child == -1 || parent->right_child == -1) {
                    continue;
                }
                AT_MiniTreeNode *left = &bvh->nodes[parent->left_child];
                AT_MiniTreeNode *right = &bvh->nodes[parent->right_child];
                // printf("\t\tLeft %d: %f with %d tri, Right %d: %f with %d tri\n", left->idx, left->aabb.SA, left->num_tri, right->idx, right->aabb.SA, right->num_tri);
                if (left->num_tri > 1) {
                    stack[stack_top++] = left;
                }
                if (right->num_tri > 1) {
                    stack[stack_top++] = right;
                }
                printf("%u\n", left->num_tri);
                printf("%u\n", right->num_tri);
            }
            printf("\n");
        }
    }

    for (uint32_t i = 0; i < num_bvh; i++) {
        AT_MiniTree_destroy(mini_trees[i]);
    }
    AT_triangle_groups_destroy(tri_groups);
    AT_triangle_arrays_destroy(triangle_arrs);
    AT_model_destroy(model);
}
