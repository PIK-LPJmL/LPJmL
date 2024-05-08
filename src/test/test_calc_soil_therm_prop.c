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
/* support for tests */
#include "support_global_variables.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"
#include "calc_soil_thermal_props.h"

/* ------- prototypes ------- */
void the_thermal_prop_should_be_as_ever(Soil_thermal_prop);

/* ------- tests ------- */
void test_no_nas_should_be_generated_in_thermal_props_if_soil_has_no_water_and_pores(void)
{
  /* --- set up soil --- */
  Soil test_soil;
  Soil_thermal_prop thermal_props;
  setup_complex_soil(&test_soil);
  /* remove soilwater and pores */
  set_soilwater_to_zero(&test_soil);
  int l;
  foreachsoillayer(l)
  {
    test_soil.wsats[l] = 0;
    test_soil.wsat[l] = 0;
  }
  
  /* --- calc soil thermal properties --- */
  calc_soil_thermal_props(UNKNOWN, &thermal_props, &test_soil, NULL, NULL, TRUE, TRUE);

  /* --- assert that thermal properties has no nas  --- */
  for (int i = 0; i < NSOILLAYER; i++)
  {
    TEST_ASSERT_TRUE(!isnan(thermal_props.c_frozen[i]));
    TEST_ASSERT_TRUE(!isnan(thermal_props.c_unfrozen[i]));
    TEST_ASSERT_TRUE(!isnan(thermal_props.lam_frozen[i]));
    TEST_ASSERT_TRUE(!isnan(thermal_props.lam_unfrozen[i]));
    TEST_ASSERT_TRUE(!isnan(thermal_props.latent_heat[i]));
  }

  teardown_soil_sub_structs(&test_soil);
}

void test_soil_thermal_properties_of_simple_soil_should_be_still_the_same(void)
{
  Soil test_soil;
  Soil_thermal_prop thermal_props;
  setup_simple_soil(&test_soil);

  calc_soil_thermal_props(UNKNOWN, &thermal_props, &test_soil, NULL, NULL, TRUE, TRUE);

  the_thermal_prop_should_be_as_ever(thermal_props);
  teardown_soil_sub_structs(&test_soil);
}

void test_conductivtiy_of_waterless_soil_should_be_as_manually_calculated()
{
  /* setup soil */
  Soil test_soil;
  Soil_thermal_prop thermal_props;
  setup_complex_soil(&test_soil);
  set_soilwater_to_zero(&test_soil);

  /* calculate thermal properties */
  calc_soil_thermal_props(UNKNOWN, &thermal_props, &test_soil, NULL, NULL, TRUE, TRUE);

  /* For elements (i.e. intervals betwee gridpoints) that are fully contained in a layer
   * the thermal conductivity should equal the dry conductivity of the layer. */
  TEST_ASSERT_EQUAL_DOUBLE(2, thermal_props.lam_frozen[0]);
  TEST_ASSERT_EQUAL_DOUBLE(0.5, thermal_props.lam_unfrozen[GPLHEAT + 1]);

  /* For elements that cross layer borders the thermal conductivity is based on the formulas for thermal resistance:
   * (1) resistance_element = resistance_layer1part_of_element + resistance_layer2part_of_element
   * (2) resistance = length/thermal_conductivity
   * The following formula is derived by plugging (2) into (1) and solving for the thermal conductivity: */
  Real expected_layer_crossing_element_condunctivity =
      (soildepth[0] + soildepth[1]) / (soildepth[0] / test_soil.k_dry[0] + soildepth[1] / test_soil.k_dry[1]);
  TEST_ASSERT_EQUAL_DOUBLE(expected_layer_crossing_element_condunctivity, thermal_props.lam_unfrozen[GPLHEAT]);

  teardown_soil_sub_structs(&test_soil);
}

/* ------- helper functions ------- */
void the_thermal_prop_should_be_as_ever(Soil_thermal_prop thermal_props)
{ 
  for (int i = 0; i < NSOILLAYER; i++)
  {
    /* values have been recorded 03/2024 */
    TEST_ASSERT_EQUAL_DOUBLE(910180, thermal_props.c_frozen[i]);
    TEST_ASSERT_EQUAL_DOUBLE(1435180, thermal_props.c_unfrozen[i]); // liquid water has a higher heat capcity than ice
    TEST_ASSERT_EQUAL_DOUBLE(1.233783111351, thermal_props.lam_frozen[i]);
    TEST_ASSERT_EQUAL_DOUBLE(0.730975324683, thermal_props.lam_unfrozen[i]); // frozen soil conducts heat better than unfrozen
    TEST_ASSERT_EQUAL_DOUBLE(75000000, thermal_props.latent_heat[i]);
  }
}
