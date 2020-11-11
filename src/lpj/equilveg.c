/**************************************************************************************/
/**                                                                                \n**/
/**                     e  q  u  i  l  v  e  g  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     After vegetation equilibrium reset decomposition parameter                 \n**/
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
#include "tree.h"

void equilveg(Cell *cell /**< pointer to cell */
             )
{
  int s,l,p;
  Stand *stand;
  Pft *pft;
  Pfttree *tree;
  Pftgrass *grass;
  
  foreachstand(stand,s,cell->standlist)
  {
    forrootsoillayer(l)
    {
      stand->soil.k_mean[l].fast=0.0;
      stand->soil.k_mean[l].slow=0.0;
    }
    foreachpft(pft,p,&stand->pftlist)
    {
      if(istree(pft))
      {
        tree=pft->data;
        tree->excess_carbon=0.0;
      }
      else
      {
        grass=pft->data;
        grass->excess_carbon=0.0;
      }
    }
    stand->soil.decomp_litter_mean.carbon=stand->soil.decomp_litter_mean.nitrogen=stand->soil.count=0;
  }
} /* of 'equilveg' */
