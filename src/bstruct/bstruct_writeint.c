/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  w  r  i  t  e  i  n  t  .  c         \n**/
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

Bool bstruct_writeint(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      int value         /**< value written to file */
                     )                  /** \return TRUE on error */
{
  short s;
  Byte token;
  if(value==0)
    token=BSTRUCT_ZERO;  // only non-zero values are written to reduce file size
  else if(value>=0 && value<=UCHAR_MAX)
    token=BSTRUCT_BYTE;
  else if(value>=SHRT_MIN && value<=SHRT_MAX)
    token=BSTRUCT_SHORT;
  else
    token=BSTRUCT_INT;
  if(bstruct_writename(bstr,token,name))
    return TRUE;
  switch(token)
  {
    case BSTRUCT_BYTE:
      token=value;
      return fwrite(&token,1,1,bstr->file)!=1;
    case BSTRUCT_SHORT:
      s=value;
      return fwrite(&s,sizeof(short),1,bstr->file)!=1;
    case BSTRUCT_INT:
      return fwrite(&value,sizeof(value),1,bstr->file)!=1;
    default:
      break;
  }
  return FALSE;
} /* of 'bstruct_writeint' */
