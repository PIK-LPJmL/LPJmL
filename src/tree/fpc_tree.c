/**************************************************************************************/
/**                                                                                \n**/
/**                      f  p  c   _  t  r  e  e  .  c                             \n**/
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

Real fpc_tree(Pft *pft /**< pointer to PFT */
             )         /** \return foliar projective cover (FPC) */
{
  Pfttree *tree;
  Real fpc_old;
  fpc_old=pft->fpc;
  tree=pft->data;
  
   pft->fpc=(tree->crownarea>0.0) ? tree->crownarea*pft->nind*
        (1.0-exp(- getpftpar(pft, lightextcoeff) * lai_tree(pft))) : 0;
  return (pft->fpc<fpc_old) ? 0 : pft->fpc-fpc_old;
} /* of 'fpc_tree' */
