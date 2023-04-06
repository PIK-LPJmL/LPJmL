/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j   .  j  s                                            \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.5.001 without land use                \n**/
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
{
#include "include/conf.h" /* include constant definitions */

/* #define DAILY_OUTPUT */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

"sim_name" : "LPJ run with PNV",   /* Simulation description */
"sim_id"   : "lpj",       /* LPJML Simulation with natural vegetation only */
"coupled_model" : null,   /* no model coupling */
"version"  : "5.5",       /* LPJmL version expected */
"random_prec" : true,     /* Random weather generator for precipitation enabled */
"random_seed" : 2,        /* seed for random number generator */
"radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
"fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE */
#ifdef FROM_RESTART
"equilsoil" : false,
"new_seed" : false,
"population" : false,      /* used by SPITFIRE model */
#else
"equilsoil" : true,
"population" : false,
#endif
"prescribe_burntarea" : false,
"prescribe_landcover" : "no_landcover",
"new_phenology": true,
"new_trf" : false,
"river_routing" : true,
"extflow" : false,
"permafrost" : true,
"johansen" : true,
"with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
"soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
"store_climate" : true,
"const_climate" : false,
"const_deposition" : false,
"shuffle_climate" : true, /* shuffle spinup climate */
"fix_climate" : false,
"no_ndeposition" : false,             /* turn off atmospheric N deposition */
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

"output_metafile" : true,   /* json metafile created (true/false) */
"default_fmt" : "raw",      /* default format for output files: "raw","txt","cdf","clm","sock" */
"default_suffix" : ".bin",  /* default file suffix for output files */
"grid_type" : "short",      /* set datatype of grid file ("short", "float", "double") */
"absyear" : false,          /* years relative to baseyear (true/false) */
"rev_lat" : false,          /* reverse latitudes in NetCDF output (true/false) */
"with_days" : true,         /* use days as units for monthly output in NetCDF files */

#ifdef FROM_RESTART

"output" : [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
{ "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
{ "id" : "fpc",              "file" : { "name" : "output/fpc"}},
{ "id" : "npp",              "file" : { "name" : "output/mnpp"}},
{ "id" : "gpp",              "file" : { "name" : "output/mgpp"}},
{ "id" : "rh",               "file" : { "name" : "output/mrh"}},
{ "id" : "fapar",            "file" : { "name" : "output/mfapar"}},
{ "id" : "transp",           "file" : { "name" : "output/mtransp"}},
{ "id" : "runoff",           "file" : { "name" : "output/mrunoff"}},
{ "id" : "evap",             "file" : { "name" : "output/mevap"}},
{ "id" : "interc",           "file" : { "name" : "output/minterc"}},
{ "id" : "swc1",             "file" : { "name" : "output/mswc1"}},
{ "id" : "swc2",             "file" : { "name" : "output/mswc2"}},
{ "id" : "firef",            "file" : { "name" : "output/firef"}},
{ "id" : "vegc",             "file" : { "name" : "output/vegc"}},
{ "id" : "soilc",            "file" : { "name" : "output/soilc"}},
{ "id" : "litc",             "file" : { "name" : "output/litc"}},
{ "id" : "flux_estabc",      "file" : { "name" : "output/flux_estab"}},
{ "id" : "pft_vegc",         "file" : { "name" : "output/pft_vegc"}},
{ "id" : "phen_tmin",        "file" : { "name" : "output/mphen_tmin"}},
{ "id" : "phen_tmax",        "file" : { "name" : "output/mphen_tmax"}},
{ "id" : "phen_light",       "file" : { "name" : "output/mphen_light"}},
{ "id" : "phen_water",       "file" : { "name" : "output/mphen_water"}},
{ "id" : "vegn",             "file" : { "name" : "output/vegn"}},
{ "id" : "soiln",            "file" : { "name" : "output/soiln"}},
{ "id" : "litn",             "file" : { "name" : "output/litn"}},
{ "id" : "soiln_layer",      "file" : { "name" : "output/soiln_layer"}},
{ "id" : "soilno3_layer",    "file" : { "name" : "output/soilno3_layer"}},
{ "id" : "soilnh4_layer",    "file" : { "name" : "output/soilnh4_layer"}},
{ "id" : "soiln_slow",       "file" : { "name" : "output/soiln_slow"}},
{ "id" : "soilnh4",          "file" : { "name" : "output/soilnh4"}},
{ "id" : "soilno3",          "file" : { "name" : "output/soilno3"}},
{ "id" : "pft_nuptake",      "file" : { "name" : "output/pft_nuptake"}},
{ "id" : "nuptake",          "file" : { "name" : "output/mnuptake"}},
{ "id" : "leaching",         "file" : { "name" : "output/mleaching"}},
{ "id" : "n2o_denit",        "file" : { "name" : "output/mn2o_denit"}},
{ "id" : "n2o_nit",          "file" : { "name" : "output/mn2o_nit"}},
{ "id" : "n2_emis",          "file" : { "name" : "output/mn2_emis"}},
{ "id" : "bnf",              "file" : { "name" : "output/mbnf"}},
{ "id" : "n_immo",           "file" : { "name" : "output/mn_immo"}},
{ "id" : "pft_ndemand",      "file" : { "name" : "output/pft_ndemand"}},
{ "id" : "firen",            "file" : { "name" : "output/firen"}},
{ "id" : "n_mineralization", "file" : { "name" : "output/mn_mineralization"}},
{ "id" : "n_volatilization", "file" : { "name" : "output/mn_volatilization"}},
{ "id" : "pft_nlimit",       "file" : { "name" : "output/pft_nlimit"}},
{ "id" : "pft_vegn",         "file" : { "name" : "output/pft_vegn"}},
{ "id" : "pft_cleaf",        "file" : { "name" : "output/pft_cleaf"}},
{ "id" : "pft_nleaf",        "file" : { "name" : "output/pft_nleaf"}},
{ "id" : "pft_laimax",       "file" : { "name" : "output/pft_laimax"}},
{ "id" : "pft_croot",        "file" : { "name" : "output/pft_croot"}},
{ "id" : "pft_nroot",        "file" : { "name" : "output/pft_nroot"}},
{ "id" : "pft_csapw",        "file" : { "name" : "output/pft_csapw"}},
{ "id" : "pft_nsapw",        "file" : { "name" : "output/pft_nsapw"}},
{ "id" : "pft_chawo",        "file" : { "name" : "output/pft_chawo"}},
{ "id" : "pft_nhawo",        "file" : { "name" : "output/pft_nhawo"}},
#ifdef WITH_SPITFIRE
{ "id" : "firec",            "file" : { "timestep" : "monthly", "unit" : "gC/m2/month", "name" : "output/mfirec"}},
{ "id" : "nfire",            "file" : { "name" : "output/mnfire"}},
{ "id" : "burntarea",        "file" : { "name" : "output/mburnt_area"}},
#else
{ "id" : "firec",            "file" : { "name" : "output/firec"}},
#endif
{ "id" : "discharge",        "file" : { "name" : "output/mdischarge"}},
{ "id" : "wateramount",      "file" : { "name" : "output/mwateramount"}},
#ifdef DAILY_OUTPUT
{ "id" : "d_npp",            "file" : { "name" : "output/d_npp"}},
{ "id" : "d_gpp",            "file" : { "name" : "output/d_gpp"}},
{ "id" : "d_rh",             "file" : { "name" : "output/d_rh"}},
{ "id" : "d_trans",          "file" : { "name" : "output/d_trans"}},
{ "id" : "d_interc",         "file" : { "name" : "output/d_interc"}},
{ "id" : "d_evap",           "file" : { "name" : "output/d_evap"}},
#endif
{ "id" : "pet",              "file" : { "name" : "output/mpet"}},
{ "id" : "albedo",           "file" : { "name" : "output/malbedo"}},
{ "id" : "maxthaw_depth",    "file" : { "name" : "output/maxthaw_depth"}},
{ "id" : "soiltemp1",        "file" : { "name" : "output/msoiltemp1"}},
{ "id" : "soiltemp2",        "file" : { "name" : "output/msoiltemp2"}},
{ "id" : "soiltemp3",        "file" : { "name" : "output/msoiltemp3"}},
{ "id" : "soilc_layer",      "file" : { "name" : "output/soilc_layer"}},
{ "id" : "agb",              "file" : { "name" : "output/agb"}},
{ "id" : "return_flow_b",    "file" : { "name" : "output/mreturn_flow_b"}},
{ "id" : "transp_b",         "file" : { "name" : "output/mtransp_b"}},
{ "id" : "evap_b",           "file" : { "name" : "output/mevap_b"}},
{ "id" : "interc_b",         "file" : { "name" : "output/mintec_b"}}
],

"crop_index" : "temperate cereals",  /* CFT for daily output */
"crop_irrigation" : false, /* irrigation flag for daily output */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

"startgrid" : "all", /* 27410 67208 60400 all grid cells */

#ifdef CHECKPOINT
"checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

"nspinup" : 8000,  /* spinup years */
"nspinyear" : 30,  /* cycle length during spinup (yr) */
"firstyear": 1901, /* first year of simulation */
"lastyear" : 1901, /* last year of simulation */
"restart" : false, /* do not start from restart file */
"write_restart" : true, /* create restart file: the last year of simulation=restart-year */
"write_restart_filename" : "restart/restart_1900_nv_stdfire.lpj", /* filename of restart file */
"restart_year": 1900 /* write restart at year */

#else

"nspinup" : 0,  /* spinup years */
"nspinyear" : 30, /* cycle length during spinup (yr)*/
"firstyear": 1901, /* first year of simulation */
"lastyear" : 2011, /* last year of simulation */
"restart" :  true, /* start from restart file */
"restart_filename" : "restart/restart_1900_nv_stdfire.lpj", /* filename of restart file */
"write_restart" : false /* do not create restart file */

#endif
}
