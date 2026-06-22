/**************************************************************************************/
/**                                                                                \n**/
/**             b  s  t  r  u  c  t  _  g  e  t  t  y  p  e  .  c                  \n**/
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

int bstruct_gettype(Bstruct bstr,   /**< pointer to restart file */
                    const char *name /**< name to get type from */
                   )                /** \return type or BSTRUCT_NOTFOUND */
{
  /* Function gets type of name */
  long long pos;
  Bool rc;
  Byte token,isout;
  /* store file position */
  pos=ftell(bstr->file);
  /* read token */
  if(fread(&token,1,1,bstr->file)!=1)
    return BSTRUCT_NOTFOUND;
  if(bstruct_isinvalidtoken(token))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR502: Invalid token %d getting for object '%s'.\n",
              token,bstruct_getname(name));
    return FALSE;
  }
  isout=bstr->isout;
  /* temporarily switch off error messages */
  bstr->isout=FALSE;
  rc=bstruct_findobject(bstr,&token,BSTRUCT_BYTE,name);
  bstr->isout=isout;
  /* restore position in file */
  fseek(bstr->file,pos,SEEK_SET);
  if(rc)
    return BSTRUCT_NOTFOUND;
  token&=63; /* strip top 2 bits in token */
  switch(token)
  {
    case BSTRUCT_BEGINARRAY1:
      return BSTRUCT_ARRAY;
    case BSTRUCT_STRING1:
      return BSTRUCT_STRING;
    case BSTRUCT_FALSE:
      return BSTRUCT_BOOL;
    case BSTRUCT_FZERO:
      return BSTRUCT_FLOAT;
    case BSTRUCT_DZERO:
      return BSTRUCT_DOUBLE;
    default:
      return token;
  }
} /* of bstruct_gettype' */
