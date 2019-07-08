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
  "radiation" : CLOUDINESS,  /* other options: CLOUDINESS, RADIATION, RADIATION_SWONLY, RADIATION_LWDOWN */
  "fire" : FIRE,            /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX */
  "firewood" : false,
  "new_phenology": true,    /* GSI phenology enabled */
  "river_routing" : false,
  "permafrost" : true,
  "with_nitrogen" : UNLIM_NITROGEN, /* other options: NO_NITROGEN, LIM_NITROGEN, UNLIM_NITROGEN */
  "const_climate" : false,
  "const_deposition" : false,
#ifdef FROM_RESTART
  "population" : false,
  "landuse" : ALL_CROPS, /* other options: NO_LANDUSE, LANDUSE, CONST_LANDUSE, ALL_CROPS */
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
  "read_residue_data" : RESIDUE_DATA, /* option to read data residue share left on field: RESIDUE_DATA, NO_RESIDUE_DATA (disables remove_residuals) */ 
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : POT_IRRIGATION,        /* NO_IRRIGATION, LIM_IRRIGATION, POT_IRRIGATION, ALL_IRRIGATION */
  "tillage_type" : READ_TILLAGE,               /* Options: TILLAGE (all agr. cells tilled), NO_TILLAGE (no cells tilled) and READ_TILLAGE (tillage dataset used) */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "laimax_interpolate" : CONST_LAI_MAX, /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE, LAIMAX_PAR  */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : true,            /* enable fertilizer input */
  "fix_fertilization" : false,           /* fix fertilizer input */
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

#include "input_newinputs.js"    /* Input files of CRU dataset */

/*===================================================================*/
/*  IV. Output data section                                          */
/*===================================================================*/

#define output /p/projects/macmit/users/herzfeld/tillage_lpj/historical_new_inputs/macmit_nitrogen_tillage
#define restart /p/projects/macmit/users/herzfeld/tillage_lpj/historical_new_inputs/macmit_nitrogen_tillage/restart

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
    { "id" : GRID,             "file" : { "fmt" : RAW, "name" : mkstr(output/grid.bin) }},
    { "id" : FPC,              "file" : { "fmt" : RAW, "name" : mkstr(output/fpc.bin)}},
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
    { "id" : PFT_VEGC,         "file" : { "fmt" : RAW, "name" : mkstr(output/pft_vegc.bin)}},
    { "id" : MPHEN_TMIN,       "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_tmin.bin)}},
    { "id" : MPHEN_TMAX,       "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_tmax.bin)}},
    { "id" : MPHEN_LIGHT,      "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_light.bin)}},
    { "id" : MPHEN_WATER,      "file" : { "fmt" : RAW, "name" : mkstr(output/mphen_water.bin)}},
    { "id" : VEGN,             "file" : { "fmt" : RAW, "name" : mkstr(output/vegn.bin)}},
    { "id" : SOILN,            "file" : { "fmt" : RAW, "name" : mkstr(output/soiln.bin)}},
    { "id" : LITN,             "file" : { "fmt" : RAW, "name" : mkstr(output/litn.bin)}},
    { "id" : SOILN_LAYER,      "file" : { "fmt" : RAW, "name" : mkstr(output/soiln_layer.bin)}},
    { "id" : SOILNO3_LAYER,    "file" : { "fmt" : RAW, "name" : mkstr(output/soilno3_layer.bin)}},
    { "id" : SOILNH4_LAYER,    "file" : { "fmt" : RAW, "name" : mkstr(output/soilnh4_layer.bin)}},
    { "id" : SOILN_SLOW,       "file" : { "fmt" : RAW, "name" : mkstr(output/soiln_slow.bin)}},
    { "id" : SOILNH4,          "file" : { "fmt" : RAW, "name" : mkstr(output/soilnh4.bin)}},
    { "id" : SOILNO3,          "file" : { "fmt" : RAW, "name" : mkstr(output/soilno3.bin)}},
    { "id" : PFT_NUPTAKE,      "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nuptake.bin)}},
    { "id" : MNUPTAKE,         "file" : { "fmt" : RAW, "name" : mkstr(output/mnuptake.bin)}},
    { "id" : MLEACHING,        "file" : { "fmt" : RAW, "name" : mkstr(output/mleaching.bin)}},
    { "id" : MN2O_DENIT,       "file" : { "fmt" : RAW, "name" : mkstr(output/mn2o_denit.bin)}},
    { "id" : MN2O_NIT,         "file" : { "fmt" : RAW, "name" : mkstr(output/mn2o_nit.bin)}},
    { "id" : MN2_EMIS,         "file" : { "fmt" : RAW, "name" : mkstr(output/mn2_emis.bin)}},
    { "id" : MBNF,             "file" : { "fmt" : RAW, "name" : mkstr(output/mbnf.bin)}},
    { "id" : MN_IMMO,          "file" : { "fmt" : RAW, "name" : mkstr(output/mn_immo.bin)}},
    { "id" : PFT_NDEMAND,      "file" : { "fmt" : RAW, "name" : mkstr(output/pft_ndemand.bin)}},
    { "id" : FIREN,            "file" : { "fmt" : RAW, "name" : mkstr(output/firen.bin)}},
    { "id" : MN_MINERALIZATION,"file" : { "fmt" : RAW, "name" : mkstr(output/mn_mineralization.bin)}},
    { "id" : MN_VOLATILIZATION,"file" : { "fmt" : RAW, "name" : mkstr(output/mn_volatilization.bin)}},
    { "id" : PFT_NLIMIT,       "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nlimit.bin)}},
    { "id" : PFT_VEGN,         "file" : { "fmt" : RAW, "name" : mkstr(output/pft_vegn.bin)}},
    { "id" : PFT_CLEAF,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_cleaf.bin)}},
    { "id" : PFT_NLEAF,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nleaf.bin)}},
    { "id" : PFT_LAIMAX,       "file" : { "fmt" : RAW, "name" : mkstr(output/pft_laimax.bin)}},
    { "id" : PFT_CROOT,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_croot.bin)}},
    { "id" : PFT_NROOT,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nroot.bin)}},
    { "id" : PFT_CSAPW,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_csapw.bin)}},
    { "id" : PFT_NSAPW,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nsapw.bin)}},
    { "id" : PFT_CHAWO,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_chawo.bin)}},
    { "id" : PFT_NHAWO,        "file" : { "fmt" : RAW, "name" : mkstr(output/pft_nhawo.bin)}},
#ifdef WITH_SPITFIRE
    { "id" : MFIREC,           "file" : { "fmt" : RAW, "name" : mkstr(output/mfirec.bin)}},
    { "id" : MNFIRE,           "file" : { "fmt" : RAW, "name" : mkstr(output/mnfire.bin)}},
    { "id" : MBURNTAREA,       "file" : { "fmt" : RAW, "name" : mkstr(output/mburnt_area.bin)}},
    { "id" : ABURNTAREA,       "file" : { "fmt" : RAW, "name" : mkstr(output/aburnt_area.bin)}},
#endif
    { "id" : MDISCHARGE,       "file" : { "fmt" : RAW, "name" : mkstr(output/mdischarge.bin)}},
    { "id" : MWATERAMOUNT,     "file" : { "fmt" : RAW, "name" : mkstr(output/mwateramount.bin)}},
    { "id" : HARVESTC,         "file" : { "fmt" : RAW, "name" : mkstr(output/flux_harvest.bin)}},
    { "id" : SDATE,            "file" : { "fmt" : RAW, "name" : mkstr(output/sdate_lai.LMAX.bin)}},
    { "id" : HDATE,            "file" : { "fmt" : RAW, "name" : mkstr(output/hdate_lai.LMAX.bin)}},
    { "id" : PFT_HARVESTC,     "file" : { "fmt" : RAW, "name" : mkstr(output/pft_harvest_lai.LMAX.SUFFIX)}},
    { "id" : CFT_AIRRIG,       "file" : { "fmt" : RAW, "name" : mkstr(output/cft_airrig_lai.LMAX.SUFFIX)}},
    { "id" : CFTFRAC,          "file" : { "fmt" : RAW, "name" : mkstr(output/cftfrac_lai.LMAX.bin)}},
    { "id" : SEASONALITY,      "file" : { "fmt" : RAW, "name" : mkstr(output/seasonality.bin)}},
#ifdef DAILY_OUTPUT
    { "id" : D_NPP,            "file" : { "fmt" : RAW, "name" : mkstr(output/d_npp.bin)}},
    { "id" : D_GPP,            "file" : { "fmt" : RAW, "name" : mkstr(output/d_gpp.bin)}},
    { "id" : D_RH,             "file" : { "fmt" : RAW, "name" : mkstr(output/d_rh.bin)}},
    { "id" : D_TRANS,          "file" : { "fmt" : RAW, "name" : mkstr(output/d_trans.bin)}},
    { "id" : D_INTERC,         "file" : { "fmt" : RAW, "name" : mkstr(output/d_interc.bin)}},
    { "id" : D_EVAP,           "file" : { "fmt" : RAW, "name" : mkstr(output/d_evap.bin)}},
#endif
/*    { "id" : MPET,             "file" : { "fmt" : RAW, "name" : mkstr(output/mpet.bin)}},
    { "id" : MALBEDO,          "file" : { "fmt" : RAW, "name" : mkstr(output/malbedo.bin)}},
    { "id" : MAXTHAW_DEPTH,    "file" : { "fmt" : RAW, "name" : mkstr(output/maxthaw_depth.bin)}},
    { "id" : MSOILTEMP1,       "file" : { "fmt" : RAW, "name" : mkstr(output/msoiltemp1.bin)}},
    { "id" : MSOILTEMP2,       "file" : { "fmt" : RAW, "name" : mkstr(output/msoiltemp2.bin)}},
    { "id" : MSOILTEMP3,       "file" : { "fmt" : RAW, "name" : mkstr(output/msoiltemp3.bin)}},
    { "id" : SOILC_LAYER,      "file" : { "fmt" : RAW, "name" : mkstr(output/soilc_layer.bin)}},
    { "id" : AGB,              "file" : { "fmt" : RAW, "name" : mkstr(output/agb.bin)}},
    { "id" : AGB_TREE,         "file" : { "fmt" : RAW, "name" : mkstr(output/agb_tree.bin)}},
    { "id" : MRETURN_FLOW_B,   "file" : { "fmt" : RAW, "name" : mkstr(output/mreturn_flow_b.bin)}},
    { "id" : MTRANSP_B,        "file" : { "fmt" : RAW, "name" : mkstr(output/mtransp_b.bin)}},
    { "id" : MEVAP_B,          "file" : { "fmt" : RAW, "name" : mkstr(output/mevap_b.bin)}},
    { "id" : MINTERC_B,        "file" : { "fmt" : RAW, "name" : mkstr(output/mintec_b.bin)}},
    { "id" : ACONV_LOSS_EVAP,  "file" : { "fmt" : RAW, "name" : mkstr(output/aconv_loss_evap.bin)}},
    { "id" : ACONV_LOSS_DRAIN, "file" : { "fmt" : RAW, "name" : mkstr(output/aconv_loss_drain.bin)}},
    { "id" : PFT_NPP,          "file" : { "fmt" : RAW, "name" : mkstr(output/pft_npp.bin)}},
    { "id" : MLITFALLC,        "file" : { "fmt" : RAW, "name" : mkstr(output/mlitterfallc.bin)}},
    { "id" : MLITFALLN,        "file" : { "fmt" : RAW, "name" : mkstr(output/mlitterfalln.bin)}},
    { "id" : ALITFALLC,        "file" : { "fmt" : RAW, "name" : mkstr(output/alitterfallc.bin)}},
    { "id" : ALITFALLN,        "file" : { "fmt" : RAW, "name" : mkstr(output/alitterfalln.bin)}},
    { "id" : MRH_LAYER0,       "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_layer0.bin)}},
    { "id" : MRH_LAYER1,       "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_layer1.bin)}},
    { "id" : MRH_LAYER2,       "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_layer2.bin)}},
    { "id" : MRH_LAYER3,       "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_layer3.bin)}},
    { "id" : MRH_LAYER4,       "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_layer4.bin)}},
    { "id" : SOILC_NAT,        "file" : { "fmt" : RAW, "name" : mkstr(output/soilc_nat.bin)}},
    { "id" : MRH_NAT,          "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_nat.bin)}},
    { "id" : MRH_NAT_LITTER,   "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_nat_litter.bin)}},
    { "id" : SOILC_AGR,        "file" : { "fmt" : RAW, "name" : mkstr(output/soilc_agr.bin)}},
    { "id" : MRH_AGR,          "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_agr.bin)}},
    { "id" : MRH_AGR_LITTER,   "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_agr_litter.bin)}},
    { "id" : SOILC_GRA,        "file" : { "fmt" : RAW, "name" : mkstr(output/soilc_gra.bin)}},
    { "id" : MRH_GRA,          "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_gra.bin)}},
    { "id" : MRH_GRA_LITTER,   "file" : { "fmt" : RAW, "name" : mkstr(output/mrh_gra_litter.bin)}}*/
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
  "checkpoint_filename" : mkstr(restart/restart_checkpoint.lpj), /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "restart" : false, /* do not start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : mkstr(restart/restart_1900_nv_stdfire_N.lpj), /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#else

  "nspinup" : 30,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2011, /* last year of simulation */
  "outputyear": 1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : mkstr(restart/restart_1900_nv_stdfire_N.lpj), /* filename of restart file */
  "write_restart" : false, /* create restart file */
  "write_restart_filename" : mkstr(restart/restart_1900_crop_stdfire_N.lpj), /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
