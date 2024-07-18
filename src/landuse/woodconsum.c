/**************************************************************************************/
/**                                                                                \n**/
/**            w  o  o  d  c  o  n  s  u  m  .  c                                  \n**/
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

#define max_frac 1.0

static Stocks veg_tree(const Pft *pft)
{
  Stocks vegc_tree;
  Pfttree *tree;
  tree=pft->data;
  vegc_tree.carbon=tree->ind.sapwood.carbon*2.0/3.0*pft->nind+(tree->ind.heartwood.carbon-tree->ind.debt.carbon)*pft->nind;  /* [gC/m2] */
  vegc_tree.nitrogen=tree->ind.sapwood.nitrogen*2.0/3.0*pft->nind+(tree->ind.heartwood.nitrogen-tree->ind.debt.nitrogen)*pft->nind;  /* [gC/m2] */
  return vegc_tree;
} /* of 'vegc_tree' */

static Stocks woodconsum_tree(Pft *pft,Real restconsum,Litter *litter)
{
/* uses equation: disturb/pft->nind = restconsum/(tree_ind*pft->nind);  */
/*               [#indiv]/[#indiv]     [gC/m2]  /      [gC/m2]          */

  Stocks vegctree;
  Stocks flux={0,0};
  Real disturb;
  Pfttree *tree;
  tree=pft->data;
  disturb=0;

  vegctree=veg_tree(pft); /* [gC/m2] */

  disturb=restconsum/vegctree.carbon*pft->nind;  /* number of individuals that need to be killed to meet the demand*/
  if(disturb>pft->nind)
    disturb=pft->nind;
  if(disturb<epsilon)
    return flux;
  flux.carbon=disturb/pft->nind*vegctree.carbon;    /* wood only [gC/m2] */
  flux.nitrogen=disturb/pft->nind*vegctree.nitrogen;    /* wood only [gN/m2] */
  litter->item[pft->litter].bg.carbon+=(disturb*tree->ind.root.carbon+disturb*tree->ind.sapwood.carbon/3.0);
  litter->item[pft->litter].bg.nitrogen+=(disturb*tree->ind.root.nitrogen+disturb*tree->ind.sapwood.nitrogen/3.0);
  litter->item[pft->litter].bg.carbon+=(1-max_frac)*flux.carbon;
  litter->item[pft->litter].bg.nitrogen+=(1-max_frac)*flux.nitrogen;
  litter->item[pft->litter].agtop.leaf.carbon+=disturb*tree->ind.leaf.carbon; /* cannot be burned, send to litter*/
  litter->item[pft->litter].agtop.leaf.nitrogen+=disturb*tree->ind.leaf.nitrogen; /* cannot be burned, send to litter*/
  update_fbd_tree(litter,pft->par->fuelbulkdensity,disturb*tree->ind.leaf.carbon,0);
  pft->nind-=disturb;
  flux.carbon*=max_frac;
  flux.nitrogen*=max_frac;
  fpc_tree(pft);
  return flux; /* harvested wood [gC/m2, gN/m2] */
} /* of 'woodconsum_tree' */

Stocks woodconsum(Stand *stand, /**< pointer to stand */
                  Real popdens  /**< population density (persons/km2) */
                 )              /**< wood and litter consumed (g/C2,gN/m2) */
{
  Real litter_frac,woodconsumption,restconsum;
  Real woodconsum;
  Stocks stocks;
  Stocks sum_litter={0,0};
  Stocks flux={0,0};
  Stocks flux_tree={0,0};
  Stocks flux_litter={0,0};
  int i,j,p,*index,temp;
  Real *vegc;
  Pft *pft;
  woodconsum=stand->cell->ml.manage.regpar->woodconsum;
  vegc=newvec(Real,getnpft(&stand->pftlist));
  check(vegc);
  index=newvec(int,getnpft(&stand->pftlist));
  check(index);

  litter_frac=woodconsumption=restconsum=0;
  for(i=0;i<getnpft(&stand->pftlist);i++)
  {
    vegc[i]=0;
    index[i]=i;
  }

  /* calculation of woodconsumption in [gC/m2] */
  woodconsumption=biomass2c(woodconsum)/stand->frac*popdens/1000*NDAYYEAR;

  for(i=1;i<NFUELCLASS;++i)
  {
    sum_litter.carbon+=litter_agtop_tree(&stand->soil.litter,i);
    sum_litter.nitrogen+=litter_agtop_nitrogen_tree(&stand->soil.litter,i);
  }
  /* calculate litter flux and reduce the litter */
  if(sum_litter.carbon>=0.00001 && stand->frac>=0.00001)
  {
    litter_frac=min(max_frac,woodconsumption/sum_litter.carbon);
    flux_litter.carbon=sum_litter.carbon*litter_frac;
    flux_litter.nitrogen=sum_litter.nitrogen*litter_frac;
    for(i=0;i<stand->soil.litter.n;i++)
      for(j=1;j<NFUELCLASS;j++)
      {
        stand->soil.litter.item[i].agtop.wood[j].carbon*=(1-litter_frac);
        stand->soil.litter.item[i].agtop.wood[j].nitrogen*=(1-litter_frac);
      }
  }
  restconsum=woodconsumption-flux_litter.carbon;
  if(restconsum > epsilon)
  {
    foreachpft(pft,p,&stand->pftlist)
      if(istree(pft))
      {
        stocks=veg_tree(pft);
        vegc[p]=stocks.carbon;
      }

    /* sort vegc in descending order, store order in index */
    for(i=getnpft(&stand->pftlist)-1;i>0;i--)
      for(j=0;i>j;j++)
      {
        if(vegc[index[j]]<vegc[index[j+1]])
        {
          temp=index[j];
          index[j]=index[j+1];
          index[j+1]=temp;
        }
      }

    /* restconsum if litter is not enough -> taking wood of trees */
    for(i=0;i<getnpft(&stand->pftlist);i++)
    {
      if(restconsum<epsilon || vegc[index[i]]<epsilon)
        break;
      flux=woodconsum_tree(getpft(&stand->pftlist,index[i]),restconsum,
                           &stand->soil.litter);

      restconsum-=flux.carbon;
      flux_tree.carbon+=flux.carbon;
      flux_tree.nitrogen+=flux.nitrogen;
    }
  }
  free(vegc);
  free(index);
  flux_tree.carbon+=flux_litter.carbon;
  flux_tree.nitrogen+=flux_litter.nitrogen;
  return flux_tree;
} /* of 'woodconsum' */

/* called in annual_natural() */
