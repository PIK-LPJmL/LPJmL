/**************************************************************************************/
/**                                                                                \n**/
/**                       s  y  s  n  a  m  e  .  c                                \n**/
/**                                                                                \n**/
/**     Function returns name. Name is determined by either                        \n**/
/**     calling uname (UNIX) or invoking 'ver' (Windows).                          \n**/
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
#include <string.h>
#ifndef _WIN32
#include <sys/utsname.h> /* only defined on Unix */
#endif
#include "types.h"

char *sysname(void) /** \return system name */
{
  static String sys;
#ifdef _WIN32
  strcpy(sys,"Windows");
#else
  struct utsname osname;
  uname(&osname);
  strncpy(sys,osname.sysname,STRING_LEN);
  sys[STRING_LEN]='\0';
#endif
  return sys;
} /* of 'sysname' */

