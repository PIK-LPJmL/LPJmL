/**************************************************************************************/
/**                                                                                \n**/
/**            f  r  e  a  d  i  g  n  i  t  i  o  n  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function read ignition data from restart file                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadignition(Bstruct file,      /**< pointer to restart file */
                   const char *name,  /**< name of object or NULL */
                   Ignition *ignition /**< data read from file */
                  )                   /** \return TRUE on error */
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  if(bstruct_readreal(file,"nesterov_accum",&ignition->nesterov_accum))
    return TRUE;
  if(bstruct_readreal(file,"nesterov_max",&ignition->nesterov_max))
    return TRUE;
  if(bstruct_readint(file,"nesterov_day",&ignition->nesterov_day))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'freadignition' */
