/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  c  r  u  m  o  n  t  h  l  y  .  j  s                  \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 6.0.001                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

"inpath" : "/p/projects/lpjml/input/historical",

"soilmap" : [null,"clay", "silty clay", "sandy clay", "clay loam", "silty clay loam",
             "sandy clay loam", "loam", "silt loam", "sandy loam", "silt",
             "loamy sand", "sand" , "rock and ice"],

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
  "delta_year" : 100,
  "icefrac" :      { "fmt" : "clm", "name" : "/p/projects/open/sibyll/palmod/climber_experiment/climber2_lgc_ice_0.5.clm"},
  "delta_temp" :   { "fmt" : "clm", "name" : "/p/projects/open/sibyll/palmod/climber_experiment/climber2_lgc_temp_0.5.clm"},
  "delta_prec" :   { "fmt" : "clm", "name" : "/p/projects/open/sibyll/palmod/climber_experiment/climber2_lgc_prec_0.5.clm"},
  "delta_lwnet" :  { "fmt" : "clm", "name" : "/p/projects/open/sibyll/palmod/climber_experiment/climber2_lgc_lwnet_0.5.clm"},
  "delta_swdown" : { "fmt" : "clm", "name" : "/p/projects/open/sibyll/palmod/climber_experiment/climber2_lgc_swdown_0.5.clm"},
  "soil" :         { "fmt" : "meta", "name" : "input_VERSION2/soil.descr"},
  "coord" :        { "fmt" : "clm", "name" : "input_VERSION2/grid.bin"},
  "countrycode" :  { "fmt" : "clm", "name" : "input_VERSION2/cow_full_2018.bin"},
  "no3deposition" :{ "fmt" : "clm", "name" : "input_VERSION2/no3_deposition_rcp8p5.clm"},
  "nh4deposition" :{ "fmt" : "clm", "name" : "input_VERSION2/nh4_deposition_rcp8p5.clm"},
  "soilpH" :       { "fmt" : "clm", "name" : "input_VERSION2/soil_ph.clm"},
  "kbf" :          { "fmt" : "cdf", "var" : "KBF", "name" :"input_VERSION2/kbf_mar.nc"},
  "slope_mean" :   { "fmt" : "clm", "name" : "input_VERSION2/GloSlope-30as_mean.clm"},
  "slope_min"  :   { "fmt" : "clm", "name" : "input_VERSION2/GloSlope-30as_min.clm"},
  "slope_max"  :   { "fmt" : "clm", "name" : "input_VERSION2/GloSlope-30as_max.clm"},
  "ch4" :          { "fmt" : "txt", "name" : "input_VERSION2/CH4_concentration_lpj.csv"},
  "hydrotopes" :   { "fmt" : "clm", "name" : "input_VERSION2/hydro2_shuffle.bin"},

//  "landuse" :      { "fmt" : "clm",  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
  "landuse" :      { "fmt" : "clm",  "name" : "/p/projects/lpjml/input/MADRAT/lu_madrat_850-2015_32bands.clm"},
//  "fertilizer_nr" : { "fmt" :"clm",  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  "fertilizer_nr" : { "fmt" :"clm",  "name" : "input_VERSION3/fertilizer_luh2v2_1900-2015_32bands.clm"},
  "manure_nr" :    { "fmt" : "clm", "name" : "input_VERSION3/manure_zhang17_1860-2014_32bands_clm2.clm"},
  "with_tillage" : { "fmt" : "clm", "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
  "residue_on_field" : { "fmt" : "clm", "name" : "/p/projects/lpjml/input/MADRAT/residues_madrat_1850-2015_16bands.clm"},
//  "sdate" : {"fmt" : "clm", "name" : "/p/projects/lpjml/input/historical/input_VERSION2/sdate_combined_filled.clm"}, /* insert prescribed sdate file name here */
  "sdate" : {"fmt" : "clm", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/GGCMI_CTWN_planting_v1.25.clm2"},
  "grassland_fixed_pft" : { "fmt" : "raw", "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
  //"landuse" :      { "fmt" : "clm",  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
  //"fertilizer_nr" : { "fmt" : "clm",  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  /* insert prescribed sdate file name here */
  "grassland_fixed_pft" : { "fmt" : "raw", "name" : "/p/projects/landuse/users/rolinski/Newinput/scenario_MO0.bin"},
  "grass_harvest_options" : { "fmt" : "raw", "name": "/p/projects/landuse/users/rolinski/Newinput/scenario_MO2.bin"},
   "sowing_ag_tree_rf" :   { "fmt" : "cdf", "var" : "planting day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_rf_growing_season_dates_v1.25.nc4"},
  "harvest_ag_tree_rf" :  { "fmt" : "cdf", "var" : "harvest day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_rf_growing_season_dates_v1.25.nc4"},
  "sowing_ag_tree_ir" :   { "fmt" : "cdf", "var" : "planting day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_ir_growing_season_dates_v1.25.nc4"},
  "harvest_ag_tree_ir" :  { "fmt" : "cdf", "var" : "harvest day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_ir_growing_season_dates_v1.25.nc4"},
  "lakes" :        { "fmt" : "meta", "name" : "input_VERSION2/glwd_lakes_and_rivers.descr"},
  "drainage" :     { "fmt" : "clm",  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : "clm",  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : "clm",  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : "clm",  "name" : "input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.tmp.dat.clm"},
  "prec" :         { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_precip_1901_2013.clm"},
  "lwnet" :        { "fmt" : "clm",  "name" : "input_VERSION2/lwnet_erainterim_1901-2011.clm"},
  "swdown" :       { "fmt" : "clm",  "name" : "input_VERSION2/swdown_erainterim_1901-2011.clm"},
  "cloud":         { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "fmt" : "clm",  "name" : "input_VERSION2/mwindspeed_1860-2100_67420.clm"},
  "tamp":          { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "lightning" :    { "fmt" : "clm",  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "fmt" : "clm", "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "fmt" : "clm",  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "fmt" : "txt",  "name" : "input_VERSION2/co2_1841-2012.dat"},
  "wetdays" :      { "fmt" : "clm",  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "fmt" : "clm",  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
