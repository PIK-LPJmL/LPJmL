/***************************************************************************/
/**                                                                       **/
/**               f  p  r  i  n  t  h  y  d  r  o  t  o  p  e  .  c       **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function prints hydrotope variables                               **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2018-08-30 16:50:19 +0200 (Do, 30 Aug 2018#$ **/
/**     By         : $Author:: sibylls                         $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

void fprinthydrotope(FILE *file, Hydrotope hydrotopes)
{
  fprintf(file, "cti_mean:\t%g\n", hydrotopes.cti_mean);
  fprintf(file, "cti_chi:\t%g\n", hydrotopes.cti_chi);
  fprintf(file, "cti_phi:\t%g\n", hydrotopes.cti_phi);
  fprintf(file, "cti_mu:\t\t%g\n", hydrotopes.cti_mu);

  fprintf(file, "skip_cell:\t%s\n", bool2str(hydrotopes.skip_cell));

  fprintf(file, "wtable_mean:\t%g\n", hydrotopes.wtable_mean);
  fprintf(file, "wtable_min:\t%g\n", hydrotopes.wtable_min);
  fprintf(file, "wtable_max:\t%g\n", hydrotopes.wtable_max);

  fprintf(file, "meanwater:\t%g\n", hydrotopes.meanwater);
  fprintf(file, "wetland_area:\t%g\n", hydrotopes.wetland_area);
  fprintf(file, "wetland_area_runmean:\t%g\n", hydrotopes.wetland_area_runmean);
  fprintf(file, "wetland_cti:\t%g\n", hydrotopes.wetland_cti);
  fprintf(file, "wetland_cti_runmean:\t%g\n", hydrotopes.wetland_cti_runmean);

  fprintf(file, "wetland wtable_current:\t%g\n", hydrotopes.wetland_wtable_current);
  fprintf(file, "wetland wtable_max:\t%g\n", hydrotopes.wetland_wtable_max);
  fprintf(file, "wetland wetland wtable_mean:\t%g\n", hydrotopes.wetland_wtable_mean);
} /* of 'fprinthydrotope' */
