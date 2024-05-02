/**************************************************************************************/
/**                                                                                \n**/
/**                l  i  t  t  e  r  _  a  g  _  g  r  a  s  s  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Function computes sum of all above-ground litter pools for grass           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real litter_ag_grass(const Litter *litter /**< pointer to litter data */
                    )                     /** \return aboveground grass litter (gC/m2) */
{
  int l;
  Real sum;
  sum=0;
  for(l=0;l<litter->n;l++)
    if(litter->ag[l].pft->type==GRASS || litter->ag[l].pft->type==CROP)
      sum+=litter->ag[l].trait.leaf;
  return sum;
} /* of 'litter_ag_grass' */
