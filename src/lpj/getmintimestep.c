/**************************************************************************************/
/**                                                                                \n**/
/**                g  e  t  m  i  n  t  i  m  e  s  t  e  p  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function returns minimum time step allowed for output                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int getmintimestep(int index /**< Index of output file */
                  )          /** \return minimum time step (DAILY, MONTHLY, ANNUAL) */
{
  switch(index)
  {
    case VEGC: case VEGN: case GLOBALFLUX: case CFTFRAC: case CFTFRAC2: case SDATE: case SDATE2:
      return ANNUAL;
    default:
      return DAILY;
  }
} /* of 'getmintimestep' */
