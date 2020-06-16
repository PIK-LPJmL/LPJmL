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
  litter->ag=(Litteritem *)realloc(litter->ag,
                                   sizeof(Litteritem)*(litter->n+1));
  check(litter->ag);
  litter->bg=(Real *)realloc(litter->bg,
                                   sizeof(Real)*(litter->n+1));
  check(litter->bg);
  litter->ag[litter->n].pft=pft;
  litter->ag[litter->n].trait.leaf=0;
  litter->bg[litter->n]=0;
  for(i=0;i<NFUELCLASS;i++)
    litter->ag[litter->n].trait.wood[i]=0;
  litter->n++;
  return litter->n;
} /* of 'addlitter' */
