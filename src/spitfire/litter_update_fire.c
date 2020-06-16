/**************************************************************************************/
/**                                                                                \n**/
/**               l i t t e r _ u p d a t e _ f i r e . c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real litter_update_fire(Litter *litter,Tracegas *emission,const Fuel *fuel)
{
  int i,j;
  Real fuel_consum,fuel_consum_pft;;
  /* litter update [gC/m2]*/
  fuel_consum=0;
  for(i=0;i<litter->n;i++)
  {
    fuel_consum_pft=litter->ag[i].trait.leaf*fuel->deadfuel_consum[0];
    litter->ag[i].trait.leaf*=(1-fuel->deadfuel_consum[0]);
    for(j=0;j<NFUELCLASS;j++)
    {
      fuel_consum_pft+=litter->ag[i].trait.wood[j]*fuel->deadfuel_consum[j];
      litter->ag[i].trait.wood[j]*=(1-fuel->deadfuel_consum[j]);
    }
#ifdef WITH_FIRE_MOISTURE
    emission->co2+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.co2 * (fuel->CME/0.94);
    emission->co+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.co * (2- fuel->CME/0.94);
#else
    emission->co2+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.co2;
    emission->co+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.co;
#endif
    emission->ch4+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.ch4;
    emission->voc+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.voc;
    emission->tpm+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.tpm;
    emission->nox+=c2biomass(fuel_consum_pft)*litter->ag[i].pft->emissionfactor.nox;
    
   
    
    fuel_consum+=fuel_consum_pft;
  }
  return fuel_consum;
} /* of litter_update_fire' */
