/*
The function applies heat conduction that happens during the timespan of a day
to the enthalpy (i.e. thermal energy) vector, given thermal properties of the soil.
The temporal discretisation is forward euler and the spatial discretiasation 
is a finite element method and a dirichlet boundary condition.
Note that the function works with an enthalpy vector but a temperature boundary condtiion.
*/

#define dayLength 3600.0 * 24.0

#include "lpj.h"
#include <math.h>

void apply_heatconduction_of_a_day(Real * enth,             /*< enthalpy vector that the method is updating */
                    const int N,               /*< number of gridpoints with free values */
                    const Real * h,          /*< distances between gridpoints  */
                    const Real temp_top,     /*< temperature of the soil surface (dirichlet bound) */
                    Soil_thermal_prop th /*< thermal properties of soil */
                    ) 
{
  Real temp[N + 1], lam[N]; /* temperature and thermal conducitivity */
  Real FF[N+1];           /* fluxes  */
  Real dt = 0.0;            /* timestep */

  int j;
  int timesteps;

  /* precompute values for better performance */
  Real h_inv[N];
  Real multi_purp[N]; /* a variable with multiple purpouses  */

  for (j = 0; j < N; j++)
  {
    multi_purp[j] = 1/( h[j] + (j<N-1 ? h[j+1] : 0.0));
    h_inv[j] = 1/h[j]; 
    /* the following lines change the meaning of th.lam_(un)frozen, which no longer contains thermal conductivities but the thermal conductivities divided by the respective elemt size */
    th.lam_frozen[j] = th.lam_frozen[j] * h_inv[j]; 
    th.lam_unfrozen[j] = th.lam_unfrozen[j] * h_inv[j];
    /* the following lines change the meaning of th.c_(un)frozen, which no longer contains heat capacities but reciprocal heat capacities */
    th.c_frozen[j]= 1/ th.c_frozen[j] ;
    th.c_unfrozen[j]= 1/ th.c_unfrozen[j] ;
  }

  FF[N]=0;
  temp[0] = temp_top; // dirichlet boundary condition
  

  float dt_inv_temporary;
  float dt_inv=0;
  for (j=0; j<N; ++j) {
      //  dt_inv_temporary = max( th.lam_unfrozen[j]*c_unfrozen_inv[j] , th.lam_frozen[j]*c_frozen_inv[j])* h_inv[j]*h_inv[j]  * 2;
       dt_inv_temporary = max( (th.lam_unfrozen[j] + (j<N-1 ? th.lam_unfrozen[j+1]:0.0) )   *th.c_unfrozen[j],
                               (th.lam_frozen[j] + (j<N-1 ? th.lam_frozen[j+1]:0.0) )    *th.c_frozen[j]    )
                               * multi_purp[j]*2;
       if (dt_inv < dt_inv_temporary)
           dt_inv = dt_inv_temporary;
  }
  timesteps= (int)(dayLength* dt_inv) +1;
  dt = dayLength/ timesteps;

  for(j=0; j<N; ++j) {
    multi_purp[j] = multi_purp[j] * dt * 2;
  }
  /*******main timestepping loop*******/
  int timestp;
    
  for(timestp=0; timestp<timesteps; timestp++ ) { 

    /* calculate temperature distribution from enthalpy */
    for (j=0; j<N; ++j){
      temp[j+1] = (enth[j] < 0 ? enth[j] *th.c_frozen[j] : 0) + /* enthalpy-temperature relation */
                  (enth[j] > th.latent_heat[j] ? (enth[j] - th.latent_heat[j]) * th.c_unfrozen[j] : 0); /* T=0 when 0<enth[j]<latent_heat */
    }

    /* calculate fluxes from temperature difference */
    for (j=0; j<N; ++j) {
      // FF[j] = (temp[j+1] - temp[j]) * 
      //         ((temp[j+1] + temp[j]) < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]);

      FF[j] = (temp[j+1] *(temp[j+1]  < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]) - 
               temp[j]   *(temp[j] < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j])) ;
    }
    
    /* apply fluxes to enthalpy */
    for (j=0; j<N; ++j) {
      enth[j] = enth[j]+ (FF[j+1] - FF[j]) * multi_purp[j];
    }

  }
}