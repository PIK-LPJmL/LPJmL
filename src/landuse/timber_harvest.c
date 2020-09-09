/**************************************************************************************/
/**                                                                                \n**/
/**             t  i  m  b  e  r  _  h  a  r  v  e  s  t  .  c                     \n**/
/**                                                                                \n**/
/**     extension of LPJ to compute timber harvest / burning                       \n**/
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

Stocks timber_harvest(Pft *pft,      /**< Pointer to tree PFT */
                      Soil *soil,      /**< Litter pool */
                      Poolpar *timber,
                      Poolpar f,     /**< fractions for timber distribution */
                      Real ftimber,
                      Real standfrac,
                      Real *nind,          /**< cannot use pft->nind, since pft is on different stand */
                      Real *trad_biofuel
#ifdef IMAGE
                    ,
                    Real timber_frac,
                    Real takeaway_image[]
#endif
                    )              /** \return harvested carbon (gC/m2) */
{
  int i;
  const Pfttree *tree;
  const Pfttreepar *treepar;
  Output *output;
  Stocks harvest={0,0};
  Real biofuel;
//TakeAway fractions for 4 tree components (stems[0], branches[1], leaves[2], roots[3])
#ifdef IMAGE
  Stocks harvestroots;
  Stocks harvestleaves;
  Real takeAway[4]={1.0,1.0,0.0,0.0};
  if(timber_frac>epsilon)
  {
     for (i=0;i<NIMAGETREEPARTS;i++){
        takeAway[i]=takeaway_image[i];
     }
  }
#endif
  tree=pft->data;
  treepar=pft->par->data;
  if(ftimber<epsilon)
    return harvest;
  output=&pft->stand->cell->output;
  /* transfer wood to product pools, assume 2/3 of sapwood to be above-ground */
#ifdef IMAGE
  harvest.carbon=(tree->ind.heartwood.carbon+tree->ind.sapwood.carbon*2.0/3.0)*
                 ftimber*(*nind)*standfrac*((0.84*takeAway[0])+(0.16*takeAway[1])); // [gC/m2(cell)]
  harvest.nitrogen=(tree->ind.heartwood.nitrogen+tree->ind.sapwood.nitrogen*2.0/3.0)*
                 ftimber*(*nind)*standfrac*((0.84*takeAway[0])+(0.16*takeAway[1])); // [gC/m2(cell)]
  harvestleaves.carbon=tree->ind.leaf.carbon*ftimber*(*nind)*standfrac*takeAway[2];
  harvestleaves.nitrogen=tree->ind.leaf.nitrogen*ftimber*(*nind)*standfrac*takeAway[2];
  harvestroots.carbon=(tree->ind.root.carbon+(tree->ind.sapwood.carbon/3.0))*ftimber*(*nind)*standfrac*takeAway[3];
  harvestroots.nitrogen=(tree->ind.root.nitrogen+(tree->ind.sapwood.nitrogen/3.0))*ftimber*(*nind)*standfrac*takeAway[3];
  timber->slow+=(harvest.carbon+harvestroots.carbon+harvestleaves.carbon)*f.slow;
  timber->fast+=(harvest.carbon+harvestroots.carbon+harvestleaves.carbon)*f.fast;
  biofuel=1.0-f.slow-f.fast; // [-]
  *trad_biofuel+=(harvest.carbon+harvestroots.carbon+harvestleaves.carbon)*biofuel*0.9; // [gC/m2(cell)]]
  /* 10% of traditional biofuel is assumed to enter fast soil pool -- may not be scaled with standfrac!*/
  soil->pool[0].fast.carbon+=(harvest.carbon+harvestroots.carbon+harvestleaves.carbon)*biofuel*0.1/standfrac;  // [gC/m2(stand)]
  soil->pool[0].fast.nitrogen+=(harvest.nitrogen+harvestroots.nitrogen+harvestleaves.nitrogen)*biofuel*0.1/standfrac;  // [gC/m2(stand)]
  /* transfer non-harvested wood, leaves, and roots of trees cut to litter */
  soil->litter.ag[pft->litter].trait.leaf.carbon+=tree->ind.leaf.carbon*ftimber*(*nind)*(1-takeAway[2]);
  soil->litter.ag[pft->litter].trait.leaf.nitrogen+=tree->ind.leaf.nitrogen*ftimber*(*nind)*(1-takeAway[2]);
  for(i=0;i<NFUELCLASS;i++)
  {
    soil->litter.ag[pft->litter].trait.wood[i].carbon+=(tree->ind.sapwood.carbon/3.0-tree->ind.debt.carbon)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*(1-takeAway[3]);
    soil->litter.ag[pft->litter].trait.wood[i].nitrogen+=(tree->ind.sapwood.nitrogen/3.0-tree->ind.debt.nitrogen)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*(1-takeAway[3]);
    soil->litter.ag[pft->litter].trait.wood[i].carbon+=(tree->ind.heartwood.carbon+tree->ind.sapwood.carbon*2.0/3.0)*
                 ftimber*(*nind)*treepar->fuelfrac[i]*(1-((0.84*takeAway[0])+(0.16*takeAway[1])));
    soil->litter.ag[pft->litter].trait.wood[i].nitrogen+=(tree->ind.heartwood.nitrogen+tree->ind.sapwood.nitrogen*2.0/3.0)*
                 ftimber*(*nind)*treepar->fuelfrac[i]*(1-((0.84*takeAway[0])+(0.16*takeAway[1])));
  }
  soil->litter.bg[pft->litter].carbon+=tree->ind.root.carbon*ftimber*(*nind)*(1-takeAway[3]); //*(1-takeAway_roots)
  soil->litter.bg[pft->litter].nitrogen+=tree->ind.root.nitrogen*ftimber*(*nind)*(1-takeAway[3]); //*(1-takeAway_roots)
  /* update carbon pools by reducing nind by number of trees cut */
  harvest.carbon+=harvestroots.carbon+harvestleaves.carbon;
  harvest.nitrogen+=harvestroots.nitrogen+harvestleaves.nitrogen;
#else
  harvest.carbon=(tree->ind.heartwood.carbon+tree->ind.sapwood.carbon*2.0/3.0)*
                 ftimber*(*nind)*standfrac;
  harvest.nitrogen=(tree->ind.heartwood.nitrogen+tree->ind.sapwood.nitrogen*2.0/3.0)*
                 ftimber*(*nind)*standfrac;
  biofuel=0;
  pft->stand->cell->ml.product.fast.nitrogen+=harvest.nitrogen*f.fast;
  pft->stand->cell->ml.product.slow.nitrogen+=harvest.nitrogen*f.slow;
  pft->stand->cell->ml.product.fast.carbon+=harvest.carbon*f.fast;
  pft->stand->cell->ml.product.slow.carbon+=harvest.carbon*f.slow;
  soil->pool[0].fast.carbon+=harvest.carbon*biofuel*0.1/standfrac;
  soil->pool[0].fast.nitrogen+=harvest.nitrogen*biofuel*0.1/standfrac;
  /* transfer non-harvested wood, leaves, and roots of trees cut to litter */
  soil->litter.ag[pft->litter].trait.leaf.carbon+=tree->ind.leaf.carbon*ftimber*(*nind);
  output->alittfall.carbon+=tree->ind.leaf.carbon*ftimber*(*nind)*standfrac;
  soil->litter.ag[pft->litter].trait.leaf.nitrogen+=tree->ind.leaf.nitrogen*ftimber*(*nind);
  output->alittfall.nitrogen+=tree->ind.leaf.nitrogen*ftimber*(*nind)*standfrac;
  soil->litter.ag[pft->litter].trait.leaf.nitrogen+=pft->bm_inc.nitrogen*ftimber;
  output->alittfall.nitrogen+=pft->bm_inc.nitrogen*ftimber*standfrac;
  for(i=0;i<NFUELCLASS;i++)
  {
    soil->litter.ag[pft->litter].trait.wood[i].carbon+=(tree->ind.sapwood.carbon/3.0-tree->ind.debt.carbon+tree->excess_carbon)*
                                         ftimber*(*nind)*treepar->fuelfrac[i];
    output->alittfall.carbon+=(tree->ind.sapwood.carbon/3.0-tree->ind.debt.carbon+tree->excess_carbon)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*standfrac;
    soil->litter.ag[pft->litter].trait.wood[i].nitrogen+=(tree->ind.sapwood.nitrogen/3.0-tree->ind.debt.nitrogen)*
                                         ftimber*(*nind)*treepar->fuelfrac[i];
    output->alittfall.nitrogen+=(tree->ind.sapwood.nitrogen/3.0-tree->ind.debt.nitrogen)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*standfrac;
  }
  soil->litter.bg[pft->litter].carbon+=tree->ind.root.carbon*ftimber*(*nind);
  output->alittfall.carbon+=tree->ind.root.carbon*ftimber*(*nind)*standfrac;
  soil->litter.bg[pft->litter].nitrogen+=tree->ind.root.nitrogen*ftimber*(*nind);
  output->alittfall.nitrogen+=tree->ind.root.nitrogen*ftimber*(*nind)*standfrac;
#endif
  /* update carbon pools by reducing nind by number of trees cut */
  *nind*=(1-ftimber);
  return harvest;
} /* of 'timber_harvest' */
