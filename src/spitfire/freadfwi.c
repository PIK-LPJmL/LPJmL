/**************************************************************************************/
/**                                                                                \n**/
/**                  f  r  e  a  d  f  w  i  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads FWI data from restart file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadfwi(Bstruct file,     /**< pointer to restart file */
              const char *name, /**< name of object or NULL */
              FWIdata *data     /**< data read from file */
             )                  /** \return TRUE on error */
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  if(bstruct_readreal(file,"ffmc",&data->ffmc))
    return TRUE;
  if(bstruct_readreal(file,"dmc",&data->dmc))
    return TRUE;
  if(bstruct_readreal(file,"dc",&data->dc))
    return TRUE;
  return bstruct_readendstruct(file,name);
} /* of 'freadfwi' */
