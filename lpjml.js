/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.7.8                           \n**/
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

//#define BMGR_BROWN    /* enables brown harvest of biomass grass, instead of green harvest (default) */

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
  "version"  : "5.7",       /* LPJmL version expected */
  "global_attrs" : {"institution" : "Potsdam Institute for Climate Impact Research",
                    "contact" : "", /* name and email address */
                    "comment" : ""  /* additional comments */
                   },       /* Global attributes for NetCDF output files */
  "coupled_model" : null,   /* no model coupling */
  "start_coupling": null,   /* Start year of model coupling: null (start_coupling is set to firstyear if coupled_model != null), int (start year of coupling) */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator or "random_seed" */
  "radiation" : "radiation",/* other options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "fire" : "fire",          /* fire disturbance enabled, other options: "no_fire", "fire", "spitfire", "spitfire_tmax" (for GLDAS input data) */
  "fire_on_grassland" : false, /* enable fire on grassland for Spitfire */
  "fdi" : "nesterov",       /* different fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
  "firewood" : false,
  "gsi_phenology" : true,   /* GSI phenology enabled */
  "transp_suction_fcn" : false, /* enable transpiration suction function (true/false) */
  "with_lakes" : true,      /* enable lakes (true/false) */
  "river_routing" : true,
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
  "store_climate" : true, /* store climate data in spin-up phase */
  "landfrac_from_file" : true, /* read cell area from file (true/false) */
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
  "prescribe_landcover" : "no_landcover", /* other options: "no_landcover", "landcoverfpc", "landcoverest" */
  "sowing_date_option" : "fixed_sdate",   /* other options: "no_fixed_sdate", "fixed_sdate", "prescribed_sdate" */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : false,                  /* intercrops on setaside */
  "residue_treatment" : "fixed_residue_remove", /* residue options: "read_residue_data", "no_residue_remove", "fixed_residue_remove" (uses param residues_in_soil) */
  "residues_fire" : false,              /* fire in residuals */
  "irrigation" : "lim",                 /* other options: "no", "lim", "pot", "all" */
  "laimax_interpolate" : "laimax_par",  /* laimax values from manage parameter file, */
                                        /* other options: "laimax_cft", "const_lai_max", "laimax_interpolate" */
  "tillage_type" : "all",               /* Options: "all" (all agr. cells tilled), "no" (no cells tilled) and "read" (tillage dataset used) */
  "till_startyear" : 1850,              /* year in which tillage should start */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "pft_residue" : "temperate cereals",
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "rw_manage" : false,                  /* rain water management */
  "laimax" : 5,                         /* maximum LAI for "const_lai_max" */
  "fertilizer_input" : "yes",           /* enable fertilizer input, other options: "no", "yes", "auto" */
  "manure_input" : true,                /* enable manure input */
  "fix_fertilization" : false,          /* fix fertilizer input */
  "others_to_crop" : true,              /* move PFT type others into PFT crop, cft_tropic for tropical,  cft_temp for temperate */
  "grazing" : "default",                /* default grazing type, other options : "default", "mowing", "ext", "int", "livestock", "none" */
  "grazing_others" : "default",         /* default grazing type for others, other options : "default", "mowing", "ext", "int", "livestock", "none" */
  "cft_temp" : "temperate cereals",
  "cft_tropic" : "maize",
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "luc_timber" : true,                  /* land-use change timber */
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "prescribe_lsuha" : false,
  "mowing_days" : [152, 335],          /* Mowing days for grassland if grass harvest options are ser */
  #ifdef BMGR_BROWN
    "biomass_grass_harvest" : "brown",   /* define brown harvest of biomass grass at top; imapcts harvest event and fn_turnover of biomass grass */
  #else
    "biomass_grass_harvest" : "green",   /* comment out define of brown harvest of biomass grass for green harvest (default) */
  #endif
  "crop_resp_fix" : false,             /* variable C:N ratio for crop respiration */
                                       /* for MAgPIE runs, turn off dynamic C:N ratio dependent respiration,
                                          which reduces yields at high N inputs */
  "crop_phu_option" : "new",
  "cropsheatfrost" : false,
  "double_harvest" : true,
  "npp_controlled_bnf" : true,

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

  "output_metafile" : true,   /* json metafile created (true/false) */
  "default_fmt" : "raw",      /* default format for output files: "raw","txt","cdf","clm","sock" */
  "default_suffix" : ".bin",  /* default file suffix for output files */
  "grid_type" : "short",      /* set datatype of grid file ("short", "float", "double") */
  "absyear" : false,          /* years relative to baseyear (true/false) */
  "rev_lat" : false,          /* reverse latitudes in NetCDF output (true/false) */
  "with_days" : true,         /* use days as units for monthly output in NetCDF files */

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

#ifdef FROM_RESTART

  "output" :
  [

/*
ID                               Fmt                        filename
-------------------------------- ------------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
    { "id" : "terr_area",        "file" : { "name" : "output/terr_area" }},
    { "id" : "land_area",        "file" : { "name" : "output/land_area" }},
    { "id" : "lake_area",        "file" : { "name" : "output/lake_area" }},
    { "id" : "fpc",              "file" : { "name" : "output/fpc" }},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output/globalflux.csv"}},
    { "id" : "npp",              "file" : { "name" : "output/mnpp"}},
    { "id" : "gpp",              "file" : { "name" : "output/mgpp"}},
    { "id" : "rh",               "file" : { "name" : "output/mrh"}},
    { "id" : "rd",               "file" : { "name" : "output/rd"}},
    { "id" : "fapar",            "file" : { "name" : "output/mfapar"}},
    { "id" : "transp",           "file" : { "name" : "output/mtransp"}},
    { "id" : "runoff",           "file" : { "name" : "output/mrunoff"}},
    { "id" : "evap",             "file" : { "name" : "output/mevap"}},
    { "id" : "interc",           "file" : { "name" : "output/minterc"}},
    { "id" : "swc1",             "file" : { "name" : "output/mswc1"}},
    { "id" : "swc2",             "file" : { "name" : "output/mswc2"}},
    { "id" : "firef",            "file" : { "name" : "output/firef"}},
    { "id" : "vegc",             "file" : { "name" : "output/vegc"}},
    { "id" : "soilc",            "file" : { "name" : "output/soilc"}},
    { "id" : "litc",             "file" : { "name" : "output/litc"}},
    { "id" : "flux_estabc",      "file" : { "name" : "output/flux_estab"}},
    { "id" : "pft_vegc",         "file" : { "name" : "output/pft_vegc"}},
    { "id" : "phen_tmin",        "file" : { "name" : "output/mphen_tmin"}},
    { "id" : "phen_tmax",        "file" : { "name" : "output/mphen_tmax"}},
    { "id" : "phen_light",       "file" : { "name" : "output/mphen_light"}},
    { "id" : "phen_water",       "file" : { "name" : "output/mphen_water"}},
    { "id" : "vegn",             "file" : { "name" : "output/vegn"}},
    { "id" : "soiln",            "file" : { "name" : "output/soiln"}},
    { "id" : "litn",             "file" : { "name" : "output/litn"}},
    { "id" : "soiln_layer",      "file" : { "name" : "output/soiln_layer"}},
    { "id" : "soilno3_layer",    "file" : { "name" : "output/soilno3_layer"}},
    { "id" : "soilnh4_layer",    "file" : { "name" : "output/soilnh4_layer"}},
    { "id" : "soiln_slow",       "file" : { "name" : "output/soiln_slow"}},
    { "id" : "soilnh4",          "file" : { "name" : "output/soilnh4"}},
    { "id" : "soilno3",          "file" : { "name" : "output/soilno3"}},
    { "id" : "pft_nuptake",      "file" : { "name" : "output/pft_nuptake"}},
    { "id" : "nuptake",          "file" : { "name" : "output/mnuptake"}},
    { "id" : "leaching",         "file" : { "name" : "output/mleaching"}},
    { "id" : "n2o_denit",        "file" : { "name" : "output/mn2o_denit"}},
    { "id" : "n2o_nit",          "file" : { "name" : "output/mn2o_nit"}},
    { "id" : "n2_emis",          "file" : { "name" : "output/mn2_emis"}},
    { "id" : "bnf",              "file" : { "name" : "output/mbnf"}},
    { "id" : "n_immo",           "file" : { "name" : "output/mn_immo"}},
    { "id" : "pft_ndemand",      "file" : { "name" : "output/pft_ndemand"}},
    { "id" : "nfert_agr",        "file" : { "name" : "output/nfert_agr"}},
    { "id" : "firen",            "file" : { "name" : "output/firen"}},
    { "id" : "n_mineralization", "file" : { "name" : "output/mn_mineralization"}},
    { "id" : "n_volatilization", "file" : { "name" : "output/mn_volatilization"}},
    { "id" : "ndepos",           "file" : { "name" : "output/ndepos"}},
    { "id" : "pft_nlimit",       "file" : { "name" : "output/pft_nlimit"}},
    { "id" : "pft_vegn",         "file" : { "name" : "output/pft_vegn"}},
    { "id" : "pft_cleaf",        "file" : { "name" : "output/pft_cleaf"}},
    { "id" : "pft_nleaf",        "file" : { "name" : "output/pft_nleaf"}},
    { "id" : "pft_laimax",       "file" : { "name" : "output/pft_laimax"}},
    { "id" : "pft_croot",        "file" : { "name" : "output/pft_croot"}},
    { "id" : "pft_nroot",        "file" : { "name" : "output/pft_nroot"}},
    { "id" : "pft_csapw",        "file" : { "name" : "output/pft_csapw"}},
    { "id" : "pft_nsapw",        "file" : { "name" : "output/pft_nsapw"}},
    { "id" : "pft_chawo",        "file" : { "name" : "output/pft_chawo"}},
    { "id" : "pft_nhawo",        "file" : { "name" : "output/pft_nhawo"}},
    { "id" : "pft_water_demand", "file" : { "name" : "output/pft_water_demand"}},
#ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec"}},
    { "id" : "nfire",            "file" : { "name" : "output/mnfire"}},
    { "id" : "burntarea",        "file" : { "name" : "output/mburnt_area"}},
#else
    { "id" : "firec",            "file" : { "name" : "output/firec"}},
#endif
    { "id" : "discharge",        "file" : { "name" : "output/mdischarge"}},
    { "id" : "wateramount",      "file" : { "name" : "output/mwateramount"}},
    { "id" : "harvestc",         "file" : { "name" : "output/flux_harvest"}},
    { "id" : "sdate",            "file" : { "name" : "output/sdate"}},
    { "id" : "pft_harvestc",     "file" : { "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "name" : "output/cftfrac"}},
    { "id" : "seasonality",      "file" : { "name" : "output/seasonality"}},
    { "id" : "pet",              "file" : { "name" : "output/mpet"}},
    { "id" : "albedo",           "file" : { "name" : "output/malbedo"}},
    { "id" : "maxthaw_depth",    "file" : { "name" : "output/maxthaw_depth"}},
    { "id" : "perc",             "file" : { "name" : "output/mperc"}},
    { "id" : "soiltemp1",        "file" : { "name" : "output/msoiltemp1"}},
    { "id" : "soiltemp2",        "file" : { "name" : "output/msoiltemp2"}},
    { "id" : "soiltemp3",        "file" : { "name" : "output/msoiltemp3"}},
    { "id" : "soilc_layer",      "file" : { "name" : "output/soilc_layer"}},
    { "id" : "agb",              "file" : { "name" : "output/agb"}},
    { "id" : "agb_tree",         "file" : { "name" : "output/agb_tree"}},
    { "id" : "return_flow_b",    "file" : { "name" : "output/mreturn_flow_b"}},
    { "id" : "transp_b",         "file" : { "name" : "output/mtransp_b"}},
    { "id" : "evap_b",           "file" : { "name" : "output/mevap_b"}},
    { "id" : "interc_b",         "file" : { "name" : "output/mintec_b"}},
    { "id" : "prod_turnover",    "file" : { "name" : "output/prod_turnover"}},
    { "id" : "deforest_emis",    "file" : { "name" : "output/deforest_emis"}},
    { "id" : "conv_loss_evap",   "file" : { "name" : "output/aconv_loss_evap"}},
    { "id" : "conv_loss_drain",  "file" : { "name" : "output/aconv_loss_drain"}}
/*------------------------------ ------------------------- ------------------------------- */
  ],

#else

  "output" :
  [
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output/globalflux_spinup.csv"}}
  ],
#endif

/*===================================================================*/
/*  V. Run settings section                                          */
/*===================================================================*/

  "startgrid" : "all", /* 27410, 67208 60400 47284 47293 47277 all grid cells */
  "endgrid"   : "all",
#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifndef FROM_RESTART

  "nspinup" : 3500,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 1901, /* last year of simulation */
  "restart" :  false, /* start from restart file */
  "outputyear" : -1599,
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2011, /* last year of simulation */
  "outputyear": 1901, /* first year output is written  */
  "baseyear": 1901, /* base year for output  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
