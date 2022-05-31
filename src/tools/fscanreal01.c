/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  r  e  a  l  0  1  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a real value in [0,1] from a text file                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "types.h"

Bool fscanreal01(LPJfile *file,      /**< pointer to a LPJ file             */
                 Real *value,        /**< real value read from file         */
                 const char *name,   /**< name of variable                  */
                 Bool with_default,  /**< allow default value               */
                 Verbosity verbosity /**< verbosity level (NO_ERR,ERR,VERB) */
                )                    /** \return TRUE on error              */
{
  if(fscanreal(file,value,name,with_default,verbosity))
    return TRUE;
  if(*value<0 || *value>1)
  {
    if(verbosity)
      fprintf(stderr,"ERROR234: Parameter '%s'=%g must be in interval [0,1].\n",
              name,*value);
    return TRUE;
  }
  return FALSE;
} /* of 'fscanreal01' */

