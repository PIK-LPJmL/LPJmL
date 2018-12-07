/**************************************************************************************/
/**                                                                                \n**/
/**               o  p  e  n  m  e  t  a  f  i  l  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions opens input desription file and opens specified binary           \n**/
/**     file. Description file has the following format:                           \n**/
/**                                                                                \n**/
/**     remark "metafile for LPJmL input"                                          \n**/
/**     file inputfile.bin                                                         \n**/
/**     firstyear 1901                                                             \n**/
/**     nyear 109                                                                  \n**/
/**     nbands 12                                                                  \n**/
/**     endian little                                                              \n**/
/**     firstcell 0                                                                \n**/
/**     ncell 67420                                                                \n**/
/**     scalar 0.1                                                                 \n**/
/**     cellsize 0.5 0.5                                                           \n**/
/**     offset 0                                                                   \n**/
/**     datatype short                                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

FILE *openmetafile(Header *header, /**< pointer to file header */
                   Bool *swap, /**< byte order has to be changed (TRUE/FALSE) */
                   size_t *offset, /**< offset in binary file */
                   const char *filename, /**< file name */
                   Bool isout /**< error output (TRUE/FALSE) */
                  )           /** \return file pointer to open file or NULL */
{
  LPJfile file;
  FILE *data;
  String key,value;
  char *name,*path,*fullname;
  int index;
  file.isjson=FALSE;
  /* open description file */
  if((file.file.file=fopen(filename,"rb"))==NULL)
  {
    if(isout)
      printfopenerr(filename);
    return NULL;
  }
  initscan(filename);
  /* set default values for swap and offset */
  *swap=FALSE;
  *offset=0;
  name=NULL;
  while(!fscantoken(file.file.file,key))
    if(!strcmp(key,"firstcell"))
    {
      if(fscanint(&file,&header->firstcell,"firstcell",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"ncell"))
    {
      if(fscanint(&file,&header->ncell,"ncell",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"firstyear"))
    {
      if(fscanint(&file,&header->firstyear,"firstyear",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"lastyear"))
    {
      if(fscanint(&file,&header->nyear,"firstyear",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      header->nyear-=header->firstyear-1;
    }
    else if(!strcmp(key,"nyear"))
    {
      if(fscanint(&file,&header->nyear,"nyear",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"nbands"))
    {
      if(fscanint(&file,&header->nbands,"nbands",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"offset"))
    {
      if(fscansize(&file,offset,"offset",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"scalar"))
    {
      if(fscanfloat(&file,&header->scalar,"scalar",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"cellsize"))
    {
      if(fscanfloat(&file,&header->cellsize_lon,"cellsize_lon",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      if(fscanfloat(&file,&header->cellsize_lat,"cellsize_lat",FALSE,isout ? ERR : NO_ERR))
      {
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"datatype"))
    {
      if(fscanstring(&file,value,"datatype",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("datatype");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      index=findstr(value,typenames,5);
      if(index==NOT_FOUND)
      {
        if(isout)
          fprintf(stderr,"ERROR221: Invalid datatype '%s' in '%s'.\n",value,filename);
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      header->datatype=(Type)index;
    }
    else if(!strcmp(key,"endian"))
    {
      if(fscanstring(&file,value,"endian",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("endian");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      if(!strcmp(value,"little"))
        *swap=bigendian();
      else if(!strcmp(value,"big"))
        *swap=!bigendian();
      else
      {
        if(isout)
          fprintf(stderr,"ERROR221: Invalid endianness '%s' in line %d of '%s'.\n",value,getlinecount(),filename);
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else if(!strcmp(key,"file"))
    {
      if(fscanstring(&file,value,"file",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("file");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
      name=strdup(value);
    }
    else if(!strcmp(key,"remark"))
    {
      if(fscanstring(&file,value,"remark",FALSE,isout ? ERR : NO_ERR))
      {
        if(isout)
          readstringerr("remark");
        free(name);
        fclose(file.file.file);
        return NULL;
      }
    }
    else
    {
      if(isout)
        fprintf(stderr,"ERROR222: Invalid key word '%s' in line %d of '%s'.\n",key,getlinecount(),filename);
      free(name);
      fclose(file.file.file);
      return NULL;
    }
  fclose(file.file.file);
  if(name==NULL)
  {
    if(isout)
      fprintf(stderr,"ERROR223: No filename specified in '%s'.\n",filename);
    return NULL;
  }
  if(name[0]=='^') 
  {
     /* if filename starts with a '^' then path of description file is added to filename */
     path=getpath(filename);
     if(path==NULL)
     {
       printallocerr("path");
       free(name);
       return NULL;
     }
     fullname=malloc(strlen(path)+strlen(name)+1);
     if(fullname==NULL)
     {
       printallocerr("name");
       free(path);
       free(name);
       return NULL;
     }
     strcpy(fullname,path);
     strcat(fullname,"/");
     strcat(fullname,name+1);
     free(name);
     free(path);
     name=fullname;
  }
  /* open data file */
  if((data=fopen(name,"rb"))==NULL  && isout)
    printfopenerr(name);
  free(name);
  return data;
} /* of 'openmetafile' */
