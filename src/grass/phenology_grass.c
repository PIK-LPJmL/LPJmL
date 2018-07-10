/**************************************************************************************/
/**                                                                                \n**/
/**             p  h  e  n  o  l  o  g  y  _  g  r  a  s  s  .  c                  \n**/
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

void phenology_grass(Pft *pft,  /**< pointer to PFT variables */
                     Real temp, /**< temperature (deg C) */
                     int day,   /**< day of year (1..365) */
                     Bool UNUSED(isdaily) /**< daily temperature data? */
                    )
{
  Real dtemp;
#ifndef YEARLY_TURNOVER
  const Pftgrasspar *grasspar;
  Pftgrass *grass;
  grasspar=pft->par->data;
  grass=pft->data;
#endif

  dtemp=temp - getpftpar(pft,gddbase);
  if(dtemp>0.0)
    pft->gdd+=dtemp;
  pft->phen=pft->gdd*pft->par->ramp;
  if(pft->phen>1)
    pft->phen=1;
  if ((pft->stand->cell->coord.lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) ||
      (pft->stand->cell->coord.lat<0.0 && day==COLDEST_DAY_SHEMISPHERE)) 
    pft->aphen=pft->gdd=0.0;
#ifndef YEARLY_TURNOVER
  if (pft->stand->type->landusetype==NATURAL)
  {
    grass->turn.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
    pft->stand->soil.litter.ag[pft->litter].trait.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    pft->stand->soil.litter.ag[pft->litter].trait.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
    update_fbd_grass(&pft->stand->soil.litter,pft->par->fuelbulkdensity,grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
  }
#endif
  pft->aphen+=pft->phen;
} /* of 'phenology_grass' */
