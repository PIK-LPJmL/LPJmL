/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.2.001                         \n**/
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
    #define DCLM CLM_WFD
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
  "sim_id"   : "lpjml",       /* LPJML Simulation type with managed land use */
  "version"  : "5.3",       /* LPJmL version expected */
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
#if (DRUN==PIXELRUN)
    "river_routing" : false, /* false set only for testing pixel runs */
#else
    "river_routing" : false,  /* river_routing true for both IRR_LIM and IRR_UNL */
#endif

  "permafrost" : true,

#if (DNIT==NIT_LIM)
  "with_nitrogen" : "lim", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
#elif (DNIT==NIT_UNL)
  "with_nitrogen" : "unlim",
  #else
#endif

  "const_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "population" : false,
  "landuse" : "all_crops", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS, ONLY_CROPS (crops scaled to 100% of cell) */
  "landuse_year_const" : 2000, /* set landuse year for CONST_LANDUSE case */
  "reservoir" : false,
  "wateruse" : "no",  /* other options: NO_WATERUSE, WATERUSE, ALL_WATERUSE */
  "equilsoil" : false,
#else
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
  "equilsoil" : true,
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : NO_LANDCOVER, /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : NO_FIXED_SDATE, //PRESCRIBED_SDATE,   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "crop_phu_option" : "new", //PRESCRIBED_CROP_PHU,    /* PRESCRIBED_CROP_PHU (PHU dataset used, requires PRESCRIBED_SDATE), SEMISTATIC_CROP_PHU (LPJmL4 semi-static PHU approach) */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : FIXED_RESIDUE_REMOVE, /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */

#if (DIRR==IRR_LIM)
  "irrigation" : "lim",        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
#elif (DIRR==IRR_UNL)
  "irrigation" : "pot",
#endif

  "tillage_type" : "all",          /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "laimax_interpolate" : "laimax_par",    /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "manure_input" : true,               /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : false,             /* move PFT type others into PFT crop, maize for tropical, wheat for temperate */
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,      /* defines which grassland management scenario to use, if false scenario_MO0.bin, else change file in grassland_fixed_pft in input.js)*/
  "prescribe_lsuha" : false,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_mappy_test_yields.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_mappy_test_highres_grid.js"    /* Input files of CRU dataset */
//#include "input_nbalance_cru_test1.js"

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

#if(DRUN==PIXELRUN)      /* defining directory for pixel run outputs */
    #define output_run  pixel
    #define restart_run pixel
#endif

#define workdir /p/projects/macmit/users/minoli/PROJECTS/MAPPY/LPJmL/outputs
#define output workdir/output_run
#define restart workdir/restart_run

  "crop_index" : TEMPERATE_CEREALS,  /* CFT for daily output */
  "crop_irrigation" : DAILY_RAINFED, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" : 
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : GRID,             "file" : { "fmt" : RAW, "name" : mkstr(output/grid.bin) }},
    { "id" : FPC,              "file" : { "fmt" : RAW, "name" : mkstr(output/fpc.bin)}},
/*    { "id" : MNPP,             "file" : { "fmt" : RAW, "name" : mkstr(output/mnpp.bin)}},
    { "id" : MGPP,             "file" : { "fmt" : RAW, "name" : mkstr(output/mgpp.bin)}},
    { "id" : MRH,              "file" : { "fmt" : RAW, "name" : mkstr(output/mrh.bin)}},
    { "id" : MFAPAR,           "file" : { "fmt" : RAW, "name" : mkstr(output/mfapar.bin)}},
    { "id" : MTRANSP,          "file" : { "fmt" : RAW, "name" : mkstr(output/mtransp.bin)}},
    { "id" : MRUNOFF,          "file" : { "fmt" : RAW, "name" : mkstr(output/mrunoff.bin)}},
    { "id" : MEVAP,            "file" : { "fmt" : RAW, "name" : mkstr(output/mevap.bin)}},
    { "id" : MINTERC,          "file" : { "fmt" : RAW, "name" : mkstr(output/minterc.bin)}},
    { "id" : MSWC1,            "file" : { "fmt" : RAW, "name" : mkstr(output/mswc1.bin)}},
    { "id" : MSWC2,            "file" : { "fmt" : RAW, "name" : mkstr(output/mswc2.bin)}},
    { "id" : FIREC,            "file" : { "fmt" : RAW, "name" : mkstr(output/firec.bin)}},
    { "id" : FIREF,            "file" : { "fmt" : RAW, "name" : mkstr(output/firef.bin)}},*/
    { "id" : VEGC,             "file" : { "fmt" : RAW, "name" : mkstr(output/vegc.bin)}},
//    { "id" : SOILC,            "file" : { "fmt" : RAW, "name" : mkstr(output/soilc.bin)}},
//    { "id" : LITC,             "file" : { "fmt" : RAW, "name" : mkstr(output/litc.bin)}},
//    { "id" : FLUX_ESTABC,      "file" : { "fmt" : RAW, "name" : mkstr(output/flux_estab.bin)}},
//    { "id" : PFT_VEGC,         "file" : { "fmt" : RAW, "name" : mkstr(output/pft_vegc.bin)}},
//    { "id" : MPHEN_TMIN,       "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_tmin.bin)}},
//    { "id" : MPHEN_TMAX,       "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_tmax.bin)}},
//    { "id" : MPHEN_LIGHT,      "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_light.bin)}},
//    { "id" : MPHEN_WATER,      "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_water.bin)}},
/*    { "id" : VEGN,             "file" : { "fmt" : RAW, "name" : mkstr(output/vegn.bin)}},
    { "id" : SOILN,            "file" : { "fmt" : RAW, "name" : mkstr(output/soiln.bin)}},
    { "id" : LITN,             "file" : { "fmt" : RAW, "name" : mkstr(output/litn.bin)}},
    { "id" : SOILN_LAYER,      "file" : { "fmt" : RAW, "name" : mkstr(output/soiln_layer.bin)}},
    { "id" : SOILNO3_LAYER,    "file" : { "fmt" : RAW, "name" : mkstr(output/soilno3_layer.bin)}},
    { "id" : SOILNH4_LAYER,    "file" : { "fmt" : RAW, "name" : mkstr(output/soilnh4_layer.bin)}},
    { "id" : SOILN_SLOW,       "file" : { "fmt" : RAW, "name" : mkstr(output/soiln_slow.bin)}},
    { "id" : SOILNH4,          "file" : { "fmt" : RAW, "name" : mkstr(output/soilnh4.bin)}},
    { "id" : SOILNO3,          "file" : { "fmt" : RAW, "name" : mkstr(output/soilno3.bin)}},*/
    { "id" : PFT_NUPTAKE,      "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nuptake.bin)}},
/*    { "id" : MNUPTAKE,         "file" : { "fmt" : RAW, "name" : mkstr(output/mnuptake.bin)}},
    { "id" : MLEACHING,        "file" : { "fmt" : RAW, "name" : mkstr(output/mleaching.bin)}},
    { "id" : MN2O_DENIT,       "file" : { "fmt" : RAW, "name" : mkstr(output/mn2o_denit.bin)}},
    { "id" : MN2O_NIT,         "file" : { "fmt" : RAW, "name" : mkstr(output/mn2o_nit.bin)}},
    { "id" : MN2_EMIS,         "file" : { "fmt" : RAW, "name" : mkstr(output/mn2_emis.bin)}},
    { "id" : MBNF,             "file" : { "fmt" : RAW, "name" : mkstr(output/mbnf.bin)}},
    { "id" : MN_IMMO,          "file" : { "fmt" : RAW, "name" : mkstr(output/mn_immo.bin)}},*/
    { "id" : PFT_NDEMAND,      "file" : { "fmt" : RAW, "name" : mkstr(output/pft_ndemand.bin)}},
/*    { "id" : FIREN,            "file" : { "fmt" : RAW, "name" : mkstr(output/firen.bin)}},
    { "id" : MN_MINERALIZATION,"file" : { "fmt" : RAW, "name" : mkstr(output/mn_mineralization.bin)}},
    { "id" : MN_VOLATILIZATION,"file" : { "fmt" : RAW, "name" : mkstr(output/mn_volatilization.bin)}},*/
    { "id" : PFT_NLIMIT,       "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nlimit.bin)}},
//    { "id" : PFT_VEGN,         "file" : { "fmt" : RAW, "name" : mkstr(output/pft_vegn.bin)}},
    { "id" : PFT_CLEAF,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_cleaf.bin)}},
    { "id" : PFT_NLEAF,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nleaf.bin)}},
    { "id" : PFT_LAIMAX,       "file" : { "fmt" : RAW, "name" : mkstr(output/pft_laimax.bin)}},
    { "id" : PFT_CROOT,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_croot.bin)}},
    { "id" : PFT_NROOT,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nroot.bin)}},
//    { "id" : PFT_CSAPW,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_csapw.bin)}},
//    { "id" : PFT_NSAPW,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nsapw.bin)}},
//    { "id" : PFT_CHAWO,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_chawo.bin)}},
//    { "id" : PFT_NHAWO,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nhawo.bin)}},
#ifdef WITH_SPITFIRE
    { "id" : FIREC,           "file" : { "fmt" : RAW, "name" : mkstr(output/mfirec.bin)}},
    { "id" : NFIRE,           "file" : { "fmt" : RAW, "name" : mkstr(output/mnfire.bin)}},
    { "id" : BURNTAREA,       "file" : { "fmt" : RAW, "name" : mkstr(output/aburnt_area.bin)}},
#endif
    { "id" : DISCHARGE,       "file" : { "fmt" : RAW, "name" : mkstr(output/mdischarge.bin)}},
//    { "id" : MWATERAMOUNT,     "file" : { "fmt" : RAW, "name" : mkstr(output/mwateramount.bin)}},
    { "id" : SYEAR,            "file" : { "fmt" : RAW, "name" : mkstr(output/syear.bin)}},
    { "id" : SYEAR2,           "file" : { "fmt" : RAW, "name" : mkstr(output/syear2.bin)}},
    { "id" : HARVESTC,         "file" : { "fmt" : RAW, "name" : mkstr(output/flux_harvestc.bin)}},
    { "id" : HARVESTN,         "file" : { "fmt" : RAW, "name" : mkstr(output/flux_harvestn.bin)}},
    { "id" : SDATE,            "file" : { "fmt" : RAW, "name" : mkstr(output/sdate.bin)}},
    { "id" : SDATE2,           "file" : { "fmt" : RAW, "name" : mkstr(output/sdate2.bin)}},
    { "id" : HDATE,            "file" : { "fmt" : RAW, "name" : mkstr(output/hdate.bin)}},
    { "id" : HDATE2,           "file" : { "fmt" : RAW, "name" : mkstr(output/hdate2.bin)}},
    { "id" : GROWING_PERIOD,   "file" : { "fmt" : RAW, "name" : mkstr(output/growing_period.bin)}},
    { "id" : GROWING_PERIOD2,  "file" : { "fmt" : RAW, "name" : mkstr(output/growing_period2.bin)}},
    { "id" : HUSUM,            "file" : { "fmt" : RAW, "name" : mkstr(output/husum.bin)}},
    { "id" : HUSUM2,           "file" : { "fmt" : RAW, "name" : mkstr(output/husum2.bin)}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvestc.SUFFIX)}},
    { "id" : PFT_HARVESTC2,    "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvestc2.SUFFIX)}},
    { "id" : PFT_HARVESTN,     "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvestn.SUFFIX)}},
    { "id" : PFT_HARVESTN2,    "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvestn2.SUFFIX)}},
    { "id" : CFT_AIRRIG,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_airrig.SUFFIX)}},
    { "id" : CFT_PET,          "file" : { "fmt" : RAW, "name" : mkstr(output/cft_pet.SUFFIX)}},
    { "id" : CFT_TRANSP,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_transp.SUFFIX)}},
    { "id" : CFT_EVAP,         "file" : { "fmt" : RAW, "name" : mkstr(output/cft_evap.SUFFIX)}},
    { "id" : CFT_INTERC,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_interc.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : RAW, "name" : mkstr(output/cftfrac.bin)}},
    { "id" : SEASONALITY,      "file" : { "fmt" : RAW, "name" : mkstr(output/seasonality.bin)}},
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : RAW, "name" : mkstr(output/d_npp.bin)}},
    { "id" : D_GPP,            "file" : { "fmt" : RAW, "name" : mkstr(output/d_gpp.bin)}},
    { "id" : D_RH,             "file" : { "fmt" : RAW, "name" : mkstr(output/d_rh.bin)}},
    { "id" : D_TRANS,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_trans.bin)}},
    { "id" : D_INTERC,         "file" : { "fmt" : RAW, "name" : mkstr(output/d_interc.bin)}},
    { "id" : D_EVAP,           "file" : { "fmt" : RAW, "name" : mkstr(output/d_evap.bin)}},
    { "id" : D_LAI,            "file" : { "fmt" : RAW, "name" : mkstr(output/d_lai.bin)}},
    { "id" : D_CLEAF,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_cleaf.bin)}},
    { "id" : D_CROOT,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_croot.bin)}},
    { "id" : D_CSO,            "file" : { "fmt" : RAW, "name" : mkstr(output/d_cso.bin)}},
    { "id" : D_CPOOL,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_cpool.bin)}},
    { "id" : D_HUSUM,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_husum.bin)}},
    { "id" : D_VDSUM,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_vdsum.bin)}}
#endif
/*    { "id" : PET,             "file" : { "fmt" : RAW, "name" : "output/mpet.bin"}},
    { "id" : ALBEDO,          "file" : { "fmt" : RAW, "name" : "output/malbedo.bin"}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : RAW, "name" : "output/maxthaw_depth.bin"}},
    { "id" : SOILTEMP1,       "file" : { "fmt" : RAW, "name" : "output/msoiltemp1.bin"}},
    { "id" : SOILTEMP2,       "file" : { "fmt" : RAW, "name" : "output/msoiltemp2.bin"}},
    { "id" : SOILTEMP3,       "file" : { "fmt" : RAW, "name" : "output/msoiltemp3.bin"}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : RAW, "name" : "output/soilc_layer.bin"}},
    { "id" : AGB,              "file" : { "fmt" : RAW, "name" : "output/agb.bin"}},
    { "id" : AGB_TREE,         "file" : { "fmt" : RAW, "name" : "output/agb_tree.bin"}},
    { "id" : RETURN_FLOW_B,   "file" : { "fmt" : RAW, "name" : "output/mreturn_flow_b.bin"}},
    { "id" : TRANSP_B,        "file" : { "fmt" : RAW, "name" : "output/mtransp_b.bin"}},
    { "id" : EVAP_B,          "file" : { "fmt" : RAW, "name" : "output/mevap_b.bin"}},
    { "id" : INTERC_B,        "file" : { "fmt" : RAW, "name" : "output/mintec_b.bin"}},
    { "id" : CONV_LOSS_EVAP,  "file" : { "fmt" : RAW, "name" : "output/aconv_loss_evap.bin"}},
    { "id" : CONV_LOSS_DRAIN, "file" : { "fmt" : RAW, "name" : "output/aconv_loss_drain.bin"}}*/
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/
#if (DRUN==PIXELRUN)
  "startgrid" :  0, /*28861 (Potsdam); 27509 (Baden-W.), 7187 (US)*/
  "endgrid" :    0,
#else
  "startgrid" :  ALL, /*31374, 27410, 67208 60400 all grid cells */
  "endgrid" :    ALL,
#endif

#ifdef CHECKPOINT
  "checkpoint_filename" : mkstr(output/restart_checkpoint.lpj), /* filename of checkpoint file */
#endif


#ifndef FROM_RESTART

  "nspinup" : 500,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1980, /* first year of simulation */
  "lastyear" : 1980, /* last year of simulation */
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
  "firstyear": 1980, /* first year of simulation */
  "lastyear" : 2012, /* last year of simulation */
  "outputyear": 1980, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : mkstr(restart/restart_1861_nv_stdfire.lpj), /* filename of restart file */
  "write_restart" : false, /* create restart file */
  "write_restart_filename" : mkstr(output/restart_2005_crop_stdfire.lpj), /* filename of restart file */
  "restart_year": 2005 /* write restart at year */

#endif /* FROM_RESTART */

}
