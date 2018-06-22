/**************************************************************************************/
/**                                                                                \n**/
/**                f  i  n  d  l  i  t  t  e  r  .  c                              \n**/
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

int findlitter(const Litter *litter, /**< pointer to litter */
               const Pftpar *pft     /**< PFT parameter array */
               )                     /** \return PFT specific litter pool */
{
  int l;
  for(l=0;l<litter->n;l++)
    if(litter->ag[l].pft==pft)
      return l;
  return NOT_FOUND;
} /* of 'findlitter' */
