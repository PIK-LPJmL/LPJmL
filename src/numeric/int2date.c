/**************************************************************************************/
/**                                                                                \n**/
/**                     i  n  t  2  d  a  t  e  .  c                               \n**/
/**                                                                                \n**/
/**     Function converts long int in the format YYYYMMDD into date                \n**/
/**     string                                                                     \n**/
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
#include "date.h"

char *int2date(long date /**< date in format YYYYMMDD */
              )          /** \return date as string */
{
  static char str[13];
  sprintf(str,"%s %2d %4d",months[(date % 10000)/100-1],
          (int)(date % 100),(int)(date/10000));
  return str;
} /* of 'int2date' */
