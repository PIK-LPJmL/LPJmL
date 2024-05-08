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
#include "apply_perc_enthalpy.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
/* lpjml modules mocked */
#include "support_fail_stub.h"
/* support for tests */
#include "support_global_variables.h"
#include "support_stand_mocks.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"

/* ------- tests ------- */
void test_soiltemp_should_remain_constant_if_inflowing_water_has_same_temp_as_soil(void)
{
  /* prepare stand with soil */
  Stand *test_stand = setup_simple_stand();
  Real soiltemp = 10.0;     /* (degree celsius) */
  Real water_inflow = 15.0; /* (mm) */
  set_enthalpies_corresponding_to_const_temp(&test_stand->soil, soiltemp);

  /* add some water to top layer and adjust enthalpy (water has same temp as soil) */
  test_stand->soil.w[0] += water_inflow / test_stand->soil.whcs[0];        /* water addition */
  Real enth_of_water = soiltemp * c_water + c_water2ice;                   /* volumetric enthalpy corresponding to soiltemp */
  test_stand->soil.perc_energy[0] = enth_of_water * water_inflow / 1000;   /* enthalpy addition due to water addition */
  test_stand->soil.wi_abs_enth_adj[0] += water_inflow;                     /* track added enthalpy */

  /* apply percolation enthalpy to soil and update its temperature */
  apply_perc_enthalpy(&test_stand->soil);
  update_mean_layer_temps(&test_stand->soil);

  /* confirm that soiltemp of top layer hs not changed */
  TEST_ASSERT_EQUAL_DOUBLE(soiltemp, test_stand->soil.temp[0]);

  teardown_stand(test_stand);
}

void test_mixing_temp_of_soil_and_waterinflow_should_be_correct(void)
{
  /* define water and soil to be mixed */
  Real watertemp = 10.0;    /* (degree celsius) */
  Real water_inflow = 15.0; /* (mm) */
  Real soiltemp = 5.0;      /* (degree celsius) */
  Stand *test_stand = setup_simple_stand();
  set_enthalpies_corresponding_to_const_temp(&test_stand->soil, soiltemp);

  /* calculate expected mixing temperature after water was added */
  Real watercontent_abs_layer = allwater((&(test_stand->soil)), 0) + allice((&(test_stand->soil)), 0);
  Real solidcontent_abs_layer = soildepth[0] - (&test_stand->soil)->wsats[0];
  Real c_soil = (c_mineral * solidcontent_abs_layer + c_water * watercontent_abs_layer) / soildepth[0]; /* heat capacity of soil */
  Real expected_mixing_temp = /* General Richman's law */
      (soiltemp * c_soil * soildepth[0] + watertemp * c_water * water_inflow) / (c_soil * soildepth[0] + c_water * water_inflow);

  /* add water to soil and adjust enthalpy */
  Real enth_of_water = watertemp * c_water + c_water2ice;
  test_stand->soil.perc_energy[0] = enth_of_water * water_inflow / 1000;
  test_stand->soil.wi_abs_enth_adj[0] += water_inflow;
  test_stand->soil.w[0] += water_inflow / test_stand->soil.whcs[0];

  /* apply enthalpy to soil */
  apply_perc_enthalpy(&test_stand->soil);
  update_mean_layer_temps(&test_stand->soil);

  /* confirm that mixing temperature is as expected */
  TEST_ASSERT_EQUAL_DOUBLE(expected_mixing_temp, test_stand->soil.temp[0]);
  teardown_stand(test_stand);
}
