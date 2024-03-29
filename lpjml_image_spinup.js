/**************************************************************************************/
/**                                                                                \n**/
/**          l  p  j  m  l  _  i  m  a  g  e  _  s  p  i  n  u  p  .  j  s         \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.3.001 spinup runs for IMAGE coupling  \n**/
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

#define IMAGE

#include "include/conf.h" /* include constant definitions */

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "coupled_model" : null,   /* no model coupling */
  "version"  : "5.3",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : "random_seed", /* seed for random number generator */
  "radiation" : "cloudiness",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fdi" : "nesterov",       /* different fire danger index formulations: WVPD_INDEX(needs GLDAS input data), NESTEROV_INDEX*/
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : true,
  "permafrost" : true,
  "with_nitrogen" : "no", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "store_climate" : true, /* store climate data in spin-up phase */
  "shuffle_climate" : false,
  "const_climate" : false,
  "fix_climate" : false,
  "fix_landuse" : false,
  "extflow" : false,
  "johansen" : true,
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "equilsoil" : false,
  "population" : false,
  "landuse" : "yes", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : false,
  "wateruse" : "no",  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
#else
  "equilsoil" : true,
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "no_fixed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : "no_residue_remove",
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim",                 /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "laimax_interpolate" : "laimax_cft",  /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "tillage_type" : "all",
  "till_startyear" : 1850,
  "black_fallow" : false,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "prescribe_lsuha" : false,
  "mowing_days" : [152, 335],          /* Mowing days for grassland if grass harvest options are ser */
  "groundwater_irrigation": false,
  "aquifer_irrigation": false,
  "others_to_crop" : false,
  "grazing" : "default",                /* default grazing type, other options : "default", "mowing", "ext", "int", "livestock", "none" */
  "istimber" : false,
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "crop_phu_option" : "new",
  "cropsheatfrost" : false,
  "double_harvest" : false,


/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_wp.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_image.js"    /* Input files for IMAGE coupling */

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef WITH_GRIDBASED
  "grid_scaled" : true,
#define SUFFIX grid.bin
#else
  "grid_scaled" : false,
#define SUFFIX pft.bin
#endif

  "float_grid" : false, /* set datatype of grid file to float (TRUE/FALSE) */

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "crop_index" : "temperate cereals",  /* CFT for daily output */
  "crop_irrigation" : true, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" :
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : "grid",       "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
    { "id" : "pft_harvestc","file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",    "file" : { "fmt" : "raw", "name" : "output/cftfrac.bin"}},
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

  "nspinup" : 1000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_image_lpj_pnv.lpj", /* filename of restart file */
  "restart_year": 1901 /* write restart at year */

#else

  "nspinup" : 300,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1969, /* last year of simulation */
  "outputyear": 1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_image_lpj_pnv.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_image_lpj_1969_crop.lpj", /* filename of restart file */
  "restart_year": 1969 /* write restart at year */

#endif
}
