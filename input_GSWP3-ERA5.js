/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  G  S  W  P  3 _  E  R  A  5  .  j  s                   \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 5.2.002                 \n**/
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
  "soil" :         { "fmt" : "meta", "name" : "input_VERSION2/soil.descr"},
  "coord" :        { "fmt" : "clm",  "name" : "input_VERSION2/grid.bin"},
  "countrycode" :  { "fmt" : "clm",  "name" : "input_VERSION2/cow_full_2018.bin"},
  "no3deposition" : { "fmt" : "clm",  "name" : "input_VERSION2/no3_deposition_rcp8p5.clm"},
  "nh4deposition" : { "fmt" : "clm",  "name" : "input_VERSION2/nh4_deposition_rcp8p5.clm"},
  "soilpH" :        { "fmt" : "clm",  "name" : "input_VERSION2/soil_ph.clm"},
//  "landuse" :      { "fmt" : "clm",  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
//  "fertilizer_nr" : { "fmt" : "clm",  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  "landuse" :      { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/MADRAT/lu_madrat_850-2015_32bands.clm"},
//  "fertilizer_nr" : { "fmt" : "clm",  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  "fertilizer_nr" : { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/MADRAT/fertilizer_luh2v2_1900-2015_32bands.clm"},
  "manure_nr" :    { "fmt" : "clm", "name" : "/p/projects/lpjml/input/MADRAT/manure_zhang17_1860-2014_32bands_clm2.clm"},
  /* insert prescribed sdate file name here */
  "with_tillage" : { "fmt" : "clm", "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
  "residue_on_field" : { "fmt" : "clm", "name" : "/p/projects/lpjml/input/MADRAT/residues_madrat_1850-2015_16bands.clm"},
  //"sdate" : {"fmt" : "clm", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/GGCMI_CTWN_planting_v1.25.clm2"},
  "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/input/sdate_1996-2005_24bands_ggcmi-ctwn-v1.25.clm2"},  /* insert prescribed sdate file name here */
  //"crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/input/phu_1996-2005_24bands_ggcmi-ctwn-v1.25.clm2"},  /* insert prescribed phu file name here */
  //"crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/SCRIPTS/Compute_GPs_PHUs/input/PHU_HadGEM2-ES_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/SCRIPTS/Compute_GPs_PHUs/input/PHU_WFDEI_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  //"grassland_fixed_pft" : { "fmt" : "raw", "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
  "grassland_fixed_pft" : { "fmt" : "raw", "name" : "/p/projects/landuse/users/rolinski/Newinput/scenario_MO0.bin"},
  "grass_harvest_options" : { "fmt" : "raw", "name": "/p/projects/landuse/users/rolinski/Newinput/scenario_MO2.bin"},
  "lakes" :        { "fmt" : "meta", "name" : "input_VERSION2/glwd_lakes_and_rivers.descr"},
  "drainage" :     { "fmt" : "clm",  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : "clm",  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : "clm",  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : "clm",  "name" : "input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "fmt" : "clm",  "name" : "GSWP3-ERA5/tas_gswp3-era5_1901-2018.clm"},
  "prec" :         { "fmt" : "clm",  "name" : "GSWP3-ERA5/pr_gswp3-era5_1901-2018.clm"},
  "lwdown" :       { "fmt" : "clm",  "name" : "GSWP3-ERA5/rlds_gswp3-era5_1901-2018.clm"},
  "swdown" :       { "fmt" : "clm",  "name" : "GSWP3-ERA5/rsds_gswp3-era5_1901-2018.clm"},
  "cloud":         { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "fmt" : "clm",  "name" : "GSWP3-ERA5/wind_gswp3-era5_1901-2018.clm"},
  "tamp":          { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "lightning" :    { "fmt" : "clm",  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "fmt" : "clm", "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "fmt" : "clm",  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "fmt" : "txt",  "name" : "input_VERSION2/co2_1841-2018.dat"},
  "wetdays" :      { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : "clm",  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
