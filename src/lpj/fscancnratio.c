/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  c  n  r  a  t  i  o  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads C:N ratio from configuration file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscancnratio(LPJfile *file,    /**< pointer to LPJ file */
                  Cnratio *ratio,   /**< on return C:N ratios read */
                  const char *name, /**< name of limit variable */
                  Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
                 )                  /** \return TRUE on error */
{
  LPJfile *f;
  f=fscanstruct(file,name,verb);
  if(f==NULL)
    return TRUE;
  if(fscanreal(f,&ratio->low,"low",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&ratio->median,"median",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&ratio->high,"high",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscancnratio' */
