/**************************************************************************************/
/**                                                                                \n**/
/**                   f  p  u  t  s  t  r  i  n  g  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints string and breaks line into pieces if output               \n**/
/**     exceeds maximum line length.                                               \n**/
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

int fputstring(FILE *file,       /**< pointer to ASCII file */
               int len,          /**< current length of line */
               const char *line, /**< string added to line */
               int max_len       /**< maximum length of line */
              )                  /** \return updated length of line */
{
  int l;
  if(line==NULL)
    return len;
  l=strlen(line);
  if(len+l>max_len) /* line too long? */
  {
    /* yes, print newline */
    putc('\n',file);
    len=l;
  }
  else
    len+=l; /* no, update length of line */
  fputs(line,file);
  return len;
} /* of 'fputstring' */
