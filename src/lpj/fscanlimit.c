/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  l  i  m  i  t  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads limit from configuration file                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanlimit(LPJfile *file,  /**< pointer to LPJ file */
                Limit *limit, /**< on return limit read */
                const char *name, /**< name of limit variable */
                Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
               )             /** \return TRUE on error */
{
  LPJfile f;
  if(fscanstruct(file,&f,name,verb))
    return TRUE;
  if(fscanreal(&f,&limit->low,"low",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&limit->high,"high",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscanlimit' */
