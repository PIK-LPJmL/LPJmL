/**************************************************************************************/
/**                                                                                \n**/
/**                    i  s  d  i  r  .  c                                         \n**/
/**                                                                                \n**/
/**     Function determines whether file is a directory                            \n**/
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

#ifndef S_ISDIR /* macro is not defined on Windows */
#define S_ISDIR(a) (a & _S_IFDIR)
#endif

Bool isdir(const char *name /**< filename */
          )                 /** \return name is a directory (TRUE/FALSE) */
{
  struct stat filestat;
  if(stat(name,&filestat))
    return FALSE;
  return S_ISDIR(filestat.st_mode);
} /* of 'isdir' */

