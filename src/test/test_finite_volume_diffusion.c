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
#include "finite_volume_diffusion.h"

#include "apply_heatconduction_of_a_day.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
#include "support_fail_stub.h"


/* ------- prototypes ------- */
Real analytical_solution1d_heateq_semiinfinite(Real, Real, Real, Real, Real, Real);
int is_solution_bounded(Real * amount, int n);
void get_layer_midpoints(Real * x, Real * h, int n);
void print_tridiagonal_matrix(const Real *a, const Real *b, const Real *c, int N);

/* ------- tests ------- */
#define TOLERANCE 0.001 /* TOLERANCE for comparison with analytical solution */


void test_finite_volume_diffusion_timestep_resistances(void)
{
  Real amount[3] = {0.0, 0.0, 0.0};
  int n = 3;
  Real h[3] = {0.1, 0.2, 0.4};
  Real diff[3] = {0.5, 0.1, 0.2};

  Real res_expected[3] = {0.1, 1.1, 2.0};

  Real res[3];
  calculate_resistances(res, h, diff, n);

  TEST_ASSERT_DOUBLE_ARRAY_WITHIN(TOLERANCE, res_expected, res, 3);
}

void test_finte_volume_diffusion_analytical_solution_should_be_met_after_day_impl(void)
{
  int n = 1000;
  Real amount[n];
  Real h[n];
  Real gas_con_air = -20.0;
  Real init_gas_conc = 10.0;
  Real diff[n];
  Real porosity[n];

  /* define physical quantities */
  for (int i = 0; i < n; i++)
  {
    h[i] = 1000.0 / n; /* length of slab = 100 m */
    porosity[i] = 0.5;

    diff[i] = 0.01;
  }

  for (int i = 100; i < 1000; i++)
  {
    h[i] = h[i] * 0.3;
  }

  for (int i = 0; i < n; i++)
  {
    amount[i] = init_gas_conc * h[i] * porosity[i]; /* g/m^2 */
  }


  Real x[n];
  get_layer_midpoints(x, h, n);

  /* apply one day of diffusion using apply_finite_volume_diffusion_of_a_day */
  int steps = 3000;
  Real dt = day2sec(1.0) / steps;
  for (int i = 0; i < steps; i++)
  {
    apply_finite_volume_diffusion_impl(amount, n, h, gas_con_air, diff, porosity, dt);
  }

  
  /* get numerical solution at gp */
  int gp = (int)(n/12.0);
  Real numerical_solution = amount[gp]/ h[gp] / porosity[gp];

  /* get analytical solution */
  Real analytical_solution = analytical_solution1d_heateq_semiinfinite(x[gp], day2sec(1.0), diff[0], porosity[0], init_gas_conc, gas_con_air);

  TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, analytical_solution, numerical_solution);
}

// void test_implicit_and_expl_should_give_sim_results(void)
// {
//   int n = 1000;
//   Real amount[n];
//   Real h[n];
//   Real gas_con_air = -20.0;
//   Real init_gas_conc = 10.0;
//   Real diff[n];
//   Real porosity[n];

//   /* define physical quantities */
//   for (int i = 0; i < n; i++)
//   {
//     h[i] = 20000.0 / n; 
//     porosity[i] = 0.5;

//     diff[i] = 0.01;
//   }

//   for (int i = 100; i < 300; i++)
//   {
//     h[i] = h[i] * 0.3;
//     diff[i] = 2;
//   }

//   for (int i = 20; i < 30; i++)
//   {
//     porosity[i] = 0.1 ; /* g/m^2 */
//     diff[i] = 100;
//   }

//   porosity[100] = 0.1;
//   porosity[101] = 0.1;

//   for (int i = 0; i < n; i++)
//   {
//     amount[i] = init_gas_conc * h[i] * porosity[i]; /* g/m^2 */
//   }

//   for (int i = 400; i < 600; i++)
//   {
//     amount[i] *= 2;
//   }

//   amount[100] = 0.0;
//   amount[101] *= 200.0;

//   Real x[n];
//   get_layer_midpoints(x, h, n);

//   Real amount_cp[n];
//   memcpy(amount_cp, amount, n * sizeof(Real));

//   /* apply one day of diffusion using apply_finite_volume_diffusion_of_a_day */
//   apply_finite_volume_diffusion_of_a_day(amount, n, h, gas_con_air, diff, porosity);

//   /* apply one day of diffusion using apply_finite_volume_diffusion_impl */
//   int steps = 10000;
//   Real dt = day2sec(1.0) / steps;
//   for (int i = 0; i < steps; i++)
//   {
//     apply_finite_volume_diffusion_impl(amount_cp, n, h, gas_con_air, diff, porosity, dt);
//   }

//   /* check soultions are similiar */
//   for (int i = 0; i < n; i++)
//   {
//     TEST_ASSERT_FLOAT_WITHIN(0.1, amount[i], amount_cp[i]);
//   }
// }

void test_total_amount_is_conserved(void)
{
  int n = 1000;
  Real amount[n];
  Real h[n];
  Real gas_con_air = 10.0;
  Real init_gas_conc = 10.0;
  Real diff[n];
  Real porosity[n];

  /* define physical quantities */
  for (int i = 0; i < n; i++)
  {
    h[i] = 20000.0 / n; /* length of slab = 100 m */
    porosity[i] = 0.5;

    diff[i] = 0.001;
  }

  for (int i = 700; i < 800; i++)
  {
    h[i] = h[i] * 0.3;
    diff[i] = 0.002;
  }

  for (int i = 820; i < 840; i++)
  {
    porosity[i] = 0.8 ; /* g/m^2 */
    diff[i] = 0.0001;
  }

  for (int i = 0; i < n; i++)
  {
    amount[i] = init_gas_conc * h[i] * porosity[i]; /* g/m^2 */
  }

  for (int i = 800; i < 900; i++)
  {
    amount[i] *= 2;
  }

  for (int i = 600; i < 620; i++)
  {
    amount[i] *= 0.5;
  }

  amount[500] *= 10000;

  /* get total amount before */
  Real total_amount_before = 0.0;
  for (int i = 0; i < n; i++)
  {
    total_amount_before += amount[i];
  }

  Bool r;
  for (int i = 0; i < 100; i++)
  {
    r = apply_finite_volume_diffusion_impl(amount, n, h, gas_con_air, diff, porosity, day2sec(1.0));
  }
  
  /* check that total gas amount is conserved */
  Real total_amount_after = 0.0;
  for (int i = 0; i < n; i++)
  {
    total_amount_after += amount[i];
  }  
  TEST_ASSERT_EQUAL_DOUBLE(total_amount_after, total_amount_before);
}

void test_arrange_matrix(void)
{
  Real a[3], b[3], c[3];
  Real h[3] = {1, 0.1, 0.5};
  Real por[3] = {5, 2, 1};
  Real res[3] = {1, 2, 3};
  Real dt = 1;
  int n = 3;

  arrange_matrix_fv(a, b, c, h, por, res, dt, n);

  TEST_ASSERT_EQUAL_DOUBLE(1.3, b[0]);
  TEST_ASSERT_EQUAL_DOUBLE(-0.2, a[0]);
  TEST_ASSERT_EQUAL_DOUBLE(-0.1, c[0]);
  TEST_ASSERT_EQUAL_DOUBLE((1+((1/6.0)*10)+2.5), b[1]);
  TEST_ASSERT_EQUAL_DOUBLE(-2.5, a[1]);
  TEST_ASSERT_EQUAL_DOUBLE(-(1/6.0)*10, c[1]);
  TEST_ASSERT_EQUAL_DOUBLE(-1/0.5*1/3.0, a[2]);
  TEST_ASSERT_EQUAL_DOUBLE(1 + (1/0.5*1/3.0), b[2]);
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

int is_solution_bounded(Real * amount, int n)
{
  for (int i = 0; i < n; i++)
  {
    if (amount[i] > 1e4 || amount[i] < -1e4)
    {
      return 0;
    }
  }
  return 1;
}

void get_layer_midpoints(Real * x, Real * h, int n)
{
  Real loc[n+1];
  loc[0] = 0.0;
  for (int i = 0; i < n; i++)
  {
    loc[i+1] = loc[i] + h[i];
  }
  /* get layer midpoint locations */
  for (int i = 0; i < n; i++)
  {
    x[i] = loc[i] + h[i] / 2;
  }
}

/* helper function to print a matrix to the console */
void print_tridiagonal_matrix(const Real *a, const Real *b, const Real *c, int N)
{
  for (int i = 0; i < N; i++)
  {
    /* loop through rows */
    for (int j = 0; j < N; j++)
    {
      /* loop through cols of the row */
      if (j == i - 1)
        printf("%5.1f ", a[i]); // Sub-diagonal
      else if (j == i)
        printf("%5.1f ", b[i]); // Main diagonal
      else if (j == i + 1)
        printf("%5.1f ", c[i]); // Super-diagonal
      else
        printf("%5.1f ", 0.0f); // Zero for other elements
    }
    printf("\n");
  }
}