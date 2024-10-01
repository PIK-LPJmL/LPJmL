/**************************************************************************************/
/**                                                                                \n**/
/**              a  l  p  h  a  a  _  t  r  e  e  .  c                             \n**/
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

Real alphaa_tree(const Pft *pft,     /**< pointer to tree PFT */
                 int UNUSED(lai_opt) /**< LAImax option */
                )                    /** \return alpha_a (0..1) */
{
  Pfttree *tree;
  Real scaler=1.0;
  tree=pft->data;
  /* sink-limitation: downscale alphaa if there is too much carbon that cannot be allocated under
     given N limitation*/
  if((tree->ind.leaf.carbon+tree->excess_carbon*tree->falloc.leaf)>10)
  {
    scaler=tree->ind.leaf.nitrogen/(tree->ind.leaf.carbon+tree->excess_carbon*tree->falloc.leaf) /
           pft->par->ncleaf.low;
    if(scaler>1)
      scaler=1.0;
    else
      scaler=(1+param.par_sink_limit)*scaler/(scaler+param.par_sink_limit);
  }
  return pft->par->alphaa*scaler;
} /* of 'alphaa_tree' */
