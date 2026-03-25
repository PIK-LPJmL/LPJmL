/**************************************************************************************/
/**                                                                                \n**/
/**               f  p  r  i  n  t  m  e  t  a  d  a  t  a  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions prints metadata information to JSOM files                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define LINE_LEN 80

void initmetadata(Metadata *metadata,  /**< metadata information */
                  const char *map_name /**< name of map or NULL */
                 )
{
  metadata->map_name=(map_name==NULL) ? MAP_NAME : map_name;
  metadata->map=NULL;
  metadata->attrs=NULL;
  metadata->n_attr=0;
  metadata->basetemp=NULL;
  metadata->basetemp_size=0;
  metadata->hlimit=NULL;
  metadata->hlimit_size=0;
  metadata->source=NULL;
  metadata->history=NULL;
  metadata->variable=NULL;
  metadata->unit=NULL;
  metadata->standard_name=NULL;
  metadata->long_name=NULL;
} /* of 'initmetadata' */

void freemetadata(Metadata *metadata /**< metadata information */
                 )
{
  freemap(metadata->map);
  freeattrs(metadata->attrs,metadata->n_attr);
  free(metadata->basetemp);
  free(metadata->hlimit);
  free(metadata->source);
  free(metadata->history);
  free(metadata->variable);
  free(metadata->unit);
  free(metadata->standard_name);
  free(metadata->long_name);
} /* of 'freemetadata' */

void fprintmetadata(FILE *file,               /**< pointer to text file */
                    const Metadata *metadata, /**< metadata information printed */
                    const char *arglist       /**< run-time arguments or NULL */
                   )

{
  int i,len;
  time_t t;
  if(metadata->history==NULL)
  {
    if(arglist!=NULL)
    {
      time(&t);
      fprintf(file,"  \"history\" : \"%s: %s\",\n",strdate(&t),arglist);
    }
  }
  else
  {
    if(arglist!=NULL)
    {
      time(&t);
      fprintf(file,"  \"history\" : \"");
      fputprintable(file,metadata->history);
      fprintf(file,"\\n%s: %s\",\n",strdate(&t),arglist);
    }
    else
    {
      fprintf(file,"  \"history\" : \"");
      fputprintable(file,metadata->history);
      fprintf(file,"\",\n");
    }
  }
  if(metadata->attrs!=NULL && metadata->n_attr>0)
  {
    fprintf(file,"  \"global_attrs\" :\n  {\n");
    for(i=0;i<metadata->n_attr;i++)
    {
      fprintf(file,"    \"%s\" : \"%s\"",metadata->attrs[i].name,metadata->attrs[i].value);
      if(i<metadata->n_attr-1)
        fprintf(file,",\n");
    }
    fprintf(file,"\n  },\n");
  }
  if(metadata->variable!=NULL)
    fprintf(file,"  \"variable\" : \"%s\",\n",metadata->variable);
  if(metadata->map!=NULL)
  {
    len=fprintf(file,"  \"%s\" : [",metadata->map_name);
    for(i=0;i<getmapsize(metadata->map);i++)
    {
      if(i)
        len+=fprintf(file,",");
      if(len>LINE_LEN)
        len=fprintf(file,"\n    ")-1;
      if(getmapitem(metadata->map,i)==NULL)
        len+=fprintf(file,"null");
      else if(metadata->map->isfloat)
        len+=fprintf(file,"%g",*((double *)getmapitem(metadata->map,i)));
      else
        len+=fprintf(file,"\"%s\"",(char *)getmapitem(metadata->map,i));
    }
    fputs("],\n",file);
  }
  if(metadata->basetemp!=NULL)
  {
    fputs("  \"basetemp\" : [\n",file);
    for(i=0;i<metadata->basetemp_size;i++)
    {
      fprintf(file,"    { \"low\" : %g, \"high\" : %g }",
              metadata->basetemp[i].low,metadata->basetemp[i].high);
      if(i<metadata->basetemp_size-1)
        fputs(",\n",file);
      else
        fputc('\n',file);
    }
    fputs("  ],\n",file);
  }
  if(metadata->hlimit!=NULL)
  {
    fputs("  \"hlimit\" : [",file);
    for(i=0;i<metadata->hlimit_size;i++)
      fprintf(file,(i<metadata->basetemp_size-1) ? "%d," : "%d",metadata->hlimit[i]);
    fputs("],\n",file);
  }
  if(metadata->unit!=NULL)
    fprintf(file,"  \"unit\" : \"%s\",\n",metadata->unit);
  if(metadata->standard_name!=NULL)
    fprintf(file,"  \"standard_name\" : \"%s\",\n",metadata->standard_name);
  if(metadata->long_name!=NULL)
    fprintf(file,"  \"long_name\" : \"%s\",\n",metadata->long_name);
} /* of 'fprintmetadata' */
