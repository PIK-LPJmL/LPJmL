/*************************************************************************************/
/**                                                                                \n**/
/**                     g  p  _  s  u  m  .  c                                     \n**/
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

Real gp_sum(const Pftlist *pftlist, /**< [in] Pft list */
            Real co2,              /**< [in] atmospheric CO2 concentration (ppm) */
            Real temp,             /**< [in] temperature (deg C) */
            Real par,              /**< [in] photosynthetic active radiation flux (J/m2/day) */
            Real daylength,        /**< [in] daylength (h) */
            Real *gp_stand_leafon, /**< [out] pot. canopy conduct.at full leaf cover (mm/s) */
            Real gp_pft[],         /**< [out] pot. canopy conductance for PFTs & CFTs (mm/s) */
            Real *fpc_total,       /**< [out] total FPC of all PFTs */
            const Config *config   /**< [in] LPJ configuration */
           )                       /** \return pot. canopy conductance (mm/s) */
{
  int p;
  Pft *pft;
  Real agd,adtmm,gp,gp_stand,rd;
  *gp_stand_leafon=gp=*fpc_total=gp_stand=0;
  if(daylength<1e-20)
  {
    foreachpft(pft,p,pftlist)
      gp_pft[getpftpar(pft,id)]=0;
    return 0;
  }
  foreachpft(pft,p,pftlist)
  {
    if(pft->par->type==CROP)
    {
      adtmm=photosynthesis(&agd,&rd,&pft->vmax,pft->par->path,LAMBDA_OPT,
                           temp_stress(pft->par,temp,daylength),pft->par->b,ppm2Pa(co2),
                           temp,
                           par*(1-getpftpar(pft,albedo_leaf))*fpar_crop(pft)*alphaa(pft,config->with_nitrogen,config->laimax_manage),
                           daylength,TRUE);
      gp=(1.6*adtmm/(ppm2bar(co2)*(1.0-LAMBDA_OPT)*hour2sec(daylength)))+
                    pft->par->gmin*fpar_crop(pft);
      gp_stand+=gp;
      gp_pft[getpftpar(pft,id)]=gp;
    }
    else
    {
      adtmm=photosynthesis(&agd,&rd,&pft->vmax,pft->par->path,LAMBDA_OPT,
                           temp_stress(pft->par,temp,daylength),pft->par->b,ppm2Pa(co2),
                           temp,
                           par*pft->fpc*alphaa(pft,config->with_nitrogen,config->laimax_manage)*(1-getpftpar(pft,albedo_leaf))*(1-pft->snowcover),
                           daylength,TRUE);
      gp=(1.6*adtmm/(ppm2bar(co2)*(1.0-LAMBDA_OPT)*hour2sec(daylength)))+
                      pft->par->gmin*pft->fpc*(1-pft->snowcover);
      gp_pft[getpftpar(pft,id)]=gp*pft->phen;
      gp_stand+=gp*pft->phen;
    }
    *fpc_total+=pft->fpc;
    *gp_stand_leafon+=gp;             // TODO /pft->phen included
  }
  *gp_stand_leafon= (gp_stand<1e-20 || *fpc_total<1e-20) ? 0 : *gp_stand_leafon/ *fpc_total;
  return (gp_stand<1e-20 || *fpc_total<1e-20) ? 0 : gp_stand/ *fpc_total;
} /* of 'gp_sum' */
