/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  w  p  .  j  s                              \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.0.002 with wood plantations           \n**/
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

  "sim_name" : "LPJmL run with wood plantations", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "version"  : "5.1",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fdi" : "nesterov_index", /* different fire danger index formulations: WVPD_INDEX(needs GLDAS input data), NESTEROV_INDEX*/
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : false,
  "permafrost" : true,
  "with_nitrogen" : "lim_nitrogen", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "store_climate" : true, /* store climate data in spin-up phase */
  "const_climate" : false,
  "shuffle_climate" : true, /* shuffle spinup climate */
  "const_deposition" : false,
  "fix_climate" : false,
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "population" : false,
  "landuse" : "landuse", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : "wateruse",  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
  "equilsoil" : false,
#else
  "population" : false,
  "landuse" : "no_landuse",
  "reservoir" : false,
  "wateruse" : "no_wateruse",
  "equilsoil" : true,
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "fixed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "remove_residuals" : false,           /* remove residuals */
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim_irrigation",      /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "laimax_interpolate" : "laimax_par",  /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "others_to_crop" : true,             /* move PFT type others into PFT crop, maize for tropical, wheat for temperate */
  "mowing_days" : [152, 335],          /* Mowing days for grassland if grass harvest options are ser */
  "crop_resp_fix" : false,             /* variable C:N ratio for crop respiration */
                                       /* for MAgPIE runs, turn off dynamic C:N ratio dependent respiration,
                                          which reduces yields at high N inputs */

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_wp.js"    /* Input parameter file */

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

  "float_grid" : false, /* set datatype of grid file to float (TRUE/FALSE) */

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
    { "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
    { "id" : "fpc",              "file" : { "fmt" : "raw", "name" : "output/fpc.bin"}},
    { "id" : "npp",              "file" : { "fmt" : "raw", "name" : "output/mnpp.bin"}},
    { "id" : "gpp",              "file" : { "fmt" : "raw", "name" : "output/mgpp.bin"}},
    { "id" : "rh",               "file" : { "fmt" : "raw", "name" : "output/mrh.bin"}},
    { "id" : "fapar",            "file" : { "fmt" : "raw", "name" : "output/mfapar.bin"}},
    { "id" : "transp",           "file" : { "fmt" : "raw", "name" : "output/mtransp.bin"}},
    { "id" : "runoff",           "file" : { "fmt" : "raw", "name" : "output/mrunoff.bin"}},
    { "id" : "evap",             "file" : { "fmt" : "raw", "name" : "output/mevap.bin"}},
    { "id" : "interc",           "file" : { "fmt" : "raw", "name" : "output/minterc.bin"}},
    { "id" : "swc1",             "file" : { "fmt" : "raw", "name" : "output/mswc1.bin"}},
    { "id" : "swc2",             "file" : { "fmt" : "raw", "name" : "output/mswc2.bin"}},
    { "id" : "firef",            "file" : { "fmt" : "raw", "name" : "output/firef.bin"}},
    { "id" : "vegc",             "file" : { "fmt" : "raw", "name" : "output/vegc.bin"}},
    { "id" : "soilc",            "file" : { "fmt" : "raw", "name" : "output/soilc.bin"}},
    { "id" : "litc",             "file" : { "fmt" : "raw", "name" : "output/litc.bin"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "raw", "name" : "output/flux_estab.bin"}},
    { "id" : "pft_vegc",         "file" : { "fmt" : "raw", "name" : "output/pft_vegc.bin"}},
    { "id" : "phen_tmin",        "file" : { "fmt" : "raw", "name" : "output/mphen_tmin.bin"}},
    { "id" : "phen_tmax",        "file" : { "fmt" : "raw", "name" : "output/mphen_tmax.bin"}},
    { "id" : "phen_light",       "file" : { "fmt" : "raw", "name" : "output/mphen_light.bin"}},
    { "id" : "phen_water",       "file" : { "fmt" : "raw", "name" : "output/mphen_water.bin"}},
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
    { "id" : "nuptake",          "file" : { "fmt" : "raw", "name" : "output/mnuptake.bin"}},
    { "id" : "leaching",         "file" : { "fmt" : "raw", "name" : "output/mleaching.bin"}},
    { "id" : "n2o_denit",        "file" : { "fmt" : "raw", "name" : "output/mn2o_denit.bin"}},
    { "id" : "n2o_nit",          "file" : { "fmt" : "raw", "name" : "output/mn2o_nit.bin"}},
    { "id" : "n2_emis",          "file" : { "fmt" : "raw", "name" : "output/mn2_emis.bin"}},
    { "id" : "bnf",              "file" : { "fmt" : "raw", "name" : "output/mbnf.bin"}},
    { "id" : "n_immo",           "file" : { "fmt" : "raw", "name" : "output/mn_immo.bin"}},
    { "id" : "pft_ndemand",      "file" : { "fmt" : "raw", "name" : "output/pft_ndemand.bin"}},
    { "id" : "firen",            "file" : { "fmt" : "raw", "name" : "output/firen.bin"}},
    { "id" : "n_mineralization", "file" : { "fmt" : "raw", "name" : "output/mn_mineralization.bin"}},
    { "id" : "n_volatilization", "file" : { "fmt" : "raw", "name" : "output/mn_volatilization.bin"}},
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
    { "id" : "firec",            "file" : { "fmt" : "raw", "timestep" : "monthly", "unit" : "gC/m2/month", "name" : "output/mfirec.bin"}},
    { "id" : "nfire",            "file" : { "fmt" : "raw", "name" : "output/mnfire.bin"}},
    { "id" : "burntarea",        "file" : { "fmt" : "raw", "name" : "output/mburnt_area.bin"}},
#else
    { "id" : "firec",            "file" : { "fmt" : "raw", "name" : "output/firec.bin"}},
#endif
    { "id" : "discharge",        "file" : { "fmt" : "raw", "name" : "output/mdischarge.bin"}},
    { "id" : "wateramount",      "file" : { "fmt" : "raw", "name" : "output/mwateramount.bin"}},
    { "id" : "harvestc",         "file" : { "fmt" : "raw", "name" : "output/flux_harvest.bin"}},
    { "id" : "sdate",            "file" : { "fmt" : "raw", "name" : "output/sdate.bin"}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "raw", "name" : "output/cftfrac.bin"}},
    { "id" : "seasonality",      "file" : { "fmt" : "raw", "name" : "output/seasonality.bin"}},
    { "id" : "pet",              "file" : { "fmt" : "raw", "name" : "output/mpet.bin"}},
    { "id" : "albedo",           "file" : { "fmt" : "raw", "name" : "output/malbedo.bin"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "raw", "name" : "output/maxthaw_depth.bin"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "raw", "name" : "output/msoiltemp1.bin"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "raw", "name" : "output/msoiltemp2.bin"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "raw", "name" : "output/msoiltemp3.bin"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "raw", "name" : "output/soilc_layer.bin"}},
    { "id" : "agb",              "file" : { "fmt" : "raw", "name" : "output/agb.bin"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "raw", "name" : "output/agb_tree.bin"}},
    { "id" : "return_flow_b",    "file" : { "fmt" : "raw", "name" : "output/mreturn_flow_b.bin"}},
    { "id" : "transp_b",         "file" : { "fmt" : "raw", "name" : "output/mtransp_b.bin"}},
    { "id" : "evap_b",           "file" : { "fmt" : "raw", "name" : "output/mevap_b.bin"}},
    { "id" : "interc_b",         "file" : { "fmt" : "raw", "name" : "output/mintec_b.bin"}},
    { "id" : "prod_turnover",    "file" : { "fmt" : "raw", "name" : "output/prod_turnover.bin"}},
    { "id" : "deforest_emis",    "file" : { "fmt" : "raw", "name" : "output/deforest_emis.bin"}},
    { "id" : "conv_loss_evap",   "file" : { "fmt" : "raw", "name" : "output/aconv_loss_evap.bin"}},
    { "id" : "conv_loss_drain",  "file" : { "fmt" : "raw", "name" : "output/aconv_loss_drain.bin"}}
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : "all", /* 27410, 67208 60400 47284 47293 47277 all grid cells */
  "endgrid" : ALL,

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 7000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "restart" :  false, /* start from restart file */
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
