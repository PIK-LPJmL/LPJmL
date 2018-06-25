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
#define c_fstem      0.70 /* Masking of the ground by stems and branches without leaves. Source: FOAM/LPJ */

Real gp_sum(const Pftlist *pftlist, /**< Pft list */
            Real co2,              /**< atmospheric CO2 concentration (ppm) */
            Real temp,             /**< temperature (deg C) */
            Real par,              /**< photosynthetic active radiation flux */
            Real daylength,        /**< daylength (h) */
            Real *gp_stand_leafon, /**< pot. canopy conduct.at full leaf cover */
            Real gp_pft[],         /**< pot. canopy conductance for PFTs & CFTs*/
            Real *fpc              /**< total FPC of all PFTs */
           )
{
  int p;
  Pft *pft;
  Real agd,adtmm,gp,gp_stand,rd,fpc_total;
  *gp_stand_leafon=gp=*fpc=gp_stand=0;
  if(daylength<1e-20)
  {
    foreachpft(pft,p,pftlist)
      gp_pft[getpftpar(pft,id)]=0;
    return 0;
  }
  foreachpft(pft,p,pftlist)
  {
    if(pft->par->type==CROP){
      adtmm=photosynthesis(&agd,&rd,pft->par->path,LAMBDA_OPT,
                         temp_stress(pft->par,temp,daylength),ppm2Pa(co2),
                         temp,
                         par*(1-getpftpar(pft, albedo_leaf))*fpar_crop(pft)*alphaa(pft),
                         daylength);
      gp=(1.6*adtmm/(ppm2bar(co2)*(1.0-LAMBDA_OPT)*hour2sec(daylength)))+
                    pft->par->gmin*fpar_crop(pft);
      gp_stand+=gp;
      gp_pft[getpftpar(pft,id)]=gp;
    }else{
      adtmm=photosynthesis(&agd,&rd,pft->par->path,LAMBDA_OPT,
                         temp_stress(pft->par,temp,daylength),ppm2Pa(co2),
                         temp,
                         par*pft->fpc*alphaa(pft)*(1-getpftpar(pft, albedo_leaf)),
                         daylength);
      gp=(1.6*adtmm/(ppm2bar(co2)*(1.0-LAMBDA_OPT)*hour2sec(daylength)))+
                      pft->par->gmin*pft->fpc;
      gp_pft[getpftpar(pft,id)]=gp*pft->phen;
      gp_stand+=gp*pft->phen;
    }
    *fpc+=pft->fpc;
    *gp_stand_leafon+=gp;                                                       // TODO /pft->phen included
  }
  fpc_total=*fpc;
  *gp_stand_leafon= (gp_stand<1e-20 || fpc_total<1e-20) ? 0 : *gp_stand_leafon/fpc_total;
  return (gp_stand<1e-20 || fpc_total<1e-20) ? 0 : gp_stand/fpc_total;
} /* of 'gp_sum' */
