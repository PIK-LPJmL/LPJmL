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
                     )              /** \return harvested carbon (gC/m2) */
{
  int i;
  const Pfttree *tree;
  const Pfttreepar *treepar;
  Output *output;
  Stocks harvest={0,0};
  Real biofuel;
  tree=pft->data;
  treepar=pft->par->data;
  harvest.carbon = harvest.nitrogen = 0.0;
  if(ftimber<epsilon)
    return harvest;
  output=&pft->stand->cell->output;
  /* transfer wood to product pools, assume 2/3 of sapwood to be above-ground */
  harvest.carbon=(tree->ind.heartwood.carbon+tree->ind.sapwood.carbon*2.0/3.0)*
                 ftimber*(*nind)*standfrac;
  harvest.nitrogen=(tree->ind.heartwood.nitrogen+tree->ind.sapwood.nitrogen*2.0/3.0)*
                 ftimber*(*nind)*standfrac;
#ifdef IMAGE
  timber->slow+=harvest.carbon*f.slow;
  timber->fast+=harvest.carbon*f.fast;
  biofuel=1.0-f.slow-f.fast;
  *trad_biofuel+=harvest.carbon*biofuel*0.9;
  /* 10% of traditional biofuel is assumed to enter fast soil pool -- may not be scaled with standfrac!*/
#else
  biofuel=0;
#endif
  soil->pool[0].fast.carbon+=harvest.carbon*biofuel*0.1/standfrac;
  soil->pool[0].fast.nitrogen+=harvest.nitrogen*biofuel*0.1/standfrac;
  /* transfer non-harvested wood, leaves, and roots of trees cut to litter */
  soil->litter.item[pft->litter].ag.leaf.carbon+=tree->ind.leaf.carbon*ftimber*(*nind);
  output->alittfall.carbon+=tree->ind.leaf.carbon*ftimber*(*nind)*standfrac;
  soil->litter.item[pft->litter].ag.leaf.nitrogen+=tree->ind.leaf.nitrogen*ftimber*(*nind);
  output->alittfall.nitrogen+=tree->ind.leaf.nitrogen*ftimber*(*nind)*standfrac;
  soil->litter.item[pft->litter].ag.leaf.nitrogen+=pft->bm_inc.nitrogen*ftimber;
  output->alittfall.nitrogen+=pft->bm_inc.nitrogen*ftimber*standfrac;
  for(i=0;i<NFUELCLASS;i++)
  {
    soil->litter.item[pft->litter].ag.wood[i].carbon+=(tree->ind.sapwood.carbon/3.0-tree->ind.debt.carbon+tree->excess_carbon)*
                                         ftimber*(*nind)*treepar->fuelfrac[i];
    output->alittfall.carbon+=(tree->ind.sapwood.carbon/3.0-tree->ind.debt.carbon+tree->excess_carbon)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*standfrac;
    soil->litter.item[pft->litter].ag.wood[i].nitrogen+=(tree->ind.sapwood.nitrogen/3.0-tree->ind.debt.nitrogen)*
                                         ftimber*(*nind)*treepar->fuelfrac[i];
    output->alittfall.nitrogen+=(tree->ind.sapwood.nitrogen/3.0-tree->ind.debt.nitrogen)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*standfrac;
  }
  soil->litter.item[pft->litter].bg.carbon+=tree->ind.root.carbon*ftimber*(*nind);
  output->alittfall.carbon+=tree->ind.root.carbon*ftimber*(*nind)*standfrac;
  soil->litter.item[pft->litter].bg.nitrogen+=tree->ind.root.nitrogen*ftimber*(*nind);
  output->alittfall.nitrogen+=tree->ind.root.nitrogen*ftimber*(*nind)*standfrac;
  /* update carbon pools by reducing nind by number of trees cut */
  *nind*=(1-ftimber);
  return harvest;
} /* of 'timber_harvest' */
