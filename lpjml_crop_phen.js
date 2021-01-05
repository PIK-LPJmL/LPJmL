/**************************************************************************************/
/**                                                                                \n**/
/**           l  p  j  m  l  _  c  r  o  p  _  p  h  e  n  .  j  s                 \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.2.002                         \n**/
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

/* ifdefs for CROP_PHENOLOGY_v01 runs*/

/*Identifiers (preprocessor only understands integer expressions in #if)*/
#define GLOBALRUN 0
#define PIXELRUN  1
#define DRUN GLOBALRUN

#if (DRUN==PIXELRUN)
  #define DAILY_OUTPUT
#endif

#define TIM_1986_2005 0
#define TIM_2080_2099 1

#define CLM_HAD 0
#define CLM_GFD 1
#define CLM_IPS 2
#define CLM_MIR 3

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

#define CO2_DYNA 0    /* transient CO2 */
#define CO2_STAT 1    /* static CO2 fixed at 2005 level */


/* ALL GCMs */
#define DIRR IRR_UNL   /* assuming unlimited water available for irrigation */
#define DNIT NIT_LIM   /* limited nitrogen to represent actual management */

/* HadGEM2-ES */
#ifdef RUN_ID_01
    #define DTIM TIM_1986_2005
    #define DCLM CLM_HAD
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  01
    #define restart_run 01
#elif defined(RUN_ID_02)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  02
    #define restart_run 01
#elif defined(RUN_ID_03)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA1
    #define DCO2 CO2_DYNA
    #define output_run  03
    #define restart_run 01
#elif defined(RUN_ID_04)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA2
    #define DCO2 CO2_DYNA
    #define output_run  04
    #define restart_run 01
#elif defined(RUN_ID_05)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA0
    #define DCO2 CO2_STAT
    #define output_run  05
    #define restart_run 01
#elif defined(RUN_ID_06)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA1
    #define DCO2 CO2_STAT
    #define output_run  06
    #define restart_run 01
#elif defined(RUN_ID_07)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA2
    #define DCO2 CO2_STAT
    #define output_run  07
    #define restart_run 01

 /* GFDL-ESM2M */
#elif RUN_ID_08
    #define DTIM TIM_1986_2005
    #define DCLM CLM_GFD
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  08
    #define restart_run 08
#elif defined(RUN_ID_09)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  09
    #define restart_run 08
#elif defined(RUN_ID_10)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA1
    #define DCO2 CO2_DYNA
    #define output_run  10
    #define restart_run 08
#elif defined(RUN_ID_11)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA2
    #define DCO2 CO2_DYNA
    #define output_run  11
    #define restart_run 08
#elif defined(RUN_ID_12)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA0
    #define DCO2 CO2_STAT
    #define output_run  12
    #define restart_run 08
#elif defined(RUN_ID_13)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA1
    #define DCO2 CO2_STAT
    #define output_run  13
    #define restart_run 08
#elif defined(RUN_ID_14)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA2
    #define DCO2 CO2_STAT
    #define output_run  14
    #define restart_run 08

/* IPSL-CM5A-LR */
#elif RUN_ID_15
    #define DTIM TIM_1986_2005
    #define DCLM CLM_IPS
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define restart_run 15
    #define output_run  15
#elif defined(RUN_ID_16)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  16
    #define restart_run 15
#elif defined(RUN_ID_17)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA1
    #define DCO2 CO2_DYNA
    #define output_run  17
    #define restart_run 15
#elif defined(RUN_ID_18)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA2
    #define DCO2 CO2_DYNA
    #define output_run  18
    #define restart_run 15
#elif defined(RUN_ID_19)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA0
    #define DCO2 CO2_STAT
    #define output_run  19
    #define restart_run 15
#elif defined(RUN_ID_20)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA1
    #define DCO2 CO2_STAT
    #define output_run  20
    #define restart_run 15
#elif defined(RUN_ID_21)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA2
    #define DCO2 CO2_STAT
    #define output_run  21
    #define restart_run 15

 /* MIROC5 */
#elif RUN_ID_22
    #define DTIM TIM_1986_2005
    #define DCLM CLM_MIR
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  22
    #define restart_run 22
#elif defined(RUN_ID_23)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA0
    #define DCO2 CO2_DYNA
    #define output_run  23
    #define restart_run 22
#elif defined(RUN_ID_24)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA1
    #define DCO2 CO2_DYNA
    #define output_run  24
    #define restart_run 22
#elif defined(RUN_ID_25)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA2
    #define DCO2 CO2_DYNA
    #define output_run  25
    #define restart_run 22
#elif defined(RUN_ID_26)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA0
    #define DCO2 CO2_STAT
    #define output_run  26
    #define restart_run 22
#elif defined(RUN_ID_27)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA1
    #define DCO2 CO2_STAT
    #define output_run  27
    #define restart_run 22
#elif defined(RUN_ID_28)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA2
    #define DCO2 CO2_STAT
    #define output_run  28
    #define restart_run 22

/* RUNS with fixed sdate or hdate */
#elif defined(RUN_ID_29)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA3
    #define DCO2 CO2_DYNA
    #define output_run  29
    #define restart_run 01
#elif defined(RUN_ID_30)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_HAD
    #define DGS  GS_SA4
    #define DCO2 CO2_DYNA
    #define output_run  30
    #define restart_run 01

#elif defined(RUN_ID_31) 
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA3
    #define DCO2 CO2_DYNA
    #define output_run  31
    #define restart_run 08
#elif defined(RUN_ID_32)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_GFD
    #define DGS  GS_SA4
    #define DCO2 CO2_DYNA
    #define output_run  32
    #define restart_run 08

#elif defined(RUN_ID_33)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA3
    #define DCO2 CO2_DYNA
    #define output_run  33
    #define restart_run 15
#elif defined(RUN_ID_34)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_IPS
    #define DGS  GS_SA4
    #define DCO2 CO2_DYNA
    #define output_run  34
    #define restart_run 15

#elif defined(RUN_ID_35)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA3
    #define DCO2 CO2_DYNA
    #define output_run  35
    #define restart_run 22
#elif defined(RUN_ID_36)
    #define DTIM TIM_2080_2099
    #define DCLM CLM_MIR
    #define DGS  GS_SA4
    #define DCO2 CO2_DYNA
    #define output_run  36
    #define restart_run 22

#elif defined(RUN_ID_37)
    #define DTIM TIM_1986_2005
    #define DCLM CLM_HAD
    #define DGS  GS_SA0_swh
    #define DCO2 CO2_DYNA
    #define output_run  37
    #define restart_run 01

#endif




{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",       /* LPJML Simulation type with managed land use */
  "version"  : "5.2",       /* LPJmL version expected */
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
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
  "with_nitrogen" : "lim_nitrogen", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
#elif (DNIT==NIT_UNL)
  "with_nitrogen" : "unlim_nitrogen",
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
  "landuse" : "const_landuse", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS, ONLY_CROPS (crops scaled to 100% of cell) */
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
  "sowing_date_option" : "prescribed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "crop_phu_option" : true,    /* PRESCRIBED_CROP_PHU (PHU dataset used, requires PRESCRIBED_SDATE), SEMISTATIC_CROP_PHU (LPJmL4 semi-static PHU approach) */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : "read_residue_data", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */

#if (DIRR==IRR_LIM)
  "irrigation" : "lim_irrigation",        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
#elif (DIRR==IRR_UNL)
  "irrigation" : "pot_irrigation",
#endif

  "tillage_type" : "read_tillage",          /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
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
  "crop_resp_fix" : false,
  "cropsheatfrost" : true,

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_crop_phen.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_crop_phen.js"    /* Input files of CRU dataset */

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

#define workdir /p/projects/macmit/users/minoli/PROJECTS/CROP_PHENOLOGY_v01/LPJmL/outputs /* global runs output main directory */
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
/*    { "id" : NPP,             "file" : { "fmt" : "raw", "name" : mkstr(output/mnpp.bin)}},
    { "id" : GPP,             "file" : { "fmt" : "raw", "name" : mkstr(output/mgpp.bin)}},
    { "id" : RH,              "file" : { "fmt" : "raw", "name" : mkstr(output/mrh.bin)}},
    { "id" : FAPAR,           "file" : { "fmt" : "raw", "name" : mkstr(output/mfapar.bin)}},
    { "id" : TRANSP,          "file" : { "fmt" : "raw", "name" : mkstr(output/mtransp.bin)}},
    { "id" : RUNOFF,          "file" : { "fmt" : "raw", "name" : mkstr(output/mrunoff.bin)}},
    { "id" : EVAP,            "file" : { "fmt" : "raw", "name" : mkstr(output/mevap.bin)}},
    { "id" : INTERC,          "file" : { "fmt" : "raw", "name" : mkstr(output/minterc.bin)}},
    { "id" : SWC1,            "file" : { "fmt" : "raw", "name" : mkstr(output/mswc1.bin)}},
    { "id" : SWC2,            "file" : { "fmt" : "raw", "name" : mkstr(output/mswc2.bin)}},
    { "id" : FIREC,            "file" : { "fmt" : "raw", "name" : mkstr(output/firec.bin)}},
    { "id" : FIREF,            "file" : { "fmt" : "raw", "name" : mkstr(output/firef.bin)}},*/
    { "id" : "vegc",             "file" : { "fmt" : "raw", "name" : mkstr(output/vegc.bin)}},
//    { "id" : SOILC,            "file" : { "fmt" : "raw", "name" : mkstr(output/soilc.bin)}},
//    { "id" : LITC,             "file" : { "fmt" : "raw", "name" : mkstr(output/litc.bin)}},
//    { "id" : FLUX_ESTABC,      "file" : { "fmt" : "raw", "name" : mkstr(output/flux_estab.bin)}},
//    { "id" : PFT_VEGC,         "file" : { "fmt" : "raw", "name" : mkstr(output/pft_vegc.bin)}},
//    { "id" : MPHEN_TMIN,       "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_tmin.bin)}},
//    { "id" : MPHEN_TMAX,       "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_tmax.bin)}},
//    { "id" : MPHEN_LIGHT,      "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_light.bin)}},
//    { "id" : MPHEN_WATER,      "file" : { "fmt" : "raw", "name" : mkstr(output/mphen_water.bin)}},
/*    { "id" : VEGN,             "file" : { "fmt" : "raw", "name" : mkstr(output/vegn.bin)}},
    { "id" : SOILN,            "file" : { "fmt" : "raw", "name" : mkstr(output/soiln.bin)}},
    { "id" : LITN,             "file" : { "fmt" : "raw", "name" : mkstr(output/litn.bin)}},
    { "id" : SOILN_LAYER,      "file" : { "fmt" : "raw", "name" : mkstr(output/soiln_layer.bin)}},
    { "id" : SOILNO3_LAYER,    "file" : { "fmt" : "raw", "name" : mkstr(output/soilno3_layer.bin)}},
    { "id" : SOILNH4_LAYER,    "file" : { "fmt" : "raw", "name" : mkstr(output/soilnh4_layer.bin)}},
    { "id" : SOILN_SLOW,       "file" : { "fmt" : "raw", "name" : mkstr(output/soiln_slow.bin)}},
    { "id" : SOILNH4,          "file" : { "fmt" : "raw", "name" : mkstr(output/soilnh4.bin)}},
    { "id" : SOILNO3,          "file" : { "fmt" : "raw", "name" : mkstr(output/soilno3.bin)}},*/
    { "id" : "pft_nuptake",      "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nuptake.bin)}},
/*    { "id" : MNUPTAKE,         "file" : { "fmt" : "raw", "name" : mkstr(output/mnuptake.bin)}},
    { "id" : MLEACHING,        "file" : { "fmt" : "raw", "name" : mkstr(output/mleaching.bin)}},
    { "id" : MN2O_DENIT,       "file" : { "fmt" : "raw", "name" : mkstr(output/mn2o_denit.bin)}},
    { "id" : MN2O_NIT,         "file" : { "fmt" : "raw", "name" : mkstr(output/mn2o_nit.bin)}},
    { "id" : MN2_EMIS,         "file" : { "fmt" : "raw", "name" : mkstr(output/mn2_emis.bin)}},
    { "id" : MBNF,             "file" : { "fmt" : "raw", "name" : mkstr(output/mbnf.bin)}},
    { "id" : MN_IMMO,          "file" : { "fmt" : "raw", "name" : mkstr(output/mn_immo.bin)}},*/
    { "id" : "pft_ndemand",      "file" : { "fmt" : "raw", "name" : mkstr(output/pft_ndemand.bin)}},
/*    { "id" : FIREN,            "file" : { "fmt" : "raw", "name" : mkstr(output/firen.bin)}},
    { "id" : MN_MINERALIZATION,"file" : { "fmt" : "raw", "name" : mkstr(output/mn_mineralization.bin)}},
    { "id" : MN_VOLATILIZATION,"file" : { "fmt" : "raw", "name" : mkstr(output/mn_volatilization.bin)}},*/
    { "id" : PFT_NLIMIT,       "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nlimit.bin)}},
//    { "id" : PFT_VEGN,         "file" : { "fmt" : "raw", "name" : mkstr(output/pft_vegn.bin)}},
    { "id" : "pft_cleaf",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_cleaf.bin)}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nleaf.bin)}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "raw", "name" : mkstr(output/pft_laimax.bin)}},
    { "id" : "pft_croot",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_croot.bin)}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nroot.bin)}},
//    { "id" : PFT_CSAPW,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_csapw.bin)}},
//    { "id" : PFT_NSAPW,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nsapw.bin)}},
//    { "id" : PFT_CHAWO,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_chawo.bin)}},
//    { "id" : PFT_NHAWO,        "file" : { "fmt" : "raw", "name" : mkstr(output/pft_nhawo.bin)}},
#ifdef WITH_SPITFIRE
    { "id" : "firec",           "file" : { "fmt" : "raw", "name" : mkstr(output/mfirec.bin)}},
    { "id" : "nfire",           "file" : { "fmt" : "raw", "name" : mkstr(output/mnfire.bin)}},
    { "id" : "burntarea",       "file" : { "fmt" : "raw", "name" : mkstr(output/mburnt_area.bin)}},
    { "id" : "burntarea",       "file" : { "fmt" : "raw", "name" : mkstr(output/aburnt_area.bin)}},
#endif
    { "id" : "discharge",       "file" : { "fmt" : "raw", "name" : mkstr(output/mdischarge.bin)}},
//    { "id" : MWATERAMOUNT,     "file" : { "fmt" : "raw", "name" : mkstr(output/mwateramount.bin)}},
    { "id" : "harvestc",         "file" : { "fmt" : "raw", "name" : mkstr(output/flux_harvestc.bin)}},
    { "id" : "harvestn",         "file" : { "fmt" : "raw", "name" : mkstr(output/flux_harvestn.bin)}},
    { "id" : "sdate",            "file" : { "fmt" : "raw", "name" : mkstr(output/sdate.bin)}},
    { "id" : "hdate",            "file" : { "fmt" : "raw", "name" : mkstr(output/hdate.bin)}},
    { "id" : "growing_period",   "file" : { "fmt" : "raw", "name" : mkstr(output/growing_period.bin)}},
    { "id" : "husum",            "file" : { "fmt" : "raw", "name" : mkstr(output/husum.bin)}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvestc.SUFFIX)}},
    { "id" : "pft_harvestn",     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvestn.SUFFIX)}},
    { "id" : "cft_airrig",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_airrig.SUFFIX)}},
    { "id" : "cft_pet",          "file" : { "fmt" : "raw", "name" : mkstr(output/cft_pet.SUFFIX)}},
    { "id" : "cft_transp",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_transp.SUFFIX)}},
    { "id" : "cft_evap",         "file" : { "fmt" : "raw", "name" : mkstr(output/cft_evap.SUFFIX)}},
    { "id" : "cft_interc",       "file" : { "fmt" : "raw", "name" : mkstr(output/cft_interc.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "raw", "name" : mkstr(output/cftfrac.bin)}},
    { "id" : "seasonality",      "file" : { "fmt" : "raw", "name" : mkstr(output/seasonality.bin)}},
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : "raw", "name" : mkstr(output/d_npp.bin)}},
    { "id" : D_GPP,            "file" : { "fmt" : "raw", "name" : mkstr(output/d_gpp.bin)}},
    { "id" : D_RH,             "file" : { "fmt" : "raw", "name" : mkstr(output/d_rh.bin)}},
    { "id" : D_TRANS,          "file" : { "fmt" : "raw", "name" : mkstr(output/d_trans.bin)}},
    { "id" : D_INTERC,         "file" : { "fmt" : "raw", "name" : mkstr(output/d_interc.bin)}},
    { "id" : D_EVAP,           "file" : { "fmt" : "raw", "name" : mkstr(output/d_evap.bin)}},
    { "id" : D_LAI,            "file" : { "fmt" : "raw", "name" : mkstr(output/d_lai.bin)}},
    { "id" : D_CLEAF,          "file" : { "fmt" : "raw", "name" : mkstr(output/d_cleaf.bin)}},
    { "id" : D_CROOT,          "file" : { "fmt" : "raw", "name" : mkstr(output/d_croot.bin)}},
    { "id" : D_CSO,            "file" : { "fmt" : "raw", "name" : mkstr(output/d_cso.bin)}},
    { "id" : D_CPOOL,          "file" : { "fmt" : "raw", "name" : mkstr(output/d_cpool.bin)}},
    { "id" : D_HUSUM,          "file" : { "fmt" : "raw", "name" : mkstr(output/d_husum.bin)}},
    { "id" : D_VDSUM,          "file" : { "fmt" : "raw", "name" : mkstr(output/d_vdsum.bin)}}
#endif
/*    { "id" : MPET,             "file" : { "fmt" : "raw", "name" : "output/mpet.bin"}},
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
    { "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : "raw", "name" : "output/aconv_loss_drain.bin"}}*/
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/
#if (DRUN==PIXELRUN)
  "startgrid" :  7187, //9353, //7223, //7175, //7212, //30115, /*31374, 27410, 67208 60400 all grid cells */
  "endgrid" :    7187, //9353, //7223, //7234, //7212, //30115,
#else
  "startgrid" :  "all", /*31374, 27410, 67208 60400 all grid cells */
#endif

#ifdef CHECKPOINT
  "checkpoint_filename" : mkstr(output/restart_checkpoint.lpj), /* filename of checkpoint file */
#endif

#if (DTIM==TIM_1986_2005)
     
  #ifndef FROM_RESTART

    "nspinup" : 5000,  /* spinup years */
    "nspinyear" : 30,  /* cycle length during spinup (yr) */
    "firstyear": 1901, /* first year of simulation */
    "lastyear" : 1901, /* last year of simulation */
    "restart" : false, /* do not start from restart file */
    "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
    "write_restart_filename" : mkstr(output/restart_1861_nv_stdfire.lpj), /* filename of restart file */
    "restart_year": 1840 /* write restart at year */

  #else
   #if (DRUN==PIXELRUN)
    "nspinup" : 60,   /* spinup years */
   #else
    "nspinup" : 390,   /* spinup years */
   #endif
    "nspinyear" : 30,  /* cycle length during spinup (yr)*/
    "firstyear": 1901, /* first year of simulation */
    "lastyear" : 2005, /* last year of simulation */
    "outputyear": 1901, /* first year output is written  */
    "restart" :  true, /* start from restart file */
    "restart_filename" : mkstr(restart/restart_1861_nv_stdfire.lpj), /* filename of restart file */
    "write_restart" : true, /* create restart file */
    "write_restart_filename" : mkstr(output/restart_2005_crop_stdfire.lpj), /* filename of restart file */
    "restart_year": 2005 /* write restart at year */

  #endif /* FROM_RESTART */

#elif (DTIM==TIM_2080_2099)
	 
    "nspinup" : 0,   /* spinup years */
    "nspinyear" : 30,  /* cycle length during spinup (yr)*/
    "firstyear": 2006, /* first year of simulation */
    "lastyear" : 2099, /* last year of simulation */
    "outputyear": 2006, /* first year output is written  */
    "restart" :  true, /* start from restart file */
    "restart_filename" : mkstr(restart/restart_2005_crop_stdfire.lpj), /* filename of restart file */
    "write_restart" : false, /* create restart file */
    "write_restart_filename" : mkstr(output/restart_2005_crop_stdfire.lpj), /* filename of restart file */
    "restart_year": 2005 /* write restart at year */
     
#endif /* TIM_2080_2099 */
}
