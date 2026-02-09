#ifndef AT_NET_H
#define AT_NET_H

#include "../../core/include/acoustic/at.h"
#include "cJSON.h"

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    const char *url;
    uint32_t timeout_ms;
    int *http_status_out;
    char *response_buf;
    size_t response_buf_size;
} AT_NetworkConfig;

AT_Result AT_simulation_to_json(
        cJSON **out_json,
        AT_Simulation *simulation
);

AT_Result AT_send_json_to_url(
    cJSON *json,
    const AT_NetworkConfig *config
);

#endif // AT_NET_N