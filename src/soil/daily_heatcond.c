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



void daily_heatcond(Real * enth,             /*< enthalpy vector that the method is updating */
                    const int N,               /*< number of gridpoints with free values */
                    const Real * h,          /*< distances between gridpoints  */
                    const Real temp_top,     /*< temperature of the soil surface (dirichlet bound) */
                    const Soil_thermal_prop th /*< thermal properties of soil */
                    ) 
{
  Real temp[N + 1], lam[N]; /* temperature and thermal conducitivity */
  Real FF[N+1];           /* fluxes  */
  Real dt = 0.0;            /* timestep */
  int j;
  int timesteps;

  /* precompute reciprocal values for better performance */
  Real h_inv[N];
  Real hph_inv[N];
  Real c_frozen_inv[N], c_unfrozen_inv[N];
  for (j = 0; j < N; j++)
  {
    h_inv[j] = 1/h[j]; 
    hph_inv[j] = 1/( h[j] + (j<N-1 ? h[j+1] : 0.0));
    c_frozen_inv[j]= 1/ th.c_frozen[j] ;
    c_unfrozen_inv[j]= 1/ th.c_unfrozen[j] ;
  }

  FF[N]=0;
  temp[0] = temp_top; // dirichlet boundary condition
  

  float dt_inv_temporary;
  float dt_inv=0;
  for (j=0; j<N; ++j) {
       dt_inv_temporary = max( th.lam_unfrozen[j]*c_unfrozen_inv[j] , th.lam_frozen[j]*c_frozen_inv[j])* h_inv[j]*h_inv[j]  * 2;
       if (dt_inv < dt_inv_temporary)
           dt_inv = dt_inv_temporary;
  }
  timesteps= (int)(dayLength* dt_inv) +1;
  dt = dayLength/ timesteps;

  /*******main timestepping loop*******/
  int timestp;
    
  for(timestp=0; timestp<timesteps; timestp++ ) { 

    /* calculate temperature distribution from enthalpy */
    for (j=0; j<N; ++j){
      temp[j+1] = (enth[j] < 0 ? enth[j] *c_frozen_inv[j] : 0) + /* enthalpy-temperature relation */
        (enth[j] > th.latent_heat[j] ? (enth[j] - th.latent_heat[j]) * c_unfrozen_inv[j] : 0); /* T=0 when 0<enth[j]<latent_heat */
    }

    /* calculate energy update from temperature differnces */
    for (j=0; j<N; ++j) {
      FF[j] = (temp[j+1] - temp[j]) * 
              ((temp[j+1] + temp[j]) < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]) * h_inv[j];
    }
    // for (j=0; j<N; ++j) {
    //   FF[j] = (temp[j+1] *(temp[j+1]  < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]) - 
    //            temp[j] *(temp[j] < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j])) 
    //            * h_inv[j];
    // }

    for (j=0; j<N; ++j) {
      enth[j] = enth[j]+ dt * (FF[j+1] - FF[j]) * hph_inv[j] * 2;
    }

  }
}