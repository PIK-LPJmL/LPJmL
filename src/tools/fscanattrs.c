/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  a  t  t  r  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads global attributes from a text file                          \n**/
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
#ifdef USE_JSON
#include <json-c/json.h>
#endif
#include "types.h"
#include "errmsg.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}

Bool fscanattrs(LPJfile *file,    /**< pointer to a LPJ file */
                Attr **attrs,     /**< array of attributes */
                int *n,           /**< size of attribute array */
                const char *name, /**< name of variable */
                Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
               )                  /** \return TRUE on error */
{
#ifdef USE_JSON
  LPJfile item;
  struct json_object_iterator it,it_end;
  const char *name2;
  String s;
  int i;
  if(fscanstruct(file,&item,name,verb))
    return TRUE;
  *n=json_object_object_length(item.file.obj);
  *attrs=newvec(Attr,*n);
  checkptr(*attrs);
  it_end=json_object_iter_end(item.file.obj);
  i=0;
  for(it=json_object_iter_begin(item.file.obj);!json_object_iter_equal(&it,&it_end);json_object_iter_next(&it))
  {
    name2=json_object_iter_peek_name(&it);
    (*attrs)[i].name=strdup(name2);
    checkptr((*attrs)[i].name);
    if(fscanstring(&item,s,name2,FALSE,verb))
      return TRUE;
    (*attrs)[i].value=strdup(s);
    checkptr((*attrs)[i].value);
    i++;
  }
#else
  *attrs=NULL;
  *n=0;
#endif
  return FALSE;
} /* of 'fscanattrs' */
