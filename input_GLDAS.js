/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  G  L  D  A  S  .  j  s                                 \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 4.0.001                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "include/conf.h" /* include constant definitions */

"input" :
{
  "soil" :         { "fmt" : RAW,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/soil_GLDAS.bin"},
  "coord" :        { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/grid_GLDAS.clm"},
  "countrycode" :  { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/country_GLDAS.clm"},
  "landuse" :      { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/landuse_GLDAS.clm"},
  /* insert prescribed sdate file name here */
  "grassland_fixed_pft" : { "fmt" : RAW, "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
  "lakes" :        { "fmt" : META, "name" : "/p/projects/lpjml/input/historical/input_VERSION2/glwd_lakes_and_rivers.descr"},
  "drainage" :     { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Tair_f_inst.clm"},
  "prec" :         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Prec.clm"},
  "lwnet" :        { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Lwnet_tavg.clm"},
  "swdown" :       { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.SWdown_f_tavg.clm"},
  "cloud":         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/historical/CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Wind_f_inst.clm"},
  "tamp":          { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "tmin":          { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Tair_min.clm"},
  "tmax":          { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Tair_max.clm"},
  "humid":         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Qair_f_inst.clm"},
  "lightning":     { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/dlightning.clm"},
  //"lightning" :    { "fmt" : CDF, "var" : "HRAC_COM_FR",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/dlightning.nc"},
  "human_ignition": { "fmt" : CLM, "name" : "/p/projects/lpjml/input/GLDAS/INPUT/human_ignition_GLDAS.clm"},
  "popdens" :      { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/popdens_GLDAS.clm"},
  "burntarea" :    { "fmt" : CLM,  "name" : "/p/projects/biodiversity/drueke/mburntarea.clm"},
  "landcover":     { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/co2_1841-2017.dat"},
  "wetdays" :      { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
