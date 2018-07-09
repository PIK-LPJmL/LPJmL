/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  p  f  t  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/*      Function writes PFT variables into binary file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"


Bool fwritepft(FILE *file,    /**< File pointer to binary file */
               const Pft *pft /**< PFT variables */
              )               /** \return TRUE on error */
{
  Byte b;
  b=(Byte)pft->par->id;
  fwrite1(&b,sizeof(b),file);
  fwrite(&pft->phen_gsi,sizeof(Phenology),1,file);
  fwrite1(&pft->wscal,sizeof(Real),file);
  fwrite1(&pft->wscal_mean,sizeof(Real),file);
  fwrite1(&pft->aphen,sizeof(Real),file);
  fwrite1(&pft->phen,sizeof(Real),file);
  /* write type-dependent PFT variables */
  if(pft->par->fwrite(file,pft))
    return TRUE;
  fwrite1(&pft->bm_inc,sizeof(Real),file);
  fwrite1(&pft->nind,sizeof(Real),file);
  fwrite1(&pft->gdd,sizeof(Real),file); 
  fwrite1(&pft->fpc,sizeof(Real),file);
  fwrite1(&pft->albedo,sizeof(Real),file);
  fwrite1(&pft->fapar,sizeof(Real),file);
  b=(Byte)pft->litter;
  fwrite1(&b,sizeof(b),file);
  return FALSE;
} /* of 'fwritepft' */
