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

char *fmt[N_FMT]={"raw","clm","clm2","txt","fms","meta","cdf","sock"};
char *time_step[]={"annual","monthly","daily"};

Bool readfilename(LPJfile *file,      /**< pointer to text file read */
                  Filename *filename, /**< returns filename and format */
                  const char *key,    /**< name of json object */
                  const char *path,   /**< path added to filename or NULL */
                  Bool isfmt,         /**< format name supplied */
                  Bool isvar,         /**< variable name supplied */
                  Bool isid,          /**< id for socket supplied */
                  Verbosity verb      /**< verbosity level (NO_ERR,ERR,VERB) */
                 )                    /** \return TRUE on error */
{
  LPJfile *f;
  const char *name;
  f=fscanstruct(file,key,verb);
  if(f==NULL)
    return TRUE;
  if(isfmt || iskeydefined(f,"fmt"))
  {
    if(fscankeywords(f,&filename->fmt,"fmt",fmt,N_FMT,FALSE,verb))
      return TRUE;
  }
  filename->issocket=FALSE;
  if(filename->fmt==FMS)
  {
    filename->timestep=NOT_FOUND;
    filename->var=NULL;
    filename->map=NULL;
    filename->name=NULL;
    filename->time=NULL;
    filename->unit=NULL;
    return FALSE;
  }
  if(iskeydefined(f,"map"))
  {
    name=fscanstring(f,NULL,"map",verb);
    if(name==NULL)
    {
      if(verb)
        readstringerr("map");
      return TRUE;
    }
    else
    {
      filename->map=strdup(name);
      if(filename->map==NULL)
      {
        printallocerr("map");
        return TRUE;
      }
    }
  }
  else
    filename->map=NULL;
  if(isvar && filename->fmt==CDF)
  {
    name=fscanstring(f,NULL,"var",verb);
    if(name==NULL)
    {
      if(verb)
        readstringerr("var");
      return TRUE;
    }
    else
    {
      filename->var=strdup(name);
      if(filename->var==NULL)
      {
        printallocerr("var");
        return TRUE;
      }
    }
    if(iskeydefined(f,"time"))
    {
      name=fscanstring(f,NULL,"time",verb);
      if(name==NULL)
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
      name=fscanstring(f,NULL,"var",verb);
      if(name==NULL)
      {
        if(verb)
          readstringerr("var");
        return TRUE;
      }
      else
      {
        filename->var=strdup(name);
        if(filename->var==NULL)
        {
          printallocerr("var");
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
  if(filename->fmt==SOCK)
  {
    if(isid)
    {
      if(fscanint(f,&filename->id,"id",FALSE,verb))
        return TRUE;
    }
    else
    {
      if(iskeydefined(f,"id"))
      {
        if(fscanint(f,&filename->id,"id",FALSE,verb))
          return TRUE;
      }
    }
    filename->name=NULL;
    filename->meta=FALSE;
    filename->issocket=TRUE;
  }
  else
  {
    name=fscanstring(f,NULL,"name",verb);
    if(name==NULL)
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
    if(iskeydefined(f,"socket"))
    {
      if(fscanbool(f,&filename->issocket,"socket",FALSE,verb))
      {
        free(filename->var);
        return TRUE;
      }
      if(filename->issocket && iskeydefined(f,"id"))
      {
        if(fscanint(f,&filename->id,"id",FALSE,verb))
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
  }
  if(iskeydefined(f,"unit"))
  {
    name=fscanstring(f,NULL,"unit",verb);
    if(name==NULL)
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
