/**************************************************************************************/
/**                                                                                \n**/
/**           m o i s t u r e 2 s o i l i c e . c                                  \n**/
/**                                                                                \n**/
/**     freezing of soil moisture.                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/*assuming negative heat*/

void moisture2soilice(Soil *soil, /**< pointer to soil data */
                      Real *heat, /**< heat (J/m2) */
                      int l       /**< soil layer */
                     )
{
  Real freeze_heat,freeze_heat_w, freeze_heat_pwp, freeze_heat_fw; /* The energy (J/m2) that will be added to heat during freezing */
  Real ice_water_old, soil_water_old,frac_freeze;
  Real adj_water;
#ifdef CHECK_BALANCE
  Real water_after, water_before,balancew;
  water_before=soilwater(soil);
#endif
  ice_water_old=allice(soil,l);
  soil_water_old=allwater(soil,l);
  /*  The heat (in J/m2) that will be released if  soil moisture is converted to ice */
  freeze_heat_w = soil->w[l]*soil->whcs[l]*1e-3*c_water2ice; /* [m]*[J/m3]=[J/m2] */
  freeze_heat_pwp = soil->wpwps[l]*(1-soil->ice_pwp[l])*1e-3*c_water2ice;
  freeze_heat_fw = soil->w_fw[l]*1e-3*c_water2ice;
  if(freeze_heat_w+freeze_heat_pwp+freeze_heat_fw > -(*heat))
  {
    freeze_heat=-(*heat);
    /* freezing: first w and pwp, then w_fw*/
    /* fraction freezing of w and pwp*/
    if((freeze_heat_w+freeze_heat_pwp)>0)
      frac_freeze=min(1,freeze_heat/(freeze_heat_w+freeze_heat_pwp));
    else
      frac_freeze=1;
    freeze_heat_w = frac_freeze*freeze_heat_w;
    freeze_heat_pwp = frac_freeze*freeze_heat_pwp;
    /*freeze_heat_w = min(freeze_heat, freeze_heat_w);
    freeze_heat_pwp = freeze_heat - freeze_heat_w;*/
    if(fabs(frac_freeze-1)<epsilon)
        freeze_heat_fw=freeze_heat-freeze_heat_w-freeze_heat_pwp;
    else
         freeze_heat_fw=0;  
  }
  else
    freeze_heat=freeze_heat_w+freeze_heat_pwp+freeze_heat_fw;
  *heat+=freeze_heat;
  if(fabs(*heat)<epsilon)
    *heat=0.0;
  /* amount of ice formed in current timestep (mm) */
  soil->ice_depth[l]+=freeze_heat_w/c_water2ice*1000; /*[mm]*/
  soil->w[l]-=freeze_heat_w/c_water2ice*1000/soil->whcs[l];
  if(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]>soil->whcs[l]+epsilon)
  {
    adj_water=(soil->w[l]*soil->whcs[l]+soil->ice_depth[l])-soil->whcs[l];
    soil->w_fw[l]+=adj_water;
    if(soil->ice_depth[l]>adj_water)
      soil->ice_depth[l]-=adj_water;
    else
    {
      adj_water-=soil->ice_depth[l];
      soil->ice_depth[l]=0;
      soil->w[l]-=adj_water/soil->whcs[l];
    }
  }
  if ((soil->w[l]*soil->whcs[l])<epsilon)
    soil->w[l]=0;
  /* conversion of water below permanent wilting point*/
  soil->ice_pwp[l]+=freeze_heat_pwp/c_water2ice*1000/soil->wpwps[l];
  if(soil->ice_pwp[l]>1.0) /* checking imprecision issues */
    soil->ice_pwp[l]=1.0;
  /* conversion of free water*/
  soil->ice_fw[l]+=freeze_heat_fw/c_water2ice*1000; /*[mm]*/
  soil->w_fw[l]-=freeze_heat_fw/c_water2ice*1000;
  if ((soil->w_fw[l])<epsilon*1e-3)
    soil->w_fw[l]=0;
  if(soil_water_old>epsilon)
  {
    soil->freeze_depth[l]+=(soildepth[l]-soil->freeze_depth[l])*
                           (allice(soil,l)-ice_water_old)/
                           soil_water_old;
    if(soil->freeze_depth[l]>soildepth[l]) /* checking imprecision issues */
      soil->freeze_depth[l]=soildepth[l];
  }
#ifdef SAFE
  if(soil->w[l]<-epsilon|| soil->w_fw[l]<-epsilon)
    fprintf(stderr,"soil.w[%d]=%.10f<0 soil.w_fw[%d]=%.10f in moisture2soilice()",l,soil->w[l],l,soil->w_fw[l]);
  if(soil->ice_depth[l]<-epsilon || soil->ice_depth[l]>soil->whcs[l]+epsilon){
    fprintf(stderr,"soil->ice_depth[%d]=%.10f soil->w[%d]=%.10f soil>whcs[%d] %.10f freeze_heat_w: %g heat: %g soilstate: %d moisture2soilice()\n",l,soil->ice_depth[l],l,soil->w[l],l,soil->whcs[l],freeze_heat_w,*heat,soil->state[l]);
  }
#endif
#ifdef CHECK_BALANCE
  water_after=soilwater(soil);
   balancew=water_after-water_before;
   if(fabs(balancew)>epsilon)
     fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid water balance in %s:  balanceW: %g water_before: %g water_after: %g \n",
       __FUNCTION__,balancew,water_before,water_after);
#endif

} /* of 'moisture2soilice' */
