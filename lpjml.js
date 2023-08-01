/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.4.003                         \n**/
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

#define DAILY_OUTPUT  /* enables daily output */

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "coupled_model" : null,   /* no model coupling */
  "version"  : "5.4",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fire_on_grassland" : false, /* enable fire on grassland for Spitfire */
  "fdi" : "nesterov",       /* different fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
  "firewood" : false,
  "new_phenology" : true,   /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : false,
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
  "store_climate" : true, /* store climate data in spin-up phase */
  "const_climate" : false,
  "shuffle_climate" : true, /* shuffle spinup climate */
  "const_deposition" : false,
  "depos_year_const" : 1901,
  "fix_climate" : false,
  "fix_landuse" : false,
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "population" : false,
  "landuse" : "no", /* other options: "no", "yes", "const", "all_crops", "only_crops" */
  "landuse_year_const" : 2000, /* set landuse year for "const" case */
  "reservoir" : true,
  "wateruse" : "no",  /* other options: "no", "yes", "all" */
  "equilsoil" : false,
#else
  "equilsoil" : true,
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "fixed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : false,                  /* intercrops on setaside */
  "residue_treatment" : "fixed_residue_remove", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim",                 /* other options: "no", "lim", "pot", "all" */
  "laimax_interpolate" : "laimax_par",  /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE */
  "tillage_type" : "all",               /* Options: "all" (all agr. cells tilled), "no" (no cells tilled) and "read" (tillage dataset used) */
  "till_startyear" : 1850,              /* year in which tillage should start */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "pft_residue" : "temperate cereals",
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : "yes",           /* enable fertilizer input, other options: "no", "yes", "auto" */
  "manure_input" : true,                /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : true,              /* move PFT type others into PFT crop, cft_tropic for tropical,  cft_temp for temperate */
  "grazing" : "default",                /* default grazing type, other options : "default", "mowing", "ext", "int", "none" */
  "grazing_others" : "default",         /* default grazing type for others, other options : "default", "mowing", "ext", "int", "none" */
  "cft_temp" : "temperate cereals",
  "cft_tropic" : "maize",
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "mowing_days" : [152, 335],          /* Mowing days for grassland if grass harvest options are ser */
  "crop_resp_fix" : false,             /* variable C:N ratio for crop respiration */
                                       /* for MAgPIE runs, turn off dynamic C:N ratio dependent respiration,
                                          which reduces yields at high N inputs */
  "crop_phu_option" : "new",
  "cropsheatfrost" : false,
  "double_harvest" : true,
  "ma_bnf" : true,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_crumonthly.js"    /* Input files of CRU dataset */

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef WITH_GRIDBASED
  "grid_scaled" : true, /* PFT-specific outputs scaled by stand->frac */
#define SUFFIX grid.bin
#else
  "grid_scaled" : false,
#define SUFFIX pft.bin
#endif

  "output_metafile" : false, /* no json metafile created */
  "float_grid" : false,      /* set datatype of grid file to float (TRUE/FALSE) */

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "crop_index" : "temperate cereals", /* CFT for daily output */
  "crop_irrigation" : false,          /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" :
  [

/*
ID                               Fmt                        filename
-------------------------------- ------------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "txt", "name" : "output/grid.txt" }},
    { "id" : "temp",             "file" : { "fmt" : "txt", "name" : "output/temp.txt"}},
    
#ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "fmt" : "raw", "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec.bin"}},
    { "id" : "nfire",            "file" : { "fmt" : "raw", "name" : "output/mnfire.bin"}},
    { "id" : "burntarea",        "file" : { "fmt" : "raw", "name" : "output/mburnt_area.bin"}},
#else
    { "id" : "firec",            "file" : { "fmt" : "raw", "name" : "output/firec.bin"}},
#endif
    { "id" : "soiltemp1",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp1.txt"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp2.txt"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp3.txt"}},
    { "id" : "soiltemp4",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp4.txt"}},
    { "id" : "soiltemp5",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp5.txt"}},
    { "id" : "soiltemp6",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp6.txt"}},
    { "id" : "swc1",        "file" : { "fmt" : "txt", "name" : "output/water1.txt"}},
    { "id" : "swc2",        "file" : { "fmt" : "txt", "name" : "output/water2.txt"}},
    { "id" : "swc3",        "file" : { "fmt" : "txt", "name" : "output/water3.txt"}},
    { "id" : "swc4",        "file" : { "fmt" : "txt", "name" : "output/water4.txt"}},
    { "id" : "swc5",        "file" : { "fmt" : "txt", "name" : "output/water5.txt"}},
    { "id" : "rain",        "file" : { "fmt" : "txt", "name" : "output/rain.txt"}}

/*------------------------------ ------------------------- ------------------------------- */
  ],

#else
  "output" :
  [

  /*
  ID                               Fmt                        filename
  -------------------------------- ------------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "txt", "name" : "output/grid.txt" }},
    { "id" : "temp",             "file" : { "fmt" : "txt", "name" : "output/temp.txt"}},
    
  #ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "fmt" : "raw", "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec.bin"}},
    { "id" : "nfire",            "file" : { "fmt" : "raw", "name" : "output/mnfire.bin"}},
    { "id" : "burntarea",        "file" : { "fmt" : "raw", "name" : "output/mburnt_area.bin"}},
  #else
    { "id" : "firec",            "file" : { "fmt" : "raw", "name" : "output/firec.bin"}},
  #endif
    { "id" : "soiltemp1",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp1.txt"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp2.txt"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp3.txt"}},
    { "id" : "soiltemp4",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp4.txt"}},
    { "id" : "soiltemp5",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp5.txt"}},
    { "id" : "soiltemp6",        "file" : { "fmt" : "txt", "name" : "output/msoiltemp6.txt"}},
    { "id" : "swc1",        "file" : { "fmt" : "txt", "name" : "output/water1.txt"}},
    { "id" : "swc2",        "file" : { "fmt" : "txt", "name" : "output/water2.txt"}},
    { "id" : "swc3",        "file" : { "fmt" : "txt", "name" : "output/water3.txt"}},
    { "id" : "swc4",        "file" : { "fmt" : "txt", "name" : "output/water4.txt"}},
    { "id" : "swc5",        "file" : { "fmt" : "txt", "name" : "output/water5.txt"}},
    { "id" : "swe",        "file" : { "fmt" : "txt", "name" : "output/snow.txt"}},
    { "id" : "rain",        "file" : { "fmt" : "txt", "name" : "output/rain.txt"}}
  /*------------------------------ ------------------------- ------------------------------- */
  ],
#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : 47284, /* 27410, 67208 60400 47284 47293 47277 all grid cells */
  "endgrid"   : 47284,
#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 4,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1940, /* first year of simulation */
  "lastyear" : 1946, /* last year of simulation */
  "restart" :  false, /* start from restart file */
  "outputyear" : 1940,
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "restart_year": 1946 /* write restart at year */

#else

  "nspinup" : 0,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1940, /* first year of simulation */
  "lastyear" : 1946, /* last year of simulation */
  "outputyear": 1940, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
