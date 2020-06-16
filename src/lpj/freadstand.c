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

Stand *freadstand(FILE *file, /**< File pointer to binary file */
                  Cell *cell, /**< Cell pointer */
                  const Pftpar pftpar[],/**< Pft parameter array */
                  int ntotpft,          /**<  total number of PFTs */
                  const Soilpar *soilpar, /**< soil parameter */
                  const Standtype standtype[], /**< array of stand types */
                  int nstand, /**< number of stand types */
                  Bool swap /**< Byte order has to be changed (TRUE/FALSE) */
                 ) /** \return allocated stand data or NULL */
{
  Stand *stand;
  Byte landusetype;
  stand=new(Stand);
  if(stand==NULL)
  {
    printallocerr("stand");
    return NULL;
  }
  stand->cell=cell;
  if(freadpftlist(file,stand,&stand->pftlist,pftpar,ntotpft,swap))
  {
    free(stand);
    return NULL;
  }
  initstand(stand);
  if(freadsoil(file,&stand->soil,soilpar,pftpar,ntotpft,swap))
  {
    free(stand);
    return NULL;
  }
  freadreal1(&stand->frac,swap,file);
  if(fread(&landusetype,sizeof(landusetype),1,file)!=1)
  {
    freepftlist(&stand->pftlist);
    freesoil(&stand->soil);
    free(stand);
    return NULL;
  }
  if(landusetype>=nstand)
  {
    fprintf(stderr,"ERROR196: Invalid value %d for stand type.\n",
            landusetype);
    freepftlist(&stand->pftlist);
    freesoil(&stand->soil);
    free(stand);
    return NULL;
  }
  stand->type=standtype+landusetype;
  /* read stand-specific data */
  if(stand->type->fread(file,stand,swap))
  {
    freestand(stand);
    return NULL;
  }
  if(freadreal(stand->frac_g,NSOILLAYER,swap,file)!=NSOILLAYER)
  {
    freestand(stand);
    return NULL;
  }
  return stand;
} /* of 'freadstand' */
