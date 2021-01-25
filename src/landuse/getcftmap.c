/**************************************************************************************/
/**                                                                                \n**/
/**           g  e  t  c  f  t  m  a  p  .  c                                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static int findpftname(const char *name,const Pftpar *pftpar,int ncft)
{
  int cft;
  for(cft=0;cft<ncft;cft++)
    if(!strcmp(name,pftpar[cft].name))
      return cft;
  return NOT_FOUND;
} /* of 'findpftname' */

int *getcftmap(LPJfile *file,       /**< pointer to LPJ config file */
               int *size,           /**< size of CFT map array */
               const char *name,
               Bool cftonly,
               int npft,            /**< numbert of natural PFTs */
               int ncft,            /**< number of crop PFTs */
               const Config *config /**< LPJ configuration */
              )                     /** \return CFT map array or NULL on error */
{
  static char *grasspft[NGRASS]={"others","grassland"};
  static char *biomasspft[NBIOMASSTYPE]={"biomass grass","biomass tree"};
  static char *wppft[1]={"woodplantation"};
  Bool *undef;
  LPJfile array,item;
  int *cftmap;
  Verbosity verbose;
  String s;
  int cft;
  Bool first=TRUE;
  if(iskeydefined(file,name))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    if(fscanarray(file,&array,size,FALSE,name,verbose))
      return NULL;
    cftmap=newvec(int,*size);
    if(cftmap==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    undef=newvec(Bool,cftonly ? ncft : getnirrig(ncft,config));
    if(undef==NULL)
    {
      printallocerr(name);
      return NULL;
    }
    if(cftonly)
      for(cft=0;cft<ncft;cft++)
        undef[cft]=TRUE;
    else
      for(cft=0;cft<getnirrig(ncft,config);cft++)
        undef[cft]=TRUE;
    for(cft=0;cft<*size;cft++)
    {
      fscanarrayindex(&array,&item,cft,verbose);

      if(fscanstring(&item,s,NULL,FALSE,verbose))
      {
        free(cftmap);
        return NULL;
      }
      cftmap[cft]=findpftname(s,config->pftpar+npft,ncft);
      if(cftmap[cft]!=NOT_FOUND)
      {
        undef[cftmap[cft]]=FALSE;
        continue;
      }
      if(!cftonly)
      {
        cftmap[cft]=findstr(s,grasspft,NGRASS);
        if(cftmap[cft]!=NOT_FOUND)
        {
          cftmap[cft]+=ncft;
          undef[cftmap[cft]]=FALSE;
          continue;
        }
        cftmap[cft]=findstr(s,biomasspft,NBIOMASSTYPE);
        if(cftmap[cft]!=NOT_FOUND)
        {
          cftmap[cft]+=ncft+NGRASS;
          undef[cftmap[cft]]=FALSE;
          continue;
        }
        if(config->nwptype)
        {
          cftmap[cft]=findstr(s,wppft,1);
          if(cftmap[cft]!=NOT_FOUND)
          {
            cftmap[cft]+=ncft+NGRASS-NBIOMASSTYPE;
            undef[cftmap[cft]]=FALSE;
            continue;
          }
        }
        if(config->nagtree)
        {
          cftmap[cft]=findpftname(s,config->pftpar+npft-config->nagtree,config->nagtree);
          if(cftmap[cft]!=NOT_FOUND)
          {
            cftmap[cft]+=ncft+NGRASS-NBIOMASSTYPE+config->nwptype;
            undef[cftmap[cft]]=FALSE;
            continue;
          }
        }
      }
      if(cftmap[cft]==NOT_FOUND)
      {
        if(verbose)
          fprintf(stderr,"ERROR241: Invalid CFT '%s' in '%s' array.\n",s,name);
        free(cftmap);
        free(undef);
        return NULL;
      }
    }
    if(isroot(*config))
    {
    for(cft=0;cft<ncft;cft++)
      if(undef[cft])
      {
        if(first)
        {
          fprintf(stderr,"WARNING010: '%s' not defined for",name); 
          first=FALSE;
        }
        fprintf(stderr," '%s'",config->pftpar[npft+cft].name);
      }
    if(!cftonly)
    {
      for(cft=0;cft<NGRASS;cft++)
        if(undef[cft+ncft])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: '%s' not defined for",name); 
            first=FALSE;
          }
          else
            fprintf(stderr,",");
          fprintf(stderr," '%s'",grasspft[cft]);
        }
      for(cft=0;cft<NGRASS;cft++)
        if(undef[cft+ncft])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: '%s' not defined for",name); 
            first=FALSE;
          }
          else
            fprintf(stderr,",");
          fprintf(stderr," '%s'",grasspft[cft]);
        }
      for(cft=0;cft<NBIOMASSTYPE;cft++)
        if(undef[cft+ncft+NGRASS])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: '%s' not defined for",name); 
            first=FALSE;
          }
          else
            fprintf(stderr,",");
          fprintf(stderr," '%s'",biomasspft[cft]);
        }
      if(config->nwptype && undef[cft+ncft+NGRASS+NBIOMASSTYPE])
      {
        if(first && isroot(*config))
        {
          fprintf(stderr,"WARNING010: '%s' not defined for",name); 
          first=FALSE;
        }
        else
          fprintf(stderr,",");
        fprintf(stderr," '%s'",wppft[0]);
      }
      for(cft=0;cft<config->nagtree;cft++)
        if(undef[cft+ncft+NGRASS+NBIOMASSTYPE+config->nwptype])
        {
          if(first)
          {
            fprintf(stderr,"WARNING010: '%s' not defined for",name); 
            first=FALSE;
          }
          else
            fprintf(stderr,",");
          fprintf(stderr," '%s'",config->pftpar[npft-config->nagtree+cft].name);
        }
    }
    if(!first)
      fprintf(stderr,".\n");
    }
    free(undef);
  }
  else
  {
    *size=(cftonly) ? ncft : getnirrig(ncft,config);
    cftmap=newvec(int,*size);
    if(cftmap==NULL)
    {
      printallocerr("cftmap");
      return NULL;
    }
    for(cft=0;cft<*size;cft++)
      cftmap[cft]=cft;
  }
  return cftmap;
} /* of 'getcftmap' */
