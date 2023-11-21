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
STATIC void use_temp_scheme(Real *, const int, const Real *, const Real *, const Real *, int);
STATIC void use_temp_scheme_implicit(Real *, const int, const Real *, const Real *, const Real *, int);
static void use_scheme_with_temp_below_zegro_deg(Real *, const int, const Real *, const Real, Soil_thermal_prop *);
STATIC void arrange_matrix(Real *, Real *, Real *, const int, const Real *, const Real *, const Real *, const Real);
STATIC void thomas_algorithm(double *, double *, double *, double *, double *, int);
STATIC void timestep_implicit(Real *, const int, const Real *, const Real *, const Real *, const Real);
STATIC void timestep_explicit_matrix(Real *, const int, const Real *, const Real *, const Real *, const Real);

void apply_heatconduction_of_a_day(
                    enum uniform_temp_sign uniform_temp_sign, /*< flag to indicate if the temperature is everywhere above 0, everywhere below 0 or mixed */
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

  if(uniform_temp_sign == ALL_ABOVE_0)
  {
    /* temperature scheme can be used */

    /* get the temperatures corresponding to the enthalpy */
    temp[0] = temp_top; /* assign dirichlet boundary condition */
    for (j=0; j<N; ++j)
      temp[j+1] = (enth[j] - th.latent_heat[j]) / th.c_unfrozen[j];
    
    /* update temperature */
    use_temp_scheme_implicit(temp, N, h, th.c_unfrozen, th.lam_unfrozen, -1);

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
    use_temp_scheme_implicit(temp, N, h, th.c_frozen, th.lam_frozen, -1);

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


STATIC void use_temp_scheme(
                    Real * temp,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real * hcap,        /*< heat capacities */
                    const Real * lam,       /*< thermal conductivities */
                    int timesteps /*< if in non testing scenario pass -1 to automatically determine timestep */
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
  Real dt = 0.0;        /* timestep */
  Real dt_inv_temporary;
  Real dt_inv=0;
  for (j=0; j<N; ++j) {
    dt_inv_temporary = (lam_divBy_h[j] + (j<N-1 ? lam_divBy_h[j+1]:0.0) ) * inv_element_midpoint_dist_divBy_c[j];
    if (dt_inv < dt_inv_temporary)
      dt_inv = dt_inv_temporary;
  }
  /* for testing a custom number of timestep can be passed to this function, in a non testing scenario -1 is passed */
  if(timesteps == -1) 
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
    
    /* calculate and apply temperature update */
    for (j=0; j<N; ++j) 
    {
      temp[j+1] = temp[j+1]+ (QQ[j] - QQ[j+1]) * inv_element_midpoint_dist_divBy_c_multBy_dt[j];
      /* the term corresponds to eq (3.20) of the master thesis */
      /* it is the temperature update during the timestep */
    }
  }
}



STATIC void use_temp_scheme_implicit(
                    Real * temp,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real * hcap,        /*< heat capacities */
                    const Real * lam,       /*< thermal conductivities */
                    int steps /*< if in non testing scenario pass -1 to automatically determine timestep */
                    )
{
  if(steps == -1)
    if(U_TEST && GPLHEAT > 3)
      steps = GPLHEAT; /* high res value */
    else 
      steps = 1; /* default value (on timestep per day) */
  Real dt = dayLength/steps;
  for (int i = 0; i < steps; i++) {
    timestep_implicit(temp, N, h, hcap, lam, dt);
  }
}




 STATIC void timestep_implicit(
                    Real * temp,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real * hcap,        /*< heat capacities */
                    const Real * lam,       /*< thermal conductivities */
                    const Real dt          /*< timestep */
                    ) 
{


  Real sub[N]; /* sub diagonal elements */
  Real main[N]; /* main diagonal elements */
  Real sup[N]; /* super diagonal elements */
  arrange_matrix(sub, main, sup, N, h, hcap, lam, dt);

  /* print mat */
  // for (int i = 0; i < N; i++) {
  //   printf("%f %f %f\n", sub[i], main[i], sup[i]);
  // }
  

  /* compute right-hand side  */
  Real rhs[N];
  int j;
  for (j=0; j<N; ++j){
    rhs[j] = (temp[j+1] * (2-main[j]) - temp[j] * sub[j] - temp[j+2] * sup[j]);
    //if(j==N-1)
      //printf("%f %f %f\n", temp[j+1], temp[j], temp[j+2]);
  }
  rhs[0] -= temp[0] * sub[0];

  /* solve tridiagonal system */
  thomas_algorithm(sub, main, sup, rhs, &temp[1], N);
 }

 /* the below function is for testing only */
STATIC void timestep_explicit_matrix(
                    Real * temp,           /*< enthalpy vector that the method is updating (excluding gridpoint at surface) */
                    /* the gridpoint at the surface is assumed to have the given temp_top temperature */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real * hcap,        /*< heat capacities */
                    const Real * lam,       /*< thermal conductivities */
                    const Real dt          /*< timestep */
                    ) 
{


  Real sub[N]; /* sub diagonal elements */
  Real main[N]; /* main diagonal elements */
  Real sup[N]; /* super diagonal elements */
  arrange_matrix(sub, main, sup, N, h, hcap, lam, dt);

  /* print mat */
  // for (int i = 0; i < N; i++) {
  //   printf("%f %f %f\n", sub[i], main[i], sup[i]);
  // }
  

  /* compute right-hand side  */
  Real rhs[N];
  int j;
  printf("dt %f\n", dt);
  for (j=0; j<N; ++j){
    rhs[j] = 2 * (temp[j+1] * (1.5-main[j]) - temp[j] * sub[j] - temp[j+2] * sup[j]);
    //if(j==N-1)
      //printf("%f %f %f\n", temp[j+1], temp[j], temp[j+2]);
  }
  //rhs[0] -= temp[0] * sub[0];

  for (j=0; j<N; ++j)
    printf("%f \n", rhs[j]);

  /* solve tridiagonal system */
  //thomas_algorithm(sub, main, sup, rhs, &temp[1], N);
  for (j=0; j<N; ++j)
    temp[j+1] = rhs[j];

 }

STATIC void arrange_matrix(           
                    Real * a,          /*< sub diagonal elements  */
                    Real * b,          /*< main diagonal elements */
                    Real * c,          /*< super diagonal elements */
                    const int N,           /*< number of gridpoints (excluding gridpoint at surface) */
                    const Real * h,        /*< distances between adjacent gridpoints  */
                    const Real * hcap,        /*< heat capacities */
                    const Real * lam ,   /*< thermal conductivities */
                    const Real dt
                    )   
{
  /*******precompute some values for better performance*******/
  Real lam_divBy_h[N];      /* thermal conducitivity divided by gridpoint distances */
  Real inv_element_midpoint_dist_divBy_c[N]; /* inverse of the distances between the midpoints of adjacent elements divided by unfrozen heat capacity */
  lam_divBy_h[0] = lam[0] / h[0];
  int j;
  Real dt_half = dt/2;
  for (j=0; j<(N-1); ++j)
  {
    lam_divBy_h[j+1] = lam[j+1] / h[j+1];
    inv_element_midpoint_dist_divBy_c[j] = 2/(h[j] +  h[j+1])/hcap[j]; 
    a[j] = - lam_divBy_h[j] * inv_element_midpoint_dist_divBy_c[j] * dt_half ;
    c[j] = - lam_divBy_h[j+1] * inv_element_midpoint_dist_divBy_c[j] * dt_half ;
    b[j] = 1 - a[j] - c[j];
  }
  inv_element_midpoint_dist_divBy_c[N-1] = (2/h[N-1])/hcap[N-1]; 
  a[N-1] = - lam_divBy_h[N-1] * inv_element_midpoint_dist_divBy_c[N-1] * dt_half;
  b[N-1] = 1 - a[N-1];
  c[N-1] = 0;

}

STATIC void thomas_algorithm(double *a, /* sub diagonal elements */
                      double *b, /* main diagonal elements */
                      double *c, /* super diagonal elements */
                      double *d, /* right hand side */
                      double *x, /* solution */
                      int n) {
    double c_prime[n - 1];
    double d_prime[n];

    /* modify coefficients by pogressing in forward direction */
    /* this codes eliminiates the sub diagnal a an norms the diagonal b 1 */
    c_prime[0] = c[0] / b[0];
    for (int i = 1; i < n - 1; i++) {
        c_prime[i] = c[i] / (b[i] - a[i] * c_prime[i - 1]);
    }

    d_prime[0] = d[0] / b[0];
    for (int i = 1; i < n; i++) {
        d_prime[i] = (d[i] - a[i] * d_prime[i - 1]) / (b[i] - a[i] * c_prime[i - 1]);
    }

    /* back substitution */
    x[n - 1] = d_prime[n - 1];
    for (int i = n - 2; i >= 0; i--) {
        x[i] = d_prime[i] - c_prime[i] * x[i + 1];
    }
}