/*----------------------------------------------------------*/
/*                                                          */
/*             fwritehydrotope.c                            */
/*  write hydrotope to restart for climLPJ                  */
/*                                                          */
/*  Thomas Kleinen (kleinen@pik-potsdam.de)                 */
/*  20/08/2009                                              */
/*                                                          */
/*                                                          */
/*----------------------------------------------------------*/

#include "lpj.h"
#include "hydrotope.h"

Bool fwritehydrotope(FILE *file, Hydrotope hydrotopes)
{
  fwrite(&hydrotopes.cti_mean, sizeof(Real), 1, file);
  fwrite(&hydrotopes.cti_chi, sizeof(Real), 1, file);
  fwrite(&hydrotopes.cti_phi, sizeof(Real), 1, file);
  fwrite(&hydrotopes.cti_mu, sizeof(Real), 1, file);

  fwrite(&hydrotopes.skip_cell, sizeof(Bool), 1, file);

  fwrite(&hydrotopes.wtable_mean, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wtable_min, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wtable_max, sizeof(Real), 1, file);

  fwrite(&hydrotopes.meanwater, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wetland_area, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wetland_area_runmean, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wetland_cti, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wetland_cti_runmean, sizeof(Real), 1, file);

  fwrite(&hydrotopes.wetland_wtable_current, sizeof(Real), 1, file);
  fwrite(&hydrotopes.wetland_wtable_max, sizeof(Real), 1, file);
  if (fwrite(&hydrotopes.wetland_wtable_mean, sizeof(Real), 1, file) != 1)
    return TRUE;
  return FALSE;
} /* of 'fwritehydrotope' */

