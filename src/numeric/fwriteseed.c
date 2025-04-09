/**************************************************************************************/
/**                                                                                \n**/
/**                         f  w  r  i  t  e  s  e  e  d  .  c                     \n**/
/**                                                                                \n**/
/**     Write seed of random number generator in JSON-like format in restart file  \n**/
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
#include "bstruct.h"
#include "swap.h"
#include "numeric.h"

Bool fwriteseed(Bstruct file,     /**< pointer to restart file */
                const char *name, /**< name of object */
                const Seed seed   /**< seed of random number generator read */
               )                  /** \return TRUE on error */
{
#ifdef USE_RAND48
  return bstruct_writeushortarray(file,name,seed,NSEED);
#else
  return bstruct_writeintarray(file,name,seed,NSEED);
#endif
} /* of 'fwriteseed' */
