/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  v  p  d  .  j  s                           \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 4.0.002                                 \n**/
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
  "sim_id"   : LPJML,       /* LPJML Simulation type with managed land use */
  "version"  : "4.0",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : RADIATION,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : SPITFIRE_TMAX,   /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fdi" : WVPD_INDEX,   /* different fire danger index formulations: WVPD_INDEX, NESTEROV_INDEX*/
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "river_routing" : false,
  "permafrost" : true,
#ifdef FROM_RESTART
  "population" : true,
  "landuse" : LANDUSE, /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : WATERUSE,  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
#else
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
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "grassland_fixed_pft" : false,

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
  "pft_output_scaled" : GRIDBASED,
#define SUFFIX grid.bin
#else
  "pft_output_scaled" : PFTBASED,
#define SUFFIX pft.bin
#endif

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "crop_index" : TEMPERATE_CEREALS,  /* CFT for daily output */
  "crop_irrigation" : DAILY_RAINFED, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" :
  [

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
    { "id" : MPHEN_TMIN,       "file" : { "fmt" : CDF, "name" : "output/mphen_tmin.nc"}},
    { "id" : MPHEN_TMAX,       "file" : { "fmt" : CDF, "name" : "output/mphen_tmax.nc"}},
    { "id" : MPHEN_LIGHT,      "file" : { "fmt" : CDF, "name" : "output/mphen_light.nc"}},
    { "id" : MPHEN_WATER,      "file" : { "fmt" : CDF, "name" : "output/mphen_water.nc"}},
    { "id" : MFIREC,           "file" : { "fmt" : CDF, "name" : "output/mfirec.nc"}},
    { "id" : MNFIRE,           "file" : { "fmt" : CDF, "name" : "output/mnfire.nc"}},
    { "id" : MBURNTAREA,       "file" : { "fmt" : CDF, "name" : "output/mburnt_area.nc"}},
    { "id" : ABURNTAREA,       "file" : { "fmt" : CDF, "name" : "output/aburnt_area.nc"}},
    { "id" : MFIREDI,          "file" : { "fmt" : CDF, "name" : "output/mfiredi.nc"}},
    { "id" : MDISCHARGE,       "file" : { "fmt" : CDF, "name" : "output/mdischarge.nc"}},
    { "id" : MWATERAMOUNT,     "file" : { "fmt" : CDF, "name" : "output/mwateramount.nc"}},
    { "id" : HARVEST,          "file" : { "fmt" : CDF, "name" : "output/flux_harvest.nc"}},
    { "id" : SDATE,            "file" : { "fmt" : CDF, "name" : "output/sdate.nc"}},
    { "id" : PFT_HARVEST,      "file" : { "fmt" : CDF, "name" : mkstr(output/pft_harvest.SUFFIX)}},
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
    { "id" : MPET,             "file" : { "fmt" : CDF, "name" : "output/mpet.nc"}},
    { "id" : MALBEDO,          "file" : { "fmt" : CDF, "name" : "output/malbedo.nc"}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : CDF, "name" : "output/maxthaw_depth.nc"}},
    { "id" : MSOILTEMP1,       "file" : { "fmt" : CDF, "name" : "output/msoiltemp1.nc"}},
    { "id" : MSOILTEMP2,       "file" : { "fmt" : CDF, "name" : "output/msoiltemp2.nc"}},
    { "id" : MSOILTEMP3,       "file" : { "fmt" : CDF, "name" : "output/msoiltemp3.nc"}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : CDF, "name" : "output/soilc_layer.nc"}},
    { "id" : AGB,              "file" : { "fmt" : CDF, "name" : "output/agb.nc"}},
    { "id" : AGB_TREE,         "file" : { "fmt" : CDF, "name" : "output/agb_tree.nc"}},
    { "id" : MRETURN_FLOW_B,   "file" : { "fmt" : CDF, "name" : "output/mreturn_flow_b.nc"}},
    { "id" : MTRANSP_B,        "file" : { "fmt" : CDF, "name" : "output/mtransp_b.nc"}},
    { "id" : MEVAP_B,          "file" : { "fmt" : CDF, "name" : "output/mevap_b.nc"}},
    { "id" : MINTERC_B,        "file" : { "fmt" : CDF, "name" : "output/mintec_b.nc"}},
    { "id" : ACONV_LOSS_EVAP,  "file" : { "fmt" : CDF, "name" : "output/aconv_loss_evap.nc"}},
    { "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : CDF, "name" : "output/aconv_loss_drain.nc"}}
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
