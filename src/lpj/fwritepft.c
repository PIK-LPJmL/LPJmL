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
  writestruct(file,NULL);
  writeint(file,"id",pft->par->id);
  writestruct(file,"phen_gsi");
  writereal(file,"tmin",pft->phen_gsi.tmin);
  writereal(file,"tmax",pft->phen_gsi.tmax);
  writereal(file,"wscal",pft->phen_gsi.wscal);
  writereal(file,"light",pft->phen_gsi.light);
  writeendstruct(file);
  writereal(file,"wscal",pft->wscal);
  writereal(file,"wscal_mean",pft->wscal_mean);
  writereal(file,"vscal",pft->vscal);
  writereal(file,"aphen",pft->aphen);
  writereal(file,"phen",pft->phen);
  /* write type-dependent PFT variables */
  if(pft->par->fwrite(file,pft))
    return TRUE;
  writestocks(file,"bm_inc",&pft->bm_inc);
  writereal(file,"nind",pft->nind);
  writereal(file,"gdd",pft->gdd);
  writereal(file,"fpc",pft->fpc);
  writereal(file,"albdo",pft->albedo);
  writereal(file,"fapae",pft->fapar);
  writereal(file,"nleaf",pft->nleaf);
  writestocks(file,"establish",&pft->establish);
  writeint(file,"litter",pft->litter);
  return writeendstruct(file);
} /* of 'fwritepft' */
