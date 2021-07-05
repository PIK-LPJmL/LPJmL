/**************************************************************************************/
/**                                                                                \n**/
/**             g  e  t  f  i  l  e  s  i  z  e  p  .  c                           \n**/
/**                                                                                \n**/
/**     Function gets file size in bytes                                           \n**/
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
#include <sys/stat.h>
#include "types.h"

long long getfilesizep(FILE *file /**< pointer to file */
                      )           /** \return file size in bytes or -1
                                      on error */
{
  struct stat filestat;
  return (file==NULL) ? -1 : ((fstat(fileno(file),&filestat)) ? -1 : filestat.st_size);
} /* of 'getfilesizep' */
