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


void setup_heatgrid(Real *);
void get_soilcontent_change(Real *, Real *, Soil *);
Real calc_surface_temp(Real, Soil *, Soil_thermal_prop);
void soiltemp(Soil *, Real, const Config *);
void modify_enthalpy_due_to_masschanges(Soil *, const Config *);
void modify_enthalpy_due_to_heatconduction(Soil *, Real, Soil_thermal_prop,const Config *);
void compute_water_ice_ratios_from_enth(Soil *, const Config *, Soil_thermal_prop);
void compute_litter_temp_from_enth(Soil * soil, Real temp_below_snow ,const Config * config,Soil_thermal_prop therm_prop);

void soiltemp(Soil *soil,          /**< pointer to soil data */
              Real temperature_below_snow,        /**< (deg C) */
              const Config *config /**< LPJmL configuration */
             )
{

  Soil_thermal_prop therm_prop;                 
  calc_soil_thermal_properties(&therm_prop, soil, NULL,  NULL, config->johansen, TRUE); 
  
  /* apply daily changes to soil enthalpy distribution */
  modify_enthalpy_due_to_masschanges(soil ,config);
  modify_enthalpy_due_to_heatconduction(soil,temperature_below_snow, therm_prop, config);

  /* compute soil thermal attributes from enthalpy */
  compute_mean_layer_temps_from_enth(soil->temp,soil->enth, therm_prop);
  compute_water_ice_ratios_from_enth(soil,config,therm_prop);
  compute_litter_temp_from_enth(soil, temperature_below_snow ,config,therm_prop);

} 


void modify_enthalpy_due_to_masschanges(Soil * soil,const Config * config){
  Soil_thermal_prop old_therm_storage_prop;                      
  Real waterdiff[NSOILLAYER], soliddiff[NSOILLAYER];  
  calc_soil_thermal_properties(&old_therm_storage_prop, soil, soil->old_totalwater,  soil->old_wsat, config->johansen, FALSE); 
  get_soilcontent_change(waterdiff, soliddiff, soil);                     
  daily_mass2heatflow(soil->enth, waterdiff, soliddiff, old_therm_storage_prop);      
}
void modify_enthalpy_due_to_heatconduction(Soil * soil, Real temp_below_snow, Soil_thermal_prop therm_prop ,const Config * config){
  Real litter_agtop_temp;
  Real h[NHEATGRIDP], top_dirichlet_BC;          
  litter_agtop_temp = (temp_below_snow + ENTH2TEMP(soil->enth,therm_prop,0))/2;
  top_dirichlet_BC = temp_below_snow  * (1 - soil->litter.agtop_cover) +
         litter_agtop_temp * soil->litter.agtop_cover;      
  setup_heatgrid(h);
  daily_heatcond(soil->enth, NHEATGRIDP, h, top_dirichlet_BC, therm_prop ); 
}


void compute_water_ice_ratios_from_enth(Soil * soil, const Config * config, Soil_thermal_prop therm_prop){
  Real freezefrac[NSOILLAYER];                         /* fraction of each layer that is frozen */
  enth2freezefrac(freezefrac, soil->enth, therm_prop); /* get frozen fraction of each layer */
  freezefrac2soil(soil, freezefrac);                   /* apply frozen fraction to soil variables */
}
void compute_litter_temp_from_enth(Soil * soil, Real temp_below_snow ,const Config * config,Soil_thermal_prop therm_prop){
  soil->litter.agtop_temp = (temp_below_snow + ENTH2TEMP(soil->enth,therm_prop,0)) / 2;
}

/* helper */
void setup_heatgrid(Real *h){
  int l,j;
  for(l=0;l<NSOILLAYER;++l)
     for(j=0;j<GPLHEAT;++j)
       h[l*GPLHEAT+j]=soildepth[l]/GPLHEAT/1000;
}
void get_soilcontent_change(Real *waterdiff, Real *soliddiff, Soil *soil)
{
  int l;
  for(l=0;l<NSOILLAYER;++l)   /* track water flow and porosity changes of other methods */
  {
    waterdiff[l] = (allwater(soil,l)+allice(soil,l) - soil->old_totalwater[l]) / soildepth[l];
    soliddiff[l] = -(soil->wsat[l] - soil->old_wsat[l]);
    soil->old_totalwater[l] = allwater(soil,l) + allice(soil,l);
    soil->old_wsat[l]       = soil->wsat[l];
  }
}



/* Old content of soiltemp */

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
