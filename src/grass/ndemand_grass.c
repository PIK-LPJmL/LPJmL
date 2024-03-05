/**************************************************************************************/
/**                                                                                \n**/
/**               n  d  e  m  a  n  d  _  g  r  a  s  s  .  c                      \n**/
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

Real ndemand_grass(const Pft *pft,    /**< pointer to PFT */
                  Real *ndemand_leaf, /**< N demand of leaf (gN/m2) */
                  Real vmax,          /**< vmax (gC/m2/day) */
                  Real daylength,     /**< day length (h) */
                  Real temp           /**< temperature (deg C) */
                 )                    /** \return total N demand  (gN/m2) */
{
  Real nc_ratio;
  const Pftgrass *grass;
  const Pftgrasspar *grasspar;
  Real ndemand_tot;

  grass=pft->data;
  grasspar=pft->par->data;
  //*ndemand_leaf=((daylength==0) ? 0: param.p*0.02314815*vmax/daylength*exp(-param.k_temp*(temp-25))*f_lai(lai_grass(pft))) +param.n0*0.001*(grass->ind.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf)*pft->nind;
  //*ndemand_leaf=((daylength==0) ? 0: param.p*0.02314815*vmax/daylength*exp(-param.k_temp*(temp-25))*f_lai(lai_grass(pft))) +pft->par->ncleaf.low*(grass->ind.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf)*pft->nind;
  //*ndemand_leaf=((daylength==0) ? 0: param.p*0.02314815*vmax/daylength*exp(-param.k_temp*(temp-25))*f_lai(lai_grass(pft)))+pft->par->ncleaf.low*(grass->ind.leaf.carbon)*pft->nind;
  *ndemand_leaf=((daylength==0) ? 0: param.p*0.02314815*vmax/daylength*exp(-param.k_temp*(temp-25))*f_lai(lai_grass(pft)))+pft->par->ncleaf.median*(grass->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.leaf-grass->turn_litt.leaf.carbon);
  if((grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind)<=epsilon)
    nc_ratio=0;
  else
    nc_ratio=(grass->ind.leaf.nitrogen-grass->turn.leaf.nitrogen+pft->bm_inc.nitrogen*grass->falloc.leaf/pft->nind)/(grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind);
  if(nc_ratio>pft->par->ncleaf.high)
    nc_ratio=pft->par->ncleaf.high;
  else if(nc_ratio<pft->par->ncleaf.low)
    nc_ratio=pft->par->ncleaf.low;
  ndemand_tot=*ndemand_leaf+(grass->ind.root.nitrogen-grass->turn.root.nitrogen)*pft->nind+nc_ratio*(grass->excess_carbon*pft->nind+pft->bm_inc.carbon)*grass->falloc.root/grasspar->ratio;

  /* unclear to me: setaside NPP seems to go to natural grass pft_npp -- should we do the same for Ndemand? */
  return ndemand_tot;
} /* of 'ndemand_grass' */
