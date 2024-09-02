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
#include "support_stand_mocks.h"
#include "support_fail_stub.h"
#include "support_manipulate_soil.h"
#include "support_assertions.h"

void apply_update_soil_thermal_state_in_equilibrium_100(Soil *);
void apply_scenario_to_soil(Soil *SoilPointer);
void change_water_content(Soil *test_soil);
void change_soil_solid_content(Soil *test_soil);
void apply_update_soil_thermal_state_100(Soil *SoilPointer, Real belowSnowforcing);
void apply_update_soil_thermal_state_10(Soil *SoilPointer, Real belowSnowforcing);
void change_soil_composition(Soil *SoilPointer);
void change_soil_composition_differently(Soil *SoilPointer);
void check_if_results_are_still_similiar(Soil *SoilPointer, double);
void make_water_change_with_accounting_for_enth(Soil *soil, Real water_inflow, Real enth_of_water, Config test_config);
void make_water_change_without_accounting_for_enth(Soil *soil, Real water_inflow);

void test_new_model_results_should_be_still_be_similiar_to_old_results()
{
  Soil test_soil;
  Soil_thermal_prop thermal_props;
  double tolerance = 0.001;
  setup_complex_soil(&test_soil);

  apply_scenario_to_soil(&test_soil);

  check_if_results_are_still_similiar(&test_soil, tolerance);
  teardown_soil_sub_structs(&test_soil);
}

void test_with_forcing_in_equilibrium_soiltemps_should_not_change()
{
  Soil test_soil;
  setup_simple_soil(&test_soil);
  Real initial_temp = test_soil.temp[0];

  apply_update_soil_thermal_state_in_equilibrium_100(&test_soil);

  int l;
  foreachsoillayer(l) { TEST_ASSERT_DOUBLE_WITHIN(epsilon, initial_temp, test_soil.temp[l]); }
  teardown_soil_sub_structs(&test_soil);
}

void test_with_forcing_in_equilibrium_soiltemps_should_not_change_also_with_snow_and_litter()
{
  Soil test_soil = {};
  setup_simple_soil(&test_soil);
  Real initial_temp = test_soil.temp[0];

  test_soil.snowpack = 0.1; /* add some snow */

  test_soil.litter.item->agtop.leaf.carbon = 0.2, /* add some plant litter */

  apply_update_soil_thermal_state_in_equilibrium_100(&test_soil);

  int l;
  foreachsoillayer(l) { TEST_ASSERT_DOUBLE_WITHIN(epsilon, initial_temp, test_soil.temp[l]); }
  teardown_soil_sub_structs(&test_soil);
}

void test_water_content_changes_should_not_influence_soiltemps()
{
  Soil test_soil;
  setup_simple_soil(&test_soil);
  Real initial_temp = test_soil.temp[0];

  change_water_content(&test_soil);
  apply_update_soil_thermal_state_in_equilibrium_100(&test_soil);

  int l;
  foreachsoillayer(l) { TEST_ASSERT_DOUBLE_WITHIN(epsilon, initial_temp, test_soil.temp[l]); }
  teardown_soil_sub_structs(&test_soil);
}

void test_soil_content_changes_should_not_influence_soiltemps()
{
  Soil test_soil;
  Soil_thermal_prop th;
  setup_simple_soil(&test_soil);
  Real initial_temp = test_soil.temp[0];

  change_soil_solid_content(&test_soil);
  apply_update_soil_thermal_state_in_equilibrium_100(&test_soil);

  int l;
  foreachsoillayer(l) { TEST_ASSERT_DOUBLE_WITHIN(epsilon, initial_temp, test_soil.temp[l]); }
  teardown_soil_sub_structs(&test_soil);
}

void test_maxthawdepth_is_correct()
{
  Soil test_soil;
  Soil_thermal_prop th;
  setup_simple_soil(&test_soil);
  calc_soil_thermal_props(UNKNOWN, &th, &test_soil, NULL, NULL, TRUE, TRUE);
  set_enthalpies_to_half_latent_heat(&test_soil);

  for (int i = 0; i < GPLHEAT; i++)
  {
    test_soil.enth[i] = th.latent_heat[i];
  }
  Real freezefrac[NSOILLAYER];                        /* fraction of each layer that is frozen */
  enth2freezefrac(freezefrac, test_soil.enth, &th); /* get frozen fraction of each layer */
  freezefrac2soil(&test_soil, freezefrac);          /* apply frozen fraction to soil variables */

  compute_maxthaw_depth(&test_soil);

  TEST_ASSERT_EQUAL_DOUBLE(200 + 150, test_soil.maxthaw_depth);
  teardown_soil_sub_structs(&test_soil);
}

void test_litter_and_snow_temp_interpolation_works_for_example()
{
  Soil test_soil = {};
  setup_simple_soil(&test_soil);
  set_enthalpies_corresponding_to_const_temp(&test_soil, 0); /* set soltemp temp to zero degree */
  Real test_soilTemp = test_soil.temp[0];
  Soil_thermal_prop therm;
  calc_soil_thermal_props(UNKNOWN, &therm, &test_soil, NULL, NULL, TRUE, TRUE);
  Real th_fr_before = therm.lam_frozen[0];
  Real h[NHEATGRIDP];
  setup_heatgrid(h);
  
  Real h_0_old = h[0];

  test_soil.litter.item->agtop.leaf.carbon = 20.0 * 0.42 * DRY_BULK_DENSITY_LITTER; /* 2 cm of litter */
  test_soil.litter.n = 1;

  test_soil.snowpack = 10.0 / 4.0; /* 1 cm of snow, see SNOWHEIGHT_PER_WATERHEIGHT */

  adjust_grid_and_therm_cond_for_snow(h, &therm, &test_soil);
  adjust_grid_and_therm_cond_for_litter(h, &therm, &test_soil);

  TEST_ASSERT_EQUAL_DOUBLE(0.03 + h_0_old, h[0]); /* should be 4 cm in total for top element length */

  compute_litter_and_snow_temp_from_enth(&test_soil, 10, &therm); /* 10 deg airtemp */

  TEST_ASSERT_EQUAL_DOUBLE((0.025 + h_0_old) / (0.03 + h_0_old) * 10.0, test_soil.temp[NSOILLAYER]); /* snow midpoint is almost the complete way from gp to snow surface */
  TEST_ASSERT_EQUAL_DOUBLE(0.02 / (0.03 + h_0_old) * 10.0, test_soil.litter.agtop_temp); /* litter midpoint is half way of element length */

  teardown_soil_sub_structs(&test_soil);
}

void test_soiltemp_should_remain_constant_after_making_water_changes_with_and_without_enthalpy_accounting(void)
{
  Stand *test_stand = setup_simple_stand();
  Real temp = 10.0;
  set_enthalpies_corresponding_to_const_temp(&test_stand->soil, temp);

  /* 1. accounted changes mean that the enthalpy corresponding to the mass (soild, water) change 
   *   is added to perc_energy and new water and solid contents are saved in the tracking variables 
   *   wi_abs_enth_adj and sol_abs_enth_adj;
   * 2. unaccounted changes modify mass (soilds, water) but do not add or substract respective enthalpy
   *   (e.g. because the exact energy may be unknown);
   * in case 2. the added/subtracted enthalpy is calculated under the assumption 
   * that the added/subtracted mass has the same temperature as the the mass
   * already present, hence the mass change does not effect temperature */

  /* the order of unaccounted and accounted water changes should not matter */
  make_water_change_without_accounting_for_enth(&test_stand->soil,10);
  make_water_change_without_accounting_for_enth(&test_stand->soil, 10);
  make_water_change_with_accounting_for_enth(&test_stand->soil, -10, temp * c_water + c_water2ice, test_config);

  /* update soil as in update_soil_thermal_state */
  Real abs_waterice_cont[NSOILLAYER];
  get_abs_waterice_cont(abs_waterice_cont, &test_stand->soil);
  modify_enth_due_to_masschanges(&test_stand->soil, abs_waterice_cont, &test_config);
  update_mean_layer_temps(&test_stand->soil);

  TEST_ASSERT_EACH_EQUAL_DOUBLE(temp, test_stand->soil.temp, NSOILLAYER);
  teardown_stand(test_stand);
}

void make_water_change_with_accounting_for_enth(Soil *soil, Real water_inflow, Real enth_of_water, Config test_config)
{
  int l;
  foreachsoillayer(l)
  {
    reconcile_layer_energy_with_water_shift(soil, l, water_inflow, enth_of_water, (&test_config));
    soil->w[l] += water_inflow / soil->whcs[l];
  }
}

void make_water_change_without_accounting_for_enth(Soil *soil, Real water_inflow)
{
  int l;
  foreachsoillayer(l) { soil->w[l] += water_inflow / soil->whcs[l]; }
}

/* Helper functions */

void apply_scenario_to_soil(Soil *SoilPointer)
{
  apply_update_soil_thermal_state_10(SoilPointer, -20);
  change_soil_composition(SoilPointer);
  apply_update_soil_thermal_state_100(SoilPointer, 60);
  change_soil_composition_differently(SoilPointer);
  apply_update_soil_thermal_state_10(SoilPointer, -30);
}

void apply_update_soil_thermal_state_100(Soil *SoilPointer, Real belowSnowforcing)
{
  for (int i = 0; i < 100; i++)
  {
    update_soil_thermal_state(SoilPointer, belowSnowforcing, &test_config);
  }
}

void apply_update_soil_thermal_state_10(Soil *SoilPointer, Real belowSnowforcing)
{
  for (int i = 0; i < 10; i++)
  {
    update_soil_thermal_state(SoilPointer, belowSnowforcing, &test_config);
  }
}

void change_soil_composition(Soil *SoilPointer)
{
  SoilPointer->w[1] = min(SoilPointer->w[1] + 0.3, 1);
  SoilPointer->w[2] = min(SoilPointer->w[1] + 0.3, 1);
  SoilPointer->w[3] = min(SoilPointer->w[1] + 0.3, 1);

  SoilPointer->wsat[4] = min(SoilPointer->wsat[4] * 2, 1);
  SoilPointer->wsat[5] = min(SoilPointer->wsat[5] * 2, 1);

  SoilPointer->wsats[4] = SoilPointer->wsat[4] * soildepth[4];
  SoilPointer->wsats[5] = SoilPointer->wsat[5] * soildepth[5];
}

void change_soil_composition_differently(Soil *SoilPointer)
{
  SoilPointer->w_fw[1] = SoilPointer->w_fw[1] / 3;
  SoilPointer->w_fw[2] = SoilPointer->w_fw[2] / 3;
  SoilPointer->w_fw[3] = SoilPointer->w_fw[3] / 3;

  SoilPointer->ice_depth[4] = SoilPointer->ice_depth[4] / 2;
  SoilPointer->ice_depth[5] = SoilPointer->ice_depth[5] / 2;
  SoilPointer->ice_depth[6] = SoilPointer->ice_depth[6] / 2;
}

void change_water_content(Soil *test_soil)
{
  test_soil->w[0] += 0.1;
  test_soil->w[1] -= 0.1;
  test_soil->ice_depth[1] -= 0.1;
  test_soil->ice_fw[2] -= 0.1;
  test_soil->w_fw[3] += 0.1;
  test_soil->wpwp[4] += 0.1;
}

void change_soil_solid_content(Soil *test_soil)
{
  test_soil->wsat[1] -= 0.1;
  test_soil->wsats[1] = soildepth[1] * test_soil->wsat[1];
}

void check_if_results_are_still_similiar(Soil *SoilPointer, double tolerance)
{
  // results from run 05/07/2024
  Real oldtemps[] = {-26.970188, -13.685909, 2.379416, 8.985205, 0.0, 0.0};
  Real liqWatContents[] = {0.0, 0.0, 154.586460, 466.766667, 0.0, 0.0};

  Real actualLiquidWaterContent[NSOILLAYER];
  int l;
  foreachsoillayer(l) { actualLiquidWaterContent[l] = allwater(SoilPointer, l); }

  update_mean_layer_temps(SoilPointer);
  foreachsoillayer(l)
  {
    TEST_ASSERT_DOUBLE_WITHIN(tolerance, oldtemps[l], SoilPointer->temp[l]);
    TEST_ASSERT_DOUBLE_WITHIN(tolerance, liqWatContents[l], actualLiquidWaterContent[l]);
  }
}

void apply_update_soil_thermal_state_in_equilibrium_100(Soil *soil_pointer)
{
  double top_layer_temp = soil_pointer->temp[0];
  for (int i = 0; i < 100; i++)
  {
    update_soil_thermal_state(soil_pointer, top_layer_temp, &test_config);
  }
}

