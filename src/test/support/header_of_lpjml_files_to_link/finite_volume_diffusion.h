void finite_volume_diffusion_timestep(Real * amount,   const int n,   const Real dt,   const Real * h,     const Real gas_con_air, const Real * diff,    const Real * porosity );
int apply_finite_volume_diffusion_of_a_day(Real * amount,   const int n,  const Real * h,     const Real gas_con_air, const Real * diff,    const Real * porosity );
void calculate_resistances(Real * res, const Real * h, const Real * diff, const int n);
void arrange_matrix_fv(Real * a,          /* sub diagonal elements  */
                           Real * b,          /* main diagonal elements */
                           Real * c,          /* super diagonal elements */
                           const Real * h,    /* distances between adjacent gridpoints  */
                           const Real * por,  /* porosities */
                           const Real * res , /* resitances */
                           const Real dt,      /* timestep */
                           const int n
                          );

Bool apply_finite_volume_diffusion_impl_crank_nicolson(Real * amount,             /* g/m^2, substance absolute amount */
                                           const int n,               /* number of gridpoints */
                                           const Real * h,            /* m, layer thicknesses (delta_x) */
                                           const Real gas_con_air,    /* g/m^2, gas concentration of substance */
                                           const Real * diff,         /* m^2/s, diffusivity */
                                           const Real * porosity,      /* m^3/m^3, porosity */
                                           const Real dt
                                           );