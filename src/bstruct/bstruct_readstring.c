/**************************************************************************************/
/**                                                                                \n**/
/**              b  s  t  r  u  c  t  _  r  e  a  d  s  t  r  i  n  g  .  c        \n**/
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

char *bstruct_readstring(Bstruct bstr,    /**< pointer to restart file */
                         const char *name /**< name of object or NULL */
                        )                 /**< \return pointer to string read or NULL on error */
{
  /* Function reads string from restart file */
  char *s;
  int len;
  Byte token,len1;
  if(bstruct_readtoken(bstr,&token,BSTRUCT_STRING,name))
    return NULL;
  if(bstruct_findobject(bstr,&token,BSTRUCT_STRING,name))
    return NULL;
  token&=63; /* strip top 4 bits in token */
  switch(token)
  {
    case BSTRUCT_STRING:
      if(freadint(&len,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR512: Cannot read string length of '%s'.\n",getname(name));
        return NULL;
      }
      if(len<0)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR526: Invalid string length %d of '%s'.\n",
                  len,getname(name));
        return NULL;
      }
      break;
    case BSTRUCT_STRING1:
      if(fread(&len1,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR512: Cannot read string length of '%s'.\n",getname(name));
        return NULL;
      }
      len=len1;
      break;
    default:
      if(bstr->isout)
        fprintf(stderr,"ERROR509: Type of '%s'=%s is not string.\n",
                getname(name),bstruct_typenames[token]);
      return NULL;
  }
  s=malloc(len+1);
  if(s==NULL)
  {
    printallocerr("string");
    return NULL;
  }
  if(fread(s,1,len,bstr->file)!=len)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR512: Cannot read string '%s' of length %d.\n",
              getname(name),len);
    free(s);
    return NULL;
  }
  s[len]='\0';
  return s;
} /* of 'bstruct_readstring' */
