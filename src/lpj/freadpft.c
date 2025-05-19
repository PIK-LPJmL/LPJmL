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

#define readreal(file,key,var) if(bstruct_readreal(file,key,var)) { fprintf(stderr,"ERROR254: Cannot read %s for PFT '%s'.\n",key,pft->par->name); return TRUE;}
#define readint(file,key,var) if(bstruct_readint(file,key,var)) { fprintf(stderr,"ERROR254: Cannot read %s for PFT '%s'.\n",key,pft->par->name); return TRUE;}

static Bool freadphen_gsi(Bstruct file,const char *name,Phenology *phen_gsi)
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  if(bstruct_readreal(file,"tmin",&phen_gsi->tmin))
    return TRUE;
  if(bstruct_readreal(file,"tmax",&phen_gsi->tmax))
    return TRUE;
  if(bstruct_readreal(file,"wscal",&phen_gsi->wscal))
    return TRUE;
  if(bstruct_readreal(file,"light",&phen_gsi->light))
    return TRUE;
  return bstruct_readendstruct(file,"phen_gsi");
} /* of 'freadphen_gsi' */

Bool freadpft(Bstruct file,          /**< pointer to restart file */
              Stand *stand,          /**< Stand pointer */
              Pft *pft,              /**< PFT variables to read */
              const Pftpar pftpar[], /**< PFT parameter array */
              int ntotpft,           /**< total number of PFTs */
              Bool separate_harvests
             )                       /** \return TRUE on error */
{
  int id;

  if(bstruct_readbeginstruct(file,NULL))
    return TRUE;
  if(bstruct_readint(file,"id",&id))
    return TRUE;
  if(id<0 || id>=ntotpft)
  {
    fprintf(stderr,"ERROR195: Invalid value %d for PFT index, must be in [0,%d].\n",id,ntotpft-1);
    return TRUE;
  }
  pft->par=pftpar+id;
  pft->stand=stand;
  if(freadphen_gsi(file,"phen_gsi",&pft->phen_gsi))
  {
    fprintf(stderr,"ERROR254: Cannot read phen_gsi for PFT '%s'.\n",pft->par->name);
    return TRUE;
  }
  readreal(file,"wscal",&pft->wscal);
  readreal(file,"wscal_mean",&pft->wscal_mean);
  readreal(file,"vscal",&pft->vscal);
  readreal(file,"aphen",&pft->aphen);
  readreal(file,"phen",&pft->phen);
  readint(file,"litter_id",&pft->litter);
  if(pft->litter<0 || pft->litter>=stand->soil.litter.n)
  {
    fprintf(stderr,"ERROR195: Invalid value %d for litter index, must be in [0,%d].\n",
            pft->litter,stand->soil.litter.n-1);
    return TRUE;
  }
  /* write type-dependent PFT variables */
  if(pft->par->fread(file,pft,separate_harvests))
  {
    fprintf(stderr,"ERROR254: Cannot read PFT-specific data for '%s'.\n",
            pft->par->name);
    return TRUE;
  }
  if(freadstocks(file,"bm_inc",&pft->bm_inc))
  {
    fprintf(stderr,"ERROR254: Cannot read bm_inc for PFT '%s'.\n",pft->par->name);
    return TRUE;
  }
  readreal(file,"nind",&pft->nind);
  readreal(file,"gdd",&pft->gdd);
  readreal(file,"fpc",&pft->fpc);
  readreal(file,"albdo",&pft->albedo);
  readreal(file,"fapar",&pft->fapar);
  readreal(file,"nleaf",&pft->nleaf);
  if(freadstocks(file,"establish",&pft->establish))
  {
    fprintf(stderr,"ERROR254: Cannot read establish for PFT '%s'.\n",pft->par->name);
    return TRUE;
  }
  pft->vmax=0;
  pft->npp_bnf=0;
  return bstruct_readendstruct(file,NULL);
} /* of 'freadpft' */
