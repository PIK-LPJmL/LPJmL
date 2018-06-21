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
                          Bool UNUSED(isdaily)
                         )
{
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  if(pft->stand->type->landusetype==NATURAL)
  {
    grass=pft->data;
    grasspar=pft->par->data;
    grass->turn.leaf+=grass->ind.leaf*grasspar->turnover.leaf/NDAYYEAR;
    litter->ag[pft->litter].trait.leaf+=grass->ind.leaf*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    update_fbd_grass(litter,pft->par->fuelbulkdensity,grass->ind.leaf*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
  }
} /* of 'turnover_daily_grass' */
