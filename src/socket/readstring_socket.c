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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "channel.h"

char *readstring_socket(Socket *socket)
{
  unsigned int n;
  char *s;
  if(readint_socket(socket,&n,1))
    return NULL;
  s=malloc(n+1);
  if(s==NULL)
    return s;
  if(n)
    read_socket(socket,s,n);
  s[n]='\0';
  return s;
} /* of 'readstring_socket' */
