/**************************************************************************************/
/**                                                                                \n**/
/**      f  r  e  a  d  r  e  s  t  a  r  t  h  e  a  d  e  r  .  c                \n**/
/**                                                                                \n**/
/**     Reading file header for LPJ restart files. Detects                         \n**/
/**     whether byte order has to be changed                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadrestartheader(FILE *file,            /**< file pointer of binary file */
                        Restartheader *header, /**< file header to be read */
                        Bool swap              /**< set to TRUE if data is in different byte order */
                       )                       /** \return TRUE on error */
{
  if(freadint(&header->landuse,1,swap,file)!=1)
    return TRUE;
  if(freadint(&header->river_routing,1,swap,file)!=1)
    return TRUE;
  if(freadint(&header->sdate_option,1,swap,file)!=1)
    return TRUE;
  if(freadint(&header->crop_option,1,swap,file)!=1)
    return TRUE;
  if(freadint(&header->double_harvest,1,swap,file)!=1)
    return TRUE;
#ifdef USE_RAND48
  return freadushort(header->seed,NSEED,swap,file)!=NSEED;
#else
  return freadint(header->seed,NSEED,swap,file)!=NSEED;
#endif
} /* of 'freadrestartheader' */
