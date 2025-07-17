/**************************************************************************************/
/**                                                                                \n**/
/**                       g  e  t  m  a  x  .  c                                   \n**/
/**                                                                                \n**/
/**     Function computes maximum of all real vector elements                      \n**/
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

Real getmax(const Real vec[], /**< array of real values */
            int size          /**< array size */
           )                  /** \return maximum of array elements */
{
  int i;
  Real max;
  max=vec[0];
  for(i=0;i<size;i++)
    if(max<vec[i])
      max=vec[i];
  return max;
} /* of 'getmax' */
