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
  "version"  : "5.1",       /* LPJmL version expected */
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : RADIATION,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : FIRE,            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : true,
  "equilsoil" :true,
  "permafrost" : true,
  "with_nitrogen" : LIM_NITROGEN, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "store_climate" : true, /* store climate data in spin-up phase */
  "const_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "population" : false,
  "landuse" : LANDUSE, /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : WATERUSE,  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
  "equilsoil" :false,
#else
  "population" : false,
  "landuse" : NO_LANDUSE,
  "reservoir" : false,
  "wateruse" : NO_WATERUSE,
  "equilsoil" :true,
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : NO_LANDCOVER, /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : FIXED_SDATE,   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "remove_residuals" : false,           /* remove residuals */
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : LIM_IRRIGATION,        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
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

#include "input_crumonthly_gswp3.js"    /* Input files of CRU dataset */

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
   /* { "id" : FPC,              "file" : { "fmt" : RAW, "name" : "../output/gswp3/fpc.bin"}},
    { "id" : NPP,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/mnpp.bin"}},
    { "id" : GPP,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/mgpp.bin"}},
    { "id" : RH,              "file" : { "fmt" : RAW, "name" : "../output/gswp3/mrh.bin"}},
    { "id" : FAPAR,           "file" : { "fmt" : RAW, "name" : "../output/gswp3/mfapar.bin"}},
    { "id" : TRANSP,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/mtransp.bin"}},
    { "id" : RUNOFF,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/mrunoff.bin"}},
    { "id" : EVAP,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/mevap.bin"}},
    { "id" : INTERC,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/minterc.bin"}},
    { "id" : SWC1,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/mswc1.bin"}},
    { "id" : SWC2,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/mswc2.bin"}},
    { "id" : FIREC,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/firec.bin"}},
    { "id" : FIREF,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/firef.bin"}},
    { "id" : VEGC,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/vegc.bin"}},
    { "id" : SOILC,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/soilc.bin"}},
    { "id" : LITC,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/litc.bin"}},
    { "id" : FLUX_ESTABC,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/flux_estab.bin"}},
    { "id" : PFT_VEGC,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_vegc.bin"}},
    { "id" : PHEN_TMIN,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/mphen_tmin.bin"}},
    { "id" : PHEN_TMAX,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/mphen_tmax.bin"}},
    { "id" : PHEN_LIGHT,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/mphen_light.bin"}},
    { "id" : PHEN_WATER,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/mphen_water.bin"}},
    { "id" : VEGN,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/vegn.bin"}},
    { "id" : SOILN,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/soiln.bin"}},
    { "id" : LITN,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/litn.bin"}},
    { "id" : SOILN_LAYER,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/soiln_layer.bin"}},
    { "id" : SOILNO3_LAYER,    "file" : { "fmt" : RAW, "name" : "../output/gswp3/soilno3_layer.bin"}},
    { "id" : SOILNH4_LAYER,    "file" : { "fmt" : RAW, "name" : "../output/gswp3/soilnh4_layer.bin"}},
    { "id" : SOILN_SLOW,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/soiln_slow.bin"}},
    { "id" : SOILNH4,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/soilnh4.bin"}},
    { "id" : SOILNO3,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/soilno3.bin"}},
    { "id" : PFT_NUPTAKE,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_nuptake.bin"}},
    { "id" : NUPTAKE,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/mnuptake.bin"}},
    { "id" : LEACHING,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/mleaching.bin"}},
    { "id" : N2O_DENIT,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/mn2o_denit.bin"}},
    { "id" : N2O_NIT,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/mn2o_nit.bin"}},
    { "id" : N2_EMIS,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/mn2_emis.bin"}},
    { "id" : BNF,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/mbnf.bin"}},
    { "id" : N_IMMO,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/mn_immo.bin"}},
    { "id" : PFT_NDEMAND,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_ndemand.bin"}},
    { "id" : FIREN,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/firen.bin"}},
    { "id" : N_MINERALIZATION,"file" : { "fmt" : RAW, "name" : "../output/gswp3/mn_mineralization.bin"}},
    { "id" : N_VOLATILIZATION,"file" : { "fmt" : RAW, "name" : "../output/gswp3/mn_volatilization.bin"}},
    { "id" : PFT_NLIMIT,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_nlimit.bin"}},
    { "id" : PFT_VEGN,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_vegn.bin"}},
    { "id" : PFT_CLEAF,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_cleaf.bin"}},
    { "id" : PFT_NLEAF,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_nleaf.bin"}},
    { "id" : PFT_LAIMAX,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_laimax.bin"}},
    { "id" : PFT_CROOT,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_croot.bin"}},
    { "id" : PFT_NROOT,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_nroot.bin"}},
    { "id" : PFT_CSAPW,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_csapw.bin"}},
    { "id" : PFT_NSAPW,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_nsapw.bin"}},
    { "id" : PFT_CHAWO,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_chawo.bin"}},
    { "id" : PFT_NHAWO,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/pft_nhawo.bin"}},*/
#ifdef WITH_SPITFIRE
    { "id" : FIREC,           "file" : { "fmt" : RAW, "name" : "output//mfirec.bin"}},
    { "id" : NFIRE,           "file" : { "fmt" : RAW, "name" : "output/mnfire.bin"}},
    { "id" : BURNTAREA,       "file" : { "fmt" : RAW, "name" : "output/mburnt_area.bin"}},
#endif
    { "id" : DISCHARGE,       "file" : { "fmt" : RAW, "name" : "output/mdischarge.bin"}},
   /* { "id" : WATERAMOUNT,     "file" : { "fmt" : RAW, "name" : "../output/gswp3/mwateramount.bin"}},
    { "id" : HARVESTC,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/flux_harvest.bin"}},
    { "id" : SDATE,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/sdate.bin"}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : RAW, "name" : mkstr(../output/gswp3/pft_harvest.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/cftfrac.bin"}},
    { "id" : SEASONALITY,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/seasonality.bin"}},*/
#ifdef DAILY_OUTPUT
   /* { "id" : D_NPP,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/d_npp.bin"}},
    { "id" : D_GPP,            "file" : { "fmt" : RAW, "name" : "../output/gswp3/d_gpp.bin"}},
    { "id" : D_RH,             "file" : { "fmt" : RAW, "name" : "../output/gswp3/d_rh.bin"}},
    { "id" : D_TRANS,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/d_trans.bin"}},
    { "id" : D_INTERC,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/d_interc.bin"}},
    { "id" : D_EVAP,           "file" : { "fmt" : RAW, "name" : "../output/gswp3/d_evap.bin"}},*/
#endif
    { "id" : PET,             "file" : { "fmt" : RAW, "name" : "output/mpet.bin"}},
   /* { "id" : ALBEDO,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/malbedo.bin"}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : RAW, "name" : "../output/gswp3/maxthaw_depth.bin"}},
    { "id" : SOILTEMP1,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/msoiltemp1.bin"}},
    { "id" : SOILTEMP2,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/msoiltemp2.bin"}},
    { "id" : SOILTEMP3,       "file" : { "fmt" : RAW, "name" : "../output/gswp3/msoiltemp3.bin"}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : RAW, "name" : "../output/gswp3/soilc_layer.bin"}},
    { "id" : AGB,              "file" : { "fmt" : RAW, "name" : "../output/gswp3/agb.bin"}},
    { "id" : AGB_TREE,         "file" : { "fmt" : RAW, "name" : "../output/gswp3/agb_tree.bin"}},
    { "id" : RETURN_FLOW_B,   "file" : { "fmt" : RAW, "name" : "../output/gswp3/mreturn_flow_b.bin"}},
    { "id" : TRANSP_B,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/mtransp_b.bin"}},
    { "id" : EVAP_B,          "file" : { "fmt" : RAW, "name" : "../output/gswp3/mevap_b.bin"}},
    { "id" : INTERC_B,        "file" : { "fmt" : RAW, "name" : "../output/gswp3/mintec_b.bin"}},
    { "id" : CONV_LOSS_EVAP,  "file" : { "fmt" : RAW, "name" : "../output/gswp3/aconv_loss_evap.bin"}},
    { "id" : CONV_LOSS_DRAIN, "file" : { "fmt" : RAW, "name" : "../output/gswp3/aconv_loss_drain.bin"}}*/
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : ALL, /* 27410, 67208 60400 47284 47293 47277 all grid cells */
  "endgrid" : ALL,

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  /*"outputyear":, /* first year output is written  */
  "restart" :  false, /* start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv5000_gswp3.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 0,   /* spinup years */
  "nspinyear" : 0,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2010, /* last year of simulation */
  "outputyear": 1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv5000_gswp3.lpj", /* filename of restart file */
  "write_restart" : false, /* create restart file */
  "write_restart_filename" : "restart/restart_2010_gswp3.lpj", /* filename of restart file */
  "restart_year": 2010 /* write restart at year */

#endif
}
