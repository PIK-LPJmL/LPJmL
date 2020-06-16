/**************************************************************************************/
/**                                                                                \n**/
/**                    g  e  t  l  a  g  .  c                                      \n**/
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

#define DEPTH 0.25  /*soil depth at which to estimate temperature (m)*/
#define DIFFUS_CONV 0.0864         /*Convert diffusivity from mm2/s to m2/day*/
#define HALF_OMEGA (M_PI/NDAYYEAR) /* corresponds to omega/2 = pi/365*/

void getlag(Soil *soil, /**< pointer to soil data */
            int month   /**< month of year (0..11) */
           )
{
  Real diffus;
  soil->meanw1*=ndaymonth1[month];
  diffus=(soil->meanw1<0.15)  ?
            (soil->par->tdiff_15-soil->par->tdiff_0)/0.15*soil->meanw1+
            soil->par->tdiff_0 :
            (soil->par->tdiff_100-soil->par->tdiff_15)/0.85*(soil->meanw1-0.15)
            +soil->par->tdiff_15;
  soil->meanw1=0;
  soil->alag= DEPTH/sqrt(diffus*DIFFUS_CONV/HALF_OMEGA);
  soil->amp=exp(-soil->alag);
} /* of 'getlag' */
