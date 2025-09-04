/**************************************************************************************/
/**                                                                                \n**/
/**     b  s  t  r  u  c  t  _  r  e  a  d  b  e  g  i  n  a  r  r  a  y  .  c     \n**/
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

Bool bstruct_readbeginarray(Bstruct bstr,     /**< pointer to restart file */
                            const char *name, /**< name of object or NULL */
                            int *size         /**< [out] size of array */
                           )                  /** \return TRUE on error */
{
  Byte b;
  if(bstruct_readtoken(bstr,&b,BSTRUCT_BEGINARRAY,name))
    return TRUE;
  if(bstruct_findobject(bstr,&b,BSTRUCT_BEGINARRAY,name))
    return TRUE;
  if(bstr->level==MAXLEVEL-1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR515: Too deep nesting of arrays, %d allowed.\n",MAXLEVEL);
    bstruct_freenamestack(bstr);
    return TRUE;
  }
  /* push object on name stack */
  bstr->namestack[bstr->level].varnames=NULL;
  bstr->namestack[bstr->level].type=BSTRUCT_BEGINARRAY;
  bstr->namestack[bstr->level].nr=0;
  /* store name of array */
  bstr->namestack[bstr->level].name=(name==NULL) ? NULL : strdup(name);
  b&=63; /* strip top 2 bits in token */
  if(b==BSTRUCT_BEGINARRAY1)
  {
    if(fread(&b,1,1,bstr->file)!=1)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR512: Cannot read array length of '%s'.\n",getname(name));
      return TRUE;
    }
    *size=b;
    bstr->namestack[bstr->level++].size=*size;
    return FALSE;
  }
  if(b!=BSTRUCT_BEGINARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not array.\n",
              getname(name),bstruct_typenames[b]);
    return TRUE;
  }
  if(freadint(size,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading array size of '%s'.\n",
              getname(name));
    return TRUE;
  }
  if(*size<0)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR526: Invalid array length %d of '%s'.\n",
              *size,getname(name));
    return TRUE;
  }
  bstr->namestack[bstr->level++].size=*size;
  return FALSE;
} /* of 'bstruct_readnbeginarray' */
