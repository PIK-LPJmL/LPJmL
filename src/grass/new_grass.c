/**************************************************************************************/
/**                                                                                \n**/
/**                       n  e  w  _  g  r  a  s  s  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates and initializes grass-specific variables                \n**/
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

void new_grass(Pft *pft,         /**< pointer to PFT variables */
               int UNUSED(year),
               int UNUSED(day)
              )        
{
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  grass=new(Pftgrass);
  check(grass);
  pft->data=grass;
  grasspar=pft->par->data;
  pft->bm_inc.carbon=pft->wscal_mean=pft->phen=0;
  pft->bm_inc.nitrogen=0;
  pft->stand->growing_days=0;
  grass->max_leaf=0;
  grass->excess_carbon=0;
  pft->vmax=0;
  pft->nind=1;
  pft->stand->growing_days=0;
  grass->falloc.leaf=grasspar->sapl.leaf/(grasspar->sapl.leaf+grasspar->sapl.root);
  grass->falloc.root=grasspar->sapl.root/(grasspar->sapl.leaf+grasspar->sapl.root);
  grass->ind.leaf.carbon=grass->ind.root.carbon=grass->turn.leaf.carbon=grass->turn.root.carbon=0;
  grass->ind.leaf.nitrogen=grass->ind.root.nitrogen=grass->turn.leaf.nitrogen=grass->turn.root.nitrogen=0;
} /* of 'new_grass' */
