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

char *getmetafilename(const char *filename, /**< file name */
                     )           /** \return file pointer to open file or NULL */
{
  FILE *file;
  String key,value;
  char *name,*path,*fullname;
  int index,ival;
  size_t offset;
  float val;
  /* open description file */
  if((file=fopen(filename,"rb"))==NULL)
  {
    printfopenerr(filename);
    return NULL;
  }
  /* set default values for swap and offset */
  name=NULL;
  while(fscanf(file,"%255s",key)==1)
    if(!strcmp(key,"firstcell"))
    {
      if(fscanint(file,&ival,"firstcell",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"ncell"))
    {
      if(fscanint(file,&ival,"ncell",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"firstyear"))
    {
      if(fscanint(file,&ival,"firstyear",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"nyear"))
    {
      if(fscanint(file,&ival,"nyear",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"nbands"))
    {
      if(fscanint(file,&ival,"nbands",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"offset"))
    {
      if(fscansize(file,&offset,"offset",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"scalar"))
    {
      if(fscanfloat(file,&fval,"scalar",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"cellsize"))
    {
      if(fscanfloat(file,&fval,"cellsize_lon",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
      if(fscanfloat(file,&fval,"cellsize_lat",isout))
      {
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"datatype"))
    {
      if(fscanstring(file,value))
      {
        if(isout)
          readstringerr("datatype");
        free(name);
        fclose(file);
        return NULL;
      }
      index=findstr(value,typenames,5);
      if(index==NOT_FOUND)
      {
        fprintf(stderr,"ERROR221: Invalid datatype '%s' in '%s'.\n",value,filename);
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"endian"))
    {
      if(fscanstring(file,value))
      {
        readstringerr("endian");
        free(name);
        fclose(file);
        return NULL;
      }
      if(strcmp(value,"little") && strcmp(value,"big"))
      {
        fprintf(stderr,"ERROR221: Invalid endianness '%s' in '%s'.\n",value,filename);
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else if(!strcmp(key,"file"))
    {
      if(fscanstring(file,value))
      {
        if(isout)
          readstringerr("file");
        free(name);
        fclose(file);
        return NULL;
      }
      name=strdup(value);
    }
    else if(!strcmp(key,"remark"))
    {
      if(fscanstring(file,value))
      {
        if(isout)
          readstringerr("remark");
        free(name);
        fclose(file);
        return NULL;
      }
    }
    else
    {
      fprintf(stderr,"ERROR222: Invalid key word '%s' in '%s'.\n",key,filename);
      free(name);
      fclose(file);
      return NULL;
    }
  fclose(file);
  if(name==NULL)
  {
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
  return name;
} /* of 'openmetafile' */
