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
  "coupled_model" : null,   /* Coupled model: null (no model coupled), string (name of coupled model) */
  "start_coupling": null,   /* Start year of model coupling: null (start_coupling is set to firstyear if coupled_model != null), int (start year of coupling) */
  "version"  : "6.0",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fire_on_grassland" : false, /* enable fire on grassland for Spitfire */
  "fdi" : "nesterov",       /* different fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
  "firewood" : false,
  "gsi_phenology" : true,   /* GSI phenology enabled */
  "transp_suction_fcn" : false, /* enable transpiration suction function (true/false) */
  "river_routing" : false,
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "dynamic_CH4" : false,
  "anomaly" : false,
  "soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "soilpar_fixyear" : 1900,
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
  "store_climate" : true, /* store climate data in spin-up phase */
  "shuffle_spinup_climate" : true, /* shuffle spinup climate */
  "fix_climate" : false,                /* fix climate after specified year */
  "fix_climate_year" : 1901,            /* year after climate is fixed */
  "fix_climate_interval" : [1901,1930],
  "fix_climate_shuffle" : true,          /* randomly shuffle climate in the interval */
  "fix_deposition_with_climate" : false, /* fix N deposition same as climate  */
  "fix_deposition" : false,              /* fix N deposition after specified year */
  "fix_deposition_year" : 1901,          /* year after deposition is fixed */
  "fix_deposition_interval" : [1901,1930],
  "fix_deposition_shuffle" : true,       /* randomly shuffle depositions in the interval */
  "fix_landuse" : false,                 /* fix land use after specfied year */
  "fix_landuse_year" : 1901,             /* year after land use is fixed */
  "fix_co2" : false,                     /* fix atmospheric CO2  after specfied year */
  "fix_co2_year" : 1901,                 /* year after CO2 is fixed */
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "population" : false,
  "landuse" : "yes", /* other options: "no", "yes", "const", "all_crops", "only_crops" */
  "landuse_year_const" : 1910, /* set landuse year for "const" case */
  "reservoir" : false,
  "wateruse" : "no",  /* other options: "no", "yes", "all" */
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
  "irrigation" : "pot",                 /* other options: "no", "lim", "pot", "all" */
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
  "luc_timber" : true,
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
  "npp_controlled_bnf" : true,                    /* Biological N fixation using Cleveland, 1999 (false) or Ma et al., 2022 (true) approach*/

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
    { "id" : "grid",             "file" : { "fmt" : "txt", "name" : "output_pixel/grid.bin" }},
    { "id" : "fpc",              "file" : { "fmt" : "txt", "name" : "output_pixel/fpc.txt" }},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output_pixel/globalflux2.csv"}},
    { "id" : "npp",              "file" : { "fmt" : "txt", "name" : "output_pixel/mnpp.txt"}},
    { "id" : "gpp",              "file" : { "fmt" : "txt", "name" : "output_pixel/mgpp.txt"}},
    { "id" : "rh",               "file" : { "fmt" : "txt", "name" : "output_pixel/mrh.txt"}},
    { "id" : "fapar",            "file" : { "fmt" : "txt", "name" : "output_pixel/mfapar.txt"}},
    { "id" : "transp",           "file" : { "fmt" : "txt", "name" : "output_pixel/mtransp.txt"}},
    { "id" : "runoff",           "file" : { "fmt" : "txt", "name" : "output_pixel/mrunoff.txt"}},
    { "id" : "evap",             "file" : { "fmt" : "txt", "name" : "output_pixel/mevap.txt"}},
    { "id" : "interc",           "file" : { "fmt" : "txt", "name" : "output_pixel/minterc.txt"}},
    { "id" : "swc1",             "file" : { "fmt" : "txt", "name" : "output_pixel/mswc1.txt"}},
    { "id" : "swc2",             "file" : { "fmt" : "txt", "name" : "output_pixel/mswc2.txt"}},
    { "id" : "firef",            "file" : { "fmt" : "txt", "name" : "output_pixel/firef.txt"}},
    { "id" : "vegc",             "file" : { "fmt" : "txt", "name" : "output_pixel/vegc.txt"}},
    { "id" : "soilc",            "file" : { "fmt" : "txt", "name" : "output_pixel/soilc.txt"}},
    { "id" : "litc",             "file" : { "fmt" : "txt", "name" : "output_pixel/litc.txt"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "txt", "name" : "output_pixel/flux_estab.txt"}},
    { "id" : "pft_vegc",         "file" : { "fmt" : "txt", "name" : "output_pixel/pft_vegc.txt"}},
    { "id" : "phen_tmin",        "file" : { "fmt" : "txt", "name" : "output_pixel/mphen_tmin.txt"}},
    { "id" : "phen_tmax",        "file" : { "fmt" : "txt", "name" : "output_pixel/mphen_tmax.txt"}},
    { "id" : "phen_light",       "file" : { "fmt" : "txt", "name" : "output_pixel/mphen_light.txt"}},
    { "id" : "phen_water",       "file" : { "fmt" : "txt", "name" : "output_pixel/mphen_water.txt"}},
    { "id" : "vegn",             "file" : { "fmt" : "txt", "name" : "output_pixel/vegn.txt"}},
    { "id" : "soiln",            "file" : { "fmt" : "txt", "name" : "output_pixel/soiln.txt"}},
    { "id" : "litn",             "file" : { "fmt" : "txt", "name" : "output_pixel/litn.txt"}},
    { "id" : "soiln_layer",      "file" : { "fmt" : "txt", "name" : "output_pixel/soiln_layer.txt"}},
    { "id" : "soilno3_layer",    "file" : { "fmt" : "txt", "name" : "output_pixel/soilno3_layer.txt"}},
    { "id" : "soilnh4_layer",    "file" : { "fmt" : "txt", "name" : "output_pixel/soilnh4_layer.txt"}},
    { "id" : "soiln_slow",       "file" : { "fmt" : "txt", "name" : "output_pixel/soiln_slow.txt"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "txt", "name" : "output_pixel/soilnh4.txt"}},
    { "id" : "soilno3",          "file" : { "fmt" : "txt", "name" : "output_pixel/soilno3.txt"}},
    { "id" : "pft_nuptake",      "file" : { "fmt" : "txt", "name" : "output_pixel/pft_nuptake.txt"}},
    { "id" : "nuptake",          "file" : { "fmt" : "txt", "name" : "output_pixel/mnuptake.txt"}},
    { "id" : "leaching",         "file" : { "fmt" : "txt", "name" : "output_pixel/mleaching.txt"}},
    { "id" : "n2o_denit",        "file" : { "fmt" : "txt", "name" : "output_pixel/mn2o_denit.txt"}},
    { "id" : "n2o_nit",          "file" : { "fmt" : "txt", "name" : "output_pixel/mn2o_nit.txt"}},
    { "id" : "n2_emis",          "file" : { "fmt" : "txt", "name" : "output_pixel/mn2_emis.txt"}},
    { "id" : "bnf",              "file" : { "fmt" : "txt", "name" : "output_pixel/mbnf.txt"}},
    { "id" : "n_immo",           "file" : { "fmt" : "txt", "name" : "output_pixel/mn_immo.txt"}},
    { "id" : "pft_ndemand",      "file" : { "fmt" : "txt", "name" : "output_pixel/pft_ndemand.txt"}},
    { "id" : "firen",            "file" : { "fmt" : "txt", "name" : "output_pixel/firen.txt"}},
    { "id" : "n_mineralization", "file" : { "fmt" : "txt", "name" : "output_pixel/mn_mineralization.txt"}},
    { "id" : "n_volatilization", "file" : { "fmt" : "txt", "name" : "output_pixel/mn_volatilization.txt"}},
    { "id" : "pft_nlimit",       "file" : { "fmt" : "txt", "name" : "output_pixel/pft_nlimit.txt"}},
    { "id" : "pft_vegn",         "file" : { "fmt" : "txt", "name" : "output_pixel/pft_vegn.txt"}},
    { "id" : "pft_cleaf",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_cleaf.txt"}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_nleaf.txt"}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "txt", "name" : "output_pixel/pft_laimax.txt"}},
    { "id" : "pft_croot",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_croot.txt"}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_nroot.txt"}},
    { "id" : "pft_csapw",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_csapw.txt"}},
    { "id" : "pft_nsapw",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_nsapw.txt"}},
    { "id" : "pft_chawo",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_chawo.txt"}},
    { "id" : "pft_nhawo",        "file" : { "fmt" : "txt", "name" : "output_pixel/pft_nhawo.txt"}},
#ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "fmt" : "txt", "timestep" : "monthly" : "unit" : "gC/m2/month", "name" : "output_pixel/mfirec.txt"}},
    { "id" : "nfire",            "file" : { "fmt" : "txt", "name" : "output_pixel/mnfire.txt"}},
    { "id" : "burntarea",        "file" : { "fmt" : "txt", "name" : "output_pixel/mburnt_area.txt"}},
#else
    { "id" : "firec",            "file" : { "fmt" : "txt", "name" : "output_pixel/firec.txt"}},
#endif
    { "id" : "discharge",        "file" : { "fmt" : "txt", "name" : "output_pixel/mdischarge.txt"}},
    { "id" : "wateramount",      "file" : { "fmt" : "txt", "name" : "output_pixel/mwateramount.txt"}},
    { "id" : "harvestc",         "file" : { "fmt" : "txt", "name" : "output_pixel/flux_harvest.txt"}},
    { "id" : "sdate",            "file" : { "fmt" : "txt", "name" : "output_pixel/sdate.txt"}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "txt", "name" : mkstr(output_pixel/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "txt", "name" : "output_pixel/cftfrac.txt"}},
    { "id" : "seasonality",      "file" : { "fmt" : "txt", "name" : "output_pixel/seasonality.txt"}},
    { "id" : "pet",              "file" : { "fmt" : "txt", "name" : "output_pixel/mpet.txt"}},
    { "id" : "albedo",           "file" : { "fmt" : "txt", "name" : "output_pixel/malbedo.txt"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "txt", "name" : "output_pixel/maxthaw_depth.txt"}},
    { "id" : "perc",             "file" : { "fmt" : "txt", "name" : "output_pixel/mperc.txt"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "txt", "name" : "output_pixel/msoiltemp1.txt"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "txt", "name" : "output_pixel/msoiltemp2.txt"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "txt", "name" : "output_pixel/msoiltemp3.txt"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "txt", "name" : "output_pixel/soilc_layer.txt"}},
    { "id" : "agb",              "file" : { "fmt" : "txt", "name" : "output_pixel/agb.txt"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "txt", "name" : "output_pixel/agb_tree.txt"}},
    { "id" : "return_flow_b",    "file" : { "fmt" : "txt", "name" : "output_pixel/mreturn_flow_b.txt"}},
    { "id" : "transp_b",         "file" : { "fmt" : "txt", "name" : "output_pixel/mtransp_b.txt"}},
    { "id" : "evap_b",           "file" : { "fmt" : "txt", "name" : "output_pixel/mevap_b.txt"}},
    { "id" : "interc_b",         "file" : { "fmt" : "txt", "name" : "output_pixel/mintec_b.txt"}},
    { "id" : "prod_turnover",    "file" : { "fmt" : "txt", "name" : "output_pixel/prod_turnover.txt"}},
    { "id" : "deforest_emis",    "file" : { "fmt" : "txt", "name" : "output_pixel/deforest_emis.txt"}},
    { "id" : "conv_loss_evap",   "file" : { "fmt" : "txt", "name" : "output_pixel/aconv_loss_evap.txt"}},
    { "id" : "conv_loss_drain",  "file" : { "fmt" : "txt", "name" : "output_pixel/aconv_loss_drain.txt"}},
    { "id" : "wetfrac",          "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/wetfrac2.txt"}},
    { "id" : "ch4_plant_gas",    "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/ch4_plant_gras_2.txt"}},
    { "id" : "ch4_emissions",    "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/ch4_emissions_2.txt"}},
    { "id" : "ch4_sink",         "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/ch4_sink_2.txt"}},
    { "id" : "ch4_ebullition",   "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/ch4_ebullition_2.txt"}},
    { "id" : "meansoilo2",       "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/meansoilo2_2.txt"}},
    { "id" : "meansoilch4",      "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/meansoilch4_2.txt"}}
    
/*------------------------------ ------------------------- ------------------------------- */
  ],

#else

  "output" :
  [
    { "id" : "fpc",              "file" : { "fmt" : "txt", "name" : "output_pixel/fpc.bin" }},
    { "id" : "npp",              "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/anpp_spinup.txt"}},
    { "id" : "gpp",              "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/agpp_spinup.txt"}},
    { "id" : "rh",               "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/arh_spinup.txt"}},
    { "id" : "fapar",            "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/afapar_spinup.txt"}},
    { "id" : "transp",           "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/atransp_spinup.txt"}},
    { "id" : "runoff",           "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/arunoff_spinup.txt"}},
    { "id" : "evap",             "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aevap_spinup.txt"}},
    { "id" : "interc",           "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/ainterc_spinup.txt"}},
    { "id" : "swc1",             "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aswc1_spinup.txt"}},
    { "id" : "swc2",             "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aswc2._spinuptxt"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "txt", "name" : "output_pixel/soilnh4_spinup.txt"}},
    { "id" : "soilno3",          "file" : { "fmt" : "txt", "name" : "output_pixel/soilno3_spinup.txt"}},
    { "id" : "nuptake",          "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/anuptake_spinup.txt"}},
    { "id" : "leaching",         "file" : { "fmt" : "txt", "timestep" : "annual", "name" : "output_pixel/aleaching_spinup.txt"}},
    { "id" : "vegc",             "file" : { "fmt" : "txt", "name" : "output_pixel/vegc_spinup.txt"}},
    { "id" : "soilc",            "file" : { "fmt" : "txt", "name" : "output_pixel/soilc_spinup.txt"}},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output_pixel/globalflux_spinup.csv"}}
  ],
#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : 8097, /*58463 13580  4276 14310 18175 49217 39049 28854 27410, 67208 60400 47284 47293 47277 8097 all grid cells */
  "endgrid" : 8097,      /*ALL*/ /*33803 output_pixel hohes soilc*/

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 5000,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "outputyear" : -28099,
  "restart" :  false, /* start from restart file */
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart_pixel/restart_1840_nv_stdfire_p.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2011, /* last year of simulation */
  "outputyear": 1841, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart_pixel/restart_1840_nv_stdfire_p.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart_pixel/restart_1900_crop_stdfire_p.lpj", /* filename of restart file */
  "restart_year": 1841 /* write restart at year */

#endif
}
