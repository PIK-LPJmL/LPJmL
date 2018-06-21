/**************************************************************************************/
/**                                                                                \n**/
/**                   f  p  u  t  p  r  i  n  t  a  b  l  e  .  c                  \n**/
/**                                                                                \n**/
/**     Function prints only printable characters to text file                     \n**/
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
#include <ctype.h>
#include "types.h"

void fputprintable(FILE *file,      /**< pointer to text file */
                   const char *line /**< string to be print */
                  )
{
  while(*line!='\0')
  {
    if(isprint(*line))
      fputc(*line,file);
    line++;
  }
} /* of 'fputprintable' */ 
