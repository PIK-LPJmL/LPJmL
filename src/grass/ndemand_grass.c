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
/** Contact: https://gitlab.pik-potsdam.de/lpjml/lpjml                             \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"
#include "agriculture.h"

Real ndemand_grass(const Pft *pft,    /* pointer to PFT */
                  Real *ndemand_leaf, /* N demand of leaf (gN/m2) */
                  Real vmax,          /* vmax (gC/m2/day) */
                  Real daylength,     /* day length (h) */
                  Real temp,          /* temperature (deg C) */
                  int npft,           /* number of natural PFTs */
                  int nbiomass,       /* number of biomass types */
                  int ncft            /* number of crop PFTs */
                 )                    /* returns total N demand  (gN/m2) */
{
  Real nc_ratio;
  const Pftgrass *grass;
  const Pftgrasspar *grasspar;
  Real ndemand_tot;
  Irrigation *data;
  data=pft->stand->data;
  grass=pft->data;
  grasspar=pft->par->data;
  *ndemand_leaf=((daylength==0) ? 0: param.p*0.02314815*vmax/daylength*exp(-param.k_temp*(temp-25))*f_lai(lai_grass(pft))) +param.n0*0.001*(grass->ind.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf);
 nc_ratio=*ndemand_leaf/(grass->ind.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf);
  if(nc_ratio>pft->par->ncleaf.high)
    nc_ratio=pft->par->ncleaf.high;
  else if(nc_ratio<pft->par->ncleaf.low)
    nc_ratio=pft->par->ncleaf.low;
  ndemand_tot=*ndemand_leaf+grass->ind.root.nitrogen+nc_ratio*
    (grass->excess_carbon*pft->nind+pft->bm_inc.carbon)*grass->falloc.root/grasspar->ratio;

  /* unclear to me: setaside NPP seems to go to natural grass pft_npp -- should we do the same for Ndemand? */
  if(pft->stand->type->landusetype==NATURAL || pft->stand->type->landusetype==SETASIDE_RF || pft->stand->type->landusetype==SETASIDE_IR)
  {
    if(ndemand_tot>vegn_sum_grass(pft)){
      pft->stand->cell->output.pft_ndemand[pft->par->id]+=ndemand_tot-vegn_sum_grass(pft);
      pft->stand->cell->balance.n_demand+=(ndemand_tot-vegn_sum_grass(pft))*pft->fpc*pft->stand->frac;
    }
  }
  else if(pft->stand->type->landusetype==BIOMASS_GRASS)
  {
    if(ndemand_tot>vegn_sum_grass(pft)){
      pft->stand->cell->output.pft_ndemand[(npft-nbiomass)+rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=ndemand_tot-vegn_sum_grass(pft); /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
      pft->stand->cell->balance.n_demand+=(ndemand_tot-vegn_sum_grass(pft))*pft->fpc*pft->stand->frac;
    }
  }
  else
  {
    if(ndemand_tot>vegn_sum_grass(pft))
    {
      pft->stand->cell->output.pft_ndemand[(npft-nbiomass)+rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=ndemand_tot-vegn_sum_grass(pft);/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
      pft->stand->cell->output.pft_ndemand[(npft-nbiomass)+rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=ndemand_tot-vegn_sum_grass(pft);/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
      pft->stand->cell->balance.n_demand+=(ndemand_tot-vegn_sum_grass(pft))*(pft->stand->cell->ml.landfrac[data->irrigation].grass[0]+pft->stand->cell->ml.landfrac[data->irrigation].grass[1]);
    }
  }
  return ndemand_tot;
} /* of 'ndemand_grass' */
