/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  c  r  u  m  o  n  t  h  l  y  .  j  s                  \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 5.1.001                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

//#include "include/conf.h" /* include constant definitions */

"inpath" : "/p/projects/lpjml/input",

"input" :
{
  "soil" :         { "fmt" : META, "name" : "ISIMIP3/soil_ISISMIP3.descr"},
//  "soil" :         { "fmt" : META, "name" : "historical/input_VERSION2/soil.descr"},
  "coord" :        { "fmt" : CLM,  "name" : "ISIMIP3/grid.bin"},
  "countrycode" :  { "fmt" : CLM,  "name" : "historical/input_VERSION2/cow_full_2018.bin"},
  "no3deposition" : { "fmt" : CLM,  "name" : "ISIMIP3/no3_deposition_2015soc_1850-2100.clm"},
  "nh4deposition" : { "fmt" : CLM,  "name" : "ISIMIP3/nh4_deposition_2015soc_1850-2100.clm"},
  "soilpH" :        { "fmt" : CLM,  "name" : "historical/input_VERSION2/soil_ph.clm"},
  "landuse" :      { "fmt" : CLM,  "name" : "ISIMIP3/cft_38bands_2015soc_1601-2100.clm"},
//  "landuse" :      { "fmt" : CLM,  "name" : "historical/input_VERSION2/cft2005_38bands_dummy.bin"},
  "fertilizer_nr" : { "fmt" : CLM,  "name" : "ISIMIP3/nfertilizer_isimip3b_38bands_2015soc_1850-2015.clm"},
  "manure_nr" :    { "fmt" : CLM, "name" : "ISIMIP3/manure_isimip3b_38bands_2015soc_1850-2015.clm"},
//  "with_tillage" : { "fmt" : CLM, "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
//  "residue_on_field" : { "fmt" : CLM, "name" : "input_VERSION3/residues_madrat_2015_19bands_dummy.clm"},
  "sdate" : {"fmt" : CLM, "name" : "/p/projects/lpjml/input/crop_calendar/sdates_ggcmi_phase3_v1.01_67420.clm"},  /* insert prescribed sdate file name here */
  "crop_phu" : {"fmt" : CLM, "name" : "/p/projects/lpjml/input/crop_calendar/phu_wfdei_ggcmi_phase3_v1.01_67420.clm"},  /* insert prescribed phu file name here */
//  "grassland_fixed_pft" : { "fmt" : RAW, "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
//  "lakes" :        { "fmt" : META, "name" : "input_VERSION2/glwd_lakes_and_rivers.descr"},
//  "drainage" :     { "fmt" : CLM,  "name" : "input_VERSION2/drainagestn.bin"},
//  "neighb_irrig" : { "fmt" : CLM,  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
//  "elevation" :    { "fmt" : CLM,  "name" : "input_VERSION2/elevation.bin"},
//  "reservoir" :    { "fmt" : CLM,  "name" : "input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "fmt" : CLM,  "name" : "historical/GGCMI_phase1/WFDEI/tas_wfdei_1979-2012.clm2"},
  "tmax" :         { "fmt" : CLM,  "name" : "historical/GGCMI_phase1/WFDEI/tasmax_wfdei_1979_2012.clm"},
  "tmin" :         { "fmt" : CLM,  "name" : "historical/GGCMI_phase1/WFDEI/tasmin_wfdei_1979_2012.clm"},
  "prec" :         { "fmt" : CLM,  "name" : "historical/GGCMI_phase1/WFDEI/prtotal_cru_wfdei_1979-2012.clm2"},
  "lwnet" :        { "fmt" : CLM,  "name" : "historical/GGCMI_phase1/WFDEI/lwnet_wfdei_1979-2012.clm2"},
  "swdown" :       { "fmt" : CLM,  "name" : "historical/GGCMI_phase1/WFDEI/rsds_wfdei_1979-2012.clm2"},
  //"cloud":         { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "fmt" : CLM,  "name" : "historical/input_VERSION2/mwindspeed_1860-2100_67420.clm"},
//  "tamp":          { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
//  "lightning" :    { "fmt" : CLM,  "name" : "input_VERSION2/mlightning.clm"},
//  "human_ignition": { "fmt" : CLM, "name" : "input_VERSION2/human_ignition.clm"},
//  "popdens" :      { "fmt" : CLM,  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
//  "burntarea" :    { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
//  "landcover":     { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "fmt" : TXT,  "name" : "historical/input_VERSION2/co2_1841-2012.dat"},
//  "wetdays" :      { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
//  "wateruse" :     { "fmt" : CLM,  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
