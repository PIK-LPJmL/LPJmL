/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  c  r  o  p  _  p  h  e  n  .  j  s                     \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 5.3.001                 \n**/
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

/* the following array describes the order of the CFT bands in the land use file */

"landusemap" : ["temperate cereals","rice", "maize", "tropical cereals", "pulses",
                "temperate roots", "tropical roots", "oil crops sunflower",
                "oil crops soybean", "oil crops groundnut", "oil crops rapeseed",
                "sugarcane","others","grassland","biomass grass","biomass tree"],

"fertilizermap" : ["temperate cereals","rice", "maize", "tropical cereals", "pulses",
                   "temperate roots", "tropical roots", "oil crops sunflower",
                   "oil crops soybean", "oil crops groundnut", "oil crops rapeseed",
                   "sugarcane","others","grassland","biomass grass","biomass tree"],

"cftmap" : ["temperate cereals","rice", "maize", "tropical cereals", "pulses",
            "temperate roots", "tropical roots", "oil crops sunflower",
            "oil crops soybean", "oil crops groundnut", "oil crops rapeseed",
            "sugarcane"],

"input" :
{
  "soil" :         { "fmt" : "meta", "name" : "input_VERSION2/soil.json"},
  "coord" :        { "fmt" : "clm",  "name" : "input_VERSION2/grid.bin"},
  "countrycode" :  { "fmt" : "clm",  "name" : "input_VERSION2/cow_full_2018.bin"},
  "no3deposition" : { "fmt" : "clm",  "name" : "input_VERSION2/no3_deposition_rcp8p5.clm"},
  "nh4deposition" : { "fmt" : "clm",  "name" : "input_VERSION2/nh4_deposition_rcp8p5.clm"},
  "soilpH" :        { "fmt" : "clm",  "name" : "input_VERSION2/soil_ph.clm"},
  "landuse" :      { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/MADRAT/lu_madrat_850-2015_32bands.clm"},

  "fertilizer_nr" : { "fmt" : "clm",  "name" : "input_VERSION3/fertilizer_luh2v2_1900-2015_32bands.clm"},
  "manure_nr" :    { "fmt" : "clm", "name" : "input_VERSION3/manure_zhang17_1860-2014_32bands_clm2.clm"},
  "with_tillage" : { "fmt" : "clm", "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
  "residue_on_field" : { "fmt" : "clm", "name" : "/p/projects/lpjml/input/MADRAT/residues_madrat_1850-2015_16bands.clm"},
  
  "grassland_fixed_pft" : { "fmt" : "raw", "name" : "/p/projects/landuse/users/rolinski/Newinput/scenario_MO0.bin"},
  "lakes" :        { "fmt" : "meta", "name" : "input_VERSION2/glwd_lakes_and_rivers.json"},
  "drainage" :     { "fmt" : "clm",  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : "clm",  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : "clm",  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : "clm",  "name" : "input_VERSION2/reservoir_info_grand5.bin"},

  /* HadGEM2-ES */

#if (DCLM==CLM_HAD)

  #if (DTIM==TIM_1986_2005)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/tas_HadGEM2-ES_historical_1861-2005.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/tasmax_HadGEM2-ES_historical_1861-2005.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/tasmin_HadGEM2-ES_historical_1861-2005.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/pr_HadGEM2-ES_historical_1861-2005.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/lwnet_HadGEM2-ES_historical_1861-2005.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/rsds_HadGEM2-ES_historical_1861-2005.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/historical/sfcWind_HadGEM2-ES_historical_1861-2005.clm"},

  #elif (DTIM==TIM_2080_2099)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/tas_HadGEM2-ES_rcp60_2006-2099.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/tasmax_HadGEM2-ES_rcp60_2006-2099.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/tasmin_HadGEM2-ES_rcp60_2006-2099.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/pr_HadGEM2-ES_rcp60_2006-2099.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/lwnet_HadGEM2-ES_rcp60_2006-2099.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/rsds_HadGEM2-ES_rcp60_2006-2099.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/HadGEM2-ES/rcp60/sfcWind_HadGEM2-ES_rcp60_2006-2099.clm"},
  #endif
  
  #if (DGS==GS_SA0)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_HadGEM2-ES_historical_1986_2005.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_HadGEM2-ES_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA1)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_HadGEM2-ES_rcp60_2060_2079.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_HadGEM2-ES_rcp60_2060_2079.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA2)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_HadGEM2-ES_rcp60_2080_2099.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_HadGEM2-ES_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA3)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/sdate_HadGEM2-ES_fix_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/PHU_HadGEM2-ES_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA4)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/sdate_HadGEM2-ES_adapt_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/PHU_HadGEM2-ES_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA0_swh)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_swh_only/sdate_HadGEM2-ES_historical_1986_2005.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_swh_only/PHU_HadGEM2-ES_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  #endif


  /* GFDL-ESM2M */

  #elif (DCLM==CLM_GFD)

  #if (DTIM==TIM_1986_2005)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/tas_GFDL-ESM2M_historical_1861-2005.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/tasmax_GFDL-ESM2M_historical_1861-2005.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/tasmin_GFDL-ESM2M_historical_1861-2005.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/pr_GFDL-ESM2M_historical_1861-2005.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/lwnet_GFDL-ESM2M_historical_1861-2005.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/rsds_GFDL-ESM2M_historical_1861-2005.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/historical/sfcWind_GFDL-ESM2M_historical_1861-2005.clm"},

  #elif (DTIM==TIM_2080_2099)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/tas_GFDL-ESM2M_rcp60_2006-2099.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/tasmax_GFDL-ESM2M_rcp60_2006-2099.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/tasmin_GFDL-ESM2M_rcp60_2006-2099.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/pr_GFDL-ESM2M_rcp60_2006-2099.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/lwnet_GFDL-ESM2M_rcp60_2006-2099.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/rsds_GFDL-ESM2M_rcp60_2006-2099.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/GFDL-ESM2M/rcp60/sfcWind_GFDL-ESM2M_rcp60_2006-2099.clm"},
  #endif
  
  #if (DGS==GS_SA0)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_GFDL-ESM2M_historical_1986_2005.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_GFDL-ESM2M_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA1)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_GFDL-ESM2M_rcp60_2060_2079.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_GFDL-ESM2M_rcp60_2060_2079.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA2)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_GFDL-ESM2M_rcp60_2080_2099.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_GFDL-ESM2M_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA3)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/sdate_GFDL-ESM2M_fix_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/PHU_GFDL-ESM2M_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA4)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/sdate_GFDL-ESM2M_adapt_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/PHU_GFDL-ESM2M_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #endif


  /* IPSL-CM5A-LR */

  #elif (DCLM==CLM_IPS)

  #if (DTIM==TIM_1986_2005)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/tas_IPSL-CM5A-LR_historical_1861-2005.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/tasmax_IPSL-CM5A-LR_historical_1861-2005.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/tasmin_IPSL-CM5A-LR_historical_1861-2005.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/pr_IPSL-CM5A-LR_historical_1861-2005.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/lwnet_IPSL-CM5A-LR_historical_1861-2005.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/rsds_IPSL-CM5A-LR_historical_1861-2005.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/historical/sfcWind_IPSL-CM5A-LR_historical_1861-2005.clm"},

  #elif (DTIM==TIM_2080_2099)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/tas_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/tasmax_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/tasmin_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/pr_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/lwnet_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/rsds_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/IPSL-CM5A-LR/rcp60/sfcWind_IPSL-CM5A-LR_rcp60_2006-2099.clm"},
  #endif
  
  #if (DGS==GS_SA0)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_IPSL-CM5A-LR_historical_1986_2005.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_IPSL-CM5A-LR_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA1)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_IPSL-CM5A-LR_rcp60_2060_2079.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_IPSL-CM5A-LR_rcp60_2060_2079.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA2)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_IPSL-CM5A-LR_rcp60_2080_2099.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_IPSL-CM5A-LR_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA3)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/sdate_IPSL-CM5A-LR_fix_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/PHU_IPSL-CM5A-LR_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA4)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/sdate_IPSL-CM5A-LR_adapt_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/PHU_IPSL-CM5A-LR_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #endif

    /* MIROC5 */

  #elif (DCLM==CLM_MIR)

  #if (DTIM==TIM_1986_2005)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/tas_MIROC5_historical_1861-2005.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/tasmax_MIROC5_historical_1861-2005.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/tasmin_MIROC5_historical_1861-2005.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/pr_MIROC5_historical_1861-2005.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/lwnet_MIROC5_historical_1861-2005.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/rsds_MIROC5_historical_1861-2005.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/historical/sfcWind_MIROC5_historical_1861-2005.clm"},

  #elif (DTIM==TIM_2080_2099)
    "temp" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/tas_MIROC5_rcp60_2006-2099.clm"},
    "tmax" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/tasmax_MIROC5_rcp60_2006-2099.clm"},
    "tmin" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/tasmin_MIROC5_rcp60_2006-2099.clm"},
    "prec" :         { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/pr_MIROC5_rcp60_2006-2099.clm"},
    "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/lwnet_MIROC5_rcp60_2006-2099.clm"},
    "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/rsds_MIROC5_rcp60_2006-2099.clm"},
    "wind":          { "fmt" : "clm",  "name" : "/p/projects/ikiimp/ISIMIP2b/input_CLM2/MIROC5/rcp60/sfcWind_MIROC5_rcp60_2006-2099.clm"},
  #endif
  
  #if (DGS==GS_SA0)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_MIROC5_historical_1986_2005.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_MIROC5_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA1)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_MIROC5_rcp60_2060_2079.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_MIROC5_rcp60_2060_2079.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA2)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/sdate_MIROC5_rcp60_2080_2099.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT/PHU_MIROC5_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA3)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/sdate_MIROC5_fix_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_sdate/PHU_MIROC5_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #elif (DGS==GS_SA4)
    "sdate" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/sdate_MIROC5_adapt_sdate.bin"},  /* insert prescribed sdate file name here */
    "crop_phu" : {"fmt" : "clm", "name" : "/p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/DATA/CROP_CALENDARS/LPJML_INPUT_fix_hdate/PHU_MIROC5_rcp60_2080_2099.bin"},  /* insert prescribed phu file name here */
  #endif


#endif /* CLM */

  "lightning" :    { "fmt" : "clm",  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "fmt" : "clm", "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "fmt" : "clm",  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  
  //"co2" :          { "fmt" : "txt",  "name" : "input_VERSION2/co2_1841-2012.dat"},
  #if (DCO2==CO2_DYNA)
  "co2" :  { "fmt" : "txt",  "name" : "/p/projects/lpjml/input/scenarios/rcp60_CO2_1765-2200.dat"},
  #elif (DCO2==CO2_STAT)
  "co2" :  { "fmt" : "txt",  "name" : "/p/projects/lpjml/input/scenarios/co2_historical_1861-2005_static2005_until2100.dat"},
  #endif

  "wetdays" :      { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : "clm",  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
