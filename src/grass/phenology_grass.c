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
                     Bool UNUSED(isdaily), /**< daily temperature data? */
                     const Config *config /**< LPJmL configuration */
                    )
{
  Real dtemp;
  const Pftgrasspar *grasspar;
  Pftgrass *grass;
  Output *output;
  grasspar=pft->par->data;
  grass=pft->data;
  output=&pft->stand->cell->output;
  dtemp=temp - getpftpar(pft,gddbase);
  if(dtemp>0.0)
    pft->gdd+=dtemp;
  pft->phen=pft->gdd*pft->par->ramp;
  if(pft->phen>1)
    pft->phen=1;
  if ((pft->stand->cell->coord.lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) ||
      (pft->stand->cell->coord.lat<0.0 && day==COLDEST_DAY_SHEMISPHERE)) 
    pft->aphen=pft->gdd=0.0;
  if (isnatural(pft->stand))
  {
    grass->turn.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn_litt.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    grass->turn_litt.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    pft->stand->soil.litter.item[pft->litter].agtop.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    getoutput(output,LITFALLC,config)+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->stand->frac;
    pft->stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover*pft->stand->frac;
    pft->bm_inc.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*(1-pft->par->fn_turnover);
    update_fbd_grass(&pft->stand->soil.litter,pft->par->fuelbulkdensity,grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
  }
  pft->aphen+=pft->phen;
} /* of 'phenology_grass' */
