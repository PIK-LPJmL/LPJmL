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
  Real vmax,vcmax25,Nleaf,leafarea;
  crop=pft->data;
#ifdef DEBUG_N
  printf("LAI=%g, N0=%g\n",lai_crop(pft),param.n0*0.001*crop->ind.leaf.carbon);
#endif
  //vmax=((crop->ind.leaf.nitrogen-param.n0*0.001*crop->ind.leaf.carbon/CCpDM)*pft->nind)/exp(-param.k_temp*(temp-25))/f_lai(lai_crop(pft))/param.p/9.6450617e-4;
  leafarea=crop->ind.leaf.carbon*pft->par->sla*pft->nind;
  if(leafarea>0)
  {
    Nleaf=pft->nleaf/leafarea; // from gN/m2 leaf area to gN/m2 ground area
    vcmax25=(Nleaf - 0.437 - 0.026*pft->par->nfixing/(pft->par->sla*CCpDM) + 1.134*pft->par->nfixing - 0.007/(pft->par->sla*CCpDM))/0.010;
    vmax=vcmax25/exp(66530/8.314*(1/degCtoK(temp)-1/degCtoK(25)))*(NSECONDSDAY*cmass*1e-6)*leafarea;
  }
  else
  {
    vmax=0;
  }
  return max(vmax,epsilon);
} /* of 'vmaxlimit_crop' */
