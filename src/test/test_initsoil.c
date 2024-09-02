/* --- c libraries --- */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- headers with no corresponding .c files --- */
#include "lpj.h"

/* --- headers with corresponding .c files that will be compiled/linked in by ceedling --- */
#include "unity.h" /* c unit testing framework */

/* modules under test */
#include "initsoil.h"
#include "calc_soil_thermal_props.h"
#include "compute_mean_layer_temps_from_enth.h"

/* support for tests */
#include "support_global_variables.h"
#include "support_stand_mocks.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"
#include "support_pedotransfer_stub.h"

/* global lpjml variables */
Real layerbound[NSOILLAYER];

void test_highest_bulk_dens_has_expected_thermal_cond(void)
{
  Real therm_cond_highest_bulk_density;
  therm_cond_highest_bulk_density = calc_soil_dry_therm_cond(2700.0);

  TEST_ASSERT_FLOAT_WITHIN(0.01, 3, therm_cond_highest_bulk_density);
}

void test_zero_bulk_dens_doesnt_cause_NA(void)
{
  Real therm_cond_highest_bulk_density;
  int myRock = ROCK;
  therm_cond_highest_bulk_density = calc_soil_dry_therm_cond(0);

  TEST_ASSERT_FLOAT_IS_NOT_NAN(therm_cond_highest_bulk_density);
}
