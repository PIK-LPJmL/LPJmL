/**************************************************************************************/
/**                                                                                \n**/
/**                   r  u  n  m  e  a  n  _  a  d  d  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function calculates running mean                                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"   /* Definition of datatype Real  */
#include "bstruct.h"
#include "numeric.h"
#include "errmsg.h"

Real runmean_add(Real runmean_current, /**< current running mean */
                 Real add,             /**< new value to add */
                 Real avtime           /**< time to everage */
                )                      /** \return updated running mean */
{
  Real tmp;

  tmp = runmean_current * ((avtime - 1.) / avtime);
  tmp += add * (1. / avtime);

  return tmp;
} /* of 'runmean_add' */
