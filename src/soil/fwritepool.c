/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  p  o  o  l  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes pool variables into restart file                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritepool(Bstruct file,     /**< pointer to restart file */
                const char *name, /**< name of object */
                const Pool *pool  /**<  pointer to pool to write */
               )                  /** \return TRUE on error */
{
  bstruct_writebeginstruct(file,name);
  fwritestocks(file,"slow",&pool->slow);
  fwritestocks(file,"fast",&pool->fast);
  return bstruct_writeendstruct(file);
} /* of 'fwritepool' */
