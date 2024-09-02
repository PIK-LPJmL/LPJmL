/* ------- c libraries ------- */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------- headers with no corresponding .c files ------- */
#include "lpj.h"

/* ------- headers with corresponding .c files that will be compiled/linked in by ceedling ------- */
/* c unit testing framework */
#include "unity.h" 
/* modules under test */
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"
#include "apply_perc_enthalpy.h"
#include "calc_soil_thermal_props.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"
#include "updatelitterproperties.h"
#include "update_soil_thermal_state.h"
/* support for tests */
#include "support_global_variables.h"
#include "support_soil_mocks.h"
#include "support_fail_stub.h"
#include "support_manipulate_soil.h"
#include "support_assertions.h"

/* ------- prototypes ------- */
Real analytical_solution1d_heateq_semiinfinite(Real, Real, Real, Real, Real, Real);

/* ------- tests ------- */
#define TOLERANCE 0.0001 /* TOLERANCE for comparison with analytical solution */
/* --- first part --- 
 * compare with standart heat equation solutions in case of no phase changes happening */
void test_after_one_and_two_heatconduction_days_temps_should_match_analytical_solution_for_pos_temps(void)
{
  Soil test_soil;
  Soil_thermal_prop th;
  Real h[NHEATGRIDP];       /* distances between gridpoints*/
  Real gp_locs[NHEATGRIDP]; /* locations of gridpoints */

  /* --- setup soil --- */
  setup_simple_soil(&test_soil);
  set_enthalpies_corresponding_to_const_temp(&test_soil, 10.0);

  /* --- get locations of gridpoints --- */
  setup_heatgrid(h);
  Real curr_location = 0.0;
  for (int i = 0; i < NHEATGRIDP; i++)
  {
    curr_location += h[i];
    gp_locs[i] = curr_location;
  }

  /* --- get thermal properties ---- */
  calc_soil_thermal_props(UNKNOWN, &th, &test_soil, NULL, NULL, TRUE, TRUE);

   /* --- apply conduction and confirm solution of first day ---- */
  update_soil_thermal_state(&test_soil, 2, &test_config);
  /* check gridpoint 100 */
  Real analytical_solution = analytical_solution1d_heateq_semiinfinite(gp_locs[100], NSECONDSDAY, th.lam_unfrozen[0], th.c_unfrozen[0], 10.0, 2.0);
  Real numerical_solution = ENTH2TEMP(test_soil.enth, &th, 100);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_solution, numerical_solution);
  /* check gridpoint 200 */
  analytical_solution = analytical_solution1d_heateq_semiinfinite(gp_locs[200], NSECONDSDAY, th.lam_unfrozen[0], th.c_unfrozen[0], 10.0, 2.0);
  numerical_solution = ENTH2TEMP(test_soil.enth, &th, 200);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_solution, numerical_solution);

   /* --- apply conduction and confirm solution of second day ---- */
  update_soil_thermal_state(&test_soil, 2, &test_config);
  /* check gridpoint 100 */
  analytical_solution = analytical_solution1d_heateq_semiinfinite(gp_locs[100], 2 * NSECONDSDAY, th.lam_unfrozen[0], th.c_unfrozen[0], 10.0, 2.0);
  numerical_solution = ENTH2TEMP(test_soil.enth, &th, 100);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_solution, numerical_solution);
  /* check gridpoint 200 */
  analytical_solution = analytical_solution1d_heateq_semiinfinite(gp_locs[200], 2 * NSECONDSDAY, th.lam_unfrozen[0], th.c_unfrozen[0], 10.0, 2.0);
  numerical_solution = ENTH2TEMP(test_soil.enth, &th, 200);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_solution, numerical_solution);

  teardown_soil_sub_structs(&test_soil);
}

void test_after_one_heat_conduction_day_temps_should_match_analytical_solution_for_negative_temps(void)
{
  Soil_thermal_prop th;
  Real h[NHEATGRIDP];       /* distances between gridpoints*/
  Real gp_locs[NHEATGRIDP]; /* locations of gridpoints */
  Soil test_soil;

  /* --- setup soil --- */
  setup_simple_soil(&test_soil);
  set_enthalpies_corresponding_to_const_temp(&test_soil, -10.0);

  /* --- get locations of gridpoints --- */
  setup_heatgrid(h);
  Real curr_location = 0.0;
  for (int i = 0; i < NHEATGRIDP; i++)
  {
    curr_location += h[i];
    gp_locs[i] = curr_location;
  }

  /* --- get thermal properties ---- */
  calc_soil_thermal_props(UNKNOWN, &th, &test_soil, NULL, NULL, TRUE, TRUE);

  /* --- apply conduction and confirm solution of first day ---- */
  update_soil_thermal_state(&test_soil, -2, &test_config);

  /* check gridpoint 100 */
  Real analytical_solution = analytical_solution1d_heateq_semiinfinite(gp_locs[100], NSECONDSDAY, th.lam_frozen[0], th.c_frozen[0], -10.0, -2.0);
  Real numerical_solution = ENTH2TEMP(test_soil.enth, &th, 100);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_solution, numerical_solution);

  teardown_soil_sub_structs(&test_soil);
}

/* --- second part --- 
 * compare with phase change analytical solution in case phase changes happen */
/*
 * The below values are analytical solutions for transient heatconduction with phasechange in a semi-infinite slab.
 * The analytical solution depends on the numerical approximation of a root of a nonlinear function,
 * which is why a matlab script was used to compute these values.
 * The mean of temperatures from 0.5 m to 1 m and 1 m to 2 m was taken to make the values
 * comparable with the output of update_soil_thermal_state.c.
 * Literature source: Alexiades, Vasilios et. al.: Mathematical Modelling of Melting and Freezing Processes, p. 47 f.
 */
#define analytical_mean_0_5m_to_1m_day_1 -9.377364007590378
#define analytical_mean_2m_to_3m_day_1   -10.986795388286655
#define analytical_mean_0_5m_to_1m_day_2 -7.669178578924899
#define analytical_mean_2m_to_3m_day_2   -10.978824688456244

void test_after_one_heat_conduction_day_temps_should_match_analytical_solution(void)
{
  Soil test_soil;
  setup_simple_soil(&test_soil);

  update_soil_thermal_state(&test_soil, 2, &test_config);

  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_mean_0_5m_to_1m_day_1, test_soil.temp[2]);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_mean_2m_to_3m_day_1, test_soil.temp[4]);

  teardown_soil_sub_structs(&test_soil);
}

void test_after_two_heat_conduction_days_temps_should_match_analytical_solution(void)
{

  Soil test_soil;
  setup_simple_soil(&test_soil);

  update_soil_thermal_state(&test_soil, 2, &test_config);
  update_soil_thermal_state(&test_soil, 2, &test_config);

  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_mean_0_5m_to_1m_day_2, test_soil.temp[2]);
  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_mean_2m_to_3m_day_2, test_soil.temp[4]);

  teardown_soil_sub_structs(&test_soil);
}

/* ------- helper functions ------- */
/* analytical solution for temps all above or all below zero degree i.e. without
 * phase change */
Real analytical_solution1d_heateq_semiinfinite(Real x, Real t, Real k, Real c, Real init, Real DB)
{
  Real alpha = k / c; // calculate the thermal diffusivity
  Real sqrt_alpha_t = sqrt(alpha * t);

  return (Real)init + (Real)(DB - init) * erfc(x / (2.0 * sqrt_alpha_t));
  /* Source:
   * https://community.ptc.com/sejnu66972/attachments/sejnu66972/PTCMathcad/176513/1/2.2_Transient_Conduction_in_Semi-Infinite_Slab.pdf
   */
}
