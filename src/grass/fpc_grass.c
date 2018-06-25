/**************************************************************************************/
/**                                                                                \n**/
/**             f  p  c  _  g  r  a  s  s  .  c                                    \n**/
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
#include "grass.h"

Real fpc_grass(Pft *pft)
{
  Real fpc_old;
  fpc_old=pft->fpc;
  const Pft *pft2;
  int p;
  Real lai_sum,fpc_sum;
  lai_sum=fpc_sum=0.0;
  foreachpft(pft2,p,&pft->stand->pftlist)
  {
    if(pft2->par->type==GRASS)
    {
      lai_sum+=lai_grass(pft2)*pft2->nind;
      fpc_sum+=(1.0-exp(-pft->par->lightextcoeff*lai_grass(pft2)*pft2->nind));
    }
  }
  pft->fpc=fpc_sum>0 ? (1.0-exp(-pft->par->lightextcoeff*lai_sum))*(1.0-exp(-pft->par->lightextcoeff*lai_grass(pft)*pft->nind))/fpc_sum : 0;
  return (pft->fpc<fpc_old) ? 0 : pft->fpc-fpc_old;
} /* 'fpc_grass' */
