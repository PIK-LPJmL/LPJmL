/**************************************************************************************/
/**                                                                                \n**/
/**              f  s  c  a  n  n  u  p  t  a  k  e  p  a  r  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads N uptake parameter from configuration file                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fscannuptakepar(LPJfile *file,       /**< pointer to LPJ file */
                     Nuptake_param *nuppar, /**< on return N uptake params read */
                     const char *key,     /**< name of N uptake parameter */
                     Verbosity verb       /**< verbosity level (NO_ERR,ERR,VERB) */
                    )                     /** \return TRUE on error */
{
  LPJfile *f;
  f=fscanstruct(file,key,verb);
  if(f==NULL)
    return TRUE;
  if(fscanreal(f,&nuppar->vmax,"vmax",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&nuppar->kmin,"kmin",FALSE,verb))
    return TRUE;
  if(fscanreal(f,&nuppar->Km,"Km",FALSE,verb))
    return TRUE;
  return FALSE;
} /* of 'fscannuptakepar' */
