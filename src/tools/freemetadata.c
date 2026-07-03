/**************************************************************************************/
/**                                                                                \n**/
/**                f  r  e  e  m  e  t  a  d  a  t  a  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates metadata information                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freemetadata(Metadata *metadata /**< metadata information */
                 )
{
  if(metadata!=NULL)
  {
    freemap(metadata->map);
    freeattrs(metadata->attrs,metadata->n_attr);
    free(metadata->basetemp);
    freecountrymap(metadata->countrymap,metadata->countrymap_size);
    free(metadata->hlimit);
    free(metadata->source);
    free(metadata->history);
    free(metadata->variable);
    free(metadata->unit);
    free(metadata->standard_name);
    free(metadata->long_name);
    /* set pointers to NULL to avoid double free corruption if freemetadat is called twice */
    initmetadata(metadata,NULL);
  }
} /* of 'freemetadata' */
