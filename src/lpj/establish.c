/**************************************************************************************/
/**                                                                                \n**/
/**              e  s  t  a  b  l  i  s  h  .  c                                   \n**/
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

Bool establish(Real gdd,              /**< growing degree days */
               const Pftpar *pftpar,  /**< PFT parameter */
               const Climbuf *climbuf, /**< climate buffer */
               Bool peatland           /* if stand == PEATLAND */
              )                       /** \return if established */ 
{
  Real temp_min20;
/*
  if(!pftpar->peatland && peatland)
	return FALSE;
*/
  if (pftpar->peatland && !peatland)
    return FALSE;
  if(isemptybuffer(climbuf->min)) /* check to avoid division by zero */
    return FALSE;
  temp_min20=getbufferavg(climbuf->min);
  return (temp_min20>=pftpar->temp.low) && 
         (temp_min20<=pftpar->temp.high) && 
         (gdd>=pftpar->gdd5min);
} /* of 'establish' */
