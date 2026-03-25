/**************************************************************************************/
/**                                                                                \n**/
/**           d  e  f  a  u  l  t  p  f  t  m  a  p  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function sets default PFT map                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *defaultpftmap(const char *name,    /**< name of map */
                   int npft,            /**< number of natural PFTs */
                   const Config *config /**< LPJ configuration */
                  )                     /** \return PFT map array or NULL on error */
{
  int pft;
  int *pftmap;
  pftmap=newvec(int,npft);
  if(pftmap==NULL)
  {
    printallocerr("pftmap");
    return NULL;
  }
  for(pft=0;pft<npft;pft++)
    pftmap[pft]=pft;
  if(isroot(*config))
  {
    fprintf(stderr,"WARNING011: Map '%s' not found, set to [",name);
    for(pft=0;pft<npft;pft++)
    {
      if(pft)
        fputc(',',stderr);
      fprintf(stderr,"\"%s\"",config->pftpar[pft].name);
    }
    fputs("].\n",stderr);
  }
  return pftmap;
} /* of 'defaultpftmap' */
