/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  w  o  p  e  n  .  c                  \n**/
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

Bstruct bstruct_wopen(const char *filename, /**< filename of restart file to creat/append */
                      Bool append,          /**< append data to existing file */
                      Bool isout            /**< enable error output on stderr */
                     )                      /** \return pointer to restart file or NULL in case of error */
{
  /* Function creates/appends restart file */
  /*   Structure of restart file:
   *   |'B'|'S'|'T'|'R'|'U'|'C'|'T'|1|0|0|0| # file header and versiona    (7 + 4 bytes)
   *   |p1|p2|p3|p4|p5|p6|p7|p8|             # pointer to name table       (8 bytes)
   *   |00TOKEN|                             # type without id             (1 byte)
   *   |10TOKEN|id|                          # type with byte id           (2 bytes)
   *   |11TOKEN|id1|id2|                     # type with word id           (3 bytes)
   *   |10TOKEN|id|d1|d2...|dn|              # type with byte id and data  (2 bytes + sizeof(data))
   *   ...
   *   |BSTRUCT_END|                         # end token                   (1 byte)
   *   |size1|size2|size3|size4|             # size of name table          (4 bytes)
   *   |n|s1|s2|...|sn|                      # name                        (1+ name length bytes)
   *   |id1|id2|                             # id                          (2 bytes)
   *   ....
   */
  long long filepos;
  char *name;
  short *id;
  Bstruct bstruct;
  int i,version,count;
  Byte len;
  bstruct=new(struct bstruct);
  if(bstruct==NULL)
  {
    printallocerr("bstruct");
    return NULL;
  }
  bstruct->isout=isout;
  /* Initialize name stack */
  bstruct->level=1;
  bstruct->namestack[0].type=BSTRUCT_BEGINSTRUCT;
  bstruct->namestack[0].nr=0;
  bstruct->namestack[0].size=0;
  bstruct->namestack[0].varnames=NULL;
  bstruct->namestack[0].name=NULL;
  bstruct->names=NULL;
  bstruct->names2=NULL;
  bstruct->count=0;
  bstruct->file=fopen(filename,(append) ? "r+b" : "wb");
  if(bstruct->file==NULL)
  {
    if(isout)
    {
      if(append)
        printfopenerr(filename);
      else
        printfcreateerr(filename);
    }
    free(bstruct);
    return NULL;
  }
  bstruct->hash=newhash(BSTRUCT_HASHSIZE,bstruct_gethashkey,free);
  if(bstruct->hash==NULL)
  {
    printallocerr("hash");
    fclose(bstruct->file);
    free(bstruct);
    return NULL;
  }
  if(append)
  {
    /* append data to old file */
    version=READ_VERSION;
    if(freadtopheader(bstruct->file,&bstruct->swap,BSTRUCT_HEADER,&version,isout))
    {
      if(isout)
        fprintf(stderr,"ERROR513: Invalid header in file '%s'.\n",filename);
      fclose(bstruct->file);
      freehash(bstruct->hash);
      free(bstruct);
      return NULL;
    }
    if(version!=BSTRUCT_VERSION)
    {
      if(isout)
        fprintf(stderr,"ERROR514: Invalid version %d in file '%s', must be %d.\n",
                version,filename,BSTRUCT_VERSION);
      fclose(bstruct->file);
      freehash(bstruct->hash);
      free(bstruct);
      return NULL;
    }
    if(bstruct->swap)
    {
      if(isout)
        fprintf(stderr,"ERROR525: Byte order must not be different file '%s'.\n",
                filename);
      fclose(bstruct->file);
      freehash(bstruct->hash);
      free(bstruct);
      return NULL;
    }
    /* read position of name table */
    if(fread(&filepos,sizeof(filepos),1,bstruct->file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR517: Cannot read file position of name table.\n");
      freehash(bstruct->hash);
      fclose(bstruct->file);
      free(bstruct);
      return NULL;
    }
    if(filepos==0)
      fseek(bstruct->file,0,SEEK_END); /* seek to end of file for append */
    else
    {
      /* a name table is stored in the restart file */
      if(fseek(bstruct->file,filepos,SEEK_SET))
      {
        if(isout)
          fprintf(stderr,"ERROR517: Cannot seek to name table.\n");
        fclose(bstruct->file);
        freehash(bstruct->hash);
        free(bstruct);
        return NULL;
      }
      /* read size of name table */
      if(fread(&count,sizeof(count),1,bstruct->file)!=1)
      {
        if(isout)
          fprintf(stderr,"ERROR517: Cannot read size of name table.\n");
        freehash(bstruct->hash);
        fclose(bstruct->file);
        free(bstruct);
        return NULL;
      }
      /* read name table */
      for(i=0;i<count;i++)
      {
        if(fread(&len,1,1,bstruct->file)!=1)
        {
          if(isout)
            fprintf(stderr,"ERROR517: Cannot read length of name in table.\n");
          freehash(bstruct->hash);
          fclose(bstruct->file);
          free(bstruct);
          return NULL;
        }
        name=malloc((int)len+1);
        if(name==NULL)
        {
          printallocerr("name");
          freehash(bstruct->hash);
          fclose(bstruct->file);
          free(bstruct);
          return NULL;
        }
        if(fread(name,len,1,bstruct->file)!=1)
        {
          if(isout)
            fprintf(stderr,"ERROR517: Cannot read name in table.\n");
          free(name);
          freehash(bstruct->hash);
          fclose(bstruct->file);
          free(bstruct);
          return NULL;
        }
        name[len]='\0';
        id=new(short);
        if(id==NULL)
        {
          printallocerr("id");
          free(name);
          freehash(bstruct->hash);
          fclose(bstruct->file);
          free(bstruct);
          return NULL;
        }
        if(fread(id,sizeof(short),1,bstruct->file)!=1)
        {
          if(isout)
            fprintf(stderr,"ERROR508: Unexpected end of file reading name table of size %d.\n",
                    count);
          free(name);
          free(id);
          freehash(bstruct->hash);
          fclose(bstruct->file);
          free(bstruct);
          return NULL;
        }
        if(addhashitem(bstruct->hash,name,id)==0)
        {
          printallocerr("hash");
          freehash(bstruct->hash);
          fclose(bstruct->file);
          free(bstruct);
          return NULL;
        }
      }
      /* skip to one byte before the name table to remove BSTRUCT_END token */
      fseek(bstruct->file,filepos-1,SEEK_SET);
    }
  }
  else
  {
    fwritetopheader(bstruct->file,BSTRUCT_HEADER,BSTRUCT_VERSION);
    /* initialize file position of name table to zero and write to file */
    filepos=0;
    fwrite(&filepos,sizeof(filepos),1,bstruct->file);
  }
  return bstruct;
} /* of 'bstruct_wopen' */
