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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "channel.h"

Bool writestring_socket(Socket *socket,const char *s)
{
  unsigned int n;
  n=strlen(s);
  if(writeint_socket(socket,&n,1))
    return TRUE;
  if(n)
    return write_socket(socket,s,n);
  return FALSE;
} /* of 'writestring_socket' */
