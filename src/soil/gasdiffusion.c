/**************************************************************************************/
/**                                                                                \n**/
/**                 g a s d i f f u s i o n . c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Calculates gas transport through the soil layers                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void gasdiffusion(Soil *soil,     /**< [inout] pointer to soil data */
                  Real airtemp,   /**< [in] air temperature (deg C) */
                  Real pch4,      /**< [in] atmospheric CH4 (ppm) */
                  Real *CH4_out,  /**< [out] CH4 emissions (gC/m2/day) */
                  Real *runoff,   /**< [out] runoff (mm/day) */
                  Real *CH4_sink  /**< [out] CH4 soil sink (gC/m2/day) */
                 )
{
  int l;
  Real D_O2[BOTTOMLAYER], D_CH4[BOTTOMLAYER]; /* oxygen/methane diffusivity [m2/s]*/
  Real delta[BOTTOMLAYER];;
  Real epsilon_CH4[BOTTOMLAYER], epsilon_O2[BOTTOMLAYER];
  Real V;                 /* total oxygen porosity */
  Real soil_moist, O2_air, O2_upper, O2_lower, dO2;
  Real dt, CH4_air, CH4_upper, CH4_lower, dCH4;
  Real tmp_water;
  Real end, start, out,in;
  Bool stop;
  unsigned long int steps, t;
  end=start=tmp_water=out=in=0;
  /*waterbalance needs to be updated*/
  start = soilmethane(soil); //do not multiply by *WC/WCH4, is used for methane fluxes here
  *runoff=*CH4_out=0;
  for (l = 0; l<BOTTOMLAYER; l++)
  {
    if ((soil->w[l] * soil->whcs[l] + soil->w_fw[l] + soil->ice_depth[l] + soil->ice_fw[l])>(soil->wsats[l] - soil->wpwps[l]))
    {
      tmp_water = (soil->w[l] * soil->whcs[l] + soil->w_fw[l] + soil->ice_depth[l] + soil->ice_fw[l]) - (soil->wsats[l] - soil->wpwps[l]);
      if (soil->w_fw[l]>tmp_water)
        soil->w_fw[l] -= tmp_water;
      else
        soil->ice_fw[l] -= tmp_water;
      if (l<BOTTOMLAYER - 1)
        soil->w_fw[l + 1] += tmp_water;
      else
        *runoff += tmp_water;
    }
  }

  /*********************Diffusion of oxygen*************************************/

  O2_air = p_s / R_gas / degCtoK(airtemp)*O2s*WO2;       /*g/m3 oxygen concentration*/
  steps = 0;
  for (l = 0; l<BOTTOMLAYER; l++)
  {
    soil_moist = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_O2[l] = max(0.001, V + soil_moist*soil->wsat[l]*BO2);
    if (V<0)
    {
      V = 0;
    }
    D_O2[l]=(D_O2_air*V + D_O2_water*soil_moist*soil->wsat[l]*BO2)*eta;  // eq. 11 in Khvorostyanov part 1 diffusivity (m2 s-1)
    if (D_O2[l]>0)
    {
      if (l == 0)
        dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / D_O2[l];
      else
        dt = 0.5*(soildepth[l] * soildepth[l-1] * 1e-6) / (0.5*(D_O2[l] + D_O2[l - 1]));
#ifdef SAFE
      if(isnan(dt))
      {
        fail(INVALID_TIMESTEP_ERR,TRUE,TRUE,"Invalid time step in gasdiffusion(), D_O2[%d]=%g",l,D_O2[l]);
      }
#endif
      steps = max(steps, (unsigned long)(timestep2sec(1.0, NSTEP_DAILY) / dt) + 1);
    }
    else
      steps = max(steps, 0);
  }

  for (t = 0; t<steps; ++t)
  {
    for (l = 0; l<BOTTOMLAYER; l++)
     delta[l]=0.;
    stop=TRUE;
    for (l = 0; l<BOTTOMLAYER; l++)
    {
      O2_upper = (l == 0) ? O2_air : soil->O2[l - 1] / soildepth[l - 1] / epsilon_O2[l - 1] * 1000;

      if (D_O2[l]>0)
      {
        dO2 = 0.5*(D_O2[l] + ((l == 0) ? D_O2[l] : D_O2[l - 1]))*timestep2sec(1.0, steps) /((l==0) ? soildepth[l] : (0.5* (soildepth[l]+ soildepth[l-1]))) * 1000
          *(O2_upper - soil->O2[l] / soildepth[l] / epsilon_O2[l] * 1000);
        if(l==0)
          dO2*=0.5;
        if(dO2>0 && l>0)
        {
          //soil->O2[l - 1] -= dO2*(soildepth[l - 1] * epsilon_O2[l - 1]) / 1000;
          delta[l - 1] -= dO2*(soildepth[l - 1] * epsilon_O2[l - 1]) / 1000;
          dO2 = dO2*(soildepth[l - 1] * epsilon_O2[l - 1]) / (soildepth[l] * epsilon_O2[l]);
        }
        else if (l>0)
        {
          //soil->O2[l - 1] -= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
          delta[l - 1] -=dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
        }

        //soil->O2[l] += dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
        delta[l] += dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
        if (soil->O2[l]< 0)
        {
         if(l>0)
           soil->O2[l - 1]-= soil->O2[l];
         soil->O2[l] = 0;
        }

        O2_lower = (l == BOTTOMLAYER - 1) ? 0 : soil->O2[l + 1] / soildepth[l + 1] / epsilon_O2[l + 1] * 1000;
        dO2 = 0.5*(D_O2[l] + ((l == BOTTOMLAYER - 1) ? D_O2[l] : D_O2[l + 1]))*timestep2sec(1.0, steps) / ((l==BOTTOMLAYER-1) ? soildepth[l] : (0.5* (soildepth[l]+ soildepth[l+1]))) * 1000
          *(O2_lower - soil->O2[l] / soildepth[l] / epsilon_O2[l] * 1000)*0.5;
        if (dO2>0 && l != (BOTTOMLAYER - 1))
        {
          //soil->O2[l + 1] -=dO2*(soildepth[l + 1] * epsilon_O2[l + 1]) / 1000;
          delta[l + 1] -=dO2*(soildepth[l + 1] * epsilon_O2[l + 1]) / 1000;
          dO2 = dO2*(soildepth[l + 1] * epsilon_O2[l + 1]) / (soildepth[l] * epsilon_O2[l]);
        }
        else if (l != BOTTOMLAYER - 1)
        {
          //soil->O2[l + 1] -= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
          delta[l + 1] -= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
        }
        if (l != BOTTOMLAYER - 1){
          //soil->O2[l]+= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
          delta[l]+= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
        }
        if (soil->O2[l]< 0) {
          if (l != BOTTOMLAYER - 1)
            soil->O2[l + 1] -= soil->O2[l];
           soil->O2[l] = 0;
        }

        if (fabs(dO2)>1e-18)
          stop=FALSE;
      }
    }
    for (l = 0; l<BOTTOMLAYER; l++)
      soil->O2[l]+=delta[l];
    if (stop || t == MAXHEATSTEPS)
      break;
  } /* of for (t = 0; t<steps; ++t) */

  /*********************Diffusion of methane*************************************/

  CH4_air = p_s / R_gas / degCtoK(airtemp)*pch4*1e-6*WCH4;    /*g/m3 air methane concentration*/
  CH4_upper = CH4_air;
  steps = 0;
  for (l = 0; l<BOTTOMLAYER; l++)
  {
    soil_moist = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_CH4[l] = max(0.001, V + soil_moist*soil->wsat[l]*BCH4);
    if (V<0)
    {
      V = 0;
    }
    D_CH4[l] = (D_CH4_air*V + D_CH4_water*soil_moist*soil->wsat[l]*BCH4)*eta;  // eq. 11 in Khvorostyanov part 1 diffusivity (m2 s-1)
    if (D_CH4[l]>epsilon)
    {
      if (l == 0)
        dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / D_CH4[l];
      else
        dt = 0.5*(soildepth[l] * soildepth[l-1] * 1e-6) / (0.5*(D_CH4[l] + D_CH4[l - 1]));
#ifdef SAFE
      if(isnan(dt))
      {
        fail(INVALID_TIMESTEP_ERR,TRUE,TRUE,"Invalid time step in gasdiffusion(), D_CH4[%d]=%g",l,D_CH4[l]);
      }
#endif
      steps = max(steps, (unsigned long)(timestep2sec(1.0, NSTEP_DAILY) / dt) + 1);
    }
    else
      steps = max(steps, 0);
  }

  for (t = 0; t<steps; ++t)
  {
    stop=TRUE;
    for (l = 0; l<BOTTOMLAYER; l++)
      delta[l]=0.;
   for (l = 0; l<BOTTOMLAYER; l++)
   {
      CH4_upper = (l == 0) ? CH4_air : soil->CH4[l - 1] / soildepth[l - 1] / epsilon_CH4[l - 1] * 1000;

      if (D_CH4[l]>0)
      {
        dCH4 = 0.5*(D_CH4[l] + ((l == 0) ? D_CH4[l] : D_CH4[l - 1]))*timestep2sec(1.0, steps)/((l==0) ? soildepth[l] : (0.5* (soildepth[l]+ soildepth[l-1]))) * 1000
            *(CH4_upper - soil->CH4[l] / soildepth[l] / epsilon_CH4[l] * 1000)*0.5;
        if (dCH4>0 && l>0)
        {
          //soil->CH4[l-1]-= dCH4*(soildepth[l - 1] * epsilon_CH4[l - 1]) / 1000;
          delta[l-1]-= dCH4*(soildepth[l - 1] * epsilon_CH4[l - 1]) / 1000;
          dCH4 = dCH4*(soildepth[l - 1] * epsilon_CH4[l - 1]) / (soildepth[l] * epsilon_CH4[l]);
        }
        else if (l>0)
        {
          //soil->CH4[l-1]-=dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
          delta[l-1]-=dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
        }
        delta[l]+= dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
        //soil->CH4[l]+= dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;

#ifdef CHECK_BALANCE
        if(dCH4<0 && l==0)
          out+=dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
        else if(l==0)
          in+=dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
#endif
        if (soil->CH4[l]< 0)
        {
          if (l>0)
            soil->CH4[l-1]-=soil->CH4[l];
          soil->CH4[l] = 0;
        }

        CH4_lower = (l == BOTTOMLAYER - 1) ? 0 : soil->CH4[l + 1] / soildepth[l + 1] / epsilon_CH4[l + 1] * 1000;

        dCH4 = 0.5*(D_CH4[l] + ((l == BOTTOMLAYER - 1) ? D_CH4[l] : D_CH4[l + 1]))*timestep2sec(1.0, steps)/(0.5* (soildepth[l]+ soildepth[l+1])) * 1000
            *(CH4_lower - soil->CH4[l] / soildepth[l] / epsilon_CH4[l] * 1000)*0.5;
        if (dCH4>0 && l != (BOTTOMLAYER - 1))
        {
          //soil->CH4[l + 1] -= dCH4*(soildepth[l + 1] * epsilon_CH4[l + 1]) / 1000;
          delta[l + 1] -= dCH4*(soildepth[l + 1] * epsilon_CH4[l + 1]) / 1000;
          dCH4 = dCH4*(soildepth[l + 1] * epsilon_CH4[l + 1]) / (soildepth[l] * epsilon_CH4[l]);
        }
        else if (l != BOTTOMLAYER - 1)
        {
          //soil->CH4[l + 1] -= dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
          delta[l + 1] -= dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
        }
        if (l != BOTTOMLAYER - 1)
        {
          //soil->CH4[l] += dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
          delta[l] += dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;
        }

        if (soil->CH4[l]< 0)
        {
          if (l != BOTTOMLAYER - 1) soil->CH4[l+1]-=soil->CH4[l];
          soil->CH4[l]=0;
        }
        if (fabs(dCH4)>1e-18)
          stop=FALSE;
      }
    }
   for (l = 0; l<BOTTOMLAYER; l++)
     soil->CH4[l]+=delta[l];
    if (stop || t == MAXHEATSTEPS)
      break;
  } /* of for (t = 0; t<steps; ++t) */
  end = soilmethane(soil); //do not multiply by *WC/WCH4, is used for methane fluxes here
#ifdef SAFE
  if (soil->w[l]< -epsilon || soil->w_fw[l]< -epsilon )
  {   fprintf(stderr,"\n\ngasdiffusion soilwater=%.6f soilice=%.6f wsats=%.6f agtop_moist=%.6f\n",
          allwater(soil,l),allice(soil,l),soil->wsats[l],soil->litter.agtop_moist);
      fflush(stderr);
      fprintf(stderr,"Soil-moisture layer %d negative: w:%g, fw:%g, soil_type %s \n\n",
          l,soil->w[l],soil->w_fw[l],soil->par->name);
  }
#endif

#ifdef CHECK_BALANCE
  if (fabs(start-end+out+in)>epsilon)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid carbon balance in %s: gasdiff %g start:%g  end:%g gasdiff-in: %g gasdiff-out: %g",
         __FUNCTION__,start-end+out+in, start, end, in,out);
#endif
  if((start-end)<0)
   *CH4_sink=start-end;
  *CH4_out=start-end;
} /* of 'gasdiffusion' */
