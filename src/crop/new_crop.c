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
              int day   /**< day of year */
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
  crop->ind.so=0.0;
  pft->bm_inc=20.0;
  crop->laimax_adjusted=1; 
  crop->flaimax=0.000083; 
  crop->demandsum=0;
  crop->supplysum=0;
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

  allocation_daily_crop(pft,0,NO_WDF,NULL); 
} /* of 'new_crop' */

/*
- new_crop() is called when a new crop is added to a stand (see newpft.c)
- sets fpc 1 for crops
- provides memory place for specific crop variables
  -> initialisation of specific crop variables (see crop.h)
*/
