#include "acoustic/at_model.h"
#include "../src/at_internal.h"
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
    cgltf_mesh *mesh = &data->meshes[0];

    if (mesh->primitives_count == 0) {
        cgltf_free(data);
        return AT_ERR_INVALID_ARGUMENT;
    }
    cgltf_primitive *primitive = &mesh->primitives[0];

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

    // Vertices
    size_t vertex_count = pos_accessor->count;
    AT_Vec3 *vertices = malloc(sizeof(AT_Vec3) * vertex_count);

    if (!vertices) {
        cgltf_free(data);
        return AT_ERR_ALLOC_ERROR;
    }

    for (size_t i = 0; i < vertex_count; i++) {
        float v[3];
        cgltf_accessor_read_float(pos_accessor, i, v, 3);
        vertices[i] = (AT_Vec3){ v[0], v[1], v[2] };
    }

    // Indices
    cgltf_accessor *idx_accessor = primitive->indices;

    if (!idx_accessor) {
        cgltf_free(data);
        free(vertices);
        return AT_ERR_INVALID_ARGUMENT;
    }

    size_t index_count = idx_accessor->count;
    uint32_t *indices = malloc(sizeof(uint32_t) * index_count);

    if (!indices) {
        cgltf_free(data);
        free(vertices);
        return AT_ERR_ALLOC_ERROR;
    }

    for (size_t i = 0; i < index_count; i++) {
        uint32_t idx = 0;
        cgltf_accessor_read_uint(idx_accessor, i, &idx, 1);
        indices[i] = idx;
    }

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
        free(vertices);
        free(indices);
        return AT_ERR_INVALID_ARGUMENT;
    }

    AT_Vec3 *normals = malloc(sizeof(AT_Vec3) * norm_accessor->count);
    for (size_t i = 0; i < norm_accessor->count; i++) {
        float n[3];
        cgltf_accessor_read_float(norm_accessor, i, n, 3);
        normals[i] = (AT_Vec3){ n[0], n[1], n[2]};
    }

    AT_Model *model = calloc(1, sizeof(AT_Model));
    if (!model) {
        cgltf_free(data);
        free(vertices);
        free(indices);
        free(normals);
        return AT_ERR_ALLOC_ERROR;
    }

    model->index_count = index_count;
    model->vertex_count = vertex_count;
    model->indices = indices;
    model->vertices = vertices;
    model->normals = normals;

    *out_model = model;

    cgltf_free(data);
    return AT_OK;
}


void AT_model_to_AABB(AT_AABB *out_aabb, const AT_Model *model)
{
    AT_Vec3 max_vec = AT_vec3(FLT_MIN, FLT_MIN, FLT_MIN);
    AT_Vec3 min_vec = AT_vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    for (unsigned long i = 0; i < model->vertex_count; i++) {
        AT_Vec3 vec = model->vertices[i];
        if (vec.x < min_vec.x) {
            min_vec.x = vec.x;
        } else if (vec.x > max_vec.x) {
            max_vec.x = vec.x;
        }
        if (vec.y < min_vec.y) {
            min_vec.y = vec.y;
        } else if (vec.y > max_vec.y) {
            max_vec.y = vec.y;
        }
        if (vec.z < min_vec.z) {
            min_vec.z = vec.z;
        } else if (vec.z > max_vec.z) {
            max_vec.z = vec.z;
        }
    }

    out_aabb->min = min_vec;
    out_aabb->max = max_vec;
}


void AT_model_destroy(AT_Model *model)
{
   if (!model) return;

   free(model->vertices);
   free(model->indices);
   free(model->normals);
   free(model);
}
