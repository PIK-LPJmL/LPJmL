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

const char *fmt[N_FMT]={"raw","clm","clm2","txt","fms","meta","cdf"};
const char *time_step[]={"annual","monthly","daily"};

Bool readfilename(LPJfile *file,      /**< pointer to text file read */
                  Filename *filename, /**< returns filename and format */
                  const char *key,    /**< name of json object */
                  const char *path,   /**< path added to filename or NULL */
                  Bool isvar,         /**< variable name supplied */
                  Verbosity verb      /**< verbosity level (NO_ERR,ERR,VERB) */
                 )                    /** \return TRUE on error */
{
  LPJfile *f;
  String name;
  f=fscanstruct(file,key,verb);
  if(f==NULL)
    return TRUE;
  if(fscankeywords(f,&filename->fmt,"fmt",fmt,N_FMT,FALSE,verb))
    return TRUE;
  if(filename->fmt==FMS)
  {
    filename->var=NULL;
    filename->name=NULL;
    filename->time=NULL;
    filename->unit=NULL;
    return FALSE;
  }
  if(isvar && filename->fmt==CDF)
  {
    if(fscanstring(f,name,"var",FALSE,verb))
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
    if(iskeydefined(f,"time"))
    {
      if(fscanstring(f,name,"time",FALSE,verb))
      {
        if(verb)
          readstringerr("time");
        return TRUE;
      }
      else
      {
        filename->time=strdup(name);
        if(filename->time==NULL)
        {
          printallocerr("time");
          return TRUE;
        }
      }
    }
    else
      filename->time=NULL;
  }
  else
  {
    if(iskeydefined(f,"var"))
    {
      if(fscanstring(f,name,"var",FALSE,verb))
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
    if(iskeydefined(f,"scale"))
    {
      filename->isscale=TRUE;
      if(fscanreal(f,&filename->scale,"scale",FALSE,verb))
      {
        if(verb)
          readstringerr("scale");
        return TRUE;
      }
      if(filename->scale==0)
      {
        if(verb)
          fprintf(stderr,"ERROR229: Scale must not be zero.\n");
        return TRUE;
      }
    }
    else
      filename->isscale=FALSE;
    filename->time=NULL;
  }
  if(fscanstring(f,name,"name",FALSE,verb))
  {
    if(verb)
      readstringerr("filename");
    free(filename->var);
    return TRUE;
  }
  filename->name=addpath(name,path); /* add path to filename */
  if(filename->name==NULL)
  {
    printallocerr("name");
    free(filename->var);
    return TRUE;
  }
  if(iskeydefined(f,"metafile"))
  {
    if(fscanbool(f,&filename->meta,"metafile",FALSE,verb))
    {
      free(filename->var);
      return TRUE;
    }
  }
  if(iskeydefined(f,"version"))
  {
    if(fscanint(f,&filename->version,"version",FALSE,verb))
    {
      free(filename->var);
      return TRUE;
    }
    if(filename->version<1 || filename->version>CLM_MAX_VERSION)
    {
      if(verb)
       fprintf(stderr,"ERROR229: Invalid version %d, must be in [1,%d].\n",
               filename->version,CLM_MAX_VERSION);
      free(filename->var);
      return TRUE;
    }

  }
  if(iskeydefined(f,"unit"))
  {
    if(fscanstring(f,name,"unit",FALSE,verb))
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
  if(iskeydefined(f,"timestep"))
  {
    if(fscantimestep(f,&filename->timestep,verb))
    {
      if(verb)
        fputs("ERROR229: Cannot read int 'timestep'.\n",stderr);
      return TRUE;
    }
  }
  else
    filename->timestep=NOT_FOUND;
  return FALSE;
} /* of 'readfilename' */
