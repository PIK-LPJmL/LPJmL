/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  f  w  i  .  c                                  \n**/
/**                                                                                \n**/
/**     Function writes Canadian fire danger index into restart file               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritefwi(Bstruct file,       /**< pointer to restart file */
               const char *name,   /**< name of object or NULL */
               const FWIdata *data /**< data written to file */
              )                    /** \return TRUE on error */
{
  bstruct_writebeginstruct(file,name);
  bstruct_writereal(file,"ffmc",data->ffmc);
  bstruct_writereal(file,"dmc",data->dmc);
  bstruct_writereal(file,"dc",data->dc);
  return bstruct_writeendstruct(file);
} /* of 'fwritefwi' */
