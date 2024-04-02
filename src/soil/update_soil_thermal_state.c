/**************************************************************************************/
/**                                                                                \n**/
/**      u p d a t e _ s o i l _ t h e r m a l _ s t a t e .  c                    \n**/
/**                                                                                \n**/
/**        Scheme to update thermal (energy) state of soil                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

// The following three values are calculated by: lawrance and slater 2007, Incorporating organic soil into a global climate model
// the values for purely organic soils are used here as a rough estimate for litter thermal properties
#define K_LITTER_DRY 0.05  // thermal conductivity of organic material when completly dry
#define K_LITTER_SAT_FROZEN 2.106374   // thermal conudcitivity of fully saturated frozen organic material with a porosity of 0.944
#define K_LITTER_SAT_UNFROZEN 0.554636  // // thermal conudcitivity of fully saturated unfrozen organic material with a porosity of 0.944

// The following two values are rough estimates
#define K_SNOW_SOILINS 0.2 // snow thermal conductivty used for soil insulation
#define SNOWHEIGHT_PER_WATEREQ 4 // snow height per m^3 snow water equivalent used for snow insulation

#define DRY_BULK_DENSITY_LITTER 71.1
// porosity is calculated using f = (p_s - p_b)/p_s 
// Hillel: Environmental Soil Physics, p15 f.
#define POROSITY_LITTER 0.952  // [fraction]

STATIC void setup_heatgrid(Real *);
STATIC void setup_heatgrid_layer_boundaries(Real *);
STATIC void get_unaccounted_changes_in_water_and_solids(Real *, Real *, Real *, Soil *);
STATIC void update_wi_and_sol_enth_adjusted(Real *, Real *, Soil *);
STATIC void modify_enth_due_to_masschanges(Soil *, Real *);
STATIC void modify_enth_due_to_heatconduction(Uniform_temp_sign, Soil *, Real, Soil_thermal_prop *);
STATIC void compute_litter_and_snow_temp_from_enth(Soil * soil, Real temp_below_snow, const Soil_thermal_prop * therm_prop);
STATIC void compute_water_ice_ratios_from_enth(Soil *, const Soil_thermal_prop *);
STATIC void calc_gp_temps(Real * gp_temps, Real * enth, const  Soil_thermal_prop *);
STATIC void compute_maxthaw_depth(Soil * soil);
STATIC void compute_bottom_bound_layer_temps_from_enth(Real *,  const Real *,const Soil_thermal_prop *);
STATIC Uniform_temp_sign check_uniform_temp_sign_throughout_soil(Real *, Real, Real *);
STATIC void get_abs_waterice_cont(Real *, Soil *);
STATIC void adjust_for_litter(Real h[], Soil_thermal_prop * therm_prop, Soil * soil, Uniform_temp_sign uniform_temp_sign);
STATIC void adjust_for_snow(Real h[], Soil_thermal_prop * therm_prop, Soil * soil, Uniform_temp_sign uniform_temp_sign);
STATIC Real calc_litter_depth(Soil * soil);
STATIC Real calc_snow_depth(Soil * soil);
STATIC Real calc_litter_therm_conductivity(Real litter_temp,  Real sat_degree);


/**** main function ****/

void update_soil_thermal_state(Soil *soil,           /**< pointer to soil data */
                               Real temp_below_snow, /**< (deg C) */
                               const Config *config  /**< LPJmL configuration */
                              )
{
  /* calculate absolute water ice content of each layer and provide it for below functions */
  Real abs_waterice_cont[NSOILLAYER];
  get_abs_waterice_cont(abs_waterice_cont, soil);

  /* check if phase changes are already present in soil or can possibly happen during timestep due to temp_bs forcing */
  /* without the need of considering phase changes the below calculations simplify significantly */
  Uniform_temp_sign uniform_temp_sign;
  uniform_temp_sign = check_uniform_temp_sign_throughout_soil(soil->enth, temp_below_snow, abs_waterice_cont);

  /* calculate soil thermal properties and provide it for below functions */
  Soil_thermal_prop therm_prop = {};
  calc_soil_thermal_props(uniform_temp_sign, &therm_prop, soil, abs_waterice_cont,  NULL, config->johansen, TRUE);

  /* apply daily changes to soil enthalpy distribution  due to heatconvection and heatconduction*/
  modify_enth_due_to_masschanges(soil, abs_waterice_cont);
  modify_enth_due_to_heatconduction(uniform_temp_sign, soil, temp_below_snow, &therm_prop);

  /* compute soil thermal attributes from enthalpy distribution and thermal properties, i.e. the derived quantities */
  compute_mean_layer_temps_from_enth(soil->temp,soil->enth, &therm_prop);
  compute_litter_and_snow_temp_from_enth(soil, temp_below_snow, &therm_prop);
  if(uniform_temp_sign == MIXED_SIGN || uniform_temp_sign == UNKNOWN)
  {
    compute_water_ice_ratios_from_enth(soil,&therm_prop);
    compute_maxthaw_depth(soil);
  }
}


/**** functions used by update_soil_thermal_state ****/

/* Function checks if forcing as well as all temperatures in the soil have the same sign.
   This simplifies calculations as no phase changes need to be considered. */
STATIC Uniform_temp_sign check_uniform_temp_sign_throughout_soil(Real * enth, Real temp_top, Real * abs_waterice_cont)
{
  int l,gp;
  int temp_sign;
  int temp_sign_top = (temp_top < 0 ? -1 : 0) +  /* get the sign of forcing temp */
                      (temp_top > 0 ?  1 : 0);
  Real vol_latent_heat;
  foreachsoillayer(l)
  {
    vol_latent_heat = abs_waterice_cont[l]/soildepth[l] * c_water2ice;
    for(gp=l*GPLHEAT;gp<(l+1)*GPLHEAT;gp++)
    {
      temp_sign = (enth[gp] < 0               ? -1 : 0) +
                  (enth[gp] > vol_latent_heat ?  1 : 0);
      if (temp_sign != temp_sign_top) /* check for all gp temps if sign is that of forcing temp */
        return MIXED_SIGN;
    }
  }

  switch (temp_sign_top)
  {
    case -1:
      return ALL_BELOW_0;
    case  0:
      return MIXED_SIGN;
    case  1:
      return ALL_ABOVE_0;
    default:
      return UNKNOWN;
  }
}

STATIC void modify_enth_due_to_masschanges(Soil * soil, Real * abs_waterice_cont)
{
  /* Apply the percolation enthalpy water percolation
     for which the corresponding enthalpy shifts are known */
  apply_perc_enthalpy(soil);

  /* Bookkeep which changes have been done to water and solids without adjusting enthalpy */
  Real waterdiff[NSOILLAYER], soliddiff[NSOILLAYER];
  get_unaccounted_changes_in_water_and_solids(waterdiff, soliddiff, abs_waterice_cont, soil);

  /* For those unaccounted changed add or remove some enthalpy to avoid temperature changes */
  apply_enth_of_untracked_mass_shifts(soil->enth, waterdiff, soliddiff, soil->wi_abs_enth_adj, soil->sol_abs_enth_adj);

  /* Set enthalpy adjusted waterice and solid content to current waterice and solid contents
     as enthalpy adjustment have been made for the unaccounted parts */
  update_wi_and_sol_enth_adjusted(waterdiff, soliddiff, soil);
}


STATIC void modify_enth_due_to_heatconduction(Uniform_temp_sign uniform_temp_sign, Soil * soil, Real temp_below_snow, Soil_thermal_prop * therm_prop)
{
  Real litter_agtop_temp;
  Real h[NHEATGRIDP], top_dirichlet_BC;

  /* Compute the dirichlet boundary condition. */
  top_dirichlet_BC = temp_below_snow ;

  /* setup grid */
  setup_heatgrid(h);

  /* modify grid and conductivity of top layer if snow is present */
  adjust_for_snow(h, therm_prop, soil, uniform_temp_sign);
  adjust_for_litter(h, therm_prop, soil, uniform_temp_sign);

  /* apply heatconduction */
  apply_heatconduction_of_a_day(uniform_temp_sign, soil->enth, h, top_dirichlet_BC, therm_prop);
}

STATIC void adjust_for_snow(Real h[], Soil_thermal_prop * therm_prop, Soil * soil, Uniform_temp_sign uniform_temp_sign)
{
  Real snowdepth = calc_snow_depth(soil);

  if(!(uniform_temp_sign == ALL_ABOVE_0))
  {
    Real froz_thermal_resistance = (h[0]/therm_prop->lam_frozen[0]) + (snowdepth/K_SNOW_SOILINS);
    Real froz_therm_cond = (h[0]+snowdepth) / froz_thermal_resistance;
    therm_prop->lam_frozen[0] = froz_therm_cond;
  }

  if(!(uniform_temp_sign == ALL_BELOW_0))
  {
    Real unfroz_thermal_resistance = (h[0]/therm_prop->lam_unfrozen[0]) + (snowdepth/K_SNOW_SOILINS);
    Real unfroz_therm_cond = (h[0]+snowdepth) / unfroz_thermal_resistance;
    therm_prop->lam_unfrozen[0] = unfroz_therm_cond;
  }

  h[0]+=snowdepth;
}


STATIC void adjust_for_litter(Real h[], Soil_thermal_prop * therm_prop, Soil * soil, Uniform_temp_sign uniform_temp_sign)
{
  Real litterdepth = calc_litter_depth(soil); // unit: [m]
  Real saturation_degree;
  
  if(soil->litter.agtop_moist < epsilon || litterdepth  < epsilon )
    saturation_degree = 0;
  else
    saturation_degree = min((soil->litter.agtop_moist/1000)/(POROSITY_LITTER * litterdepth), 1);

  Real lam_litter = calc_litter_therm_conductivity(soil->litter.agtop_temp, saturation_degree);

  if(!(uniform_temp_sign == ALL_ABOVE_0))
  {
    Real froz_thermal_resistance = (h[0]/therm_prop->lam_frozen[0]) + (litterdepth/lam_litter );
    Real froz_therm_cond = (h[0]+litterdepth) / froz_thermal_resistance;
    therm_prop->lam_frozen[0] = froz_therm_cond;
  }

  if(!(uniform_temp_sign == ALL_BELOW_0))
  {
    Real unfroz_thermal_resistance = (h[0]/therm_prop->lam_unfrozen[0]) + (litterdepth/lam_litter );
    Real unfroz_therm_cond = (h[0]+litterdepth) / unfroz_thermal_resistance;
    therm_prop->lam_unfrozen[0] = unfroz_therm_cond;
  }

  h[0]+=litterdepth;
}

STATIC Real calc_litter_therm_conductivity(Real litter_temp,  // temperature of litter
                                           Real sat_degree   // saturation degree
                                           ) 
{
  // source: lawrance and slater 2007, Incorporating organic soil into a global climate model
  // values for purely organic soils are used for the litter layer

  Real lam_sat;     // saturated thermal conductivity; unit: W m-1 K-1
  Real ke;          // kersten number

  if(sat_degree < epsilon) 
    return(K_LITTER_DRY);

  if(litter_temp < 0) // water in litter is assumed to be frozen
  {
    ke = sat_degree;
    lam_sat = K_LITTER_SAT_FROZEN;
  }
  else
  {
    ke = (sat_degree < 0.1 ? 0 : log10(sat_degree) + 1);
    lam_sat = K_LITTER_SAT_UNFROZEN;
  }

  return((lam_sat   - K_LITTER_DRY) * ke   + K_LITTER_DRY);
}

STATIC Real calc_litter_depth(Soil * soil)
{
  Real dm_sum = calc_litter_dm_sum(soil);
  return((dm_sum/1000)/DRY_BULK_DENSITY_LITTER); // resulting unit: [m]
}

STATIC Real calc_snow_depth(Soil * soil)
{
  return(soil->snowpack*SNOWHEIGHT_PER_WATEREQ*1e-3); // resulting unit: [m] 
}

STATIC void compute_water_ice_ratios_from_enth(Soil * soil, const Soil_thermal_prop *therm_prop)
{
  Real freezefrac[NSOILLAYER];                         /* fraction of each layer that is frozen */
  enth2freezefrac(freezefrac, soil->enth, therm_prop); /* get frozen fraction of each layer */
  freezefrac2soil(soil, freezefrac);                   /* apply frozen fraction to soil variables */
}

STATIC void compute_litter_and_snow_temp_from_enth(Soil * soil, Real airtemp , const Soil_thermal_prop *therm_prop)
{
  Real snowdepth = calc_snow_depth(soil);
  Real litterdepth = calc_litter_depth(soil);
  Real dist_top_gp_soilsurf = soildepth[0]/1000/(GPLHEAT *2); // top soil grdpoint to soil surface / soil litter interface distance 
  Real top_soil_gp_to_snow_surf_dist = dist_top_gp_soilsurf + litterdepth + snowdepth;

  Real top_soil_gp_temp = ENTH2TEMP(soil->enth,therm_prop,0);

  Real weight_airtemp_litter = (dist_top_gp_soilsurf + litterdepth/2)/top_soil_gp_to_snow_surf_dist;
  Real litter_temp = top_soil_gp_temp * (1- weight_airtemp_litter) + airtemp * weight_airtemp_litter;

  Real weight_airtemp_snow = (top_soil_gp_to_snow_surf_dist - snowdepth/2)/top_soil_gp_to_snow_surf_dist;
  Real snow_temp = top_soil_gp_temp * (1- weight_airtemp_snow) + airtemp * weight_airtemp_snow;
  

  soil->litter.agtop_temp = litter_temp;
  soil->temp[NSOILLAYER] = snow_temp;
}


STATIC void compute_maxthaw_depth(Soil * soil)
{
  Real depth_to_first_ice=0;
  int l;
  for (l=0;l<=BOTTOMLAYER;l++)
  {
    depth_to_first_ice+=soildepth[l];
    if(soil->freeze_depth[l]>epsilon)
      break;
  }
  depth_to_first_ice-=soil->freeze_depth[l];
  if (soil->maxthaw_depth<depth_to_first_ice)
   soil->maxthaw_depth=depth_to_first_ice;
}


/**** small helper functions  ****/

STATIC void get_abs_waterice_cont(Real * abs_waterice_cont, Soil * soil)
{
  int l;
  foreachsoillayer(l)
    abs_waterice_cont[l] = allice(soil,l) + allwater(soil,l);
}

/* The function evenly distributes grid points within each soil layer.
   For a single grid point per layer (GPLHEAT=1), it's placed at the layer's midpoint.
   With multiple grid points, they are arranged so that the distance from the border to the
   nearest grid points is half the distance between adjacent grid points within the layer.
   Note that the intervals betweens adjacent gridpoints are called elements. Thus, with this
   grid arrangement, the elements can cross layer borders. */

STATIC void setup_heatgrid(Real *h /**< distances between adjacent gridpoints */
                          )
{
  int l,j;
  Real nodes[NHEATGRIDP];
  Real soillayer_depth_m=0; /* sollayerdepth in meter*/
  Real layer_border_m=0;    /* depth of current layer border in meter*/

  for(l=0;l<NSOILLAYER;++l)
  {
    soillayer_depth_m= soildepth[l]/1000;
    for(j=0;j<GPLHEAT;++j)
    {
      nodes[l*GPLHEAT+j]=layer_border_m+soillayer_depth_m/(GPLHEAT*2)+(soillayer_depth_m/GPLHEAT)*j;
    }
    layer_border_m+=soillayer_depth_m;
  }

  for(l=0;l<NSOILLAYER;++l)
    for(j=0;j<GPLHEAT;++j)
      h[l*GPLHEAT+j] = nodes[l*GPLHEAT+j] - (l*GPLHEAT+j>0?nodes[l*GPLHEAT+j-1]:0);

}


STATIC void get_unaccounted_changes_in_water_and_solids(Real *waterdiff, Real *soliddiff, Real * abs_waterice_cont, Soil *soil)
{
  int l;
  foreachsoillayer(l)
  {
    waterdiff[l] = (abs_waterice_cont[l] - soil->wi_abs_enth_adj[l]);
    soliddiff[l] = (soildepth[l]-soil->wsats[l])-soil->sol_abs_enth_adj[l];
  }
}

STATIC void update_wi_and_sol_enth_adjusted(Real * waterdiff, Real *soliddiff, Soil * soil){
  int l;
  foreachsoillayer(l)
  {
    soil->wi_abs_enth_adj[l]  = soil->wi_abs_enth_adj[l]  + waterdiff[l];
    soil->sol_abs_enth_adj[l] = soil->sol_abs_enth_adj[l] + soliddiff[l];
  }
}

/**** unused functions; Only exist for testing purposes or backward compatibility ****/

/* Function returns temperature at all individual gridpoints. */
STATIC void calc_gp_temps(Real * gp_temps, Real * enth,const  Soil_thermal_prop *th)
{
  int gp;
  for(gp=0;gp<NHEATGRIDP;gp++)
  {
    gp_temps[gp]=ENTH2TEMP(enth, th, gp);
  }
}


/* Function defines an alternative grid where elements do not cross layer boundaries.
   It allows to extract tempeatures at layer boundaries, since last gridpoint of each layer is on its boundary
   to next layer. Is an alternative to setupheat_grid. */
STATIC void setup_heatgrid_layer_boundaries(Real *h)
{
  int l,j;
  for(l=0;l<NSOILLAYER;++l)
  {
    for(j=0;j<GPLHEAT;++j)
    {
      h[l*GPLHEAT+j]= (soildepth[l]/1000)/GPLHEAT;
    }
  }
}


/* Function computes bottom boundary layer temeperatures. Should be used in combination with setup_heatgrid_layer_boundaries.
   Is an alternative to compute_mean_layer_temps_from_enth. */
STATIC void compute_bottom_bound_layer_temps_from_enth(Real *temp,
                                                       const Real *enth,
                                                       const Soil_thermal_prop *th
                                                      )
{
  int layer;
  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    temp[layer]= ENTH2TEMP(enth,th,(layer+1)*GPLHEAT-1); /* take the bottom most gp of each layer */
  }
}
