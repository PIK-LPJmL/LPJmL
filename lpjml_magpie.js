/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  m  a  g  p  i  e  .  j  s                  \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.3.001                         \n**/
/**                                                                                \n**/
/** Configuration file is divided into five sections:                              \n**/
/**                                                                                \n**/
/**  I.   Simulation description and type section                                  \n**/
/**  II.  Input parameter section                                                  \n**/
/**  III. Input data section                                                       \n**/
/**  IV.  Output data section                                                      \n**/
/**  V.   Run settings section                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "include/conf.h" /* include constant definitions */

//#define DAILY_OUTPUT  /* enables daily output */

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/
#ifdef NOCO2
#define FROM_RESTART
#endif

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",       /* LPJML Simulation type with managed land use */
  "version"  : "5.3",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
#ifdef CRU4
  "radiation" : "cloudiness",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
#else
  "radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
#endif
  "fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,
  "river_routing" : false,
  "permafrost" : true,
  "store_climate" : true,
  "const_climate" : false,
  "const_deposition" : false,
  "with_nitrogen" : "lim", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
#ifdef FROM_RESTART
  "new_seed" : false,
  "population" : false,
  "landuse" : "all_crops", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : "no",  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
  //"with_nitrogen" : UNLIM_NITROGEN, //AUTO_FERTILIZER, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "equilsoil" : false,
#else
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
  "equilsoil" : true,
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "prescribed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "crop_phu_option" : "prescribed",     /* PRESCRIBED_CROP_PHU (PHU dataset used, requires PRESCRIBED_SDATE), SEMISTATIC_CROP_PHU (LPJmL4 semi-static PHU approach) */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "black_fallow" : false,
  "no_ndeposition" : false,
  "remove_residuals" : false,           /* remove residuals */
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim",        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "laimax_interpolate" : "laimax_par", /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : "auto",            /* enable fertilizer input */
  "residue_treatment" : "read_residue_data", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "tillage_type" : "read",          /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "till_startyear" : 1850,
  "manure_input" : true,               /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : false,             /* move PFT type others into PFT crop, maize for tropical, wheat for temperate */
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "prescribe_lsuha" : false,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_magpie.js"    /* Input files of CRU dataset */

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef NOCO2
#ifdef HADGEM
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp8p5/noco2
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp6p0/noco2
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp4p5/noco2
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp2p6/noco2
#endif
#elif IPSL
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp8p5/noco2
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp6p0/noco2
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp4p5/noco2
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp2p6/noco2
#endif
#elif MIROC
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp8p5/noco2
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp6p0/noco2
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp4p5/noco2
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp2p6/noco2
#endif
#elif NORESM
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp8p5/noco2
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp6p0/noco2
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp4p5/noco2
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp2p6/noco2
#endif
#elif GFDL
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp8p5/noco2
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp6p0/noco2
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp4p5/noco2
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp2p6/noco2
#endif
#elif CRU4
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/CRU_4/noco2
#endif
#else
#ifdef HADGEM
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp8p5
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp6p0_autofert
//#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp6p0
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp4p5
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/HadGEM2-ES/rcp2p6
#endif
#elif IPSL
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp8p5
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp6p0
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp4p5
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/IPSL_CM5A_LR/rcp2p6
#endif
#elif MIROC
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp8p5
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp6p0
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp4p5
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/MIROC5/rcp2p6
#endif
#elif NORESM
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp8p5
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp6p0
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp4p5
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/NorESM1_M/rcp2p6
#endif
#elif GFDL
#ifdef RCP8p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp8p5
#elif RCP6p0
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp6p0
#elif RCP4p5
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp4p5
#elif RCP2p6
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/GFDL_ESM2M/rcp2p6
#endif
#elif CRU4
#define output /p/projects/landuse/users/cmueller/LPJmLGGCMIp3crops_for_MAgPIE/CRU_4
#endif
#endif

#ifdef WITH_GRIDBASED
  "pft_output_scaled" : GRIDBASED,
#define SUFFIX grid.bin
#else
  "pft_output_scaled" : PFTBASED,
#define SUFFIX pft.bin
#endif

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "crop_index" : TEMPERATE_CEREALS,  /* CFT for daily output */
  "crop_irrigation" : DAILY_RAINFED, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" : 
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : "grid",               "file" : { "fmt" : "raw", "name" : mkstr(output/grid.bin) }},
    { "id" : "sdate",              "file" : { "fmt" : "raw", "name" : mkstr(output/sdate.bin)}},
    { "id" : "hdate",              "file" : { "fmt" : "raw", "name" : mkstr(output/hdate.bin)}},
    { "id" : "meanvegcmangrass",   "file" : { "fmt" : "raw", "name" : mkstr(output/mean_vegc_mangrass.bin)}},
    { "id" : "mgrass_soilc",       "file" : { "fmt" : "raw", "name" : mkstr(output/soilc_mangrass.bin)}},
    { "id" : "mgrass_litc",        "file" : { "fmt" : "raw", "name" : mkstr(output/litc_mangrass.bin)}},
    { "id" : "mgrass_soiln",       "file" : { "fmt" : "raw", "name" : mkstr(output/soiln_mangrass.bin)}},
    { "id" : "mgrass_litn",        "file" : { "fmt" : "raw", "name" : mkstr(output/litn_mangrass.bin)}},
    { "id" : "cft_nfert",          "file" : { "fmt" : "raw", "name" : mkstr(output/cft_nfert.bin)}},
    { "id" : "pft_harvestc",       "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cft_airrig",         "file" : { "fmt" : "raw", "name" : mkstr(output/cft_airrig.SUFFIX)}},
    { "id" : "cft_consump_water_b","file" : { "fmt" : "raw", "name" : mkstr(output/cft_consump_water_b.SUFFIX)}},
    { "id" : "cft_transp_b","file" : { "fmt" : "raw", "name" : mkstr(output/cft_transp_b.SUFFIX)}},
    { "id" : "cft_evap_b","file" : { "fmt" : "raw", "name" : mkstr(output/cft_evap_b.SUFFIX)}},
    { "id" : "cft_interc_b","file" : { "fmt" : "raw", "name" : mkstr(output/cft_interc_b.SUFFIX)}},
    { "id" : "cftfrac",            "file" : { "fmt" : "raw", "name" : mkstr(output/cftfrac.bin)}},
    { "id" : "cft_consump_water_b","file" : { "fmt" : "raw", "name" : mkstr(output/cft_consump_water_b.SUFFIX)}},
    { "id" : "cft_transp_b",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_transp_b.SUFFIX)}},
    { "id" : "cft_evap_b",         "file" : { "fmt" : "raw", "name" : mkstr(output/cft_evap_b.SUFFIX)}},
    { "id" : "cft_interc_b",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_interc_b.SUFFIX)}}
    { "id" : "decay_leaf_nv",      "file" : { "fmt" : "raw", "name" : mkstr(output/decay_leaf_nv.bin)}},
    { "id" : "decay_leaf_agr",     "file" : { "fmt" : "raw", "name" : mkstr(output/decay_leaf_agr.bin)}},
    { "id" : "decay_wood_nv",      "file" : { "fmt" : "raw", "name" : mkstr(output/decay_wood_nv.bin)}},
    { "id" : "decay_wood_agr",     "file" : { "fmt" : "raw", "name" : mkstr(output/decay_wood_agr.bin)}},
    { "id" : "response_layer_nv",  "file" : { "fmt" : "raw", "name" : mkstr(output/response_layer_nv.bin)}},
    { "id" : "response_layer_agr", "file" : { "fmt" : "raw", "name" : mkstr(output/response_layer_agr.bin)}},
    { "id" : "cshift_fast_nv",     "file" : { "fmt" : "raw", "name" : mkstr(output/cshift_fast_nv.bin)}},
    { "id" : "cshift_slow_nv",     "file" : { "fmt" : "raw", "name" : mkstr(output/cshift_slow_agr.bin)}}

/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : "all", /* 27410, 67208 60400 all grid cells */
  "endgrid" : ALL,

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 7000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
#ifdef CRU4
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
#else
  "firstyear": 1951, /* first year of simulation */
  "lastyear" : 1951, /* last year of simulation */
#endif
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
#ifdef CRU4
  "write_restart_filename" : mkstr(output/restart_1901_nv_stdfire.lpj), /* filename of restart file */
#else
  "write_restart_filename" : mkstr(output/../restart_1951_nv_stdfire_norr.lpj), /* filename of restart file */
#endif
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
#ifdef CRU4
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2018, /* last year of simulation */
#else
  "firstyear": 1951, /* first year of simulation */
  "lastyear" : 2099, /* last year of simulation */
#endif
  "outputyear": 1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
#ifdef CRU4
#ifdef NOCO2
  "restart_filename" : mkstr(output/../restart_1901_nv_stdfire.lpj), /* filename of restart file */
#else
  "restart_filename" : mkstr(output/restart_1901_nv_stdfire.lpj), /* filename of restart file */
#endif
#else
#ifdef NOCO2
  "restart_filename" : mkstr(output/../../restart_1951_nv_stdfire_norr.lpj), /* filename of restart file */
#else
  "restart_filename" : mkstr(output/../restart_1951_nv_stdfire_norr.lpj), /* filename of restart file */
#endif
#endif
  "write_restart" : true, /* create restart file */
#ifdef CRU4
  "write_restart_filename" : mkstr(output/restart_1900_crop_stdfire_norr.lpj), /* filename of restart file */
  "restart_year": 1900 /* write restart at year */
#else
  "write_restart_filename" : mkstr(output/restart_1950_crop_stdfire_norr.lpj), /* filename of restart file */
  "restart_year": 1950 /* write restart at year */
#endif

#endif
}
