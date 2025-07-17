/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  r  e  a  d  r  e  a  l  .  c         \n**/
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

Bool bstruct_readreal(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Real *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte token;
  if(bstruct_readtoken(bstr,&token,(sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT,name))
    return TRUE;
  if(bstruct_findobject(bstr,&token,(sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT,name))
    return TRUE;
  token&=63; /* strip top 3 bits in token */
  if(token==BSTRUCT_FZERO)
  {
    *value=0;
    return FALSE;
  }
  if(token!=((sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not real.\n",
              getname(name),bstruct_typenames[token]);
    return TRUE;
  }
  if(freadreal(value,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading double '%s'.\n",
              getname(name));
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readreal' */
