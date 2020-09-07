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
               int npft,            /**< numbert of natural PFTs */
               int ncft,            /**< number of crop PFTs */
               const Config *config /**< LPJ configuration */
              )                     /** \return CFT map array or NULL on error */
{
  static char *grasspft[NGRASS]={"others","grassland"};
  LPJfile array,item;
  int *cftmap;
  Verbosity verbose;
  String s;
  int cft;
  if(iskeydefined(file,"cftmap"))
  {
    verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
    if(fscanarray(file,&array,size,FALSE,"cftmap",verbose))
      return NULL;
    cftmap=newvec(int,*size);
    if(cftmap==NULL)
    {
      printallocerr("cftmap");
      return NULL;
    }
    for(cft=0;cft<*size;cft++)
    {
      fscanarrayindex(&array,&item,cft,verbose);

      if(fscanstring(&item,s,NULL,FALSE,verbose))
      {
        free(cftmap);
        return NULL;
      }
      cftmap[cft]=findstr(s,grasspft,NGRASS);
      if(cftmap[cft]==NOT_FOUND)
      {
        cftmap[cft]=findpftname(s,config->pftpar+npft,ncft);
        if(cftmap[cft]==NOT_FOUND)
        {
          if(verbose)
            fprintf(stderr,"ERROR241: CFT '%s' not found in CFT list.\n",s);
          free(cftmap);
          return NULL;
        }
      }
      else
        cftmap[cft]+=ncft;
    }
  }
  else
  {
    *size=ncft+NGRASS;
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
