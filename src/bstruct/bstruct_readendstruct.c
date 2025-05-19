/**************************************************************************************/
/**                                                                                \n**/
/**       b  s  t  r  u  c  t  _  r  e  a  d  e  n  d  s  t  r  u  c  t  .  c      \n**/
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

Bool bstruct_readendstruct(Bstruct bstr,    /**< pointer to restart file */
                           const char *name /**< object name */
                          )                 /** \return TRUE on error */
{
  /* Function ends current struct and skips all remaining data in struct */
  int i;
  Byte token;
  if(fread(&token,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading token for endstruct of '%s'.\n",
              getname(name));
    return TRUE;
  }
  while(token!=BSTRUCT_ENDSTRUCT)
  {
    if(token==BSTRUCT_END)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR508: Unexpected end token for endstruct of '%s'.\n",
                getname(name));
    }
#ifdef DEBUG_BSTRUCT
    printf("skip token %s in readendstruct\n",bstruct_typenames[token & 63]);
#endif
    if(token!=BSTRUCT_ENDARRAY)
    {
      /* skip object name */
      if((token & 128)==128) /* all top 4 bits in token set, object name length stored in next byte */
      {
        if(fseek(bstr->file,((token & 64)==64) ? sizeof(short) : 1,SEEK_CUR))
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR507: Unexpected end of file skipping object name processing endstruct of '%s'.\n",
                    getname(name));
          return TRUE;
        }
      }
      if(bstruct_skipdata(bstr,token))
        return TRUE;
      if(fread(&token,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading token for endstruct of '%s'.\n",
                  getname(name));
        return TRUE;
      }
    }
    else
    {
      if(bstr->isout)
      {
        fprintf(stderr,"ERROR503: End of array found, endstruct of '%s' expected.\n",
                getname(name));
        printnamestack(bstr);
      }
      return TRUE;
    }
  } /* of while(token!=BSTRUCT_ENDSTRUCT) */
  if(bstr->level)
  {
    /* check for match name for struct and endstruct */
    if(name==NULL)
    {
      if(bstr->namestack[bstr->level-1].name!=NULL)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR520: Endstruct of '%s' found, but unnamed expected.\n",
                  name);
        return TRUE;
      }
    }
    else
    {
      if(bstr->namestack[bstr->level-1].name==NULL)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR520: Unnamed endstruct found, but '%s' expected.\n",
                  name);
        return TRUE;
      }
      else if(strcmp(name,bstr->namestack[bstr->level-1].name))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR520: Endstruct of '%s' found, but '%s' expected.\n",
                  bstr->namestack[bstr->level-1].name,name);
        return TRUE;
      }
    }
    /* remove list of names for this level */
    foreachlistitem(i,bstr->namestack[bstr->level-1].varnames)
    {
      free(getlistitem(bstr->namestack[bstr->level-1].varnames,i));
    }
    freelist(bstr->namestack[bstr->level-1].varnames);
    free(bstr->namestack[bstr->level-1].name);
    bstr->level--;
  }
  else if(bstr->level==0)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR516: Too many endstructs found.\n");
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readendstruct' */
