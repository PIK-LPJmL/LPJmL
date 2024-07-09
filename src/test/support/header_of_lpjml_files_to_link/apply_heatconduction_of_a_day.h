void use_enth_scheme(Real *, const Real *, const Real, const Soil_thermal_prop *);
void use_temp_scheme_implicit(Real *, const Real *, const Real *, const Real *, int);
void arrange_matrix(Real *, Real *, Real *, const Real *, const Real *, const Real *, const Real);
void thomas_algorithm(double *, double *, double *, double *, double *);
void timestep_implicit(Real *, const Real *, const Real *, const Real *, const Real);
