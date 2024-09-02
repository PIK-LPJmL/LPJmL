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
#include "setaside.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h"
/* lpjml modules mocked */
#include "support_fail_stub.h"
#include "support_pedotransfer_stub.h"
#include "support_mixsoilenergy_stubs.h"
/* support for tests */
#include "support_global_variables.h"
#include "support_stand_mocks.h"
#include "support_soil_mocks.h"
#include "support_manipulate_soil.h"

/* ------- tests ------- */
void test_energy_balance_for_mix_soil_is_closed()
{
  Stand *stand1 = setup_simple_stand();
  Stand *stand2 = setup_simple_stand();
  Real cellarea = 100; /* (m2) */

  /* define stand fractions */
  stand1->frac = 0.4;
  stand2->frac = 0.2;

  /* define some soil energies */
  int l;
  foreachsoillayer(l)
  {
    stand1->soil.enth[l] = 100;
    stand2->soil.enth[l] = 200;
  }

  /* get total absolute energy */
  Real stand1_area = stand1->frac*cellarea;
  Real stand2_area = stand2->frac*cellarea;
  Real total_energy_before_mixing = 0; /* (J) */
  foreachsoillayer(l)
  {
    total_energy_before_mixing += stand1->soil.enth[l] * stand1_area * soildepth[l]/1000; /* absolute energy */
    total_energy_before_mixing += stand2->soil.enth[l] * stand2_area * soildepth[l]/1000;
  }

  /* mix energy */
  mixsoilenergy(stand1, stand2, &test_config);
  stand1->frac += stand2->frac;
  stand1_area = stand1->frac*cellarea;

  /* verify that total energy is still the same */
  Real total_energy_after_mixing = 0;
  foreachsoillayer(l)
    total_energy_after_mixing += stand1->soil.enth[l] * stand1_area * soildepth[l]/1000;

  TEST_ASSERT_EQUAL_DOUBLE(total_energy_before_mixing, total_energy_after_mixing);

  teardown_stand(stand1);
  teardown_stand(stand2);
}
