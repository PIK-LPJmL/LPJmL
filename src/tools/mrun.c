/**************************************************************************************/
/**                                                                                \n**/
/**                        m  r  u  n  .  c                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include "types.h"

#define uSecScale 1.0e-6 /* Microsecond conversions */

double mrun(void) /** \return time in sec with microsecond resolution */
{
  struct timeval tp;
  struct timezone tzp;
  gettimeofday(&tp,&tzp);
  return  (((double)tp.tv_usec) * uSecScale) + (double)tp.tv_sec;
} /* of 'mrun' */
