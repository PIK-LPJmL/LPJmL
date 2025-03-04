/**************************************************************************************/
/**                                                                                \n**/
/**                   finite_volume_diffusion.c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

/* source of method : alexiades, mathematical modeling of melting and freezing processes p.181 ff.
 *
 * solves the PDE: 
 * por(x) * d/dt g(x,t) = d/dx (D(x) d/dx g(x,t)) 
 * with boundary conditions:
 * g(0,t) = g_air
 * d/dx g(L,t) = 0
 *
 * g(x,t) is the gas concentration of the substance in the soil
 * por(x) is the total methane/O2 porosity of the soil
 * D(x) is the diffusivity of the substance in the soil
 *
 * The equation is equivalent to the heat equation:
 * c(x) * d/dt T(x,t) = d/dx (lam(x) d/dx T(x,t))
 *
 * The update is made directly to the absolute amount of the substance in the layer i amount_i:
 * g_i(t) = amount_i(t) / (por_i * h_i)
 * See Khovorostyanov 2008 Vulnerability of permafrost ... p. 254
*/

#define MAXTIMESTEPFV 10000000

void finite_volume_diffusion_timestep(Real * amount,          /**< g/m^2, substance absolute amount */
                                      const int n,            /**< number of gridpoints */
                                      const Real dt,          /**< s, timestep */
                                      const Real * h,         /**< m, layer thicknesses (delta_x) */
                                      const Real gas_con_air, /**< g/m^2, gas concentration of substance */
                                      const Real * res,       /**< resistance */
                                      const Real * porosity   /**< m^3/m^3, porosity */
                                     )
{
  /* define variables */
  Real gas_con[n+1]; /* g/m^3, gas concentration of substance */
  Real flux[n+1];      /* g/s/m^2, flux */

  /* calculate the substance gas concentration */
  gas_con[0] = gas_con_air;
  int j;
  for (j=0; j<n; ++j)
    gas_con[j+1] = (amount[j] / h[j]) / porosity[j];

  /* calculate fluxes */
  for (j=0; j<n; ++j)
    flux[j] = - (gas_con[j+1] - gas_con[j]) / res[j]; /* equation (18) p. 189 */
  flux[n] = 0; /* no flux at the bottom */

  /* update the substance amount */
  for (j=0; j<n; ++j)
    /* the below equation is equation (13) p. 187 multiplied with h[j]
     * the second derivative is visible when the below equation is divided by h[j]
     */
    amount[j] += dt * (flux[j] - flux[j+1]); 
} /* of 'finite_volume_diffusion_timestep' */

void calculate_resistances(Real * res, const Real * h, const Real * diff, const int n)
{
  res[0] = (h[0]/2)/diff[0]; /* equation (25) p. 191 */
  int j;
  for (j=1; j<n; ++j)
    res[j] = (h[j-1]/2)/diff[j-1] + (h[j]/2)/diff[j]; /* equation (17) p. 189 */
} /* of 'calculate_resistances' */

Bool apply_finite_volume_diffusion_of_a_day(Real * amount,         /**< g/m^2, substance absolute amount */
                                           const int n,            /**< number of gridpoints */
                                           const Real * h,         /**< m, layer thicknesses (delta_x) */
                                           const Real gas_con_air, /**< g/m^2, gas concentration of substance */
                                           const Real * diff,      /**< m^2/s, diffusivity */
                                           const Real * porosity   /**< m^3/m^3, porosity */
                                          )                        /** \return TRUE on error */
{
  /* calculate resistances */
  Real res[n];
  calculate_resistances(res, h, diff, n);
  
  /* get max possible timestep that gurantess stability */
  Real dt_temp;
  /* get possible timestep for top boundary */
  Real alpha = diff[0]/porosity[0]; /* rho * c = volumetric heat capacity <-> porosity */
  dt_temp = h[0]*h[0]/(alpha*3)*0.1; /* equation (40a) p. 196 */
  for (int j=1; j<n; ++j)
  {
    dt_temp = min(dt_temp, (porosity[j]*h[j])/((1/res[j-1]+1/res[j]))*0.1); /* equation (39) p. 196*/
  }
  int steps = (int)(day2sec(1.0)/dt_temp) + 1; /* get number timesteps that ensures small enough timestep */
  if (steps <= 0 || steps > MAXTIMESTEPFV)
    return TRUE; /* timestep too large or too small */
  Real dt = day2sec(1.0)/steps; /* final timestep */

  /* apply timesteps */
  for (int i=0; i<steps; ++i)
    finite_volume_diffusion_timestep(amount, n, dt, h, gas_con_air, res, porosity);

  return FALSE;
} /* of 'apply_finite_volume_diffusion_of_a_day' */



/* This function arranges the matrix for the implicit timestep.
 * The equations can be found in the master thesis supplement equation (7-9). */
STATIC void arrange_matrix_fv(Real * a,          /* sub diagonal elements  */
                              Real * b,          /* main diagonal elements */
                              Real * c,          /* super diagonal elements */
                              const Real * h,    /* distances between adjacent gridpoints  */
                              const Real * por,  /* porosities */
                              const Real * res,  /* resitances */
                              const Real dt,     /* timestep */
                              const int n        /* number of gridpoints */
                             )
{

  /* --- precompute some values --- */

  /* --- arrange all rows except last --- */
  int j;
  for (j=0; j<(n-1); ++j)
  {
    /* loop over the rows of the matrix */
    a[j] = - 1/res[j] * 1/(por[j] * h[j]) * dt;
    c[j] = - 1/res[j+1] * 1/(por[j] * h[j]) * dt;
    b[j] = 1 - a[j] - c[j];
  }

  /* --- arrange last row --- */
  a[n-1] = - 1/res[n-1] * 1/(por[n-1] * h[n-1]) * dt;
  c[n-1] = 0;
  b[n-1] = 1 - a[n-1];

} /* of 'arrange_matrix' */

Bool apply_finite_volume_diffusion_impl(Real * amount,          /**< g/m^2, substance absolute amount */
                                        const int n,            /**< number of gridpoints */
                                        const Real * h,         /**< m, layer thicknesses (delta_x) */
                                        const Real gas_con_air, /**< g/m^2, gas concentration of substance */
                                        const Real * diff,      /**< m^2/s, diffusivity */
                                        const Real * porosity,  /**< m^3/m^3, porosity */
                                        const Real dt           /**< timestep */
                                       )                        /** \return TRUE on error */
{
  /* calculate resistances */
  Real res[n];
  calculate_resistances(res, h, diff, n);
  Real gas_con_new[n]; 
  Real gas_con_current[n];

  /* get current gas concentration */
  int j;
  for (j=0; j<n; ++j)
    gas_con_current[j] = (amount[j] / h[j]) / porosity[j];
  
  Real a[n], b[n], c[n]; /* matrix diagonals */
  Real rhs[n] = {}; /* right hand side of the equation */
  arrange_matrix_fv(a, b, c, h, porosity, res, dt, n);
  rhs[0] = dt * gas_con_air/(porosity[0] * h[0]) / res[0];

  /* add gas con current to rhs */
  for (j=0; j<n; ++j)
    rhs[j] += gas_con_current[j];

  /* Solve the equation A x = rhs, for x */
  thomas_algorithm(a, b, c, rhs, gas_con_new, n);

  /* get back amounts */
  for (j=0; j<n; ++j)
    amount[j] = gas_con_new[j] * porosity[j] * h[j];

  return FALSE;
} /* of 'apply_finite_volume_diffusion_of_a_day' */



/* This function arranges the matrix for the implicit timestep.
 * The equations can be found in the master thesis supplement equation (7-9). */
STATIC void arrange_matrix_fv_crank_nicolson(Real * a,          /* sub diagonal elements  */
                                              Real * b,         /* main diagonal elements */
                                              Real * c,         /* super diagonal elements */
                                              const Real * h,   /* distances between adjacent gridpoints  */
                                              const Real * por, /* porosities */
                                              const Real * res, /* resitances */
                                              const Real dt,    /* timestep */
                                              const int n       /* number of gridpoints */
                                             )
{

  /* --- precompute some values --- */

  /* --- arrange all rows except last --- */
  int j;
  for (j=0; j<(n-1); ++j)
  {
    /* loop over the rows of the matrix */
    a[j] = - 1/res[j] * 1/(por[j] * h[j]) * dt * 0.5;
    c[j] = - 1/res[j+1] * 1/(por[j] * h[j]) * dt * 0.5;
    b[j] = 1 - a[j] - c[j];
  }

  /* --- arrange last row --- */
  a[n-1] = - 1/res[n-1] * 1/(por[n-1] * h[n-1]) * dt * 0.5;
  c[n-1] = 0;
  b[n-1] = 1 - a[n-1];

} /* of 'arrange_matrix' */

Bool apply_finite_volume_diffusion_impl_crank_nicolson(Real * amount,          /**< g/m^2, substance absolute amount */
                                                       const int n,            /**< number of gridpoints */
                                                       const Real * h,         /**< m, layer thicknesses (delta_x) */
                                                       const Real gas_con_air, /**< g/m^2, gas concentration of substance */
                                                       const Real * diff,      /**< m^2/s, diffusivity */
                                                       const Real * porosity,  /**< m^3/m^3, porosity */
                                                       const Real dt           /**< timestep */
                                                      )                        /** \return TRUE on error */
{
  /* calculate resistances */
  Real res[n];
  calculate_resistances(res, h, diff, n);
  Real gas_con_new[n]; 
  Real gas_con_current[n];

  /* get current gas condcentration */
  int j;
  for (j=0; j<n; ++j)
    gas_con_current[j] = (amount[j] / h[j]) / porosity[j];
  
  Real a[n], b[n], c[n]; /* matrix diagonals */
  Real rhs[n] = {}; /* right hand side of the equation */
  arrange_matrix_fv_crank_nicolson(a, b, c, h, porosity, res, dt, n);
  rhs[0] = dt * gas_con_air/(porosity[0] * h[0]) / res[0]; /* incorporates Tair of next and current time */

  /* add gas con current to rhs */
  for (j=0; j<n; ++j)
    rhs[j] += gas_con_current[j];

  /* add fluxes from current timesteps to rhs */
  rhs[0] += 0.5 * dt / (h[0] * porosity[0]) * ( -1 * gas_con_current[0] * (1/res[0] + 1/res[1]) + 
                                                gas_con_current[1] / res[1] );
  for (j=1; j<n-1; ++j)
    rhs[j] += 0.5 * dt / (h[j] * porosity[j]) * ( gas_con_current[j-1] / res[j] - 
                                                  gas_con_current[j] * (1/res[j] + 1/res[j+1]) + 
                                                  gas_con_current[j+1] / res[j+1] );

  rhs[n-1] += 0.5 * dt / (h[n-1] * porosity[n-1]) * ( gas_con_current[n-2] / res[n-1] - 
                                                      gas_con_current[n-1] / res[n-1]);

  /* Solve the equation A x = rhs, for x */
  thomas_algorithm(a, b, c, rhs, gas_con_new, n);

  /* get back amounts */
  for (j=0; j<n; ++j)
    amount[j] = gas_con_new[j] * porosity[j] * h[j];

  return FALSE;
} /* of 'apply_finite_volume_diffusion_of_a_day' */
