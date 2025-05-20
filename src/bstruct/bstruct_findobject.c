/**************************************************************************************/
/**                                                                                \n**/
/**              b  s  t  r  u  c  t  _  f  i  n  d  o  b  j  e  c  t  .  c        \n**/
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

static long long findname(Bstruct bstruct,         /**< pointer to restart file */
                                  Byte *token,     /**< token of found object */
                                  short id         /**< name to search for */
                                 )                 /** \return file position of object found or -1 */
{
  /* Function finds name in list of already read names of current struct */
  Var *var;
  int i;
  if(bstruct->isout)
    bstruct->imiss++;
  foreachlistitem(i,bstruct->namestack[bstruct->level-1].varnames)
  {
    var=getlistitem(bstruct->namestack[bstruct->level-1].varnames,i);
    if(id==var->id)
    {
      /* name found, return token and file position of object */
      *token=var->token;
      return var->filepos;
    }
  }
  return -1; /* name not found */
}  /* of 'findname' */

Bool bstruct_findobject(Bstruct bstr,        /**< pointer to restart file */
                        Byte *token,         /**< token matching name */
                        Byte token_expected, /**< token expected */
                        const char *name     /**< name to search for */
                       )                     /** \return TRUE if name was not found */
{
  /* Function finds object with specified name in struct */
  short *id,id2;
  Hashitem *item,key;
  Var *var;
  long long filepos;
  Byte id1;
#ifdef DEBUG_BSTRUCT
  printf("Looking for %s\n",name);
#endif
  bstr->namestack[bstr->level-1].nr++;
  if(name==NULL)
  {
    if(bstr->namestack[bstr->level-1].type==BSTRUCT_BEGINSTRUCT)
    {
      if(bstr->isout)
      {
        fprintf(stderr,"ERROR521: Object name for %s must be specified in struct '%s'.\n",
               bstruct_typenames[token_expected],getname(bstr->namestack[bstr->level-1].name));
        bstruct_printnamestack(bstr);
      }
      return TRUE;
    }
    if((*token & 128)==128)
    {
      if(bstr->isout)
      {
        fprintf(stderr,"ERROR504: Expected no object name for %s, but name found.\n",
               bstruct_typenames[token_expected]);
        bstruct_printnamestack(bstr);
      }
      return TRUE;
    }
  }
  else
  {
    if(bstr->namestack[bstr->level-1].type==BSTRUCT_BEGINARRAY)
    {
      if(bstr->isout)
      {
        fprintf(stderr,"ERROR521: Object name '%s' for %s not allowed in array '%s'.\n",name,
                bstruct_typenames[token_expected],getname(bstr->namestack[bstr->level-1].name));
        bstruct_printnamestack(bstr);
      }
      return TRUE;
    }
    key.key=strdup(name);
    if(key.key==NULL)
    {
      printallocerr("key");
      return TRUE;
    }
    /* Using binary search for finding the name in the ordered name table */
    item=bsearch(&key,bstr->names,bstr->count,sizeof(Hashitem),bstruct_cmpname);
    free(key.key);
    if(item==NULL)
    {
      /* not found, return with error */
      if(bstr->isout)
      {
        fprintf(stderr,"ERROR506: Object name '%s' for %s in struct '%s' not found in name table.\n",
                name,bstruct_typenames[token_expected],getname(bstr->namestack[bstr->level-1].name));
        bstruct_printnamestack(bstr);
      }
      /* undo last read */
      fseek(bstr->file,-1,SEEK_CUR);
      return TRUE;
    }
    id=item->data;
#ifdef DEBUG_BSTRUCT
    printf("Object '%s' is %d\n",name,*id);
#endif
    if(*token==BSTRUCT_ENDSTRUCT || *token==BSTRUCT_END)
    {
       /* find name in the list of already read objects */
       filepos=findname(bstr,token,*id);
       if(filepos==-1)
       {
         /* not found, return with error */
         if(bstr->isout)
         {
           fprintf(stderr,"ERROR506: Object '%s' for %s not found in struct '%s'.\n",
                   name,bstruct_typenames[token_expected],getname(bstr->namestack[bstr->level-1].name));
           bstruct_printnamestack(bstr);
         }
         /* undo last read */
         fseek(bstr->file,-1,SEEK_CUR);
         return TRUE;
       }
       /* found, goto object position */
       fseek(bstr->file,filepos,SEEK_SET);
       return FALSE;
    }
    /* read object name */
    if((*token & 128)==128) /* top bit in token set, object name stored in next byte or short */
    {
      if((*token & 64)==64) /* bit 7 set, id is of type short */
      {
        if(freadshort(&id2,1,bstr->swap,bstr->file)!=1)
        {
          if(bstr->isout)
          {
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name for %s.\n",
                    bstruct_typenames[token_expected]);
            bstruct_printnamestack(bstr);
          }
          return TRUE;
        }
      }
      else
      {
        if(fread(&id1,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
          {
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name for %s.\n",
                    bstruct_typenames[token_expected]);
            bstruct_printnamestack(bstr);
          }
          return TRUE;
        }
        id2=id1;
      }
#ifdef DEBUG_BSTRUCT
      printf("Object is %d\n",id2);
#endif
    }
    else
    {
      if(bstr->isout)
      {
        fprintf(stderr,"ERROR504: Expected object name '%s' for %s, but no name found.\n",name,
                bstruct_typenames[token_expected]);
        bstruct_printnamestack(bstr);
      }
      return TRUE;
    }
    if(*id!=id2)
    {
      /* name not found, search for it */
      do
      {
        var=new(Var);
        var->filepos=ftell(bstr->file);
        var->token=*token;
        var->id=id2;
        /* add name and position to the list of already read objects */
        addlistitem(bstr->namestack[bstr->level-1].varnames,var);
#ifdef DEBUG_BSTRUCT
        printf("%s=%d not found, %d\n",name,*id,id2);
#endif
        if(bstruct_skipdata(bstr,*token))
        {
          if(bstr->isout)
          {
            fprintf(stderr,"ERROR505: Cannot skip to next object, object '%s' for %s not found.\n",
                    name,bstruct_typenames[token_expected]);
            bstruct_printnamestack(bstr);
          }
          return TRUE;
        }
        /* read next token */
        if(fread(token,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
          {
            fprintf(stderr,"ERROR508: Unexpected end of file reading token at searching for name '%s' of %s.\n",
                    name,bstruct_typenames[token_expected]);
            bstruct_printnamestack(bstr);
          }
          return TRUE;
        }
        if(*token==BSTRUCT_ENDSTRUCT || *token==BSTRUCT_END)
        {
          /* find name in the list of already read objects */
          filepos=findname(bstr,token,*id);
          if(filepos==-1)
          {
            /* not found, return with error */
            if(bstr->isout)
            {
              fprintf(stderr,"ERROR506: Object '%s' for %s not found in struct '%s'.\n",
                      name,bstruct_typenames[token_expected],
                      getname(bstr->namestack[bstr->level-1].name));
              bstruct_printnamestack(bstr);
            }
            /* undo last read */
            fseek(bstr->file,-1,SEEK_CUR);
            return TRUE;
          }
          /* found, goto object position */
          fseek(bstr->file,filepos,SEEK_SET);
          return FALSE;
        }
        if(*token==BSTRUCT_ENDARRAY)
        {
          if(bstr->isout)
          {
            fprintf(stderr,"ERROR506: Object '%s' for %s not found.\n",name,
                    bstruct_typenames[token_expected]);
            bstruct_printnamestack(bstr);
          }
          return TRUE;
        }
        /* read next name */
        if((*token & 128)==128) /* top bit in token set, object name stored in next byte or short */
        {
          if((*token & 64)==64) /* bit 7 set, id is of type short */
          {
            if(freadshort(&id2,1,bstr->swap,bstr->file)!=1)
            {
              if(bstr->isout)
              {
                fprintf(stderr,"ERROR508: Unexpected end of file reading object name for %s.\n",
                        bstruct_typenames[token_expected]);
                bstruct_printnamestack(bstr);
              }
              return TRUE;
            }
          }
          else
          {
            if(fread(&id1,1,1,bstr->file)!=1)
            {
              if(bstr->isout)
              {
                fprintf(stderr,"ERROR508: Unexpected end of file reading object name for %s.\n",
                        bstruct_typenames[token_expected]);
                bstruct_printnamestack(bstr);
              }
              return TRUE;
            }
            id2=id1;
          }
        }
      }while(*id!=id2);
    }
    else
    {
      var=new(Var);
      var->filepos=ftell(bstr->file);
      var->token=*token;
      var->id=id2;
      addlistitem(bstr->namestack[bstr->level-1].varnames,var);
    }
  }
  return FALSE;
} /* of 'bstruct_findobject' */
