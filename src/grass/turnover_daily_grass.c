/**************************************************************************************/
/**                                                                                \n**/
/**        t u r n o v e r _ d a i l y _ g r a s s . c                             \n**/
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

void turnover_daily_grass(Litter *litter,
                          Pft *pft,
                          Real UNUSED(temp),
                          int UNUSED(day),
                          Bool UNUSED(isdaily),
                          const Config *config /**< LPJmL configuration*/
                         )
{
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Output *output;
  if(pft->stand->type->landusetype==NATURAL  || pft->stand->type->landusetype==WETLAND)
  {
    grass=pft->data;
    grasspar=pft->par->data;
    output=&pft->stand->cell->output;
    grass->turn.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn_litt.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    grass->turn_litt.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    litter->item[pft->litter].ag.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    getoutput(output,LITFALLC,config)+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->stand->frac;
    litter->item[pft->litter].ag.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->stand->frac*pft->par->fn_turnover;
    update_fbd_grass(litter,pft->par->fuelbulkdensity,grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
  }
} /* of 'turnover_daily_grass' */
