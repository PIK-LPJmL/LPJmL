/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  n  e  t  c  d  f  .  j  s                  \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 4.0.001                         \n**/
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
"sim_id"   : LPJML,       /* LPJML Simulation type with managed land use */
"version"  : "5.1",       /* LPJmL version expected */
"random_prec" : true, /* Random weather generator for precipitation enabled */
"random_seed" : 2, /* seed for random number generator */
"radiation" : CLOUDINESS,
"fire" : FIRE,                 /* fire disturbance enabled */
"firewood" : false,
#ifdef FROM_RESTART
"population" : false,
"landuse" : LANDUSE,
 "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
#else
"population" : false,
"landuse" : NO_LANDUSE,
#endif
"prescribe_burntarea" : false,
"prescribe_landcover" : NO_LANDCOVER,
"new_phenology": false,
"river_routing": false,
"permafrost" : true,
"with_nitrogen": NO_NITROGEN,
"irrigation" : POT_IRRIGATION,
"sowing_date_option" : FIXED_SDATE,
"sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
"laimax_interpolate" : LAIMAX_CFT, /* laimax values from manage parameter file */
"laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
"reservoir" : false,
"grassland_fixed_pft" : false,
"wateruse" : false,
"intercrop" : true,                   /* intercrops on setaside */
"remove_residuals" : false,           /* remove residuals */
"residues_fire" : false,              /* fire in residuals */
"istimber": false,
"rw_manage" : false,                  /* rain water management */

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
"pft_output_scaled" : GRIDBASED,
#define SUFFIX "grid.nc"
#else
"pft_output_scaled" : PFTBASED,
#define SUFFIX "pft.nc"
#endif

#ifdef FROM_RESTART

"global_netcdf" : false,  /* global NetCDF grid (TRUE/FALSE) */

"output" : [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
{ "id" : GRID,             "file" : { "fmt" : CDF, "name" : "output/grid.nc" }},
{ "id" : FPC,              "file" : { "fmt" : CDF, "name" : "output/fpc.nc"}},
{ "id" : MNPP,             "file" : { "fmt" : CDF, "name" : "output/mnpp.nc"}},
{ "id" : MGPP,             "file" : { "fmt" : CDF, "name" : "output/mgpp.nc"}},
{ "id" : MRH,              "file" : { "fmt" : CDF, "name" : "output/mrh.nc"}},
{ "id" : MFAPAR,           "file" : { "fmt" : CDF, "name" : "output/mfapar.nc"}},
{ "id" : MTRANSP,          "file" : { "fmt" : CDF, "name" : "output/mtransp.nc"}},
{ "id" : MRUNOFF,          "file" : { "fmt" : CDF, "name" : "output/mrunoff.nc"}},
{ "id" : MEVAP,            "file" : { "fmt" : CDF, "name" : "output/mevap.nc"}},
{ "id" : MINTERC,          "file" : { "fmt" : CDF, "name" : "output/minterc.nc"}},
{ "id" : MSWC1,            "file" : { "fmt" : CDF, "name" : "output/mswc1.nc"}},
{ "id" : MSWC2,            "file" : { "fmt" : CDF, "name" : "output/mswc2.nc"}},
{ "id" : FIREC,            "file" : { "fmt" : CDF, "name" : "output/firec.nc"}},
{ "id" : FIREF,            "file" : { "fmt" : CDF, "name" : "output/firef.nc"}},
{ "id" : VEGC,             "file" : { "fmt" : CDF, "name" : "output/vegc.nc"}},
{ "id" : SOILC,            "file" : { "fmt" : CDF, "name" : "output/soilc.nc"}},
{ "id" : LITC,             "file" : { "fmt" : CDF, "name" : "output/litc.nc"}},
{ "id" : FLUX_ESTAB,       "file" : { "fmt" : CDF, "name" : "output/flux_estab.nc"}},
#ifdef GSI_PHENOLOGY
{ "id" : MPHEN_TMIN,       "file" : { "fmt" : CDF, "name" : "output/mphen_tmin.nc"}},
{ "id" : MPHEN_TMAX,       "file" : { "fmt" : CDF, "name" : "output/mphen_tmax.nc"}},
{ "id" : MPHEN_LIGHT,      "file" : { "fmt" : CDF, "name" : "output/mphen_light.nc"}},
{ "id" : MPHEN_WATER,      "file" : { "fmt" : CDF, "name" : "output/mphen_water.nc"}},
#endif
#ifdef WITH_SPITFIRE
{ "id" : MFIREC,           "file" : { "fmt" : CDF, "name" : "output/mfirec.nc"}},
{ "id" : MNFIRE,           "file" : { "fmt" : CDF, "name" : "output/mnfire.nc"}},
{ "id" : MBURNTAREA,       "file" : { "fmt" : CDF, "name" : "output/mburnt_area.nc"}},
#endif
#ifdef RIVER_ROUTING
{ "id" : MDISCHARGE,       "file" : { "fmt" : CDF, "name" : "output/mdischarge.nc"}},
{ "id" : MWATERAMOUNT,     "file" : { "fmt" : CDF, "name" : "output/mwateramount.nc"}},
#endif
#ifdef WITH_LANDUSE
{ "id" : HARVEST,          "file" : { "fmt" : CDF, "name" : "output/flux_harvest.nc"}},
{ "id" : SDATE,            "file" : { "fmt" : CDF, "name" : "output/sdate.nc"}},
#ifdef WITH_GRIDBASED
{ "id" : PFT_HARVEST,      "file" : { "fmt" : CDF, "name" : "output/pft_harvest.grid.nc"}},
#else
{ "id" : PFT_HARVEST,      "file" : { "fmt" : CDF, "name" : "output/pft_harvest.pft.nc"}},
#endif
{ "id" : CFTFRAC,          "file" : { "fmt" : CDF, "name" : "output/cftfrac.nc"}},
{ "id" : SEASONALITY,      "file" : { "fmt" : CDF, "name" : "output/seasonality.nc"}},
#endif
#ifdef DAILY_OUTPUT
{ "id" : D_NPP,            "file" : { "fmt" : CDF, "name" : "output/d_npp.nc"}},
{ "id" : D_GPP,            "file" : { "fmt" : CDF, "name" : "output/d_gpp.nc"}},
{ "id" : D_RH,             "file" : { "fmt" : CDF, "name" : "output/d_rh.nc"}},
{ "id" : D_TRANS,          "file" : { "fmt" : CDF, "name" : "output/d_trans.nc"}},
{ "id" : D_INTERC,         "file" : { "fmt" : CDF, "name" : "output/d_interc.nc"}},
{ "id" : D_EVAP,           "file" : { "fmt" : CDF, "name" : "output/d_evap.nc"}},
#endif
{ "id" : MPET,             "file" : { "fmt" : CDF, "name" : "output/mpet.nc"}},
{ "id" : MALBEDO,          "file" : { "fmt" : CDF, "name" : "output/malbedo.nc"}},
{ "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : CDF, "name" : "output/maxthaw_depth.nc"}},
{ "id" : MSOILTEMP1,       "file" : { "fmt" : CDF, "name" : "output/msoiltemp1.nc"}},
{ "id" : MSOILTEMP2,       "file" : { "fmt" : CDF, "name" : "output/msoiltemp2.nc"}},
{ "id" : MSOILTEMP3,       "file" : { "fmt" : CDF, "name" : "output/msoiltemp3.nc"}},
{ "id" : SOILC_LAYER,      "file" : { "fmt" : CDF, "name" : "output/soilc_layer.nc"}},
{ "id" : AGB,              "file" : { "fmt" : CDF, "name" : "output/agb.nc"}},
{ "id" : MRETURN_FLOW_B,   "file" : { "fmt" : CDF, "name" : "output/mreturn_flow_b.nc"}},
{ "id" : MTRANSP_B,        "file" : { "fmt" : CDF, "name" : "output/mtransp_b.nc"}},
{ "id" : MEVAP_B,          "file" : { "fmt" : CDF, "name" : "output/mevap_b.nc"}},
{ "id" : MINTERC_B,        "file" : { "fmt" : CDF, "name" : "output/mintec_b.nc"}},
{ "id" : ACONV_LOSS_EVAP,  "file" : { "fmt" : CDF, "name" : "output/aconv_loss_evap.nc"}},
{ "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : CDF, "name" : "output/aconv_loss_drain.nc"}}
/*------------------------ ---------------------- ------------------------------- */
],

"crop_index" : TEMPERATE_CEREALS,  /* CFT for daily output_SPITFIRE-optpar */
"crop_irrigation" : DAILY_RAINFED, /* irrigation flag for daily output_SPITFIRE-optpar */

#else

"output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

"startgrid" : ALL, /* 27410 67208 60400 all grid cells */

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
"lastyear" : 2011, /* last year of simulation */
"restart" :  true, /* start from restart file */
"restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
"write_restart" : true, /* create restart file */
"write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
"restart_year": 1900 /* write restart at year */

#endif
}
