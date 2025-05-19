/**************************************************************************************/
/**                                                                                \n**/
/**   b  s  t  r  u  c  t  _  r  e  a  d  d  o  u  b  l  e  a  r  r  a  y  .  c    \n**/
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

Bool bstruct_readdoublearray(Bstruct bstr,     /**< pointer to restart file */
                             const char *name, /**< name of object or NULL */
                             double data[],     /**< array read from file */
                             int size          /**< size of array */
                            )                  /** \return TRUE on error */
{
  int i,n;
  if(bstruct_readbeginarray(bstr,name,&n))
    return TRUE;
  if(size!=n)
  {
    if(bstr->isout)
    {
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",
              getname(name),n,size);
      bstruct_printnamestack(bstr);
    }
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readdouble(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr,name);
} /* of 'bstruct_readdoublearray' */
