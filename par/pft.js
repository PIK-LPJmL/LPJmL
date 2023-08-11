/**************************************************************************************/
/**                                                                                \n**/
/**              p  f  t  .  j  s                                                  \n**/
/**                                                                                \n**/
/**  PFT and CFT parameter file for LPJmL version 5.7.1                            \n**/
/**  CFTs parameters must be put after PFTs                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#define CTON_LEAF 30.0
#define CTON_ROOT 30.0
#define CTON_POOL 100.0
#define CTON_SO 100.0
#define CTON_SAP 330.0
#ifdef WITH_SPITFIRE
#define FLAM_TREE 0.3
#define FLAM_GRASS 0.3
#else
#define FLAM_TREE 0.3
#define FLAM_GRASS 0.3
#endif
#define K_LITTER10 0.97       /* now only used for crops and value a middle value of C4 grasses; need to be updated with TRY-database*/
#define APREC_MIN 100.0
#define ALLOM1 100.0
#define ALLOM2 40.0
#define ALLOM3 0.67     /*0.5*/
#define ALLOM4 0.3
#define APHEN_MAX 245
#define APHEN_MIN 60    /* minimum aphen for cold-induced senescence */
#define HEIGHT_MAX 100.0  /* maximum height of trees */
#define REPROD_COST 0.1 /* reproduction cost */
#define K_EST 0.12 /* maximum overall sapling establishment rate (indiv/m2) */
#ifdef WITH_SPITFIRE
#define MORT_MAX 0.03
#else
#define MORT_MAX 0.03
#endif
#define MORT_MAX_GRASS 0.00
#define FN_TURNOVER 0.3      /* fraction of N not recovered before turnover */
#define FN_TURNOVER_EV_BR 0.7   /* fraction of N not recovered before turnover */
#define FN_TURNOVER_EV 0.8   /* fraction of N not recovered before turnover */
#define FN_TURNOVER_GR 0.3   /* fraction of N not recovered before turnover */
#ifdef BMGR_BROWN
#define FN_TURNOVER_BMGR 0.30    /* 0.30 for brown harvest + specific harvest dates */
#else
#define FN_TURNOVER_BMGR 0.69    /* 0.68 for green harvest + specific harvest dates */
#endif 
#define ALPHAA_NITROGEN 0.5  /* alphaa for simulations with nitrogen limitation */
#define RATIO_SAPW 13.5 /* relative C:N ratio of sapwood */
#define CN_BL_EG_MX 46.2
#define CN_BL_EG_MN 15.6
#define CN_BL_EG_MD 26.8
#define CN_NL_EG_MX 63.8
#define CN_NL_EG_MN 31.8
#define CN_NL_EG_MD 45.0
#define CN_BL_DC_MX 34.6
#define CN_BL_DC_MN 15.4
#define CN_BL_DC_MD 23.1
#define CN_NL_DC_MX 36.9
#define CN_NL_DC_MN 18.4
#define CN_NL_DC_MD 26.0
#define CN_GC3_MX 37.9
#define CN_GC3_MN 10.5
#define CN_GC3_MD 19.9
#define CN_GC4_MX 66.9
#define CN_GC4_MN 17.4
#define CN_GC4_MD 34.0
#define CN_CROPS_MEAN 25.0 /* cnleaf mean for grasses, table A.3.1, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
#define K_LATOSA 6e3      /* leaf area to sapwood area */
#define BC3 0.015         /*leaf respiration as fraction of Vmax for C3 plants */
#define BC4 0.035         /* leaf respiration as fraction of Vmax for C4 plants */
#define IRRIG_THRESHOLD_C3_DRY   0.8 /* irrigation threshold C3, prec < aprec_lim */
#define IRRIG_THRESHOLD_C3_HUMID 0.9 /* irrigation threshold C3, prec >= aprec_lim */
#define IRRIG_THRESHOLD_C4       0.7 /* irrigation threshold C4 */
#define TEMP_BNF_MIN 0.5
#define TEMP_BNF_MAX 45.0
#define SWC_BNF_LOW 0.0
#define SWC_BNF_HIGH 0.5
#define PHI_BNF_1 0.0
#define PHI_BNF_2 2.0
#define MAXBNFCOST 0.25
#define MAXBNFCOSTTR 0.007 /* Fraction of NPP available for BNF multiplied by climates legume share */
#define MAXBNFCOSTTE 0.0014
#define MAXBNFCOSTBO 0.0056
#define BNF_COST 6.0

"pftpar" :
[
  /* first pft */
  {
    "name" : "tropical broadleaved evergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.952,    /* beta_root original 0.962*/
    "minwscal" : 0.0,       /* minwscal 3*/
    "gmin"  : 1.6,          /* gmin 4*/
    "respcoeff" : 0.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.12,        /* resist 8*/
    "longevity" : 1.6,      /* leaf longevity 10*/
    "sla" : 0.01986,         /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 1000.0,         /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 10.0,            /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.13,   /* albedo of green leaves */
    "albedo_stem" : 0.1,    /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
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
      "base": 4.997,        /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.44          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 2.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 25.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : 15.5, "high" : 1000.0}, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.38009,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000334, /* scaling factor nesterov fire danger index */
    "vpd_par" : 47.22296, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 25, /* fuel bulk density */
    "emission_factor" : { "co2" : 1580.0, "co" :  103.0, "ch4" : 6.80, "voc" : 8.10, "tpm" : 8.50, "nox" : 1.999}, /* emission factors */
    "aprec_min" : APREC_MIN,  /* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,  /* flam */
    "k_litter10" : { "leaf" : 0.93, "wood" : 0.039 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 2.75, /* Q10_wood */
    "vmax_up" : 2.8,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_EG_MN, "median" : CN_BL_EG_MD, "high" : CN_BL_EG_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_EV_BR, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4, /* windspeed dampening */
    "roughness_length" : 2.0,  /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",  /* leaftype */
    "turnover" : {"leaf" : 2.0, "sapwood" : 30.0, "root" : 2.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 25.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "reprod_cost" : REPROD_COST,  /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.1487, /* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0301, /* bark thickness par1 */
    "barkthick_par2" : 0.0281, /* bark thickness par2 */
    "crown_mort_rck" : 1.00, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 20, "high" : 35},  /* Yu and Zhuang 2020 tropical forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTR,
    "bnf_cost" : BNF_COST
  },
/*--------------------------------------------------------------------------*/
/* 2. pft */
  {
    "name" : "tropical broadleaved raingreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.981,    /* beta_root  original 0.962*/
    "minwscal" : 0.35,      /* minwscal 3*/
    "gmin"  : 1.8,          /* gmin 4*/
    "respcoeff" : 0.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "sla" : 0.03233,         /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 1000.0,         /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 10.0,            /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.14,   /* albedo of green leaves */
    "albedo_stem" : 0.1,   /* albedo of stems */
    "albedo_litter" : 0.1, /* albedo of litter */
    "snowcanopyfrac" : 0.5, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.24,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 7.66,        /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.1           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.625,      /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 38.64,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.1           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 13.01,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.1           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 7.97,       /* new phenology: slope of water limiting function */
      "base" : 22.21,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.1348        /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "raingreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 2.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 25.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : 15.5, "high" : 1000.0}, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.51395,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000334, /* scaling factor nesterov fire danger index */
    "vpd_par" : 15.43195, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 13.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1664.0, "co" :  63.0, "ch4" : 2.20, "voc" : 3.40, "tpm" : 8.50, "nox" : 2.540}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 1.17, "wood" : 0.039 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 2.75, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_DC_MN, "median" : CN_BL_DC_MD, "high" : CN_BL_DC_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 2.0,  /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",  /* leaftype */
    "turnover" : {"leaf" : 1.0, "sapwood" : 30.0, "root" : 1.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 15.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.061, /* scorch height (F) */
    "crownlength" : 0.10, /* crown length (CL) */
    "barkthick_par1" : 0.1085, /* bark thickness par1 */
    "barkthick_par2" : 0.2120, /* bark thickness par2 */
    "crown_mort_rck" : 0.05, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 20, "high" : 35},  /* Yu and Zhuang 2020 tropical forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTR,
    "bnf_cost" : BNF_COST 
  },
/*---------------------------------------------------------------------------------------------*/
/* 3. pft */
  {
    "name": "temperate needleleaved evergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.976,    /* beta_root 1 */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 1.0,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.12,        /* resist 8*/
    "longevity" : 4.0,      /* leaf longevity 10*/
    "sla" : 0.01049,          /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 1000.0,         /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 900.0,      /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 5.0, //7.0,            /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.137,  /* albedo of green leaves */
    "albedo_stem" : 0.04,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.1, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.4,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.2172,     /* new phenology: slope of cold-temperature limiting function */
      "base" : -7.813,      /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.83,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 35.26,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 20,         /* new phenology: slope of light limiting function */
      "base" : 4.872,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5,          /* new phenology: slope of water limiting function */
      "base" : 8.613,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.7           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 42.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -2.0, "high" : 22.0 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.32198,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /*scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 25.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.7, "wood" : 0.041 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.97, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_NL_EG_MN, "median" : CN_NL_EG_MD, "high" : CN_NL_EG_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_EV, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0,     /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "needleleaved",/* leaftype */
    "turnover" : {"leaf" : 4.0, "sapwood" : 25.0, "root" : 4.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 15.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.1000,/* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0367, /* bark thickness par1 */
    "barkthick_par2" : 0.0592, /* bark thickness par2 */
    "crown_mort_rck" : 1.00, /* crown damage (rCK) */
    "crown_mort_p" : 3.75,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 16, "high" : 35},  /* Yu and Zhuang 2020 temperate coniferous forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTE,
    "bnf_cost" : BNF_COST 
 },
/*--------------------------------------------------------------------------*/
/* 4. pft */
  {
    "name" : "temperate broadleaved evergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.964,    /* beta_root 1 */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 1.5,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 1.6,      /* leaf longevity 10*/
    "sla" : 0.01986,         /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 1000.0,         /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 1200.0,     /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 7.0,            /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.15,   /* albedo of green leaves */
    "albedo_stem" : 0.04,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.55,       /* new phenology: slope of cold-temperature limiting function */
      "base" : -0.6297,     /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.6,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 41.12,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 18.83,      /* new phenology: slope of light limiting function */
      "base" : 39.32,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5,          /* new phenology: slope of water limiting function */
      "base" : 8.821,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.8           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 42.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : 3.0, "high" : 18.8 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.43740,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000334, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 22.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.86, "wood" : 0.104 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.37, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_EG_MN, "median" : CN_BL_EG_MD, "high" : CN_BL_EG_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_EV, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0, /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 1.0, "sapwood" : 25.0, "root" : 1.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 15.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.3710,/* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0451, /* bark thickness par1 */
    "barkthick_par2" : 0.1412, /* bark thickness par2 */
    "crown_mort_rck" : 0.95, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 18, "high" : 35},  /* Yu and Zhuang 2020 temperate evergreen forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTE,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 5. pft */
  {
    "name" : "temperate broadleaved summergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.966,    /* beta_root */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 1.0,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 120.0,          /* nmax 7*/
    "resist" : 0.3,         /* resist 8*/
    "longevity" : 0.45,     /* leaf longevity 10*/
    "sla" : 0.03233,          /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 300.0,          /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 1200.0,     /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 7.0,            /* emax 35*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.15,   /* albedo of green leaves */
    "albedo_stem" : 0.04,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.6,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.2591,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 5.69,        /* new phenology: inflection point of cold-temperature limiting function (deg C), originally 13.69 from Forkel etal. */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.74,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 41.51,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 58,         /* new phenology: slope of light limiting function */
      "base" : 59.78,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5.24,       /* new phenology: slope of water limiting function */
      "base" : 20.96,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.8           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "summergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 25.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -17.0, "high" : 15.5 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28880,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 22.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.95, "wood" : 0.104 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.37, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_DC_MN, "median" : CN_BL_DC_MD, "high" : CN_BL_DC_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0, /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 1.0, "sapwood" : 25.0, "root" : 1.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 25.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "aphen_min" : APHEN_MIN,
    "aphen_max" : APHEN_MAX,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.0940,/* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0347, /* bark thickness par1 */
    "barkthick_par2" : 0.1086, /* bark thickness par2 */
    "crown_mort_rck" : 1.00, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 18, "high" : 35},  /* Yu and Zhuang 2020 temperate deciduous forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTE,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 6. pft */
  {
    "name" : "boreal needleleaved evergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.955,    /* beta_root original 0.943*/
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.8,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.12,        /* resist 8*/
    "longevity" : 4.0,      /* leaf longevity 10*/
    "sla" : 0.01049,          /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 1000.0,         /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 600.0,      /* gdd5min 30*/
    "twmax" : 23.0,          /* twmax 31*/
    "twmax_daily" : 25.0,    /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 5.0, //7.0,            /* emax 35*/
    "intc" : 0.06,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.1,    /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.15, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.4,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
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
      "slope" : 14,         /* new phenology: slope of light limiting function */
      "base" : 3.04,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5,          /* new phenology: slope of water limiting function */
      "base" : 0.007695,    /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.8           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15.0, "high" : 25.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -32.5, "high" : -2.0}, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28670,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 25.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.76, "wood" : 0.041 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.97, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_NL_EG_MN, "median": CN_NL_EG_MD, "high" : CN_NL_EG_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_EV, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0, /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "needleleaved",/* leaftype */
    "turnover" : {"leaf" : 4.0, "sapwood" : 25.0, "root" : 4.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 20.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.1100,/* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0292, /* bark thickness par1 */
    "barkthick_par2" : 0.2632, /* bark thickness par2 */
    "crown_mort_rck" : 1.00, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 12, "high" : 25},  /* Yu and Zhuang 2020 boreal forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTBO,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 7. pft */
  {
    "name" : "boreal broadleaved summergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.955,    /* beta_root original 0.943*/
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.8,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.3,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "sla" : 0.03233,          /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 200.0,          /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 350.0,      /* gdd5min 30*/
    "twmax" : 23.0,          /* twmax 31*/
    "twmax_daily" : 25.0,    /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0, /* min_temprange 34*/
    "emax": 7.0,            /* emax 35*/
    "intc" : 0.06,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.1,    /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.15,/* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.2153,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 2.045,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.74,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 41.51,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 58,         /* new phenology: slope of light limiting function */
      "base" : 59.78,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5.24,       /* new phenology: slope of water limiting function */
      "base" : 20.96,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.7           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "summergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15.0, "high" : 25.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000, "high" : -2.0}, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28670,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 22.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.94, "wood" : 0.104 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.37, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_DC_MN, "median": CN_BL_DC_MD, "high" : CN_BL_DC_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0,     /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 1.0, "sapwood" : 25.0, "root" : 1.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 20.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "aphen_min" : APHEN_MIN,
    "aphen_max" : APHEN_MAX,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.0940,/* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0347, /* bark thickness par1 */
    "barkthick_par2" : 0.1086, /* bark thickness par2 */
    "crown_mort_rck" : 1.00, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 12, "high" : 25},  /* Yu and Zhuang 2020 boreal forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTBO,
    "bnf_cost" : BNF_COST
  },
/*--------------------------------------------------------------------------*/
/* 8. pft */
  {
    "name" : "boreal needleleaved summergreen tree",
    "type" : "tree",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.955,     /* beta_root original 0.943*/
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.3,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.12,        /* resist 8*/
    "longevity" : 0.65,     /* leaf longevity 10*/
    "sla" : 0.02118,        /* specific leaf area */
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 200.0,          /* ramp 19*/
    "lai_sapl" : 1.500,     /* lai_sapl 21*/
    "gdd5min" : 350.0,      /* gdd5min 30*/
    "twmax" : 23.0,          /* twmax 31*/
    "twmax_daily" : 25.0,    /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : 30.0,  /* min_temprange 34*/
    "emax": 5.0, //7.0,            /* emax 35*/
    "intc" : 0.06,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.12,   /* albedo of green leaves */
    "albedo_stem" : 0.05,   /* albedo of stems */
    "albedo_litter" : 0.01, /* albedo of litter */
    "snowcanopyfrac" : 0.15,/* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.6,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.21,       /* new phenology: slope of cold-temperature limiting function 0.15 */
      "base" : -4.165,      /* new phenology: inflection point of cold-temperature limiting function (deg C) -4.165*/
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.24,        /* new phenology: slope of warm-temperature limiting function tmax_sl 0.24*/
      "base" : 44.6,         /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2            /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 58,          /* new phenology: slope of light limiting function 95 */
      "base" : 59.78,        /* new phenology: inflection point of light limiting function (Wm-2)130.1  */
      "tau" : 0.2            /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5,           /* new phenology: slope of water limiting function */
      "base" : 2.344,        /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.8            /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "summergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15.0, "high" : 25.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -46, "high" : -5.4 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28670,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 22.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.76, "wood" : 0.041 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.97, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_NL_DC_MN, "median": CN_NL_DC_MD, "high" : CN_NL_DC_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0,     /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "needleleaved",/* leaftype */
    "turnover" : {"leaf" : 1.0, "sapwood" : 25.0, "root" : 1.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 20.0, /* crownarea_max 20*/
    "wood_sapl" : 1.2,      /* sapwood sapling 22*/
    "aphen_min" : 10,
    "aphen_max" : 200,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : ALLOM1,      /* allometry */
    "allom2" : ALLOM2,
    "allom3" : ALLOM3,
    "allom4" : ALLOM4,
    "k_latosa" : K_LATOSA,
    "height_max" : HEIGHT_MAX, /* maximum height of tree */
    "scorchheight_f_param" : 0.0940,/* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0347, /* bark thickness par1 */
    "barkthick_par2" : 0.1086, /* bark thickness par2 */
    "crown_mort_rck" : 1.00, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est": K_EST,         /* k_est */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 12, "high" : 25},  /* Yu and Zhuang 2020 boreal forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTBO,
    "bnf_cost" : BNF_COST
 },
/*--------------------------------------------------------------------------*/
/* 9. pft */
  {
    "name" : "Tropical C4 grass",
    "type" : "grass",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [39.0, 61.0, 74.0, 80.0], /* curve number */
    "beta_root" : 0.972,    /* beta_root */
    "minwscal" : 0.20,      /* minwscal 3*/
    "gmin"  : 1.5,          /* gmin 4*/
    "respcoeff" : 0.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.01,        /* resist 8*/
    "longevity" : 0.4,      /* leaf longevity 10*/
    "sla" : 0.040373,        /* specific leaf area */
    "lmro_ratio" : 0.6,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 100.0,          /* ramp 19*/
    "lai_sapl" : 0.1,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 10.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.21,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.4,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX_GRASS,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "any",      /* phenology */
    "path" : "C4",            /* pathway */
    "temp_co2" : { "low" : 6.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 45.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC4, /* leaf respiration as fraction of Vmax for C4 plants */
    "temp" : { "low" : 7.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.46513,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 3.85899, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1664.0, "co" :  63.0, "ch4" : 2.20, "voc" : 3.40, "tpm" : 8.50, "nox" : 2.540}, /* emission factors */
    "aprec_min" : 100,                        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,     /* flam */
    "k_litter10" : { "leaf" : 0.97, "wood" : 0.97 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.0, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.19,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_GC4_MN, "median" : CN_GC4_MD, "high" : CN_GC4_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.05,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_GR, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6,/* windspeed dampening */
    "roughness_length" : 0.03,  /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C4, "humid" : IRRIG_THRESHOLD_C4 },
    "turnover" : {"leaf" : 1.0, "root" : 2.0}, /* turnover leaf  root 9 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, and root 13,15*/
    "ratio" : 1.16,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 20, "high" : 35},  /* Yu and Zhuang 2020 grassland*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTR,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 10. pft */
  {
    "name" : "Temperate C3 grass",
    "type" : "grass",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [39.0, 61.0, 74.0, 80.0], /* curve number */
    "beta_root" : 0.943,    /* beta_root */
    "minwscal" : 0.20,      /* minwscal 3*/
    "gmin"  : 0.8,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.01,        /* resist 8*/
    "longevity" : 0.35,     /* leaf longevity 10*/
    "sla" : 0.042242,        /* specific leaf area */
    "lmro_ratio" : 0.8,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 100.0,          /* ramp 19*/
    "lai_sapl" : 0.1,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 7.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.23,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX_GRASS,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "any",      /* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 45.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 10.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -39.0, "high" : 15.5 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.38184,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 4.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 100,       /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,     /* flam */
    "k_litter10" : { "leaf" : 1.2, "wood" : 1.2 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.0, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.19,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_GC3_MN, "median" : CN_GC3_MD, "high" : CN_GC3_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.05,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_GR, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "turnover" : {"leaf" : 1.0, "root" : 2.0}, /* turnover leaf  root 9 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, and root 13,15*/
    "ratio" : 1.16,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 18, "high" : 35},  /* Yu and Zhuang 2020 grassland*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTE,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 11. pft */
  {
    "name" : "Polar C3 grass",
    "type" : "grass",
    "cultivation_type" : "none", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [39.0, 61.0, 74.0, 80.0], /* curve number */
    "beta_root" : 0.943,    /* beta_root */
    "minwscal" : 0.20,      /* minwscal 3*/
    "gmin"  : 0.8,          /* gmin 4*/
    "respcoeff" : 1.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.01,        /* resist 8*/
    "longevity" : 0.35,     /* leaf longevity 10*/
    "sla" : 0.042242,        /* specific leaf area */
    "lmro_ratio" : 0.6,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 100.0,          /* ramp 19*/
    "lai_sapl" : 0.1,     /* lai_sapl 21*/
    "gdd5min" : 0.01,       /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : 18.0,  /* min_temprange 34*/
    "emax": 7.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : ALPHAA_NITROGEN, /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.23,   /* albedo of green leaves */
    "albedo_stem" : 0.1,    /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.13,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 2.79,        /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 26.12,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 50,          /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.38          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.88,       /* new phenology: slope of water limiting function */
      "base" : 1,           /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.94          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX_GRASS,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "any",      /* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 10.0, "high" : 25.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : -2.6 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.38184,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 4.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,     /* flam */
    "k_litter10" : { "leaf" : 1.2, "wood" : 1.2 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.0, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.19,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_GC3_MN, "median" : CN_GC3_MD, "high" : CN_GC3_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.05,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER_GR, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6,  /* windspeed dampening */
    "roughness_length" : 0.03,    /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "turnover" : {"leaf" : 1.0, "root" : 2.0}, /* turnover leaf  root 9 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, and root 13,15*/
    "ratio" : 1.16,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 12, "high" : 25},  /* Yu and Zhuang 2020 grassland*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTBO,
    "bnf_cost" : BNF_COST 
  },
/*----------------------------------------------------------------------------------------*/
/* 1. bft */
  {
    "name" : "bioenergy tropical tree",
    "type" : "tree",
    "cultivation_type" : "biomass",/* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.975,    /* beta_root */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.64,          /* gmin 4; increase for N-version acc. to proportion in LPJmL4 difference PFT1*/
    "respcoeff" : 0.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 1.0,         /* resist 8*/
    "longevity" : 2.0,      /* leaf longevity 10*/
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 1000.0,         /* ramp 19*/
    "lai_sapl" : 1.6,       /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 10.0,            /* emax 35; increase for N-version acc. to proportion in LPJmL4 difference PFT1*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : 0.8,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
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
    "mort_max" : 0.005,     /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "evergreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 2.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 25.0, "high" : 38.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : 7.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.38009,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.000034, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 13.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1664.0, "co" :  63.0, "ch4" : 2.20, "voc" : 3.40, "tpm" : 8.50, "nox" : 2.540}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.93, "wood" : 0.039 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 2.75, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_EG_MN, "median" : CN_BL_EG_MD, "high" : CN_BL_EG_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.15,       /* knstore */
    "fn_turnover" : FN_TURNOVER_EV_BR, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4, /* windspeed dampening */
    "roughness_length" : 1.0,/* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 2.0, "sapwood" : 10.0, "root" : 2.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 2.0,  /* crownarea_max 20*/
    "wood_sapl" : 2.2,      /* sapwood sapling 22*/
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : 110,         /* allometry */
    "allom2" : 35,
    "allom3" : 0.75,
    "allom4" : ALLOM4,
    "k_latosa" : 4000,      /* represent species investing in biomass (wood) production */
    "height_max" : 8,       /* maximum height of tree */
    "scorchheight_f_param" : 0.061,/* scorch height (F) */
    "crownlength" : 0.10 , /* crown length (CL) */
    "barkthick_par1" : 0.1085, /* bark thickness par1 */
    "barkthick_par2" : 0.2120, /* bark thickness par2 */
    "crown_mort_rck" : 0.05, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est" : 0.5,          /* k_est,  Giardina, 2002 */
    "rotation" : 8,         /* rotation */
    "max_rotation_length" : 40 /* max_rotation_length */
  },
/*--------------------------------------------------------------------------*/
/* 2. bft */
  {
    "name" : "bioenergy temperate tree",
    "type" : "tree",
    "cultivation_type" : "biomass",/* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [30.0, 55.0, 70.0, 77.0], /* curve number */
    "beta_root" : 0.966,    /* beta_root */
    "minwscal" : 0.00,      /* minwscal 3*/
    "gmin"  : 0.4,          /* gmin 4; increase for N-version acc. to proportion in LPJmL4 difference PFT5*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 120.0,          /* nmax 7*/
    "resist" : 0.95,        /* resist 8*/
    "longevity" : 0.45,     /* leaf longevity 10*/
    "lmro_ratio" : 1.0,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 300.0,          /* ramp 19*/
    "lai_sapl" : 1.6,       /* lai_sapl 2.1, larger sapling used for plantations, original value 1.5  */
    "gdd5min" : 300.0,      /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 7.0,            /* emax 35; increase for N-version acc. to proportion in LPJmL4 difference PFT5*/
    "intc" : 0.02,          /* intc 36*/
    "alphaa" : 0.8,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.14,   /* albedo of green leaves */
    "albedo_stem" : 0.04,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.6,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.2153,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 5,           /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.74,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 41.51,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 58,         /* new phenology: slope of light limiting function */
      "base" : 59.78,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 5.24,       /* new phenology: slope of water limiting function */
      "base" : 20.96,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.8           /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : 0.005,     /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "summergreen", /* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 38.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -30.0, "high" : 8 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.28880,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 22.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_TREE,     /* flam */
    "k_litter10" : { "leaf" : 0.95, "wood" : 0.104 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.37, /* Q10_wood */
    "vmax_up" : 2.8,              /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": CN_BL_DC_MN, "median" : CN_BL_DC_MD, "high" : CN_BL_DC_MX}, /* 10.7 79.4 cnleaf min max, based on TRY data, prepared by Boris Sakschewski */
    "knstore" : 0.15,       /* knstore */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.4,  /* windspeed dampening */
    "roughness_length" : 1.0,     /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "leaftype" : "broadleaved",/* leaftype */
    "turnover" : {"leaf" : 1.0, "sapwood" : 10.0, "root" : 1.0}, /* turnover leaf  sapwood root 9 11 12*/
    "cn_ratio" : {"leaf" : CTON_LEAF, "sapwood" :  CTON_SAP, "root" : CTON_ROOT}, /* C:N mass ratio for leaf, sapwood, and root 13,14,15*/
    "ratio" : {"sapwood" :  RATIO_SAPW, "root" :  1.16}, /* relative C:N ratio of sapwood and root, Friend et al. 1997, Ecological Modeling, Table 4*/
    "crownarea_max" : 1.5,  /* crownarea_max 20*/
    "wood_sapl" : 2.5,      /* sapwood sapling 22*/
    "aphen_min" : APHEN_MIN,
    "aphen_max" : APHEN_MAX,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "allom1" : 110,         /* allometry */
    "allom2" : 35,
    "allom3" : 0.75,
    "allom4" : ALLOM4,
    "k_latosa" : 4000,    /* represent species investing in biomass (wood) production */
    "height_max" : 8,       /* maximum height of tree */
    "scorchheight_f_param" : 0.0940, /* scorch height (F) */
    "crownlength" : 0.3334, /* crown length (CL) */
    "barkthick_par1" : 0.0347, /* bark thickness par1 */
    "barkthick_par2" : 0.1086, /* bark thickness par2 */
    "crown_mort_rck" : 1.0, /* crown damage (rCK) */
    "crown_mort_p" : 3.00,  /* crown damage (p)     */
    "fuelfraction" : [0.045,0.075,0.21,0.67], /* fuel fraction */
    "k_est" : 0.8,          /* k_est TIM 1.5*/
    "rotation" : 8,         /* rotation */
    "max_rotation_length" : 40, /* max_rotation_length */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 18, "high" : 35},  /* Yu and Zhuang 2020 temperate deciduous forest*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTE,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 3. bft ONLY FOR BIOENERGY*/
  {
    "name" : "bioenergy C4 grass",
    "type" : "grass",
    "cultivation_type" : "biomass", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [39.0, 61.0, 74.0, 80.0], /* curve number */
    "beta_root" : 0.972,    /* beta_root */
    "minwscal" : 0.20,      /* minwscal 3*/
    "gmin"  : 1.5,          /* gmin 4; acc. to PFT 9*/
    "respcoeff" : 0.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 1.0,         /* resist 8*/
    "longevity" : 0.25,     /* leaf longevity 10*/
    "sla" : 0.0825,         /* median for Miscanthus in TRY database */
    "lmro_ratio" : 2.50,   /* somewhere in the range of literature values H08, Guo, Rakic*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 100.0,          /* ramp 19*/
    "lai_sapl" : 0.1,     /* lai_sapl 21; acc. to PFT 9*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 5.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 10.0,            /* emax 35; acc. to PFT 9*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 0.8,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.21,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.1,  /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.6,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "any",      /* phenology */
    "path" : "C4",            /* pathway */
    "temp_co2" : { "low" : 4.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 15.0, "high" : 45.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC4, /* leaf respiration as fraction of Vmax for C4 plants */
    "temp" : { "low" : -40.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.46513,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor nesterov fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1664.0, "co" :  63.0, "ch4" : 2.20, "voc" : 3.40, "tpm" : 8.50, "nox" : 2.540}, /* emission factors */
    "aprec_min" : APREC_MIN,/* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,     /* flam */
    "k_litter10" : { "leaf" : 0.97, "wood" : 0.97 }, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1.0, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.19,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 34.0, "median" : 90.0, "high" : 132.0}, /* interannual variability in Heaton et al. (2009) */
    "knstore" : 0.05,       /* knstore, acc. to PFT 9 */
    "fn_turnover" : FN_TURNOVER_BMGR, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6,/* windspeed dampening */
    "roughness_length" : 0.03,  /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C4, "humid" : IRRIG_THRESHOLD_C4 },
    "turnover" : {"leaf" : 1.0, "root" : 2.0}, /* no leaf turnover for biomass grasses anymore --> all C in harvest, N partially recovered*/
    "cn_ratio" : {"leaf" : 90.0, "root" : CTON_ROOT}, /* C:N mass ratio for leaf and root 13,15*/
    "ratio" : 1.16,
    "reprod_cost" : REPROD_COST, /* reproduction cost */
    "temp_bnf_lim" : {"low" : TEMP_BNF_MIN, "high" : TEMP_BNF_MAX},
    "temp_bnf_opt" : {"low" : 18, "high" : 35},  /* Yu and Zhuang 2020 grassland*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],
    "nfixpot" : 0.01,  /* Yu and Zhuang 2020 minimum*/
    "maxbnfcost" : MAXBNFCOSTTR,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 1. cft */
  {
    "name" : "temperate cereals",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.0001,    /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 0.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22         /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 0.0, "high" : 40.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 12.0, "high" : 17.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03, /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "temp wtyp calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 258, "sdatesh" : 90, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 330,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 12, "temp_spring" :  5, "temp_vern" : 12, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : -4, "high" : 17 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 3, "high" : 10 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 70,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 3, "high" : 10 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 8,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 20,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 1700.0,  "high" : 2876.9}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1000.0,  "high" : 2648.4}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 0.0, "high" :  0.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.05,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.05,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.45,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.7,        /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 2.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.50,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.20,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.99, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 2. cft */
  {
    "name" : "rice",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.33,     /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 10.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 6.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 45.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : 1.0, "humid" : 1.0 },
    "calcmethod_sdate" : "prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 100, "sdatesh" : 180, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 180,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  18, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 24,              /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 0,               /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1600.0,  "high" : 1800.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 10.0, "high" :  10.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.10,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.05,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.80,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.50,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.25,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 1.3, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 3. cft */
  {
    "name" : "maize",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.33,     /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 6.0,         /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C4",            /* pathway */
    "temp_co2" : { "low" : 8.0, "high" : 42.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 21.0, "high" : 26.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC4, /* leaf respiration as fraction of Vmax for C4 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6,  /* windspeed dampening */
    "roughness_length" : 0.03,    /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C4, "humid" : IRRIG_THRESHOLD_C4 },
    "calcmethod_sdate" : "temp prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 1, "sdatesh" : 181, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 240,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  14, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1600.0,  "high" : 1600.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 5.0, "high" :  15.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.10,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.05,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.75,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 4.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.50,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.30,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.83, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 4. cft */
  {
    "name" : "tropical cereals",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 10.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C4",            /* pathway */
    "temp_co2" : { "low" : 6.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 45.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC4, /* leaf respiration as fraction of Vmax for C4 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C4, "humid" : IRRIG_THRESHOLD_C4 },
    "calcmethod_sdate" : "prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 80, "sdatesh" : 260, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 240,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  12, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1500.0,  "high" : 1500.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 10.0, "high" :  10.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.01,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.85,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.25,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.10,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.99, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 5. cft */
  {
    "name" : "pulses",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 1.0,         /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 45.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 10.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "temp prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 1, "sdatesh" : 181, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 300,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  10, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 2000.0,  "high" : 2000.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 1.0, "high" :  1.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.01,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.90,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 4.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 4.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.45,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.10,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.45, "pool": 3},                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
    "temp_bnf_lim" : {"low" : 1, "high" : 40},  /* Ma et al., 2022 faba bean*/
    "temp_bnf_opt" : {"low" : 16, "high" : 25},  /* Ma et al., 2022 faba bean*/
    "swc_bnf" : {"low" : SWC_BNF_LOW, "high" : SWC_BNF_HIGH},  /* Ma et al., 2022 faba bean*/
    "phi_bnf" : [PHI_BNF_1, PHI_BNF_2],  /* Ma et al., 2022 faba bean*/
    "nfixpot" : 0.5,  /* Yu and Zhuang 2020 middle of proposed range*/
    "maxbnfcost" : MAXBNFCOST,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 6. cft */
  {
    "name": "temperate roots",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 3.0,         /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : -4.0, "high" : 45.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 10.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6,   /* windspeed dampening */
    "roughness_length" : 0.03,     /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "temp styp calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 90, "sdatesh" : 270, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 260,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  8, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 2700.0,  "high" : 2700.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 3.0, "high" :  3.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.05,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.75,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.75, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 3.5,          /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 1.25,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 0.5, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 1.74, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 7. cft */
  {
    "name" : "tropical roots", /* re-parameterized as Cassava, 14.12.2009 KW */
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 15.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 6.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 45.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 80, "sdatesh" :  180, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 330,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  22, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 2000.0,  "high" : 2000.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 15.0, "high" :  15.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.05,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.75,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.75, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 2.0,          /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 1.10,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 0.5, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 3.27, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 8. cft */
  {
    "name" : "oil crops sunflower",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.3,       /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.33,     /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 6.0,         /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 8.0, "high" : 42.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 25.0, "high" : 32.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "temp styp calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 1, "sdatesh" :  181, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 240,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  13, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1000.0,  "high" : 1600.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 2460,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 6.0, "high" :  6.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.01,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.70,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.40,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.20,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 1.04, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 9. cft */
  {
    "name" : "oil crops soybean",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : true,           /* N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.66,     /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 10.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 5.0, "high" : 45.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 28.0, "high" : 32.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 140, "sdatesh" :  320, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 240,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  13, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 70,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1000.0,  "high" : 1000.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 10.0, "high" :  10.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.05,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.70,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.40,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.10,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 0.5, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.42, "pool": 3},                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
    "temp_bnf_lim" : {"low" : 5, "high" : 44},  /* Ma et al., 2022 soy bean*/
    "temp_bnf_opt" : {"low" : 20, "high" : 35},  /* Ma et al., 2022 soy bean*/
    "swc_bnf" : {"low" : 0.2, "high" : 0.8},  /* Ma et al., 2022 soy bean*/
    "phi_bnf" : [-0.33, 1.67],  /* Ma et al., 2022 soy bean*/
    "nfixpot" : 0.5,  /* Yu and Zhuang 2020 middle of proposed range*/
    "maxbnfcost" : MAXBNFCOST,
    "bnf_cost" : BNF_COST 
  },
/*--------------------------------------------------------------------------*/
/* 10. cft */
  {
    "name": "oil crops groundnut",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.5,      /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 14.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 6.0, "high" : 55.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 20.0, "high" : 45.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 100, "sdatesh" :  280, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 240,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 30,     /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  15, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 70,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1500.0,  "high" : 1500.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 14.0, "high" :  14.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.15,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.01,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.75,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 5.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 5.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.40,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.30,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 0.5, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.68, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 11. cft */
  {
    "name" : "oil crops rapeseed",
    "type" : "crop",
    "cultivation_type" : "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 1.0,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.41,     /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 0.0,         /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.3111,     /* new phenology: slope of cold-temperature limiting function */
      "base" : 4.979,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.01011       /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 0.24,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 32.04,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 23,         /* new phenology: slope of light limiting function */
      "base" : 75.94,       /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.22          /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.5222,     /* new phenology: slope of water limiting function */
      "base" : 53.07,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.01001       /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C3",            /* pathway */
    "temp_co2" : { "low" : 0.0, "high" : 40.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 12.0, "high" : 17.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC3, /* leaf respiration as fraction of Vmax for C3 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C3_DRY, "humid" : IRRIG_THRESHOLD_C3_HUMID },
    "calcmethod_sdate" : "temp wtyp calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 241, "sdatesh" :  61, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 210,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 0,      /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 17, "temp_spring" :  5, "temp_vern" : 12, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : -4, "high" : 17 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 3, "high" : 10 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 70,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 3, "high" : 10 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 8,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 20,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 2100.0,  "high" : 3279.7}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 1000.0,  "high" : 2648.4}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,        /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 0.0, "high" :  0.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.05,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.01,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.50,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.85,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.00, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 7.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 7.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.30,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.15,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 0.76, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  },
/*--------------------------------------------------------------------------*/
/* 12. cft */
  {
    "name" : "sugarcane",
    "type" : "crop",
    "cultivation_type": "annual crop", /* cultivation_type */
    "nfixing" : false,           /* No N fixing in Ma et al., 2022 appraoch */
    "cn" : [60.0, 72.0, 80.0, 84.0], /* curve number */
    "beta_root" : 0.969,    /* beta_root */
    "minwscal" : 0.30,      /* minwscal 3*/
    "gmin"  : 0.5,          /* gmin 4*/
    "respcoeff" : 0.2,      /* respcoeff 5*/
    "nmax" : 100.0,          /* nmax 7*/
    "resist" : 0.5,         /* resist 8*/
    "longevity" : 0.66,     /* leaf longevity 10*/
    "lmro_ratio" : 1.5,     /* lmro_ratio 18*/
    "lmro_offset" : 0.5,     /* lmro_ratio 18*/
    "ramp" : 500.0,          /* ramp 19*/
    "lai_sapl" : 0.001,     /* lai_sapl 21*/
    "gdd5min" : 0.0,        /* gdd5min 30*/
    "twmax" : 1000.0,        /* twmax 31*/
    "twmax_daily" : 1000.0,  /* twmax_daily 31*/
    "gddbase" : 6.0,        /* gddbase (deg C) 33*/
    "min_temprange" : -1000.0,/* min_temprange 34*/
    "emax": 8.0,            /* emax 35*/
    "intc" : 0.01,          /* intc 36*/
    "alphaa" : 1.0,         /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    "albedo_leaf" : 0.18,   /* albedo of green leaves */
    "albedo_stem" : 0.15,   /* albedo of stems */
    "albedo_litter" : 0.06, /* albedo of litter */
    "snowcanopyfrac" : 0.4, /* maximum snow coverage in green canopy */
    "lightextcoeff" : 0.5,  /* lightextcoeff, light extinction coeffcient in Lambert-Beer equation */
    "tmin" :
    {
      "slope" : 0.91,       /* new phenology: slope of cold-temperature limiting function */
      "base" : 6.418,       /* new phenology: inflection point of cold-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day cold-temperature limiting fct */
    },
    "tmax" :
    {
      "slope" : 1.47,       /* new phenology: slope of warm-temperature limiting function tmax_sl */
      "base" : 29.16,       /* new phenology: inflection point of warm-temperature limiting function (deg C) */
      "tau" : 0.2           /* new phenology: change rate of actual to previous day warm-temperature limiting fct */
    },
    "light" :
    {
      "slope" : 64.23,      /* new phenology: slope of light limiting function */
      "base" : 69.9,        /* new phenology: inflection point of light limiting function (Wm-2) */
      "tau" : 0.4           /* new phenology: change rate of actual to previous day light limiting function */
    },
    "wscal" :
    {
      "slope" : 0.1,        /* new phenology: slope of water limiting function */
      "base" : 41.72,       /* new phenology: inflection point of water limiting function (% water availability)  */
      "tau" : 0.17          /* new phenology: change rate of actual to previous day water limiting function */
    },
    "mort_max" : MORT_MAX,  /* asymptotic maximum mortality rate (1/year) */
    "phenology" : "cropgreen",/* phenology */
    "path" : "C4",            /* pathway */
    "temp_co2" : { "low" : 8.0, "high" : 42.0 }, /* lower and upper temperature limit for co2 (deg C) 24 27*/
    "temp_photos" : { "low" : 18.0, "high" : 30.0},/* lower and upper limit of temperature optimum for photosynthesis(deg C) 25 26*/
    "b":  BC4, /* leaf respiration as fraction of Vmax for C4 plants */
    "temp" : { "low" : -1000.0, "high" : 1000 }, /* lower and upper coldest monthly mean temperature(deg C) 28 29*/
    "soc_k" : 0.40428,     /* shape factor for soil organic matter vertical distribution*/
    "alpha_fuelp" : 0.0000667, /* scaling factor fire danger index */
    "vpd_par" : 6, /*scaling factor vpd fire danger index*/
    "fuelbulkdensity" : 2.0, /* fuel bulk density */
    "emission_factor" : { "co2" : 1568.0, "co" :  106.0, "ch4" : 4.80, "voc" : 5.70, "tpm" : 17.60, "nox" : 3.240}, /* emission factors */
    "aprec_min" : 0,        /* minimum annual precipitation to establish */
    "flam" : FLAM_GRASS,    /* flam */
    "k_litter10" : { "leaf" : K_LITTER10, "wood" : K_LITTER10}, /* K_LITTER10 turnover rate after Brovkin etal 2012*/
    "k_litter10_q10_wood" : 1, /* Q10_wood */
    "vmax_up" : 5.51,             /* vmax_up, Maximum N uptake capacity per unit fine root mass, Smith et al. 2014 */
    "kNmin": 0.05,                /* kNmin, Rate of N uptake not associated with Michaelis-Menten Kinetics, Zaehle&Friend 2000 */
    "KNmin" : 1.48,               /* KNmin, Half saturation concentration of fine root N uptake, Smith et al. 2014 */
    "cnratio_leaf": {"low": 14.3, "median" : CN_CROPS_MEAN, "high" : 58.8},  /* cnleaf min max, White et al. 2000 doi: 10.1175/1087-3562(2000)004<0003:PASAOT>2.0.CO;2*/
    "knstore" : 0.1,       /* knstore, Smith et al. 2014 */
    "fn_turnover" : FN_TURNOVER, /* fraction of N not recovered before turnover */
    "windspeed_dampening" : 0.6, /* windspeed dampening */
    "roughness_length" : 0.03,   /* roughness length */
    "irrig_threshold" : { "dry" : IRRIG_THRESHOLD_C4, "humid" : IRRIG_THRESHOLD_C4 },
    "calcmethod_sdate" : "temp prec calc", /* calc_sdate: method to calculate the sowing date*/
    "sdatenh" : 120, "sdatesh" : 300, /* sdatenh,sdatesh: init sowing date for northern and southern hemisphere (julian day) */
    "hlimit" : 360,         /* hlimit: max length of crop cycle  */
    "fallow_days" : 0,      /* fallow_days: wait after harvest until next sowing */
    "temp_fall" : 1000, "temp_spring" :  14, "temp_vern" : 1000, /* temp_fall, temp_spring, temp_vernalization: thresholds for sowing date f(T)*/
    "tv_eff" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is possible (deg C)*/
    "tv_opt" : { "low" : 1000, "high" : 1000 }, /* min & max tv: lower and upper temperature threshold under which vernalization is optimal (deg C)*/
    "pvd_max" : 0,              /* pvd_max: number of vernalising days required*/
    "trg" : { "low" : 1000, "high" : 1000 }, /* min & max trg: temperature under which vernalization is possible (deg C)*/
    "pvd" : 0,              /* pvd: number of vernalising days required*/
    "psens": 1.0,           /* psens: sensitivity to the photoperiod effect [0-1](1 means no sensitivity)*/
    "pb" : 0,               /* pb: basal photoperiod (h)(pb<ps for longer days plants)*/
    "ps" : 24,              /* ps: saturating photoperiod (h) (ps<pb for shorter days plants)*/
    "phuw" : { "low" : 0.0,  "high" : 0.0}, /* min & max phu: potential heat units required for plant maturity winter(deg C)*/
    "phus" : { "low" : 2000.0,  "high" : 4000.0}, /* min & max phu: potential heat units required for plant maturity summer(deg C)*/
    "phu_par" : 9999,       /* phu parameter for determining the variability of phu */
    "basetemp": { "low" : 11.0, "high" :  15.0}, /* min & max basetemp: base temperature */
    "fphuc" : 0.01,         /* fphuc: fraction of growing season 1 [0-1]*/
    "flaimaxc" : 0.01,      /* flaimaxc: fraction of plant maximal LAI 1 [0-1]*/
    "fphuk" : 0.40,         /* fphuk: fraction of growing season 2 [0-1]*/
    "flaimaxk" : 0.95,      /* flaimaxk: fraction of plant maximal LAI 2 [0-1]*/
    "fphusen" : 0.95,       /* fphusen: fraction of growing period at which LAI starts decreasing [0-1]*/
    "flaimaxharvest" : 0.50, /* flaimaxharvest: fraction of plant maximal LAI still present at harvest [0-1]*/
    "laimax" : 6.0,         /* laimax: plant maximal LAI (m2leaf/m2)*/
    "laimin" : 2.0,         /* laimin: plant maximal LAI (m2leaf/m2) in 1950*/
    "hiopt" : 0.80,         /* hiopt: optimum harvest index HI reached at harvest*/
    "himin" : 0.80,         /* himin: minimum harvest index HI reached at harvest*/
    "shapesenescencenorm" : 2.0, /* shapesenescencenorm */
    "cn_ratio" : {"leaf" : CTON_LEAF, "root" : CTON_ROOT, "so" : CTON_SO, "pool" : CTON_POOL}, /* C:N mass ratio for root, storage organ, and pool */
    "ratio" : {"root": 1.16, "so": 4.57, "pool": 3}                    /* relative C:N ratios of root, storage organ, and pool; for roots: Friend et al. 1997, Ecological Modeling, Table 4 */
  }
],
/*--------------------------------------------------------------------------*/
