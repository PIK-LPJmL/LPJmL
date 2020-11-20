/**************************************************************************************/
/**                                                                                \n**/
/**                r  e  a  d  f  i  l  e  n  a  m  e  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads format and file name                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readfilename(LPJfile *file,      /**< pointer to text file read */
                  Filename *filename, /**< returns filename and format */
                  const char *key,    /**< name of json object */
                  const char *path,   /**< path added to filename or NULL */
                  Bool isvar,         /**< variable name supplied */
                  Verbosity verb      /**< verbosity level (NO_ERR,ERR,VERB) */
                 )                    /** \return TRUE on error */
{
  LPJfile f;
  String name;
  if(fscanstruct(file,&f,key,verb))
    return TRUE;
  if(fscanint(&f,&filename->fmt,"fmt",FALSE,verb))
    return TRUE;
  if(filename->fmt<0 || filename->fmt>CDF)
  {
    if(verb)
      fprintf(stderr,"ERROR205: Invalid value %d for input format.\n",
              filename->fmt);
    return TRUE;
  }
  if(filename->fmt==FMS)
  {
    filename->var=NULL;
    filename->name=NULL;
    filename->unit=NULL;
    return FALSE;
  }
  if(isvar && filename->fmt==CDF)
  {
    if(fscanstring(&f,name,"var",FALSE,verb))
    {
      if(verb)
        readstringerr("variable");
      return TRUE;
    }
    else
    {
      filename->var=strdup(name);
      if(filename->var==NULL)
      {
        printallocerr("variable");
        return TRUE;
      }
    }
  }
  else
    filename->var=NULL;
  if(fscanstring(&f,name,"name",FALSE,verb))
  {
    if(verb)
      readstringerr("filename");
    free(filename->var);
    return TRUE;
  }
  filename->name=addpath(name,path); /* add path to filename */
  if(filename->name==NULL)
  {
    free(filename->var);
    return TRUE;
  }
  if(iskeydefined(&f,"unit"))
  {
    if(fscanstring(&f,name,"unit",FALSE,verb))
    {
      if(verb)
        readstringerr("unit");
      return TRUE;
    }
    else
    {
      filename->unit=strdup(name);
      if(filename->unit==NULL)
      {
        printallocerr("unit");
        return TRUE;
      }
    }
  }
  else
    filename->unit=NULL;
  if(iskeydefined(&f,"timestep"))
  {
    if(fscanint(&f,&filename->timestep,"timestep",FALSE,verb))
    {
     
      if(verb)
        fputs("ERRROR229: Cannot read int 'timestep'.\n",stderr);
      return TRUE;
    }
    if(filename->timestep<0 || filename->timestep>DAILY)
    {
      if(verb)
        fprintf(stderr,"ERRROR229: Invalid value %d for 'timestep'.\n",
                filename->timestep);
      return TRUE;
    }
  }
  else
    filename->timestep=NOT_FOUND;
  return FALSE;
} /* of 'readfilename' */
