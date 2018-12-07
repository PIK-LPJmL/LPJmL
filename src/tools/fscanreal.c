/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  r  e  a  l  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a real value from a text file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef USE_JSON
#include <json-c/json.h>
#endif
#include "types.h"

Bool fscanreal(LPJfile *file,    /**< pointer to a LPJ file             */
               Real *val,        /**< real value read from file         */
               const char *name, /**< name of variable                  */
               Bool with_default, /**< allow default value */
               Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
              )                  /** \return TRUE on error              */
{
  double x;
  Bool rc;
  String line,token;
  char *ptr;
#ifdef USE_JSON
  struct json_object *item;
  if(file->isjson)
  {
    if(!json_object_object_get_ex(file->file.obj,name,&item))
    {
      if(with_default)
      {
        if(verb)
          fprintf(stderr,"WARNING027: Name '%s' for real not found, set to %g.\n",name,*val);
        return FALSE;
      }
      else
      {
        if(verb)
          fprintf(stderr,"ERROR225: Name '%s' for real not found.\n",name);
        return TRUE;
      }
    }
    if(json_object_get_type(item)!=json_type_double)
    {
      if(json_object_get_type(item)!=json_type_int)
      {
        if(verb)
          fprintf(stderr,"ERROR226: Type of '%s' is not real.\n",name);
        return TRUE;
      }
      *val=json_object_get_int(item);
    }
    else
      *val=json_object_get_double(item);
    if (verb >= VERB)
      printf("\"%s\" : %g\n",name,*val);
    return FALSE;
  }
#endif
  rc=fscantoken(file->file.file,token);
  if(!rc)
  {
     x=strtod(token,&ptr);
     rc=*ptr!='\0';
  }
  if(rc || isnan(x))
  {
    if(verb)
    {
      fprintf(stderr,"ERROR101: Cannot read int '%s' in line %d of '%s', ",
            name,getlinecount(),getfilename());
      if(strlen(token)>0)
      {
        fputs("read:\n",stderr);

        if(fgets(line,STRING_LEN,file->file.file)!=NULL)
          line[strlen(line)-1]='\0';
        else
          line[0]='\0';
        fprintf(stderr,"          '%s%s'\n           ", token,line);
        frepeatch(stderr,'^',strlen(token));
        fputc('\n',stderr);
      }
      else
        fputs("EOF reached.\n",stderr);
    }
    return TRUE; /* error occurred at read */
  }
  *val=(Real)x;
  if (verb >= VERB)
    printf("\"%s\" : %g\n",name,*val);
  return FALSE;  /* no error */
} /* of 'fscanreal' */
