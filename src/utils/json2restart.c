/**************************************************************************************/
/**                                                                                \n**/
/**                j  s  o  n  2  r  e  s  t  a  r  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Utility converts JSON file to restart file                                 \n**/
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
#include <errno.h>
#include <json-c/json.h>
#include "types.h"
#include "errmsg.h"
#include "hash.h"
#include "bstruct.h"

static Bool index_found=FALSE;

#define USAGE "Usage: %s [-index name] [-noindex] jsonfile restartfile\n"

Bool parse_object(Bstruct bstr,struct json_object *file,const char *index);

Bool getobject(Bstruct bstr,struct json_object *obj,const char *name,const char *index)
{
  const char *s;
  double value;
  long long filepos,*pos;
  int i,ivalue,size;
  switch(json_object_get_type(obj))
  {
    case json_type_null:
      bstruct_writenull(bstr,name);
      break;
    case json_type_double:
      value=json_object_get_double(obj);
      bstruct_writedouble(bstr,name,value);
      break;
    case json_type_int:
      ivalue=json_object_get_int(obj);
      bstruct_writeint(bstr,name,ivalue);
      break;
    case json_type_boolean:
      ivalue=json_object_get_boolean(obj);
      bstruct_writebool(bstr,name,ivalue);
      break;
    case json_type_string:
      s=json_object_get_string(obj);
      bstruct_writestring(bstr,name,s);
      break;
    case json_type_array:
      size=json_object_array_length(obj);
      if(name!=NULL && index!=NULL && !strcmp(name,index))
      {
        index_found=TRUE;
        bstruct_writebeginindexarray(bstr,name,&filepos,size);
        pos=newvec(long long,size);
        if(pos==NULL)
        {
          printallocerr("index");
          return TRUE;
        }
        for(i=0;i<size;i++)
        {
          pos[i]=bstruct_getarrayindex(bstr);
          if(getobject(bstr,json_object_array_get_idx(obj,i),NULL,index))
          {
            free(pos);
            return TRUE;
          }
        }
        bstruct_writearrayindex(bstr,filepos,pos,0,size);
        free(pos);
      }
      else
      {
        bstruct_writebeginarray(bstr,name,size);
        for(i=0;i<size;i++)
          if(getobject(bstr,json_object_array_get_idx(obj,i),NULL,index))
            return TRUE;
      }
      bstruct_writeendarray(bstr);
      break;
    case json_type_object:
      bstruct_writebeginstruct(bstr,name);
      if(parse_object(bstr,obj,index))
        return TRUE;
      bstruct_writeendstruct(bstr);
      break;
    default:
      fprintf(stderr,"Unsupported object type.\n");
      return TRUE;
  } /* of switch() */
  return FALSE;
} /* of 'getobject' */

Bool parse_object(Bstruct bstr,LPJfile *file,const char *index)
{
  struct json_object_iterator it,it_end;
  struct json_object *obj;
  const char *name;
  it_end=json_object_iter_end(file);
  for(it=json_object_iter_begin(file);!json_object_iter_equal(&it,&it_end);json_object_iter_next(&it))
  {
    name=json_object_iter_peek_name(&it);
    obj=json_object_iter_peek_value(&it);
    if(getobject(bstr,obj,name,index))
     return TRUE;
  }
  return FALSE;
} /* of 'parse_object' */

int main(int argc,char **argv)
{
  FILE *file;
  LPJfile *lpjfile;
  char *index;
  Bstruct bstr;
  Bool rc;
  int iarg;
  index="grid";
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-index"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-index'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        index=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-noindex"))
        index=NULL;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+2)
  {
    fprintf(stderr,"Filename(s) missing.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  /* open JSON file */
  if((file=fopen(argv[iarg],"r"))==NULL)
  {
    printfopenerr(argv[iarg]);
    return EXIT_FAILURE;
  }
  bstr=bstruct_create(argv[iarg+1]);
  if(bstr==NULL)
    return EXIT_FAILURE;
  initscan(argv[iarg]);
  lpjfile=parse_json(file,ERR);
  fclose(file);
  if(lpjfile==NULL)
    return EXIT_FAILURE;
  rc=parse_object(bstr,lpjfile,index);
  bstruct_finish(bstr);
  json_object_put(lpjfile);
  if(index!=NULL && !index_found)
    fprintf(stderr,"No array '%s' found in JSON file '%s' where array index should be written.\n",
            index,argv[iarg]);
  return (rc) ? EXIT_FAILURE : EXIT_SUCCESS;
} /* of 'main' */
