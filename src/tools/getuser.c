/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  u  s  e  r  .  c                                     \n**/
/**                                                                                \n**/
/**     Function gets user name                                                    \n**/
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
#include "types.h"
#ifdef _WIN32
#include <windows.h>
#define INFO_BUFFER_SIZE 1024
#else
#include <unistd.h>
#include <pwd.h>
#endif

char *getuser(void) /** \return user name */
{
#ifdef _WIN32
  static TCHAR infoBuf[INFO_BUFFER_SIZE];
  DWORD bufCharCount = INFO_BUFFER_SIZE;

  /* Get and display the user name.  */
  bufCharCount = INFO_BUFFER_SIZE;
  GetUserName( infoBuf, &bufCharCount );
  return infoBuf;
#else
  struct passwd *who;
  who=getpwuid(getuid());
  return who->pw_name;
#endif
} /* of 'getuser' */
