/**************************************************************************************/
/**                                                                                \n**/
/**               f i r e d a n g e r i n d e x . c                                \n**/
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

//#define alpha_fuelp 0.000337

Real firedangerindex(Real char_moist_factor,
                     Real char_alpha_fuel,
                     Real nesterov_accum,
                     const Pftlist *pftlist
                    )
{
  Real d_fdi,alpha_fuelp_ave;
  const Pft *pft;
  int p,n;
  alpha_fuelp_ave=0;
  n=getnpft(pftlist);
  if(n>0)
  {
    foreachpft(pft,p,pftlist)
      alpha_fuelp_ave+=pft->par->alpha_fuelp;
  
    alpha_fuelp_ave/=n; 
  } 
#ifdef SAFE
  if(char_alpha_fuel < 0)
    fprintf(stderr,"char_alpha_fuel %f in firedangerindex\n",char_alpha_fuel);
#endif
  /* Calculate Fire Danger Index */
  if(nesterov_accum <= 0 || char_moist_factor <=0)
    d_fdi = 0;
  else
    d_fdi = (0.0 > (1.0-(1.0 / char_moist_factor * (exp(-alpha_fuelp_ave * nesterov_accum)))) ?
             0 : (1.0-(1.0 / char_moist_factor * (exp(-alpha_fuelp_ave * nesterov_accum)))));

  //d_fdi = (0.0 > (1.0-(1.0 / char_moist_factor * (exp(-alpha_fuelp * nesterov_accum)))) ?
    //        0 : (1.0-(1.0 / char_moist_factor * (exp(-alpha_fuelp * nesterov_accum)))));
  //printf("char_moist_factor:%g, firedangeindex:%g \n",char_moist_factor,d_fdi);

  return d_fdi;
} /* of 'firedangerindex' */
