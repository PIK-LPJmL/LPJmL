/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  .  j  s                                       \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.6.13                          \n**/
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
  "version"  : "5.6",       /* LPJmL version expected */
  "random_prec" : true,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation",/* type of radiation input to be used, options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "fire" : "fire",          /* fire disturbance enabled, options: "no_fire", "fire", "spitfire", "spitfire_tmax" (for GLDAS input data) */
  "fire_on_grassland" : false, /* enable fire on grassland for Spitfire */
  "fdi" : "nesterov",       /* fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
  "firewood" : false,       /* NOT TESTED - DO NOT ENABLE */
  "new_phenology" : true,   /* enable GSI phenology */
  "new_trf" : false,        /* enable new transpiration reduction function - NOT TESTED */
  "river_routing" : true,   /* enable river routing (requires input matching grid and lakes/reservoirs) */
  "extflow" : false,        /* enable discharge inflow for regional runs (requires extflow_filename) */
  "permafrost" : true,      /* enable permafrost */
  "johansen" : true,        /* enable johansen way of temp. conductivity in soils (see src/soil/soilconduct.c) */
  "soilpar_option" : "no_fixed_soilpar", /* calculation of soil parameters, options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "soilpar_fixyear" : 1900  /* year to fix soilpars for soilpar_option fixed_soilpar */
  "with_nitrogen" : "lim",  /* options: "no", "lim", "unlim" */
  "nitrogen_coupled" : false, /* nitrogen stress coupled to water stress */
  "store_climate" : true,   /* store climate data in spin-up phase */
  "const_climate" : false,  /* reuse the first 30 years of the climate input */
  "shuffle_climate" : true, /* shuffle spinup climate and/or climate in fix_climate run */
  "const_deposition" : false,/* enable constant deposition; requires depos_year_const and fix_climate_cycle */
  "depos_year_const" : 1901, /* startyear that is used from deposition input for const_deposition */
  "fix_climate" : false,    /* enable a fixed climate input period, requires fix_climate_cycle and fix_climate_year, shuffe_climate also applies here */
  "fix_climate_cycle" : 30, /* reuse fix_climate_year - fix_climate_year + x years of the climate and/or deposition input */
  "fix_climate_year" : 30,  /* baseyear (start/middle -> noshuffle/shuffle) that is used from climate input for fix_climate */
  "fix_landuse" : false,    /* fix landuse after fix_landuseyear; different than landuse = const - where all years are the same */
  "fix_landuse_year" : false, /* year at which landuse is fixed */
#ifdef FROM_RESTART
  "new_seed" : false,       /* read random seed from restart file */
  "population" : false,     /* use population input (for spitfire) */
  "landuse" : "yes",        /* landuse setting; options: "no", "yes", "const", "all_crops", "only_crops" */
  "landuse_year_const" : 2000, /* set landuse year for "const" and "only_crops" cases */
  "reservoir" : true,       /* enables reservoirs (changes discharge and allows for irrigation water source), requires river_routing=true and elevation and reservoir input files  */
  "wateruse" : "yes",       /* HIL water withdrawals and return flows, requires wateruse input, options: "no", "yes" (only for years prescribed in input), "all" (use first year for years before and last year of input for years after period prescibed in input file) */
  "equilsoil" : false,      /* enables soil equilibration  (natural vegetation spinup) */
#else
  "equilsoil" : true,       /* enables soil equilibration  (natural vegetation spinup) */
  "population" : false,     /* use population input (for spitfire) */
  "landuse" : "no",         /* landuse setting; options: "no", "yes", "const", "all_crops", "only_crops" */
  "reservoir" : false,      /* enables reservoirs (changes discharge and allows for irrigation water source), requires river_routing=true and elevation and reservoir input files  */
  "wateruse" : "no",        /* HIL water withdrawals and return flows, requires wateruse input, options: "no", "yes" (only for years prescribed in input), "all" (use first year for years before and last year of input for years after period prescibed in input file) */
#endif
  "prescribe_burntarea" : false, /* prescribe burned area on nat vegetation (for spitfire)  */
  "prescribe_landcover" : "no_landcover", /* prescribe natural veg composition, options: "no_landcover", "landcoverfpc" (FPC), "landcoverest" (establishment) */
  "sowing_date_option" : "fixed_sdate",   /* options: no_fixed_sdate, fixed_sdate, prescribed_sdate */
  "sdate_fixyear" : 1970,               /* year in which sowing dates shall be fixed, when using fixed_sdate */
  "intercrop" : false,                  /* enables intercrops on setaside */
  "residue_treatment" : "fixed_residue_remove", /* residue options: "read_residue_data" (requires input dataset on fraction to remain on field), "no_residue_remove" (all non-harvested plant components to litter), "fixed_residue_remove" (fraction of non-harvested above ground biomass to remain in soil according to param residues_in_soil) */
  "residues_fire" : false,              /* enables burning of residuals on cropland - non-functional? */
  "irrigation" : "lim",                 /* options: "no", "lim", "pot", "all" */
  "laimax_interpolate" : "laimax_par",  /* laimax settings, options:  "laimax_par" (values from pft.js), "laimax_cft" (values from manage_irrig_*.js), "const_lai_max" (constant values set in flag laimax), "laimax_interpolate" (interpolates laimin and laimax based on values from pft.js - non-functional?) */
  "laimax" : 5,                         /* maximum LAI for laimax_interpolate = "const_lai_max" */
  "tillage_type" : "all",               /* Options: "all" (all agr. cells tilled), "no" (no cells tilled) and "read" (tillage dataset used) */
  "till_startyear" : 1850,              /* year in which tillage should start (currently only for tillage_type="read") */
  "till_fallow" : false,                /* tillage on */
  "black_fallow" : false,               /* simulation with black fallow on PNV */
  "pft_residue" : "temperate cereals",  /* ?  */
  "no_ndeposition" : false,             /* turn off atmospheric N deposition */
  "rw_manage" : false,                  /* enable rain water management; specific parameters in lpjparam.js */
  "fertilizer_input" : "yes",           /* fertilizer input setting, options: "no" (crops only depend on N in soils and from deposition), "yes", "auto" (applies exact N demand required by managed vegetation directly to plants - removes any N stress) */
  "manure_input" : true,                /* enable manure input */
  "fix_fertilization" : false,          /* fertilizer and manure input setting, options: "false" (prescribed time/cell/cft specific input), "true" (global constant rates from lpjmlparam.js) */
  "others_to_crop" : true,              /* true: cultivate others as a separate stand of cft according to setting "cft_temp"/"cft_trop"; false: simulate others as grassland with setting "grazing_others" */
  "cft_temp" : "temperate cereals",     /* cft name (see pft.js) to simulate others in temperate regions (abs(lat)>30°), if others_to_crop is true  */
  "cft_tropic" : "maize",               /* cft name (see pft.js) to simulate others in tropical regions (abs(lat)<30°), if others_to_crop is true  */
  "grazing_others" : "default",         /* default grazing type for others, options : "default", "mowing", "ext", "int", "livestock", "none" */
  "grazing" : "default",                /* default grazing type, options : "default", "mowing", "ext", "int", "livestock", "none" */
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "istimber" : true,                    /* true: extract timber carbon (timber_harvest output) if natural vegetation is cleared for managed land, false: carbon is put in litter */
  "grassland_fixed_pft" : false,        /* prescribe per grid cell the grass pft to be grown, requires additional input - non-functional? */
  "grass_harvest_options" : false,      /* true: grid-specific grassland management, requires input file; false: default management (setting "grazing") everywhere */
  "prescribe_lsuha" : false,           /* prescribe livestock unit per hectare, input required (needed for "grazing" = "livestock") */
  "mowing_days" : [152, 335],          /* Mowing days (day-of-year) for grassland if "grazing" = "mowing" */
  "crop_resp_fix" : false,             /* C:N ratio setting affecting crop respiration - options: true (fixed - avoids yield reductions at high n inputs); false (variable/dynamic) */
  "crop_phu_option" : "new",           /* defines setting for phenological heat unit and vernalization requirements for crops; options: "old", "new", "prescribed" */
  "cropsheatfrost" : false,            /* enable extreme temperature crop damages - requires tmin/tmax inputs - NOT TESTED */
  "double_harvest" : true,             /* true: enables outputs for second growing season (e.g. harvest2.bin; sdate2.bin - have to be explicitly added to outputs); false: crop specific outputs over all growing seasons are added up/overwritten - caution: this interplays with the setting "grid_scaled" */
  "npp_controlled_bnf" : true,         /* enable npp controlled bnf based on Ma et al. 2022, alternatively old Cleveland et al. 1999*/

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

  "output_metafile" : true, /* no json metafile created */
  "float_grid" : false,      /* set datatype of grid file to float (TRUE/FALSE) */

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
    { "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
    { "id" : "fpc",              "file" : { "fmt" : "raw", "name" : "output/fpc.bin" }},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output/globalflux.csv"}},
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
#ifdef WITH_SPITFIRE
    { "id" : "firec",            "file" : { "fmt" : "raw", "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec.bin"}},
    { "id" : "nfire",            "file" : { "fmt" : "raw", "name" : "output/mnfire.bin"}},
    { "id" : "burntarea",        "file" : { "fmt" : "raw", "name" : "output/mburnt_area.bin"}},
#else
    { "id" : "firec",            "file" : { "fmt" : "raw", "name" : "output/firec.bin"}},
#endif
    { "id" : "discharge",        "file" : { "fmt" : "raw", "name" : "output/mdischarge.bin"}},
    { "id" : "wateramount",      "file" : { "fmt" : "raw", "name" : "output/mwateramount.bin"}},
    { "id" : "harvestc",         "file" : { "fmt" : "raw", "name" : "output/flux_harvest.bin"}},
    { "id" : "sdate",            "file" : { "fmt" : "raw", "name" : "output/sdate.bin"}},
    { "id" : "pft_harvestc",     "file" : { "fmt" : "raw", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "raw", "name" : "output/cftfrac.bin"}},
    { "id" : "seasonality",      "file" : { "fmt" : "raw", "name" : "output/seasonality.bin"}},
    { "id" : "pet",              "file" : { "fmt" : "raw", "name" : "output/mpet.bin"}},
    { "id" : "albedo",           "file" : { "fmt" : "raw", "name" : "output/malbedo.bin"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "raw", "name" : "output/maxthaw_depth.bin"}},
    { "id" : "perc",             "file" : { "fmt" : "raw", "name" : "output/mperc.bin"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "raw", "name" : "output/msoiltemp1.bin"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "raw", "name" : "output/msoiltemp2.bin"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "raw", "name" : "output/msoiltemp3.bin"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "raw", "name" : "output/soilc_layer.bin"}},
    { "id" : "agb",              "file" : { "fmt" : "raw", "name" : "output/agb.bin"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "raw", "name" : "output/agb_tree.bin"}},
    { "id" : "return_flow_b",    "file" : { "fmt" : "raw", "name" : "output/mreturn_flow_b.bin"}},
    { "id" : "transp_b",         "file" : { "fmt" : "raw", "name" : "output/mtransp_b.bin"}},
    { "id" : "evap_b",           "file" : { "fmt" : "raw", "name" : "output/mevap_b.bin"}},
    { "id" : "interc_b",         "file" : { "fmt" : "raw", "name" : "output/mintec_b.bin"}},
    { "id" : "prod_turnover",    "file" : { "fmt" : "raw", "name" : "output/prod_turnover.bin"}},
    { "id" : "deforest_emis",    "file" : { "fmt" : "raw", "name" : "output/deforest_emis.bin"}},
    { "id" : "conv_loss_evap",   "file" : { "fmt" : "raw", "name" : "output/aconv_loss_evap.bin"}},
    { "id" : "conv_loss_drain",  "file" : { "fmt" : "raw", "name" : "output/aconv_loss_drain.bin"}}
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
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1900_crop_stdfire.lpj", /* filename of restart file */
  "restart_year": 1900 /* write restart at year */

#endif
}
