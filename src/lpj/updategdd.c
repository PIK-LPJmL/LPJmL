/**************************************************************************************/
/**                                                                                \n**/
/**                     u  p  d  a  t  e  g  d  d  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function updates growing degree days                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void updategdd(Real gdd[],            /**< Growing degree days array */
               const Pftpar pftpar[], /**< PFT parameter vector of length npft */
               int npft,              /**< Number of natural PFTs */
               Real temp              /**< temperature (deg C) */
              )
{
  int p;
  Real dtemp;
  for(p=0;p<npft;p++)
  {
    dtemp=temp-pftpar[p].gddbase;
    if(dtemp>0)
      gdd[p]+=dtemp;
  }
} /* of 'updategdd' */
