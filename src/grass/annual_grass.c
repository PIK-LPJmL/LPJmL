/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  g  r  a  s  s  .  c                            \n**/
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

Bool annual_grass(Stand *stand, /**< pointer to stand */
                  Pft *pft,     /**< pointer to PFT variables */
                  Real * UNUSED(fpc_inc), /**< FPC increment */
                  Bool UNUSED(isdaily) /**< daily temperature data? */
                 )              /** \return TRUE on death */
{
  Bool isdead=FALSE;
  if(stand->type->landusetype!=GRASSLAND && stand->type->landusetype!=BIOMASS_GRASS)
  {
    turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR);
    isdead=allocation_grass(&stand->soil.litter,pft);
  }
  stand->growing_days=0;
  if(!isdead && !pft->prescribe_fpc)
    isdead=!survive(pft->par,&stand->cell->climbuf);
  return isdead;
} /* of 'annual_grass' */
