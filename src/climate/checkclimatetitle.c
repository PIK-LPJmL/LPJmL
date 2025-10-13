/**************************************************************************************/
/**                                                                                \n**/
/**               c  h  e  c  k  c  l  i  m  a  t  e  t  i  t  l  e  .  c          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for matching titles for climate input files                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool checkclimatetitle(const Attr *attrs,    /**< pointer to array of attributes or NULL */
                       int n_attr,           /**< size of array attribute */
                       const char *filename, /**< filename of climate input file */
                       Config *config        /**< LPJmL configuration */
                      )                      /** \return FALSE on matching titles */
{
  char *climate;
  Bool rc=FALSE;
  if(attrs!=NULL)
  {
    climate=getattr(attrs,n_attr,"title");
    if(climate!=NULL)
    {
      if(config->climate==NULL)
        config->climate=climate;
      else
      {
        if(strcmp(climate,config->climate))
        {
          if(isroot(*config))
            fprintf(stderr,"WARNING043: Climate %s in file '%s' differs from %s.\n",
                    climate,filename,config->climate);
          rc=TRUE;
        }
        free(climate);
      }
    }
  }
  return rc;
} /* of 'checkclimatetitle' */
