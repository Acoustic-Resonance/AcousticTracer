#include "at_net.h"
#include "../../core/src/at_internal.h"
#include "../../core/src/at_voxel.h"
#include "acoustic/at.h"
#include "acoustic/at_result.h"
#include "cJSON.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

AT_Result AT_simulation_to_json(cJSON **out_json, AT_Simulation *simulation)
{
    if (!out_json || *out_json || !simulation)
        return AT_ERR_INVALID_ARGUMENT;

    size_t FRAME_NUM_BUFFER_LENGTH = sizeof(uint8_t);
    cJSON *json = cJSON_CreateObject();

    AT_Voxel *voxels = simulation->voxel_grid;
    uint32_t num_voxels = simulation->num_voxels;
    uint32_t num_bins = AT_voxel_get_num_bins(simulation);

    for (uint32_t f = 0; f < num_bins; f++)
    {

        char frame_num[FRAME_NUM_BUFFER_LENGTH];
        sprintf(frame_num, "frame_%d", f);

        cJSON *frame_data = cJSON_CreateArray();

        for (uint32_t v = 0; v < num_voxels; v++)
        {

            AT_Voxel voxel = voxels[v];

            float energy = (f < voxel.count) ? voxel.items[f] : 0;

            // TODO: IF ENERGY OVER MIN THRESHOLD
            if (energy <= 0)
                continue;

            char voxel_num[num_voxels];
            sprintf(voxel_num, "%d", v);

            cJSON *voxel_data = cJSON_CreateObject();

            cJSON_AddNumberToObject(voxel_data, voxel_num, energy);
            cJSON_AddItemToArray(frame_data, voxel_data);
        }

        cJSON_AddItemToObject(json, frame_num, frame_data);
    }
    *out_json = json;
    return AT_OK;
}

AT_Result AT_simulation_to_binary(uint8_t **out_buf, size_t *out_size, AT_Simulation *simulation)
{
    if (!out_buf || !out_size || !simulation)
        return AT_ERR_INVALID_ARGUMENT;

    AT_Voxel *voxels = simulation->voxel_grid;
    uint32_t num_voxels = simulation->num_voxels;
    uint32_t num_frames = AT_voxel_get_num_bins(simulation);

    /* First pass: count active voxels per frame */
    uint32_t *counts = calloc(num_frames, sizeof(uint32_t));
    if (!counts)
        return AT_ERR_ALLOC_ERROR;

    for (uint32_t f = 0; f < num_frames; f++)
    {
        for (uint32_t v = 0; v < num_voxels; v++)
        {
            float energy = (f < voxels[v].count) ? voxels[v].items[f] : 0.0f;
            if (energy > 0.0f)
                counts[f]++;
        }
    }

    /*
     * Binary layout (all multi-byte values little-endian):
     *   Header      16 bytes: magic "ATRB" (4) + numFrames (4) + numVoxels (4) + reserved (4)
     *   Frame table  numFrames * 8 bytes: offset (4) + count (4) per frame
     *   Frame data   per frame: indices[count] (uint32) then energies[count] (float32)
     */
    size_t header_sz = 16;
    size_t table_sz = (size_t)num_frames * 8;
    size_t data_sz = 0;
    for (uint32_t f = 0; f < num_frames; f++)
        data_sz += (size_t)counts[f] * 8;

    size_t total = header_sz + table_sz + data_sz;
    uint8_t *buf = malloc(total);
    if (!buf)
    {
        free(counts);
        return AT_ERR_ALLOC_ERROR;
    }

    /* Header */
    buf[0] = 'A';
    buf[1] = 'T';
    buf[2] = 'R';
    buf[3] = 'B';
    memcpy(buf + 4, &num_frames, 4);
    memcpy(buf + 8, &num_voxels, 4);
    memset(buf + 12, 0, 4);

    /* Frame table + data */
    size_t data_pos = header_sz + table_sz;
    for (uint32_t f = 0; f < num_frames; f++)
    {
        uint32_t off32 = (uint32_t)data_pos;
        memcpy(buf + header_sz + (size_t)f * 8, &off32, 4);
        memcpy(buf + header_sz + (size_t)f * 8 + 4, &counts[f], 4);

        /* SoA: write all indices first, then all energies */
        uint32_t written = 0;
        for (uint32_t v = 0; v < num_voxels; v++)
        {
            float energy = (f < voxels[v].count) ? voxels[v].items[f] : 0.0f;
            if (energy > 0.0f)
            {
                memcpy(buf + data_pos + (size_t)written * 4, &v, 4);
                memcpy(buf + data_pos + (size_t)counts[f] * 4 + (size_t)written * 4, &energy, 4);
                written++;
            }
        }
        data_pos += (size_t)counts[f] * 8;
    }

    *out_buf = buf;
    *out_size = total;
    free(counts);
    return AT_OK;
}

void AT_raytracer()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(8080)};
    bind(server_fd, (sockaddr *)&address, sizeof(address));
    listen(server_fd, 4);
    printf("Server running on 127.0.0.1:8080\n");

    while (1)
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
            continue;

        // request headers
        char buffer[BUFFER_SIZE] = {0};
        int total = 0, bytes = 0;
        while (total < BUFFER_SIZE - 1)
        {
            bytes = read(client_fd, buffer + total, BUFFER_SIZE - 1 - total);
            if (bytes <= 0)
                break;
            total += bytes;
            if (strstr(buffer, "\r\n\r\n"))
                break;
        }

        // CORS preflight options
        if (strncmp(buffer, "OPTIONS /run", 12) == 0)
        {
            const char *resp =
                "HTTP/1.1 204 No Content\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
                "Access-Control-Allow-Headers: content-type\r\n"
                "Access-Control-Max-Age: 86400\r\n"
                "Content-Length: 0\r\n"
                "\r\n";
            write(client_fd, resp, strlen(resp));
            close(client_fd);
            continue;
        }

        // POST
        if (strncmp(buffer, "POST /run", 9) != 0)
        {
            const char *err =
                "HTTP/1.1 404 Not Found\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
                "Access-Control-Allow-Headers: content-type\r\n"
                "Content-Length: 0\r\n\r\n";
            write(client_fd, err, strlen(err));
            close(client_fd);
            continue;
        }

        // find body start
        char *body_start = strstr(buffer, "\r\n\r\n");
        if (!body_start)
        {
            const char *err =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
                "Access-Control-Allow-Headers: content-type\r\n"
                "Content-Length: 0\r\n\r\n";
            write(client_fd, err, strlen(err));
            close(client_fd);
            continue;
        }
        body_start += 4;

        // parse the content-length
        int content_length = 0;
        char *cl = strstr(buffer, "Content-Length: ");
        if (!cl)
            cl = strstr(buffer, "content-length: ");
        if (cl)
            content_length = atoi(cl + 16);

        int header_len = (int)(body_start - buffer);
        int body_received = total - header_len;
        while (body_received < content_length)
        {
            int space = BUFFER_SIZE - 1 - header_len - body_received;
            // body too large :/
            if (space <= 0)
                break;
            bytes = read(client_fd, buffer + header_len + body_received, space);
            if (bytes <= 0)
                break;
            body_received += bytes;
        }
        buffer[header_len + body_received] = '\0';

        // parse config from JSON

        char filename[256] = {0};
        char filepath[512] = {0};
        float voxel_size = 0.0f;
        uint32_t num_rays = 0;
        uint32_t fps = 0;
        AT_MaterialType material = {0};

        cJSON *cjson = cJSON_Parse(body_start);
        cJSON *j;

        j = cJSON_GetObjectItemCaseSensitive(cjson, "fileName");
        if (cJSON_IsString(j))
        {
            strncpy(filename, j->valuestring, sizeof(filename) - 1);
            snprintf(filepath, sizeof(filepath), "../assets/glb/%s", filename);
        }
        j = cJSON_GetObjectItemCaseSensitive(cjson, "voxelSize");
        if (cJSON_IsNumber(j))
        {
            voxel_size = (float)j->valuedouble;
        }
        j = cJSON_GetObjectItemCaseSensitive(cjson, "numRays");
        if (cJSON_IsNumber(j))
        {
            num_rays = (uint32_t)j->valueint;
        }
        j = cJSON_GetObjectItemCaseSensitive(cjson, "fps");
        if (cJSON_IsNumber(j))
        {
            fps = (uint32_t)j->valueint;
        }

        // material
        j = cJSON_GetObjectItemCaseSensitive(cjson, "material");
        if (cJSON_IsString(j))
        {
            const char *material_str = j->valuestring;
            if (strcmp(material_str, "Plastic") == 0)
            {
                material = AT_MATERIAL_PLASTIC;
            }
            else if (strcmp(material_str, "Wood") == 0)
            {
                material = AT_MATERIAL_WOOD;
            }
            else if (strcmp(material_str, "Concrete") == 0)
            {
                material = AT_MATERIAL_CONCRETE;
            }
            else
            {
                material = AT_MATERIAL_CONCRETE;
            }
        }

        AT_Source source = {0};

        // source
        cJSON *source_position = cJSON_GetObjectItemCaseSensitive(cjson, "selectedSource");
        source_position = cJSON_GetObjectItemCaseSensitive(source_position, "position");
        j = cJSON_GetObjectItemCaseSensitive(source_position, "x");
        if (cJSON_IsNumber(j))
        {
            source.position.x = j->valuedouble;
        }
        j = cJSON_GetObjectItemCaseSensitive(source_position, "y");
        if (cJSON_IsNumber(j))
        {
            source.position.y = j->valuedouble;
        }
        j = cJSON_GetObjectItemCaseSensitive(source_position, "z");
        if (cJSON_IsNumber(j))
        {
            source.position.z = j->valuedouble;
        }

        // direction
        cJSON *source_direction = cJSON_GetObjectItemCaseSensitive(cjson, "selectedSource");
        source_direction = cJSON_GetObjectItemCaseSensitive(source_direction, "direction");
        j = cJSON_GetObjectItemCaseSensitive(source_direction, "x");
        if (cJSON_IsNumber(j))
        {
            source.direction.x = j->valuedouble;
        }
        j = cJSON_GetObjectItemCaseSensitive(source_direction, "y");
        if (cJSON_IsNumber(j))
        {
            source.direction.y = j->valuedouble;
        }
        j = cJSON_GetObjectItemCaseSensitive(source_direction, "z");
        if (cJSON_IsNumber(j))
        {
            source.direction.z = j->valuedouble;
        }

        printf("SOURCE DIRECTION: %f, %f, %f\n", source.direction.x, source.direction.y, source.direction.z);
        printf("SOURCE POSITION: %f, %f, %f\n", source.position.x, source.position.y, source.position.z);

        cJSON_Delete(cjson);

        // run raytracer

        AT_Model *model = NULL;
        AT_Result res = AT_model_create(&model, filepath);
        AT_handle_result(res, "Error creating model\n");

        AT_Source sources[1];
        sources[0] = source;

        AT_SceneConfig conf = {
            .environment = model,
            .material = material,
            .num_sources = 1,
            .sources = sources};

        AT_Scene *scene = NULL;
        res = AT_scene_create(&scene, &conf);
        AT_handle_result(res, "Error creating scene\n");

        AT_Settings settings = {
            .fps = fps,
            .num_rays = num_rays,
            .voxel_size = voxel_size};

        AT_Simulation *sim = NULL;
        res = AT_simulation_create(&sim, scene, &settings);
        AT_handle_result(res, "Error creating simulation\n");

        res = AT_simulation_run(sim);
        AT_handle_result(res, "Error running simulation\n");

        uint8_t *result_buf = NULL;
        size_t result_size = 0;
        res = AT_simulation_to_binary(&result_buf, &result_size, sim);
        AT_handle_result(res, "Error converting simulation to binary\n");

        char header[256];
        snprintf(header, sizeof(header),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/octet-stream\r\n"
                 "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                 "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
                 "Access-Control-Allow-Headers: content-type\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n",
                 result_size);
        write(client_fd, header, strlen(header));

        size_t sent = 0;
        while (sent < result_size)
        {
            ssize_t w = write(client_fd, result_buf + sent, result_size - sent);
            if (w <= 0)
                break;
            sent += (size_t)w;
        }
        close(client_fd);

        AT_simulation_destroy(sim);
        AT_scene_destroy(scene);
        cJSON_Delete(sim_json);
    }

    close(server_fd);
}
