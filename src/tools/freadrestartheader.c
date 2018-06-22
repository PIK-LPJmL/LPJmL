/**************************************************************************************/
/**                                                                                \n**/
/**      f  r  e  a  d  r  e  s  t  a  r  t  h  e  a  d  e  r  .  c                \n**/
/**                                                                                \n**/
/**     Reading file header for LPJ related files. Detects                         \n**/
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

Bool freadrestartheader(FILE *file, /**< file pointer of binary file */
                        Restartheader *header, /**< file header to be read */
                        Bool swap /**< set to TRUE if data is in different order */
                       ) /** \return TRUE on error */
{
  if(fread(header,sizeof(Restartheader),1,file)!=1)
    return TRUE;
  if(swap)
  {
    header->landuse=swapint(header->landuse);
    header->river_routing=swapint(header->river_routing);
    header->sdate_option=swapint(header->sdate_option);
  }
  return FALSE;
} /* of 'freadrestartheader' */
