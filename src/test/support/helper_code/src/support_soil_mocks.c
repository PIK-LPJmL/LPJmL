#include "lpj.h"
#include "support_manipulate_soil.h"

/* --- helper --- */

void initializeSoil(Soil *soil)
{
  /* set all bytes of the soil structure to zero */
  memset(soil, 0, sizeof(Soil));
}

void add_litter(Soil *test_soil)
{
  Litteritem *lit = calloc(1, sizeof(Litteritem));
  test_soil->litter.item = lit;
}

void teardown_soil_sub_structs(Soil *test_soil)
{
  free(test_soil->litter.item);
}

void compute_dependent_soil_variables(Soil *test_soil)
{
  int l;
  foreachsoillayer(l)
  {
    test_soil->wpwps[l] = test_soil->wpwp[l] * soildepth[l];
    test_soil->wsats[l] = soildepth[l] * test_soil->wsat[l];
    test_soil->wi_abs_enth_adj[l] = allwater(test_soil, l) + allice(test_soil, l);
    test_soil->sol_abs_enth_adj[l] = soildepth[l] - test_soil->wsats[l];
  }
}

/* --- soils --- */

void setup_simple_soil(Soil *test_soil)
{
  /* create homogenous soil 
   * use already allocated memory location for soil
   * but allocate new memory for sub structs like litter */
  int l, k;

  initializeSoil(test_soil);
  test_soil->litter.agtop_cover = 0.0;

  foreachsoillayer(l)
  {
    test_soil->k_dry[l] = 0.5;
    test_soil->w[l] = 0.3;
    test_soil->whc[l] = 0.5;
    test_soil->whcs[l] = test_soil->whc[l] * soildepth[l];
    test_soil->wpwp[l] = 0.05;
    test_soil->w_fw[l] = 0.05 * soildepth[l];
    test_soil->wsat[l] = 0.8;
    test_soil->beta_soil[l] = 0.5;
    test_soil->Ks[l] = 0.1;
    for (k = 0; k < GPLHEAT; k++)
      test_soil->enth[GPLHEAT * l + k] = -10000000;
  }

  compute_dependent_soil_variables(test_soil);
  update_mean_layer_temps(test_soil);
  add_litter(test_soil);
}

void setup_complex_soil(Soil *test_soil)
{
  /* modify simple homogenous soil to obtain inhomogenous complex soil */
  int k;

  setup_simple_soil(test_soil);
  test_soil->litter.agtop_cover = 0.3;

  /* layer 0 - low water content, high dry conductivity */
  test_soil->w[0] = 0.001;
  test_soil->wsat[0] = 0.4;
  test_soil->k_dry[0] = 2;

  /* layer 1 - zero water content, high energy */
  test_soil->w[1] = 0.0;
  for (k = 0; k < GPLHEAT; k++)
    test_soil->enth[GPLHEAT * 1 + k] = 300000000;

  /* layer 2 - fully saturated */
  test_soil->w[2] = 1;
  test_soil->whcs[2] = 0.2 * soildepth[2];
  test_soil->wsat[2] = 0.6;

  /* layer 3 - low wilting point water content */
  test_soil->wpwp[3] = 0.0001;

  /* layer 5 - full ice */
  test_soil->ice_depth[5] = soildepth[5];
  for (k = 0; k < GPLHEAT; k++)
    test_soil->enth[GPLHEAT * 5 + k] = -200000;

  compute_dependent_soil_variables(test_soil);
  update_mean_layer_temps(test_soil);
  add_litter(test_soil);
}
