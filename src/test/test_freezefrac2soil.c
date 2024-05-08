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
#include "freezefrac2soil.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
/* support for tests */
#include "support_global_variables.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"
#include "support_assertions.h"

/* ------- prototypes ------- */
void calculate_freeze_fractions(Real *, Soil *);
void the_waterice_ratios_should_match_the_applied_freezefracs(Soil *, Real *);

/* ------- tests ------- */
void test_ice_to_waterice_ratio_should_match_freezefrac_when_half_frozen()
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_simple_soil(&test_soil);

  set_enthalpies_to_half_latent_heat(&test_soil); /* half unfreeze soil */
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  the_waterice_ratios_should_match_the_applied_freezefracs(&test_soil, freezefrac);

  teardown_soil_sub_structs(&test_soil);
}

void test_ice_to_waterice_ratio_should_match_freezefrac_when_unfrozen()
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_simple_soil(&test_soil);

  set_enthalpies_to_latent_heat(&test_soil); /* fully unfreeze soil */
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  the_waterice_ratios_should_match_the_applied_freezefracs(&test_soil, freezefrac);

  teardown_soil_sub_structs(&test_soil);
}

void test_ice_to_waterice_ratio_should_match_freezefrac_when_frozen()
{
  Soil test_soil;
  Real freezefrac[NSOILLAYER];
  setup_simple_soil(&test_soil);

  set_enthalpies_to_zero(&test_soil); /* fully freeze soil */
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  the_waterice_ratios_should_match_the_applied_freezefracs(&test_soil, freezefrac);

  teardown_soil_sub_structs(&test_soil);
}

void test_waterice_totals_should_remain_invariant_at_half_freezing()
{
  int l;

  Soil test_soil;
  Real waterice_old[NSOILLAYER];
  Real waterice_new[NSOILLAYER];
  Real freezefrac[NSOILLAYER];
  setup_complex_soil(&test_soil);

  /* --- record water ice content before changing enthalpy --- */
  foreachsoillayer(l) 
    waterice_old[l] = allwater((&test_soil), l) + allice((&test_soil), l);

  /* --- change enthalpy and recompute freeze fractions and water ice contents --- */
  set_enthalpies_to_half_latent_heat(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  /* --- compute new water ice total and confirm it hasn't changed --- */
  foreachsoillayer(l) 
    waterice_new[l] = allwater((&test_soil), l) + allice((&test_soil), l);
  TEST_ASSERT_EQUAL_DOUBLE_ARRAY(waterice_old, waterice_new, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_capillary_water_total_should_remain_invariant_at_half_freezing()
{
  int l;

  Soil test_soil;
  Real capilarry_water_old[NSOILLAYER];
  Real capilarry_water_new[NSOILLAYER];
  Real freezefrac[NSOILLAYER];
  setup_complex_soil(&test_soil);

  /* --- record capillary water content before changing enthalpy --- */
  foreachsoillayer(l) 
    capilarry_water_old[l] = test_soil.w[l] * test_soil.whcs[l] + test_soil.ice_depth[l];

  /* --- change enthalpy and recompute freeze fractions and water ice contents --- */
  set_enthalpies_to_half_latent_heat(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  /* --- compute new capillary water total and confirm it hasn't changed --- */
  foreachsoillayer(l) 
    capilarry_water_new[l] = test_soil.w[l] * test_soil.whcs[l] + test_soil.ice_depth[l];
  TEST_ASSERT_EQUAL_DOUBLE_ARRAY(capilarry_water_old, capilarry_water_new, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_free_water_total_should_remain_invariant_at_half_freezing()
{
  int l;

  Soil test_soil;
  Real free_water_old[NSOILLAYER];
  Real free_water_new[NSOILLAYER];
  Real freezefrac[NSOILLAYER];
  setup_complex_soil(&test_soil);

  /* --- record free water content before changing enthalpy --- */
  foreachsoillayer(l) 
    free_water_old[l] = test_soil.w_fw[l] + test_soil.ice_fw[l];

  /* --- change enthalpy and recompute freeze fractions and water ice contents --- */
  set_enthalpies_to_half_latent_heat(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  /* --- compute new free water total and confirm it hasn't changed --- */
  foreachsoillayer(l) 
    free_water_new[l] = test_soil.w_fw[l] + test_soil.ice_fw[l];
  TEST_ASSERT_EQUAL_DOUBLE_ARRAY(free_water_old, free_water_new, NSOILLAYER);

  teardown_soil_sub_structs(&test_soil);
}

void test_enthalpy_implied_by_water_content_should_match_actual_enthalpy_at_half_freezing()
{
  int l;

  Soil test_soil;
  Real waterice[NSOILLAYER];
  Real freezefrac[NSOILLAYER];
  setup_complex_soil(&test_soil);

  /* --- half freeze soil --- */
  set_enthalpies_to_half_latent_heat(&test_soil);
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  Real latent_heat_implied_by_water_content;
  foreachsoillayer(l)
  {
    /*                                         total water content in m^3       latent heat   soil volume of layer m^3 */
    /*                                                      =                        =                    =            */
    latent_heat_implied_by_water_content = (allwater((&test_soil), l) / 1000) * c_water2ice / (soildepth[l] / 1000);
    TEST_ASSERT_EQUAL_DOUBLE(latent_heat_implied_by_water_content, test_soil.enth[GPLHEAT * l]);
  }

  teardown_soil_sub_structs(&test_soil);
}

void test_no_nas_should_be_generated_with_waterless_poreless_soil()
{
  Soil test_soil;
  Real waterice[NSOILLAYER];
  Real freezefrac[NSOILLAYER];

  /* setup waterless half frozen soil */
  set_enthalpies_to_half_latent_heat(&test_soil);
  setup_complex_soil(&test_soil);
  set_soilwater_to_zero(&test_soil);

  /* set pores to zero for layer 3 */
  test_soil.wsats[3] = 0;
  test_soil.wsat[3] = 0;

  /* calc freeze fraction and water ice ratios */
  calculate_freeze_fractions(freezefrac, &test_soil);
  freezefrac2soil(&test_soil, freezefrac);

  /* confirm abscence of nas */
  no_nas_should_have_been_generated_in_soil(test_soil);

  teardown_soil_sub_structs(&test_soil);
}

/* ------- helper functions ------- */
void the_waterice_ratios_should_match_the_applied_freezefracs(Soil *test_soil, Real *freezefrac)
{
  int l;
  double ice_ratio;
  foreachsoillayer(l)
  {
    ice_ratio = allice(test_soil, l) / (allwater(test_soil, l) + allice(test_soil, l));
    TEST_ASSERT_EQUAL_DOUBLE(freezefrac[l], ice_ratio);
  }
}

void calculate_freeze_fractions(Real *freezefrac, Soil *test_soil)
{
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, test_soil, NULL, NULL, TRUE, TRUE);
  enth2freezefrac(freezefrac, test_soil->enth, &thermal_props);
}
