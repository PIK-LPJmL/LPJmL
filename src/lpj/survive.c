/**************************************************************************************/
/**                                                                                \n**/
/**                         s  u  r  v  i  v  e  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines wheher PFT is beyond its climatological                \n**/
/**     limits. Killed biomass is tranferred to litter                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool survive(const Pftpar *pftpar, /**< pointer to PFT parameter */
             const Climbuf *climbuf /**< climate buffer */
            )                       /** \return TRUE if PFT survives */
{
  Real temp_min20,temp_max20;
  
  temp_min20=getbufferavg(climbuf->min);
  temp_max20=getbufferavg(climbuf->max);
  return (temp_min20>=pftpar->temp.low) || 
         (temp_max20-temp_min20>=pftpar->min_temprange);

} /* of 'survive' */
