/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  .  c                                    \n**/
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "types.h"
#include "errmsg.h"
#include "swap.h"
#include "list.h"
#include "hash.h"
#include "bstruct.h"

//#define DEBUG_BSTRUCT

#define BSTRUCT_HEADER "BSTRUCT"
#define BSTRUCT_VERSION 1
#define BSTRUCT_MAXTOKEN BSTRUCT_ENDARRAY
#define BSTRUCT_HASHSIZE 1023
#define MAXLEVEL 15 /**< maximum number of nested structs */

static const size_t bstruct_typesizes[]={1,sizeof(short),sizeof(int),sizeof(float),
                                         sizeof(double),0,0,sizeof(unsigned short),0};

char *bstruct_typenames[]={"byte","short","int","float","double","bool","bool","ushort",
                           "zero","string","string1","array","array1","struct","indexarray",
                           "endstruct","endarray"};

#define getname(name) (name==NULL) ? "unnamed" : name

struct bstruct
{
  FILE *file; /**< pointer to binary file */
  Bool swap;  /**< byte order has to be changed at reading */
  Bool isout; /**< error output on stderr enabled */
  int level;  /**< number of nested structs */
  int imiss;  /**< number of objects not in right order */
  struct
  {
    char *name;         /**< name of struct */
    List *varnames;     /**< list of objects in struct */
  } namestack[MAXLEVEL]; /**< list of objects names for each nested level */
  Hash hash;  /**< hash for object names used for writing restart files */
  int count;       /**< size of name table */
  Hashitem *names; /**< name table used for reading restart files */
};            /**< Definition of opaque datatype Bstruct */

typedef struct
{
  long long filepos; /**< position of object in file */
  short id;          /**< name of object */
  Byte token;        /**< token of object */
} Var;

static int hashkey(const void *ptr,int size)
{
  /* Function converts string into hash value */
  const unsigned char *item=ptr;
  unsigned int key;
  key=0;
  while(*item!='\0')
    key=(key*256+(*item++)) % size;
  return key;
} /* of 'hashkey' */

static int cmpname(const void *ptr1,const void *ptr2)
{
  const Hashitem *h1=ptr1;
  const Hashitem *h2=ptr2;
  return strcmp(h1->key,h2->key);
} /* of 'cmpnanme' */

static int cmpkey(const void *ptr1,const void *ptr2)
{
  return strcmp((const char *)ptr1,(const char *)ptr2);
} /* of 'cmpkey' */

static void freenamestack(Bstruct bstruct)
{
  /* Function deallocates stack of already read objects */
  int i,j;
  for(i=0;i<bstruct->level;i++)
  {
    free(bstruct->namestack[i].name);
    foreachlistitem(j,bstruct->namestack[i].varnames)
      free(getlistitem(bstruct->namestack[i].varnames,j));
    freelist(bstruct->namestack[i].varnames);
  }
  bstruct->level=0;
} /* of 'freenamestack' */

static long long findname(Bstruct bstruct, /**< pointer to restart file */
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

static Bool readtoken(Bstruct bstr,     /**< pointer to restart file */
                      Byte *token_read, /**< token read from file */
                      Byte token,       /**< token expected */
                      const char *name  /**< name of object expected */
                     )                  /** \return TRUE on error */
{
  /* Function reads token from file */
  if(fread(token_read,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR501: Cannot read '%s': %s.\n",
              name,strerror(errno));
    return TRUE;
  }
  if((*token_read & 63)>BSTRUCT_MAXTOKEN)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR502: Invalid token %d reading %s.\n",
                *token_read,bstruct_typenames[token & 63]);
      else
        fprintf(stderr,"ERROR502: Invalid token %d reading %s '%s'.\n",
                *token_read,bstruct_typenames[token & 63],name);
     }
     return TRUE;
  }
  if(*token_read==BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR503: End of array found, %s expected.\n",
                bstruct_typenames[token & 63]);
      else
        fprintf(stderr,"ERROR506: Object '%s' not found in array.\n",name);
    }
    return TRUE;
  }
  return FALSE;
} /* of 'readtoken' */

static Bool skipdata(Bstruct bstr, /**< pointer to restart file */
                     Byte token    /**< token */
                    )              /** \return TRUE on error */
{
  /* Function skips one object in restart file */
  int string_len;
  Byte len,b;
  if((token & 63)>BSTRUCT_MAXTOKEN)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR502: Invalid token %d skipping data.\n",token);
    return TRUE;
  }
  switch(token & 63) /* strip top 2 bits */
  {
    case BSTRUCT_STRUCT:
      do
      {
        /* skip whole struct */
        if(fread(&b,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading token in struct.\n");
          return TRUE;
        }
        if((b & 63)>BSTRUCT_MAXTOKEN)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR502: Invalid token %d skipping data.\n",b);
          return TRUE;
        }
#ifdef DEBUB_BSTRUCT
        printf("type %s\n",bstruct_typenames[b & 63]);
#endif
        if(b==BSTRUCT_ENDARRAY)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of array token found in struct.\n");
          return TRUE;
        }
        if(b!=BSTRUCT_ENDSTRUCT)
        {
          /*skip object name */
          if((b & 128)==128) /* top bit in token set, object name stored in next byte or short */
          {
            if(fseek(bstr->file,((b & 64)==64) ? sizeof(short) : 1,SEEK_CUR))
            {
              if(bstr->isout)
                fprintf(stderr,"ERROR507: Unexpected end of file skipping object name.\n");
              return TRUE;
            }
          }
          /* call skipdata() recursively */
          if(skipdata(bstr,b))
            return TRUE;
        }
      } while(b!=BSTRUCT_ENDSTRUCT);
      break;
    case BSTRUCT_ARRAY: case BSTRUCT_ARRAY1:
      /* skip array size */
      if(fseek(bstr->file,((token & 63)==BSTRUCT_ARRAY1) ? 1 : sizeof(int),SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping array length.\n");
        return TRUE;
      }
      /* skip whole array */
      do
      {
        if(fread(&b,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading token in array.\n");
          return TRUE;
        }
        if((b & 63)>BSTRUCT_MAXTOKEN)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR502: Invalid token %d skipping data.\n",b);
          return TRUE;
        }
        if(b==BSTRUCT_ENDSTRUCT)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of struct token in array found.\n");
          return TRUE;
        }
        if(b==BSTRUCT_INDEXARRAY)
        {
          if(freadint(&string_len,1,bstr->swap,bstr->file)!=1)
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR508: Unexpected end of file reading index array length.\n");
            return TRUE;
          }
          if(fseek(bstr->file,sizeof(long long)*string_len,SEEK_CUR))
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR507: Unexpected end of file skipping index array of size %d.\n",
                      string_len);
            return TRUE;
          }
        }
        else if(b!=BSTRUCT_ENDARRAY)
        {
          /* skip object name */
          if((b & 128)==128) /* top bit in token set, object name stored in next byte or short */
          {
            if(fseek(bstr->file,((b & 64)==64) ? sizeof(short) : 1,SEEK_CUR))
            {
              if(bstr->isout)
                fprintf(stderr,"ERROR507: Unexpected end of file skipping object name.\n");
              return TRUE;
            }
          }
          /* call skipdata() recursively */
          if(skipdata(bstr,b))
            return TRUE;
        }
      } while(b!=BSTRUCT_ENDARRAY);
      break;
    case BSTRUCT_STRING:
      if(freadint(&string_len,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading string length.\n");
        return TRUE;
      }
      if(fseek(bstr->file,string_len,SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping string of length %d.\n",
                  string_len);
        return TRUE;
      }
      break;
    case BSTRUCT_STRING1:
      if(fread(&len,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading string length.\n");
        return TRUE;
      }
      if(fseek(bstr->file,len,SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping string of length %d.\n",
                  len);
        return TRUE;
      }
      break;
    default:
      /* skip object data */
      if(fseek(bstr->file,bstruct_typesizes[token & 63],SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR507: Unexpected end of file skipping %s.\n",
                  bstruct_typenames[token & 63]);
        return TRUE;
      }
  } /* of switch(token) */
  return FALSE;
} /* of 'skipdata' */

static Bool findobject(Bstruct bstr,    /**< pointer to restart file */
                       Byte *token,     /**< token matching name */
                       const char *name /**< name to search for */
                      )                 /** \return TRUE if name was not found */
{
  short *id,id2;
  Hashitem *item,key;
  Var *var;
  long long filepos;
  Byte id1;
#ifdef DEBUG_BSTRUCT
  printf("Looking for %s\n",name);
#endif
  if(name==NULL)
  {
    if((*token & 128)==128)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR504: Expected no object name, but name found.\n");
      return TRUE;
    }
  }
  else
  {
    key.key=strdup(name);
    item=bsearch(&key,bstr->names,bstr->count,sizeof(Hashitem),cmpname);
    free(key.key);
    if(item==NULL)
    {
      /* not found, return with error */
      if(bstr->isout)
        fprintf(stderr,"ERROR506: Object '%s' in struct '%s' not found in name table.\n",
                name,getname(bstr->namestack[bstr->level-1].name));
      /* undo last read */
      fseek(bstr->file,-1,SEEK_CUR);
      return TRUE;
    }
    id=item->data;
#ifdef DEBUG_BSTRUCT
    printf("Object '%s' is %d\n",name,*id);
#endif
    if(*token==BSTRUCT_ENDSTRUCT)
    {
       /* find name in the list of already read objects */
       filepos=findname(bstr,token,*id);
       if(filepos==-1)
       {
         /* not found, return with error */
         if(bstr->isout)
           fprintf(stderr,"ERROR506: Object '%s' not found in struct '%s'.\n",
                   name,getname(bstr->namestack[bstr->level-1].name));
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
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name.\n");
          return TRUE;
        }
      }
      else
      {
        if(fread(&id1,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name.\n");
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
        fprintf(stderr,"ERROR504: Expected object name '%s', but no name found.\n",name);
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
        if(skipdata(bstr,*token))
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR505: Cannot skip to next object, '%s' not found.\n",
                    name);
          return TRUE;
        }
        /* read next token */
        if(fread(token,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading token.\n");
          return TRUE;
        }
        if(*token==BSTRUCT_ENDSTRUCT)
        {
          /* find name in the list of already read objects */
          filepos=findname(bstr,token,*id);
          if(filepos==-1)
          {
            /* not found, return with error */
            if(bstr->isout)
              fprintf(stderr,"ERROR506: Object '%s' not found in struct '%s'.\n",
                      name,getname(bstr->namestack[bstr->level-1].name));
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
            fprintf(stderr,"ERROR506: Object '%s' not found.\n",name);
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
                fprintf(stderr,"ERROR508: Unexpected end of file reading object name.\n");
              return TRUE;
            }
          }
          else
          {
            if(fread(&id1,1,1,bstr->file)!=1)
            {
              if(bstr->isout)
                fprintf(stderr,"ERROR508: Unexpected end of file reading object name.\n");
              return TRUE;
            }
            id2=id1;
          }
        }
      }while(*id!=id2);
    }
  }
  return FALSE;
} /* of 'findobject' */

static Bool writename(FILE *file,       /**< pointer to binary file */
                      Hash hash,        /**< hash for object names */
                      Byte token,       /**< token written to file */
                      const String name /**< object name written to file or NULL */
                     )                  /** \return TRUE on error */
{
  /* Function writes token and name of object into restart file */
  int len,count;
  short *id;
  Byte id1;
  char *s;
  len=(name==NULL) ? 0 : strlen(name);
  if(len==0)
    return fwrite(&token,1,1,file)!=1;
  if(len>UCHAR_MAX)
  {
    fprintf(stderr,"ERROR510: String too long for key, must be less than 256.\n");
    return TRUE;
  }
  token|=128; /* set top bit in token */
  fwrite(&token,1,1,file);
  id=gethashitem(hash,name);
  if(id==NULL)
  {
    /* name not found in hash, add name */
    count=gethashcount(hash);
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
      printallocerr("name");
      return TRUE;
    }
    if(addhashitem(hash,s,id)==0)
    {
      printallocerr("hash");
      return TRUE;
    }
  }
  if(*id>UCHAR_MAX)
  {
    token|=64; /* set bit 7 in token */
    return fwrite(id,sizeof(short),1,file)!=1;
  }
  id1=*id;
  return fwrite(&id1,1,1,file)!=1;
} /* of 'writename' */

int bstruct_getmiss(const Bstruct bstruct /**< pointer to restart file */
                   )                      /** \return number of objects not in right order */
{
  return bstruct->imiss;
} /* of 'bstruct_getmiss' */

Bstruct bstruct_create(const char *filename /**< filename of restart file to create */
                      )                     /** \return pointer to restart file or NULL in case of error */
{
  /* Function creates restart file */
  Bstruct bstruct;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=TRUE;
  bstruct->level=0;
  bstruct->count=0;
  bstruct->names=NULL;
  bstruct->file=fopen(filename,"wb");
  if(bstruct->file==NULL)
  {
    printfcreateerr(filename);
    free(bstruct);
    return NULL;
  }
  bstruct->hash=newhash(BSTRUCT_HASHSIZE,hashkey,cmpkey,free);
  if(bstruct->hash==NULL)
  {
    printallocerr("hash");
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  fwritetopheader(bstruct->file,BSTRUCT_HEADER,BSTRUCT_VERSION);
  /* skip space for file position for name table */
  fseek(bstruct->file,sizeof(long long),SEEK_CUR);
  return bstruct;
} /* of 'bstruct_create' */

Bstruct bstruct_open(const char *filename, /**< filename of restart file to open */
                     Bool isout            /**< enable error output on stderr */
                    )                      /** \return pointer to restart file or NULL in case of error */
{
  /* Function opens restart file for reading and reads name table at the end of the file */
  Bstruct bstruct;
  int version,i;
  long long filepos,save;
  Byte len;
  short *id;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=isout;
  bstruct->imiss=0;
  bstruct->level=1;
  bstruct->hash=NULL;
  bstruct->file=fopen(filename,"rb");
  if(bstruct->file==NULL)
  {
    if(isout)
      printfopenerr(filename);
    free(bstruct);
    return NULL;
  }
  version=READ_VERSION;
  if(freadtopheader(bstruct->file,&bstruct->swap,BSTRUCT_HEADER,&version,isout))
  {
    if(isout)
      fprintf(stderr,"ERROR513: Invalid header in file '%s'.\n",filename);
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  if(version!=BSTRUCT_VERSION)
  {
    if(isout)
      fprintf(stderr,"ERROR514: Invalid version %d in file '%s', must be %d.\n",
              version,filename,BSTRUCT_VERSION);
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  bstruct->namestack[0].name=NULL;
  bstruct->namestack[0].varnames=newlist(0);
  if(bstruct->namestack[0].varnames==NULL)
  {
    printallocerr("namestack");
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  if(freadlong(&filepos,1,bstruct->swap,bstruct->file)!=1)
  {
    if(isout)
      fprintf(stderr,"ERROR517: Cannot read pointer to name table.\n");
    fclose(bstruct->file);
    freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  /* save file position and seek to table */
  save=ftell(bstruct->file);
  if(fseek(bstruct->file,filepos,SEEK_SET))
  {
    if(isout)
      fprintf(stderr,"ERROR517: Cannot seek to name table.\n");
    fclose(bstruct->file);
    freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  /* read name table */
  if(freadint(&bstruct->count,1,bstruct->swap,bstruct->file)!=1)
  {
    if(isout)
      fprintf(stderr,"ERROR517: Cannot read size of name table.\n");
    fclose(bstruct->file);
    freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  bstruct->names=newvec(Hashitem,bstruct->count);
  if(bstruct->names==NULL)
  {
    printallocerr("names");
    fclose(bstruct->file);
    freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  for(i=0;i<bstruct->count;i++)
  {
    if(fread(&len,1,1,bstruct->file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR517: Cannot read length of name in table.\n");
      fclose(bstruct->file);
      freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    bstruct->names[i].key=malloc((int)len+1);
    if(bstruct->names[i].key==NULL)
    {
      printallocerr("key");
      fclose(bstruct->file);
      freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    if(fread(bstruct->names[i].key,len,1,bstruct->file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR508: Unexpected end file reading name table of size %d.\n",
               bstruct->count);
      fclose(bstruct->file);
      freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    ((char *)bstruct->names[i].key)[len]='\0';
    id=new(short);
    if(id==NULL)
    {
      printallocerr("id");
      fclose(bstruct->file);
      freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    if(freadshort(id,1,bstruct->swap,bstruct->file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR508: Unexpected end of file reading name table of size %d.\n",
                bstruct->count);
      fclose(bstruct->file);
      freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    bstruct->names[i].data=id;
#ifdef DEBUG_BSTRUCT
    printf("%d: '%s'\n",*id,(char *)bstruct->names[i].key);
#endif
  } /* of for(i=0;i<bstruct->count;i++) */
  /* restore file position */
  fseek(bstruct->file,save,SEEK_SET);
  return bstruct;
} /* of 'bstruct_open' */

FILE *bstruct_getfile(Bstruct bstruct /**< pointer to restart file */
                     )                /** \return pointer to open file */
{
  return bstruct->file;
} /* of 'bstruct_getfile' */

Hash bstruct_gethash(Bstruct bstruct /**< pointer to restart file */
                     )                /** \return pointer to hash */
{
  return bstruct->hash;
} /* of 'bstruct_gethash' */

void bstruct_freehash(Bstruct bstruct /**< pointer to restart file */
                     )
{
  freehash(bstruct->hash);
  bstruct->hash=NULL;
} /* of 'bstruct_freehash' */

Bstruct bstruct_append(const char *filename, /**< filename of restart file to append */
                       Bool isout            /**< enable error output on stderr */
                      )                      /** \return pointer to restart file or NULL in case of error */
{
  /* Function opens restart file and seeks at the end of file to append data */
  Bstruct bstruct;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=isout;
  bstruct->level=0;
  bstruct->count=0;
  bstruct->names=NULL;
  bstruct->file=fopen(filename,"r+b");
  if(bstruct->file==NULL)
  {
    if(isout)
      printfopenerr(filename);
    free(bstruct);
    return NULL;
  }
  fseek(bstruct->file,0,SEEK_END); /* seek to end of file for append */
  bstruct->hash=newhash(BSTRUCT_HASHSIZE,hashkey,cmpkey,free);
  if(bstruct->hash==NULL)
  {
    printallocerr("hash");
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  return bstruct;
} /* of 'bstruct_append' */

void bstruct_close(Bstruct bstruct)
{
  long long filepos;
  int i,count;
  Hashitem *list;
  Byte len;
  short *id;
  /* Function writes name table, closes restart file and frees memory */
  if(bstruct!=NULL)
  {
    filepos=ftell(bstruct->file);
    if(bstruct->hash!=NULL)
    {
      /* convert hash table into array */
      list=hash2array(bstruct->hash);
      if(list==NULL)
      {
        printallocerr("list");
        return;
      }
      /* sort array by names */
      count=gethashcount(bstruct->hash);
      qsort(list,count,sizeof(Hashitem),cmpname);
      /* write name table at end of restart file */
      fwrite(&count,sizeof(int),1,bstruct->file);
      for(i=0;i<gethashcount(bstruct->hash);i++)
      {
        len=strlen(list[i].key);
        fwrite(&len,1,1,bstruct->file);
        fwrite(list[i].key,len,1,bstruct->file);
        id=list[i].data;
        fwrite(id,sizeof(short),1,bstruct->file);
#ifdef DEBUG_BSTRUCT
        printf("%d: '%s'\n",*id,(char *)list[i].key);
#endif
      }
      free(list);
      /* write file position of name table after file header */
      fseek(bstruct->file,strlen(BSTRUCT_HEADER)+sizeof(int),SEEK_SET);
      fwrite(&filepos,sizeof(filepos),1,bstruct->file);
      freehash(bstruct->hash);
    }
    freenamestack(bstruct);
    fclose(bstruct->file);
    /* free name table */
    for(i=0;i<bstruct->count;i++)
    {
      free(bstruct->names[i].key);
      free(bstruct->names[i].data);
    }
    free(bstruct->names);
    free(bstruct);
  }
} /* of 'bstruct_close' */

void bstruct_setout(Bstruct bstruct, /**< pointer to restart file */
                    Bool isout       /**< enable error output on stderr (TRUE/FALSE) */
                   )
{
  /* Function enables/disables error output on stderr */
  bstruct->isout=isout;
} /* of 'bstruct_setout' */

Bool bstruct_isdefined(Bstruct bstr,   /**< pointer to restart file */
                       const char *key /**< key to compare */
                      )                /** \return TRUE if key is identical to name read from file */
{
  long long pos;
  Bool rc;
  Byte token,isout;
  /* store file position */
  pos=ftell(bstr->file);
  /* read token */
  if(fread(&token,1,1,bstr->file)!=1)
    return FALSE;
  if(token==BSTRUCT_ENDARRAY || token==BSTRUCT_ENDSTRUCT)
  {
    fseek(bstr->file,pos,SEEK_SET);
    return FALSE;
  }
  isout=bstr->isout;
  /* temporarily switch off error messages */
  bstr->isout=FALSE;
  rc=findobject(bstr,&token,key);
  bstr->isout=isout;
  /* restore position in file */
  fseek(bstr->file,pos,SEEK_SET);
  return !rc;
} /* of bstruct_isdefined' */

Bool bstruct_writebool(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       Bool value        /**< value written to file */
                      )                  /** \return TRUE on error */
{
  return writename(bstr->file,bstr->hash,value ? BSTRUCT_TRUE : BSTRUCT_FALSE,name);
} /* of 'bstruct_writebool' */

Bool bstruct_writebyte(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       Byte value        /**< value written to file */
                      )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte token;
  token=(value==0) ? BSTRUCT_ZERO : BSTRUCT_BYTE;
  if(writename(bstr->file,bstr->hash,token,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writebyte' */

Bool bstruct_writeint(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      int value         /**< value written to file */
                     )                  /** \return TRUE on error */
{
  short s;
  Byte token;
  if(value==0)
    token=BSTRUCT_ZERO;
  else if(value>=0 && value<=UCHAR_MAX)
    token=BSTRUCT_BYTE;
  else if(value>=SHRT_MIN && value<=SHRT_MAX)
    token=BSTRUCT_SHORT;
  else
    token=BSTRUCT_INT;
  if(writename(bstr->file,bstr->hash,token,name))
    return TRUE;
  switch(token)
  {
    case BSTRUCT_BYTE:
      token=value;
      return fwrite(&token,1,1,bstr->file)!=1;
    case BSTRUCT_SHORT:
      s=value;
      return fwrite(&s,sizeof(short),1,bstr->file)!=1;
    case BSTRUCT_INT:
      return fwrite(&value,sizeof(value),1,bstr->file)!=1;
    default:
      break;
  }
  return FALSE;
} /* of 'bstruct_writeint' */

Bool bstruct_writeintarray(Bstruct bstr,      /**< pointer to restart file */
                           const char *name,  /**< name of object or NULL */
                           const int value[], /**< array written to file */
                           int size           /**< size of array */
                          )                   /** \return TRUE on error */
{
  int i;
  if(bstruct_writearray(bstr,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(bstruct_writeint(bstr,NULL,value[i]))
      return TRUE;
  return bstruct_writeendarray(bstr);
} /* of 'bstruct_writeintarray */

Bool bstruct_writeushortarray(Bstruct bstr,                 /**< pointer to restart file */
                              const char *name,             /**< name of object or NULL */
                              const unsigned short value[], /**< array written to file */
                              int size                      /**< size of array */
                             )                              /** \return TRUE on error */
{
  int i;
  if(bstruct_writearray(bstr,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(bstruct_writeushort(bstr,NULL,value[i]))
      return TRUE;
  return bstruct_writeendarray(bstr);
} /* of 'bstruct_writeushortarray */

Bool bstruct_writeshortarray(Bstruct bstr,        /**< pointer to restart file */
                             const char *name,    /**< name of object or NULL */
                             const short value[], /**< array written to file */
                             int size             /**< size of array */
                            )                     /** \return TRUE on error */
{
  int i;
  if(bstruct_writearray(bstr,name,size))
    return TRUE;
  for(i=0;i<size;i++)
    if(bstruct_writeshort(bstr,NULL,value[i]))
      return TRUE;
  return bstruct_writeendarray(bstr);
} /* of 'bstruct_writeshortarray */

Bool bstruct_writeushort(Bstruct bstr,        /**< pointer to restart file */
                         const char *name,    /**< name of object or NULL */
                         unsigned short value /**< value written to file */
                        )                     /** \return TRUE on error */
{
  Byte token;
  if(value==0)
    token=BSTRUCT_ZERO;
  else if(value>=0 && value<=UCHAR_MAX)
    token=BSTRUCT_BYTE;
  else
    token=BSTRUCT_USHORT;
  if(writename(bstr->file,bstr->hash,token,name))
    return TRUE;
  switch(token)
  {
    case BSTRUCT_BYTE:
      token=value;
      return fwrite(&token,1,1,bstr->file)!=1;
    case BSTRUCT_USHORT:
      return fwrite(&value,sizeof(value),1,bstr->file)!=1;
    default:
      break;
  }
  return FALSE;
} /* of 'bstruct_writeushort' */

Bool bstruct_writeshort(Bstruct bstr,     /**< pointer to restart file */
                        const char *name, /**< name of object or NULL */
                        short value       /**< value written to file */
                       )                  /** \return TRUE on error */
{
  Byte token;
  if(value==0)
    token=BSTRUCT_ZERO;
  else if(value>=0 && value<=UCHAR_MAX)
    token=BSTRUCT_BYTE;
  else
    token=BSTRUCT_SHORT;
  if(writename(bstr->file,bstr->hash,token,name))
    return TRUE;
  switch(token)
  {
    case BSTRUCT_BYTE:
      token=value;
      return fwrite(&token,1,1,bstr->file)!=1;
    case BSTRUCT_SHORT:
      return fwrite(&value,sizeof(value),1,bstr->file)!=1;
    default:
      break;
  }
  return FALSE;
} /* of 'bstruct_writeshort' */


Bool bstruct_writereal(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       Real value        /**< value written to file */
                      )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte token;
  if(value==0.0)
    token=BSTRUCT_ZERO;
  else
    token=(sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT;
  if(writename(bstr->file,bstr->hash,token,name))
    return TRUE;
  if(value!=0.0)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writereal' */

Bool bstruct_writefloat(Bstruct bstr,     /**< pointer to restart file */
                        const char *name, /**< name of object or NULL */
                        float value       /**< value written to file */
                       )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte token;
  token=(value==0) ? BSTRUCT_ZERO : BSTRUCT_FLOAT;
  if(writename(bstr->file,bstr->hash,token,name))
    return TRUE;
  if(value!=0.0)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writefloat' */

Bool bstruct_writestring(Bstruct bstr,     /**< pointer to restart file */
                         const char *name, /**< name of object or NULL */
                         const char *value /**< string written to file */
                        )                  /** \return TRUE on error */
{
  int len;
  Byte b;
  len=strlen(value);
  b=(len<=UCHAR_MAX) ? BSTRUCT_STRING1 : BSTRUCT_STRING;
  if(writename(bstr->file,bstr->hash,b,name))
    return TRUE;
  if(len<=UCHAR_MAX)
  {
    b=len;
    fwrite(&b,1,1,bstr->file);
  }
  else
    fwrite(&len,sizeof(len),1,bstr->file);
  return fwrite(value,1,len,bstr->file)!=len;
} /* of 'bstruct_writestring' */

Bool bstruct_writearray(Bstruct bstr,     /**< pointer to restart file */
                        const char *name, /**< name of object or NULL */
                        int size          /**< size of array */
                       )                  /** \return TRUE on error */
{
  /* define array with size elements */
  Byte b;
  b=(size<=UCHAR_MAX) ? BSTRUCT_ARRAY1 : BSTRUCT_ARRAY;
  if(writename(bstr->file,bstr->hash,b,name))
    return TRUE;
  if(size<=UCHAR_MAX)
  {
    b=size;
    return fwrite(&b,1,1,bstr->file)!=1;
  }
  else
    return fwrite(&size,sizeof(size),1,bstr->file)!=1;
} /* of 'bstruct_writearray' */

Bool bstruct_writeindexarray(Bstruct bstr,       /**< pointer to restart file */
                             const char *name,   /**< name of object */
                             long long *filepos, /**< [out] file position of index vector */
                             int size            /**< size of array */
                            )                    /** \return TRUE on error */
{
  /* define array with index vector and get position of first element of index vector */
  Byte token;
  if(bstruct_writearray(bstr,name,size))
    return TRUE;
  token=BSTRUCT_INDEXARRAY;
  fwrite(&token,1,1,bstr->file);
  if(fwrite(&size,sizeof(size),1,bstr->file)!=1)
    return TRUE;
  *filepos=ftell(bstr->file);
  return fseek(bstr->file,sizeof(long long)*size,SEEK_CUR);
} /* of 'bstruct_writeindexarray' */

Bool bstruct_writerealarray(Bstruct bstr,     /**< pointer to restart file */
                            const char *name, /**< name of object or NULL */
                            const Real vec[], /**< array written to file */
                            int size          /**< size of array */
                           )                  /** \return TRUE on error */
{
  int i;
  bstruct_writearray(bstr,name,size);
  for(i=0;i<size;i++)
    bstruct_writereal(bstr,NULL,vec[i]);
  return bstruct_writeendarray(bstr);
} /* of 'bstruct_writerealarray' */

Bool bstruct_writestruct(Bstruct bstr,    /**< pointer to restart file */
                         const char *name /**< name of object or NULL */
                        )                 /** \return TRUE on error */
{
  return writename(bstr->file,bstr->hash,BSTRUCT_STRUCT,name);
} /* of 'bstruct_writestruct' */

Bool bstruct_writeendstruct(Bstruct bstr /**< pointer to restart file */
                           )             /** \return TRUE on error */
{
  Byte token;
  token=BSTRUCT_ENDSTRUCT;
  return fwrite(&token,1,1,bstr->file)!=1;
} /* of 'bstruct_writeendstruct' */

Bool bstruct_writeendarray(Bstruct bstr /**< pointer to restart file */
                          )             /** \return TRUE on error */
{
  Byte token;
  token=BSTRUCT_ENDARRAY;
  return fwrite(&token,1,1,bstr->file)!=1;
} /* of 'bstruct_writeendarray' */

Bool bstruct_readdata(Bstruct bstr,      /**< pointer to restart file */
                      Bstruct_data *data /**< data read from file */
                     )                   /**  \return TRUE on error */
{
  /* Function reads one data object */
  Byte len,id1;
  int string_length,i;
  short id,*id2;
  data->name=NULL;
  if(fread(&data->token,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading token.\n");
    return TRUE;
  }
  if(data->token==BSTRUCT_ENDARRAY || data->token==BSTRUCT_ENDSTRUCT)
    return FALSE;
  if(data->token!=BSTRUCT_INDEXARRAY)
  {
    if((data->token & 128)==128) /* top bit in token set, object name stored in next byte or short */
    {
      if((data->token & 64)==64) /* bit 7 set, id is of type short */
      {
        if(freadshort(&id,1,bstr->swap,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name  %s.\n",
                    bstruct_typenames[data->token & 63]);
          return TRUE;
        }
      }
      else
      {
        if(fread(&id1,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading object name  %s.\n",
                    bstruct_typenames[data->token & 63]);
          return TRUE;
        }
        id=id1;
      }
      for(i=0;i<bstr->count;i++)
      {
        id2=bstr->names[i].data;
        if(id==*id2)
        {
          data->name=bstr->names[i].key;
          break;
        }
      }
    }
  }
  data->token &= 63; /* strip top 2 bits in token */
  switch(data->token)
  {
    case BSTRUCT_FALSE:
      data->data.b=FALSE;
      return FALSE;
    case BSTRUCT_TRUE:
       data->data.b=TRUE;
      return FALSE;
    case BSTRUCT_BYTE:
      if(fread(&data->data.b,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_USHORT:
      if(freadushort(&data->data.us,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_SHORT:
      if(freadshort(&data->data.s,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_INT:
      if(freadint(&data->data.i,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_FLOAT:
      if(freadfloat(&data->data.f,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_DOUBLE:
      if(freaddouble(&data->data.d,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading %s '%s'.\n",
                  bstruct_typenames[data->token & 63],getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      if(data->token==BSTRUCT_STRING1)
      {
        if(fread(&len,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading string length of '%s'.\n",
                    getname(data->name));
          return TRUE;
        }
        string_length=len;
      }
      else
      {
        if(freadint(&string_length,1,bstr->swap,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading string length of '%s'.\n",
                    getname(data->name));
          return TRUE;
        }
      }
      data->data.string=malloc(string_length+1);
      if(data->data.string==NULL)
      {
        printallocerr("string");
        return TRUE;
      }
      if(fread(data->data.string,1,string_length,bstr->file)!=string_length)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading string '%s' of size %d.\n",
                  getname(data->name),string_length);
        free(data->data.string);
        data->data.string=NULL;
        return TRUE;
      }
      data->data.string[string_length]='\0';
      return FALSE;
    case BSTRUCT_ARRAY1:
      if(fread(&len,1,1,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading array size of '%s'.\n",
                  getname(data->name));
        return TRUE;
      }
      data->size=len;
      return FALSE;
    case BSTRUCT_ARRAY:
      if(freadint(&data->size,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading array size of '%s'.\n",
                  getname(data->name));
        return TRUE;
      }
      return FALSE;
    case BSTRUCT_INDEXARRAY:
      if(freadint(&data->size,1,bstr->swap,bstr->file)!=1)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading size of index array.\n");
        return TRUE;
      }
      data->data.index=newvec(long long,data->size);
      if(data->data.index==NULL)
      {
        printallocerr("index");
        return TRUE;
      }
      if(freadlong(data->data.index,data->size,bstr->swap,bstr->file)!=data->size)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR508: Unexpected end of file reading index array of size %d.\n",
                  data->size);
        free(data->data.index);
        data->data.index=NULL;
        return TRUE;
      }
      return FALSE;
    default:
      return FALSE;
  } /* of  switch(data->token) */
  return FALSE;
} /* of 'bstruct_readdata' */

void bstruct_freedata(Bstruct_data *data)
{
  switch(data->token)
  {
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      free(data->data.string);
      break;
    case BSTRUCT_INDEXARRAY:
      free(data->data.index);
      break;
    default:
      break;
  }
} /* of 'bstruct_freedata' */

void bstruct_fprintdata(FILE *file,              /* pointer to text file */
                        const Bstruct_data *data /* ponter to data object to print */
                       )
{
  switch(data->token)
  {
    case BSTRUCT_ZERO:
      fputc('0',file);
      break;
    case BSTRUCT_TRUE: case BSTRUCT_FALSE:
      fprintf(file,"%s",bool2str(data->data.b));
      break;
    case BSTRUCT_BYTE:
      fprintf(file,"%d",data->data.b);
      break;
    case BSTRUCT_SHORT:
      fprintf(file,"%d",data->data.s);
      break;
    case BSTRUCT_USHORT:
      fprintf(file,"%d",data->data.us);
      break;
    case BSTRUCT_INT:
      fprintf(file,"%d",data->data.i);
      break;
    case BSTRUCT_FLOAT:
      fprintf(file,"%g",data->data.f);
      break;
    case BSTRUCT_DOUBLE:
      fprintf(file,"%g",data->data.d);
      break;
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      fprintf(file,"\"%s\"",data->data.string);
      break;
    default:
      break;
  } /* of switch(data->token) */
} /* of 'bstruct_fprintdata' */

Bool bstruct_readbool(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Bool *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_FALSE,name))
    return TRUE;
  if(findobject(bstr,&b,name))
    return TRUE;
  b&=63;
  switch(b)
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
              getname(name),bstruct_typenames[b & 63]);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readbool' */

Bool bstruct_readbyte(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Byte *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_BYTE,name))
    return TRUE;
  if(findobject(bstr,&token,name))
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

Bool bstruct_readint(Bstruct bstr,     /**< pointer to restart file */
                     const char *name, /**< name of object or NULL */
                     int *value        /**< value read from file */
                    )                  /** \return TRUE on error */
{
  short s;
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_INT,name))
    return TRUE;
  if(findobject(bstr,&token,name))
    return TRUE;
  token&=63; /* strip top 2 bits in token */
  switch(token)
  {
    case BSTRUCT_ZERO:
      *value=0;
      return FALSE;
    case BSTRUCT_BYTE:
      if(fread(&token,1,1,bstr->file)!=1)
        return TRUE;
      *value=token;
      return FALSE;
    case BSTRUCT_SHORT:
      if(freadshort(&s,1,bstr->swap,bstr->file)!=1)
        return TRUE;
      *value=s;
      return FALSE;
    case BSTRUCT_INT:
      return freadint(value,1,bstr->swap,bstr->file)!=1;
    default:
      if(bstr->isout)
        fprintf(stderr,"ERROR509: Type of '%s'=%s is not int.\n",
                getname(name),bstruct_typenames[token]);
      return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readint' */

Bool bstruct_readshort(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       short *value      /**< value read from file */
                      )                  /** \return TRUE on error */
{
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_SHORT,name))
    return TRUE;
  if(findobject(bstr,&token,name))
    return TRUE;
  token&=63; /* strip top 2 bits in token */
  switch(token)
  {
    case BSTRUCT_ZERO:
      *value=0;
      return FALSE;
    case BSTRUCT_BYTE:
      if(fread(&token,1,1,bstr->file)!=1)
        return TRUE;
      *value=token;
      return FALSE;
    case BSTRUCT_SHORT:
      if(freadshort(value,1,bstr->swap,bstr->file)!=1)
        return TRUE;
      return FALSE;
    default:
      if(bstr->isout)
        fprintf(stderr,"ERROR509: Type of '%s'=%s is not short.\n",
                getname(name),bstruct_typenames[token]);
      return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readshort' */

Bool bstruct_readushort(Bstruct bstr,         /**< pointer to restart file */
                        const char *name,     /**< name of object or NULL */
                        unsigned short *value /**< value read from file */
                       )                      /** \return TRUE on error */
{
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_USHORT,name))
    return TRUE;
  if(findobject(bstr,&token,name))
    return TRUE;
  token&=63; /* strip top 2 bits in token */
  switch(token)
  {
    case BSTRUCT_ZERO:
      *value=0;
      return FALSE;
    case BSTRUCT_BYTE:
      if(fread(&token,1,1,bstr->file)!=1)
        return TRUE;
      *value=token;
      return FALSE;
    case BSTRUCT_USHORT:
      if(freadushort(value,1,bstr->swap,bstr->file)!=1)
        return TRUE;
      return FALSE;
    default:
      if(bstr->isout)
        fprintf(stderr,"ERROR509: Type of '%s'=%s is not unsigned short.\n",
                getname(name),bstruct_typenames[token]);
      return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readushort' */

Bool bstruct_readfloat(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       float *value      /**< value read from file */
                      )                  /** \return TRUE on error */
{
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_FLOAT,name))
    return TRUE;
  if(findobject(bstr,&token,name))
    return TRUE;
  token&=63; /* strip top 2 bits in token */
  if(token==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(token!=BSTRUCT_FLOAT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not float.\n",
              getname(name),bstruct_typenames[token]);
    return TRUE;
  }
  if(freadfloat(value,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading float '%s'.\n",
              getname(name));
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readfloat' */

Bool bstruct_readreal(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Real *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte token;
  if(readtoken(bstr,&token,(sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT,name))
    return TRUE;
  if(findobject(bstr,&token,name))
    return TRUE;
  token&=63; /* strip top 3 bits in token */
  if(token==BSTRUCT_ZERO)
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
} /* of 'readreal' */

char *bstruct_readstring(Bstruct bstr,    /**< pointer to restart file */
                         const char *name /**< name of object or NULL */
                        )                 /**< \return pointer to string read or NULL on error */
{
  char *s;
  int len;
  Byte token,len1;
  if(readtoken(bstr,&token,BSTRUCT_STRING,name))
    return NULL;
  if(findobject(bstr,&token,name))
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

Bool bstruct_readarray(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       int *size         /**< [out] size of array */
                      )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_ARRAY,name))
    return TRUE;
  if(findobject(bstr,&b,name))
    return TRUE;
  b&=63; /* strip top 3 bits in token */
  if(b==BSTRUCT_ARRAY1)
  {
    if(fread(&b,1,1,bstr->file)!=1)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR512: Cannot read array length of '%s'.\n",getname(name));
      return TRUE;
    }
    *size=b;
    return FALSE;
  }
  if(b!=BSTRUCT_ARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not array.\n",
              getname(name),bstruct_typenames[b]);
    return TRUE;
  }
  return freadint(size,1,bstr->swap,bstr->file)!=1;
} /* of 'bstruct_readarray' */

Bool bstruct_readindexarray(Bstruct bstr,    /**< pointer to restart file */
                            long long *data, /**< name of object or NULL */
                            int size         /**< [out] size of array */
                           )                 /** \return TRUE on error */
{
  int n;
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_INDEXARRAY,NULL))
    return TRUE;
  if(token!=BSTRUCT_INDEXARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of %s is not an indexarray.\n",
              bstruct_typenames[token & 63]);
    return TRUE;
  }
  if(freadint(&n,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR512: Cannot read length of index array.\n");
    return TRUE;
  }
  if(n!=size)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of index array=%d not %d.\n",n,size);
    return TRUE;
  }
  if(freadlong(data,size,bstr->swap,bstr->file)!=size)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading index array of size %d.\n",
              size);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readindexarray' */

Bool bstruct_seekindexarray(Bstruct bstr, /**< pointer to restart file */
                            int index,    /**< index of array to seek */
                            int size      /**< [out] size of array */
                           )              /** \return TRUE on error */
{
  long long pos;
  int n;
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_INDEXARRAY,NULL))
    return TRUE;
  if(token!=BSTRUCT_INDEXARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of %s is not an indexarray.\n",
              bstruct_typenames[token & 63]);
    return TRUE;
  }
  if(freadint(&n,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR512: Cannot read length of index array.\n");
    return TRUE;
  }
  if(n!=size)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of index array=%d not %d.\n",n,size);
    return TRUE;
  }
  if(fseek(bstr->file,sizeof(long long)*index,SEEK_CUR))
  {
    fprintf(stderr,"ERROR511: Cannot skip to %d in index array.\n",index);
    return TRUE;
  }
  if(freadlong(&pos,1,bstr->swap,bstr->file)!=1)
  {
    fprintf(stderr,"ERROR512: Cannot read file index for %d.\n",index);
    return TRUE;
  }
  if(fseek(bstr->file,pos,SEEK_SET))
  {
    fprintf(stderr,"ERROR511: Cannot skip to file position %lld.\n",pos);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_seekindexarray' */

Bool bstruct_readshortarray(Bstruct bstr,     /**< pointer to restart file */
                            const char *name, /**< name of object or NULL */
                            short data[],     /**< array read from file */
                            int size          /**< size of array */
                           )                  /** \return TRUE on error */
{
  int i,n;
  if(bstruct_readarray(bstr,name,&n))
    return TRUE;
  if(size!=n)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",
              getname(name),n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readshort(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr,name);
} /* of 'bstruct_readshortarray' */

Bool bstruct_readushortarray(Bstruct bstr,          /**< pointer to restart file */
                             const char *name,      /**< name of object or NULL */
                             unsigned short data[], /**< array read from file */
                             int size               /**< size of array */
                            )                       /** \return TRUE on error */
{
  int i,n;
  if(bstruct_readarray(bstr,name,&n))
    return TRUE;
  if(size!=n)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",
              getname(name),n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readushort(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr,name);
} /* of 'bstruct_readushortarray' */

Bool bstruct_readintarray(Bstruct bstr,     /**< pointer to restart file */
                          const char *name, /**< name of object or NULL */
                          int data[],       /**< array read from file */
                          int size          /**< size of array */
                         )                  /** \return TRUE on error */
{
  int i,n;
  if(bstruct_readarray(bstr,name,&n))
    return TRUE;
  if(size!=n)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",
              getname(name),n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readint(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr,name);
} /* of 'bstruct_readintarray' */

Bool bstruct_readrealarray(Bstruct bstr,     /**< pointer to restart file */
                           const char *name, /**< name of object or NULL */
                           Real data[],      /**< array read from file */
                           int size          /**< size of array */
                          )                  /** \return TRUE on error */
{
  int i,n;
  if(bstruct_readarray(bstr,name,&n))
    return TRUE;
  if(size!=n)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",
              getname(name),n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readreal(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr,name);
} /* of 'bstruct_readrealarray' */

Real *bstruct_readvarrealarray(Bstruct bstr,     /**< pointer to restart file */
                               const char *name, /**< name of object or NULL */
                               int *size         /**< [out] size of array */
                              )                  /** \return pointer to array read or NULL on error */
{
  Real *data;
  int i;
  if(bstruct_readarray(bstr,name,size))
    return NULL;
  data=newvec(Real,*size);
  if(data==NULL)
  {
    printallocerr("data");
    return NULL;
  }
  for(i=0;i<*size;i++)
    if(bstruct_readreal(bstr,NULL,data+i))
    {
      free(data);
      return NULL;
    }
  if(bstruct_readendarray(bstr,name))
  {
    free(data);
    return NULL;
  }
  return data;
} /* of 'bstruct_readvarrealarray' */

long long bstruct_getarrayindex(Bstruct bstr)
{
  return ftell(bstr->file);
} /* of 'bstruct_getarrayindex' */

void bstruct_sync(Bstruct bstr)
{
#ifndef _WIN32
  fsync(fileno(bstr->file));
#endif
} /* of 'bstruct_sync' */

Bool bstruct_writearrayindex(Bstruct bstr,      /**< pointer to restart file */
                             long long filepos, /**< file position of index vector */
                             long long index[], /**< position vector to write */
                             int offset,
                             int size           /**< size of index vector */
                            )                   /** \return TRUE on error */
{
  Bool rc;
  if(fseek(bstr->file,filepos+sizeof(long long)*offset,SEEK_SET))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR519: Cannout skip to position %d in index array.\n",offset);
    return TRUE;
  }
  /* write position vector */
  rc=fwrite(index,sizeof(long long),size,bstr->file)!=size;
  fseek(bstr->file,0,SEEK_END);
  return rc;
} /* of bstruct_writearrayindex' */

Bool bstruct_readstruct(Bstruct bstr,    /**< pointer to restart file */
                        const char *name /**< name of object or NULL */
                       )                 /** \return TRUE on error */
{
  Byte token;
  if(readtoken(bstr,&token,BSTRUCT_STRUCT,name))
    return TRUE;
  if(findobject(bstr,&token,name))
    return TRUE;
  if((token & 63) !=BSTRUCT_STRUCT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not struct.\n",
              name,bstruct_typenames[token & 63]);
    return TRUE;
  }
  if(bstr->level==MAXLEVEL-1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR515: Too deep nesting of structs, %d allowed.\n",MAXLEVEL);
    freenamestack(bstr);
    return TRUE;
  }
  /* add empty list of names for this new level */
  bstr->namestack[bstr->level].varnames=newlist(0);
  /* store name of struct */
  bstr->namestack[bstr->level++].name=(name==NULL) ? NULL : strdup(name);
  return FALSE;
} /* of 'bstruct_readstruct' */

Bool bstruct_readendstruct(Bstruct bstr,    /**< pointer to restart file */
                           const char *name /**< object name */
                          )                 /** \return TRUE on error */
{
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
      if(skipdata(bstr,token))
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
        fprintf(stderr,"ERROR503: End of array found, endstruct of '%s' expected.\n",
                getname(name));
      return TRUE;
    }
  }
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

Bool bstruct_readendarray(Bstruct bstr,    /**< pointer to restart file */
                          const char *name /**< object name */
                         )                 /** \return TRUE on error */
{
  Byte token;
  if(fread(&token,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR508: Unexpected end of file reading token for endarray of '%s'.\n",
              getname(name));
    return TRUE;
  }
  if(token!=BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR509: Type=%s is not endarrary.\n",
                bstruct_typenames[token & 63]);
      else
        fprintf(stderr,"ERROR509: Type=%s is not endarray for array '%s'.\n",
                bstruct_typenames[token & 63],name);
    }
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readendarray' */
