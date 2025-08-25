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
#ifdef NRECOVERY_COST
                          Real temp,
#else
                          Real UNUSED(temp),
#endif
                          int UNUSED(day),
                          Bool UNUSED(isdaily),
                          const Config *config /**< LPJmL configuration*/
                         )
{
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Output *output;
  #ifdef NRECOVERY_COST
  Real nrecovered=0.0, navailable=0.0;
  Real nplant_demand=0.0, ndemand_leaf=0.0;
  Real npp_for_recovery=0.0;
  #endif

  if(pft->stand->type->landusetype==NATURAL)
  {
    grass=pft->data;
    grasspar=pft->par->data;
    output=&pft->stand->cell->output;
    grass->turn.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
    grass->turn_litt.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    grass->turn_litt.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    litter->item[pft->litter].agtop.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
    getoutput(output,LITFALLC,config)+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->stand->frac;
    litter->item[pft->litter].agtop.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->stand->frac*pft->par->fn_turnover;
#ifdef NRECOVERY_COST
    nplant_demand=ndemand(pft,&ndemand_leaf,pft->vmax,temp)*(1+pft->par->knstore);
    npp_for_recovery = max(0.0,pft->bm_inc.carbon * pft->par->fnpp_nrecovery);
    if((nplant_demand>pft->bm_inc.nitrogen || pft->bm_inc.nitrogen<2) && npp_for_recovery > epsilon)
    {
      navailable=nrecovered=max(0.0,grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*(1-pft->par->fn_turnover));
      if(nrecovered < npp_for_recovery / nrecover_price(grass->ind.leaf.nitrogen, grass->ind.leaf.carbon, pft->par->sla)) 
      {
        pft->bm_inc.nitrogen += nrecovered;
        pft->npp_nrecovery += nrecovered * nrecover_price(grass->ind.leaf.nitrogen, grass->ind.leaf.carbon, pft->par->sla); 
      }
      else
      {
        nrecovered = npp_for_recovery / nrecover_price(grass->ind.leaf.nitrogen, grass->ind.leaf.carbon, pft->par->sla); /* limited by available NPP */
        pft->bm_inc.nitrogen +=  nrecovered;
        pft->stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen += (navailable - nrecovered);
        pft->npp_nrecovery += npp_for_recovery;
      }
    }
    else
    {
      /* if no N demand, N recovery is skipped and the shared available for recovery is returned to litter */
      pft->stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*(1.0-pft->par->fn_turnover);
    }
#else
    pft->bm_inc.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*(1.0-pft->par->fn_turnover);
#endif
    update_fbd_grass(litter,pft->par->fuelbulkdensity,grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
  }
} /* of 'turnover_daily_grass' */
