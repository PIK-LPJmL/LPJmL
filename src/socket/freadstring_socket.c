/**************************************************************************************/
/**                                                                                \n**/
/**        r  e  a  d  s  t  r  i  n  g  _  s  o  c  k  e  t  .  c                 \n**/
/**                                                                                \n**/
/**     Function reads string from socket                                          \n**/
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
#include "channel.h"

void readstring_socket_(Socket *socket,char *s,int *err,int len)
{
  int n;
  if(readint_socket(socket,&n,1))
    *err=1;
  else if(n>len)
    *err=1;
  else if(n)
    *err=read_socket(socket,s,n);
  else
    *err=0;
} /* of 'readstring_socket' */
