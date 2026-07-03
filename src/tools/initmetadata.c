/**************************************************************************************/
/**                                                                                \n**/
/**               i  n  i  t  m  e  t  a  d  a  t  a  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes metadata information                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

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
  metadata->countrymap=NULL;
  metadata->countrymap_size=0;
  metadata->hlimit=NULL;
  metadata->hlimit_size=0;
  metadata->source=NULL;
  metadata->history=NULL;
  metadata->variable=NULL;
  metadata->unit=NULL;
  metadata->standard_name=NULL;
  metadata->long_name=NULL;
} /* of 'initmetadata' */
