/**************************************************************************************/
/**                                                                                \n**/
/**                  c  m  p  l  a  n  d  u  s  e  m  a  p  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function compares the land use map in the landuse file with the landusemap \n**/
/**     defined in the LPJmL configuration file                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void cmplandusemap(const char *filename,   /**< filename of landuse file */
                   const int *landusemap,  /**< land use map array */
                   int size,               /**< size of land use map array */
                   const char *name,       /**< name of land use map array to compare with */
                   const int *landusemap2, /**< land use map array to compare with */
                   int size2,              /**< size of land use map array to compare with */
                   int npft,               /**< number of natutal PFTs */
                   int ncft,               /**< number of crop PFTs */
                   const Config *config    /**< LPJmL configuration */
                  )
{
  int i;
  for(i=0;i<min(size,size2);i++)
    if(landusemap[i]!=landusemap2[i])
      fprintf(stderr,"WARNING037: land use '%s' in '%s' differs from land use '%s' in '%s' at position %d, land use in '%s' used.\n",
              landusemap[i]==NOT_FOUND ? "not found" : getcftname(landusemap[i],npft,ncft,config),filename,
              landusemap2[i]==NOT_FOUND ? "not found" : getcftname(landusemap2[i],npft,ncft,config),name,i,name);

  if(size>size2)
    fprintf(stderr,"WARNING037: Size of map in '%s'=%d is greater than size of '%s'=%d.\n",
            filename,size,name,size2);
  else if(size<size2)
    fprintf(stderr,"WARNING037: Size of map in '%s'=%d is less than size of '%s'=%d.\n",
            filename,size,name,size2);
} /* of 'cmplandusemap' */
