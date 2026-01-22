#ifndef AT_MODEL_H
#define AT_MODEL_H

#include "at.h"
#include "at_math.h"

typedef struct AT_Model AT_Model;

AT_Result AT_model_create(AT_Model **out_model, const char *filepath);
void AT_model_destroy(AT_Model *model);

#endif // AT_MODEL_H