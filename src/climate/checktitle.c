/**************************************************************************************/
/**                                                                                \n**/
/**               c  h  e  c  k  t  i  t  l  e  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for matching titles for input files                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool checktitle(const Attr *attrs,    /**< pointer to array of attributes or NULL */
                int n_attr,           /**< size of array attribute */
                const char *filename, /**< filename of climate input file */
                char **title,         /**< title or NULL */
                Bool isout            /**< output to stderr (TRUE/FALSE) */
               )                      /** \return FALSE on matching titles */
{
  char *climate;
  Bool rc=FALSE;
  if(attrs!=NULL)
  {
    climate=getattr(attrs,n_attr,"title");
    if(climate!=NULL)
    {
      if(*title==NULL)
        *title=climate;
      else
      {
        if(strcmp(climate,*title))
        {
          if(isout)
            fprintf(stderr,"WARNING043: Title %s in file '%s' differs from %s.\n",
                    climate,filename,*title);
          rc=TRUE;
        }
        free(climate);
      }
    }
  }
  return rc;
} /* of 'checktitle' */
