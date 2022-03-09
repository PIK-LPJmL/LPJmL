/**************************************************************************************/
/**                                                                                \n**/
/**               d  e  f  a  u  l  t  s  o  i  l  m  a  p  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function sets default 1:1 soil map from LPJ configuration file             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *defaultsoilmap(int *size,           /**< size of soil map array */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return soil map array or NULL on error */
{
  int s,*soilmap;
  if(isroot(*config))
    fprintf(stderr,"WARNING011: Map 'soilmap' not found, default 1:1 mapping assumed.\n");
  *size=config->nsoil+1;
  soilmap=newvec(int,*size);
  if(soilmap==NULL)
  {
    printallocerr("soilmap");
    return NULL;
  }
  for(s=0;s<=config->nsoil;s++)
    soilmap[s]=s;
  return soilmap;
} /* of 'defaultsoilmap' */
