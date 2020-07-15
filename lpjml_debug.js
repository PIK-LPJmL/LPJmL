/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.2.001                         \n**/
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

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : LPJML,       /* LPJML Simulation type with managed land use */
  "version"  : "5.2",       /* LPJmL version expected */
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : RADIATION,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : FIRE,            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "river_routing" : false,
  "permafrost" : true,
  "with_nitrogen" : LIM_NITROGEN, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "const_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "population" : false,
  "landuse" : CONST_LANDUSE, /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS, ONLY_CROPS (crops scaled to 100% of cell) */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : false,
  "wateruse" : NO_WATERUSE,  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
#else
  "population" : false,
  "landuse" : NO_LANDUSE,
  "reservoir" : false,
  "wateruse" : NO_WATERUSE,
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : NO_LANDCOVER, /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : PRESCRIBED_SDATE,   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "crop_phu_option" : PRESCRIBED_CROP_PHU,    /* PRESCRIBED_CROP_PHU (PHU dataset used, requires PRESCRIBED_SDATE), SEMISTATIC_CROP_PHU (LPJmL4 semi-static PHU approach) */
  "sdate_fixyear" : 2000,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : NO_RESIDUE_REMOVE, /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : POT_IRRIGATION,        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "tillage_type" : NO_TILLAGE,          /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "no_ndeposition" : true,             /* turn off atmospheric N deposition */
  "laimax_interpolate" : LAIMAX_PAR,    /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "manure_input" : true,               /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : false,             /* move PFT type others into PFT crop, maize for tropical, wheat for temperate */
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "par/lpjparam.js"      /* LPJ parameter file */
#include "par/soil.js"          /* Soil parameter file */
#include "par/pft_phase3.js"    /* PFT parameter file*/
#include "par/manage_laimax_alphaa_fao_rev4453_20180507.js" /* Management parameter file */
#include "par/manage_reg.js"    /* Management parameter file for regions*/
#include "par/outputvars.js"

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input/input_38band_wfdei.js"

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef WITH_GRIDBASED
  "pft_output_scaled" : GRIDBASED,
#define SUFFIX grid.bin
#else
  "pft_output_scaled" : PFTBASED,
#define SUFFIX pft.bin
#endif

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "crop_index" : MAIZE,  /* CFT for daily output */
  "crop_irrigation" : DAILY_RAINFED, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" : 
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : GRID,             "file" : { "fmt" : RAW, "name" : "output/grid.bin" }},
    { "id" : CFT_MSWC,     "file" : { "fmt" : RAW, "name" : "output/cft_mrootmoist.bin"}},
    { "id" : MROOTMOIST,     "file" : { "fmt" : RAW, "name" : "output/mroormoist_1m.bin"}}
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : 27410, /* 27410, 67208 60400 all grid cells */
  "endgrid" : 27410,

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 30,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : false, /* create restart file: the last year of simulation=restart-year */

#else

  "nspinup" : 2,   /* spinup years */
  "nspinyear" : 2,  /* cycle length during spinup (yr)*/
  "firstyear": 1979, /* first year of simulation */
  "lastyear" : 2012, /* last year of simulation */
  "outputyear": 1979, /* first year output is written  */
  "restart" :  false, /* start from restart file */
 // "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : false, /* create restart file */
 // "write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
 // "restart_year": 1900 /* write restart at year */

#endif
}
