/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  w  r  .  c                                       \n**/
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

Real getwr(const Soil *soil,const Real rootdist[])
{
  Real wr;
  int l;
  wr=0;
  for(l=0;l<LASTLAYER;l++)
    wr+=rootdist[l]*(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l]);
  return wr;
} /* of 'getwr' */
