/**************************************************************************************/
/**                                                                                \n**/
/**                   l  p  j  m  l  _  s  p  i  t  f  i  r  e  .  j  s            \n**/
/**                                                                                \n**/
/** Default configuration file for LPJmL C Version 5.7.9-FIRE                      \n**/
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
  "version"  : "5.7",       /* LPJmL version expected */
  "global_attrs" : {"institution" : "Potsdam Institute for Climate Impact Research",
                    "contact" : "", /* name and email address */
                    "comment" : ""  /* additional comments */
                   },       /* Global attributes for NetCDF output files */

  "coupled_model" : null,
  "random_prec" : false,     /* Random weather generator for precipitation enabled */
  "random_seed" : 2,        /* seed for random number generator */
  "radiation" : "radiation_lwdown",/* other options: "cloudiness", "radiation", "radiation_swonly", "radiation_lwdown" */
  "isswnet" : false,
  "landfrac_from_file" : true,
  "fire_base" : false,
  "fire" : "spitfire_tmax",          /* fire disturbance enabled, other options: NO_FIRE, FIRE, SPITFIRE, SPITFIRE_TMAX (for GLDAS input data) */
  "fdi" : "wvpd",       /* different fire danger index formulations: "wvpd" (needs GLDAS input data), "nesterov" */
   "max_firesize" : false,
  "fireduration" : [{"stand" : "grassland", "duration" : [1,120] ,"ndayfire" : 1},
                    {"stand" : "natural", "duration" : [120,480], "ndayfire" : 3}],
  "firewood" : false,
  "gsi_phenology" : true,   /* GSI phenology enabled */
  "new_trf" : false,        /* new transpiration reduction function disabled */
  "gsilivefuel"  : true,
  "with_lakes" : true,
  "river_routing" : false,
  "extflow" : false,
  "permafrost" : true,
  "johansen" : true,
  "transp_suction_fcn" : false,
  "soilpar_option" : "no_fixed_soilpar", /* other options "no_fixed_soilpar", "fixed_soilpar", "prescribed_soilpar" */
  "with_nitrogen" : "lim", /* other options: "no", "lim", "unlim" */
  "store_climate" : true, /* store climate data in spin-up phase */
  "const_climate" : false,
  "shuffle_spinup_climate" : true, /* shuffle spinup climate */
  "const_deposition" : false,
  "fix_climate" : false,
  "fix_landuse" : false,
  "fix_deposition" : false,
  "fix_co2" : false,
  "relative_humidity" : false,
#ifdef RUN
  "prescribe_ignition" : false,
#else
  "prescribe_ignition" : false,
#endif
#ifdef SPINUP
  "equilsoil" : true,
  "population" : false,
  "landuse" : "no",
  "reservoir" : false,
  "wateruse" : "no",
#else
  "new_seed" : false, /* read random seed from restart file */
  "population" : true,
  "human_ign_prob" : false,
  "landuse" : "yes", /* other options: "no", "yes", "const", "all_crops", "only_crops" */
  "landuse_year_const" : 2000, /* set landuse year for "const" case */
  "reservoir" : true,
  "wateruse" : "no",  /* other options: "no", "yes", "all" */
  "equilsoil" : false,
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
  "grazing" : "livestock",              /* default grazing type, other options : "default", "mowing", "ext", "int", "none" */
  "cft_temp" : "temperate cereals",
  "cft_tropic" : "maize",
  "grassonly" : false,                  /* set all cropland including others to zero but keep managed grasslands */
  "luc_timber" : true,
  "grassland_fixed_pft" : false,
  "grass_harvest_options" : false,
  "mowing_days" : [152, 335],          /* Mowing days for grassland if grass harvest options are ser */
  "crop_resp_fix" : false,             /* variable C:N ratio for crop respiration */
                                       /* for MAgPIE runs, turn off dynamic C:N ratio dependent respiration,
                                          which reduces yields at high N inputs */
  "prescribe_lsuha" : true,
  "crop_phu_option" : "new",
  "cropsheatfrost" : false,
  "double_harvest" : true,
  "npp_controlled_bnf" : true,                    /* Biological N fixation using Cleveland, 1999 (false) or Ma et al., 2022 (true) approach

/*===================================================================*/
/*  II. Input parameter section                                      */
/*===================================================================*/

#include "param_vpd.js"    /* Input parameter file */

/*===================================================================*/
/*  III. Input data section                                          */
/*===================================================================*/

#include "input_wfde5_cru.js"    /* Input files of CRU dataset */

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

#define mkstr(s) xstr(s) /* putting string in quotation marks */
#define xstr(s) #s

  "output_metafile" : true,   /* json metafile created (true/false) */
  "default_fmt" : "raw",      /* default format for output files: "raw","txt","cdf","clm","sock" */
  "default_suffix" : ".bin",  /* default file suffix for output files */
  "grid_type" : "short",      /* set datatype of grid file ("short", "float", "double") */
  "absyear" : false,          /* years relative to baseyear (true/false) */
  "rev_lat" : false,          /* reverse latitudes in NetCDF output (true/false) */
  "with_days" : true,         /* use days as units for monthly output in NetCDF files */

#ifdef RUN

  "output" :
  [

/*
ID                               Fmt                        filename
-------------------------------- ------------------------- ----------------------------- */
    { "id" : "grid",             "file" : { "fmt" : "raw", "name" : "output/grid.bin" }},
    { "id" : "terr_area",        "file" : { "fmt" : "cdf", "name" : "output/area.nc" }},
    { "id" : "fpc",              "file" : { "fmt" : "cdf", "name" : "output/fpc.nc" }},
    { "id" : "globalflux",       "file" : { "fmt" : "txt", "name" : "output/globalflux.csv"}},
    { "id" : "npp",              "file" : { "fmt" : "cdf", "name" : "output/mnpp.nc"}},
    { "id" : "gpp",              "file" : { "fmt" : "cdf", "name" : "output/mgpp.nc"}},
    { "id" : "rh",               "file" : { "fmt" : "cdf", "name" : "output/mrh.nc"}},
    { "id" : "fapar",            "file" : { "fmt" : "cdf", "name" : "output/mfapar.nc"}},
    { "id" : "transp",           "file" : { "fmt" : "cdf", "name" : "output/mtransp.nc"}},
    { "id" : "runoff",           "file" : { "fmt" : "cdf", "name" : "output/mrunoff.nc"}},
    { "id" : "evap",             "file" : { "fmt" : "cdf", "name" : "output/mevap.nc"}},
    { "id" : "interc",           "file" : { "fmt" : "cdf", "name" : "output/minterc.nc"}},
    { "id" : "swc1",             "file" : { "fmt" : "cdf", "name" : "output/mswc1.nc"}},
    { "id" : "swc2",             "file" : { "fmt" : "cdf", "name" : "output/mswc2.nc"}},
    { "id" : "firef",            "file" : { "fmt" : "cdf", "name" : "output/firef.nc"}},
    { "id" : "vegc",             "file" : { "fmt" : "cdf", "name" : "output/vegc.nc"}},
    { "id" : "soilc",            "file" : { "fmt" : "cdf", "name" : "output/soilc.nc"}},
    { "id" : "litc",             "file" : { "fmt" : "cdf", "name" : "output/litc.nc"}},
    { "id" : "flux_estabc",      "file" : { "fmt" : "cdf", "name" : "output/flux_estab.nc"}},
    { "id" : "pft_vegc",         "file" : { "fmt" : "cdf", "name" : "output/pft_vegc.nc"}},
    { "id" : "phen_tmin",        "file" : { "fmt" : "cdf", "name" : "output/mphen_tmin.nc"}},
    { "id" : "phen_tmax",        "file" : { "fmt" : "cdf", "name" : "output/mphen_tmax.nc"}},
    { "id" : "phen_light",       "file" : { "fmt" : "cdf", "name" : "output/mphen_light.nc"}},
    { "id" : "phen_water",       "file" : { "fmt" : "cdf", "name" : "output/mphen_water.nc"}},
    { "id" : "pft_phen",         "file" : { "fmt" : "cdf", "name" : "output/pft_phen.nc"}},
    { "id" : "vegn",             "file" : { "fmt" : "cdf", "name" : "output/vegn.nc"}},
    { "id" : "soiln",            "file" : { "fmt" : "cdf", "name" : "output/soiln.nc"}},
    { "id" : "litn",             "file" : { "fmt" : "cdf", "name" : "output/litn.nc"}},
    { "id" : "soiln_layer",      "file" : { "fmt" : "cdf", "name" : "output/soiln_layer.nc"}},
    { "id" : "soilno3_layer",    "file" : { "fmt" : "cdf", "name" : "output/soilno3_layer.nc"}},
    { "id" : "soilnh4_layer",    "file" : { "fmt" : "cdf", "name" : "output/soilnh4_layer.nc"}},
    { "id" : "soiln_slow",       "file" : { "fmt" : "cdf", "name" : "output/soiln_slow.nc"}},
    { "id" : "soilnh4",          "file" : { "fmt" : "cdf", "name" : "output/soilnh4.nc"}},
    { "id" : "soilno3",          "file" : { "fmt" : "cdf", "name" : "output/soilno3.nc"}},
    //{ "id" : "pft_nuptake",      "file" : { "fmt" : "cdf", "name" : "output/pft_nuptake.nc"}},
    { "id" : "nuptake",          "file" : { "fmt" : "cdf", "name" : "output/mnuptake.nc"}},
    { "id" : "leaching",         "file" : { "fmt" : "cdf", "name" : "output/mleaching.nc"}},
    { "id" : "n2o_denit",        "file" : { "fmt" : "cdf", "name" : "output/mn2o_denit.nc"}},
    { "id" : "n2o_nit",          "file" : { "fmt" : "cdf", "name" : "output/mn2o_nit.nc"}},
    { "id" : "n2_emis",          "file" : { "fmt" : "cdf", "name" : "output/mn2_emis.nc"}},
    { "id" : "bnf",              "file" : { "fmt" : "cdf", "name" : "output/mbnf.nc"}},
    { "id" : "n_immo",           "file" : { "fmt" : "cdf", "name" : "output/mn_immo.nc"}},
    //{ "id" : "pft_ndemand",      "file" : { "fmt" : "cdf", "name" : "output/pft_ndemand.nc"}},
    { "id" : "firen",            "file" : { "fmt" : "cdf", "name" : "output/firen.nc"}},
    { "id" : "n_mineralization", "file" : { "fmt" : "cdf", "name" : "output/mn_mineralization.nc"}},
    { "id" : "n_volatilization", "file" : { "fmt" : "cdf", "name" : "output/mn_volatilization.nc"}},
    /*{ "id" : "pft_nlimit",       "file" : { "fmt" : "cdf", "name" : "output/pft_nlimit.nc"}},
    { "id" : "pft_vegn",         "file" : { "fmt" : "cdf", "name" : "output/pft_vegn.nc"}},
    { "id" : "pft_cleaf",        "file" : { "fmt" : "cdf", "name" : "output/pft_cleaf.nc"}},
    { "id" : "pft_nleaf",        "file" : { "fmt" : "cdf", "name" : "output/pft_nleaf.nc"}},
    { "id" : "pft_laimax",       "file" : { "fmt" : "cdf", "name" : "output/pft_laimax.nc"}},
    { "id" : "pft_croot",        "file" : { "fmt" : "cdf", "name" : "output/pft_croot.nc"}},
    { "id" : "pft_nroot",        "file" : { "fmt" : "cdf", "name" : "output/pft_nroot.nc"}},
    { "id" : "pft_csapw",        "file" : { "fmt" : "cdf", "name" : "output/pft_csapw.nc"}},
    { "id" : "pft_nsapw",        "file" : { "fmt" : "cdf", "name" : "output/pft_nsapw.nc"}},
    { "id" : "pft_chawo",        "file" : { "fmt" : "cdf", "name" : "output/pft_chawo.nc"}},
    { "id" : "pft_nhawo",        "file" : { "fmt" : "cdf", "name" : "output/pft_nhawo.nc"}},*/
    { "id" : "firec",            "file" : { "fmt" : "cdf", "timestep" : "monthly" , "unit" : "gC/m2/month", "name" : "output/mfirec.nc"}},
    { "id" : "nfire",            "file" : { "fmt" : "cdf", "name" : "output/mnfire.nc"}},
    { "id" : "fireduration",     "file" : { "fmt" : "cdf", "name" : "output/mfireduration.nc"}},
    { "id" : "firedurationdays", "file" : { "fmt" : "cdf", "name" : "output/mfiredurationdays.nc"}},
    { "id" : "firedi",           "file" : { "fmt" : "cdf", "name" : "output/mfiredi.nc"}},
    { "id" : "burntarea",        "file" : { "fmt" : "cdf", "name" : "output/mburnt_area.nc"}},
    { "id" : "discharge",        "file" : { "fmt" : "cdf", "name" : "output/mdischarge.nc"}},
    { "id" : "wateramount",      "file" : { "fmt" : "cdf", "name" : "output/mwateramount.nc"}},
    { "id" : "harvestc",         "file" : { "fmt" : "cdf", "name" : "output/flux_harvest.nc"}},
    { "id" : "sdate",            "file" : { "fmt" : "cdf", "name" : "output/sdate.nc"}},
    //{ "id" : "pft_harvestc",     "file" : { "fmt" : "cdf", "name" : mkstr(output/pft_harvest.SUFFIX)}},
    { "id" : "cftfrac",          "file" : { "fmt" : "cdf", "name" : "output/cftfrac.nc"}},
    { "id" : "seasonality",      "file" : { "fmt" : "cdf", "name" : "output/seasonality.nc"}},
    { "id" : "pet",              "file" : { "fmt" : "cdf", "name" : "output/mpet.nc"}},
    { "id" : "albedo",           "file" : { "fmt" : "cdf", "name" : "output/malbedo.nc"}},
    { "id" : "maxthaw_depth",    "file" : { "fmt" : "cdf", "name" : "output/maxthaw_depth.nc"}},
    { "id" : "perc",             "file" : { "fmt" : "cdf", "name" : "output/mperc.nc"}},
    { "id" : "soiltemp1",        "file" : { "fmt" : "cdf", "name" : "output/msoiltemp1.nc"}},
    { "id" : "soiltemp2",        "file" : { "fmt" : "cdf", "name" : "output/msoiltemp2.nc"}},
    { "id" : "soiltemp3",        "file" : { "fmt" : "cdf", "name" : "output/msoiltemp3.nc"}},
    { "id" : "soilc_layer",      "file" : { "fmt" : "cdf", "name" : "output/soilc_layer.nc"}},
    { "id" : "agb",              "file" : { "fmt" : "cdf", "name" : "output/agb.nc"}},
    { "id" : "agb_tree",         "file" : { "fmt" : "cdf", "name" : "output/agb_tree.nc"}},
    { "id" : "return_flow_b",    "file" : { "fmt" : "cdf", "name" : "output/mreturn_flow_b.nc"}},
    { "id" : "transp_b",         "file" : { "fmt" : "cdf", "name" : "output/mtransp_b.nc"}},
    { "id" : "evap_b",           "file" : { "fmt" : "cdf", "name" : "output/mevap_b.nc"}},
    { "id" : "interc_b",         "file" : { "fmt" : "cdf", "name" : "output/mintec_b.nc"}},
    { "id" : "prod_turnover",    "file" : { "fmt" : "cdf", "name" : "output/prod_turnover.nc"}},
    { "id" : "deforest_emis",    "file" : { "fmt" : "cdf", "name" : "output/deforest_emis.nc"}},
    { "id" : "conv_loss_evap",   "file" : { "fmt" : "cdf", "name" : "output/aconv_loss_evap.nc"}},
    { "id" : "conv_loss_drain",  "file" : { "fmt" : "cdf", "name" : "output/aconv_loss_drain.nc"}},
    { "id" : "human_ignition",   "file" : { "fmt" : "cdf", "name" : "output/human_ignition.nc"}},
    { "id" : "lightning",        "file" : { "fmt" : "cdf", "name" : "output/lightning.nc"}},
    { "id" : "ros",              "file" : { "fmt" : "cdf", "name" : "output/mros.nc"}},
    { "id" : "firesize",         "file" : { "fmt" : "cdf", "name" : "output/mfiresize.nc"}},
    { "id" : "surface_fi",       "file" : { "fmt" : "cdf", "name" : "output/msurface_fi.nc"}},
    { "id" : "firedays",         "file" : { "fmt" : "cdf", "name" : "output/mfiredays.nc"}},
    { "id" : "ndayfire",         "file" : { "fmt" : "cdf", "name" : "output/mndayfire.nc"}},
    { "id" : "stand_burntarea",  "file" : { "fmt" : "cdf", "name" : "output/burntarea_stand.nc"}},
    { "id" : "littermoist",      "file" : { "fmt" : "cdf", "name" : "output/mlitter_moist.nc"}},
    //{ "id" : "pft_litterc",      "file" : { "fmt" : "cdf", "name" : "output/mlitterc_pft.nc"}},
    { "id" : "livegrass",        "file" : { "fmt" : "cdf", "name" : "output/mlivegrass.nc"}},
    { "id" : "dlm_livegrass",        "file" : { "fmt" : "cdf", "name" : "output/mdlm_livegrass.nc"}},
    { "id" : "dfmc",        "file" : { "fmt" : "cdf", "name" : "output/mdfmc.nc"}},
    //{ "id" : "gsi_diff",        "file" : { "fmt" : "cdf", "name" : "output/mgsi_diff.nc"}},
    { "id" : "fuel",             "file" : { "fmt" : "cdf", "name" : "output/fuel.nc"}}

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

  "startgrid" : "all",//good: 29649, //bad: 30964, //"all", //24552,//"all",//"all"//109661 //117928 /* "all" 117928 27410, 67208 60400 47284 47293 47277 all grid cells */

#ifdef CHECKPOINT
  "checkpoint_filename" : "restart/restart_checkpoint.lpj", /* filename of checkpoint file */
#endif

#ifdef SPINUP

  "nspinup" : 3500,  /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr) */
  "firstyear": 1979, /* first year of simulation */
  "lastyear" : 1979, /* last year of simulation */
  "restart" :  false, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv_stdfire.lpj", /* filename of restart file */

  "outputyear" : -5021,
  "write_restart" : true, /* create restart file: the last year of simulation=restart-year */
  "write_restart_filename" : "restart/restart_1840_nv.lpj", /* filename of restart file */
  "restart_year": 1901 /* write restart at year */

#elif defined(LANDUSESPINUP)

  "nspinup" : 390,   /* spinup years */
  "nspinyear" : 30,  /* cycle length during spinup (yr)*/
  "firstyear": 1979, /* first year of simulation */
  "lastyear" : 1994, /* last year of simulation */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1840_nv.lpj", /* filename of restart file */
  "write_restart" : true, /* create restart file */
  "write_restart_filename" : "restart/restart_1994_crop.lpj", /* filename of restart file */
  "restart_year": 1994 /* write restart at year */

#elif defined(RUN)
  "nspinup" : 0,   /* spinup years */
  "nspinyear" : 11,  /* cycle length during spinup (yr)*/
  "firstyear": 1995, /* first year of simulation */
  "lastyear" : 2016, /* last year of simulation */
  "outputyear": 1995, /* first year output is written  */
  "restart" :  true, /* start from restart file */
  "restart_filename" : "restart/restart_1994_crop.lpj", /* filename of restart file */
  "write_restart" : false, /* create restart file */
  "write_restart_filename" : "restart/restart_2016_crop.lpj", /* filename of restart file */
  "restart_year": 2016 /* write restart at year */
#endif
}
