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

Bool fwrite_crop(FILE *file, /**< pointer to file data */
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
  fwrite1(&crop->ind,sizeof(Cropphys2),file);
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
  fwrite1(&crop->frostkill,sizeof(int),file);
  fwrite1(&crop->supplysum,sizeof(Real),file);
#ifdef DOUBLE_HARVEST
  fwrite1(&crop->petsum,sizeof(Real),file);
  fwrite1(&crop->evapsum,sizeof(Real),file);
  fwrite1(&crop->transpsum,sizeof(Real),file);
  fwrite1(&crop->intercsum,sizeof(Real),file);
  fwrite1(&crop->precsum,sizeof(Real),file);
  fwrite1(&crop->sradsum,sizeof(Real),file);
  fwrite1(&crop->irrig_apply,sizeof(Real),file);
  fwrite1(&crop->tempsum,sizeof(Real),file);
  fwrite1(&crop->nirsum,sizeof(Real),file);
  fwrite1(&crop->lgp,sizeof(Real),file);
  fwrite1(&crop->sdate,sizeof(int),file);
  fwrite1(&crop->sowing_year,sizeof(int),file);
#endif
  return FALSE;
} /* of 'fwrite_crop' */
