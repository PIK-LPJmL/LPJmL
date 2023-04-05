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

Real growing_season_index(Real gsi_old,const Dailyclimate *climate,Bool relative_humidity,Real daylength)
{
  Real itmin,iphoto,ivpd,gsi;
  itmin=max(0,min(1.,1./7*climate->tmin+2./7));
  iphoto=max(0,min(1.,daylength-10));
  ivpd=max(0,min(1,getvpd(climate,relative_humidity)/3200-9./32.));
  gsi=itmin*iphoto*ivpd;
  return gsi_old*20/21.0+gsi/21.0;
} /* of 'growing_season_index' */
