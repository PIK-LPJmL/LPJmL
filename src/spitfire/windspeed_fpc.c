/**************************************************************************************/
/**                                                                                \n**/
/**                  w i n d s p e e d _ f p c . c                                 \n**/
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

Real windspeed_fpc(Real windspeed,        /**< wind speed (m/s) */
                   const Pftlist *pftlist /**< array of established PFTs */
                  )                       /** \return canopy windspeed (m/min) */
{
  Real windsp_fpc,fpc_sum;
  const Pft *pft;
  int p;
  windsp_fpc=0;
  fpc_sum=0;
  foreachpft(pft,p,pftlist)
  {
    windsp_fpc+=pft->fpc*pft->par->windspeed;
    fpc_sum+=pft->fpc;
  }
  return (fpc_sum>0) ? windsp_fpc*60*windspeed/fpc_sum : windspeed*60; /* convert m/s ->m/min */
} /* of 'windspeed_fpc' */
