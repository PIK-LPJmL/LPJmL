/**************************************************************************************/
/**                                                                                \n**/
/**                  i  s  a  b  s  p  a  t  h  .  c                               \n**/
/**                                                                                \n**/
/**     Function determines whether filename begins with absolute path,            \n**/
/**     i.e. on UNIX begins with '/'                                               \n**/
/**          on Windows begins with '/','\' or drive letter 'a:'                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "types.h"

Bool isabspath(const char *name /**< filenanme */
              )                 /** \return TRUE if filename start with absolute path */
{
#ifdef _WIN32
  if(name[0]=='/' || name[0]=='\\')
    return TRUE;
  if(strlen(name)>1 && isalpha(name[0]) && name[1]==':')
    return TRUE;
  return FALSE;
#else
  return (name[0]=='/');
#endif
} /* of 'isabspath' */
