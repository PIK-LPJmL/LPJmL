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
/* lpjml modules under test */
#include "apply_heatconduction_of_a_day.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
/* lpjml modules mocked */
#include "support_fail_stub.h"
/* support for tests */
#include "support_global_variables.h"

/* ------- function prototypes ------- */
void print_tridiagonal_matrix(const Real *, const Real *, const Real *, int);

/* ------- tests ------- */
void test_arrange_matrix_outputs_correct_matrix()
{
  const int N = 6;                              /* number of gridpoints */
  Real h[] =    {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; /* inter gridpoint distances */
  Real hcap[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; /* heat capacity */
  Real lam[] =  {1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; /* thermal conductivity */
  Real a[N], b[N], c[N]; /* matrix diagonals */

  arrange_matrix(a, b, c, h, hcap, lam, 2);

  /* Define expected matrix by its diagonals 
   *  3 -1  0  0  0  0
   * -1  3 -1  0  0  0
   *  0 -1  3 -1  0  0
   *  0  0 -1  3 -1  0
   *  0  0  0 -1  3 -1
   *  0  0  0  0 -2  3
   * (Note that the n-th vector entry corresponds to the n-th matrix row;
   * Thus the first entry of a and last entry entry of c are not part of the matrix)
  */
  Real expected_a[] = {-1.0, -1.0, -1.0, -1.0, -1.0, -2.0}; /* expected sub diagonal */
  Real expected_b[] = { 3.0,  3.0,  3.0,  3.0,  3.0,  3.0}; /* expected main diagonal */
  Real expected_c[] = {-1.0, -1.0, -1.0, -1.0, -1.0,  0};   /* expected super diagonal */


  /* Assert each element of a, b, c */
  for (int i = 0; i < N; i++)
  {
    TEST_ASSERT_FLOAT_WITHIN(1e-9, expected_a[i], a[i]);
    TEST_ASSERT_FLOAT_WITHIN(1e-9, expected_b[i], b[i]);
    TEST_ASSERT_FLOAT_WITHIN(1e-9, expected_c[i], c[i]);
  }
}

/* Note that the enth and temp schemes are different methods,
 * hence results cannot expected to be exactly equal,
 * but should nevertheless be close,
 * since they solve the same PDE.
 * We define a tolarance for comparison between schemes. */
#define TOL 1e-5 
void test_enth_scheme_and_temp_scheme_have_almost_same_results(void)
{
  const int N = 6;                 /* number of free gridpoints */
  Real temp1[N + 1], temp2[N + 1]; /* temp vectors include extra gridpoint at the surface */
  Real enth[N];
  Real h[N], hcap[N], lam[N];
  Soil_thermal_prop th = {};
  int i;

  /* initialize temp and thermal properties with inhomogeneous data */
  for (int i = 0; i < N; ++i)
  {
    temp1[i] = 5;
    temp2[i] = 5;
    h[i] = 1 + sin((Real)i/N)+1;
    hcap[i] = (0.1 + (Real)i / N) * 100;
    lam[i] = (0.1 + (Real)i / N * 2) / 100;
  }

  /* make some random changes to the data to have it even more inhomogenous */
  temp1[5] = temp2[5] = 5;
  temp1[4] = temp2[4] = 2;
  lam[3] *= 0.5;
  hcap[2] *= 10;

  /* define temp corresponding enthalpy and assign to thermal properties structure */
  for (i = 0; i < N; ++i)
  {
    enth[i] = temp2[i + 1] * hcap[i] + 1;
    th.c_unfrozen[i] = hcap[i];
    th.c_frozen[i] = 1;
    th.lam_unfrozen[i] = lam[i];
    th.lam_frozen[i] = 1;
    th.latent_heat[i] = 1;
  }

  /* run both schemes */
  temp2[0] = 10;
  for (i = 0; i < 10; ++i)
  {
    use_enth_scheme(enth, 
                    h, 
                    10, // set top boundary condition
                    &th);
    use_temp_scheme_implicit(temp2, 
                             h, 
                             hcap, 
                             lam, 
                             10000 // use 1000 steps within the day
                             );
  }

  /* confirm that the resulting temperatures are almost the same */
  for (int i = 0; i < N; ++i)
  {
    TEST_ASSERT_FLOAT_WITHIN(TOL, ENTH2TEMP(enth, &th, i), temp2[i + 1]);
  }

  /* Run the schemes further with modified boundary condition */
  temp2[0] = 25.4;
  for (i = 0; i < 10; ++i)
  {
    use_enth_scheme(enth, h, 25.4, &th);
    use_temp_scheme_implicit(temp2, h, hcap, lam, 10000);
  }

  /* Confirm that the resulting temperatures are still almost the same */
  for (int i = 0; i < N; ++i)
  {
    TEST_ASSERT_FLOAT_WITHIN(TOL, ENTH2TEMP(enth, &th, i), temp2[i + 1]);
  }
}

void test_thomas_algorithm_correctly_solves_tridiagonal_linear_system(void)
{
  /* Solve the equation A x = rhs, for x */

  /* Define a tridiagonal test matrix by giving its diagonals 
   *  1.0   1.0   0     0     0     0
   * -1.1   1.0   2.0   0     0     0
   *  0    -1.2  -3.0   5.0   0     0
   *  0     0     1.3   3.0   6.0   0
   *  0     0     0    -1.4   3.0  -1.0 
   *  0     0     0     0    -1.5   3.0  
   * */
  Real sup[] =  {1.0, 2.0,  5.0, 6.0, -1.0,  0  };
  Real main[] = {1.0, 1.0, -3.0, 3.0,  3.0,  3.0};
  Real sub[] =  {0,  -1.1, -1.2, 1.3, -1.4, -1.5};

  /* Define a test right-hand-side */
  Real rhs[] = {1, 2, 3, 4, 5, 6};

  /* Define the expected result */
  Real expected_result[] = {-5.3831591, 6.3831591, -5.1523171, -0.9594320, 2.2627181, 3.1313590};
  /* verify with
   * https://matrixcalc.org/slu.html#solve-using-Gaussian-elimination%28%7B%7B1,1,0,0,0,0,1%7D,%7B-1%2e1,1,2,0,0,0,2%7D,%7B0,-1%2e2,-3,5,0,0,3%7D,%7B0,0,1%2e3,3,6,0,4%7D,%7B0,0,0,-1%2e4,3,-1,5%7D,%7B0,0,0,0,-1%2e5,3,6%7D%7D%29 
   */

  /* Apply the algorithm */
  Real result[6]; /* vector used to store the return value of the function */
  thomas_algorithm(sub, main, sup, rhs, result);

  /* Verify that the result is as expected */
  int i;
  for (i = 0; i < NHEATGRIDP; ++i)
  {
    TEST_ASSERT_FLOAT_WITHIN(1e-6, expected_result[i], result[i]);
  }

}

/* ------- helper functions ------- */
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
