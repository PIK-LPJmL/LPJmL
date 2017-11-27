/**************************************************************************************/
/**                                                                                \n**/
/**                  f   r  e  p  e  a  t  c  h  .  c                              \n**/
/**                                                                                \n**/
/**     Function prints character repeatedly                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include "types.h"

void frepeatch(FILE *file, /**< pointer to text file */
               char c,     /**< character to repeat */
               int count   /**< repeat count */
              )
{
  int i;
  for(i=0;i<count;i++)
    putc(c,file);
} /* of 'frepeatch' */
