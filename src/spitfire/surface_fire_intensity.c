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

Real surface_fire_intensity(Real fuel_consump,
                            Real fire_frac,
                            Real ros_forward)
{
  Real surface_intens;
  if (fire_frac <= param.fire_intens)
    surface_intens = 0;
  else
  {
    fuel_consump*=1e-3;
    ros_forward/=60.0;
    surface_intens=HEAT_CONTENT_FUEL*(fuel_consump/fire_frac)*ros_forward;
  }
  return surface_intens;   
} /* of 'surface_fire_intensity' */
