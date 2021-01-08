/**************************************************************************************/
/**                                                                                \n**/
/**                  p  a  r  a  m  .  h                                           \n**/
/**                                                                                \n**/
/**     Header file for LPJmL parameters                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef PARAM_H
#define PARAM_H

/* Definition of datatypes */

typedef struct
{
  Real k_litter10;
  Poolpar k_soil10;
  Real maxsnowpack; /**< max. snow pack (mm) */
  Real soildepth_evap; /**< depth of sublayer at top of upper soil layer (mm) */
  Real soil_infil;      /**< soil infiltration */
  Real co2_p;       /**< pre-industrial CO2 concentration (ppm) */
  Real k;
  Real theta;      /**< co-limitation (shape) parameter */
  Real k_beer;     /**< Extinction coefficient */
  Real alphac3;    /**< intrinsic quantum efficiency of CO2 uptake in C4 plants */
  Real alphac4;    /**< intrinsic quantum efficiency of CO2 uptake in C3 plants */
  Real bc3;        /**< leaf respiration as fraction of Vmax for C3 plants */
  Real bc4;        /**< leaf respiration as fraction of vmax for C4 plants */
  Real r_growth;     /**< growth respiration */
  Real GM;           /**< empirical parameter in demand function */
  Real ALPHAM;       /**< Priestley-Taylor coefficient (Demand,maximum-value) */
  Real ko25;         /**< Michaelis constant for O2 (Pa) at 25 deg C */
  Real kc25;          /**< Michaelis constant for CO2 (Pa) at 25 deg C */
  Real atmfrac;      /**< fraction of decomposed litter emitted as CO2 to the atmosphere */
  Real fastfrac;     /**< fraction of soil-bound decomposed litter entering the intermediate soil carbon pool */
  int veg_equil_year;
  Real k_max;        /**< maximum fraction of soil->NH4 assumed to be nitrified Parton, 2001*/
  Real k_2;          /**< fraction of nitrified N lost as N20 flux Parton, 2001*/

  Real k_mort;       /**< coefficient of growth efficiency in mortality equation (k_mort2) */
  Real fpc_tree_max; /**< maximum foliage projective cover for trees */
  Real temp_response_a; /**< parameter in temperature response fucntion */
  Real temp_response_b; /**< parameter in temperature response fucntion */
  Real p;            /**< regression coefficent from Haxeltine and Prentice : N=p Vmax+n0 */
  Real n0;           /**< regression coefficient from Haxltine and Prentice */
  Real q_ash;        /**< fraction of nitrogen going to litter after fire */
  Real sapwood_recovery; /**< recovery of sapwood nitrogen */
  Real T_m;          /**< parameter in N uptake temperature function */
  Real T_0;
  Real T_r;
  Real lsuha;       /**< livestock density for grassland management (lsuha) */
  /* IRRIGATION */
  Real aprec_lim;    /**< annual precipitation limit for C3 irrigation threshold */
  Real irrig_threshold_c3_dry; /**< soil moisture irrigation threshold for C3 crops, annual precip < aprec_lim */
  Real irrig_threshold_c3_humid; /**< soil moisture irrigation threshold for C3 crops, annual precip >= aprec_lim */
  Real irrig_threshold_c4; /**< soil moisture irrigation threshold for C4 crops*/
  Real irrig_threshold_rice; /**< soil moisture irrigation threshold for RICE */
  Real irrigation_soilfrac; /**< fraction of soil filled with water during irrigation event */
  Real ec_canal[3];     /**< conveyance efficiency for open canal irrigation systems for soil types sand, loam, clay */
  Real ec_pipe;      /**< conveyance efficiency for pressurized irrigation systems */
  Real sat_level[4];   /**< saturation level for the three irrigation systems (surf,sprink,drip) */
  Real drip_evap;        /**< reduction of drip soil evap */
  Real firedura;        /**< scaling factor for fire duration */
  Real fire_intens;     /**< threshold of fireintensity for which burnt area can be modeled */
  Real hum_ign;         /**< a parameter for scaling the human ignintions within SPITFIRE */
  Real residues_in_soil;  /**< minimum residues in soil*/
  Real nfert_split;     /**< threshold fertilizer input for split application */
  Real manure_cn;       /* CN ratio of applied manure gC/gN */
  Real nfert_split_frac;       /* fraction of fertilizer to be applied at sowing */
  Real nfert_no3_frac;  /* fraction of NO3 in fertilizer */
  Real nmanure_nh4_frac;  /* fraction of NH4 in manure */
  Real residue_frac; /**< fraction of residues to be submerged with tillage */
  Real mixing_efficiency; /**< density factor to simulate changes in bulk density by tillage */
  Real residue_rate;        /**< fixed residue rate in gC/m2/yr */
  Real residue_pool;        /**< fixed residue pool in gC/m2 */
  Real residue_cn;          /**< CN ratio of prescribed residues */
  Real residue_fbg;         /**< belowground fraction of prescribed residues */
  Real fertilizer_rate;     /**< fixed fertilizer application rate in gN/m2/yr */
  Real manure_rate;         /**< fixed manure application rate in gN/m2/yr */
  Real min_c_bnf;
  Real par_sink_limit;  /**< Michaelis-Menten scaling of sink limitation */
  Real k_temp;          /**< factor of temperature dependence of nitrogen demand for Rubisco activity */
  Real fburnt;          /**< fraction of trees burnt at deforestation */
  Real ftimber;         /**< timber fraction at deforestation */
  Real harvest_fast_frac;  /**< fraction of harvested wood going to fast pools */
  Real esoil_reduction; /**< reduction of soil evaporation */
  Poolpar product_turnover; /**< fast and slow product turnover (1/yr) */
  Real rw_buffer_max;   /**< size of rainwater harvesting tank */
  Real frac_ro_stored;  /**< fraction of surface runoff stored in tank */
  Real rw_irrig_thres;  /**< threshold to apply rw_irrigation */
  Real soil_infil_rw;      /**< soil infiltration improvement */
  Real yield_gap_bridge; /**< yield-gap bridge: factor by which laimax value is closed (7 - country-value)*factor */
  Real allocation_threshold; /**< allocation threshold for daily grassland allocation */
  Real hfrac2;
  Real rootreduction;     /**< fraction used to calculate amount of roots dying at harvest in managed grasslands */
} Param;

/* Declaration of global variable */

extern Param param;

/* Declaration of functions */

extern Bool fscanparam(LPJfile *,const Config *);

#endif
