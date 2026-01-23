#include "acoustic/at.h"

typedef struct AT_Simulation AT_Simulation;

AT_Result AT_simulation_create(AT_Simulation **out_simulation, const AT_Scene *scene, const AT_Settings *settings);
AT_Result AT_simulation_run(AT_Simulation *simulation);
void AT_simulation_destroy(AT_Simulation *simulation);
