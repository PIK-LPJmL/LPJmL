/*
The function applies heat conduction that happens during the timespan of a day
to the enthalpy (i.e. thermal energy) vector, given thermal properties of the soil.
The temporal discretisation is forward euler and the spatial discretiasation 
is a finite element method and a dirichlet boundary condition.
Note that the function works with an enthalpy vector but a temperature boundary condtiion.
*/

#define dayLength 86400.0 /*3600.0 * 24.0*/

#include "lpj.h"
#include <math.h>

static void use_enth_scheme(Real *, const int, const Real *, const Real, Soil_thermal_prop *);
static void use_temp_scheme(Real *, const int, const Real *, const Real *, const Real *);
static void use_scheme_with_temp_below_zegro_deg(Real *, const int, const Real *, const Real, Soil_thermal_prop *);
static enum uniform_temp_sign check_uniform_temp_sign_throughout_soil(Real *, Real, Soil_thermal_prop *, int);
enum uniform_temp_sign {ALL_BELOW_0, MIXED_SIGN, ALL_ABOVE_0};

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
  int j; /* gridpoint index */

  Real temp[N+1];     /* temperature array (including surface gridpoint with index 0)*/
  /* since the surface gridpoint is included in temp but exluded in enth  */
  /* the kth gridpoint (gp) (when calling the surface gp the 0th gp) corresponds to temp[k] and enth[k-1]  */

  /* to choose if the standard and cheap heat conduction temperature methad can be used or the more expensive enthalpy method needs to be applied
     check if the temperature forcing and temperatures in the soil column show any sign changes or are all negative/ all positive */
  enum uniform_temp_sign uniform_temp_sign = check_uniform_temp_sign_throughout_soil(enth, temp_top, &th, N);

  if(uniform_temp_sign == ALL_ABOVE_0)
  {
    /* temperature scheme can be used */

    /* get the temperatures corresponding to the enthalpy */
    temp[0] = temp_top; /* assign dirichlet boundary condition */
    for (j=0; j<N; ++j)
      temp[j+1] = (enth[j] - th.latent_heat[j]) / th.c_unfrozen[j];
    
    /* update temperature */
    use_temp_scheme(temp, N, h, th.c_unfrozen, th.lam_unfrozen);

    /* get the enthalpies corresponding to the updated temperatures */
    for (j=0; j<N; ++j)
      enth[j] = temp[j+1] * th.c_unfrozen[j] + th.latent_heat[j];
  } else if(uniform_temp_sign == ALL_BELOW_0){
    /* temperature scheme can be used */

    /* get the temperatures corresponding to the enthalpy */
    temp[0] = temp_top; /* assign dirichlet boundary condition */
    for (j=0; j<N; ++j)
      temp[j+1] = enth[j] / th.c_frozen[j];

    /* update temperature */
    use_temp_scheme(temp, N, h, th.c_frozen, th.lam_frozen);

    /* get the enthalpies corresponding to the updated temperatures */
    for (j=0; j<N; ++j)
      enth[j] = temp[j+1] * th.c_frozen[j];
  } else if(uniform_temp_sign == MIXED_SIGN){
    /* enthalpy scheme has to be used */
    use_enth_scheme(enth, N, h, temp_top, &th);
  } else
  {
    printf("Error: uniform_temp_sign is not one of the three possible values. \n");
    exit(-1);
  }    
    
}

static enum uniform_temp_sign check_uniform_temp_sign_throughout_soil(Real * enth, Real temp_top, Soil_thermal_prop * th, int N){
  int j;
  int temp_sign;
  int temp_sign_top = (temp_top < 0 ? -1 : 0) + 
                      (temp_top > 0 ?  1 : 0);
  for (j=0; j<N; ++j)
  {
    temp_sign = (enth[j] < 0                  ? -1 : 0) + 
                (enth[j] > th->latent_heat[j] ?  1 : 0); 
    if (temp_sign != temp_sign_top)
      return MIXED_SIGN;
  }
  // switch case
  switch (temp_sign_top)
  {
  case -1: return ALL_BELOW_0; break;
  case  0: return MIXED_SIGN; break;
  case  1: return ALL_ABOVE_0; break;
  }
}

static void use_enth_scheme(
                    Real * enth,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real temp_top,   /*< temperature of the soil surface (dirichlet boundary) */
                    Soil_thermal_prop *th   /*< thermal properties of soil */
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
    /* the following lines change the meaning of th->lam_(un)frozen, which no longer 
    contains thermal conductivities but the thermal conductivities divided by the respective elemt size */
    th->lam_frozen[j] = th->lam_frozen[j] * h_inv[j]; 
    th->lam_unfrozen[j] = th->lam_unfrozen[j] * h_inv[j];
    /* the following lines change the meaning of th->c_(un)frozen, which no longer 
    contains heat capacities but reciprocal heat capacities */
    th->c_frozen[j]= 1/ th->c_frozen[j] ;
    th->c_unfrozen[j]= 1/ th->c_unfrozen[j] ;

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
    dt_inv_temporary = max( (th->lam_unfrozen[j] + (j<N-1 ? th->lam_unfrozen[j+1]:0.0) )   *th->c_unfrozen[j],
                            (th->lam_frozen[j]   + (j<N-1 ? th->lam_frozen[j+1]:0.0) )     *th->c_frozen[j]    )
                               * inv_element_midpoint_dist[j];
    if (dt_inv < dt_inv_temporary)
      dt_inv = dt_inv_temporary;
  }
  timesteps= (int)(dayLength* dt_inv) +1;
  dt = dayLength/ timesteps; /* final timestep */

  QQ[N]=0;            /* it is intended that the last value of QQ stays at zero during timestepping */
  temp[0] = temp_top; /* assign dirichlet boundary condition */


  /*******main timestepping loop*******/
  int timestp;
  for(timestp=0; timestp<timesteps; ++timestp) { 
    /* calculate gridpoint temperatures from gridpoint enthalpies */
    for (j=0; j<N; ++j)
    {
      temp[j+1] = (enth[j] < 0                 ?  enth[j] *                        th->c_frozen[j]   : 0) + 
                  (enth[j] > th->latent_heat[j] ? (enth[j] - th->latent_heat[j]) * th->c_unfrozen[j] : 0); 
                  /* T=0 when 0<enth[j]<latent_heat */
      /* the term corresponds to eq (3.18) and (2.4) of the master thesis */
      /* it is the standart across-phase enthalpy temperature relation */
    }

    /* calculate heat fluxes from temperature difference */
    for (j=0; j<N; ++j) 
    {
      QQ[j] = -(temp[j+1] *(temp[j+1]  < 0 ? th->lam_frozen[j] : th->lam_unfrozen[j]) - 
                temp[j]   *(temp[j]    < 0 ? th->lam_frozen[j] : th->lam_unfrozen[j])) ;
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


static void use_temp_scheme(
                    Real * temp,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real * hcap,        /*< heat capacities */
                    const Real * lam       /*< thermal conductivities */
                    ) 
{

  /*******precompute some values for better performance*******/
  Real lam_divBy_h[N];      /* thermal conducitivity divided by gridpoint distances */
  Real inv_element_midpoint_dist_divBy_c[N]; /* inverse of the distances between the midpoints of adjacent elements divided by unfrozen heat capacity */
  /* the last entry is the distance of the midpoint to the element boundary */

  int j; /* gridpoint index */
  for (j=0; j<N; ++j)
  {
    lam_divBy_h[j] = lam[j] / h[j];
    inv_element_midpoint_dist_divBy_c[j] = (2/(h[j] + (j<N-1 ? h[j+1] : 0.0)))/hcap[j]; 
  }
  
  /*******calulate max possible timestep*******/
  /* seek for the maximum that still guarantees stability.*/
  int timesteps;        /* number of timesteps */
  Real dt = 0.0;        /* timestep */
  Real dt_inv_temporary;
  Real dt_inv=0;
  for (j=0; j<N; ++j) {
    dt_inv_temporary = (lam_divBy_h[j] + (j<N-1 ? lam_divBy_h[j+1]:0.0) ) * inv_element_midpoint_dist_divBy_c[j];
    if (dt_inv < dt_inv_temporary)
      dt_inv = dt_inv_temporary;
  }
  timesteps= (int)(dayLength* dt_inv) +1;
  dt = dayLength/ timesteps; /* final timestep */

  /* precompute multiplication with dt */
  Real inv_element_midpoint_dist_divBy_c_multBy_dt[N]; /* inverse of the distances between the midpoints of adjacent elements divided by unfrozen heat capacity */
  for(j=0; j<N; ++j)
    inv_element_midpoint_dist_divBy_c_multBy_dt[j] = inv_element_midpoint_dist_divBy_c[j] * dt;

  /* prepare QQ */
  Real QQ[N+1];       /* term resulting from the finite element method  */
  /* can be interpreted as the negative of the heatflux from gp j to gp j+1  */
  QQ[N]=0;            /* it is intended that the last value of QQ stays at zero during timestepping */

  /*******main timestepping loop*******/
  int timestp;
  for(timestp=0; timestp<timesteps; ++timestp) { 
    /* calculate heat fluxes from temperature difference */
    for (j=0; j<N; ++j) 
    {
      QQ[j] = -(temp[j+1] - temp[j]) * lam_divBy_h[j];
      /* the term corresponds to eq (3.15) of the master thesis multiplied with -1 */
      /* an interpretation is the heat flux between gp j+1 and j, see Fourriers law */
    }
    
    /* calculate and apply enthalpy update */
    for (j=0; j<N; ++j) 
    {
      temp[j+1] = temp[j+1]+ (QQ[j] - QQ[j+1]) * inv_element_midpoint_dist_divBy_c_multBy_dt[j];
      /* the term corresponds to eq (3.20) of the master thesis */
      /* it is the enthalpy update during the timestep */
    }
  }
}