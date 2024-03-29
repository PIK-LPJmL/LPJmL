/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  G  L  D  A  S  .  j  s                                 \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 5.3.001                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

"soilmap" : [null,"clay", "silty clay", "sandy clay", "clay loam", "silty clay loam",
             "sandy clay loam", "loam", "silt loam", "sandy loam", "silt",
             "loamy sand", "sand", "rock and ice"],

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
  "soil" :         { "fmt" : "raw",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/soil_GLDAS.bin"},
  "coord" :        { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/grid_GLDAS.clm"},
  "countrycode" :  { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/country_GLDAS.clm"},
  "landuse" :      { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/landuse_GLDAS.clm"},
  /* insert prescribed sdate file name here */
  "grassland_fixed_pft" : { "fmt" : "raw", "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
  "lakes" :        { "fmt" : "meta", "name" : "/p/projects/lpjml/input/historical/input_VERSION2/glwd_lakes_and_rivers.json"},
  "drainage" :     { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Tair_f_inst.clm"},
  "prec" :         { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Prec.clm"},
  "lwnet" :        { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Lwnet_tavg.clm"},
  "swdown" :       { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.SWdown_f_tavg.clm"},
  "cloud":         { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Wind_f_inst.clm"},
  "tamp":          { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "tmin":          { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Tair_min.clm"},
  "tmax":          { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Tair_max.clm"},
  "humid":         { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/GLDAS_NOAH05_daily_1948-2017.Qair_f_inst.clm"},
  "lightning":     { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/dlightning.clm"},
  //"lightning" :    { "fmt" : "cdf", "var" : "HRAC_COM_FR",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/dlightning.nc"},
  "human_ignition": { "fmt" : "clm", "name" : "/p/projects/lpjml/input/GLDAS/INPUT/human_ignition_GLDAS.clm"},
  "popdens" :      { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/GLDAS/INPUT/popdens_GLDAS.clm"},
  "burntarea" :    { "fmt" : "clm",  "name" : "/p/projects/biodiversity/drueke/mburntarea.clm"},
  "landcover":     { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "fmt" : "txt",  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/co2_1841-2017.dat"},
  "wetdays" :      { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
