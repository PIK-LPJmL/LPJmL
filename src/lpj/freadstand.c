/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  s  t  a  n  d  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads stand data from file                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Stand *freadstand(Bstruct file, /**< File pointer to restart file */
                  const char *name,    /**< name of object */
                  Cell *cell, /**< Cell pointer */
                  const Pftpar pftpar[],/**< Pft parameter array */
                  int ntotpft,          /**<  total number of PFTs */
                  const Soilpar *soilpar, /**< soil parameter */
                  const Standtype standtype[], /**< array of stand types */
                  int nstand, /**< number of stand types */
                  Bool separate_harvests
                 ) /** \return allocated stand data or NULL */
{
  Stand *stand;
  Byte landusetype;
  if(bstruct_readstruct(file,name))
    return NULL;
  stand=new(Stand);
  if(stand==NULL)
  {
    printallocerr("stand");
    return NULL;
  }
  stand->cell=cell;
  if(bstruct_readbyte(file,"landusetype",&landusetype))
  {
    free(stand);
    return NULL;
  }
  if(landusetype>=nstand)
  {
    fprintf(stderr,"ERROR196: Invalid value %d for stand type, must be in [0,%d].\n",
            landusetype,nstand-1);
    free(stand);
    return NULL;
  }
  stand->type=standtype+landusetype;
  if(freadpftlist(file,"pftlist",stand,&stand->pftlist,pftpar,ntotpft,separate_harvests))
  {
    fprintf(stderr,"ERROR254: Cannot read PFT list for %s stand.\n",stand->type->name);
    free(stand);
    return NULL;
  }
  initstand(stand);
  if(freadsoil(file,"soil",&stand->soil,soilpar,pftpar,ntotpft))
  {
    fprintf(stderr,"ERROR254: Cannot read soil data for %s stand.\n",stand->type->name);
    free(stand);
    return NULL;
  }
  if(bstruct_readreal(file,"Hag_Beta",&stand->Hag_Beta))
  {
    fprintf(stderr,"ERROR254: Cannot read Hag_Beta for %s stand.\n",stand->type->name);
    free(stand);
    return NULL;
  }
  if(bstruct_readreal(file,"slope_mean",&stand->slope_mean))
  {
    fprintf(stderr,"ERROR254: Cannot read slope_mean for %s stand.\n",stand->type->name);
    free(stand);
    return NULL;
  }
  if(bstruct_readreal(file,"frac",&stand->frac))
  {
    fprintf(stderr,"ERROR254: Cannot read stand fraction for %s stand.\n",stand->type->name);
    free(stand);
    return NULL;
  }
  stand->data=NULL;
  /* read stand-specific data */
  if(stand->type->fread(file,stand))
  {
    fprintf(stderr,"ERROR254: Cannot read stand-specific data for %s stand.\n",stand->type->name);
    freestand(stand);
    return NULL;
  }
  if(bstruct_readrealarray(file,"frac_g",stand->frac_g,NSOILLAYER))
  {
    fprintf(stderr,"ERROR254: Cannot read frac_g array for %s stand.\n",stand->type->name);
    freestand(stand);
    return NULL;
  }
  if(bstruct_readendstruct(file,name))
  {
    freestand(stand);
    return NULL;
  }
  return stand;
} /* of 'freadstand' */
