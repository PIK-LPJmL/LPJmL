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

void gasdiffusion(Soil *soil,    /**< [inout] pointer to soil data */
                  Real airtemp,  /**< [in] air temperature (deg C) */
                  Real pch4,     /**< [in] atmospheric CH4 (ppm) */
                  Real *CH4_out, /**< [out] CH4 emissions (gC/m2/day) */
                  Real *runoff   /**< [out] runoff (mm/day) */
                 )
{
  int l;
  Real D_O2[LASTLAYER], D_CH4[LASTLAYER]; /* oxygen/methane diffusivity [m2/s]*/
  Real epsilon_CH4[LASTLAYER], epsilon_O2[LASTLAYER];
  Real V;                 /* total oxygen porosity */
  Real soil_moist, O2_air, O2_upper, O2_lower, dO2;
  Real dt, CH4_air, CH4_upper, CH4_lower, dCH4;
  Real tmp_water;
  Real end, start;
  unsigned long int steps, t;
  end = start = tmp_water = 0;
  /*waterbalance needs to be updated*/
  start = soilmethane(soil);
  *runoff=0;
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
  /*  for(l=0;l<BOTTOMLAYER;l++)
  if (soil->w[l]< 0 || soil->w_fw[l]<0 )
  {
  fprintf(stderr,"negative soil water in gasdiffusion w= %3.5f w_fw= %3.5f\n",soil->w[l],soil->w_fw[l]);
  fflush(stdout);
  }
  */
  /*********************Diffusion of oxygen*************************************/

  O2_air = p_s / R_gas / degCtoK(airtemp)*O2s*WO2;       /*g/m3 oxygen concentration*/
  steps = 0;
  for (l = 0; l<LASTLAYER; l++)
  {
    soil_moist = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_O2[l] = max(0.001, V + soil_moist*soil->wsat[l]*BO2);
    if (V<0)
    {
      //fprintf(stderr,"negative soil water-V in gasdiffusion v= %3.5f\n",V);
      //fflush(stdout);
      V = 0;
    }
    D_O2[l]=(D_O2_air*V + D_O2_water*soil_moist*soil->wsat[l]*BO2)*eta;  // Gleichung 11 in Khvorostyanov part 1 diffusivity (m2 s-1)
    if (D_O2[l]>epsilon)
    {
      if (l == 0)
        dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / D_O2[l];
      else
        dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / (0.5*(D_O2[l] + D_O2[l - 1]));
      dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / D_O2[l] * epsilon_O2[l];
      steps = max(steps, (unsigned long)(timestep2sec(1.0, NSTEP_DAILY) / dt) + 1);
    }
    else
      steps = max(steps, 0);
  }

  for (t = 0; t<steps; ++t)
  {
    for (l = 0; l<BOTTOMLAYER; l++)
    {
      O2_upper = (l == 0) ? O2_air : soil->O2[l - 1] / soildepth[l - 1] / epsilon_O2[l - 1] * 1000;
      O2_lower = (l == BOTTOMLAYER - 1) ? 0 : soil->O2[l + 1] / soildepth[l + 1] / epsilon_O2[l + 1] * 1000;

      if (D_O2[l]>0)
      {
        dO2 = 0.5*(D_O2[l] + ((l == 0) ? D_O2_air : D_O2[l - 1]))*timestep2sec(1.0, steps) / (soildepth[l] * soildepth[l] * 1e-6)
          *(O2_upper - soil->O2[l] / soildepth[l] / epsilon_O2[l] * 1000);
        if(dO2>0 && (soil->O2[l-1]-dO2*soildepth[l]*epsilon_O2[l]/1000)<soil->O2[l]/soildepth[l]/epsilon_O2[l]*1000 && l>0)
        {
          soil->O2[l - 1] -= dO2*(soildepth[l - 1] * epsilon_O2[l - 1]) / (soildepth[l] * epsilon_O2[l])*(soildepth[l] * epsilon_O2[l]) / 1000;
          dO2 = dO2*(soildepth[l - 1] * epsilon_O2[l - 1]) / (soildepth[l] * epsilon_O2[l]);
        }
        else if (l>0)
          soil->O2[l - 1] -= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;

        soil->O2[l] += dO2*(soildepth[l] * epsilon_O2[l]) / 1000;

        if (soil->O2[l]< 0)
        {
          //fprintf(stderr,"1 O2 gasdiffusion layer[%d] diffussion:%g O2:%g O2_concentration: %g diffussion2:%g\n",l,dO2*(soildepth[l]*epsilon_O2)/1000,soil->O2[l],soil->O2[l]/soil->par->wsats[l]/(1-soil_moist)/soildepth[l]*1000,dO2*(soildepth[l+1]*epsilon_O2_l)/(soildepth[l]*epsilon_O2)*(soildepth[l]*epsilon_O2)/1000);
          soil->O2[l] = 0;
        }

        dO2 = 0.5*(D_O2[l] + ((l == BOTTOMLAYER - 1) ? D_O2[l] : D_O2[l + 1]))*timestep2sec(1.0, steps) / (soildepth[l] * soildepth[l] * 1e-6)
          *(O2_lower - soil->O2[l] / soildepth[l] / epsilon_O2[l] * 1000);
        if (dO2>0 && (soil->O2[l + 1] - dO2*soildepth[l] * epsilon_O2[l] / 1000)<soil->O2[l] / soildepth[l] / epsilon_O2[l] * 1000 && l != (BOTTOMLAYER - 1))
        {
          soil->O2[l + 1] -= dO2*(soildepth[l + 1] * epsilon_O2[l + 1]) / (soildepth[l] * epsilon_O2[l])*(soildepth[l] * epsilon_O2[l]) / 1000;
          dO2 = dO2*(soildepth[l + 1] * epsilon_O2[l + 1]) / (soildepth[l] * epsilon_O2[l]);
        }
        else if (l != BOTTOMLAYER - 1)
          soil->O2[l + 1] -= dO2*(soildepth[l] * epsilon_O2[l]) / 1000;
        if (l != BOTTOMLAYER - 1) soil->O2[l] += dO2*(soildepth[l] * epsilon_O2[l]) / 1000;

        if (soil->O2[l]< 0) {
          //fprintf(stderr,"2 O2 gasdiffusion layer[%d] diffussion:%g O2:%g O2_concentration: %g diffussion2:%g\n",l,dO2*(soildepth[l]*epsilon_O2)/1000,soil->O2[l],soil->O2[l]/soil->par->wsats[l]/(1-soil_moist)/soildepth[l]*1000,dO2*(soildepth[l+1]*epsilon_O2_l)/(soildepth[l]*epsilon_O2)*(soildepth[l]*epsilon_O2)/1000);
          soil->O2[l] = 0;
        }

        if (soil->O2[l + 1]< 0 && l != (BOTTOMLAYER - 1)) {
          fprintf(stderr, "2.1 O2 gasdiffusion layer[%d] diffussion:%g O2:%g  DO2: %g O2_lower:%g epsilon_02:%g steps:%lu\n", l, dO2*(soildepth[l] * epsilon_O2[l]) / 1000, soil->O2[l + 1], D_O2[l], O2_lower, epsilon_O2[l], steps);
        }

        //if(soil->O2[l]>1e+8) soil->O2[l]=O2_air*(soildepth[l]*epsilon_O2[l])/1000;
        if (fabs(dO2)<1e-18 || t == maxheatsteps)
          break;
      }
    }
  }
  /*********************Diffusion of methane*************************************/


  CH4_air = p_s / R_gas / degCtoK(airtemp)*pch4*1e-6*WCH4;    /*g/m3 air methane concentration*/
  CH4_upper = CH4_air;
  steps = 0;
  for (l = 0; l<LASTLAYER; l++)
  {
    soil_moist = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_CH4[l] = max(0.001, V + soil_moist*soil->wsat[l]*BCH4);
    if (V<0)
    {
      //fprintf(stderr,"negative soil water-V in gasdiffusion v= %3.5f\n",V);
      //fflush(stdout);
      V = 0;
    }
    D_CH4[l] = (D_CH4_air*V + D_CH4_water*soil_moist*soil->wsat[l]*BCH4)*eta;  // Gleichung 11 in Khvorostyanov part 1 diffusivity (m2 s-1)
    if (D_CH4[l]>epsilon)
    {
      if (l == 0)
        dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / D_CH4[l];
      else
        dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / (0.5*(D_CH4[l] + D_CH4[l - 1]));
      dt = 0.5*(soildepth[l] * soildepth[l] * 1e-6) / D_CH4[l] * epsilon_CH4[l];
      steps = max(steps, (unsigned long)(timestep2sec(1.0, NSTEP_DAILY) / dt) + 1);
    }
    else
      steps = max(steps, 0);
  }

  for (t = 0; t<steps; ++t)
  {
    for (l = 0; l<BOTTOMLAYER; l++)
    {
      CH4_upper = (l == 0) ? CH4_air : soil->CH4[l - 1] / soildepth[l - 1] / epsilon_CH4[l - 1] * 1000;
      CH4_lower = (l == BOTTOMLAYER - 1) ? 0 : soil->CH4[l + 1] / soildepth[l + 1] / epsilon_CH4[l + 1] * 1000;

      if (D_CH4[l]>0)
      {
        dCH4 = 0.5*(D_CH4[l] + ((l == 0) ? D_CH4_air : D_CH4[l - 1]))*timestep2sec(1.0, steps) / (soildepth[l] * soildepth[l] * 1e-6)
          *(CH4_upper - soil->CH4[l] / soildepth[l] / epsilon_CH4[l] * 1000);
        if (dCH4>0 && l>0)
        {
          soil->CH4[l - 1] -= dCH4*(soildepth[l - 1] * epsilon_CH4[l - 1]) / (soildepth[l] * epsilon_CH4[l])*(soildepth[l] * epsilon_CH4[l]) / 1000;
          dCH4 = dCH4*(soildepth[l - 1] * epsilon_CH4[l - 1]) / (soildepth[l] * epsilon_CH4[l]);
        }
        else if (l>0)
          soil->CH4[l - 1] -= dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;

        soil->CH4[l] += dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;

        if (soil->CH4[l]< 0) {
          //fprintf(stderr,"1 CH4 gasdiffusion layer[%d] diffussion:%g CH4:%g CH4_concentration: %g diffussion2:%g\n",l,dCH4*(soildepth[l]*epsilon_CH4)/1000,soil->CH4[l],soil->CH4[l]/soil->par->wsats[l]/(1-soil_moist)/soildepth[l]*1000,dCH4*(soildepth[l+1]*epsilon_CH4_l)/(soildepth[l]*epsilon_CH4)*(soildepth[l]*epsilon_CH4)/1000);
          soil->CH4[l] = 0;
        }

        dCH4 = 0.5*(D_CH4[l] + ((l == BOTTOMLAYER - 1) ? D_CH4[l] : D_CH4[l + 1]))*timestep2sec(1.0, steps) / (soildepth[l] * soildepth[l] * 1e-6)
          *(CH4_lower - soil->CH4[l] / soildepth[l] / epsilon_CH4[l] * 1000);
        if (dCH4>0 && l != (BOTTOMLAYER - 1))
        {
          soil->CH4[l + 1] -= dCH4*(soildepth[l + 1] * epsilon_CH4[l + 1]) / (soildepth[l] * epsilon_CH4[l])*(soildepth[l] * epsilon_CH4[l]) / 1000;
          dCH4 = dCH4*(soildepth[l + 1] * epsilon_CH4[l + 1]) / (soildepth[l] * epsilon_CH4[l]);
        }
        else if (l != BOTTOMLAYER - 1)
          soil->CH4[l + 1] -= dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;

        if (l != BOTTOMLAYER - 1) soil->CH4[l] += dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000;

        if (soil->CH4[l]< 0)
        {
          //fprintf(stderr,"2 CH4 gasdiffusion layer[%d] diffussion:%g CH4:%g CH4_concentration: %g diffussion2:%g\n",l,dCH4*(soildepth[l]*epsilon_CH4)/1000,soil->CH4[l],soil->CH4[l]/soil->par->wsats[l]/(1-soil_moist)/soildepth[l]*1000,dCH4*(soildepth[l+1]*epsilon_CH4_l)/(soildepth[l]*epsilon_CH4)*(soildepth[l]*epsilon_CH4)/1000);
          soil->CH4[l] = 0;
        }

        if (soil->CH4[l + 1]< 0 && l != (BOTTOMLAYER - 1))
        {
          fprintf(stderr, "2.1 CH4 gasdiffusion layer[%d] diffussion:%g CH4:%g  DCH4: %g CH4_lower:%g epsilon_CH4:%g steps:%lu\n", l, dCH4*(soildepth[l] * epsilon_CH4[l]) / 1000, soil->CH4[l + 1], D_CH4[l], CH4_lower, epsilon_CH4[l], steps);
        }
        if (fabs(dCH4)<1e-18 || t == maxheatsteps)
          break;
      }
    }
  }
  end = soilmethane(soil);
#ifdef CHECK_BALANCE2
  if (fabs(start - end - *CH4_out)>0.1)
    fprintf(stderr, "C-ERROR: %g start:%g  ende:%g gasdiff: %g\n", start - end - *CH4_out, start, end, *CH4_out);
#endif
  *CH4_out = start - end;
  //fprintf(stdout,"CH4 emmsisions from gasdiffusion %.4f\n",*CH4_out);
} /* of 'gasdiffusion' */
