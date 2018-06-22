/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  h  o  s  t  .  c                                     \n**/
/**                                                                                \n**/
/**     Function gets host name                                                    \n**/
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
#include <limits.h>
#include "types.h"
#ifdef _WIN32
#include "windows.h"
#define INFO_BUFFER_SIZE 1024
#else
#include <unistd.h>
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX STRING_LEN
#endif
#endif

char *gethost(void) /** \return get host name */
{
#ifdef _WIN32
  static TCHAR infoBuf[INFO_BUFFER_SIZE];
  DWORD  bufCharCount = INFO_BUFFER_SIZE;

  /* Get and display the name of the computer.  */
  bufCharCount = INFO_BUFFER_SIZE;
  GetComputerName(infoBuf,&bufCharCount);
  return infoBuf;
#else
  static char name[HOST_NAME_MAX];
  gethostname(name,HOST_NAME_MAX);
  return name;
#endif
} /* of 'gethost' */
