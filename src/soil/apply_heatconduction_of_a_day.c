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

void apply_heatconduction_of_a_day(
                    Real * enth,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real temp_top,   /*< temperature of the soil surface (dirichlet boundary) */
                    Soil_thermal_prop th   /*< thermal properties of soil */
                    /* thermal conductivity, heat capacity, latent heat */
                    ) 
{
  Real temp[N + 1];     /* temperature array (including surface gridpoint with index 0)*/
  /* since the surface gridpoint is included in temp but exluded in enth  */
  /* the kth gridpoint (gp) (when calling the surface gp the 0th gp) corresponds to temp[k] and enth[k-1]  */
  Real lam[N];          /* thermal conducitivity */
  Real QQ[N+1];         /* term resulting from the finite element method  */
  /* can be interpreted as the negative of the heatflux from gp j to gp j+1  */
  Real dt = 0.0;        /* timestep */
  int j; /* loop iteration */
  int timesteps;



  /*******precompute some values for better performance*******/
  Real h_inv[N];      /* inverse of gridpoint distances */
  Real inv_element_midpoint_dist[N]; /* inverse of the distances between the midpoints of adjacent elements */
  /* the last entry is the distance of the midpoint to the element boundary */
  for (j = 0; j < N; ++j)
  {
    h_inv[j] = 1/h[j]; 
    /* the following lines change the meaning of th.lam_(un)frozen, which no longer 
    contains thermal conductivities but the thermal conductivities divided by the respective elemt size */
    th.lam_frozen[j] = th.lam_frozen[j] * h_inv[j]; 
    th.lam_unfrozen[j] = th.lam_unfrozen[j] * h_inv[j];
    /* the following lines change the meaning of th.c_(un)frozen, which no longer 
    contains heat capacities but reciprocal heat capacities */
    th.c_frozen[j]= 1/ th.c_frozen[j] ;
    th.c_unfrozen[j]= 1/ th.c_unfrozen[j] ;

    inv_element_midpoint_dist[j] = 2/(h[j] + (j<N-1 ? h[j+1] : 0.0)); 
  }

  
  /*******calulate max possible timestep*******/
  /* This timestep is the maximum that still guarantees stability.
  It guarantees stability by ensuring that the ||.||_E norm of all matrices D(z)
  corresponding to the piecewise affine integrator map from current to next timestep 
  enth_c -> enth_n is smaller than 1 and hence the integrator map is a contraction. 
  The estimate for the matrix norm used here is abit stronger than the one derived
  in the thesis. (see equation (3.40) of thesis) */
  float dt_inv_temporary;
  float dt_inv=0;
  for (j=0; j<N; ++j) {
    dt_inv_temporary = max( (th.lam_unfrozen[j] + (j<N-1 ? th.lam_unfrozen[j+1]:0.0) )   *th.c_unfrozen[j],
                            (th.lam_frozen[j]   + (j<N-1 ? th.lam_frozen[j+1]:0.0) )     *th.c_frozen[j]    )
                               * inv_element_midpoint_dist[j];
    if (dt_inv < dt_inv_temporary)
      dt_inv = dt_inv_temporary;
  }
  timesteps= (int)(dayLength* dt_inv) +1;
  dt = dayLength/ timesteps; /* final timestep */



  /*******main timestepping loop*******/
  QQ[N]=0;            /* it is intended that the last value of QQ stays at zero during timestepping */
  temp[0] = temp_top; /* assign dirichlet boundary condition */
  int timestp;
  for(timestp=0; timestp<timesteps; ++timestp) { 
    /* calculate gridpoint temperatures from gridpoint enthalpies */
    for (j=0; j<N; ++j)
    {
      temp[j+1] = (enth[j] < 0                 ?  enth[j] *                      th.c_frozen[j]   : 0) + 
                  (enth[j] > th.latent_heat[j] ? (enth[j] - th.latent_heat[j]) * th.c_unfrozen[j] : 0); 
                  /* T=0 when 0<enth[j]<latent_heat */
      /* the term corresponds to eq (3.18) and (2.4) of the master thesis */
      /* it is the standart across-phase enthalpy temperature relation */
    }

    /* calculate heat fluxes from temperature difference */
    for (j=0; j<N; ++j) 
    {
      QQ[j] = -(temp[j+1] *(temp[j+1]  < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j]) - 
                temp[j]   *(temp[j]    < 0 ? th.lam_frozen[j] : th.lam_unfrozen[j])) ;
      /* the term corresponds to eq (3.15) of the master thesis multiplied with -1 */
      /* an interpretation is the heat flux between gp j+1 and j, see Fourriers law */
    }
    
    /* calculate and apply enthalpy update */
    for (j=0; j<N; ++j) 
    {
      enth[j] = enth[j]+ dt * (QQ[j] - QQ[j+1]) * inv_element_midpoint_dist[j];
      /* the term corresponds to eq (3.20) of the master thesis */
      /* it is the enthalpy update during the timestep */
    }
  }
}