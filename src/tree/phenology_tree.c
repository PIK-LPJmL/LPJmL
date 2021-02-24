/**************************************************************************************/
/**                                                                                \n**/
/**             p  h  e  n  o  l  o  g  y  _  t  r  e  e  .  c                     \n**/
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

void phenology_tree(Pft *pft,    /**< pointer to PFT variables */
                    Real temp,   /**< temperature (deg C) */
                    int day,     /**< day of year (1..365) */
                    Bool isdaily, /**< daily temperatures (TRUE/FALSE) */
                    const Config *config /**< LPJmL configuration */
                   )
{
  Pfttree *tree;
  Real dtemp,gddtw;
  Pfttreepar *treepar;
  Output *output;
  tree=pft->data;
  treepar=pft->par->data;
  output=&pft->stand->cell->output;
  dtemp=temp - getpftpar(pft,gddbase);
  gddtw=temp - ((isdaily) ? getpftpar(pft,twmax_daily) : getpftpar(pft,twmax));
  if(dtemp>0.0)
    pft->gdd+=dtemp;
  tree->gddtw+= (gddtw>0.0) ? gddtw : 0.0;
  switch(getpftpar(pft,phenology))
  {
    case SUMMERGREEN:
      if(pft->aphen<treepar->aphen_max)
      {
        pft->phen=pft->gdd*getpftpar(pft,ramp);
        if(pft->phen>1)
          pft->phen=1;
      }
      else
      {
         if(!tree->isphen)
         {
           if (tree->turn.leaf.carbon!=treepar->sapl.leaf.carbon*treepar->turnover.leaf)
           {
             tree->turn.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf;
             tree->turn.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf;
             tree->turn_litt.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind;
             tree->turn_litt.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind;
             pft->stand->soil.litter.item[pft->litter].ag.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind;
             getoutput(output,LITFALLC,config)+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind*pft->stand->frac;
             pft->stand->soil.litter.item[pft->litter].ag.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*pft->par->fn_turnover;
             getoutput(output,LITFALLN,config)+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*pft->par->fn_turnover*pft->stand->frac;
             tree->turn_nbminc+= tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*(1-pft->par->fn_turnover);

             update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind,0);
           }
           pft->phen=0.0;
         }
         tree->isphen=TRUE;
      }
      /* cold-induced senescence;
         works only for linearly interpolated, monthly temperature data! */
      if(dtemp<0 && pft->aphen>treepar->aphen_min && !tree->isphen && tree->turn.leaf.carbon<epsilon)
      {
        tree->isphen=TRUE;
        pft->phen=pft->gdd=0.0;
        if (tree->turn.leaf.carbon!=treepar->sapl.leaf.carbon*treepar->turnover.leaf)
        {
          tree->turn.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf;
          tree->turn.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf;
          tree->turn_litt.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind;
          tree->turn_litt.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind;
          pft->stand->soil.litter.item[pft->litter].ag.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind;
          getoutput(output,LITFALLC,config)+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind*pft->stand->frac;
          pft->stand->soil.litter.item[pft->litter].ag.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*pft->par->fn_turnover;
          getoutput(output,LITFALLN,config)+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*pft->par->fn_turnover*pft->stand->frac;
          tree->turn_nbminc+= tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*(1-pft->par->fn_turnover);
          update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind,0);
        }
      }
      break;
    case RAINGREEN:
      if(tree->aphen_raingreen>80 && !tree->isphen && tree->turn.leaf.carbon<epsilon && pft->phen<epsilon)
      {
        tree->isphen=TRUE;
        if (tree->turn.leaf.carbon!=treepar->sapl.leaf.carbon*treepar->turnover.leaf)
        {
          tree->turn.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf;
          tree->turn.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf;
          tree->turn_litt.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind;
          tree->turn_litt.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind;
          pft->stand->soil.litter.item[pft->litter].ag.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind;
          getoutput(output,LITFALLC,config)+=tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind*pft->stand->frac;
          pft->stand->soil.litter.item[pft->litter].ag.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*pft->par->fn_turnover;
          getoutput(output,LITFALLN,config)+=tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*pft->par->fn_turnover*pft->stand->frac;
          tree->turn_nbminc+= tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*(1-pft->par->fn_turnover);
          update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*pft->nind,0);
        }
      }

      if(pft->wscal<getpftpar(pft,minwscal))
        pft->phen=0.0;
      else
      {
        pft->phen=1.0;
        tree->aphen_raingreen++;
        pft->aphen++;
      }
      break;
    case EVERGREEN:
      pft->phen=1;
      tree->turn.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf/NDAYYEAR;
      tree->turn.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf/NDAYYEAR;
      tree->turn_litt.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf/NDAYYEAR*pft->nind;
      tree->turn_litt.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf/NDAYYEAR*pft->nind;
      pft->stand->soil.litter.item[pft->litter].ag.leaf.carbon+=tree->ind.leaf.carbon*treepar->turnover.leaf/NDAYYEAR*pft->nind;
      getoutput(output,LITFALLC,config)+=tree->ind.leaf.carbon*treepar->turnover.leaf/NDAYYEAR*pft->nind*pft->stand->frac;
      pft->stand->soil.litter.item[pft->litter].ag.leaf.nitrogen+=tree->ind.leaf.nitrogen*treepar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
      getoutput(output,LITFALLN,config)+=tree->ind.leaf.nitrogen*treepar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover*pft->stand->frac;
      tree->turn_nbminc+= tree->ind.leaf.nitrogen*treepar->turnover.leaf*pft->nind*(1-pft->par->fn_turnover)/NDAYYEAR;
      update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf/NDAYYEAR*pft->nind,0);
      break;
  } /* of 'switch' */
  if ((pft->stand->cell->coord.lat>=0.0 && day==NDAYYEAR) ||
      (pft->stand->cell->coord.lat<0.0 && day==COLDEST_DAY_SHEMISPHERE))
  {
    pft->aphen=pft->gdd=0.0;
    tree->isphen=FALSE;
  }
  if (pft->phen>0)
    pft->aphen++;

} /* of 'phenology_tree' */
