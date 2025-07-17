/**************************************************************************************/
/**                                                                                \n**/
/**                       g  e  t  a  v  g  .  c                                   \n**/
/**                                                                                \n**/
/**     Function computes average of all real vector elements                      \n**/
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
#include "types.h"
#include "hash.h"
#include "bstruct.h"
#include "numeric.h"

Real getavg(const Real vec[], /**< array of real values */
            int size          /**< array size */
           )                  /** \return average of array elements */
{
  int i;
  Real sum=0;

  for(i=0;i<size;i++)
    sum+=vec[i];

  return sum/size;
} /* of 'getavg' */
