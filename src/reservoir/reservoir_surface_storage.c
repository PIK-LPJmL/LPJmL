/**************************************************************************************/
/**                                                                                \n**/
/**          r e s e r v o i r _ s u r f a c e _ s t o r a g e . c                 \n**/
/**                                                                                \n**/
/**     Function reads reservoir data from binary file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real reservoir_surface_storage(const Resdata *resdata /**< reservoir data */
                              )         /** \return surface storage (dm3) */
{
  Real surface_storage;
  int i;
  surface_storage=resdata->dmass;
  for(i=0;i<NIRRIGDAYS;i++)
    surface_storage+=resdata->dfout_irrigation_daily[i];
  return surface_storage;
} /* of 'reservoir_surface_storage' */
