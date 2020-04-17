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

/* ifdefs for MACMIT_INTENSIFICATION runs*/

/*Identifiers (preprocessor only understands integer expressions in #if)*/
#define TIM_1986_2005 1
#define TIM_2080_2099 2

#define SCN_HISTOR 1
#define SCN_RCP60  2

#define CLM_WFD 1
#define CLM_HAD 2
#define CLM_GFD 3
#define CLM_IPS 4
#define CLM_MIR 5

#define IRR_LIM 1
#define IRR_UNL 2

#define NIT_LIM 1
#define NIT_UNL 2

#define GS_DAT 1
#define GS_SA0 2
#define GS_SA1 3

/* WFDI */
#ifdef RUN_ID_01
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_WFD //1//"CLM_WFD"
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_DAT /* GS_SA0, GS_SA1 */
    #define output_run  01_tim.1986.2005_scn.histor_clm.wfd_irr.lim_nit.lim_gs.dat_res.dat_til.dat
    #define restart_run 01_tim.1986.2005_scn.histor_clm.wfd_irr.lim_nit.lim_gs.dat_res.dat_til.dat
#elif defined(RUN_ID_02)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_WFD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_DAT /* GS_SA0, GS_SA1 */
    #define output_run  02_tim.1986.2005_scn.histor_clm.wfd_irr.unl_nit.unl_gs.dat_res.dat_til.dat
    #define restart_run 01_tim.1986.2005_scn.histor_clm.wfd_irr.lim_nit.lim_gs.dat_res.dat_til.dat
 
 /* HadGEM2-ES */
#elif defined(RUN_ID_03)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_HAD //2 //"CLM_HAD"
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_07)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_HAD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  07_tim.1986.2005_scn.histor_clm.had_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_11)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_HAD
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  11_tim.2080.2099_scn.rcp60_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_15)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_HAD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run 15_tim.2080.2099_scn.rcp60_clm.had_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_19)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_HAD
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run 19_tim.2080.2099_scn.rcp60_clm.had_irr.lim_nit.lim_gs.sa1_res.dat_til.dat
    #define restart_run 03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_23)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_HAD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run 23_tim.2080.2099_scn.rcp60_clm.had_irr.unl_nit.unl_gs.sa1_res.dat_til.dat
    #define restart_run 03_tim.1986.2005_scn.histor_clm.had_irr.lim_nit.lim_gs.sa0_res.dat_til.dat

 /* GFDL-ESM2M */
#elif defined(RUN_ID_04)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_GFD
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_08)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_GFD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  08_tim.1986.2005_scn.histor_clm.gfd_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_12)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_GFD
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  12_tim.2080.2099_scn.rcp60_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_16)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_GFD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  16_tim.2080.2099_scn.rcp60_clm.gfd_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_20)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_GFD
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run  20_tim.2080.2099_scn.rcp60_clm.gfd_irr.lim_nit.lim_gs.sa1_res.dat_til.dat
    #define restart_run 04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_24)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_GFD
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run  24_tim.2080.2099_scn.rcp60_clm.gfd_irr.unl_nit.unl_gs.sa1_res.dat_til.dat
    #define restart_run 04_tim.1986.2005_scn.histor_clm.gfd_irr.lim_nit.lim_gs.sa0_res.dat_til.dat

/* IPSL-CM5A-LR */
#elif defined(RUN_ID_05)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_IPS
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_09)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_IPS
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  09_tim.1986.2005_scn.histor_clm.ips_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_13)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_IPS
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  13_tim.2080.2099_scn.rcp60_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_17)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_IPS
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  17_tim.2080.2099_scn.rcp60_clm.ips_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_21)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_IPS
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run  21_tim.2080.2099_scn.rcp60_clm.ips_irr.lim_nit.lim_gs.sa1_res.dat_til.dat
    #define restart_run 05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_25)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_IPS
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run  25_tim.2080.2099_scn.rcp60_clm.ips_irr.unl_nit.unl_gs.sa1_res.dat_til.dat
    #define restart_run 05_tim.1986.2005_scn.histor_clm.ips_irr.lim_nit.lim_gs.sa0_res.dat_til.dat

 /* MIROC5 */
 #elif defined(RUN_ID_06)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_MIR
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_10)
    #define DTIM TIM_1986_2005
    #define DSCN SCN_HISTOR
    #define DCLM CLM_MIR
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  10_tim.1986.2005_scn.histor_clm.mir_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_14)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_MIR
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  14_tim.2080.2099_scn.rcp60_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
    #define restart_run 06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_18)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_MIR
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA0 /* GS_SA0, GS_SA1 */
    #define output_run  18_tim.2080.2099_scn.rcp60_clm.mir_irr.unl_nit.unl_gs.sa0_res.dat_til.dat
    #define restart_run 06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_22)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_MIR
    #define DIRR IRR_LIM /* IRR_UNL */
    #define DNIT NIT_LIM /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run  22_tim.2080.2099_scn.rcp60_clm.mir_irr.lim_nit.lim_gs.sa1_res.dat_til.dat
    #define restart_run 06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat
#elif defined(RUN_ID_26)
    #define DTIM TIM_2080_2099
    #define DSCN SCN_RCP60
    #define DCLM CLM_MIR
    #define DIRR IRR_UNL /* IRR_UNL */
    #define DNIT NIT_UNL /* NIT_UNL */
    #define DGS  GS_SA1 /* GS_SA0, GS_SA1 */
    #define output_run  26_tim.2080.2099_scn.rcp60_clm.mir_irr.unl_nit.unl_gs.sa1_res.dat_til.dat
    #define restart_run 06_tim.1986.2005_scn.histor_clm.mir_irr.lim_nit.lim_gs.sa0_res.dat_til.dat

#else
#endif /* RUN_ID */

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : LPJML,       /* LPJML Simulation type with managed land use */
  "version"  : "5.2",       /* LPJmL version expected */
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : RADIATION,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : FIRE,            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */

#if (DIRR==IRR_LIM)
  "river_routing" : true,  //true, false set only for testing pixel runs
#elif (DIRR==IRR_UNL)
  "river_routing" : true,  //true,
#endif

  "permafrost" : true,

#if (DNIT==NIT_LIM)
  "with_nitrogen" : LIM_NITROGEN, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
#elif (DNIT==NIT_UNL)
  "with_nitrogen" : UNLIM_NITROGEN,
  #else
#endif

  "const_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "population" : false,
  "landuse" : ALL_CROPS, /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS, ONLY_CROPS (crops scaled to 100% of cell) */
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
  "sowing_date_option" : PRESCRIBED_SDATE,   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "crop_phu_option" : PRESCRIBED_CROP_PHU,    /* PRESCRIBED_CROP_PHU (PHU dataset used, requires PRESCRIBED_SDATE), SEMISTATIC_CROP_PHU (LPJmL4 semi-static PHU approach) */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : READ_RESIDUE_DATA, /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */

#if (DIRR==IRR_LIM)
  "irrigation" : LIM_IRRIGATION,        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
#elif (DIRR==IRR_UNL)
  "irrigation" : POT_IRRIGATION,
#endif

  "tillage_type" : READ_TILLAGE,          /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "laimax_interpolate" : LAIMAX_PAR,    /* laimax values from manage parameter file, */
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

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_macmit_intensification.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_macmit_intensification.js"    /* Input files of CRU dataset */

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

#define workdir /p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/outputs
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
/*    { "id" : FPC,              "file" : { "fmt" : RAW, "name" : mkstr(output/fpc.bin)}},
    { "id" : MNPP,             "file" : { "fmt" : RAW, "name" : mkstr(output/mnpp.bin)}},
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
    { "id" : FIREF,            "file" : { "fmt" : RAW, "name" : mkstr(output/firef.bin)}},
    { "id" : VEGC,             "file" : { "fmt" : RAW, "name" : mkstr(output/vegc.bin)}},
    { "id" : SOILC,            "file" : { "fmt" : RAW, "name" : mkstr(output/soilc.bin)}},
    { "id" : LITC,             "file" : { "fmt" : RAW, "name" : mkstr(output/litc.bin)}},
    { "id" : FLUX_ESTABC,      "file" : { "fmt" : RAW, "name" : mkstr(output/flux_estab.bin)}},
    { "id" : PFT_VEGC,         "file" : { "fmt" : RAW, "name" : mkstr(output/pft_vegc.bin)}},*/
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
    { "id" : MFIREC,           "file" : { "fmt" : RAW, "name" : mkstr(output/mfirec.bin)}},
    { "id" : MNFIRE,           "file" : { "fmt" : RAW, "name" : mkstr(output/mnfire.bin)}},
    { "id" : MBURNTAREA,       "file" : { "fmt" : RAW, "name" : mkstr(output/mburnt_area.bin)}},
    { "id" : ABURNTAREA,       "file" : { "fmt" : RAW, "name" : mkstr(output/aburnt_area.bin)}},
#endif
    { "id" : MDISCHARGE,       "file" : { "fmt" : RAW, "name" : mkstr(output/mdischarge.bin)}},
//    { "id" : MWATERAMOUNT,     "file" : { "fmt" : RAW, "name" : mkstr(output/mwateramount.bin)}},
    { "id" : HARVESTC,         "file" : { "fmt" : RAW, "name" : mkstr(output/flux_harvestc.bin)}},
    { "id" : HARVESTN,         "file" : { "fmt" : RAW, "name" : mkstr(output/flux_harvestn.bin)}},
    { "id" : SDATE,            "file" : { "fmt" : RAW, "name" : mkstr(output/sdate.bin)}},
    { "id" : HDATE,            "file" : { "fmt" : RAW, "name" : mkstr(output/hdate.bin)}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvestc.SUFFIX)}},
    { "id" : PFT_HARVESTN,     "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvestn.SUFFIX)}},
    { "id" : CFT_AIRRIG,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_airrig.SUFFIX)}},
    { "id" : CFT_PET,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_pet.SUFFIX)}},
    { "id" : CFT_TRANSP,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_transp.SUFFIX)}},
    { "id" : CFT_EVAP,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_evap.SUFFIX)}},
    { "id" : CFT_INTERC,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_interc.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : RAW, "name" : mkstr(output/cftfrac.bin)}},
    { "id" : SEASONALITY,      "file" : { "fmt" : RAW, "name" : mkstr(output/seasonality.bin)}} //,
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : RAW, "name" : "output/d_npp.bin"}},
    { "id" : D_GPP,            "file" : { "fmt" : RAW, "name" : "output/d_gpp.bin"}},
    { "id" : D_RH,             "file" : { "fmt" : RAW, "name" : "output/d_rh.bin"}},
    { "id" : D_TRANS,          "file" : { "fmt" : RAW, "name" : "output/d_trans.bin"}},
    { "id" : D_INTERC,         "file" : { "fmt" : RAW, "name" : "output/d_interc.bin"}},
    { "id" : D_EVAP,           "file" : { "fmt" : RAW, "name" : "output/d_evap.bin"}},
#endif
/*    { "id" : MPET,             "file" : { "fmt" : RAW, "name" : "output/mpet.bin"}},
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
    { "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : RAW, "name" : "output/aconv_loss_drain.bin"}}*/
/*------------------------ ---------------------- ------------------------------- */
  ],

#else

  "output" : [],  /* no output written */

#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" :  27410, //ALL, /* 27410, 67208 60400 all grid cells */
  "endgrid" : 27410, //ALL,

#ifdef CHECKPOINT
  "checkpoint_filename" : mkstr(output/restart_checkpoint.lpj), /* filename of checkpoint file */
#endif

#if (DCLM==CLM_WFD)

    #ifndef FROM_RESTART

      "nspinup" : 5000,  /* spinup years */
      "nspinyear" : 30,  /* cycle length during spinup (yr) */
      "firstyear": 1979, //1901, /* first year of simulation */
      "lastyear" : 1979, //1901, /* last year of simulation */
      "restart" : false, /* do not start from restart file */
      "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
      //"write_restart_filename" : mkstr(output/restart_1840_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
      "write_restart_filename" : mkstr(output/restart_1978_nv_stdfire.lpj), /* filename of restart file */
      "restart_year": 1978, //1840 /* write restart at year */

    #else

      "nspinup" : 390,   /* spinup years */
      "nspinyear" : 30,  /* cycle length during spinup (yr)*/
      "firstyear": 1979, //1901, /* first year of simulation */
      "lastyear" : 2005, /* last year of simulation */
      "outputyear": 1979, //1901, /* first year output is written  */
      "restart" :  true, /* start from restart file */
      //"restart_filename" : mkstr(output/restart_1840_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
      "restart_filename" : mkstr(output/restart_1978_nv_stdfire.lpj), /* filename of restart file */
      //"write_restart" : true, /* create restart file */
      "write_restart" : false, /* create restart file */
      "write_restart_filename" : mkstr(output/restart_1900_crop_stdfire.lpj), /* filename of restart file */
      "restart_year": 1900 /* write restart at year */
    
     #endif
    
#else

     #if (DTIM==TIM_1986_2005)
     
     #ifndef FROM_RESTART

      "nspinup" : 5000,  /* spinup years */
      "nspinyear" : 30,  /* cycle length during spinup (yr) */
      "firstyear": 1901, /* first year of simulation */
      "lastyear" : 1901, /* last year of simulation */
      "restart" : false, /* do not start from restart file */
      "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
      "write_restart_filename" : mkstr(output/restart_1861_nv_stdfire.lpj), /* filename of restart file */
      //"write_restart_filename" : mkstr(output/restart_1978_nv_stdfire.lpj), /* filename of restart file */
      "restart_year": 1840 /* write restart at year */

     #else

       "nspinup" : 390,   /* spinup years */
       "nspinyear" : 30,  /* cycle length during spinup (yr)*/
       "firstyear": 1901, /* first year of simulation */
       "lastyear" : 2005, /* last year of simulation */
       "outputyear": 1901, /* first year output is written  */
       "restart" :  true, /* start from restart file */
       "restart_filename" : mkstr(output/restart_1861_nv_stdfire.lpj), /* filename of restart file */
       //"restart_filename" : mkstr(output/restart_1978_nv_stdfire.lpj), /* filename of restart file */
       "write_restart" : true, /* create restart file */
       //"write_restart" : false, /* create restart file */
       "write_restart_filename" : mkstr(output/restart_2005_crop_stdfire.lpj), /* filename of restart file */
       "restart_year": 2005 /* write restart at year */

      #endif /* FROM_RESTART */

     #elif (DTIM==TIM_2080_2099)
	 
       //#ifndef FROM_RESTART

        //"nspinup" : 5000,  /* spinup years */
        //"nspinyear" : 30,  /* cycle length during spinup (yr) */
        //"firstyear": 1901, /* first year of simulation */
        //"lastyear" : 1901, /* last year of simulation */
        //"restart" : false, /* do not start from restart file */
        //"write_restart" : true, /* create restart file: the last year of simulation=restart-year */
        //"write_restart_filename" : mkstr(output/restart_1840_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
        //"write_restart_filename" : mkstr(output/restart_1978_nv_stdfire.lpj), /* filename of restart file */
        //"restart_year": 1840 /* write restart at year */
        
       //#else

        "nspinup" : 0,   /* spinup years */
        "nspinyear" : 30,  /* cycle length during spinup (yr)*/
        "firstyear": 2006, /* first year of simulation */
        "lastyear" : 2099, /* last year of simulation */
        "outputyear": 2006, /* first year output is written  */
        "restart" :  true, /* start from restart file */
        "restart_filename" : mkstr(restart/restart_2005_crop_stdfire.lpj), /* filename of restart file */
        //"restart_filename" : mkstr(output/restart_1978_nv_stdfire.lpj), /* filename of restart file */
        //"write_restart" : true, /* create restart file */
        "write_restart" : false, /* create restart file */
        "write_restart_filename" : mkstr(output/restart_2005_crop_stdfire.lpj), /* filename of restart file */
        "restart_year": 2005 /* write restart at year */

    // #endif /* FROM_RESTART */
     
     #endif /* TIM_2080_2099 */

#endif /* CLM */
}