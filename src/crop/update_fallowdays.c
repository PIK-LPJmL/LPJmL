/**************************************************************************************/
/**                                                                                \n**/
/**       u  p  d  a  t  e  _  f  a  l  l  o  w  d  a  y  s  .  c                  \n**/
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

void update_fallowdays(Cropdates *cropdates, /**< crop dates */
                       Real lat,             /**< Latitude (deg) */
                       int day,              /**< day (1..365) */
                       int ncft              /**< number of crop PFTs */
                      )
{
  int cft;
  
  for(cft=0;cft<ncft;cft++)
  {

    cropdates[cft].fallow--;
    cropdates[cft].fallow_irrig--;
  
    if((lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) ||
       (lat<0.0 && day==COLDEST_DAY_SHEMISPHERE)) 
      cropdates[cft].fallow=cropdates[cft].fallow_irrig=0;
  }
} /* of 'update_fallowdays' */

/*local function update_fallowdays() is called in sowing()*/
