/**************************************************************************************/
/**                                                                                \n**/
/**                 f  s  c  a  n  a  g  t  r  e  e  m  a  p  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads agricultural tree mapping from LPJ configuration file       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *fscanagtreemap(LPJfile *file,       /**< pointer to LPJ config file */
                    const char *name,    /**< name of map */
                    int npft,            /**< number of natural PFTs */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return CFT map array or NULL on error */
{
  Bool *undef;
  LPJfile array,item;
  int *cftmap;
  Verbosity verbose;
  String s;
  int cft,size;
  if(iskeydefined(file,name))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    if(fscanarray(file,&array,&size,FALSE,name,verbose))
      return NULL;
    if(size!=config->nagtree)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR243: Size of map '%s'=%d does not match the number of agricultural trees %d.\n",
                name,size,config->nagtree);
      return NULL;
    }
    cftmap=newvec(int,config->nagtree);
    if(cftmap==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    undef=newvec(Bool,config->nagtree);
    if(undef==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    for(cft=0;cft<config->nagtree;cft++)
        undef[cft]=TRUE;
    for(cft=0;cft<config->nagtree;cft++)
    {
      fscanarrayindex(&array,&item,cft,verbose);
      if(isnull(&item,NULL))
      {
        cftmap[cft]=NOT_FOUND;
        continue;
      }
      if(!isstring(&item,NULL))
      {
        if(verbose)
          fprintf(stderr,"ERROR226: Datatype of element %d in map '%s' is not of type string.\n",
                  cft+1,name);
        free(cftmap);
        return NULL;
      }
      if(fscanstring(&item,s,NULL,FALSE,verbose))
      {
        free(cftmap);
        return NULL;
      }
      cftmap[cft]=findpftname(s,config->pftpar+npft-config->nagtree,config->nagtree);
      if(cftmap[cft]!=NOT_FOUND)
        undef[cftmap[cft]]=FALSE;
      else
      {
        if(verbose)
          fprintf(stderr,"ERROR244: Unknown agricultural tree \"%s\" in map '%s'.\n",s,name);
        free(cftmap);
        return NULL;
      }
    } /* of for(cft=0...) */
    for(cft=0;cft<config->nagtree;cft++)
      if(undef[cft])
      {
        if(isroot(*config))
           fprintf(stderr,"ERROR244: Map '%s' not defined for '%s'.\n",
                   name,config->pftpar[npft-config->nagtree+cft].name);
        free(cftmap);
        return NULL;
      }
    free(undef);
  }
  else
  {
    /* no map defined, set default one */
    cftmap=newvec(int,config->nagtree);
    if(cftmap==NULL)
    {
      printallocerr("cftmap");
      return NULL;
    }
    for(cft=0;cft<config->nagtree;cft++)
      cftmap[cft]=cft;
    if(isroot(*config))
    {
      fprintf(stderr,"WARNING011: Map '%s' not found, set to [",name);
      for(cft=0;cft<config->nagtree;cft++)
      {
        if(cft)
          fputc(',',stderr);
        fprintf(stderr,"\"%s\"",config->pftpar[npft-config->nagtree+cft].name);
      }
      fputs("].\n",stderr);
    }
  }
  return cftmap;
} /* of 'fscanagtreemap' */
