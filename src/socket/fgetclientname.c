/**************************************************************************************/
/**                                                                                \n**/
/**            f  g  e  t  c  l  i  e  n  t  n  a  m  e  .  c                      \n**/
/**                                                                                \n**/
/**     Function gets client hostname from socket                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "channel.h"

void getclientname_(const Socket *socket,
                    char *name,
                    int *err,
                    int len
                   )
{
  char *s;
  s=getclientname(socket);
  if(s==NULL)
    *err=1;
  else
  {
    strncpy(name,s,len);
    *err=0;
  }
} /* of 'getclientname' */
