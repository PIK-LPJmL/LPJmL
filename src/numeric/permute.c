/**************************************************************************************/
/**                                                                                \n**/
/**                   p  e  r  m  u  t  e  .  c                                    \n**/
/**                                                                                \n**/
/**     Creates int vector with random permutation                                 \n**/
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
#include <math.h>
#include "types.h"
#include "bstruct.h"
#include "numeric.h"

void permute(int vec[],int size,Seed seed)
{
  int i,index,swp;
  for(i=0;i<size;i++)
    vec[i]=i;
  for(i=0;i<size-1;i++)
  {
    index=i+(int)(erand48(seed)*(size-i));
    swp=vec[i];
    vec[i]=vec[index];
    vec[index]=swp;
  }
} /* of 'permute' */
