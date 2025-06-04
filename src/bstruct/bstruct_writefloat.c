/**************************************************************************************/
/**                                                                                \n**/
/**           b  s  t  r  u  c  t  _  w  r  i  t  e  f  l  o  a  t  .  c           \n**/
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

Bool bstruct_writefloat(Bstruct bstr,     /**< pointer to restart file */
                        const char *name, /**< name of object or NULL */
                        float value       /**< value written to file */
                       )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte token;
  token=(value==0) ? BSTRUCT_FZERO : BSTRUCT_FLOAT;  // only non-zero values are written to reduce file size
  if(bstruct_writename(bstr,token,name))
    return TRUE;
  if(value!=0.0)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writefloat' */
