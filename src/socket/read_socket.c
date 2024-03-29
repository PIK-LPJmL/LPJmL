/**************************************************************************************/
/**                                                                                \n**/
/**                 r  e  a  d  _  s  o  c  k  e  t  .  c                          \n**/
/**                                                                                \n**/
/**     Function reads bytes from socket                                           \n**/
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

Bool read_socket(Socket *socket,void *data,int n)
{
  int i,j;
#ifdef USE_TIMING
  double tstart,tend;
  tstart=mrun();
#endif
  i=n;
  do
  {
    j=recv(socket->channel,(char *)data+n-i,i,0);
    if(j<0)
      return TRUE;
    i-=j;
  }while(i);
#ifdef USE_TIMING
  tend=mrun();
  timing+=tend-tstart;
#endif
  return FALSE;
} /* of 'read_socket' */
