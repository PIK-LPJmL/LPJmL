/**************************************************************************************/
/**                                                                                \n**/
/**                    s  t  r  d  a  t  e  .  c                                   \n**/
/**                                                                                \n**/
/**     Function implements ctime() without trailing newline                       \n**/
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

char *strdate(const time_t *t)
{
  static String s;
  strftime(s,STRING_LEN,"%y-%m-%d",localtime(t));
  return s;
} /* of 'strdate' */
