/**************************************************************************************/
/**                                                                                \n**/
/**              i  n  p  u  t  _  f  m  s  .  j  s                                \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 4.0.002                 \n**/
/** and FMS coupler                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "include/conf.h" /* include constant definitions */

"input" :
{
  "soil" : { "fmt" : "raw", "name" : "/p/projects/climber3/petri/POEM/exp/CM2M_coarse_BLING_LPJ_05/Data-For-LPJ/Soil/soil_new.bin"},
  "coord" : { "fmt" : "clm", "name" :  "/p/projects/climber3/petri/POEM/exp/CM2M_coarse_BLING_LPJ_05/Data-For-LPJ/grid.bin"},
  "country" : { "fmt" : "clm", "name" : "input_VERSION2/cow_full_2018.bin"},
  "landuse" : { "fmt" : "clm", "name" : "input_VERSION2/cft1700_2005_bioenergy_sc.bin"},
  "lakes" : { "fmt" : "raw", "name" : "/p/projects/climber3/petri/POEM/exp/CM2M_coarse_BLING_LPJ_05/Data-For-LPJ/Lakes/glwd_lakes-90881.bin"},
  "drainage" : { "fmt" : "clm", "name" : "/p/projects/climber3/petri/POEM/exp/CM2M_coarse_BLING_LPJ_05/Data-For-LPJ/Rivers/drainage.bin"},
  "runoff2ocean_map": { "fmt" : "clm", "name" : " /p/projects/climber3/gengel/POEM/mom5.0.2/exp/CM2M_coarse_BLING_LPJ_05/Data-For-LPJ/runoff_coord.bin"}, /*together with this file, all runoff is send directly to ocean*/
  "neighb_irrig" : { "fmt" : "clm", "name" : "input_VERSION2/neighb_irrigation.bin"},
  "elevation" : { "fmt" : "clm", "name" : "input_VERSION2/elevation.bin"},
  "reservoir" : { "fmt" : "clm", "name" : "input_VERSION2/reservoir_info_grand5.bin"},
  "temp" : { "fmt" : "fms" }, /* temperature data from FMS coupler */
  "prec" : { "fmt" : "fms" },/* precipitation data from FMS coupler */
  "lwnet" : { "fmt" : "fms" },/* net longwave radiation data from FMS coupler */
  "swdown" : { "fmt" : "fms" },/* downwelling shortwave radiation data from FMS coupler */
  "windspeed" : { "fmt" : "fms" }, /* windspeed data from FMS coupler */
  "tamp" : { "fmt" : "fms" },/* diurnal temp range data from FMS coupler */
  "lightning" : { "fmt" : "clm", "nane" : "input_VERSION2/mlightning.clm"},
  "human_ignition" : { "fmt" : "clm", "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" : { "fmt" : "clm", "name" : "input_VERSION2/popdens_HYDE_1901_2010_bi.clm"},
  "co2" : { "fmt" : FMS }/* CO2 data from FMS coupler */
},
