/**************************************************************************************/
/**                                                                                \n**/
/**                         r  a  n  d  .  c                                       \n**/
/**                                                                                \n**/
/**     Random number generator                                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include "types.h"
#include "numeric.h"

#define modulus 2147483647
#define multiplier 16807 
#define q 127773
#define r 2836
#define MASK 123459876

static int seed=12345678; /* seed for random number generator (see randfrac) */

void setseed(int init /**< seed of random number generator */
            ) 
{
  seed=init;
} /* of 'setseed' */

int getseed(void) /** \return seed of random number generator */
{
  return seed;
} /* of 'getseed' */

Real randfrac(void) /** \return random number */
{
 /*
  * DESCRIPTION
  * Returns a random floating-point number in the range 0-1.
  * Uses and updates the global variable 'seed' which may be initialised to any
  * positive integral value (the same initial value will result in the same 
  * sequence of returned values on subsequent calls to randfloat every time
  * the program is run)
  *
  * Reference: Park & Miller 1988 CACM 31: 1192
  * based on Numerical Recipes ran0() 
  *
  */
  int k;
  Real ans;
  seed^=MASK; /* XORing to avoid seed being 0 */
  k=seed/q;
  seed=multiplier*(seed-k*q)-r*k;
  if (seed<0) 
    seed+=modulus;
  ans=(Real)seed/(Real)modulus;
  seed^=MASK; /* XORing to avoid seed being 0 */
  return ans;
} /* of 'randfrac' */
