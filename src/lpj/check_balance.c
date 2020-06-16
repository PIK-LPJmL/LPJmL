/**************************************************************************************/
/**                                                                                \n**/
/**                    c h e c k _ b a l a n c e .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Jens Heinke                                                                \n**/
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
    balance=flux.prec+flux.wd_unsustainable-flux.evap-flux.transp-flux.interc-flux.evap_lake-flux.evap_res
            -flux.discharge-flux.conv_loss_evap-flux.delta_surface_storage-flux.delta_soil_storage-flux.wateruse;

  balance=(flux.area>0) ? balance/flux.area : 0.0;
  if(config->ischeckpoint)
    startyear=max(config->firstyear,config->checkpointyear)+1;
  else
    startyear=config->firstyear+1;
  if(year>startyear && fabs(balance)>1e-3)
  {
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR030: "
#else
    fail(INVALID_WATER_BALANCE_ERR,TRUE,
#endif
         "y: %d GlobW_BALANCE-error: %.5f prec:%.2f wd_unsustainable:%.2f vapour_flux:%.2f discharge:%.2f delta_storage:%.2f\n",
         year,balance*flux.area,flux.prec,flux.wd_unsustainable,
         (flux.evap+flux.transp+flux.interc+flux.evap_lake+flux.evap_res+flux.conv_loss_evap),
         flux.discharge,(flux.delta_surface_storage+flux.delta_soil_storage));
    fflush(stdout);
  }

} /* of 'check_balance' */
