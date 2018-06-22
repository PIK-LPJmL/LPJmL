/**************************************************************************************/
/**                                                                                \n**/
/**        r  e  a  d  i  n  t  _  s  o  c  k  e  t  .  c                          \n**/
/**                                                                                \n**/
/**     Function reads int array from socket. Byte order is changed                \n**/
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

Bool readint_socket(Socket *socket,int *data,int n)
{
  int i;
  if(read_socket(socket,(char *)data,sizeof(int)*n))
    return TRUE;
  if(socket->swap)
    for(i=0;i<n;i++)
      data[i]=swapint(data[i]);
  return FALSE;
} /* of 'readint_socket' */
