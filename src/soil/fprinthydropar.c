/***************************************************************************/
/**                                                                       **/
/**            f  p  r  i  n  t  h  y  d  r  o  p  a  r  .  c             **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2016-07-06 11:19:45 +0200 (Mi, 06 Jul 2016#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

void fprinthydropar(FILE *file  /* pointer to text file */
)
{
  fprintf(file, "wland_min:\t%g\n"
    "cti_max:\t%g\n"
    "lat_min:\t%g\n"
    "wtab_thres:\t%g\n"
    "cti_thres:\t%g\n",
    hydropar.wland_min, hydropar.cti_max, hydropar.lat_min,
    hydropar.wtab_thres, hydropar.cti_thres);
} /* of 'fprinthydropar' */
