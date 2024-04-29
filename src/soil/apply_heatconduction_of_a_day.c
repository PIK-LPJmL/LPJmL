/**************************************************************************************/
/**                                                                                \n**/
/**                    apply_heatconduction_of_a_day.c                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

/*

 * This function calculates and applies heat conduction fluxes over the span of a day 
 * to the enthalpy (i.e., thermal energy) vector.
 * The underlying model is the enthalpy formulation of the heat conduction PDE
 * d/dt enth = d/dx (lam d/dx temp),
 * in its weak form, where lam is the thermal coductivity.
 * It employs an adaptive numerical method for this, having different 
 * underlying submethods from which the most suitable is selected based on the data.
 * Submethod a) is chosen if soil temperatures and surface temperature
 * are uniformly above or below 0 degrees Celsius. It utilizes a computationally efficient 
 * standard heat conduction scheme based on finite element space and Crank-Nicolson time 
 * discretization, with a timestep of one day.
 * Submethod b) is selected when temperature signs are not uniform. 
 * This method applies an enthalpy scheme based on the enthalpy formulation using 
 * finite element space and explicit forward Euler time discretization, requiring shorter 
 * timesteps to maintain stability. The cheap method a) cannot be applied here due to 
 * nonlinearity in the temp <-> enth relation introduced by latent heat of fusion of water.
 * Both methods impose the surface temperature as a Dirichlet boundary condition.

*/

#include "lpj.h"
#include <math.h>

/* declare internally used functions */
STATIC void use_enth_scheme(Real *, const Real *, const Real, const Soil_thermal_prop *);
STATIC void use_temp_scheme_implicit(Real *, const Real *, const Real *, const Real *, int);
STATIC void arrange_matrix(Real *, Real *, Real *, const Real *, const Real *, const Real *, const Real);
STATIC void thomas_algorithm(const double *, const double *, const double *,const double *, double *);
STATIC void timestep_implicit(Real *, const Real *, const Real *, const Real *, const Real);

/******** main function *********/
void apply_heatconduction_of_a_day(Uniform_temp_sign uniform_temp_sign, /**< flag to indicate if the temperatures all have the same signs */
                                   Real * enth,                         /**< enthalpy vector that the method is updating (excluding gridpoint at surface) (J/m3) */
                                   const Real * h,                      /**< distances between adjacent gridpoints (m) */
                                   const Real temp_top,                 /**< temperature of the ground surface (GST), (dirichlet boundary condition) */
                                   const Soil_thermal_prop * th         /**< thermal properties of soil (thermal conductivity, heat capacity, latent heat) */
                                  )
{
  int j; /* gridpoint index */

  Real temp[NHEATGRIDP+1];     /* temperature array (including surface gridpoint with index 0)*/
  /* Note that temp[k] and enth[k-1] thus both correspond to the same kth gridpoint (gp) (starting from 0)*/

  /* take the decision if the standard and cheap heat conduction temperature method can be used or the more
     expensive enthalpy method needs to be applied, by evaluating uniform_temp_sign */
  if(uniform_temp_sign == ALL_ABOVE_0)
  {
    /* temperature scheme can be used with unfrozen soil props */

    /* get temperatures corresponding to the enthalpies */
    temp[0] = temp_top; /* assign dirichlet boundary condition */
    for (j=0; j<NHEATGRIDP; ++j)
      temp[j+1] = (enth[j]-th->latent_heat[j]) / th->c_unfrozen[j];

    /* update temperature */
    use_temp_scheme_implicit(temp, h, th->c_unfrozen, th->lam_unfrozen, -1);

    /* get back the enthalpies corresponding to the updated temperatures */
    for (j=0; j<NHEATGRIDP; ++j)
      enth[j] = temp[j+1] * th->c_unfrozen[j] + th->latent_heat[j];
  }
  else if(uniform_temp_sign == ALL_BELOW_0)
  {
    /* temperature scheme can be used with frozen soil props */

    /* get temperatures corresponding to the enthalpies */
    temp[0] = temp_top; /* assign dirichlet boundary condition */
    for (j=0; j<NHEATGRIDP; ++j)
      temp[j+1] = enth[j] / th->c_frozen[j];

    /* update temperature */
    use_temp_scheme_implicit(temp, h, th->c_frozen, th->lam_frozen, -1);

    /* get the enthalpies corresponding to the updated temperatures */
    for (j=0; j<NHEATGRIDP; ++j)
      enth[j] = temp[j+1] * th->c_frozen[j];
  }
  else if(uniform_temp_sign == MIXED_SIGN)
  {
    /* enthalpy scheme has to be used */
    use_enth_scheme(enth, h, temp_top, th);
  }
  else
  {
    fail(INVALID_TEMP_SIGN_ERR,FALSE,"uniform_temp_sign=%d is not one of the three possible values",uniform_temp_sign);
  }
} /* of 'apply_heatconduction_of_a_day' */


/******** underlying numerical methods ********/

/* The function applies the expensive explicit enthalpy scheme */
STATIC void use_enth_scheme(
                    Real * enth,          
                    const Real * h,       
                    const Real temp_top,  
                    const Soil_thermal_prop * th   
                    ) 
{
  Real temp[NHEATGRIDP + 1];     /* temperature array (including surface gridpoint with index 0) */
  /* since the surface gridpoint is included in temp but exluded in enth  */
  /* the kth gridpoint (gp) (when calling the surface gp the 0th gp) corresponds to temp[k] and enth[k-1]  */
  Real lam_fro_dBh[NHEATGRIDP], lam_unfro_dBh[NHEATGRIDP]; /* thermal conducitivity divided by gridpoint distances */ 
  Real inv_c_fro[NHEATGRIDP], inv_c_unfro[NHEATGRIDP];     /* inverses of frozen and unfrozen heat capacities */
  Real QQ[NHEATGRIDP+1];         /* term resulting from the finite element method  */
  /* QQ can be interpreted as the heatflux from gp j to gp j+1  */
  Real dt = 0.0;        /* timestep */


  int j; /* loop iteration */
  int timesteps;

  /* --- precompute some values for better performance --- */
  Real h_inv[NHEATGRIDP];      /* inverse of gridpoint distances */
  Real inv_element_midpoint_dist[NHEATGRIDP]; /* inverse of the distances between the midpoints of adjacent elements */
  /* the last entry is the distance of the midpoint to the element boundary */
  for (j = 0; j < NHEATGRIDP; ++j)
  {
    h_inv[j] = 1/h[j];
    /* thermal conductivities divided by the respective element size */
    lam_fro_dBh[j] = th->lam_frozen[j] * h_inv[j];
    lam_unfro_dBh[j] = th->lam_unfrozen[j] * h_inv[j];
    /* reciprocal heat capacities */
    inv_c_fro[j]= 1/th->c_frozen[j] ;
    inv_c_unfro[j]= 1/th->c_unfrozen[j] ;

    inv_element_midpoint_dist[j] = 2/(h[j] + (j<NHEATGRIDP-1?h[j+1]:0.0));
    /* it is intended that the last element of the vector is 2/h[NHEATGRIDP-1]
     * corresponding to the bottom left entry of M_h^-1, see eq (3.20)  */
  }

  /* --- calulate max possible timestep --- */
  /* 
   * This timestep is the maximum that still guarantees stability.
   * It guarantees stability by ensuring that the ||.||_E norm of all matrices D(z)
   * corresponding to the piecewise affine integrator map from current to next timestep
   * enth_c -> enth_n is smaller than 1 and hence the integrator map is a contraction.
   * The estimate for the matrix norm used here is derived from eq (3.43) of the thesis
   * (see also equation (3.40)) 
   */
  Real dt_inv_temporary;
  Real dt_inv=0;
  for (j=0; j<NHEATGRIDP; ++j)
  {
    dt_inv_temporary = max((lam_unfro_dBh[j] + (j<NHEATGRIDP-1 ? lam_unfro_dBh[j+1]:0.0)) * inv_c_unfro[j],
                           (lam_fro_dBh[j]   + (j<NHEATGRIDP-1 ? lam_fro_dBh[j+1]:0.0))   * inv_c_fro[j] )
                            * inv_element_midpoint_dist[j];
    if (dt_inv < dt_inv_temporary)
      dt_inv = dt_inv_temporary;
  }
#ifdef SAFE
  if(isnan(dt_inv))
    fail(INVALID_TIMESTEP_ERR, TRUE, "Invalid time step in %s", __FUNCTION__);
#endif
  timesteps = (int)(day2sec(dt_inv)) + 1; /* get number timesteps that ensures small enough timestep */
  dt = day2sec(1.0 / timesteps); /* final timestep */

  /* --- main timestepping loop --- */
  QQ[NHEATGRIDP]=0;   /* it is intended that the last value of QQ stays at zero during timestepping */
  temp[0] = temp_top; /* assign dirichlet boundary condition */
  int timestp;
  for(timestp=0; timestp<timesteps; ++timestp)
  {
    /* calculate gridpoint temperatures from gridpoint enthalpies */
    for (j=0; j<NHEATGRIDP; ++j)
    {
      temp[j+1] = (enth[j] < 0                  ?  enth[j]                       * inv_c_fro[j]   : 0) +
                  (enth[j] > th->latent_heat[j] ? (enth[j] - th->latent_heat[j]) * inv_c_unfro[j] : 0);
                  /* T=0 when 0<enth[j]<latent_heat */
      /* the term corresponds to eq (3.18) and (2.4) of the master thesis */
      /* it is the standart across-phase enthalpy temperature relation */
    }

    /* calculate heat fluxes from temperature difference */
    for (j=0; j<NHEATGRIDP; ++j)
    {
      QQ[j] = -(temp[j+1] *(temp[j+1]  < 0 ? lam_fro_dBh[j] : lam_unfro_dBh[j]) -
                temp[j]   *(temp[j]    < 0 ? lam_fro_dBh[j] : lam_unfro_dBh[j])) ;
      /* the term corresponds to eq (3.15) of the master thesis multiplied with -1 */
      /* an interpretation is the heat flux from gp j to j+1, see Fourriers law */
    }

    /* calculate and apply enthalpy update */
    for (j=0; j<NHEATGRIDP; ++j)
    {
      enth[j] = enth[j]+ dt * (QQ[j] - QQ[j+1]) * inv_element_midpoint_dist[j];
      /* the term corresponds to eq (3.20) of the master thesis */
      /* it is the enthalpy update during the timestep */
    }
  }
} /* of 'use_enth_scheme' */

/* The function applies the cheaper implicit enthalpy scheme */
STATIC void use_temp_scheme_implicit(Real * temp,
                                     const Real * h,
                                     const Real * hcap,
                                     const Real * lam,
                                     int steps
                                    )
{
  int i;
  /* determine number of timesteps to be performed for the day 
   * for non untit testing it is just 1 */
  if(steps == -1)
    #ifdef U_TEST
      steps = GPLHEAT; /* high res value */
    #else
      steps = 1; /* default value (on timestep per day) */
    #endif
  Real dt = day2sec(1.0/steps);

  /* do implicit timestepping */
  for (i = 0; i < steps; i++)
    timestep_implicit(temp, h, hcap, lam, dt);
} /* of 'use_temp_scheme_implicit' */



/* This function peforms a single implicit timestep,
 * for the temperature scheme.
 * A linear system of the form `Ax = rhs` (eq (6) 
 * of supplement) is solved for this.
 * The matrix A is tridiagonal and given by the sub main and
 * superdiagonal vectors. Please see the supplement of
 * the master thesis for the explanation. */
STATIC void timestep_implicit(Real * temp,
                              const Real * h,
                              const Real * hcap,
                              const Real * lam,
                              const Real dt
                             )
{
  Real sub[NHEATGRIDP];  /* sub diagonal elements */
  Real main[NHEATGRIDP]; /* main diagonal elements */
  Real sup[NHEATGRIDP];  /* super diagonal elements */

  /* --- arrange matrix --- */
  arrange_matrix(sub, main, sup, h, hcap, lam, dt);

  /* --- compute right-hand side ---  */
  /* See supplement of master thesis equation (6) */
  Real rhs[NHEATGRIDP];
  int j;
  /* matrix vector product Y * temp */
  rhs[0] =  temp[1] * (2-main[0]) - temp[2] * sup[0];
  for (j=1; j<(NHEATGRIDP-1); ++j)
    rhs[j] = temp[j+1] * (2-main[j]) - temp[j] * sub[j] - temp[j+2] * sup[j]; // equation (10-12)
  rhs[NHEATGRIDP-1] = temp[NHEATGRIDP] * (2-main[NHEATGRIDP-1]) - temp[NHEATGRIDP-1] * sub[NHEATGRIDP-1];
  /* add vector L */
  rhs[0] -= 2 * temp[0] * sub[0];

  /* --- solve tridiagonal system with the thomas algorithm --- */
  thomas_algorithm(sub, main, sup, rhs, &temp[1]);
} /* of 'timestep_implicit' */

/* This function arranges the matrix for the implicit timestep.
 * The equations can be found in the master thesis supplement equation (7-9). */
STATIC void arrange_matrix(Real * a,          /* sub diagonal elements  */
                           Real * b,          /* main diagonal elements */
                           Real * c,          /* super diagonal elements */
                           const Real * h,    /* distances between adjacent gridpoints  */
                           const Real * hcap, /* heat capacities */
                           const Real * lam , /* thermal conductivities */
                           const Real dt      /* timestep */
                          )
{

  /* --- precompute some values --- */
  Real lam_divBy_h[NHEATGRIDP];                        /* thermal conducitivity divided by gridpoint distances */
  Real inv_element_midpoint_dist_divBy_c[NHEATGRIDP];  /* inverse of the distances between the midpoints of adjacent elements divided by unfrozen heat capacity */
  lam_divBy_h[0] = lam[0] / h[0];
  int j;
  Real dt_half = dt/2;

  /* --- arrange all rows except last --- */
  for (j=0; j<(NHEATGRIDP-1); ++j)
  {
    /* loop over the rows of the matrix */
    lam_divBy_h[j+1] = lam[j+1] / h[j+1];
    inv_element_midpoint_dist_divBy_c[j] = 2/(h[j] +  h[j+1])/hcap[j];
    a[j] = - lam_divBy_h[j] * inv_element_midpoint_dist_divBy_c[j] * dt_half ; /* equation (8) */
    c[j] = - lam_divBy_h[j+1] * inv_element_midpoint_dist_divBy_c[j] * dt_half ; /* equation (7) */
    b[j] = 1 - a[j] - c[j]; /* equation (9) */
  }

  /* --- arrange last row --- */
  inv_element_midpoint_dist_divBy_c[NHEATGRIDP-1] = (2/h[NHEATGRIDP-1])/hcap[NHEATGRIDP-1];
  a[NHEATGRIDP-1] = - lam_divBy_h[NHEATGRIDP-1] * inv_element_midpoint_dist_divBy_c[NHEATGRIDP-1] * dt_half;
  b[NHEATGRIDP-1] = 1 - a[NHEATGRIDP-1];
  c[NHEATGRIDP-1] = 0;
} /* of 'arrange_matrix' */

/* This function performs the standard thomas algorithm to solve a
   tridiagonal matrix system. */
STATIC void thomas_algorithm(const double *a, /* sub diagonal elements */
                             const double *b, /* main diagonal elements */
                             const double *c, /* super diagonal elements */
                             const double *d, /* right hand side */
                             double *x        /* solution */
                            )
{
  double c_prime[NHEATGRIDP-1];
  double d_prime[NHEATGRIDP];
  int i;

  /* modify coefficients by progressing in forward direction */
  /* this codes eliminiates the sub diagnal a an norms the diagonal b 1 */
  c_prime[0] = c[0] / b[0];
  for (i=1; i<NHEATGRIDP-1; i++)
    c_prime[i] = c[i] / (b[i] - a[i] * c_prime[i-1]);

  d_prime[0] = d[0] / b[0];
  for (i=1; i<NHEATGRIDP; i++)
    d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) / (b[i] - a[i] * c_prime[i-1]);

  /* back substitution */
  x[NHEATGRIDP-1] = d_prime[NHEATGRIDP-1];
  for (i = NHEATGRIDP-2; i>=0; i--)
    x[i] = d_prime[i] - c_prime[i] * x[i+1];
} /* of 'thomas_algorithm' */
