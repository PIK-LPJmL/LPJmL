/**************************************************************************************/
/**                                                                                \n**/
/**        t u r n o v e r _ d a i l y _ t r e e . c                               \n**/
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

static void f_turnover_tree(Pft *pft,       /**< pointer to tree PFT */
                            int day,        /**< day (1..365) */
                            const Config *config, /**< LPJmL configuration */
                            Real factor
                          )
{
  Pfttree *tree;
  Pfttreepar *treepar;
#ifdef NRECOVERY_COST
  Real nrecovered=0.0, navailable=0.0, nplant_demand=0.0, ndemand_leaf=0.0;
  Real npp_for_recovery=0.0;
#endif
  tree=pft->data;
  treepar=pft->par->data;

  tree->turn.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*factor;
  tree->turn.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor;
  tree->turn_litt.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*factor*pft->nind;
  tree->turn_litt.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor*pft->nind;
  pft->stand->soil.litter.item[pft->litter].agtop.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*factor*pft->nind;
  getoutput(&pft->stand->cell->output,LITFALLC,config)+=tree->ind.leaf.carbon*treepar->turnover.leaf*factor*pft->nind*pft->stand->frac;
  pft->stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor*pft->nind*pft->par->fn_turnover;
  getoutput(&pft->stand->cell->output,LITFALLN,config)+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor*pft->nind*pft->stand->frac*pft->par->fn_turnover;

#ifdef NRECOVERY_COST
  nplant_demand=ndemand(pft,&ndemand_leaf,pft->vmax,temp)*(1+pft->par->knstore);
  npp_for_recovery=max(0.0,pft->bm_inc.carbon*pft->par->nrecovery_npp);
  if((nplant_demand>pft->bm_inc.nitrogen || pft->bm_inc.nitrogen<2) && npp_for_recovery>epsilon){
    navailable=nrecovered=max(0.0,tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor*pft->nind*(1-pft->par->fn_turnover));
    if(nrecovered<npp_for_recovery/nrecover_price(tree->ind.leaf.nitrogen,tree->ind.leaf.carbon,pft->par->sla)) 
    {
      pft->bm_inc.nitrogen+=nrecovered;
      pft->npp_nrecovery+=nrecovered*nrecover_price(tree->ind.leaf.nitrogen,tree->ind.leaf.carbon,pft->par->sla);
    }
    else
    {
      nrecovered=npp_for_recovery/nrecover_price(tree->ind.leaf.nitrogen,tree->ind.leaf.carbon,pft->par->sla); /* limited by available NPP */
      pft->bm_inc.nitrogen+=nrecovered;
      pft->stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen+=(navailable-nrecovered);
      pft->npp_nrecovery+=npp_for_recovery;
    }
  }
  else
  {
    /* if no N demand, N recovery is skipped and the shared available for recovery is returned to litter */
    pft->stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor*pft->nind*(1.0-pft->par->fn_turnover);
  }
#else
  pft->bm_inc.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*factor*pft->nind*(1-pft->par->fn_turnover);
#endif
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*factor*pft->nind,0);
  
}

void turnover_daily_tree(Litter *litter, /**< pointer to litter data */
                         Pft *pft,       /**< pointer to tree PFT */
                         Real temp,      /**< air temperature (deg C) */
                         int day,        /**< day (1..365) */
                         Bool isdaily,   /**< daily temperature data? */
                         const Config *config /**< LPJmL configuration */
                        )
{
  Pfttree *tree;
  Pfttreepar *treepar;
  tree=pft->data;
  treepar=pft->par->data;
  Real dtemp,gddtw;
  dtemp=temp-getpftpar(pft,gddbase);
  gddtw=temp-((isdaily) ? getpftpar(pft,twmax_daily) : getpftpar(pft,twmax));
  tree->gddtw+=(gddtw>0.0) ? gddtw : 0.0;

  switch(getpftpar(pft,phenology))
  {
    case SUMMERGREEN:
      if(!tree->isphen && tree->turn.leaf.carbon<epsilon)
      {

        if(pft->aphen>treepar->aphen_max)
        {
          f_turnover_tree(pft,day,config,1.0);
          tree->isphen=TRUE;
        }
        else if(dtemp<0 && pft->aphen>treepar->aphen_min)
        {
          f_turnover_tree(pft,day,config,1.0);
          tree->isphen=TRUE;
        }
        else if(((pft->stand->cell->coord.lat>=0.0 && day==NDAYYEAR-1) ||
                (pft->stand->cell->coord.lat<0.0 && day==COLDEST_DAY_SHEMISPHERE-1))
      		  	  && pft->aphen>treepar->aphen_min)
        {
          f_turnover_tree(pft,day,config,1.0);
          tree->isphen=TRUE;
        }
      }
      break;
    default:
      f_turnover_tree(pft,day,config,1.0/NDAYYEAR);
      break;
  } /*switch*/
} /* of 'turnover_daily_tree' */
