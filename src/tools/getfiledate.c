/**************************************************************************************/
/**                                                                                \n**/
/**             g  e  t  f  i  l  e  d  a  t  e  .  c                              \n**/
/**                                                                                \n**/
/**     Function gets modification time of file                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include "types.h"

time_t getfiledate(const char *name /**< filename */
                  )                 /** \return modification time or -1
                                       on error */
{
  struct stat filestat;
  return (stat(name,&filestat)) ? -1 : filestat.st_mtime;
} /* of 'getfiledate' */
