/**************************************************************************************/
/**                                                                                \n**/
/**             f  w  r  i  t  e  _  c  r  o  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes crop specific variables into binary file                   \n**/
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

Bool fwrite_crop(FILE *file,    /**< pointer to file data */
                 const Pft *pft /**< pointer to PFT data */
                )               /** \return TRUE on error */
{
  const Pftcrop *crop;
  crop=pft->data;
  fwrite1(&pft->nlimit,sizeof(Real),file);
  fwrite1(&crop->wtype,sizeof(int),file);
  fwrite1(&crop->growingdays,sizeof(int),file);
  fwrite1(&crop->pvd,sizeof(Real),file);
  fwrite1(&crop->phu,sizeof(Real),file);
  fwrite1(&crop->basetemp,sizeof(Real),file);
  fwrite1(&crop->senescence,sizeof(int),file);
  fwrite1(&crop->senescence0,sizeof(int),file);
  fwrite1(&crop->husum,sizeof(Real),file);
  fwrite1(&crop->vdsum,sizeof(Real),file);
  fwrite1(&crop->fphu,sizeof(Real),file);
  fwrite1(&crop->ind.leaf.carbon,sizeof(Real),file);
  fwrite1(&crop->ind.leaf.nitrogen,sizeof(Real),file);
  fwrite1(&crop->ind.root.carbon,sizeof(Real),file);
  fwrite1(&crop->ind.root.nitrogen,sizeof(Real),file);
  fwrite1(&crop->ind.pool.carbon,sizeof(Real),file);
  fwrite1(&crop->ind.pool.nitrogen,sizeof(Real),file);
  fwrite1(&crop->ind.so.carbon,sizeof(Real),file);
  fwrite1(&crop->ind.so.nitrogen,sizeof(Real),file);
  fwrite1(&crop->flaimax,sizeof(Real),file);
  fwrite1(&crop->lai,sizeof(Real),file);
  fwrite1(&crop->lai000,sizeof(Real),file);
  fwrite1(&crop->laimax_adjusted,sizeof(Real),file);
  fwrite1(&crop->lai_nppdeficit,sizeof(Real),file);
  fwrite1(&crop->demandsum,sizeof(Real),file);
  fwrite1(&crop->ndemandsum,sizeof(Real),file);
  fwrite1(&crop->nuptakesum,sizeof(Real),file);
  fwrite1(&crop->nfertilizer,sizeof(Real),file);
  fwrite1(&crop->nmanure,sizeof(Real),file);
  fwrite1(&crop->vscal_sum,sizeof(Real),file);
  if(crop->sh!=NULL)
  {
    fwrite1(&crop->sh->petsum,sizeof(Real),file);
    fwrite1(&crop->sh->evapsum,sizeof(Real),file);
    fwrite1(&crop->sh->transpsum,sizeof(Real),file);
    fwrite1(&crop->sh->intercsum,sizeof(Real),file);
    fwrite1(&crop->sh->precsum,sizeof(Real),file);
    fwrite1(&crop->sh->sradsum,sizeof(Real),file);
    fwrite1(&crop->sh->irrig_apply,sizeof(Real),file);
    fwrite1(&crop->sh->tempsum,sizeof(Real),file);
    fwrite1(&crop->sh->nirsum,sizeof(Real),file);
    fwrite1(&crop->sh->lgp,sizeof(Real),file);
    fwrite1(&crop->sh->runoffsum,sizeof(Real),file);
    fwrite1(&crop->sh->n2o_denitsum,sizeof(Real),file);
    fwrite1(&crop->sh->n2o_nitsum,sizeof(Real),file);
    fwrite1(&crop->sh->n2_emissum,sizeof(Real),file);
    fwrite1(&crop->sh->leachingsum,sizeof(Real),file);
    fwrite1(&crop->sh->c_emissum,sizeof(Real),file);
    fwrite1(&crop->sh->nfertsum,sizeof(Real),file);
    fwrite1(&crop->sh->sdate,sizeof(int),file);
    fwrite1(&crop->sh->sowing_year,sizeof(int),file);
  }
  fwrite1(&crop->supplysum,sizeof(Real),file);
  return FALSE;
} /* of 'fwrite_crop' */
