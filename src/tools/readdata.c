/**************************************************************************************/
/**                                                                                \n**/
/**                   r  e  a  d  d  a  t  a  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for reading JSON-like objects from restart file                  \n**/
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
#include <math.h>
#include "types.h"
#include "errmsg.h"
#include "swap.h"

static Bool readtoken(FILE *file,Byte *b,int token)
{
  if(fread(b,1,1,file)!=1)
  {
    fprintf(stderr,"ERROR604: Cannot read token: %s.\n",
            strerror(errno));
    return TRUE;
  }
  if(*b>LPJ_ENDARRAY)
  {
    fprintf(stderr,"ERRRO606: Invalid token %d.\n",*b);
    return TRUE;
  }
  if(*b==LPJ_ENDARRAY || *b==LPJ_ENDSTRUCT)
  {
    fprintf(stderr,"ERROR605: Unexpected %s found, %s expected.\n",
            typenames[*b],typenames[token]);
    return TRUE;
  }
  return FALSE;
} /* of 'readtoken' */

static Bool cmpkey(FILE *file,Byte *token,const char *name,Bool swap)
{
  char *s;
  Byte b;
  fread(&b,1,1,file);
  if(name==NULL)
  {
    if(b!=0)
    {
      s=malloc((int)b+1);
      s[b]='\0';
      fread(s,1,b,file);
      fprintf(stderr,"ERROR601: Expected no key, but '%s' found.\n",s);
      free(s);
      return TRUE;
    }
  }
  else
  { 
    s=malloc((int)b+1);
    s[b]='\0';
    fread(s,1,b,file);
    if(strcmp(name,s))
    {
      do
      {
        //printf("%s not found, %s\n",name,s);
        if(skipdata(file,*token,swap))
          return TRUE;
        fread(token,1,1,file);
        if(*token==LPJ_ENDSTRUCT || *token==LPJ_ENDARRAY)
        {
          fprintf(stderr,"ERROR601: Key '%s' not found.\n",name);
          free(s);
          return TRUE;
        }
        free(s);
        fread(&b,1,1,file);
        s=malloc((int)b+1);
        s[b]='\0';
        fread(s,1,b,file);
      }while(strcmp(s,name));   
      free(s);
      return TRUE;
    }
    free(s);
  }
  return FALSE;
} /* of 'cmpkey' */

Bool skipdata(FILE *file, /**< pointer to restart file */
              Byte token, /**< token */
              Bool swap   /**< byte order has to be swapped */
             )            /** \return TRUE on error */
{
  int string_len;
  Byte len,b;
  switch(token)
  {
    case LPJ_STRUCT:
      do
      {
         fread(&b,1,1,file);
         if(b==LPJ_ENDARRAY)
         {
            fprintf(stderr,"ERROR607: Unexpected end of array found.\n");
            return TRUE;
         }
         if(b!=LPJ_ENDSTRUCT)
         {
           fread(&len,1,1,file);
           fseek(file,len,SEEK_CUR);
           if(skipdata(file,b,swap))
             return TRUE;
         }
       }while(b!=LPJ_ENDSTRUCT);
       break;
    case LPJ_ARRAY:
      fseek(file,sizeof(int),SEEK_CUR);
      do
      {
         fread(&b,1,1,file);
         if(b==LPJ_ENDSTRUCT)
         {
            fprintf(stderr,"ERROR607: Unexpected end of struct found.\n");
            return TRUE;
         }
         if(b!=LPJ_ENDARRAY)
         {
           fread(&len,1,1,file);
           fseek(file,len,SEEK_CUR);
           if(skipdata(file,b,swap))
             return TRUE;
          }
       }while(b!=LPJ_ENDARRAY);
       break;
     case LPJ_STRING:
       if(freadint(&string_len,1,swap,file))
         return TRUE;
       fseek(file,string_len,SEEK_CUR);
       break;
     default:
       fseek(file,typesizes[token],SEEK_CUR);
  }
  return FALSE;
} /* of 'skipdata' */

Bool readbool(FILE *file,       /**< pointer to restart file */
              const char *name, /**< name of object or NULL */
              Bool *value,      /**< value read from file */
              Bool swap         /**< byte order has to be swapped */
             )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_BOOL))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b!=LPJ_BOOL)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not bool.\n",name,typenames[b]);
    return TRUE;
  }
  if(fread(&b,sizeof(b),1,file)!=1)
    return TRUE;
  *value=b;
  return FALSE;
} /* of 'readbool' */


int readbyte(FILE *file,       /**< pointer to restart file */
             const char *name, /**< name of object or NULL */
             Byte *value,      /**< value read from file */
             Bool swap         /**< byte order has to be swapped */
            )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_BYTE))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b==LPJ_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=LPJ_BYTE)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not byte.\n",name,typenames[b]);
    return TRUE;
  }
  return fread(value,1,1,file)!=1;
} /* of 'writebyte' */

Bool readint(FILE *file,       /**< pointer to restart file */
             const char *name, /**< name of object or NULL */
             int *value,       /**< value read from file */
             Bool swap         /**< byte order has to be swapped */
            )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_INT))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b==LPJ_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=LPJ_INT)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not int.\n",name,typenames[b]);
    return TRUE;
  }
  return freadint(value,1,swap,file)!=1;
} /* of 'writeint' */

Bool readshort(FILE *file,       /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               short *value,     /**< value read from file */
               Bool swap         /**< byte order has to be swapped */
              )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_SHORT))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b==LPJ_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=LPJ_SHORT)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not short.\n",name,typenames[b]);
    return TRUE;
  }
  return freadshort(value,1,swap,file)!=1;
} /* of 'readshort' */

Bool readushort(FILE *file,            /**< pointer to restart file */
                const char *name,      /**< name of object or NULL */
                unsigned short *value, /**< value read from file */
                Bool swap              /**< byte order has to be swapped */
               )                       /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_USHORT))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b==LPJ_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=LPJ_USHORT)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not unsigned short.\n",name,typenames[b]);
    return TRUE;
  }
  return freadushort(value,1,swap,file)!=1;
} /* of 'readshort' */

Bool readfloat(FILE *file,       /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               float *value,     /**< value read from file */
               Bool swap         /**< byte order has to be swapped */
              )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_FLOAT))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b==LPJ_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=LPJ_FLOAT)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not float.\n",name,typenames[b]);
    return TRUE;
  }
  return freadfloat(value,1,swap,file)!=1;
} /* of 'readfloat' */

Bool readreal(FILE *file,        /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               Real *value,      /**< value read from file */
               Bool swap         /**< byte order has to be swapped */
              )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,(sizeof(Real)==sizeof(double)) ? LPJ_DOUBLE : LPJ_FLOAT))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b==LPJ_ZERO)
  {
    *value=0;
    return FALSE;
  }
  if(b!=((sizeof(Real)==sizeof(double)) ? LPJ_DOUBLE : LPJ_FLOAT))
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not real.\n",name,typenames[b]);
    return TRUE;
  }
  return freadreal(value,1,swap,file)!=1;
} /* of 'readreal' */

char *readstring(FILE *file,       /**< pointer to restart file */
                 const char *name, /**< name of object or NULL */
                 Bool swap         /**< byte order has to be swapped */
                )                  /**< \return pointer to string read or NULL on error */
{
  char *s;
  int len;
  Byte b;
  if(readtoken(file,&b,LPJ_STRING))
    return NULL;
  if(cmpkey(file,&b,name,swap))
    return NULL;
  if(b!=LPJ_STRING)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not string.\n",name,typenames[b]);
    return NULL;
  }
  if(freadint(&len,1,swap,file))
    return NULL;
  s=malloc(len+1);
  if(s==NULL)
  {
    printallocerr("string");
    return NULL;
  }
  if(fread(s,1,len,file)!=len)
  {
    free(s);
    return NULL;
  }
  s[len]='\0';
  return s;
} /* of 'readstring' */

Bool readarray(FILE *file,       /**< pointer to restart file */
               const char *name, /**< name of object or NULL */
               int *size,        /**< [out] size of arrary */
               Bool swap         /**< byte order has to be swapped */
              )                  /** \return TRUE on error */
{
  Byte b;
  if(readtoken(file,&b,LPJ_ARRAY))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b!=LPJ_ARRAY)
  {
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not array.\n",name,typenames[b]);
    return TRUE;
  }
  return freadint(size,1,swap,file)!=1;
} /* of 'readarray' */

Bool readshortarray(FILE *file,       /**< pointer to restart file */
                    const char *name, /**< name of object or NULL */
                    short data[],
                    int size,
                    Bool swap         /**< byte order has to be swapped */
                   )                  /** \return TRUE on error */
{
  int i,n;
  if(readarray(file,name,&n,swap))
    return TRUE;
  if(size!=n)
  {
    fprintf(stderr,"ERROR602: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(readshort(file,NULL,data+i,swap))
      return TRUE;
  return readendarray(file);
} /* of 'readshortarray' */

Bool readushortarray(FILE *file,            /**< pointer to restart file */
                     const char *name,      /**< name of object or NULL */
                     unsigned short data[], /**< array read from file */
                     int size,              /**< size of array */
                     Bool swap              /**< byte order has to be swapped */
                    )                       /** \return TRUE on error */
{
  int i,n;
  if(readarray(file,name,&n,swap))
    return TRUE;
  if(size!=n)
  {
    fprintf(stderr,"ERROR602: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(readushort(file,NULL,data+i,swap))
      return TRUE;
  return readendarray(file);
} /* of 'readushortarray' */

Bool readintarray(FILE *file,       /**< pointer to restart file */
                  const char *name, /**< name of object or NULL */
                  int data[],       /**< array read from file */
                  int size,         /**< size of array */
                  Bool swap         /**< byte order has to be swapped */
                 )                  /** \return TRUE on error */
{
  int i,n;
  if(readarray(file,name,&n,swap))
    return TRUE;
  if(size!=n)
  {
    fprintf(stderr,"ERROR602: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(readint(file,NULL,data+i,swap))
      return TRUE;
  return readendarray(file);
} /* of 'readintarray' */

Bool readrealarray(FILE *file,       /**< pointer to restart file */
                   const char *name, /**< name of object or NULL */
                   Real data[],      /**< array read from file */
                   int size,         /**< size of array */
                   Bool swap         /**< byte order has to be swapped */
                  )                  /** \return TRUE on error */
{
  int i,n;
  if(readarray(file,name,&n,swap))
    return TRUE;
  if(size!=n)
  {
    fprintf(stderr,"ERROR602: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(readreal(file,NULL,data+i,swap))
      return TRUE;
  return readendarray(file);
} /* of 'readrealarray' */

Bool readstocksarray(FILE *file,       /**< pointer to restart file */
                     const char *name, /**< name of object or NULL */
                     Stocks data[],    /**< array read from file */
                     int size,         /**< size of array */
                     Bool swap         /**< byte order has to be swapped */
                    )                  /** \return TRUE on error */
{
  int i,n;
  if(readarray(file,name,&n,swap))
    return TRUE;
  if(size!=n)
  {
    fprintf(stderr,"ERROR602: Size of array '%s'=%d is not %d.\n",name,n,size);
    return TRUE;
  }
  for(i=0;i<n;i++)
    if(readstocks(file,NULL,data+i,swap))
      return TRUE;
  return readendarray(file);
} /* of 'readstocksarray' */

Real *readvarrealarray(FILE *file,       /**< pointer to restart file */
                       const char *name, /**< name of object or NULL */
                       int *size,        /**< [out] size of array */
                       Bool swap         /**< byte order has to be swapped */
                      )                  /** \return pointer to array read or NULL on error */
{
  Real *data;
  int i;
  if(readarray(file,name,size,swap))
    return NULL;
  data=newvec(Real,*size);
  if(data==NULL)
  {
    printallocerr("data");
    return NULL;
  }
  for(i=0;i<*size;i++)
    if(readreal(file,NULL,data+i,swap))
    {
      free(data);
      return NULL;
    }
  if(readendarray(file))
  {
    free(data);
    return NULL;
  }
  return data;
} /* of 'readvarrealarray' */

Bool readstocks(FILE *file,       /**< pointer to restart file */
                const char *name, /**< name of object or NULL */
                Stocks *stocks,   /**< data read from file */
                Bool swap         /**< byte order has to be swapped */
               )                  /** \return TRUE on error */
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readreal(file,"C",&stocks->carbon,swap))
    return TRUE;
  if(readreal(file,"N",&stocks->nitrogen,swap))
    return TRUE;
  return readendstruct(file);
}

Bool readstruct(FILE *file,       /**< pointer to restart file */
                const char *name, /**< name of object or NULL */
                Bool swap         /**< byte order has to be swapped */
               )                  /** \return TRUE on error */
{ 
  Byte b;
  if(readtoken(file,&b,LPJ_STRUCT))
    return TRUE;
  if(cmpkey(file,&b,name,swap))
    return TRUE;
  if(b!=LPJ_STRUCT)
  { 
    fprintf(stderr,"ERROR601: Type of '%s'=%s is not struct.\n",name,typenames[b]);
    return TRUE;
  }
  return FALSE;
} /* of 'readstruct' */

Bool readendstruct(FILE *file /**< pointer to restart file */
                  )           /** \return TRUE on error */
{
  Byte b;
  fread(&b,1,1,file);
  if(b!=LPJ_ENDSTRUCT)
  {
    fprintf(stderr,"ERROR601: Type=%s is not endstruct.\n",typenames[b]);
    return TRUE;
  }
  return FALSE;
} /* of 'readendstruct' */

Bool readendarray(FILE *file /**< pointer to restart file */
                 )           /** \return TRUE on error */
{
  Byte b;
  fread(&b,1,1,file);
  if(b!=LPJ_ENDARRAY)
  { 
    fprintf(stderr,"ERROR601: Type=%s is not endarrary.\n",typenames[b]);
    return TRUE;
  }
  return FALSE;
} /* of 'readendarray' */
