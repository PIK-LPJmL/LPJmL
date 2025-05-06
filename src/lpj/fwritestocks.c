/**************************************************************************************/
/**                                                                                \n**/
/**                 f  w  r  i  t  e  s  t  o  c  k  s  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes stock data to restart file                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritestocks(Bstruct file,        /**< pointer to restart file */
                  const char *name,    /**< name of object or NULL */
                  const Stocks *stocks /**< stocks written to file */
                 )                     /** \return TRUE on error */
{
  bstruct_writestruct(file,name);
  bstruct_writereal(file,"carbon",stocks->carbon);
  bstruct_writereal(file,"nitrogen",stocks->nitrogen);
  return bstruct_writeendstruct(file);
} /* of 'fwritestocks' */

Bool fwritestocksarray(Bstruct file,       /**< pointer to restart file */
                       const char *name,   /**< name of object or NULL */
                       const Stocks vec[], /**< array written to file */
                       int size            /**< size of arary */
                      )                    /** \return TRUE on error */
{
  int i;
  bstruct_writearray(file,name,size);
  for(i=0;i<size;i++)
    fwritestocks(file,NULL,vec+i);
  return bstruct_writeendarray(file);
} /* of 'fwritestocksarray' */
