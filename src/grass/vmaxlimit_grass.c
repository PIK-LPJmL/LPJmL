/**************************************************************************************/
/**                                                                                \n**/
/**               v  m  a  x  l  i  m  i  t  _  g  r  a  s  s  .  c                \n**/
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

Real vmaxlimit_grass(const Pft *pft, /**< pointer to PFT */
                     Real daylength, /**< day length (h) */
                     Real temp       /**< temperature (deg C) */
                    )                /** \return vmax (gC/m2/day) */
{
  const Pftgrass *grass;
  Real vmax,vcmax25,Nleaf,leafarea;
  grass=pft->data; 
  //vmax=(pft->nleaf-param.n0*0.001*(grass->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.leaf-grass->turn_litt.leaf.carbon)/CCpDM)/exp(-param.k_temp*(temp-25))/f_lai(lai_grass(pft))/param.p/9.6450617e-4;
  leafarea=grass->ind.leaf.carbon*pft->par->sla*pft->nind*pft->phen;
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
  return min(pft->vmax,max(vmax,0.0001));
  } /* of 'vmaxlimit_grass' */
