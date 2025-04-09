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

#define readreal2(file,key,var) if(bstruct_readreal(file,key,var)) { fprintf(stderr,"ERROR254: Cannot read %s for PFT '%s'.\n",key,pft->par->name); return TRUE;}

Bool freadpft(Bstruct file,          /**< pointer to restart file */
              Stand *stand,          /**< Stand pointer */
              Pft *pft,              /**< PFT variables to read */
              const Pftpar pftpar[], /**< PFT parameter array */
              int ntotpft,           /**< total number of PFTs */
              Bool separate_harvests
             )                       /** \return TRUE on error */
{
  int id;

  if(bstruct_readstruct(file,NULL))
    return TRUE;
  if(bstruct_readint(file,"id",&id))
    return TRUE;
  if(id>=ntotpft)
  {
    fprintf(stderr,"ERROR195: Invalid value %d for PFT index, must be in [0,%d].\n",id,ntotpft-1);
    return TRUE;
  }
  pft->par=pftpar+id;
  pft->stand=stand;
  if(bstruct_readstruct(file,"phen_gsi"))
  {
    fprintf(stderr,"ERROR254: Cannot read phen_gsi for PFT '%s'.\n",pft->par->name);
    return TRUE;
  }
  readreal2(file,"tmin",&pft->phen_gsi.tmin);
  readreal2(file,"tmax",&pft->phen_gsi.tmax);
  readreal2(file,"wscal",&pft->phen_gsi.wscal);
  readreal2(file,"light",&pft->phen_gsi.light);
  if(bstruct_readendstruct(file))
    return TRUE;
  readreal2(file,"wscal",&pft->wscal);
  readreal2(file,"wscal_mean",&pft->wscal_mean);
  readreal2(file,"vscal",&pft->vscal);
  readreal2(file,"aphen",&pft->aphen);
  readreal2(file,"phen",&pft->phen);
  /* write type-dependent PFT variables */
  if(pft->par->fread(file,pft,separate_harvests))
  {
    fprintf(stderr,"ERROR254: Cannot read PFT-specific data for '%s'.\n",
            pft->par->name);
    return TRUE;
  }
  if(freadstocks(file,"bm_inc",&pft->bm_inc))
    return TRUE;
  readreal2(file,"nind",&pft->nind);
  readreal2(file,"gdd",&pft->gdd);
  readreal2(file,"fpc",&pft->fpc);
  readreal2(file,"albdo",&pft->albedo);
  readreal2(file,"fapae",&pft->fapar);
  readreal2(file,"nleaf",&pft->nleaf);
  if(freadstocks(file,"establish",&pft->establish))
    return TRUE;
  if(bstruct_readint(file,"litter",&pft->litter))
    return TRUE;
  pft->vmax=0;
  pft->npp_bnf=0;
  return bstruct_readendstruct(file);
} /* of 'freadpft' */
