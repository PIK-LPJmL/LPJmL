/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  a  d  p  f  t  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads PFT data from file                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadpft(FILE *file,            /**< pointer to binary file */
              Stand *stand,          /**< Stand pointer */
              Pft *pft,              /**< PFT variables to read */
              const Pftpar pftpar[], /**< PFT parameter array */
              int ntotpft,           /**< total number of PFTs */
              Bool separate_harvests,
              Bool swap              /**< Byte order has to be changed
                                        (TRUE/FALSE) */
             )                       /** \return TRUE on error */
{
  Byte id;


  if(fread(&id,sizeof(id),1,file)!=1)
    return TRUE;
  if(id>=ntotpft)
  {
    fprintf(stderr,"ERROR195: Invalid value %d for PFT index, must be in [0,%d].\n",id,ntotpft-1);
    return TRUE;
  }
  pft->par=pftpar+id;
  pft->stand=stand;
  freadreal((Real *)(&pft->phen_gsi),sizeof(Phenology)/sizeof(Real),swap,file);
  freadreal1(&pft->wscal,swap,file);
  freadreal1(&pft->wscal_mean,swap,file);
  freadreal1(&pft->vscal,swap,file);
  freadreal1(&pft->aphen,swap,file);
  freadreal1(&pft->phen,swap,file);
  /* read class-dependent PFT variables */
  if(pft->par->fread(file,pft,separate_harvests,swap))
  {
    fprintf(stderr,"ERROR254: Cannot read PFT-specific data.\n");
    return TRUE;
  }
  freadreal((Real *)&pft->bm_inc,sizeof(Stocks)/sizeof(Real),swap,file);
  freadreal1(&pft->nind,swap,file);
  freadreal1(&pft->gdd,swap,file);
  freadreal1(&pft->fpc,swap,file);
  freadreal1(&pft->albedo,swap,file);
  freadreal1(&pft->fapar,swap,file);
  freadreal1(&pft->nleaf,swap,file);
  freadreal((Real *)&pft->establish,sizeof(Stocks)/sizeof(Real),swap,file);
  pft->vmax=0;
  pft->npp_bnf=0;
  if(fread(&id,sizeof(id),1,file)!=1)
    return TRUE;
  pft->litter=id;
  return FALSE;
} /* of 'freadpft' */
