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

void turnover_daily_tree(Litter *litter, /**< pointer to litter data */
                         Pft *pft,       /**< pointer to tree PFT */
                         Real temp,      /**< air temperature (deg C) */
                         Bool isdaily    /**< daily temperature data? */
                        )
{
  Pfttree *tree;
  Pfttreepar *treepar;
  tree=pft->data;
  treepar=pft->par->data;
  Real dtemp,gddtw;
  dtemp=temp - getpftpar(pft,gddbase);
  gddtw=temp - ((isdaily) ? getpftpar(pft,twmax_daily) : getpftpar(pft,twmax));
  tree->gddtw+= (gddtw>0.0) ? gddtw : 0.0;

  switch(getpftpar(pft,phenology))
  {
    case SUMMERGREEN:
      if(pft->aphen>treepar->aphen_max && !tree->isphen && tree->turn.leaf<epsilon)
      {
        tree->turn.leaf+=tree->ind.leaf*treepar->turnover.leaf;
        tree->turn_litt.leaf+=tree->ind.leaf*treepar->turnover.leaf*pft->nind;
        pft->stand->soil.litter.ag[pft->litter].trait.leaf+=tree->ind.leaf*treepar->turnover.leaf*pft->nind;
        update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf*treepar->turnover.leaf*pft->nind,0);
        tree->isphen=TRUE;
      }
      else if(dtemp<0 && pft->aphen>treepar->aphen_min && !tree->isphen && tree->turn.leaf<epsilon)
      {
        tree->isphen=TRUE;
        tree->turn.leaf+=tree->ind.leaf*treepar->turnover.leaf;
        tree->turn_litt.leaf+=tree->ind.leaf*treepar->turnover.leaf*pft->nind;
        litter->ag[pft->litter].trait.leaf+=tree->ind.leaf*treepar->turnover.leaf*pft->nind;
        update_fbd_tree(litter,pft->par->fuelbulkdensity,tree->ind.leaf*treepar->turnover.leaf*pft->nind,0);

      }
      break;
    default:
      tree->turn.leaf+=tree->ind.leaf*treepar->turnover.leaf/NDAYYEAR;
      tree->turn_litt.leaf+=tree->ind.leaf*treepar->turnover.leaf/NDAYYEAR*pft->nind;
      litter->ag[pft->litter].trait.leaf+=tree->ind.leaf*treepar->turnover.leaf/NDAYYEAR*pft->nind;
      update_fbd_tree(litter,pft->par->fuelbulkdensity,tree->ind.leaf*treepar->turnover.leaf/NDAYYEAR*pft->nind,0);
      break;
  } /*switch*/
} /* of 'turnover_daily_tree' */
