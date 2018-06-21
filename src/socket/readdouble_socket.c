/**************************************************************************************/
/**                                                                                \n**/
/**        r   e  a  d  d  o  u  b  l  e  _  s  o  c  k  e  t  .  c                \n**/
/**                                                                                \n**/
/**     Function reads double array from socket. Byte order is changed             \n**/
/**     if endianness is different from sender.                                    \n**/
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
#include "swap.h"
#include "channel.h"

Bool readdouble_socket(Socket *socket,double *data,int n)
{
  Num *vec;
  int i;
  vec=(Num *)data;
  if(read_socket(socket,(char *)vec,sizeof(double)*n))
    return TRUE;
  if(socket->swap)
    for(i=0;i<n;i++)
      data[i]=swapdouble(vec[i]);
  return FALSE;
} /* of 'readdouble_socket' */
