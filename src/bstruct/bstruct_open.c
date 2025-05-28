/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  o  p  e  n  .  c                     \n**/
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
  /* Initialize name stack */
  bstruct->namestack[0].name=strdup("root");
  bstruct->namestack[0].type=BSTRUCT_BEGINSTRUCT;
  bstruct->namestack[0].nr=0;
  bstruct->namestack[0].size=0;
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
      fprintf(stderr,"ERROR517: Cannot read position of name table.\n");
    fclose(bstruct->file);
    bstruct_freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  if(filepos==0)
  {
    if(isout)
      fprintf(stderr,"ERROR524: No name table found in '%s'.\n",filename);
    fclose(bstruct->file);
    bstruct_freenamestack(bstruct);
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
    bstruct_freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  /* read name table */
  if(freadint(&bstruct->count,1,bstruct->swap,bstruct->file)!=1)
  {
    if(isout)
      fprintf(stderr,"ERROR517: Cannot read size of name table.\n");
    fclose(bstruct->file);
    bstruct_freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  bstruct->names=newvec(Hashitem,bstruct->count);
  if(bstruct->names==NULL)
  {
    printallocerr("names");
    fclose(bstruct->file);
    bstruct_freenamestack(bstruct);
    free(bstruct);
    return NULL;
  }
  bstruct->names2=newvec(Hashitem,bstruct->count);
  if(bstruct->names2==NULL)
  {
    printallocerr("names");
    fclose(bstruct->file);
    free(bstruct->names);
    bstruct_freenamestack(bstruct);
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
      bstruct_freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    bstruct->names[i].key=malloc((int)len+1);
    if(bstruct->names[i].key==NULL)
    {
      printallocerr("key");
      fclose(bstruct->file);
      bstruct_freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    if(fread(bstruct->names[i].key,len,1,bstruct->file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR508: Unexpected end of file reading name table of size %d.\n",
               bstruct->count);
      fclose(bstruct->file);
      bstruct_freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    ((char *)bstruct->names[i].key)[len]='\0';
    id=new(short);
    if(id==NULL)
    {
      printallocerr("id");
      fclose(bstruct->file);
      bstruct_freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    if(freadshort(id,1,bstruct->swap,bstruct->file)!=1)
    {
      if(isout)
        fprintf(stderr,"ERROR508: Unexpected end of file reading name table of size %d.\n",
                bstruct->count);
      fclose(bstruct->file);
      bstruct_freenamestack(bstruct);
      free(bstruct);
      return NULL;
    }
    bstruct->names[i].data=id;
#ifdef DEBUG_BSTRUCT
    printf("%d: '%s'\n",*id,(char *)bstruct->names[i].key);
#endif
  } /* of for(i=0;i<bstruct->count;i++) */
  memcpy(bstruct->names2,bstruct->names,sizeof(Hashitem)*bstruct->count);
  /* sort by id */
  qsort(bstruct->names2,bstruct->count,sizeof(Hashitem),bstruct_cmpdata);
  /* restore file position */
  fseek(bstruct->file,save,SEEK_SET);
  return bstruct;
} /* of 'bstruct_open' */
