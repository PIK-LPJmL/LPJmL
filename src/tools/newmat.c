/**************************************************************************************/
/**                                                                                \n**/
/**                     n  e  w  m  a  t  .  c                                     \n**/
/**                                                                                \n**/
/**     Function allocates 2-dimensional array. 2-D matrix is allocated            \n**/
/**     as one block of memory                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "types.h"

void **newmat(size_t size,int ysize,int xsize)
{
  char **mat;
  int y;
  mat=newvec(char *,ysize);
  if(mat==NULL)
    return NULL;
  mat[0]=malloc(size*xsize*ysize);
  if(mat[0]==NULL)
  {
    free(mat);
    return NULL;
  }
  for(y=1;y<ysize;y++)
    mat[y]=mat[y-1]+xsize*size;
  return (void **)mat;
} /* of 'newmat' */
