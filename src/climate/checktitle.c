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
                const char *filename, /**< filename of  input file */
                char **title,         /**< title or NULL */
                Bool isout            /**< output to stderr (TRUE/FALSE) */
               )                      /** \return FALSE on matching titles */
{
  char *mytitle;
  Bool rc=FALSE;
  if(attrs!=NULL)
  {
    mytitle=getattr(attrs,n_attr,"title");
    if(mytitle!=NULL)
    {
      if(*title==NULL)
        *title=mytitle;
      else
      {
        if(strcmp(mytitle,*title))
        {
          if(isout)
            fprintf(stderr,"WARNING043: Title '%s' in file '%s' differs from '%s'.\n",
                    mytitle,filename,*title);
          rc=TRUE;
        }
        free(mytitle);
      }
    }
  }
  return rc;
} /* of 'checktitle' */
