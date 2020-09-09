/*--------------------------------------------------------------------------*/
/* 1. wft */
  {
    "name" : "woodplantation temperate tree",
    "type" : TREE,
    "cultivation_type" : "wp",/* cultivation_type */
    "cn" : [30., 55., 70., 77.], /* curve number */
    "beta_root" : 0.966,    /* beta_root */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.620513,     /* gmin 4*/
    "respcoeff" : 1.47534,  /* respcoeff 5*/
    "nmax" : 120.,          /* nmax 7*/
    "resist" : 0.95,        /* resist 8*/
    "longevity" : 0.464605, /* leaf longevity 10*/
    "lmro_ratio" : 1.16441, /* lmro_ratio 18*/
    "ramp" : 100.,          /* ramp 19*/
    "lai_sapl" : 1.3935,    /* lai_sapl 21*/
    "gdd5min" : 300.0,      /* gdd5min 30*/
    "twmax" : 1000.,        /* twmax 31*/
    "twmax_daily" : 1000.,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.,/* min_temprange 34*/
    "emax": 5.45822,        /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : 0.614725,    /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.139,  /* albedo of green leaves */
    "albedo_stem" : 0.04,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.25,/* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
    "slope" : 0.216,          /* new phenology: slope of cold-temperature limiting function */
    "base" : -1.407,          /* new phenology: inflection point of cold-temperature limiting function (deg C) */
    "tau" : 0.2               /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
        "slope" : 1.785,      /* new phenology: slope of warm-temperature limiting function tmax_sl */
        "base" : 38.385,      /* new phenology: inflection point of warm-temperature limiting function (deg C) */
        "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
        "slope" : 39,         /* new phenology: slope of light limiting function */
        "base" : 35.326,      /* new phenology: inflection point of light limiting function (Wm-2) */
        "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
        "slope" : 5.12,       /* new phenology: slope of water limiting function */
        "base" : 14.787,      /* new phenology: inflection point of water limiting function (% water availability)  */
        "tau" : 0.8           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : 0.0635576,   /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "summergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15., "high" : 30. },/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "temp" : { "low" : -8.0, "high" : 11 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28880,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 22.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.825, "wood" : 0.073 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.67, /* Q10_wood */
    "windspeed_dampening" : 0.4, /* windspeed dampening */
    "roughness_length" : 1.0,/* roughness length */
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 1.30942, "sapwood" : 20.0009, "root" : 1.30942}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "crownarea_max" : 15.0,  /* crownarea_max 20*/
    "wood_sapl" : 1.02171,      /* sapwood sapling 22*/
    "aphen_min" : APHEN_MIN,
    "aphen_max" : APHEN_MAX,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : 125.823,         /* allometry */
    "allom2" : 53.9572,
    "allom3" : 1.02133,
    "allom4" : ALLOM4,
    "height_max" : HEIGHT_MAX,   /* maximum height of tree */
    "scorchheight_f_param" : 0.0940,/* scorch height (F) */
    "crownlength" : 0.3334,      /* crown length (CL) */
    "barkthick_par1" : 0.347,    /* bark thickness par1 */
    "barkthick_par2" : 0.1086,   /* bark thickness par2 */
    "crown_mort_rck" : 1.00,     /* crown damage (rCK) */
    "crown_mort_p" : 3.00,       /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est" : K_EST,             /* k_est,  Giardina, 2002 */
    "rotation" : 500,            /* rotation */
    "max_rotation_length" : 500, /* max_rotation_length */
    "P_init" : 0.15394           /* Initial stand density (only used for wood plantations) */	
  },
/*--------------------------------------------------------------------------*/
/* 2. wft */
  {
    "name" : "woodplantation tropical tree",
    "type" : TREE,
    "cultivation_type" : "wp",/* cultivation_type */
    "cn" : [30., 55., 70., 77.], /* curve number */
    "beta_root" : 0.962,    /* beta_root */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.48295,      /* gmin 4*/
    "respcoeff" : 0.162926, /* respcoeff 5*/
    "nmax" : 100.,          /* nmax 7*/
    "resist" : 1.0,         /* resist 8*/
    "longevity" : 1.42577,  /* leaf longevity 10*/
    "lmro_ratio" : 1.54496, /* lmro_ratio 18*/
    "ramp" : 1000.,         /* ramp 19*/
    "lai_sapl" : 1.44744,   /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.,        /* twmax 31*/
    "twmax_daily" : 1000.,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.,/* min_temprange 34*/
    "emax": 11.7021,        /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : 0.596536,    /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.13,   /* albedo of green leaves */
    "albedo_stem" : 0.04,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.6,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 1.01,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 8.3,         /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.86,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 38.64,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 77.17,      /* new phenology: slope of light limiting function */
      "base" : 55.53,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.52          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5.14,       /* new phenology: slope of water limiting function */
      "base" : 4.997,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.44          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : 0.0584912, /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 2.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 25., "high" : 38. },/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "temp" : { "low" : 11.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.38009,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.000034, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 13.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1664.0, "co" :  63.0, "ch4" : 2.20, "voc" : 3.40, "tpm" : 8.50, "nox" : 2.540}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.93, "wood" : 0.039 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 2.75, /* Q10_wood */
    "windspeed_dampening" : 0.4, /* windspeed dampening */
    "roughness_length" : 1.0,/* roughness length */
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 1.75095, "sapwood" : 46.2736, "root" : 1.75095}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "crownarea_max" : 15.0,        /* crownarea_max 20*/
    "wood_sapl" : 1.22952,         /* sapwood sapling 22*/
    "aphen_min" : APHEN_MIN,
    "aphen_max" : APHEN_MAX,
    "reprod_cost" : REPROD_COST,   /* reproduction cost */
    "allom1" : 166.319,            /* allometry */
    "allom2" : 41.1424,
    "allom3" : 0.840484,
    "allom4" : ALLOM4,
    "height_max" : HEIGHT_MAX,     /* maximum height of tree */
    "scorchheight_f_param" : 0.061,/* scorch height (F) */
    "crownlength" : 0.10 ,         /* crown length (CL) */
    "barkthick_par1" : 0.1085,     /* bark thickness par1 */
    "barkthick_par2" : 0.2120,     /* bark thickness par2 */
    "crown_mort_rck" : 0.05,       /* crown damage (rCK) */
    "crown_mort_p" : 3.00,         /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est" : 0.5,                 /* k_est,  Giardina, 2002 */
    "rotation" : 500,              /* rotation */
    "max_rotation_length" : 500,   /* max_rotation_length */
    "P_init" : 0.147257            /* Initial stand density (only used for wood plantations) */	
  },
/*--------------------------------------------------------------------------*/
/* 3. wft */
  {
    "name" : "woodplantation boreal tree",
    "type" : TREE,
    "cultivation_type" : "wp",/* cultivation_type */
    "cn" : [30., 55., 70., 77.], /* curve number */
    "beta_root" : 0.943,    /* beta_root */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.279355,     /* gmin 4*/
    "respcoeff" : 1.48852,  /* respcoeff 5*/
    "nmax" : 100.,          /* nmax 7*/
    "resist" : 1.0,         /* resist 8*/
    "longevity" : 3.95806,  /* leaf longevity 10*/
    "lmro_ratio" : 1.48852, /* lmro_ratio 18*/
    "ramp" : 1000.,         /* ramp 19*/
    "lai_sapl" : 1.6676,    /* lai_sapl 21*/
    "gdd5min" : 600.0,      /* gdd5min 30*/
    "twmax" : 23.0,         /* twmax 31*/
    "twmax_daily" : 25.0,   /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.,/* min_temprange 34*/
    "emax": 6.07702,        /* emax 35*/
    "intc" : 0.06,          /* intc 36*/
    "alphaa" : 0.53296,     /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.13,   /* albedo of green leaves */
    "albedo_stem" : 0.1,    /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.1, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.1008,     /* new phenology: slope of cold-temperature limiting function */
      "base" : -7.516,      /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 27.32,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 14.0,       /* new phenology: slope of light limiting function */
      "base" : 3.04,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5.0,       /* new phenology: slope of water limiting function */
      "base" : 0.007695,   /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.8          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : 0.0480544, /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15., "high" : 25. },/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "temp" : { "low" : -32.5, "high" : -8.0 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28670,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 25.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.76, "wood" : 0.041 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.97, /* Q10_wood */
    "windspeed_dampening" : 0.4, /* windspeed dampening */
    "roughness_length" : 1.0,/* roughness length */
    "leaftype" : "needleleaved",/* leaftype */
    "turnover" : {"leaf" : 4.74487, "sapwood" : 15.688, "root" : 4.74487}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "crownarea_max" : 15.0,  /* crownarea_max 20*/
    "wood_sapl" : 1.37836,       /* sapwood sapling 22*/
    "aphen_min" : APHEN_MIN,
    "aphen_max" : APHEN_MAX,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : 86.4058,          /* allometry */
    "allom2" : 36.5433,
    "allom3" : 1.21058,
    "allom4" : ALLOM4,
    "height_max" : HEIGHT_MAX,   /* maximum height of tree */
    "scorchheight_f_param" : 0.1100,/* scorch height (F) */
    "crownlength" : 0.3334 ,     /* crown length (CL) */
    "barkthick_par1" : 0.0292,   /* bark thickness par1 */
    "barkthick_par2" : 0.2632,   /* bark thickness par2 */
    "crown_mort_rck" : 1.00,     /* crown damage (rCK) */
    "crown_mort_p" : 3.00,       /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est" : K_EST,             /* k_est,  Giardina, 2002 */
    "rotation" : 500,            /* rotation */
    "max_rotation_length" : 500, /* max_rotation_length */
    "P_init" : 0.169781          /* Initial stand density (only used for wood plantations) */	
   },
