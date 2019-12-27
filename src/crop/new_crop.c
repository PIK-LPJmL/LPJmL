/**************************************************************************************/
/**                                                                                \n**/
/**             n  e  w  _  c  r  o  p  .  c                                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates and initializes crop-specific variables                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

void new_crop(Pft *pft, /**< pointer to PFT data */
              int year, /**< year (AD) */
              int day,  /**< day of year */
              int with_nitrogen
             )
{
  Pftcrop *crop;
  Pftcroppar *par;
  crop=new(Pftcrop);
  check(crop);
  pft->data=crop;
  par=pft->par->data;

  pft->fpc=1;
  pft->nind=1;

  crop->growingdays=0;

  crop->vdsum=crop->husum=crop->fphu=0.0;
  crop->vscal_sum=0.0;
  crop->ind.so.carbon=0.0;
  crop->ind.so.nitrogen=0.0;
  crop->ind.root.nitrogen=0.0;
  crop->ind.leaf.nitrogen=0.0;
  crop->ind.pool.nitrogen=0.0;
  pft->bm_inc.carbon=20.0;
  pft->bm_inc.nitrogen=0.7; /*C:N ratio of seed = 29, see redmine #1560*/
  crop->laimax_adjusted=1; 
  crop->flaimax=0.000083; 
  crop->demandsum=0;
  crop->ndemandsum=crop->nuptakesum=0;
  crop->supplysum=0;
  pft->vmax=0;
#ifdef DOUBLE_HARVEST
  crop->petsum=0.0;
  crop->evapsum=0.0;
  crop->transpsum=0.0;
  crop->intercsum=0.0;
  crop->precsum=0.0;
  crop->sradsum=0.0;
  crop->pirrww=0.0;
  crop->tempsum=0.0;
  crop->nirsum=0.0;
  crop->lgp=0.0;
  crop->sowing_year=year;
  crop->sdate=day;
#endif
  crop->lai=crop->lai000=crop->flaimax*par->laimax;
  crop->lai_nppdeficit=0.0;
  pft->phen=crop->lai/par->laimax;
  crop->senescence=FALSE;
  pft->nleaf = 0;
  pft->vscal = 1;
  crop->nfertilizer=0;

  allocation_daily_crop(pft,0,NO_WDF,with_nitrogen,NULL); 
} /* of 'new_crop' */

/*
- new_crop() is called when a new crop is added to a stand (see newpft.c)
- sets fpc 1 for crops
- provides memory place for specific crop variables
  -> initialisation of specific crop variables (see crop.h)
*/
