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

STATIC void setup_heatgrid(Real *);
STATIC void setup_heatgrid_layer_boundaries(Real *);
STATIC void get_unaccounted_changes_in_water_and_solids(Real *, Real *, Real *, Soil *);
STATIC void update_wi_and_sol_enth_adjusted(Real *, Real *, Soil *);
STATIC void modify_enth_due_to_masschanges(Soil *, Real *, const Config *);
STATIC void modify_enth_due_to_heatconduction(enum uniform_temp_sign, Soil *, Real, Soil_thermal_prop *,const Config *);
STATIC void compute_litter_temp_from_enth(Soil * soil, Real temp_below_snow, const Soil_thermal_prop * therm_prop);
STATIC void compute_water_ice_ratios_from_enth(Soil *, const Soil_thermal_prop *);
STATIC void calc_gp_temps(Real * gp_temps, Real * enth, const  Soil_thermal_prop *);
STATIC void compute_maxthaw_depth(Soil * soil);
STATIC void compute_bottom_bound_layer_temps_from_enth(Real *,  const Real *,const Soil_thermal_prop *);
STATIC enum uniform_temp_sign check_uniform_temp_sign_throughout_soil(Real *, Real, Real *);
STATIC void get_abs_waterice_cont(Real *, Soil *);


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
  enum uniform_temp_sign uniform_temp_sign;
  uniform_temp_sign = check_uniform_temp_sign_throughout_soil(soil->enth, temp_below_snow, abs_waterice_cont);

  /* calculate soil thermal properties and provide it for below functions */
  Soil_thermal_prop therm_prop = {};
  calc_soil_thermal_props(uniform_temp_sign, &therm_prop, soil, abs_waterice_cont,  NULL, config->johansen, TRUE);

  /* apply daily changes to soil enthalpy distribution due to heatconvection and heatconduction*/
  modify_enth_due_to_masschanges(soil, abs_waterice_cont, config);
  modify_enth_due_to_heatconduction(uniform_temp_sign, soil, temp_below_snow, &therm_prop, config);

  /* compute soil thermal attributes from enthalpy distribution and thermal properties, i.e. the derived quantities */
  compute_mean_layer_temps_from_enth(soil->temp,soil->enth, &therm_prop);
  compute_litter_temp_from_enth(soil, temp_below_snow, &therm_prop);
  if(uniform_temp_sign == MIXED_SIGN || uniform_temp_sign == UNKNOWN)
  {
    compute_water_ice_ratios_from_enth(soil,&therm_prop);
    compute_maxthaw_depth(soil);
  }
} 


/**** functions used by update_soil_thermal_state ****/

/* Function checks if forcing as well as all temperatures in the soil have the same sign.
   This simplifies calculations as no phase changes need to be considered. */
STATIC enum uniform_temp_sign check_uniform_temp_sign_throughout_soil(Real * enth, Real temp_top, Real * abs_waterice_cont){
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
  case -1: return ALL_BELOW_0; break;
  case  0: return MIXED_SIGN; break;
  case  1: return ALL_ABOVE_0; break;
  }
  return UNKNOWN;
}

STATIC void modify_enth_due_to_masschanges(Soil * soil, Real * abs_waterice_cont, const Config * config)
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

STATIC void adjust_for_snow(Real h[], Soil_thermal_prop * therm_prop, Soil * soil){
  Real snowdepth = soil->snowpack*4*1e-3;

  Real froz_thermal_resistance = (h[0]/therm_prop->lam_frozen[0]) + (snowdepth/0.5 );
  Real froz_therm_cond = (h[0]+snowdepth) / froz_thermal_resistance;
  therm_prop->lam_frozen[0] = froz_therm_cond;

  //printf("frozen lam at 0: %f, frozen lam at 1: %f\n", therm_prop->lam_frozen[0], therm_prop->lam_frozen[1]);
  //printf("h0 %f, snowdepth %f\n", h[0], snowdepth);

  Real unfroz_thermal_resistance = (h[0]/therm_prop->lam_unfrozen[0]) + (snowdepth/0.5 );
  Real unfroz_therm_cond = (h[0]+snowdepth) / unfroz_thermal_resistance;
  therm_prop->lam_unfrozen[0] = unfroz_therm_cond;

  h[0]+=snowdepth;
}

STATIC void modify_enth_due_to_heatconduction(enum uniform_temp_sign uniform_temp_sign, Soil * soil, Real temp_below_snow, Soil_thermal_prop * therm_prop ,const Config * config)
{
  Real litter_agtop_temp;
  Real h[NHEATGRIDP], top_dirichlet_BC;

  /* Compute the dirichlet boundary condition.
     The below snow temp is mixed with the upmost gp temp to get 
     litter temp, which is then weighted according to agtop_cover. */
  litter_agtop_temp = (temp_below_snow + ENTH2TEMP(soil->enth,therm_prop,0))/2;
  top_dirichlet_BC = temp_below_snow  * (1 - soil->litter.agtop_cover) +
         litter_agtop_temp * soil->litter.agtop_cover;

  /* setup grid */
  setup_heatgrid(h);

  /* modify grid and conductivity of top layer if snow is present */
  adjust_for_snow(h, therm_prop, soil);

  /* apply heatconduction */
  apply_heatconduction_of_a_day(uniform_temp_sign, soil->enth, h, top_dirichlet_BC, therm_prop ); 
}

STATIC void compute_water_ice_ratios_from_enth(Soil * soil, const Soil_thermal_prop *therm_prop)
{
  Real freezefrac[NSOILLAYER];                         /* fraction of each layer that is frozen */
  enth2freezefrac(freezefrac, soil->enth, therm_prop); /* get frozen fraction of each layer */
  freezefrac2soil(soil, freezefrac);                   /* apply frozen fraction to soil variables */
}

STATIC void compute_litter_temp_from_enth(Soil * soil, Real temp_below_snow , const Soil_thermal_prop *therm_prop)
{
  soil->litter.agtop_temp = (temp_below_snow + ENTH2TEMP(soil->enth,therm_prop,0)) / 2;
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
  Real soillayer_depth_m=0;
  Real layer_border_m=0;

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