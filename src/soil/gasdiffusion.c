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

//#define EXPLICIT

void gasdiffusion(Soil *soil,     /**< [inout] pointer to soil data */
                  Real airtemp,   /**< [in] air temperature (deg C) */
                  Real pch4,      /**< [in] atmospheric CH4 (ppm) */
                  Real *CH4_out,  /**< [out] CH4 emissions (gC/m2/day) */
                  Real *runoff,   /**< [out] runoff (mm/day) */
                  Real *CH4_sink,  /**< [out] CH4 soil sink (gC/m2/day) */
                  const Real *bCH4,
                  const Real *bO2,
                  int timesteps
                 )
{
  int l;
#ifdef EXPLICIT
  int diffsteps = 1;
#else
  int diffsteps = 1;
#endif
  Real dt = day2sec(1) / diffsteps/timesteps;
  Real D_O2[BOTTOMLAYER], D_CH4[BOTTOMLAYER]; /* oxygen/methane diffusivity [m2/s]*/
  Real epsilon_CH4[BOTTOMLAYER], epsilon_O2[BOTTOMLAYER];
  Real V;                 /* total oxygen porosity */
  Real soil_moist, O2_air;
  Real CH4_air;
  Real tmp_water;
  Real end, start, out,in;
#ifdef USE_TIMING
  double tstart;
  timing_start(tstart);
 #endif

  end=start=tmp_water=out=in=0;
  /*waterbalance needs to be updated*/
  start = soilmethane(soil); //do not multiply by *WC/WCH4, is used for methane fluxes here
  *CH4_out=*CH4_sink=0;


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
      if(soil->w_fw[l]<0 && soil->w_fw[l]>-epsilon) soil->w_fw[l]=0;
    }
  }

  /* get distances in meter */
  Real h[BOTTOMLAYER]; /* thickness of soil layers in m */
  for (l = 0; l<BOTTOMLAYER; l++)
    h[l] = soildepth[l] * 1e-3;

  /*********************Diffusion of oxygen*************************************/

  O2_air = p_s / R_gas / degCtoK(airtemp)*O2s*WO2;       /*g/m3 oxygen concentration*/
  Bool do_diffusion = TRUE;
#ifndef EXPLICIT
  Bool r = FALSE;
#endif

  for (l = 0; l<BOTTOMLAYER; l++)
  {
    soil_moist = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_O2[l] = getepsilon_O2(V,soil_moist,soil->wsat[l],bO2[l]);
    if (V<0)
    {
      V = 0;
    }
    D_O2[l]=D_O2_air*V*eta + D_O2_water*soil_moist*soil->wsat[l];  // eq. 11 in Khvorostyanov part 1 diffusivity (m2 s-1)
    //D_O2[l]=D_O2_air*(V*V*V)/(soil->wsat[l]*soil->wsat[l])*pow(airtemp+273.15,1.75)+ D_O2_water*soil_moist*soil->wsat[l]; //Moldrup
    if (epsilon_O2[l] <= 0.01 &&  (soil->freeze_depth[l]+epsilon)>=soildepth[l])
      do_diffusion = FALSE;
  }
#ifdef EXPLICIT
  Real res[BOTTOMLAYER];
  calculate_resistances(res, h, D_O2, BOTTOMLAYER);
#endif

  if (do_diffusion)
    for (l = 0; l<diffsteps; l++)
#ifdef EXPLICIT
      finite_volume_diffusion_timestep(soil->O2, BOTTOMLAYER, dt,h, O2_air, res, epsilon_O2);
#else
      r = apply_finite_volume_diffusion_impl(soil->O2, BOTTOMLAYER, h, O2_air, D_O2, epsilon_O2, dt);
  if(r)
  {
    /* print all diffusivities and porosities */
    for (l = 0; l<BOTTOMLAYER; l++)
      printf("D_O2[%d]=%g, epsilon_O2[%d]=%g\n", l, D_O2[l], l, epsilon_O2[l]);
  

    perror("Error in gasdiffusion: apply_finite_volume_diffusion_of_a_day for O2 returned TRUE");
  }
#endif

  /*********************Diffusion of methane*************************************/

  CH4_air = p_s / R_gas / degCtoK(airtemp)*pch4*1e-6*WCH4;    /*g/m3 air methane concentration*/
  do_diffusion = TRUE;
  for (l = 0; l<BOTTOMLAYER; l++)
  {
    soil_moist = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_CH4[l] = getepsilon_CH4(V,soil_moist,soil->wsat[l],bCH4[l]);
    if (V<0)
    {
      V = 0;
    }
    D_CH4[l] = D_CH4_air*V*eta + D_CH4_water*soil_moist*soil->wsat[l];  // eq. 11 in Khvorostyanov part 1 diffusivity (m2 s-1)
    //D_CH4[l]=D_CH4_air*(V*V*V)/(soil->wsat[l]*soil->wsat[l])*pow(airtemp+273.15,1.75)+ D_CH4_water*soil_moist*soil->wsat[l]; //Moldrup
    if (epsilon_CH4[l] <= 0.01 && (soil->freeze_depth[l]+epsilon)>=soildepth[l])
      do_diffusion = FALSE;
  }
#ifdef EXPLICIT
  calculate_resistances(res, h, D_CH4, BOTTOMLAYER);
#endif
  if (do_diffusion)
    for (l = 0; l<diffsteps; l++)
#ifdef EXPLICIT
     finite_volume_diffusion_timestep(soil->CH4, BOTTOMLAYER, dt,h, CH4_air, res, epsilon_CH4);
#else
     r = apply_finite_volume_diffusion_impl(soil->CH4, BOTTOMLAYER, h, CH4_air, D_CH4, epsilon_CH4, dt);
  if(r)
  {
        /* print all diffusivities and porosities */
    for (l = 0; l<BOTTOMLAYER; l++)
      printf("D_CH4[%d]=%g, epsilon_CH4[%d]=%g\n", l, D_CH4[l], l, epsilon_CH4[l]);
    perror("Error in gasdiffusion: apply_finite_volume_diffusion_of_a_day for CH4 returned TRUE");
  }
#endif

  end = soilmethane(soil); //do not multiply by *WC/WCH4, is used for methane fluxes here
#ifdef SAFE
  for (l = 0; l<BOTTOMLAYER; l++)
    if (soil->w[l]< -epsilon || soil->w_fw[l]< -epsilon )
    {
      fprintf(stderr,"\n\ngasdiffusion soilwater=%.6f soilice=%.6f wsats=%.6f agtop_moist=%.6f\n",
              allwater(soil,l),allice(soil,l),soil->wsats[l],soil->litter.agtop_moist);
      fflush(stderr);
      fprintf(stderr,"Soil-moisture layer %d negative: w:%g, fw:%g, soil_type %s \n\n",
              l,soil->w[l],soil->w_fw[l],soil->par->name);
    }
#endif

  if((start-end)<0)
   *CH4_sink=start-end;
  else
   *CH4_out=start-end;
#ifdef USE_TIMING
  timing_stop(DAILY_LITTERSOM_FCN,tstart);
#endif
} /* of 'gasdiffusion' */
