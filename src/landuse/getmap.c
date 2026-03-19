/**************************************************************************************/
/**                                                                                \n**/
/**                       g  e  t  m  a  p  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets landuse mapping from string array                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool getmap(Map *map,            /**< string array */
            const char *filename, /**< filename of landuse file */
            const char *name,
            Bool cftonly,
            int **cftmap,
            int *cftmapsize,
            int npft,      /**< number of natural PFTs */
            int ncft,      /**< number of crop PFTs */
            Config *config /**< LPJmL configuration */
           )
{
  int *landusemap;
  if(map!=NULL)
  {
    if(map->isfloat)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR255: '%s' array in '%s' must be of type string, not float.\n",
                name,filename);
      freemap(map);
      return TRUE;
    }
    landusemap=getcftmap(map,filename,cftonly,npft,ncft,config);
    if(landusemap!=NULL)
    {
      if(*cftmap==NULL)
      {
        *cftmap=landusemap;
        *cftmapsize=getmapsize(map);
      }
      else
      {
        if(isroot(*config))
          cmplandusemap(filename,landusemap,getmapsize(map),
                        name,*cftmap,*cftmapsize,npft,ncft,config);
        free(landusemap);
      }
    }
    freemap(map);
  }
  return FALSE;
} /* of 'getmap' */
