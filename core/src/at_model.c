#include "acoustic/at_model.h"
#include "../src/at_internal.h"
#include "../src/at_utils.h"
#include "../src/at_aabb.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "cgltf.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

AT_Result AT_model_create(AT_Model **out_model, const char *filepath)
{
    if (!out_model || *out_model || !filepath) return AT_ERR_INVALID_ARGUMENT;

    cgltf_options options = {0};
    cgltf_data *data = NULL;
    cgltf_result res = cgltf_parse_file(&options, filepath, &data);

    if (res != cgltf_result_success) return AT_ERR_INVALID_ARGUMENT;

    res = cgltf_load_buffers(&options, data, filepath);

    if (res != cgltf_result_success) {
        cgltf_free(data);
        return AT_ERR_INVALID_ARGUMENT;
    }

    if (data->meshes_count == 0) {
        cgltf_free(data);
        return AT_ERR_INVALID_ARGUMENT;
    }

    uint32_t total_vertices = 0;
    uint32_t total_indices = 0;
    uint32_t total_normals = 0;

    uint32_t vertex_index = 0;
    uint32_t index_index = 0;
    uint32_t normal_index = 0;

    cgltf_mesh *mesh = &data->meshes[0];

    if (mesh->primitives_count == 0) {
        cgltf_free(data);
        return AT_ERR_INVALID_ARGUMENT;
    }
    for (unsigned long i = 0; i < mesh->primitives_count; i++) {

        cgltf_primitive *primitive = &mesh->primitives[i];

        cgltf_accessor *pos_accessor = NULL;
        for (size_t i = 0; i < primitive->attributes_count; i++) {
            if (primitive->attributes[i].type == cgltf_attribute_type_position) {
                pos_accessor = primitive->attributes[i].data;
                break;
            }
        }

        if (!pos_accessor) {
            cgltf_free(data);
            return AT_ERR_INVALID_ARGUMENT;
        }

        size_t vertex_count = pos_accessor->count;

        cgltf_accessor *idx_accessor = primitive->indices;

        if (!idx_accessor) {
            cgltf_free(data);
            return AT_ERR_INVALID_ARGUMENT;
        }

        size_t index_count = idx_accessor->count;

        // Normals
        cgltf_accessor *norm_accessor = NULL;
        for (size_t i = 0; i < primitive->attributes_count; i++) {
            if (primitive->attributes[i].type == cgltf_attribute_type_normal) {
                norm_accessor = primitive->attributes[i].data;
                break;
            }
        }
        if (!norm_accessor) {
            cgltf_free(data);
            return AT_ERR_INVALID_ARGUMENT;
        }

        size_t normal_count = norm_accessor->count;

        total_vertices += vertex_count;
        total_indices += index_count;
        total_normals += normal_count;

    }

    AT_Vec3 *vertices = malloc(sizeof(AT_Vec3) * total_vertices);

    if (!vertices) {
        cgltf_free(data);
        return AT_ERR_ALLOC_ERROR;
    }

    uint32_t *indices = malloc(sizeof(uint32_t) * total_indices);

    if (!indices) {
        cgltf_free(data);
        free(vertices);
        return AT_ERR_ALLOC_ERROR;
    }

    AT_Vec3 *normals = malloc(sizeof(AT_Vec3) * total_normals);

    if (!normals) {
        cgltf_free(data);
        free(vertices);
        free(indices);
        return AT_ERR_ALLOC_ERROR;
    }

    for (unsigned long i = 0; i < mesh->primitives_count; i++) {

        size_t base_vertex = vertex_index;

        cgltf_primitive *primitive = &mesh->primitives[i];

        cgltf_accessor *pos_accessor = NULL;
        for (size_t i = 0; i < primitive->attributes_count; i++) {
            if (primitive->attributes[i].type == cgltf_attribute_type_position) {
                pos_accessor = primitive->attributes[i].data;
                break;
            }
        }

        // Vertices
        size_t vertex_count = pos_accessor->count;
        for (size_t i = 0; i < vertex_count; i++) {
            float v[3];
            cgltf_accessor_read_float(pos_accessor, i, v, 3);
            vertices[vertex_index + i] = (AT_Vec3){ v[0], v[1], v[2] };
        }
        vertex_index += vertex_count;


        // Indices
        cgltf_accessor *idx_accessor = primitive->indices;
        size_t index_count = idx_accessor->count;
        for (size_t i = 0; i < index_count; i++) {
            uint32_t idx = 0;
            cgltf_accessor_read_uint(idx_accessor, i, &idx, 1);
            indices[index_index + i] = idx + base_vertex;
        }
        index_index += index_count;

        // Normals
        cgltf_accessor *norm_accessor = NULL;
        for (size_t i = 0; i < primitive->attributes_count; i++) {
            if (primitive->attributes[i].type == cgltf_attribute_type_normal) {
                norm_accessor = primitive->attributes[i].data;
                break;
            }
        }
        for (size_t i = 0; i < norm_accessor->count; i++) {
            float n[3];
            cgltf_accessor_read_float(norm_accessor, i, n, 3);
            normals[normal_index + i] = (AT_Vec3){ n[0], n[1], n[2]};
        }
        normal_index += norm_accessor->count;
    }

    AT_Model *model = calloc(1, sizeof(AT_Model));
    if (!model) {
        cgltf_free(data);
        free(vertices);
        free(indices);
        free(normals);
        return AT_ERR_ALLOC_ERROR;
    }

    model->index_count = total_indices;
    model->vertex_count = total_vertices;
    model->indices = indices;
    model->vertices = vertices;
    model->normals = normals;

    *out_model = model;

    cgltf_free(data);
    return AT_OK;
}

void AT_model_destroy(AT_Model *model)
{
    if (!model) return;

    free(model->vertices);
    free(model->indices);
    free(model->normals);
    free(model);
}

void AT_model_to_AABB(AT_AABB *out_aabb, const AT_Model *model)
{
    AT_Vec3 min_vec = AT_vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    AT_Vec3 max_vec = AT_vec3(FLT_MIN, FLT_MIN, FLT_MIN);
    for (unsigned long i = 0; i < model->vertex_count; i++) {
        AT_Vec3 vec = model->vertices[i];
        min_vec.x = AT_min(min_vec.x, vec.x);
        min_vec.y = AT_min(min_vec.y, vec.y);
        min_vec.z = AT_min(min_vec.z, vec.z);
        max_vec.x = AT_max(max_vec.x, vec.x);
        max_vec.y = AT_max(max_vec.y, vec.y);
        max_vec.z = AT_max(max_vec.z, vec.z);
    }

    out_aabb->min = min_vec;
    out_aabb->max = max_vec;
    out_aabb->midpoint = AT_AABB_calc_midpoint(out_aabb);
}


AT_Result AT_model_get_triangles(AT_Triangle **out_triangles, const AT_Model *model)
{
    uint32_t triangle_count = model->index_count / 3;
    AT_Triangle *ts = (AT_Triangle*)malloc(sizeof(AT_Triangle) * triangle_count);
    if (!ts) return AT_ERR_ALLOC_ERROR;
    for (uint32_t i = 0; i < triangle_count; i++) {
        ts[i] = (AT_Triangle){
            .v1 = model->vertices[model->indices[i*3 + 0]],
            .v2 = model->vertices[model->indices[i*3 + 1]],
            .v3 = model->vertices[model->indices[i*3 + 2]]
        };
        ts[i].aabb = AT_AABB_from_triangle(&ts[i]);
    }
    *out_triangles = ts;
    return AT_OK;
}
