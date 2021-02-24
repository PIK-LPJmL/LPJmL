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

#include "include/conf.h" /* include constant definitions */

"inpath" : "/p/projects/lpjml/input/historical",

"input" :
{
  "soil"  : { "fmt" : META, "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/soil_mappy_0.5deg_test.descr"},
  "coord" : { "fmt" : CLM,  "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/grid_mappy_0.5deg_67420.bin"},
  "countrycode" : { "fmt" : CDF, "var" : "country", "name" : "/p/projects/biodiversity/cru_netcdf/cow_full_2018.nc"},
  "regioncode"  : { "fmt" : CDF, "var" : "region", "name" : "/p/projects/biodiversity/cru_netcdf/reg_full_2018.nc"},
  //"no3deposition" : { "fmt" : CLM,  "name" : "input_VERSION2/no3_deposition_rcp8p5.clm"},
  //"nh4deposition" : { "fmt" : CLM,  "name" : "input_VERSION2/nh4_deposition_rcp8p5.clm"},
  "no3deposition" : { "fmt" : CDF, "var" : "no3deposition", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/no3_deposition_2015soc_1850-2100.nc"},
  "nh4deposition" : { "fmt" : CDF, "var" : "nh4deposition", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/nh4_deposition_2015soc_1850-2100.nc"},
  //"soilpH" :        { "fmt" : CLM,  "name" : "input_VERSION2/soil_ph.clm"},
  "soilpH" :        { "fmt" : CDF, "var" : "soilpH", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/soil_pH.nc4"},
  //"landuse" :      { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/MADRAT/lu_madrat_850-2015_32bands.clm"},
  //"fertilizer_nr" : { "fmt" : CLM,  "name" : "input_VERSION3/fertilizer_luh2v2_1900-2015_32bands.clm"},
  //"manure_nr" :    { "fmt" : CLM, "name" : "input_VERSION3/manure_zhang17_1860-2014_32bands_clm2.clm"},
  //"with_tillage" : { "fmt" : CLM, "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
  //"residue_on_field" : { "fmt" : CLM, "name" : "/p/projects/lpjml/input/MADRAT/residues_madrat_1850-2015_16bands.clm"},
  
  "landuse" :       { "fmt" : CDF, "var" : "cftfrac", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/lu_madrat_850-2015_32bands.nc"},
  "fertilizer_nr" : { "fmt" : CDF, "var" : "nfertilizer", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/fertilizer_luh2v2_1900-2015_32bands.nc"},
  "manure_nr" :     { "fmt" : CDF, "var" : "manure", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/manure_zhang17_1860-2014_32bands_clm2.nc"},
  //"with_tillage" :  { "fmt" : CDF, "var" : "tillage", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/lpj_tillage_CA_1973-2010.nc"},
  //"residue_on_field" : { "fmt" : CDF, "var" : "residues", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/residues_madrat_1850-2015_16bands.nc"},

  //"grassland_fixed_pft" : { "fmt" : RAW, "name" : "/p/projects/landuse/users/rolinski/Newinput/scenario_MO0.bin"},
  "lakes" :        { "fmt" : META, "name" : "input_VERSION2/glwd_lakes_and_rivers.descr"},
  "drainage" :     { "fmt" : CLM,  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : CLM,  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : CLM,  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : CLM,  "name" : "input_VERSION2/reservoir_info_grand5.bin"},

/* WFDEI */

#if (DCLM==CLM_WFD)

    "temp" :         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/tas_wfdei_1979-2012.clm2"},
    "tmax" :         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/tasmax_wfdei_1979_2012.clm"},
    "tmin" :         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/tasmin_wfdei_1979_2012.clm"},
    "prec" :         { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/prtotal_cru_wfdei_1979-2012.clm2"},
    "lwnet" :        { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/lwnet_wfdei_1979-2012.clm2"},
    "swdown" :       { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/GGCMI_phase1/WFDEI/rsds_wfdei_1979-2012.clm2"},
    "wind":          { "fmt" : CLM,  "name" : "/p/projects/lpjml/input/historical/ISIMIP3a/obsclim/GSWP3-W5E5/sfcwind_gswp3-w5e5_obsclim_1901-2016.clm"},
    //"wind":          { "fmt" : CDF, "var" : "mwindspeed", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/mwindspeed_67420_1900-2015.nc"},

/* (DGS==GS_SA0) */
#if (DGS==GS_SA0)
    "sdate" : {"fmt" : CLM, "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_WFDEI_historical_1986_2005.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : CLM, "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_WFDEI_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
#elif (DGS==GS_OA0)
    "sdate" : {"fmt" : CLM, "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_WFDEI_ggcmi_ph3.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : CLM, "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_WFDEI_historical_1986_2005_ggcmi_phase3.bin"},  /* insert prescribed phu file name here */
    //  "sdate"    : {"fmt" : CDF, "var" : "sdate", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/sdate_WFDEI_ggcmi_ph3.nc"},
    //  "crop_phu" : {"fmt" : CDF, "var" : "phu", "name" : "/p/projects/macmit/data/MAPPY/lpjml_inputs/PHU_WFDEI_historical_1986_2005_ggcmi_phase3.nc"},
#endif /* WFDEI  */
#endif /* CLM */

  "lightning" :    { "fmt" : CLM,  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "fmt" : CLM, "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "fmt" : CLM,  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  
  //"co2" :          { "fmt" : TXT,  "name" : "input_VERSION2/co2_1841-2012.dat"},
  #if (DCO2==CO2_DYNA)
  "co2" :  { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/rcp60_CO2_1765-2200.dat"},
  #elif (DCO2==CO2_STAT)
  "co2" :  { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/co2_historical_1861-2005_static2005_until2100.dat"},
  #endif

  "wetdays" :      { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : CLM,  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
