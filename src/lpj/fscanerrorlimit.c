/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  e  r  r  o  r  l  i  m  i  t  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads error limits from configuration file                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanerrorlimit(LPJfile *file,      /**< pointer to LPJ file */
                     Error_limit *limit, /**< on return error limits read */
                     const char *name,   /**< name of limit variable */
                     Verbosity verb      /**< verbosity level (NO_ERR,ERR,VERB) */
                    )                    /** \return TRUE on error */
{
  LPJfile *f;
  f=fscanstruct(file,name,verb);
  if(f==NULL)
    return TRUE;
  if(fscanreal(f,&limit->stocks.carbon,"carbon",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&limit->stocks.nitrogen,"nitrogen",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&limit->w_local,"water_local",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&limit->w_global,"water_global",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscanerrorlimit' */
