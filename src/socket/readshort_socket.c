/**************************************************************************************/
/**                                                                                \n**/
/**        r  e  a  d  s  h  o  r  t  _  s  o  c  k  e  t  .  c                    \n**/
/**                                                                                \n**/
/**     Function reads short array from socket. Byte order is changed              \n**/
/**     if endianness is different from sender                                     \n**/
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

Bool readshort_socket(Socket *socket,short *data,int n)
{
  int i;
  if(read_socket(socket,(char *)data,sizeof(short)*n))
    return TRUE;
  if(socket->swap)
    for(i=0;i<n;i++)
      data[i]=swapshort(data[i]); /* change byte order */
  return FALSE;
} /* of 'readshort_socket' */
