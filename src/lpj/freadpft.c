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

#define readreal2(file,key,var,swap) if(readreal(file,key,var,swap)) { fprintf(stderr,"ERROR254: Cannot read %s for PFT '%s'.\n",key,pft->par->name); return TRUE;}

Bool freadpft(FILE *file,            /**< pointer to restart file */
              Stand *stand,          /**< Stand pointer */
              Pft *pft,              /**< PFT variables to read */
              const Pftpar pftpar[], /**< PFT parameter array */
              int ntotpft,           /**< total number of PFTs */
              Bool separate_harvests,
              Bool swap              /**< Byte order has to be changed
                                        (TRUE/FALSE) */
             )                       /** \return TRUE on error */
{
  int id;

  if(readstruct(file,NULL,swap))
    return TRUE;
  if(readint(file,"id",&id,swap))
    return TRUE;
  if(id>=ntotpft)
  {
    fprintf(stderr,"ERROR195: Invalid value %d for PFT index, must be in [0,%d].\n",id,ntotpft-1);
    return TRUE;
  }
  pft->par=pftpar+id;
  pft->stand=stand;
  if(readstruct(file,"phen_gsi",swap))
  {
    fprintf(stderr,"ERROR254: Cannot read phen_gsi for PFT '%s'.\n",pft->par->name);
    return TRUE;
  }
  readreal2(file,"tmin",&pft->phen_gsi.tmin,swap);
  readreal2(file,"tmax",&pft->phen_gsi.tmax,swap);
  readreal2(file,"wscal",&pft->phen_gsi.wscal,swap);
  readreal2(file,"light",&pft->phen_gsi.light,swap);
  if(readendstruct(file))
    return TRUE;
  readreal2(file,"wscal",&pft->wscal,swap);
  readreal2(file,"wscal_mean",&pft->wscal_mean,swap);
  readreal2(file,"vscal",&pft->vscal,swap);
  readreal2(file,"aphen",&pft->aphen,swap);
  readreal2(file,"phen",&pft->phen,swap);
  /* write type-dependent PFT variables */
  if(pft->par->fread(file,pft,separate_harvests,swap))
  {
    fprintf(stderr,"ERROR254: Cannot read PFT-specific data for '%s'.\n",
            pft->par->name);
    return TRUE;
  }
  if(readstocks(file,"bm_inc",&pft->bm_inc,swap))
    return TRUE;
  readreal2(file,"nind",&pft->nind,swap);
  readreal2(file,"gdd",&pft->gdd,swap);
  readreal2(file,"fpc",&pft->fpc,swap);
  readreal2(file,"albdo",&pft->albedo,swap);
  readreal2(file,"fapae",&pft->fapar,swap);
  readreal2(file,"nleaf",&pft->nleaf,swap);
  if(readstocks(file,"establish",&pft->establish,swap))
    return TRUE;
  if(readint(file,"litter",&pft->litter,swap))
    return TRUE;
  pft->vmax=0;
  pft->npp_bnf=0;
  return readendstruct(file);
} /* of 'freadpft' */
