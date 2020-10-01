/**************************************************************************************/
/**                                                                                \n**/
/**                         s  e  t  s  e  e  d  .  c                              \n**/
/**                                                                                \n**/
/**     Set seed of random number generator                                        \n**/
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
#include <limits.h>
#include "types.h"
#include "numeric.h"

void setseed(Seed seed,int start)
{
#ifdef USE_RAND48
  seed[0]=13070;
  seed[1]=start % (USHRT_MAX+1);
  seed[2]=start / (USHRT_MAX+1);
  seed48(seed);
#else
  seed[0]=start;
#endif
} /* of 'setseed' */
