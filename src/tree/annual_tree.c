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

Bool annual_tree(Stand *stand,       /**< pointer to stand */
                 Pft *pft,           /**< pointer to PFT */
                 Real *fpc_inc,      /**< FPC increment */
                 Bool isdaily,       /**< daily temperature data (TRUE/FALSE) */
                 const Config *config /**< LPJmL configuration */
                )                    /** \return TRUE on death */
{
  Stocks turnover_ind;
  Bool isdead;
  Real stress;
  turnover_ind=turnover_tree(&stand->soil.litter,pft,config);
  isdead=allocation_tree(&stand->soil.litter,pft,fpc_inc,config);
  if(pft->inun_count>pft->par->inun_dur)
  {
    stress=pft->inun_count/pft->par->inun_dur;
    if(stress>2)
      isdead=TRUE;
  }
  if(!isdead)
  {
    isdead=mortality_tree(&stand->soil.litter,pft,turnover_ind.carbon,
                          stand->cell->climbuf.temp_max,isdaily,config);
  if (!(pft->stand->prescribe_landcover==LANDCOVERFPC && pft->stand->type->landusetype==NATURAL) &&
      !isdead)  /* still not dead? */
      isdead=!survive(pft->par,&stand->cell->climbuf);
  }
  return isdead;
} /* of 'annual_tree' */
