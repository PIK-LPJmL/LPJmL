/**************************************************************************************/
/**                                                                                \n**/
/*          s u r f a c e _ f i r e _ i n t e n s i t y . c                        \n**/
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

#define HEAT_CONTENT_FUEL 18000.0

Real surface_fire_intensity(Real ros_forward,
                            Fuel *fuel)
{
  Real surface_intens;
  ros_forward/=60.0;
  surface_intens=(fuel->char_sigma > 0) ? (384/ft2cm(1))/fuel->char_sigma*fuel->ignition_rate*ros_forward : 0.0; /* calculated using Albini 1976 approach */
  return surface_intens;   
} /* of 'surface_fire_intensity' */
