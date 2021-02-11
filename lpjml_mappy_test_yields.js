/**************************************************************************************/
/**                                                                                \n**/
/**           l  p  j  m  l  _  c  r  o  p  _  p  h  e  n  .  j  s                 \n**/
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

//#define DAILY_OUTPUT  /* enables daily output */

/*Identifiers (preprocessor only understands integer expressions in #if)*/
#define GLOBALRUN 0
#define PIXELRUN  1
#define DRUN PIXELRUN //GLOBALRUN

#if (DRUN==PIXELRUN)
  #define DAILY_OUTPUT
#endif

#define TIM_1986_2005 0
#define TIM_2080_2099 1

#define CLM_HAD 0
#define CLM_GFD 1
#define CLM_IPS 2
#define CLM_MIR 3
#define CLM_WFD 4

#define IRR_LIM 0
#define IRR_UNL 1

#define NIT_LIM 0
#define NIT_UNL 1

#define GS_SA0 0      /* reference sdate & phus (unchanged management) */
#define GS_SA1 1      /* delayed adaptation */
#define GS_SA2 2      /* complete adaptation */
#define GS_SA3 4      /* adaptation of hdate only (fixed sdate) */
#define GS_SA4 5      /* adaptation of sdate only (fixed hdate) */
#define GS_SA0_swh 6  /* reference sdate & phus for runs with spring wheat everywhere */
#define GS_OA0 7      /* observed crop calendars (ggcmi ph3) */

#define CO2_DYNA 0    /* transient CO2 */
#define CO2_STAT 1    /* static CO2 fixed at 2005 level */


/* ALL GCMs */
#define DIRR IRR_UNL   /* assuming unlimited water available for irrigation */
#define DNIT NIT_LIM   /* limited nitrogen to represent actual management */


/* Run with WFDEI climate (computed crop calendars) */
#ifdef RUN_ID_01
    #define DTIM TIM_1986_2005
    #define DCLM CLM_WFD
    #define DGS  GS_SA0
    #define DCO2_DYNA
    #define output_run  01
    #define restart_run 01
/* Run with WFDEI climate (ggcmi ph3 crop calendars) */
#elif defined(RUN_ID_02)
    #define DTIM TIM_1986_2005
    #define DCLM CLM_WFD
    #define DGS  GS_OA0
    #define DCO2_DYNA
    #define output_run  02
    #define restart_run 02
/* Run with CRU climate (ggcmi ph3 crop calendars) */
#elif defined(RUN_ID_03)
    #define DTIM TIM_1986_2005
    //#define DCLM CLM_WFD
    #define DGS  GS_OA0
    #define DCO2_DYNA
    #define output_run  03
    #define restart_run 03
#endif




{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "version"  : "5.3",       /* LPJmL version expected */
  "random_prec" : false,    /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : false,
#if (DRUN==PIXELRUN)
    "river_routing" : false, /* false set only for testing pixel runs */
#else
    "river_routing" : true,  /* river_routing true for both IRR_LIM and IRR_UNL */
#endif

  "permafrost" : true,

#if (DNIT==NIT_LIM)
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
#elif (DNIT==NIT_UNL)
  "with_nitrogen" : "unlim",
  #else
#endif

  "const_climate" : false,
  "const_deposition" : false,
  "fix_climate" : false,
  "shuffle_climate" : true,
  "store_climate" : true,
#ifdef FROM_RESTART
  "new_seed" : false,
  "equilsoil" : false,
  "population" : false,
  "landuse" : "all_crops", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS, ONLY_CROPS (crops scaled to 100% of cell) */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : true,
  "wateruse" : "yes",  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
#else
  "equilsoil" : true,
  "population" : false,
  "landuse" : "no_landuse",
  "reservoir" : false,
  "wateruse" : "no",
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "prescribed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "crop_phu_option" : "new",    /* PRESCRIBED_CROP_PHU (PHU dataset used, requires PRESCRIBED_SDATE), SEMISTATIC_CROP_PHU (LPJmL4 semi-static PHU approach) */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : "fixed_residue_remove", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */

#if (DIRR==IRR_LIM)
  "irrigation" : "lim",                 /* other options "no", "lim", "pot", "all" */
#elif (DIRR==IRR_UNL)
  "irrigation" : "pot",
#endif

  "tillage_type" : "all",      /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "till_startyear" : 1850,              /* year in which tillage should start */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "laimax_interpolate" : "laimax_par",  /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "manure_input" : true,                /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : false,             /* move PFT type others into PFT crop, maize for tropical, wheat for temperate */
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,      /* defines which grassland management scenario to use, if false scenario_MO0.bin, else change file in grassland_fixed_pft in input.js)*/
  "crop_resp_fix" : false,
  "cropsheatfrost" : true,
  "double_harvest" : true,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_mappy_test_yields.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

//#include "input_mappy_test_yields.js"    /* Input files of CRU dataset */
#include "input_nbalance_cru_test1.js"

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#ifdef WITH_GRIDBASED
  "grid_scaled" : true,
#define SUFFIX grid.bin
#else
  "grid_scaled" : false,
#define SUFFIX pft.bin
#endif

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

#if(DRUN==PIXELRUN)      /* defining directory for pixel run outputs */
    #define output_run  pixel
    #define restart_run pixel
#endif

#define workdir /p/projects/macmit/users/minoli/PROJECTS/MAPPY/LPJmL/outputs
#define output workdir/output_run
#define restart workdir/restart_run

  "crop_index" : "temperate cereals",  /* CFT for daily output */
  "crop_irrigation" : false, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" : 
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "raw", "name" : mkstr(output/grid.bin) }},
    { "id" : "fpc",              "file" : { "fmt" : "raw", "name" : mkstr(output/fpc.bin)}},
/*    { "id" : "npp",             "file" : { "fmt" : "raw", "name" : mkstr(output/mnpp.bin)}},
    { "id" : "gpp",             "file" : { "fmt" : "raw", "name" : mkstr(output/mgpp.bin)}},
    { "id" : "rh",              "file" : { "fmt" : "raw", "name" : mkstr(output/mrh.bin)}},
    { "id" : "fapar",           "file" : { "fmt" : "raw", "name" : mkstr(output/mfapar.bin)}},
    { "id" : "transp",          "file" : { "fmt" : "raw", "name" : mkstr(output/mtransp.bin)}},
    { "id" : "runoff",          "file" : { "fmt" : "raw", "name" : mkstr(output/mrunoff.bin)}},
    { "id" : "evap",            "file" : { "fmt" : "raw", "name" : mkstr(output/mevap.bin)}},
    { "id" : "interc",          "file" : { "fmt" : "raw", "name" : mkstr(output/minterc.bin)}},
    { "id" : "swc1",            "file" : { "fmt" : "raw", "name" : mkstr(output/mswc1.bin)}},
    { "id" : "swc2",            "file" : { "fmt" : "raw", "name" : mkstr(output/mswc2.bin)}},
    { "id" : "firec",            "file" : { "fmt" : "raw", "name" : mkstr(output/firec.bin)}},
    { "id" : "firef",            "file" : { "fmt" : "raw", "name" : mkstr(output/firef.bin)}},*/
    { "id" : "vegc",             "file" : { "fmt" : "raw", "name" : mkstr(output/vegc.bin)}},
//    { "id" : "soilc",            "file" : { "fmt" : "raw", "name" : mkstr(output/soilc.bin)}},
//    { "id" : "litc",             "file" : { "fmt" : "raw", "name" : mkstr(output/litc.bin)}},
//    { "id" : "flux_estabc",      "file" : { "fmt" : "raw", "name" : mkstr(output/flux_estab.bin)}},
//    { "id" : "pft_vegc",         "file" : { "fmt" : "raw", "name" : mkstr(output/pft_vegc.bin)}},
//    { "id" : "phen_tmin",       "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_tmin.bin)}},
//    { "id" : "phen_tmax",       "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_tmax.bin)}},
//    { "id" : "phen_light",      "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_light.bin)}},
//    { "id" : "phen_water",      "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_water.bin)}},
/*    { "id" : "vegn",             "file" : { "fmt" : "raw", "name" : mkstr(output/vegn.bin)}},
    { "id" : "soiln",            "file" : { "fmt" : "raw", "name" : mkstr(output/soiln.bin)}},
    { "id" : "litn",             "file" : { "fmt" : "raw", "name" : mkstr(output/litn.bin)}},
    { "id" : "soiln_layer",      "file" : { "fmt" : "raw", "name" : mkstr(output/soiln_layer.bin)}},
    { "id" : "soilno3_layer",    "file" : { "fmt" : "raw", "name" : mkstr(output/soilno3_layer.bin)}},
    { "id" : "soilnh4_layer",    "file" : { "fmt" : "raw", "name" : mkstr(output/soilnh4_layer.bin)}},
    { "id" : "soiln_slow",       "file" : { "fmt" : "raw", "name" : mkstr(output/soiln_slow.bin)}},
    { "id" : "soilnh4",          "file" : { "fmt" : "raw", "name" : mkstr(output/soilnh4.bin)}},
    { "id" : "soilno3",          "file" : { "fmt" : "raw", "name" : mkstr(output/soilno3.bin)}},*/
    { "id" : "pft_nuptake",      "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nuptake.bin)}},
/*    { "id" : "nuptake",         "file" : { "fmt" : "raw", "name" : mkstr(output/mnuptake.bin)}},
    { "id" : "leaching",        "file" : { "fmt" : "raw", "name" : mkstr(output/mleaching.bin)}},
    { "id" : "n2o_denit",       "file" : { "fmt" : "raw", "name" : mkstr(output/mn2o_denit.bin)}},
    { "id" : "n2o_nit",         "file" : { "fmt" : "raw", "name" : mkstr(output/mn2o_nit.bin)}},
    { "id" : "n2_emis",         "file" : { "fmt" : "raw", "name" : mkstr(output/mn2_emis.bin)}},
    { "id" : "bnf",             "file" : { "fmt" : "raw", "name" : mkstr(output/mbnf.bin)}},
    { "id" : "n_immo",          "file" : { "fmt" : "raw", "name" : mkstr(output/mn_immo.bin)}},*/
    { "id" : "pft_ndemand",      "file" : { "fmt" : "raw", "name" : mkstr(output/pft_ndemand.bin)}},
/*    { "id" : "firen",            "file" : { "fmt" : "raw", "name" : mkstr(output/firen.bin)}},
    { "id" : "n_mineralization","file" : { "fmt" : "raw", "name" : mkstr(output/mn_mineralization.bin)}},
    { "id" : "n_volatilization","file" : { "fmt" : "raw", "name" : mkstr(output/mn_volatilization.bin)}},*/
    { "id" : "pft_nlimit",       "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nlimit.bin)}},
//    { "id" : "pft_vegn",         "file" : { "fmt" : "raw", "name" : mkstr(output/pft_vegn.bin)}},
    { "id" : "pft_cleaf",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_cleaf.bin)}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nleaf.bin)}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "raw", "name" : mkstr(output/pft_laimax.bin)}},
    { "id" : "pft_croot",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_croot.bin)}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nroot.bin)}},
//    { "id" : "pft_csapw,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_csapw.bin)}},
//    { "id" : "pft_nsapw,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nsapw.bin)}},
//    { "id" : "pft_chawo,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_chawo.bin)}},
//    { "id" : "pft_nhawo,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nhawo.bin)}},
#ifdef WITH_SPITFIRE
    { "id" : "firec",           "file" : { "fmt" : "raw", "timestep" : "monthly" : "unit" : "gC/m2/month", "name" : "output/mfirec.bin"}},
    { "id" : "nfire",           "file" : { "fmt" : "raw", "name" : mkstr(output/mnfire.bin)}},
    { "id" : "burntarea",       "file" : { "fmt" : "raw", "name" : mkstr(output/mburnt_area.bin)}},
    { "id" : "burntarea",       "file" : { "fmt" : "raw", "name" : mkstr(output/aburnt_area.bin)}},
#endif
    { "id" : "discharge",       "file" : { "fmt" : "raw", "name" : mkstr(output/mdischarge.bin)}},
//    { "id" : "wateramount",     "file" : { "fmt" : "raw", "name" : mkstr(output/mwateramount.bin)}},
    { "id" : "syear",            "file" : { "fmt" : RAW, "name" : mkstr(output/syear.bin)}},
    { "id" : "syear2",           "file" : { "fmt" : RAW, "name" : mkstr(output/syear2.bin)}},
    { "id" : "harvestc",         "file" : { "fmt" : "raw", "name" : mkstr(output/flux_harvestc.bin)}},
    { "id" : "harvestn",         "file" : { "fmt" : "raw", "name" : mkstr(output/flux_harvestn.bin)}},
    { "id" : "sdate",            "file" : { "fmt" : "raw", "name" : mkstr(output/sdate.bin)}},
    { "id" : "sdate2",           "file" : { "fmt" : "raw", "name" : mkstr(output/sdate2.bin)}},
    { "id" : "hdate",            "file" : { "fmt" : "raw", "name" : mkstr(output/hdate.bin)}},
    { "id" : "hdate2",           "file" : { "fmt" : "raw", "name" : mkstr(output/hdate.bin)}},
    { "id" : "growing_period",   "file" : { "fmt" : "raw", "name" : mkstr(output/growing_period.bin)}},
    { "id" : "growing_period2",  "file" : { "fmt" : "raw", "name" : mkstr(output/growing_period2.bin)}},
    { "id" : "husum",            "file" : { "fmt" : "raw", "name" : mkstr(output/husum.bin)}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvestc.SUFFIX)}},
    { "id" : "pft_harvestc2",    "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvestc2.SUFFIX)}},
    { "id" : "pft_harvestn",     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvestn.SUFFIX)}},
    { "id" : "pft_harvestn2",    "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvestn2.SUFFIX)}},
    { "id" : "cft_airrig",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_airrig.SUFFIX)}},
    { "id" : "cft_pet",          "file" : { "fmt" : "raw", "name" : mkstr(output/cft_pet.SUFFIX)}},
    { "id" : "cft_transp",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_transp.SUFFIX)}},
    { "id" : "cft_evap",         "file" : { "fmt" : "raw", "name" : mkstr(output/cft_evap.SUFFIX)}},
    { "id" : "cft_interc",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_interc.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "raw", "name" : mkstr(output/cftfrac.bin)}},
    { "id" : "seasonality",      "file" : { "fmt" : "raw", "name" : mkstr(output/seasonality.bin)}},
#ifdef DAILY_OUTPUT
    { "id" : "d_npp",            "file" : { "fmt" : "raw", "name" : mkstr(output/d_npp.bin)}},
    { "id" : "d_gpp",            "file" : { "fmt" : "raw", "name" : mkstr(output/d_gpp.bin)}},
    { "id" : "d_rh",             "file" : { "fmt" : "raw", "name" : mkstr(output/d_rh.bin)}},
    { "id" : "d_trans",          "file" : { "fmt" : "raw", "name" : mkstr(output/d_trans.bin)}},
    { "id" : "d_interc",         "file" : { "fmt" : "raw", "name" : mkstr(output/d_interc.bin)}},
    { "id" : "d_evap",           "file" : { "fmt" : "raw", "name" : mkstr(output/d_evap.bin)}},
    { "id" : "d_lai",            "file" : { "fmt" : "raw", "name" : mkstr(output/d_lai.bin)}},
    { "id" : "d_cleaf",          "file" : { "fmt" : "raw", "name" : mkstr(output/d_cleaf.bin)}},
    { "id" : "d_croot",          "file" : { "fmt" : "raw", "name" : mkstr(output/d_croot.bin)}},
    { "id" : "d_cso",            "file" : { "fmt" : "raw", "name" : mkstr(output/d_cso.bin)}},
    { "id" : "d_cpool",          "file" : { "fmt" : "raw", "name" : mkstr(output/d_cpool.bin)}},
    { "id" : "d_husum",          "file" : { "fmt" : "raw", "name" : mkstr(output/d_husum.bin)}},
    { "id" : "d_vdsum",          "file" : { "fmt" : "raw", "name" : mkstr(output/d_vdsum.bin)}}
#endif
/*    { "id" : "pet",             "file" : { "fmt" : "raw", "name" : "output/mpet.bin"}},
    { "id" : "albedo",          "file" : { "fmt" : "raw", "name" : "output/malbedo.bin"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "raw", "name" : "output/maxthaw_depth.bin"}},
    { "id" : "soiltemp1",       "file" : { "fmt" : "raw", "name" : "output/msoiltemp1.bin"}},
    { "id" : "soiltemp2",       "file" : { "fmt" : "raw", "name" : "output/msoiltemp2.bin"}},
    { "id" : "soiltemp3",       "file" : { "fmt" : "raw", "name" : "output/msoiltemp3.bin"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "raw", "name" : "output/soilc_layer.bin"}},
    { "id" : "agb",              "file" : { "fmt" : "raw", "name" : "output/agb.bin"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "raw", "name" : "output/agb_tree.bin"}},
    { "id" : "return_flow_b",   "file" : { "fmt" : "raw", "name" : "output/mreturn_flow_b.bin"}},
    { "id" : "transp_b",        "file" : { "fmt" : "raw", "name" : "output/mtransp_b.bin"}},
    { "id" : "evap_b",          "file" : { "fmt" : "raw", "name" : "output/mevap_b.bin"}},
    { "id" : "interc_b",        "file" : { "fmt" : "raw", "name" : "output/mintec_b.bin"}},
    { "id" : "conv_loss_evap",  "file" : { "fmt" : "raw", "name" : "output/aconv_loss_evap.bin"}},
    { "id" : "conv_loss_drain", "file" : { "fmt" : "raw", "name" : "output/aconv_loss_drain.bin"}}*/
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/
#if (DRUN==PIXELRUN)
  "startgrid" :  7187, //7187, //9353, //7223, //7175, //7212, //30115, /*31374, 27410, 67208 60400 all grid cells */
  "endgrid" :    7187, //7187, //9353, //7223, //7234, //7212, //30115,
#else
  "startgrid" :  "all", /*31374, 27410, 67208 60400 all grid cells */
#endif

#ifdef CHECKPOINT
  "checkpoint_filename" : mkstr(output/restart_checkpoint.lpj), /* filename of checkpoint file */
#endif


#ifndef FROM_RESTART

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1979, /* first year of simulation */
  "lastyear" : 1979, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : mkstr(output/restart_1861_nv_stdfire.lpj), /* filename of restart file */ /*restart_1861_nv_stdfire_rrouting_true.lpj*/
  "restart_year": 1919 /* write restart at year */

#else

 #if (DRUN==PIXELRUN)
  "nspinup" : 60,   /* spinup years */
 #else
  "nspinup" : 390,   /* spinup years */
 #endif
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1979, /* first year of simulation */
  "lastyear" : 2012, /* last year of simulation */
  "outputyear": 1979, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : mkstr(restart/restart_1861_nv_stdfire.lpj), /* filename of restart file */
  "write_restart" : false, /* create restart file */
  "write_restart_filename" : mkstr(output/restart_2005_crop_stdfire.lpj), /* filename of restart file */
  "restart_year": 2005 /* write restart at year */

#endif /* FROM_RESTART */

}
