/**************************************************************************************/
/**                                                                                \n**/
/**                       i  v  e  c  _  s  u  m  .  c                             \n**/
/**                                                                                \n**/
/**     Function computes sum of all integer vector elements                       \n**/
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
#include "numeric.h"

int ivec_sum(const int vec[], /**< array of integer values */
             int size         /**< array size */
            )                 /** \return sum of array elements */
{
  int i;
  int sum=0;

  for(i=0;i<size;i++) 
    sum+=vec[i];
   
  return sum;
} /* of 'ivec_sum' */
