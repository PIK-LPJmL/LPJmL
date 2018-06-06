/**************************************************************************************/
/**                                                                                \n**/
/**        r  e  a  d  f  l  o  a  t  _  s  o  c  k  e  t  .  c                    \n**/
/**                                                                                \n**/
/**     Function reads float array from socket. Byte order is changed              \n**/
/**     if endianness is different from sender.                                    \n**/
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
#include "swap.h"
#include "channel.h"

Bool readfloat_socket(Socket *socket,float *data,int n)
{
  int *vec;
  int i;
  vec=(int *)data;
  if(read_socket(socket,(char *)vec,sizeof(float)*n))
    return TRUE;
  if(socket->swap)
    for(i=0;i<n;i++)
      vec[i]=swapint(vec[i]);
  return FALSE;
} /* of 'readfloat_socket' */
