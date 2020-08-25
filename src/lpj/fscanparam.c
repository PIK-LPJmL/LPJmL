/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  a  r  a  m  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads LPJmL parameter                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanparamreal(file,var,name) \
  if(fscanreal(file,var,name,FALSE,verbosity)) return TRUE;
#define fscanparamint(file,var,name) \
  if(fscanint(file,var,name,FALSE,verbosity)) return TRUE;
#define fscanparambool(file,var,name) \
  if(fscanbool(file,var,name,FALSE,verbosity)) return TRUE;
#define fscanparampoolpar(file,var,name) \
  if(fscanpoolpar(file,var,name,verbosity)) return TRUE;

Param param; /* global LPJ parameter */

Bool fscanparam(LPJfile *file,       /**< File pointer to text file */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error  */
{
  LPJfile f;
  Verbosity verbosity;
  verbosity=(isroot(*config))  ? config->scan_verbose : NO_ERR;
  if(verbosity>=VERB)
    printf("// LPJ parameters\n");
  if(fscanstruct(file,&f,"param",verbosity))
    return TRUE;
  fscanparamreal(&f,&param.k_litter10,"k_litter10");
  fscanparampoolpar(&f,&param.k_soil10,"k_soil10");
  fscanparamreal(&f,&param.maxsnowpack,"maxsnowpack");
  fscanparamreal(&f,&param.soildepth_evap,"soildepth_evap");
  fscanparamreal(&f,&param.co2_p,"co2_p");
  fscanparamreal(&f,&param.k,"k");
  fscanparamreal(&f,&param.theta,"theta");
  fscanparamreal(&f,&param.k_beer,"k_beer");
  fscanparamreal(&f,&param.alphac3,"alphac3");
  fscanparamreal(&f,&param.alphac4,"alphac4");
  fscanparamreal(&f,&param.bc3,"bc3");
  fscanparamreal(&f,&param.bc4,"bc4");
  fscanparamreal(&f,&param.r_growth,"r_growth");
  fscanparamreal(&f,&param.GM,"GM");
  fscanparamreal(&f,&param.ALPHAM,"ALPHAM");
  fscanparamreal(&f,&param.ko25,"ko25");
  fscanparamreal(&f,&param.kc25,"kc25");
  fscanparamreal(&f,&param.atmfrac,"atmfrac");
  fscanparamreal(&f,&param.fastfrac,"fastfrac");
  fscanparamreal(&f,&param.k_max,"k_max");
  fscanparamreal(&f,&param.k_2,"k_2");
  fscanparamreal(&f,&param.k_mort,"k_mort");
  fscanparamreal(&f,&param.temp_response_a,"temp_response_a");
  fscanparamreal(&f,&param.temp_response_b,"temp_response_b");
  fscanparamreal(&f,&param.p,"p");
  fscanparamreal(&f,&param.n0,"n0");
  fscanparamreal(&f,&param.k_temp,"k_temp");
  fscanparamreal(&f,&param.denit_threshold,"denit_threshold");
  fscanparamreal(&f,&param.min_c_bnf,"min_c_bnf");
  fscanparamreal(&f,&param.par_sink_limit,"par_sink_limit");
  fscanparamreal(&f,&param.q_ash,"q_ash");
  fscanparamreal(&f,&param.sapwood_recovery,"sapwood_recovery");
  fscanparamreal(&f,&param.T_m,"T_m");
  fscanparamreal(&f,&param.T_0,"T_0");
  fscanparamreal(&f,&param.T_r,"T_r");
  fscanparamreal(&f,&param.residue_rate,"residue_rate");
  fscanparamreal(&f,&param.residue_pool,"residue_pool");
  fscanparamreal(&f,&param.residue_cn,"residue_cn");
  fscanparamreal(&f,&param.residue_fbg,"residue_fbg");
  fscanparamreal(&f,&param.manure_cn,"manure_cn");
  fscanparamreal(&f,&param.fertilizer_rate,"fertilizer_rate");
  fscanparamreal(&f,&param.manure_rate,"manure_rate");
  fscanparamreal(&f,&param.residue_frac,"residue_frac");
  fscanparamreal(&f,&param.mixing_efficiency,"mixing_efficiency");
  fscanparamint(&f,&param.till_startyear,"till_startyear");
  if(config->withlanduse!=NO_LANDUSE)
  {
    fscanparamreal(&f,&param.lsuha,"lsuha");
    fscanparamreal(&f,&param.aprec_lim,"aprec_lim");
    fscanparamreal(&f,&param.irrig_threshold_c3_dry,"irrig_threshold_c3_dry");
    fscanparamreal(&f,&param.irrig_threshold_c3_humid,
                   "irrig_threshold_c3_humid");
    fscanparamreal(&f,&param.irrig_threshold_c4,"irrig_threshold_c4");
    fscanparamreal(&f,&param.irrig_threshold_rice,"irrig_threshold_rice");
    fscanparamreal(&f,&param.irrigation_soilfrac,"irrig_soilfrac");
    fscanparamreal(&f,&param.ec_canal[0],"canal_conveyance_eff_sand");
    fscanparamreal(&f,&param.ec_canal[1],"canal_conveyance_eff_loam");
    fscanparamreal(&f,&param.ec_canal[2],"canal_conveyance_eff_clay");
    fscanparamreal(&f,&param.ec_pipe,"pipe_conveyance_eff");
    param.sat_level[0]=0; /* default value */
    fscanparamreal(&f,&param.sat_level[1],"saturation_level_surf");
    fscanparamreal(&f,&param.sat_level[2],"saturation_level_sprink");
    fscanparamreal(&f,&param.sat_level[3],"saturation_level_drip");
    fscanparamreal(&f,&param.drip_evap,"drip_evap_reduction");
    fscanparamreal(&f,&param.residues_in_soil,"residues_in_soil");
    fscanparamreal(&f,&param.nfert_split,"nfert_split");
    fscanparamreal(&f,&param.nfert_split_frac,"nfert_split_frac");
    fscanparamreal(&f,&param.nfert_no3_frac,"nfert_no3_frac");
    fscanparamreal(&f,&param.nmanure_nh4_frac,"nmanure_nh4_frac");
    fscanparamreal(&f,&param.fburnt,"fburnt");
    fscanparamreal(&f,&param.ftimber,"ftimber");
    if(config->rw_manage)
    {
      fscanparamreal(&f,&param.esoil_reduction,"esoil_reduction");
      fscanparamreal(&f,&param.rw_buffer_max,"rw_buffer_max");
      fscanparamreal(&f,&param.frac_ro_stored,"frac_ro_stored");
      fscanparamreal(&f,&param.rw_irrig_thres,"rw_irrig_thres");
      fscanparamreal(&f,&param.soil_infil,"soil_infil");
      fscanparamreal(&f,&param.yield_gap_bridge,"yield_gap_bridge");
    }
    fscanparamreal(&f,&param.allocation_threshold,"allocation_threshold");
    fscanparamreal(&f,&param.rootreduction,"rootreduction");
  }
  param.k_litter10/=NDAYYEAR;
  param.k_soil10.fast/=NDAYYEAR;
  param.k_soil10.slow/=NDAYYEAR;
  return FALSE;
} /* of 'fscanparam' */
