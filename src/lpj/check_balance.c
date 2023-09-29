/**************************************************************************************/
/**                                                                                \n**/
/**                    c h e c k _ b a l a n c e .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks global water balance                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void check_balance(Flux flux,           /**< global carbon and water fluxes */
                   int year,            /**< simulation year (AD) */
                   const Config *config /**< LPJmL configuration */
                  )
{

  Real balance=0;
  int startyear;

  if(config->river_routing)
    balance=flux.ext+flux.prec+flux.wd_unsustainable-flux.evap-flux.transp-flux.interc-flux.evap_lake-flux.evap_res
            -flux.discharge-flux.conv_loss_evap-flux.delta_surface_storage-flux.delta_soil_storage-flux.wateruse-flux.excess_water;

  balance=(flux.area>0) ? balance/flux.area : 0.0;
  if(config->ischeckpoint)
    startyear=max(config->firstyear,config->checkpointyear)+1;
  else
    startyear=config->firstyear+1;
  if(year>startyear && fabs(balance)>1e-3)
  {
    fail(GLOBAL_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,
         "Global water balance not closed in year %d: diff=%.5g(%.5g), prec=%.5g, wd_unsustainable=%.5g, vapour_flux=%.5g, discharge=%.5g, delta_storage=%.5g, excess_water=%.5g"
         ,year,balance*flux.area,balance,flux.prec,flux.wd_unsustainable,
         (flux.evap+flux.transp+flux.interc+flux.evap_lake+flux.evap_res+flux.conv_loss_evap),
         flux.discharge,(flux.delta_surface_storage+flux.delta_soil_storage),flux.excess_water);
  }

} /* of 'check_balance' */
