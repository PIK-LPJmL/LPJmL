/**************************************************************************************/
/**                                                                                \n**/
/**         u  p  d  a  t  e  _  c  r  o  p  d  a  t  e  s  .  c                   \n**/
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

void update_cropdates(Cropdates cropdates[], /**< array of crop dates */
                      int ncft /**< number of crop dates */
                     )
{
  int cft;
  for(cft=0;cft<ncft;cft++)
    if(cropdates[cft].last_update_fall>=0)
    {
      cropdates[cft].last_update_fall++;
      cropdates[cft].last_update_spring++;
      cropdates[cft].last_update_vern++;

      if(cropdates[cft].last_update_fall>10)
        cropdates[cft].fall_sdate20=0;
      if(cropdates[cft].last_update_spring>10)
        cropdates[cft].spring_sdate20=0;
      if(cropdates[cft].last_update_vern>10)
        cropdates[cft].vern_date20=0;
    }
} /* 'update_cropdates' */
