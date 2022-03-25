/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  s  o  i  l  m  a  p  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads soil map from LPJ configuration file                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int findsoilid(const char *name,       /**< name of soil type to find in array */
               const Soilpar *soilpar, /**< soil parameter array */
               int nsoil               /**< size of soil parameter array */
              )                        /** \return index in PFT array or NOT_FOUND */
{
  int s;
  for(s=0;s<nsoil;s++)
    if(!strcmp(name,soilpar[s].name))
      return s;
  return NOT_FOUND;
}  /* of 'findsoilid' */

int *fscansoilmap(LPJfile *file,       /**< pointer to LPJ config file */
                  int *size,           /**< size of soil map array */
                  const Config *config /**< LPJ configuration */
                 )                     /** \return soil map array or NULL on error */
{
  Bool *undef;
  int s,*soilmap;
  LPJfile array,item;
  String name;
  Verbosity verbose;
  Bool first;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(iskeydefined(file,"soilmap"))
  {
    if(fscanarray(file,&array,size,FALSE,"soilmap",verbose))
      return NULL;
    if(*size==0)
    {
      if(verbose)
        fprintf(stderr,"ERROR255: Size of 'soilmap' array must not be zero.\n");
      return NULL;
    }
    soilmap=newvec(int,*size);
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
    for(s=0;s<*size;s++)
    {
      fscanarrayindex(&array,&item,s,verbose);
      if(isnull(&item))
      {
        soilmap[s]=0;
        continue;
      }
      if(isstring(&item,NULL))
      {
        if(fscanstring(&item,name,NULL,FALSE,verbose))
        {
          free(soilmap);
          free(undef);
          return NULL;
        }
        soilmap[s]=findsoilid(name,config->soilpar,config->nsoil);
        if(soilmap[s]==NOT_FOUND)
        {
          if(verbose)
          {
            fprintf(stderr,"ERROR254: Soil type '%s' not in soil parameter array, must be in [",name);
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
      }
      else
      {
        if(!isint(&item,NULL))
        {
          if(verbose)
            fprintf(stderr,"ERROR226: Invalid datatype of element %d in 'soilmap', must be string or int.\n",s+1);
          free(soilmap);
          free(undef);
          return NULL;
        }
        if(fscanint(&item,soilmap+s,NULL,FALSE,verbose))
        {
          free(soilmap);
          free(undef);
          return NULL;
        }
        if(soilmap[s]<0 || soilmap[s]>=config->nsoil)
        {
          if(verbose)
            fprintf(stderr,"ERROR254: Invalid soil type %d in 'soilmap', must be in [0,%d].\n",
                    soilmap[s],config->nsoil-1);
          free(soilmap);
          free(undef);
          return NULL;
        }
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
            fprintf(stderr,"WARNING036: 'soilmap' not defined for soil type");
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
  }
  else
  {
    if(verbose)
      fprintf(stderr,"ERROR255: Map 'soilmap' not found.\n");
    soilmap=NULL;
  }
  return soilmap;
} /* of 'fscansoilmap' */
