/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  a  d  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads crop-specific variables from binary file                    \n**/
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

Bool fread_crop(FILE *file,          /**< file pointer */
                Pft *pft,            /**< PFT data to be read */
                Bool double_harvest, /**< double harvest output enabled? */
                Bool swap            /**< if true data is in different byte order */
               )                     /** \return TRUE on error */
{
  Pftcrop *crop;
  crop=new(Pftcrop);
  pft->data=crop;
  if(crop==NULL)
  {
    printallocerr("crop");
    return TRUE;
  }
  freadreal1(&pft->nlimit,swap,file);
  freadint1(&crop->wtype,swap,file);
  freadint1(&crop->growingdays,swap,file);
  freadreal1(&crop->pvd,swap,file);
  freadreal1(&crop->phu,swap,file);
  freadreal1(&crop->basetemp,swap,file);
  freadint1(&crop->senescence,swap,file);
  freadint1(&crop->senescence0,swap,file);
  freadreal1(&crop->husum,swap,file);
  freadreal1(&crop->vdsum,swap,file);
  freadreal1(&crop->fphu,swap,file);
  freadreal1(&crop->ind.leaf.carbon,swap,file);
  freadreal1(&crop->ind.leaf.nitrogen,swap,file);
  freadreal1(&crop->ind.root.carbon,swap,file);
  freadreal1(&crop->ind.root.nitrogen,swap,file);
  freadreal1(&crop->ind.pool.carbon,swap,file);
  freadreal1(&crop->ind.pool.nitrogen,swap,file);
  freadreal1(&crop->ind.so.carbon,swap,file);
  freadreal1(&crop->ind.so.nitrogen,swap,file);

  freadreal1(&crop->flaimax,swap,file);
  freadreal1(&crop->lai,swap,file);
  freadreal1(&crop->lai000,swap,file);
  freadreal1(&crop->laimax_adjusted,swap,file);
  freadreal1(&crop->lai_nppdeficit,swap,file);
  freadreal1(&crop->demandsum,swap,file);
  freadreal1(&crop->ndemandsum,swap,file);
  freadreal1(&crop->nuptakesum,swap,file);
  freadreal1(&crop->nfertilizer,swap,file);
  freadreal1(&crop->nmanure,swap,file);
  freadreal1(&crop->vscal_sum,swap,file);
  freadint1(&crop->frostkill,swap,file);
  if(pft->stand->type->landusetype==AGRICULTURE && double_harvest)
  {
    crop->dh=new(Double_harvest);
    if(crop->dh==NULL)
    {
      printallocerr("crop");
      free(crop);
      return TRUE;
    }
    freadreal1(&crop->dh->petsum,swap,file);
    freadreal1(&crop->dh->evapsum,swap,file);
    freadreal1(&crop->dh->transpsum,swap,file);
    freadreal1(&crop->dh->intercsum,swap,file);
    freadreal1(&crop->dh->precsum,swap,file);
    freadreal1(&crop->dh->sradsum,swap,file);
    freadreal1(&crop->dh->irrig_apply,swap,file);
    freadreal1(&crop->dh->tempsum,swap,file);
    freadreal1(&crop->dh->nirsum,swap,file);
    freadreal1(&crop->dh->lgp,swap,file);
    freadreal1(&crop->dh->runoffsum,swap,file);
    freadreal1(&crop->dh->n2o_denitsum,swap,file);
    freadreal1(&crop->dh->n2o_nitsum,swap,file);
    freadreal1(&crop->dh->n2_emissum,swap,file);
    freadreal1(&crop->dh->leachingsum,swap,file);
    freadreal1(&crop->dh->c_emissum,swap,file);
    freadreal1(&crop->dh->nfertsum,swap,file);
    freadint1(&crop->dh->sdate,swap,file);
    freadint1(&crop->dh->sowing_year,swap,file);
  }
  else
    crop->dh=NULL;
  return freadreal1(&crop->supplysum,swap,file)!=1;
} /* of 'fread_crop' */
