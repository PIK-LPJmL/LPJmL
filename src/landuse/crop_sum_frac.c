/**************************************************************************************/
/**                                                                                \n**/
/**            c  r  o  p  _  s  u  m  _  f  r  a  c  .  c                         \n**/
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

Real crop_sum_frac(Landfrac landfrac[2], /**< land fractions */
                   int ncft,             /**< number of crop PFTs */
                   Real waterfrac,       /**< lake fraction (0..1) */
                   Bool irrig            /**< for irrigated crop or not? */
                  )                      /** \return sum of land fractions */
{
   Real factor,sumfrac,sumfracall;

   sumfrac=landfrac_sum(landfrac,ncft,irrig);
   sumfracall=landfrac_sum(landfrac,ncft,TRUE)+landfrac_sum(landfrac,ncft,FALSE);

   /* if the cropfrac > 1- reservoirfrac, reduce the cropfrac to the maximum area */
   if(sumfracall>(1-waterfrac))
   {
     factor=(1-waterfrac)/sumfracall;
     scalelandfrac(landfrac,ncft,factor);
   }
   else
     factor=1;
   
   return sumfrac*factor;

} /* of 'crop_sum_frac' */
