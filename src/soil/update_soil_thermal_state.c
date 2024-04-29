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

/* For the snow thermal insulation the following alternative
 * value for snowheight per water height is used
 * compare to c_watertosnow */
#define SNOWHEIGHT_PER_WATERHEIGHT 4

/* declare internally used functions */
STATIC void setup_heatgrid(Real *);
STATIC void get_unaccounted_changes_in_water_and_solids(Real *, Real *, const Real *, const Soil *);
STATIC void update_wi_and_sol_enth_adjusted(const Real *, const Real *, Soil *);
STATIC void modify_enth_due_to_masschanges(Soil *, const Real *);
STATIC void modify_enth_due_to_heatconduction(Uniform_temp_sign, Soil *, Real, Soil_thermal_prop *);
STATIC void compute_litter_and_snow_temp_from_enth(Soil *, Real, const Soil_thermal_prop *);
STATIC void compute_water_ice_ratios_from_enth(Soil *, const Soil_thermal_prop *);
STATIC void compute_maxthaw_depth(Soil *);
STATIC Uniform_temp_sign check_uniform_temp_sign_throughout_soil(const Real *, Real, const Real *);
STATIC void get_abs_waterice_cont(Real *, const Soil *);
STATIC void adjust_grid_and_therm_cond_for_litter(Real *, Soil_thermal_prop *, const Soil *, Uniform_temp_sign);
STATIC void adjust_grid_and_therm_cond_for_snow(Real *, Soil_thermal_prop *, const Soil *, Uniform_temp_sign);
STATIC Real calc_litter_depth(const Soil *);
STATIC Real calc_snow_depth(const Soil *);
STATIC Real calc_litter_therm_conductivity(Real, Real);


/******** main function ********/
void update_soil_thermal_state(Soil *soil,           /**< pointer to soil data */
                               Real airtemp,         /**< (deg C) */
                               const Config *config  /**< LPJmL configuration */
                              )
{
  /* calculate absolute water ice content of each layer and provide it for below functions */
  Real abs_waterice_cont[NSOILLAYER];
  get_abs_waterice_cont(abs_waterice_cont, soil);

  /* check if phase changes are already present in soil or can possibly happen during timestep due to airtemp forcing */
  /* without the need of considering phase changes the below calculations simplify significantly */
  Uniform_temp_sign uniform_temp_sign;
  uniform_temp_sign = check_uniform_temp_sign_throughout_soil(soil->enth, airtemp, abs_waterice_cont);

  /* calculate soil thermal properties and provide it for below functions */
  Soil_thermal_prop therm_prop = {}; /* initialize struct */
  calc_soil_thermal_props(uniform_temp_sign, &therm_prop, soil, abs_waterice_cont, NULL, config->johansen, TRUE);

  /* apply daily changes to soil enthalpy distribution  due to heatconvection and heatconduction */
  modify_enth_due_to_masschanges(soil, abs_waterice_cont);
  modify_enth_due_to_heatconduction(uniform_temp_sign, soil, airtemp, &therm_prop);

  /* compute soil thermal attributes from enthalpy distribution and thermal properties, i.e. the derived quantities */
  compute_mean_layer_temps_from_enth(soil->temp, soil->enth, &therm_prop);
  compute_litter_and_snow_temp_from_enth(soil, airtemp, &therm_prop);
  compute_water_ice_ratios_from_enth(soil, &therm_prop);
  compute_maxthaw_depth(soil);
} /* of 'update_soil_thermal_state' */


/******** functions used by update_soil_thermal_state ********/

/* The function checks if airtemp forcing as well as all temperatures within the soil have the same sign.
 * This simplifies calculations as no phase changes need to be considered. */
STATIC Uniform_temp_sign check_uniform_temp_sign_throughout_soil(const Real *enth, Real temp_top, const Real *abs_waterice_cont)
{
  int l, gp;
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
} /* of 'check_uniform_temp_sign_throughout_soil' */

STATIC void modify_enth_due_to_masschanges(Soil *soil, const Real *abs_waterice_cont)
{
  /* apply the percolation enthalpy of water percolation
     for which the corresponding enthalpy shifts are known */
  apply_perc_enthalpy(soil);

  /* bookkeep which changes have been done to water and solids without adjusting enthalpy */
  Real waterdiff[NSOILLAYER], soliddiff[NSOILLAYER];
  get_unaccounted_changes_in_water_and_solids(waterdiff, soliddiff, abs_waterice_cont, soil);

  /* for those unaccounted changed add or remove some enthalpy to avoid temperature changes */
  apply_enth_of_untracked_mass_shifts(soil->enth, waterdiff, soliddiff, soil->wi_abs_enth_adj, soil->sol_abs_enth_adj);

  /* set enthalpy adjusted waterice and solid content to current waterice and solid contents
     as enthalpy adjustment have been made for the unaccounted parts */
  update_wi_and_sol_enth_adjusted(waterdiff, soliddiff, soil);
} /* of 'modify_enth_due_to_masschanges' */

STATIC void modify_enth_due_to_heatconduction(Uniform_temp_sign uniform_temp_sign, Soil *soil, Real airtemp, Soil_thermal_prop *therm_prop)
{
  Real h[NHEATGRIDP], top_dirichlet_BC;

  /* set the dirichlet boundary condition */
  top_dirichlet_BC = airtemp;

  /* setup grid */
  setup_heatgrid(h);

  /* modify grid and conductivity of top element if snow or litter is present */
  adjust_grid_and_therm_cond_for_snow(h, therm_prop, soil, uniform_temp_sign);
  adjust_grid_and_therm_cond_for_litter(h, therm_prop, soil, uniform_temp_sign);

  /* apply numerical heatconduction scheme */
  apply_heatconduction_of_a_day(uniform_temp_sign, soil->enth, h, top_dirichlet_BC, therm_prop);
} /* of 'modify_enth_due_to_heatconduction' */

STATIC void adjust_grid_and_therm_cond_for_snow(Real h[], Soil_thermal_prop *therm_prop, const Soil *soil, Uniform_temp_sign uniform_temp_sign)
{
  /* calc snowdepth */
  Real snowdepth = calc_snow_depth(soil);

  /* adjust frozen thermal conductivity */
  if(!(uniform_temp_sign == ALL_ABOVE_0))
  {
    /* use fact that thermal resistances sum up for layers in serial arrangement */
    Real froz_thermal_resistance = (h[0]/therm_prop->lam_frozen[0]) + (snowdepth/lambda_snow);
    Real froz_therm_cond = (h[0]+snowdepth) / froz_thermal_resistance;
    therm_prop->lam_frozen[0] = froz_therm_cond;
  }

  /* adjust unfrozen thermal conductivity */
  if(!(uniform_temp_sign == ALL_BELOW_0))
  {
    Real unfroz_thermal_resistance = (h[0]/therm_prop->lam_unfrozen[0]) + (snowdepth/lambda_snow);
    Real unfroz_therm_cond = (h[0]+snowdepth) / unfroz_thermal_resistance;
    therm_prop->lam_unfrozen[0] = unfroz_therm_cond;
  }

  /* adjust grid */
  h[0]+=snowdepth;
} /* of 'adjust_grid_and_therm_cond_for_snow' */

STATIC void adjust_grid_and_therm_cond_for_litter(Real h[], Soil_thermal_prop *therm_prop,const Soil *soil, Uniform_temp_sign uniform_temp_sign)
{
  Real litterdepth = calc_litter_depth(soil); /* (m) */
  Real saturation_degree;

  if(soil->litter.agtop_moist < epsilon || litterdepth  < epsilon )
    saturation_degree = 0;
  else
    saturation_degree = min((soil->litter.agtop_moist/1000) / (POROSITY_LITTER * litterdepth), 1);

  Real lam_litter = calc_litter_therm_conductivity(soil->litter.agtop_temp, saturation_degree);

  /* adjust frozen thermal conductivity */
  if(!(uniform_temp_sign == ALL_ABOVE_0))
  {
    /* use fact that thermal resistances sum up for layers in serial arrangement */
    Real froz_thermal_resistance = (h[0]/therm_prop->lam_frozen[0]) + (litterdepth/lam_litter );
    Real froz_therm_cond = (h[0]+litterdepth) / froz_thermal_resistance;
    therm_prop->lam_frozen[0] = froz_therm_cond;
  }

  /* adjust unfrozen thermal conductivity */
  if(!(uniform_temp_sign == ALL_BELOW_0))
  {
    Real unfroz_thermal_resistance = (h[0]/therm_prop->lam_unfrozen[0]) + (litterdepth/lam_litter );
    Real unfroz_therm_cond = (h[0]+litterdepth) / unfroz_thermal_resistance;
    therm_prop->lam_unfrozen[0] = unfroz_therm_cond;
  }

  /* adjust grid */
  h[0]+=litterdepth;
} /* of 'adjust_grid_and_therm_cond_for_litter' */

STATIC Real calc_litter_therm_conductivity(Real litter_temp, Real sat_degree)
{
  /* source: lawrance and slater 2007, Incorporating organic soil into a global climate model
   * their values for purely organic soils are used for the litter layer */

  Real lam_sat;       /* saturated thermal conductivity */
  Real ke;            /* kersten number */

  if(sat_degree < epsilon)
    return(K_LITTER_DRY);

  if(litter_temp < 0) /* water in litter is then assumed to be frozen */
  {
    ke = sat_degree;
    lam_sat = K_LITTER_SAT_FROZEN;
  }
  else
  {
    ke = (sat_degree < 0.1 ? 0 : log10(sat_degree) + 1);
    lam_sat = K_LITTER_SAT_UNFROZEN;
  }

  return((lam_sat - K_LITTER_DRY) * ke + K_LITTER_DRY);
} /* of 'calc_litter_therm_conductivity' */

STATIC void compute_litter_and_snow_temp_from_enth(Soil *soil, Real airtemp , const Soil_thermal_prop *therm_prop)
{
  /* the function assumes that above the soil surface there is first the litter layer and then on top the snow layer */

  /* calculate the relevant distances */
  Real snowdepth = calc_snow_depth(soil);
  Real litterdepth = calc_litter_depth(soil);
  Real dist_top_gp_soilsurf = soildepth[0]/1000 / (GPLHEAT*2); /* top soil gridpoint to soil surface / soil litter interface distance */
  Real top_soil_gp_to_snow_surf_dist = dist_top_gp_soilsurf + litterdepth + snowdepth; /* full distance from top soil gp to snow surface */

  /* calculate the temperature of the top soillayer gridpoint */
  Real top_soil_gp_temp = ENTH2TEMP(soil->enth, therm_prop, 0);

  /* interpolate litter temp from top soil gp temp and airtemp */
  /* weight is used for the air temperature */
  Real weight_airtemp_litter = (dist_top_gp_soilsurf + litterdepth/2) / top_soil_gp_to_snow_surf_dist;
  Real litter_temp = top_soil_gp_temp * weight_airtemp_litter + airtemp * (1-weight_airtemp_litter);

  /* interpolate snowtemp from top soil gp temp and airtemp */
  /* weight is used for the air temperature */
  Real weight_airtemp_snow = (top_soil_gp_to_snow_surf_dist - snowdepth/2) / top_soil_gp_to_snow_surf_dist;
  Real snow_temp = top_soil_gp_temp * (1-weight_airtemp_snow) + airtemp * weight_airtemp_snow;

  /* assign interpolated temperatures */
  soil->litter.agtop_temp = litter_temp;
  soil->temp[NSOILLAYER] = snow_temp;
} /* of 'compute_litter_and_snow_temp_from_enth' */

/* Function to compute or update the maximum of depths until the first ice is reached */
STATIC void compute_maxthaw_depth(Soil *soil)
{
  Real depth_to_first_ice=0;
  int l;
  for (l=0; l<=BOTTOMLAYER; l++)
  {
    depth_to_first_ice+=soildepth[l];
    if(soil->freeze_depth[l]>epsilon)
      break;
  }
  depth_to_first_ice-=soil->freeze_depth[l];

  /* update the maxthaw_depth if new maximum is reached */
  if (soil->maxthaw_depth<depth_to_first_ice)
    soil->maxthaw_depth=depth_to_first_ice;
} /* of 'compute_maxthaw_depth' */


/******** small helper functions  ********/

STATIC Real calc_litter_depth(const Soil *soil)
{
  Real dm_sum = calc_litter_dm_sum(soil);
  return((dm_sum/1000)/DRY_BULK_DENSITY_LITTER); // (m)
}

STATIC Real calc_snow_depth(const Soil *soil)
{
  return(soil->snowpack*SNOWHEIGHT_PER_WATERHEIGHT*1e-3); // (m)
}

STATIC void compute_water_ice_ratios_from_enth(Soil *soil, const Soil_thermal_prop *therm_prop)
{
  Real freezefrac[NSOILLAYER];                         /* fraction of each layer that is frozen */
  enth2freezefrac(freezefrac, soil->enth, therm_prop); /* get frozen fraction of each layer */
  freezefrac2soil(soil, freezefrac);                   /* apply frozen fraction to all relevant soil variables */
}

STATIC void get_abs_waterice_cont(Real *abs_waterice_cont, const Soil *soil)
{
  int l;
  foreachsoillayer(l)
    abs_waterice_cont[l] = allice(soil, l) + allwater(soil, l);
}

/* The function evenly distributes grid points within each soil layer.
 * For a single grid point per layer (GPLHEAT=1), it's placed at the layer's midpoint.
 * With multiple grid points, they are arranged so that the distance from the border to the
 * nearest grid points is half the distance between adjacent grid points within the layer.
 * Note that the intervals betweens adjacent gridpoints are called elements. Thus, with this
 * grid arrangement, the elements can cross layer borders. */
STATIC void setup_heatgrid(Real *h /**< distances between adjacent gridpoints */
                          )
{
  int l,j;
  Real nodes[NHEATGRIDP];
  Real soillayer_depth_m=0; /* sollayerdepth in meter*/
  Real layer_border_m=0;    /* depth of current layer border in meter*/

  for(l=0; l<NSOILLAYER; ++l)
  {
    soillayer_depth_m= soildepth[l]/1000;
    for(j=0; j<GPLHEAT; ++j)
    {
      nodes[l*GPLHEAT+j]=layer_border_m+soillayer_depth_m/(GPLHEAT*2)+(soillayer_depth_m/GPLHEAT)*j;
    }
    layer_border_m+=soillayer_depth_m;
  }

  for(l=0; l<NSOILLAYER; ++l)
    for(j=0; j<GPLHEAT; ++j)
      h[l*GPLHEAT+j] = nodes[l*GPLHEAT+j] - (l*GPLHEAT+j>0?nodes[l*GPLHEAT+j-1]:0);
}


STATIC void get_unaccounted_changes_in_water_and_solids(Real *waterdiff, Real *soliddiff, const Real *abs_waterice_cont, const Soil *soil)
{
  int l;
  foreachsoillayer(l)
  {
    waterdiff[l] = (abs_waterice_cont[l] - soil->wi_abs_enth_adj[l]);
    soliddiff[l] = (soildepth[l]-soil->wsats[l])-soil->sol_abs_enth_adj[l];
  }
}

STATIC void update_wi_and_sol_enth_adjusted(const Real *waterdiff, const Real *soliddiff, Soil *soil)
{
  int l;
  foreachsoillayer(l)
  {
    /* set the bookkeeping variables to the currect soil contents,
     * as for all soil mass changes corresponding enthalpy adjustments have been made */
    soil->wi_abs_enth_adj[l]  = soil->wi_abs_enth_adj[l]  + waterdiff[l];
    soil->sol_abs_enth_adj[l] = soil->sol_abs_enth_adj[l] + soliddiff[l];
  }
}
