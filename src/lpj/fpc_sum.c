/**************************************************************************************/
/**                                                                                \n**/
/**                       f  p  c  _  s  u  m  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates sum of foliar projective covers (FPC) of               \n**/
/**     established PFTs                                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real fpc_sum(Real fpc_type[], /**< on return, FPC sum of each PFT class */
             int ntype,       /**< number of different PFT classes */
             const Pftlist *pftlist /**< List of established PFTs */
            ) /** \return total FPC of established PFTs */
{
  const Pft *pft;
  int p,i;
  Real fpc;
  for(i=0;i<ntype;i++)
    fpc_type[i]=0; 
  fpc=0;
  foreachpft(pft,p,pftlist)
  {
    fpc+=pft->fpc;   
    fpc_type[pft->par->type]+=pft->fpc; 
  }
  return fpc;
} /* of 'fpc_sum' */
