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
#include "bstruct.h"

static const size_t bstruct_typesizes[]={1,sizeof(short),sizeof(int),sizeof(float),
                                         sizeof(double),1,sizeof(unsigned short),0};

char *bstruct_typenames[]={"byte","short","int","float","double","bool","ushort",
                           "zero","string","array","array1","struct","indexarray",
                           "endstruct","endarray"};

struct bstruct
{
  FILE *file; /**< pointer to binary */
  Bool swap;  /**< byte order has to be changed at reading */
  Bool isout; /**< error output on stderr enabled */
};            /**< Definition of opaque datatype Bstruct */

static Bool skipdata(Bstruct,Byte);

static Bool readtoken(Bstruct bstr,Byte *b,int token,const char *name)
{
  if(fread(b,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR501: Cannot read '%s': %s.\n",
              name,strerror(errno));
    return TRUE;
  }
  if(*b>BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR502: Invalid token %d reading %s.\n",*b,bstruct_typenames[token]);
      else
        fprintf(stderr,"ERROR502: Invalid token %d reading '%s'.\n",*b,name);
    }
    return TRUE;
  }
  if(*b==BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR503: End of array found, %s expected.\n",
                bstruct_typenames[token]);
      else
        fprintf(stderr,"ERROR508: Object '%s' not found in array.\n",name);
    }
    return TRUE;
  }
  if(*b==BSTRUCT_ENDSTRUCT)
  {
    if(bstr->isout)
    {
      if(name==NULL)
        fprintf(stderr,"ERROR503: End of struct found, %s expected.\n",
                bstruct_typenames[token]);
      else
        fprintf(stderr,"ERROR503: Object '%s' not found in struct.\n",name);
    }
    return TRUE;
  }
  return FALSE;
} /* of 'readtoken' */

static Bool cmpkey(Bstruct bstr,    /**< pointer to restart file */
                   Byte *token,     /**< token matching name */
                   const char *name /**< name to search for */
                  )                 /** \return TRUE if name was not found */
{
  char *s;
  Byte name_length;
  if(fread(&name_length,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR513: Unexpected end of file reading object name length.\n");
    return TRUE;
  }
  if(name==NULL)
  {
    if(name_length!=0)
    {
      s=malloc((int)name_length+1);
      if(s==NULL)
      {
        printallocerr("name");
        return TRUE;
      }
      s[name_length]='\0';
      if(fread(s,1,name_length,bstr->file)!=name_length)
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR513: Unexpected end of file reading object name.\n");
        free(s);
        return TRUE;
      }
      if(bstr->isout)
        fprintf(stderr,"ERROR504: Expected no object name, but '%s' found.\n",s);
      free(s);
      return TRUE;
    }
  }
  else
  {
    s=malloc((int)name_length+1);
    if(s==NULL)
    {
      printallocerr("name");
      return TRUE;
    }
    s[name_length]='\0';
    if(fread(s,1,name_length,bstr->file)!=name_length)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR513: Unexpected end of file reading object name.\n");
      free(s);
      return TRUE;
    }
    if(strcmp(name,s))
    {
      /* name not found, search for it */
      do
      {
#ifdef DEBUG_BSTRUCT
        printf("%s not found, %s\n",name,s);
#endif
        if(skipdata(bstr,*token))
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR505: Cannot skip to next object, '%s' not found.\n",
                    name);
          free(s);
          return TRUE;
        }
        /* read next token */
        if(fread(token,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR513: Unexpected end of file reading token.\n");
          free(s);
          return TRUE;
        }
        if(*token==BSTRUCT_ENDSTRUCT || *token==BSTRUCT_ENDARRAY)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR506: Object '%s' not found.\n",name);
          free(s);
          return TRUE;
        }
        free(s);
        /* read next name */
        if(fread(&name_length,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR513: Unexpected end of file reading object name length.\n");
          return TRUE;
        }
        s=malloc((int)name_length+1);
        if(s==NULL)
        {
          printallocerr("name");
          return TRUE;
        }
        s[name_length]='\0';
        if(fread(s,1,name_length,bstr->file)!=name_length)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR513: Unexpected end of file reading object name.\n");
          free(s);
          return TRUE;
        }
      }while(strcmp(s,name));
    }
    free(s);
  }
  return FALSE;
} /* of 'cmpkey' */

static Bool writename(FILE *file,const String name)
{
  /* Function write name of object into restart file */
  Bool rc;
  Byte b;
  if(name==NULL)
    b=0;
  else
  {
    if(strlen(name)>UCHAR_MAX)
    {
      fprintf(stderr,"ERROR610: String too long for key, must be less than 256.\n");
      return TRUE;
    }
    b=strlen(name);
  }
  rc=fwrite(&b,1,1,file)!=1;
  if(b)
    rc=fwrite(name,1,b,file)!=b;
  return rc;
} /* of 'writename' */

Bstruct bstruct_create(const char *filename)
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
  bstruct->file=fopen(filename,"wb");
  if(bstruct->file==NULL)
  {
    printfcreateerr(filename);
    free(bstruct->file);
    free(bstruct);
    return NULL;
  }
  fwritetopheader(bstruct->file,BSTRUCT_HEADER,BSTRUCT_VERSION);
  return bstruct;
} /* of 'bstruct_create' */

Bstruct bstruct_open(const char *filename,Bool isout)
{
  /* Function opens restart file for reading */
  Bstruct bstruct;
  int version;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=isout;
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
  return bstruct;
} /* of 'bstruct_open' */

FILE *bstruct_getfile(Bstruct bstruct)
{
  return bstruct->file;
} /* of 'bstruct_getfile' */

Bstruct bstruct_append(const char *filename,Bool isout)
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
  bstruct->file=fopen(filename,"r+b");
  if(bstruct->file==NULL)
  {
    if(isout)
      printfopenerr(filename);
    free(bstruct);
    return NULL;
  }
  fseek(bstruct->file,0,SEEK_END); /* seek to end of file for append */
  return bstruct;
} /* of 'bstruct_append' */

void bstruct_close(Bstruct bstruct)
{
  /* Function closes restart file and frees memory */
  if(bstruct!=NULL)
  {
    fclose(bstruct->file);
    free(bstruct);
  }
} /* of 'bstruct_close' */

void bstruct_setout(Bstruct bstruct,Bool isout)
{
  /* Function enables/disables error output on stderr */
  bstruct->isout=isout;
} /* of 'bstruct_isout ' */

Bool bstruct_isdefined(Bstruct bstr,   /**< pointer to restart file */
                       const char *key /**< key to compare */
                      )                /** \return TRUE if key is identical to name read from file */
{
  long long pos;
  Bool rc;
  Byte b,isout;
  /* store file position */
  pos=ftell(bstr->file);
  /* read token */
  if(fread(&b,1,1,bstr->file)!=1)
    return FALSE;
  if(b==BSTRUCT_ENDARRAY || b==BSTRUCT_ENDSTRUCT)
  {
    fseek(bstr->file,pos,SEEK_SET);
    return FALSE;
  }
  isout=bstr->isout;
  /* temporarily switch off error messages */
  bstr->isout=FALSE; 
  rc=cmpkey(bstr,&b,key);
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
  Byte b;
  b=BSTRUCT_BOOL;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
    return TRUE;
  b=value;
  return fwrite(&b,sizeof(b),1,bstr->file)!=1;
} /* of 'bstruct_writebool' */

Bool bstruct_writebyte(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       Byte value        /**< value written to file */
                      )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? BSTRUCT_ZERO : BSTRUCT_BYTE;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
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
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? BSTRUCT_ZERO : BSTRUCT_INT;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writeint' */

Bool bstruct_writeintarray(Bstruct bstr,      /**< pointer to restart file */
                           const char *name,  /**< name of object or NULL */
                           const int value[], /**< array written to file */
                           int size           /**< size of arary */
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
                              int size                      /**< size of arary */
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
                             int size             /**< size of arary */
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

Bool bstruct_writeushort(Bstruct bstr,         /**< pointer to restart file */
                          const char *name,    /**< name of object or NULL */
                          unsigned short value /**< value written to file */
                        )                      /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? BSTRUCT_ZERO : BSTRUCT_USHORT;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writeushort' */

Bool bstruct_writeshort(Bstruct bstr,     /**< pointer to restart file */
                        const char *name, /**< name of object or NULL */
                        short value       /**< value written to file */
                       )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  b=(value==0) ? BSTRUCT_ZERO : BSTRUCT_SHORT;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
    return TRUE;
  if(value)
    rc=fwrite(&value,sizeof(value),1,bstr->file)!=1;
  return rc;
} /* of 'bstruct_writeshort' */


Bool bstruct_writereal(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       Real value        /**< value written to file */
                      )                  /** \return TRUE on error */
{
  Bool rc=FALSE;
  Byte b;
  if(value==0.0)
    b=BSTRUCT_ZERO;
  else
    b=(sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
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
  Byte b;
  b=(value==0) ? BSTRUCT_ZERO : BSTRUCT_FLOAT;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
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
  b=BSTRUCT_STRING;
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
    return TRUE;
  len=strlen(value);
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
  fwrite(&b,1,1,bstr->file);
  if(writename(bstr->file,name))
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
  Byte b;
  if(bstruct_writearray(bstr,name,size))
    return TRUE;
  b=BSTRUCT_INDEXARRAY;
  fwrite(&b,1,1,bstr->file);
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
  Byte b;
  b=BSTRUCT_STRUCT;
  fwrite(&b,1,1,bstr->file);
  return writename(bstr->file,name);
} /* of 'bstruct_writestruct' */

Bool bstruct_writeendstruct(Bstruct bstr /**< pointer to restart file */
                           )             /** \return TRUE on error */
{
  Byte b;
  b=BSTRUCT_ENDSTRUCT;
  return fwrite(&b,1,1,bstr->file)!=1;
} /* of 'bstruct_writeendstruct' */

Bool bstruct_writeendarray(Bstruct bstr /**< pointer to restart file */
                          )             /** \return TRUE on error */
{
  Byte b;
  b=BSTRUCT_ENDARRAY;
  return fwrite(&b,1,1,bstr->file)!=1;
} /* of 'bstruct_writeendarray' */

static Bool skipdata(Bstruct bstr, /**< pointer to restart file */
                     Byte token    /**< token */
                    )              /** \return TRUE on error */
{
  /* Function skips one object in restart file */
  int string_len;
  Byte len,b;
  switch(token)
  {
    case BSTRUCT_STRUCT:
      do
      {
        /* skip whole array */
        if(fread(&b,1,1,bstr->file)!=1)
          return TRUE;
#ifdef DEBUB_BSTRUCT
        printf("type %s\n",bstruct_typenames[b]);
#endif
        if(b==BSTRUCT_ENDARRAY)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR507: Unexpected end of array token found in struct found.\n");
          return TRUE;
        }
        if(b!=BSTRUCT_ENDSTRUCT)
        {
           /*skip object name */
          if(fread(&len,1,1,bstr->file)!=1)
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR513: Unexpected end of file reading object name length.\n");
            return TRUE;
          }
          if(fseek(bstr->file,len,SEEK_CUR))
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR513: Unexpected end of file skipping object name.\n");
            return TRUE;
          }
          /* call skipdata() recursively */
          if(skipdata(bstr,b))
            return TRUE;
        }
      } while(b!=BSTRUCT_ENDSTRUCT);
      break;
    case BSTRUCT_ARRAY: case BSTRUCT_ARRAY1:
      /* skip array size */
      if(fseek(bstr->file,(token==BSTRUCT_ARRAY1) ? 1 : sizeof(int),SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR513: Unexpected end of file skipping array length.\n");
        return TRUE;
      }
      /* skip whole array */
      do
      {
        if(fread(&b,1,1,bstr->file)!=1)
        {
          if(bstr->isout)
            fprintf(stderr,"ERROR513: Unexpected end of file reading token.\n");
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
              fprintf(stderr,"ERROR513: Unexpected end of file reading index array length.\n");
            return TRUE;
          }
          if(fseek(bstr->file,sizeof(long long)*string_len,SEEK_CUR))
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR513: Unexpected end of file skipping index array.\n");
            return TRUE;
          }
        }
        else if(b!=BSTRUCT_ENDARRAY)
        {
          /*skip object name */
          if(fread(&len,1,1,bstr->file)!=1)
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR513: Unexpected end of file reading object name length.\n");
            return TRUE;
          }
          if(fseek(bstr->file,len,SEEK_CUR))
          {
            if(bstr->isout)
              fprintf(stderr,"ERROR513: Unexpected end of file skipping object name.\n");
            return TRUE;
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
          fprintf(stderr,"ERROR513: Unexpected end of file reading string length.\n");
        return TRUE;
      }
      if(fseek(bstr->file,string_len,SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR513: Unexpected end of file skipping string.\n");
        return TRUE;
      }
      break;
    default:
      /* skip object data */
      if(fseek(bstr->file,bstruct_typesizes[token],SEEK_CUR))
      {
        if(bstr->isout)
          fprintf(stderr,"ERROR513: Unexpected end of file skipping %s.\n",
                  bstruct_typenames[token]);
        return TRUE;
      }
  } /* of switch(token) */
  return FALSE;
} /* of 'skipdata' */

Bool bstruct_readbool(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Bool *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_BOOL,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b!=BSTRUCT_BOOL)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not bool.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  if(fread(&b,sizeof(b),1,bstr->file)!=1)
    return TRUE;
  *value=b;
  return FALSE;
} /* of 'bstruct_readbool' */


Bool bstruct_readbyte(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Byte *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_BYTE,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=BSTRUCT_BYTE)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not byte.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return fread(value,1,1,bstr->file)!=1;
} /* of 'bstruct_readbyte' */

Bool bstruct_readint(Bstruct bstr,     /**< pointer to restart file */
                     const char *name, /**< name of object or NULL */
                     int *value        /**< value read from file */
                    )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_INT,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=BSTRUCT_INT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not int.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return freadint(value,1,bstr->swap,bstr->file)!=1;
} /* of 'bstruct_readint' */

Bool bstruct_readshort(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       short *value      /**< value read from file */
                      )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_SHORT,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=BSTRUCT_SHORT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not short.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return freadshort(value,1,bstr->swap,bstr->file)!=1;
} /* of 'bstruct_readshort' */

Bool bstruct_readushort(Bstruct bstr,         /**< pointer to restart file */
                        const char *name,     /**< name of object or NULL */
                        unsigned short *value /**< value read from file */
                       )                      /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_USHORT,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=BSTRUCT_USHORT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not unsigned short.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return freadushort(value,1,bstr->swap,bstr->file)!=1;
} /* of 'bstruct_readshort' */

Bool bstruct_readfloat(Bstruct bstr,     /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       float *value      /**< value read from file */
                      )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_FLOAT,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=BSTRUCT_FLOAT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not float.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return freadfloat(value,1,bstr->swap,bstr->file)!=1;
} /* of 'bstruct_readfloat' */

Bool bstruct_readreal(Bstruct bstr,     /**< pointer to restart file */
                      const char *name, /**< name of object or NULL */
                      Real *value       /**< value read from file */
                     )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,(sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=((sizeof(Real)==sizeof(double)) ? BSTRUCT_DOUBLE : BSTRUCT_FLOAT))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not real.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return freadreal(value,1,bstr->swap,bstr->file)!=1;
} /* of 'readreal' */

char *bstruct_readstring(Bstruct bstr,    /**< pointer to restart file */
                         const char *name /**< name of object or NULL */
                        )                 /**< \return pointer to string read or NULL on error */
{
  char *s;
  int len;
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_STRING,name))
    return NULL;
  if(cmpkey(bstr,&b,name))
    return NULL;
  if(b!=BSTRUCT_STRING)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not string.\n",
              name,bstruct_typenames[b]);
    return NULL;
  }
  if(freadint(&len,1,bstr->swap,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR512: Cannot read string length of '%s'.\n",name);
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
      fprintf(stderr,"ERROR512: Cannot read string '%s'.\n",name);
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
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b==BSTRUCT_ARRAY1)
  {
    if(fread(&b,1,1,bstr->file)!=1)
    {
      if(bstr->isout)
        fprintf(stderr,"ERROR512: Cannot read array length of '%s'.\n",name);
      return TRUE;
    }
    *size=b;
    return FALSE;
  }
  if(b!=BSTRUCT_ARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not array.\n",
              name,bstruct_typenames[b]);
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
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_ARRAY,NULL))
    return TRUE;
  if(b!=BSTRUCT_INDEXARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of %s is not an indexarray.\n",
              bstruct_typenames[b]);
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
  return freadlong(data,size,bstr->swap,bstr->file)!=size;
} /* of 'bstruct_readindexarray' */

Bool bstruct_seekindexarray(Bstruct bstr, /**< pointer to restart file */
                            int index,    /**< index of array to seek */
                            int size      /**< [out] size of array */
                           )              /** \return TRUE on error */
{
  long long pos;
  int n;
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_INDEXARRAY,NULL))
    return TRUE;
  if(b!=BSTRUCT_INDEXARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of %s is not an indexarray.\n",
              bstruct_typenames[b]);
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
  return fseek(bstr->file,pos,SEEK_SET);
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
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readshort(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr);
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
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readushort(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr);
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
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readint(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr);
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
      fprintf(stderr,"ERROR510: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(bstruct_readreal(bstr,NULL,data+i))
      return TRUE;
  return bstruct_readendarray(bstr);
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
  if(bstruct_readendarray(bstr))
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

Bool bstruct_writearrayindex(Bstruct bstr,
                             long long filepos,
                             long long vec[],
                             int offset,
                             int size
                            )                    /** \return TRUE on error */
{
  if(fseek(bstr->file,filepos+sizeof(long long)*offset,SEEK_SET))
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR514: Cannout skip to position %d in index array.\n",offset);
    return TRUE;
  }
  /* write position vector */
  return fwrite(vec,sizeof(long long),size,bstr->file)!=size;
} /* of bstruct_writearrayindex' */

Bool bstruct_readstruct(Bstruct bstr,    /**< pointer to restart file */
                        const char *name /**< name of object or NULL */
                       )                 /** \return TRUE on error */
{
  Byte b;
  if(readtoken(bstr,&b,BSTRUCT_STRUCT,name))
    return TRUE;
  if(cmpkey(bstr,&b,name))
    return TRUE;
  if(b!=BSTRUCT_STRUCT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type of '%s'=%s is not struct.\n",
              name,bstruct_typenames[b]);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readstruct' */

Bool bstruct_readendstruct(Bstruct bstr /**< pointer to restart file */
                          )             /** \return TRUE on error */
{
  Byte b;
  if(fread(&b,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR513: Unexpected end of file reading token.\n");
    return TRUE;
  }
  if(b!=BSTRUCT_ENDSTRUCT)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type=%s is not endstruct.\n",bstruct_typenames[b]);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readendstruct' */

Bool bstruct_readendarray(Bstruct bstr /**< pointer to restart file */
                         )             /** \return TRUE on error */
{
  Byte b;
  if(fread(&b,1,1,bstr->file)!=1)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR513: Unexpected end of file reading token.\n");
    return TRUE;
  }
  if(b!=BSTRUCT_ENDARRAY)
  {
    if(bstr->isout)
      fprintf(stderr,"ERROR509: Type=%s is not endarrary.\n",bstruct_typenames[b]);
    return TRUE;
  }
  return FALSE;
} /* of 'bstruct_readendarray' */
