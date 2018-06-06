/**************************************************************************************/
/**                                                                                \n**/
/**                r  e  a  d  u  i  n  t  v  e  c  .  c                           \n**/
/**                                                                                \n**/
/**     Function reads unsigned int array from binary file converting              \n**/
/**     from given data type.                                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "swap.h"

Bool readuintvec(FILE *file,          /**< pointer to binary file */
                 unsigned int data[], /**< array of unsigned ints to read from file */
                 size_t n,            /**< size of array */
                 Bool swap, /**< byte order has to be swapped (TRUE/FALSE) */
                 Type type            /**< type of data in file */
                )                     /** \return TRUE on error */
{
  unsigned short *svec;
  float *fvec;
  Byte *bvec;
  double *dvec;
  size_t i;
  switch(type)
  {
    case LPJ_BYTE:
      bvec=newvec(Byte,n);
      if(bvec==NULL)
        return TRUE;
      if(fread(bvec,1,n,file)!=n)
      {
        free(bvec);
        return TRUE;
      }
      for(i=0;i<n;i++)
        data[i]=bvec[i]; 
      free(bvec);
      break;
    case LPJ_SHORT:
      svec=newvec(unsigned short,n);
      if(svec==NULL)
        return TRUE;
      if(freadushort(svec,n,swap,file)!=n)
      {
        free(svec);
        return TRUE;
      }
      for(i=0;i<n;i++)
        data[i]=svec[i]; 
      free(svec);
      break;
    case LPJ_INT:
      if(freaduint(data,n,swap,file)!=n)
        return TRUE;
      break;
    case LPJ_FLOAT:
      fvec=newvec(float,n);
      if(fvec==NULL)
        return TRUE;
      if(freadfloat(fvec,n,swap,file)!=n)
      {
        free(fvec);
        return TRUE;
      }
      for(i=0;i<n;i++)
        data[i]=(unsigned int)fvec[i]; 
      free(fvec);
      break;
    case LPJ_DOUBLE:
      dvec=newvec(double,n);
      if(dvec==NULL)
        return TRUE;
      if(freaddouble(dvec,n,swap,file)!=n)
      {
        free(dvec);
        return TRUE;
      }
      for(i=0;i<n;i++)
        data[i]=(unsigned int)dvec[i]; 
      free(dvec);
      break;
  } /* of switch */
  return FALSE;
} /* of 'readuintvec' */
