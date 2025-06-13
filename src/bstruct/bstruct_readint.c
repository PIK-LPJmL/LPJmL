/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  r  e  a  d  i  n  t  .  c            \n**/
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

Bool bstruct_readint(Bstruct bstr,     /**< pointer to restart file */
                     const char *name, /**< name of object or NULL */
                     int *value        /**< value read from file */
                    )                  /** \return TRUE on error */
{
  unsigned short us;
  short s;
  Byte token;
  if(bstruct_readtoken(bstr,&token,BSTRUCT_INT,name))
    return TRUE;
  if(bstruct_findobject(bstr,&token,BSTRUCT_INT,name))
    return TRUE;
  token&=63; /* strip top 2 bits in token */
  switch(token)
  {
    case BSTRUCT_ZERO:
      *value=0;
      return FALSE;
    case BSTRUCT_BYTE:
      if(fread(&token,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading int '%s'.\n",
                  getname(name));
        return TRUE;
      }
      *value=token;
      return FALSE;
    case BSTRUCT_SHORT:
      if(freadshort(&s,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading int '%s'.\n",
                  getname(name));
        return TRUE;
      }
      *value=s;
      return FALSE;
    case BSTRUCT_USHORT:
      if(freadushort(&us,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading int '%s'.\n",
                  getname(name));
        return TRUE;
      }
      *value=us;
      return FALSE;
    case BSTRUCT_INT:
      if(freadint(value,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading int '%s'.\n",
                  getname(name));
        return TRUE;
      }
      return FALSE;
    default:
      if(bstr->isout)
        fprintf(stderr,"ERROR509: Type of '%s'=%s is not int.\n",
                getname(name),bstruct_typenames[token]);
      return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readint' */
