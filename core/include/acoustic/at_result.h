#ifndef AT_RESULT_H
#define AT_RESULT_H

#include "at.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static inline void AT_handle_result(const AT_Result res, const char *err_msg, ...)
{
    va_list args;
    va_start(args, err_msg);

    switch (res) {
        case AT_OK:
            break;

        case AT_ERR_ALLOC_ERROR:
            vfprintf(stderr, err_msg, args);
            fprintf(stderr, "ALLOCATION ERROR\n");
            break;

        case AT_ERR_INVALID_ARGUMENT:
            vfprintf(stderr, err_msg, args);
            fprintf(stderr, "INVALID ARGUMENT\n");
            break;

        case AT_ERR_NETWORK_FAILURE:
            vfprintf(stderr, err_msg, args);
            fprintf(stderr, "NETWORK_FAILURE\n");
            break;
    }
    va_end(args);
}

#endif // AT_RESULT_H
