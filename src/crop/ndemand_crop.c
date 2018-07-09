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
#include "agriculture.h"

Real ndemand_crop(const Pft *pft,     /**< pointer to PFT */
                  Real *ndemand_leaf, /**< N demand of leaf (gN/m2) */
                  Real vmax,          /**< vmax (gC/m2/day) */
                  Real daylength,     /**< day length (h) */
                  Real temp,          /**< temperature (deg C) */
                  int UNUSED(npft),   /**< number of natural PFTs */
                  int nbiomass,       /**< number of biomass types */
                  int ncft            /**< number of crop PFTs */
                 )                    /** \return total N demand  (gN/m2) */
{
  Real nc_ratio;
  const Pftcrop *crop;
  const Pftcroppar *croppar;
  Real ndemand_tot;
  Irrigation *data;
  data=pft->stand->data;
  crop=pft->data;
  croppar=pft->par->data;
  *ndemand_leaf=((daylength==0) ? 0 : param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_crop(pft)))+param.n0*0.001*crop->ind.leaf.carbon;
  if(crop->ind.leaf.carbon>0)
    nc_ratio=*ndemand_leaf/crop->ind.leaf.carbon;
  else
    nc_ratio=0;
  if(nc_ratio>pft->par->ncleaf.high)
    nc_ratio=pft->par->ncleaf.high;
  else if(nc_ratio<pft->par->ncleaf.low)
    nc_ratio=pft->par->ncleaf.low;
  ndemand_tot=*ndemand_leaf+nc_ratio*(crop->ind.root.carbon/croppar->ratio.root+crop->ind.pool.carbon/croppar->ratio.pool+crop->ind.so.carbon/croppar->ratio.so);
  if(ndemand_tot>pft->bm_inc.nitrogen)
    pft->stand->cell->output.pft_ndemand[(pft->par->id-nbiomass)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=ndemand_tot-pft->bm_inc.nitrogen;
  /* as long as bm_inc is equal to total biomass, this is fine */
  pft->stand->cell->balance.n_demand+=(ndemand_tot-pft->bm_inc.nitrogen)*pft->stand->frac;
  return ndemand_tot;
} /* of 'ndemand_crop' */
