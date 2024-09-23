/**************************************************************************************/
/**                                                                                \n**/
/**              a  l  p  h  a  a  _  g  r  a  s  s  .  c                          \n**/
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
#include "grass.h"

Real alphaa_grass(const Pft *pft,     /**< pointer to grass PFT */
                  int UNUSED(lai_opt) /**< LAImax option */
                 )                    /** \return alpha_a (0..1) */
{
  Pftgrass *grass;
  Real scaler=1.0;
  grass=pft->data;

  if((grass->ind.leaf.carbon+grass->excess_carbon*grass->falloc.leaf)>10)
  {
    scaler=grass->ind.leaf.nitrogen/(grass->ind.leaf.carbon+grass->excess_carbon*grass->falloc.leaf) /
           pft->par->ncleaf.low;
    if(scaler>1)
      scaler=1.0;
    else
      scaler=(1+param.par_sink_limit)*scaler/(scaler+param.par_sink_limit);
  }
  return pft->par->alphaa*scaler;
} /* of 'alphaa_grass' */
