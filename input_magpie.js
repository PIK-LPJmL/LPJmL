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
  "soil" :         { "fmt" : META, "name" : "input_VERSION2/soil.json"},
  "coord" :        { "fmt" : CLM,  "name" : "input_VERSION2/grid.bin"},
  "countrycode" :  { "fmt" : CLM,  "name" : "input_VERSION2/cow_full_2018.bin"},
  "no3deposition" : { "fmt" : CLM,  "name" : "input_VERSION2/no3_deposition_rcp8p5.clm"},
  "nh4deposition" : { "fmt" : CLM,  "name" : "input_VERSION2/nh4_deposition_rcp8p5.clm"},
  "soilpH" :        { "fmt" : CLM,  "name" : "input_VERSION2/soil_ph.clm"},
  "landuse" :      { "fmt" : CLM,  "name" : "input_VERSION2/cft1700_2005_irrigation_systems_64bands.bin"},
  //"fertilizer_nr" : { "fmt" : CLM,  "name" : "input_VERSION2/fertilizer_ggcmi.clm2"},
  "fertilizer_nr" : { "fmt" : CLM,  "name" : "input_VERSION3/fertilizer_luh2v2_1900-2015_32bands.clm"},
  "manure_nr" :    { "fmt" : CLM, "name" : "input_VERSION3/manure_zhang17_1860-2014_32bands_clm2.clm"},
  "with_tillage" : { "fmt" : CLM, "name" : "input_VERSION3/lpj_tillage_CA_1973-2010.clm"},
  "residue_on_field" : { "fmt" : CLM, "name" : "/p/projects/lpjml/input/MADRAT/residues_madrat_1850-2015_16bands.clm"},
  //"sdate" : {"fmt" : CLM, "name" : "/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/input/sdate_1996-2005_24bands_ggcmi-ctwn-v1.25.clm2"},  /* insert prescribed sdate file name here */
  "sdate" : {"fmt" : CLM, "name" : "/p/projects/lpjml/input/crop_calendar/sdates_ggcmi_phase3_v1.01_67420_24bands.clm"},  /* insert prescribed sdate file name here */
  //"crop_phu" : {"fmt" : CLM, "name" : "/p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/SCRIPTS/Compute_GPs_PHUs/input/PHU_WFDEI_historical_1986_2005.bin"},  /* insert prescribed phu file name here */
  "crop_phu" : {"fmt" : CLM, "name" : "/p/projects/lpjml/input/crop_calendar/phu_agmerra_ggcmi_phase3_v1.01_67420_24bands.clm"},  /* insert prescribed phu file name here */
  /* insert prescribed sdate file name here */
  "grassland_fixed_pft" : { "fmt" : RAW, "name" : "/p/projects/landuse/users/rolinski/Newinput/scenario_MO0.bin"},
  "grass_harvest_options" : { "fmt" : RAW, "name": "/p/projects/landuse/users/rolinski/Newinput/scenario_MO2.bin"},
  "lakes" :        { "fmt" : META, "name" : "input_VERSION2/glwd_lakes_and_rivers.json"},
  "drainage" :     { "fmt" : CLM,  "name" : "input_VERSION2/drainagestn.bin"},
  "neighb_irrig" : { "fmt" : CLM,  "name" : "input_VERSION2/neighb_irrig_stn.bin"},
  "elevation" :    { "fmt" : CLM,  "name" : "input_VERSION2/elevation.bin"},
  "reservoir" :    { "fmt" : CLM,  "name" : "input_VERSION2/reservoir_info_grand5.bin"},
#ifdef NOCO2
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/co2_historical_1861-2005_static2005_until2100.dat"},
#else
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/historical/input_VERSION2/co2_1841-2018.dat"},
#ifdef RCP8p5
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/rcp85_CO2_1765-2200.dat"},
#elif RCP6p0
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/rcp60_CO2_1765-2200.dat"},
#elif RCP4p5
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/rcp45_CO2_1765-2200.dat"},
#elif RCP2p6
  "co2" :          { "fmt" : TXT,  "name" : "/p/projects/lpjml/input/scenarios/rcp26_CO2_1765-2200.dat"},
#endif
#endif
#ifdef FROM_RESTART
#ifdef CRU4
  "temp" :         { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.tmp.clm"},
  "prec" :         { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.pre.clm"},
  "cloud":         { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.cld.clm"},
#elif HADGEM
#ifdef RCP8p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp8p5/lwnet_hadgem2-es_rcp8p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp8p5/rsds_hadgem2-es_rcp8p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp8p5/tas_hadgem2-es_rcp8p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp8p5/pr_hadgem2-es_rcp8p5_1951-2099.clm"},
#elif RCP6p0
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp6p0/lwnet_hadgem2-es_rcp6p0_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp6p0/rsds_hadgem2-es_rcp6p0_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp6p0/tas_hadgem2-es_rcp6p0_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp6p0/pr_hadgem2-es_rcp6p0_1951-2099.clm"},
#elif RCP4p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp4p5/lwnet_hadgem2-es_rcp4p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp4p5/rsds_hadgem2-es_rcp4p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp4p5/tas_hadgem2-es_rcp4p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp4p5/pr_hadgem2-es_rcp4p5_1951-2099.clm"},
#elif RCP2p6
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp2p6/lwnet_hadgem2-es_rcp2p6_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp2p6/rsds_hadgem2-es_rcp2p6_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp2p6/tas_hadgem2-es_rcp2p6_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/rcp2p6/pr_hadgem2-es_rcp2p6_1951-2099.clm"},
#endif
#elif IPSL
#ifdef RCP8p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp8p5/lwnet_ipsl-cm5a-lr_rcp8p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp8p5/rsds_ipsl-cm5a-lr_rcp8p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp8p5/tas_ipsl-cm5a-lr_rcp8p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp8p5/pr_ipsl-cm5a-lr_rcp8p5_1951-2099.clm"},
#elif RCP6p0
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp6p0/lwnet_ipsl-cm5a-lr_rcp6p0_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp6p0/rsds_ipsl-cm5a-lr_rcp6p0_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp6p0/tas_ipsl-cm5a-lr_rcp6p0_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp6p0/pr_ipsl-cm5a-lr_rcp6p0_1951-2099.clm"},
#elif RCP4p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp4p5/lwnet_ipsl-cm5a-lr_rcp4p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp4p5/rsds_ipsl-cm5a-lr_rcp4p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp4p5/tas_ipsl-cm5a-lr_rcp4p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp4p5/pr_ipsl-cm5a-lr_rcp4p5_1951-2099.clm"},
#elif RCP2p6
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp2p6/lwnet_ipsl-cm5a-lr_rcp2p6_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp2p6/rsds_ipsl-cm5a-lr_rcp2p6_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp2p6/tas_ipsl-cm5a-lr_rcp2p6_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/rcp2p6/pr_ipsl-cm5a-lr_rcp2p6_1951-2099.clm"},
#endif
#elif MIROC
#ifdef RCP8p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp8p5/lwnet_miroc-esm-chem_rcp8p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp8p5/rsds_miroc-esm-chem_rcp8p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp8p5/tas_miroc-esm-chem_rcp8p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp8p5/pr_miroc-esm-chem_rcp8p5_1951-2099.clm"},
#elif RCP6p0
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp6p0/lwnet_miroc-esm-chem_rcp6p0_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp6p0/rsds_miroc-esm-chem_rcp6p0_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp6p0/tas_miroc-esm-chem_rcp6p0_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp6p0/pr_miroc-esm-chem_rcp6p0_1951-2099.clm"},
#elif RCP4p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp4p5/lwnet_miroc-esm-chem_rcp4p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp4p5/rsds_miroc-esm-chem_rcp4p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp4p5/tas_miroc-esm-chem_rcp4p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp4p5/pr_miroc-esm-chem_rcp4p5_1951-2099.clm"},
#elif RCP2p6
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp2p6/lwnet_miroc-esm-chem_rcp2p6_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp2p6/rsds_miroc-esm-chem_rcp2p6_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp2p6/tas_miroc-esm-chem_rcp2p6_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/rcp2p6/pr_miroc-esm-chem_rcp2p6_1951-2099.clm"},
#endif
#elif NORESM
#ifdef RCP8p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp8p5/lwnet_noresm1-m_rcp8p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp8p5/rsds_noresm1-m_rcp8p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp8p5/tas_noresm1-m_rcp8p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp8p5/pr_noresm1-m_rcp8p5_1951-2099.clm"},
#elif RCP6p0
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp6p0/lwnet_noresm1-m_rcp6p0_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp6p0/rsds_noresm1-m_rcp6p0_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp6p0/tas_noresm1-m_rcp6p0_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp6p0/pr_noresm1-m_rcp6p0_1951-2099.clm"},
#elif RCP4p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp4p5/lwnet_noresm1-m_rcp4p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp4p5/rsds_noresm1-m_rcp4p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp4p5/tas_noresm1-m_rcp4p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp4p5/pr_noresm1-m_rcp4p5_1951-2099.clm"},
#elif RCP2p6
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp2p6/lwnet_noresm1-m_rcp2p6_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp2p6/rsds_noresm1-m_rcp2p6_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp2p6/tas_noresm1-m_rcp2p6_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/rcp2p6/pr_noresm1-m_rcp2p6_1951-2099.clm"},
#endif
#elif GFDL
#ifdef RCP8p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp8p5/lwnet_gfdl-esm2m_rcp8p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp8p5/rsds_gfdl-esm2m_rcp8p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp8p5/tas_gfdl-esm2m_rcp8p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp8p5/pr_gfdl-esm2m_rcp8p5_1951-2099.clm"},
#elif RCP6p0
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp6p0/lwnet_gfdl-esm2m_rcp6p0_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp6p0/rsds_gfdl-esm2m_rcp6p0_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp6p0/tas_gfdl-esm2m_rcp6p0_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp6p0/pr_gfdl-esm2m_rcp6p0_1951-2099.clm"},
#elif RCP4p5
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp4p5/lwnet_gfdl-esm2m_rcp4p5_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp4p5/rsds_gfdl-esm2m_rcp4p5_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp4p5/tas_gfdl-esm2m_rcp4p5_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp4p5/pr_gfdl-esm2m_rcp4p5_1951-2099.clm"},
#elif RCP2p6
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp2p6/lwnet_gfdl-esm2m_rcp2p6_1951-2099.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp2p6/rsds_gfdl-esm2m_rcp2p6_1951-2099.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp2p6/tas_gfdl-esm2m_rcp2p6_1951-2099.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/rcp2p6/pr_gfdl-esm2m_rcp2p6_1951-2099.clm"},
#endif
#endif
#else
#ifdef CRU4
  "temp" :         { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.tmp.clm"},
  "prec" :         { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.pre.clm"},
  "cloud":         { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.cld.clm"},
#elif HADGEM
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/spinup/lwnet_hadgem2-es_historical_1951-1980_detrended.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/spinup/rsds_hadgem2-es_historical_1951-1980_detrended.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/spinup/tas_hadgem2-es_historical_1951-1980_detrended.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/HadGEM2-ES/spinup/pr_hadgem2-es_historical_1951-1980_detrended.clm"},
#elif IPSL
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/spinup/lwnet_ipsl-cm5a-lr_historical_1951-1980_detrended.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/spinup/rsds_ipsl-cm5a-lr_historical_1951-1980_detrended.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/spinup/tas_ipsl-cm5a-lr_historical_1951-1980_detrended.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/IPSL-CM5A-LR/spinup/pr_ipsl-cm5a-lr_historical_1951-1980_detrended.clm"},
#elif MIROC
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/spinup/lwnet_miroc-esm-chem_historical_1951-1980_detrended.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/spinup/rsds_miroc-esm-chem_historical_1951-1980_detrended.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/spinup/tas_miroc-esm-chem_historical_1951-1980_detrended.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/MIROC-ESM-CHEM/spinup/pr_miroc-esm-chem_historical_1951-1980_detrended.clm"},
#elif NORESM
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/spinup/lwnet_noresm1-m_historical_1951-1980_detrended.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/spinup/rsds_noresm1-m_historical_1951-1980_detrended.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/spinup/tas_noresm1-m_historical_1951-1980_detrended.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/NorESM1-M/spinup/pr_noresm1-m_historical_1951-1980_detrended.clm"},
#elif GFDL
  "lwnet" :        { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/spinup/lwnet_gfdl-esm2m_historical_1951-1980_detrended.clm"},
  "swdown" :       { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/spinup/rsds_gfdl-esm2m_historical_1951-1980_detrended.clm"},
  "temp" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/spinup/tas_gfdl-esm2m_historical_1951-1980_detrended.clm"},
  "prec" :         { "fmt" : CLM2,  "name" : "/p/projects/lpjml/input/scenarios/ISIMIP-FT/GFDL-ESM2M/spinup/pr_gfdl-esm2m_historical_1951-1980_detrended.clm"},
#endif
#endif
  "wind":          { "fmt" : CLM,  "name" : "input_VERSION2/mwindspeed_1860-2100_67420.clm"},
  "tamp":          { "fmt" : CLM,  "name" : "CRUDATA_TS3_23/cru_ts3.23.1901.2014.dtr.dat.clm"}, /* diurnal temp. range */
  "lightning" :    { "fmt" : CLM,  "name" : "input_VERSION2/mlightning.clm"},
  "human_ignition": { "fmt" : CLM, "name" : "input_VERSION2/human_ignition.clm"},
  "popdens" :      { "fmt" : CLM,  "name" : "input_VERSION2/popdens_HYDE3_1901_2011_bi.clm"},
  "burntarea" :    { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/GFED_CNFDB_ALFDB_Interp.BA.360.720.1901.2012.30days.clm"},
  "landcover":     { "fmt" : CLM,  "name" : "/data/biosx/mforkel/input_new/landcover_synmap_koeppen_vcf_newPFT_forLPJ_20130910.clm"},/*synmap_koeppen_vcf_NewPFT_adjustedByLanduse_SpinupTransitionPrescribed_forLPJ.clm*/
  "wetdays" :      { "fmt" : CLM,  "name" : "CRU_TS4.03/cru_ts4.03.1901.2018.wet.clm"},
  "wateruse" :     { "fmt" : CLM,  "name" : "input_VERSION2/wateruse_1900_2000.bin" } /* water consumption for industry,household and livestock */
},
