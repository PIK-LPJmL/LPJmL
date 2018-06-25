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

static Real vegc_tree(const Pft *pft)
{
  Real vegc_tree;
  Pfttree *tree;
  tree=pft->data;
  vegc_tree=tree->ind.sapwood*2.0/3.0*pft->nind+(tree->ind.heartwood-tree->ind.debt)*pft->nind;  /* [gC/m2] */
  return vegc_tree;
} /* of 'vegc_tree' */

static Real woodconsum_tree(Pft *pft,Real restconsum,Litter *litter)
{
/* uses equation: disturb/pft->nind = restconsum/(tree_ind*pft->nind);  */
/*               [#indiv]/[#indiv]     [gC/m2]  /      [gC/m2]          */

  Real flux,vegctree,disturb;
  Pfttree *tree;
  tree=pft->data;
  flux=vegctree=disturb=0;

  vegctree=vegc_tree(pft); /* [gC/m2] */

  disturb=restconsum/vegctree*pft->nind;  /* number of individuals that need to be killed to meet the demand*/
  if(disturb>pft->nind)
    disturb=pft->nind;
  if(disturb<epsilon)
    return 0;
  flux=disturb/pft->nind*vegctree;    /* wood only [gC/m2] */
  litter->bg[pft->litter]+=(disturb*tree->ind.root+disturb*tree->ind.sapwood/3.0);
  litter->bg[pft->litter]+=(1-max_frac)*flux;
  litter->ag[pft->litter].trait.leaf+=disturb*tree->ind.leaf; /* cannot be burned, send to litter*/
  update_fbd_tree(litter,pft->par->fuelbulkdensity,disturb*tree->ind.leaf,0);
  pft->nind-=disturb;

  return flux*max_frac; /* harvested wood [gC/m2] */
} /* of 'woodconsum_tree' */

Real woodconsum(Stand *stand, /**< pointer to stand */
                Real popdens  /**< population density (persons/km2) */
               )              /**< wood and litter consumed (g/C2) */
{
  Real flux,flux_tree,flux_litter,litter_frac,woodconsumption,restconsum;
  Real sum_litter,woodconsum;
  int i,j,p,*index,temp;
  Real *vegc;
  Pft *pft;
  woodconsum=stand->cell->ml.manage.regpar->woodconsum;
  vegc=newvec(Real,getnpft(&stand->pftlist));
  check(vegc);
  index=newvec(int,getnpft(&stand->pftlist));
  check(index);

  flux=flux_tree=flux_litter=litter_frac=woodconsumption=restconsum=0;
  sum_litter=0;
  for(i=0;i<getnpft(&stand->pftlist);i++)
  {
    vegc[i]=0;
    index[i]=i;
  }

  /* calculation of woodconsumption in [gC/m2] */
  woodconsumption=biomass2c(woodconsum)/stand->frac*popdens/1000*NDAYYEAR;

  for(i=1;i<NFUELCLASS;++i)
    sum_litter+=litter_ag_tree(&stand->soil.litter,i);
  /* calculate litter flux and reduce the litter */
  if(sum_litter>=0.00001 && stand->frac>=0.00001)
  {
    litter_frac=min(max_frac,woodconsumption/sum_litter);
    flux_litter=sum_litter*litter_frac;
    for(i=0;i<stand->soil.litter.n;i++)
      for(j=1;j<NFUELCLASS;j++)
        stand->soil.litter.ag[i].trait.wood[j]*=(1-litter_frac);
  }
  restconsum=woodconsumption-flux_litter;
  if(restconsum > epsilon)
  {
    foreachpft(pft,p,&stand->pftlist)
      if(istree(pft))
        vegc[p]=vegc_tree(pft);

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

      restconsum-=flux;
      flux_tree+=flux;
    }
  }
  free(vegc);
  free(index);
  return flux_tree+flux_litter;
} /* of 'woodconsum' */

/* called in annual_natural() */
