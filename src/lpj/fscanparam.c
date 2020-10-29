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
#define fscanparamreal01(file,var,name) \
  if(fscanreal01(file,var,name,FALSE,verbosity)) return TRUE;
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
  if(param.k_litter10<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'k_litter10'=%g must be greater than zero.\n",
              param.k_litter10);
    return TRUE;
  }
  fscanparampoolpar(&f,&param.k_soil10,"k_soil10");
  if(param.k_soil10.slow<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'k_soil10.slow'=%g must be greater than zero.\n",
              param.k_soil10.slow);
    return TRUE;
  }
  if(param.k_soil10.fast<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'k_soil10.fast'=%g must be greater than zero.\n",
              param.k_soil10.fast);
    return TRUE;
  }
  fscanparamreal(&f,&param.maxsnowpack,"maxsnowpack");
  fscanparamreal(&f,&param.soildepth_evap,"soildepth_evap");
  fscanparamreal(&f,&param.soil_infil,"soil_infil");
  if(param.soil_infil<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'soil_infil'=%g must be greater than zero.\n",
              param.soil_infil);
    return TRUE;
  }
  fscanparamreal(&f,&param.co2_p,"co2_p");
  fscanparamreal(&f,&param.k,"k");
  if(param.k<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'k'=%g must be greater than zero.\n",
              param.k);
    return TRUE;
  }
  fscanparamreal01(&f,&param.theta,"theta");
  fscanparamreal(&f,&param.k_beer,"k_beer");
  fscanparamreal(&f,&param.alphac3,"alphac3");
  fscanparamreal(&f,&param.alphac4,"alphac4");
  fscanparamreal(&f,&param.bc3,"bc3");
  fscanparamreal(&f,&param.bc4,"bc4");
  fscanparamreal01(&f,&param.r_growth,"r_growth");
  fscanparamreal(&f,&param.GM,"GM");
  fscanparamreal(&f,&param.ALPHAM,"ALPHAM");
  fscanparamreal(&f,&param.ko25,"ko25");
  fscanparamreal(&f,&param.kc25,"kc25");
  fscanparamreal01(&f,&param.atmfrac,"atmfrac");
  fscanparamreal01(&f,&param.fastfrac,"fastfrac");
  fscanparamreal(&f,&param.k_mort,"k_mort");
  fscanparamreal01(&f,&param.fpc_tree_max,"fpc_tree_max");
  fscanparamreal(&f,&param.temp_response_a,"temp_response_a");
  fscanparamreal(&f,&param.temp_response_b,"temp_response_b");
  if(config->with_nitrogen)
  {
    fscanparamreal(&f,&param.k_max,"k_max");
    fscanparamreal(&f,&param.k_2,"k_2");
    fscanparamreal(&f,&param.p,"p");
    fscanparamreal(&f,&param.n0,"n0");
    fscanparamreal(&f,&param.k_temp,"k_temp");
    fscanparamreal(&f,&param.min_c_bnf,"min_c_bnf");
    fscanparamreal(&f,&param.par_sink_limit,"par_sink_limit");
    fscanparamreal(&f,&param.q_ash,"q_ash");
    fscanparamreal(&f,&param.sapwood_recovery,"sapwood_recovery");
    fscanparamreal(&f,&param.T_m,"T_m");
    fscanparamreal(&f,&param.T_0,"T_0");
    fscanparamreal(&f,&param.T_r,"T_r");
  }
  else
    param.q_ash=param.sapwood_recovery=0;
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    fscanparamreal(&f,&param.firedura,"firedura");
    fscanparamreal(&f,&param.fire_intens,"fire_intens");
    fscanparamreal(&f,&param.hum_ign,"hum_ign");
  }
  if(config->withlanduse!=NO_LANDUSE)
  {
    fscanparamreal(&f,&param.lsuha,"lsuha");
    fscanparamreal(&f,&param.aprec_lim,"aprec_lim");
    fscanparamreal01(&f,&param.irrig_threshold_c3_dry,"irrig_threshold_c3_dry");
    fscanparamreal01(&f,&param.irrig_threshold_c3_humid,
                   "irrig_threshold_c3_humid");
    fscanparamreal01(&f,&param.irrig_threshold_c4,"irrig_threshold_c4");
    fscanparamreal01(&f,&param.irrig_threshold_rice,"irrig_threshold_rice");
    fscanparamreal01(&f,&param.irrigation_soilfrac,"irrig_soilfrac");
    fscanparamreal01(&f,&param.ec_canal[0],"canal_conveyance_eff_sand");
    fscanparamreal01(&f,&param.ec_canal[1],"canal_conveyance_eff_loam");
    fscanparamreal01(&f,&param.ec_canal[2],"canal_conveyance_eff_clay");
    fscanparamreal01(&f,&param.ec_pipe,"pipe_conveyance_eff");
    param.sat_level[0]=0; /* default value */
    fscanparamreal(&f,&param.sat_level[1],"saturation_level_surf");
    fscanparamreal(&f,&param.sat_level[2],"saturation_level_sprink");
    fscanparamreal(&f,&param.sat_level[3],"saturation_level_drip");
    fscanparamreal01(&f,&param.drip_evap,"drip_evap_reduction");
    fscanparamreal01(&f,&param.residues_in_soil,"residues_in_soil");
    if(config->with_nitrogen)
    {
      fscanparamreal(&f,&param.nfert_split,"nfert_split");
    }
    if(config->istimber)
    {
      fscanparamreal01(&f,&param.fburnt,"fburnt");
      fscanparamreal01(&f,&param.ftimber,"ftimber");
      fscanparamreal01(&f,&param.harvest_fast_frac,"harvest_fast_frac");
      fscanparampoolpar(&f,&param.product_turnover,"product_turnover");
    }
    if(config->rw_manage)
    {
      fscanparamreal01(&f,&param.esoil_reduction,"esoil_reduction");
      fscanparamreal(&f,&param.rw_buffer_max,"rw_buffer_max");
      fscanparamreal01(&f,&param.frac_ro_stored,"frac_ro_stored");
      fscanparamreal(&f,&param.rw_irrig_thres,"rw_irrig_thres");
      fscanparamreal(&f,&param.soil_infil_rw,"soil_infil_rw");
      if(param.soil_infil_rw<param.soil_infil)
      {
        if(isroot(*config))
          fprintf(stderr,"WARNING030: Parameter 'soil_infil_rw'=%g less than 'soil_infil', set to %g.\n",param.soil_infil_rw,param.soil_infil);
        param.soil_infil_rw=param.soil_infil;
      }
      fscanparamreal(&f,&param.yield_gap_bridge,"yield_gap_bridge");
    }
    if(config->with_nitrogen)
    {
      fscanparamreal(&f,&param.allocation_threshold,"allocation_threshold");
    }
    fscanparamreal(&f,&param.hfrac2,"hfrac2");
    fscanparamreal01(&f,&param.rootreduction,"rootreduction");
  }
  param.k_litter10/=NDAYYEAR;
  param.k_soil10.fast/=NDAYYEAR;
  param.k_soil10.slow/=NDAYYEAR;
  return FALSE;
} /* of 'fscanparam' */
