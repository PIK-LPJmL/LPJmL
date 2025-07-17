/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  r  e  a  d  b  y  t  e  .  c         \n**/
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

Bool bstruct_readbyte(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Byte *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte token;
  if(bstruct_readtoken(bstr,&token,BSTRUCT_BYTE,name))
    return TRUE;
  if(bstruct_findobject(bstr,&token,BSTRUCT_BYTE,name))
    return TRUE;
  token&=63; /* strip top 2 bits in token */
  if(token==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(token!=BSTRUCT_BYTE)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not byte.\n",
              getname(name),bstruct_typenames[token]);
    return TRUE;
  }
  if(fread(value,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading bool '%s'.\n",
              getname(name));
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readbyte' */
