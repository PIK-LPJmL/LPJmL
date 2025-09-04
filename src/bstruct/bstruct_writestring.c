/**************************************************************************************/
/**                                                                                \n**/
/**          b  s  t  r  u  c  t  _  w  r  i  t  e  s  t  r  i  n  g  .  c         \n**/
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

Bool bstruct_writestring(Bstruct bstr,     /**< pointer to restart file */
                         const char *name, /**< name of object or NULL */
                         const char *value /**< string written to file */
                        )                  /** \return TRUE on error */
{
  /* Function writes string to restart file */
  int len;
  Byte b;
  len=(value==NULL ) ? 0 : strlen(value);
  b=(len<=UCHAR_MAX) ? BSTRUCT_STRING1 : BSTRUCT_STRING;
  if(bstruct_writename(bstr,b,name))
    return TRUE;
  if(len<=UCHAR_MAX)
  {
    b=len;
    fwrite(&b,1,1,bstr->file);
  }
  else
    fwrite(&len,sizeof(len),1,bstr->file);
  return fwrite(value,1,len,bstr->file)!=len;
} /* of 'bstruct_writestring' */
