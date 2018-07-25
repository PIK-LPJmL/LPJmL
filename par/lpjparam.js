/**************************************************************************************/
/**                                                                                \n**/
/**              l  p  j  p  a  r  a  m  .  j  s                                   \n**/
/**                                                                                \n**/
/**     LPJ parameter file for LPJmL version 5.1.001                               \n**/
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
  "co2_p" : 278.0,           /* pre-industrial CO2 (ppmv) */
  "k" : 0.0548,              /* k    k = 7.4e-7 * atomic_mass_C / atomic_mass_N * seconds_per_day = 0.0548 Sprugel et al. 1996, Eqn 7*/
  "theta" : 0.9,             /* theta */
  "k_beer" : 0.5,            /* k_beer */
  "alphac3" : 0.08,          /* alphac3 */
  "alphac4" : 0.053,         /* alphac4 */
  "bc3" : 0.015,             /* bc3 leaf respiration as fraction of Vmax for C3 plants */
  "bc4" : 0.035,             /* bc4 leaf respiration as fraction of Vmax for C4 plants */
  "r_growth" : 0.25,         /* r_growth */
  "GM" : 2.41,               /* GM empirical parameter in demand function */
  "ALPHAM" : 1.485,          /* ALPHAM Priestley-Taylor coefficient*/ 
  "ko25" : 3.0e4,            /* Michaelis constant for O2 (Pa) at 25 deg C */
  "kc25" : 30.,              /* Michaelis constant for CO2 (Pa) at 25 deg C */
  "atmfrac" : 0.6,           /* atmfrac */
  "fastfrac" : 0.98,         /* fastfrac */
  "k_max": 0.10,             /* k_max, maximum fraction of soil->NH4 assumed to be nitrified Parton, 2001*/
  "k_2": 0.02,               /* k_2, fraction of nitrified N lost as N20 flux Parton, 2001*/
  "p" : 25,                  /* Haxeltine & Prentice regression coefficient */
  "n0" : 7.15,               /* Haxeltine & Prentice regression coefficient */
  "k_temp" : 0.02,           /* factor of temperature dependence of nitrogen demand for Rubisco activity */
  "denit_threshold" : 0.8,   /* denitrification threshold */
  "min_c_bnf" : 20,          /* threshold C root content for BNF */
  "par_sink_limit" : 0.2,    /* Michaelis-Menten scaler of sink limitation */
  "q_ash" : 0.45,            /* fraction of nitrogen going to litter after fire */
  "sapwood_recovery" : 0.3,  /* recovery of sapwood nitrogen */
  "k_mort" : 0.4,            /* coefficient of growth efficiency in mortality equation (k_mort2) */
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
  "nfert_split" : 5,                  /* threshold fertilizer input for split application */
  "residues_in_soil" : 0.1,           /* minimum residues in soil*/
  "fburnt" : 0,                       /* fraction of trees burnt at deforestation */
  "ftimber" : 1                       /* timber fraction at deforestation */
},
