/**************************************************************************************/
/**                                                                                \n**/
/**               f  s  c  a  n  p  h  e  n  p  a  r  a  m  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads phenology parameter from configuration file                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscanphenparam(LPJfile *file,       /**< pointer to LPJ file */
                    Phen_param *phenpar, /**< on return phenology params read */
                    const char *key,     /**< name of phenology parameter */
                    Verbosity verb       /**< verbosity level (NO_ERR,ERR,VERB) */
                   )                     /** \return TRUE on error */
{
  LPJfile f;
  if(fscanstruct(file,&f,key,verb))
    return TRUE;
  if(fscanreal(&f,&phenpar->sl,"slope",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&phenpar->base,"base",FALSE,verb))
    return TRUE;
  if(fscanreal(&f,&phenpar->tau,"tau",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscanphenparam' */
