/**************************************************************************************/
/**                                                                                \n**/
/**             b  s  t  r  u  c  t  _  i  s  n  u  l  l   .  c                    \n**/
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

Bool bstruct_isnull(Bstruct bstr,    /**< pointer to restart file */
                    const char *name /**< name of object or NULL */
                   )                 /** \return TRUE if is null object */
{
  /* Function checks if object is null ´*/
  long long pos;
  Bool rc;
  Byte token;
  /* store file position */
  pos=ftell(bstr->file);
  /* read token */
  if(fread(&token,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading token.\n");
    return FALSE;
  }
  rc=bstruct_findobject(bstr,&token,BSTRUCT_NULL,name);
  if(!rc && (token & 63)==BSTRUCT_NULL)
    return TRUE;
  /* restore position in file */
  fseek(bstr->file,pos,SEEK_SET);
  return FALSE;
} /* of bstruct_isnull' */
