#include "acoustic/at_model.h"
#include "../src/at_internal.h"
#include "../src/at_utils.h"
#include "../src/at_aabb.h"
#include "acoustic/at.h"
#include "acoustic/at_math.h"
#include "cgltf.h"

#include <stdint.h>
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

    if (data->nodes_count == 0 && data->meshes_count == 0) {
        cgltf_free(data);
        return AT_ERR_INVALID_ARGUMENT;
    }

    /* ---------------------------------------------------------------
     * Pass 1: walk every node that has a mesh, accumulate counts.
     * We collect (node, primitive) pairs so pass 2 can reuse them.
     * ------------------------------------------------------------- */

    /* Flat list of (node ptr, primitive ptr) we will process */
    typedef struct { const cgltf_node *node; const cgltf_primitive *prim; } NodePrim;
    size_t pair_cap = 64;
    size_t pair_count = 0;
    NodePrim *pairs = malloc(pair_cap * sizeof(NodePrim));
    if (!pairs) { cgltf_free(data); return AT_ERR_ALLOC_ERROR; }

    uint32_t total_vertices = 0;
    uint32_t total_indices  = 0;

    for (size_t ni = 0; ni < data->nodes_count; ni++) {
        const cgltf_node *node = &data->nodes[ni];
        if (!node->mesh) continue;

        for (size_t pi = 0; pi < node->mesh->primitives_count; pi++) {
            const cgltf_primitive *prim = &node->mesh->primitives[pi];

            /* position accessor */
            cgltf_accessor *pos_acc = NULL;
            cgltf_accessor *nrm_acc = NULL;
            for (size_t ai = 0; ai < prim->attributes_count; ai++) {
                if (prim->attributes[ai].type == cgltf_attribute_type_position)
                    pos_acc = prim->attributes[ai].data;
                if (prim->attributes[ai].type == cgltf_attribute_type_normal)
                    nrm_acc = prim->attributes[ai].data;
            }
            if (!pos_acc || !prim->indices || !nrm_acc) continue; /* skip degenerate */

            total_vertices += (uint32_t)pos_acc->count;
            total_indices  += (uint32_t)prim->indices->count;

            /* grow pair list if needed */
            if (pair_count == pair_cap) {
                pair_cap *= 2;
                NodePrim *tmp = realloc(pairs, pair_cap * sizeof(NodePrim));
                if (!tmp) { free(pairs); cgltf_free(data); return AT_ERR_ALLOC_ERROR; }
                pairs = tmp;
            }
            pairs[pair_count++] = (NodePrim){ node, prim };
        }
    }

    if (total_vertices == 0 || total_indices == 0) {
        free(pairs);
        cgltf_free(data);
        return AT_ERR_INVALID_ARGUMENT;
    }

    /* ---------------------------------------------------------------
     * Allocate output buffers
     * ------------------------------------------------------------- */
    AT_Vec3   *vertices          = malloc(sizeof(AT_Vec3)        * total_vertices);
    uint32_t  *indices           = malloc(sizeof(uint32_t)       * total_indices);
    AT_Vec3   *normals           = malloc(sizeof(AT_Vec3)        * total_vertices);
    uint32_t  *triangle_materials = malloc(sizeof(AT_MaterialType)      * (total_indices / 3));

    if (!vertices || !indices || !normals || !triangle_materials) {
        free(vertices); free(indices); free(normals); free(triangle_materials);
        free(pairs); cgltf_free(data);
        return AT_ERR_ALLOC_ERROR;
    }

    /* ---------------------------------------------------------------
     * Pass 2: fill buffers, applying each node's world transform.
     * ------------------------------------------------------------- */
    uint32_t vertex_cursor = 0;
    uint32_t index_cursor  = 0;

    for (size_t pi = 0; pi < pair_count; pi++) {
        const cgltf_node      *node = pairs[pi].node;
        const cgltf_primitive *prim = pairs[pi].prim;

        /* Build 4x4 world matrix for this node (cgltf gives column-major float[16]) */
        float world[16];
        cgltf_node_transform_world(node, world);

        /* Helper lambdas (via macros) for transforming points and directions.
         * world is column-major: world[col*4 + row]
         *   point  : w = world * (x,y,z,1)
         *   vector : w = world * (x,y,z,0)  (direction / normal – no translation)
         * For normals we need the inverse-transpose of the upper-left 3x3.
         * cgltf_node_transform_world already includes all ancestor transforms.
         */

        /* --- vertices (positions) --- */
        cgltf_accessor *pos_acc = NULL;
        cgltf_accessor *nrm_acc = NULL;
        for (size_t ai = 0; ai < prim->attributes_count; ai++) {
            if (prim->attributes[ai].type == cgltf_attribute_type_position)
                pos_acc = prim->attributes[ai].data;
            if (prim->attributes[ai].type == cgltf_attribute_type_normal)
                nrm_acc = prim->attributes[ai].data;
        }

        size_t vert_count = pos_acc->count;
        uint32_t base_vertex = vertex_cursor;

        for (size_t vi = 0; vi < vert_count; vi++) {
            float lp[3];
            cgltf_accessor_read_float(pos_acc, vi, lp, 3);

            /* transform point: p' = M * (lp, 1) */
            float wx = world[0]*lp[0] + world[4]*lp[1] + world[8] *lp[2] + world[12];
            float wy = world[1]*lp[0] + world[5]*lp[1] + world[9] *lp[2] + world[13];
            float wz = world[2]*lp[0] + world[6]*lp[1] + world[10]*lp[2] + world[14];

            vertices[vertex_cursor + vi] = (AT_Vec3){{wx, wy, wz}};
        }

        /* --- normals (direction – needs inverse-transpose of upper-left 3x3) --- */
        /*
         * For a TRS matrix with uniform scale the normal transform equals the
         * rotation part.  For non-uniform scale we must use the inverse-transpose.
         * We compute it analytically for the 3x3 upper-left sub-matrix.
         */
        {
            /* extract upper-left 3x3 (column-major) */
            float m00=world[0], m10=world[1], m20=world[2];
            float m01=world[4], m11=world[5], m21=world[6];
            float m02=world[8], m12=world[9], m22=world[10];

            /* cofactor matrix (= adjugate^t) — each entry is its 2x2 minor det */
            float c00 = m11*m22 - m21*m12;
            float c01 = m21*m02 - m01*m22;
            float c02 = m01*m12 - m11*m02;
            float c10 = m20*m12 - m10*m22;
            float c11 = m00*m22 - m20*m02;
            float c12 = m10*m02 - m00*m12;
            float c20 = m10*m21 - m20*m11;
            float c21 = m20*m01 - m00*m21;
            float c22 = m00*m11 - m10*m01;

            /* det (for sanity; we don't need to divide because normals are renormalized) */
            /* float det = m00*c00 + m01*c10 + m02*c20; */

            for (size_t vi = 0; vi < nrm_acc->count; vi++) {
                float ln[3];
                cgltf_accessor_read_float(nrm_acc, vi, ln, 3);

                /* multiply by cofactor matrix (= inverse-transpose * det) */
                float nx = c00*ln[0] + c01*ln[1] + c02*ln[2];
                float ny = c10*ln[0] + c11*ln[1] + c12*ln[2];
                float nz = c20*ln[0] + c21*ln[1] + c22*ln[2];

                /* renormalize */
                float len = sqrtf(nx*nx + ny*ny + nz*nz);
                if (len > 1e-8f) { nx /= len; ny /= len; nz /= len; }

                normals[vertex_cursor + vi] = (AT_Vec3){{nx, ny, nz}};
            }
        }

        vertex_cursor += (uint32_t)vert_count;

        /* --- indices (offset by base_vertex so all primitives share one buffer) --- */
        cgltf_accessor *idx_acc = prim->indices;
        size_t idx_count = idx_acc->count;
        for (size_t ii = 0; ii < idx_count; ii++) {
            uint32_t idx = 0;
            cgltf_accessor_read_uint(idx_acc, ii, &idx, 1);
            indices[index_cursor + ii] = idx + base_vertex;
        }

        index_cursor += (uint32_t)idx_count;
    }

    free(pairs);

    /* ---------------------------------------------------------------
     * Build AT_Model
     * ------------------------------------------------------------- */
    AT_Model *model = calloc(1, sizeof(AT_Model));
    if (!model) {
        free(vertices); free(indices); free(normals); free(triangle_materials);
        cgltf_free(data);
        return AT_ERR_ALLOC_ERROR;
    }

    model->vertex_count       = total_vertices;
    model->index_count        = total_indices;
    model->vertices           = vertices;
    model->indices            = indices;
    model->normals            = normals;
    model->triangle_materials = triangle_materials;

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
    free(model->triangle_materials);
    free(model);
}

void AT_model_to_AABB(AT_AABB *out_aabb, const AT_Model *model)
{
    for (unsigned long i = 0; i < model->vertex_count; i++) {
        AT_Vec3 vec = model->vertices[i];
        AT_AABB_grow(out_aabb, vec);
    }

    out_aabb->midpoint = AT_AABB_calc_midpoint(out_aabb);
    out_aabb->SA = AT_AABB_get_SA(*out_aabb);
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
        ts[i].left = 0;
    }
    *out_triangles = ts;
    return AT_OK;
}
