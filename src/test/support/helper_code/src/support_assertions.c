#include "lpj.h"
#include "unity.h"

void no_nas_should_have_been_generated_in_soil(Soil soil)
{
  int i, j;

  // Assuming LASTLAYER and NSOILLAYER are defined as positive integers
  for (i = 0; i < LASTLAYER; i++)
  {
    TEST_ASSERT_FALSE(isnan(soil.NO3[i]));
    TEST_ASSERT_FALSE(isnan(soil.NH4[i]));
  }

  for (i = 0; i < NSOILLAYER; i++)
  {
    TEST_ASSERT_FALSE(isnan(soil.w[i]));
    TEST_ASSERT_FALSE(isnan(soil.w_fw[i]));
    TEST_ASSERT_FALSE(isnan(soil.perc_energy[i]));
    TEST_ASSERT_FALSE(isnan(soil.temp[i]));
    TEST_ASSERT_FALSE(isnan(soil.Ks[i]));
    TEST_ASSERT_FALSE(isnan(soil.wpwp[i]));
    TEST_ASSERT_FALSE(isnan(soil.wfc[i]));
    TEST_ASSERT_FALSE(isnan(soil.wsat[i]));
    TEST_ASSERT_FALSE(isnan(soil.whc[i]));
    TEST_ASSERT_FALSE(isnan(soil.wsats[i]));
    TEST_ASSERT_FALSE(isnan(soil.whcs[i]));
    TEST_ASSERT_FALSE(isnan(soil.wpwps[i]));
    TEST_ASSERT_FALSE(isnan(soil.ice_depth[i]));
    TEST_ASSERT_FALSE(isnan(soil.ice_fw[i]));
    TEST_ASSERT_FALSE(isnan(soil.freeze_depth[i]));
    TEST_ASSERT_FALSE(isnan(soil.ice_pwp[i]));
    TEST_ASSERT_FALSE(isnan(soil.k_dry[i]));
    TEST_ASSERT_FALSE(isnan(soil.bulkdens[i]));
    TEST_ASSERT_FALSE(isnan(soil.beta_soil[i]));
    TEST_ASSERT_FALSE(isnan(soil.wi_abs_enth_adj[i]));
    TEST_ASSERT_FALSE(isnan(soil.sol_abs_enth_adj[i]));
  }

  TEST_ASSERT_FALSE(isnan(soil.w_evap));
  TEST_ASSERT_FALSE(isnan(soil.alag));
  TEST_ASSERT_FALSE(isnan(soil.amp));
  TEST_ASSERT_FALSE(isnan(soil.meanw1));
  TEST_ASSERT_FALSE(isnan(soil.snowpack));
  TEST_ASSERT_FALSE(isnan(soil.snowheight));
  TEST_ASSERT_FALSE(isnan(soil.snowfraction));
  TEST_ASSERT_FALSE(isnan(soil.temp[NSOILLAYER]));
  TEST_ASSERT_FALSE(isnan(soil.maxthaw_depth));
  TEST_ASSERT_FALSE(isnan(soil.mean_maxthaw));
  TEST_ASSERT_FALSE(isnan(soil.YEDOMA));
  TEST_ASSERT_FALSE(isnan(soil.rw_buffer));

  for (i = 0; i < NHEATGRIDP; i++)
  {
    TEST_ASSERT_FALSE(isnan(soil.enth[i]));
  }
}

void the_temps_should_have_remained_equal(Real *tempsNow, Real startTemp)
{
  int l;
  foreachsoillayer(l) { TEST_ASSERT_EQUAL_DOUBLE(startTemp, tempsNow[l]); }
}
