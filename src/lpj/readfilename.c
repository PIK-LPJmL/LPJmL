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

Bool readfilename(FILE *file,         /**< pointer to text file read */
                  Filename *filename, /**< returns filename and format */
                  const char *path,   /**< path added to filename or NULL */
                  Bool isvar,         /**< variable name supplied */
                  Verbosity verb      /**< verbosity level (NO_ERR,ERR,VERB) */
                 )                    /** \return TRUE on error */
{
  String name;
  if(fscanint(file,&filename->fmt,"format",verb))
    return TRUE;
  if(filename->fmt<0 || filename->fmt>CDF)
  {
    if(verb)
      fprintf(stderr,"ERROR205: Invalid value for input format in line %d of '%s'.\n",getlinecount(),getfilename());
    return TRUE;
  }
  if(filename->fmt==FMS)
  {
    filename->var=NULL;
    filename->name=NULL;
    return FALSE;
  }
  if(isvar && filename->fmt==CDF)
  {
    if(fscanstring(file,name,verb!=NO_ERR))
    {
      if(verb)
        readstringerr("variable");
      return TRUE;
    }
    else
    {
      if(verb>=VERB)
        printf("variable %s\n",name);
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
  if(fscanstring(file,name,verb!=NO_ERR))
  {
    if(verb)
      readstringerr("filename");
    free(filename->var);
    return TRUE;
  }
  if(verb>=VERB)
    printf("filename %s\n",name);
  filename->name=addpath(name,path); /* add path to filename */
  if(filename->name==NULL)
  {
    free(filename->var);
    return TRUE;
  }
  return FALSE;
} /* of 'readfilename' */
