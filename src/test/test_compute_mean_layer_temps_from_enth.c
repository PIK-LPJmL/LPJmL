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
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
/* support for tests */
#include "support_global_variables.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"

/* ------- prototypes ------- */
void the_mean_layer_temps_should_equal_mean_of_original_temps(Real *layerTemps, Real *originalTemps);

/* ------- tests ------- */
void test_soillayer_temps_should_be_zero_if_enthalpy_is_zero()
{
  Soil test_soil;
  setup_simple_soil(&test_soil);

  set_enthalpies_to_zero(&test_soil);
  update_mean_layer_temps(&test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(0, test_soil.temp, NSOILLAYER);
  teardown_soil_sub_structs(&test_soil);
}

void test_soillayer_temps_should_be_zero_if_enthalpy_equals_latent_heat()
{
  Soil test_soil;
  setup_simple_soil(&test_soil);

  set_enthalpies_to_latent_heat(&test_soil);
  update_mean_layer_temps(&test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(0, test_soil.temp, NSOILLAYER);
  teardown_soil_sub_structs(&test_soil);
}

void test_soillayer_temps_should_recover_if_enthalpy_corresponds_to_given_rand_temps()
{
  Soil test_soil;
  Real random_temp = 0;
  Real range = 50;
  for (int i = 0; i < 1000; i++)
  {
    setup_complex_soil(&test_soil);
    Real random_temp = (double)rand()/(double)(RAND_MAX/(range*2))-range; // generates a number in [-range , range] 
    set_enthalpies_corresponding_to_const_temp(&test_soil, random_temp);
    update_mean_layer_temps(&test_soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(random_temp, test_soil.temp, NSOILLAYER);
    teardown_soil_sub_structs(&test_soil);
  }
}

void test_correct_mean_layertemps_should_recover_if_enthalpies_correspond_to_linearly_increasing_temps()
{
  Soil test_soil;
  Real linear_increase_temps[NHEATGRIDP];

  setup_complex_soil(&test_soil);
  set_enthalpies_corresponding_to_linearly_increasing_temps(&test_soil, linear_increase_temps);
  update_mean_layer_temps(&test_soil);

  the_mean_layer_temps_should_equal_mean_of_original_temps(test_soil.temp, linear_increase_temps);
  teardown_soil_sub_structs(&test_soil);
}

/* ------- helper functions ------- */
void the_mean_layer_temps_should_equal_mean_of_original_temps(Real *actual_layer_temps, Real *original_temps)
{
  double temp_mean = 0;
  for (int l = 0; l < NSOILLAYER; l++)
  {
    temp_mean = 0;
    for (int j = 0; j < GPLHEAT; j++)
    {
      temp_mean += original_temps[GPLHEAT * l + j] / GPLHEAT;
    }
    TEST_ASSERT_EQUAL_DOUBLE(temp_mean, actual_layer_temps[l]);
    temp_mean = 0;
  }
}
