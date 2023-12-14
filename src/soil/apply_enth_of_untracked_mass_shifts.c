#include "lpj.h"

/*
Function to update enthalpy from incoming and outgoing water and massflow
A change in water content due to inflow or ouflow of water
normally goes in hand with a change of volumetric enthalpy of the soil,
since the inflowing or outflowing water normally carries some enthalpy.
This function updates the enthalpy vector based on changes in volumetric water content
and the assumption that the volumetric enthalpy of outgoing and incoming water
equals the volumetric enthalpy of the water that stays at the respective gridpoint.
The function can be used if one does not know the energy content of the outgoing or incoming water
or does not want the water flow to influence soil temperatures.
The function now also accounts for enthalpy changes due changes of solid components in the
same way as for the water.
*/


void apply_enth_of_untracked_mass_shifts(Real enth[],             /**< enthalpy vector that is updated*/
                                         const Real water_diff[], /**< vector with absolute change of volumetric water content per layer */
                                         const Real solid_diff[], /**< vector with absolute change of solid components per layer */
                                         const Real wi_abs_enth_adj[], /**< vector with enthalpy adjusted water ice content */
                                         const Real sol_abs_enth_adj[] /**< vector with enthalpy adjusted solid content */
                                        )
{
  int l,j;   /* l=layer, j is for sublayer */
  int gp;    /* gridpoint */
  Real gp_water_energy; /* gridpoint water energy */
  Real gp_solid_energy; /* gridpoint solid energy */
  Real rel_waterice_cont; /* relative water ice content */
  Real rel_solid_cont;    /* relative solid content */
  Real vol_latent_heat;   /* volumetric latent heat of soil (=latent heat of water * rel. waterice cont.) */
  Real c_quo_wat = c_mineral/c_water;
  Real c_quo_ice = c_mineral/c_ice;

  for(l=0;l<NSOILLAYER;++l)
  {
    rel_waterice_cont = wi_abs_enth_adj[l]/soildepth[l];
    rel_solid_cont = sol_abs_enth_adj[l]/soildepth[l];
    vol_latent_heat = rel_waterice_cont * c_water2ice;

    for(j=0;j<GPLHEAT;++j)
    {
      gp=GPLHEAT*l+j;
         /* get volumetric enthalpy of current water at the gridpoint */
      if(enth[gp] >= vol_latent_heat)
        /* the below formula is equivalent to
           gridpoint_water_energy = gridpoint_soil_temp * vol_heat_capacity_of_water + vol_latent_heat_of_water */
        gp_water_energy = (enth[gp] - vol_latent_heat) / (rel_waterice_cont + c_quo_wat * rel_solid_cont) + c_water2ice;
      else if (enth[gp] <= 0)
        /* the below formula is equivalent to
           gridpoint_ice_energy = gridpoint_soil_temp * vol_heat_capacity_of_ice */
        gp_water_energy =  enth[gp]                    / (rel_waterice_cont + c_quo_ice * rel_solid_cont);
      else /* the only option left is that enth is between 0 and total potential latent heat of soil */
        gp_water_energy =  enth[gp];

      /* get volumetric enthalpy of current solids at the gridpoint */
      /* total volumetric enthalpy is sum of */
      /* volumetric enthalpy of water times vol. water content */
      /* and volumetric enthalpy of solid components times vol. solid content */
      gp_solid_energy = (enth[gp] - gp_water_energy * rel_waterice_cont)/rel_solid_cont;

      /* add the energy from the inflowing or outflowing water
         assuming it has has the
         the same volumetric enthalpy as the water already there  */
      enth[gp]+=gp_water_energy*water_diff[l]/soildepth[l];

      /* add the energy from the incoming or outgoing*/
      /* solid components assuming they have the */
      /* same volumetric enthalpy as the solid components already there */
      enth[gp]+=gp_solid_energy*solid_diff[l]/soildepth[l];
    }
  }
} /* of 'apply_enth_of_untracked_mass_shifts' */
