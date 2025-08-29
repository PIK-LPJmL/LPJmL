/**************************************************************************************/
/**                                                                                \n**/
/**              b  s  t  r  u  c  t  _  r  e  a  d  b  o  o  l  .  c              \n**/
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

Bool bstruct_readbool(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Bool *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte token;
  if(bstruct_readtoken(bstr,&token,BSTRUCT_FALSE,name))
    return TRUE;
  if(bstruct_findobject(bstr,&token,BSTRUCT_FALSE,name))
    return TRUE;
  token&=63;
  switch(token)
  {
    case BSTRUCT_FALSE:
      *value=FALSE;
      return FALSE;
    case BSTRUCT_TRUE:
      *value=TRUE;
      return FALSE;
    default:
      if(bstr->isout)
        fprintf(stderr,"ERROR509: Type of '%s'=%s is not bool.\n",
                getname(name),bstruct_typenames[token & 63]);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readbool' */
