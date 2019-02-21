/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  c  r  u  m  o  n  t  h  l  y  .  j  s                  \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 4.0.002                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "include/conf.h" /* include constant definitions */

"inpath" : "/p/projects/lpjml/input/historical",

"input" :
{
  "soil" :         { "fmt" : META, "name" : "input_VERSION2/soil.descr"},
  "coord" :        { "fmt" : CLM,  "name" : "input_VERSION2/grid.bin"},
  "countrycode" :  { "fmt" : CLM,  "name" : "input_VERSION2/cow_full_2018.bin"},
  "landuse" :      { "fmt" : CLM,  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
  /* insert prescribed sdate file name here */
  "grassland_fixed_pft" : { "fmt" : RAW, "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
  "lakes" :        { "fmt" : META, "name" : "input_VERSION2/glwd_lakes_and_rivers.descr"},
  "drainage" :     { "fmt" : CLM,  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : CLM,  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : CLM,  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : CLM,  "name" : "input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.tmp.dat.clm"},
  "prec" :         { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_precip_1901_2013.clm"},
  "lwnet" :        { "fmt" : CLM,  "name" : "input_VERSION2/lwnet_erainterim_1901-2011.clm"},
  "swdown" :       { "fmt" : CLM,  "name" : "input_VERSION2/swdown_erainterim_1901-2011.clm"},
  "cloud":         { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "fmt" : CLM,  "name" : "input_VERSION2/mwindspeed_1860-2100_67420.clm"},
  "tamp":          { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "lightning" :    { "fmt" : CLM,  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "fmt" : CLM, "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "fmt" : CLM,  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "fmt" : TXT,  "name" : "input_VERSION2/co2_1841-2012.dat"},
  "wetdays" :      { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : CLM,  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
