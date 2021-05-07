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

static int findsoilid(const char *name,       /**< name of soil type to find in array */
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
  int s,*soilmap;
  LPJfile array,item;
  String name;
  Verbosity verbose;
  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;
  if(fscanarray(file,&array,size,FALSE,"soilmap",verbose))
    return NULL;
  soilmap=newvec(int,*size);
  if(soilmap==NULL)
  {
    printallocerr("soilmap");
    return NULL;
  } 
  for(s=0;s<*size;s++)
  {
    fscanarrayindex(&array,&item,s,verbose); 
    if(fscanstring(&item,name,NULL,FALSE,verbose))
    {
      free(soilmap);
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
        free(soilmap);
        return NULL;
      }
      free(soilmap);
      return NULL;
    }
  }
  return soilmap;
} /* of 'fscansoilmap' */
