/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  p  u  t  _  c  r  u  m  o  n  t  h  l  y  .  j  s                  \n**/
/**                                                                                \n**/
/** Configuration file for input dataset for LPJ C Version 5.7.9                   \n**/
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
             "loamy sand", "sand", "rock and ice"],

/* The following array is used for mapping prescribed FPC */

"landcovermap" : ["tropical broadleaved evergreen tree", "tropical broadleaved raingreen tree",
                  "temperate needleleaved evergreen tree", "temperate broadleaved evergreen tree",
                  "temperate broadleaved summergreen tree", "boreal needleleaved evergreen tree",
                  "boreal broadleaved summergreen tree", "boreal needleleaved summergreen tree",
                  "Tropical C4 grass", "Temperate C3 grass", "Polar C3 grass"],

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
  "soil" :         { "id" : 41, "fmt" : "meta", "name" : "input_VERSION2/soil.json"},
  "coord" :        { "id" : 43, "fmt" : "clm",  "name" : "input_VERSION2/grid.bin"},
  "landfrac" :     { "id" : 44, "fmt" : "clm",  "name" : "/p/projects/lpjml/inputs/grid/30min/landfrac_gadm36.bin"},
  "countrycode" :  { "id" : 25, "fmt" : "clm",  "name" : "input_VERSION2/cow_full_2018.bin"},
  "no3deposition" : { "id" : 17, "fmt" : "clm",  "name" : "input_VERSION2/no3_deposition_rcp8p5.clm"},
  "nh4deposition" : { "id" : 16, "fmt" : "clm",  "name" : "input_VERSION2/nh4_deposition_rcp8p5.clm"},
  "soilpH" :        { "id" : 26, "fmt" : "clm",  "name" : "input_VERSION2/soil_ph.clm"},
  "landuse" :      { "id" : 6, "fmt" : "clm",  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
//  "landuse" :      { "id" : 6, "fmt" : "clm",  "name" : "/p/projects/lpjml/input/MADRAT/lu_madrat_850-2015_32bands.clm"},
//  "fertilizer_nr" : { "id" : 18, "fmt" :"clm",  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  "fertilizer_nr" : { "id" : 18, "fmt" :"clm",  "name" : "input_VERSION3/fertilizer_luh2v2_1900-2015_32bands.clm"},
  "manure_nr" :    { "id" : 19, "fmt" : "clm", "name" : "input_VERSION3/manure_zhang17_1860-2014_32bands_clm2.clm"},
  "with_tillage" : { "id" : 7, "fmt" : "clm", "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
  "residue_on_field" : { "id" : 8, "fmt" : "clm", "name" : "/p/projects/lpjml/input/MADRAT/residues_madrat_1850-2015_16bands.clm"},
//  "sdate" : { "id" : 28,"fmt" : "clm", "name" : "/p/projects/lpjml/input/historical/input_VERSION2/sdate_combined_filled.clm"}, /* insert prescribed sdate file name here */
  "sdate" : { "id" : 28,"fmt" : "clm", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/GGCMI_CTWN_planting_v1.25.clm2"},
  "grassland_fixed_pft" : { "id" : 29, "fmt" : "raw", "name" : "/home/rolinski/LPJ/Newinput/scenario_MO0.bin"},
  //"landuse" :      { "id" : 6, "fmt" : "clm",  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
  //"fertilizer_nr" : { "id" : 18, "fmt" : "clm",  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  /* insert prescribed sdate file name here */
  "grassland_fixed_pft" : { "id" : 30, "fmt" : "raw", "name" : "/p/projects/landuse/users/rolinski/Newinput/scenario_MO0.bin"},
  "grass_harvest_options" : { "id" : 31, "fmt" : "raw", "name": "/p/projects/landuse/users/rolinski/Newinput/scenario_MO2.bin"},
  "grassland_lsuha" : { "id" : 24, "fmt" : "clm",  "name" : "/p/projects/lpjml/input/Grazing/grassland_lsuha_2000-2000.clm"},
  "sowing_ag_tree_rf" :   { "id" : 32, "fmt" : "cdf", "var" : "planting day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_rf_growing_season_dates_v1.25.nc4"},
  "harvest_ag_tree_rf" :  { "id" : 33, "fmt" : "cdf", "var" : "harvest day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_rf_growing_season_dates_v1.25.nc4"},
  "sowing_ag_tree_ir" :   { "id" : 34, "fmt" : "cdf", "var" : "planting day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_ir_growing_season_dates_v1.25.nc4"},
  "harvest_ag_tree_ir" :  { "id" : 35, "fmt" : "cdf", "var" : "harvest day", "name" : "/p/projects/landuse/users/cmueller/GGCMI/crop_calendar/Cotton_ir_growing_season_dates_v1.25.nc4"},
  "lakes" :        { "id" : 36, "fmt" : "meta", "name" : "input_VERSION2/glwd_lakes_and_rivers.json"},
  "drainage" :     { "id" : 37, "fmt" : "clm",  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "id" : 38, "fmt" : "clm",  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "id" : 39, "fmt" : "clm",  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "id" : 40, "fmt" : "clm",  "name" : "input_VERSION2/reservoir_info_grand5.bin"},
  "temp" :         { "id" : 1, "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.tmp.dat.clm"},
  "prec" :         { "id" : 2, "fmt" : "clm",  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_precip_1901_2013.clm"},
  "lwnet" :        { "id" : 4, "fmt" : "clm",  "name" : "input_VERSION2/lwnet_erainterim_1901-2011.clm"},
  "lwdown" :       { "id" : 43, "fmt" : "clm",  "name" : "DUMMYLOCATION"},
  "swdown" :       { "id" : 3, "fmt" : "clm",  "name" : "input_VERSION2/swdown_erainterim_1901-2011.clm"},
  "cloud":         { "id" : 0, "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.cld.dat.clm"},
  "wind":          { "id" : 15, "fmt" : "clm",  "name" : "input_VERSION2/mwindspeed_1860-2100_67420.clm"},
  "tamp":          { "id" : 3, "fmt" : "clm",  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "lightning" :    { "id" : 23, "fmt" : "clm",  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "id" : 22, "fmt" : "clm", "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "id" : 21, "fmt" : "clm",  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "id" : 13, "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "id" : 42, "fmt" : "clm",  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "co2" :          { "id" : 5, "fmt" : "txt",  "name" : "input_VERSION2/co2_1841-2012.dat"},
  "wetdays" :      { "id" : 12, "fmt" : "clm",  "name" : "CRUDATA_TS3_23/gpcc_v7_cruts3_23_wet_1901_2013.clm"},
  "wateruse" :     { "id" : 20, "fmt" : "clm",  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
