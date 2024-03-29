/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Configuration file for LPJmL C Version 5.2.002                                 \n**/
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
  "sim_id"   : "lpjml",       /* LPJML Simulation type with managed land use */
  "version"  : "5.2",       /* LPJmL version expected */
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : "fire",            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "new_trf" : true,
  "river_routing" : true,
  "permafrost" : true,
  "with_nitrogen" : "lim_nitrogen", /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "store_climate" : true,
  "shuffle_climate" : true,
  "const_climate" : false,
  "fix_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "new_seed" : false,
  "equilsoil" : false,
  "population" : false,
  "landuse" : "all_crops", /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS, ONLY_CROPS (crops scaled to 100% of cell) */
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
  "irrigation" : "lim_irrigation",        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "tillage_type" : "read_tillage",          /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "till_startyear" : 1850,              /* year in which tillage should start */
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

#include "param_intensification.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_intensification.js"    /* Input files of CRU dataset */

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

#define output /p/projects/macmit/users/minoli/PROJECTS/MACMIT_INTENSIFICATION/LPJmL/output/test

  "crop_index" : "temperate cereals",  /* CFT for daily output */
  "crop_irrigation" : false, /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" : 
  [

/*
ID                         Fmt                    filename
-------------------------- ---------------------- ----------------------------- */
    { "id" : GRID,             "file" : { "fmt" : RAW, "name" : mkstr(output/grid.bin) }},
/*    { "id" : FPC,              "file" : { "fmt" : RAW, "name" : mkstr(output/fpc.bin)}},
    { "id" : NPP,             "file" : { "fmt" : RAW, "name" : mkstr(output/mnpp.bin)}},
    { "id" : GPP,             "file" : { "fmt" : RAW, "name" : mkstr(output/mgpp.bin)}},
    { "id" : RH,              "file" : { "fmt" : RAW, "name" : mkstr(output/mrh.bin)}},
    { "id" : FAPAR,           "file" : { "fmt" : RAW, "name" : mkstr(output/mfapar.bin)}},
    { "id" : TRANSP,          "file" : { "fmt" : RAW, "name" : mkstr(output/mtransp.bin)}},
    { "id" : RUNOFF,          "file" : { "fmt" : RAW, "name" : mkstr(output/mrunoff.bin)}},
    { "id" : EVAP,            "file" : { "fmt" : RAW, "name" : mkstr(output/mevap.bin)}},
    { "id" : INTERC,          "file" : { "fmt" : RAW, "name" : mkstr(output/minterc.bin)}},
    { "id" : SWC1,            "file" : { "fmt" : RAW, "name" : mkstr(output/mswc1.bin)}},
    { "id" : SWC2,            "file" : { "fmt" : RAW, "name" : mkstr(output/mswc2.bin)}},
    { "id" : FIREC,            "file" : { "fmt" : RAW, "name" : mkstr(output/firec.bin)}},
    { "id" : FIREF,            "file" : { "fmt" : RAW, "name" : mkstr(output/firef.bin)}},
    { "id" : VEGC,             "file" : { "fmt" : RAW, "name" : mkstr(output/vegc.bin)}},
    { "id" : SOILC,            "file" : { "fmt" : RAW, "name" : mkstr(output/soilc.bin)}},
    { "id" : LITC,             "file" : { "fmt" : RAW, "name" : mkstr(output/litc.bin)}},
    { "id" : FLUX_ESTABC,      "file" : { "fmt" : RAW, "name" : mkstr(output/flux_estab.bin)}},
    { "id" : PFT_VEGC,         "file" : { "fmt" : RAW, "name" : mkstr(output/pft_vegc.bin)}},*/
//    { "id" : PHEN_TMIN,       "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_tmin.bin)}},
//    { "id" : PHEN_TMAX,       "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_tmax.bin)}},
//    { "id" : PHEN_LIGHT,      "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_light.bin)}},
//    { "id" : PHEN_WATER,      "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_water.bin)}},
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
/*    { "id" : NUPTAKE,         "file" : { "fmt" : RAW, "name" : mkstr(output/mnuptake.bin)}},
    { "id" : LEACHING,        "file" : { "fmt" : RAW, "name" : mkstr(output/mleaching.bin)}},
    { "id" : N2O_DENIT,       "file" : { "fmt" : RAW, "name" : mkstr(output/mn2o_denit.bin)}},
    { "id" : N2O_NIT,         "file" : { "fmt" : RAW, "name" : mkstr(output/mn2o_nit.bin)}},
    { "id" : N2_EMIS,         "file" : { "fmt" : RAW, "name" : mkstr(output/mn2_emis.bin)}},
    { "id" : BNF,             "file" : { "fmt" : RAW, "name" : mkstr(output/mbnf.bin)}},
    { "id" : N_IMMO,          "file" : { "fmt" : RAW, "name" : mkstr(output/mn_immo.bin)}},*/
    { "id" : PFT_NDEMAND,      "file" : { "fmt" : RAW, "name" : mkstr(output/pft_ndemand.bin)}},
/*    { "id" : FIREN,            "file" : { "fmt" : RAW, "name" : mkstr(output/firen.bin)}},
    { "id" : N_MINERALIZATION,"file" : { "fmt" : RAW, "name" : mkstr(output/mn_mineralization.bin)}},
    { "id" : N_VOLATILIZATION,"file" : { "fmt" : RAW, "name" : mkstr(output/mn_volatilization.bin)}},*/
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
//    { "id" : WATERAMOUNT,     "file" : { "fmt" : RAW, "name" : mkstr(output/mwateramount.bin)}},
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

  "startgrid" :  ALL, /* 27410, 67208 60400 all grid cells */
  "endgrid" : ALL,

#ifdef CHECKPOINT
  "checkpoint_filename" : mkstr(output/restart_checkpoint.lpj), /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1979, //1901, /* first year of simulation */
  "lastyear" : 1979, //1901, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  //"write_restart_filename" : mkstr(output/restart_1840_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
  "write_restart_filename" : mkstr(output/restart_1978_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
  "restart_year": 1978, //1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1979, //1901, /* first year of simulation */
  "lastyear" : 2005, /* last year of simulation */
  "outputyear": 1979, //1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  //"restart_filename" : mkstr(output/restart_1840_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
  "restart_filename" : mkstr(output/restart_1840_nv_stdfire_withrrouting_test.lpj), /* filename of restart file */
  //"write_restart" : true, /* create restart file */
  "write_restart" : false, /* create restart file */
  "write_restart_filename" : mkstr(output/restart_1900_crop_stdfire_test.lpj), /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
