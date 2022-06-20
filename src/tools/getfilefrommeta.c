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
  FILE *file;
  char *name,*path,*fullname;
  size_t offset;
  Bool swap;
  /* open description file */
  if((file=fopen(filename,"r"))==NULL)
  {
    if(isout)
      printfopenerr(filename);
    return NULL;
  }
  initscan(filename);
  name=parse_json_metafile(file,NULL,NULL,NULL,&offset,&swap,isout ? ERR : NO_ERR);
  fclose(file);
  if(name==NULL)
  {
    if(isout)
      fprintf(stderr,"ERROR223: No filename specified in '%s'.\n",filename);
    return NULL;
  }
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
