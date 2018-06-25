/**************************************************************************************/
/**                                                                                \n**/
/**           s o i l i c e 2 m o i s t u r e . c                                  \n**/
/**                                                                                \n**/
/**     thawing of soil ice.                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/* assuming positive heat */

void soilice2moisture(Soil *soil, /**< pointer to soil data */
                      Real *heat, /**< heat (J/m2) */
                      int l       /**< soil layer */
                     )
{
  Real melt_heat, melt_heat_ice, melt_heat_pwp, melt_heat_fw; /* The energy avaible to melt ice (J/m2)  */
  Real ice_water_old, soil_water_old,frac_melt;
  
  ice_water_old=allice(soil,l);
  soil_water_old=allwater(soil,l);
  
  melt_heat_ice = soil->ice_depth[l]*1e-3*c_water2ice;
  melt_heat_fw = soil->ice_fw[l]*1e-3*c_water2ice;
  melt_heat_pwp = soil->par->wpwps[l]*soil->ice_pwp[l]*1e-3*c_water2ice;
  if(melt_heat_ice+melt_heat_pwp+melt_heat_fw > (*heat))
  {
    melt_heat=*heat;
    melt_heat_fw=min(melt_heat,melt_heat_fw);
    /* fraction melting of ice and ice_pwp*/
    frac_melt=min(1,(melt_heat-melt_heat_fw)/(melt_heat_ice+melt_heat_pwp));
    melt_heat_ice = frac_melt*melt_heat_ice;
    melt_heat_pwp = frac_melt*melt_heat_pwp;
     
  }
  else
    melt_heat=melt_heat_ice+melt_heat_pwp+melt_heat_fw;
  *heat-=melt_heat;
  soil->ice_depth[l]-=melt_heat_ice/c_water2ice*1000;/*[mm]*/
  soil->w[l]+=(melt_heat_ice/c_water2ice)/soil->par->whcs[l]*1000; /*fraction of whcs*/
  if (fabs(soil->ice_depth[l])<epsilon)
    soil->ice_depth[l]=0;
  /* conversion of water below permanent wilting point*/
  soil->ice_pwp[l]-=melt_heat_pwp/c_water2ice*1000/soil->par->wpwps[l];
  if(fabs(soil->ice_pwp[l])<epsilon)
    soil->ice_pwp[l]=0.0;
  /* conversion of free water */
  soil->ice_fw[l]-=melt_heat_fw/c_water2ice*1000;/*[mm]*/
  soil->w_fw[l]+=melt_heat_fw/c_water2ice*1000; /*mm*/
  if (fabs(soil->ice_fw[l])<epsilon)
    soil->ice_fw[l]=0;
#ifdef SAFE
  if(soil->ice_depth[l]<0 || soil->ice_depth[l]>(soil->par->whcs[l]+epsilon))
    fprintf(stderr,"soil->ice_depth[%d]=%.10f in soilice2moisture().\n",l,soil->ice_depth[l]);
  if(soil->ice_pwp[l]<0)
    fprintf(stderr,"soil->ice_pwp[%d]=%.10f in soilice2moisture().\n",l,soil->ice_pwp[l]);
  if(soil->w[l]<0)
    fprintf(stderr,"w[%d]=%.10f soil->ice_depth[l]=%.10f in soilice2moisture().\n",l,soil->w[l],soil->ice_depth[l]);
#endif
  if(ice_water_old > epsilon)
    soil->freeze_depth[l]-=soil->freeze_depth[l]*(allwater(soil,l)-soil_water_old)/ice_water_old;
  if (fabs(soil->freeze_depth[l])<epsilon)
    soil->freeze_depth[l]=0;
} /* of 'soilice2moisture' */
