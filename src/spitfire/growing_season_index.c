/**************************************************************************************/
/**                                                                                \n**/
/**               g r o w i n g _ s e a s o n _ i n d e x . c                      \n**/
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

Real growing_season_index(Real gsi_old,                /**< old cumulative GSI (0..1) */
                          Real *gsi,                   /**< actual GSI (0..1) */
                          const Dailyclimate *climate, /**< daily climate data */
                          Bool relative_humidity,      /**< humidity is relative humidity (TRUE/FALSE) */
                          Real daylength               /**< day length (h) */
                         )                             /** \return updated cumulative GSI */
{
  Real itmin,iphoto,ivpd;
  itmin=max(0,min(1.0,1./7.0*climate->tmin+2.0/7.0));
  iphoto=max(0,min(1.0,daylength-10));
  ivpd=max(0,min(1,1-getvpd(climate,relative_humidity)/3200+9.0/32.0));
  *gsi=itmin*iphoto*ivpd;
#ifdef DEBUG
  printf("gsi_old=%g,itmin=%g,iphoto=%g,ivpd=%g,vpd=%g\n",gsi_old,itmin,iphoto,ivpd,getvpd(climate,relative_humidity));
#endif
  return gsi_old*20.0/21.0+*gsi/21.0;
} /* of 'growing_season_index' */
