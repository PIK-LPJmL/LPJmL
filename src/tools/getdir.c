/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  d  i  r  .  c                                        \n**/
/**                                                                                \n**/
/**     Function gets current working directory                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#define PATH_MAX STRING_LEN /* PATH_MAX is not defined on Windows */
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "types.h"

char *getdir(void) /** \return path of working directory */
{
  static char name[PATH_MAX];
  if(getcwd(name,PATH_MAX)==NULL)
    strcpy(name,"n/a");
  return name;
} /* of 'getdir' */
