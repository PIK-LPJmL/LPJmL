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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanparamreal(file,var,name) \
  if(fscanreal(file,var,name,verbosity)) return TRUE; 
#define fscanparamint(file,var,name) \
  if(fscanint(file,var,name,verbosity)) return TRUE;

Param param; /* global LPJ parameter */

Bool fscanparam(FILE  *file,         /**< File pointer to text file */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error  */
{
  Verbosity verbosity;
  verbosity=(isroot(*config))  ? config->scan_verbose : NO_ERR;
  if(verbosity>=VERB)
    printf("// LPJ parameters\n");
  fscanparamreal(file,&param.k_litter10,"k_litter10");
  fscanparamreal(file,&param.k_soil10.fast,"fast k_soil10");
  fscanparamreal(file,&param.k_soil10.slow,"slow k_soil10");
  fscanparamreal(file,&param.maxsnowpack,"max snow pack");
  fscanparamreal(file,&param.soildepth_evap,"soildepth_evap");
  fscanparamreal(file,&param.co2_p,"co2_p");
  fscanparamreal(file,&param.k,"k");
  fscanparamreal(file,&param.theta,"theta");
  fscanparamreal(file,&param.k_beer,"k_beer");
  fscanparamreal(file,&param.alphac3,"alphac3");
  fscanparamreal(file,&param.alphac4,"alphac4");
  fscanparamreal(file,&param.bc3,"bc3");
  fscanparamreal(file,&param.bc4,"bc4");
  fscanparamreal(file,&param.r_growth,"r_growth");
  fscanparamreal(file,&param.GM,"GM");
  fscanparamreal(file,&param.ALPHAM,"ALPHAM");
  fscanparamreal(file,&param.ko25,"ko25");
  fscanparamreal(file,&param.kc25,"kc25");
  fscanparamreal(file,&param.atmfrac,"atmfrac");
  fscanparamreal(file,&param.fastfrac,"fastfrac");
  fscanparamreal(file,&param.k_mort,"k_mort");
  
  if(config->withlanduse!=NO_LANDUSE)
  {
    fscanparamreal(file,&param.aprec_lim,"aprec_lim");
    fscanparamreal(file,&param.irrig_threshold_c3_dry,"irrig_threshold_c3_dry");
    fscanparamreal(file,&param.irrig_threshold_c3_humid,
                   "irrig_threshold_c3_humid");
    fscanparamreal(file,&param.irrig_threshold_c4,"irrig_threshold_c4");
    fscanparamreal(file,&param.irrig_threshold_rice,"irrig_threshold_rice");
    fscanparamreal(file,&param.irrigation_soilfrac,"irrigation_soilfrac");
    fscanparamreal(file,&param.ec_canal[0],"canal_conveyance_eff_sand");
    fscanparamreal(file,&param.ec_canal[1],"canal_conveyance_eff_loam");
    fscanparamreal(file,&param.ec_canal[2],"canal_conveyance_eff_clay");
    fscanparamreal(file,&param.ec_pipe,"pipe_conveyance_eff");
    param.sat_level[0]=0; /* default value */
    fscanparamreal(file,&param.sat_level[1],"saturation_level_surf");
    fscanparamreal(file,&param.sat_level[2],"saturation_level_sprink");
    fscanparamreal(file,&param.sat_level[3],"saturation_level_drip");
    fscanparamreal(file,&param.drip_evap,"drip_evap_reduction");
    fscanparamreal(file,&param.laimax,"laimax");
    fscanparamint(file,&param.intercrop,"intercrop");
    fscanparamint(file,&param.remove_residuals,"remove_residuals");
    fscanparamint(file,&param.sdate_fixyear,"sdate_fixyear");
    fscanparamint(file,&param.landuse_year_const,"landuse_year_const");
    if(config->rw_manage)
    {
      fscanparamreal(file,&param.esoil_reduction,
                  "esoil_reduction");
      fscanparamreal(file,&param.rw_buffer_max,
                   "rw_buffer_max");
      fscanparamreal(file,&param.frac_ro_stored,
                   "frac_ro_stored");
      fscanparamreal(file,&param.rw_irrig_thres,
                    "rw_irrig_thres");
      fscanparamreal(file,&param.soil_infil,
                    "soil_infil");
      fscanparamreal(file,&param.yield_gap_bridge,
                    "yield_gap_bridge");
      }
  }
  param.k_litter10/=NDAYYEAR;
  param.k_soil10.fast/=NDAYYEAR;
  param.k_soil10.slow/=NDAYYEAR;
  return FALSE;
} /* of 'fscanparam' */
