/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  v  p  d  .  j  s                           \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.3.001                                 \n**/
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

  "sim_name" : "LPJmL run with SPITFIRE and VPD fire danger index", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "version"  : "5.3",       /* LPJmL version expected */
  "random_prec" : false,    /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire_on_grassland" : true, /* enable fire on grassland for Spitfire */
  "fire" : "spitfire_tmax", /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fdi" : "wvpd",           /* different fire danger index formulations: WVPD_INDEX, NESTEROV_INDEX*/
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : false,
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "with_nitrogen" : "no", /* other options: "no", "lim", "unlim" */
  "store_climate" : true, /* store climate data in spin-up phase */
  "const_climate" : false,
  "shuffle_climate" : false,
  "fix_climate" : false,
  "fix_landuse" : false,
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "equilsoil" : false,
  "population" : true,
  "landuse" : "yes",   /* other options: "no", "yes", "const", "all_crops" */
  "landuse_year_const" : 2000, /* set landuse year for "const" or "all_crops" case */
  "reservoir" : true,
  "wateruse" : "yes",  /* other options: "no", "yes", "all" */
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
  "sdate_fixyear" : 1970,                 /* year in which sowing dates shall be fixed */
  "tillage_type" : "all",                 /* Options: "all" (all agr. cells tilled), "no" (no cells tilled) and "read" (tillage dataset used) */
  "till_startyear" : 1850,                /* year in which tillage should start */
  "black_fallow" : false,                 /* simulation with black fallow on PNV */
  "no_ndeposition" : false,               /* turn off atmospheric N deposition */
  "intercrop" : true,                     /* intercrops on setaside */
  "residue_treatment" : "fixed_residue_remove", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */
  "residues_fire" : false,                /* fire in residuals */
  "irrigation" : "lim",                   /* other options: "no", "lim", "pot", "all" */
  "laimax_interpolate" : "laimax_cft",    /* laimax values from manage parameter file, */
                                          /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE */
  "rw_manage" : false,                    /* rain water management */
  "laimax" : 5,                           /* maximum LAI for CONST_LAI_MAX */
  "grassonly" : false,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "prescribe_lsuha" : false,
  "others_to_crop" : false,
  "grazing" : "default",                  /* default grazing type, other options : "default", "mowing", "ext", "int", "livestock", "none" */
  "istimber" : true,
  "crop_phu_option" : "new",
  "cropsheatfrost" : false,
  "double_harvest" : false,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_vpd.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_GLDAS.js"    /* Input files of CRU dataset */

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
  "crop_irrigation" : false, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" :
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "cdf", "name" : "output/grid.nc" }},
    { "id" : "fpc",              "file" : { "fmt" : "cdf", "name" : "output/fpc.nc"}},
    { "id" : "npp",              "file" : { "fmt" : "cdf", "name" : "output/mnpp.nc"}},
    { "id" : "gpp",              "file" : { "fmt" : "cdf", "name" : "output/mgpp.nc"}},
    { "id" : "rh",               "file" : { "fmt" : "cdf", "name" : "output/mrh.nc"}},
    { "id" : "fapar",            "file" : { "fmt" : "cdf", "name" : "output/mfapar.nc"}},
    { "id" : "transp",           "file" : { "fmt" : "cdf", "name" : "output/mtransp.nc"}},
    { "id" : "runoff",           "file" : { "fmt" : "cdf", "name" : "output/mrunoff.nc"}},
    { "id" : "evap",             "file" : { "fmt" : "cdf", "name" : "output/mevap.nc"}},
    { "id" : "interc",           "file" : { "fmt" : "cdf", "name" : "output/minterc.nc"}},
    { "id" : "swc1",             "file" : { "fmt" : "cdf", "name" : "output/mswc1.nc"}},
    { "id" : "swc2",             "file" : { "fmt" : "cdf", "name" : "output/mswc2.nc"}},
    { "id" : "firec",            "file" : { "fmt" : "cdf", "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec.nc"}},
    { "id" : "firef",            "file" : { "fmt" : "cdf", "name" : "output/firef.nc"}},
    { "id" : "vegc",             "file" : { "fmt" : "cdf", "name" : "output/vegc.nc"}},
    { "id" : "soilc",            "file" : { "fmt" : "cdf", "name" : "output/soilc.nc"}},
    { "id" : "litc",             "file" : { "fmt" : "cdf", "name" : "output/litc.nc"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "cdf", "name" : "output/flux_estab.nc"}},
    { "id" : "phen_tmin",        "file" : { "fmt" : "cdf", "name" : "output/mphen_tmin.nc"}},
    { "id" : "phen_tmax",        "file" : { "fmt" : "cdf", "name" : "output/mphen_tmax.nc"}},
    { "id" : "phen_light",       "file" : { "fmt" : "cdf", "name" : "output/mphen_light.nc"}},
    { "id" : "phen_water",       "file" : { "fmt" : "cdf", "name" : "output/mphen_water.nc"}},
    { "id" : "nfire",            "file" : { "fmt" : "cdf", "name" : "output/mnfire.nc"}},
    { "id" : "burntarea",        "file" : { "fmt" : "cdf", "name" : "output/mburnt_area.nc"}},
    { "id" : "firedi",           "file" : { "fmt" : "cdf", "name" : "output/mfiredi.nc"}},
    { "id" : "discharge",        "file" : { "fmt" : "cdf", "name" : "output/mdischarge.nc"}},
    { "id" : "wateramount",      "file" : { "fmt" : "cdf", "name" : "output/mwateramount.nc"}},
    { "id" : "harvestc",         "file" : { "fmt" : "cdf", "name" : "output/flux_harvest.nc"}},
    { "id" : "sdate",            "file" : { "fmt" : "cdf", "name" : "output/sdate.nc"}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "cdf", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "cdf", "name" : "output/cftfrac.nc"}},
    { "id" : "seasonality",      "file" : { "fmt" : "cdf", "name" : "output/seasonality.nc"}},
#ifdef DAILY_OUTPUT
    { "id" : "d_npp",            "file" : { "fmt" : "cdf", "name" : "output/d_npp.nc"}},
    { "id" : "d_gpp",            "file" : { "fmt" : "cdf", "name" : "output/d_gpp.nc"}},
    { "id" : "d_rh",             "file" : { "fmt" : "cdf", "name" : "output/d_rh.nc"}},
    { "id" : "d_trans",          "file" : { "fmt" : "cdf", "name" : "output/d_trans.nc"}},
    { "id" : "d_interc",         "file" : { "fmt" : "cdf", "name" : "output/d_interc.nc"}},
    { "id" : "d_evap",           "file" : { "fmt" : "cdf", "name" : "output/d_evap.nc"}},
#endif
    { "id" : "pet",              "file" : { "fmt" : "cdf", "name" : "output/mpet.nc"}},
    { "id" : "albedo",           "file" : { "fmt" : "cdf", "name" : "output/malbedo.nc"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "cdf", "name" : "output/maxthaw_depth.nc"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "cdf", "name" : "output/msoiltemp1.nc"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "cdf", "name" : "output/msoiltemp2.nc"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "cdf", "name" : "output/msoiltemp3.nc"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "cdf", "name" : "output/soilc_layer.nc"}},
    { "id" : "agb",              "file" : { "fmt" : "cdf", "name" : "output/agb.nc"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "cdf", "name" : "output/agb_tree.nc"}},
    { "id" : "return_flow_b",    "file" : { "fmt" : "cdf", "name" : "output/mreturn_flow_b.nc"}},
    { "id" : "transp_b",         "file" : { "fmt" : "cdf", "name" : "output/mtransp_b.nc"}},
    { "id" : "evap_b",           "file" : { "fmt" : "cdf", "name" : "output/mevap_b.nc"}},
    { "id" : "interc_b",         "file" : { "fmt" : "cdf", "name" : "output/mintec_b.nc"}},
    { "id" : "conv_loss_evap",   "file" : { "fmt" : "cdf", "name" : "output/aconv_loss_evap.nc"}},
    { "id" : "conv_loss_drain",  "file" : { "fmt" : "cdf", "name" : "output/aconv_loss_drain.nc"}}
/*------------------------ ---------------------- ------------------------------- */
  ],

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

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1948, /* first year of simulation */
  "lastyear" : 1948, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "restart_year": 1948 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1948, /* first year of simulation */
  "lastyear" : 2017, /* last year of simulation */
  "outputyear": 1948, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
  "restart_year": 1948 /* write restart at year */

#endif
}
