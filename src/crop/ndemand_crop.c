/**************************************************************************************/
/**                                                                                \n**/
/**               n  d  e  m  a  n  d  _  c  r  o  p  .  c                         \n**/
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

Real ndemand_crop(const Pft *pft,     /**< pointer to PFT */
                  Real *ndemand_leaf, /**< N demand of leaf (gN/m2) */
                  Real vmax,          /**< vmax (gC/m2/day) */
                  Real daylength,     /**< day length (h) */
                  Real temp           /**< temperature (deg C) */
                 )                    /** \return total N demand  (gN/m2) */
{
  Real nc_ratio,vcmax25,leafarea;
  const Pftcrop *crop;
  const Pftcroppar *croppar;
  Real ndemand_tot;
  crop=pft->data;
  croppar=pft->par->data;
  //*ndemand_leaf=((daylength==0) ? 0 : param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_crop(pft)))+param.n0*0.001*crop->ind.leaf.carbon;
  //*ndemand_leaf=((daylength==0) ? 0 : param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_crop(pft)))+pft->par->ncleaf.low*crop->ind.leaf.carbon;
  //*ndemand_leaf=((daylength==0) ? 0 : param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_crop(pft)))+pft->par->ncleaf.median*crop->ind.leaf.carbon*pft->nind;
  //*ndemand_leaf=param.p*9.6450617e-4*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_crop(pft))+param.n0*0.001*crop->ind.leaf.carbon*pft->nind/CCpDM;
  leafarea=crop->ind.leaf.carbon*pft->par->sla*pft->nind;
  if(leafarea>0)
  {
    vcmax25=vmax/(NSECONDSDAY*cmass*1e-6)*exp(66530/8.314*(1/degCtoK(temp)-1/degCtoK(25)))/leafarea; //from gC/m2/day (per m2 ground area at temp) to µmol/m2/s (per m2 leaf area at 25 degC)
    *ndemand_leaf=0.007/(pft->par->sla*CCpDM) + 0.010*vcmax25 - 1.134*pft->par->nfixing + 0.026*pft->par->nfixing/(pft->par->sla*CCpDM) + 0.437;
    *ndemand_leaf*=leafarea; // from gN/m2 leaf area to gN/m2 ground area
  }
  else
  {
    *ndemand_leaf=0;
  }
  if(crop->ind.leaf.carbon>0)
    nc_ratio=*ndemand_leaf/crop->ind.leaf.carbon;
  else
    nc_ratio=0;
  if(nc_ratio>pft->par->ncleaf.high)
    nc_ratio=pft->par->ncleaf.high;
  else if(nc_ratio<pft->par->ncleaf.low)
    nc_ratio=pft->par->ncleaf.low;
  ndemand_tot=*ndemand_leaf+nc_ratio*(crop->ind.root.carbon/croppar->ratio.root+crop->ind.pool.carbon/croppar->ratio.pool+crop->ind.so.carbon/croppar->ratio.so)*pft->nind;
  /* as long as bm_inc is equal to total biomass, this is fine */
  return ndemand_tot;
} /* of 'ndemand_crop' */
