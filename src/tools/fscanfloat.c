/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  f  l  o  a  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a float value from a text file                              \n**/
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
#ifdef USE_JSON
#include <json-c/json.h>
#endif

#include "types.h"

Bool fscanfloat(LPJfile *file,    /**< pointer to LPJ file */
                float *value,     /**< float to be read from file */
                const char *name, /**< name of variable */
                Bool with_default, /**< allow default value */
                Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
               )                  /** \return TRUE on error */
{
  String line,token;
  char *endptr;
  Bool rc;
#ifdef USE_JSON
  struct json_object *item;  
  if(file->isjson)
  {
    if(!json_object_object_get_ex(file->file.obj,name,&item))
    {
      if(with_default)
      {
        if(verb)
          fprintf(stderr,"WARNING027: Name '%s' for float not found, set to %g.\n",name,*value);
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
      if(verb)
        fprintf(stderr,"ERROR226: Type of '%s' is not real.\n",name);
      return TRUE;
    }
    *value=json_object_get_double(item);
    if (verb >= VERB)
      printf("\"%s\" : %g\n",name,*value);
    return FALSE;
  }
#endif
  rc=fscantoken(file->file.file,token);
  if(!rc)
  {
    *value=(float)strtod(token,&endptr);
    rc=*endptr!='\0';
  }
  if(rc && verb)
  {
    fprintf(stderr,"ERROR101: Cannot read float '%s' in line %d of '%s', ",
                   name,getlinecount(),getfilename());
    if(strlen(token)>0)
    {
      fputs("read:\n",stderr);
      if(fgets(line,STRING_LEN,file->file.file)!=NULL)
        line[strlen(line)-1]='\0';
      else
        line[0]='\0';
      fprintf(stderr,"          '%s%s'\n           ",token,line);
      frepeatch(stderr,'^',strlen(token));
      fputc('\n',stderr);
    }
    else 
      fputs("EOF reached.\n",stderr);
  }
  else if (verb >= VERB)
    printf("\"%s\" : %g\n",name,*value);
  return rc;
} /* of 'fscanfloat' */
