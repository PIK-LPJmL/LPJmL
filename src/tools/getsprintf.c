/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  s  p  r  i  n  t  f  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints formatted output into allocated string                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "types.h"

char *getsprintf(const char *fmt, /**< format string */
                 ...              /**< optional parameter for output */
                )                 /** \return allocated string with output or NULL on error */
{
  char *s;
  size_t len;
  va_list ap;
  va_start(ap,fmt);
#ifdef _WIN32
  len = _vscprintf(fmt, ap);
#else
  len = vsnprintf(NULL,0,fmt, ap);
#endif
  va_end(ap);
  if(len<0)
  {
    fprintf(stderr,"ERROR247: Invalid format in '%s'.\n",fmt);
    return NULL;
  }
  s=malloc(len+1);
  if(s==NULL)
    return NULL;
  va_start(ap,fmt);
#ifdef _WIN32
  _vsnprintf_s(s, len+1, _TRUNCATE, fmt, ap);
#else
  vsnprintf(s,len+1,fmt, ap);
#endif
  va_end(ap);
  return s;
} /* of 'getsprintf' */
