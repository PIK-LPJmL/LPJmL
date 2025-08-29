/**************************************************************************************/
/**                                                                                \n**/
/**  b  s  t  r  u  c  t  _  r  e  a  d  v  a  r  r  e  a  l  a  r  r  a  y  .  c  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for reading/writing JSON-like objects from binary file           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "bstruct_intern.h"

Real *bstruct_readvarrealarray(Bstruct bstr,     /**< pointer to restart file */
                               const char *name, /**< name of object or NULL */
                               int *size         /**< [out] size of array */
                              )                  /** \return pointer to array read or NULL on error */
{
  Real *data;
  int i;
  if(bstruct_readbeginarray(bstr,name,size))
    return NULL;
  data=newvec(Real,*size);
  if(data==NULL)
  {
    printallocerr("data");
    return NULL;
  }
  for(i=0;i<*size;i++)
    if(bstruct_readreal(bstr,NULL,data+i))
    {
      free(data);
      return NULL;
    }
  if(bstruct_readendarray(bstr,name))
  {
    free(data);
    return NULL;
  }
  return data;
} /* of 'bstruct_readvarrealarray' */
