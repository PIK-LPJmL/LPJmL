/**************************************************************************************/
/**                                                                                \n**/
/**                     o  u  t  p  u  t  i  n  d  e  x  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates index in output storage and checks for valid index     \n**/
/**     into array                                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int outputindex(int index,           /**< index of output file */
                int i,               /**< index in output array */
                const Config *config /**< LPJmL configuration */
               )                     /** \return index in output storage to write data */
{
  if(i<0 || i>=config->outputsize[index])
    fail(INVALID_BOUNDARY_ERR,TRUE,TRUE,"Boundary=%d for output '%s' out of bounds, must be <%d",
         i,config->outnames[index].name,config->outputsize[index]);
  return config->outputmap[index]+i;
} /* of 'outputindex' */
