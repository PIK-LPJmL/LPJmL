/**************************************************************************************/
/**                                                                                \n**/
/**                            r  e  a  d  s  e  e  d  .  c                        \n**/
/**                                                                                \n**/
/**     Read seed of random number generator from restart file                     \n**/
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
#include "swap.h"
#include "numeric.h"

Bool readseed(FILE *file, /**< pointer to restart file */
              const char *name, /**< name of object */
              Seed seed,  /**< seed of random number generator read */
              Bool swap   /**< byte order has to be swapped */
             )            /** \return TRUE on error */
{
#ifdef USE_RAND48
  return readushortarray(file,name,seed,NSEED,swap);
#else
  return readintarray(file,name,seed,NSEED,swap);
#endif
} /* of 'readseed' */
