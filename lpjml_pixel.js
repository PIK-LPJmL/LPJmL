/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.3.001                         \n**/
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

//#define DAILY_OUTPUT  /* enables daily output */

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "version"  : "5.3",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fire_on_grassland" : false, /* enable fire on grassland for Spitfire */
  "fdi" : "nesterov",       /* different fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
  "firewood" : false,
  "new_phenology" : true,   /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : false,
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
  "store_climate" : true, /* store climate data in spin-up phase */
  "const_climate" : false,
  "shuffle_climate" : true, /* shuffle spinup climate */
  "const_deposition" : false,
  "fix_climate" : false,
  "fix_landuse" : false,
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "population" : false,
  "landuse" : "yes", /* other options: "no", "yes", "const", "all_crops", "only_crops" */
  "landuse_year_const" : 2000, /* set landuse year for "const" case */
  "reservoir" : true,
  "wateruse" : "yes",  /* other options: "no", "yes", "all" */
  "equilsoil" : false,
#else
  "equilsoil" : true,
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "fixed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : true,                   /* intercrops on setaside */
  "residue_treatment" : "fixed_residue_remove", /* residue options: READ_RESIDUE_DATA, NO_RESIDUE_REMOVE, FIXED_RESIDUE_REMOVE (uses param residues_in_soil) */ 
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim",                 /* other options: "no", "lim", "pot", "all" */
  "laimax_interpolate" : "laimax_par",  /* laimax values from manage parameter file, */
                                        /* other options: LAIMAX_CFT, CONST_LAI_MAX, LAIMAX_INTERPOLATE */
  "tillage_type" : "all",               /* Options: "all" (all agr. cells tilled), "no" (no cells tilled) and "read" (tillage dataset used) */
  "till_startyear" : 1850,              /* year in which tillage should start */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "pft_residue" : "temperate cereals",
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for CONST_LAI_MAX */
  "fertilizer_input" : "yes",           /* enable fertilizer input, other options: "no", "yes", "auto" */
  "manure_input" : true,                /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : true,              /* move PFT type others into PFT crop, cft_tropic for tropical,  cft_temp for temperate */
  "grazing" : "default",                /* default grazing type, other options : "default", "mowing", "ext", "int", "livestock", "none" */
  "cft_temp" : "temperate cereals",
  "cft_tropic" : "maize",
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "prescribe_lsuha" : false,
  "mowing_days" : [152, 335],          /* Mowing days for grassland if grass harvest options are ser */
  "crop_resp_fix" : false,             /* variable C:N ratio for crop respiration */
                                       /* for MAgPIE runs, turn off dynamic C:N ratio dependent respiration,
                                          which reduces yields at high N inputs */
  "crop_phu_option" : "new",
  "cropsheatfrost" : false,
  "double_harvest" : true,

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
  "grid_scaled" : true, /* PFT-specific outputs scaled by stand->frac */
#define SUFFIX grid.bin
#else
  "grid_scaled" : false,
#define SUFFIX pft.bin
#endif

  "float_grid" : false, /* set datatype of grid file to float (TRUE/FALSE) */

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "crop_index" : "temperate cereals", /* CFT for daily output */
  "crop_irrigation" : false,          /* irrigation flag for daily output */

#ifdef FROM_RESTART

  "output" :
  [

/*
ID                               Fmt                        filename
-------------------------------- ------------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output_pixel/grid.bin" }},
    { "id" : "fpc",              "file" : { "fmt" : "raw", "name" : "output_pixel/fpc.bin" }},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output_pixel/globalflux.csv"}},
    { "id" : "npp",              "file" : { "fmt" : "raw", "name" : "output_pixel/mnpp.bin"}},
    { "id" : "gpp",              "file" : { "fmt" : "raw", "name" : "output_pixel/mgpp.bin"}},
    { "id" : "rh",               "file" : { "fmt" : "raw", "name" : "output_pixel/mrh.bin"}},
    { "id" : "fapar",            "file" : { "fmt" : "raw", "name" : "output_pixel/mfapar.bin"}},
    { "id" : "transp",           "file" : { "fmt" : "raw", "name" : "output_pixel/mtransp.bin"}},
    { "id" : "runoff",           "file" : { "fmt" : "raw", "name" : "output_pixel/mrunoff.bin"}},
    { "id" : "evap",             "file" : { "fmt" : "raw", "name" : "output_pixel/mevap.bin"}},
    { "id" : "interc",           "file" : { "fmt" : "raw", "name" : "output_pixel/minterc.bin"}},
    { "id" : "swc1",             "file" : { "fmt" : "raw", "name" : "output_pixel/mswc1.bin"}},
    { "id" : "swc2",             "file" : { "fmt" : "raw", "name" : "output_pixel/mswc2.bin"}},
    { "id" : "firef",            "file" : { "fmt" : "raw", "name" : "output_pixel/firef.bin"}},
    { "id" : "vegc",             "file" : { "fmt" : "raw", "name" : "output_pixel/vegc.bin"}},
    { "id" : "soilc",            "file" : { "fmt" : "raw", "name" : "output_pixel/soilc.bin"}},
    { "id" : "litc",             "file" : { "fmt" : "raw", "name" : "output_pixel/litc.bin"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "raw", "name" : "output_pixel/flux_estab.bin"}},
    { "id" : "pft_vegc",         "file" : { "fmt" : "raw", "name" : "output_pixel/pft_vegc.bin"}},
    { "id" : "phen_tmin",        "file" : { "fmt" : "raw", "name" : "output_pixel/mphen_tmin.bin"}},
    { "id" : "phen_tmax",        "file" : { "fmt" : "raw", "name" : "output_pixel/mphen_tmax.bin"}},
    { "id" : "phen_light",       "file" : { "fmt" : "raw", "name" : "output_pixel/mphen_light.bin"}},
    { "id" : "phen_water",       "file" : { "fmt" : "raw", "name" : "output_pixel/mphen_water.bin"}},
    { "id" : "vegn",             "file" : { "fmt" : "raw", "name" : "output_pixel/vegn.bin"}},
    { "id" : "soiln",            "file" : { "fmt" : "raw", "name" : "output_pixel/soiln.bin"}},
    { "id" : "litn",             "file" : { "fmt" : "raw", "name" : "output_pixel/litn.bin"}},
    { "id" : "soiln_layer",      "file" : { "fmt" : "raw", "name" : "output_pixel/soiln_layer.bin"}},
    { "id" : "soilno3_layer",    "file" : { "fmt" : "raw", "name" : "output_pixel/soilno3_layer.bin"}},
    { "id" : "soilnh4_layer",    "file" : { "fmt" : "raw", "name" : "output_pixel/soilnh4_layer.bin"}},
    { "id" : "soiln_slow",       "file" : { "fmt" : "raw", "name" : "output_pixel/soiln_slow.bin"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "raw", "name" : "output_pixel/soilnh4.bin"}},
    { "id" : "soilno3",          "file" : { "fmt" : "raw", "name" : "output_pixel/soilno3.bin"}},
    { "id" : "pft_nuptake",      "file" : { "fmt" : "raw", "name" : "output_pixel/pft_nuptake.bin"}},
    { "id" : "nuptake",          "file" : { "fmt" : "raw", "name" : "output_pixel/mnuptake.bin"}},
    { "id" : "leaching",         "file" : { "fmt" : "raw", "name" : "output_pixel/mleaching.bin"}},
    { "id" : "n2o_denit",        "file" : { "fmt" : "raw", "name" : "output_pixel/mn2o_denit.bin"}},
    { "id" : "n2o_nit",          "file" : { "fmt" : "raw", "name" : "output_pixel/mn2o_nit.bin"}},
    { "id" : "n2_emis",          "file" : { "fmt" : "raw", "name" : "output_pixel/mn2_emis.bin"}},
    { "id" : "bnf",              "file" : { "fmt" : "raw", "name" : "output_pixel/mbnf.bin"}},
    { "id" : "n_immo",           "file" : { "fmt" : "raw", "name" : "output_pixel/mn_immo.bin"}},
    { "id" : "pft_ndemand",      "file" : { "fmt" : "raw", "name" : "output_pixel/pft_ndemand.bin"}},
    { "id" : "firen",            "file" : { "fmt" : "raw", "name" : "output_pixel/firen.bin"}},
    { "id" : "n_mineralization", "file" : { "fmt" : "raw", "name" : "output_pixel/mn_mineralization.bin"}},
    { "id" : "n_volatilization", "file" : { "fmt" : "raw", "name" : "output_pixel/mn_volatilization.bin"}},
    { "id" : "pft_nlimit",       "file" : { "fmt" : "raw", "name" : "output_pixel/pft_nlimit.bin"}},
    { "id" : "pft_vegn",         "file" : { "fmt" : "raw", "name" : "output_pixel/pft_vegn.bin"}},
    { "id" : "pft_cleaf",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_cleaf.bin"}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_nleaf.bin"}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "raw", "name" : "output_pixel/pft_laimax.bin"}},
    { "id" : "pft_croot",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_croot.bin"}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_nroot.bin"}},
    { "id" : "pft_csapw",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_csapw.bin"}},
    { "id" : "pft_nsapw",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_nsapw.bin"}},
    { "id" : "pft_chawo",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_chawo.bin"}},
    { "id" : "pft_nhawo",        "file" : { "fmt" : "raw", "name" : "output_pixel/pft_nhawo.bin"}},
#ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "fmt" : "raw", "timestep" : "monthly" : "unit" : "gC/m2/month", "name" : "output_pixel/mfirec.bin"}},
    { "id" : "nfire",            "file" : { "fmt" : "raw", "name" : "output_pixel/mnfire.bin"}},
    { "id" : "burntarea",        "file" : { "fmt" : "raw", "name" : "output_pixel/mburnt_area.bin"}},
#else
    { "id" : "firec",            "file" : { "fmt" : "raw", "name" : "output_pixel/firec.bin"}},
#endif
    { "id" : "discharge",        "file" : { "fmt" : "raw", "name" : "output_pixel/mdischarge.bin"}},
    { "id" : "wateramount",      "file" : { "fmt" : "raw", "name" : "output_pixel/mwateramount.bin"}},
    { "id" : "harvestc",         "file" : { "fmt" : "raw", "name" : "output_pixel/flux_harvest.bin"}},
    { "id" : "sdate",            "file" : { "fmt" : "raw", "name" : "output_pixel/sdate.bin"}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "raw", "name" : mkstr(output_pixel/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "raw", "name" : "output_pixel/cftfrac.bin"}},
    { "id" : "seasonality",      "file" : { "fmt" : "raw", "name" : "output_pixel/seasonality.bin"}},
    { "id" : "pet",              "file" : { "fmt" : "raw", "name" : "output_pixel/mpet.bin"}},
    { "id" : "albedo",           "file" : { "fmt" : "raw", "name" : "output_pixel/malbedo.bin"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "raw", "name" : "output_pixel/maxthaw_depth.bin"}},
    { "id" : "perc",             "file" : { "fmt" : "raw", "name" : "output_pixel/mperc.bin"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "raw", "name" : "output_pixel/msoiltemp1.bin"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "raw", "name" : "output_pixel/msoiltemp2.bin"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "raw", "name" : "output_pixel/msoiltemp3.bin"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "raw", "name" : "output_pixel/soilc_layer.bin"}},
    { "id" : "agb",              "file" : { "fmt" : "raw", "name" : "output_pixel/agb.bin"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "raw", "name" : "output_pixel/agb_tree.bin"}},
    { "id" : "return_flow_b",    "file" : { "fmt" : "raw", "name" : "output_pixel/mreturn_flow_b.bin"}},
    { "id" : "transp_b",         "file" : { "fmt" : "raw", "name" : "output_pixel/mtransp_b.bin"}},
    { "id" : "evap_b",           "file" : { "fmt" : "raw", "name" : "output_pixel/mevap_b.bin"}},
    { "id" : "interc_b",         "file" : { "fmt" : "raw", "name" : "output_pixel/mintec_b.bin"}},
    { "id" : "prod_turnover",    "file" : { "fmt" : "raw", "name" : "output_pixel/prod_turnover.bin"}},
    { "id" : "deforest_emis",    "file" : { "fmt" : "raw", "name" : "output_pixel/deforest_emis.bin"}},
    { "id" : "conv_loss_evap",   "file" : { "fmt" : "raw", "name" : "output_pixel/aconv_loss_evap.bin"}},
    { "id" : "conv_loss_drain",  "file" : { "fmt" : "raw", "name" : "output_pixel/aconv_loss_drain.bin"}}
/*------------------------------ ------------------------- ------------------------------- */
  ],

#else

  "output" :
  [
    { "id" : "fpc",              "file" : { "fmt" : "txt", "name" : "output_pixel/fpc.bin" }},
    { "id" : "npp",              "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/anpp.txt"}},
    { "id" : "gpp",              "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/agpp.txt"}},
    { "id" : "rh",               "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/arh.txt"}},
    { "id" : "fapar",            "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/afapar.txt"}},
    { "id" : "transp",           "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/atransp.txt"}},
    { "id" : "runoff",           "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/arunoff.txt"}},
    { "id" : "evap",             "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aevap.txt"}},
    { "id" : "interc",           "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/ainterc.txt"}},
    { "id" : "swc1",             "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aswc1.txt"}},
    { "id" : "swc2",             "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aswc2.txt"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "txt", "name" : "output_pixel/soilnh4.txt"}},
    { "id" : "soilno3",          "file" : { "fmt" : "txt", "name" : "output_pixel/soilno3.txt"}},
    { "id" : "nuptake",          "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/anuptake.txt"}},
    { "id" : "leaching",         "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aleaching.txt"}},
    { "id" : "vegc",             "file" : { "fmt" : "txt", "name" : "output_pixel/vegc.txt"}},
    { "id" : "soilc",            "file" : { "fmt" : "txt", "name" : "output_pixel/soilc.txt"}},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output_pixel/globalflux_spinup.csv"}}
  ],
#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : 14338, /* 27410, 67208 60400 47284 47293 47277 all grid cells */
  "endgrid" : 14338,      /*ALL*/ /*33803 output_pixel hohes soilc*/

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 20000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "outputyear" : -28099,
  "restart" :  false, /* start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "/p/projects/open/sibyll/LPJmL_fixation_test/restart/restart_1840_nv_stdfirep_p.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2011, /* last year of simulation */
  "outputyear": 1901, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire_p.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1900_crop_stdfire_p.lpj", /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
