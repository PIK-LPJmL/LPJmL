/**************************************************************************************/
/**                                                                                \n**/
/**        t u r n o v e r _ m o n t h l y _ g r a s s . c                         \n**/
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

void turnover_monthly_grass(Litter *litter,Pft *pft)
{
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  if(pft->stand->type->landusetype==NATURAL)
  {
    grasspar=pft->par->data;
    grass=pft->data;
    grass->turn.root+=grass->ind.root*grasspar->turnover.root/NMONTH;
    litter->bg[pft->litter]+=grass->ind.root*grasspar->turnover.root/NMONTH*pft->nind;
  }
} /* of 'turnover_monthly_grass' */
