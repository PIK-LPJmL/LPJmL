/**************************************************************************************/
/**                                                                                \n**/
/**                   f  p  u  t  p  r  i  n  t  a  b  l  e  .  c                  \n**/
/**                                                                                \n**/
/**     Function prints only printable characters, converts special char           \n**/
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
    if((unsigned char)*line>=' ') /* is char a printable character? */
      fputc(*line,file); /* yes, print it */
    else
      switch(*line) /* no */
      {
        case '\a':
          fputs("\\a",file); /* bell */
          break;
        case '\t':
          fputs("\\t",file); /* tab */
          break;
        case '\r':
          fputs("\\r",file); /* carriage return */
          break;
        case '\f':
          fputs("\\f",file); /* form feed */
          break;
        case '\v':
          fputs("\\v",file); /* vertical tab */
          break;
        case '\n':
          fputs("\\n",file); /* newline */
          break;
        case '\b':
          fputs("\\b",file); /* backspace */
          break;
        default:
          fprintf(file,"\\%03o",(unsigned int)*line); /* print char as 3 digit octal number */
      }
    line++;
  }
} /* of 'fputprintable' */ 
