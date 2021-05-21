/**************************************************************************************/
/**                                                                                \n**/
/**      f  w  r  i  t  e  r  e  s  t  a  r  t  h  e  a  d  e  r  .  c             \n**/
/**                                                                                \n**/
/**     Writing file header for LPJ restart files.                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwriterestartheader(FILE *file,                 /**< file pointer of binary file */
                         const Restartheader *header /**< file header to be written */
                        )                            /** \return TRUE on error */
{
  if(fwrite(&header->landuse,sizeof(int),1,file)!=1)
    return TRUE;
  if(fwrite(&header->river_routing,sizeof(int),1,file)!=1)
    return TRUE;
  if(fwrite(&header->sdate_option,sizeof(int),1,file)!=1)
    return TRUE;
  if(fwrite(&header->crop_option,sizeof(int),1,file)!=1)
    return TRUE;
  if(fwrite(&header->double_harvest,sizeof(int),1,file)!=1)
    return TRUE;
#ifdef USE_RAND48
  return fwrite(header->seed,sizeof(short),NSEED,file)!=NSEED;
#else
  return fwrite(header->seed,sizeof(int),NSEED,file)!=NSEED;
#endif
} /* of 'fwriterestartheader' */
