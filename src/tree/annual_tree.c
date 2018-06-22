/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  t  r  e  e  .  c                               \n**/
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

Bool annual_tree(Stand *stand,  /**< pointer to stand */
                 Pft *pft,      /**< pointer to PFT */
                 Real *fpc_inc, /**< FPC increment */
                 Bool isdaily   /**< daily temperature data (TRUE/FALSE) */
                )               /** \return TRUE on death */
{
  Real turnover_ind;
  Bool isdead;
  turnover_ind=turnover_tree(&stand->soil.litter,pft);
  isdead=allocation_tree(&stand->soil.litter,pft,fpc_inc);
  if(!isdead)
  {
    isdead=mortality_tree(&stand->soil.litter,pft,turnover_ind,
                          stand->cell->climbuf.temp_max,isdaily);
    if (!pft->prescribe_fpc && !isdead)  /* still not dead? */
      isdead=!survive(pft->par,&stand->cell->climbuf);
  }
  return isdead;
} /* of 'annual_tree' */
