/**************************************************************************************/
/**                                                                                \n**/
/**                 f  r  e  a  d  p  o  o  l  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads pool variables from restart file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadpool(Bstruct file,     /**< pointer to restart file */
               const char *name, /**< name oj object */
               Pool *pool        /**< pointer to pool to read */
              )                  /** \return TRUE on error */
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  if(freadstocks(file,"slow",&pool->slow))
    return TRUE;
  if(freadstocks(file,"fast",&pool->fast))
    return TRUE;
  if(bstruct_readendstruct(file,name))
    return TRUE;
  return FALSE;
} /* of 'freadpool' */
