/**************************************************************************************/
/**                                                                                \n**/
/**               g  e  t  f  i  l  e  f  r  o  m  m  e  t  a  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads file name from metafile                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

char *getfilefrommeta(const char *filename, /**< name of metafile */
                      Bool isout            /**< error output (TRUE/FALSE) */
                     )                      /** \return file name or NULL */
{
  LPJfile file;
  String key,value;
  char *name,*path,*fullname;
  size_t offset;
  Bool swap;
  file.isjson=FALSE;
  /* open description file */
  if((file.file.file=fopen(filename,"r"))==NULL)
  {
    if(isout)
      printfopenerr(filename);
    return NULL;
  }
  initscan(filename);
  name=NULL;
  while(!fscantoken(file.file.file,key))
    if(key[0]=='{')
    {
#ifdef USE_JSON
      name=parse_json_metafile(&file,key,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,&offset,&swap,isout ? ERR : NO_ERR);
      break;
#else
      if(isout)
        fprintf(stderr,"ERROR229: JSON format not supported for metafile '%s' in this version of LPJmL.\n",
                filename);
      free(name);
      fclose(file.file.file);
      return NULL;
#endif
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
      if(name==NULL)
      {
        printallocerr("name");
        fclose(file.file.file);
        return NULL;
      }
      break;
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
  else
  {
    path=getpath(filename);
    fullname=addpath(name,path);
    if(fullname==NULL)
    {
     printallocerr("name");
     free(path);
     free(name);
     return NULL;
    }
    free(name);
    free(path);
    name=fullname;
  }
  return name;
} /* of 'getfilefrommeta' */

char *getrealfilename(const Filename *filename)
{
  char *name;
  if(filename->fmt==META)
  {
    name=getfilefrommeta(filename->name,TRUE);
    return (name==NULL) ? strdup(filename->name) : name;
  }
  else
    return strdup(filename->name);
} /* of 'getrealfilename' */
