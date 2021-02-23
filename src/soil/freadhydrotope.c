/*----------------------------------------------------------*/
/*                                                          */
/*             freadhydrotopes.c                            */
/*  read hydrotopes to restart for climLPJ                  */
/*                                                          */
/*  Thomas Kleinen (kleinen@pik-potsdam.de)                 */
/*  20/08/2009                                              */
/*                                                          */
/*                                                          */
/*----------------------------------------------------------*/

#include "lpj.h"
#include "hydrotope.h"

Bool freadhydrotope(FILE *file, Hydrotope *hydrotopes, Bool swap)
{
  freadreal(&hydrotopes->cti_mean, 1, swap, file);
  freadreal(&hydrotopes->cti_chi, 1, swap, file);
  freadreal(&hydrotopes->cti_phi, 1, swap, file);
  freadreal(&hydrotopes->cti_mu, 1, swap, file);

  freadint(&hydrotopes->skip_cell, 1, swap, file);

  freadreal(&hydrotopes->wtable_mean, 1, swap, file);
  freadreal(&hydrotopes->wtable_min, 1, swap, file);
  freadreal(&hydrotopes->wtable_max, 1, swap, file);

  freadreal(&hydrotopes->meanwater, 1, swap, file);
  freadreal(&hydrotopes->wetland_area, 1, swap, file);
  freadreal(&hydrotopes->wetland_area_runmean, 1, swap, file);
  freadreal(&hydrotopes->wetland_cti, 1, swap, file);
  freadreal(&hydrotopes->wetland_cti_runmean, 1, swap, file);

  freadreal(&hydrotopes->wetland_wtable_current, 1, swap, file);
  freadreal(&hydrotopes->wetland_wtable_max, 1, swap, file);
  if (freadreal(&hydrotopes->wetland_wtable_mean, 1, swap, file) != 1)
    return TRUE;
  return FALSE;
} /* of 'freadhydrotope' */

