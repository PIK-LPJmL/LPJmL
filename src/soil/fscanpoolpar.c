/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  o  o  l  p  a  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads slow and fast pool from configuration file                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanpoolpar(LPJfile *file,    /**< pointer to LPJ file */
                 Poolpar *pool,       /**< on return pool read */
                 const char *name, /**< name of pool */
                 Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
                )                  /** \return TRUE on error */
{
  LPJfile *f;
  f=fscanstruct(file,name,verb);
  if(f==NULL)
    return TRUE;
  if(fscanreal(f,&pool->fast,"fast",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&pool->slow,"slow",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscanpoolpar' */
