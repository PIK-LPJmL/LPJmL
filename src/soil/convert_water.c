/**************************************************************************************/
/**                                                                                \n**/
/**                 c  o  n  v  e  r  t  _  w  a  t  e  r  .  c                    \n**/
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

void convert_water(Soil* soil, /**< pointer to soil data */
                   int l,      /**< soil layer */
                   Real* heat  /**< heat (J/m2) */
                  )            /** \return void */
{
  Real heatcap,
       freeze_heat,
       melt_heat;
#ifdef CHECK_BALANCE
  Real water_after, water_before,balancew;
  water_before=soilwater(soil);
#endif
  switch (getstate(soil->temp+l))
  {
    case AT_T_ZERO:  /** most probable, continue below */
      break;
    case BELOW_T_ZERO: 
      if(allwater(soil,l)>epsilon)  /* e.g. rain on frozen ground */
      {
        /**
        * energy from freezing water heats soil, no change in heat flux
        **/
        heatcap=soilheatcap(soil,l);  /* [J/m2/K] */
        /**
        * The heat (in J/m2) that will be released if soil moisture is converted to ice
        **/
        freeze_heat = allwater(soil,l)*1e-3*c_water2ice; /* [m]*[J/m3]=[J/m2] */
        /**
        * freeze all available water or freeze water until T_zero is reached
        **/
        freeze_heat = min((T_zero-soil->temp[l])*heatcap,freeze_heat);
        if(freeze_heat>epsilon)
        {
          soil->temp[l] += freeze_heat/heatcap;
          freeze_heat *= (-1);
          moisture2soilice(soil,&freeze_heat,l);  
          soil->state[l]=(short)getstate(soil->temp+l);
        }        
      }
      else
      {
        if ((soil->w[l])<0)
        {
          fprintf(stderr,"w[%d]=%g < 0 in convert_water()\n",l,soil->w[l]);
          soil->w[l]=0.0;
        }
        if (soil->w[l]<0)
          fail(PERM_ERR,TRUE,FALSE,"soil->w[%d]=%.10f<0 in convert_water()",l,
               soil->w[l]);
      }
      break;
    case ABOVE_T_ZERO:
      if(allice(soil,l)>epsilon)
      {
        /**
        * energy used for melting cools soil directly, no change in heat flux
        **/
        heatcap=soilheatcap(soil,l);/* [J/m2/K] */
        melt_heat=(allice(soil,l)*1e-3)*c_water2ice;/* [J/m2] */
        /**
        * thaw all available water or until T_zero is reached
        **/
        melt_heat=min((soil->temp[l]-T_zero)*heatcap,melt_heat);
        if(melt_heat>epsilon)
        {
          soil->temp[l] -= melt_heat/heatcap;
          soilice2moisture(soil,&melt_heat,l); 
          soil->state[l]=(short)getstate(soil->temp+l);
        }
      }     
      else if (soil->ice_depth[l]<0)
        fail(PERM_ERR,TRUE,FALSE,"ice_depth[%d]=%g<0 in convert_water()",l,soil->ice_depth[l]);
      break;
    default:
      /* do nothing */
      break;
  } /* of switch */
  /**
  * now use the energy from heat for any additional water/ice conversions
  **/
  if(soil->state[l]==AT_T_ZERO)
  {
    if(*heat>epsilon && (allice(soil,l)>epsilon))   /** melting */
      soilice2moisture(soil,heat,l);
    else if(*heat<-epsilon && (allwater(soil,l)>epsilon))   /** freezing */
      moisture2soilice(soil,heat,l);
  }
  if (soil->ice_depth[l]<0)
  {
    //fprintf(stderr,"ice_depth[%d]=%g < 0 in convert_water()\n",l,soil->ice_depth[l]);
    if (soil->ice_depth[l]<-epsilon)
      soil->ice_depth[l]=0;
    else
      fail(PERM_ERR,TRUE,FALSE,"ice_depth[%d]=%g<0 in convert_water()",l,soil->ice_depth[l]);
  }
#ifdef CHECK_BALANCE
  water_after=soilwater(soil);
   balancew=water_after-water_before;
   if(fabs(balancew)>epsilon)
     fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid water balance in %s:  balanceW: %g water_before: %g water_after: %g \n",
       __FUNCTION__,balancew,water_before,water_after);
#endif

} /* of 'convert_water' */
