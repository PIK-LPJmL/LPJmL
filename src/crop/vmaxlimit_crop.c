/**************************************************************************************/
/**                                                                                \n**/
/**               v  m  a  x  l  i  m  i  t  _  c  r  o  p  .  c                   \n**/
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
#include "crop.h"

Real vmaxlimit_crop(const Pft *pft, /**< pointer to PFT */
                    Real daylength, /**< day length (h) */
                    Real temp       /**< temperature (deg C) */
                   )                /** \return vmax (gC/m2/day) */
{
  const Pftcrop *crop;
  Real vmax;
  crop=pft->data;
#ifdef DEBUG_N
  printf("LAI=%g, N0=%g\n",lai_crop(pft),param.n0*0.001*crop->ind.leaf.carbon);
#endif
  //vmax=((crop->ind.leaf.nitrogen-param.n0*0.001*crop->ind.leaf.carbon/CCpDM)*pft->nind)/exp(-param.k_temp*(temp-25))/f_lai(lai_crop(pft))/param.p/9.6450617e-4;
  vmax=(crop->ind.leaf.nitrogen-pft->par->ncleaf.low*crop->ind.leaf.carbon*pft->nind)/exp(66530/8.314*(1/degCtoK(temp)-1/degCtoK(25)))/0.010*(NSECONDSDAY*cmass*1e-6);
  return max(vmax,epsilon);
} /* of 'vmaxlimit_crop' */
