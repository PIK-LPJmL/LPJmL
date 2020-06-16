/**************************************************************************************/
/**                                                                                \n**/
/**               a  d  j  u  s  t  _  g  r  a  s  s  .  c                         \n**/
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

void adjust_grass(Litter * UNUSED(litter),Pft *pft,Real grass_fpc, Real fpc_max)
{
  Real fpc_end;
  if(grass_fpc>fpc_max)
  {
    fpc_grass(pft);
    fpc_end=pft->fpc-(grass_fpc-fpc_max)*pft->fpc/grass_fpc;
    pft->fpc = fpc_end;
    //printf("PFT:%s fpc_obs=%g fpc=%g\n",pft->par->name, pft->fpc_obs, pft->fpc);
  }
} /* of 'adjust_grass' */

/**************************************************************************************/
/**                                                                                \n**/
/** CALLED FROM:                                                                   \n**/
/**   establishment.c (foreachpft)                                                 \n**/
/** PURPOSE:                                                                       \n**/
/**   reduce nind and fpc if total fpc for all trees exceeds                       \n**/
/**   0.95 (FPC_TREE_MAX)                                                          \n**/
/** DETAILED DESCRIPTION:                                                          \n**/
/**   litter_update_tree is used to transfer the "waste" carbon into the           \n**/
/**   litter pool in order to the maintain carbon balance                          \n**/
/**                                                                                \n**/
/**************************************************************************************/
