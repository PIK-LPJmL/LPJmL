/**************************************************************************************/
/**                                                                                \n**/
/**             i  n  i  t  _  t  r  e  e  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes annual variables                                      \n**/
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

void init_tree(Pft *pft /**< pointer to tree PFT */
              )
{
  Pfttree *tree;
  tree=pft->data;
  pft->bm_inc=pft->wscal_mean=0;
#ifdef DAILY_ESTABLISHMENT
  pft->established=FALSE;
#endif
  tree->gddtw=tree->aphen_raingreen=0;
} /* of 'init_tree' */
