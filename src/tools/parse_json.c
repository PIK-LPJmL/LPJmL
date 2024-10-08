/**************************************************************************************/
/**                                                                                \n**/
/**               p  a  r  s  e  _  j  s  o  n  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions parses JSON file                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <json-c/json.h>
#include "lpj.h"

LPJfile *parse_json(FILE *file,         /**< pointer to JSON text file */
                    Verbosity verbosity /**< verbosity level */
                   )                    /** \return pointer to parsed object or NULL on error */
{
  Bool first=TRUE;
  LPJfile *lpjfile;
  char *line;
  enum json_tokener_error json_error;
  struct json_tokener *tok;
  tok=json_tokener_new();
#ifdef STRICT_JSON
  json_tokener_set_flags(tok,JSON_TOKENER_STRICT);
#endif
  while((line=fscanline(file))!=NULL)  /* read line from file */
  {
    if(first)
    {
      /* check, whether file is a json file */
      if(line[0]!='{')
      {
        if(verbosity)
          fprintf(stderr,"ERROR248: JSON file '%s' does not start with opening '{' in line %d.\n",
                  getfilename(),getlinecount()-1);
        free(line);
        json_tokener_free(tok);
        return NULL;
      }
      else
        first=FALSE;
    }
    lpjfile=json_tokener_parse_ex(tok,line,strlen(line));
    json_error=json_tokener_get_error(tok);
    if(json_error!=json_tokener_continue)
      break;
    free(line);
  }
  json_tokener_free(tok);
  if(first)
  {
    if(verbosity)
      fprintf(stderr,"ERROR228: Unexpected end of file in '%s'.\n",getfilename());
    return NULL;
  }
  if(json_error!=json_tokener_success)
  {
    if(verbosity)
    {
      fprintf(stderr,"ERROR228: Cannot parse JSON file '%s' in line %d, %s:\n",
              getfilename(),getlinecount()-1,(json_error==json_tokener_continue) ? "missing closing '}'" : json_tokener_error_desc(json_error));
      if(json_error!=json_tokener_continue)
        fprintf(stderr,"%s:%d:%s",getfilename(),getlinecount()-1,line);
    }
    free(line);
    json_object_put(lpjfile);
    return NULL;
  }
  free(line);
  return lpjfile;
} /* of 'parse_json' */
