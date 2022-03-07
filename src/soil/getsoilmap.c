/**************************************************************************************/
/**                                                                                \n**/
/**               g  e  t  s  o  i  l  m  a  p  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads soil map from String array                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *getsoilmap(List *map,           /**< pointer to String array */
                const Config *config /**< LPJ configuration */
               )                     /** \return soil map array or NULL on error */
{
  Bool *undef;
  int s,*soilmap;
  Verbosity verbose;
  Bool first;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(isempty(map))
  {
    if(verbose)
      fprintf(stderr,"ERROR255: Size of 'map' array in '%s' must not be zero.\n",
             config->soil_filename.name);
    return NULL;
  }
  soilmap=newvec(int,getlistlen(map));
  if(soilmap==NULL)
  {
    printallocerr("soilmap");
    return NULL;
  }
  undef=newvec(Bool,config->nsoil);
  if(undef==NULL)
  {
    printallocerr("undef");
    free(soilmap);
    return NULL;
  }
  for(s=0;s<config->nsoil;s++)
    undef[s]=TRUE;
  for(s=0;s<getlistlen(map);s++)
  {
    if(getlistitem(map,s)==NULL)
    {
      soilmap[s]=0;
      continue;
    }
    soilmap[s]=findsoilid(getlistitem(map,s),config->soilpar,config->nsoil);
    if(soilmap[s]==NOT_FOUND)
    {
      if(verbose)
      {
        fprintf(stderr,"ERROR254: Soil type '%s' not in map in '%s', must be in [",
                (char *)getlistitem(map,s),config->soil_filename.name);
        for(s=0;s<config->nsoil;s++)
        {
          fprintf(stderr,"'%s'",config->soilpar[s].name);
          if(s<config->nsoil-1)
            fprintf(stderr,",");
        }
        fprintf(stderr,"].\n");
      }
      free(undef);
      free(soilmap);
      return NULL;
    }
    undef[soilmap[s]]=FALSE;
    soilmap[s]++;
  }
  if(verbose)
  {
    first=TRUE;
    for(s=0;s<config->nsoil;s++)
      if(undef[s])
      {
        if(first)
        {
          fprintf(stderr,"WARNING036: 'map' not defined in '%s' for soil type",
                  config->soil_filename.name);
          first=FALSE;
        }
        else
          fputc(',',stderr);
        fprintf(stderr," '%s'",config->soilpar[s].name);
      }
    if(!first)
      fprintf(stderr,".\n");
  }
  free(undef);
  return soilmap;
} /* of 'getsoilmap' */
