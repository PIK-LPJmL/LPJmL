/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j   .  j  s                                            \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.1.001 without land use                \n**/
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
"version"  : "5.1",       /* LPJmL version expected */
"random_prec" : true,     /* Random weather generator for precipitation enabled */
"random_seed" : 2,        /* seed for random number generator */
"radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
"fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE */
#ifdef FROM_RESTART
"population" : false,      /* used by SPITFIRE model */
#else
"population" : false,
#endif
"prescribe_burntarea" : false,
"prescribe_landcover" : "no_landcover", 
"new_phenology": true,
"river_routing" : true,
"equilsoil" :false,
"permafrost" : true,
"with_nitrogen" : "lim_nitrogen", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
"const_climate" : false,
"const_deposition" : false,

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

#ifdef FROM_RESTART

"output" : [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
{ "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
{ "id" : "fpc",              "file" : { "fmt" : "raw", "name" : "output/fpc.bin"}},
{ "id" : "mnpp",             "file" : { "fmt" : "raw", "name" : "output/mnpp.bin"}},
{ "id" : "mgpp",             "file" : { "fmt" : "raw", "name" : "output/mgpp.bin"}},
{ "id" : "mrh",              "file" : { "fmt" : "raw", "name" : "output/mrh.bin"}},
{ "id" : "mfapar",           "file" : { "fmt" : "raw", "name" : "output/mfapar.bin"}},
{ "id" : "mtransp",          "file" : { "fmt" : "raw", "name" : "output/mtransp.bin"}},
{ "id" : "mrunoff",          "file" : { "fmt" : "raw", "name" : "output/mrunoff.bin"}},
{ "id" : "mevap",            "file" : { "fmt" : "raw", "name" : "output/mevap.bin"}},
{ "id" : "minterc",          "file" : { "fmt" : "raw", "name" : "output/minterc.bin"}},
{ "id" : "mswc1",            "file" : { "fmt" : "raw", "name" : "output/mswc1.bin"}},
{ "id" : "mswc2",            "file" : { "fmt" : "raw", "name" : "output/mswc2.bin"}},
{ "id" : "firec",            "file" : { "fmt" : "raw", "name" : "output/firec.bin"}},
{ "id" : "firef",            "file" : { "fmt" : "raw", "name" : "output/firef.bin"}},
{ "id" : "vegc",             "file" : { "fmt" : "raw", "name" : "output/vegc.bin"}},
{ "id" : "soilc",            "file" : { "fmt" : "raw", "name" : "output/soilc.bin"}},
{ "id" : "litc",             "file" : { "fmt" : "raw", "name" : "output/litc.bin"}},
{ "id" : "flux_estabc",      "file" : { "fmt" : "raw", "name" : "output/flux_estab.bin"}},
{ "id" : "pft_vegc",         "file" : { "fmt" : "raw", "name" : "output/pft_vegc.bin"}},
{ "id" : "mphen_tmin",       "file" : { "fmt" : "raw", "name" : "output/mphen_tmin.bin"}},
{ "id" : "mphen_tmax",       "file" : { "fmt" : "raw", "name" : "output/mphen_tmax.bin"}},
{ "id" : "mphen_light",      "file" : { "fmt" : "raw", "name" : "output/mphen_light.bin"}},
{ "id" : "mphen_water",      "file" : { "fmt" : "raw", "name" : "output/mphen_water.bin"}},
{ "id" : "vegn",             "file" : { "fmt" : "raw", "name" : "output/vegn.bin"}},
{ "id" : "soiln",            "file" : { "fmt" : "raw", "name" : "output/soiln.bin"}},
{ "id" : "litn",             "file" : { "fmt" : "raw", "name" : "output/litn.bin"}},
{ "id" : "soiln_layer",      "file" : { "fmt" : "raw", "name" : "output/soiln_layer.bin"}},
{ "id" : "soilno3_layer",    "file" : { "fmt" : "raw", "name" : "output/soilno3_layer.bin"}},
{ "id" : "soilnh4_layer",    "file" : { "fmt" : "raw", "name" : "output/soilnh4_layer.bin"}},
{ "id" : "soiln_slow",       "file" : { "fmt" : "raw", "name" : "output/soiln_slow.bin"}},
{ "id" : "soilnh4",          "file" : { "fmt" : "raw", "name" : "output/soilnh4.bin"}},
{ "id" : "soilno3",          "file" : { "fmt" : "raw", "name" : "output/soilno3.bin"}},
{ "id" : "pft_nuptake",      "file" : { "fmt" : "raw", "name" : "output/pft_nuptake.bin"}},
{ "id" : "mnuptake",         "file" : { "fmt" : "raw", "name" : "output/mnuptake.bin"}},
{ "id" : "mleaching",        "file" : { "fmt" : "raw", "name" : "output/mleaching.bin"}},
{ "id" : "mn2o_denit",       "file" : { "fmt" : "raw", "name" : "output/mn2o_denit.bin"}},
{ "id" : "mn2o_nit",         "file" : { "fmt" : "raw", "name" : "output/mn2o_nit.bin"}},
{ "id" : "mn2_emis",         "file" : { "fmt" : "raw", "name" : "output/mn2_emis.bin"}},
{ "id" : "mbnf",             "file" : { "fmt" : "raw", "name" : "output/mbnf.bin"}},
{ "id" : "mn_immo",          "file" : { "fmt" : "raw", "name" : "output/mn_immo.bin"}},
{ "id" : "pft_ndemand",      "file" : { "fmt" : "raw", "name" : "output/pft_ndemand.bin"}},
{ "id" : "firen",            "file" : { "fmt" : "raw", "name" : "output/firen.bin"}},
{ "id" : "mn_mineralization","file" : { "fmt" : "raw", "name" : "output/mn_mineralization.bin"}},
{ "id" : "mn_volatilization","file" : { "fmt" : "raw", "name" : "output/mn_volatilization.bin"}},
{ "id" : "pft_nlimit",       "file" : { "fmt" : "raw", "name" : "output/pft_nlimit.bin"}},
{ "id" : "pft_vegn",         "file" : { "fmt" : "raw", "name" : "output/pft_vegn.bin"}},
{ "id" : "pft_cleaf",        "file" : { "fmt" : "raw", "name" : "output/pft_cleaf.bin"}},
{ "id" : "pft_nleaf",        "file" : { "fmt" : "raw", "name" : "output/pft_nleaf.bin"}},
{ "id" : "pft_laimax",       "file" : { "fmt" : "raw", "name" : "output/pft_laimax.bin"}},
{ "id" : "pft_croot",        "file" : { "fmt" : "raw", "name" : "output/pft_croot.bin"}},
{ "id" : "pft_nroot",        "file" : { "fmt" : "raw", "name" : "output/pft_nroot.bin"}},
{ "id" : "pft_csapw",        "file" : { "fmt" : "raw", "name" : "output/pft_csapw.bin"}},
{ "id" : "pft_nsapw",        "file" : { "fmt" : "raw", "name" : "output/pft_nsapw.bin"}},
{ "id" : "pft_chawo",        "file" : { "fmt" : "raw", "name" : "output/pft_chawo.bin"}},
{ "id" : "pft_nhawo",        "file" : { "fmt" : "raw", "name" : "output/pft_nhawo.bin"}},
#ifdef WITH_SPITFIRE
{ "id" : "mfirec",           "file" : { "fmt" : "raw", "name" : "output/mfirec.bin"}},
{ "id" : "mnfire",           "file" : { "fmt" : "raw", "name" : "output/mnfire.bin"}},
{ "id" : "mburntarea",       "file" : { "fmt" : "raw", "name" : "output/mburnt_area.bin"}},
#endif
{ "id" : "mdischarge",       "file" : { "fmt" : "raw", "name" : "output/mdischarge.bin"}},
{ "id" : "mwateramount",     "file" : { "fmt" : "raw", "name" : "output/mwateramount.bin"}},
#ifdef DAILY_OUTPUT
{ "id" : "d_npp",            "file" : { "fmt" : "raw", "name" : "output/d_npp.bin"}},
{ "id" : "d_gpp",            "file" : { "fmt" : "raw", "name" : "output/d_gpp.bin"}},
{ "id" : "d_rh",             "file" : { "fmt" : "raw", "name" : "output/d_rh.bin"}},
{ "id" : "d_trans",          "file" : { "fmt" : "raw", "name" : "output/d_trans.bin"}},
{ "id" : "d_interc",         "file" : { "fmt" : "raw", "name" : "output/d_interc.bin"}},
{ "id" : "d_evap",           "file" : { "fmt" : "raw", "name" : "output/d_evap.bin"}},
#endif
{ "id" : "mpet",             "file" : { "fmt" : "raw", "name" : "output/mpet.bin"}},
{ "id" : "malbedo",          "file" : { "fmt" : "raw", "name" : "output/malbedo.bin"}},
{ "id" : "maxthaw_depth",    "file" : { "fmt" : "raw", "name" : "output/maxthaw_depth.bin"}},
{ "id" : "msoiltemp1",       "file" : { "fmt" : "raw", "name" : "output/msoiltemp1.bin"}},
{ "id" : "msoiltemp2",       "file" : { "fmt" : "raw", "name" : "output/msoiltemp2.bin"}},
{ "id" : "msoiltemp3",       "file" : { "fmt" : "raw", "name" : "output/msoiltemp3.bin"}},
{ "id" : "soilc_layer",      "file" : { "fmt" : "raw", "name" : "output/soilc_layer.bin"}},
{ "id" : "agb",              "file" : { "fmt" : "raw", "name" : "output/agb.bin"}},
{ "id" : "mreturn_flow_b",   "file" : { "fmt" : "raw", "name" : "output/mreturn_flow_b.bin"}},
{ "id" : "mtransp_b",        "file" : { "fmt" : "raw", "name" : "output/mtransp_b.bin"}},
{ "id" : "mevap_b",          "file" : { "fmt" : "raw", "name" : "output/mevap_b.bin"}},
{ "id" : "minterc_b",        "file" : { "fmt" : "raw", "name" : "output/mintec_b.bin"}},
{ "id" : "aconv_loss_evap",  "file" : { "fmt" : "raw", "name" : "output/aconv_loss_evap.bin"}},
{ "id" : "aconv_loss_drain", "file" : { "fmt" : "raw", "name" : "output/aconv_loss_drain.bin"}}
/*------------------------ ---------------------- ------------------------------- */
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

"nspinup" : 5000,  /* spinup years */
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
