/**************************************************************************************/
/**                                                                                \n**/
/**                  c  m  p  s  o  i  l  m  a  p  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function compares teh soilmap in the soil file with the soilmap defined    \n**/
/**     in the LPJmL configuration file                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void cmpsoilmap(const int *soilmap,  /**< soilmap array */
                int size,            /**< size of soilmap array */
                const Config *config /**< LPJmL configuration */
               ) 
{
  int i;
  for(i=0;i<min(size,config->soilmap_size);i++)
    if(soilmap[i]!=config->soilmap[i])
      fprintf(stderr,"WARMING037: Soil '%s' in '%s' differs from soil '%s' in 'soilmap' at position %d.\n",
              soilmap[i]==0 ? "null" : config->soilpar[soilmap[i]-1].name,config->soil_filename.name,
              config->soilmap[i]==0 ? "null" : config->soilpar[config->soilmap[i-1]].name,i);

  if(size>config->soilmap_size)
    fprintf(stderr,"WARNING037: Size of soilmap in '%s'=%d is greater than size of 'soilmap'=%d.\n",
            config->soil_filename.name,size,config->soilmap_size); 
  else if(size<config->soilmap_size)
    fprintf(stderr,"WARNING037: Size of soilmap in '%s'=%d is less than size of 'soilmap'=%d.\n",
            config->soil_filename.name,size,config->soilmap_size); 
} /* of 'cmpsoilmap' */
