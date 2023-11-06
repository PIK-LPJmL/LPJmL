/**************************************************************************************/
/**                                                                                \n**/
/**               m  e  r  g  e  a  t  t  r  s  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions merges attribute lists.                                          \n**/
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
#include <math.h>
#include "types.h"
#include "errmsg.h"

static int findattr(const char *name,Attr *attrs,int n)
{
  int i;
  for(i=0;i<n;i++)
   if(!strcmp(name,attrs[i].name))
     return i;
  return NOT_FOUND;
} /* of 'findattr' */

void mergeattrs(Attr **attrs,       /**< pointer to array of attributes */
                int *n,             /**< length of attribute list */
                const Attr *attrs2, /**< attribute list to merge */
                int n2              /**< length of attribute list to merge */
               )
{
  int i,index;
  for(i=0;i<n2;i++)
  {
    /* is name already defined? */
    index=findattr(attrs2[i].name,*attrs,*n);
    if(index==NOT_FOUND)
    {
      /* no, append attribute list with new entry */
      *attrs=realloc(*attrs,sizeof(Attr)*(*n+1));
      (*attrs)[*n].name=strdup(attrs2[i].name);
      (*attrs)[*n].value=strdup(attrs2[i].value);
      (*n)++;
    }
    else
    {
      /* yes, replace value */
      free((*attrs)[index].value);
      (*attrs)[index].value=strdup(attrs2[i].value);
    }
  }
} /* of 'mergeattrs' */
