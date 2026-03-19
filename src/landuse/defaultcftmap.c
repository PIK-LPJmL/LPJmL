/**************************************************************************************/
/**                                                                                \n**/
/**                    d  e  f  a  u  l  t  c  f  t  m  a  p  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function sets default landuse mapping                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

int *defaultcftmap(int *size,           /**< size of CFT map array */
                   const char *name,    /**< name of map */
                   Bool cftonly,        /**< set only crop PFTs */
                   int npft,            /**< number of natural PFTs */
                   int ncft,            /**< number of crop PFTs */
                   const Config *config /**< LPJ configuration */
                  )                     /** \return default CFT map array or NULL on error */
{
  int *cftmap;
  int cft;
  *size=(cftonly) ? ncft : getnirrig(ncft,config);
  cftmap=newvec(int,*size);
  if(cftmap==NULL)
  {
    printallocerr("cftmap");
    return NULL;
  }
  for(cft=0;cft<*size;cft++)
    cftmap[cft]=cft;
  if(isroot(*config))
  {
    fprintf(stderr,"WARNING011: Map '%s' not found, set to [",name);
    fprintpftnames(stderr,config->pftpar+npft,ncft);
    if(!cftonly)
    {
      for(cft=0;cft<NGRASS;cft++)
        fprintf(stderr,",\"%s\"",grassland_names[cft]);
      for(cft=0;cft<NBIOMASSTYPE;cft++)
        fprintf(stderr,",\"%s\"",biomass_names[cft]);
      for(cft=0;cft<config->nwft;cft++)
        fprintf(stderr,",\"%s\"",woodplantation_names[cft]);
      for(cft=0;cft<config->nagtree;cft++)
        fprintf(stderr,",\"%s\"",config->pftpar[npft-config->nagtree+cft].name);
    }
    fputs("].\n",stderr);
  }
  return cftmap;
} /* of 'defaultcftmap' */
