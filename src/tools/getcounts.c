/**************************************************************************************/
/**                                                                                \n**/
/**                  g  e  t  c  o  u  n  t  s  .  c                               \n**/
/**                                                                                \n**/
/**     Function computes counts and offsets used by MPI_Gatherv/                  \n**/
/**     MPI_Scatterv                                                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include "types.h"

void getcounts(int counts[], /**< number of cells for each task */
               int offsets[], /**< cell offsets for each task */
               int size,      /**< total number of cells */
               int n,         /**< number of item per cell */
               int tasks      /**< number of tasks */
              )
{
  int i;
  for(i=0;i<tasks;i++)
  {
    counts[i]=size/tasks;
    if(i<size % tasks)
      counts[i]++;
    counts[i]*=n;
  }
  offsets[0]=0;
  for(i=1;i<tasks;i++)
    offsets[i]=offsets[i-1]+counts[i-1];
} /* of 'getcounts' */
