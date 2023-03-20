/**************************************************************************************/
/**                                                                                \n**/
/**                    s  o  i  l  t  e  m  p  .  c                                \n**/
/**                                                                                \n**/
/**     More advanced soil temperature and moisture scheme                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/*#define USE_LINEAR_CONTACT_T  */ /*linear interpolation between temperatures seems to give a reasonable approximation of contact temperatures between layers*/

#define rho_om 1300.9     /* density of organic matter [kg/m3]*/
#define bd_leaves 20.0    /* bulk density of non-woody material, different values can be used (see Enrique et al. 1999 [kg/m3])*/
#define  heatcap_om 2.5e6 /* volumetric heat capacity of organic matter [J/m3/K] */
#define lambda_litter 0.1

Real soiltemp_lag(const Soil *soil,      /**< Soil data */
                  const Climbuf *climbuf /**< Climate buffer */
                 )                       /** \return soil temperature (deg C) */
{
  Real a,b,temp_lag;
  if(soil->w[0]<epsilon)
    return climbuf->temp[NDAYS-1];
  linreg(&a,&b,climbuf->temp,NDAYS);
  temp_lag=a+b*(NDAYS-1-soil->alag*LAG_CONV);
  return climbuf->atemp_mean+soil->amp*(temp_lag-climbuf->atemp_mean);
} /* of 'soiltemp_lag' */

/* heat conduction equation: dT/dt = th_diff*d2T/dz2
 * is solved with a finite-difference solution
 * algorithm and stability criterion are taken from:
 * Y.Bayazitoglu / M.N.Oezisik: Elements of Heat Transfer (1988)
 */

void soiltemp(Soil *soil,          /**< pointer to soil data */
              Real temp_bs,        /**< temperature below snow (deg C) */
              const Config *config /**< LPJmL configuration */
             )
{
  Soil_thermal_prop th;
  Real waterdiff[NSOILLAYER];
  Real new_totalwater;
  Real h[NHEATGRIDP];
  int l,j;


  for(l=0;l<NSOILLAYER;++l)
     for(j=0;j<GPLHEAT;++j)
       h[l*GPLHEAT+j]=soildepth[l]/GPLHEAT/1000;


  /* apply enthalpy changes coming from water flow */
  soil_therm_prop(&th,soil, soil->old_totalwater ,config->johansen);
  for(l=0;l<NSOILLAYER;++l)
  {
    new_totalwater =allwater(soil,l)+allice(soil,l);
    waterdiff[l]=new_totalwater-soil->old_totalwater[l];
    waterdiff[l]=waterdiff[l]/soildepth[l];
    soil->old_totalwater[l]=new_totalwater;
  }
  update_e_from_wf(soil->enth, waterdiff, th);


  soil_therm_prop(&th,soil, soil->old_totalwater ,config->johansen);
  //daily_heatcond(soil->enth, NHEATGRIDP,h, temp_bs,th );
  derive_T_from_e(soil->temp,soil->enth,th);

  printf("top temp: %0.8f, top water: %0.2f, soil: %p \n",ENTH2TEMP(soil->enth,th,1), allwater(soil,0)+allice(soil,0), soil );
} /* of 'soiltemp' */
