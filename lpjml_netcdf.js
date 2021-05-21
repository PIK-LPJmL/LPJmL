/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  n  e  t  c  d  f  .  j  s                  \n**/
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

// #define DAILY_OUTPUT

{

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

"sim_name" : "LPJmL Run", /* Simulation description */
"sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
"version"  : "5.3",       /* LPJmL version expected */
"random_prec" : true,     /* Random weather generator for precipitation enabled */
"random_seed" : 2,        /* seed for random number generator */
"radiation" : "cloudiness",
"fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
"fdi" : "nesterov_index",   /* different fire danger index formulations: WVPD_INDEX(needs GLDAS input data), NESTEROV_INDEX*/
"shuffle_climate" : false,
"firewood" : false,
#ifdef FROM_RESTART
"new_seed" : false,
"population" : false,
"landuse" : "yes",
"equilsoil" : false,
#else
"equilsoil" : true,
"population" : false,
"landuse" : "no",
#endif
"prescribe_burntarea" : false,
"prescribe_landcover" : "no_landcover",
"new_phenology": false,
"new_trf" : false,        /* new transpiration reduction function disabled */
"river_routing": false,
"permafrost" : true,
"with_nitrogen": "lim",
"store_climate" : true, /* store climate data in spin-up phase */
"const_climate" : false,
"fix_climate" : false,
"const_deposition" : false,
"fertilizer_input" : "yes",
"fix_fertilization" : false,          /* fix fertilizer input */
"check_climate" : true,               /* check climate input before start */
"irrigation" : "pot",
"sowing_date_option" : "fixed_sdate",
"sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
"residue_treatment" : "fixed_residue_remove", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */
"residues_fire" : false,              /* fire in residuals */
"laimax_interpolate" : "laimax_par",  /* laimax values from manage parameter file */
"laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
"tillage_type" : "all",           /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
"till_startyear" : 1850,              /* year in which tillage should start */
"black_fallow" : false,               /* simulation with black fallow on PNV */
"no_ndeposition" : false,             /* turn off atmospheric N deposition */
"reservoir" : false,
"grassland_fixed_pft" : false,
"wateruse" : "no",
"grass_harvest_options" : false,
"intercrop" : true,                   /* intercrops on setaside */
"landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
"residues_fire" : false,              /* fire in residuals */
"istimber": false,
"rw_manage" : false,                  /* rain water management */
"manure_input" : false,               /* enable manure input */
"others_to_crop" : true,
"cft_temp" : "temperate cereals",
"cft_tropic" : "maize",
"grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
"crop_resp_fix" : false,
"crop_phu_option" : "new",
"cropsheatfrost" : false,
"double_harvest" : true,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_netcdf.js"    /* Input files of CRU dataset */

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef WITH_GRIDBASED
"grid_scaled" : true,
#define SUFFIX "grid.nc"
#else
"grid_scaled" : false,
#define SUFFIX "pft.nc"
#endif

"float_grid" : false, /* set datatype of grid file to float (TRUE/FALSE) */

#ifdef FROM_RESTART

"global_netcdf" : false,  /* global NetCDF grid (TRUE/FALSE) */
"float_grid" : false, /* set datatype of grid file to float (TRUE/FALSE) */

"output" : [

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
{ "id" : "firef",            "file" : { "fmt" : "cdf", "name" : "output/firef.nc"}},
{ "id" : "vegc",             "file" : { "fmt" : "cdf", "name" : "output/vegc.nc"}},
{ "id" : "soilc",            "file" : { "fmt" : "cdf", "name" : "output/soilc.nc"}},
{ "id" : "litc",             "file" : { "fmt" : "cdf", "name" : "output/litc.nc"}},
{ "id" : "flux_estabc",       "file" : { "fmt" : "cdf", "name" : "output/flux_estabc.nc"}},
{ "id" : "phen_tmin",        "file" : { "fmt" : "cdf", "name" : "output/mphen_tmin.nc"}},
{ "id" : "phen_tmax",        "file" : { "fmt" : "cdf", "name" : "output/mphen_tmax.nc"}},
{ "id" : "phen_light",       "file" : { "fmt" : "cdf", "name" : "output/mphen_light.nc"}},
{ "id" : "phen_water",       "file" : { "fmt" : "cdf", "name" : "output/mphen_water.nc"}},
#ifdef WITH_SPITFIRE
{ "id" : "firec",            "file" : { "fmt" : "cdf", "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec.nc"}},
{ "id" : "nfire",            "file" : { "fmt" : "cdf", "name" : "output/mnfire.nc"}},
{ "id" : "burntarea",        "file" : { "fmt" : "cdf", "name" : "output/mburnt_area.nc"}},
#else
{ "id" : "firec",            "file" : { "fmt" : "cdf", "name" : "output/firec.nc"}},
#endif
#ifdef RIVER_ROUTING
{ "id" : "discharge",        "file" : { "fmt" : "cdf", "name" : "output/mdischarge.nc"}},
{ "id" : "wateramount",      "file" : { "fmt" : "cdf", "name" : "output/mwateramount.nc"}},
#endif
#ifdef WITH_LANDUSE
{ "id" : "harvest",          "file" : { "fmt" : "cdf", "name" : "output/flux_harvest.nc"}},
{ "id" : "sdate",            "file" : { "fmt" : "cdf", "name" : "output/sdate.nc"}},
#ifdef WITH_GRIDBASED
{ "id" : "pft_harvest",      "file" : { "fmt" : "cdf", "name" : "output/pft_harvest.grid.nc"}},
#else
{ "id" : "pft_harvest",      "file" : { "fmt" : "cdf", "name" : "output/pft_harvest.pft.nc"}},
#endif
{ "id" : "cftfrac",          "file" : { "fmt" : "cdf", "name" : "output/cftfrac.nc"}},
{ "id" : "seasonality",      "file" : { "fmt" : "cdf", "name" : "output/seasonality.nc"}},
#endif
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
{ "id" : "soiltemp",         "file" : { "fmt" : "cdf", "name" : "output/msoiltemp.nc"}},
{ "id" : "soilc_layer",      "file" : { "fmt" : "cdf", "name" : "output/soilc_layer.nc"}},
{ "id" : "agb",              "file" : { "fmt" : "cdf", "name" : "output/agb.nc"}},
{ "id" : "return_flow_b",    "file" : { "fmt" : "cdf", "name" : "output/mreturn_flow_b.nc"}},
{ "id" : "transp_b",         "file" : { "fmt" : "cdf", "name" : "output/mtransp_b.nc"}},
{ "id" : "evap_b",           "file" : { "fmt" : "cdf", "name" : "output/mevap_b.nc"}},
{ "id" : "interc_b",         "file" : { "fmt" : "cdf", "name" : "output/mintec_b.nc"}},
{ "id" : "conv_loss_evap",   "file" : { "fmt" : "cdf", "name" : "output/aconv_loss_evap.nc"}},
{ "id" : "conv_loss_drain",  "file" : { "fmt" : "cdf", "name" : "output/aconv_loss_drain.nc"}}
/*------------------------ ---------------------- ------------------------------- */
],

"crop_index" : "temperate cereals",  /* CFT for daily output_SPITFIRE-optpar */
"crop_irrigation" : false, /* irrigation flag for daily output_SPITFIRE-optpar */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

"startgrid" : "all", /* 27410 67208 60400 all grid cells */

#ifdef CHECKPOINT
"checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

"nspinup" : 5000,  /* spinup years */
"nspinyear" : 30,  /* cycle length during spinup (yr) */
"firstyear": 1901, /* first year of simulation */
"lastyear" : 1901, /* last year of simulation */
"restart" : false, /* do not start from restart file */
"write_restart" : true, /* create restart file: the last year of simulation=restart-year */
"write_restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
"restart_year": 1840 /* write restart at year */

#else

"nspinup" : 390,  /* spinup years */
"nspinyear" : 30, /*cycle length during spinup (yr)*/
"firstyear": 1901, /* first year of simulation */
"lastyear" : 2009, /* last year of simulation */
"restart" :  true, /* start from restart file */
"restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
"write_restart" : true, /* create restart file */
"write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
"restart_year": 1900 /* write restart at year */

#endif
}
