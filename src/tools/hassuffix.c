/**************************************************************************************/
/**                                                                                \n**/
/**                   h  a  s  s  u  f  f  i  x  .  c                              \n**/
/**                                                                                \n**/
/**     Function checks whether filename has specified suffix                      \n**/
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

Bool hassuffix(const char *name,  /**< filename with suffix */
               const char *suffix /**< file suffix including '.' */
              )                   /** \return TRUE if name has suffix */
{
#ifdef SAFE
  if(name==NULL || suffix==NULL)
    return FALSE;
#endif
  return (strlen(name)>=strlen(suffix) &&
          !strcmp(name+strlen(name)-strlen(suffix),suffix));
} /* of 'hassuffix' */
