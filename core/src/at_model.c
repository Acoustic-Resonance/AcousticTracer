#include "../include/acoustic/at_model.h"

#define CGLTF_IMPLEMENTATION

#include "../external/cgltf.h"

#include <stdio.h>
#include <stdlib.h>

struct AT_Model {
    AT_Vec3 *vertices;
    // TODO: normals
    uint32_t *indices;
    size_t vertex_count;
    size_t index_count;
};

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
    
    AT_Model *model = calloc(1, sizeof(AT_Model));
    if (!model) {
        cgltf_free(data);
        free(vertices);
        free(indices);
        return AT_ERR_ALLOC_ERROR;
    }
    
    model->index_count = index_count;
    model->vertex_count = vertex_count;
    model->indices = indices;
    model->vertices = vertices;
    
    *out_model = model;
    
    cgltf_free(data);
    return AT_OK;
}

void AT_model_destroy(AT_Model *model)
{
   if (!model) return;

   free(model->vertices);
   free(model->indices);
   free(model); 
}