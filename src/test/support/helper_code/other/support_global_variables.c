#include "lpj.h"
#include "natural.h"
#include "agriculture.h"
#include "grassland.h"
Real soildepth[] = {200.0, 300.0, 500.0, 1000.0, 1000.0, 10000.0};
Config test_config = {.johansen = TRUE, .percolation_heattransfer = TRUE};
Param param = {.soil_infil = 6, .percthres = 1, .frac_ro_stored = 0.8, .rw_buffer_max = 20};
Standtype setaside_rf_stand={};
Standtype setaside_ir_stand={};
