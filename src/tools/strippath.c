/**************************************************************************************/
/**                                                                                \n**/
/**                   s  t  r  i  p  p  a  t  h  .  c                              \n**/
/**                                                                                \n**/
/**     Function strips path from filename                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

const char *strippath(const char *name /**< filename with path */
                     )                 /** \return pointer to filename */
{
  int i;
#ifdef SAFE
  if(name==NULL)
    return NULL;
#endif
  for(i=strlen(name)-1;i>=0;i--)
#ifdef _WIN32
    if(name[i]=='/' || name[i]=='\\' || name[i]==':')
    /* check for slash, backslash and ':' of drive */
#else
    if(name[i]=='/') /* check for slash only */
#endif
      break;
  return name+i+1;
} /* of 'strippath' */
