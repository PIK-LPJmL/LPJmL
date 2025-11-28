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
#include "infil_perc.h"

/* support for tests */
#include "support_global_variables.h"
#include "support_soil_mocks.h"
#include "support_stand_mocks.h"
#include "support_fail_stub.h"
#include "support_manipulate_soil.h"
#include "support_assertions.h"
#include "support_sprintcoord_stub.h"

void test_with_percolation_of_same_temp_soiltemps_should_not_change(void)
{
  Stand *st = setup_simple_stand();
  Real return_flow_b = 0;
  Real initial_temp = 5.0;
  Soil_thermal_prop th;

  set_enthalpies_corresponding_to_const_temp(&st->soil, initial_temp);
  for (int i = 0; i < 10; i++)
  {
    update_soil_thermal_state(&st->soil, initial_temp, &test_config);
    infil_perc(st, 20, initial_temp * c_water + c_water2ice, 20, &return_flow_b, 1, 1, &test_config);
  }

  the_temps_should_have_remained_equal(st->soil.temp, initial_temp);
  teardown_stand(st);
}

void test_with_drip_irrigation_of_same_temp_soiltemps_should_not_change(void)
{
  Stand *st = setup_simple_stand();
  Real return_flow_b = 0;
  Real initial_temp = 5.0;
  Soil_thermal_prop th;
  Irrigation *irr = st->data;
  irr->irrig_system = DRIP;

  set_enthalpies_corresponding_to_const_temp(&st->soil, initial_temp);
  for (int i = 0; i < 10; i++)
  {
    update_soil_thermal_state(&st->soil, initial_temp, &test_config);
    infil_perc(st, 20, initial_temp * c_water + c_water2ice, 0, &return_flow_b, 1, 1, &test_config);
  }

  the_temps_should_have_remained_equal(st->soil.temp, initial_temp);
  teardown_stand(st);
}

void test_with_spinkler_irrigation_of_same_temp_soiltemps_should_not_change(void)
{
  Stand *st = setup_simple_stand();
  Real return_flow_b = 0;
  Real initial_temp = 5.0;
  Soil_thermal_prop th;
  Irrigation *irr = st->data;
  irr->irrig_system = SPRINK;

  set_enthalpies_corresponding_to_const_temp(&st->soil, initial_temp);
  for (int i = 0; i < 10; i++)
  {
    update_soil_thermal_state(&st->soil, initial_temp, &test_config);
    infil_perc(st, 20, initial_temp * c_water + c_water2ice, 0, &return_flow_b, 1, 1, &test_config);
  }

  the_temps_should_have_remained_equal(st->soil.temp, initial_temp);
  teardown_stand(st);
}
