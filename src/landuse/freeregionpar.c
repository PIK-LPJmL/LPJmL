/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  e  r  e  g  i  o  n  p  a  r  .  c                      \n**/
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

void freeregionpar(Regionpar regionpar[], /**< array of region params */
                   int nregions           /**< size of array */
                  )
{
  int i;
  for(i=0;i<nregions;i++)
    free(regionpar[i].name);
  free(regionpar);
} /* of 'freeregionpar' */
