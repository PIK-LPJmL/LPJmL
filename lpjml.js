/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.1.001                         \n**/
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

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : LPJML,       /* LPJML Simulation type with managed land use */
  "version"  : "5.2",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : RADIATION,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : FIRE,            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "river_routing" : true,
  "permafrost" : true,
  "with_nitrogen" : LIM_NITROGEN, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "const_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "population" : false,
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
  "tillage" : true,                     /* enable tillaging */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "laimax_interpolate" : LAIMAX_PAR,    /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,

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
    { "id" : GRID,             "file" : { "fmt" : RAW, "name" : "output/grid.bin" }},
    { "id" : FPC,              "file" : { "fmt" : RAW, "name" : "output/fpc.bin"}},
    { "id" : MNPP,             "file" : { "fmt" : RAW, "name" : "output/mnpp.bin"}},
    { "id" : MGPP,             "file" : { "fmt" : RAW, "name" : "output/mgpp.bin"}},
    { "id" : MRH,              "file" : { "fmt" : RAW, "name" : "output/mrh.bin"}},
    { "id" : MFAPAR,           "file" : { "fmt" : RAW, "name" : "output/mfapar.bin"}},
    { "id" : MTRANSP,          "file" : { "fmt" : RAW, "name" : "output/mtransp.bin"}},
    { "id" : MRUNOFF,          "file" : { "fmt" : RAW, "name" : "output/mrunoff.bin"}},
    { "id" : MEVAP,            "file" : { "fmt" : RAW, "name" : "output/mevap.bin"}},
    { "id" : MINTERC,          "file" : { "fmt" : RAW, "name" : "output/minterc.bin"}},
    { "id" : MSWC1,            "file" : { "fmt" : RAW, "name" : "output/mswc1.bin"}},
    { "id" : MSWC2,            "file" : { "fmt" : RAW, "name" : "output/mswc2.bin"}},
    { "id" : FIREC,            "file" : { "fmt" : RAW, "name" : "output/firec.bin"}},
    { "id" : FIREF,            "file" : { "fmt" : RAW, "name" : "output/firef.bin"}},
    { "id" : VEGC,             "file" : { "fmt" : RAW, "name" : "output/vegc.bin"}},
    { "id" : SOILC,            "file" : { "fmt" : RAW, "name" : "output/soilc.bin"}},
    { "id" : LITC,             "file" : { "fmt" : RAW, "name" : "output/litc.bin"}},
    { "id" : FLUX_ESTABC,      "file" : { "fmt" : RAW, "name" : "output/flux_estab.bin"}},
    { "id" : PFT_VEGC,         "file" : { "fmt" : RAW, "name" : "output/pft_vegc.bin"}},
    { "id" : MPHEN_TMIN,       "file" : { "fmt" : RAW, "name" : "output/mphen_tmin.bin"}},
    { "id" : MPHEN_TMAX,       "file" : { "fmt" : RAW, "name" : "output/mphen_tmax.bin"}},
    { "id" : MPHEN_LIGHT,      "file" : { "fmt" : RAW, "name" : "output/mphen_light.bin"}},
    { "id" : MPHEN_WATER,      "file" : { "fmt" : RAW, "name" : "output/mphen_water.bin"}},
    { "id" : VEGN,             "file" : { "fmt" : RAW, "name" : "output/vegn.bin"}},
    { "id" : SOILN,            "file" : { "fmt" : RAW, "name" : "output/soiln.bin"}},
    { "id" : LITN,             "file" : { "fmt" : RAW, "name" : "output/litn.bin"}},
    { "id" : SOILN_LAYER,      "file" : { "fmt" : RAW, "name" : "output/soiln_layer.bin"}},
    { "id" : SOILNO3_LAYER,    "file" : { "fmt" : RAW, "name" : "output/soilno3_layer.bin"}},
    { "id" : SOILNH4_LAYER,    "file" : { "fmt" : RAW, "name" : "output/soilnh4_layer.bin"}},
    { "id" : SOILN_SLOW,       "file" : { "fmt" : RAW, "name" : "output/soiln_slow.bin"}},
    { "id" : SOILNH4,          "file" : { "fmt" : RAW, "name" : "output/soilnh4.bin"}},
    { "id" : SOILNO3,          "file" : { "fmt" : RAW, "name" : "output/soilno3.bin"}},
    { "id" : PFT_NUPTAKE,      "file" : { "fmt" : RAW, "name" : "output/pft_nuptake.bin"}},
    { "id" : MNUPTAKE,         "file" : { "fmt" : RAW, "name" : "output/mnuptake.bin"}},
    { "id" : MLEACHING,        "file" : { "fmt" : RAW, "name" : "output/mleaching.bin"}},
    { "id" : MN2O_DENIT,       "file" : { "fmt" : RAW, "name" : "output/mn2o_denit.bin"}},
    { "id" : MN2O_NIT,         "file" : { "fmt" : RAW, "name" : "output/mn2o_nit.bin"}},
    { "id" : MN2_EMIS,         "file" : { "fmt" : RAW, "name" : "output/mn2_emis.bin"}},
    { "id" : MBNF,             "file" : { "fmt" : RAW, "name" : "output/mbnf.bin"}},
    { "id" : MN_IMMO,          "file" : { "fmt" : RAW, "name" : "output/mn_immo.bin"}},
    { "id" : PFT_NDEMAND,      "file" : { "fmt" : RAW, "name" : "output/pft_ndemand.bin"}},
    { "id" : FIREN,            "file" : { "fmt" : RAW, "name" : "output/firen.bin"}},
    { "id" : MN_MINERALIZATION,"file" : { "fmt" : RAW, "name" : "output/mn_mineralization.bin"}},
    { "id" : MN_VOLATILIZATION,"file" : { "fmt" : RAW, "name" : "output/mn_volatilization.bin"}},
    { "id" : PFT_NLIMIT,       "file" : { "fmt" : RAW, "name" : "output/pft_nlimit.bin"}},
    { "id" : PFT_VEGN,         "file" : { "fmt" : RAW, "name" : "output/pft_vegn.bin"}},
    { "id" : PFT_CLEAF,        "file" : { "fmt" : RAW, "name" : "output/pft_cleaf.bin"}},
    { "id" : PFT_NLEAF,        "file" : { "fmt" : RAW, "name" : "output/pft_nleaf.bin"}},
    { "id" : PFT_LAIMAX,       "file" : { "fmt" : RAW, "name" : "output/pft_laimax.bin"}},
    { "id" : PFT_CROOT,        "file" : { "fmt" : RAW, "name" : "output/pft_croot.bin"}},
    { "id" : PFT_NROOT,        "file" : { "fmt" : RAW, "name" : "output/pft_nroot.bin"}},
    { "id" : PFT_CSAPW,        "file" : { "fmt" : RAW, "name" : "output/pft_csapw.bin"}},
    { "id" : PFT_NSAPW,        "file" : { "fmt" : RAW, "name" : "output/pft_nsapw.bin"}},
    { "id" : PFT_CHAWO,        "file" : { "fmt" : RAW, "name" : "output/pft_chawo.bin"}},
    { "id" : PFT_NHAWO,        "file" : { "fmt" : RAW, "name" : "output/pft_nhawo.bin"}},
#ifdef WITH_SPITFIRE
    { "id" : MFIREC,           "file" : { "fmt" : RAW, "name" : "output/mfirec.bin"}},
    { "id" : MNFIRE,           "file" : { "fmt" : RAW, "name" : "output/mnfire.bin"}},
    { "id" : MBURNTAREA,       "file" : { "fmt" : RAW, "name" : "output/mburnt_area.bin"}},
    { "id" : ABURNTAREA,       "file" : { "fmt" : RAW, "name" : "output/aburnt_area.bin"}},
#endif
    { "id" : MDISCHARGE,       "file" : { "fmt" : RAW, "name" : "output/mdischarge.bin"}},
    { "id" : MWATERAMOUNT,     "file" : { "fmt" : RAW, "name" : "output/mwateramount.bin"}},
    { "id" : HARVESTC,         "file" : { "fmt" : RAW, "name" : "output/flux_harvest.bin"}},
    { "id" : SDATE,            "file" : { "fmt" : RAW, "name" : "output/sdate.bin"}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : RAW, "name" : "output/cftfrac.bin"}},
    { "id" : SEASONALITY,      "file" : { "fmt" : RAW, "name" : "output/seasonality.bin"}},
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : RAW, "name" : "output/d_npp.bin"}},
    { "id" : D_GPP,            "file" : { "fmt" : RAW, "name" : "output/d_gpp.bin"}},
    { "id" : D_RH,             "file" : { "fmt" : RAW, "name" : "output/d_rh.bin"}},
    { "id" : D_TRANS,          "file" : { "fmt" : RAW, "name" : "output/d_trans.bin"}},
    { "id" : D_INTERC,         "file" : { "fmt" : RAW, "name" : "output/d_interc.bin"}},
    { "id" : D_EVAP,           "file" : { "fmt" : RAW, "name" : "output/d_evap.bin"}},
#endif
    { "id" : MPET,             "file" : { "fmt" : RAW, "name" : "output/mpet.bin"}},
    { "id" : MALBEDO,          "file" : { "fmt" : RAW, "name" : "output/malbedo.bin"}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : RAW, "name" : "output/maxthaw_depth.bin"}},
    { "id" : MSOILTEMP1,       "file" : { "fmt" : RAW, "name" : "output/msoiltemp1.bin"}},
    { "id" : MSOILTEMP2,       "file" : { "fmt" : RAW, "name" : "output/msoiltemp2.bin"}},
    { "id" : MSOILTEMP3,       "file" : { "fmt" : RAW, "name" : "output/msoiltemp3.bin"}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : RAW, "name" : "output/soilc_layer.bin"}},
    { "id" : AGB,              "file" : { "fmt" : RAW, "name" : "output/agb.bin"}},
    { "id" : AGB_TREE,         "file" : { "fmt" : RAW, "name" : "output/agb_tree.bin"}},
    { "id" : MRETURN_FLOW_B,   "file" : { "fmt" : RAW, "name" : "output/mreturn_flow_b.bin"}},
    { "id" : MTRANSP_B,        "file" : { "fmt" : RAW, "name" : "output/mtransp_b.bin"}},
    { "id" : MEVAP_B,          "file" : { "fmt" : RAW, "name" : "output/mevap_b.bin"}},
    { "id" : MINTERC_B,        "file" : { "fmt" : RAW, "name" : "output/mintec_b.bin"}},
    { "id" : ACONV_LOSS_EVAP,  "file" : { "fmt" : RAW, "name" : "output/aconv_loss_evap.bin"}},
    { "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : RAW, "name" : "output/aconv_loss_drain.bin"}}
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
