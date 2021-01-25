/**************************************************************************************/
/**                                                                                \n**/
/**              l  p  j  p  a  r  a  m  _  v  p  d  .  j  s                       \n**/
/**                                                                                \n**/
/**     LPJ parameter file for LPJmL version 5.3.001                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

"param" :
{
  "k_litter10" : 0.3,        /* k_litter10  (1/yr) */
  "k_soil10" : { "fast" : 0.03, "slow":  0.001}, /* fast, slow k_soil10  (1/yr) */
  "maxsnowpack": 20000.0,    /* max. snow pack (mm) */
  "soildepth_evap" : 300.0,  /* depth of sublayer at top of upper soil layer (mm) */
  "soil_infil" : 2.0,        /* default soil infiltration */
  "co2_p" : 278.0,           /* pre-industrial CO2 (ppmv) */
  "k" : 0.0548,              /* k    k = 7.4e-7 * atomic_mass_C / atomic_mass_N * seconds_per_day = 0.0548 Sprugel et al. 1996, Eqn 7*/
  "theta" : 0.7,             /* theta */
  "k_beer" : 0.5,            /* k_beer */
  "alphac3" : 0.08,          /* alphac3 */
  "alphac4" : 0.053,         /* alphac4 */
  "bc3" : 0.015,             /* bc3 leaf respiration as fraction of Vmax for C3 plants */
  "bc4" : 0.035,             /* bc4 leaf respiration as fraction of Vmax for C4 plants */
  "r_growth" : 0.25,         /* r_growth */
  "GM" : 3.26,               /* GM empirical parameter in demand function */
  "ALPHAM" : 1.391,          /* ALPHAM Priestley-Taylor coefficient*/
  "ko25" : 3.0e4,            /* Michaelis constant for O2 (Pa) at 25 deg C */
  "kc25" : 30.,              /* Michaelis constant for CO2 (Pa) at 25 deg C */
  "atmfrac" : 0.7,           /* atmfrac */
  "fastfrac" : 0.98,         /* fastfrac */
  "veg_equil_year" : 990,
  "bioturbate" : 0.5,        /* bioturbation (0 = no bioturbation) */
  "k_max": 0.10,             /* k_max, maximum fraction of soil->NH4 assumed to be nitrifi
ed Parton, 2001*/
  "fpc_tree_max" : 0.95,     /* maximum foliage projective cover for trees */
  "temp_response_a" : 66.02, /* Parameter in temperature response function */
  "temp_response_b" : 56.02, /* Parameter in temperature response function */
  "k_2": 0.02,               /* k_2, fraction of nitrified N lost as N20 flux Parton, 2001
*/
  "p" : 25,                  /* Haxeltine & Prentice regression coefficient */
  "n0" : 7.15,               /* Haxeltine & Prentice regression coefficient */
  "k_temp" : 0.02,           /* factor of temperature dependence of nitrogen demand for Ru
bisco activity */
  "denit_threshold" : 0.8,   /* denitrification threshold */
  "min_c_bnf" : 20,          /* threshold C root content for BNF */
  "par_sink_limit" : 0.2,    /* Michaelis-Menten scaler of sink limitation */
  "q_ash" : 0.45,            /* fraction of nitrogen going to litter after fire */
  "sapwood_recovery" : 0.3,  /* recovery of sapwood nitrogen */
  "T_m" : 15.0,              /* parameter in N uptake temperature function */
  "T_0" : -25.0,             /* parameter in N uptake temperature function */
  "T_r" : 15.0,              /* parameter in N uptake temperature function */
  "k_mort" : 0.2,            /* coefficient of growth efficiency in mortality equation (k_mort2) */
  "residue_rate": 200,       /* fixed residue rate in gC/m2/yr, ignored if <=0 and if pool >0  */
  "residue_pool" : 100,      /* fixed aboveground residue pool in gC/m2, ignored if <=0, overrules constant rate */
  "residue_cn": 20,         /* CN ratio of prescribed residues */
  "residue_fbg": 0.25,      /* belowground fraction of prescribed residues */
  "residue_frac" : 0.95,      /* fraction of residues to be submerged by tillage */
  "manure_cn": 14.5,        /* CN ration of manure gC/gN */
  "fertilizer_rate" : 10,     /* default: 20; fixed fertilizer application rate in gN/m2/yr */
  "manure_rate" : 0,          /* default: 20; fixed manure application rate in gN/m2/yr */
  "mixing_efficiency" : 0.9,  /* mixing efficiency of tillage */
  "fpc_tree_max" : 0.95,     /* maximum foliage projective cover for trees */
  "temp_response_a" : 56.02, /* Parameter in temperature response function */
  "temp_response_b" : 46.02, /* Parameter in temperature response function */
  "lsuha" : 0.0,             /* livestock density */
  "aprec_lim" : 900,         /* annual prec limit for C3 threshold*/
  "irrig_threshold_c3_dry" : 0.8,     /* irrigation threshold C3, prec < aprec_lim */
  "irrig_threshold_c3_humid" : 0.9,   /* irrigation threshold C3, prec >= aprec_lim */
  "irrig_threshold_c4" : 0.7,         /* irrigation threshold C4 */
  "irrig_threshold_rice" : 1.0,       /* irrigation threshold RICE */
  "irrig_soilfrac" : 1.0,             /* fraction of soil filled with water during irrigation event */
  "canal_conveyance_eff_sand" : 0.7,  /* open canal conveyance efficiency, soil type sand (Ks > 20)*/
  "canal_conveyance_eff_loam" : 0.75, /* open canal conveyance efficiency, soil type loam (10<=Ks<=20)*/
  "canal_conveyance_eff_clay" : 0.8,  /* open canal conveyance efficiency, soil type clay (Ks<10) */
  "pipe_conveyance_eff" : 0.95,       /* pressurized conveyance efficiency*/
  "saturation_level_surf" : 1.15,     /* saturation level surface irrigation*/
  "saturation_level_sprink" : 0.55,   /* saturation level sprinkler irrigation*/
  "saturation_level_drip" : 0.05,     /* saturation level drip irrigation*/
  "drip_evap_reduction" : 0.6,        /* reduction of drip soil evap */
  "residues_in_soil" : 0.1,           /* minimum residues in soil*/
  "esoil_reduction" : 0.0,            /* reduction of soil evaporation */
  "rw_buffer_max" : 0.0,              /* size of rainwater harvesting tank [mm] */
  "frac_ro_stored" : 0.0,             /* fraction of surface runoff stored in tank */
  "rw_irrig_thres" : 0.0,             /* threshold to apply rw_irrigation */
  "soil_infil_rw" : 2.0,              /* values > 2 (default) increase soil infiltration on rainfed and irrigated managed land */
  "yield_gap_bridge" : 0.0,           /* factor by which laimax value is closed (7 - country-value)*factor */
  "nfert_split" : 5,                  /* threshold fertilizer input for split application */
  "residues_in_soil" : 0.1,           /* minimum residues in soil*/
  "fburnt" : 1.0,                     /* fraction of trees burnt at deforestation, refers to remainder after timber harvest */
  "ftimber" : 0.76,                   /* timber fraction at deforestation */
  "harvest_fast_frac" : 0.34,         /* fraction of harvested wood going to fast pools */
  "product_turnover" : { "fast" : 0.1, "slow" : 0.01}, /* product turnover (1/yr) */
  "allocation_threshold" : 35.0,      /* allocation threshold for daily grassland allocation */
  "hfrac2" : 2500.0,
  "firedura" : -8.79,                 /* scaling factor for fire duration for the WVPD*/
  "hum_ign" : -0.53,		      /* a parameter for scaling the human ignintions within SPITFIRE */
  "fire_intens" : 0.0104,             /* threshold of fireintensity for which burnt area can be modeled */
  "rootreduction" : 0.5,              /* fraction used to calculate amount of roots dying at harvest in managed grasslands */
  "phen_limit" : 0.5                  /* phen. limit for agricultural trees */
},
