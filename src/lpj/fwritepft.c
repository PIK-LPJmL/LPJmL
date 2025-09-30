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


Bool fwritepft(Bstruct file,  /**< File pointer to binary file */
               const Pft *pft /**< PFT variables */
              )               /** \return TRUE on error */
{
  bstruct_writebeginstruct(file,NULL);
  bstruct_writeint(file,"id",pft->par->id);
  bstruct_writebeginstruct(file,"phen_gsi");
  bstruct_writereal(file,"tmin",pft->phen_gsi.tmin);
  bstruct_writereal(file,"tmax",pft->phen_gsi.tmax);
  bstruct_writereal(file,"wscal",pft->phen_gsi.wscal);
  bstruct_writereal(file,"light",pft->phen_gsi.light);
  bstruct_writeendstruct(file);
  bstruct_writereal(file,"wscal",pft->wscal);
  bstruct_writereal(file,"wscal_mean",pft->wscal_mean);
  bstruct_writereal(file,"vscal",pft->vscal);
  bstruct_writereal(file,"aphen",pft->aphen);
  bstruct_writereal(file,"phen",pft->phen);
  bstruct_writeint(file,"litter_id",pft->litter);
  /* write type-dependent PFT variables */
  if(pft->par->fwrite(file,pft))
    return TRUE;
  fwritestocks(file,"bm_inc",&pft->bm_inc);
  bstruct_writereal(file,"nind",pft->nind);
  bstruct_writereal(file,"gdd",pft->gdd);
  bstruct_writereal(file,"fpc",pft->fpc);
  bstruct_writereal(file,"albedo",pft->albedo);
  bstruct_writereal(file,"fapar",pft->fapar);
  bstruct_writereal(file,"nleaf",pft->nleaf);
  fwritestocks(file,"establish",&pft->establish);
  return bstruct_writeendstruct(file);
} /* of 'fwritepft' */
