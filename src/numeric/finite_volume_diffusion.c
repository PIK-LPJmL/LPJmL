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

#define MAXTIMESTEPFV 1000000

void finite_volume_diffusion_timestep(Real * amount,           /* g/m^2, substance absolute amount */
                                      const int n,               /* number of gridpoints */
                                      const Real dt,             /* s, timestep */
                                      const Real * h,            /* m, layer thicknesses (delta_x) */
                                      const Real gas_con_air,    /* g/m^2, gas concentration of substance */
                                      const Real * res,          /* resistance */
                                      const Real * porosity      /* m^3/m^3, porosity */
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

Bool apply_finite_volume_diffusion_of_a_day(Real * amount,             /* g/m^2, substance absolute amount */
                                           const int n,               /* number of gridpoints */
                                           const Real * h,            /* m, layer thicknesses (delta_x) */
                                           const Real gas_con_air,    /* g/m^2, gas concentration of substance */
                                           const Real * diff,         /* m^2/s, diffusivity */
                                           const Real * porosity      /* m^3/m^3, porosity */
                                           )
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
