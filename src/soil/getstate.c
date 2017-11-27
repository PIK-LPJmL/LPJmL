/**************************************************************************************/
/**                                                                                \n**/
/**                 g  e  t  s  t  a  t  e  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Soilstate getstate(Real *temp /**< temperature (deg C) */
                  )           /** \return soil state (BELOW_T_ZERO, AT_T_ZERO, ABOVE_T_ZERO) */
{
  if (*temp-T_zero>epsilon)
    return ABOVE_T_ZERO;
  else if (*temp-T_zero<-epsilon)
    return BELOW_T_ZERO;
  else
  {
    *temp=T_zero;
    return AT_T_ZERO;
  }
} /* of 'getstate' */
