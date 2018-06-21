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

Real windspeed_fpc(Real windspeed,const Pftlist *pftlist)
{
  Real windsp_fpc;
  const Pft *pft;
  int p;
  windsp_fpc=0;
  foreachpft(pft,p,pftlist)
    windsp_fpc+=pft->fpc*pft->par->windspeed;
  return windsp_fpc*60*windspeed;
} /* of 'windspeed_fpc' */
