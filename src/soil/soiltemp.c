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


void soiltemp(Soil *soil,          /**< pointer to soil data */
              Real temp_bs,        /**< temperature below snow (deg C) */
              const Config *config /**< LPJmL configuration */
             )
{

  Soil_thermal_prop th;                                /* thermal properties of soil */
  Real waterdiff[NSOILLAYER], soliddiff[NSOILLAYER];   /* change in water and solid contents, since last call */
  Real h[NHEATGRIDP], upDiBound;                       /* gridpoimt distances, upper dirichlet boundary condition */
  Real freezefrac[NSOILLAYER];                         /* fraction of each layer that is frozen */
  int l,j;
  
  /*****  Prognostic Part  ****/
  /* set up the refined heatgrid */
  for(l=0;l<NSOILLAYER;++l)
     for(j=0;j<GPLHEAT;++j)
       h[l*GPLHEAT+j]=soildepth[l]/GPLHEAT/1000;

  /* apply enthalpy changes coming from water flow and porosity changes */
  for(l=0;l<NSOILLAYER;++l)   /* track water flow and porosity changes of other methods */
  {
    waterdiff[l] = (allwater(soil,l)+allice(soil,l) - soil->old_totalwater[l]) / soildepth[l];
    soliddiff[l] = -(soil->wsat[l] - soil->old_wsat[l]);
    soil->old_totalwater[l] = allwater(soil,l) + allice(soil,l);
    soil->old_wsat[l]       = soil->wsat[l];
  }
  soil_therm_prop(&th,soil, soil->old_totalwater, soil->old_wsat, config->johansen, FALSE);
  daily_mass2heatflow(soil->enth, waterdiff, soliddiff, th);

  /* apply enthalpy changes due to heatconduction */
  soil->litter.agtop_temp = (temp_bs + ENTH2TEMP(soil->enth,th,0)) / 2;
  upDiBound               =  temp_bs                 * (1 - soil->litter.agtop_cover) +
                             soil->litter.agtop_temp * soil->litter.agtop_cover; 
  soil_therm_prop(&th,soil, NULL,NULL ,config->johansen,TRUE);
  daily_heatcond(soil->enth, NHEATGRIDP,h, upDiBound,th );

  /*****  Diagnostic Part  ****/
  enth2layertemp(soil->temp,soil->enth,th);    /* get layer temperture averages */
  enth2freezefrac(freezefrac, soil->enth, th); /* get frozen fraction of each layer */
  freezefrac2soil(soil,freezefrac);            /* apply frozen fraction to soil variables */

} /* of 'soiltemp' */
