/**************************************************************************************/
/**                                                                                \n**/
/**    b s t r u c t _ w r i t e b e g i n i n d e x a r r a y . c                 \n**/
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

Bool bstruct_writebeginindexarray(Bstruct bstr,       /**< pointer to restart file */
                                  const char *name,   /**< name of object */
                                  long long *filepos, /**< [out] file position of index vector */
                                  int size            /**< size of array */
                                 )                    /** \return TRUE on error */
{
  /* define array with index vector and get position of first element of index vector */
  Byte token;
  if(bstruct_writebeginarray(bstr,name,size))
    return TRUE;
  token=BSTRUCT_INDEXARRAY;
  fwrite(&token,1,1,bstr->file);
  if(fwrite(&size,sizeof(size),1,bstr->file)!=1)
    return TRUE;
  *filepos=ftell(bstr->file);
  return fseek(bstr->file,sizeof(long long)*size,SEEK_CUR);
} /* of 'bstruct_writebeginindexarray' */
