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

Real timber_harvest(Pft *pft,      /**< Pointer to tree PFT */
                    Soil *soil,      /**< Litter pool */
                    Pool *timber,
                    Pool f,        /**< fractions for timber distribution */
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
  Real harvest;                          /* Carbon harvested as timber [gC/m2/a] */
  Real harvestroots;
  Real harvestleaves;
  Real biofuel;
//TakeAway fractions for 4 tree components (stems[0], branches[1], leaves[2], roots[3])
  Real takeAway[4]={1.0,1.0,0.0,0.0};
#ifdef IMAGE
  if(timber_frac>epsilon);{
     for (i=0;i<NIMAGETREEPARTS;i++){
        takeAway[i]=takeaway_image[i];
     }
  }
#endif
  tree=pft->data;
  treepar=pft->par->data;
  if(ftimber<epsilon)
    return(0.);
  /* transfer wood to product pools, assume 2/3 of sapwood to be above-ground */
  harvest=(tree->ind.heartwood+tree->ind.sapwood*2.0/3.0)*
                 ftimber*(*nind)*standfrac*((0.84*takeAway[0])+(0.16*takeAway[1])); // [gC/m2(cell)]
  harvestleaves=tree->ind.leaf*ftimber*(*nind)*standfrac*takeAway[2];
  harvestroots=(tree->ind.root+(tree->ind.sapwood/3.0))*ftimber*(*nind)*standfrac*takeAway[3];
  timber->slow+=(harvest+harvestroots+harvestleaves)*f.slow;
  timber->fast+=(harvest+harvestroots+harvestleaves)*f.fast; 
  biofuel=1.0-f.slow-f.fast; // [-]
  *trad_biofuel+=(harvest+harvestroots+harvestleaves)*biofuel*0.9; // [gC/m2(cell)]]
  /* 10% of traditional biofuel is assumed to enter fast soil pool -- may not be scaled with standfrac!*/
  soil->cpool[0].fast+=(harvest+harvestroots+harvestleaves)*biofuel*0.1/standfrac;  // [gC/m2(stand)]
  /* transfer non-harvested wood, leaves, and roots of trees cut to litter */
  soil->litter.ag[pft->litter].trait.leaf+=tree->ind.leaf*ftimber*(*nind)*(1-takeAway[2]);
  for(i=0;i<NFUELCLASS;i++)
  {
    soil->litter.ag[pft->litter].trait.wood[i]+=(tree->ind.sapwood/3.0-tree->ind.debt)*
                                         ftimber*(*nind)*treepar->fuelfrac[i]*(1-takeAway[3]);
    soil->litter.ag[pft->litter].trait.wood[i]+=(tree->ind.heartwood+tree->ind.sapwood*2.0/3.0)*
                 ftimber*(*nind)*treepar->fuelfrac[i]*(1-((0.84*takeAway[0])+(0.16*takeAway[1])));
  }
  soil->litter.bg[pft->litter]+=tree->ind.root*ftimber*(*nind)*(1-takeAway[3]); //*(1-takeAway_roots)
  /* update carbon pools by reducing nind by number of trees cut */
  *nind*=(1-ftimber);
  return(harvest+harvestroots+harvestleaves);
} /* of 'timber_harvest' */
