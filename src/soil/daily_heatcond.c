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


void daily_heatcond(double * enth,             /*< enthalpy vector that the method is updating */
                    const int N,               /*< number of gridpoints with free values */
                    const double * h,          /*< distances between gridpoints  */
                    const double temp_top,     /*< temperature of the soil surface (dirichlet bound) */
                    const Soil_thermal_prop th /*< thermal properties of soil */
                    ) 
{
  int steps = 0;              /* number of timesteps */
  int steps_temp = 0;         /* temp number of timesteps  */
  double temp[N + 1], lam[N]; /* temperature and thermal conducitivity */
  double upper_flux = 0.0;    /* heat fluxes for each gridpoint */
  double lower_flux = 0.0;
  double weight = 0.0;        /* weight for conductivity of partly frozen elements */
  double FF = 0.0;            /* enthalpy update */
  double dt = 0.0;            /* timestep */
  double dt_temp = 0.0; 
  double t_cur = 0.0;         /* curent time */
  int i, j;

  /* initialise thermal conductivity and temperature */
  temp[0] = temp_top; // dirichlet boundary condition
  for (j=0; j < N; ++j) {
    lam[j] = (enth[j] < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]);
    temp[j + 1] = 0;
  }

  /*******main timestepping loop*******/
  while (t_cur<(dayLength-epsilon)) { 

    /* calculate temperature distribution from enthalpy */
    for (j=0; j<N; ++j){
      temp[j+1] = (enth[j] < 0 ? enth[j] / th.c_frozen[j] : 0) + /* enthalpy-temperature relation */
                  (enth[j] > th.latent_heat[j] ?
                    (enth[j] - th.latent_heat[j]) / th.c_unfrozen[j] : 0
                  ); /* T=0 when 0<enth[j]<latent_heat */
    }

    /* get thermal conductivities */
    for (j=0; j<N; ++j) { /* this could be optimised further by only updating lam when needed. */
      if (temp[j] * temp[j+1] >= 0)
        lam[j] = ((temp[j] + temp[j+1]) / 2 < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]);
      else {
        weight = temp[j] / (temp[j] - temp[j + 1]);
        lam[j] = weight     * (temp[j]  <0 ? th.lam_frozen[j] : th.lam_unfrozen[j]) +
                 (1-weight) * (temp[j+1]<0 ? th.lam_frozen[j] : th.lam_unfrozen[j]);
      }
    }

    /* calculate maximum tiimestep possible */
    dt=INFINITY;
    for (j=0; j<N; ++j) {
        dt_temp = (enth[j] > 0 ? th.c_unfrozen[j] : th.c_frozen[j]) / lam[j] * (h[j] * h[j]) / 2;
        if (dt > dt_temp)
            dt = dt_temp;
    }
    if( t_cur +dt > dayLength) 
        dt = dayLength - t_cur; /* if the timestep exceed the timespan of a day */
    t_cur+=dt;

    /* calculate energy update from temperature differnces */
    for (j=0; j<N; ++j) {
      lower_flux = (j<N-1 ? (temp[j+2] - temp[j+1]) * lam[j+1] / h[j+1] : 0);
      upper_flux =          (temp[j+1] - temp[j])   * lam[j]   / h[j]       ;
      FF = (lower_flux - upper_flux) / (h[j] + (j<N-1 ? h[j+1] : 0.0)) * 2;
      enth[j] = enth[j] + dt * FF;
    }
    
  }
}