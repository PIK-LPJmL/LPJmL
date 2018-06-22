/**************************************************************************************/
/**                                                                                \n**/
/**                     r  e  a  d  r  e  a  l  v  e  c  .  c                      \n**/
/**                                                                                \n**/
/**     Function reads real array from binary file converting from given           \n**/
/**     data type.                                                                 \n**/
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
#include <string.h>
#include "types.h"
#include "swap.h"

Bool readrealvec(FILE *file,     /**< pointer to binary file */
                 Real data[],    /**< array of reals to read from file */
                 Real intercept, /**< intercept for rescaling data */
                 Real slope,     /**< slope for rescaling data */
                 size_t n,       /**< size of real array */
                 Bool swap,      /**< byte order has to be swapped (TRUE/FALSE) */
                 Type type       /**< type of data in file */
                )                /** \return TRUE on error */
{
  int *ivec;
  float *fvec;
  short *svec;
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
        data[i]=intercept+bvec[i]*slope; 
      free(bvec);
      break;
    case LPJ_SHORT:
      svec=newvec(short,n);
      if(svec==NULL)
        return TRUE;
      if(freadshort(svec,n,swap,file)!=n)
      {
        free(svec);
        return TRUE;
      }
      for(i=0;i<n;i++)
        data[i]=intercept+svec[i]*slope; 
      free(svec);
      break;
    case LPJ_INT:
      ivec=newvec(int,n);
      if(ivec==NULL)
        return TRUE;
      if(freadint(ivec,n,swap,file)!=n)
      {
        free(ivec);
        return TRUE;
      }
      for(i=0;i<n;i++)
        data[i]=intercept+ivec[i]*slope; 
      free(ivec);
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
        data[i]=intercept+fvec[i]*slope; 
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
        data[i]=intercept+dvec[i]*slope; 
      free(dvec);
      break;
  } /* of switch */
  return FALSE;
} /* of 'readrealvec' */
