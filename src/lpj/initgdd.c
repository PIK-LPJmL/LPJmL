/**************************************************************************************/
/**                                                                                \n**/
/**                     i  n  i  t  g  d  d  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes growing degree days array to zero                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void initgdd(Real gdd[],      /**< Growing degree days array */
             int npft         /**< Number of natural PFTs */
            )
{
  int p;
  /* Initialize gdd array to zero */
  for(p=0;p<npft;p++)
    gdd[p]=0;
} /* of 'initgdd' */
