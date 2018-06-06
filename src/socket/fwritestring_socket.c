/**************************************************************************************/
/**                                                                                \n**/
/**    w  r  i  t  e  s  t  r  i  n  g  _  s  o  c  k  e  t  .  c                  \n**/
/**                                                                                \n**/
/**     Function writes string to socket                                           \n**/
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
#include <string.h>
#include "types.h"
#include "channel.h"

void writestring_socket_(Socket *socket,const char *s,int *err,int len)
{
  if(writeint_socket(socket,&len,1))
    *err=1;
  else if(len)
    *err=write_socket(socket,s,len);
  else
    *err=0;
} /* of 'writestring_socket' */
