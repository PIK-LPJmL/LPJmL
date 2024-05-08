#include "lpj.h"
#include "support_soil_mocks.h"

/* --- helper --- */

void teardown_stand(Stand *stand)
{
  free((void *)stand->type);
  free((void *)stand->data);
  free((void *)stand->soil.par);
  free((void *)stand->cell->output.data);
  free((void *)stand->cell);
  teardown_soil_sub_structs(&stand->soil);
  free(stand);
}

/* --- stands --- */

Stand *setup_simple_stand()
{
  /* create stand with simple soil
   * allocates memory for stand and all sub structs (like cell) */

  /* allocate and initialize sub structs */
  Real *data = calloc(200, sizeof(Real));
  Cell *cell = calloc(1, sizeof(Cell));
  Standtype *type = calloc(1, sizeof(Standtype));
  Irrigation *irrig = calloc(1, sizeof(Irrigation));
  Soilpar *par = calloc(1, sizeof(Soilpar));
  Stand *stand = calloc(1, sizeof(Stand));


  /* set pointer relations */
  cell->output.data = data;
  stand->type = type;
  stand->cell = cell;
  stand->soil.par = par;
  stand->data = irrig;
  
  /* configure stand and sub structures */
  type->landusetype = AGRICULTURE;
  irrig->irrig_system = DRIP;
  par->sand = 1;
  stand->frac = 0.4;
  setup_simple_soil(&stand->soil);

  return stand;
}

Stand *setup_complex_stand()
{
  /* modify simple stand by exchanging soil */
  Stand *stand = setup_simple_stand();
  teardown_soil_sub_structs(&stand->soil);
  setup_complex_soil(&stand->soil);
  return stand;
}
