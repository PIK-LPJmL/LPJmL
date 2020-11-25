/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  r  c  p  8  5  _  n  o  n  .  j  s         \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.1.001                                 \n**/
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

  "sim_name" : "LPJmL run with RCP85 climatology without nitrogen", /* Simulation description */
  "sim_id"   : LPJML,       /* LPJML Simulation type with managed land use */
  "version"  : "5.1",       /* LPJmL version expected */
  "random_prec" : false,    /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : RADIATION,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : FIRE,            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,
  "river_routing" : true,
  "permafrost" : true,
  "with_nitrogen" : NO_NITROGEN, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "const_climate" : false,
  "const_deposition" : false,
  "shuffle_climate" : true,
  "fix_climate" : true,
  "fix_landuse" : false,
  "fix_climate_year" : 2094,
  "fix_climate_cycle": 11,
#ifdef FROM_RESTART
  "new_seed" : false,
  "equilsoil" : false,
  "population" : false,
  "landuse" : LANDUSE, /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : ALL_WATERUSE,  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
#else
  "equilsoil" : true,
  "population" : false,
  "landuse" : NO_LANDUSE,
  "reservoir" : false,
  "wateruse" : NO_WATERUSE,
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : NO_LANDCOVER, /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : FIXED_SDATE,   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "remove_residuals" : false,           /* remove residuals */
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : LIM_IRRIGATION,        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "laimax_interpolate" : LAIMAX_CFT,    /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "others_to_crop" : true,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_non.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

#include "input_rcp85.js"    /* Input files of RCP85 dataset */

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef WITH_GRIDBASED
  "pft_output_scaled" : GRIDBASED,
#define SUFFIX grid.nc
#else
  "pft_output_scaled" : PFTBASED,
#define SUFFIX pft.nc
#endif

  "crop_index" : TEMPERATE_CEREALS,  /* CFT for daily output */
  "crop_irrigation" : DAILY_RAINFED, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" : 
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : GRID,             "file" : { "fmt" : RAW, "name" : "output/grid.bin" }},
    { "id" : FPC,              "file" : { "fmt" : CDF, "name" : "output/fpc.nc"}},
    { "id" : NPP,              "file" : { "fmt" : CDF, "name" : "output/mnpp.nc"}},
    { "id" : GPP,              "file" : { "fmt" : CDF, "name" : "output/mgpp.nc"}},
    { "id" : RH,               "file" : { "fmt" : CDF, "name" : "output/mrh.nc"}},
    { "id" : FAPAR,            "file" : { "fmt" : CDF, "name" : "output/mfapar.nc"}},
    { "id" : TRANSP,           "file" : { "fmt" : CDF, "name" : "output/mtransp.nc"}},
    { "id" : RUNOFF,           "file" : { "fmt" : CDF, "name" : "output/mrunoff.nc"}},
    { "id" : EVAP,             "file" : { "fmt" : CDF, "name" : "output/mevap.nc"}},
    { "id" : INTERC,           "file" : { "fmt" : CDF, "name" : "output/minterc.nc"}},
    { "id" : SWC1,             "file" : { "fmt" : CDF, "name" : "output/mswc1.nc"}},
    { "id" : SWC2,             "file" : { "fmt" : CDF, "name" : "output/mswc2.nc"}},
    { "id" : FIREF,            "file" : { "fmt" : CDF, "name" : "output/firef.nc"}},
    { "id" : VEGC,             "file" : { "fmt" : CDF, "name" : "output/vegc.nc"}},
    { "id" : SOILC,            "file" : { "fmt" : CDF, "name" : "output/soilc.nc"}},
    { "id" : LITC,             "file" : { "fmt" : CDF, "name" : "output/litc.nc"}},
    { "id" : FLUX_ESTABC,      "file" : { "fmt" : CDF, "name" : "output/flux_estab.nc"}},
    { "id" : PFT_VEGC,         "file" : { "fmt" : CDF, "name" : "output/pft_vegc.nc"}},
    { "id" : PHEN_TMIN,        "file" : { "fmt" : CDF, "name" : "output/mphen_tmin.nc"}},
    { "id" : PHEN_TMAX,        "file" : { "fmt" : CDF, "name" : "output/mphen_tmax.nc"}},
    { "id" : PHEN_LIGHT,       "file" : { "fmt" : CDF, "name" : "output/mphen_light.nc"}},
    { "id" : PHEN_WATER,       "file" : { "fmt" : CDF, "name" : "output/mphen_water.nc"}},
    { "id" : VEGN,             "file" : { "fmt" : CDF, "name" : "output/vegn.nc"}},
    { "id" : LITFALLC,         "file" : { "fmt" : CDF, "name" : "output/alitfallc.nc"}},
    { "id" : PFT_CLEAF,        "file" : { "fmt" : CDF, "name" : "output/pft_cleaf.nc"}},
    { "id" : PFT_LAIMAX,       "file" : { "fmt" : CDF, "name" : "output/pft_laimax.nc"}},
    { "id" : PFT_CROOT,        "file" : { "fmt" : CDF, "name" : "output/pft_croot.nc"}},
    { "id" : PFT_CSAPW,        "file" : { "fmt" : CDF, "name" : "output/pft_csapw.nc"}},
    { "id" : PFT_CHAWO,        "file" : { "fmt" : CDF, "name" : "output/pft_chawo.nc"}},
#ifdef WITH_SPITFIRE
    { "id" : FIREC,            "file" : { "fmt" : CDF, "timestep" : MONTHLY, "unit" : "gC/m2/month", "name" : "output/mfirec.nc"}},
    { "id" : NFIRE,            "file" : { "fmt" : CDF, "name" : "output/mnfire.nc"}},
    { "id" : BURNTAREA,        "file" : { "fmt" : CDF, "name" : "output/mburnt_area.nc"}},
#else
    { "id" : FIREC,            "file" : { "fmt" : CDF, "name" : "output/firec.nc"}},
#endif
    { "id" : DISCHARGE,        "file" : { "fmt" : CDF, "name" : "output/mdischarge.nc"}},
    { "id" : WATERAMOUNT,      "file" : { "fmt" : CDF, "name" : "output/mwateramount.nc"}},
    { "id" : HARVESTC,         "file" : { "fmt" : CDF, "name" : "output/flux_harvestc.nc"}},
    { "id" : SDATE,            "file" : { "fmt" : CDF, "name" : "output/sdate.nc"}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : CDF, "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : CDF, "name" : "output/cftfrac.nc"}},
    { "id" : SEASONALITY,      "file" : { "fmt" : CDF, "name" : "output/seasonality.nc"}},
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : CDF, "name" : "output/d_npp.nc"}},
    { "id" : D_GPP,            "file" : { "fmt" : CDF, "name" : "output/d_gpp.nc"}},
    { "id" : D_RH,             "file" : { "fmt" : CDF, "name" : "output/d_rh.nc"}},
    { "id" : D_TRANS,          "file" : { "fmt" : CDF, "name" : "output/d_trans.nc"}},
    { "id" : D_INTERC,         "file" : { "fmt" : CDF, "name" : "output/d_interc.nc"}},
    { "id" : D_EVAP,           "file" : { "fmt" : CDF, "name" : "output/d_evap.nc"}},
#endif
    { "id" : DEFOREST_EMIS,    "file" : { "fmt" : CDF, "name" : "output/deforest_emis.nc"}},
    { "id" : PROD_TURNOVER,    "file" : { "fmt" : CDF, "name" : "output/prod_turnover.nc"}},
    { "id" : PRODUCT_POOL_FAST,"file" : { "fmt" : CDF, "name" : "output/product_pool_fast.nc"}},
    { "id" : PRODUCT_POOL_SLOW,"file" : { "fmt" : CDF, "name" : "output/product_pool_slow.nc"}},
    { "id" : PET,              "file" : { "fmt" : CDF, "name" : "output/mpet.nc"}},
    { "id" : ALBEDO,           "file" : { "fmt" : CDF, "name" : "output/malbedo.nc"}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : CDF, "name" : "output/maxthaw_depth.nc"}},
    { "id" : SOILTEMP1,        "file" : { "fmt" : CDF, "name" : "output/msoiltemp1.nc"}},
    { "id" : SOILTEMP2,        "file" : { "fmt" : CDF, "name" : "output/msoiltemp2.nc"}},
    { "id" : SOILTEMP3,        "file" : { "fmt" : CDF, "name" : "output/msoiltemp3.nc"}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : CDF, "name" : "output/soilc_layer.nc"}},
    { "id" : AGB,              "file" : { "fmt" : CDF, "name" : "output/agb.nc"}},
    { "id" : AGB_TREE,         "file" : { "fmt" : CDF, "name" : "output/agb_tree.nc"}},
    { "id" : RETURN_FLOW_B,    "file" : { "fmt" : CDF, "name" : "output/mreturn_flow_b.nc"}},
    { "id" : TRANSP_B,         "file" : { "fmt" : CDF, "name" : "output/mtransp_b.nc"}},
    { "id" : EVAP_B,           "file" : { "fmt" : CDF, "name" : "output/mevap_b.nc"}},
    { "id" : INTERC_B,         "file" : { "fmt" : CDF, "name" : "output/mintec_b.nc"}},
    { "id" : CONV_LOSS_EVAP,   "file" : { "fmt" : CDF, "name" : "output/aconv_loss_evap.nc"}},
    { "id" : CONV_LOSS_DRAIN,  "file" : { "fmt" : CDF, "name" : "output/aconv_loss_drain.nc"}}
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : ALL, /* 27410, 67208 60400 all grid cells */
  "endgrid" : ALL,

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1861, /* first year of simulation */
  "lastyear" : 1861, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1860_nv_stdfire.lpj", /* filename of restart file */
  "restart_year": 1860 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1861, /* first year of simulation */
  "lastyear" : 2599, /* last year of simulation */
  "outputyear": 1861, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1860_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : false, /* create restart file */

#endif
}
