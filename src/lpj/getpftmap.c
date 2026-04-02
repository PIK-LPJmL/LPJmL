/**************************************************************************************/
/**                                                                                \n**/
/**                    g  e  t  p  f  t  m  a  p  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets PFT mapping from string array                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *getpftmap(const Map *map,      /**< pointer to string array  */
               const char *name,    /**< name for map */
               int npft,            /**< number of natural PFTs */
               const Config *config /**< LPJ configuration */
              )                     /** \return PFT map array or NULL on error */
{
  Bool *undef;
  int *pftmap;
  int pft;
  Bool first=TRUE;
  if(map->isfloat)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR255: Map '%s' must be of type string, not float.\n",name);
    return NULL;
  }
  pftmap=newvec(int,getmapsize(map));
  if(pftmap==NULL)
  {
    printallocerr("pftmap");
    return NULL;
  }
  undef=newvec(Bool,npft);
  if(undef==NULL)
  {
    printallocerr("undef");
    return NULL;
  }
  for(pft=0;pft<npft;pft++)
     undef[pft]=TRUE;
  for(pft=0;pft<getmapsize(map);pft++)
  {
    if(getmapitem(map,pft)==NULL)
    {
      pftmap[pft]=NOT_FOUND;
      continue;
    }
    pftmap[pft]=findpftname(getmapitem(map,pft),config->pftpar,npft);
    if(pftmap[pft]!=NOT_FOUND)
    {
      undef[pftmap[pft]]=FALSE;
      continue;
    }
    if(pftmap[pft]==NOT_FOUND)
    {
      if(isroot(*config))
        fprintf(stderr,"WARNING011: Unknown PFT \"%s\" in map '%s', will be ignored.\n",
                (char *)getmapitem(map,pft),name);
    }
  } /* of for(pft=0...) */
  if(isroot(*config))
  {
    for(pft=0;pft<npft;pft++)
      if(undef[pft])
      {
        if(first)
        {
          fprintf(stderr,"WARNING010: Map in '%s' not defined for",name);
          first=FALSE;
        }
        else
          fputc(',',stderr);
        fprintf(stderr," \"%s\"",config->pftpar[pft].name);
      }
    if(!first)
      fprintf(stderr,", set to zero.\n");
  } /* of isroot(*config) */
  free(undef);
  return pftmap;
} /* of 'getpftmap' */
