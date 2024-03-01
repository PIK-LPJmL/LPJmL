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
#define fscanparamint(file,var,name) \
  if(fscanint(file,var,name,FALSE,verbosity)) return TRUE;
#define fscanparampoolpar(file,var,name) \
  if(fscanpoolpar(file,var,name,verbosity)) return TRUE;

Param param; /* global LPJ parameter */

Bool fscanparam(LPJfile *file,       /**< File pointer to text file */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error  */
{
  LPJfile *f;
  int l;
  Verbosity verbosity;
  verbosity=(isroot(*config))  ? config->scan_verbose : NO_ERR;
  if(verbosity>=VERB)
    printf("// LPJ parameters\n");
  f=fscanstruct(file,"param",verbosity);
  if(f==NULL)
    return TRUE;
  if(fscanerrorlimit(f,&param.error_limit,"error_limits",verbosity))
    return TRUE;
  if(fscanrealarray(f,soildepth,NSOILLAYER,"soildepth",verbosity))
    return TRUE;
  /* calculate layerbound and midlayer from soildepth */
  layerbound[0]=soildepth[0];
  midlayer[0]=soildepth[0]*0.5;
  for(l=1;l<NSOILLAYER;l++)
  {
    layerbound[l]=layerbound[l-1]+soildepth[l];
    midlayer[l]=layerbound[l-1]+soildepth[l]*0.5;
  }
  foreachsoillayer(l)
  {
    if(soildepth[l]<=0)
    {
      if(verbosity)
        fprintf(stderr,"ERROR234: Soil depth of layer %d=%g must be greater than zero.\n",l,soildepth[l]);
      return TRUE;
    }
    logmidlayer[l]=log10(midlayer[l]/midlayer[NSOILLAYER-2]);
  }
  if(fscanrealarray(f,fbd_fac,NFUELCLASS,"fbd_fac",verbosity))
    return TRUE;
  if(config->landfrac_from_file)
  {
    fscanparamreal01(f,&param.minlandfrac,"minlandfrac");
  }
  fscanparamreal(f,&param.k_litter10,"k_litter10");
  if(param.k_litter10<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'k_litter10'=%g must be greater than zero.\n",
              param.k_litter10);
    return TRUE;
  }
  fscanparampoolpar(f,&param.k_soil10,"k_soil10");
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
  if(config->with_nitrogen)
  {
    fscanparampoolpar(f,&param.init_soiln,"init_soiln");
    if(param.init_soiln.slow<=0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR234: Parameter 'init_soiln.slow'=%g must be greater than zero.\n",
                param.init_soiln.slow);
      return TRUE;
    }
    if(param.init_soiln.fast<=0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR234: Parameter 'init_soiln.fast'=%g must be greater than zero.\n",
                param.init_soiln.fast);
      return TRUE;
    }
  }
  fscanparamreal(f,&param.maxsnowpack,"maxsnowpack");
  fscanparamreal(f,&param.soildepth_evap,"soildepth_evap");
  fscanparamreal(f,&param.soil_infil,"soil_infil");
  if(param.soil_infil<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'soil_infil'=%g must be greater than zero.\n",
              param.soil_infil);
    return TRUE;
  }
  fscanparamreal(f,&param.soil_infil_litter,"soil_infil_litter");
  fscanparamreal(f,&param.k,"k");
  if(param.k<=0)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'k'=%g must be greater than zero.\n",
              param.k);
    return TRUE;
  }
  fscanparamreal01(f,&param.theta,"theta");
  fscanparamreal(f,&param.alphac3,"alphac3");
  fscanparamreal(f,&param.alphac4,"alphac4");
  fscanparamreal01(f,&param.r_growth,"r_growth");
  fscanparamreal(f,&param.GM,"GM");
  fscanparamreal(f,&param.ALPHAM,"ALPHAM");
  fscanparamreal(f,&param.ko25,"ko25");
  fscanparamreal(f,&param.kc25,"kc25");
  fscanparamreal01(f,&param.atmfrac,"atmfrac");
  fscanparamreal01(f,&param.fastfrac,"fastfrac");
  fscanparamreal(f,&param.bioturbate,"bioturbate");
  if(param.bioturbate<0 || param.bioturbate>=1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR234: Parameter 'bioturbate'=%g must be in [0,1).\n",
              param.bioturbate);
    return TRUE;
  }
  param.bioturbate= 1-pow(1-param.bioturbate,1./NDAYYEAR);
  fscanparamint(f,&param.veg_equil_year,"veg_equil_year");
  fscanparambool(f,&param.veg_equil_unlim,"veg_equil_unlim");
  fscanparamint(f,&param.nequilsoil,"nequilsoil");
  fscanparamint(f,&param.equisoil_interval,"equisoil_interval");
  fscanparamint(f,&param.equisoil_years,"equisoil_years");
  fscanparamint(f,&param.equisoil_fadeout,"equisoil_fadeout");
  fscanparamreal(f,&param.k_mort,"k_mort");
  fscanparamreal01(f,&param.fpc_tree_max,"fpc_tree_max");
  fscanparamreal(f,&param.temp_response,"temp_response");
  fscanparamreal01(f,&param.percthres,"percthres");
  if(config->with_nitrogen)
  {
    fscanparamreal(f,&param.k_max,"k_max");
    fscanparamreal(f,&param.k_2,"k_2");
    fscanparamreal(f,&param.p,"p");
    fscanparamreal(f,&param.n0,"n0");
    fscanparamreal(f,&param.k_temp,"k_temp");
    fscanparamreal(f,&param.min_c_bnf,"min_c_bnf");
    fscanparamreal(f,&param.par_sink_limit,"par_sink_limit");
    fscanparamreal01(f,&param.q_ash,"q_ash");
    fscanparamreal01(f,&param.sapwood_recovery,"sapwood_recovery");
    fscanparamreal(f,&param.T_m,"T_m");
    fscanparamreal(f,&param.T_0,"T_0");
    fscanparamreal(f,&param.T_r,"T_r");
    if(param.T_r==param.T_0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR238: Parameter T_0=%g must not be identical to parameter T_r=%g.\n",
                param.T_0,param.T_r);
      return TRUE;
    }
    fscanparamreal(f,&param.fertilizer_rate,"fertilizer_rate");
    if(config->residue_treatment)
    {
      fscanparamreal(f,&param.residue_cn,"residue_cn");
      if(param.residue_cn<=0)
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR238: Parameter residue_cn=%g must be greater than zero.\n",
                  param.residue_cn);
        return TRUE;
      }
    }
  }
  else
  {
    param.q_ash=param.sapwood_recovery=param.fertilizer_rate=0;
    param.residue_cn=1;
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    fscanparamreal(f,&param.firedura,"firedura");
    fscanparamreal(f,&param.fire_intens,"fire_intens");
    fscanparamreal(f,&param.hum_ign,"hum_ign");
  }
  if(config->residue_treatment)
  {
    fscanparamreal(f,&param.residue_rate,"residue_rate");
    fscanparamreal(f,&param.residue_pool,"residue_pool");
    fscanparamreal01(f,&param.residue_fbg,"residue_fbg");
  }
  fscanparamreal01(f,&param.residue_frac,"residue_frac");
  fscanparamreal01(f,&param.mixing_efficiency,"mixing_efficiency");
  if(config->withlanduse!=NO_LANDUSE)
  {
    fscanparamreal01(f,&param.tinyfrac,"tinyfrac");
    fscanparamreal(f,&param.lsuha,"lsuha");
    fscanparamreal(f,&param.aprec_lim,"aprec_lim");
    fscanparamreal01(f,&param.irrigation_soilfrac,"irrig_soilfrac");
    fscanparamreal01(f,&param.ec_canal[0],"canal_conveyance_eff_sand");
    fscanparamreal01(f,&param.ec_canal[1],"canal_conveyance_eff_loam");
    fscanparamreal01(f,&param.ec_canal[2],"canal_conveyance_eff_clay");
    fscanparamreal01(f,&param.ec_pipe,"pipe_conveyance_eff");
    param.sat_level[0]=0; /* default value */
    fscanparamreal(f,&param.sat_level[1],"saturation_level_surf");
    fscanparamreal(f,&param.sat_level[2],"saturation_level_sprink");
    fscanparamreal(f,&param.sat_level[3],"saturation_level_drip");
    fscanparamreal01(f,&param.drip_evap,"drip_evap_reduction");
    fscanparamreal01(f,&param.residues_in_soil,"residues_in_soil");
    if(config->with_nitrogen)
    {
      fscanparamreal(f,&param.manure_cn,"manure_cn");
      fscanparamreal(f,&param.manure_rate,"manure_rate");
      fscanparamreal(f,&param.nfert_split,"nfert_split");
      fscanparamreal01(f,&param.nfert_split_frac,"nfert_split_frac");
      fscanparamreal01(f,&param.nfert_no3_frac,"nfert_no3_frac");
      fscanparamreal01(f,&param.nmanure_nh4_frac,"nmanure_nh4_frac");
    }
    fscanparamreal01(f,&param.ftimber_wp,"ftimber_wp");
    if(config->luc_timber)
    {
      fscanparamreal01(f,&param.fburnt,"fburnt");
      fscanparamreal01(f,&param.ftimber,"ftimber");
      fscanparamreal01(f,&param.harvest_fast_frac,"harvest_fast_frac");
      fscanparampoolpar(f,&param.product_turnover,"product_turnover");
    }
    if(config->rw_manage)
    {
      fscanparamreal01(f,&param.esoil_reduction,"esoil_reduction");
      fscanparamreal(f,&param.rw_buffer_max,"rw_buffer_max");
      fscanparamreal01(f,&param.frac_ro_stored,"frac_ro_stored");
      fscanparamreal(f,&param.rw_irrig_thres,"rw_irrig_thres");
      fscanparamreal(f,&param.soil_infil_rw,"soil_infil_rw");
      if(param.soil_infil_rw<param.soil_infil)
      {
        if(isroot(*config))
          fprintf(stderr,"WARNING030: Parameter 'soil_infil_rw'=%g less than 'soil_infil', set to %g.\n",param.soil_infil_rw,param.soil_infil);
        if(config->pedantic)
          return TRUE;
        param.soil_infil_rw=param.soil_infil;
      }
      fscanparamreal(f,&param.yield_gap_bridge,"yield_gap_bridge");
    }
    if(config->with_nitrogen)
    {
      fscanparamreal(f,&param.allocation_threshold,"allocation_threshold");
      fscanparamreal01(f,&param.nfrac_grassharvest,"nfrac_grassharvest");
      fscanparamreal01(f,&param.nfrac_grazing,"nfrac_grazing");
    }
    else
      param.nfrac_grassharvest=param.nfrac_grazing=0;
    fscanparamreal(f,&param.hfrac2,"hfrac2");
    fscanparamreal01(f,&param.hfrac_biomass,"hfrac_biomass");
    fscanparamreal01(f,&param.rootreduction,"rootreduction");
    fscanparamreal01(f,&param.phen_limit,"phen_limit");
    fscanparamint(f,&param.bmgr_harvest_day_nh,"bmgr_harvest_day_nh");
    fscanparamint(f,&param.bmgr_harvest_day_sh,"bmgr_harvest_day_sh");
  }
  param.k_litter10/=NDAYYEAR;
  param.k_soil10.fast/=NDAYYEAR;
  param.k_soil10.slow/=NDAYYEAR;
  return FALSE;
} /* of 'fscanparam' */

Bool fscanparamcft(LPJfile *file,       /**< File pointer to text file */
                   const Config *config /**< LPJ configuration */
                  )                     /** \return TRUE on error  */
{
  LPJfile *f;
  const char *name;
  Verbosity verbosity;
  verbosity=(isroot(*config))  ? config->scan_verbose : NO_ERR;
  if(verbosity>=VERB)
    printf("// LPJ parameters\n");
  f=fscanstruct(file,"param",verbosity);
  if(f==NULL)
    return TRUE;
  if(config->withlanduse!=NO_LANDUSE && config->with_nitrogen)
  {
    name=fscanstring(f,NULL,"cft_fertday_temp",verbosity);
    if(name==NULL)
      return TRUE;
    param.cft_fertday_temp=findpftname(name,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
    if(param.cft_fertday_temp==NOT_FOUND)
    {
      if(verbosity)
      {
        fprintf(stderr,"ERROR230: Invalid CFT '%s' for 'cft_fertday_temp', must be ",name);
        fprintpftnames(stderr,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
        fputs(".\n",stderr);
      }
      return TRUE;
    }
    name=fscanstring(f,NULL,"cft_fertday_tropic",verbosity);
    if(name==NULL)
      return TRUE;
    param.cft_fertday_tropic=findpftname(name,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
    if(param.cft_fertday_tropic==NOT_FOUND)
    {
      if(verbosity)
      {
        fprintf(stderr,"ERROR230: Invalid CFT '%s' for 'cft_fertday_tropic', must be ",name);
        fprintpftnames(stderr,config->pftpar+config->npft[GRASS]+config->npft[TREE],config->npft[CROP]);
        fputs(".\n",stderr);
      }
      return TRUE;
    }
  }
  return FALSE;
} /* of 'fscanparamcft' */
