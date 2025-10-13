/**************************************************************************************/
/**                                                                                \n**/
/**               g e t l i m i t a r r a y f r o m j s o n . c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads limit array from JSON file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <json-c/json.h>
#include "lpj.h"

Limit *getlimitarrayfromjson(const char *filename, /**< name of JSON file */
                             int *size,            /**< size of limit array */
                             const char *key,      /**< name of limit array in JSON file */
                             Verbosity verb        /**< verbosity level (NO_ERR,ERR,VERB) */
                            )                      /** limit array read or NULL on error */
{
  FILE *file;
  Limit *limits;
  LPJfile *lpjfile,*array,*item;
  int i;
  if((file=fopen(filename,"r"))==NULL)
  {
    if(verb)
      printfopenerr(filename);
    return NULL;
  }
  lpjfile=parse_json(file,verb);
  fclose(file);
  if(lpjfile==NULL)
    return NULL;
  array=fscanarray(lpjfile,size,key,verb);
  if(array==NULL)
  {
    closeconfig(lpjfile);
    return NULL;
  }
  limits=newvec(Limit,*size);
  if(limits==NULL)
  {
    printallocerr("limits");
    closeconfig(lpjfile);
    return NULL;
  }
  for(i=0;i<*size;i++)
  {
    item=fscanarrayindex(array,i);
    if(fscanlimit(item,limits+i,NULL,verb))
    {
      free(limits);
      closeconfig(lpjfile);
      return NULL;
    }
  }
  closeconfig(lpjfile);
  return limits;
} /* of 'getlimitarrayfromjson' */
