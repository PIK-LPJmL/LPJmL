/**************************************************************************************/
/**                                                                                \n**/
/**                   h  a  s  a  n  y  s  u  f  f  i  x  .  c                     \n**/
/**                                                                                \n**/
/**     Function checks whether filename has any suffix                            \n**/
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
#include "types.h"

Bool hasanysuffix(const char *name /**< filename */
                 )                 /** \return TRUE if name has suffix */
{
  int i;
#ifdef SAFE
  if(name==NULL)
    return FALSE;
#endif
  for(i=strlen(name)-1;i>=0;i--)
#ifdef _WIN32
    if(name[i]=='/' || name[i]=='\\')
#else
    if(name[i]=='/')
#endif
      return FALSE;
    else if(name[i]=='.')
      return TRUE;
  return FALSE;
} /* of 'hasanysuffix' */
