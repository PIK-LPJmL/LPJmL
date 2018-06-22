/**************************************************************************************/
/**                                                                                \n**/
/**                        a  d  d  p  a  t  h  .  c                               \n**/
/**                                                                                \n**/
/**     Function add path to filename. Only done for filenames without             \n**/
/**     an absolute path.                                                          \n**/
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

char *addpath(const char *name, /**< filename */
              const char *path  /**< path or NULL */
             )                  /** \return allocated string or NULL */
{
  char *s;
  if(path!=NULL && !isabspath(name))
  {
    s=malloc(strlen(path)+strlen(name)+2);
    if(s==NULL)
      return NULL;
    return strcat(strcat(strcpy(s,path),"/"),name);
  }
  else
    return strdup(name);
} /* of 'addpath' */
