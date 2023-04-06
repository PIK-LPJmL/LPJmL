/**************************************************************************************/
/**                                                                                \n**/
/**                f  p  r  i  n  t  a  t  t  r  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints global attributes                                          \n**/
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

void fprintattrs(FILE *file,        /**< pointer to text file */
                 const Attr *attrs, /**< array of global attributes */
                 int n_attr         /**< size of array */
                )
{
  int i,len=0;
  if(n_attr>0) /* is anything to print? */
  {
    /* calculate maximum length of line */
    for(i=0;i<n_attr;i++)
      if(strlen(attrs[i].name)+strlen(attrs[i].value)>len)
        len=strlen(attrs[i].name)+strlen(attrs[i].value);
    /* draw top border */
    fputc('/',file);
    frepeatch(file,'-',len+2);
    fputs("\\\n",file);
    for(i=0;i<n_attr;i++)
    {
      fprintf(file,"|%s: %s",attrs[i].name,attrs[i].value);
      frepeatch(file,' ',len-strlen(attrs[i].name)-strlen(attrs[i].value));
      fputs("|\n",file);
    }
    /* draw bottom border */
    fputc('\\',file);
    frepeatch(file,'-',len+2);
    fputs("/\n",file);
  }
} /* of 'fprintattrs' */
