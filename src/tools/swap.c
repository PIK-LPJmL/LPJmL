/**************************************************************************************/
/**                                                                                \n**/
/**                     s  w  a  p  .  c                                           \n**/
/**                                                                                \n**/
/**     Converts big endian into little endian and vice versa.                     \n**/
/**     Needed for reading data for architectures with different                   \n**/
/**     endianness, e.g., IBM Power6 vs. Intel Xeon.                               \n**/
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

static void swap(char *a,char *b)
{
  char h;
  h=*a;
  *a=*b;
  *b=h;
} /* of 'swap' */

short int swapshort(short int x /**< short to be swapped */
                   )            /** \return swapped short */
{
  swap((char *)&x,(char *)(&x)+1);
  return x;
} /* of 'swapshort' */

unsigned short swapushort(unsigned short x)
{
  swap((char *)&x,(char *)(&x)+1);
  return x;
} /* of 'swapushort' */

int swapint(int x /**< integer to be swapped */
           )      /** \return swapped integer */
{
  swap((char *)&x,(char *)(&x)+3);
  swap((char *)&x+1,(char *)(&x)+2);
  return x;
} /* of 'swapint' */

unsigned int swapuint(unsigned int x /**< unsigned integer to be swapped */
                     )               /** \return swapped integer */
{
  swap((char *)&x,(char *)(&x)+3);
  swap((char *)&x+1,(char *)(&x)+2);
  return x;
} /* of 'swapuint' */

long long swaplong(long long x)
{
  swap((char *)&x,(char *)(&x)+7);
  swap((char *)&x+1,(char *)(&x)+6);
  swap((char *)&x+2,(char *)(&x)+5);
  swap((char *)&x+3,(char *)(&x)+4);
  return x;
} /* of 'swaplong' */

double swapdouble(Num num)
{
  double ret;
  Num x;
  x.hi=swapint(num.lo);
  x.lo=swapint(num.hi);
  memcpy(&ret,&x,sizeof(Num));
  return ret;
} /* of 'swapdouble' */

float swapfloat(int num)
{
  float ret;
  num=swapint(num);
  memcpy(&ret,&num,sizeof(int));
  return ret;
} /* of 'swapfloat' */

size_t freadfloat(float *data, /**< array of floats to be read */
                  size_t n,    /**< size of array */
                  Bool swap,   /**< byte order has to be swapped */
                  FILE *file   /**< pointer to binary file */
                 )             /** \return number of floats read */ 
{
  size_t i,rc;
  int *tmp;
  tmp=(int *)data;
  rc=fread(tmp,sizeof(int),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapfloat(tmp[i]);
  return rc;
} /* of 'freadfloat' */

size_t freaddouble(double *data, /**< array of doubles to be read */
                   size_t n,     /**< size of array */
                   Bool swap,    /**< byte order has to be swapped */
                   FILE *file    /**< pointer to binary file */
                  )              /** \return number of doubles read */ 
{
  size_t i,rc;
  Num *tmp;
  tmp=(Num *)data;
  rc=fread(tmp,sizeof(Num),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapdouble(tmp[i]);
  return rc;
} /* of 'freaddouble' */

size_t freadreal(Real *data, /**< array of reals to be read */
                 size_t n,   /**< size of array */
                 Bool swap,  /**< byte order has to be swapped */
                 FILE *file  /**< pointer to binary file */
                )            /** \return number of reals read */ 
{
  if(sizeof(Real)==sizeof(double))
    return freaddouble((double *)data,n,swap,file);
  else
    return freadfloat((float *)data,n,swap,file);
} /* of 'freadreal' */

size_t freadlong(long long *data, /**< array of long integers to be read */
                 size_t n,        /**< size of array */
                 Bool swap,       /**< byte order has to be swapped */
                 FILE *file       /**< pointer to binary file */
                )                 /** \return number of long integers read */
{
  size_t i,rc;
  rc=fread(data,sizeof(long long),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swaplong(data[i]);
  return rc;
} /* of 'freadlong' */

size_t freadint(int *data, /**< array of integers to be read */
                size_t n,  /**< size of array */
                Bool swap, /**< byte order has to be swapped */
                FILE *file /**< pointer to binary file */
               )           /** \return number of integers read */
{
  size_t i,rc;
  rc=fread(data,sizeof(int),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapint(data[i]);
  return rc;
} /* of 'freadint' */

size_t freaduint(unsigned int *data, /**< array of unsigned integers to be read */
                 size_t n,  /**< size of array */
                 Bool swap, /**< byte order has to be swapped */
                 FILE *file /**< pointer to binary file */
                )           /** \return number of integers read */
{
  size_t i,rc;
  rc=fread(data,sizeof(unsigned int),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapuint(data[i]);
  return rc;
} /* of 'freadint' */

size_t freadushort(unsigned short *data, /**< array of unsigned shorts to be read */
                   size_t n,  /**< size of array */
                   Bool swap, /**< byte order has to be swapped */
                   FILE *file /**< pointer to binary file */
                  )           /** \return number of shorts read */
{
  size_t i,rc;
  rc=fread(data,sizeof(unsigned short),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapushort(data[i]);
  return rc;
} /* of 'freadushort' */

size_t freadshort(short *data, /**< array of shorts to be read */
                  size_t n,    /**< size of array */
                  Bool swap,   /**< byte order has to be swapped */
                  FILE *file   /**< pointer to binary file */
                 )             /** \return number of shorts read */
{
  size_t i,rc;
  rc=fread(data,sizeof(short),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapshort(data[i]);
  return rc;
} /* of 'freadshort' */
