/**************************************************************************************/
/**                                                                                \n**/
/**                a  d  d  l  i  t  t  e  r  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Function adds PFT-specific above ground litter pool                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int addlitter(Litter *litter,    /**< pointer to litter */
              const Pftpar *pft  /**< pointer to PFT parameter */
              )                  /** \return PFT-specific above ground litter pool */
{
  int i;
  litter->item=(Litteritem *)realloc(litter->item,
                                     sizeof(Litteritem)*(litter->n+1));
  check(litter->item);
  litter->item[litter->n].pft=pft;
  litter->item[litter->n].agtop.leaf.carbon=0;
  litter->item[litter->n].agtop.leaf.nitrogen=0;
  litter->item[litter->n].agsub.leaf.carbon=0;
  litter->item[litter->n].agsub.leaf.nitrogen=0;
  litter->item[litter->n].bg.carbon=0;
  litter->item[litter->n].bg.nitrogen=0;
  for(i=0;i<NFUELCLASS;i++)
  {
    litter->item[litter->n].agtop.wood[i].carbon=0;
    litter->item[litter->n].agtop.wood[i].nitrogen=0;
    litter->item[litter->n].agsub.wood[i].carbon=0;
    litter->item[litter->n].agsub.wood[i].nitrogen=0;
  }
  litter->n++;
  return litter->n;
} /* of 'addlitter' */
