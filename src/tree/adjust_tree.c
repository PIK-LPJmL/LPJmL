/**************************************************************************************/
/**                                                                                \n**/
/**               a  d  j  u  s  t  _  t  r  e  e  .  c                            \n**/
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
#include "tree.h"

void adjust_tree(Litter *litter, /**< pointer to litter */
                 Pft *pft,       /** pointer to tree PFT */
                 Real tree_fpc,  /**< tree foliage projective cover */
                 Real fpc_max    /**< maximum foliage projectove cover */
                )
{

  Real frac,fpc_end,nind_old;
  int i;

  if(tree_fpc>fpc_max)
  {
    fpc_tree(pft);
    fpc_end=pft->fpc-(tree_fpc-fpc_max)*pft->fpc/tree_fpc;
    nind_old=pft->nind;
    for(i=0; fpc_end<pft->fpc && i<40;i++)
    {
      frac=fpc_end/pft->fpc;
      pft->nind*=frac;
      fpc_tree(pft);
    }
    litter_update_tree(litter,pft,nind_old-pft->nind);
/*printf("PFT:%s fpc_obs=%g fpc=%g\n",pft->par->name, pft->fpc_obs, pft->fpc);*/

  }
} /* of 'adjust_tree' */

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
