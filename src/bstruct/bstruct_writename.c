/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  w  r  i  t  e  n  a  m  e  .  c      \n**/
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

Bool bstruct_writename(Bstruct bstr,    /**< pointer to restart file */
                       Byte token,      /**< token written to file */
                       const char *name /**< object name written to file or NULL */
                      )                 /** \return TRUE on error */
{
  /* Function writes token and id of name of object into restart file */
  int len,count;
  short *id;
  Byte id1;
  char *s;
  bstr->namestack[bstr->level-1].nr++;
  len=(name==NULL) ? 0 : strlen(name);
  if(len==0)
  {
    if(bstr->namestack[bstr->level-1].type==BSTRUCT_BEGINSTRUCT)
    {
      fprintf(stderr,"ERROR521: Object name for %s must be specified in struct '%s'.\n",
              bstruct_typenames[token],
              getname(bstr->namestack[bstr->level-1].name));
      bstruct_printnamestack(bstr);
    }
    return fwrite(&token,1,1,bstr->file)!=1;
  }
  else
  {
    if(bstr->namestack[bstr->level-1].type==BSTRUCT_BEGINARRAY)
    {
      fprintf(stderr,"ERROR521: Object name '%s' for %s not allowed in array '%s'.\n",name,
              bstruct_typenames[token],
              getname(bstr->namestack[bstr->level-1].name));
      bstruct_printnamestack(bstr);
    }
  }
  if(len>UCHAR_MAX)
  {
    fprintf(stderr,"ERROR510: String too long for key, must be less than 256.\n");
    return TRUE;
  }
  token|=128; /* set top bit in token */
  fwrite(&token,1,1,bstr->file);
  /* Look in hash table whether name has already been used */
  id=gethashitem(bstr->hash,name);
  if(id==NULL)
  {
    /* name not found in hash, add name and new id to hash table */
    count=gethashcount(bstr->hash);
    if(count==SHRT_MAX)
    {
      fprintf(stderr,"ERROR518: Maximum number of names=%d in table reached.\n",SHRT_MAX);
      return TRUE;
    }
    id=new(short);
    if(id==NULL)
    {
      printallocerr("id");
      return TRUE;
    }
    *id=count;
    s=strdup(name);
    if(s==NULL)
    {
      free(id);
      printallocerr("name");
      return TRUE;
    }
    if(addhashitem(bstr->hash,s,id)==0)
    {
      free(id);
      free(s);
      printallocerr("hash");
      return TRUE;
    }
  }
  if(*id>UCHAR_MAX)
  {
    token|=64; /* set bit 7 in token */
    return fwrite(id,sizeof(short),1,bstr->file)!=1;
  }
  id1=*id;
  return fwrite(&id1,1,1,bstr->file)!=1;
} /* of 'bstruct_writename' */
