/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
<<<<<<< HEAD
/** Default configuration file for LPJmL C Version 6.0.001                         \n**/
=======
/** Default configuration file for LPJmL C Version 5.6.25                          \n**/
>>>>>>> master
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

{   /* LPJmL configuration in JSON format */

/*===================================================================*/
/*  I. Simulation description and type section                       */
/*===================================================================*/

  "sim_name" : "LPJmL Run", /* Simulation description */
  "sim_id"   : "lpjml",     /* LPJML Simulation type with managed land use */
<<<<<<< HEAD
  "version"  : "6.0",       /* LPJmL version expected */
=======
  "coupled_model" : null,   /* Coupled model: null (no model coupled), string (name of coupled model) */
  "start_coupling": null,   /* Start year of model coupling: null (start_coupling is set to firstyear if coupled_model != null), int (start year of coupling) */
  "version"  : "5.6",       /* LPJmL version expected */
>>>>>>> master
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* other options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "fire" : "fire",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fire_on_grassland" : false, /* enable fire on grassland for Spitfire */
  "fdi" : "nesterov",       /* different fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
  "firewood" : false,
<<<<<<< HEAD
  "new_phenology" : true,   /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "river_routing" : false,
=======
  "gsi_phenology" : true,   /* GSI phenology enabled */
  "transp_suction_fcn" : false, /* enable transpiration suction function (true/false) */
  "river_routing" : true,
>>>>>>> master
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "dynamic_CH4" : false,
  "anomaly" : false,
  "soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
  "nitrogen_coupled" : true, /* nitrogen stress coupled to water stress */ 
  "store_climate" : true, /* store climate data in spin-up phase */
<<<<<<< HEAD
  "const_climate" : false,
  "shuffle_climate" : true, /* shuffle spinup climate */
  "const_deposition" : false,
  "depos_year_const" : 1901,
  "fix_climate" : false,
  "fix_landuse" : false,
=======
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
>>>>>>> master
#ifdef FROM_RESTART
  "new_seed" : false, /* read random seed from restart file */
  "population" : false,
  "landuse" : "yes", /* other options: "no", "yes", "const", "all_crops", "only_crops" */
  "landuse_year_const" : 2100, /* set landuse year for "const" case */
  "reservoir" : false,
  "wateruse" : "no",  /* other options: "no", "yes", "all" */
  "equilsoil" : false,
#else
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
  "equilsoil" : false, 
#endif
  "prescribe_burntarea" : false,
  "prescribe_landcover" : "no_landcover", /* NO_LANDCOVER, LANDCOVERFPC, LANDCOVEREST */
  "sowing_date_option" : "fixed_sdate",   /* NO_FIXED_SDATE, FIXED_SDATE, PRESCRIBED_SDATE */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed */
  "intercrop" : false,                  /* intercrops on setaside */
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
  "grazing_others" : "default",         /* default grazing type for others, other options : "default", "mowing", "ext", "int", "livestock", "none" */
  "cft_temp" : "temperate cereals",
  "cft_tropic" : "maize",
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "luc_timber" : true,                  /* land-use change timber */
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
<<<<<<< HEAD
  "ma_bnf" : true,                    /* Biological N fixation using Cleveland, 1999 (false) or Ma et al., 2022 (true) approach
=======
  "npp_controlled_bnf" : true,
>>>>>>> master

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
#define SUFFIX grid.nc
#else
  "grid_scaled" : false,
#define SUFFIX pft.nc
#endif

  "output_metafile" : true, /* no json metafile created */
  "float_grid" : false,      /* set datatype of grid file to float (TRUE/FALSE) */

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

#ifdef FROM_RESTART

  "output" :
  [

/*
ID                               Fmt                        filename
-------------------------------- ------------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "cdf", "name" : "output/grid.nc" }},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output/globalflux.csv"}},
<<<<<<< HEAD
    { "id" : "fpc",              "file" : { "fmt" : "cdf", "name" : "output/fpc.nc"}},
    { "id" : "wpc",              "file" : { "fmt" : "cdf", "name" : "output/wpc.nc"}},
    { "id" : "npp",              "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/npp.nc"}},
    { "id" : "gpp",              "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/gpp.nc"}},
    { "id" : "rh",               "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/rh.nc"}},
    { "id" : "transp",           "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/transp.nc"}},
    { "id" : "runoff",           "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/runoff.nc"}},
    { "id" : "evap",             "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/evap.nc"}},
    { "id" : "interc",           "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/interc.nc"}},
    { "id" : "swc1",             "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/swc1.nc"}},
    { "id" : "swc2",             "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/swc2.nc"}},
    { "id" : "vegc",             "file" : { "fmt" : "cdf", "name" : "output/vegc.nc"}},
    { "id" : "soilc",            "file" : { "fmt" : "cdf", "name" : "output/soilc.nc"}},
    { "id" : "litc",             "file" : { "fmt" : "cdf", "name" : "output/litc.nc"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "cdf", "name" : "output/flux_estab.nc"}},
    { "id" : "pft_vegc",         "file" : { "fmt" : "cdf", "name" : "output/pft_vegc.nc"}},
    { "id" : "vegn",             "file" : { "fmt" : "cdf", "name" : "output/vegn.nc"}},
    { "id" : "soiln",            "file" : { "fmt" : "cdf", "name" : "output/soiln.nc"}},
    { "id" : "litn",             "file" : { "fmt" : "cdf", "name" : "output/litn.nc"}},
    { "id" : "soiln_layer",      "file" : { "fmt" : "cdf", "name" : "output/soiln_layer.nc"}},
    { "id" : "soilno3_layer",    "file" : { "fmt" : "cdf", "name" : "output/soilno3_layer.nc"}},
    { "id" : "soilnh4_layer",    "file" : { "fmt" : "cdf", "name" : "output/soilnh4_layer.nc"}},
    { "id" : "soiln_slow",       "file" : { "fmt" : "cdf", "name" : "output/soiln_slow.nc"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "cdf", "name" : "output/soilnh4.nc"}},
    { "id" : "soilno3",          "file" : { "fmt" : "cdf", "name" : "output/soilno3.nc"}},
    { "id" : "bnf",              "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/bnf.nc"}},
    { "id" : "leaching",         "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/leaching.nc"}},
    { "id" : "nuptake",          "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/nuptake.nc"}}, 
    { "id" : "cftfrac",          "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/cftfrac.nc"}}, 
/*    { "id" : "pft_nuptake",      "file" : { "fmt" : "cdf", "name" : "output/pft_nuptake.nc"}},
    { "id" : "n2o_denit",        "file" : { "fmt" : "cdf", "name" : "output/mn2o_denit.nc"}},
    { "id" : "n2o_nit",          "file" : { "fmt" : "cdf", "name" : "output/mn2o_nit.nc"}},
    { "id" : "n2_emis",          "file" : { "fmt" : "cdf", "name" : "output/mn2_emis.nc"}},
    { "id" : "n_immo",           "file" : { "fmt" : "cdf", "name" : "output/mn_immo.nc"}},
    { "id" : "pft_ndemand",      "file" : { "fmt" : "cdf", "name" : "output/pft_ndemand.nc"}},
    { "id" : "firen",            "file" : { "fmt" : "cdf", "name" : "output/firen.nc"}},
    { "id" : "n_mineralization", "file" : { "fmt" : "cdf", "name" : "output/mn_mineralization.nc"}},
    { "id" : "n_volatilization", "file" : { "fmt" : "cdf", "name" : "output/mn_volatilization.nc"}},
    { "id" : "pft_nlimit",       "file" : { "fmt" : "cdf", "name" : "output/pft_nlimit.nc"}},
 */
    { "id" : "pft_vegn",         "file" : { "fmt" : "cdf", "name" : "output/pft_vegn.nc"}},
/*    { "id" : "pft_cleaf",        "file" : { "fmt" : "cdf", "name" : "output/pft_cleaf.nc"}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "cdf", "name" : "output/pft_nleaf.nc"}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "cdf", "name" : "output/pft_laimax.nc"}},
    { "id" : "pft_croot",        "file" : { "fmt" : "cdf", "name" : "output/pft_croot.nc"}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "cdf", "name" : "output/pft_nroot.nc"}},
    { "id" : "pft_csapw",        "file" : { "fmt" : "cdf", "name" : "output/pft_csapw.nc"}},
    { "id" : "pft_nsapw",        "file" : { "fmt" : "cdf", "name" : "output/pft_nsapw.nc"}},
    { "id" : "pft_chawo",        "file" : { "fmt" : "cdf", "name" : "output/pft_chawo.nc"}},
    { "id" : "pft_nhawo",        "file" : { "fmt" : "cdf", "name" : "output/pft_nhawo.nc"}},
*/
    { "id" : "wtab",             "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/wtab.nc"}},
    { "id" : "mwater",           "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/mwater.nc"}},
    { "id" : "wetfrac",          "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/wetfrac.nc"}},

=======
    { "id" : "npp",              "file" : { "fmt" : "raw", "name" : "output/mnpp.bin"}},
    { "id" : "gpp",              "file" : { "fmt" : "raw", "name" : "output/mgpp.bin"}},
    { "id" : "rh",               "file" : { "fmt" : "raw", "name" : "output/mrh.bin"}},
    { "id" : "fapar",            "file" : { "fmt" : "raw", "name" : "output/mfapar.bin"}},
    { "id" : "transp",           "file" : { "fmt" : "raw", "name" : "output/mtransp.bin"}},
    { "id" : "runoff",           "file" : { "fmt" : "raw", "name" : "output/mrunoff.bin"}},
    { "id" : "evap",             "file" : { "fmt" : "raw", "name" : "output/mevap.bin"}},
    { "id" : "interc",           "file" : { "fmt" : "raw", "name" : "output/minterc.bin"}},
    { "id" : "swc1",             "file" : { "fmt" : "raw", "name" : "output/mswc1.bin"}},
    { "id" : "swc2",             "file" : { "fmt" : "raw", "name" : "output/mswc2.bin"}},
    { "id" : "firef",            "file" : { "fmt" : "raw", "name" : "output/firef.bin"}},
    { "id" : "vegc",             "file" : { "fmt" : "raw", "name" : "output/vegc.bin"}},
    { "id" : "soilc",            "file" : { "fmt" : "raw", "name" : "output/soilc.bin"}},
    { "id" : "litc",             "file" : { "fmt" : "raw", "name" : "output/litc.bin"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "raw", "name" : "output/flux_estab.bin"}},
    { "id" : "pft_vegc",         "file" : { "fmt" : "raw", "name" : "output/pft_vegc.bin"}},
    { "id" : "phen_tmin",        "file" : { "fmt" : "raw", "name" : "output/mphen_tmin.bin"}},
    { "id" : "phen_tmax",        "file" : { "fmt" : "raw", "name" : "output/mphen_tmax.bin"}},
    { "id" : "phen_light",       "file" : { "fmt" : "raw", "name" : "output/mphen_light.bin"}},
    { "id" : "phen_water",       "file" : { "fmt" : "raw", "name" : "output/mphen_water.bin"}},
    { "id" : "vegn",             "file" : { "fmt" : "raw", "name" : "output/vegn.bin"}},
    { "id" : "soiln",            "file" : { "fmt" : "raw", "name" : "output/soiln.bin"}},
    { "id" : "litn",             "file" : { "fmt" : "raw", "name" : "output/litn.bin"}},
    { "id" : "soiln_layer",      "file" : { "fmt" : "raw", "name" : "output/soiln_layer.bin"}},
    { "id" : "soilno3_layer",    "file" : { "fmt" : "raw", "name" : "output/soilno3_layer.bin"}},
    { "id" : "soilnh4_layer",    "file" : { "fmt" : "raw", "name" : "output/soilnh4_layer.bin"}},
    { "id" : "soiln_slow",       "file" : { "fmt" : "raw", "name" : "output/soiln_slow.bin"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "raw", "name" : "output/soilnh4.bin"}},
    { "id" : "soilno3",          "file" : { "fmt" : "raw", "name" : "output/soilno3.bin"}},
    { "id" : "pft_nuptake",      "file" : { "fmt" : "raw", "name" : "output/pft_nuptake.bin"}},
    { "id" : "nuptake",          "file" : { "fmt" : "raw", "name" : "output/mnuptake.bin"}},
    { "id" : "leaching",         "file" : { "fmt" : "raw", "name" : "output/mleaching.bin"}},
    { "id" : "n2o_denit",        "file" : { "fmt" : "raw", "name" : "output/mn2o_denit.bin"}},
    { "id" : "n2o_nit",          "file" : { "fmt" : "raw", "name" : "output/mn2o_nit.bin"}},
    { "id" : "n2_emis",          "file" : { "fmt" : "raw", "name" : "output/mn2_emis.bin"}},
    { "id" : "bnf",              "file" : { "fmt" : "raw", "name" : "output/mbnf.bin"}},
    { "id" : "n_immo",           "file" : { "fmt" : "raw", "name" : "output/mn_immo.bin"}},
    { "id" : "pft_ndemand",      "file" : { "fmt" : "raw", "name" : "output/pft_ndemand.bin"}},
    { "id" : "nfert_agr",      "file" : { "fmt" : "raw", "name" : "output/nfert_agr.bin"}},
    { "id" : "firen",            "file" : { "fmt" : "raw", "name" : "output/firen.bin"}},
    { "id" : "n_mineralization", "file" : { "fmt" : "raw", "name" : "output/mn_mineralization.bin"}},
    { "id" : "n_volatilization", "file" : { "fmt" : "raw", "name" : "output/mn_volatilization.bin"}},
    { "id" : "pft_nlimit",       "file" : { "fmt" : "raw", "name" : "output/pft_nlimit.bin"}},
    { "id" : "pft_vegn",         "file" : { "fmt" : "raw", "name" : "output/pft_vegn.bin"}},
    { "id" : "pft_cleaf",        "file" : { "fmt" : "raw", "name" : "output/pft_cleaf.bin"}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "raw", "name" : "output/pft_nleaf.bin"}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "raw", "name" : "output/pft_laimax.bin"}},
    { "id" : "pft_croot",        "file" : { "fmt" : "raw", "name" : "output/pft_croot.bin"}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "raw", "name" : "output/pft_nroot.bin"}},
    { "id" : "pft_csapw",        "file" : { "fmt" : "raw", "name" : "output/pft_csapw.bin"}},
    { "id" : "pft_nsapw",        "file" : { "fmt" : "raw", "name" : "output/pft_nsapw.bin"}},
    { "id" : "pft_chawo",        "file" : { "fmt" : "raw", "name" : "output/pft_chawo.bin"}},
    { "id" : "pft_nhawo",        "file" : { "fmt" : "raw", "name" : "output/pft_nhawo.bin"}},
>>>>>>> master
#ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "fmt" : "cdf", "timestep" : "monthly", "unit" : "gC/m2/month", "name" : "output/mfirec.nc"}},
    { "id" : "nfire",            "file" : { "fmt" : "cdf", "name" : "output/mnfire.nc"}},
    { "id" : "burntarea",        "file" : { "fmt" : "cdf", "name" : "output/mburnt_area.nc"}},
#else
    { "id" : "firec",            "file" : { "fmt" : "cdf", "name" : "output/firec.nc"}},
#endif

    { "id" : "discharge",        "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/discharge.nc"}},
    { "id" : "harvestc",         "file" : { "fmt" : "cdf", "name" : "output/flux_harvest.nc"}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "cdf", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "pet",              "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/pet.nc"}},
    { "id" : "albedo",           "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/albedo.nc"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "cdf", "name" : "output/maxthaw_depth.nc"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/soiltemp1.nc"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/soiltemp2.nc"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/soiltemp3.nc"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "cdf", "name" : "output/soilc_layer.nc"}},
    { "id" : "agb",              "file" : { "fmt" : "cdf", "name" : "output/agb.nc"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "cdf", "name" : "output/agb_tree.nc"}},
    { "id" : "soilc_slow",       "file" : { "fmt" : "cdf", "name" : "output/soilc_slow.nc"}},
    { "id" : "ch4_ebullition",   "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/ch4_ebullition.nc"}},
    { "id" : "ch4_plant_gas",    "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/ch4_plant_gas.nc"}},
    { "id" : "ch4_emissions",    "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/ch4_emissions.nc"}},
    { "id" : "ch4_sink",         "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/ch4_sink.nc"}},
    { "id" : "ch4_rice_em",      "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/ch4_rice_em.nc"}},
    { "id" : "meansoilo2",       "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/meansoilo2.nc"}},
    { "id" : "meansoilch4",      "file" : { "fmt" : "cdf", "timestep" : "annual", "name" : "output/meansoilch4.nc"}}
    
/*------------------------------ ------------------------- ------------------------------- */
  ],
#else
  "output" :
  [
    { "id" : "ch4_emissions",    "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/ch4_emissions_spinup6.nc"}},
    { "id" : "ch4_sink",         "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/ch4_sink_spinup6.nc"}},
    { "id" : "ch4_ebullition",   "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/ch4_ebullition_spinup6.nc"}},
    { "id" : "ch4_plant_gas",    "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/ch4_plant_gas_spinup6.nc"}},
    { "id" : "soilc",            "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/soilc_spinup6.nc"}},
    { "id" : "soilc_slow",       "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/soilc_slow_spinup6.nc"}},
    { "id" : "litc",             "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/litc_spinup6.nc"}},
    { "id" : "vegc",             "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/vegc_spinup6.nc"}},
    { "id" : "fpc",              "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/fpc_spinup6.nc"}},
    { "id" : "wpc",              "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/wpc_spinup6.nc"}},
    { "id" : "mwater",           "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/mwater_spinup6.nc"}},
    { "id" : "npp",              "file" : { "fmt" : "cdf", "timestep" : 100, "name" : "output/npp_spinup6.nc"}},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output/globalflux_spinup6.csv"}}
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
<<<<<<< HEAD
  "outputyear" : -15599,
=======
  "outputyear" : -1599,
>>>>>>> master
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv_stdfire6.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#else

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1901, /* first year of simulation */
  "lastyear" : 2009, /* last year of simulation */
  "outputyear": 1841, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire6.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1840_crop_stdfire6.lpj", /* filename of restart file */
  "restart_year": 1840 /* write restart at year */

#endif
}
