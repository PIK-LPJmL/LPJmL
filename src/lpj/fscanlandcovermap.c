/**************************************************************************************/
/**                                                                                \n**/
/**           f  s  c  a  n  l  a  n  d  c  o  v  e  r  m  a  p  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads landcover mapping from LPJ configuration file               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *fscanlandcovermap(LPJfile *file,       /**< pointer to LPJ config file */
                       int *size,           /**< size of PFT map array */
                       const char *name,    /**< name of map */
                       int npft,            /**< number of natural PFTs */
                       const Config *config /**< LPJ configuration */
                      )                     /** \return PFT map array or NULL on error */
{
  Bool *undef;
  LPJfile *array,*item;
  int *pftmap;
  Verbosity verbose;
  const char *s;
  int pft,nnat;
  Bool first;
  nnat=getnnat(npft,config);
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(iskeydefined(file,name))
  {
    array=fscanarray(file,size,name,verbose);
    if(array==NULL)
      return NULL;
    pftmap=newvec(int,*size);
    if(pftmap==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    undef=newvec(Bool,nnat);
    if(undef==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    for(pft=0;pft<nnat;pft++)
        undef[pft]=TRUE;
    for(pft=0;pft<*size;pft++)
    {
      item=fscanarrayindex(array,pft);
      if(isnull(item,NULL))
      {
        pftmap[pft]=NOT_FOUND;
        continue;
      }
      if(!isstring(item,NULL))
      {
        if(verbose)
          fprintf(stderr,"ERROR226: Datatype of element %d in map '%s' is not of type string.\n",
                  pft+1,name);
        free(pftmap);
        return NULL;
      }
      s=fscanstring(item,NULL,NULL,verbose);
      if(s==NULL)
      {
        free(pftmap);
        return NULL;
      }
      pftmap[pft]=findpftname(s,config->pftpar,nnat);
      if(pftmap[pft]!=NOT_FOUND)
        undef[pftmap[pft]]=FALSE;
      else
      {
        if(verbose)
          fprintf(stderr,"ERROR244: Unknown PFT \"%s\" in map '%s'.\n",s,name);
        free(pftmap);
        return NULL;
      }
    } /* of for(pft=0...) */
    if(verbose)
    {
      first=TRUE;
      for(pft=0;pft<nnat;pft++)
        if(undef[pft])
        {
          if(first)
          {
            fprintf(stderr,"ERROR244: Map '%s' not defined for ",name);
            first=FALSE;
          }
          else
             fputs(", ",stderr);
          fprintf(stderr,"'%s'",config->pftpar[pft].name);
        }
      if(!first)
        fputs(".\n",stderr);
    }
    free(undef);
  }
  else
  {
    /* no map defined, set default one */
    *size=nnat;
    pftmap=newvec(int,nnat);
    if(pftmap==NULL)
    {
      printallocerr("pftmap");
      return NULL;
    }
    for(pft=0;pft<nnat;pft++)
      pftmap[pft]=pft;
    if(verbose)
    {
      fprintf(stderr,"WARNING011: Map '%s' not found, set to [",name);
      for(pft=0;pft<nnat;pft++)
      {
        if(pft)
          fputc(',',stderr);
        fprintf(stderr,"\"%s\"",config->pftpar[pft].name);
      }
      fputs("].\n",stderr);
    }
  }
  return pftmap;
} /* of 'fscanlandcovermap' */
