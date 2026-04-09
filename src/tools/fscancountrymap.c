/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  c  o  u  n  t  r  y  m  a  p  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads counry map from  JSON file                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Countryname *fscancountrymap(LPJfile *file,      /**< pointer to JSON file */
                             int *size,          /**< size of array */
                             const char *key,    /**< name of country map array */
                             Verbosity verbosity /**< verbosity level */
                            )                    /** \return allocated country map or NULL  on error */
{
  Countryname *countrymap;
  LPJfile *array,*item,*str;
  const char *name;
  int i;
  array=fscanarray(file,size,key,verbosity);
  if(array==NULL)
    return NULL;
  countrymap=newvec(Countryname,*size);
  if(countrymap==NULL)
  {
    printallocerr("countrymap");
    return NULL;
  }
  for(i=0;i<*size;i++)
  {
    item=fscanarrayindex(array,i);
    str=fscanstruct(item,NULL,verbosity);
    if(str==NULL)
      return NULL;
    name=fscanstring(str,NULL,"name",verbosity);
    if(name==NULL)
      return NULL;
    countrymap[i].name=strdup(name);
    name=fscanstring(str,NULL,"alpha-3",verbosity);
    if(name==NULL)
      return NULL;
    countrymap[i].alpha_3=strdup(name);
  }
  return countrymap;
} /* of 'fscancountrymap' */
