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
              const Config *config /**< LPJmL configuration */
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
  crop->basetemp=par->basetemp.low;
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
  if(pft->stand->type->landusetype==AGRICULTURE && config->separate_harvests)
  {
    crop->sh=new(Separate_harvests);
    check(crop->sh);
    crop->sh->petsum=0.0;
    crop->sh->evapsum=0.0;
    crop->sh->transpsum=0.0;
    crop->sh->intercsum=0.0;
    crop->sh->precsum=0.0;
    crop->sh->sradsum=0.0;
    crop->sh->irrig_apply=0.0;
    crop->sh->tempsum=0.0;
    crop->sh->nirsum=0.0;
    crop->sh->lgp=0.0;
    crop->sh->sowing_year=year;
    crop->sh->sdate=day;
    crop->sh->runoffsum=0.0;
    crop->sh->n2o_denitsum=0.0;
    crop->sh->n2o_nitsum=0.0;
    crop->sh->n2_emissum=0.0;
    crop->sh->leachingsum=0.0;
    crop->sh->c_emissum=0.0;
    crop->sh->nfertsum=0.0;
  }
  else
    crop->sh=NULL;
  crop->lai=crop->lai000=crop->flaimax*par->laimax;
  crop->lai_nppdeficit=0.0;
  pft->phen=crop->lai/par->laimax;
  crop->senescence=crop->senescence0=FALSE;
  pft->nleaf = 0;
  pft->vscal = 1;
  crop->nfertilizer=0;
  crop->nmanure=0;

  allocation_daily_crop(pft,0,NO_WDF,config);
} /* of 'new_crop' */

/*
- new_crop() is called when a new crop is added to a stand (see newpft.c)
- sets fpc 1 for crops
- provides memory place for specific crop variables
  -> initialisation of specific crop variables (see crop.h)
*/
