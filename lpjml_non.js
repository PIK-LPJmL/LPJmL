/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  n  o  n  .  j  s                           \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.1.001 without nitrogen limitation     \n**/
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

  "sim_name" : "LPJmL Run without nitrogen", /* Simulation description */
  "sim_id"   : "lpjml",       /* LPJML Simulation type with managed land use */
  "version"  : "5.1",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "river_routing" : true,
  "permafrost" : true,
  "with_nitrogen" : "no_nitrogen", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "const_climate" : false,
#ifdef FROM_RESTART
  "equilsoil" : false,
  "population" : false,
  "landuse" : "landuse", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : "wateruse",  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
#else
  "equilsoil" : true,
  "population" : false,
  "landuse" : "no_landuse",
  "reservoir" : false,
  "wateruse" : "no_wateruse",
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "fixed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "remove_residuals" : false,           /* remove residuals */
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim_irrigation",        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "laimax_interpolate" : "laimax_cft",    /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "istimber" : false,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "others_to_crop" : true,             /* move PFT type others into PFT crop, maize for tropical, wheat for temperate */

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_non.js"    /* Input parameter file without nitrogen */

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
    { "id" : GRID,             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
    { "id" : FPC,              "file" : { "fmt" : "raw", "name" : "output/fpc.bin"}},
    { "id" : MNPP,             "file" : { "fmt" : "raw", "name" : "output/mnpp.bin"}},
    { "id" : MGPP,             "file" : { "fmt" : "raw", "name" : "output/mgpp.bin"}},
    { "id" : MRH,              "file" : { "fmt" : "raw", "name" : "output/mrh.bin"}},
    { "id" : MFAPAR,           "file" : { "fmt" : "raw", "name" : "output/mfapar.bin"}},
    { "id" : MTRANSP,          "file" : { "fmt" : "raw", "name" : "output/mtransp.bin"}},
    { "id" : MRUNOFF,          "file" : { "fmt" : "raw", "name" : "output/mrunoff.bin"}},
    { "id" : MEVAP,            "file" : { "fmt" : "raw", "name" : "output/mevap.bin"}},
    { "id" : MINTERC,          "file" : { "fmt" : "raw", "name" : "output/minterc.bin"}},
    { "id" : MSWC1,            "file" : { "fmt" : "raw", "name" : "output/mswc1.bin"}},
    { "id" : MSWC2,            "file" : { "fmt" : "raw", "name" : "output/mswc2.bin"}},
    { "id" : FIREC,            "file" : { "fmt" : "raw", "name" : "output/firec.bin"}},
    { "id" : FIREF,            "file" : { "fmt" : "raw", "name" : "output/firef.bin"}},
    { "id" : VEGC,             "file" : { "fmt" : "raw", "name" : "output/vegc.bin"}},
    { "id" : SOILC,            "file" : { "fmt" : "raw", "name" : "output/soilc.bin"}},
    { "id" : LITC,             "file" : { "fmt" : "raw", "name" : "output/litc.bin"}},
    { "id" : FLUX_ESTABC,      "file" : { "fmt" : "raw", "name" : "output/flux_estab.bin"}},
    { "id" : PFT_VEGC,         "file" : { "fmt" : "raw", "name" : "output/pft_vegc.bin"}},
    { "id" : MPHEN_TMIN,       "file" : { "fmt" : "raw", "name" : "output/mphen_tmin.bin"}},
    { "id" : MPHEN_TMAX,       "file" : { "fmt" : "raw", "name" : "output/mphen_tmax.bin"}},
    { "id" : MPHEN_LIGHT,      "file" : { "fmt" : "raw", "name" : "output/mphen_light.bin"}},
    { "id" : MPHEN_WATER,      "file" : { "fmt" : "raw", "name" : "output/mphen_water.bin"}},
#ifdef WITH_SPITFIRE
    { "id" : MFIREC,           "file" : { "fmt" : "raw", "name" : "output/mfirec.bin"}},
    { "id" : MNFIRE,           "file" : { "fmt" : "raw", "name" : "output/mnfire.bin"}},
    { "id" : MBURNTAREA,       "file" : { "fmt" : "raw", "name" : "output/mburnt_area.bin"}},
    { "id" : ABURNTAREA,       "file" : { "fmt" : "raw", "name" : "output/aburnt_area.bin"}},
#endif
    { "id" : MDISCHARGE,       "file" : { "fmt" : "raw", "name" : "output/mdischarge.bin"}},
    { "id" : MWATERAMOUNT,     "file" : { "fmt" : "raw", "name" : "output/mwateramount.bin"}},
    { "id" : HARVESTC,         "file" : { "fmt" : "raw", "name" : "output/flux_harvest.bin"}},
    { "id" : SDATE,            "file" : { "fmt" : "raw", "name" : "output/sdate.bin"}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : "raw", "name" : "output/cftfrac.bin"}},
    { "id" : SEASONALITY,      "file" : { "fmt" : "raw", "name" : "output/seasonality.bin"}},
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : "raw", "name" : "output/d_npp.bin"}},
    { "id" : D_GPP,            "file" : { "fmt" : "raw", "name" : "output/d_gpp.bin"}},
    { "id" : D_RH,             "file" : { "fmt" : "raw", "name" : "output/d_rh.bin"}},
    { "id" : D_TRANS,          "file" : { "fmt" : "raw", "name" : "output/d_trans.bin"}},
    { "id" : D_INTERC,         "file" : { "fmt" : "raw", "name" : "output/d_interc.bin"}},
    { "id" : D_EVAP,           "file" : { "fmt" : "raw", "name" : "output/d_evap.bin"}},
#endif
    { "id" : MPET,             "file" : { "fmt" : "raw", "name" : "output/mpet.bin"}},
    { "id" : MALBEDO,          "file" : { "fmt" : "raw", "name" : "output/malbedo.bin"}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : "raw", "name" : "output/maxthaw_depth.bin"}},
    { "id" : MSOILTEMP1,       "file" : { "fmt" : "raw", "name" : "output/msoiltemp1.bin"}},
    { "id" : MSOILTEMP2,       "file" : { "fmt" : "raw", "name" : "output/msoiltemp2.bin"}},
    { "id" : MSOILTEMP3,       "file" : { "fmt" : "raw", "name" : "output/msoiltemp3.bin"}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : "raw", "name" : "output/soilc_layer.bin"}},
    { "id" : AGB,              "file" : { "fmt" : "raw", "name" : "output/agb.bin"}},
    { "id" : AGB_TREE,         "file" : { "fmt" : "raw", "name" : "output/agb_tree.bin"}},
    { "id" : MRETURN_FLOW_B,   "file" : { "fmt" : "raw", "name" : "output/mreturn_flow_b.bin"}},
    { "id" : MTRANSP_B,        "file" : { "fmt" : "raw", "name" : "output/mtransp_b.bin"}},
    { "id" : MEVAP_B,          "file" : { "fmt" : "raw", "name" : "output/mevap_b.bin"}},
    { "id" : MINTERC_B,        "file" : { "fmt" : "raw", "name" : "output/mintec_b.bin"}},
    { "id" : ACONV_LOSS_EVAP,  "file" : { "fmt" : "raw", "name" : "output/aconv_loss_evap.bin"}},
    { "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : "raw", "name" : "output/aconv_loss_drain.bin"}}
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
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2011, /* last year of simulation */
  "outputyear": 1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
