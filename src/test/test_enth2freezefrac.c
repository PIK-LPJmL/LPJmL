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
#include "enth2freezefrac.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
/* support for tests */
#include "support_global_variables.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"

/* ------- prototypes ------- */
void calculate_freeze_fractions(Real *, Soil *);

/* ------- tests ------- */
void test_freezefrac_should_be_zero_if_soil_contains_full_latentheat()
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_simple_soil(&test_soil);

  set_enthalpies_to_latent_heat(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(0, freezefrac, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_freezefrac_should_be_one_if_soil_contains_no_latentheat()
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_simple_soil(&test_soil);

  set_enthalpies_to_zero(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(1, freezefrac, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_freezefrac_should_be_half_if_soil_contains_half_latentheat()
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_simple_soil(&test_soil);

  set_enthalpies_to_half_latent_heat(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(0.5, freezefrac, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_freezefrac_should_be_one_if_soil_has_no_water_and_zero_enth(void)
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_complex_soil(&test_soil);
  set_soilwater_to_zero(&test_soil);
  set_enthalpies_to_zero(&test_soil);

  calculate_freeze_fractions(freezefrac, &test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(1, freezefrac, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_freezefrac_should_be_zero_if_soil_has_no_water_and_positive_enth(void)
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_complex_soil(&test_soil);
  set_soilwater_to_zero(&test_soil);
  set_enthalpies_corresponding_to_const_temp(&test_soil, 0.001); /* any slightly positive enthalpy */

  calculate_freeze_fractions(freezefrac, &test_soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(0, freezefrac, NSOILLAYER);
  
  teardown_soil_sub_structs(&test_soil);
}

/* ------- helper functions ------- */
void calculate_freeze_fractions(Real *freezefrac, Soil *SoilPointer)
{
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, SoilPointer, NULL, NULL, TRUE, TRUE);
  enth2freezefrac(freezefrac, SoilPointer->enth, &thermal_props);
}
