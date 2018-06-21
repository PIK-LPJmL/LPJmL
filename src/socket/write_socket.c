/**************************************************************************************/
/**                                                                                \n**/
/**              w  r  i  t  e  _  s  o  c  k  e  t  .  c                          \n**/
/**                                                                                \n**/
/**     Function writes bytes to socket                                            \n**/
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
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include "types.h"
#include "channel.h"

Bool write_socket(Socket *socket,const void *buffer,int n)
{
  int i,j;
  i=0;
  do
  {
    j=send(socket->channel,(char *)buffer+i,n,0);
    if(j<0)
      return TRUE;
    i+=j;
    n-=j;
  }while(n);
  return FALSE;
} /* of 'write_socket' */
