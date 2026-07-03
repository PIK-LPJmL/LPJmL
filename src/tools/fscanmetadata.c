/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  m  e  t  a  d  a  t  a  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads metadata information from JSON file                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanmetadata(LPJfile *file,      /**< pointer to JSON file */
                   Metadata *metadata, /**< metadata information read */
                   Verbosity verbosity /**< verbosity level */
                  )                    /** \return TRUE on error */
{
  LPJfile *array,*item;
  const char *val;
  int i;
  if(iskeydefined(file,"global_attrs"))
  {
    if(fscanattrs(file,&metadata->attrs,&metadata->n_attr,"global_attrs",verbosity))
      return TRUE;
  }
  if(iskeydefined(file,metadata->map_name))
    metadata->map=fscanmap(file,metadata->map_name,verbosity);
  if(iskeydefined(file,"countrymap"))
  {
    metadata->countrymap=fscancountrymap(file,&metadata->countrymap_size,"countrymap",verbosity);
    if(metadata->countrymap==NULL)
      return TRUE;
  }
  if(iskeydefined(file,"basetemp"))
  {
    array=fscanarray(file,&metadata->basetemp_size,"basetemp",verbosity);
    if(array==NULL)
      return TRUE;
    metadata->basetemp=newvec(Limit,metadata->basetemp_size);
    if(metadata->basetemp==NULL)
    {
      printallocerr("limits");
      return TRUE;
    }
    for(i=0;i<metadata->basetemp_size;i++)
    {
      item=fscanarrayindex(array,i);
      if(fscanlimit(item,metadata->basetemp+i,NULL,verbosity))
        return TRUE;
    }
  }
  if(iskeydefined(file,"hlimit"))
    metadata->hlimit=fscanvarintarray(file,&metadata->hlimit_size,"hlimit",verbosity);
  if(iskeydefined(file,"variable"))
  {
    val=fscanstring(file,NULL,"variable",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->variable=strdup(val);
  }
  if(iskeydefined(file,"source"))
  {
    val=fscanstring(file,NULL,"source",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->source=strdup(val);
  }
  if(iskeydefined(file,"history"))
  {
    val=fscanstring(file,NULL,"history",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->history=strdup(val);
  }
  if(iskeydefined(file,"unit"))
  {
    val=fscanstring(file,NULL,"unit",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->unit=strdup(val);
  }
  if(iskeydefined(file,"standard_name"))
  {
    val=fscanstring(file,NULL,"standard_name",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->standard_name=strdup(val);
  }
  if(iskeydefined(file,"long_name"))
  {
    val=fscanstring(file,NULL,"long_name",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->long_name=strdup(val);
  }
  if(iskeydefined(file,"descr"))
  {
    val=fscanstring(file,NULL,"descr",verbosity);
    if(val==NULL)
      return TRUE;
    metadata->long_name=strdup(val);
  }
  return FALSE;
} /* of 'fscanmetadata' */
