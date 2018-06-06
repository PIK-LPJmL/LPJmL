/**************************************************************************************/
/**                                                                                \n**/
/**                  c  h  e  c  k  f  m  t  .  c                                  \n**/
/**                                                                                \n**/
/**     Functions checks whether format string contains one %arg format            \n**/
/**     specifier                                                                  \n**/
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
#include "types.h"

Bool checkfmt(const char *fmt, /**< format string */
              char arg         /**< conversion character */
             )                 /** \return TRUE on error */
{
  Bool found;
  if(fmt==NULL)
    return TRUE;
  found=FALSE;
  while(*fmt!='\0')
   if(*fmt=='%') 
   {
     fmt++;
     if(*fmt==arg)
     {
       if(found)
         return TRUE;
       found=TRUE;
       fmt++;
     }
     else if(*fmt=='%')
       fmt++;
     else
       return TRUE;
    }
    else
     fmt++;
  return !found;
} /* of 'checkfmt' */
