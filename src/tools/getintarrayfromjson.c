/**************************************************************************************/
/**                                                                                \n**/
/**               g e t i n t a r r a y f r o m j s o n . c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads int array from JSON file                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int *getintarrayfromjson(const char *filename, /**< name of JSON file */
                         int *size,            /**< size of int array */
                         const char *key,      /**< name of int array in JSON file */
                         Verbosity verb        /**< verbosity level (NO_ERR,ERR,VERB) */
                        )                      /** int array read or NULL on error */
{
  FILE *file;
  int *array;
  LPJfile *lpjfile;
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
  array=fscanvarintarray(lpjfile,size,key,verb);
  closeconfig(lpjfile);
  return array;
} /* of 'getintarrayfromjson' */
