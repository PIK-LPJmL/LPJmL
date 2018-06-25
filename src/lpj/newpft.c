/**************************************************************************************/
/**                                                                                \n**/
/**                       n  e  w  p  f  t  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates and initializes PFT variables                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void newpft(Pft *pft,            /**< PFT variables */
            Stand *stand,        /**< Stand pointer */
            const Pftpar *pftpar,/**< Parameter of PFT */
            int year,            /**< anno domini */
            int day              /**< day of year (1..365) */
           )
{
  int index;
  pft->par=pftpar;
  /* find litter pool for PFT */
  index=findlitter(&stand->soil.litter,pftpar);
  if(index==NOT_FOUND)
  {
    /* not found, add new litter pool for PFT */
    pft->litter=addlitter(&stand->soil.litter,pftpar)-1;
  }
  else
    /* save index of litter pool */
    pft->litter=index;
  /* Initialize variables to zero */
  pft->stand=stand;
  pft->fpc=pft->nind=pft->wscal=pft->aphen=pft->bm_inc=pft->wscal_mean=
           pft->gdd=pft->phen=pft->fapar=0.0;
 pft->phen_gsi.tmin=pft->phen_gsi.light=pft->albedo=0;
 pft->phen_gsi.tmax=pft->phen_gsi.wscal=1;
#ifdef DAILY_ESTABLISHMENT
  pft->established=FALSE;
#endif
  pft->prescribe_fpc=FALSE;
  pft->par->newpft(pft,year,day); /* type-specific allocation of memory */
} /* of 'newpft' */
